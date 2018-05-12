// #################################################################################################
//  aworx - Unit Tests
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alox.hpp"
#include "aworx_unittests.hpp"
#include "alib/alox/logtools.hpp"
#include "alib/strings/boxing/debug.hpp"

#include "alib/expressions/expression.hpp"
#include "alib/expressions/detail/parser.hpp"
#include "alib/expressions/compiler.hpp"
#include "alib/expressions/detail/ast.hpp"
#include "alib/expressions/detail/parser_impl.hpp"
#include "alib/expressions/scope.hpp"
#include "alib/expressions/detail/program.hpp"

#if defined(_MSC_VER)
#   define _USE_MATH_DEFINES
#endif
#include <math.h>

#define TESTCLASSNAME       CPP_ALib_Expr_Specials


using namespace std;
using namespace aworx;
using namespace aworx::lib;
using namespace aworx::lib::expressions;
using namespace aworx::lib::expressions::detail;

ALIB_WARNINGS_MACRO_NOT_USED_OFF

namespace ut_aworx {

// #################################################################################################
// ### Test method used by the test below, with two macros. One for expressions understood by
// ### C++, one for those that are not compatible.
// #################################################################################################
#define   CCOMP_CONSTEXPR(expr)               testExpression( ALIB_CALLER, ut, compiler, scope, ALIB_STRINGIFY(expr), expr  , 1 );
#define   CONSTEXPR(expr, result)             testExpression( ALIB_CALLER, ut, compiler, scope, ALIB_STRINGIFY(expr), result, 1 );
#define   EXPRESSION(expr, result,expProgLen) testExpression( ALIB_CALLER, ut, compiler, scope, ALIB_STRINGIFY(expr), result, expProgLen );

extern
SPExpression testExpression( const NTString& file, int line, const NTString& func,
                             AWorxUnitTesting&     ut,
                             Compiler&             compiler,
                             expressions::Scope&   scope,
                             const String&         expressionString,
                             const Box&            expected,
                             integer               programLength                               );

#if !ALIB_DEBUG
    #define      PRINTPRGRM(expr)
#else
    #define      PRINTPRGRM(expr) printProgram( ALIB_CALLER, ut, compiler, ALIB_STRINGIFY(expr) );

extern
SPExpression printProgram( const NTString& file, int line, const NTString& func,
                           AWorxUnitTesting&  ut,
                           Compiler&          compiler,
                           const String&      expressionString         );
#endif



// #################################################################################################
// ### Test Class
// #################################################################################################

UT_CLASS()

// #################################################################################################
// #### Math
// #################################################################################################
UT_METHOD(Math)
{
    UT_INIT();

    Compiler compiler;
    compiler.SetupDefaults();
    SPExpression expression;

    lib::expressions::Scope scope(compiler.CfgFormatter);

    CONSTEXPR( PI                      , M_PI            );
    CONSTEXPR( PI*2.0                  , M_PI *2.0       );
    CONSTEXPR( E                       , M_E             );
    CONSTEXPR( sin(1.0)                , sin(1.0)        );
    CONSTEXPR( sin(PI)                 , 0.0             );
    CONSTEXPR( sin(PI/2)               , sin(M_PI/2)     );
    CONSTEXPR( cos(PI)                 , -1.0            );
    CONSTEXPR( cos(PI/2)               , cos(M_PI/2)     );
    CONSTEXPR( tan(PI)                 , tan(M_PI)       );
    CONSTEXPR( tan(PI/5)               , tan(M_PI/5)     );
    CONSTEXPR( asin(sin(PI/5))         , M_PI/5          );
    CONSTEXPR( acos(cos(PI/5))         , M_PI/5          );
    CONSTEXPR( atan(tan(PI/5))         , M_PI/5          );
    CONSTEXPR( asinh(sinh(PI/5))       , M_PI/5          );
    CONSTEXPR( acosh(cosh(PI/5))       , M_PI/5          );
    CONSTEXPR( atanh(tanh(PI/5))       , M_PI/5          );
    CONSTEXPR( exp(1.0)                , M_E             );
    CONSTEXPR( exp2(2.0)               , 4.0             );
    CONSTEXPR( log(E)                  , 1.0             );
    CONSTEXPR( log2(2.0)               , 1.0             );
    CONSTEXPR( log10(100.0)            , 2.0             );
    CONSTEXPR( exp10(2.0)              , 100.0           );
    CONSTEXPR( sqrt(100.0)             , 10.0            );
    CONSTEXPR( cbrt(1000.0)            , 10.0            );
    CONSTEXPR( abs(-3)                 , 3               );
    CONSTEXPR( abs(PI)                 , M_PI            );
    CONSTEXPR( abs(-PI)                , M_PI            );
    CONSTEXPR( abs(-2)                 , 2               );
    CONSTEXPR( floor(PI)               , 3.0             );
    CONSTEXPR( floor(-PI)              , -4.0            );
    CONSTEXPR( ceil(PI)                , 4.0             );
    CONSTEXPR( ceil(-PI)               , -3.0            );
    CONSTEXPR( trunc(PI)               , 3.0             );
    CONSTEXPR( trunc(-PI)              , -3.0            );
    CONSTEXPR( round(PI)               , 3.0             );
    CONSTEXPR( round(-PI)              , -3.0            );
    CONSTEXPR( round(E)                , 3.0             );
    CONSTEXPR( round(-E)               , -3.0            );
    CONSTEXPR( rint(E)                 , 3               );
    CONSTEXPR( remainder(E,2.0)        , M_E - 2.0       );
    CONSTEXPR( max(PI,E)               , M_PI            );
    CONSTEXPR( min(PI,E)               , M_E             );
    CONSTEXPR( max(3,4)                , 4               );
    CONSTEXPR( min(3,4)                , 3               );
}

// #################################################################################################
// #### DateTime
// #################################################################################################
UT_METHOD(DateTime)
{
    UT_INIT();

    Compiler compiler;
    compiler.SetupDefaults();
    SPExpression expression;

    lib::expressions::Scope scope(compiler.CfgFormatter);

    // DateTime::Duration construction functions
#if !defined(_WIN32) // not accurate enough
    CONSTEXPR( nanoseconds(0)                    , DateTime::Duration()                       );
    CONSTEXPR( nanoseconds(83)                   , DateTime::Duration::FromNanoseconds(83)    );
    CONSTEXPR( nanoseconds(83)* 100              , DateTime::Duration::FromNanoseconds(8300)  );
    CONSTEXPR( 1000 * nanoseconds(83)            , DateTime::Duration::FromMicroseconds(83)   );
#endif
    CONSTEXPR( microseconds(10) + milliseconds(5), DateTime::Duration::FromMicroseconds(5010) );

    CONSTEXPR( milliseconds(5)                   , DateTime::Duration::FromAbsoluteMilliseconds(5)    );
    CONSTEXPR( seconds(5)                        , DateTime::Duration::FromAbsoluteSeconds(5)         );
    CONSTEXPR( seconds(5)  + Millis(3)           , DateTime::Duration::FromAbsoluteMilliseconds(5003) );
    CONSTEXPR( minute(5)                         , DateTime::Duration::FromAbsoluteMinutes(5)         );
    CONSTEXPR( minutes(5)  + seconds(3)          , DateTime::Duration::FromAbsoluteSeconds(303)       );
    CONSTEXPR( hours(5)                          , DateTime::Duration::FromAbsoluteHours(5)           );
    CONSTEXPR( Hour(1)                           , DateTime::Duration::FromAbsoluteSeconds(3600)      );
    CONSTEXPR( Hour(1)     + seconds(3)          , DateTime::Duration::FromAbsoluteSeconds(3603)      );
    CONSTEXPR( Hour(5)     + Minutes(3)          , DateTime::Duration::FromAbsoluteMinutes(303)       );
    CONSTEXPR( Days(5)                           , DateTime::Duration::FromAbsoluteDays(5)            );
    CONSTEXPR( Days(1)     + Hour(3)             , DateTime::Duration::FromAbsoluteHours(27)          );
    CONSTEXPR( Months(5)                         , DateTime::Duration::FromAbsoluteDays(150)          );
    CONSTEXPR( Years(1)                          , DateTime::Duration::FromAbsoluteDays(365)          );
    CONSTEXPR( Years(2)                          , DateTime::Duration::FromAbsoluteDays(730)          );


    CONSTEXPR( microseconds(5.2)                 , DateTime::Duration::FromMicroseconds(5.2)    );
    CONSTEXPR( milliseconds(5.2)                 , DateTime::Duration::FromMilliseconds(5.2)    );
    CONSTEXPR( seconds(5.2)                      , DateTime::Duration::FromSeconds(5.2)         );
    CONSTEXPR( minute(5.2)                       , DateTime::Duration::FromMinutes(5.2)         );
    CONSTEXPR( hours(5.2)                        , DateTime::Duration::FromHours(5.2)           );
    CONSTEXPR( Days(5.2)                         , DateTime::Duration::FromDays(5.2)            );
    CONSTEXPR( Months(5.2)                       , DateTime::Duration::FromDays(30.0 * 5.2)     );


    // DateTime::Duration calculus
    CONSTEXPR( milliseconds(100) *2              , DateTime::Duration::FromMilliseconds(200)    );
    CONSTEXPR( 3 * milliseconds(100)             , DateTime::Duration::FromMilliseconds(300)    );
    CONSTEXPR( milliseconds(100) *2.5            , DateTime::Duration::FromMilliseconds(250)    );
    CONSTEXPR( 3.5 * milliseconds(100)           , DateTime::Duration::FromMilliseconds(350)    );

    CONSTEXPR( milliseconds(100) / 2             , DateTime::Duration::FromMilliseconds(50 )    );
    CONSTEXPR( milliseconds(100) / 3.333333333333, DateTime::Duration::FromMilliseconds(30 )    );

    CONSTEXPR( InHertz(milliseconds(200))        , 5.0    );

    CONSTEXPR( milliseconds(1) == milliseconds(1)    , true    );
    CONSTEXPR( milliseconds(1) == milliseconds(2)    , false   );
    CONSTEXPR( milliseconds(1) != milliseconds(1)    , false   );
    CONSTEXPR( milliseconds(1) != milliseconds(2)    , true    );
    CONSTEXPR( milliseconds(1) <  milliseconds(2)    , true    );
    CONSTEXPR( milliseconds(1) <  milliseconds(1)    , false   );
    CONSTEXPR( milliseconds(1) <= milliseconds(2)    , true    );
    CONSTEXPR( milliseconds(1) <= milliseconds(1)    , true    );
    CONSTEXPR( milliseconds(2) <= milliseconds(1)    , false   );
    CONSTEXPR( milliseconds(2) >  milliseconds(1)    , true    );
    CONSTEXPR( milliseconds(1) >  milliseconds(1)    , false   );
    CONSTEXPR( milliseconds(2) >= milliseconds(1)    , true    );
    CONSTEXPR( milliseconds(1) >= milliseconds(1)    , true    );
    CONSTEXPR( milliseconds(1) >= milliseconds(2)    , false   );

    // DateTime
    EXPRESSION( Age(Now)  < milliseconds(1)          , true    , 4);
    EXPRESSION( IsOlderThan(Now, milliseconds(1) )   , false   , 3);
    EXPRESSION( IsOlderThan(Now, nanoseconds(0) )    , true    , 3);
    EXPRESSION( now == now                           , false   , 3);
    EXPRESSION( now != now                           , true    , 3);
    EXPRESSION( now <  now                           , true    , 3);
    EXPRESSION( now <= now                           , true    , 3);
    EXPRESSION( now >  now                           , false   , 3);
    EXPRESSION( now >= now                           , false   , 3);

    CONSTEXPR( UTCDateTime(2018,1,31,14,5) , CalendarDateTime(2018,1,31,14,5).Get(Timezone::UTC) );
    CONSTEXPR(    DateTime(2018,1,31,14,5) , CalendarDateTime(2018,1,31,14,5).Get() );

    CONSTEXPR( GetYear       (DateTime(2018,1,31,14,5)) , 2018 );
    CONSTEXPR( GetMonth      (DateTime(2018,1,31,14,5)) , 1    );
    CONSTEXPR( GetDay        (DateTime(2018,1,31,14,5)) , 31   );
    CONSTEXPR( GetDayOfWeek  (DateTime(2018,1,31,14,5)) , 3    );
    CONSTEXPR( GetDayO       (DateTime(2018,1,31,14,5)) , 3    );
    CONSTEXPR( GetHour       (DateTime(2018,1,31,14,5)) , 14   );
    CONSTEXPR( GetMinute     (DateTime(2018,1,31,14,5)) , 5    );
    CONSTEXPR( GetMin        (DateTime(2018,1,31,14,5)) , 5    );
    CONSTEXPR( GetMillisecond(DateTime(2018,1,31,14,5)) , 0    );
    CONSTEXPR( GetMilli      (DateTime(2018,1,31,14,5)) , 0    );

    CONSTEXPR( GetUTCYear       (UTCDateTime(2018,1,31,14,5)) , 2018 );
    CONSTEXPR( GetUTCMonth      (UTCDateTime(2018,1,31,14,5)) , 1    );
    CONSTEXPR( GetUTCDay        (UTCDateTime(2018,1,31,14,5)) , 31   );
    CONSTEXPR( GetUTCDayOfWeek  (UTCDateTime(2018,1,31,14,5)) , 3    );
    CONSTEXPR( GetUTCDayO       (UTCDateTime(2018,1,31,14,5)) , 3    );
    CONSTEXPR( GetUTCHour       (UTCDateTime(2018,1,31,14,5)) , 14   );
    CONSTEXPR( GetUTCMinute     (UTCDateTime(2018,1,31,14,5)) , 5    );
    CONSTEXPR( GetUTCMin        (UTCDateTime(2018,1,31,14,5)) , 5    );
    CONSTEXPR( GetUTCMillisecond(UTCDateTime(2018,1,31,14,5)) , 0    );
    CONSTEXPR( GetUTCMilli      (UTCDateTime(2018,1,31,14,5)) , 0    );

    EXPRESSION( now < now + seconds(1)                    , true  , 5 );
    EXPRESSION( now < now - seconds(1)                    , false , 5 );
    EXPRESSION( now < seconds(1) + now                    , true  , 5 );
    EXPRESSION( now - DateTime(1969) > years(47)         , true  , 5 );
    EXPRESSION( inDays( now - DateTime(1969)) /365 >49   , true  , 8 );
    EXPRESSION( today    <= now                           , true  , 3 );
    EXPRESSION( UTCToday <= now                           , true  , 3 );


#if !ALIB_FEAT_EXPRESSIONS_SPIRIT_PARSER

    EXPRESSION(    GetMonth(DateTime(2018, 1)) == January
                && GetMonth(DateTime(2018, 2)) == Feb
                && GetMonth(DateTime(2018, 3)) == Mar
                && GetMonth(DateTime(2018, 4)) == Apr
                && GetMonth(DateTime(2018, 5)) == May
                && GetMonth(DateTime(2018, 6)) == Jun
                && GetMonth(DateTime(2018, 7)) == Jul
                && GetMonth(DateTime(2018, 8)) == Aug
                && GetMonth(DateTime(2018, 9)) == Sept
                && GetMonth(DateTime(2018,10)) == Oct
                && GetMonth(DateTime(2018,11)) == Novem
                && GetMonth(DateTime(2018,12)) == Dec             , true, 1 )

//! [DOX_ALIB_EXPR_BOOST_LONG]
    EXPRESSION(    GetDayOfWeek(DateTime(2018, 2, 12)) == Monday
                && GetDayOfWeek(DateTime(2018, 2, 13)) == Tue
                && GetDayOfWeek(DateTime(2018, 2, 14)) == Wed
                && GetDayOfWeek(DateTime(2018, 2, 15)) == thu
                && GetDayOfWeek(DateTime(2018, 2, 16)) == fri
                && GetDayOfWeek(DateTime(2018, 2, 17)) == sat
                && GetDayOfWeek(DateTime(2018, 2, 18)) == sun    , true , 1);
//! [DOX_ALIB_EXPR_BOOST_LONG]

#endif

}

// #################################################################################################
// #### StringMatch
// #################################################################################################
UT_METHOD(Strings)
{
    UT_INIT();

    Compiler compiler;
    compiler.SetupDefaults();
    SPExpression expression;

    lib::expressions::Scope scope(compiler.CfgFormatter);

    // a string!
    CONSTEXPR( "String" , ASTR("String")  );


    // our spirit parser can't parse escape sequences in strings
    #if !ALIB_FEAT_EXPRESSIONS_SPIRIT_PARSER
        EXPRESSION( "a" + tab  +"b"   , ASTR("a\tb") , 1     );
        EXPRESSION( "a" + newl +"b"   , String32("a")<< NewLine << "b" , 1     );
    #endif

    EXPRESSION( (random ? "nonconst" : "nonconst" ) *  "nonc*"                        , true  , 7     );
    EXPRESSION( (random ? "nonconst" : "nonconst" ) * (random ? "nonc*" : "nonc*" )   , true  , 11    );
    EXPRESSION( "Constant" * (random ? "onst*" : "onst*" )                            , false , 7     );
    EXPRESSION( "Constant" * (random ? "Const*" : "Const*" )                          , true  , 7     );
    EXPRESSION( "Constant" * "Const*"                                                 , true  , 1     );


    EXPRESSION( Compare("String", "String"       )           , 0      , 1     );
    EXPRESSION( Compare("aaa"   , "bbb"          ) <0        , true   , 1     );
    EXPRESSION( Compare("bbb"   , "aaa"          ) >0        , true   , 1     );
    EXPRESSION( Compare("String", "STRING"       ) >0        , true   , 1     );
    EXPRESSION( Compare("String", "STRING", false) >0        , true   , 1     );
    EXPRESSION( Compare("String", "STRING", true )           , 0      , 1     );

    EXPRESSION(   "aaa"  <  "aaa"          , false  , 1     );
    EXPRESSION(   "aaa"  <  "bbb"          , true   , 1     );
    EXPRESSION(   "aaa"  <= "bbb"          , true   , 1     );

    EXPRESSION(   "aaa"  >  "aaa"          , false  , 1     );
    EXPRESSION(   "aaa"  >  "bbb"          , false  , 1     );
    EXPRESSION(   "aaa"  >= "bbb"          , false  , 1     );


    EXPRESSION( StartsWith("String", "Str")           , true   , 1     );
    EXPRESSION( StartsWith("String", "str")           , false  , 1     );
    EXPRESSION( StartsWith("String", "str", false)    , false  , 1     );
    EXPRESSION( StartsWith("String", "str", true)     , true   , 1     );

    EXPRESSION( EndsWith("String", "ing")             , true   , 1     );
    EXPRESSION( EndsWith("String", "ING")             , false  , 1     );
    EXPRESSION( EndsWith("String", "ING", false)      , false  , 1     );
    EXPRESSION( EndsWith("String", "ING", true )      , true   , 1     );

    EXPRESSION( len("String")      , 6       , 1     );
    EXPRESSION( len("S")           , 1       , 1     );
    EXPRESSION( length("")         , 0       , 1     );


    CONSTEXPR( "String"[0]    , ASTR("S")  );
    CONSTEXPR( "String"[1]    , ASTR("t")  );
    CONSTEXPR( "String"[2]    , ASTR("r")  );
    CONSTEXPR( "String"[-1]   , ASTR("")   );
    CONSTEXPR( "String"[10]   , ASTR("")   );

    EXPRESSION( Substring("String", 3    ) , ASTR("ing"), 1  );
    EXPRESSION( Substring("String", 3,  2) , ASTR("in") , 1  );
    EXPRESSION( Substring("String", -3, 5) , ASTR("St") , 1  );
    EXPRESSION( index("String", "ri")      , 2    , 1  );
    EXPRESSION( index("String", "r" )      , 2    , 1  );
    EXPRESSION( index("String", "xx" )     , -1   , 1  );
    EXPRESSION( indexof("String", "x" )      , -1   , 1  );
    EXPRESSION( count("abcabca" , "a" )      , 3   , 1  );
    EXPRESSION( count("abcabca" , "ab" )     , 2   , 1  );
    EXPRESSION( count("abcabca" , "x"  )     , 0   , 1  );
    EXPRESSION( count("abcabca" , "xx" )     , 0   , 1  );

    #if !ALIB_FEAT_EXPRESSIONS_SPIRIT_PARSER
        EXPRESSION( trim     ("  abc\t "  )           , ASTR("abc")      , 1  );
        EXPRESSION( trimStart("  abc\t "  )           , ASTR("abc\t ")   , 1  );
        EXPRESSION( trimEnd ("  abc\t "  )            , ASTR("  abc")    , 1  );
    #endif

    EXPRESSION( trim     ("xyzabcxyz", "zxy"  )   , ASTR("abc")      , 1  );
    EXPRESSION( trimStart("xyzabcxyz", "zxy"  )   , ASTR("abcxyz")   , 1  );
    EXPRESSION( trimEnd  ("xyzabcxyz", "zxy"  )   , ASTR("xyzabc")   , 1  );

    EXPRESSION( WildcardMatch("wildcard", "*ldca*"         )   , true      , 1  );
    EXPRESSION( WildcardMatch("wildcard", "*ldca*",   true )   , true      , 1  );
    EXPRESSION( WildcardMatch("wildcard", "*LDCA*"         )   , false     , 1  );
    EXPRESSION( WildcardMatch("wildcard", "*LDCA*",   false)   , false     , 1  );
    EXPRESSION( WildcardMatch("wildcard", "*LDCA*",   true )   , true      , 1  );

#if ALIB_FEAT_BOOST_REGEX
    EXPRESSION( RegexMatch("regex", "[qrs]+ege*x*"     )   , true      , 1  );
    EXPRESSION( RegexMatch("regex", "[qXs]+ege*x*"     )   , false     , 1  );
    EXPRESSION( RegexMatch("regex", "[qrs]+ege*Y*"     )   , false     , 1  );
#endif

    EXPRESSION( int("42")      , 42    , 1  );
    EXPRESSION( int("0x12")    , 18    , 1  );
    EXPRESSION( int("0o12" )   , 10    , 1  );
    EXPRESSION( int("0b101")   ,  5    , 1  );

    EXPRESSION( str(42 )       , ASTR("42")    , 1  );
    EXPRESSION( hex(18 )       , ASTR("12")    , 1  );
    EXPRESSION( oct(10 )       , ASTR("12")    , 1  );
    EXPRESSION( bin( 5 )       , ASTR("101")   , 1  );
    EXPRESSION( hex(18 ,4)     , ASTR("0012")  , 1  );
    EXPRESSION( oct(10 ,4)     , ASTR("0012")  , 1  );
    EXPRESSION( bin( 5 ,4)     , ASTR("0101")  , 1  );


    EXPRESSION( float("3.14" )   , 3.14  , 1  );
    EXPRESSION( float("3.14E2" ) , 314.0 , 1  );


    EXPRESSION( token("abc, cde ,efg ", ",", 0       ) , ASTR("abc")    , 1  );
    EXPRESSION( token("abc, cde ,efg ", ",", 1       ) , ASTR(" cde ")  , 1  );
    EXPRESSION( token("abc, cde ,efg ", ",", 2       ) , ASTR("efg ")   , 1  );
    EXPRESSION( trim(token("abc, cde ,efg ", ",", 2 )) , ASTR("efg")    , 1  );

    EXPRESSION( replace("abcabcabc", "a"   ,"X"     ) ,   ASTR("XbcXbcXbc")            ,  1  );
    EXPRESSION( replace("abcabcabc", "abc" ,"X"     ) ,   ASTR("XXX")                  ,  1  );
    EXPRESSION( replace("abcabcabc", "abc" ,"12345" ) ,   ASTR("123451234512345")      ,  1  );

    EXPRESSION( repeat("abc", 0 ) ,   ASTR("")               ,  1  );
    EXPRESSION( repeat("abc", 1 ) ,   ASTR("abc")            ,  1  );
    EXPRESSION( repeat("abc", 3 ) ,   ASTR("abcabcabc")      ,  1  );
}

UT_CLASS_END

}; //namespace

ALIB_WARNINGS_RESTORE

