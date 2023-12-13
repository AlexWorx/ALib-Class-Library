// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2023 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)
#   if !defined(HPP_ALIB_SYSTEM_SYSTEM)
#      include "alib/system/system.hpp"
#   endif
#   if !defined (HPP_ALIB_FS_MODULES_DISTRIBUTION)
#      include "alib/lib/fs_modules/distribution.hpp"
#   endif
#   if !defined (HPP_ALIB_STRINGS_NUMBERFORMAT)
#      include "alib/strings/numberformat.hpp"
#   endif
#   if !defined (HPP_ALIB_STRINGS_FORMAT)
#       include "alib/strings/format.hpp"
#   endif
#   if !defined (HPP_ALIB_STRINGS_SUBSTRING)
#      include "alib/strings/substring.hpp"
#   endif
#   if !defined(HPP_ALIB_TIME_TIME)
#      include "alib/time/time.hpp"
#   endif
#   if !defined (HPP_ALIB_RESOURCES_RESOURCES)
#      include "alib/resources/resources.hpp"
#   endif
#   if !defined (HPP_ALIB_RESULTS_RESULTS)
#      include "alib/results/results.hpp"
#   endif
#   if !defined (HPP_ALIB_SYSTEM_DIRECTORY)
#      include "alib/system/directory.hpp"
#   endif
#   if !defined (HPP_ALIB_SYSTEM_CALENDAR)
#      include "alib/system/calendar.hpp"
#   endif
#   if !defined (HPP_ALIB_ENUMS_RECORDBOOTSTRAP)
#      include "alib/enums/recordbootstrap.hpp"
#   endif
#   if !defined(HPP_ALIB_ENUMS_SERIALIZATION)
#      include "alib/enums/serialization.hpp"
#   endif
#if !defined(HPP_ALIB_TEXT_FWDS)
#   include "alib/text/fwds.hpp"
#endif
#endif // !defined(ALIB_DOX)

namespace aworx { namespace lib { namespace system {


// #################################################################################################
// CalendarDateTime
// #################################################################################################
void CalendarDateTime::Clear()
{
    Year=
    Month=
    Day=
    Hour=
    Minute=
    Second=
    Millisecond=    0;
    DayOfWeek=      -1;
}

void CalendarDateTime::Set( const DateTime& timeStamp, Timezone timezone )
{
    Clear();

    #if defined( _WIN32 )
        SYSTEMTIME st= timeStamp.ToSystemTime( timezone );

        Year=       st.wYear;
        Day=        st.wDay;
        DayOfWeek=  st.wDayOfWeek;
        Month=      st.wMonth;
        Hour=       st.wHour;
        Minute=     st.wMinute;
        Second=     st.wSecond;

    #elif defined (__GLIBCXX__) || defined(__APPLE__)  || defined(__ANDROID_NDK__)
        struct tm  tm;
        time_t tt= timeStamp.InEpochSeconds();
        if ( timezone == Timezone::UTC )
        {
            tm.tm_isdst=      0; // daylight saving off
            gmtime_r( &tt, &tm );
        }
        else
        {
            tm.tm_isdst=     -1; // daylight saving auto
            localtime_r( &tt, &tm );
        }

        Year=       tm.tm_year + 1900;
        Day=        tm.tm_mday;
        DayOfWeek=  tm.tm_wday;
        Month=      tm.tm_mon + 1;
        Second=     tm.tm_sec;
        Hour=       tm.tm_hour;
        Minute=     tm.tm_min;

    #else
        #pragma message ("Unknown Platform in file: " __FILE__ )
    #endif
}

DateTime  CalendarDateTime::Get( Timezone timezone )                                           const
{
    DateTime result(Initialization::Suppress);

    #if defined( _WIN32 )

        SYSTEMTIME st;
        st.wYear=           (WORD) Year;
        st.wDay=            (WORD) Day;
        st.wDayOfWeek=      (WORD) DayOfWeek;
        st.wMonth=          (WORD) Month;
        st.wHour=           (WORD) Hour;
        st.wMinute=         (WORD) Minute;
        st.wSecond=         (WORD) Second;
        st.wMilliseconds=   0;

        result= DateTime::FromSystemTime( st, timezone );

    #elif defined (__GLIBCXX__) || defined(__APPLE__) || defined(__ANDROID_NDK__)
        struct tm  tm;
        tm.tm_year=       Year - 1900;
        tm.tm_mday=       Day;
        tm.tm_mon=        Month -1;
        tm.tm_hour=       Hour;
        tm.tm_min=        Minute;
        tm.tm_sec=        Second;

        time_t tt;
        if ( timezone == Timezone::UTC )
        {
            tm.tm_isdst=      0; // daylight saving off
            tt= timegm( &tm );
        }
        else
        {
            tm.tm_isdst=     -1; // daylight saving auto
            tt= mktime( &tm );
        }

        result= DateTime::FromEpochSeconds( tt );


    #else
        #pragma message ("Unknown Platform in file: " __FILE__ )
    #endif

    return result;
}



// #################################################################################################
// CalendarDuration
// #################################################################################################
void CalendarDuration::Clear()
{
    Days=
    Hours=
    Minutes=
    Seconds=
    Milliseconds=
    Microseconds=
    Nanoseconds=    0;
}
#define  NanosPerDay           INT64_C( 86400000000000 ) ///< Constant denoting number of nanoseconds of a day.
#define  NanosPerHour          INT64_C(  3600000000000 ) ///< Constant denoting number of nanoseconds of an hour.
#define  NanosPerMinute        INT64_C(    60000000000 ) ///< Constant denoting number of nanoseconds of a minute.
#define  NanosPerSecond        INT64_C(     1000000000 ) ///< Constant denoting number of nanoseconds of a second.
#define  NanosPerMillisecond   INT64_C(        1000000 ) ///< Constant denoting number of nanoseconds of a millisecond.
#define  NanosPerMicrosecond   INT64_C(           1000 ) ///< Constant denoting number of nanoseconds of a microsecond.


void CalendarDuration::FromNanoSeconds( int64_t nanos )
{
    Clear();
    decltype(nanos) fract;
    if ( nanos > NanosPerDay )          { Days=         static_cast<int>( fract= nanos / NanosPerDay         );  nanos-= fract * NanosPerDay;         }
    if ( nanos > NanosPerHour )         { Hours=        static_cast<int>( fract= nanos / NanosPerHour        );  nanos-= fract * NanosPerHour;        }
    if ( nanos > NanosPerMinute )       { Minutes=      static_cast<int>( fract= nanos / NanosPerMinute      );  nanos-= fract * NanosPerMinute;      }
    if ( nanos > NanosPerSecond )       { Seconds=      static_cast<int>( fract= nanos / NanosPerSecond      );  nanos-= fract * NanosPerSecond;      }
    if ( nanos > NanosPerMillisecond )  { Milliseconds= static_cast<int>( fract= nanos / NanosPerMillisecond );  nanos-= fract * NanosPerMillisecond; }
    if ( nanos > NanosPerMicrosecond )  { Microseconds= static_cast<int>( fract= nanos / NanosPerMicrosecond );                                       }
}

int64_t     CalendarDuration::ToNanoSeconds()
{
    return      Days          * NanosPerDay
             +  Hours         * NanosPerHour
             +  Minutes       * NanosPerMinute
             +  Seconds       * NanosPerSecond
             +  Milliseconds  * NanosPerMillisecond
             +  Microseconds  * NanosPerMicrosecond
             +  Nanoseconds;
}

// #################################################################################################
// CalendarDate
// #################################################################################################
void CalendarDate::Set( const DateTime& dateTime, Timezone timezone )
{
    CalendarDateTime    cdt(dateTime, timezone);
    stamp=      uint32_t( cdt.Year        ) << 12
            |   uint32_t( cdt.Month       ) <<  8
            |   uint32_t( cdt.Day         ) <<  3
            |   uint32_t( cdt.DayOfWeek   )        ;
}
void CalendarDate::Set( int year, int month, int day, int dayOfWeek )
{
    ALIB_ASSERT_ERROR( year  >= 0  && year  <= 1048575, "SYSTEM", "CalendarDate: Years must be between 0 and 1,048,575." )
    ALIB_ASSERT_ERROR( month >= 1  && month <= 12     , "SYSTEM", "CalendarDate: Months must be between 1 and 12." )
    ALIB_ASSERT_ERROR( day   >= 1  && day   <= 31     , "SYSTEM", "CalendarDate: Days must be between 1 and 31." )
    ALIB_ASSERT_ERROR(            dayOfWeek <=  6     , "SYSTEM", "CalendarDate: Day of week must be either negative or between 0 and 6." )

    // get day of week, if not given
    if( dayOfWeek< 0 )
    {
        dayOfWeek= CalendarDateTime( CalendarDateTime( year, month, day, 12, 0, 0 ).Get(),
                                     Timezone::UTC                                         ).DayOfWeek;
    }
    #if ALIB_DEBUG
    else
    {
        CalendarDateTime    cdt( year, month, day, 12, 0, 0 );
        cdt= CalendarDateTime( cdt.Get( )); // get day of week
        ALIB_ASSERT_ERROR( dayOfWeek == cdt.DayOfWeek, "SYSTEM",
                           "Day of week does not correspond to given date. Should be: ", cdt.DayOfWeek )
    }
    #endif
    stamp=      uint32_t( year        ) << 12
            |   uint32_t( month       ) <<  8
            |   uint32_t( day         ) <<  3
            |   uint32_t( dayOfWeek   )        ;
}

DateTime  CalendarDate::Get( Timezone timezone, int hour, int minute, int second )             const
{
    return CalendarDateTime( Year(), Month(), Day(), hour, minute, second).Get( timezone );
}

CalendarDate CalendarDate::operator+( int daysToAdd )                                          const
{
    // use the system for it
    return CalendarDate( CalendarDateTime( Year(), Month(), Day(), 12 ).Get()
                         + DateTime::Duration::FromAbsoluteDays( daysToAdd ),
                         Timezone::UTC );
}

CalendarDate CalendarDate::operator++()
{
    auto day  = Day  ();
    auto month= Month();
    auto year = Year ();

    // can we do it manually?
    if(    ( month != 2 && day != 30 )
        || ( month == 2 && day != 28 ) )
    {
        if(    ( day != 31               )
            && ( day != 29 || month != 2 ) )
        {
            day++;
        }
        else
        {
            day= 1;
            if( ++month == 13 )
            {
                month= 1;
                ++year;
            }
        }
        Set( year, month, day, (DayOfWeek() + 1) % 7 );
        return *this;
    }

    // use the system for it
    CalendarDateTime cdt( year, month, day,  12 );
    return *this= CalendarDate( cdt.Get() + DateTime::Duration::FromAbsoluteDays(1), Timezone::UTC );
}

CalendarDate CalendarDate::operator--()
{
    auto day  = Day();

    // can we do it manually?
    if( day > 1 )
    {
        stamp=   ( stamp & ~LowerMask<8, uint32_t>()  )
               | ( uint32_t( day-1 ) << 3             )
               | ( ( (stamp & 7) + 6 ) % 7            ) ; // +6 corresponds to -1 in modulo op
        return *this;
    }

    // use the system for it
    return *this= CalendarDate( Get() - DateTime::Duration::FromAbsoluteDays(1), Timezone::UTC );
}

}}}// namespace [aworx::lib::system]

