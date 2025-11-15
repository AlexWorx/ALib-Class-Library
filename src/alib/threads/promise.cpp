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
#if !ALIB_SINGLE_THREADED
#   include <condition_variable>
#   include <atomic>
#endif // !ALIB_SINGLE_THREADED
#include <unordered_map>
#include <future>
//============================================== Module ============================================
#if ALIB_C20_MODULES
    module   ALib.Threads;
    import   ALib.Lang;
#  if ALIB_STRINGS
    import   ALib.Strings;
#  endif
#else
#   include "ALib.Lang.H"
#   include "ALib.Threads.H"
#   include "ALib.Strings.H"
#endif
//========================================== Implementation ========================================

#if !ALIB_SINGLE_THREADED && ALIB_DEBUG && ALIB_STRINGS && !DOXYGEN
namespace alib::threads {

void  Promise::Fulfill(const CallerInfo& ci, State state) {
    ALIB_ASSERT_ERROR( DbgFulfillCI.File == nullptr, "THREADS",
                       "Promise was already fulfilled. Repeated calls not allowed.\n"
                       "  This call:    {}\n"
                       "  Earlier call: {}", ci, DbgFulfillCI )
    DbgFulfillCI= ci;

    promise.set_value(state);
}

Promise::State Promise::Wait(const CallerInfo& ci) {
    ALIB_ASSERT_ERROR( DbgWaitCI.File == nullptr, "THREADS",
                       "Promise was already awaited. Repeated calls not allowed.\n"
                       "  Received with: ", DbgWaitCI )

    if ( !DbgWaitTimeLimit.IsZero() ) {
        Ticks::Duration waitDuration=  DbgWaitTimeLimit;
        Ticks overallTimer;
        Ticks waitTimer;
        while ( future.wait_for( (waitDuration - waitTimer.Age()).Export() )
                != std::future_status::ready )
        {
            if ( waitTimer.Age() < waitDuration )
                continue; // spurious wakeup

            ALIB_WARNING( "THREADS", "Waiting for a Promise since {}.", overallTimer.Age() )
            waitTimer.Reset();
    }   }

    DbgWaitCI= ci;
    return future.get();
}
Promise::State Promise::WaitFor( const Ticks::Duration::TDuration& maxWaitTimeSpan,
                                 const CallerInfo&                 ci               ) {
    ALIB_ASSERT_ERROR( DbgWaitCI.File == nullptr, "THREADS",
                       "Promise was already awaited. Repeated calls not allowed.\n"
                       "  Received with: ", DbgWaitCI )

    if ( future.wait_for(maxWaitTimeSpan) == std::future_status::timeout )
        return State::Unfulfilled;

    DbgWaitCI= ci;
    return future.get();
}

Promise::State Promise::WaitUntil( const Ticks& wakeUpTime, const CallerInfo& ci ) {
    ALIB_ASSERT_ERROR( DbgWaitCI.File == nullptr, "THREADS",
                       "Promise was already awaited. Repeated calls not allowed.\n"
                       "  Received with: ", DbgWaitCI )

    if ( future.wait_until(wakeUpTime.Export()) == std::future_status::timeout )
        return State::Unfulfilled;

    DbgWaitCI= ci;
    return future.get();
}

} // namespace [alib::threads]

#endif // !ALIB_SINGLE_THREADED
