/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_expressions of the \aliblong.
 *
 * \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_EXPRESSIONS_SCOPE
#define HPP_ALIB_EXPRESSIONS_SCOPE

#ifndef HPP_ALIB_LANG_FORMAT_FORMATTER_STD
#   include "alib/lang/format/formatterstdimpl.hpp"
#endif

#if !defined (HPP_ALIB_MONOMEM_HASHMAP)
#   include "alib/monomem/hashmap.hpp"
#endif

#if !defined (HPP_ALIB_MONOMEM_STDCONTAINERMA)
#   include "alib/monomem/stdcontainerma.hpp"
#endif

namespace alib {  namespace expressions {

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
 * All objects in these fields will be deleted with method \alib{expressions,Scope::Reset}.
 **************************************************************************************************/
struct ScopeResource
{
    /** The virtual destructor needed for virtual types. */
    virtual ~ScopeResource()                                                                      {}
};

/** ************************************************************************************************
 * This type is used as the default class to provide access to program data when evaluating
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
 * With each reuse, method #Reset will be invoked internally.
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
     * Block-allocator used to store temporary data and results. The allocated data within
     * this object becomes cleared automatically by method #Reset, in the moment
     * an expression is evaluated a next time (usually with different custom scope data).
     *
     * Note that this allocator is \b not cleared for the compile-time scope object #CTScope.
     */
    MonoAllocator                           Allocator;

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
    Scope*                                  CTScope;

    /**
     * This is the argument stack used by class \alib{expressions,detail::VirtualMachine} when
     * evaluating expressions.
     */
    std::vector<alib::Box>                 Stack;

    /** Stack of nested expressions called during evaluation. Used to detect cyclic expressions. */
    std::vector<Expression*, StdContMA<Expression*>>        NestedExpressions;

    /**
     * Simple list of user-defined, virtually deletable objects.
     * The objects in this vector will be deleted with #Reset.
     */
    std::vector<ScopeResource*,StdContMA<ScopeResource*>>   Resources;

    /**
     * A list of user-defined, named resources. Named resources may be allocated at compile-time
     * and used at evaluation-time.
     */
    HashMap<NString, ScopeResource*>        NamedResources;

    /**
     * Used to convert numbers to strings and vice versa. In addition expression function
     * \b %Format of built-in compiler plugin \alib{expressions::plugins,Strings} uses this object
     * to perform the formatting of arbitrary objects according to a given format string.
     *
     * Hence, to support customized format strings, a different formatter is to be passed here.
     * Default format string conventions provided with \alib are
     * \alib{lang::format,FormatterPythonStyle,python style} and
     * \alib{lang::format,FormatterJavaStyle,java/printf-like style}.
     *
     * The default implementation of method \alib{expressions,Compiler::getCompileTimeScope}
     * provides field \alib{expressions,Compiler::CfgFormatter} with the constructor of the
     * default compile-time scope.
     */
    SPFormatter                             Formatter;


    /** ********************************************************************************************
     * Constructor.<br>
     * Usually, for parameter \p formatter field \alib{expressions,Compiler::CfgFormatter} should
     * be provided.
     *
     * @param formatter A reference to a \c std::shared_ptr holding a formatter.
     *                  \alib{expressions,Compiler::CfgFormatter}.
     **********************************************************************************************/
    ALIB_API        Scope( SPFormatter& formatter );

    /** ********************************************************************************************
     * Virtual destructor
     **********************************************************************************************/
    virtual        ~Scope()
    {
        Reset();
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
    bool            IsCompileTime()
    {
        return CTScope == nullptr;
    }

    /** ********************************************************************************************
     * Scope objects usually are reused, either for evaluating the same expression using
     * different scoped data (attached to derived versions of this class), or for evaluating
     * different expression.<br>
     * Such reuse is internally detected and if so, this method is invoked.
     *
     * Instances of this class used as compilation scope, are not reset during the life-cycle
     * of an expression.
     *
     * Derived versions of this class need to free allocations performed by callback functions.
     **********************************************************************************************/
    ALIB_API
    virtual void    Reset();

};

} // namespace alib[::expressions]

/// Type alias in namespace \b alib. Renamed to not collide with #alib::lox::Scope.
using     ExpressionScope=    expressions::Scope;


} // namespace [alib]


#endif // HPP_ALIB_EXPRESSIONS_SCOPE
