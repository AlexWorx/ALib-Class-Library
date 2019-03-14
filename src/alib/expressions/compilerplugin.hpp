// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################

#ifndef HPP_ALIB_EXPRESSIONS_COMPILERPLUGIN
#define HPP_ALIB_EXPRESSIONS_COMPILERPLUGIN

#ifndef HPP_ALIB_EXPRESSIONS_EXPRESSIONS
#   include "alib/expressions/expressions.hpp"
#endif

#ifndef HPP_ALIB_EXPRESSIONS_SCOPE
#   include "alib/expressions/scope.hpp"
#endif

#ifndef HPP_ALIB_EXPRESSIONS_COMPILER
#   include "alib/expressions/compiler.hpp"
#endif

namespace aworx { namespace lib { namespace expressions {


/** ************************************************************************************************
 * This interface class represents "plug-ins" which are attachable to container class
 * \alib{expressions,Compiler}. The compiler uses the plug-ins to <em>"compile"</em> single
 * nodes of <em>abstract syntax trees</em>, which are the intermediate, internal result of parsing
 * expression strings.
 *
 * In depth information on how this class is used is given in the
 * \ref aworx::lib::expressions "User Manual And Tutorial" of module \alibmod_nolink_expressions.
 *
 * The overloaded methods #TryCompilation are not abstract, but have a default implementation
 * that return constant \c false. A plug-in derived from this class needs to override only those
 * methods that represent syntax elements of an expression, that it volunteers to take
 * responsibility for.
 *
 * The overloaded methods have one input/output parameter which is of one of the following types:
 * - \alib{expressions::CompilerPlugin,CIFunction},
 * - \alib{expressions::CompilerPlugin,CIUnaryOp},
 * - \alib{expressions::CompilerPlugin,CIBinaryOp} or
 * - \alib{expressions::CompilerPlugin,CIAutoCast}.
 *
 * These inner types of this struct are derived from likewise inner struct
 * \alib{expressions::CompilerPlugin,CompilationInfo}, which provides all output members and
 * those input members that are common between the four descendants. In other words, the variants
 * of the struct provide detail information on the node that is actually compiled.
 *
 * When overriding one of the overloaded methods #TryCompilation, it has to be checked if
 * the custom plug-in is responsible for the permutation of given parameter types which are passed.
 * If so, the actual 'compilation' is performed, by filling the result members of the given
 * struct and by returning \c true. Each overloaded
 * method provides a suitable return type, which are defined as inner types of this class.
 *
 * The output information comprises a \ref alib_expressions_prereq_sb "sample box" that
 * determines the result type of the native C++ function that is later invoked when the expression
 * is evaluated against a scope.
 * Together with this type information, the pointer to the callback function itself needs to be
 * set in the given struct.
 *
 * Alternatively, if a constant result is to be returned, the pointer to the callback function
 * might be kept \e nulled. In this case, the \b %Box object that represents the result type
 * of the callback function is used to provide the constant value, instead of just a sample value.
 *
 * There are two possibilities, why a compiler plug-in might return a constant value, instead of
 * a function:
 * 1. \e Identifiers (parameterless functions) might just represent constant values. (For example
 *    identifiers \c "True" or \c "Monday" do that, while \c "Today" returns a callback function.).
 * 2. The compiler provides information about whether the given parameters are constant values.
 *    In this case, a compiler plug-in may decide to evaluate the result of the function at
 *    compile-time. This is often, but not always possible and depends largely on the fact
 *    if scope information is used with the function as well. <br>
 *    For more details on this topic see manual section
 *    \ref alib_expressions_details_optimizations "11.5 Optimizations".
 *
 * \attention
 *   If constant data is returned, it has to be assured, that the contents of the returned
 *   \b %Box remains valid during the lifecycle of the expression. This is assured for all
 *   C++ fundamental types. For custom types it depends on where the constant value is received
 *   from and how boxing is performed.
 *   (By default, custom types bigger as two "words" (2 x 64/32 bits) are boxed as pointers to the
 *   assigned object.)<br>
 *   Field \alib{expressions::CompilerPlugin,CompilationInfo::CompileTimeScope}
 *   is to be used for compile-time allocations.
 *
 * The plug-ins are attached to the \b %Compiler using a dedicated prioritization defined
 * with (arithmetical) enum type \alib{expressions,CompilePriorities}.
 * This means, that if a higher prioritized plug-in already compiles a certain
 * permutation of expression node type and argument types, then a lower prioritized plug-in is not
 * even asked to do so. With this concept, <b>ALib Expressions</b> provides a lot of flexibility:
 * the built-in operators and identifiers can be unplugged completely or just sparsely "superseded"
 * in certain aspects  by adding a custom \b %CompilerPlugin with a higher priority than that of
 * the built-in one.
 *
 * \note
 *   Instead of deriving from this struct, it is recommended to consider convenience struct
 *   \alib{expressions,plugins::Calculus} instead.
 *
 * \note
 *   More information on this topic is given in manual section
 *   \ref alib_expressions_cpcc_calculus "5.4 Class Calculus"
 **************************************************************************************************/
struct CompilerPlugin
{
    /**
     * The name of the plug-in. This is used with exception information and
     * \alib{expressions::detail,VirtualMachine::DbgList,program listings} which are available in
     * debug-compilations of the library.
     */
    const NString   Name;

    /** The compiler that this plug-in is attached to. */
    Compiler&       Cmplr;

    /** ********************************************************************************************
     * Public inner base struct which provides input and output information for compiling
     * single entities (nodes of the \e AST) of a parsed expression.
     *
     * Descendant types
     * - \alib{expressions::CompilerPlugin,CIFunction},
     * - \alib{expressions::CompilerPlugin,CIUnaryOp} and
     * - \alib{expressions::CompilerPlugin,CIBinaryOp}
     *
     * extend this struct with input information, specific to the type of <em>AST</em>-node
     * currently compiled. Special, descendant type
     * - \alib{expressions::CompilerPlugin,CIAutoCast}
     *
     * in addition adds specific output members.
     *
     * Together, these four descendants comprise the parameters of the four overloaded methods
     * \alib{expressions::CompilerPlugin,TryCompilation}.
     **********************************************************************************************/
    struct CompilationInfo
    {
        /**
         * The scope found here is the same object that is passed to method
         * \alib{expressions,Compiler::Compile}, which internally invokes the overloaded
         * methods #TryCompilation, that receive an object of this type.
         *
         * If a compiled expression syntax element evaluates to a constant stored in
         * #TypeOrValue it has to be assured that the boxed data is to available during the
         * lifecycle of the expression.
         *
         * To allocate custom compilation data, a custom, derived type, might for example be
         * extended with simple\c std::vector of pointers to the created objects.
         * (Attention: Vectors of value-types must not be used, as with their growth, the objects
         * get moved within the heap memory!).<br>.
         * Any data allocated, is to be deleted in the virtual destructor of the custom scope type.
         */
        Scope&              CompileTimeScope;

        /**
         * Input: A start iterator to \ref alib_expressions_prereq_sb "sample boxes"
         * that provide the argument types to search a native callback function for.<br>
         * In case that descendant classes denote that arguments found in this iterator are
         * constant, then the boxes are not just sample boxes, but contain the constant values.
         */
        ArgIterator         ArgsBegin;

        /**
         * Input: The end iterator to #ArgsBegin. The expression <c>ArgsEnd - ArgsBegin</c> may be
         * used to receive the number of arguments provided.
         */
        ArgIterator         ArgsEnd;

        #if ALIB_DEBUG
            /**
             * Output: The C++ name of the callback function. This field is available only in debug
             * compilations of the library. Hence, setting it must be performed with preprocessor
             * conditionals.
             */
            const nchar*    DbgCallbackName                                               = nullptr;
        #endif

        /**
         *  Output: The native C++ callback function to be set by one of the plug-ins.
         */
        CallbackDecl        Callback                                                      = nullptr;

        /**
         * Output: Specifies the return type of #Callback, respectively, as the name indicates, the
         * result value in case of constant results.<br>
         * Note that in case of constant compile-time values, it might be necessary to allocate
         * compile-time memory for the values. For this, field #CompileTimeScope is to be used.
         */
        Box                 TypeOrValue                                                   = nullptr;


        /**
         * Constructor.
         * @param scope   The scope usable for allocation of constant values (compile-time
         *                allocations).
         */
        CompilationInfo( Scope& scope )
        : CompileTimeScope( scope               )
        , ArgsBegin       ( scope.Stack.begin() )
        , ArgsEnd         ( scope.Stack.end()   )
        {}
    };

    /** ********************************************************************************************
     * Info struct for compiling expression identifiers and functions.
     * This struct is used with method \ref TryCompilation(CIFunction&) to provide information
     * to derived compiler plug-ins, as well as to receive information back.
     **********************************************************************************************/
    struct CIFunction     : public CompilationInfo
    {
        /**
         * Input: The identifier name to search.
         */
        AString&                Name;

        /**
         * Input: If the function was given as a pure "identifier" this flag is \c true.
         * \see See also flag
         *      \alib{expressions::Compilation,AllowOmittingParenthesesOfParameterlessFunctions} of
         *      field  \see See also \alib{expressions,Compiler::CfgCompilation.}.
         */
        bool                    NoParentheses;

        /**
         * Input: Denotes if all arguments provided are constant values. Operator callbacks that do
         * not use context information from the scope, should calculate the then constant result
         * and return this value instead of the callback method. (Compile-time optimization.)
         */
        bool                    AllArgsAreConst;

        /**
         * A plug-in may add names of functions that matched, while the arguments did not.
         * This will be stored in the details of a potential exception, if no other plug-in
         * compiles this function and may be displayed to the end-user.
         */
        std::vector<AString>&   FunctionsWithNonMatchingArguments;

        /**
         * Constructor.
         * @param scope          Passed to parent.
         * @param name           Stored in #Name.
         * @param noParentheses  Stored in #NoParentheses.
         * @param argsAreConst   Passed to #AllArgsAreConst.
         * @param hints          Stored in #FunctionsWithNonMatchingArguments.
         */
        CIFunction( Scope&                scope,
                    AString&              name,
                    bool                  noParentheses,
                    bool                  argsAreConst,
                    std::vector<AString>& hints        )
        : CompilationInfo( scope )
        , Name           (name)
        , NoParentheses  (noParentheses)
        , AllArgsAreConst(argsAreConst)
        , FunctionsWithNonMatchingArguments(hints)
        {}


        /**
         * Returns the number of arguments given.
         * @return The number of arguments the function call requested.
         */
        inline size_t QtyArgs()
        {
            return static_cast<size_t>( ArgsEnd - ArgsBegin );
        }

        /**
         * Returns the argument number \p{no}.
         * @param no The number of the argument requested.
         * @return A reference to the requested argument.
         */
        inline Box& Arg( size_t no )
        {
            return *(ArgsBegin + static_cast<ptrdiff_t>( no ) );
        }


    };

    /** ********************************************************************************************
     * Info struct for compiling expression an unary operation.
     * This struct is used with method \ref TryCompilation(CIUnaryOp&) to provide information
     * to derived compiler plug-ins, as well as to receive information back.
     **********************************************************************************************/
    struct CIUnaryOp      : public CompilationInfo
    {
        String&         Operator;   ///< Input/Output: The unary operation.

        /**
         * Input: Denotes if the argument is a constant value. Operator callbacks that do
         * not use context information from the scope, should calculate the then constant result
         * and return this value instead of the callback method. (Compile-time optimization.)
         */
        bool            ArgIsConst;


        /**
         * Constructor.
         * @param scope Passed to parent.
         * @param operation  Stored in #Operator.
         * @param argIsConst Passed to field #ArgIsConst.
         *
         */
        CIUnaryOp( Scope& scope, String& operation, bool argIsConst )
        : CompilationInfo( scope      )
        , Operator       ( operation  )
        , ArgIsConst     ( argIsConst )
        {}
    };

    /** ********************************************************************************************
     * Info struct for compiling expression a binary operation.
     * This struct is used with method \ref TryCompilation(CIBinaryOp&) to provide information
     * to derived compiler plug-ins, as well as to receive information back.
     *
     * Information about whether the arguments are constants is separately given for \e lhs and
     * \e rhs with fields #LhsIsConst and #RhsIsConst.
     *
     * If both flags evaluate to \c true, operator callback functions that do not use context
     * information from the scope (or otherwise rely on external or random data), should calculate
     * the - then constant - result at compile-time and return a constant value instead of the
     * callback method.
     *
     * If exactly one of the argument is constant, then some operators might detect further
     * possibilities of optimization. Such optimization is operator-specific but goes along the
     * lines of the following samples:
     *
     *      term    +       0     -> term
     *      term    -       0     -> term
     *      term    *       0     -> 0
     *      term    *       1     -> term
     *      term    /       1     -> term
     *      term   &&   false     -> false
     *      term   &&    true     -> term
     *      term   ||    true     -> true
     *      term   ||   false     -> term
     *
     * These cases have in common that the result of the operator is either a constant
     * or equals the non-constant argument.
     *
     * In both cases inherited field \c %Callback is to be left \c nullptr. If the result is
     * constant, inherited field \b %TypeOrValue is to be set to the constant value.
     * In the second case that the non-constant argument equals the result, this can be indicated
     * by setting field #NonConstArgIsResult to \c true. In this case, the compiler will choose
     * the non-constant argument and drop the other.
     *
     * \note
     *   Class \alib{expressions::plugins,Calculus} which specializes this plug-in class,
     *   provides a convenient way to define the optimization rules described here, along with
     *   its mechanics to support binary operator compilation.
     **********************************************************************************************/
    struct CIBinaryOp     : public CompilationInfo
    {
        String&         Operator;     ///< Input/Output: The binary operation symbol.

        /** Input: Denotes if the lhs-argument is a constant value. */
        bool            LhsIsConst;

        /** Input: Denotes if the rhs-argument is a constant value. */
        bool            RhsIsConst;

        /** Output: Used with optimization, see this struct's documentation for more information. */
        bool            NonConstArgIsResult;


        /**
         * Constructor.
         * @param scope        Passed to parent.
         * @param op           Stored in #Operator.
         * @param lhsIsConst   Stored in #LhsIsConst.
         * @param rhsIsConst   Stored in #RhsIsConst.
         */
        CIBinaryOp( Scope& scope, String& op, bool lhsIsConst, bool rhsIsConst)
        : CompilationInfo    ( scope      )
        , Operator           ( op         )
        , LhsIsConst         ( lhsIsConst )
        , RhsIsConst         ( rhsIsConst )
        , NonConstArgIsResult( false      )
        {}
    };

    /** ********************************************************************************************
     * Info struct for compiling automatic type casts. Such automatic cast is tried to be inserted
     * into the expression program by the compiler if:
     *
     * - An unary operator for a type can not be found.
     * - A binary operator for a combination of types can not be found.
     * - Two different types for \c T and \c F were given with conditional operator <c>Q ? T : F</c>.
     *
     * For which the scenarios a cast is needed can be determined with field #Op and also by
     * checking the number of given arguments.
     *
     * Built-in compiler plug-in \alib{expressions::plugins,AutoCast} ignores unary operations.
     * For binary operations, it just always tries to match both types to the 'major' one. It does
     * this, as it is more probable, that for two same types an operator is available.<br>
     *
     * In contrast to this, a custom plug-in may choose to cast both values to a joint one or to
     * any combination of types that it provides an operator for!
     *
     * Compile-time optimization is supported with auto-casts the same as with other compilation
     * mechanics.
     * Information about whether the arguments are constants is separately given for the first and
     * second argument with fields #IsConst and #RhsIsConst.
     *
     * Hence, if a plug-in leaves the parent field
     * \alib{expressions::CompilerPlugin::CompilationInfo,Callback} and/or field #CallbackRhs
     * \e nulled, but stores a constant casted result value in
     * \alib{expressions::CompilerPlugin::CompilationInfo,TypeOrValue} and/or #TypeOrValueRhs,
     * then this is detected by the compiler and instead of inserting a cast function call, the
     * original constant value is replaced with the returned constant value(s).
     *
     * If a cast function is compiled (returned with this struct), and the resulting program
     * should be duly \e "decompilable", then along with the callback information, a
     * compilable expression function name has to be returned in field
     * #ReverseCastFunctionName, respectively #ReverseCastFunctionNameRhs.
     * For further information on this topic see
     * \ref alib_expressions_details_optimizations_norm "11.5.6 Optimized Expression Strings"
     **********************************************************************************************/
    struct CIAutoCast  : public CompilationInfo
    {
        /**
         * The operator that the cast is required for. If this is <b>'?:'</b> then
         * the request is made for conditional operator <c>Q ? T : F</c>. In this case, the
         * requirement is to cast both given arguments to the same type - otherwise, the
         * conditional operator does not compile!
         */
        String&             Op;

        /** Input: denotes if the unary argument, respectively the lhs argument of a binary
            operation, is a constant value. */
        bool                IsConst;

        /** Input: denotes if rhs argument is constant value. */
        bool                RhsIsConst;

        /**
         * Output: Native C++ callback function to cast the first type with.
         * \note
         *   The optional callback function for casting the left-hand side type is returned with
         *   inherited field \alib{expressions::CompilerPlugin::CompilationInfo,Callback}.
         */
        CallbackDecl        CallbackRhs;

        /**
         * Output: Specifies the return type of #CallbackRhs, respectively, as the name indicates,
         * the result value in case of constant a result.<br>
         * In case of constant compile-time values, it might be necessary to allocate
         * compile-time memory for the values. For this, field #CompileTimeScope is to be used.
         * \note
         *   The optional return type and value casting the left-hand side argument is returned with
         *   inherited field \alib{expressions::CompilerPlugin::CompilationInfo,TypeOrValue}.
         */
        Box                 TypeOrValueRhs;

        /**
         * This is the name of the left-hand side cast function, respectively that of the unary
         * argument's cast function, that is used when an expression with auto-cast
         * functions is \e decompiled to generate compilable, optimized expression strings.
         */
        String              ReverseCastFunctionName;

        /**
         * This is the name of the right-hand side cast function that is inserted when an expression
         * with an auto-cast functions is \e decompiled to generate compilable, optimized expression
         * strings.
         */
        String              ReverseCastFunctionNameRhs;

        #if ALIB_DEBUG
            /**
             * Output: The C++ name of the callback function. This field is available only in debug
             * compilations of the library. Hence, setting it must be performed with preprocessor
             * conditionals.
             */
            const nchar*    DbgCallbackNameRhs;
        #endif

        /**
         * Constructor.
         * @param scope         Passed to parent.
         * @param op            Stored in #Op.
         * @param isConst       Stored in IsConst.
         * @param rhsIsConst    Stored in RhsIsConst.
         */
        CIAutoCast( Scope& scope, String& op, bool isConst, bool rhsIsConst )
        : CompilationInfo   ( scope       )
        , Op                ( op          )
        , IsConst           ( isConst     )
        , RhsIsConst        ( rhsIsConst  )
        , CallbackRhs       ( nullptr     )
        , TypeOrValueRhs    ( nullptr     )
        {
        }
    };

    /** ********************************************************************************************
     * Constructor.
     * @param name      Assigned to field #Name.
     * @param compiler  The compiler we will get attached to. Gets stored in field #Cmplr.
     **********************************************************************************************/
                CompilerPlugin( const NString& name, Compiler& compiler )
    : Name( name )
    , Cmplr( compiler)
    {}

    /** ********************************************************************************************
     * Virtual destructor
     **********************************************************************************************/
    virtual    ~CompilerPlugin()
    {}


    /** ********************************************************************************************
     * Used to compile identifiers (parameterless functions ) and functions parsed from
     * expression strings.
     *
     * The function name is given as an in/out parameter. Implementations might (should) choose to
     * - on the one hand allow abbreviations and letter case insensitive identifier recognitions, and
     * - consequently return 'corrected' identifier names if an identifier name matched and the
     *   element got compiled.
     *
     * Such corrected names will appear in the normalized expression strings returned by
     * \alib{expressions,Expression::GetNormalizedString}, in case flag
     * \alib{expressions,Normalization::ReplaceFunctionNames} is set in field
     * \alib{expressions,Compiler::CfgNormalization}.
     *
     * On success, this method has to provide a (native C++) callback function that accepts
     * start and end iterators of boxed arguments which are of the same time as proposed by
     * the corresponding iterators found in parameter \p{ciFunction}, along with the return type
     * of that function.
     *
     * Alternatively, if a constant identifier is compiled or if all parameters are known to be
     * constant at compile-time, a constant value might be returned.
     * For details of the input and output parameters of the function, see struct
     * \alib{expressions::CompilerPlugin,CIFunction}.
     *
     * @param[in,out]  ciFunction  The compilation info struct.
     * @return Implementations have to return \c true if the given info struct was filled, or in
     *         other words, if the plug-in chose to compile the <em>AST</em>-node.<br>
     *         This default implementation returns \c false to indicate that no compilation
     *         was done.
     **********************************************************************************************/
    virtual bool TryCompilation( CIFunction&  ciFunction )      { (void) ciFunction; return false; }

    /** ********************************************************************************************
     * Used to compile unary operators parsed from expressions.
     *
     * On success, this method has to provide a native C++ callback function together with
     * a \ref alib_expressions_prereq_sb "sample box" that specifies its return type.
     * Both are to be stored in output parameter \p{result}.
     *
     * Alternatively, if field \c %ArgIsConst of the given compilation info struct is \c true,
     * a constant value might be returned.
     * For details of the input and output parameters of the function, see struct
     * \alib{expressions::CompilerPlugin,CIUnaryOp}.
     *
     * The implementation might allow alias operators. If such alias is found, the used original
     * operator should be returned in/out parameter \p{operation}.
     *
     * Such "corrected" operators will appear in the normalized expression strings returned by
     * \alib{expressions,Expression::GetNormalizedString}, in case flag
     * \alib{expressions,Normalization::ReplaceAliasOperators} is set in field
     * \alib{expressions,Compiler::CfgNormalization}.
     *
     * @param[in,out]  ciUnaryOp  The compilation info struct.
     * @return Implementations have to return \c true if the given info struct was filled, or in
     *         other words, if the plug-in chose to compile the <em>AST</em>-node.<br>
     *         This default implementation returns \c false to indicate that no compilation
     *         was done.
     **********************************************************************************************/
    virtual bool TryCompilation( CIUnaryOp&   ciUnaryOp )        { (void) ciUnaryOp; return false; }

    /** ********************************************************************************************
     * Used to compile binary operators parsed from expressions.
     *
     * On success, this method has to provide a native C++ callback function together with
     * a \ref alib_expressions_prereq_sb "sample box" that specifies its return type.
     * Both are to be stored in output parameter \p{result}.
     *
     * Alternatively, depending on fields \c %LhsIsConst and \c %RhsIsConst of the given compilation
     * info struct, a constant value might be returned or, as a third alternative, field
     * \c %NonConstArgIsResult may be set to \c true, if an "identity" operation is detected
     * For details of the input and output parameters of the function, see struct
     * \alib{expressions::CompilerPlugin,CIBinaryOp}.
     *
     * The implementation might allow alias operators. If such alias is found, the used original
     * operator should be returned in/out parameter \p{operation}.
     *
     * Such "corrected" operators will appear in the normalized expression strings returned by
     * \alib{expressions,Expression::GetNormalizedString}, in case flag
     * \alib{expressions,Normalization::ReplaceAliasOperators} is set in field
     * \alib{expressions,Compiler::CfgNormalization}.
     *
     *
     * @param[in,out]  ciBinaryOp  The compilation info struct.
     * @return Implementations have to return \c true if the given info struct was filled, or in
     *         other words, if the plug-in chose to compile the <em>AST</em>-node.<br>
     *         This default implementation returns \c false to indicate that no compilation
     *         was done.
     **********************************************************************************************/
    virtual bool TryCompilation( CIBinaryOp&  ciBinaryOp )      { (void) ciBinaryOp; return false; }

    /** ********************************************************************************************
     * Used to provide information to the compiler for casting types.
     *
     * For details on how this method is overridden, consult the documentation of the input/output
     * parameter type \alib{expressions::CompilerPlugin,CIAutoCast}.
     *
     *
     * @param[in,out]  ciAutoCast  The compilation info struct.
     * @return Implementations have to return \c true if the given info struct was filled, or in
     *         other words, if the plug-in chose to provide auto-cast information as requested.<br>
     *         This default implementation returns \c false to indicate that no compilation
     *         was done.
     **********************************************************************************************/
    virtual bool TryCompilation( CIAutoCast& ciAutoCast )
    { (void) ciAutoCast; return false; }

};

}} // namespace aworx[::lib::expressions]

/// Type alias in namespace #aworx.
using     CompilerPlugin=    aworx::lib::expressions::CompilerPlugin;

} // namespace [aworx]


#endif // HPP_ALIB_EXPRESSIONS_COMPILERPLUGIN
