// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined (HPP_ALIB_SYSTEM_CALENDAR)
    #include "alib/system/calendar.hpp"
#endif

#if !defined (HPP_ALIB_SYSTEM_SYSTEM)
    #include "alib/system/system.hpp"
#endif

#if !defined (HPP_ALIB_STRINGS_NUMBERFORMAT)
    #include "alib/strings/numberformat.hpp"
#endif

#if !defined (HPP_ALIB_STRINGS_SUBSTRING)
    #include "alib/strings/substring.hpp"
#endif

// For code compatibility with ALox Java/C++
// We have to use underscore as the start of the name and for this have to disable a compiler
// warning. But this is a local code (cpp file) anyhow.
#if defined(__clang__)
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif

    #define _NC _<false>

#if defined(__clang__)
    #pragma clang diagnostic pop
#endif


namespace aworx { namespace lib { namespace system {


// #################################################################################################
// CalendarDateTime
// #################################################################################################

void CalendarDateTime::Clear()
{
    Year=
    Month=
    Day=
    DayOfWeek=
    Hour=
    Minute=
    Second=
    Millisecond=    0;
}

void CalendarDateTime::Set( const DateTime& timeStamp, Timezone timezone )
{
    Clear();

    #if defined (__GLIBCXX__) || defined(__APPLE__)
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

    #elif defined( _WIN32 )
        SYSTEMTIME st= timeStamp.ToSystemTime( timezone );

        Year=       st.wYear;
        Day=        st.wDay;
        DayOfWeek=  st.wDayOfWeek;
        Month=      st.wMonth;
        Hour=       st.wHour;
        Minute=     st.wMinute;
        Second=     st.wSecond;

    #else
        #pragma message ("Unknown Platform in file: " __FILE__ )
    #endif
}

DateTime  CalendarDateTime::Get( Timezone timezone )
{
    DateTime result(Initialization::Suppress);

    #if defined (__GLIBCXX__) || defined(__APPLE__)
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


    #elif defined( _WIN32 )

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

    #else
        #pragma message ("Unknown Platform in file: " __FILE__ )
    #endif

    return result;
}


AString& CalendarDateTime::Format( Substring format, AString& target, CurrentData targetData )
{
    if ( targetData == CurrentData::Clear )
        target.Reset();

    // this ensures that target is not nulled, as all other appends are NC-versions
    target._("");
    NumberFormat* nf= &NumberFormat::Computational;

    while ( format.IsNotEmpty() )
    {
        // read n equal characters
        int   n=  1;
        character c=  format.ConsumeChar();
        while (  format.ConsumeChar(c) )
            n++;

        switch (c)
        {
            case '\'': // single quotes
            {
                // one or more pairs of single quotes?
                if ( n > 1 )
                {
                    int pairs= n >> 1;
                    target.InsertChars<false>( '\'', pairs );
                    n-= (pairs << 1);
                }

                // one single quote?
                if ( n == 1 )
                {
                    // search end
                    integer end= format.IndexOf( '\'' );
                    if ( end < 1 )
                    {
                        ALIB_WARNING( "Format Error: Missing single Quote" );
                        target <<     "Format Error: Missing single Quote" ;
                        return target;
                    }

                    target._NC( format, 0, end );
                    format.ConsumeChars<false>( end + 1 );
                }

            } break;

            case 's': // second
                target.template _<false>( aworx::Format( Second, n, nf ) );
                break;

            case 'm': //minute
                target._NC( aworx::Format( Minute, n, nf ) );
                break;

            case 'K': // hour 0..11
                target._NC( aworx::Format( Hour % 12, n, nf ) );
                target._NC( Hour < 12 ? " am" : " pm" );
                break;

            case 'H': // hour 0..23
                target._NC( aworx::Format( Hour,   n, nf ) );
                break;

            case 'd': // day
                     if ( n <= 2 )     target._NC( aworx::Format( Day, n, nf) );
                else if ( n == 3 )     target._NC( System::days[DayOfWeek], 0, 3 );
                else                   target._NC( System::days[DayOfWeek]    );
                break;

            case 'M': // month
                     if ( n <= 2 )     target._NC( aworx::Format( Month, n, nf ) );
                else if ( n == 3 )     target._NC( System::months[Month-1], 0, 3 );
                else                   target._NC( System::months[Month-1]     );
                break;

            case 'y': // year
                     if ( n == 1 )     target._NC( aworx::Format(Year,        1, nf) );
                else if ( n == 2 )     target._NC( aworx::Format(Year %  100, 2, nf) );
                else                   target._NC( aworx::Format(Year,        n, nf) );
                break;

            default: // otherwise: copy what was in
                target.InsertChars<false>( c, n );
                break;
        }

    }

    return target;
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


}}}// namespace [aworx::lib::system]

