// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################

#include "alib/alib.hpp"

#if !defined (HPP_ALIB_TIME_CALENDAR)
    #include "alib/time/calendar.hpp"
#endif

#if !defined (HPP_ALIB_TIME_TICKSCONVERTER)
#   include "alib/time/ticksconverter.hpp"
#endif


#if ALIB_MODULE_STRINGS
    #if !defined (HPP_ALIB_STRINGS_SUBSTRING)
        #include "alib/strings/substring.hpp"
    #endif

    #if !defined (HPP_ALIB_STRINGS_NUMBERFORMAT)
        #include "alib/strings/numberformat.hpp"
    #endif
#endif

#if !defined (_GLIBCXX_ALGORITHM) && !defined (_ALGORITHM_)
    #include <algorithm>
#endif


using namespace std::chrono;

//! @cond NO_DOX

namespace aworx { namespace lib {

// The singleton of the library class located in namespace aworx::lib.
time::TimeLib TIME;

String   time::TimeLib::months[12];
String   time::TimeLib::days  [ 7];

namespace time {

// #################################################################################################
// TimeLib
// #################################################################################################
void TimeLib::init( Phases phase )
{
    if( phase == Phases::resourceset )
    {
        #if ALIB_MODULE_BOXING
            boxing::DefineInterface<time::DateTime, false, time::IFormat_DateTime>();
            ALIB_BOXING_DEFINE_IAPPLY_FOR_APPLICABLE_TYPE_W( time::DateTime::Duration )
            ALIB_BOXING_DEFINE_IAPPLY_FOR_APPLICABLE_TYPE_N( time::DateTime::Duration )
            ALIB_BOXING_DEFINE_IAPPLY_FOR_APPLICABLE_TYPE_N( time::Ticks::Duration )
            ALIB_BOXING_DEFINE_IAPPLY_FOR_APPLICABLE_TYPE_W( time::Ticks::Duration )
        #endif

        Res->AddBulk( ResourceCategory.ToCString()
        , ASTR("Months"), ASTR("January,February,March,April,May,June,July,August,September,October,November,December")
        , ASTR("Days"),   ASTR("Sunday,Monday,Tuesday,Wednesday,Thursday,Friday,Saturday,")

        , ASTR("TS_ZERO") , ASTR("zero time")
        , ASTR("DayP")    , ASTR(" days")
        , ASTR("DayS")    , ASTR(" day")
        , ASTR("HourP")   , ASTR(" hours")
        , ASTR("HourS")   , ASTR(" hour")
        , ASTR("MinP")    , ASTR(" minutes")
        , ASTR("MinS")    , ASTR(" minute")
        , ASTR("SecP")    , ASTR(" seconds")
        , ASTR("SecS")    , ASTR(" second")
        , ASTR("MlSecP")  , ASTR(" ms")
        , ASTR("MlSecS")  , ASTR(" ms")
        , ASTR("McSecP")  , ASTR(" \u00B5s")
        , ASTR("McSecS")  , ASTR(" \u00B5s")
        , ASTR("NSecP")   , ASTR(" ns")
        , ASTR("NSecS")   , ASTR(" ns")
        ,nullptr );


        // create a ticks object that marks the time of creation of ALIB
        creationTime= new Ticks();
    }

    else if( phase == Phases::final )
    {
        Substring parser;
        parser= Get( ASTR("Months") ); for( int i= 0 ; i < 12 ; ++i ) months[i]= parser.ConsumeToken(',');
        parser= Get( ASTR("Days")   ); for( int i= 0 ; i <  7 ; ++i )   days[i]= parser.ConsumeToken(',');
    }
}

void TimeLib::terminationCleanUp()
{
    delete creationTime;
}


// #################################################################################################
// TicksConverter
// #################################################################################################
void TicksConverter::SyncClocks( int qtyRepeats )
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
        // a) we have to use a unsigned integer, and
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
#if defined( _WIN32 )

// filetime_duration has the same layout as FILETIME; 100ns intervals
using filetime_duration = duration<int64_t, std::ratio::ratio<1, 10000000>>;

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

SYSTEMTIME  DateTime::ToSystemTime( Timezone timezone ) const
{
    FILETIME ft= ToFileTime();
    SYSTEMTIME result;
    if ( timezone == Timezone::UTC )
        FileTimeToSystemTime( &ft, &result );
    else
    {
        SYSTEMTIME utc;
        FileTimeToSystemTime( &ft, &utc );
        SystemTimeToTzSpecificLocalTime( NULL, &utc, &result );
    }
    return result;
}

DateTime DateTime::FromSystemTime( const SYSTEMTIME& st, Timezone timezone )
{
    FILETIME ft;
    if ( timezone == Timezone::UTC )
        SystemTimeToFileTime( &st, &ft );
    else
    {
        SYSTEMTIME utc;
        TzSpecificLocalTimeToSystemTime( NULL, &st, &utc);
        SystemTimeToFileTime( &utc, &ft );
    }
    return DateTime::FromFileTime( ft );
}

#endif


#if ALIB_MODULE_BOXING  && !defined(DOX_PARSER)

//! [DOX_ALIB_BOXING_IFORMAT_DATETIME]
void IFormat_DateTime::Invoke( const Box& box, const String& formatSpec, AString& target )
{
    CalendarDateTime tct( box.Unbox<DateTime>() );
    tct.Format( formatSpec, target );
}
//! [DOX_ALIB_BOXING_IFORMAT_DATETIME]

#endif

}}}// namespace [aworx::lib::time]



// #################################################################################################
// T_Apply<DateTime::Duration>
// #################################################################################################
#include "alib/strings/numberformat.hpp"
using namespace aworx;
using namespace aworx::lib::time;
using namespace aworx::lib::strings;

namespace {
void applyDateTime(AString& target, const DateTime::Duration pSrc)
{
    using Duration= DateTime::Duration;
    Duration src= pSrc;
    auto nanos= src.InNanoseconds();
    if( nanos == 0 )
    {
        target << aworx::lib::TIME.Get( ASTR("TS_ZERO") );
        return;
    }

    if( nanos < 0 )
    {
        target << ASTR("- ");
        src= Duration() - src;
    }

    NumberFormat nf( NumberFormat::Global );
    nf.FractionalPartWidth= 2;
    int64_t v= src.InAbsoluteDays();
    if( v >= 10 )
    {
        target << Format( src.InDays(), &nf ) << aworx::lib::TIME.Get( ASTR("DayP")   );
        return;
    }

    if( v >  0 )
    {
        target << v << ( v != 1 ?  aworx::lib::TIME.Get( ASTR("DayP") )
                                :  aworx::lib::TIME.Get( ASTR("DayS") ) );

        Duration cpy= src - ( Duration::FromAbsoluteDays(v) );

        target << ' ' << Format( cpy.InHours(), &nf ) << aworx::lib::TIME.Get( ASTR("HourP")   );
        return;
    }

    v= src.InAbsoluteHours();
    if( v >  0 )
    {
        target << v << ( v != 1 ?  aworx::lib::TIME.Get( ASTR("HourP") )
                                :  aworx::lib::TIME.Get( ASTR("HourS") ) );

        Duration cpy= src - ( Duration::FromAbsoluteHours(v) );

        auto minutes= cpy.InAbsoluteMinutes();
        target << ' ' << minutes <<  (minutes!= 1 ?  aworx::lib::TIME.Get( ASTR("MinP") )
                                                  :  aworx::lib::TIME.Get( ASTR("MinS") ) );
        return;
    }

    v= src.InAbsoluteMinutes();
    if( v > 0 )
    {
        target << v << ( v != 1 ?  aworx::lib::TIME.Get( ASTR("MinP") )
                                :  aworx::lib::TIME.Get( ASTR("MinS") ) );

        Duration cpy= src - ( Duration::FromAbsoluteMinutes(v) );

        auto seconds= cpy.InAbsoluteSeconds();
        target << ' ' << seconds <<  (seconds!= 1 ?  aworx::lib::TIME.Get( ASTR("SecP") )
                                                  :  aworx::lib::TIME.Get( ASTR("SecS") ) );
        return;
    }


    v= src.InAbsoluteSeconds();
    if( v > 0 )
    {
        target << Format( src.InSeconds(), &nf ) << aworx::lib::TIME.Get( ASTR("SecP")   );
        return;
    }

    nf.DecMinimumFieldWidth= 3;

    auto val= src.InAbsoluteMilliseconds();
    if( val >= 1 )
    {
        target << Format(val,&nf) << ( val!= 1  ?  aworx::lib::TIME.Get( ASTR("MlSecP") )
                                                :  aworx::lib::TIME.Get( ASTR("MlSecS") ) );
        return;
    }

    val= src.InAbsoluteMicroseconds();
    if( val >= 1 )
    {
        target << Format(val,&nf) << ( val!= 1  ?  aworx::lib::TIME.Get( ASTR("McSecP") )
                                                :  aworx::lib::TIME.Get( ASTR("McSecS") ) );
        return;
    }

    val= src.InNanoseconds();
    target << Format(val,&nf) << ( val!= 1  ?  aworx::lib::TIME.Get( ASTR("NSecP") )
                                            :  aworx::lib::TIME.Get( ASTR("NSecS") ) );
    return;
}
} //anonymous namespace

integer aworx::lib::strings::T_Apply<DateTime::Duration,character>::Apply(AStringBase<character>& target, const DateTime::Duration pSrc)
{
    applyDateTime( target, pSrc );
    return 1;
}

integer aworx::lib::strings::T_Apply<DateTime::Duration,complementChar>::Apply(AStringBase<complementChar>& target, const DateTime::Duration pSrc)
{
    aworx::lib::strings::PreallocatedStringBase<character, 128> buffer;
    applyDateTime( buffer, pSrc );
    target._<false>( buffer );
    return 1;
}

integer aworx::lib::strings::T_Apply<Ticks::Duration,character>::Apply(AStringBase<character>& target, const Ticks::Duration pSrc)
{
    // simply convert the ticks-duration to an DateTime duration and use its apply function
    applyDateTime( target, DateTime::Duration::FromNanoseconds( pSrc.InNanoseconds() ));
    return 1;
}

integer aworx::lib::strings::T_Apply<Ticks::Duration,complementChar>::Apply(AStringBase<complementChar>& target, const Ticks::Duration pSrc)
{
    // simply convert the ticks-duration to an DateTime duration and use its apply function
    aworx::lib::strings::PreallocatedStringBase<character, 128> buffer;
    applyDateTime( buffer, DateTime::Duration::FromNanoseconds( pSrc.InNanoseconds() ));
    target._<false>( buffer );
    return 1;
}

//! @endcond




