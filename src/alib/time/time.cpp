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
#include "alib/alib.inl"
// ===========================================   Module   ==========================================
#if ALIB_C20_MODULES
    module ALib.Time;
    #if ALIB_BOXING
      import    ALib.Boxing;
    #endif

#else
#   include "ALib.Time.H"
#   include "ALib.Boxing.H"
#endif
// ======================================   Implementation   =======================================
using namespace std::chrono;

namespace alib {

/// This namespace provides types for calendrical date and time processing as well as
/// for non-calendrical steady and monotonic time measurement.
///
/// This namespace and the types provided, are always included in any \alibbuild.
///
/// Besides this reference documentation, further information is provided with
/// \ref alib_mod_time "Programmer's Manual" of \alib_time_nl.
namespace time {

// #################################################################################################
// Module Bootstrap/Termination
// #################################################################################################
#if !DOXYGEN
namespace { Ticks   creationTime; }
#endif

#   include "ALib.Lang.CIFunctions.H"

Ticks&  CreationTime()                                                     { return creationTime; }

}} // namespace [alib::time]

