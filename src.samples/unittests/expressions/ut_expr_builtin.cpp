// #################################################################################################
//  AWorx ALib Unit Tests
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib_precompile.hpp"
#include "alib_test_selection.hpp"
#if ALIB_UT_EXPRESSIONS

#include "ALib.ALox.H"
#include "ALib.Time.H"
#include "ALib.Boxing.H"
#include "ALib.Strings.Calendar.H"
#include "ALib.Strings.Token.H"
#include "ALib.Exceptions.H"
#include "ALib.Camp.H"
#include "ALib.Files.H"
#include "ALib.Expressions.H"

#include <cmath>
#if !defined(M_PI)
#   define M_PI            3.14159265358979323846
#endif
#if !defined(M_E)
#   define M_E            2.7182818284590452354
#endif
#if !defined(M_LN10)
#   define M_LN10        2.30258509299404568402
#endif



#define TESTCLASSNAME       UT_Expr_Builtin
#include "aworx_unittests.hpp"


using namespace std;
using namespace alib;
using namespace alib;
using namespace alib::expressions;
using namespace alib::expressions::detail;
#include "expressions/ut_expr_testfunc.hpp"

ALIB_WARNINGS_MACRO_NOT_USED_OFF

namespace ut_aworx {

extern int TestTokenConsistency(AWorxUnitTesting& ut, Token* tokens, int qtyTokens );



// #################################################################################################
// ### Test Class
// #################################################################################################

UT_CLASS

// #################################################################################################
// #### Math
// #################################################################################################
UT_METHOD(Math)
{
    UT_INIT()

    Compiler compiler;
    compiler.SetupDefaults();
    Expression expression;

    expressions::Scope scope(compiler.CfgFormatter);

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
#if !(defined(ALIB_UT_AVOID_ANALYZER_WARNINGS) && ALIB_SIZEOF_INTEGER == 4)
    CONSTEXPR( exp10(2.0)              , 100.0           );
#endif
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
#if ALIB_CAMP
UT_METHOD(DateTime)
{
    UT_INIT()

    Compiler compiler;
    compiler.SetupDefaults();
    Expression expression;

    expressions::Scope scope(compiler.CfgFormatter);

    // DateTime::Duration construction functions
#if !defined(_WIN32) // not accurate enough
    CONSTEXPR( nanoseconds(0)                    , DateTime::Duration()                       );
    CONSTEXPR( nanoseconds(83)                   , DateTime::Duration::FromNanoseconds(83)    );
    CONSTEXPR( nanoseconds(83)* 100              , DateTime::Duration::FromNanoseconds(8300)  );
    CONSTEXPR( 1000 * nanoseconds(83)            , DateTime::Duration::FromMicroseconds(83)   );
#endif
    CONSTEXPR( microseconds(10) + milliseconds(5), DateTime::Duration::FromMicroseconds(5010) );

    CONSTEXPR( milliseconds(5)                   , DateTime::Duration::FromAbsoluteMilliseconds(5)    );
    CONSTEXPR( millis(5)                         , DateTime::Duration::FromAbsoluteMilliseconds(5)    );
    CONSTEXPR( MSs(5)                            , DateTime::Duration::FromAbsoluteMilliseconds(5)    );
    CONSTEXPR( ms(5)                             , DateTime::Duration::FromAbsoluteMilliseconds(5)    );
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
    CONSTEXPR( millisecs(5.2)                    , DateTime::Duration::FromMilliseconds(5.2)    );
    CONSTEXPR( msecs(5.2)                        , DateTime::Duration::FromMilliseconds(5.2)    );
    CONSTEXPR( msec(5.2)                         , DateTime::Duration::FromMilliseconds(5.2)    );
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
    CONSTEXPR( InHz(milliseconds(200))           , 5.0    );

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
    #if !ALIB_UT_AVOID_ANALYZER_WARNINGS
      EXPRESSION( Age(Now)  < milliseconds(1)          , true    , 4);
      EXPRESSION( IsOlderThan(Now, milliseconds(1) )   , false   , 3);
    #endif
#if !defined(__APPLE__) && !defined(_LIBCPP_VERSION) && !defined(__MINGW32__)
    EXPRESSION( IsOlderThan(Now, nanoseconds(0) )    , true    , 3);
    EXPRESSION( now == now                           , false   , 3);
    EXPRESSION( now != now                           , true    , 3);
    EXPRESSION( now <  now                           , true    , 3);
    EXPRESSION( now >= now                           , false   , 3);
#endif
    EXPRESSION( now <= now                           , true    , 3);
    EXPRESSION( now >  now                           , false   , 3);

    CONSTEXPR( UTCDateTime(2019,1,31,14,5) , CalendarDateTime(2019,1,31,14,5).Get(lang::Timezone::UTC) );
    CONSTEXPR(    DateTime(2019,1,31,14,5) , CalendarDateTime(2019,1,31,14,5).Get() );

    CONSTEXPR( GetYear       (DateTime(2019,1,31,14,5)) , 2019 );
    CONSTEXPR( GetMonth      (DateTime(2019,1,31,14,5)) , 1    );
    CONSTEXPR( GetDay        (DateTime(2019,1,31,14,5)) , 31   );
    CONSTEXPR( GetDayOfWeek  (DateTime(2019,1,31,14,5)) , 4    );
    CONSTEXPR( GetDOW        (DateTime(2019,1,31,14,5)) , 4    );
    CONSTEXPR( GetHour       (DateTime(2019,1,31,14,5)) , 14   );
    CONSTEXPR( GetMinute     (DateTime(2019,1,31,14,5)) , 5    );
    CONSTEXPR( GetMin        (DateTime(2019,1,31,14,5)) , 5    );
    CONSTEXPR( GetMillisecond(DateTime(2019,1,31,14,5)) , 0    );
    CONSTEXPR( GetMS         (DateTime(2019,1,31,14,5)) , 0    );

    CONSTEXPR( GetUTCYear       (UTCDateTime(2019,1,31,14,5)) , 2019 );
    CONSTEXPR( GetUTCMonth      (UTCDateTime(2019,1,31,14,5)) , 1    );
    CONSTEXPR( GetUTCDay        (UTCDateTime(2019,1,31,14,5)) , 31   );
    CONSTEXPR( GetUTCDayOfWeek  (UTCDateTime(2019,1,31,14,5)) , 4    );
    CONSTEXPR( GetUDOW          (UTCDateTime(2019,1,31,14,5)) , 4    );
    CONSTEXPR( GetUTCHour       (UTCDateTime(2019,1,31,14,5)) , 14   );
    CONSTEXPR( GetUTCMinute     (UTCDateTime(2019,1,31,14,5)) , 5    );
    CONSTEXPR( GetUTCMin        (UTCDateTime(2019,1,31,14,5)) , 5    );
    CONSTEXPR( GetUTCMillisecond(UTCDateTime(2019,1,31,14,5)) , 0    );
    CONSTEXPR( GetUMS           (UTCDateTime(2019,1,31,14,5)) , 0    );

    EXPRESSION( now < now + seconds(1)                   , true  , 5 );
    EXPRESSION( now < now - seconds(1)                   , false , 5 );
    EXPRESSION( now < seconds(1) + now                   , true  , 5 );
    EXPRESSION( now - DateTime(1969) > years(47)         , true  , 5 );
    EXPRESSION( inDays( now - DateTime(1969)) /365 >49   , true  , 8 );
    EXPRESSION( today    <= now                          , true  , 3 );
    EXPRESSION( UTCToday <= now                          , true  , 3 );

    EXPRESSION(    GetMonth(DateTime(2019, 1)) == January
                && GetMonth(DateTime(2019, 2)) == Feb
                && GetMonth(DateTime(2019, 3)) == Mar
                && GetMonth(DateTime(2019, 4)) == Apr
                && GetMonth(DateTime(2019, 5)) == May
                && GetMonth(DateTime(2019, 6)) == Jun
                && GetMonth(DateTime(2019, 7)) == Jul
                && GetMonth(DateTime(2019, 8)) == Aug
                && GetMonth(DateTime(2019, 9)) == Sept
                && GetMonth(DateTime(2019,10)) == Oct
                && GetMonth(DateTime(2019,11)) == Novem
                && GetMonth(DateTime(2019,12)) == Dec             , true, 1 );

    EXPRESSION(    GetDayOfWeek(DateTime(2019, 2, 11)) == Monday
                && GetDayOfWeek(DateTime(2019, 2, 12)) == Tue
                && GetDayOfWeek(DateTime(2019, 2, 13)) == Wed
                && GetDayOfWeek(DateTime(2019, 2, 14)) == thu
                && GetDayOfWeek(DateTime(2019, 2, 15)) == fri
                && GetDayOfWeek(DateTime(2019, 2, 16)) == sat
                && GetDayOfWeek(DateTime(2019, 2, 17)) == sun    , true , 1);
}
#endif


// #################################################################################################
// #### StringMatch
// #################################################################################################
UT_METHOD(Strings)
{
    UT_INIT()

    Compiler compiler;
    compiler.SetupDefaults();
    Expression expression;

    expressions::Scope scope(compiler.CfgFormatter);

    // a string!
    CONSTEXPR( "String" , A_CHAR("String")  );


    EXPRESSION( "a" + tab  +"b"   , A_CHAR("a\tb") , 1     );
    EXPRESSION( "a" + newl +"b"   , String32("a")<< NEW_LINE << "b" , 1     );

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


    CONSTEXPR( "String"[0]    , A_CHAR("S")  );
    CONSTEXPR( "String"[1]    , A_CHAR("t")  );
    CONSTEXPR( "String"[2]    , A_CHAR("r")  );
    CONSTEXPR( "String"[-1]   , A_CHAR("")   );
    CONSTEXPR( "String"[10]   , A_CHAR("")   );

    EXPRESSION( Substring("String", 3    ) , A_CHAR("ing"), 1  );
    EXPRESSION( Substring("String", 3,  2) , A_CHAR("in") , 1  );
    EXPRESSION( Substring("String", -3, 5) , A_CHAR("St") , 1  );
    EXPRESSION( indexof("String", "ri")      , 2    , 1  );
    EXPRESSION( indexo ("String", "r" )      , 2    , 1  );
    EXPRESSION( indo   ("String", "xx" )     , -1   , 1  );
    EXPRESSION( indOf  ("String", "x" )      , -1   , 1  );
    EXPRESSION( count("abcabca" , "a" )      , 3   , 1  );
    EXPRESSION( count("abcabca" , "ab" )     , 2   , 1  );
    EXPRESSION( count("abcabca" , "x"  )     , 0   , 1  );
    EXPRESSION( count("abcabca" , "xx" )     , 0   , 1  );

    EXPRESSION( trim     ("  abc\t "  )           , A_CHAR("abc")      , 1  );
    EXPRESSION( trimStart("  abc\t "  )           , A_CHAR("abc\t ")   , 1  );
    EXPRESSION( trimEnd ("  abc\t "  )            , A_CHAR("  abc")    , 1  );

    EXPRESSION( trim     ("xyzabcxyz", "zxy"  )   , A_CHAR("abc")      , 1  );
    EXPRESSION( trimStart("xyzabcxyz", "zxy"  )   , A_CHAR("abcxyz")   , 1  );
    EXPRESSION( trimEnd  ("xyzabcxyz", "zxy"  )   , A_CHAR("xyzabc")   , 1  );

    EXPRESSION( WildcardMatch("wildcard", "*ldca*"         )   , true      , 1  );
    EXPRESSION( WildcardMatch("wildcard", "*ldca*",   true )   , true      , 1  );
    EXPRESSION( WildcardMatch("wildcard", "*LDCA*"         )   , false     , 1  );
    EXPRESSION( WildcardMatch("wildcard", "*LDCA*",   false)   , false     , 1  );
    EXPRESSION( WildcardMatch("wildcard", "*LDCA*",   true )   , true      , 1  );

#if ALIB_FEAT_BOOST_REGEX && (!ALIB_CHARACTERS_WIDE || ALIB_CHARACTERS_NATIVE_WCHAR)
    EXPRESSION( RegexMatch("regex", "[qrs]+ege*x*"     )   , true      , 1  );
    EXPRESSION( RegexMatch("regex", "[qXs]+ege*x*"     )   , false     , 1  );
    EXPRESSION( RegexMatch("regex", "[qrs]+ege*Y*"     )   , false     , 1  );
#endif

    EXPRESSION( int("42")      , 42    , 1  );
    EXPRESSION( int("0x12")    , 18    , 1  );
    EXPRESSION( int("0o12" )   , 10    , 1  );
    EXPRESSION( int("0b101")   ,  5    , 1  );

    EXPRESSION( str(42 )       , A_CHAR("42")    , 1  );
    EXPRESSION( hex(18 )       , A_CHAR("12")    , 1  );
    EXPRESSION( oct(10 )       , A_CHAR("12")    , 1  );
    EXPRESSION( bin( 5 )       , A_CHAR("101")   , 1  );
    EXPRESSION( hex(18 ,4)     , A_CHAR("0012")  , 1  );
    EXPRESSION( oct(10 ,4)     , A_CHAR("0012")  , 1  );
    EXPRESSION( bin( 5 ,4)     , A_CHAR("0101")  , 1  );


    EXPRESSION( float("3.14" )   , 3.14  , 1  );
    EXPRESSION( float("3.14E2" ) , 314.0 , 1  );


    EXPRESSION( token("abc, cde ,efg ", ",", 0       ) , A_CHAR("abc")    , 1  );
    EXPRESSION( token("abc, cde ,efg ", ",", 1       ) , A_CHAR(" cde ")  , 1  );
    EXPRESSION( token("abc, cde ,efg ", ",", 2       ) , A_CHAR("efg ")   , 1  );
    EXPRESSION( trim(token("abc, cde ,efg ", ",", 2 )) , A_CHAR("efg")    , 1  );

    EXPRESSION( replace("abcabcabc", "a"   ,"X"     ) ,   A_CHAR("XbcXbcXbc")            ,  1  );
    EXPRESSION( replace("abcabcabc", "abc" ,"X"     ) ,   A_CHAR("XXX")                  ,  1  );
    EXPRESSION( replace("abcabcabc", "abc" ,"12345" ) ,   A_CHAR("123451234512345")      ,  1  );

    EXPRESSION( repeat("abc", 0 ) ,   A_CHAR("")               ,  1  );
    EXPRESSION( repeat("abc", 1 ) ,   A_CHAR("abc")            ,  1  );
    EXPRESSION( repeat("abc", 3 ) ,   A_CHAR("abcabcabc")      ,  1  );
}

// #################################################################################################
// ### TokenConsistency
// #################################################################################################
#if ALIB_CAMP
UT_METHOD(TokenConsistency)
{
    UT_INIT()

    #if ALIB_FILES
        constexpr int FLS= 39;
    #else
        constexpr int FLS= 0;
    #endif

    #if ALIB_FEAT_BOOST_REGEX && (!ALIB_CHARACTERS_WIDE || ALIB_CHARACTERS_NATIVE_WCHAR)
        constexpr int SSZ= 25;
    #else
        constexpr int SSZ= 24;
    #endif
    Token tokens[9+1+58+34+SSZ+FLS];
    strings::util::LoadResourcedTokens( EXPRESSIONS, "CPA"     ,&tokens[0]             ALIB_DBG(,  9));
    strings::util::LoadResourcedTokens( EXPRESSIONS, "CPALen"  ,&tokens[9]             ALIB_DBG(,  1));
    strings::util::LoadResourcedTokens( EXPRESSIONS, "CPD"     ,&tokens[9+1]           ALIB_DBG(, 58));
    strings::util::LoadResourcedTokens( EXPRESSIONS, "CPM"     ,&tokens[9+1+58]        ALIB_DBG(, 34));
    strings::util::LoadResourcedTokens( EXPRESSIONS, "CPS"     ,&tokens[9+1+58+34]     ALIB_DBG(,SSZ));
    #if ALIB_FILES
    strings::util::LoadResourcedTokens( alib::FILES, "CPF"     ,&tokens[9+1+58+34+SSZ] ALIB_DBG(,FLS));
    #endif

    // The following 10 warnings are OK, as they are overloaded with different parameter types.
    //    Abbreviation "Int" of token "Integer" matches against token "Integer"
    //    Abbreviation "Float" of token "Float  " matches against token "Float  "
    //    Abbreviation "Oct  " of token "October" matches against token "Octal  "
    //    Abbreviation "Mon  " of token "Monday " matches against token "Months "
    //    Abbreviation "Min  " of token "MinuteS" matches against token "min    "
    //    Abbreviation "Mon  " of token "Months " matches against token "Monday "
    //    Abbreviation "min  " of token "min    " matches against token "MinuteS"
    //    Abbreviation "Int  " of token "Integer" matches against token "Integer"
    //    Abbreviation "Float" of token "Float  " matches against token "Float  "
    //    Abbreviation "Oct  " of token "Octal  " matches against token "October"
    // Should new warnings come up, it has to be checked if the new functions differ in parameters
    // and the value may be increased.
    UT_EQ( 10, TestTokenConsistency(ut, tokens, 9+1+58+34+SSZ+FLS ) )
}
#endif


#include "aworx_unittests_end.hpp"

} //namespace

ALIB_WARNINGS_RESTORE

#endif // ALIB_UT_EXPRESSIONS
