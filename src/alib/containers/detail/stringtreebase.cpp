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
#include "alib/containers/containers.prepro.hpp"
// ===========================================   Module   ==========================================
#if ALIB_C20_MODULES
    module ALib.Containers.StringTree;
    import   ALib.Lang;
#else
#   include "ALib.Lang.H"
#   include "ALib.Containers.StringTree.H"
#endif
// ======================================   Implementation   =======================================
namespace alib::containers {

#if ALIB_DEBUG
    uinteger  DBG_STATS_STRINGTREE_NAMES         = 0;
    uinteger  DBG_STATS_STRINGTREE_NAME_OVERFLOWS= 0;
#endif



} // namespace [alib::containers]

