// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)
#if !defined(HPP_ALIB_TIME_TIME)
#   include "alib/time/time.hpp"
#endif

#if !defined (HPP_ALIB_TIME_TICKSCONVERTER)
#   include "alib/time/tickconverter.hpp"
#endif

#endif // !defined(ALIB_DOX)

using namespace std::chrono;

#if ALIB_BOXING
    ALIB_BOXING_VTABLE_DEFINE( alib::Ticks              , vt_time_ticks             )
    ALIB_BOXING_VTABLE_DEFINE( alib::Ticks::Duration    , vt_time_ticks_duration    )
    ALIB_BOXING_VTABLE_DEFINE( alib::DateTime           , vt_time_datetime          )
    ALIB_BOXING_VTABLE_DEFINE( alib::DateTime::Duration , vt_time_datetime_duration )
#endif

namespace alib {

/**
 * This namespace of \alib provides types for calendrical date and time processing as well as
 * for non-calendrical steady and monotonic time measurement.
 *
 * Besides this reference documentation, further information is provided with
 * \ref alib_mod_time "ALib Module Time - Programmer's Manual".
 */
namespace time {

// #################################################################################################
// Module Bootstrap/Termination
// #################################################################################################
#if !defined(ALIB_DOX)
namespace
{
    Ticks*   creationTime= nullptr;
}
#endif

void    Bootstrap()
{
    // create a ticks object that marks the time of creation of this non-camp module
    if( !creationTime )
        creationTime= new Ticks();

    ALIB_IF_BOXING(
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_time_ticks             )
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_time_ticks_duration    )
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_time_datetime          )
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_time_datetime_duration )  )
}

void    Shutdown()
{
    if( creationTime )
    {
        delete creationTime;
        creationTime= nullptr;
    }
}

Ticks&  CreationTime()
{
    return *creationTime;
}


// #################################################################################################
// TickConverter
// #################################################################################################
void TickConverter::SyncClocks( int qtyRepeats )
{
    Ticks::TTimePoint       steadyClock;
    DateTime::TTimePoint    systemClock;
    uint64_t lastDiff= 0;
    for( int i= 0 ; i < qtyRepeats ; ++i )
    {
        systemClock= system_clock::now();
        steadyClock= steady_clock::now();

        auto systemCount= systemClock.time_since_epoch().count();
        auto steadyCount= steadyClock.time_since_epoch().count();

        // This can not be optimized, because:
        // a) we have to use an unsigned integer, and
        // b) we have to take into account which clock was measured first and which last. If
        //    interrupted between the calls, the difference either shrinks or increases.
        if( systemCount < steadyCount )
        {
            uint64_t diff= static_cast<uint64_t >( steadyCount - systemCount );
            if( lastDiff == 0 || diff < lastDiff )
            {
                steadyClockSyncTime= steadyClock;
                systemClockSyncTime= systemClock;
                lastDiff= diff;
            }
        }
        else
        {
            uint64_t diff= static_cast<uint64_t >( systemCount - steadyCount  );
            if( lastDiff == 0 || diff > lastDiff )
            {
                steadyClockSyncTime= steadyClock;
                systemClockSyncTime= systemClock;
                lastDiff= diff;
            }
        }
    }
}



// #################################################################################################
// Windows OS specific: file time, system time
// #################################################################################################
#if defined( _WIN32 ) && !defined(ALIB_DOX)

// filetime_duration has the same layout as FILETIME; 100ns intervals
using filetime_duration = duration<int64_t, std::ratio<1, 10000000> >;

// January 1, 1601 (NT epoch) - January 1, 1970 (Unix epoch):
constexpr duration<int64_t> nt_to_unix_epoch{INT64_C(-11644473600)};

FILETIME  DateTime::ToFileTime()        const
{
    const auto   asDuration = duration_cast<filetime_duration>( stamp.time_since_epoch() );
    const auto   withNtEpoch= asDuration - nt_to_unix_epoch;
    const auto   rawCount   = withNtEpoch.count();

    FILETIME result;
    result.dwLowDateTime  = static_cast<DWORD>(rawCount); // discards upper bits
    result.dwHighDateTime = static_cast<DWORD>(rawCount >> 32);
    return result;

}

ULARGE_INTEGER  DateTime::ToFileTimeLI()   const
{
    FILETIME ft= ToFileTime();
    ULARGE_INTEGER result;
    result.HighPart= ft.dwHighDateTime;
    result.LowPart=  ft.dwLowDateTime;
    return result;
}

DateTime DateTime::FromFileTime( const FILETIME& fileTime )
{
    const filetime_duration ftDuration { static_cast<int64_t>(  (static_cast<uint64_t>(   fileTime.dwHighDateTime) << 32)
                                                                                        | fileTime.dwLowDateTime          ) };
    return DateTime( TTimePoint(ftDuration + nt_to_unix_epoch));
}

DateTime DateTime::FromFileTime( const ULARGE_INTEGER& ft )
{
    FILETIME fileTime;
    fileTime.dwLowDateTime  = ft.LowPart;
    fileTime.dwHighDateTime = ft.HighPart;
    return FromFileTime( fileTime );
}

SYSTEMTIME  DateTime::ToSystemTime( lang::Timezone timezone ) const
{
    FILETIME ft= ToFileTime();
    SYSTEMTIME result;
    if ( timezone == lang::Timezone::UTC )
        FileTimeToSystemTime( &ft, &result );
    else
    {
        SYSTEMTIME utc;
        FileTimeToSystemTime( &ft, &utc );
        SystemTimeToTzSpecificLocalTime( NULL, &utc, &result );
    }
    return result;
}

DateTime DateTime::FromSystemTime( const SYSTEMTIME& st, lang::Timezone timezone )
{
    FILETIME ft;
    if ( timezone == lang::Timezone::UTC )
        SystemTimeToFileTime( &st, &ft );
    else
    {
        SYSTEMTIME utc;
        TzSpecificLocalTimeToSystemTime( NULL, &st, &utc);
        SystemTimeToFileTime( &utc, &ft );
    }
    return DateTime::FromFileTime( ft );
}
#endif // defined( _WIN32 ) && !defined(ALIB_DOX)
}} // namespace [alib::time]
