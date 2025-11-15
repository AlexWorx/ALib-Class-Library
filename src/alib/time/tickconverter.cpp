//##################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
#include "alib_precompile.hpp"
#if !defined(ALIB_C20_MODULES) || ((ALIB_C20_MODULES != 0) && (ALIB_C20_MODULES != 1))
#   error "Symbol ALIB_C20_MODULES has to be given to the compiler as either 0 or 1"
#endif
#if ALIB_C20_MODULES
    module;
#endif
//========================================= Global Fragment ========================================
#include "alib/alib.inl"
//============================================== Module ============================================
#if ALIB_C20_MODULES
    module ALib.Time;
#else
#   include "ALib.Time.H"
#endif
//========================================== Implementation ========================================
using namespace std::chrono;
namespace alib::time {


#   include "ALib.Lang.CIMethods.H"

//##################################################################################################
// TickConverter
//##################################################################################################
void TickConverter::SyncClocks( int qtyRepeats ) {
    Ticks::TTimePoint       steadyClock;
    DateTime::TTimePoint    systemClock;
    uint64_t lastDiff= 0;
    for( int i= 0 ; i < qtyRepeats ; ++i ) {
        systemClock= system_clock::now();
        steadyClock= steady_clock::now();

        auto systemCount= systemClock.time_since_epoch().count();
        auto steadyCount= steadyClock.time_since_epoch().count();

        // This cannot be optimized, because:
        // a) we have to use an unsigned integer, and
        // b) we have to take into account which clock was measured first and which last. If
        //    interrupted between the calls, the difference either shrinks or increases.
        if( systemCount < steadyCount ) {
            uint64_t diff= uint64_t( steadyCount - systemCount );
            if( lastDiff == 0 || diff < lastDiff ) {
                steadyClockSyncTime= steadyClock;
                systemClockSyncTime= systemClock;
                lastDiff= diff;
            }
        } else {
            uint64_t diff= uint64_t( systemCount - steadyCount  );
            if( lastDiff == 0 || diff > lastDiff ) {
                steadyClockSyncTime= steadyClock;
                systemClockSyncTime= systemClock;
                lastDiff= diff;
}   }   }   }

} // namespace [alib::time]
