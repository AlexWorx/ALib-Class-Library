// #################################################################################################
//  alib::lox::loggers - ALox Logging Library
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
#include "alib/alox/alox.prepro.hpp"
#include <iostream>
// ===========================================   Module   ==========================================
#if ALIB_C20_MODULES
    module ALib.ALox.Impl;
    import   ALib.Lang;
    import   ALib.Strings;
#else
#   include "ALib.Lang.H"
#   include "ALib.Strings.H"
#   include "ALib.ALox.H"
#   include "ALib.ALox.Impl.H"
#endif
// ======================================   Implementation   =======================================
using namespace alib;

bool ConsoleLogger::notifyLogOp( lang::Phase phase )
{
    if ( phase == lang::Phase::End )
        std::cout << std::endl;
    return true;
}

alib::integer ConsoleLogger::logSubstring( const String& buffer, integer start, integer length )
{
    return writer.WriteAndGetWideLength( buffer.Substring<NC>( start, length ) );
}

