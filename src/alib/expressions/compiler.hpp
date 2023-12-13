/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_expressions of the \aliblong.
 *
 * \emoji :copyright: 2013-2023 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_EXPRESSIONS_COMPILER
#define HPP_ALIB_EXPRESSIONS_COMPILER

#ifndef HPP_ALIB_EXPRESSIONS_EXPRESSION
#   include "alib/expressions/expression.hpp"
#endif

#ifndef HPP_ALIB_FS_PLUGINS_PLUGINS
#   include "alib/lib/fs_plugins/plugins.hpp"
#endif

#if !defined(HPP_ALIB_COMPATIBILITY_STD_TYPEINFO)
#   include "alib/compatibility/std_typeinfo.hpp"
#endif

#if !defined(HPP_ALIB_STRINGS_UTIL_TOKEN)
#   include "alib/strings/util/token.hpp"
#endif

#if !defined(HPP_ALIB_TEXT_FWDS)
#   include "alib/text/fwds.hpp"
#endif

#if !defined(HPP_ALIB_RESULTS_REPORT)
#   include "alib/results/report.hpp"
#endif

#if !defined (HPP_ALIB_MONOMEM_HASHMAP)
#   include "alib/monomem/hashmap.hpp"
#endif

#if !defined (HPP_ALIB_MONOMEM_LIST)
#   include "alib/monomem/list.hpp"
#endif

namespace aworx { namespace lib { namespace expressions {

// forwards
namespace detail { struct Parser; }
struct  CompilerPlugin;

/** ************************************************************************************************
 * An implementation of this abstract interface class may be attached to field
 * \alib{expressions,Compiler::Repository} to enable an \e automated the definition and retrieval
 * of expression strings of nested expressions.
 *
 * A default implementation, which uses typical \alib mechanics to define expression
 * strings, is given with class \alib{expressions,StandardRepository}.
 *
 * More information about automated nested expressions and the use of this interface class is
 * provided with manual chapter \ref alib_expressions_nested_config.
 **************************************************************************************************/
class ExpressionRepository
{
    public:
        /** Virtual destructor.   */
        virtual ~ExpressionRepository() {}

        /** ****************************************************************************************
         * This method is called during compilation of expressions in the case a named expression
         * is not found by method \alib{expressions,Compiler::GetNamed}.
         *
         * @param      identifier  The name of the required expression.
         * @param[out] target      The target to write the requested expression string to.
         * @return \c true, if the expression string could be retrieved, \c false otherwise.
         *         If \c true is returned and \p{target} is still empty, then the string is defined
         *         to be empty, which throws an exception on compilation.
         ******************************************************************************************/
        virtual bool        Get( const String& identifier, AString& target )                    = 0;
};

/** ************************************************************************************************
 * This is a central class of library module \alib_expressions_nl used to compile
 * expression strings.
 *
 * For information about general use and features of this class consult the
 * \ref aworx::lib::expressions "ALib Expressions User Manual".
 *
 * ## Friends ##
 * class \alib{expressions,Expression}
 **************************************************************************************************/
class Compiler  : public lib::detail::PluginContainer<CompilerPlugin, CompilePriorities>
{
    #if !defined(ALIB_DOX)
        friend class  Expression;
        friend struct detail::Parser;
        friend class  detail::Program;
    #endif

    public:
       /** Alias shortcut to the type of the plug-in vector inherited from \b %PluginContainer. */
       using Plugins= std::vector<lib::detail::PluginContainer<CompilerPlugin, CompilePriorities>::Slot>;

    // #############################################################################################
    // internal fields
    // #############################################################################################
    protected:
        /** Memory for used for permanent allocations during the set-up phase.
         *  Later it is also used for temporary allocations during compilation and reset to its
         *  state after setup. */
        MonoAllocator                       allocator;

        /** The expression parser. */
        detail::Parser*                     parser                                        = nullptr;

        /** The map of Type names and bit flag values. */
        HashMap<TypeFunctors::Key, NAString,
                TypeFunctors::Hash,
                TypeFunctors::EqualTo>      typeMap;

        /** The map of 'named' expressions. */
        HashMap< AString, SPExpression,
                 std::hash    <String>,
                 std::equal_to<String> >    namedExpressions;


    // #############################################################################################
    // Public fields
    // #############################################################################################
    public:

        /**
         * The list of unary operators.
         * To define a new unary operator, method #AddUnaryOperator should be used.
         * In debug-builds, the methods asserts that no double insertions are performed.
         *
         * Custom insertions may be performed before or after invoking #SetupDefaults,
         * but before a first expression is compiled.
         *
         *
         * Flag \alib{expressions,Compilation::DefaultUnaryOperators} controls if method
         * \b %SetupDefaults adds operators resourced with enumeration
         * \alib{expressions,DefaultUnaryOperators}.
         */
        List<String>                       UnaryOperators;

        /**
         * This public map allows to define alias names for unary operators. Such names
         * must consist only of alphabetic characters.
         *
         * Flag \alib{expressions,Compilation::DefaultAlphabeticOperatorAliases} controls if method
         * #SetupDefaults adds the aliases defined with resourced data records of enumeration
         * \alib{expressions,DefaultAlphabeticUnaryOperatorAliases} to this map.
         */
        HashMap<String, String,
                aworx::hash_string_ignore_case<character>,
                aworx::equal_to_string_ignore_case<character> > AlphabeticUnaryOperatorAliases;

        /**
         * This public map allows to define alias names for binary operators. Such names
         * must consist only of alphabetic characters.
         *
         * Flag \alib{expressions,Compilation::DefaultAlphabeticOperatorAliases} controls if method
         * #SetupDefaults adds the aliases defined with resourced data records of enumeration
         * \alib{expressions,DefaultAlphabeticBinaryOperatorAliases} to this map.
         */
        HashMap<String, String,
                aworx::hash_string_ignore_case<character>,
                aworx::equal_to_string_ignore_case<character> > AlphabeticBinaryOperatorAliases;

        /**
         * Adds an unary operator to this expression compiler.
         * This method should be invoked prior to prior to invoking #SetupDefaults.
         *
         * Operator symbols must be added only once.
         *
         * @param symbol The operator symbol.
         */
        void AddUnaryOperator( const String& symbol  )
        {
            #if ALIB_DEBUG
                for( auto& op : UnaryOperators )
                    if( op.Equals( symbol ) )
                        ALIB_ASSERT_ERROR( false, "EXPR", "Unary operator {!Q'} already defined.", symbol )
            #endif
            UnaryOperators.EmplaceBack(symbol);
        }


        /**
         * The list of binary operators. The map stores the precedences.
         *
         * To define a new binary operator, entry may be added prior to invoking #SetupDefaults
         * using method #AddBinaryOperator.
         *
         * Flag \alib{expressions,Compilation::DefaultUnaryOperators} controls if method
         * \b %SetupDefaults adds operators resourced with enumeration
         * \alib{expressions,DefaultBinaryOperators}.
         */
        HashMap<String, int>                BinaryOperators;

        /**
         * Adds a binary operator to this expression compiler.
         * This method should be invoked prior to prior to invoking #SetupDefaults.
         *
         * Operator symbols must be added only once.
         *
         * @param symbol        The operator symbol.
         * @param precedence    The precedence of the operator.
         */
        void AddBinaryOperator( const String& symbol, int precedence )
        {
            ALIB_DBG( auto result= )
            BinaryOperators.EmplaceOrAssign(symbol, precedence);
            ALIB_ASSERT_ERROR( result.second == true, "EXPR", // was inserted
                               "Binary operator {!Q'} already defined.", symbol )
        }

        /**
         * Returns the precedence of given operator \b symbol.
         *
         * @param symbol        The operator symbol.
         * @return The precedence of the operator as defined with #AddBinaryOperator.
         */
        int GetBinaryOperatorPrecedence( const String& symbol )
        {
            // search in operator table first
            auto it= BinaryOperators.Find( symbol );
            if( it != BinaryOperators.end() )
                return it.Mapped();

            // have an alias?
            auto aliasOp= AlphabeticBinaryOperatorAliases.Find( symbol );
            ALIB_ASSERT_ERROR( aliasOp != AlphabeticBinaryOperatorAliases.end(), "EXPR",
                               "Unknown binary operator {!Q'}.", symbol       )

            it= BinaryOperators.Find( aliasOp.Mapped() );
            ALIB_ASSERT_ERROR( it != BinaryOperators.end(), "EXPR",
                               "Unknown binary operator {!Q'} which was aliased by {!Q'}.",
                               aliasOp->second, symbol       )
            return it.Mapped();
        }


        /**
         * \alib{enums,T_EnumIsBitwise,Bitwise enum class} that lists the plug-ins built-into
         * \alib expression library. This bitfield is used with method #SetupDefaults, which
         * reads the flags set in #CfgBuiltInPlugins and installs the plug-ins accordingly.
         */
        enum class BuiltInPlugins
        {
            NONE               =  0,         ///< Installs no plug-in.
            ALL                = -1,         ///< Installs all plug-ins.
            ElvisOperator      = (1 << 1),   ///< Installs \alib{expressions,plugins::ElvisOperator}.
            AutoCast           = (1 << 2),   ///< Installs \alib{expressions,plugins::AutoCast}.
            Arithmetics        = (1 << 3),   ///< Installs \alib{expressions,plugins::Arithmetics}.
            Math               = (1 << 4),   ///< Installs \alib{expressions,plugins::Math}.
            Strings            = (1 << 5),   ///< Installs \alib{expressions,plugins::Strings}.
ALIB_IF_SYSTEM(
            DateAndTime        = (1 << 6), ) ///< Installs \alib{expressions,plugins::DateAndTime}. )
        };


        /**
         * Bitfield that defines the built-in compiler plug-ins that are created and inserted
         * by method #SetupDefaults.
         *
         * Defaults to \alib{expressions::Compiler,BuiltInPlugins::ALL}
         *
         * \note
         *   The built-in plug-ins are allowed to be shared by different compiler instances.
         *   To reach that, they must not be disabled here (or method #SetupDefaults must be
         *   omitted) and instead created once and registered with the compiler using
         *   inherited method \alib{detail::PluginContainer::InsertPlugin}. In that case, parameter
         *   \p{responsibility} of that method has to be provided as
         *   \c Responsibility::KeepWithSender.
         */
        BuiltInPlugins              CfgBuiltInPlugins                         = BuiltInPlugins::ALL;

        /**
         * The operator defined in this public field is used to address nested expressions.
         * The operator used can be customized to any defined (!) unary operator.
         *
         * To disable nested expression parsing, change to empty or \e nulled string.
         *
         * \note
         *   If changed, the change hast to be made \b prior to invoking #SetupDefaults. Otherwise,
         *   the default configuration for
         *   \ref alib_expressions_operators_verbal "verbal operator aliases" would be set in
         *   effect to a wrong (or disabled) target operator.
         *
         * Defaults to \c "*".
         *
         * \see Manual chapter \ref alib_expressions_nested "10. Nested Expressions".
         *
         */
        String                      CfgNestedExpressionOperator                   = A_CHAR("*");

        /**
         * Name descriptor for nested expression function.
         * Defaults to "Expression" with minimum abbreviation of \c 4 characters, ignoring letter
         * case.<br>
         * Resourced with key \c "EF"
         *
         * \see Manual chapter \ref alib_expressions_nested "10. Nested Expressions".
         */
        strings::util::Token        CfgNestedExpressionFunction;

        /**
         * Keyword used with optional third parameter of expression function.
         * Defaults to \c "throw". Resourced with key \c "EF"
         *
         * \see Manual chapter \ref alib_expressions_nested "10. Nested Expressions".
         */
        String                      CfgNestedExpressionThrowIdentifier;

        /**
         * Compilation flags.
         */
        Compilation                 CfgCompilation                           = Compilation::DEFAULT;

        /**
         * Flags that allow to tweak the result of the normalization of the originally parsed
         * expression string.
         */
        Normalization               CfgNormalization                       = Normalization::DEFAULT;

        /**
         * This list is used with the normalization of the expression string. It contains
         * strings, which are not allowed to appear in normalized expression strings.
         *
         * In the current library version, such strings might occur only if flags
         * \alib{expressions,Normalization::UnaryOpSpace},
         * \alib{expressions,Normalization::UnaryOpSpaceIfUnaryFollows} or
         * \alib{expressions,Normalization::BinaryOpSpaces} are not set.
         * As a sample see expression:
         *
         *          1 - -2
         *
         * If no space is written after binary operators, this expression would be normalized
         * to:
         *          1--2
         *
         * While this just looks irritating to a homo sapiens, it would be even illegal if
         * well known C++ operator <c>--</c> was supported. Therefore, string <c>"--"</c> is added
         * to this map in method #SetupDefaults, which forces the normalization logic to insert
         * a space, even if spaces are otherwise configured to be omitted.
         */
        std::vector<AString>        CfgNormalizationDisallowed;

        /**
         * Formatter used throughout all phases of the life-cycle of an expression:
         * - Used for parsing expression strings (Using the encapsulated
         *   \alib{strings,TNumberFormat,NumberFormat} object. This is also the reason, why the type
         *   of formatter is \b %Formatter and not its more abstract base % \b %Formatter.)
         * - Used for the generation of the normalized expression strings.
         * - Used for the generation of constant string objects during compilation.
         * - Used for the generation of string objects during evaluation.
         *
         * For the latter two, a copy of the pointer is provided through field
         * \alib{expressions,Scope::Formatter}.
         * It is important not to change the formatting parameters in between the creation of the
         * compile-time scope and the creation of the evaluation-time scope. If done,
         * the expression results could differ when optimizations during compilation are performed.
         *
         * In the constructor of this class, a \alib{text,Formatter::Clone,clone} of the
         * \alib \alib{text::Formatter,GetDefault,default formatter} is set here.
         */
        SPFormatter                 CfgFormatter;

        /**
         * Optional pointer to a default or custom implementation of a repository that provides
         * expression strings for named nested expressions.
         *
         * \see Manual chapter \ref alib_expressions_nested_config.
         */
        ExpressionRepository*       Repository                                            = nullptr;

    // #############################################################################################
    // Public Interface
    // #############################################################################################
    public:
        /** ****************************************************************************************
         * Constructor. Construction usually is three steps:
         * 1. Create class instance
         * 2. Set the various configuration options (fields prefixed <c>Cfg</c>).
         * 3. Invoke #SetupDefaults.
         ******************************************************************************************/
        ALIB_API            Compiler();

        /** ****************************************************************************************
         * Destructor
         ******************************************************************************************/
        ALIB_API
        virtual            ~Compiler();

        /** ****************************************************************************************
         * Registers a (custom) type name with this compiler. The name will be used to
         * display a type name to end-users, for example, when malformed expressions throw an
         * exception.
         *
         * The built-in types get registered in the constructor of the compiler, where the
         * type names are read from the \alib{resources,ResourcePool} of static library object
         * \alib{EXPRESSIONS}.
         *
         * It is recommended that custom plug-ins invoke this method in their constructor.
         *
         * \note
         *   There is no general need to register types for using them
         *   with \alib_expressions_nl. Parameter  \p{name} of this method is exclusively
         *   used to generate textual, human readable output!
         *
         * @param sample    A \ref alib_expressions_prereq_sb "sample value"
         *                  of the type, which is implicitly boxed when passed.
         * @param name      The name of the type.
         ******************************************************************************************/
        ALIB_API
        void                AddType(Type sample, const NString& name);

        /** ****************************************************************************************
         * Returns the name of the type of the boxed value.
         * Note that custom types need to be registered with method #AddType.
         *
         * If given argument \p{box} is \ref alib_boxing_more_void_void "a void box", the returned
         * string is <b>"NONE"</b>.
         *
         * @param box  The box to receive the type name for.
         * @return The type name as string.
         ******************************************************************************************/
        ALIB_API
        NString             TypeName(Type box);

        /** ****************************************************************************************
         * Writes the signature of a function (as for example found in
         * \alib{expressions::plugins,Calculus::FunctionEntry}) to a string buffer.
         *
         * @param boxArray  The start of the array of pointers.
         * @param qty       The array's length.
         * @param target    The string to write into.
         ******************************************************************************************/
        ALIB_API
        void                WriteFunctionSignature( Box** boxArray,  size_t qty,  AString& target );

        /** ****************************************************************************************
         * Writes the signature of an argument list to a string buffer.
         *
         * @param begin     Iterator to the first argument.
         * @param end       End-iterator.
         * @param target    The string to write into.
         ******************************************************************************************/
        ALIB_API
        void                WriteFunctionSignature( ArgIterator     begin,
                                                    ArgIterator     end,
                                                    AString&        target   );

        /** ****************************************************************************************
         * Completes construction according to configuration options provided with fields
         * prefixed <c>Cfg</c>.
         * In detail, the following actions are performed:
         * - If flag \alib{expressions,Compilation::DefaultUnaryOperators} is set in field
         *   #CfgCompilation, then
         *   - the unary operators listed in the \ref alib_enums_records "enum records" of
         *     \alib{expressions,DefaultUnaryOperators} are added using #AddUnaryOperator.
         *   - If flag \alib{expressions,Compilation::DefaultAlphabeticOperatorAliases} is set in
         *     field #CfgCompilation, then the alphabetic alias names found in
         *     the data records of enumeration
         *     \alib{expressions,DefaultAlphabeticUnaryOperatorAliases}
         *     are set in field #AlphabeticUnaryOperatorAliases. (As of the current version this
         *     is only the token \b NOT aliasing operator <b>"!"</b>.
         * - If flag \alib{expressions,Compilation::DefaultBinaryOperators} is set in field
         *   #CfgCompilation, then
         *   - the binary operators listed in the data records of enumeration
         *     \alib{expressions,DefaultBinaryOperators} are added using #AddBinaryOperator.
         *   - If flag \alib{expressions,Compilation::AllowSubscriptOperator} is not set, the
         *     subscript operator is omitted.
         *   - If flag \alib{expressions,Compilation::DefaultAlphabeticOperatorAliases} is set in
         *     field #CfgCompilation, then the alphabetic alias names found in
         *     the data records of enumeration
         *     \alib{expressions,DefaultAlphabeticBinaryOperatorAliases}
         *     are set in field #AlphabeticBinaryOperatorAliases. (As of the current version this
         *     is only the token \b NOT aliasing operator <b>"!"</b>.
         * - Strings <b>"++"</b> and <b>"--"</b> are added to field #CfgNormalizationDisallowed
         *   to prevent the unintentional creation of these potential operators in normalizations.
         *   (This way, a space character will be added between binary <b>"+"</b> and unary
         *    <b>"+"</b>, respectively binary <b>"-"</b> and unary <b>"-"</b> operators,
         *   even if the normalization options do not suggest to do this.
         * - Depending on the flags set in #CfgBuiltInPlugins, the plug-ins listed in
         *   enumeration #BuiltInPlugins are created and added.
         ******************************************************************************************/
        ALIB_API
        void                SetupDefaults();

        /** ****************************************************************************************
         * Firstly, this method parses the given expression string thereby builds an intermediate
         * internal \alib{expressions,detail::AST,abstract syntax tree}.
         * This tree is then translated into a \alib{expressions,detail::Program} which evaluates
         * the expression in respect to a \alib{expressions,Scope} when executed.
         *
         * For building the program, attached \alib{expressions,CompilerPlugin} instances
         * are queried.
         *
         * During this process, a \alib{expressions,Normalization,normalized} version of the input
         * string is always created. While the original input string can
         * be accessed with \alib{expressions,Expression::GetOriginalString}, the normalized version
         * can be retrieved after the compilation with method
         * \alib{expressions,Expression::GetNormalizedString}. During compilation, the normalization
         * rules (flags) stored in field #CfgNormalization are applied.
         *
         * For compilation, a compile-time scope object is created by calling virtual method
         * #getCompileTimeScope. If not overwritten, a standard scope object is used.
         * If custom callbacks exists that are invokable at compile-time
         * (to perform program optimization) and that at the same time rely on custom mechanics
         * provided with a custom scope class, then a custom, derived version of this class
         * has to be used that overrides method #getCompileTimeScope accordingly.
         *
         * @param expressionString  The string to parse.
         * @return A shared pointer to the compiled expression. Contains \c nullptr on failure.
         ******************************************************************************************/
        virtual ALIB_API
        SPExpression        Compile( const String& expressionString );

        /** ****************************************************************************************
         * Compiles the given \p{expressionString} and adds a compiled expression to the map of
         * named expressions.
         *
         * @param name              The name used to map the expression.
         * @param expressionString  The string to parse.
         *                          If nulled, entry \p{name} is removed (if existing).
         * @return \c true if an expression with the same named existed and was replaced.
         *         Otherwise returns \c false.
         ******************************************************************************************/
        virtual ALIB_API
        bool                AddNamed( const String& name, const String& expressionString );

        /** ****************************************************************************************
         * Removes a named expression. This is just a shortcut to #AddNamed, providing
         * a nulled string for parameter \p{expressionString}.
         *
         * @param name  The name of the expression to be removed.
         * @return \c true if an expression with the given name existed and was removed.
         *         Otherwise returns \c false.
         ******************************************************************************************/
        bool                RemoveNamed( const String& name )
        {
            return AddNamed( name, NullString() );
        }

        /** ****************************************************************************************
         * Returns a named expression previously defined using #AddNamed.
         * In the case that no expression with the given named was defined and optional interface
         * #Repository is set, then this interface is used to retrieve a corresponding expression
         * string and compile the named expression 'on the fly'.
         *
         * \see
         *   Manual chapter \ref alib_expressions_nested_config.
         *
         * @param name  The name of the expression.
         * @return In case of success a shared pointer to the expression.
         ******************************************************************************************/
        virtual ALIB_API
        SPExpression        GetNamed( const String& name );

    // #############################################################################################
    // Protected Interface
    // #############################################################################################
    protected:
        /** ****************************************************************************************
         * This virtual method is called by #Compile to create the scope object used
         * for allocations that are made for intermediate results at compile-time.
         *
         * Compile-time allocations occur when expression terms that use purely constant arguments
         * are evaluated at compile-time. Such optimization consequently leads to the invocation
         * of callback functions at compile-time.
         *
         * If now, custom callback functions rely on custom allocation mechanics, provided with
         * custom scope types, then this method has to be overridden to return such custom
         * scope object.
         *
         * @return The default implementation returns the standard scope object.
         *         For its construction, field #CfgFormatter is provided.<br>
         *         Derived compilers may need to return a scope object of a derived type.
         ******************************************************************************************/
        virtual  ALIB_API
        Scope*              getCompileTimeScope();

        /** ****************************************************************************************
         * Implements \alib{expressions,Expression::GetOptimizedString}.
         *
         * @param expression the expression to generate an optimized string for.
         ******************************************************************************************/
        ALIB_API
        void                getOptimizedExpressionString( Expression* expression );
};


}} // namespace aworx[::lib::expressions]

/// Type alias in namespace #aworx.
using     Compiler=           lib::expressions::Compiler;


}// namespace [aworx]

ALIB_ENUMS_MAKE_BITWISE(aworx::lib::expressions::Compiler::BuiltInPlugins)


#endif // HPP_ALIB_EXPRESSIONS_COMPILER
