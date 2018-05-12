// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################

//! @cond NO_DOX

#include "alib/alib.hpp"
#include "alib/strings/substring.hpp"
#include "alib/time/datetime.hpp"
#include "alib/time/calendar.hpp"

#include "dateandtime.hpp"
#include "../compiler.hpp"
#include <math.h>

#define ARG0           (*args)
#define ARG1           (*(args+1))
#define INT(box)       (box).Unbox<boxed_int         >()
#define FLT(box)       (box).Unbox<double            >()
#define DT(box)        (box).Unbox<DateTime          >()
#define DUR(box)       (box).Unbox<DateTime::Duration>()
#define FUNC( name,...) Box name( Scope& scope,                                                 \
                                  ArgIterator  args,                                            \
                                  ArgIterator  end    )                                         \
                                  { (void) scope; (void) args; (void) end; __VA_ARGS__ }

namespace aworx { namespace lib { namespace expressions { namespace plugins {


namespace {

// #################################################################################################
// ### Reverse generation: convert program constants to expression strings
// #################################################################################################
//! [DOX_ALIB_EXPR_IToLiteral_3]
class IToLiteral_Duration : public IToLiteral, public Singleton<IToLiteral_Duration>
{
    virtual void Invoke( const Box& constantValue, AString& expressionString )
    {
        // unbox  the time span to nanoseconds
        auto duration= constantValue.Unbox<DateTime::Duration>();

        auto value= duration.InNanoseconds();

        // find the best matching magnitude
        NString result;
        if( value == 0 )
            result= "Milliseconds";
        else
        {
            result= "Nanoseconds";

            if( (value % 1000) == 0 )
            {
                value/= 1000;
                result= "Microseconds";
                if( (value % 1000) == 0 )
                {
                    value/= 1000;
                    result= "Milliseconds";
                    if( (value % 1000) == 0 )
                    {
                        value/= 1000;
                        result= "Seconds";
                        if( (value % 60) == 0 )
                        {
                            value/= 60;
                            result= "Minutes";
                            if( (value % 60) == 0 )
                            {
                                value/= 60;
                                result= "Hours";
                                if( (value % 24) == 0 )
                                {
                                    value/= 24;
                                    result= "Days";
                                }
                            }
                        }
                    }
                }
            }
        }

        // write the function argument
        expressionString << result << '(' << value << ')' ;
    }
};
//! [DOX_ALIB_EXPR_IToLiteral_3]

class IToLiteral_DateTime : public IToLiteral, public Singleton<IToLiteral_DateTime>
{
    virtual void Invoke( const Box& constantValue, AString& expressionString )
    {
        CalendarDateTime ct( constantValue.Unbox<DateTime>(), Timezone::UTC );

        expressionString << "UTCDateTime( " << ct.Year         << ','
                                             << ct.Month        << ','
                                             << ct.Day          << ','
                                             << ct.Hour         << ','
                                             << ct.Minute       << ','
                                             << ct.Second       << ','
                                             << ct.Millisecond  << ')' ;
    }
};


// Parameter signatures
std::vector<Box>  SIG_NONE;
std::vector<Box>  SIG_DT;
std::vector<Box>  SIG_DUR;
std::vector<Box>  SIG_DT_DUR;
std::vector<Box>  SIG_INT;
std::vector<Box>  SIG_INT_VAR;
std::vector<Box>  SIG_FLT;

// #################################################################################################
// ### DateTime
// #################################################################################################
FUNC( dateTime        , auto qtyArgs= end-args;
                        return CalendarDateTime(                static_cast<int>( INT(ARG0) )         , // year
                                             ( qtyArgs> 1  ) ?  static_cast<int>( INT(*(args+1)) ) : 1, // month
                                             ( qtyArgs> 2  ) ?  static_cast<int>( INT(*(args+2)) ) : 1, // day
                                             ( qtyArgs> 3  ) ?  static_cast<int>( INT(*(args+3)) ) : 0, // hour
                                             ( qtyArgs> 4  ) ?  static_cast<int>( INT(*(args+4)) ) : 0, // minute
                                             ( qtyArgs> 5  ) ?  static_cast<int>( INT(*(args+5)) ) : 0, // second
                                             ( qtyArgs> 6  ) ?  static_cast<int>( INT(*(args+6)) ) : 0  // millisecond
                                           )
                               .Get(Timezone::Local);
           )

FUNC( utcDateTime     , auto qtyArgs= end-args;
                        return CalendarDateTime(                static_cast<int>( INT(ARG0) )         , // year
                                             ( qtyArgs> 1  ) ?  static_cast<int>( INT(*(args+1)) ) : 1, // month
                                             ( qtyArgs> 2  ) ?  static_cast<int>( INT(*(args+2)) ) : 1, // day
                                             ( qtyArgs> 3  ) ?  static_cast<int>( INT(*(args+3)) ) : 0, // hour
                                             ( qtyArgs> 4  ) ?  static_cast<int>( INT(*(args+4)) ) : 0, // minute
                                             ( qtyArgs> 5  ) ?  static_cast<int>( INT(*(args+5)) ) : 0, // second
                                             ( qtyArgs> 6  ) ?  static_cast<int>( INT(*(args+6)) ) : 0  // millisecond
                                           )
                               .Get(Timezone::UTC);
           )

FUNC( today           , CalendarDateTime ct(DateTime(), Timezone::Local);
                        ct.Hour= ct.Minute= ct.Second= ct.Millisecond= 0;
                        return ct.Get(Timezone::Local);                     )
FUNC( utcToday        , CalendarDateTime ct(DateTime(), Timezone::UTC);
                        ct.Hour= ct.Minute= ct.Second= ct.Millisecond= 0;
                        return ct.Get(Timezone::UTC);                     )
FUNC( now             , return DateTime();                                 )
FUNC( age             , return DT(ARG0).Age();                            )
FUNC( isOlderThan     , return DT(ARG0).IsOlderThan( DUR(ARG1) );      )
FUNC( year            , return CalendarDateTime(DT(ARG0), Timezone::Local).Year;        )
FUNC( month           , return CalendarDateTime(DT(ARG0), Timezone::Local).Month;       )
FUNC( day             , return CalendarDateTime(DT(ARG0), Timezone::Local).Day;         )
FUNC( dayOfWeek       , return CalendarDateTime(DT(ARG0), Timezone::Local).DayOfWeek;   )
FUNC( hour            , return CalendarDateTime(DT(ARG0), Timezone::Local).Hour;        )
FUNC( minute          , return CalendarDateTime(DT(ARG0), Timezone::Local).Minute;      )
FUNC( millisecond     , return CalendarDateTime(DT(ARG0), Timezone::Local).Millisecond; )
FUNC( utcYear         , return CalendarDateTime(DT(ARG0), Timezone::UTC  ).Year;        )
FUNC( utcMonth        , return CalendarDateTime(DT(ARG0), Timezone::UTC  ).Month;       )
FUNC( utcDay          , return CalendarDateTime(DT(ARG0), Timezone::UTC  ).Day;         )
FUNC( utcDayOfWeek    , return CalendarDateTime(DT(ARG0), Timezone::UTC  ).DayOfWeek;   )
FUNC( utcHour         , return CalendarDateTime(DT(ARG0), Timezone::UTC  ).Hour;        )
FUNC( utcMinute       , return CalendarDateTime(DT(ARG0), Timezone::UTC  ).Minute;      )
FUNC( utcMillisecond  , return CalendarDateTime(DT(ARG0), Timezone::UTC  ).Millisecond; )


// #################################################################################################
// ### Duration
// #################################################################################################


// constructor functions
FUNC( nanosecondsInt  , return DateTime::Duration::FromNanoseconds          ( INT(ARG0)       ); )

FUNC( microsecondsInt , return DateTime::Duration::FromAbsoluteMicroseconds ( INT(ARG0)       ); )
FUNC( millisecondsInt , return DateTime::Duration::FromAbsoluteMilliseconds ( INT(ARG0)       ); )
FUNC( secondsInt      , return DateTime::Duration::FromAbsoluteSeconds      ( INT(ARG0)       ); )
FUNC( minutesInt      , return DateTime::Duration::FromAbsoluteMinutes      ( INT(ARG0)       ); )
FUNC( hoursInt        , return DateTime::Duration::FromAbsoluteHours        ( INT(ARG0)       ); )
FUNC( daysInt         , return DateTime::Duration::FromAbsoluteDays         ( INT(ARG0)       ); )
FUNC( weeksInt        , return DateTime::Duration::FromAbsoluteDays         ( INT(ARG0) *   7 ); )
FUNC( monthsInt       , return DateTime::Duration::FromAbsoluteDays         ( INT(ARG0) *  30 ); )
FUNC( yearsInt        , return DateTime::Duration::FromAbsoluteDays         ( INT(ARG0) * 365 ); )

FUNC( microsecondsFlt , return DateTime::Duration::FromMicroseconds         ( FLT(ARG0)         ); )
FUNC( millisecondsFlt , return DateTime::Duration::FromMilliseconds         ( FLT(ARG0)         ); )
FUNC( secondsFlt      , return DateTime::Duration::FromSeconds              ( FLT(ARG0)         ); )
FUNC( minutesFlt      , return DateTime::Duration::FromMinutes              ( FLT(ARG0)         ); )
FUNC( hoursFlt        , return DateTime::Duration::FromHours                ( FLT(ARG0)         ); )
FUNC( daysFlt         , return DateTime::Duration::FromDays                 ( FLT(ARG0)         ); )
FUNC( weeksFlt        , return DateTime::Duration::FromDays                 ( FLT(ARG0) *   7.0 ); )
FUNC( monthsFlt       , return DateTime::Duration::FromDays                 ( FLT(ARG0) *  30.0 ); )
FUNC( yearsFlt        , return DateTime::Duration::FromDays                 ( FLT(ARG0) * 365.0 ); )


// conversion
FUNC( inDays          , return DUR(ARG0).InDays       (); )
FUNC( inHours         , return DUR(ARG0).InHours      (); )
FUNC( inMinutes       , return DUR(ARG0).InMinutes    (); )
FUNC( inSeconds       , return DUR(ARG0).InSeconds    (); )
FUNC( inMilliseconds  , return DUR(ARG0).InMilliseconds(); )
FUNC( inMicroseconds  , return DUR(ARG0).InMicroseconds(); )
FUNC( inNanoseconds   , return DUR(ARG0).InNanoseconds (); )
FUNC( inHertz         , return DUR(ARG0).InHertz(2);       )

// binary operators time stamp
FUNC( add_DTDUR , return  DT(ARG0)  +  DUR(ARG1);   )
FUNC( add_DURDT , return  DT(ARG1)  +  DUR(ARG0);   )
FUNC( sub_DTDUR , return  DT(ARG0)  -  DUR(ARG1);   )
FUNC( sub_DTDT  , return  DT(ARG0)  -  DT(ARG1);   )
FUNC(   eqDT    , return  DT(ARG0)  == DT(ARG1);   )
FUNC(  neqDT    , return  DT(ARG0)  != DT(ARG1);   )
FUNC(   gtDT    , return  DT(ARG0)  >  DT(ARG1);   )
FUNC( gteqDT    , return  DT(ARG0)  >= DT(ARG1);   )
FUNC(   smDT    , return  DT(ARG0)  <  DT(ARG1);   )
FUNC( smeqDT    , return  DT(ARG0)  <= DT(ARG1);   )


// binary operators time span
FUNC( add_DURDUR , return  DUR(ARG0)  +  DUR(ARG1);   )
FUNC( sub_DURDUR , return  DUR(ARG0)  -  DUR(ARG1);   )
FUNC( mul_DURF   , return  DUR(ARG0)  *  FLT(ARG1);   )
FUNC( mul_FDUR   , return  DUR(ARG1)  *  FLT(ARG0);   )
FUNC( mul_DURI   , return  DUR(ARG0)  *  INT(ARG1);   )
FUNC( mul_IDUR   , return  DUR(ARG1)  *  INT(ARG0);   )
FUNC( div_DURF   , return  DUR(ARG0)  /  FLT(ARG1);   )
FUNC( div_DURI   , return  DUR(ARG0)  /  INT(ARG1);   )

FUNC(   eqDUR    , return  DUR(ARG0)  == DUR(ARG1);   )
FUNC(  neqDUR    , return  DUR(ARG0)  != DUR(ARG1);   )
FUNC(   gtDUR    , return  DUR(ARG0)  >  DUR(ARG1);   )
FUNC( gteqDUR    , return  DUR(ARG0)  >= DUR(ARG1);   )
FUNC(   smDUR    , return  DUR(ARG0)  <  DUR(ARG1);   )
FUNC( smeqDUR    , return  DUR(ARG0)  <= DUR(ARG1);   )


// #################################################################################################
// ### Duration
// #################################################################################################
constexpr Calculus::BinaryOpTableEntry  binaryOpTable[] =
{
    { ASTR("+") , Types::DateTime , Types::Duration , CALCULUS_CALLBACK( add_DTDUR ), Types::DateTime ,Calculus::CTI },
    { ASTR("+") , Types::Duration , Types::DateTime , CALCULUS_CALLBACK( add_DURDT ), Types::DateTime ,Calculus::CTI },
    { ASTR("-") , Types::DateTime , Types::Duration , CALCULUS_CALLBACK( sub_DTDUR ), Types::DateTime ,Calculus::CTI },
    { ASTR("-") , Types::DateTime , Types::DateTime , CALCULUS_CALLBACK( sub_DTDT  ), Types::Duration ,Calculus::CTI },
    { ASTR("=="), Types::DateTime , Types::DateTime , CALCULUS_CALLBACK(   eqDT    ), Types::Boolean  ,Calculus::CTI },
    { ASTR("!="), Types::DateTime , Types::DateTime , CALCULUS_CALLBACK(  neqDT    ), Types::Boolean  ,Calculus::CTI },
    { ASTR(">") , Types::DateTime , Types::DateTime , CALCULUS_CALLBACK(   gtDT    ), Types::Boolean  ,Calculus::CTI },
    { ASTR(">="), Types::DateTime , Types::DateTime , CALCULUS_CALLBACK( gteqDT    ), Types::Boolean  ,Calculus::CTI },
    { ASTR("<") , Types::DateTime , Types::DateTime , CALCULUS_CALLBACK(   smDT    ), Types::Boolean  ,Calculus::CTI },
    { ASTR("<="), Types::DateTime , Types::DateTime , CALCULUS_CALLBACK( smeqDT    ), Types::Boolean  ,Calculus::CTI },

    { ASTR("+") , Types::Duration , Types::Duration , CALCULUS_CALLBACK( add_DURDUR), Types::Duration ,Calculus::CTI },
    { ASTR("-") , Types::Duration , Types::Duration , CALCULUS_CALLBACK( sub_DURDUR), Types::Duration ,Calculus::CTI },
    { ASTR("*") , Types::Duration , Types::Float    , CALCULUS_CALLBACK( mul_DURF  ), Types::Duration ,Calculus::CTI },
    { ASTR("*") , Types::Float    , Types::Duration , CALCULUS_CALLBACK( mul_FDUR  ), Types::Duration ,Calculus::CTI },
    { ASTR("*") , Types::Duration , Types::Integer  , CALCULUS_CALLBACK( mul_DURI  ), Types::Duration ,Calculus::CTI },
    { ASTR("*") , Types::Integer  , Types::Duration , CALCULUS_CALLBACK( mul_IDUR  ), Types::Duration ,Calculus::CTI },
    { ASTR("/") , Types::Duration , Types::Float    , CALCULUS_CALLBACK( div_DURF  ), Types::Duration ,Calculus::CTI },
    { ASTR("/") , Types::Duration , Types::Integer  , CALCULUS_CALLBACK( div_DURI  ), Types::Duration ,Calculus::CTI },

    { ASTR("=="), Types::Duration , Types::Duration , CALCULUS_CALLBACK(   eqDUR   ), Types::Boolean  ,Calculus::CTI },
    { ASTR("!="), Types::Duration , Types::Duration , CALCULUS_CALLBACK(  neqDUR   ), Types::Boolean  ,Calculus::CTI },
    { ASTR(">") , Types::Duration , Types::Duration , CALCULUS_CALLBACK(   gtDUR   ), Types::Boolean  ,Calculus::CTI },
    { ASTR(">="), Types::Duration , Types::Duration , CALCULUS_CALLBACK( gteqDUR   ), Types::Boolean  ,Calculus::CTI },
    { ASTR("<") , Types::Duration , Types::Duration , CALCULUS_CALLBACK(   smDUR   ), Types::Boolean  ,Calculus::CTI },
    { ASTR("<="), Types::Duration , Types::Duration , CALCULUS_CALLBACK( smeqDUR   ), Types::Boolean  ,Calculus::CTI },
};

}; // anonymous namespace


// #################################################################################################
// ### DateAndTime - Constructor. Creates the hash map
// #################################################################################################
void  DateAndTime::Init()
{
//! [DOX_ALIB_EXPR_IToLiteral_2]
// register ToLiteral interface for class DateTime::Duration with boxing
boxing::DefineInterface<time::DateTime::Duration, false, IToLiteral_Duration>();
//! [DOX_ALIB_EXPR_IToLiteral_2]
boxing::DefineInterface<time::DateTime, false, IToLiteral_DateTime>();
}

DateAndTime::DateAndTime( Compiler& compiler )
: Calculus( "ALib DateAndTime", compiler )
{
    // set signature vectors
    SIG_DT       = { Types::DateTime                  };
    SIG_DUR      = { Types::Duration                  };
    SIG_DT_DUR   = { Types::DateTime, Types::Duration };
    SIG_INT      = { Types::Integer                   };
    SIG_INT_VAR  = { Types::Integer , nullptr         };
    SIG_FLT      = { Types::Float                     };

    // load identifier/function names from resources
    FunctionNameDescriptor functionNames[66];
    LoadResourcedFunctionDescriptors( lib::EXPRESSIONS, ASTR("DateAndTime"), functionNames);
    FunctionNameDescriptor* descriptor= functionNames;

    // Constant identifiers
    ConstantIdentifiers=
    {
        // January to december
        { *descriptor++,  1 },  { *descriptor++,  2 },  { *descriptor++,  3 },  { *descriptor++,  4 },
        { *descriptor++,  5 },  { *descriptor++,  6 },  { *descriptor++,  7 },  { *descriptor++,  8 },
        { *descriptor++,  9 },  { *descriptor++, 10 },  { *descriptor++, 11 },  { *descriptor++, 12 },

        // Sunday to saturday
        { *descriptor++,  0 },  { *descriptor++,  1 },  { *descriptor++,  2 },  { *descriptor++,  3 },
        { *descriptor++,  4 },  { *descriptor++,  5 },  { *descriptor++,  6 },
    };


    // functions
    Functions=
    {
//! [DOX_ALIB_EXPR_IToLiteral_1]
        { *descriptor++, &SIG_INT    , CALCULUS_CALLBACK(nanosecondsInt  ), Types::Duration , CTI },
//! [DOX_ALIB_EXPR_IToLiteral_1]
        { *descriptor  , &SIG_INT    , CALCULUS_CALLBACK(microsecondsInt ), Types::Duration , CTI },
        { *descriptor++, &SIG_FLT    , CALCULUS_CALLBACK(microsecondsFlt ), Types::Duration , CTI },
        { *descriptor  , &SIG_INT    , CALCULUS_CALLBACK(millisecondsInt ), Types::Duration , CTI },
        { *descriptor++, &SIG_FLT    , CALCULUS_CALLBACK(millisecondsFlt ), Types::Duration , CTI },
        { *descriptor  , &SIG_INT    , CALCULUS_CALLBACK(secondsInt      ), Types::Duration , CTI },
        { *descriptor++, &SIG_FLT    , CALCULUS_CALLBACK(secondsFlt      ), Types::Duration , CTI },
        { *descriptor  , &SIG_INT    , CALCULUS_CALLBACK(minutesInt      ), Types::Duration , CTI },
        { *descriptor++, &SIG_FLT    , CALCULUS_CALLBACK(minutesFlt      ), Types::Duration , CTI },
        { *descriptor  , &SIG_INT    , CALCULUS_CALLBACK(hoursInt        ), Types::Duration , CTI },
        { *descriptor++, &SIG_FLT    , CALCULUS_CALLBACK(hoursFlt        ), Types::Duration , CTI },
        { *descriptor  , &SIG_INT    , CALCULUS_CALLBACK(daysInt         ), Types::Duration , CTI },
        { *descriptor++, &SIG_FLT    , CALCULUS_CALLBACK(daysFlt         ), Types::Duration , CTI },
        { *descriptor  , &SIG_INT    , CALCULUS_CALLBACK(weeksInt        ), Types::Duration , CTI },
        { *descriptor++, &SIG_FLT    , CALCULUS_CALLBACK(weeksFlt        ), Types::Duration , CTI },
        { *descriptor  , &SIG_INT    , CALCULUS_CALLBACK(monthsInt       ), Types::Duration , CTI },
        { *descriptor++, &SIG_FLT    , CALCULUS_CALLBACK(monthsFlt       ), Types::Duration , CTI },
        { *descriptor  , &SIG_INT    , CALCULUS_CALLBACK(yearsInt        ), Types::Duration , CTI },
        { *descriptor++, &SIG_FLT    , CALCULUS_CALLBACK(yearsFlt        ), Types::Duration , CTI },

        { *descriptor++, &SIG_DUR    , CALCULUS_CALLBACK(inDays          ), Types::Float    , CTI },
        { *descriptor++, &SIG_DUR    , CALCULUS_CALLBACK(inHours         ), Types::Float    , CTI },
        { *descriptor++, &SIG_DUR    , CALCULUS_CALLBACK(inMinutes       ), Types::Float    , CTI },
        { *descriptor++, &SIG_DUR    , CALCULUS_CALLBACK(inSeconds       ), Types::Float    , CTI },
        { *descriptor++, &SIG_DUR    , CALCULUS_CALLBACK(inMilliseconds  ), Types::Float    , CTI },
        { *descriptor++, &SIG_DUR    , CALCULUS_CALLBACK(inMicroseconds  ), Types::Float    , CTI },
        { *descriptor++, &SIG_DUR    , CALCULUS_CALLBACK(inNanoseconds   ), Types::Float    , CTI },
        { *descriptor++, &SIG_DUR    , CALCULUS_CALLBACK(inHertz         ), Types::Float    , CTI },
        { *descriptor++, &SIG_INT_VAR, CALCULUS_CALLBACK(dateTime       ), Types::DateTime , CTI },
        { *descriptor++, &SIG_INT_VAR, CALCULUS_CALLBACK(utcDateTime    ), Types::DateTime , CTI },
        { *descriptor++, &SIG_NONE   , CALCULUS_CALLBACK(now             ), Types::DateTime , ETI },
        { *descriptor++, &SIG_NONE   , CALCULUS_CALLBACK(today           ), Types::DateTime , ETI },
        { *descriptor++, &SIG_NONE   , CALCULUS_CALLBACK(utcToday        ), Types::DateTime , ETI },
        { *descriptor++, &SIG_DT     , CALCULUS_CALLBACK(age             ), Types::Duration , ETI },
        { *descriptor++, &SIG_DT_DUR , CALCULUS_CALLBACK(isOlderThan     ), Types::Boolean  , ETI },
        { *descriptor++, &SIG_DT     , CALCULUS_CALLBACK(year            ), Types::Integer  , CTI },
        { *descriptor++, &SIG_DT     , CALCULUS_CALLBACK(month           ), Types::Integer  , CTI },
        { *descriptor++, &SIG_DT     , CALCULUS_CALLBACK(day             ), Types::Integer  , CTI },
        { *descriptor++, &SIG_DT     , CALCULUS_CALLBACK(dayOfWeek       ), Types::Integer  , CTI },
        { *descriptor++, &SIG_DT     , CALCULUS_CALLBACK(hour            ), Types::Integer  , CTI },
        { *descriptor++, &SIG_DT     , CALCULUS_CALLBACK(minute          ), Types::Integer  , CTI },
        { *descriptor++, &SIG_DT     , CALCULUS_CALLBACK(millisecond     ), Types::Integer  , CTI },
        { *descriptor++, &SIG_DT     , CALCULUS_CALLBACK(utcYear         ), Types::Integer  , CTI },
        { *descriptor++, &SIG_DT     , CALCULUS_CALLBACK(utcMonth        ), Types::Integer  , CTI },
        { *descriptor++, &SIG_DT     , CALCULUS_CALLBACK(utcDay          ), Types::Integer  , CTI },
        { *descriptor++, &SIG_DT     , CALCULUS_CALLBACK(utcDayOfWeek    ), Types::Integer  , CTI },
        { *descriptor++, &SIG_DT     , CALCULUS_CALLBACK(utcHour         ), Types::Integer  , CTI },
        { *descriptor++, &SIG_DT     , CALCULUS_CALLBACK(utcMinute       ), Types::Integer  , CTI },
        { *descriptor++, &SIG_DT     , CALCULUS_CALLBACK(utcMillisecond  ), Types::Integer  , CTI },
    };

    // binary operators
    AddBinaryOps( binaryOpTable );
};


}}}} // namespace [aworx::lib::expressions::detail]

#undef BOL
#undef INT
#undef FLT
#undef FUNC
#undef FUNC
#undef UN_MAP_ENTRY
#undef BIN_MAP_ENTRY
#undef BIN_ALIAS_ENTRY


//! @endcond
