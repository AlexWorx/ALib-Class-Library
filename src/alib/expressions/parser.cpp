// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib_precompile.hpp"
#if !defined(ALIB_C20_MODULES) || ((ALIB_C20_MODULES != 0) && (ALIB_C20_MODULES != 1))
#   error "Symbol ALIB_C20_MODULES has to be given to the compiler as either 0 or 1"
#endif
#if ALIB_C20_MODULES
    module;
#endif
// ======================================   Global Fragment   ======================================
#include "alib/expressions/expressions.prepro.hpp"

// ===========================================   Module   ==========================================
#if ALIB_C20_MODULES
    module ALib.Expressions;
    import   ALib.Expressions.Impl;
#else
#   include "ALib.Expressions.H"
#   include "ALib.Expressions.Impl.H"
#endif
// ======================================   Implementation   =======================================
namespace alib {  namespace expressions { namespace detail {

// static creation method
Parser* Parser::Create( Compiler& compiler )
{
    return compiler.allocator().New<detail::ParserImpl>(compiler, compiler.allocator );
}


}}} // namespace [alib::expressions::detail]
