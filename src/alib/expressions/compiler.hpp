// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#ifndef HPP_ALIB_EXPRESSIONS_COMPILER
#define HPP_ALIB_EXPRESSIONS_COMPILER

#ifndef HPP_ALIB_EXPRESSIONS_EXPRESSION
#   include "alib/expressions/expression.hpp"
#endif

#if ALIB_MODULE_CONFIGURATION
#   ifndef HPP_ALIB_CONFIG_CONFIGURATION
#       include "alib/config/configuration.hpp"
#   endif
#endif

#ifndef HPP_ALIB_PLUGINCONTAINER
#   include "alib/lib/plugincontainer.hpp"
#endif

#if !defined(HPP_ALIB_LIB_TYPEMAP)
#    include "alib/lib/typemap.hpp"
#endif

#if !defined(HPP_ALIB_STRINGS_UTIL_TOKEN)
#   include "alib/strings/util/token.hpp"
#endif

#if !defined(HPP_ALIB_STRINGFORMAT_FWDS)
    #include "alib/stringformat/fwds.hpp"
#endif

namespace aworx {

// forwards

namespace lib { namespace expressions {
namespace detail { struct Parser; }

struct  CompilerPlugin;

/** ************************************************************************************************
 * This is a central class of library module \alibmod_nolink_expressions used to compile
 * expression strings.
 *
 * For information about general use and features of this class consult the
 * \ref aworx::lib::expressions "ALib Expressions User Manual".
 *
 * ## Friends ##
 * class \alib{expressions,Expression}
 **************************************************************************************************/
class Compiler  : public PluginContainer<CompilerPlugin, CompilePriorities>
{
    #if !ALIB_DOCUMENTATION_PARSER
        // Expressions are friends.
        friend class Expression;
    #endif

    public:
       /** Alias shortcut to the type of the plug-in vector inherited from \b %PluginContainer. */
       using Plugins= std::vector<PluginContainer<CompilerPlugin, CompilePriorities>::Slot>;

    // #############################################################################################
    // internal fields
    // #############################################################################################
    protected:
        /** The parser based on \c boost::spirit. */
        detail::Parser*                         parser                                    = nullptr;


        /** The map of Type names and bit flag values. */
        TypeMap<NString>                        types;


        /** The map of 'named' expressions. */
        UnorderedAStringMap<SPExpression>       namedExpressions;

        #if ALIB_MODULE_CONFIGURATION
            /** A variable object, reused (performance vs memory) */
            Variable        var;
        #endif // ALIB_MODULE_CONFIGURATION


    // #############################################################################################
    // Public fields
    // #############################################################################################
    public:

        /**
         * The list of unary operators.
         * To define a new unary operator, entry may be added prior to invoking #SetupDefaults
         * using method #AddUnaryOperator.
         *
         * Flag \alib{expressions,Compilation::DefaultUnaryOperators} controls if method
         * \b %SetupDefaults adds operators resourced with enumeration
         * \alib{expressions,DefaultUnaryOperators}.
         */
        std::vector<String>                     UnaryOperators;

        /**
         * This public map allows to define alias names for unary operators. Such names
         * must consist only of alphabetic characters.
         *
         * Flag \alib{expressions,Compilation::DefaultAlphabeticOperatorAliases} controls if method
         * #SetupDefaults adds the aliases defined with resourced meta data of enumeration
         * \alib{expressions,DefaultAlphabeticUnaryOperatorAliases} to this map.
         */
        UnorderedStringMapIgnoreCase<String>    AlphabeticUnaryOperatorAliases;

        /**
         * This public map allows to define alias names for binary operators. Such names
         * must consist only of alphabetic characters.
         *
         * Flag \alib{expressions,Compilation::DefaultAlphabeticOperatorAliases} controls if method
         * #SetupDefaults adds the aliases defined with resourced meta data of enumeration
         * \alib{expressions,DefaultAlphabeticBinaryOperatorAliases} to this map.
         */
        UnorderedStringMapIgnoreCase<String>    AlphabeticBinaryOperatorAliases;

        /**
         * Adds a unary operator to this expression compiler.
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
                    {
                        ALIB_ASSERT_ERROR( false, "Unary operator '{}' already defined.", symbol )
                    }
            #endif
            UnaryOperators.emplace_back(symbol);
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
        UnorderedStringMap<int>                 BinaryOperators;

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
            ALIB_ASSERT_ERROR( BinaryOperators.find( symbol ) == BinaryOperators.end(),
                               "Binary operator '{}' already defined.", symbol )
            BinaryOperators[symbol]= precedence;
        }

        /**
         * Returns the precedence of given operator \b symbol.
         *
         * @param symbol        The operator symbol.
         * @return The precedence of the operator as defined with #AddBinaryOperator.
         */
        int GetBinaryOperatorPrecedence( const String& symbol )
        {
            auto it= BinaryOperators.find( symbol );
            if( it == BinaryOperators.end() )
            {
                auto aliasOp= AlphabeticBinaryOperatorAliases.find( symbol );

                ALIB_ASSERT_ERROR( aliasOp != AlphabeticBinaryOperatorAliases.end(),
                                   "Unknown binary operator '{}'.", symbol       )

                it= BinaryOperators.find( aliasOp->second );
                ALIB_ASSERT_ERROR( it != BinaryOperators.end(),
                                   "Unknown binary operator '{}' which was aliased by '{}'.",
                                   aliasOp->second, symbol       )
            }
            return it->second;
        }


    #if ALIB_MODULE_CONFIGURATION

        /**
         * This object defaults to \c nullptr and may be set in during bootstrap of the comiler
         * object (at the same time the plug-ins are attached, etc.)
         *
         * If set, method #getExpressionString will use this object to try to find named
         * expression in the configuration.
         */
        Configuration*          Config                                                    = nullptr;

        /**
         * May be filled with configuration category names. A named expression's identifier will
         * be first tried to be loaded using each category name added here. If this fails,
         * it is checked if an underscore <c>'_'</c>character is found in the name, if yes,
         * it is then tried to extract the category from the variable name.
         *
         * Note: Adding an empty string allows to search in the unnamed-category as well.
         */
        std::vector<AString>    DefaultCategories;

        /**
         * Within this vector, all variables loaded 'automatically' from #Configuration with
         * default implementation of method #getExpressionString are stored here. The tuple
         * string values provide:
         * - the priority,
         * - the category name,
         * - the variable name and
         * - the name of the nested expression.
         *
         * The latter is needed, as it might differ from the combination of the category and
         * variable name due to definitions in #DefaultCategories.
         *
         * This vector might be used for various things, e.g. logging out status information.
         * It is also used by method StoreLoadedExpressions.
         *
         * \note
         *   This list is never cleared by this class. It might be cleared from outside
         *   (e.g. to release memory) without causing any side effects, other than what
         *   an application itself volunteers to do with this information.
         */
        std::vector<std::tuple<Priorities,AString,AString,String>>  VariablesLoaded;
    #endif

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
#if ALIB_MODULE_SYSTEM
            DateAndTime        = (1 << 6),   ///< Installs \alib{expressions,plugins::DateAndTime}.
#endif
        };


        /**
         * Bitfield that defines the built-in compiler plug-ins that are created and inserted
         * by method #SetupDefaults.
         *
         * Defaults to \alib{expressions::Compiler,BuiltInPlugins::ALL}
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
        String                      CfgNestedExpressionOperator                         = A_CHAR("*");

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
         *   \alib{strings,TNumberFormat,NumberFormat} object. This is also the reason, why the type of
         *   formatter is \b %Formatter and not its more abstract base % \b %Formatter.)
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
         * In the constructor of this class, a \alib{stringformat,Formatter::Clone,clone} of the
         * \alib \alib{stringformat::Stringformat,GetDefaultFormatter,default formatter} is set here.
         */
        SPFormatter              CfgFormatter;

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
         * The built-in types get registered in the constructor of the compiler as follows:
         * \snippet "alib/expressions/compiler.cpp"  DOX_ALIB_EXPRESSIONS_COMPILER_REGISTERING_TYPES
         *
         * It is recommended that custom plug-ins invoke this method in their constructor.
         *
         * \note
         *   There is no general need to register types for using them
         *   with \alibmod_nolink_expressions. Parameter  \p{name} of this method is exclusively
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
         * @param identifier        The name used to map the expression.
         * @param expressionString  The string to parse.
         *                          If \c nullptr an existing expression is removed.
         * @return \c true if an expression with the same named existed and was replaced.
         *         Otherwise returns \c false.
         ******************************************************************************************/
        virtual ALIB_API
        bool                AddNamed( const String& identifier, const String& expressionString );

        /** ****************************************************************************************
         * Removes a named expression. This is just a shortcut to #AddNamed, providing
         * \c nullptr for parameter \p{expressionString}.
         *
         * @param identifier        The name of the named expression to be removed.
         * @return \c true if an expression with the given name existed and was removed.
         *         Otherwise returns \c false.
         ******************************************************************************************/
        inline
        bool                RemoveNamed( const String& identifier )
        {
            return AddNamed( identifier, NullString() );
        }

        /** ****************************************************************************************
         * Returns a named expression. Either this expression has to have been added using
         * #AddNamed prior to retrieving it, or implementations of this class overwrite protected
         * method #getExpressionString, which allows to receive expression strings 'on the fly'.
         *
         * @param identifier  The name of the expression.
         * @return In case of success a shared pointer to the expression.
         ******************************************************************************************/
        virtual ALIB_API
        SPExpression        GetNamed( const String& identifier );

        #if ALIB_MODULE_CONFIGURATION
            /** ************************************************************************************
             * Stores back all expression strings which had been automatically loaded from
             * the plugin specified by \p{slot} to configuration system. For storing, the parsed
             * expression description is used. Thus, this method may be used to write a "clean"
             * expression string, that does not use abbreviations for identifiers, has no
             * unnecessary whitespaces, etc.
             *
             * Note the importance of parameter \p{slot}: Usually, only such expression variables
             * should be stored that had been loaded from a configuration file. Expression strings
             * provided e.g. via CLI parameters should be considered temporary ones and hence not
             * written.
             *
             * @param slot  The slot of the configuration plug-in of which loaded variables are to
             *              be written back.
             * @return The number of variables written.
             **************************************************************************************/
            ALIB_API
            int                 StoreLoadedExpressions( Priorities slot= Priorities::Standard );
        #endif // ALIB_MODULE_CONFIGURATION

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
         * @return The default implementation returns the standard scope object. As constructor
         *         parameter \p{formatter}, field #CfgFormatter is provided.<br>
         *         Derived types may need to return a scope object of derived type.
         ******************************************************************************************/
        virtual  ALIB_API
        Scope*              getCompileTimeScope();

        /** ****************************************************************************************
         * This method is called by #GetNamed if a named expression requested is not found.
         *
         * If nested expressions that have not been 'manually' added using #AddNamed, are to be
         * supported, this method has either be overwritten to receive expression strings in a
         * customized way for the given expression \p{name}, or the built-in mechanism of loading
         * expression strings from the \ref #aworx::lib::config "ALib Configuration System"
         * has to be activated. Activation is done by setting field #Configuration to a valid
         * instance prior to using the compiler.
         *
         * The latter allows to have expressions located in INI-files, provided with command line
         * parameters and so forth.
         *
         * @param identifier  The name of the expression
         * @param target      The target to write the expression string to.
         * @return This \c true, if the expression string could be retrieved, \c false otherwise.
         *         If \c true is returned and \p{target} is still empty, then the string is defined
         *         to be empty, which throws an exception on compilation.
         ******************************************************************************************/
        virtual ALIB_API
        bool                getExpressionString( const String& identifier, AString& target );

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
using     Compiler=           aworx::lib::expressions::Compiler;


}// namespace [aworx]

ALIB_ENUM_IS_BITWISE(aworx::lib::expressions::Compiler::BuiltInPlugins)


#endif // HPP_ALIB_EXPRESSIONS_COMPILER
