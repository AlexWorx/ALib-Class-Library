// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)
#if !defined (HPP_ALIB_EXPRESSIONS_PLUGINS_DATEANDTIME)
#   include "alib/expressions/plugins/dateandtime.hpp"
#endif
#endif // !defined(ALIB_DOX)

#if ALIB_CAMP

#if !defined(ALIB_DOX)
#if !defined (HPP_ALIB_CAMP_CALENDAR)
#   include "alib/lang/system/calendar.hpp"
#endif
#endif // !defined(ALIB_DOX)

//! @cond NO_DOX

#define ARG0           (*args)
#define ARG1           (*(args+1))
#define INT(box)       (box).Unbox<integer           >()
#define FLT(box)       (box).Unbox<double            >()
#define DT(box)        (box).Unbox<DateTime          >()
#define DUR(box)       (box).Unbox<DateTime::Duration>()
#define FUNC( name,...) Box name( Scope& scope,                                                 \
                                  ArgIterator  args,                                            \
                                  ArgIterator  end    )                                         \
                                  { (void) scope; (void) args; (void) end; __VA_ARGS__ }

#if !ALIB_FEAT_BOXING_BIJECTIVE_INTEGRALS
#   define TOINT(arg)                      arg
#else
#   define TOINT(arg) static_cast<integer>(arg)
#endif

namespace alib {  namespace expressions { namespace plugins {


namespace {

// #################################################################################################
// ### Reverse generation: convert program constants to expression strings
// #################################################################################################
DOX_MARKER([DOX_ALIB_EXPR_FToLiteral_3])
void FToLiteral_Duration( const Box& constantValue, AString& expressionString )
{
    // Unbox the time span and convert to nanoseconds
    auto value= constantValue.Unbox<DateTime::Duration>().InNanoseconds();

    // Find the best matching magnitude
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

    // wWite the function argument
    expressionString << result << '(' << value << ')' ;
}
DOX_MARKER([DOX_ALIB_EXPR_FToLiteral_3])

void FToLiteral_DateTime( const Box& constantValue, AString& expressionString )
{
    CalendarDateTime ct( constantValue.Unbox<DateTime>(), lang::Timezone::UTC );

    expressionString << "UTCDateTime( " << ct.Year         << ','
                                        << ct.Month        << ','
                                        << ct.Day          << ','
                                        << ct.Hour         << ','
                                        << ct.Minute       << ','
                                        << ct.Second       << ','
                                        << ct.Millisecond  << ')' ;
}


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
                               .Get(lang::Timezone::Local);
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
                               .Get(lang::Timezone::UTC);
           )

FUNC( today           , CalendarDateTime ct(DateTime(), lang::Timezone::Local);
                        ct.Hour= ct.Minute= ct.Second= ct.Millisecond= 0;
                        return ct.Get(lang::Timezone::Local);                     )
FUNC( utcToday        , CalendarDateTime ct(DateTime(), lang::Timezone::UTC);
                        ct.Hour= ct.Minute= ct.Second= ct.Millisecond= 0;
                        return ct.Get(lang::Timezone::UTC);                     )
FUNC( now             , return DateTime();                                 )
FUNC( age             , return DT(ARG0).Age();                            )
FUNC( isOlderThan     , return DT(ARG0).IsOlderThan( DUR(ARG1) );      )
FUNC( year            , return TOINT(CalendarDateTime(DT(ARG0), lang::Timezone::Local).Year       ); )
FUNC( month           , return TOINT(CalendarDateTime(DT(ARG0), lang::Timezone::Local).Month      ); )
FUNC( day             , return TOINT(CalendarDateTime(DT(ARG0), lang::Timezone::Local).Day        ); )
FUNC( dayOfWeek       , return TOINT(CalendarDateTime(DT(ARG0), lang::Timezone::Local).DayOfWeek  ); )
FUNC( hour            , return TOINT(CalendarDateTime(DT(ARG0), lang::Timezone::Local).Hour       ); )
FUNC( minute          , return TOINT(CalendarDateTime(DT(ARG0), lang::Timezone::Local).Minute     ); )
FUNC( millisecond     , return TOINT(CalendarDateTime(DT(ARG0), lang::Timezone::Local).Millisecond); )
FUNC( utcYear         , return TOINT(CalendarDateTime(DT(ARG0), lang::Timezone::UTC  ).Year       ); )
FUNC( utcMonth        , return TOINT(CalendarDateTime(DT(ARG0), lang::Timezone::UTC  ).Month      ); )
FUNC( utcDay          , return TOINT(CalendarDateTime(DT(ARG0), lang::Timezone::UTC  ).Day        ); )
FUNC( utcDayOfWeek    , return TOINT(CalendarDateTime(DT(ARG0), lang::Timezone::UTC  ).DayOfWeek  ); )
FUNC( utcHour         , return TOINT(CalendarDateTime(DT(ARG0), lang::Timezone::UTC  ).Hour       ); )
FUNC( utcMinute       , return TOINT(CalendarDateTime(DT(ARG0), lang::Timezone::UTC  ).Minute     ); )
FUNC( utcMillisecond  , return TOINT(CalendarDateTime(DT(ARG0), lang::Timezone::UTC  ).Millisecond); )


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
FUNC( add_DTDUR , return  DT(ARG0)  +  DUR(ARG1);  )
FUNC( add_DURDT , return  DT(ARG1)  +  DUR(ARG0);  )
FUNC( sub_DTDUR , return  DT(ARG0)  -  DUR(ARG1);  )
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
FUNC( mul_DURI   , return  DUR(ARG0)  *  static_cast<int64_t>( INT(ARG1) );   )
FUNC( mul_IDUR   , return  DUR(ARG1)  *  static_cast<int64_t>( INT(ARG0) );   )
FUNC( div_DURF   , return  DUR(ARG0)  /  FLT(ARG1);   )
FUNC( div_DURI   , return  DUR(ARG0)  /  static_cast<int64_t>( INT(ARG1) );   )

FUNC(   eqDUR    , return  DUR(ARG0)  == DUR(ARG1);   )
FUNC(  neqDUR    , return  DUR(ARG0)  != DUR(ARG1);   )
FUNC(   gtDUR    , return  DUR(ARG0)  >  DUR(ARG1);   )
FUNC( gteqDUR    , return  DUR(ARG0)  >= DUR(ARG1);   )
FUNC(   smDUR    , return  DUR(ARG0)  <  DUR(ARG1);   )
FUNC( smeqDUR    , return  DUR(ARG0)  <= DUR(ARG1);   )


// #################################################################################################
// ### Duration
// #################################################################################################
Calculus::OperatorTableEntry  binaryOpTableDateTime[] =
{
    { A_CHAR("+") , Types::DateTime , Types::Duration , CALCULUS_CALLBACK( add_DTDUR ), Types::DateTime ,Calculus::CTI },
    { A_CHAR("+") , Types::Duration , Types::DateTime , CALCULUS_CALLBACK( add_DURDT ), Types::DateTime ,Calculus::CTI },
    { A_CHAR("-") , Types::DateTime , Types::Duration , CALCULUS_CALLBACK( sub_DTDUR ), Types::DateTime ,Calculus::CTI },
    { A_CHAR("-") , Types::DateTime , Types::DateTime , CALCULUS_CALLBACK( sub_DTDT  ), Types::Duration ,Calculus::CTI },
    { A_CHAR("=="), Types::DateTime , Types::DateTime , CALCULUS_CALLBACK(   eqDT    ), Types::Boolean  ,Calculus::CTI },
    { A_CHAR("!="), Types::DateTime , Types::DateTime , CALCULUS_CALLBACK(  neqDT    ), Types::Boolean  ,Calculus::CTI },
    { A_CHAR(">") , Types::DateTime , Types::DateTime , CALCULUS_CALLBACK(   gtDT    ), Types::Boolean  ,Calculus::CTI },
    { A_CHAR(">="), Types::DateTime , Types::DateTime , CALCULUS_CALLBACK( gteqDT    ), Types::Boolean  ,Calculus::CTI },
    { A_CHAR("<") , Types::DateTime , Types::DateTime , CALCULUS_CALLBACK(   smDT    ), Types::Boolean  ,Calculus::CTI },
    { A_CHAR("<="), Types::DateTime , Types::DateTime , CALCULUS_CALLBACK( smeqDT    ), Types::Boolean  ,Calculus::CTI },

    { A_CHAR("+") , Types::Duration , Types::Duration , CALCULUS_CALLBACK( add_DURDUR), Types::Duration ,Calculus::CTI },
    { A_CHAR("-") , Types::Duration , Types::Duration , CALCULUS_CALLBACK( sub_DURDUR), Types::Duration ,Calculus::CTI },
    { A_CHAR("*") , Types::Duration , Types::Float    , CALCULUS_CALLBACK( mul_DURF  ), Types::Duration ,Calculus::CTI },
    { A_CHAR("*") , Types::Float    , Types::Duration , CALCULUS_CALLBACK( mul_FDUR  ), Types::Duration ,Calculus::CTI },
    { A_CHAR("*") , Types::Duration , Types::Integer  , CALCULUS_CALLBACK( mul_DURI  ), Types::Duration ,Calculus::CTI },
    { A_CHAR("*") , Types::Integer  , Types::Duration , CALCULUS_CALLBACK( mul_IDUR  ), Types::Duration ,Calculus::CTI },
    { A_CHAR("/") , Types::Duration , Types::Float    , CALCULUS_CALLBACK( div_DURF  ), Types::Duration ,Calculus::CTI },
    { A_CHAR("/") , Types::Duration , Types::Integer  , CALCULUS_CALLBACK( div_DURI  ), Types::Duration ,Calculus::CTI },

    { A_CHAR("=="), Types::Duration , Types::Duration , CALCULUS_CALLBACK(   eqDUR   ), Types::Boolean  ,Calculus::CTI },
    { A_CHAR("!="), Types::Duration , Types::Duration , CALCULUS_CALLBACK(  neqDUR   ), Types::Boolean  ,Calculus::CTI },
    { A_CHAR(">") , Types::Duration , Types::Duration , CALCULUS_CALLBACK(   gtDUR   ), Types::Boolean  ,Calculus::CTI },
    { A_CHAR(">="), Types::Duration , Types::Duration , CALCULUS_CALLBACK( gteqDUR   ), Types::Boolean  ,Calculus::CTI },
    { A_CHAR("<") , Types::Duration , Types::Duration , CALCULUS_CALLBACK(   smDUR   ), Types::Boolean  ,Calculus::CTI },
    { A_CHAR("<="), Types::Duration , Types::Duration , CALCULUS_CALLBACK( smeqDUR   ), Types::Boolean  ,Calculus::CTI },
};

} // anonymous namespace


// #################################################################################################
// ### DateAndTime - Constructor. Creates the hash map
// #################################################################################################
void  DateAndTime::Bootstrap()
{
DOX_MARKER([DOX_ALIB_EXPR_FToLiteral_2])
// register ToLiteral interface for class DateTime::Duration with boxing
boxing::BootstrapRegister<FToLiteral, boxing::TMappedTo<time::DateTime::Duration>>( FToLiteral_Duration );
DOX_MARKER([DOX_ALIB_EXPR_FToLiteral_2])
boxing::BootstrapRegister<FToLiteral, boxing::TMappedTo<time::DateTime          >>( FToLiteral_DateTime );
}

DateAndTime::DateAndTime( Compiler& compiler )
: Calculus( "ALib DateAndTime", compiler )
{
    // load identifier/function names from resources
    constexpr int tableSize= 58;
    Token functionNames[tableSize];
    Token::LoadResourcedTokens( EXPRESSIONS, "CPD", functionNames
                                ALIB_DBG(,tableSize)                                     );
    ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
    Token* descriptor= functionNames;

    // Constant identifiers
    ConstantIdentifiers=
    {
        // January to december
        { *descriptor++,  TOINT(1) },  { *descriptor++, TOINT( 2) },  { *descriptor++, TOINT( 3) },  { *descriptor++, TOINT( 4) },
        { *descriptor++,  TOINT(5) },  { *descriptor++, TOINT( 6) },  { *descriptor++, TOINT( 7) },  { *descriptor++, TOINT( 8) },
        { *descriptor++,  TOINT(9) },  { *descriptor++, TOINT(10) },  { *descriptor++, TOINT(11) },  { *descriptor++, TOINT(12) },

        // Sunday to saturday
        { *descriptor++,  TOINT(0) },  { *descriptor++,  TOINT(1) },  { *descriptor++,  TOINT(2) },  { *descriptor++, TOINT( 3) },
        { *descriptor++,  TOINT(4) },  { *descriptor++,  TOINT(5) },  { *descriptor++,  TOINT(6) },
    };

    // functions
    Functions=
    {
DOX_MARKER([DOX_ALIB_EXPR_FToLiteral_1])
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::I   ), CALCULUS_CALLBACK(nanosecondsInt  ), &Types::Duration , CTI },
DOX_MARKER([DOX_ALIB_EXPR_FToLiteral_1])
        { *descriptor  , CALCULUS_SIGNATURE(Signatures::I   ), CALCULUS_CALLBACK(microsecondsInt ), &Types::Duration , CTI },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::F   ), CALCULUS_CALLBACK(microsecondsFlt ), &Types::Duration , CTI },
        { *descriptor  , CALCULUS_SIGNATURE(Signatures::I   ), CALCULUS_CALLBACK(millisecondsInt ), &Types::Duration , CTI },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::F   ), CALCULUS_CALLBACK(millisecondsFlt ), &Types::Duration , CTI },
        { *descriptor  , CALCULUS_SIGNATURE(Signatures::I   ), CALCULUS_CALLBACK(secondsInt      ), &Types::Duration , CTI },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::F   ), CALCULUS_CALLBACK(secondsFlt      ), &Types::Duration , CTI },
        { *descriptor  , CALCULUS_SIGNATURE(Signatures::I   ), CALCULUS_CALLBACK(minutesInt      ), &Types::Duration , CTI },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::F   ), CALCULUS_CALLBACK(minutesFlt      ), &Types::Duration , CTI },
        { *descriptor  , CALCULUS_SIGNATURE(Signatures::I   ), CALCULUS_CALLBACK(hoursInt        ), &Types::Duration , CTI },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::F   ), CALCULUS_CALLBACK(hoursFlt        ), &Types::Duration , CTI },
        { *descriptor  , CALCULUS_SIGNATURE(Signatures::I   ), CALCULUS_CALLBACK(daysInt         ), &Types::Duration , CTI },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::F   ), CALCULUS_CALLBACK(daysFlt         ), &Types::Duration , CTI },
        { *descriptor  , CALCULUS_SIGNATURE(Signatures::I   ), CALCULUS_CALLBACK(weeksInt        ), &Types::Duration , CTI },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::F   ), CALCULUS_CALLBACK(weeksFlt        ), &Types::Duration , CTI },
        { *descriptor  , CALCULUS_SIGNATURE(Signatures::I   ), CALCULUS_CALLBACK(monthsInt       ), &Types::Duration , CTI },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::F   ), CALCULUS_CALLBACK(monthsFlt       ), &Types::Duration , CTI },
        { *descriptor  , CALCULUS_SIGNATURE(Signatures::I   ), CALCULUS_CALLBACK(yearsInt        ), &Types::Duration , CTI },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::F   ), CALCULUS_CALLBACK(yearsFlt        ), &Types::Duration , CTI },

        { *descriptor++, CALCULUS_SIGNATURE(Signatures::Dur ), CALCULUS_CALLBACK(inDays          ), &Types::Float    , CTI },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::Dur ), CALCULUS_CALLBACK(inHours         ), &Types::Float    , CTI },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::Dur ), CALCULUS_CALLBACK(inMinutes       ), &Types::Float    , CTI },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::Dur ), CALCULUS_CALLBACK(inSeconds       ), &Types::Float    , CTI },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::Dur ), CALCULUS_CALLBACK(inMilliseconds  ), &Types::Float    , CTI },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::Dur ), CALCULUS_CALLBACK(inMicroseconds  ), &Types::Float    , CTI },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::Dur ), CALCULUS_CALLBACK(inNanoseconds   ), &Types::Float    , CTI },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::Dur ), CALCULUS_CALLBACK(inHertz         ), &Types::Float    , CTI },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::IVar), CALCULUS_CALLBACK(dateTime        ), &Types::DateTime , CTI },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::IVar), CALCULUS_CALLBACK(utcDateTime     ), &Types::DateTime , CTI },
        { *descriptor++, CALCULUS_SIGNATURE(nullptr         ), CALCULUS_CALLBACK(now             ), &Types::DateTime , ETI },
        { *descriptor++, CALCULUS_SIGNATURE(nullptr         ), CALCULUS_CALLBACK(today           ), &Types::DateTime , ETI },
        { *descriptor++, CALCULUS_SIGNATURE(nullptr         ), CALCULUS_CALLBACK(utcToday        ), &Types::DateTime , ETI },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::D   ), CALCULUS_CALLBACK(age             ), &Types::Duration , ETI },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::DDur), CALCULUS_CALLBACK(isOlderThan     ), &Types::Boolean  , ETI },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::D   ), CALCULUS_CALLBACK(year            ), &Types::Integer  , CTI },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::D   ), CALCULUS_CALLBACK(month           ), &Types::Integer  , CTI },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::D   ), CALCULUS_CALLBACK(day             ), &Types::Integer  , CTI },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::D   ), CALCULUS_CALLBACK(dayOfWeek       ), &Types::Integer  , CTI },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::D   ), CALCULUS_CALLBACK(hour            ), &Types::Integer  , CTI },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::D   ), CALCULUS_CALLBACK(minute          ), &Types::Integer  , CTI },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::D   ), CALCULUS_CALLBACK(millisecond     ), &Types::Integer  , CTI },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::D   ), CALCULUS_CALLBACK(utcYear         ), &Types::Integer  , CTI },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::D   ), CALCULUS_CALLBACK(utcMonth        ), &Types::Integer  , CTI },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::D   ), CALCULUS_CALLBACK(utcDay          ), &Types::Integer  , CTI },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::D   ), CALCULUS_CALLBACK(utcDayOfWeek    ), &Types::Integer  , CTI },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::D   ), CALCULUS_CALLBACK(utcHour         ), &Types::Integer  , CTI },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::D   ), CALCULUS_CALLBACK(utcMinute       ), &Types::Integer  , CTI },
        { *descriptor++, CALCULUS_SIGNATURE(Signatures::D   ), CALCULUS_CALLBACK(utcMillisecond  ), &Types::Integer  , CTI },
    };

    // binary operators
    AddOperators( binaryOpTableDateTime );

    ALIB_ASSERT_ERROR( descriptor - functionNames == tableSize, "EXPR",
                       "Descriptor table size mismatch: Consumed {} descriptors, {} available.",
                       descriptor - functionNames, tableSize  )
    ALIB_WARNINGS_RESTORE
/**/}


}}} // namespace [alib::expressions::detail]

#undef BOL
#undef INT
#undef FLT
#undef FUNC
#undef FUNC
#undef UN_MAP_ENTRY
#undef BIN_MAP_ENTRY
#undef BIN_ALIAS_ENTRY

#endif // ALIB_CAMP

//! @endcond
