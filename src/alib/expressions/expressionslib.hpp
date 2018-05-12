// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
/** @file */ // Hello Doxygen

// check for alib.hpp already there but not us
#if !defined (HPP_ALIB)
    #error "include \"alib/alib.hpp\" before including this header"
#endif
#if defined(HPP_COM_ALIB_TEST_INCLUDES) && defined(HPP_ALIB_EXPRESSIONS_EXPRESSIONSLIB)
    #error "Header already included"
#endif

// then, set include guard
#ifndef HPP_ALIB_EXPRESSIONS_EXPRESSIONSLIB
//! @cond NO_DOX
#define HPP_ALIB_EXPRESSIONS_EXPRESSIONSLIB 1
//! @endcond

#if !defined(HPP_ALIB_LANG_EXCEPTION)
    #include "alib/lang/exception.hpp"
#endif

/**
 * @addtogroup GrpALibCompilerSymbols
 * @{
 *  \def  ALIB_FEAT_EXPRESSIONS_SPIRIT_PARSER_ON
 *    This compiler symbol enables the use of alternative an an alternative parser for
 *    module \alibmod_expressions based on
 *    [boost::spirit](http://www.boost.org/doc/libs/1_66_0/libs/spirit/doc/html/index.html).
 *
 *    Note that setting this flag does not only choose to compile the \c boost::spirit based
 *    parser, but also choose to use it by default.
 *
 *    \attention
 *      This is an experimental feature only. Some features of this library will be silently (!)
 *      lost with its activation.
 *      It is discouraged to use the \c boost::spirit parser in combination with this library in
 *      production environments!
 *
 *    \attention
 *      Please read the according \ref alib_expressions_appendix_boostspirit "manual section"
 *      for more information on the use of the \c boost::spirit parser.
 *
 *  \def  ALIB_FEAT_EXPRESSIONS_SPIRIT_PARSER_OFF
 *    This compiler symbol disables the use of an alternative parser for module \alibmod_expressions based
 *    on [boost::spirit](http://www.boost.org/doc/libs/1_66_0/libs/spirit/doc/html/index.html).
 *
 *    This is the default, hence the symbol is not needed to be passed, but it is available for
 *    completeness.<br>
 *
 *    See \ref ALIB_FEAT_EXPRESSIONS_SPIRIT_PARSER_ON and the according
 *    \ref alib_expressions_appendix_boostspirit "manual section" for more information on the use
 *    of the \c boost::spirit parser.
 * @}
 *
 * @addtogroup GrpALibCodeSelectorSymbols
 * @{
 *  \def  ALIB_FEAT_EXPRESSIONS_SPIRIT_PARSER
 *    Selects code within module \alibmod_expressions that switches the
 *    integration of an alternative parser for module \alibmod_expressions based on
 *    [boost::spirit](http://www.boost.org/doc/libs/1_66_0/libs/spirit/doc/html/index.html).
 *
 *    \attention
 *      This is an experimental feature only. Some features of this library will be silently (!)
 *      lost with its activation.
 *      It is discouraged to use the \c boost::spirit parser in combination with this library in
 *      production environments!
 *
 *    \attention
 *      Please read the according \ref alib_expressions_appendix_boostspirit "manual section"
 *      for more information on the use of the \c boost::spirit parser.
 *
 *    \see See also compiler symbol \ref ALIB_FEAT_EXPRESSIONS_SPIRIT_PARSER_ON.
 * @}
 */
#if defined(DOX_PARSER)
    #define     ALIB_FEAT_EXPRESSIONS_SPIRIT_PARSER 1
    #define     ALIB_FEAT_EXPRESSIONS_SPIRIT_PARSER_ON
    #define     ALIB_FEAT_EXPRESSIONS_SPIRIT_PARSER_OFF
#else
    #if defined(ALIB_FEAT_EXPRESSIONS_SPIRIT_PARSER)
        #error "ALIB_FEAT_EXPRESSIONS_SPIRIT_PARSER must not be set from outside"
    #endif

    #if defined(ALIB_FEAT_EXPRESSIONS_SPIRIT_PARSER_ON) && defined(ALIB_FEAT_EXPRESSIONS_SPIRIT_PARSER_OFF)
        #error "ALIB_FEAT_EXPRESSIONS_SPIRIT_PARSER_ON / ALIB_FEAT_EXPRESSIONS_SPIRIT_PARSER_OFF are both set"
    #endif

    #if defined(ALIB_FEAT_EXPRESSIONS_SPIRIT_PARSER_ON)
        #define ALIB_FEAT_EXPRESSIONS_SPIRIT_PARSER 1
    #else
        #define ALIB_FEAT_EXPRESSIONS_SPIRIT_PARSER 0
    #endif
#endif //DOX_PARSER


namespace aworx { namespace lib { namespace expressions {

struct Scope;


/** ************************************************************************************************
 * Exceptions thrown by module \alibmod_nolink_expressions.
 *
 * \note
 *   This enum \ref ALIB_LANG_EXCEPTIONS "is equipped with" meta information according to
 *   class \alib{lang,Exception}.
 **************************************************************************************************/
enum class Exceptions
{
    /// General error thrown by the parser.
    SyntaxError                             =1 ,

    /** Detail entry (negative element value) used with various exceptions. Shows the expression
     *  in a first line and a marker of the syntax error position in a second. */
    ExpressionInfo                          = -1,

    /**
     * This is added to exceptions when when a \c std::exception is wrapped.
     * The 'what()' string is included as entry parameter.
     * (This is a detail entry with negative element value).
     */
    StdExceptionInfo                        =-2,

    /// Thrown when an empty string is tried to be compiled.
    EmptyExpressionString                   =2,

    /// Syntax error with concrete information about what the parser expected at given position.
    SyntaxErrorExpectation                  =5,

    /// Unknown unary operator symbol found when parsing expression string.
    UnknownUnaryOperatorSymbol              =6,

    /// Unknown binary operator symbol found when parsing expression string.
    UnknownBinaryOperatorSymbol             =7,

    /// Compile-time exception thrown when an expression uses an unknown identifier name.
    UnknownIdentifier                       =14,

    /// Compile-time exception thrown when an expression uses an unknown function name.
    UnknownFunction                         =15,

    /** Optional info entry (negative element value) on a function with matching name but
     * non-matching arguments found. */
    FunctionHint                            =-15,

    /** Compile-time exception thrown when an unary operator is not applicable to the given
     * argument type. */
    UnaryOperatorNotDefined                 =16,

    /** Compile-time exception thrown when a binary operator is not applicable to the combination of
     * left-hand and right-and argument type.  */
    BinaryOperatorNotDefined                =17,

    /**
     * Compile-time exception thrown when function parentheses are missing with parameterless
     * functions, while flag
     * \alib{expressions::Compilation,AllowOmittingParenthesesOfParameterlessFunctions} is not set
     * in \alib{expressions,Compiler::CfgCompilation}.
     */
    MissingFunctionParentheses              =22,

    /**
     * Compile-time exception thrown when empty function parentheses are given with identifiers
     * (parameterless functions), while flag
     * \alib{expressions::Compilation,AllowEmptyParenthesesForIdentifierFunctions} is not set
     * in \alib{expressions,Compiler::CfgCompilation}.
     */
    IdentifierWithFunctionParentheses       =23,

    /**
     * Compile-time exception thrown when incompatible types are given with conditional
     * term <c>Q ? T : F</c>. Note that before throwing this exception, the compiler tries to
     * insert auto-cast callback functions, and if found, performs one second try to compile
     * the operator for the new pair of operatnd types.
     */
    IncompatibleTypesInConditional          =25,


    /**
     * This is a "wrapper" exception thrown when a plug-in or a compile-time invoked callback
     * function throws an exception of type \c std::exception or of type
     * \alib{lang,Exceptions} with a code that is not of this enumeration type.<br>
     * Used only if compilation \alib{expressions,Compilation::PluginExceptionFallThrough}
     * is not set.
     */
    ExceptionInPlugin                       =30,

    /**
     * This is a "wrapper" exception thrown when callback functions throws an exception of type
     * \c std::exception or of type \alib{lang,Exceptions}.
     * Used only if compilation \alib{expressions,Compilation::CallbackExceptionFallThrough}
     * is not set.
     */
    ExceptionInCallback                     =40,

    // ############################       Nested expressions      ##################################

    /**
     * Compile-time exception thrown when unary expression operator is used with a non-constant
     * nested expression name.  */
    NamedExpressionNotConstant              =50,

    /// Compile-time exception thrown when an expression refers to an unknown named nested expression.
    NamedExpressionNotFound                 =51,

    /**
     * Compile-time exception thrown when a named nested expression which is searched an inserted
     * at compile-time could not be found.<br>
     * Overwrites exception \b %NamedExpressionNotFound.
     *
     * \see Manual chapter \ref alib_expressions_nested "10. Nested Expressions".
     */
    NestedExpressionNotFoundCT              =52,

    /**
     * Compile-time exception thrown when when wrong arguments were passed to function
     * \alib{expressions,Compiler::CfgNestedExpressionFunction}.
     *
     * \see Manual chapter \ref alib_expressions_nested "10. Nested Expressions".
     */
    NestedExpressionCallArgumentMismatch     =53,

    /**
     * Evaluation-time exception thrown when a named nested expression which is searched only at
     * evaluation-time could not be found.<br>
     * Overwrites exception \b %NamedExpressionNotFound.
     * \see Manual chapter \ref alib_expressions_nested "10. Nested Expressions".
     */
    NestedExpressionNotFoundET              =54,

    /**
     * Informational exception entry (negative element value) providing the name of the expression
     * that caused any other exception than \c NamedExpressionNotFound during evaluation of a
     * nested expression.
     *
     * Note that entries of this type might be repeated when expressions are recursively nested.
     * The number of entries in the exception corresponds to the depth of nesting.
     */
    WhenEvaluatingNestedExpression          =-54,

    /**
     * Evaluation-time exception thrown when a named nested expression which was searched and found
     * only at evaluation-time, returned a different result type than specified.<br>
     *
     * \see Manual chapter \ref alib_expressions_nested "10. Nested Expressions".
     */
    NestedExpressionResultTypeError         = 55,

    /**
     * Evaluation-time exception thrown when a circular relationship of nested expressions
     * is detected, which comprises an infinite loop.
     */
    CircularNestedExpressions               = 56,

    /**
     * A list of informational entries of this type is given with exception
     * \b %CircularNestedExpressions. Each entry provides the name of the expression that
     * was evaluated and the name of the nested expression that it called. Hence, the list of
     * entries of this type comprise the "call stack" that leaded to the circular call that
     * caused the exception.
     */
    CircularNestedExpressionsInfo           =-56,
};

/** ************************************************************************************************
 * \alib{lang,T_EnumIsArithmetical,Arithmetical enumeration} of slots used to prioritize
 * \alib{expressions,CompilerPlugin} instances attached to instances of class
 * \alib{expressions,Compiler} at runtime.
 **************************************************************************************************/
enum class CompilePriorities
{
    /// Used to denote that no variable or plug-in was found.
    NONE                    = 0,

    /**
     * Built-in operator <c>?:</c> (like conditional, but no 'true'-expression).
     * \see \alib{expressions,plugins::ElvisOperator}
     */
    ElvisOperator           = 100,

    /**
     * Auto-cast plug-in.
     */
    AutoCast                = 200,

    /**
     * Built-in String comparison and manipulation.
     */
    Strings                 = 300,


    /**
     * Collection of built-in unary and binary operators for boolean, integer and floating point values.
     */
    Arithmetics             = 400,

    /**
     * Collection of built-in unary and binary operators for boolean, integer and floating point values.
     */
    Math                    = 500,

    /**
     * Collection of date and time functions based on \ref aworx::lib::time.
     */
    DateAndTime             = 600,

    /**
     * This is where custom plug-ins usually should be placed. It gives them a higher priority
     * than all built-in plug-ins have.<br>
     * If more than one plug-in should be installed, add 1, 2, ... to the enum element.
     */
    Custom                  = 1000,

};

/** ************************************************************************************************
 * This struct holds static \ref alib_expressions_prereq_sb "sample values" for
 * the built-in supported types of module \alibmod_nolink_expressions.
 *
 * These boxes are mainly used by built-in compiler plug-ins to denote the corresponding type.
 * Customized plug-ins are proposed to use these boxes to denote internal types as well and
 * besides that to define static sample boxes for each custom type that they introduce in a
 * similar fashion.
 *
 * \note
 *   Of-course, just sample any value might be passed where a type is expected, but using these
 *   pre-defined objects is more efficient in respect to code size. In respect to to execution
 *   performance, the sample values are rather irrelevant, as those are used almost exclusively
 *   at expression compile-time.
 **************************************************************************************************/
struct Types
{
    /// Sample <em>type-box</em> for C++ type <c>bool</c>.
    static ALIB_API Box Boolean;

    /** Sample <em>type-box</em> for integer types
     *  (Precisely for type \alib{boxing::ftypes,boxed_int}).*/
    static ALIB_API Box Integer;

    /// Sample <em>type-box</em> for C++ type <c>double</c>.
    static ALIB_API Box Float;

    /** Sample <em>type-box</em> for string types. While internally, \alib class
     *  \alib{strings,StringBase,String} is used, due to the magic of module \alibmod_boxing, almost any custom
     *  string type is compatible, including of-course \c std::string.*/
    static ALIB_API Box String;

    /// Sample <em>type-box</em> for date and time values of type \alib{time,DateTime}).
    static ALIB_API Box DateTime;

    /// Sample <em>type-box</em> for values of type \alib{time,TimePointBase::Duration,DateTime::Duration}).
    static ALIB_API Box Duration;
};


/** ************************************************************************************************
 * This enumeration lists the built-in unary operators.
 * The associated \alib{lang,T_EnumMetaDataDecl,enum meta data} provides the operator symbols.
 **************************************************************************************************/
enum class DefaultUnaryOperators
{
    NONE            ,  ///< Not an operator.

    Positive        ,  ///< <c>'+'</c> operator (usually returns identity value).
    Negative        ,  ///< <c>'-'</c> operator, negates a value.

    BoolNot         ,  ///< Boolean not (<c>'!'</c>).
    BitNot          ,  ///< Bitwise not (<c>'~'</c>).

    Indirection     ,  ///< Unary <c>'*'</c> operator. Similar to the C++ indirection operator, this
                       ///< is the default operator uses with nested expressions, which is defined
                       ///< with \alib{expressions,Compiler::CfgNestedExpressionOperator}.
};

/** ************************************************************************************************
 * This enumeration lists the built-in binary operators.
 * The associated \alib{lang,T_EnumMetaDataDecl,enum meta data} provides the operator symbols,
 * as well as the precedence value of the operator.
 **************************************************************************************************/
enum class DefaultBinaryOperators
{
    NONE            ,  ///< Not an operator.
    Subscript       ,  ///< Array subscripting (<c>'[]'</c>). Precedence hardcoded with parser.

    Multiply        ,  ///< Arithmetic multiplication (<c>'*'</c>). Precedence \c 900.
    Divide          ,  ///< Arithmetic division (<c>'/'</c>). Precedence \c 900.
    Modulo          ,  ///< Arithmetic modulo (<c>'%%'</c>). Precedence \c 900.

    Add             ,  ///< Arithmetic addition (<c>'+'</c>). Precedence \c 800.
    Subtract        ,  ///< Arithmetic subtraction (<c>'-'</c>). Precedence \c 800.

    ShiftLeft       ,  ///< Bitwise shifting of integer values (<c>'<<'</c>). Precedence \c 700.
    ShiftRight      ,  ///< Bitwise shifting of integer values (<c>'>>'</c>). Precedence \c 700.

    Smaller         ,  ///< Smaller operator (<c>'<'</c>). Precedence \c 600.
    SmallerOrEqual  ,  ///< Smaller or equal operator (<c>'<='</c>). Precedence \c 600.
    Greater         ,  ///< Greater operator (<c>'>'</c>). Precedence \c 600.
    GreaterOrEqual  ,  ///< Greater or equal operator (<c>'>='</c>). Precedence \c 600.

    Equal           ,  ///< Equal operator (<c>'=='</c>). Precedence \c 500.
    NotEqual        ,  ///< Not equal operator (<c>'!='</c>). Precedence \c 500.

    BitAnd          ,  ///< Binary and (<c>'&'</c>). "and"s all bits of two integer values. Precedence \c 470.
    BitXOr          ,  ///< Binary xor (<c>'^'</c>). "xor"s all bits of two integer values. Precedence \c 460.
    BitOr           ,  ///< Binary or  (<c>'|'</c>). "or"s all bits of two integer values. Precedence \c 450.
    BoolAnd         ,  ///< Boolean and (<c>'&&'</c>). Result is boolean. Precedence \c 440.
    BoolOr          ,  ///< Boolean or (<c>'||'</c>). Result is boolean. Precedence \c 430.

    Assign          ,  ///< Assignment. By default, this is used as alias operator for
                       ///< operator \b Equal by the built-in compiler plug-ins.
                       ///< See \alib{expressions::Compilation,AliasEqualsOperatorWithAssignOperator}
                       ///< for more information.<br>
                       ///< Precedence \c 300.


    Elvis            , ///< Binary version of ternary operator <c>Q ? T : F</c> with
                       ///< second operand (\c T) not given.
                       ///< Whitespaces are allowed between '?' and ':'.<br>
                       ///< Precedence \c 200.
};

/** ************************************************************************************************
 * This enumeration lists the built-in verbal alias names for unary operators.
 *
 * The associated \alib{lang,T_EnumMetaDataDecl,enum meta data} provides the operator verbs
 * as well as the replacement operator.
 *
 * Flag \alib{expressions,Compilation::DefaultAlphabeticOperatorAliases} controls if method
 * \alib{expressions,Compiler::SetupDefaults} adds the aliases to the compiler.
 **************************************************************************************************/
enum class DefaultAlphabeticUnaryOperatorAliases
{
    Not             , ///< Verbal alias \c "Not" to operator <c>'!'</c>.
};

/** ************************************************************************************************
 * This enumeration lists the built-in verbal alias names for binary operators.
 *
 * The associated \alib{lang,T_EnumMetaDataDecl,enum meta data} provides the operator verbs
 * as well as the replacement operator.
 *
 * Flag \alib{expressions,Compilation::DefaultAlphabeticOperatorAliases} controls if method
 * \alib{expressions,Compiler::SetupDefaults} adds the aliases to the compiler.
 **************************************************************************************************/
enum class DefaultAlphabeticBinaryOperatorAliases
{
    And             ,  ///< Verbal alias \c "And" to boolean and operator <c>'&&'</c>.
    Or              ,  ///< Verbal alias \c "Or" to boolean or operator <c>'||'</c>.
    Sm              ,  ///< Verbal alias \c "Sm" to operator <c>'<'</c>.
    Smaller         ,  ///< Verbal alias \c "Smaller" to operator <c>'<'</c>.
    SmEq            ,  ///< Verbal alias \c "Smeq" to operator <c>'<='</c>.
    SmallerOrEqual  ,  ///< Verbal alias \c "Smaller_or_equal" to operator <c>'<='</c>.
    Gt              ,  ///< Verbal alias \c "Gt" to operator <c>'>'</c>.
    Greater         ,  ///< Verbal alias \c "Greater" to operator <c>'>'</c>.
    GtEq            ,  ///< Verbal alias \c "Gteq" to operator <c>'>='</c>.
    GreaterOrEqual  ,  ///< Verbal alias \c "Greater_or_equal" to operator <c>'>='</c>.
    Eq              ,  ///< Verbal alias \c "Eq" to operator <c>'=='</c>.
    Equals          ,  ///< Verbal alias \c "Equals" to operator <c>'=='</c>.
    NEq             ,  ///< Verbal alias \c "Neq" to operator <c>'!='</c>.
    NotEqual        ,  ///< Verbal alias \c "Not_equals" to operator <c>'!='</c>.
};



/** ************************************************************************************************
 * \alib{lang,T_EnumIsBitwise,Bitwise} enum class defining options of expression compilation.
 * Flags defined with this type are set in field \alib{expressions,Compiler::CfgCompilation}.
 **************************************************************************************************/
enum class Compilation
{
    /**
     * If this flag is set, all unary operators given in \alib{expressions,DefaultUnaryOperators}
     * are defined with method \alib{expressions,Compiler::SetupDefaults}.
     *
     * Note that the enumeration class is equipped with
     * \alib{lang,T_EnumMetaDataDecl,resourced enum meta data}, which might be changed as an
     * alternative to clearing this flag and provide own definitions.
     *
     * This flag is set by default.
     */
    DefaultUnaryOperators                               = (1 <<  1),

    /**
     * If this flag is set, all binary operators given in \alib{expressions,DefaultBinaryOperators}
     * are defined with method \alib{expressions,Compiler::SetupDefaults}.
     *
     * Note that the enumeration class is equipped with
     * \alib{lang,T_EnumMetaDataDecl,resourced enum meta data}, which might be changed as an
     * alternative to clearing this flag and provide own definitions.
     *
     * This flag is set by default.
     */
    DefaultBinaryOperators                              = (1 <<  2),

    /**
     * If this flag is set, all alphabetic operator aliases given in enumerations
     * \alib{expressions,DefaultAlphabeticUnaryOperatorAliases} and
     * \alib{expressions,DefaultAlphabeticBinaryOperatorAliases}
     * are defined with method \alib{expressions,Compiler::SetupDefaults}.
     *
     * Note that both enumeration classes are equipped with
     * \alib{lang,T_EnumMetaDataDecl,resourced enum meta data}, which might be changed as an
     * alternative to clearing this flag and provide own definitions.
     *
     * This flag is set by default.
     */
    DefaultAlphabeticOperatorAliases                    = (1 <<  3),

    /**
     * If this flag is set, alphabetic operator aliases defined in
     * \alib{expressions,Compiler::AlphabeticUnaryOperatorAliases} and
     * \alib{expressions,Compiler::AlphabeticBinaryOperatorAliases} are parsed ignoring letter
     * case.
     *
     * \note
     *   Even when this flag is cleared, no two verbal operator aliases that are equal
     *   when case is ignored must be defined (e.g "or" and "OR" ).
     *
     * This flag is set by default.
     */
    AlphabeticOperatorsIgnoreCase                       = (1 <<  4),

    /**
     * If not set, array subscript operator <c>[]</c> is not supported and its use will cause
     * parse errors errors.
     *
     * Note that compiler plug-ins get the array subscript operator presented for compilation
     * as a usual binary operator given as \alib{expressions,DefaultBinaryOperators::Subscript}. This makes
     * its support with custom types quite simple.
     *
     * \attention This flag has to be changed to a custom state prior to performing the first
     *            compilation of an expression with a dedicated \alib{expressions,Compiler}.
     *            The rationale behind this is, that the compiler creates the parser system at its
     *            first use, which in turn does not check the flag after creation.
     *
     * This flag is set by default.
     */
    AllowSubscriptOperator                              = (1 <<  5),

    /**
     * Used with constructor of compiler plug-ins
     *  \alib{expressions,plugins::Arithmetics} and
     *  \alib{expressions,plugins::Strings}.
     * (If they are activated in \alib{expressions,Compiler::CfgBuiltInPlugins} or "manually"
     * added.)
     *
     * Denotes if the assignment operator <c>=</c> is an alias for equal operator <c>==</c>.
     * If set to \c false, the use of the <c>=</c> operator with several any argument type,
     * throws a compilation exception, unless it is compiled by a custom plug-in.
     *
     * \attention
     *   In addition to aliasing the <c>==</c> with <c>=</c>, the latter also receives a
     *   higher precedence of parsing! If this flag is not set, the precedence of
     *   \e assign <c>=</c>,follows the the C++ standards and thus is below the group of
     *   <c>&</c>, <c>^</c>, <c>|</c>,  <c>&&</c>,  <c>||</c> and <c>? :</c>. If the alias
     *   setting is activated, then the precedence is raised to be on the same level as
     *   boolean equal <c>==</c> and thus higher than the aforementioned operators!
     */
    AliasEqualsOperatorWithAssignOperator               = (1 <<  6),

    /**
     * Used with constructor of compiler plug-in
     * \alib{expressions,plugins::Arithmetics}.
     * (If it is activated in \alib{expressions,Compiler::CfgBuiltInPlugins} or "manually" added.)
     *
     * Denotes if the unary bitwise operator <c>~</c> and binary bitwise operators
     * <c>&</c>, <c>|</c> and <c>^</c> should be allowed for boolean values.
     * If set to \c false, the use of the bitwise operators with boolean arguments, throws
     * a compilation exception, unless the bitwise operators are compiled by a custom plug-in.
     */
    AllowBitwiseBooleanOperations                       = (1 <<  7),


    /**
     * If \c false, compile-time exception
     * \alib{expressions,Exception::MissingFunctionParentheses} is thrown if a parameterless
     * function is stated without (otherwise redundant) parentheses <c>'()'</c>.
     *
     * If \c true, this is tolerated.
     *
     * \note
     *   This flag is not tested, and the exception is not thrown, by the compiler itself, but
     *   by class \alib{expressions::plugins,Calculus}.<br>
     *   Even if this flag is \c false, still functions without parentheses may be allowed
     *   by setting \c nullptr to field
     *   \alib{expressions::plugins,Calculus::FunctionEntry::ArgTypes} when registering a
     *   function. In this case the function is considered rather being an "identifier"
     *   than a function. (Still it is the very same as a function, it is just a matter of
     *   wording here.)<br>
     *
     *   The other way round, if a function is registered as an identifier, flag
     *   \b %AllowEmptyParenthesesForIdentifierFunctions, controls if exception
     *   \alib{expressions,Exception::IdentifierWithFunctionParentheses} is to be thrown
     *   if empty parentheses are given on functions that are registered as
     *   identifier-style functions.
     */
    AllowOmittingParenthesesOfParameterlessFunctions    = (1 <<  8),

    /**
     * If \c false, compile-time exception
     * \alib{expressions,Exception::IdentifierWithFunctionParentheses} is thrown if a
     * parameterless function, declared as 'identifier style' is used with parentheses
     * <c>'()'</c>.
     *
     * If \c true, this is tolerated.
     *
     * \see
     *   For more information, see the note in documentation of flag
     *   \b %AllowOmittingParenthesesOfParameterlessFunctions.
     */
    AllowEmptyParenthesesForIdentifierFunctions         = (1 <<  9),

    /**
     * If this flag is set (the default), the
     * \alib{expressions,Compiler::CfgNestedExpressionOperator,unary nested expression operator}
     * is activated, as well as the single-parameter overload of the
     * \alib{expressions,Compiler::CfgNestedExpressionFunction,nested expression function}.
     * If the field is cleared, then only evaluation-time nested expressions are allowed,
     * which is the two- and three-parameter version of the expression function.
     * Other uses are then causing compilation exception
     * \alib{expressions,Exceptions::UnaryOperatorNotDefined}, respectively
     * \alib{expressions,Exceptions::NestedExpressionCallArgumentMismatch}.
     */
    AllowCompileTimeNestedExpressions                   = (1 << 10),

    /**
     * If this flag is set (the default), then identifiers that follow the unary nested
     * expression operator (defaults to <c>'*'</c>), are internally converted to corresponding
     * string literals - just as if they were given as a quoted string.
     *
     * \see
     *   Normalization flag
     *   \alib{expressions::Normalization,QuoteUnaryNestedExpressionOperatorArgument}.
     */
    AllowIdentifiersForNestedExpressions                = (1 << 11),

    /**
     * If not set (the default), then names of named expressions are not distinguished by
     * letter case.
     */
    CaseSensitiveNamedExpressions                       = (1 << 12),

    /**
     * Controls whether exceptions of type \c std::exception thrown in plug-ins during compilation
     * are caught by the compiler and transformed to \alib{expressions,Exceptions::ExceptionInPlugin}
     *
     * Note that exceptions thrown in callback functions which are evaluated at compile-time
     * against constant parameters, are considered plug-in exceptions.
     */
    PluginExceptionFallThrough                          = (1 << 13),


    /**
     * This is an "evaluation-time compiler flag". If not set (the default) exceptions of type
     * \alib{lang,Exception} and \c std::exception thrown in callback functions during expression
     * evaluation are caught and transformed to \alib{expressions,Exceptions::ExceptionInCallback}
     * letter case.
     */
    CallbackExceptionFallThrough                        = (1 << 14),

    /**
     * If this flag is set, no optimizations are performed when assembling the program.
     *
     * \note
     *   There is absolutely no reason for setting this flag, other than for running the
     *   unit tests. Or for playing with the little virtual machine implemented with this
     *   library and having fun understanding the non-optimized program listings generated.
     *   The assembly language is easy, it has only four virtual assembly commands - plus a fifth
     *   for invoking programs of nested expressions.
     */
    NoOptimization                                      = (1 << 20),


    #if ALIB_FEAT_EXPRESSIONS_SPIRIT_PARSER
        /**
         * If set, instead of the built-in hand-crafted parser, an alternative parser implementation
         * based on
         * [boost::spirit](http://www.boost.org/doc/libs/1_66_0/libs/spirit/doc/html/index.html)
         * is used.
         *
         * This flag is available only with compilations that specify preprocessor macro
         * \ref ALIB_FEAT_EXPRESSIONS_SPIRIT_PARSER. In this case the flag is on by default.
         *
         * \note The flag is tested only once with the first compilation of an expression by an
         *       expression compiler instance. Subsequent compilations with the same compiler
         *       object will re-use the parser created once.
         *
         */
        UseSpiritParser                                 = (1 << 21),
    #endif

    /**
     * Default value. Evaluates to:<br>
     * <c> DefaultUnaryOperators                            + </c><br>
     * <c> DefaultBinaryOperators                           + </c><br>
     * <c> DefaultAlphabeticOperatorAliases                 + </c><br>
     * <c> AlphabeticOperatorsIgnoreCase                    + </c><br>
     * <c> AliasEqualsOperatorWithAssignOperator            + </c><br>
     * <c> AllowBitwiseBooleanOperations                    + </c><br>
     * <c> AllowSubscriptOperator                           + </c><br>
     * <c> AllowOmittingParenthesesOfParameterlessFunctions + </c><br>
     * <c> AllowEmptyParenthesesForIdentifierFunctions      + </c><br>
     * <c> AllowCompileTimeNestedExpressions                + </c><br>
     * <c> AllowIdentifiersForNestedExpressions             + </c><br>
     */
    DEFAULT=    DefaultUnaryOperators
              + DefaultBinaryOperators
              + DefaultAlphabeticOperatorAliases
              + AlphabeticOperatorsIgnoreCase
              + AliasEqualsOperatorWithAssignOperator
              + AllowBitwiseBooleanOperations
              + AllowSubscriptOperator
              + AllowOmittingParenthesesOfParameterlessFunctions
              + AllowEmptyParenthesesForIdentifierFunctions
              + AllowCompileTimeNestedExpressions
              + AllowIdentifiersForNestedExpressions
    #if ALIB_FEAT_EXPRESSIONS_SPIRIT_PARSER
              + UseSpiritParser
    #endif
};

/** ************************************************************************************************
 * \alib{lang,T_EnumIsBitwise,Bitwise} enum class defining options for formatting parsed
 * expressions. The normalized conversion of the expression input string is available
 * with method \alib{expressions,Expression::GetNormalizedString}, after an expression was
 * successfully \alib{expressions,Compiler::Compiler,compiled}.
 *
 * The normalization flags are stored per compiler instance, consequently using a public compiler
 * field namely \alib{expressions,Compiler::CfgNormalization}.
 **************************************************************************************************/
enum class Normalization : uint64_t
{
    /**
     * Replace given shortened and letter case mismatched identifier and function names with
     * completed versions.<br>
     * This flag is set with \b %DEFAULT.
     */
    ReplaceFunctionNames                        = (1LLU <<  1),

    /**
     * Replace alias operators with effective operators.<br>
     * This flag is not set with \b %DEFAULT.
     */
    ReplaceAliasOperators                       = (1LLU <<  2),

    /**
     * This is one of four flags that together allow five possible normalization options for
     * \ref alib_expressions_operators_verbal "verbal alias operators".
     * If more than one flag is set, the one with the highest precedence is used.
     *
     * The following table lists the flags, their precedence and the type of normalization:
     *
     * |Precedence  | Flag                             | Description
     * |------------|----------------------------------|------------------------------
     * | 0 | \<none set\>                              | A verbal operator is normalized as given in original expression string.
     * | 1 | ReplaceVerbalOperatorsToSymbolic          | Replaces verbal operators with the symbolic operator that they represent.
     * | 2 | ReplaceVerbalOperatorsToLowerCase         | Converts verbal operators to lower case letters.
     * | 3 | ReplaceVerbalOperatorsToUpperCase         | Converts verbal operators to upper case letters.
     * | 4 | ReplaceVerbalOperatorsToDefinedLetterCase | Uses the writing specified with the definition of the verbal operator.
     *
     * With configuration \alib{expressions::Normalization,DEFAULT}, flag
     * \b %ReplaceVerbalOperatorsToUpperCase is set.
     */
    ReplaceVerbalOperatorsToSymbolic            = (1LLU <<  3),

    /**
     * See sibling flag \alib{expressions::Normalization,ReplaceVerbalOperatorsToSymbolic}.
     */
    ReplaceVerbalOperatorsToLowerCase           = (1LLU <<  4),

    /**
     * See sibling flag \alib{expressions::Normalization,ReplaceVerbalOperatorsToSymbolic}.
     */
    ReplaceVerbalOperatorsToUpperCase           = (1LLU <<  5),

    /**
     * See sibling flag \alib{expressions::Normalization,ReplaceVerbalOperatorsToSymbolic}.
     */
    ReplaceVerbalOperatorsToDefinedLetterCase   = (1LLU <<  6),

    /**
     * Converts nested expression names that have been given unquoted to a quoted string literal.
     * If this is not set, the quotation remains as given in original expression string.
     * This flag is \e not set with \b %DEFAULT.
     *
     * \see
     *   Compilation flag
     *   \alib{expressions::Compilation,AllowIdentifiersForNestedExpressions}.
     */
    QuoteUnaryNestedExpressionOperatorArgument  = (1LLU <<  7),

    /**
     * Remove redundant provisions unary operators <c>'+'</c> and <c>'-'</c> from number
     * literals.<br>
     * This flag is \e not set with \b %DEFAULT.
     */
    RemoveRedundantUnaryOpsOnNumberLiterals     = (1LLU <<  8),

    /**
     * Write a space after an unary operator (if no brackets around arguments and no unary operator
     * follows).<br>
     * This flag is \e not set with \b %DEFAULT.
     */
    UnaryOpSpace                                = (1LLU <<  9),

    /**
     * Write a space between two unary operators.<br>
     * This flag is \e not set with \b %DEFAULT.
     */
    UnaryOpSpaceIfUnaryFollows                  = (1LLU << 10),

    /**
     * Write a space before opening bracket of bracketed arguments of unary operators.<br>
     * This flag is \e not set with \b %DEFAULT.
     */
    UnaryOpSpaceIfBracketFollows                = (1LLU << 11),

    /**
     * Write a space after an opening and before a closing bracket of arguments of unary operators.<br>
     * This flag is \e not set with \b %DEFAULT.
     */
    UnaryOpInnerBracketSpace                    = (1LLU << 12),

    /**
     * If this flag is set, normalization inserts redundant brackets to to the argument of an
     * unary operation, if that argument is an unary operation itself.
     *
     * This flag is superseded by \b %RedundantUnaryOpBrackets.<br>
     * This flag is \e not set with \b %DEFAULT.
     */
    RedundantBracketsBetweenTwoUnaryOps         = (1LLU << 13),

    /**
     * If this flag is set, normalization inserts redundant brackets to arguments of unary
     * operators.
     *
     * This flag supersedes by \b %RedundantBracketsBetweenTwoUnaryOps.<br>
     * This flag is \e not set with \b %DEFAULT.
     */
    RedundantUnaryOpBrackets                    = (1LLU << 14),


    /**
     * Write a space before and after binary operator symbol.<br>
     * This flag is set with \b %DEFAULT.
     */
    BinaryOpSpaces                              = (1LLU << 15),

    /**
     * Write a space after an opening and before a closing bracket of arguments of binary
     * operators and around a conditional expression.<br>
     * This flag is \e not set with \b %DEFAULT.
     */
    InnerBracketSpace                           = (1LLU << 16),

    /**
     * Write a space before opening and after closing bracket of arguments of binary
     * operators and around a conditional expression.<br>
     * This flag is \e not set with \b %DEFAULT.
     */
    OuterBracketSpace                           = (1LLU << 17),

    /**
     * If this flag is set, normalization inserts redundant brackets to the right-hand side
     * operand of binary operators if that operand is a binary operator itself and has a higher
     * precedence.
     *
     * The following samples demonstrate why this is useful if normalization targets humans:
     *
     * Without flag set     |     With flag set
     * ---------------------|----------------------
     * 1 - 2 - 3                        |  <em>no change</em>
     * 1 - 2 * 3                        |  1 - (2 * 3)
     * 1 * 2 - 3                        |  <em>no change</em>
     * 1 - 2 * 3 - 4 - 5 * 6 - 7        |  1 - (2 * 3) - 4 - (5 * 6) - 7
     * true == false && true            |  <em>no change</em>
     * true && false == true            |  true && (false == true)
     * true && false == false && true   |  true && (false == false) && true
     * true && false == (false && true) |  true && (false == (false && true))
     * true && false == true < false    |  true && (false == (true < false))
     * true && false == false == true   |  true && (false == false == true)
     * This flag is superseded by \b %RedundantBinaryOpBrackets.<br>
     * This flag is set with \b %DEFAULT.
     */
    RedundantRhsBracketsIfRhsIsStrongerBinaryOp = (1LLU << 18),

    /**
     * If this flag is set, normalization inserts redundant brackets to both
     * operands of binary operators if both are binary operators themselves.
     *
     * The following samples demonstrate why this is useful if normalization strings target humans:
     *
     * Without flag set     |     With flag set
     * ---------------------|----------------------
     * 1 - 2 - 3            |  <em>no change</em>
     * 1 - 2 - 3 - 4        |  <em>no change</em>
     * 1 - 2 - (3 - 4)      |  (1 - 2) - (3 - 4)
     * 1 - 2 - 3 - (4 - 5)  |  (1 - 2 - 3) - (4 - 5)
     * 1 - 2 - (3 - 4) - 5  |  (1 - 2) - (3 - 4) - 5
     *
     * This flag is superseded by \b %RedundantBinaryOpBrackets.<br>
     * This flag is set with \b %DEFAULT.
     */
    RedundantBracketsIfLhsAndRhsAreBinaryOps    = (1LLU << 19),

    /**
     * If this flag is set, normalization inserts redundant brackets around each binary operator
     * sub-expression. This flag supersedes flags
     * \b %RedundantRhsBracketsIfRhsIsStrongerBinaryOp and
     * \b %RedundantBracketsIfLhsAndRhsAreBinaryOps.
     *
     * It is not recommended to set this flag. It is only useful to debug expressions and
     * understand exactly what precedences operators have.
     *
     * This flag is set with \b %DEFAULT.
     */
    RedundantBinaryOpBrackets                   = (1LLU << 20),


    /**
     * Write a space before character <c>?</c> of ternary conditional operator.<br>
     * This flag is set with \b %DEFAULT.
     */
    ConditionalOpSpaceBeforeQM                  = (1LLU << 21),

    /**
     * Write a space after character <c>?</c> of a ternary conditional operator.<br>
     * This flag is set with \b %DEFAULT.
     */
    ConditionalOpSpaceAfterQM                   = (1LLU << 22),

    /**
     * Write a space before character <c>:</c> of ternary conditional operator.<br>
     * This flag is set with \b %DEFAULT.
     */
    ConditionalOpSpaceBeforeColon               = (1LLU << 23),

    /**
     * Write a space after character <c>:</c> of ternary conditional operator.<br>
     * This flag is set with \b %DEFAULT.
     */
    ConditionalOpSpaceAfterColon                = (1LLU << 24),

    /**
     * If this flag is set, normalization inserts redundant brackets around the conditional
     * operator expressions <c>Q ? T : F</c>, if it is not the root node.
     *
     * This flag is set with \b %DEFAULT.
     */
    RedundantConditionalOpBrackets              = (1LLU << 25),

    /**
     * Write a space before between the function identifier name and the opening bracket of the
     * argument list.<br>
     * This flag is \e not set with \b %DEFAULT.
     */
    FunctionSpaceBeforeOpeningBracket           = (1LLU << 26),

    /**
     * Write a space after the opening and before the closing bracket of argument list of a
     * function.<br>
     * This flag is set with \b %DEFAULT.
     */
    FunctionInnerBracketSpace                   = (1LLU << 27),

    /**
     * Write a space between the opening and the closing bracket of an empty argument list of a
     * function.<br>
     * This flag is set with \b %DEFAULT.
     */
    FunctionInnerBracketSpaceIfNoArguments      = (1LLU << 28),

    /**
     * Write a space before a comma of an argument separator of an function's argument list.<br>
     * This flag is \e not set with \b %DEFAULT.
     */
    FunctionSpaceBeforeComma                    = (1LLU << 29),

    /**
     * Write a space after a comma of an argument separator of a function's argument list.<br>
     * This flag is set with \b %DEFAULT.
     */
    FunctionSpaceAfterComma                     = (1LLU << 30),

    /**
     * Write a space before array subscript operator <c>'[]'</c>.<br>
     * This flag is \e not set with \b %DEFAULT.
     */
    SubscriptSpaceBeforeBrackets                = (1LLU << 31),

    /**
     * Write a space after the opening and before the closing bracket of array subscript operator
     * <c>'[]'</c>.<br>
     * This flag is \e not set with \b %DEFAULT.
     */
    SubscriptInnerBracketSpace                  = (1LLU << 32),


    /**
     * If this flag is set, floating point literals are normalized in scientific format when
     * given in scientific format. If it is not set, then numbers given in scientific format
     * might be written as usual floating point values, dependent on their value. E.g.
     * a given <c>1.0e1</c> would be converted to <c>10.0</c>.
     *
     * Note that scientific format can be forced for all floating point number output by setting
     * flag \alib{strings,NumberFormatBase::ForceScientific,NumberFormat::ForceScientific} of member
     * member \alib{strings::format,FormatterBase::DefaultNumberFormat,Formatter::DefaultNumberFormat} which in turn is found
     * in member \alib{expressions,Compiler::CfgFormatter}. Such setting would supersede
     * this flag.
     *
     * This flag is \e not set with \b %DEFAULT.
     */
    KeepScientificFormat                        = (1LLU << 33 ),

    /**
     * If this flag is set, integral literals will be normalized to hexadecimal format.
     * If this flag is not set, then integrals will be normalized in the number system that they had
     * been provided in.
     *
     * This flags supersedes flags \b %ForceOctal and \b %ForceBinary.
     *
     * This flag is \e not set with \b %DEFAULT.
     */
    ForceHexadecimal                            = (1LLU << 34 ),

    /**
     * If this flag is set, integral literals will be normalized to octal format.
     * If this flag is not set, then integrals will be normalized in the number system that they had
     * been provided in.
     *
     * This flags is superseded by flag \b %ForceHexadecimal and supersedes flag \b %ForceBinary.
     *
     * This flag is \e not set with \b %DEFAULT.
     */
    ForceOctal                                  = (1LLU << 35 ),

    /**
     * If this flag is set, integral literals will be normalized to binary format.
     * If this flag is not set, then integrals will be normalized in the number system that they had
     * been provided in.
     *
     * This flags is superseded by flags \b %ForceHexadecimal and \b %ForceOctal.
     *
     * This flag is \e not set with \b %DEFAULT.
     */
    ForceBinary                                 = (1LLU << 36 ),

    /**
     * All flags are cleared, may be used for testing bits.
     */
    NONE                                       = 0L,

    /**
     * All flags are cleared, hence no whitespaces and unnecessary brackets are written and
     * identifiers as they have been given (potentially abbreviated and ignoring letter case).
     */
    COMPACT                                     = 0L,

    /**
     * Default value. Evaluates to:<br>
     * <c>ReplaceFunctionNames                        + </c><br>
     * <c>ReplaceVerbalOperatorsToUpperCase           + </c><br>
     * <br>
     * <c>RemoveRedundantUnaryOpsOnNumberLiterals     + </c><br>
     * <c>BinaryOpSpaces                              + </c><br>
     * <c>RedundantRhsBracketsIfRhsIsStrongerBinaryOp + </c><br>
     * <c>RedundantBracketsIfLhsAndRhsAreBinaryOps    + </c><br>
     * <br>
     * <c>ConditionalOpSpaceBeforeQM                  + </c><br>
     * <c>ConditionalOpSpaceBeforeColon               + </c><br>
     * <c>ConditionalOpSpaceAfterQM                   + </c><br>
     * <c>ConditionalOpSpaceAfterColon                + </c><br>
     * <c>RedundantConditionalOpBrackets              + </c><br>
     * <br>
     * <c>FunctionInnerBracketSpace                   + </c><br>
     * <c>FunctionSpaceAfterComma                     + </c><br>
     */
    DEFAULT=    ReplaceFunctionNames
              + ReplaceVerbalOperatorsToUpperCase

              + RemoveRedundantUnaryOpsOnNumberLiterals
              + BinaryOpSpaces
              + RedundantRhsBracketsIfRhsIsStrongerBinaryOp
              + RedundantBracketsIfLhsAndRhsAreBinaryOps

              + ConditionalOpSpaceBeforeQM
              + ConditionalOpSpaceBeforeColon
              + ConditionalOpSpaceAfterQM
              + ConditionalOpSpaceAfterColon
              + RedundantConditionalOpBrackets

              + FunctionInnerBracketSpace
              + FunctionSpaceAfterComma
};

/**
 * Type definition for passing boxes as sample types.
 *
 * \see For more information, see
 *      \ref alib_expressions_prereq_sb "3.2 Type definitions With Sample Boxes".
 */
using Type= const aworx::lib::boxing::Box&;

/**
 * Type definition for passing boxed constant expression values.
 *
 * \see For more information, see
 *      \ref alib_expressions_prereq_sb "3.2 Type definitions With Sample Boxes".
 */
using ArgIterator= std::vector<Box>::iterator;

/**
 * Function pointer implementing native callback functions, for expression functions and
 * operators. The implementations are are defined and selected by the compiler plugins.
 * They are called (executed) when an expression is evaluated.
 *
 * @param      scope     The expression's evaluation scope.
 * @param[out] argsBegin An iterator that returns \b %Box objects.
 *                       The first entry takes also the result.
 * @param[out] argsEnd   The end-iterator.
 *
 */
using  CallbackDecl = Box (*)( Scope& scope, ArgIterator argsBegin, ArgIterator argsEnd );


/**
 * This \ref alib_namespace_boxing_interfaces "ALib boxing interface" is used to
 * create parsable expression "literals" from constant values of custom type stored in boxes.
 * The interface is used by the library in the following circumstances:
 * - If custom identifiers, functions or operator callback functions return a custom type.
 * - If such types can be constants and are announced to the compiler as such.
 * - If method \alib{expressions,Expression::GetOptimizedString} is called.
 * - And, not necessarily: if such string is to be used as input to compiling expressions
 *
 * The challenge here is to convert constants of custom types back into a normalized, human readable
 * but also compilable expression string.
 * As the expression syntax only defines the built-in literal types
 * \alib{expressions::Types,Integer}, \alib{expressions::Types,Float} and
 * \alib{expressions::Types,String}, the constants have to be created using either appropriate
 * custom identifiers or "constructor functions" that have to be provided in addition along with the
 * as soon as the conditions above are met. These identifiers and constructor functions in turn
 * need to be compile-time evaluatable to ensure that recompiling the optimized string results
 * to constants.
 *
 * So far, this probably sounded a little complicated, but a step-by step sample taken from
 * built-in date/time functionality demonstrates the use quite well:
 *
 * 1. %Compiler plug-in \alib{expressions::plugins,DateAndTime} introduces \alib class
 *    \alib{time,TimePointBase::Duration} to expressions. This is for example done by defining identifiers as follows:
 *
 *    \snippet "plugins/dateandtime.cpp"  DOX_ALIB_EXPR_IToLiteral_1
 *
 * The "constructor functions" are declared to be \alib{expressions::plugins,Calculus::CTI,compile-time invokable}
 * and return are a constant at compile-time if their input parameter is constant.
 * When the program - that may due to optimization not contain the identifiers any more - becomes
 * de-compiled, these constants have to be written to the normalized expression string in a way that
 * corresponding constant values of type \b %Duration are expressed.
 *
 * To perform exactly this task, this boxing interface class is used.
 * To enable the mechanism, static method \alib{expressions::plugins,DateAndTime::Init} is invoked
 * at bootstrap by the library. This announces a corresponding implementation named
 * \b %IToLiteral_Duration:
 *
 * \snippet "expressions/plugins/dateandtime.cpp"  DOX_ALIB_EXPR_IToLiteral_2
 *
 * This interface is used on to decompile constants of custom types found in a program.
 * The implementation is done in an anonymous namespace of the compilation unit of compiler plug-in
 * \b %DateAndTime.  It looks as follows:
 *
 * \snippet "expressions/plugins/dateandtime.cpp"  DOX_ALIB_EXPR_IToLiteral_3
 *
 * As it can be understood from the code, the interface implementation tries to find the best
 * matching "constructor function" for time span given, writes its name and the constant parameter
 * enclosed by brackets \c "()".
 *
 * Only with such interface implementation in place - one that covers all possible constants - this
 * library is able to create <b>parsable, normalized, optimized expression strings</b>.
 * To finalize this example, see what the result for three sample expressions look like:
 *
 *  \snippet "DOX_ALIB_EXPRESSIONS_TUT_WONO-100.txt"     OUTPUT
 *  \snippet "DOX_ALIB_EXPRESSIONS_TUT_WONO-101.txt"     OUTPUT
 *  \snippet "DOX_ALIB_EXPRESSIONS_TUT_WONO-102.txt"     OUTPUT
 *
 * \note
 *   When integrating module \alibmod_nolink_expressions into a software, a decision has to be taken:
 *   "Should optimized expression strings be presented to the end-user?"<br>
 *
 * \note
 *   While it is obvious, that sample two demonstrates that to be useful, sample three probably
 *   demonstrates the opposite!<br>
 *   It is important to understand, that the decision depends on the custom use case, where for
 *   example the technical understanding of a typical end-user may be taken into account.
 *
 * \note
 *   If the decision is taken, \b not to present optimized expression strings to the end-user, this
 *   has absolutely no influence on the evaluation performance: All three expressions,
 *   - the original input,
 *   - the normalized output and
 *   - the optimized, normalized output,
 * \note
 *   are leading to the very same (optimized) internal program when compiled!
 *
 * \note
 *   Of-course, if no optimization strings are presented to the end-user and hence are not
 *   recompiled (or copy/pasted by such users), then the implementation of this boxing interface is
 *   not needed for your custom type, as it is only invoked with method
 *   \alib{expressions,Expression::GetOptimizedString}!
 */
struct IToLiteral : public lib::boxing::Interface
{
    /// Constructor providing our runtime type to the parent.
    IToLiteral() : Interface( typeid(IToLiteral) ) {}

    /**
     * The interface method taking the box of type \b %TickSpan and the expression string to write
     * the "literal" value into.
     *
     * @param constantValue    The constant program value that is about to be written into
     *                         \p{expressionString}.
     * @param expressionString The expression string that is currently generated.
     */
    ALIB_API
    virtual void Invoke( const Box& constantValue, AString& expressionString )                                      = 0;
};


/** ************************************************************************************************
 * The library class for module \alibmod_nolink_expressions.
 *
 * If compilation symbol \ref ALIB_MODULE_EXPRESSIONS is set, library \ref aworx::lib::ALIB will
 * create a dependency to this library's singleton \ref aworx::lib::EXPRESSIONS.
 **************************************************************************************************/
class Expressions  : public lib::lang::Library
{
    public:
        /**
         * Constructor
         */
        ALIB_API  Expressions();

    protected:
        /** ****************************************************************************************
         * Initializes this library and namespace.
         *
         * @param phase  The initialization phase to perform.
         ******************************************************************************************/
        virtual void        init( Phases phase )                                           override;

        /** ****************************************************************************************
         * Frees resources of the this library and namespace.
         ******************************************************************************************/
        virtual void        terminationCleanUp()                                           override;

}; // class Expressions

/** ************************************************************************************************
 * Type definition used with namespace function #MatchFunctionName.
 * Arrays of this type can be defined in resources and retrieved with function
 * #LoadResourcedFunctionDescriptors.
 **************************************************************************************************/
struct FunctionNameDescriptor
{
    String          Name;           ///< The identifier/function name.
    lang::Case      Sensitivity;    ///< The letter case sensitivity.
    int             MinLength;      ///< The minimum abbreviation length.
};

/** ****************************************************************************************
 * Loads function name descriptors from resources. This method is static and receives the
 * library as a parameter. This way, it might be used by custom compiler plug-ins in the
 * same fashion as it is used by the built-in ones.
 *
 * Parses from the resourced string entries of the form
 * - 1. The identifier/function name.
 * - 2. A comma \c ','.
 * - 3. Character \c 'I' or \c 'S' to denotes case sensitivity.
 * - 4. An integer denoting the minimum length of the function name (abbreviation).
 *
 * (No comma between the entries).
 *
 * \attention The length of the given table has to fit to number of entries found in
 *            the resources.
 *
 * @param library       The resource library to load the resource from.
 * @param resourceName  The resource string to parse.
 * @param target        The table to fill.
 ******************************************************************************************/
ALIB_API
void LoadResourcedFunctionDescriptors( Library& library, const String& resourceName,
                                       FunctionNameDescriptor *target );

/** ************************************************************************************************
 * Utility class used to match optionally abbreviated and case insensitive function names.
 * @param haystack    The correct function name, minimum length and letter case sensitivity.
 * @param needle      The potentially abbreviated name to match.
 * @return \c true if \p{needle} matches \p{haystack}, \c false otherwise.
 **************************************************************************************************/
bool MatchFunctionName( const FunctionNameDescriptor& haystack,  const String& needle );

} // namespace aworx::lib[::expressions]

/** ************************************************************************************************
 * The static library singleton of #aworx::lib::expressions library class.
 **************************************************************************************************/
ALIB_API
extern expressions::Expressions EXPRESSIONS;


}} // namespace [aworx::lib]

ALIB_LANG_ENUM_IS_ARITHMETICAL( aworx::lib::expressions::CompilePriorities )

ALIB_LANG_ENUM_NAMED          ( aworx::lib::expressions::DefaultUnaryOperators  )
ALIB_LANG_ENUM_META_DATA      ( aworx::lib::expressions::DefaultBinaryOperators, String, int )

ALIB_LANG_ENUM_META_DATA      ( aworx::lib::expressions::DefaultAlphabeticUnaryOperatorAliases , String, String )
ALIB_LANG_ENUM_META_DATA      ( aworx::lib::expressions::DefaultAlphabeticBinaryOperatorAliases, String, String )

ALIB_LANG_EXCEPTIONS          ( aworx::lib::expressions::Exceptions, lib::EXPRESSIONS, ASTR("Exceptions") )
ALIB_LANG_ENUM_IS_BITWISE     ( aworx::lib::expressions::Compilation   )
ALIB_LANG_ENUM_IS_BITWISE     ( aworx::lib::expressions::Normalization )

#endif // HPP_ALIB_EXPRESSIONS_EXPRESSIONSLIB
