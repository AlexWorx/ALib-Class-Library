/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_expressions of the \aliblong.
 *
 * \emoji :copyright: 2013-2019 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_EXPRESSIONS_DETAIL_PARSER
#define HPP_ALIB_EXPRESSIONS_DETAIL_PARSER

#if !defined (HPP_ALIB_STRINGS_NUMBERFORMAT)
#   include "alib/strings/numberformat.hpp"
#endif

ALIB_ASSERT_MODULE(EXPRESSIONS)


namespace aworx { namespace lib {

namespace monomem { class MonoAllocator; }

namespace expressions {

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
     *   of \c boost header includes files to the code entities using module \alib_expressions_nl.
     *
     * @param expressionString The string to parse.
     * @param numberFormat     Used to parse literal numbers.
     * @param allocator        Used for temporary objects, including AST nodes, converted string
     *                         data, etc.
     * @return  The abstract syntax tree representing the expression.
     */
    virtual detail::AST* Parse( const String&           expressionString,
                                NumberFormat*           numberFormat,
                                monomem::MonoAllocator* allocator                     ) = 0;


    /**
     * Static method to create a parser object.
     *
     * @param compiler The compiler that needs a parser.
     * @return The parser.
     */
    ALIB_API static
    Parser*     Create( Compiler& compiler );
};

}}}} // namespace [aworx::lib::expressions::detail]


#endif // HPP_ALIB_EXPRESSIONS_DETAIL_PARSER
