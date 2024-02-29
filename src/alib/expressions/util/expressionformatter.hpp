/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_expressions of the \aliblong.
 *
 * \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_EXPRESSIONS_UTIL_EXPRESSION_FORMATTER
#define HPP_ALIB_EXPRESSIONS_UTIL_EXPRESSION_FORMATTER

#if !defined (HPP_ALIB_CAMP_MESSAGE_EXCEPTION)
#   include "alib/lang/message/exception.hpp"
#endif

ALIB_ASSERT_MODULE(EXPRESSIONS)


#if !defined (HPP_ALIB_LANG_FORMAT_FORMATTER_STD)
#   include "alib/lang/format/formatterstdimpl.hpp"
#endif

#if !defined (HPP_ALIB_EXPRESSIONS_EXPRESSION)
#   include "alib/expressions/expression.hpp"
#endif


namespace alib { namespace expressions::util {

/** ************************************************************************************************
 * This class allows to use \alib_expressions_nl within format strings used with
 * \alib{lang::format,Formatter,ALib Formatters} and thus allows to expose customizable format strings to
 * end-users, which leverage an application's expression subsystem.
 *
 * To understand the concept of this class, please read the documentation of class
 * \alib{lang::format,PropertyFormatter} \b first, which is almost a 1:1 copy of this class but
 * uses simple "property callback functions" instead of expressions.
 *
 * \note
 *    This documentation rather documents the differences to \b %PropertyFormatter instead of
 *    repeating what is explained already in the sibling class.
 *
 * This class can be used in the same way as \alib{lang::format,PropertyFormatter}. However,
 * instead of the provision of a table of type
 * \alib{lang::format,PropertyFormatter::TCallbackTable}, this class expects an expression
 * compiler in the constructor.
 *
 * With that, simple expressions comprised of just a single identifier term can be used in
 * exactly the same way as demonstrated with \b PropertyFormatter. For example, format string:
 *
 *      "{name} is aged {age} and his/her hobby is {hobby}"
 *
 * would compile three expressions (<em>"name"</em>, <em>"age"</em> and <em>"hobby"</em>) and
 * convert the format string to just
 *
 *      "{} is aged {} and his/her hobby is {}"
 *
 * Then, with the invocation of method #Format, an expression scope has to be provided, which
 * is used to evaluate each expression and add the result value to the list of format parameters.
 *
 * To separate the given expressions strings from other formatting information provided in a
 * placeholder, a special separator character is used. This character is provided with construction
 * and defaults to symbol \c '@'. A format string that uses
 * \alib{lang::format,FormatterPythonStyle,python formatting syntax} might look like this:
 *
 *      "The surface is {width * height@:>5.2} sqm."
 *
 * <p>
\I{################################################################################################}
 * # Reference Documentation #
 * @throws alib::lang::format::FMTExceptions::UnknownPropertyInFormatString
 **************************************************************************************************/
class ExpressionFormatter
{
    protected:
        /** The expression compiler (as given in the constructor). */
        Compiler*                   compiler;

        /** The formatter to use (as given in the constructor). */
        SPFormatter                 stdFormatter;

        /** The original format string. Used only for exception information. */
        const AString               originalFormatString;

        /** The resulting format string passed to \alib formatters. */
        AString                     formatStringStripped;

        /** The expressions functions to receive the format data. */
        std::vector<SPExpression>   expressions;

    public:

        /** ****************************************************************************************
         * Constructor. Processes the given format string and compiles embedded expressions, which
         * are then used with invocations of method #Format.
         *
         * @param formatString        The format string as described in the class documentation.
         * @param compiler            The expression compiler.
         * @param formatter           The formatter to use. Defaults to \c nullptr which selects
         *                            \alib{lang::format,Formatter::GetDefault,default formatter}.
         * @param separatorChar       The character that is to be used to separate the expression
         *                            from the placeholder format information in
         *                            \p{customFormatString}.<br>
         *                            Defaults to <c>'@'</c>.
         *
         * @throws alib::lang::format::FMTExceptions::UnknownPropertyInFormatString.
         ******************************************************************************************/
        ALIB_API
        ExpressionFormatter( const String   formatString,
                             Compiler*      compiler,
                             SPFormatter    formatter            = nullptr,
                             character      separatorChar        = '@'          );

        /** ****************************************************************************************
         * Writes the formatted output of the expressions in accordance to the given \p{scope}
         * object to given \p{target} string.
         *
         * @param target    The target string to write into.
         * @param scope     The scope used to evaluate the expressions found in the format string.
         ******************************************************************************************/
        ALIB_API
        void    Format( AString& target, expressions::Scope&  scope );


}; // class ExpressionFormatter

} // namespace alib[::expressions::util]

/// Type alias in namespace \b alib.
using ExpressionFormatter  =    expressions::util::ExpressionFormatter;

} // namespace [alib]


#endif // HPP_ALIB_EXPRESSIONS_UTIL_EXPRESSION_FORMATTER
