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
#include "alib/strings/strings.prepro.hpp"
//============================================== Module ============================================
#if ALIB_C20_MODULES
    module ALib.Strings.Calendar;
    import   ALib.Lang;
#else
#   include "ALib.Lang.H"
#   include "ALib.Strings.H"
#   include "ALib.Strings.Calendar.H"
#endif
//========================================== Implementation ========================================

namespace   alib::strings::util {

#if !DOXYGEN
String CalendarDateTime::MONTH_NAMES[12]
#if ALIB_CAMP
    ;
#else
DOX_MARKER( [CDT_MONTH_NAMES])
={ A_CHAR("January"  ),  A_CHAR("February" ),
   A_CHAR("March"    ),  A_CHAR("April"    ),
   A_CHAR("May"      ),  A_CHAR("June"     ),
   A_CHAR("July"     ),  A_CHAR("August"   ),
   A_CHAR("September"),  A_CHAR("October"  ),
   A_CHAR("November" ),  A_CHAR("December" )  };
DOX_MARKER( [CDT_MONTH_NAMES])

#endif

String CalendarDateTime::DAY_NAMES[7]
#if ALIB_CAMP
    ;
#else
DOX_MARKER( [CDT_DAY_NAMES])
={ A_CHAR("Sunday"   ),  A_CHAR("Monday"   ),
   A_CHAR("Tuesday"  ),  A_CHAR("Wednesday"),
   A_CHAR("Thursday" ),  A_CHAR("Friday"   ),
   A_CHAR("Saturday" )                        };
DOX_MARKER( [CDT_DAY_NAMES])
#endif

#endif // !DOXYGEN


//##################################################################################################
// CalendarDateTime
//##################################################################################################
void CalendarDateTime::Clear() {
    Year=
    Month=
    Day=
    Hour=
    Minute=
    Second=
    Millisecond=    0;
    DayOfWeek=      -1;
}

void CalendarDateTime::Set( const DateTime& timeStamp, lang::Timezone timezone ) {
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

    #elif defined (__GLIBCXX__) || defined(_LIBCPP_VERSION) || defined(__APPLE__) || defined(__ANDROID_NDK__)
        struct tm  tm;
        time_t tt= timeStamp.InEpochSeconds();
        if ( timezone == lang::Timezone::UTC ) {
            tm.tm_isdst=      0; // daylight saving off
            gmtime_r( &tt, &tm );
        } else {
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

DateTime  CalendarDateTime::Get( lang::Timezone timezone )                                   const {
    DateTime result(lang::Initialization::Suppress);

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

    #elif defined (__GLIBCXX__) || defined(_LIBCPP_VERSION) || defined(__APPLE__) || defined(__ANDROID_NDK__)
        struct tm  tm;
        tm.tm_year=       Year - 1900;
        tm.tm_mday=       Day;
        tm.tm_mon=        Month -1;
        tm.tm_hour=       Hour;
        tm.tm_min=        Minute;
        tm.tm_sec=        Second;

        time_t tt;
        if ( timezone == lang::Timezone::UTC ) {
            tm.tm_isdst=      0; // daylight saving off
            tt= timegm( &tm );
        } else {
            tm.tm_isdst=     -1; // daylight saving auto
            tt= mktime( &tm );
        }

        result= DateTime::FromEpochSeconds( tt );


    #else
        #pragma message ("Unknown Platform in file: " __FILE__ )
    #endif

    return result;
}



//##################################################################################################
// CalendarDuration
//##################################################################################################
void CalendarDuration::Clear() {
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


void CalendarDuration::FromNanoSeconds( int64_t nanos ) {
    Clear();
    decltype(nanos) fract;
    if ( nanos > NanosPerDay )          { Days=         int( fract= nanos / NanosPerDay         );  nanos-= fract * NanosPerDay;         }
    if ( nanos > NanosPerHour )         { Hours=        int( fract= nanos / NanosPerHour        );  nanos-= fract * NanosPerHour;        }
    if ( nanos > NanosPerMinute )       { Minutes=      int( fract= nanos / NanosPerMinute      );  nanos-= fract * NanosPerMinute;      }
    if ( nanos > NanosPerSecond )       { Seconds=      int( fract= nanos / NanosPerSecond      );  nanos-= fract * NanosPerSecond;      }
    if ( nanos > NanosPerMillisecond )  { Milliseconds= int( fract= nanos / NanosPerMillisecond );  nanos-= fract * NanosPerMillisecond; }
    if ( nanos > NanosPerMicrosecond )  { Microseconds= int( fract= nanos / NanosPerMicrosecond );                                       }
}

int64_t     CalendarDuration::ToNanoSeconds() {
    return      Days          * NanosPerDay
             +  Hours         * NanosPerHour
             +  Minutes       * NanosPerMinute
             +  Seconds       * NanosPerSecond
             +  Milliseconds  * NanosPerMillisecond
             +  Microseconds  * NanosPerMicrosecond
             +  Nanoseconds;
}

//##################################################################################################
// CalendarDate
//##################################################################################################
void CalendarDate::Set( const DateTime& dateTime, lang::Timezone timezone ) {
    CalendarDateTime    cdt(dateTime, timezone);
    stamp=      uint32_t( cdt.Year        ) << 12
            |   uint32_t( cdt.Month       ) <<  8
            |   uint32_t( cdt.Day         ) <<  3
            |   uint32_t( cdt.DayOfWeek   )        ;
}
void CalendarDate::Set( int year, int month, int day, int dayOfWeek ) {
    ALIB_ASSERT_ERROR( year  >= 0  && year  <= 1048575, "CAMP", "CalendarDate: Years must be between 0 and 1,048,575. Given: ", year )
    ALIB_ASSERT_ERROR( month >= 1  && month <= 12     , "CAMP", "CalendarDate: Months must be between 1 and 12. Given: ", month )
    ALIB_ASSERT_ERROR( day   >= 1  && day   <= 31     , "CAMP", "CalendarDate: Days must be between 1 and 31. Given: ", day )
    ALIB_ASSERT_ERROR(            dayOfWeek <=  6     , "CAMP", "CalendarDate: Day of week must be either negative or between 0 and 6. Given: ", dayOfWeek)

    // get day of week, if not given
    if( dayOfWeek< 0 ) {
        dayOfWeek= CalendarDateTime( CalendarDateTime( year, month, day, 12, 0, 0 ).Get(),
                                     lang::Timezone::UTC                                ).DayOfWeek;
    }
    #if ALIB_DEBUG
    else {
        CalendarDateTime    cdt( year, month, day, 12, 0, 0 );
        cdt= CalendarDateTime( cdt.Get( )); // get day of week
        ALIB_ASSERT_ERROR( dayOfWeek == cdt.DayOfWeek, "CAMP",
          "Given day of week {} does not correspond to given date. Should be: ",
          dayOfWeek, cdt.DayOfWeek )
    }
    #endif
    stamp=      uint32_t( year        ) << 12
            |   uint32_t( month       ) <<  8
            |   uint32_t( day         ) <<  3
            |   uint32_t( dayOfWeek   )        ;
}

DateTime  CalendarDate::Get( lang::Timezone timezone, int hour, int minute, int second )       const
{ return CalendarDateTime( Year(), Month(), Day(), hour, minute, second).Get( timezone ); }

CalendarDate CalendarDate::operator+( int daysToAdd )                                        const {
    // use the system for it
    return CalendarDate( CalendarDateTime( Year(), Month(), Day(), 12 ).Get()
                         + DateTime::Duration::FromAbsoluteDays( daysToAdd ),
                         lang::Timezone::UTC );
}

CalendarDate CalendarDate::operator++() {
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
        } else {
            day= 1;
            if( ++month == 13 ) {
                month= 1;
                ++year;
        }   }
        Set( year, month, day, (DayOfWeek() + 1) % 7 );
        return *this;
    }

    // use the system for it
    CalendarDateTime cdt( year, month, day,  12 );
    return *this= CalendarDate( cdt.Get() + DateTime::Duration::FromAbsoluteDays(1), lang::Timezone::UTC );
}

CalendarDate CalendarDate::operator--() {
    auto day  = Day();

    // can we do it manually?
    if( day > 1 ) {
        stamp=   ( stamp & ~lang::LowerMask<8, uint32_t>()  )
               | ( uint32_t( day-1 ) << 3             )
               | ( ( (stamp & 7) + 6 ) % 7            ) ; // +6 corresponds to -1 in modulo op
        return *this;
    }

    // use the system for it
    return *this= CalendarDate( Get() - DateTime::Duration::FromAbsoluteDays(1), lang::Timezone::UTC );
}


//##################################################################################################
// CalendarDateTime::Format
//##################################################################################################
AString& CalendarDateTime::Format( Substring format, AString& target, lang::CurrentData targetData ) const {
    if ( targetData == lang::CurrentData::Clear )
        target.Reset();

    // this ensures that target is not nulled, as all other appends are NC-versions
    target._("");
    NumberFormat* nf= &NumberFormat::Computational;

    while ( format.IsNotEmpty() ) {
        // read n equal characters
        int   n=  1;
        character c=  format.ConsumeChar();
        while ( format.ConsumeChar(c) )
            ++n;

        switch (c) {
            case '\'': // single quotes
            {
                // one or more pairs of single quotes?
                if ( n > 1 ) {
                    int pairs= n / 2;
                    target.InsertChars<NC>( '\'', pairs );
                    n-= (pairs * 2);
                }

                // one single quote?
                if ( n == 1 ) {
                    // search end
                    integer end= format.IndexOf( '\'' );
                    if ( end < 1 ) {
                        ALIB_WARNING( "ALIB", "Format Error: Missing single Quote" )
                        target <<     "Format Error: Missing closing single quote character <'>" ;
                        return target;
                    }

                    target._<NC>( format, 0, end );
                    format.ConsumeChars<NC>( end + 1 );
                }

            } break;

            case 's': // second
                target.template _<NC>( alib::Dec( Second, n, nf ) );
                break;

            case 'm': //minute
                target._<NC>( alib::Dec( Minute, n, nf ) );
                break;

            case 'K': // hour 0..11
                target._<NC>( alib::Dec( Hour % 12, n, nf ) );
                target._<NC>( Hour < 12 ? " am" : " pm" );
                break;

            case 'H': // hour 0..23
                target._<NC>( alib::Dec( Hour,   n, nf ) );
                break;

            case 'd': // day
                     if ( n <= 2 )     target._<NC>( alib::Dec( Day, n, nf) );
                else if ( n == 3 )     target._<NC>( DAY_NAMES[DayOfWeek], 0, 3 );
                else                   target._<NC>( DAY_NAMES[DayOfWeek]    );
                break;

            case 'M': // month
                     if ( n <= 2 )     target._<NC>( alib::Dec( Month, n, nf ) );
                else if ( n == 3 )     target._<NC>( MONTH_NAMES[Month-1], 0, 3 );
                else                   target._<NC>( MONTH_NAMES[Month-1]     );
                break;

            case 'y': // year
                     if ( n == 1 )     target._<NC>( alib::Dec(Year,        1, nf) );
                else if ( n == 2 )     target._<NC>( alib::Dec(Year %  100, 2, nf) );
                else                   target._<NC>( alib::Dec(Year,        n, nf) );
                break;

            default: // otherwise: copy what was in
                target.InsertChars<NC>( c, n );
                break;
        }

    }

    return target;
}



} // namespace [  alib::system]
