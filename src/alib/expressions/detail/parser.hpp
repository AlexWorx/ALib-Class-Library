//==================================================================================================
/// \file
/// This header file is part of module \alib_expressions of the \aliblong.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_EXPRESSIONS_DETAIL_PARSER
#define HPP_ALIB_EXPRESSIONS_DETAIL_PARSER
#pragma once
#include "alib/strings/numberformat.hpp"

ALIB_ASSERT_MODULE(EXPRESSIONS)

namespace alib          {
namespace monomem       {  template<typename TAllocator>class TMonoAllocator; }
namespace expressions   { class Compiler;
namespace detail        { struct AST;

//==================================================================================================
/// This detail class constitutes an abstract base class for expression parsers.
//==================================================================================================
struct Parser
{
    /// Virtual destructor.
    virtual    ~Parser()                                                                          {}

    /// Parses the given expression string.
    /// \note
    ///   The return value is hidden by using <c>void*</c>. This is to allow avoid flooding
    ///   of \c boost header includes files to the code entities using module \alib_expressions_nl.
    ///
    /// @param expressionString The string to parse.
    /// @param numberFormat     Used to parse literal numbers.
    /// @return  The abstract syntax tree representing the expression.
    virtual detail::AST* Parse( const String&            expressionString,
                                NumberFormat*            numberFormat                )          = 0;


    /// Static method to create a parser object.
    ///
    /// @param compiler The compiler that needs a parser.
    /// @return The parser.
    ALIB_API static
    Parser*     Create( Compiler& compiler );
};

}}} // namespace [alib::expressions::detail]


#endif // HPP_ALIB_EXPRESSIONS_DETAIL_PARSER

