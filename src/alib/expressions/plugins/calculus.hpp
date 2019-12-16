/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_expressions of the \aliblong.
 *
 * \emoji :copyright: 2013-2019 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_EXPRESSIONS_PLUGINS_CALCULUS
#define HPP_ALIB_EXPRESSIONS_PLUGINS_CALCULUS

#ifndef HPP_ALIB_EXPRESSIONS_COMPILERPLUGIN
#   include "alib/expressions/compilerplugin.hpp"
#endif


namespace aworx { namespace lib { namespace expressions {

/**
 * This inner namespace of module \alib_expressions_nl contains the implementations
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
 * - \alib{expressions::CompilerPlugin,TryCompilation(CIUnaryOp&)},
 * - \alib{expressions::CompilerPlugin,TryCompilation(CIBinaryOp&)}  and
 * - \alib{expressions::CompilerPlugin,TryCompilation(CIAutoCast&)}.
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
 * the built-in plug-ins provided with module \alib_expressions.
 * You will see that these implementations are quite straight forward and use some 'handy' local
 * preprocessor macros that may be copied and used for custom implementations.
 *
 * ## 3. Implementing Identifiers and Functions ##
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
 * - With debug builds, besides the callback function pointer, the C++ name of the callback
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
 * ## 4. Implementing Operators ##
 *
 * ### 4.1 Unary And Binary Operators ###
 * Apart from some specialities for binary operators documented in the next section, this class
 * treats unary and binary the same.
 * Wherever a second operator argument's type is requested, in case of unary operators static
 * type specifier \alib{expressions,Types::Void} is to be given.
 *
 * The compilation of unary and binary operators is supported by populating hash map #Operators.
 * For feeding the map with entries, the following convenience types and methods are provided:
 * - #AddOperator<br>
 *   This function adds compilation information for a single operator to field #Operators.
 *
 * - #OperatorTableEntry<br>
 *   This is a type definition that allows to define tables with compilation information on
 *   operators. It is recommended to create such tables as \c constexpr data in an anonymous
 *   namespace of the compilation unit.
 *
 * - #AddOperators<br>
 *   This is a pair of overloaded functions. One of them is templated and just used to deduce
 *   the length of the given table of static data. This table is then fed as a pointer, together
 *   with the table's size to the second method, which in turn feeds the table entries into
 *   field #Operators.
 *
 * In other words: Method #AddOperator defines a single operator, while #AddOperators defines
 * "bulk" data on operators which is defined in a static table.
 * For details of the functions and types, consult the corresponding documentation.
 *
 * As documented in user manual section
 * \ref alib_expressions_operators_aliases "9.4 Type-Specific Operator Aliases",
 * module \alib_expressions_nl supports the use of alias operators.
 * This is reflected by this class with:
 *
 * - #OperatorAliases<br>
 *   A hash map that collects information about unary and binary operator aliases.
 *
 * - #AddOperatorAlias<br>
 *   This function adds information about an operator alias to field #OperatorAliases.
 *
 * - #OperatorAliasTableEntry
 *   A type definition that allows to define tables with information about operator aliases.
 *   It is recommended to create such tables as \c constexpr data in an anonymous
 *   namespace of the compilation unit.
 *
 * - AddOperatorAliases<br>
 *   This is a pair of overloaded functions. One of them is templated and just used to deduce
 *   the length of the given table of static data. This table is then fed as a pointer, together
 *   with the table size to the second method, which in turn feeds the table entries into
 *   field #OperatorAliases.
 *
 *
 * ### 4.2 Specifics For Binary Operators ###
 *
 * #### Aliasing '==' With '=': ####
 *  With the use of this class it is \e not necessary to define alias <c>'='</c> for binary
 *  operator <c>'=='</c>, because this alias replacement is internally always made for any
 *  combination of argument types, when compilation flag
 *  \alib{expressions,Compilation::AliasEqualsOperatorWithAssignOperator} is set  in field
 *  \alib{expressions,Compiler::CfgCompilation}.
 *
 * #### Aliasing Bitwise Boolean Operators: ####
 *  In contrast to the above, compilation flag \alib{expressions,Compilation::AllowBitwiseBooleanOperators}
 *  affects only built-in type \e boolean - just as the flag's name suggests.
 *  The flag is therefore tested only in derived plug-in \alib{expressions,plugins::Arithmetics}.
 *  In other words: to allow for example operator <c>'&'</c> to be used as an alias for operator
 *  <c>'&&'</c> defined on custom types, this has to be explicitly added as a set alias definitions
 *  for each combination of types in question.
 *
 * #### Support For Compile-Time Optimization: ####
 * For binary operators, this class provides a mechanism to provide information on possible
 * compile-time optimizations.
 * Samples of possible binary operator optimizations are given in documentation of struct
 * \alib{expressions,CompilerPlugin::CIBinaryOp}.
 *
 * The following fields and methods are provided:
 *
 * - #BinaryOperatorOptimizations<br>
 *   A hash-map that collects information about possible optimizations of binary operators
 *   when either of the operands are a specific constant value.
 *
 * - #BinaryOpOptimizationsTableEntry<br>
 *   A type definition that allows to feed tables (arrays of this type) with information about
 *   binary operator optimizations.
 *   It is recommended to create such tables as \c constexpr data in an anonymous
 *   namespace of the compilation unit.
 *
 * - #AddBinaryOpOptimizations<br>
 *   A pair of overloaded functions. One of them is templated and just used to deduce
 *   the length of the given table of static data. This table is then fed as a pointer, together
 *   with the table size to the second method, which in turn feeds the table entries into
 *   hash map #BinaryOperatorOptimizations.
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

    /**
     * This class uses monotonic allocation, which is well supported by the common way how this
     * type is used.
     */
    MonoAllocator          allocator;

    /** ********************************************************************************************
     * Constructor.
     * @param name     Assigned to field \alib{expressions,CompilerPlugin::Name}.
     * @param compiler The compiler we will get attached to
     **********************************************************************************************/
                    Calculus( const NString& name, Compiler& compiler )
    : CompilerPlugin( name, compiler )
    , allocator                  ( 4 * 1024 )
    , Operators                  ( &allocator )
    , OperatorAliases            ( &allocator )
    , BinaryOperatorOptimizations( &allocator )
    {
        #if ALIB_DEBUG_MONOMEM
            allocator.LogDomain= "MA/EXPR/CLCLS";
        #endif
    }

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
             * The C++ name of the callback function (only available with debug builds of the
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
    // Operators
    // #############################################################################################
    protected:

        /** Key type for operator hash maps #Operators and OperatorAliases. */
        struct OperatorKey
        {
            const String           op;          ///< A string defining the operator.
            const std::type_info&  lhs;         ///< Left-hand side type.
            const std::type_info&  rhs;         ///< Right-hand side type. For unary operators
                                                ///< equals to <c>typeid(void)</c>.

            /** Hash functor for operator hash map. */
            struct Hash
            {
                /** Calculates a hash code for objects of type \b OperatorKey.
                 *  @param src The node to hash.
                 *  @return The hash code.                                      */
                std::size_t operator()(const OperatorKey& src)                                 const
                {
                    return      src.op.Hashcode()
                             +  4026031ul * src.lhs.hash_code()
                             +  8175383ul * src.rhs.hash_code();
                }

            };

            /** Equality functor for operator hash map. */
            struct EqualTo
            {
                /** Compares two objects of type \b OperatorKey.
                 *  @param left  The left-hand side object.
                 *  @param right The left-hand side object.
                 *  @return The result of the comparison.                      */
                bool        operator()(const OperatorKey& left, const OperatorKey& right )     const
                {
                    return     left.op       == right.op
                            && left.lhs      == right.lhs
                            && left.rhs      == right.rhs;
                }
            };
        };


    public:

    /**
     * Hash map assigning combinations of (unary and binary) operators and its argument types to a
     * tuple providing information about a callback function.
     *
     * The tuple stored, contains the function pointer and the functions's return type.
     * A third member of type #CTInvokable indicates whether the callback function is allowed to be
     * invoked on the \alib{expressions,Scope} object used at compile-time.
     * This scope object is of the same (eventually custom) type as the one for evaluation, however
     * the evaluation-specific data is not set. In other words, the third tuple member denotes
     * if during program compilation the function might be invoked when the operator's argumeent(s)
     * are constant.
     *
     * A fourth tuple member of type \alib{strings,TString,String} is available only with debug builds
     * and receives the name of the callback function.
     *
     * \note
     *   This map, similar to map #OperatorAliases is best to be filled using corresponding
     *   \e add-methods #AddOperator and #AddOperators.<br>
     *   Usually this is done once in the constructor of derived classes.
     */
    HashMap<OperatorKey,
            std::tuple<CallbackDecl, Box, CTInvokable  ALIB_DBG( , const char* ) >,
            OperatorKey::Hash,
            OperatorKey::EqualTo> Operators;

    /**
     * Hash map assigning combinations of alias versions of operators and their argument types to
     * the original operator.
     *
     * \note
     *   This map, similar to map #Operators is best to be filled using corresponding
     *   \e add-methods #AddOperatorAlias and #AddOperatorAliases.<br>
     *   Usually this is done once in the constructor of derived classes.
     */
    HashMap<OperatorKey,
            String,
            OperatorKey::Hash,
            OperatorKey::EqualTo> OperatorAliases;


    /**
     * Entry of input tables (arrays) used with methods #AddOperators to perform bulk-loading of
     * compile definition data into map #Operators.<br>
     * The tuple elements are:
     * - The operator to compile.
     * - The type of the first argument of the operator.
     * - The type of the right-hand side argument of the operator.
     *   For unary operators, value \alib{expressions,Types::Void} is to be provided.
     * - The callback function. Set to \c nullptr if operator evaluates constant.
     * - The C++ name of the callback function. (This tuple element is only available in debug
     *   compilations of the library.)
     * - The result type sample box, respectively, if \b callback is \c nullptr, the constant result
     *   value.
     * - Flag to denote if the callback function allows compile-time invocation and thus on constant
     *   input the program can be optimized. This is true e.g. for arithmetic functions, but usually
     *   not for custom operators that rely on scope objects available only at evaluation time.
     */
    using OperatorTableEntry=      const std::tuple< String, Type, Type,
                                                     CallbackDecl,
                                                     ALIB_DBG(const char* ,)
                                                     Type, CTInvokable>;

   /**
    * Entry of input tables (arrays) used with method #AddOperatorAliases to perform bulk-loading
    * of operator alias definition data into map #OperatorAliases.<br>
    * The tuple elements are:
    * - The alias operator.
    * - The type of first argument of the operator.
    * - The type of the right-hand side argument of the operator.
    *   For unary operators, value \alib{expressions,Types::Void} is to be provided.
    * - The operator that gets aliased.
    */
    using OperatorAliasTableEntry= const std::tuple<String, Type, Type, String>;



    #if defined(ALIB_DOX)
    /** ********************************************************************************************
     * Adds an entry to the operator definition map #Operators.
     *
     * \see
     *    If multiple operators are to be defined, consider the use of #AddOperators, which is a
     *    variant of this method that allows effective bulk loading.
     *
     * @param op              The operator.
     * @param lhsType         The type of the first argument that the operator is defined for.
     * @param rhsType         The type of the right-hand side argument that the operator is defined
     *                        for.
     *                        For unary operators, value \alib{expressions,Types::Void} is to be
     *                        provided.
     * @param callback        The callback function to execute.
     * @param dbgCallbackName The name of the C++ name of the callback function.
     *                        \note This parameter is available only in debug version of the
     *                        library.
     * @param cti             See #CTInvokable for the meaning of this flag.
     * @param resultType      The result type of the callback function.
     **********************************************************************************************/
    void AddOperator     ( const String& op, Type lhsType, Type rhsType, CallbackDecl callback,
                       #if ALIB_DEBUG
                           const char*   dbgCallbackName,
                       #endif
                           Type resultType,
                           CTInvokable   cti     );
    #else // clang would complain about the doxing of parameter dbgCallbackName
    ALIB_API
    void AddOperator     ( const String& op, Type lhsType, Type rhsType, CallbackDecl callback,
                       #if ALIB_DEBUG
                           const char*   dbgCallbackName,
                       #endif
                           Type resultType,
                           CTInvokable   cti     );
    #endif


    /** ********************************************************************************************
     * Templated helper method. Deduces the array size of the given table and passes it
     * to \ref AddOperators(OperatorTableEntry* table, size_t length).
     *
     * @tparam TCapacity Implicitly deferred size of the array provided.
     * @param  table     The table containing operator compilation information.
     **********************************************************************************************/
    template<size_t TCapacity>
    void AddOperators    ( OperatorTableEntry (&table) [TCapacity]  )
    {
        AddOperators( &table[0], TCapacity );
    }

    /** ********************************************************************************************
     * Loads all entries of the given table into hash map #Operators.
     *
     * Note, that usually, the given table is a constexpr array located in an anonymous namespace
     * of a compilation unit.<br>
     * It can be passed as a reference to templated helper method, which defers the length of the
     * table implicitly.
     *
     * @param  table     The table containing operator compilation information.
     * @param  length    The table containing operator compilation information.
     **********************************************************************************************/
    ALIB_API
    void AddOperators    ( OperatorTableEntry* table, size_t length );

    /** ********************************************************************************************
     * Adds an alias operator to hash table #OperatorAliases.
     *
     * \see
     *    If multiple alias operators are to be defined, consider the use of #AddOperatorAliases,
     *    which is a variant of this method that allows effective bulk loading.
     *
     * @param alias    The alias for operator \p{op}.
     * @param lhsType  The left-hand side argument type that the operator is defined for.
     * @param rhsType  The right-hand side argument type that the operator is defined for.
     * @param op       The operator aliased by \p{alias}.
     **********************************************************************************************/
    void AddOperatorAlias  ( const String& alias, Type lhsType, Type rhsType, const String& op );

    /** ********************************************************************************************
     * Templated helper method. Deduces the array size of the given table and passes it
     * to \ref AddOperatorAliases(OperatorAliasTableEntry* table, size_t length).
     *
     * @tparam TCapacity Implicitly deferred size of the array provided.
     * @param  table     The table containing operator compilation information.
     **********************************************************************************************/
    template<size_t TCapacity>
    void AddOperatorAliases( OperatorAliasTableEntry (&table) [TCapacity]  )
    {
        AddOperatorAliases( &table[0], TCapacity );
    }

    /** ********************************************************************************************
     * Loads all entries of the given table into hash map #OperatorAliases.
     *
     * Note, that usually, the given table is a constexpr array located in an anonymous namespace
     * of a compilation unit.<br>
     * It can be passed as a reference to templated helper method, which defers the length of the
     * table implicitly.
     *
     * @param  table     The table containing operator compilation information.
     * @param  length    The table containing operator compilation information.
     **********************************************************************************************/
    void AddOperatorAliases( OperatorAliasTableEntry* table, size_t length );


    // #############################################################################################
    // Binary operator optimizations
    // #############################################################################################
    protected:

        /** Key type for operator hash maps #Operators and OperatorAliases. */
        struct BinOpOptKey
        {
            const String           op;        ///< The operator to optimize.
            Side                   constSide; ///< Denotes a left- or right-hand side optimization.
            const Box              constVal;  ///< The type and value of the constant argument.
            const std::type_info&  other;     ///< The type of the non-constant argument.

            /** Hash functor for operator hash map. */
            struct Hash
            {
                /** Calculates a hash code for objects of type \b OperatorKey.
                 *  @param src The node to hash.
                 *  @return The hash code.                                      */
                std::size_t operator()(const BinOpOptKey& src)                                 const
                {
                    return  (   std::hash<String>()(src.op)
                              +  6949ul * std::hash<Box>()(src.constVal)
                              + 14033ul * src.other.hash_code()
                            ) ^ ( src.constSide == Side::Left ? static_cast<size_t>( 0)
                                                              : static_cast<size_t>(-1) );
                }
            };

            /** Equality functor for operator hash map. */
            struct EqualTo
            {
                /** Compares two objects of type \b OperatorKey.
                 *  @param lhs The left-hand side object.
                 *  @param rhs The left-hand side object.
                 *  @return The result of the comparison.                      */
                bool        operator()(const BinOpOptKey& lhs, const BinOpOptKey& rhs )        const
                {
                    return     lhs.op       == rhs.op
                            && lhs.constSide== rhs.constSide
                            && lhs.constVal == rhs.constVal
                            && lhs.other    == rhs.other;
                }
            };
        };

    public:
    /**
     * Hash map storing optimization information for binary operators where either argument is
     * constant.<br>
     * This map may be filled with #AddBinaryOpOptimizations, which is usually done in the.
     * constructor of derived classes.
     *
     * The stored element of type \b Box may contain either, a constant result value that replaces
     * the binary operator (as in <c> x || true</c>) or be a \e nulled box, which indicates that
     * the result equals the non-constant argument (as in <c>x && true</c>).
     */
    HashMap <BinOpOptKey, Box,
             BinOpOptKey::Hash,
             BinOpOptKey::EqualTo>                              BinaryOperatorOptimizations;

    /**
     * Entry of arrays used with methods #AddBinaryOpOptimizations to perform bulk-loading of
     * optimization data to hash map #BinaryOperatorOptimizations.<br>
     * The tuple element's meanings are:
     * - The operator to optimize.
     * - Denotes if an optimization applies if the left-hand side or right-hand side argument
     *   is constant.
     * - The type and value of the constant argument.
     * - The type of the non-constant argument.
     * - Either, a constant result value that replaces the binary operator
     *   (as in <c> x || true</c>) or a \e nulled box, which indicates that the result equals the
     *   non-constant argument (as in <c>x && true</c>).
     */
    using BinaryOpOptimizationsTableEntry= const std::tuple<String, Side, Type, const Box&, const Box&>;


    /** ********************************************************************************************
     * Templated helper method. Deduces the array size of the given table and passes it
     * to \ref AddBinaryOpOptimizations(BinaryOpOptimizationsTableEntry*, size_t).
     *
     * @tparam TCapacity Implicitly deferred size of the array provided.
     * @param  table     The table containing operator compilation information.
     **********************************************************************************************/
    template<size_t TCapacity>
    void AddBinaryOpOptimizations( BinaryOpOptimizationsTableEntry (&table) [TCapacity] )
    {
        AddBinaryOpOptimizations( &table[0], TCapacity );
    }

    /** ********************************************************************************************
     * Loads all entries of the given table into hash map #BinaryOperatorOptimizations.
     *
     * Note, that usually, the given table is a constexpr array located in an anonymous namespace
     * of a compilation unit.<br>
     * It can be passed as a reference to templated helper method, which defers the length of the
     * table implicitly.
     *
     * @param  table     The table containing operator compilation information.
     * @param  length    The table containing operator compilation information.
     **********************************************************************************************/
    ALIB_API
    void AddBinaryOpOptimizations( BinaryOpOptimizationsTableEntry* table, size_t length );

    /** ********************************************************************************************
     * Searches in #Operators for an entry matching the combination of
     * \alib{expressions,CIUnaryOp::Operator} and the argument type of operand found with iterator
     * \alib{expressions,CompilationInfo::ArgsBegin}.
     * (The second argument type of the key of the hash map #Operators is set to
     * \alib{expressions,Types::Void}).
     * If found, the corresponding callback function and result type are added the \p{CIUnaryOp}.
     *
     * Before the search, it is checked whether the given operator is an alias for another
     * operator. Operator aliases might be defined by filling map #OperatorAliases in the
     * constructor of the derived types.
     * If so, the corrected operator is returned with in/out parameter
     * \alib{expressions,CIUnaryOp::Operator}.
     *
     * @param[out]    ciUnaryOp    The compilation result.
     * @return \c true if an entry was found in #Operators and a corresponding command was added to
     *         \p{ciUnaryOp}. \c false otherwise.
     **********************************************************************************************/
    ALIB_API
    virtual bool    TryCompilation( CIUnaryOp& ciUnaryOp )                                 override;


    /** ********************************************************************************************
     * Searches in #Operators for an entry matching the combination of
     * \alib{expressions,CIBinaryOp::Operator} and the argument types of operands found with
     * argument iterators
     * \alib{expressions,CompilationInfo::ArgsBegin} and
     * \alib{expressions,CompilationInfo::ArgsEnd}.
     * If found, the corresponding callback function and result type are added the \p{CIBinaryOp}.
     *
     * Before the search, it is checked whether the given operator is an alias for another
     * operator. Operator aliases might be defined by filling map #OperatorAliases in the
     * constructor of the derived types.
     * If so, the corrected operator is returned with in/out parameter
     * \alib{expressions,CIBinaryOp::Operator}.
     *
     * @param[in,out]  ciBinaryOp  The compilation info struct.
     * @return \c true if an entry was found in #Operators and a corresponding command was added
     *         to \p{ciBinaryOp}. \c false otherwise.
     **********************************************************************************************/
    ALIB_API
    virtual bool    TryCompilation( CIBinaryOp& ciBinaryOp )                               override;


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
             * The C++ name of the callback function (only available with debug builds of the
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
     * \alib{expressions,CIAutoCast::Operator} and the type(s) that might be auto-casted.
     *
     * An entry in #AutoCasts might also be defined to work on just all operators.
     *
     * For the very frequent use case of auto-casting custom enum types to integral types, only
     * fields
     * \alib{expression::Calculus,AutoCastEntry::Type} and
     * \alib{expression::Calculus,AutoCastEntry::ReverseCastFunctionName} have to be provided.
     *
     * \note
     *   This method of this helper class is not applicable if one of the following conditions apply
     *   to a use case:
     *   - Different auto-casts are to be applied for the first and second arguments of binary
     *     operators.
     *   - The custom auto-cast method is not compile-time invokable.
     *
     * \note
     *   In this case, a custom implementation of this method has to be provided to fetch
     *   these cases. The custom method might then invoke this base implementation.
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
using     Calculus=    lib::expressions::plugins::Calculus;

} // namespace [aworx]




#endif // HPP_ALIB_EXPRESSIONS_PLUGINS_CALCULUS
