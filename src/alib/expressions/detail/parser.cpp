// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !DOXYGEN
#   include "alib/expressions/detail/parser.hpp"
#   include "alib/expressions/compiler.hpp"
#   include "alib/expressions/detail/parser_impl.hpp"
#endif // !DOXYGEN


namespace alib {  namespace expressions { namespace detail {

// static creation method
Parser* Parser::Create( Compiler& compiler )
{
    return compiler.allocator().New<detail::ParserImpl>(compiler, compiler.allocator );
}


}}} // namespace [alib::expressions::detail]
