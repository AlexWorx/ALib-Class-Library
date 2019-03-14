// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#ifndef HPP_ALIB_EXPRESSIONS_DETAIL_PARSER
#define HPP_ALIB_EXPRESSIONS_DETAIL_PARSER

#if !defined (HPP_ALIB_STRINGS_NUMBERFORMAT)
#   include "alib/strings/numberformat.hpp"
#endif

ALIB_ASSERT_MODULE(EXPRESSIONS)



namespace aworx { namespace lib { namespace expressions {

class Compiler;

namespace detail {

struct AST;


/** ************************************************************************************************
 * This detail class constitutes an abstract base class for expression parsers.
 **************************************************************************************************/
struct Parser
{
    /**
     * Virtual destructor.
     */
    virtual    ~Parser()
    {}

    /**
     * Parses the given expression string.
     * \note
     *   The return value is hidden by using <c>void*</c>. This is to allow avoid flooding
     *   of \c boost header includes files to the code entities using module \alibmod_nolink_expressions.
     *
     * @param expressionString The string to parse.
     * @param numberFormat     Used to parse literal numbers.
     * @return  The abstract syntax tree representing the expression.
     */
    virtual detail::AST* Parse( const String&   expressionString,
                                NumberFormat*   numberFormat                      ) = 0;


    /**
     * Static method to create a parser object.
     *
     * \note
     *   This library provides two implementations of parsers. See the according
     *   \ref alib_expressions_appendix_boostspirit "manual section" for more information.
     *
     * @param compiler The compiler that needs a parser.
     * @return The parser.
     */
    ALIB_API static
    Parser*     Create( Compiler& compiler );
};

}}}}; // namespace [aworx::lib::expressions::detail]


#endif // HPP_ALIB_EXPRESSIONS_DETAIL_PARSER
