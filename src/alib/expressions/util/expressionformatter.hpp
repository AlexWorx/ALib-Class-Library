/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_expressions of the \aliblong.
 *
 * \emoji :copyright: 2013-2019 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_EXPRESSIONS_UTIL_EXPRESSION_FORMATTER
#define HPP_ALIB_EXPRESSIONS_UTIL_EXPRESSION_FORMATTER

#if !defined (HPP_ALIB_RESULTS_EXCEPTION)
#   include "alib/results/exception.hpp"
#endif

ALIB_ASSERT_MODULE(EXPRESSIONS)


#if !defined (HPP_ALIB_TEXT_FORMATTER_STD)
#   include "alib/text/formatterstdimpl.hpp"
#endif

#if !defined (HPP_ALIB_EXPRESSIONS_EXPRESSION)
#   include "alib/expressions/expression.hpp"
#endif


namespace aworx { namespace lib { namespace expressions { namespace util {


/** ************************************************************************************************
 * This class allows to use \alib_expressions_nl within format strings used with class
 * \alib{text,Formatter,ALib Formatters}.
 *
 * To understand the concept of this class, please read the documentation of class
 * \alib{text,PropertyFormatter} \b first, which is almost a 1:1 copy of this class but
 * uses simple "property callback functions" instead of expressions.
 *
 * \note
 *    This documentation rather documents the differences to \b %PropertyFormatter instead of
 *    repeating what is explained already in the sibling class.
 *
 * This class can be used in the same way as \alib{text,PropertyFormatter}. However,
 * instead of the provision of a table of type
 * \alib{text,PropertyFormatter::TCallbackTable}, this class expects an expression
 * compiler in the constructor.
 *
 * With that, simple expressions comprised of just a single identifier term can be used in
 * exactly the same way as demonstrated with \b PropertyFormatter. For example, format string:
 *
 *      "{@name} is aged {@age} and his/her hobby is {@hobby}"
 *
 * would compile three expressions (<em>"name"</em>, <em>"age"</em> and <em>"hobby"</em>) and
 * convert the format string to just
 *
 *      "{} is aged {} and his/her hobby is {}"
 *
 * Then, with the invocation of method #Format, an expression scope has to be provided, which
 * is used to evaluate each expression and add the result value to the list of format parameters.
 *
 * Now, by enclosing the expression with an additional pair of brackets, complex expression strings
 * become allowed, for example:
 *
 *      "The surface is: {@{width * height}}"
 *
 * \note The outer curly brackets denote the placeholder of the format string when formatter
 *       \alib{text,FormatterPythonStyle,FormatterPythonStyle} is used.<br>
 *       The ’@’-character together with the inner curly brackets denote the expression.
 *       As shown above, if the expression string contains only one single word containing only
 *       alphabetical characters, the inner curly braces can be omitted.
 *
 * This way, it is very easy to expose customizable format strings to end-users, which leverage
 * an application's expression subsystem.
 *
 * <p>
\~Comment ####################################################################################### \~
 * # Reference Documentation #
 * @throws aworx::lib::text::Exceptions::UnknownPropertyInFormatString
 **************************************************************************************************/
class ExpressionFormatter
{
    protected:
        /** The expression compiler (as given in the constructor). */
        Compiler*                   compiler;

        /** The formatter to use (as given in the constructor). */
        SPFormatter                 stdFormatter;

        /** The original format string. Used only for exception information. */
        AString                     originalFormatString;

        /** The resulting format string passed to \alib formatters. */
        AString                     formatString;

        /** The expressions functions to receive the format data. */
        std::vector<SPExpression>   expressions;

    public:

        /** ****************************************************************************************
         * Constructor. Processes the given format string and compiles embedded expressions, which
         * are then used with invocations of method #Format.
         *
         * @param customFormatString  The format string as described in the class documentation.
         * @param compiler            The expression compiler.
         * @param formatter           The formatter to use. Defaults to \c nullptr which selects
         *                            \alib{text,Formatter::GetDefault,default formatter}.
         * @param ESCCharacter        The prefix used to search expressions in \p{customFormatString}.
         *                            Defaults to <c>'@'</c>.
         * @param ESCOpeningBracket   The opening bracket to enclose expressions with.
         *                            This has to be following to \p{ESCCharacter} in the format
         *                            string.
         *                            Defaults to <c>'{'</c>.
         * @param ESCClosingBracket   The closing bracket to enclose expressions with.
         *                            Defaults to <c>'}'</c>.
         *
         * @throws aworx::lib::text::Exceptions::UnknownPropertyInFormatString.
         ******************************************************************************************/
        ALIB_API
        ExpressionFormatter( const String   customFormatString,
                             Compiler*      compiler,
                             SPFormatter    formatter            = nullptr,
                             character      ESCCharacter         = '@',
                             character      ESCOpeningBracket    = '{',
                             character      ESCClosingBracket    = '}'           );

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

}}} // namespace aworx[::lib::expressions::util]

/// Type alias in namespace #aworx.
using ExpressionFormatter  =    lib::expressions::util::ExpressionFormatter;

}  // namespace [aworx]


#endif // HPP_ALIB_EXPRESSIONS_UTIL_EXPRESSION_FORMATTER
