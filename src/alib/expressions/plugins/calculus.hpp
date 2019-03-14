// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################

#ifndef HPP_ALIB_EXPRESSIONS_PLUGINS_CALCULUS
#define HPP_ALIB_EXPRESSIONS_PLUGINS_CALCULUS

#ifndef HPP_ALIB_EXPRESSIONS_COMPILERPLUGIN
#   include "alib/expressions/compilerplugin.hpp"
#endif

#if !defined(HPP_ALIB_COMPATIBILITY_STD_STRINGS_FUNCTIONAL)
    #include "alib/compatibility/std_strings_functional.hpp"
#endif


namespace aworx { namespace lib { namespace expressions {

/**
 * This inner namespace of module \alibmod_nolink_expressions contains the implementations
 * of built-in compiler plug-ins.
 *
 * In addition, helper class \b %Calculus is defined here, which is the foundation for most of
 * the built-in functionality.
 *
 * It is strongly recommended to use this helper class for the creation of custom compiler plug-ins
 * instead of deriving such from class \alib{expressions,CompilerPlugin} directly.
 */
namespace plugins {

#if ALIB_DEBUG
    #define CALCULUS_CALLBACK(func)     func, ALIB_NSTRINGIFY(func)
    #define CALCULUS_DEFAULT_AUTOCAST   nullptr, nullptr
#else
    #define CALCULUS_CALLBACK(func)     func
    #define CALCULUS_DEFAULT_AUTOCAST   nullptr
#endif

#define CALCULUS_SIGNATURE(BoxPointerArray)  BoxPointerArray, std::extent<decltype(BoxPointerArray)>::value



/** ************************************************************************************************
 * ## 1. Introduction ##
 * This struct specializes \b %CompilerPlugin and provides generic approaches and implementation
 * of variants of method \b %TryCompilation.
 * Consequently, this is the base type of most built-in compiler plug-ins, and is strongly
 * recommended to consider this struct as the preferred base when implementing custom plug-ins.
 *
 * The class implements virtual, overloaded methods
 * - \alib{expressions::CompilerPlugin,TryCompilation(CIFunction&)},
 * - \alib{expressions::CompilerPlugin,TryCompilation(CIUnaryOp&)} and
 * - \alib{expressions::CompilerPlugin,TryCompilation(CIBinaryOp&)}.
 *
 * For each <em>AST</em> node type to compile, this class holds a vector or a hash map to store
 * all information needed for compilation.
 *
 * The simple schema of using this class is:
 * 1. Derive a custom type from this class.
 * 2. Create constant value objects and C++ native callback functions.
 * 3. Create constant (\c constexpr) tables with all compilation information.
 * 4. In the constructor of the derived class, feed the tables into to this struct, using the
 *    corresponding helper methods.
 *
 * \note
 *   Sample code and detailed explanations are given with user manual section
 *   \ref alib_expressions_cpcc_calculus "5.4 Class Calculus"
 *   and tutorial section
 *   \ref alib_expressions_tut_ffext "6. Extending The File Filter Sample"
 *
 * <p>
 * \note
 *   The name "Calculus" seems a little overstated, we admit.
 *
 * ## 2. Choose Anonymous Namespaces ##
 *
 * A good design pattern is to put the callback functions, constant values and compilation information
 * tables into an anonymous namespace at the start of the compilation unit (.cc or .cpp file) of
 * your custom plug-in. This way, the linker is not bothered with the function and object names,
 * which solely are referenced by their address and never need to get linked to other units.
 *
 * Just after this anonymous namespace, the implementation of the constructor of the custom
 * plug-in, should be placed. As explained above, its duty is to fills in the vector/hash map using
 * the right combination of operator or function  argument types, as well as providing a proper
 * \ref alib_expressions_prereq_sb "sample box" that corresponds to the type of the
 * output parameter of the native function.
 *
 * When implementing a custom plug-in, it may be helpful to have a look at the source code of
 * the built-in plug-ins provided with module \alibmod_expressions.
 * You will see that these implementations are quite straight forward and use some 'handy' local
 * preprocessor macros that may be copied and used for custom implementations.
 *
 * ## 3. Specifics For Identifiers and Functions ##
 * While parent class \b %CompilerPlugin does not make any direct distinction between functions
 * that are always returning a constant value and those that don't, this class does.
 * Arguably such functions are always parameterless, hence identifiers. Samples for such constants
 * are \b "PI" or \b "True".<br>
 * Such constant identifiers are supported by populating table #ConstantIdentifiers which is a
 * \c std::vector of elements of type \b %ConstantIdentifiersEntry.
 * For details on each "table column", refer to the documentation of the fields of
 * \alib{expressions::plugins::Calculus,ConstantIdentifierEntry}.
 *
 * Non-constant identifiers and functions are supported by populating table
 * #Functions, which is a \c std::vector of elements of type \b %FunctionEntry.
 * For details on each "table column", refer to the documentation of the fields of
 * \alib{expressions::plugins::Calculus,FunctionEntry}.
 *
 * Some notes:
 * - It can be specified if function names are case sensitive and whether they might be abbreviated.
 * - The list of arguments (their types) accepted by a function is to be provided as a
 *   \c std::vector of \ref alib_expressions_prereq_sb "sample boxes". It is
 *   recommended to define such vectors once per unique function "signature" in the anonymous
 *   namespace section of the compilation unit and fill its contents once in the constructor of the
 *   custom plug-in. Such vectors can then be reused for each function that shares the same
 *   signature.
 * - Variadic functions are supported by adding a final \e nulled \b %Box to the argument list.
 *   All sample argument types before this box are mandatory, but an arbitrary amount of arguments
 *   of likewise arbitrary type may be followed. It is also allowed to add just that one \e nulled
 *   \b %Box to the signature vector, which leads to functions that accept just any number of any
 *   type of argument, including zero arguments.
 * - In debug compilations, besides the callback function pointer, the C++ name of the callback
 *   function is to be provided. For this, macro \ref CALCULUS_CALLBACK is defined.
 *   The macro creates a stringified version of the given function pointer, separated by a comma.
 * - Flag \alib{expressions::plugins::Calculus,FunctionEntry::IsCTInvokable} is a boolean value
 *   that denotes whether a function can be evaluated at compile-time in the case that all of the
 *   parameters given in the expression are constant.
 *   If so, this struct will do the invocation at compile-time and return the constant result value
 *   instead of the function call.<br>
 *   Most built-in functions are compile-time invokable. For example most mathematical functions
 *   like \c log(Float) or \c sin(Float) can be evaluated at compile-time (again, only in the case
 *   that the given parameters are constant). The reason is, that
 *   these functions are independent from custom scope data.
 *   In contrast to this, custom functions, especially even parameterless identifiers usually are
 *   dependent on scope information and thus often can not be evaluated at compile-time.
 *
 * ## 4. Specifics For Unary Operators ##
 *
 * The compilation of \alib{expressions,DefaultUnaryOperators,unary operators} is supported by populating nested
 * hash map #UnaryOpMap. Because feeding a nested hash-map is a rather complicated process that
 * can not easily be done using static table data, several helpers are provided:
 * - #AddUnaryOp<br>
 *   This function adds compilation information for a single unary operator to #UnaryOpMap.
 *
 * - #UnaryOpTableEntry<br>
 *   This is a type definition that allows to define tables with compilation information on unary
 *   operators. It is recommended to create such tables as \c constexpr data in an anonymous
 *   namespace of the compilation unit.
 *
 * - #AddUnaryOps<br>
 *   This is a pair of overloaded functions. One of them is templated and just used to deduce
 *   the length of the given table of static data. This table is then fed as a pointer, together
 *   with the table size to the second method, which in turn feeds the table entries into
 *   #UnaryOpMap.
 *
 * In other words: Method #AddUnaryOp feeds one single operator to the nested hash-map, while
 * #AddUnaryOps feeds static "bulk" data to it. For details of the functions and types, consult
 * the corresponding documentation.
 *
 * As documented in user manual section
 * \ref alib_expressions_operators_aliases "9.4 Type-Specific Operator Aliases",
 * module \alibmod_nolink_expressions supports the use of alias
 * operators. For unary operators, this is reflected by this class with:
 *
 * - #UnaryOpAliases<br>
 *   A nested hash map that collects information about unary operator aliases.
 *
 * - #AddUnaryOpAlias<br>
 *   This function adds information about an unary operator alias to #UnaryOpAliases.
 *
 * - #UnaryOpAliasTableEntry
 *   A type definition that allows to define tables with information about unary operator aliases.
 *   It is recommended to create such tables as \c constexpr data in an anonymous
 *   namespace of the compilation unit.
 *
 * - AddUnaryOpAliases<br>
 *   This is a pair of overloaded functions. One of them is templated and just used to deduce
 *   the length of the given table of static data. This table is then fed as a pointer, together
 *   with the table size to the second method, which in turn feeds the table entries into
 *   #UnaryOpAliases.
 *
 *
 * ## 5. Specifics For Binary Operators ##
 *
 * The compilation of binary operators is supported by populating nested hash map #BinaryOpMap.
 * Because feeding a nested hash-map is a rather complicated process that
 * can not easily be done using static table data, several helper methods are provided:
 * - #AddBinaryOp<br>
 *   This function adds compilation information for a single binary operator to #BinaryOpMap.
 *
 * - #BinaryOpTableEntry<br>
 *   This is a type definition that allows to define tables with compilation information on binary
 *   operators. It is recommended to create such tables as \c constexpr data in an anonymous
 *   namespace of the compilation unit.
 *
 * - #AddBinaryOps<br>
 *   This is a pair of overloaded functions. One of them is templated and just used to deduce
 *   the length of the given table of static data. This table is then fed as a pointer, together
 *   with the table size to the second method, which in turn feeds the table entries into
 *   #BinaryOpMap.
 *
 * In other words: Method #AddBinaryOp feeds one single operator to the nested hash-map, while
 * #AddBinaryOps feeds static "bulk" data to it. For details of the functions and types, consult
 * the corresponding documentation.
 *
 * As documented in user manual section
 * \ref alib_expressions_operators_aliases "9.4 Type-Specific Operator Aliases",
 * module \alibmod_nolink_expressions supports the use of alias
 * operators. For binary operators, this is reflected by this class with:
 *
 * - #BinaryOpAliases<br>
 *   A nested hash map that collects information about binary operator aliases.
 *
 * - #AddBinaryOpAlias<br>
 *   This function adds information about a binary operator alias to #BinaryOpAliases.
 *
 * - #BinaryOpAliasTableEntry<br>
 *   A type definition that allows to define tables with information about binary operator aliases.
 *   It is recommended to create such tables as \c constexpr data in an anonymous
 *   namespace of the compilation unit.
 *
 * - #AddBinaryOpAliases<br>
 *   This is a pair of overloaded functions. One of them is templated and just used to deduce
 *   the length of the given table of static data. This table is then fed as a pointer, together
 *   with the table size to the second method, which in turn feeds the table entries into
 *   #BinaryOpAliases.
 *
 * \note
 *   It is \e not necessary to define an alias operator <c>'='</c> for operator <c>'=='</c>, when
 *   using this class, because this alias replacement is internally always made by this class, if
 *   compilation flag \alib{expressions,Compilation::AliasEqualsOperatorWithAssignOperator} is set
 *   in field \alib{expressions,Compiler::CfgCompilation}.
 *
 * \note
 *   In contrast to this, compilation flag \alib{expressions,Compilation::AllowBitwiseBooleanOperations}
 *   affects only built-in type \e boolean - just as the flag's name suggests.
 *   The flag is tested only in derived plug-in \alib{expressions,plugins::Arithmetics}.
 *   In other words: to allow for example operator <c>'&'</c> to be used as an alias for operator
 *   <c>'&&'</c> defined on custom types, this has to be explicitly added as an alias definition.
 *
 * Finally, for binary operators, this type provides a mechanism to provide information on
 * compile-time optimizations. The following fields and methods are provided:
 *
 * - #BinaryOpConstLHSOptimizations<br>
 *   A nested hash-map that collects information about possible optimizations of binary operations
 *   when the left-hand side operator is a specific constant value.
 *
 * - #BinaryOpConstRHSOptimizations<br>
 *   A nested hash-map that collects information about possible optimizations of binary operations
 *   when the right-hand side operator is a specific constant value.
 *
 * - #BinaryOpOptimizationsTableEntry<br>
 *   A type definition that allows to define tables with information about binary operator
 *   optimizations..
 *   It is recommended to create such tables as \c constexpr data in an anonymous
 *   namespace of the compilation unit.
 *
 * - #AddBinaryOpOptimizations<br>
 *   A pair of overloaded functions. One of them is templated and just used to deduce
 *   the length of the given table of static data. This table is then fed as a pointer, together
 *   with the table size to the second method, which in turn feeds the table entries into
 *   either #BinaryOpConstLHSOptimizations or #BinaryOpConstRHSOptimizations.
 *
 * Samples of binary operator optimizations are given in documentation of struct
 * \alib{expressions,CompilerPlugin::CIBinaryOp}, which is filled with the optimization
 * information given by the use of above listed helpers.
 *
 * # Reference Documentation #
 **************************************************************************************************/
struct Calculus   : public CompilerPlugin
{
    /**
     * Boolean to denote if a callback function allows compile-time invocation.
     * If \c true, on constant function input (either from expression string literals or sub-expressions
     * that have been optimized to constant input) the program can be optimized by invoking the
     * \alib{expressions,CallbackDecl} already at compile-time.
     *
     * This flag is set for most built-in functions, e.g. arithmetic calculations, but usually
     * can not be set custom callbacks, as those usually rely on custom scope objects which are
     * available only at evaluation-time.
     *
     * \note
     *   This type is used with helper class \alib{expressions,plugins::Calculus} but exposed
     *   as a namespace type for convenience, together with constants #CTI and
     *   #ETI
     *
     */
    using CTInvokable= bool;


    /**
     * Used for values of #CTInvokable flags.<br>
     * The use of this constant makes code more readable.
     */
    static constexpr CTInvokable  CTI  = true;

    /**
     * Used for values of #CTInvokable flags to denote that a callback function
     * is only invokable at evaluation-time.<br>
     * The use of this constant makes code more readable.
     */
    static constexpr CTInvokable  ETI  = false;


    /** ********************************************************************************************
     * Constructor.
     * @param name     Assigned to field \alib{expressions,CompilerPlugin::Name}.
     * @param compiler The compiler we will get attached to
     **********************************************************************************************/
                    Calculus( const NString& name, Compiler& compiler )
    : CompilerPlugin( name, compiler )
    {}

    /** ********************************************************************************************
     * Virtual destructor.
     **********************************************************************************************/
    virtual        ~Calculus()                                                              override
    {}


    // #############################################################################################
    // Constant Identifiers, Identifiers and Functions
    // #############################################################################################

    /**
     * An entry of field #ConstantIdentifiers. Describes how the identifier is recognized and
     * the constant value to return for it.
     */
    struct ConstantIdentifierEntry
    {
        /** The name, minimum length and letter case sensitivity  of the function to recognize. */
        Token                       Descriptor;

        /** The constant result. */
        Box                         Result;
    };

    /** List of identifiers that return constant values to be compiled by this plug-in. */
    std::vector<ConstantIdentifierEntry>   ConstantIdentifiers;

    /**
     * An entry of field #Functions. Describes how the function is recognized and
     * the callback function and return type for it.
     */
    struct FunctionEntry
    {
        /** The name, minimum length and letter case sensitivity  of the function to recognize. */
        Token                       Descriptor;

        /**
         * A pointer to list of pointers to sample boxes that describe the function signature.
         * If \c nullptr, then the function does not accept parameters (aka is an identifier).
         *
         * To denote variadic parameters (similar to C/C++ ellipsis operator \c "..." ), either
         * \c nullptr or a \ref alib_boxing_more_void_void "void box" may be given as the last
         * array element. All prior provided boxes represent mandatory arguments, while the
         * function accepts an arbitrary amount of arguments of arbitrary type in addition.
         *
         * The length of this list is given with field #SignatureLength.
         *
         * \see Macro \ref CALCULUS_SIGNATURE which is recommended to be used to pass both
         *      fields (this and #SignatureLength). The macro accepts a C++ array of \b Box* and
         *      deducts the array's length from the declaration type of the given array.
         */
        Box**                       Signature;

        /**
         * See #Signature for more information.
         */
        size_t                      SignatureLength;

        /** Callback function to add to the program. If \c nullptr, field #ResultType is
         *  used as both: a constant value added to the program and the result type! */
        CallbackDecl                Callback;


        #if ALIB_DEBUG
            /**
             * The C++ name of the callback function (only available in debug compilations of the
             * library. Use preprocessor macro \ref CALCULUS_CALLBACK to provide this field
             * together with field #Callback. The macro selects to prune the name string
             * in release compilations. */
            const char*             DbgCallbackName;
        #endif

        /**
         * The result type given as a pointer to a \ref alib_expressions_prereq_sb "sample box".
         *
         * \note
         *   If #Callback is \c nullptr, this box changes its meaning from being just a sample that
         *   provides the return type of the callback function, to being the 'real' constant
         *   result value that the function represents. However, it is preferable, to
         *   implement such constant functions using field
         *   \alib{expressions::plugins,Calculus::ConstantIdentifiers}
         */
        Box*                        ResultType;

        /**
         * Denotes, if the callback function is allowed to be invoked on the
         * \alib{expressions,Scope} object used at compile-time.
         * This scope object is of the same (eventually custom) type as the one for evaluation,
         * however the evaluation-specific data is not set.
         * If allowed, such invocation is performed, if all function arguments are constant and
         * instead of the function, the result is returned.
         */
        CTInvokable                 IsCTInvokable;
    };

    /** List of functions to be compiled by this plug-in. */
    std::vector<FunctionEntry>      Functions;

    /** ********************************************************************************************
     * Searches in vectors #Functions and #ConstantIdentifiers for an entry matching \p{name} and,
     * if found, adds either a constant value or a callback function to \p{ciFunction}.
     *
     * This plug-in corrects abbreviated and letter case differences in functions within
     * in/out parameter \alib{expressions::CompilerPlugin,CIFunction::Name}.
     *
     * @param[in,out]  ciFunction  The compilation result.
     * @return \c true if an entry was found in #Functions and a corresponding command
     *         was added to \p{ciFunction}. \c false otherwise.
     **********************************************************************************************/
    ALIB_API
    virtual bool    TryCompilation( CIFunction& ciFunction )                              override;


    // #############################################################################################
    // Unary operators
    // #############################################################################################
    /**
     * Nested hash map assigning combinations of unary operators and argument type to a tuple of an
     * evaluation function and a return type.
     *
     * A third member of type #CTInvokable is to be set to \c true, if the
     * callback function is allowed to be invoked on the \alib{expressions,Scope} object used
     * at compile-time.
     * This scope object is of the same (eventually custom) type as the one for evaluation, however
     * the evaluation-specific data is not set. In other words, the third tuple member denotes
     * if during program compilation, constant input values might be evaluated right away.
     *
     * A fourth tuple member of type \alib{strings,TString,String} is available only in debug compilations
     * and receives the name of the callback function.
     *
     * \note
     *   This nested map, the same as #UnaryOpAliases, #BinaryOpMap and #BinaryOpAliases
     *   is to be filled using corresponding \e add-methods.
     *   Usually this is done in the constructor of derived classes.
     */
    UnorderedStringMap <TypeMap<std::tuple<CallbackDecl, Box, CTInvokable
                                                               #if ALIB_DEBUG
                                                                      , const char*
                                                               #endif
                                                                                    >>>  UnaryOpMap;

    /**
     * Nested hash map assigning combinations of alias versions of unary operators and their
     * the argument type to the original operator.
     *
     * \note
     *   This nested map, the same as #UnaryOpMap, #BinaryOpMap and #BinaryOpAliases
     *   is to be filled using corresponding \e add-methods.
     *   Usually this is done in the constructor of derived classes.
     */
    UnorderedStringMap<TypeMap<String>>                                              UnaryOpAliases;

    /**
     * Entry of arrays used with methods #AddUnaryOps to perform bulk-loading of compile definition
     * data into #UnaryOpMap.
     * The tuple elements are:<br>
     * - The operator to compile.
     * - The argument type of the unary operation.
     * - The callback function. Set to \c nullptr if operator evaluates always constant.
     * - The C++ name of the callback function. (This tuple element is only available in debug
     *   compilations of the library.)
     * - The result type sample box, respectively, if \b callback is \c nullptr, the constant result
     *   value.
     * - Flag to denote if the callback function allows compile-time invocation and thus on constant
     *   input the program can be optimized. This is true e.g. for arithmetic functions, but usually
     *   not for custom operators that rely on scope objects available only at evaluation time.
     */
    using UnaryOpTableEntry= const std::tuple<String, Type, CallbackDecl,
                                                                           #if ALIB_DEBUG
                                                                        const char*,
                                                                           #endif
                                                                                Type, CTInvokable>;

   /**
    * Entry of arrays used with methods #AddUnaryOpAliases to perform bulk-loading of operator
    * alias definition data into #UnaryOpAliases.
    * The tuple elements are:
    * - The alias operator.
    * - The argument type of the unary operation.
    * - The operator that gets aliased.
    */
    using UnaryOpAliasTableEntry= const std::tuple<String, Type, String>;

    #if ALIB_DOCUMENTATION_PARSER
    /** ********************************************************************************************
     * Adds an unary operator's callback function and return type to the compilation map
     * #UnaryOpMap.
     * \see Consider using #AddUnaryOps, a variant of this method that allows effective
     *      bulk loading.
     *
     * @param op              The operator.
     * @param argType         The argument type that the operator is defined for.
     * @param callback        The callback function to execute.
     * @param dbgCallbackName The name of the C++ name of the callback function.
     *                        \note This parameter is available only in debug version of the
     *                        library.
     * @param cti             See #CTInvokable for the meaning of this flag.
     * @param resultType      The result type of the callback function.
     **********************************************************************************************/
    void AddUnaryOp     ( const String& op, Type argType, CallbackTypes::Unary callback,
                          #if ALIB_DEBUG
                            const char* dbgCallbackName,
                          #endif
                          Type resultType,
                          CTInvokable  cti     );

    #else // clang would complain about the doxing parameter dbgCallbackName
    void AddUnaryOp     ( const String& op, Type argType, CallbackDecl callback,
                          #if ALIB_DEBUG
                            const char* dbgCallbackName,
                          #endif
                          Type resultType,
                          CTInvokable  cti
                          );
    #endif

    /** ********************************************************************************************
     * Templated helper method. Deduces the array size of the given table and passes it
     * to \ref AddUnaryOps(UnaryOpTableEntry* table, size_t length).
     *
     * @param  table     The table containing operator compilation information.
     * @tparam TCapacity Implicitly deferred size of the array provided.
     **********************************************************************************************/
    template<size_t TCapacity>
    inline
    void AddUnaryOps    ( UnaryOpTableEntry (&table) [TCapacity]  )
    {
        AddUnaryOps( &table[0], TCapacity );
    }

    /** ********************************************************************************************
     * Loads all entries of the given table into nested map #UnaryOpMap.
     *
     * Note, that usually, the given table is a constexpr array located in an anonymous namespace
     * of a compilation unit.<br>
     * It can be passed as a reference to templated helper method, which defers the length of the
     * table implicitly.
     *
     * \note
     *   It is slightly more performant and causes less memory fragmentation, if the given table is
     *   sorted by the operator (first) column.
     *
     * @param  table     The table containing operator compilation information.
     * @param  length    The table containing operator compilation information.
     **********************************************************************************************/
    void AddUnaryOps    ( UnaryOpTableEntry* table, size_t length );

    /** ********************************************************************************************
     * Adds an alias operator to nested hash table #UnaryOpAliases.
     * \see Consider using #AddUnaryOpAliases, a variant of this method that allows effective
     *      bulk loading.
     *
     * @param alias    The alias for operator \p{op}.
     * @param argType  The argument type that the operator is defined for.
     * @param op       The operator aliased by \p{alias}.
     **********************************************************************************************/
    void AddUnaryOpAlias  ( const String& alias, Type argType, const String& op );

    /** ********************************************************************************************
     * Templated helper method. Deduces the array size of the given table and passes it
     * to \ref AddUnaryOpAliases(UnaryOpAliasTableEntry* table, size_t length).
     * @param  table     The table containing operator compilation information.
     * @tparam TCapacity Implicitly deferred size of the array provided.
     **********************************************************************************************/
    template<size_t TCapacity>
    inline
    void AddUnaryOpAliases( UnaryOpAliasTableEntry (&table) [TCapacity]  )
    {
        AddUnaryOpAliases( &table[0], TCapacity );
    }

    /** ********************************************************************************************
     * Loads all entries of the given table into nested map #UnaryOpAliases.
     *
     * Note, that usually, the given table is a constexpr array located in an anonymous namespace
     * of a compilation unit.<br>
     * It can be passed as a reference to templated helper method, which defers the length of the
     * table implicitly.
     *
     * \note
     *   It is slightly more performant and causes less memory fragmentation, if the given table is
     *   sorted by the operator (first) column.
     *
     * @param  table     The table containing operator compilation information.
     * @param  length    The table containing operator compilation information.
     **********************************************************************************************/
    void AddUnaryOpAliases( UnaryOpAliasTableEntry* table, size_t length );

    /** ********************************************************************************************
     * Searches in #UnaryOpMap for an entry matching the combination of
     * \alib{expressions,CIUnaryOp::Operator} and the argument type of operand found with iterator
     * \alib{expressions,CompilationInfo::ArgsBegin}.
     * If found, the corresponding callback function and result type are added the \p{CIUnaryOp}.
     *
     * Before the search, it is checked whether the given operation is an alias for another
     * operator. Operator aliases might be defined by filling map #UnaryOpAliases in the constructor
     * of the derived types.
     * The corrected operator is to be returned in in/out parameter
     * \alib{expressions,CIUnaryOp::Operator}.
     *
     * @param[out]    ciUnaryOp    The compilation result.
     * @return \c true if an entry was found in #UnaryOpMap and a corresponding command was added to
     *         \p{CIUnaryOp}. \c false otherwise.
     **********************************************************************************************/
    ALIB_API
    virtual bool    TryCompilation( CIUnaryOp&    ciUnaryOp  )                          override;



    // #############################################################################################
    // Binary operators
    // #############################################################################################
    /**
     * Nested hash map assigning combinations of binary operators and its argument types to a
     * tuple of n callback function and its return type.
     *
     * A third member of type #CTInvokable is to be set to \c true, if the
     * callback function is allowed to be invoked on the \alib{expressions,Scope} object used at
     * compile-time.
     * This scope object is of the same (eventually custom) type as the one for evaluation, however
     * the evaluation-specific data is not set. In other words, the third tuple member denotes
     * if during program compilation, constant input values might be evaluated right away.
     *
     * A fourth tuple member of type \alib{strings,TString,String} is available only in debug compilations
     * and receives the name of the callback function.
     *
     *
     * \note
     *   This nested map, the same as #BinaryOpAliases, #UnaryOpMap and #UnaryOpAliases
     *   is to be filled using corresponding \e add-methods.
     *   Usually this is done in the constructor of derived classes.
     */
    UnorderedStringMap<TypeMap<TypeMap<std::tuple<CallbackDecl, Box, CTInvokable
                                                                #if ALIB_DEBUG
                                                                             , const char*
                                                                #endif
                                                                                   >>>> BinaryOpMap;

    /**
     * Nested hash map assigning combinations of alias versions of binary operators and their
     * the argument types to the original operator.
     *
     * \note
     *   This nested map, the same as #BinaryOpMap, #UnaryOpMap and #UnaryOpAliases
     *   is to be filled using corresponding \e add-methods.
     *   Usually this is done in the constructor of derived classes.
     */
    UnorderedStringMap <TypeMap<TypeMap<String>>>                                   BinaryOpAliases;

    /**
     * Entry of arrays used with methods #AddBinaryOps to perform bulk-loading of compile definition
     * data into #BinaryOpMap.
     * The tuple elements are:
     * - The operator to compile.
     * - The left-hand side type of the binary operation.
     * - The right-hand side type of the binary operation.
     * - The callback function. Set to \c nullptr if operator evaluates constant.
     * - The C++ name of the callback function. (This tuple element is only available in debug
     *   compilations of the library.)
     * - The result type sample box, respectively, if \b callback is \c nullptr, the constant result
     *   value.
     * - Flag to denote if the callback function allows compile-time invocation and thus on constant
     *   input the program can be optimized. This is true e.g. for arithmetic functions, but usually
     *   not for custom operators that rely on scope objects available only at evaluation time.
     */
    using BinaryOpTableEntry=      const std::tuple<String, Type, Type, CallbackDecl,
                                                #if ALIB_DEBUG
                                                   const char*,
                                                #endif
                                                                Type, CTInvokable>;

   /**
    * Entry of arrays used with methods  T
    * \alib{expressions::plugins::Calculus,AddBinaryOpAliases,AddBinaryOpAliases}
    *  to perform bulk-loading of operator
    * alias definition data into #BinaryOpAliases.
    * The tuple elements are:
    * - The alias operator.
    * - The left-hand side type of the binary operation.
    * - The right-hand side type of the binary operation.
    * - The operator that gets aliased.
    */
    using BinaryOpAliasTableEntry= const std::tuple<String, Type, Type, String>;

    /**
     * Nested hash map assigning combinations of binary operators and left-hand side operator
     * constants to optimization information.
     *
     * This map is to be filled with #AddBinaryOpOptimizations, which is usually done in the.
     * constructor of derived classes.
     */
    UnorderedStringMap<TypeMap<UnorderedBoxMap<Box>>>          BinaryOpConstLHSOptimizations;

    /**
     * Nested hash map assigning combinations of binary operators and right-hand side operator
     * constants to optimization information.
     *
     * This map is to be filled with #AddBinaryOpOptimizations, which is usually done in the.
     * constructor of derived classes.
     */
    UnorderedStringMap<TypeMap<UnorderedBoxMap<Box>>>          BinaryOpConstRHSOptimizations;


    /**
     * Entry of arrays used with methods #AddBinaryOpOptimizations to perform bulk-loading of
     * optimization data for maps #BinaryOpConstLHSOptimizations and #BinaryOpConstRHSOptimizations.
     * The tuple elements are:
     * - The alias operator.
     * - The type of the non-constant argument.
     * - The type and value of the constant argument.
     * - Either, a constant result value that replaces the binary operation
     *   (as in <c> x || true</c>) or a \e nulled box, which indicates that the result equals the
     *   non-constant argument (as in <c>x && true</c>).
     */
    using BinaryOpOptimizationsTableEntry=   const std::tuple<String,Type, const Box&, const Box&>;




    #if ALIB_DOCUMENTATION_PARSER
    /** ********************************************************************************************
     * Adds a binary operator's callback function and return type to the compilation map
     * #BinaryOpMap.
     * \see Consider using #AddBinaryOps, a variant of this method that allows effective
     *      bulk loading.
     *
     * @param op              The operator.
     * @param lhsType         The left-hand side argument type that the operator is defined for.
     * @param rhsType         The right-hand side argument type that the operator is defined for.
     * @param callback        The callback function to execute.
     * @param dbgCallbackName The name of the C++ name of the callback function.
     *                        \note This parameter is available only in debug version of the
     *                        library.
     * @param cti             See #CTInvokable for the meaning of this flag.
     * @param resultType      The result type of the callback function.
     **********************************************************************************************/
    void AddBinaryOp     ( const String& op, Type lhsType, Type rhsType, CallbackTypes::Binary callback,
                           #if ALIB_DEBUG
                             const char* dbgCallbackName,
                           #endif
                           Type resultType,
                           CTInvokable  cti     );
    #else // clang would complain about the doxing parameter dbgCallbackName
    ALIB_API
    void AddBinaryOp     ( const String& op, Type lhsType, Type rhsType, CallbackDecl callback,
                           #if ALIB_DEBUG
                             const char* dbgCallbackName,
                           #endif
                           Type resultType,
                           CTInvokable  cti     );
    #endif


    /** ********************************************************************************************
     * Templated helper method. Deduces the array size of the given table and passes it
     * to \ref AddBinaryOps(BinaryOpTableEntry* table, size_t length).
     * @param  table     The table containing operator compilation information.
     * @tparam TCapacity Implicitly deferred size of the array provided.
     **********************************************************************************************/
    template<size_t TCapacity>
    inline
    void AddBinaryOps    ( BinaryOpTableEntry (&table) [TCapacity]  )
    {
        AddBinaryOps( &table[0], TCapacity );
    }

    /** ********************************************************************************************
     * Loads all entries of the given table into nested map #BinaryOpMap.
     *
     * Note, that usually, the given table is a constexpr array located in an anonymous namespace
     * of a compilation unit.<br>
     * It can be passed as a reference to templated helper method, which defers the length of the
     * table implicitly.
     *
     * \note
     *   It is slightly more performant and causes less memory fragmentation, if the given table is
     *   sorted by the operator column (first)  and as a second order by the \p{lhsType}
     *   column (second).
     *
     * @param  table     The table containing operator compilation information.
     * @param  length    The table containing operator compilation information.
     **********************************************************************************************/
    ALIB_API
    void AddBinaryOps    ( BinaryOpTableEntry* table, size_t length );

    /** ********************************************************************************************
     * Adds an alias operator to nested hash table #BinaryOpAliases.
     * \see Consider using #AddBinaryOpAliases, a variant of this method that allows effective
     *      bulk loading.
     *
     * @param alias    The alias for operator \p{op}.
     * @param lhsType  The left-hand side argument type that the operator is defined for.
     * @param rhsType  The right-hand side argument type that the operator is defined for.
     * @param op       The operator aliased by \p{alias}.
     **********************************************************************************************/
    void AddBinaryOpAlias  ( const String& alias, Type lhsType, Type rhsType, const String& op );

    /** ********************************************************************************************
     * Templated helper method. Deduces the array size of the given table and passes it
     * to \ref AddBinaryOpAliases(BinaryOpAliasTableEntry* table, size_t length).
     * @param  table     The table containing operator compilation information.
     * @tparam TCapacity Implicitly deferred size of the array provided.
     **********************************************************************************************/
    template<size_t TCapacity>
    inline
    void AddBinaryOpAliases( BinaryOpAliasTableEntry (&table) [TCapacity]  )
    {
        AddBinaryOpAliases( &table[0], TCapacity );
    }

    /** ********************************************************************************************
     * Loads all entries of the given table into nested map #BinaryOpAliases.
     *
     * Note, that usually, the given table is a constexpr array located in an anonymous namespace
     * of a compilation unit.<br>
     * It can be passed as a reference to templated helper method, which defers the length of the
     * table implicitly.
     *
     * \note
     *   It is slightly more performant and causes less memory fragmentation, if the given table is
     *   sorted by the operator column (first)  and as a second order by the \p{lhsType}
     *   column (second).
     *
     * @param  table     The table containing operator compilation information.
     * @param  length    The table containing operator compilation information.
     **********************************************************************************************/
    void AddBinaryOpAliases( BinaryOpAliasTableEntry* table, size_t length );


    /** ********************************************************************************************
     * Templated helper method. Deduces the array size of the given table and passes it
     * to \ref AddBinaryOpOptimizations(BinaryOpOptimizationsTableEntry*, size_t, bool).
     * @param  table     The table containing operator compilation information.
     * @param  lhsOrRhs  If \c false, the entries passed go into table
     *                   #BinaryOpConstLHSOptimizations which provides information about
     *                   optimizations if the lhs-operand is constant.
     *                   If \c true, table #BinaryOpConstRHSOptimizations is chosen, which provides
     *                   information about optimizations if the rhs-operand is constant.
     *
     * @tparam TCapacity Implicitly deferred size of the array provided.
     **********************************************************************************************/
    template<size_t TCapacity>
    inline
    void AddBinaryOpOptimizations( BinaryOpOptimizationsTableEntry (&table) [TCapacity],
                                   bool lhsOrRhs )
    {
        AddBinaryOpOptimizations( &table[0], TCapacity, lhsOrRhs );
    }

    /** ********************************************************************************************
     * Loads all entries of the given table into nested map #BinaryOpConstLHSOptimizations
     * respectively BinaryOpConstRHSOptimizations.
     *
     * Note, that usually, the given table is a constexpr array located in an anonymous namespace
     * of a compilation unit.<br>
     * It can be passed as a reference to templated helper method, which defers the length of the
     * table implicitly.
     *
     * @param  table     The table containing operator compilation information.
     * @param  length    The table containing operator compilation information.
     * @param  lhsOrRhs  If \c false, the entries passed go into table
     *                   #BinaryOpConstLHSOptimizations which provides information about
     *                   optimizations if the lhs-operand is constant.
     *                   If \c true, table #BinaryOpConstRHSOptimizations is chosen, which provides
     *                   information about optimizations if the rhs-operand is constant.
     **********************************************************************************************/
    ALIB_API
    void AddBinaryOpOptimizations( BinaryOpOptimizationsTableEntry* table, size_t length,
                                   bool lhsOrRhs );

    /** ********************************************************************************************
     * Searches in #BinaryOpMap for an entry matching the combination of
     * \alib{expressions,CIBinaryOp::Operator} and the argument types of operands found with
     * argument iterators
     * \alib{expressions,CompilationInfo::ArgsBegin} and
     * \alib{expressions,CompilationInfo::ArgsEnd}.
     * If found, the corresponding callback function and result type are added the \p{CIBinaryOp}.
     *
     * Before the search, it is checked whether the given operation is an alias for another
     * operator. Operator aliases might be defined by filling map #BinaryOpAliases in the
     * constructor of the derived types.
     * The corrected operator is to be returned in in/out parameter
     * \alib{expressions,CIBinaryOp::Operator}.
     *
     * @param[in,out]  ciBinaryOp  The compilation info struct.
     * @return \c true if an entry was found in #BinaryOpMap and a corresponding command was added
     *         to \p{CIBinaryOp}. \c false otherwise.
     **********************************************************************************************/
    ALIB_API
    virtual bool    TryCompilation( CIBinaryOp&  ciBinaryOp      )                         override;


    // #############################################################################################
    // Auto-Casts
    // #############################################################################################

    /**
     * An entry of field #AutoCasts. Defines auto-casts for custom types.
     */
    struct AutoCastEntry
    {
        /** The type that is to be automatically casted.*/
        Box                        Type;

        /**
         * List of operators that the auto-cast accepts.
         * If \e nulled, then just any operator that is not in #OperatorsDeclined is accepted.
         */
        std::vector<String>*        OperatorsAccepted;

        /**
         * List of operators that the auto-cast does not accept. An operator should not appear
         * in this both lists, this one and list #OperatorsAccepted. However, it is does, then the
         * operator is not accepted.
         *
         * A value of \c nullptr is allowed to indicate no declined operators.
         */
        std::vector<String>*        OperatorsDeclined;

        /**
         * Callback function to add to the program that performs the auto-cast.
         *
         * If \c nullptr is given, then an internal, predefined callback is used, which
         * returns a value of type \alib{expressions,Types::Integer} which is generated by
         * taking the \alib{boxing,Box::Data,raw value} of the argument box. This is
         * especially useful for any boxed enum type that is to be made compatible
         * with bitwise boolean operators (and other integral calculations and functions).
         */
        CallbackDecl                Callback;


        #if ALIB_DEBUG
            /**
             * The C++ name of the callback function (only available in debug compilations of the
             * library. Use preprocessor macro \ref CALCULUS_CALLBACK to provide this field
             * together with field #Callback. The macro selects to prune the name string
             * in release compilations.
             *
             * If #Callback is set to nullptr, the name of the internal function (\e "any2int")
             * is inserted automatically. Instead of aforementioned macro \ref CALCULUS_CALLBACK
             * use macro \ref CALCULUS_DEFAULT_AUTOCAST instead.
             */
            const char*             DbgCallbackName;
        #endif

        /**
         * The result type given a \ref alib_expressions_prereq_sb "sample box".
         *
         * If field #Callback is \c nullptr to specify the use of the internal, default cast
         * function, this field will be ignored and \alib{expressions,Types::Integer}, will
         * be set instead. Hence, in this case, this field can be specified as \c nullptr.
         */
        Box                         ResultType;

        /**
         * This is the name of the function that reverses the cast. The function is used when an
         * expression with an auto-cast functions is \e decompiled to generate compilable,
         * optimized expression strings.
         *
         * \note
         *   This method is needed only if "normalized, optimized expression strings" are
         *   to be generated. For more information on this topic consult manual section
         *   \ref alib_expressions_details_optimizations_norm.
         *
         * \note
         *   Note that, If the aformentioned feature is used, then this function name has to be
         *   provided together with the implementation of the expression function itself,
         *   even if the internal default cast implementation (activated by setting field
         *   #Callback to \c nullptr) is used. The rationale is, that this library can not
         *   automatically convert integral types back to a custom type. This is even true
         *   for simple enumeration types.
         */
        String                      ReverseCastFunctionName;
    };

    /** List of auto-casts to be compiled by this plug-in. */
    std::vector<AutoCastEntry>      AutoCasts;


    /** ********************************************************************************************
     * Searches in #AutoCasts for an entry matching the combination of
     * \alib{expressions,CIBinaryOp::Operator} and the type(s) that might be auto-casted.
     *
     * An entry in #AutoCasts might also be defined to work on just all operators.
     *
     * For the very frequent use case of auto-casting custom enum types to integral types, only
     * fields
     * \alib{expression::Calculus,AutoCastEntry::Type} and
     * \alib{expression::Calculus,AutoCastEntry::ReverseCastFunctionName} have to be provided.
     *
     * \note
     *   This method of this helper class is not applicable (what means that a custom
     *   version has to be implemented) if one of the following items apply to a use case:
     *   - Different auto-casts are to be applied for the first and second arguments of binary
     *     operators.
     *   - The custom auto-cast method is not compile-time invokable.
     *
     * @param[in,out]  autoCast  The compilation info struct.
     * @return \c true if a matching entry was found in #AutoCasts and a corresponding command
     *         was added to \p{autoCast}. \c false otherwise.
     **********************************************************************************************/
    ALIB_API
    virtual bool    TryCompilation( CIAutoCast& autoCast)                                  override;
};



}}} // namespace aworx[::lib::expressions::plugin]

/// Type alias in namespace #aworx.
using     Calculus=    aworx::lib::expressions::plugins::Calculus;

} // namespace [aworx]




#endif // HPP_ALIB_EXPRESSIONS_PLUGINS_CALCULUS
