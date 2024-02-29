// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)
#   if !defined (HPP_ALIB_EXPRESSIONS_DETAIL_PARSER)
#      include "alib/expressions/detail/parser.hpp"
#   endif
#   if !defined (HPP_ALIB_EXPRESSIONS_COMPILER)
#      include "alib/expressions/compiler.hpp"
#   endif
#   if !defined (HPP_ALIB_EXPRESSIONS_DETAIL_PARSER_IMPL)
#      include "alib/expressions/detail/parser_impl.hpp"
#   endif
#endif // !defined(ALIB_DOX)


namespace alib {  namespace expressions { namespace detail {

// static creation method
Parser* Parser::Create( Compiler& compiler )
{
    return compiler.allocator.Emplace<detail::ParserImpl>( compiler, &compiler.allocator );
}


}}} // namespace [alib::expressions::detail]
