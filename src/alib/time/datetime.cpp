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

//##################################################################################################
// Windows OS specific: file time, system time
//##################################################################################################
#if defined( _WIN32 ) && !DOXYGEN

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
    const filetime_duration ftDuration { int64_t(  (uint64_t(   fileTime.dwHighDateTime) << 32)
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
#endif // defined( _WIN32 ) && !DOXYGEN
} // namespace [alib::time]
