/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_expressions of the \aliblong.
 *
 * \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_EXPRESSIONS_PLUGINS_STRINGS
#define HPP_ALIB_EXPRESSIONS_PLUGINS_STRINGS

#ifndef HPP_ALIB_EXPRESSIONS_PLUGINS_CALCULUS
#   include "alib/expressions/plugins/calculus.hpp"
#endif


namespace alib {  namespace expressions { namespace plugins {


/** ************************************************************************************************
 * This built-in \alib{expressions,CompilerPlugin} of \alib_expressions_nl
 * compiles identifiers, functions and operators with character string type operands or return
 * types.
 *
 * By default, this plug-in is \alib{expressions,Compiler::CfgBuiltInPlugins,automatically created}
 * and inserted into each instance of class \alib{expressions,Compiler} with the invocation of
 * \alib{expressions::Compiler,SetupDefaults}.
 *
 * <b>Constants:</b><br>
 *
 * Type    | Name        | Min. Abbreviation | Description
 * --------|-------------|-------------------|--------------
 * String  |\b NewLine   | nl    | System dependent new line character combination ( <c>"\n"</c> or <c>"\r\n"</c>).
 * String  |\b Tabulator | tab   | String with single tabulator character (<c>"\t"</c>).
 *
 * <br>
 * <b>Functions:</b><br>
 *
 * Note: All identifier and function names are defined case insensitive.
 *
 * Return Type    | Name      | Min. Abbreviation| Signature | Description
 * ---------------|-----------|------------------|-----------|-------------
 * String  | \b String        | str      | ...                     | Creates and returns a string representation of the arguments, concatenated from left to right.
 * String  | \b ToUpper       | tu       | String                  | Converts all appropriate characters of a string to upper case letters. This function is aliased by unary operator <c>'+'</c>.
 * String  | \b ToLower       | tl       | String                  | Converts all appropriate characters of a string to lower case letters. This function is aliased by unary operator <c>'-'</c>.
 * Boolean | \b Compare       | comp     | String, String          | Returns \c 0 if strings are equal, a negative value if the first string is "smaller" than the second and a positive otherwise.
 * Boolean | \b Compare       | comp     | String, String, Boolean | Same as Compare(String,String) if third parameter is \c false. If it is \c true letter case is ignored.
 * Boolean | \b StartsWith    | sw       | String, String          | Tests if the first given string starts with the second. Comparison is case sensitive.
 * Boolean | \b StartsWith    | sw       | String, String, Boolean | Tests if the first given string starts with the second. The third parameter is \c true, letter case is ignored.
 * Boolean | \b EndsWith      | ew       | String, String          | Tests if the first given string ends with the second. Comparison is case sensitive.
 * Boolean | \b EndsWith      | ew       | String, String, Boolean | Tests if the first given string ends with the second. The third parameter is \c true, letter case is ignored.
 * String  | \b Substring     | subs     | String, Integer         | Returns the sub-string, starting at given position (2nd argument) to the end of the string..
 * String  | \b Substring     | subs     | String, Integer, Integer| Returns the sub-string, starting at given position (2nd argument) with the given length (3rd argument).
 * Integer | \b IndexOf       | indo     | String, String          | Searches the first occurrence of the second string in the first. Returns the position if found, otherwise \c -1.
 * Integer | \b Count         | count    | String, String          | Counts the number of occurrences of the second string in the first.
 * String  | \b Replace       | repl     | String, String, String  | Returns a copy of the 1st argument, with occurrences of 2nd argument replaced by 3rd argument.
 * String  | \b Repeat        | repeat   | String, Integer         | Returns 1st argument concatenated \e N times (2nd argument).
 * String  | \b Token         | tok      | String, String, Integer | Parses tokens separated by character (2nd argument) in string (1st argument) an returns the n-th (3rd argument).
 * String  | \b Trim          | trim     | String                  | Trims the string at both sides by removing the following whitespace characters: <c>' '</c>, <c>'\\n'</c>, <c>'\\r'</c> and <c>'\\t'</c>.
 * String  | \b Trim          | trim     | String, String          | Trims the string at both sides by removing the whitespace characters listed in the second string.
 * String  | \b TrimStart     | trims    | String                  | Trims the string at the start by removing the following whitespace characters: <c>' '</c>, <c>'\\n'</c>, <c>'\\r'</c> and <c>'\\t'</c>.
 * String  | \b TrimStart     | trims    | String, String          | Trims the string at the start by removing the whitespace characters listed in the second string.
 * String  | \b TrimEnd       | trime    | String                  | Trims the string at the end by removing the following whitespace characters: <c>' '</c>, <c>'\\n'</c>, <c>'\\r'</c> and <c>'\\t'</c>.
 * String  | \b TrimEnd       | trime    | String, String          | Trims the string at the end by removing the whitespace characters listed in the second string.
 * Integer | \b Integer       | int      | String                  | Parses an integral value from the string. Allows decimal, hexadecimal, octal and binary formats. Literal prefixes default to <c>'0x'</c>, <c>'0o'</c> and <c>'0b'</c>, which can be changed by configuring object \alib{lang::format::Formatter,DefaultNumberFormat} of field \alib{expressions,Scope::Formatter}.
 * Float   | \b Float         | float    | String                  | Parses a floating point value from the string. Allows scientific format and change of decimal point character by configuring object \alib{lang::format::Formatter,DefaultNumberFormat} of field \alib{expressions,Scope::Formatter}.
 * String  | \b Hexadecimal   | hex      | Integer                 | Converts an integral value to a hexadecimal string representation. Format options are available with object \alib{lang::format::Formatter,DefaultNumberFormat} of field \alib{expressions,Scope::Formatter}. The literal prefix (e.g. \c "0x") is not written.
 * String  | \b Hexadecimal   | hex      | Integer, Integer        | Converts an integral value to a hexadecimal string representation of the given output width (2nd argument). Format options are available with object \alib{lang::format::Formatter,DefaultNumberFormat} of field \alib{expressions,Scope::Formatter}. The literal prefix (e.g. \c "0x") is not written.
 * String  | \b Octal         | oct      | Integer                 | Converts an integral value to a binary string representation. Format options are available with object \alib{lang::format::Formatter,DefaultNumberFormat} of field \alib{expressions,Scope::Formatter}. The literal prefix (e.g. \c "0x") is not written.
 * String  | \b Octal         | oct      | Integer, Integer        | Converts an integral value to a binary string representation of the given output width (2nd argument). Format options are available with object \alib{lang::format::Formatter,DefaultNumberFormat} of field \alib{expressions,Scope::Formatter}. The literal prefix (e.g. \c "0x") is not written.
 * String  | \b Binary        | bin      | Integer                 | Converts an integral value to an octal string representation. Format options are available with object \alib{lang::format::Formatter,DefaultNumberFormat} of field \alib{expressions,Scope::Formatter}. The literal prefix (e.g. \c "0x") is not written.
 * String  | \b Binary        | bin      | Integer, Integer        | Converts an integral value to an octal string representation of the given output width (2nd argument). Format options are available with object \alib{lang::format::Formatter,DefaultNumberFormat} of field \alib{expressions,Scope::Formatter}. The literal prefix (e.g. \c "0x") is not written.
 * String  | \b Format        | format   | String, ...             | Formats the given variadic parameters according to the given format string. See notes below, for further information.
 * Boolean | \b WildCardMatch | wcm      | String, String          | Matches a string against a wildcard pattern. See notes below, for further information.
 * Boolean | \b RegExMatch    | rem      | String, String          | Matches a string against a regex pattern. See notes below, for further information.
 *
 * <br>
 * <b>Unary Operators:</b><br>
 *
 * | Return Type | Operator| Argument Type | Description|
 * |-------------|---------|---------------|---------------------
 * | String      |<b>+</b> | String        | Alias to function \b %ToUpper.
 * | String      |<b>-</b> | String        | Alias to function \b %ToLower.
 * | Boolean     |<b>!</b> | String        | Tests a string for emptiness. See notes below.
 *
 * <br>
 * <b>Binary Operators:</b><br>
 *
 * | Return Type | Lhs Type | Operator   | Rhs Type | Description
 * |-------------|----------|------------|----------|--------------------
 * |String       | String   |<b>+</b>  | Integer  | Concatenates an integral value to a string. The number conversion is performed using object \alib{lang::format::Formatter,DefaultNumberFormat} of field \alib{expressions,Scope::Formatter}.
 * |String       | String   |<b>+</b>  | Float    | Concatenates a floating point value to a string. The number conversion is performed using object \alib{lang::format::Formatter,DefaultNumberFormat} of field \alib{expressions,Scope::Formatter}.
 * |String       | String   |<b>+</b>  | \<any\>  | Converts a boxed value of arbitrary type to a string and appends the result to another string. The conversion of the boxed value is performed by invoking box-function \alib{boxing,FAppend}.
 * |String       | Integer  |<b>+</b>  | String   | Converts an integral value to a string and concatenates another string. The number conversion is performed using object \alib{lang::format::Formatter,DefaultNumberFormat} of field \alib{expressions,Scope::Formatter}.
 * |String       | Float    |<b>+</b>  | String   | Converts a floating point value to a string and concatenates another string. The number conversion is performed using object \alib{lang::format::Formatter,DefaultNumberFormat} of field \alib{expressions,Scope::Formatter}.
 * |String       | \<any\>  |<b>+</b>  | String   | Converts a boxed value of arbitrary type to a string and appends another string. The conversion of the boxed value is performed by invoking box-function \alib{boxing,FAppend}.
 * |String       | String   |<b>+</b>  | String   | Concatenates two strings.
 * |Boolean      | String   |<b><</b>  | String   | Compares two strings using method \alib{strings,TString::CompareTo,String::CompareTo}.
 * |Boolean      | String   |<b><=</b> | String   | Compares two strings using method \alib{strings,TString::CompareTo,String::CompareTo}.
 * |Boolean      | String   |<b>></b>  | String   | Compares two strings using method \alib{strings,TString::CompareTo,String::CompareTo}.
 * |Boolean      | String   |<b>>=</b> | String   | Compares two strings using method \alib{strings,TString::CompareTo,String::CompareTo}.
 * |Boolean      | String   |<b>==</b> | String   | Compares two strings using method \alib{strings,TString::Equals,String::Equals}.
 * |Boolean      | String   |<b>!=</b> | String   | Compares two strings using method \alib{strings,TString::Equals,String::Equals}.
 * |Boolean      | String   |<b>*</b>  | String   | Alias to expression function \b %WildCardMatch. See notes below, for further information.
 * |Boolean      | String   |<b>\%</b> | String   | Alias to expression function \b %RegexMatch. See notes below, for further information.
 * |Boolean      | String   |<b>[]</b> | Integer  | Returns the sub-string of length \c 1 at index \e rhs in string \e lhs.
 *
 *
 \I{################################################################################################}
 * # Notes And Hints #
 *
 * ### Compile-Time Invokable: ###
 * All callback functions are defined compile-time invokable. This means that redundancies in
 * string expressions emerging from operations on constant strings are optimized (pruned) by the
 * compiler.
 *
 * ### Underlying %String %Types: ###
 * While all string manipulation is based on \alib classes \alib{strings,TString,String},
 * \alib{strings,TAString,AString} and \alib{strings,TSubstring,Substring}, due to the
 * "harmonizing" way that string classes become boxed, none of these types appears as an expression
 * result type. This means that custom expression functions can unbox strings that have been created
 * by callback functions of this plug-in to their own custom string type (e.g. \c std::string,
 * \c QString, or whatever), and, the other way round, results of custom callback functions that
 * returned custom strings objects, can seamlessly be used by functions and operators defined here.
 *
 * Details of this are explained in chapters
 * \ref alib_boxing_bijective "3. Non-Bijective Type Relationships" and
 * \ref alib_boxing_strings   "10. Boxing Character Strings" of the
 * \ref alib_mod_boxing "Programmer's Manual" of module \alib_boxing_nl.
 *
 *
 * ### Determine A String's Length ###
 *
 * The length of the string can be determined with function \b %Length, which is defined with
 * compiler plugin \alib{expressions::plugins,Arithmetics} (because it works on all arrays).
 *
 * ### Test For Empty %Strings: ###
 *
 * Unary operator \alib{expressions::DefaultUnaryOperators,BoolNot} (<c>'!'</c>)  may be used to check if a
 * string is not empty. The operator returns \c true  if the string is empty and \c false otherwise.<br>
 *
 * Consequently, to test for non-empty strings, <c>'!!'</c> may be written. The outer
 * <em>not-</em>operator is then a <em>not-</em>operator on the boolean result of the inner.<br>
 * Alternatively, binary operators (see below) against an emtpy string can be used:
 *
 *         myIndentifier == ""
 *         myIndentifier != ""
 *
 * As a result, this expressions is a tautology:
 *
 *         !myIdentifier == (myIndentifier == "")
 *
 * Finally with strings, the built-in implementation of the
 * \alib{expressions,plugins::ElvisOperator,elvis operator} is quite useful to avoid empty
 * strings. The expression:
 *
 *         myIdentifier ?: "Default"
 *
 * returns "Default" if \e myIdentifier evaluates to an empty string.
 *
 *
 * ### Case conversion: ###
 * Unary operators \alib{expressions::DefaultUnaryOperators,Positive} <c>'+'</c> and
 * \alib{expressions::DefaultUnaryOperators,Negative} <c>'-'</c> are alias to functions \b %ToUpper and
 * \b %ToLower.
 * Hence, the expressions
 *
 *          ToUpper( "Hello " ) + ToLower( "World" )
 *          +"Hello " + -"World"
 * are equivalent.
 *
 *
 * ### Concatenation: ###
 * Binary operator <c>'+'</c> is compiled by this plug-in if one of the arguments (or both) is of
 * string type. If one argument is not of string type, it becomes converted. For the conversion,
 * the following rules apply:
 *
 * - Types \alib{expressions::Types,Integer} and \alib{expressions::Types,Float} are converted
 *   to string types using object \alib{lang::format::Formatter,DefaultNumberFormat} of
 *   field \alib{expressions,Scope::Formatter}.
 *   With this, detailed options for the number output format is available.
 *   Even more options are available when using expression function \b %Format instead of
 *   the <c>'+'</c> operator.
 *
 * <p>
 * - Any other type is converted to a string representation by using box-function
 *   \alib{boxing,FAppend}. For information on how to implement this for custom
 *   types, consult the documentation of module \alib_boxing.
 *   The following provides a quick sample snippet, taken from the unit tests:
 *
 *   1. The custom type:
 *      \snippet "ut_alib_expr.cpp"    DOX_ALIB_EXPRESSIONS_STRINGOPS_IAPPEND_1
 *
 *   2. Definition of functor \alib{strings,T_Append} type <b>MyType</b>.
 *      Because the type will be boxed as a pointer, we also define the functor for the pointer type:
 *      \snippet "ut_alib_expr.cpp"    DOX_ALIB_EXPRESSIONS_STRINGOPS_IAPPEND_2
 *
 *   3. In the bootstrap section of an application, an implementation of the templated interface
 *      function has be registered with \alib_boxing_nl:
 *      \snippet "ut_alib_expr.cpp"    DOX_ALIB_EXPRESSIONS_STRINGOPS_IAPPEND_3
 *
 *   With this short setup code, objects of type \e MyType, which might be returned by custom
 *   identifiers, can be added to strings in expressions!
 *
 * <p>
 * - With type \alib{expressions::Types,Boolean}, the \alib{lang::resources,ResourcePool,resourced}
 *   string values generally used with box-function \alib{boxing,FAppend} defined for
 *   C++ type \c bool are used.
 *   These string resources default to \b "true" and \b "false". While resources can be customized,
 *   such customization has process-wide effect.<br>
 *   To express other values, \ref alib_expressions_builtin_ternary "conditional operator"
 *   (<c>Q ? T : F</c>) can be used by the end-user:
 *
 *       "Is true: " + {boolean expression}                     // Uses the global resourced strings
 *       "Is true: " + ( {boolean expression} ? "Yes" : "No" )  // Custom conversion per expression.
 *
 * ### Comparison: ###
 * Operators <c>'=='</c>, <c>'!='</c>, <c>'<'</c>, <c>'<='</c>, <c>'>'</c> and <c>'>='</c>
 * perform comparisons between two string operands.
 * Internally, to perform the comparison, methods \alib{strings,TString::Equals,String::Equals} and
 * \alib{strings,TString::CompareTo,String::CompareTo} are used.
 *
 * Case insensitive comparison can be performed by converting the operand(s) to upper case
 * using expression function \b %ToUpper or its "alias operator", unary <c>'+'</c>.
 * A more effective way is to use overloaded function \b %Compare that accepts an optional
 * third operator of type \b %Boolean, which if \c true is given performs a case insensitive
 * search without performing string conversions.
 *
 * \anchor alib_expressions_Strings_Formatting
 * ### Formatting: ###
 * Function <b>Format(String, ...)</b> offers full featured string formatting in expressions.
 * Being based on classes of underlying module \alib_basecamp, there is a choice between
 * \alib{lang::format,FormatterPythonStyle,python style} or
 * \alib{lang::format,FormatterJavaStyle,Java/printf style} format strings. It is even possible
 * to allow both formats, of-course not within the same string, but within different
 * evaluations of the same expression!
 *
 * Please consult the documentation of classes
 * - \alib{lang::format,Formatter},
 * - \alib{lang::format,FormatterPythonStyle,FormatterPythonStyle} and
 * - \alib{lang::format,FormatterJavaStyle,FormatterJavaStyle} for details.
 *
 *
 * ### Wildcard Match: ###
 * Wildcard match is implemented with expression function \b %WildcardMatch, respectively its "alias
 * operator" <c>'*'</c>.
 * The first (left-hand side) argument is the
 * string that is searched, the second (right-hand side) argument is the pattern string to be
 * matched.
 *
 * Wildcards characters are  (<c>'*'</c>) and  (<c>'?'</c>). For example, expressions
 *
 *          WildcardMatch("This is ALib Expressions", "*A?ib*")
 *          "This is ALib Expressions" * "*A?ib*"
 *
 * are equivalent and return boolean \c true.
 *
 * For case insensitive search, the both strings are to be converted to upper case, as in
 * expression:
 *
 *          +filename  *  "*.JPG"
 *
 * \note
 *   A more performant alternative for the latest sample expression is:
 *
 *          EndsWith( filename, ".jpg", true )
 *
 * ### Regular %Expression Match: ###
 * Regular expression match is implemented with expression function \b %RegexMatch, respectively its
 * "alias operator" <c>'\%'</c>.
 * The first (left-hand side) argument is the
 * string that is searched, the second (right-hand side) argument is the pattern string to be matched.
 *
 * The regular expression syntax is compatible with
 * \https{Perl Regular Expressions,perldoc.perl.org/perlretut.html}.
 * The feature is implemented with using \https{boost::regex library,www.boost.org}.
 *
 * \attention
 *   The regular expression features of \alib and therefore of this compiler plug-in, are only
 *   available with the definition of compiler symbol \ref ALIB_FEAT_BOOST_REGEX
 *   and if either \ref ALIB_CHARACTERS_WIDE is \c false or \ref ALIB_CHARACTERS_NATIVE_WCHAR
 *   equals \c true.
 *
 \I{################################################################################################}
 * # Reference Documentation #
 **************************************************************************************************/
struct Strings   : public plugins::Calculus
{
    /** ********************************************************************************************
     * Constructor. Creates the hash map.
     * @param compiler The compiler we will get attached to.
     **********************************************************************************************/
    ALIB_API    Strings( Compiler& compiler );

    /** ********************************************************************************************
     * Virtual destructor
     **********************************************************************************************/
    virtual    ~Strings()                                                                   override
    {}

    /** ********************************************************************************************
     * Overwrites the implementation of class \b %Calculus. While this is usually not needed,
     * this class uses this for wildcard and regular expression matching functions.
     * These functions implement a special behavior in respect to allocate "matcher" classes
     * at compile-time in the case that the pattern string is constant (which usually is).
     * Parent helper class \alib{expressions::plugins,Calculus} does provide mechanics for such
     * rather complicated special actions.
     *
     * Of-course, the original method is called alternatively.
     *
     * @param[in,out]  ciFunction  The compilation info struct.
     * @return \c true if compilation information was given, \c false otherwise.
     **********************************************************************************************/
    ALIB_API
    virtual bool    TryCompilation( CIFunction&   ciFunction)                              override;

    /** ********************************************************************************************
     * Overwrites the implementation of class \b %Calculus. While this is usually not needed,
     * this class uses this to fetch arbitrary boxed types for certain operations, because
     * the interface mechanics of module \alib_boxing allows us to convert any custom type
     * to a string.
     *
     * Of-course, the original method is called alternatively.
     *
     * @param[in,out]  ciBinaryOp  The compilation info struct.
     * @return \c true if compilation information was given, \c false otherwise.
     **********************************************************************************************/
    ALIB_API
    virtual bool    TryCompilation( CIBinaryOp&   ciBinaryOp )                             override;

};

/** ************************************************************************************************
 * This is the callback method for string function <b>%String(...)</b>, which converts an arbitrary
 * amount of arguments of arbitrary type to a concatenated string.
 *
 * The function is compile-time invokable.
 *
 * \note
 *   As an exclamation to the rule, this function is not defined in an anonymous namespace, but
 *   exposed through the C++ header file of this struct. The rationale for this
 *   is that the function is also used for auto-casting custom types to strings, which is performed
 *   with compiler plug-in \alib{expressions,plugins::AutoCast}.
 *
 * @param scope The scope.
 * @param args  The single argument.
 * @return A boxed \alib{monomem::TMAString}.
 **************************************************************************************************/
ALIB_API
Box CBToString( Scope& scope, ArgIterator  args, ArgIterator );

/** ************************************************************************************************
 * This is the callback method for string function <b>%Format(...)</b>, which formats an arbitrary
 * amount of arguments according to a given format string.
 *
 * The function is compile-time invokable.
 *
 * \note
 *   As an exclamation to the rule, this function is not defined in an anonymous namespace, but
 *   exposed through the C++ header file of this struct. The rationale for this
 *   is that the function this way can be called by other plug-ins as well.
 *
 * @param scope The scope.
 * @param args  The single argument.
 * @return A boxed \alib{monomem::TMAString}.
 **************************************************************************************************/
ALIB_API
Box CBFormat( Scope& scope, ArgIterator  args, ArgIterator );

}}} // namespace [alib::expressions::plugins]

#endif // HPP_ALIB_EXPRESSIONS_PLUGINS_STRINGS
