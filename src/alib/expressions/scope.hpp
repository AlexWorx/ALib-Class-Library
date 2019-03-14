// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################

#ifndef HPP_ALIB_EXPRESSIONS_SCOPE
#define HPP_ALIB_EXPRESSIONS_SCOPE

#ifndef HPP_ALIB_STRINGFORMAT_FORMATTER_STD
#   include "alib/stringformat/formatterstdimpl.hpp"
#endif

#if !defined(HPP_ALIB_COMPATIBILITY_STD_STRINGS_FUNCTIONAL)
    #include "alib/compatibility/std_strings_functional.hpp"
#endif

#if !defined(HPP_ALIB_MEMORY_MEMORYBLOCKS)
    #include "alib/memory/memoryblocks.hpp"
#endif

namespace aworx { namespace lib { namespace expressions {

class Compiler;
class Expression;

/** ************************************************************************************************
 * This class acts as a simple virtual container to store custom resources in
 * \alib{expressions,Scope} objects.
 *
 * To do so, custom derived types would simply add a custom member object together with a virtual
 * destructor that assures that the member object(s) get rightfully deleted.
 *
 * Instances of this type allocated with operator \c new, may be stored in class \b %Scope in
 * containers accessible with fields \alib{expressions::Scope,Resources} and
 * \alib{expressions::Scope,NamedResources}.
 * All objects in these fields will be deleted with method \alib{expressions,Scope::Clear}.
 **************************************************************************************************/
struct ScopeResource
{
    virtual ~ScopeResource()
    {}
};

/** ************************************************************************************************
 * This type is used as base class to provide access to program data when evaluating
 * \alib expressions.
 * Usually a derived type which contains references to necessary application data is passed to
 * method \alib{expressions,Expression::Evaluate}.
 * Then, custom callback functions may cast this object back to its original type and access such
 * application data.
 *
 * Also, scope objects are used to store intermediate results and of-course the final one,
 * in the case that such results are not of a simple type that can be boxed
 * \alib{boxing,Box} "by value".
 *
 * For this, different simple allocator objects are provided. A custom, derived type may add own
 * storage facilities, for example vectors of custom objects which are deleted when the vector is
 * deleted.
 *
 * A scope object can be reused for evaluating the same expression several times. Prior to
 * evaluation, the custom "scoped data" has to be set.
 * With each reuse, method #Clear will be invoked internally.
 * Hence, if custom 'allocators' are added in derived types, this method has
 * to be overwritten to a) invoke the original method and b) clean such custom types.
 *
 * One singleton of this type, used to store compile-time data is created with virtual
 * method \alib{expressions,Compiler::getCompileTimeScope}. If compile-time invokable custom
 * callback methods use custom storage allocators, this method has to be overridden to return
 * the proper custom version of this class. (Note, this is not needed for the evaluation-time
 * instances, as this is created in the custom code unit anyhow and passed to method
 * \alib{expressions,Expression::Evaluate}.
 *
 * \see
 *    For details on the use of scopes see manual chapter
 *    \ref alib_expressions_scopes "8 Scopes".
 *
 **************************************************************************************************/
struct Scope
{
    /**
     * This is a pointer to the compile-time scope. This is available only at evaluation time
     * and primarily is used to access field #NamedResources. This allows to create resources
     * at compile-time, which can be used for evaluation.<br>
     *
     * A sample use case is implemented with built-in compiler plug-in
     * \alib{expressions::plugins,Strings}. When wildcard or regex matching is performed on
     * constant pattern strings, the matching class (which itself "compiles" the pattern once) is
     * created once and reused during evaluation.
     */
    Scope*                              CTScope;

    /**
     * This is the argument stack used by class \alib{expressions,detail::VirtualMachine} when
     * evaluating expressions.
     */
    std::vector<aworx::Box>             Stack;

    /** List of nested expressions called during evaluation. Used to detect cyclic expressions. */
    std::vector<Expression*>            nestedExpressionStack;

    /**
     * Block-allocator used to store temporary data and results. The allocated data within
     * this object becomes cleared automatically by method #Clear, in the moment
     * an expression is evaluated a next time (usually with different custom scope data).
     */
    MemoryBlocks                        Memory;

    /**
     * Simple list of user-defined, virtually deletable objects.
     * The objects in this vector will be deleted with #Clear.
     */
    std::vector<ScopeResource*>         Resources;

    /**
     * A list of user-defined, named resources. Named resources may be allocated at compile-time
     * and used at evaluation-time.
     */
    UnorderedStringMap<ScopeResource*, nchar> NamedResources;

    /**
     * Used to convert numbers to strings and vice versa. In addition expression function
     * \b %Format of built-in compiler plugin \alib{expressions::plugins,Strings} uses this object
     * to perform the formatting of arbitrary objects according to a given format string.
     *
     * Hence, to support customized format strings, a different formatter is to be passed here.
     * Default format string conventions provided with \alib are
     * \alib{stringformat,FormatterPythonStyle,python style} and
     * \alib{stringformat,FormatterJavaStyle,java/printf-like style}.
     *
     * The default implementation of method \alib{expressions,Compiler::getCompileTimeScope}
     * provides field \alib{expressions,Compiler::CfgFormatter} with the constructor of the
     * default compile-time scope.
     */
    SPFormatter                      Formatter;


    /** ********************************************************************************************
     * Constructor.<br>
     * Usually, for parameter formatter field \alib{expressions,Compiler::CfgFormatter} should
     * be provided.
     *
     * @param formatter A reference to a \c std::shared_ptr holding a formatter.
     *                  \alib{expressions,Compiler::CfgFormatter}.
     **********************************************************************************************/
    ALIB_API        Scope( SPFormatter& formatter );

    /** ********************************************************************************************
     * Virtual destructor
     **********************************************************************************************/
    inline
    virtual        ~Scope()
    {
        Clear();
    }

    /** ********************************************************************************************
     * This function may be called (respectively provides reliable results) only from within
     * callback functions that this scope is passed to.
     *
     * \note
     *   Usually, this method is not needed to be checked. It is useful and provided to support
     *   more complicated management of resources, i.e. allocation of resources at compile-time
     *   which are later used for evaluation.
     *
     * @return \c true if this is a compile-time invocation, \c false otherwise.
     **********************************************************************************************/
    inline bool     IsCompileTime()
    {
        return CTScope == nullptr;
    }

    /** ********************************************************************************************
     * Scope objects usually are reused, either for evaluating the same expression using
     * different scoped data (attached to derived versions of this class), or for evaluating
     * different expression.<br>
     * Such reuse is internally detected and if so, this method is invoked.
     *
     * Instances of this class used as compilation scope, are not cleared during the life-cycle
     * of an expression.
     *
     * Derived versions of this class need to free allocations performed by callback functions.
     **********************************************************************************************/
    ALIB_API
    virtual void    Clear();

};

}} // namespace aworx[::lib::expressions]

/// Type alias in namespace #aworx. Renamed to not collide with #aworx::lib::lox::Scope.
using     ExpressionScope=    aworx::lib::expressions::Scope;


}// namespace [aworx]


#endif // HPP_ALIB_EXPRESSIONS_SCOPE
