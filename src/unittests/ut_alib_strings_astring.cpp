// #################################################################################################
//  aworx - Unit Tests
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alox.hpp"

#define TESTCLASSNAME       CPP_ALib_Strings_AString
#include "aworx_unittests.hpp"
#include "alib/compatibility/std_string.hpp"

#include <iostream>
#include <sstream>

#include <clocale>

// Windows.h might bring in max/min macros
#if defined( max )
    #undef max
    #undef min
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

using namespace std;
using namespace aworx;

namespace ut_aworx {


UT_CLASS()


//--------------------------------------------------------------------------------------------------
//--- Test Constructors
//--------------------------------------------------------------------------------------------------
UT_METHOD( Constructors )
{
    UT_INIT();

    AString* ms;
    char*     csNull= nullptr;
               ms= new AString();                        UT_EQ    ( 0 , ms->Capacity()   );    UT_EQ( ms->Length(), 0 );
    delete ms; ms= new AString( 0 );                     UT_EQ    ( 0 , ms->Capacity()   );    UT_EQ( ms->Length(), 0 );
    delete ms; ms= new AString( csNull );                UT_EQ    ( 0 , ms->Capacity()   );    UT_EQ( ms->Length(), 0 );
    delete ms; ms= new AString( "" );                    UT_TRUE  ( ms->Capacity() >  0 );    UT_EQ( ms->Length(), 0 );
    delete ms; ms= new AString( 25 );                    UT_TRUE  ( ms->Capacity() >  0 );    UT_EQ( ms->Length(), 0 );
    delete ms; ms= new AString( "Test" );                UT_TRUE  ( ms->Capacity() >= 4 );    UT_EQ( ms->Length(), 4 );

    AString tNullString;
    delete ms; ms= new AString( tNullString, 1,     0 );     UT_TRUE  ( ms->Length() == 0 );  UT_TRUE  ( ms->IsNull() );
    delete ms; ms= new AString( tNullString, -1000, 0 );     UT_TRUE  ( ms->Length() == 0 );  UT_TRUE  ( ms->IsNull() );
    delete ms; ms= new AString( tNullString, -1000, 1 );     UT_TRUE  ( ms->Length() == 0 );  UT_TRUE  ( ms->IsNull() );
    delete ms; ms= new AString( tNullString, -100,  1000);   UT_TRUE  ( ms->Length() == 0 );  UT_TRUE  ( ms->IsNull() );

    AString tASEmpty("");
    delete ms; ms= new AString( tASEmpty, 1,     0 );    UT_TRUE  ( ms->Length()     == 0 );  UT_TRUE  ( ms->Capacity() > 0 );
    delete ms; ms= new AString( tASEmpty, -1000, 0 );    UT_TRUE  ( ms->Length()     == 0 );  UT_TRUE  ( ms->Capacity() > 0 );
    delete ms; ms= new AString( tASEmpty, -1000, 1 );    UT_TRUE  ( ms->Length()     == 0 );  UT_TRUE  ( ms->Capacity() > 0 );
    delete ms; ms= new AString( tASEmpty, -100,  1000);  UT_TRUE  ( ms->Length()     == 0 );  UT_TRUE  ( ms->Capacity() > 0 );

    AString tAS( ASTR("0123456789") );
    delete ms; ms= new AString( tAS, 5);                 UT_TRUE  ( ms->Capacity() >=   5 && ms->Length() ==  5 );    UT_EQ( ASTR("56789")       , *ms);
    delete ms; ms= new AString( tAS, 5, 100);            UT_TRUE  ( ms->Capacity() >=   5 && ms->Length() ==  5 );    UT_EQ( ASTR("56789")       , *ms);
    delete ms; ms= new AString( tAS, -5);                UT_TRUE  ( ms->Capacity() >=  10 && ms->Length() == 10 );    UT_EQ( ASTR("0123456789")  , *ms);
    delete ms; ms= new AString( tAS, -5, 3);             UT_TRUE  ( ms->Capacity() >    0 && ms->Length() ==  0 );    UT_EQ( ASTR("")            , *ms);
    delete ms; ms= new AString( tAS, 50, 3);             UT_TRUE  ( ms->Capacity() >    0 && ms->Length() ==  0 );    UT_EQ( ASTR("")            , *ms);
    delete ms; ms= new AString( tAS, 10, 3);             UT_TRUE  ( ms->Capacity() >    0 && ms->Length() ==  0 );    UT_EQ( ASTR("")            , *ms);
    delete ms; ms= new AString( tAS, -5, 10);            UT_TRUE  ( ms->Capacity() >=   5 && ms->Length() ==  5 );    UT_EQ( ASTR("01234")       , *ms);
    delete ms; ms= new AString( tAS, -5, 100);           UT_TRUE  ( ms->Capacity() >=  10 && ms->Length() == 10 );    UT_EQ( ASTR("0123456789")  , *ms);

    std::basic_string<character> tS(ASTR("0123456789"));
    delete ms; ms= new AString( ASTR("") );                    UT_TRUE  ( ms->Length() == 0 );    UT_TRUE    ( ms->Capacity() > 0 );
    delete ms; ms= new AString( ASTR(""), 1,     0 );          UT_TRUE  ( ms->Length() == 0 );    UT_TRUE    ( ms->Capacity() > 0 );
    delete ms; ms= new AString( ASTR(""), -1000, 0 );          UT_TRUE  ( ms->Length() == 0 );    UT_TRUE    ( ms->Capacity() > 0 );
    delete ms; ms= new AString( ASTR(""), -1000, 1 );          UT_TRUE  ( ms->Length() == 0 );    UT_TRUE    ( ms->Capacity() > 0 );
    delete ms; ms= new AString( ASTR(""), -100,  1000);        UT_TRUE  ( ms->Length() == 0 );    UT_TRUE    ( ms->Capacity() > 0 );
    delete ms; ms= new AString( tS );                    UT_TRUE  ( ms->Capacity() >=  10 && ms->Length() == 10 );
                                                         UT_EQ( ASTR("0123456789")  , *ms );
    delete ms; ms= new AString( tS, 5);                  UT_TRUE  ( ms->Capacity() >=  5  && ms->Length() == 5 );
                                                         UT_EQ( ASTR("56789")       , *ms );
    delete ms; ms= new AString( tS, 5, 100);             UT_TRUE  ( ms->Capacity() >=  5  && ms->Length() == 5 );
                                                         UT_EQ( ASTR("56789")       ,*ms );
    delete ms; ms= new AString( tS, -5);                 UT_TRUE  ( ms->Capacity() >=  10 && ms->Length() == 10 );
                                                         UT_EQ( ASTR("0123456789")  , *ms );
    delete ms; ms= new AString( tS, -5, 10);             UT_TRUE  ( ms->Capacity() >=  5  && ms->Length() == 5 );
                                                         UT_EQ( ASTR("01234")       , *ms );
    delete ms; ms= new AString( tS, -5, 100);            UT_TRUE  ( ms->Capacity() >=  10 && ms->Length() == 10 );
                                                         UT_EQ( ASTR("0123456789")  , *ms );
    const character* tSBEmpty= ASTR("");
    delete ms; ms= new AString( tSBEmpty );              UT_TRUE  ( ms->Length() == 0 );
    delete ms; ms= new AString( tSBEmpty, 1,     0 );    UT_TRUE  ( ms->Length() == 0 );
    delete ms; ms= new AString( tSBEmpty, -1000, 0 );    UT_TRUE  ( ms->Length() == 0 );
    delete ms; ms= new AString( tSBEmpty, -1000, 1 );    UT_TRUE  ( ms->Length() == 0 );
    delete ms; ms= new AString( tSBEmpty, -100,  1000);  UT_TRUE  ( ms->Length() == 0 );

    const character*  tSB= ASTR("0123456789");
    delete ms; ms= new AString( tSB );                   UT_TRUE  ( ms->Capacity() >=  10 && ms->Length() == 10 ); UT_EQ( ASTR("0123456789")   , *ms );
    delete ms; ms= new AString( tSB, 5);                 UT_TRUE  ( ms->Capacity() >=  5  && ms->Length() == 5  ); UT_EQ( ASTR("56789")        , *ms );
    delete ms; ms= new AString( tSB, 5, 100);            UT_TRUE  ( ms->Capacity() >=  5  && ms->Length() == 5  ); UT_EQ( ASTR("56789")        , *ms );
    delete ms; ms= new AString( tSB, -5);                UT_TRUE  ( ms->Capacity() >=  10 && ms->Length() == 10 ); UT_EQ( ASTR("0123456789")   , *ms );
    delete ms; ms= new AString( tSB, -5, 10);            UT_TRUE  ( ms->Capacity() >=  5  && ms->Length() == 5  ); UT_EQ( ASTR("01234")        , *ms );
    delete ms; ms= new AString( tSB, -5, 100);           UT_TRUE  ( ms->Capacity() >=  10 && ms->Length() == 10 );
    delete ms;
}


//--------------------------------------------------------------------------------------------------
//--- Test IsNull
//--------------------------------------------------------------------------------------------------
UT_METHOD( IsNull )
{
    UT_INIT();

    AString ms;                UT_TRUE( ms.IsNull()    ); UT_TRUE(  ms.IsNull() );  UT_TRUE( !ms.IsNotNull() );  UT_TRUE(  ms.IsEmpty() );  UT_TRUE( !ms.IsNotEmpty() );
    ms= ASTR("");              UT_TRUE( ms.IsNotNull() ); UT_TRUE( !ms.IsNull() );  UT_TRUE(  ms.IsNotNull() );  UT_TRUE(  ms.IsEmpty() );  UT_TRUE( !ms.IsNotEmpty() );
    ms= ASTR("x");             UT_TRUE( ms.IsNotNull() ); UT_TRUE( !ms.IsNull() );  UT_TRUE(  ms.IsNotNull() );  UT_TRUE( !ms.IsEmpty() );  UT_TRUE(  ms.IsNotEmpty() );

    ms= static_cast<char*>( nullptr);
                               UT_TRUE( ms.IsNull()    ); UT_TRUE(  ms.IsNull() );  UT_TRUE( !ms.IsNotNull() );  UT_TRUE(  ms.IsEmpty() );  UT_TRUE( !ms.IsNotEmpty() );
    ms._( ASTR(""));           UT_TRUE( ms.IsNotNull() ); UT_TRUE( !ms.IsNull() );  UT_TRUE(  ms.IsNotNull() );  UT_TRUE(  ms.IsEmpty() );  UT_TRUE( !ms.IsNotEmpty() );
    ms._( ASTR("xx"));         UT_TRUE( ms.IsNotNull() ); UT_TRUE( !ms.IsNull() );  UT_TRUE(  ms.IsNotNull() );  UT_TRUE( !ms.IsEmpty() );  UT_TRUE(  ms.IsNotEmpty() );

    ms.SetNull();              UT_TRUE( ms.IsNull()    ); UT_TRUE(  ms.IsNull() );  UT_TRUE( !ms.IsNotNull() );  UT_TRUE(  ms.IsEmpty() );  UT_TRUE( !ms.IsNotEmpty() );
    ms.SetBuffer(5);           UT_TRUE( ms.IsNotNull() ); UT_TRUE( !ms.IsNull() );  UT_TRUE(  ms.IsNotNull() );  UT_TRUE(  ms.IsEmpty() );  UT_TRUE( !ms.IsNotEmpty() );

    ms.SetNull();              UT_TRUE( ms.IsNull()    ); UT_TRUE(  ms.IsNull() );  UT_TRUE( !ms.IsNotNull() );  UT_TRUE(  ms.IsEmpty() );  UT_TRUE( !ms.IsNotEmpty() );
    ms= ASTR("");              UT_TRUE( ms.IsNotNull() ); UT_TRUE( !ms.IsNull() );  UT_TRUE(  ms.IsNotNull() );  UT_TRUE(  ms.IsEmpty() );  UT_TRUE( !ms.IsNotEmpty() );

    ms.SetNull();              UT_TRUE( ms.IsNull()    ); UT_TRUE(  ms.IsNull() );  UT_TRUE( !ms.IsNotNull() );  UT_TRUE(  ms.IsEmpty() );  UT_TRUE( !ms.IsNotEmpty() );
    ms= ASTR("abc");           UT_TRUE( ms.IsNotNull() ); UT_TRUE( !ms.IsNull() );  UT_TRUE(  ms.IsNotNull() );  UT_TRUE( !ms.IsEmpty() );  UT_TRUE(  ms.IsNotEmpty() );
}

//--------------------------------------------------------------------------------------------------
//--- Test FirstBackCharAt
//--------------------------------------------------------------------------------------------------
UT_METHOD( CharAt )
{
    UT_INIT();

    AString ms;
    UT_EQ(  ms.CharAtStart       ()    ,    '\0' );
    UT_EQ(  ms.CharAtEnd         ()    ,    '\0' );
    UT_EQ(  ms.CharAt            ( -1 ),    '\0' );
    UT_EQ(  ms.CharAt            (  0 ),    '\0' );
    UT_EQ(  ms.CharAt            (  1 ),    '\0' );

    ms.Clear()._('A');

    UT_EQ(  ms.CharAtStart       ()    ,    'A'  );
    UT_EQ(  ms.CharAtStart<false>()    ,    'A'  );
    UT_EQ(  ms.CharAtEnd         ()    ,    'A'  );
    UT_EQ(  ms.CharAtEnd<false>  ()    ,    'A'  );
    UT_EQ(  ms.CharAt            ( -1 ),    '\0' );
    UT_EQ(  ms.CharAt            (  0 ),    'A'  );
    UT_EQ(  ms.CharAt            (  1 ),    '\0' );
    UT_EQ(  ms.CharAt<false>     (  0 ),    'A'  );

    UT_EQ(  ms                   [  0 ],    'A'  );

    ms.Clear()._("ABC");

    UT_EQ(  ms.CharAtStart       ()    ,    'A'  );
    UT_EQ(  ms.CharAtStart<false>()    ,    'A'  );
    UT_EQ(  ms.CharAtEnd         ()    ,    'C'  );
    UT_EQ(  ms.CharAtEnd<false>  ()    ,    'C'  );
    UT_EQ(  ms.CharAt            ( -1 ),    '\0' );
    UT_EQ(  ms.CharAt            (  0 ),    'A'  );
    UT_EQ(  ms.CharAt            (  1 ),    'B'  );
    UT_EQ(  ms.CharAt            (  2 ),    'C'  );
    UT_EQ(  ms.CharAt            (  3 ),    '\0' );

    UT_EQ(  ms.CharAt<false>     (  0 ),    'A'  );
    UT_EQ(  ms.CharAt<false>     (  1 ),    'B'  );
    UT_EQ(  ms.CharAt<false>     (  2 ),    'C'  );

    UT_EQ(  ms                   [  0 ],    'A'  );
    UT_EQ(  ms                   [  1 ],    'B'  );
    UT_EQ(  ms                   [  2 ],    'C'  );

    ms.Clear()._("ABC");
    ms.SetCharAt( -1, '@' );    UT_EQ( ASTR("ABC"),   ms );
    ms.SetCharAt(  0, 'X' );    UT_EQ( ASTR("XBC"),   ms );
    ms.SetCharAt(  1, 'Y' );    UT_EQ( ASTR("XYC"),   ms );
    ms.SetCharAt(  2, 'Z' );    UT_EQ( ASTR("XYZ"),   ms );
    ms.SetCharAt(  3, '$' );    UT_EQ( ASTR("XYZ"),   ms );
}


//--------------------------------------------------------------------------------------------------
//--- Test DeleteInsert
//--------------------------------------------------------------------------------------------------
UT_METHOD( DeleteInsertFillReplace )
{
    UT_INIT();

    AString ms;
    // delete
    {
        ms.Clear()._(ASTR("0123456789"));   ms.Delete                 (  5           );      UT_EQ( ASTR("01234"),        ms );
        ms.Clear()._(ASTR("0123456789"));   ms.Delete                 (  5,      0   );      UT_EQ( ASTR("0123456789"),   ms );
        ms.Clear()._(ASTR("0123456789"));   ms.Delete                 (  5,     -1   );      UT_EQ( ASTR("0123456789"),   ms );
        ms.Clear()._(ASTR("0123456789"));   ms.Delete                 (  5,      1   );      UT_EQ( ASTR("012346789"),    ms );
        ms.Clear()._(ASTR("0123456789"));   ms.Delete                 (  5,    100   );      UT_EQ( ASTR("01234"),        ms );
        ms.Clear()._(ASTR("0123456789"));   ms.Delete                 (  -5,     10  );      UT_EQ( ASTR("56789"),        ms );

        ms.Clear()._(ASTR("0123456789"));   ms.Delete<false>          (  5,    5     );      UT_EQ( ASTR("01234"),        ms );
        ms.Clear()._(ASTR("0123456789"));   ms.Delete<false>          (  0,    5     );      UT_EQ( ASTR("56789"),        ms );
        ms.Clear()._(ASTR("0123456789"));   ms.Delete<false>          (  0,    1     );      UT_EQ( ASTR("123456789"),    ms );
        ms.Clear()._(ASTR("0123456789"));   ms.Delete<false>          (  9,    1     );      UT_EQ( ASTR("012345678"),    ms );

        ms.Clear()._(ASTR("0123456789"));   ms.DeleteStart            (  -2          );      UT_EQ( ASTR("0123456789"),   ms );
        ms.Clear()._(ASTR("0123456789"));   ms.DeleteStart            (  -1          );      UT_EQ( ASTR("0123456789"),   ms );
        ms.Clear()._(ASTR("0123456789"));   ms.DeleteStart            (   0          );      UT_EQ( ASTR("0123456789"),   ms );
        ms.Clear()._(ASTR("0123456789"));   ms.DeleteStart            (   1          );      UT_EQ(  ASTR("123456789"),   ms );
        ms.Clear()._(ASTR("0123456789"));   ms.DeleteStart            (   2          );      UT_EQ(   ASTR("23456789"),   ms );
        ms.Clear()._(ASTR("0123456789"));   ms.DeleteStart            (   9          );      UT_EQ(          ASTR("9"),   ms );
        ms.Clear()._(ASTR("0123456789"));   ms.DeleteStart            (  10          );      UT_EQ(           ASTR(""),   ms );
        ms.Clear()._(ASTR("0123456789"));   ms.DeleteStart            (  11          );      UT_EQ(           ASTR(""),   ms );

        ms.Clear()._(ASTR("0123456789"));   ms.DeleteStart<false>     (   0          );      UT_EQ( ASTR("0123456789"),   ms );
        ms.Clear()._(ASTR("0123456789"));   ms.DeleteStart<false>     (   1          );      UT_EQ(  ASTR("123456789"),   ms );
        ms.Clear()._(ASTR("0123456789"));   ms.DeleteStart<false>     (   2          );      UT_EQ(   ASTR("23456789"),   ms );
        ms.Clear()._(ASTR("0123456789"));   ms.DeleteStart<false>     (   9          );      UT_EQ(          ASTR("9"),   ms );
        ms.Clear()._(ASTR("0123456789"));   ms.DeleteStart<false>     (  10          );      UT_EQ(           ASTR(""),   ms );

        ms.Clear()._(ASTR("0123456789"));   ms.DeleteEnd              (  -2          );      UT_EQ( ASTR("0123456789"),   ms );
        ms.Clear()._(ASTR("0123456789"));   ms.DeleteEnd              (  -1          );      UT_EQ( ASTR("0123456789"),   ms );
        ms.Clear()._(ASTR("0123456789"));   ms.DeleteEnd              (   0          );      UT_EQ( ASTR("0123456789"),   ms );
        ms.Clear()._(ASTR("0123456789"));   ms.DeleteEnd              (   1          );      UT_EQ( ASTR("012345678") ,   ms );
        ms.Clear()._(ASTR("0123456789"));   ms.DeleteEnd              (   2          );      UT_EQ( ASTR("01234567")  ,   ms );
        ms.Clear()._(ASTR("0123456789"));   ms.DeleteEnd              (   9          );      UT_EQ( ASTR("0")         ,   ms );
        ms.Clear()._(ASTR("0123456789"));   ms.DeleteEnd              (  10          );      UT_EQ( ASTR("")          ,   ms );
        ms.Clear()._(ASTR("0123456789"));   ms.DeleteEnd              (  11          );      UT_EQ( ASTR("")          ,   ms );

        ms.Clear()._(ASTR("0123456789"));   ms.DeleteEnd <false>      (   0          );      UT_EQ( ASTR("0123456789"),   ms );
        ms.Clear()._(ASTR("0123456789"));   ms.DeleteEnd <false>      (   1          );      UT_EQ( ASTR("012345678") ,   ms );
        ms.Clear()._(ASTR("0123456789"));   ms.DeleteEnd <false>      (   2          );      UT_EQ( ASTR("01234567")  ,   ms );
        ms.Clear()._(ASTR("0123456789"));   ms.DeleteEnd <false>      (   9          );      UT_EQ( ASTR("0")         ,   ms );
        ms.Clear()._(ASTR("0123456789"));   ms.DeleteEnd <false>      (  10          );      UT_EQ( ASTR("")          ,   ms );
    }

    // InsertChars
    {
        ms.Clear()._(ASTR("0123456789"));   ms.InsertChars            ( ' ',  1,  -1 );      UT_EQ( ASTR("0123456789"),   ms );
        ms.Clear()._(ASTR("0123456789"));   ms.InsertChars            ( ' ',  1,  20 );      UT_EQ( ASTR("0123456789"),   ms );
        ms.Clear()._(ASTR("0123456789"));   ms.InsertChars            ( ' ',  0,   0 );      UT_EQ( ASTR("0123456789"),   ms );

        ms.Clear()._(ASTR("0123456789"));   ms.InsertChars            ( ' ',  1,   0 );      UT_EQ( ASTR(" 0123456789"),  ms );
        ms.Clear()._(ASTR("0123456789"));   ms.InsertChars            ( ' ',  1,   1 );      UT_EQ( ASTR("0 123456789"),  ms );
        ms.Clear()._(ASTR("0123456789"));   ms.InsertChars            ( ' ',  2,   0 );      UT_EQ( ASTR("  0123456789"), ms );
        ms.Clear()._(ASTR("0123456789"));   ms.InsertChars            ( ' ',  2,   1 );      UT_EQ( ASTR("0  123456789"), ms );

        ms.Clear()._(ASTR("0123456789"));   ms.InsertChars            ( ' ',  1,  11 );      UT_EQ( ASTR("0123456789"),   ms );
        ms.Clear()._(ASTR("0123456789"));   ms.InsertChars            ( ' ',  1,  10 );      UT_EQ( ASTR("0123456789 "),  ms );
        ms.Clear()._(ASTR("0123456789"));   ms.InsertChars            ( '@',  3,   5 );      UT_EQ( ASTR("01234@@@56789"),ms );
    }

    // InsertAt
    {
        ms.Clear()._(ASTR("0123456789"));   ms.InsertAt               ( ASTR("TST"),  -2 );        UT_EQ( ASTR("0123456789"),    ms );
        ms.Clear()._(ASTR("0123456789"));   ms.InsertAt               ( ASTR("TST"),  -1 );        UT_EQ( ASTR("0123456789"),    ms );
        ms.Clear()._(ASTR("0123456789"));   ms.InsertAt               ( ASTR("TST"),   0 );        UT_EQ( ASTR("TST0123456789"), ms );
        ms.Clear()._(ASTR("0123456789"));   ms.InsertAt               ( ASTR("TST"),   1 );        UT_EQ( ASTR("0TST123456789"), ms );
        ms.Clear()._(ASTR("0123456789"));   ms.InsertAt               ( ASTR("TST"),   8 );        UT_EQ( ASTR("01234567TST89"), ms );
        ms.Clear()._(ASTR("0123456789"));   ms.InsertAt               ( ASTR("TST"),   9 );        UT_EQ( ASTR("012345678TST9"), ms );
        ms.Clear()._(ASTR("0123456789"));   ms.InsertAt               ( ASTR("TST"),  10 );        UT_EQ( ASTR("0123456789TST"), ms );
        ms.Clear()._(ASTR("0123456789"));   ms.InsertAt               ( ASTR("TST"),  11 );        UT_EQ( ASTR("0123456789"),    ms );
        ms.Clear()._(ASTR("0123456789"));   ms.InsertAt               ( ASTR("TST"),  12 );        UT_EQ( ASTR("0123456789"),    ms );

        ms.Clear()._(ASTR("0123456789"));   ms.InsertAt<false>        ( ASTR("TST"),   0 );        UT_EQ( ASTR("TST0123456789"), ms );
        ms.Clear()._(ASTR("0123456789"));   ms.InsertAt<false>        ( ASTR("TST"),   1 );        UT_EQ( ASTR("0TST123456789"), ms );
        ms.Clear()._(ASTR("0123456789"));   ms.InsertAt<false>        ( ASTR("TST"),   8 );        UT_EQ( ASTR("01234567TST89"), ms );
        ms.Clear()._(ASTR("0123456789"));   ms.InsertAt<false>        ( ASTR("TST"),   9 );        UT_EQ( ASTR("012345678TST9"), ms );
        ms.Clear()._(ASTR("0123456789"));   ms.InsertAt<false>        ( ASTR("TST"),  10 );        UT_EQ( ASTR("0123456789TST"), ms );

    }

    // ReplaceSubstring
    {
        String r= ASTR("ABC");
        ms.Clear()._(ASTR("0123456789"));   ms.ReplaceSubstring       ( r,   0,  10)  ;      UT_EQ( ASTR("ABC"),          ms );
        ms.Clear()._(ASTR("0123456789"));   ms.ReplaceSubstring       ( r, -10, 100)  ;      UT_EQ( ASTR("0123456789"),   ms );
        ms.Clear()._(ASTR("0123456789"));   ms.ReplaceSubstring       ( r, -10,  5 )  ;      UT_EQ( ASTR("0123456789"),   ms );
        ms.Clear()._(ASTR("0123456789"));   ms.ReplaceSubstring       ( r, -10, 10 )  ;      UT_EQ( ASTR("0123456789"),   ms );
        ms.Clear()._(ASTR("0123456789"));   ms.ReplaceSubstring       ( r, -10, 11 )  ;      UT_EQ( ASTR("0123456789"),   ms );
        ms.Clear()._(ASTR("0123456789"));   ms.ReplaceSubstring       ( r,   0,  1 )  ;      UT_EQ( ASTR("ABC123456789"), ms );
        ms.Clear()._(ASTR("0123456789"));   ms.ReplaceSubstring       ( r,   0,  2 )  ;      UT_EQ( ASTR("ABC23456789"),  ms );
        ms.Clear()._(ASTR("0123456789"));   ms.ReplaceSubstring       ( r,   1,  1 )  ;      UT_EQ( ASTR("0ABC23456789"), ms );
        ms.Clear()._(ASTR("0123456789"));   ms.ReplaceSubstring       ( r,   1,  2 )  ;      UT_EQ( ASTR("0ABC3456789"),  ms );
        ms.Clear()._(ASTR("0123456789"));   ms.ReplaceSubstring       ( r,   8,  1 )  ;      UT_EQ( ASTR("01234567ABC9"), ms );
        ms.Clear()._(ASTR("0123456789"));   ms.ReplaceSubstring       ( r,   8,  2 )  ;      UT_EQ( ASTR("01234567ABC"),  ms );
        ms.Clear()._(ASTR("0123456789"));   ms.ReplaceSubstring       ( r,   8,  3 )  ;      UT_EQ( ASTR("0123456789"),   ms );

        ms.Clear()._(ASTR("0123456789"));   ms.ReplaceSubstring<false>( r,   0,  1 )  ;      UT_EQ( ASTR("ABC123456789"), ms );
        ms.Clear()._(ASTR("0123456789"));   ms.ReplaceSubstring<false>( r,   0,  2 )  ;      UT_EQ( ASTR("ABC23456789"),  ms );
        ms.Clear()._(ASTR("0123456789"));   ms.ReplaceSubstring<false>( r,   1,  1 )  ;      UT_EQ( ASTR("0ABC23456789"), ms );
        ms.Clear()._(ASTR("0123456789"));   ms.ReplaceSubstring<false>( r,   1,  2 )  ;      UT_EQ( ASTR("0ABC3456789"),  ms );
        ms.Clear()._(ASTR("0123456789"));   ms.ReplaceSubstring<false>( r,   8,  1 )  ;      UT_EQ( ASTR("01234567ABC9"), ms );
        ms.Clear()._(ASTR("0123456789"));   ms.ReplaceSubstring<false>( r,   8,  2 )  ;      UT_EQ( ASTR("01234567ABC"),  ms );
    }

    // ReplaceRegion
    {
        ms.Clear()._(ASTR("0123456789"));   ms.ReplaceRegion          ( '@',   0,  10)  ;    UT_EQ( ASTR("@@@@@@@@@@"),   ms );
        ms.Clear()._(ASTR("0123456789"));   ms.ReplaceRegion          ( '@', -10, 100)  ;    UT_EQ( ASTR("@@@@@@@@@@"),   ms );
        ms.Clear()._(ASTR("0123456789"));   ms.ReplaceRegion          ( '@', -10,  5 )  ;    UT_EQ( ASTR("0123456789"),   ms );
        ms.Clear()._(ASTR("0123456789"));   ms.ReplaceRegion          ( '@', -10, 10 )  ;    UT_EQ( ASTR("0123456789"),   ms );
        ms.Clear()._(ASTR("0123456789"));   ms.ReplaceRegion          ( '@', -10, 11 )  ;    UT_EQ( ASTR("@123456789"),   ms );
        ms.Clear()._(ASTR("0123456789"));   ms.ReplaceRegion          ( '@',   0,  1 )  ;    UT_EQ( ASTR("@123456789"),   ms );
        ms.Clear()._(ASTR("0123456789"));   ms.ReplaceRegion          ( '@',   0,  2 )  ;    UT_EQ( ASTR("@@23456789"),   ms );
        ms.Clear()._(ASTR("0123456789"));   ms.ReplaceRegion          ( '@',   1,  1 )  ;    UT_EQ( ASTR("0@23456789"),   ms );
        ms.Clear()._(ASTR("0123456789"));   ms.ReplaceRegion          ( '@',   1,  2 )  ;    UT_EQ( ASTR("0@@3456789"),   ms );
        ms.Clear()._(ASTR("0123456789"));   ms.ReplaceRegion          ( '@',   8,  1 )  ;    UT_EQ( ASTR("01234567@9"),   ms );
        ms.Clear()._(ASTR("0123456789"));   ms.ReplaceRegion          ( '@',   8,  2 )  ;    UT_EQ( ASTR("01234567@@"),   ms );
        ms.Clear()._(ASTR("0123456789"));   ms.ReplaceRegion          ( '@',   8,  3 )  ;    UT_EQ( ASTR("01234567@@"),   ms );
        ms.Clear()._(ASTR("0123456789"));   ms.ReplaceRegion<false>   ( '@',   0,  1 )  ;    UT_EQ( ASTR("@123456789"),   ms );
        ms.Clear()._(ASTR("0123456789"));   ms.ReplaceRegion<false>   ( '@',   0,  2 )  ;    UT_EQ( ASTR("@@23456789"),   ms );
        ms.Clear()._(ASTR("0123456789"));   ms.ReplaceRegion<false>   ( '@',   1,  1 )  ;    UT_EQ( ASTR("0@23456789"),   ms );
        ms.Clear()._(ASTR("0123456789"));   ms.ReplaceRegion<false>   ( '@',   1,  2 )  ;    UT_EQ( ASTR("0@@3456789"),   ms );
        ms.Clear()._(ASTR("0123456789"));   ms.ReplaceRegion<false>   ( '@',   8,  1 )  ;    UT_EQ( ASTR("01234567@9"),   ms );
        ms.Clear()._(ASTR("0123456789"));   ms.ReplaceRegion<false>   ( '@',   8,  2 )  ;    UT_EQ( ASTR("01234567@@"),   ms );
    }

}

//--------------------------------------------------------------------------------------------------
//--- Test Conversions
//--------------------------------------------------------------------------------------------------
UT_METHOD( Conversions )
{
    UT_INIT();

    AString   ms;
    basic_string<character>     s;
    ms= ASTR("0123456789");

    s= ToStdString(ms  );                     UT_EQ( s.c_str(), ASTR("0123456789") );
    s= ToStdString(ms,  5);                   UT_EQ( s.c_str(), ASTR("56789") );
    s= ToStdString(ms,  5, 100);              UT_EQ( s.c_str(), ASTR("56789") );
    s= ToStdString(ms,  -5);                  UT_EQ( s.c_str(), ASTR("0123456789") );
    s= ToStdString(ms,  -5, 10);              UT_EQ( s.c_str(), ASTR("01234") );
    s= ToStdString(ms,  -5, 100);             UT_EQ( s.c_str(), ASTR("0123456789") );

    s= ToStdString(ms,  0,  0);               UT_EQ( s.c_str(), EmptyString );
    s= ToStdString(ms,  5,  0);               UT_EQ( s.c_str(), EmptyString );
    s= ToStdString(ms,  20, 0);               UT_EQ( s.c_str(), EmptyString );
    s= ToStdString(ms,  0,  -1);              UT_EQ( s.c_str(), EmptyString );
    s= ToStdString(ms,  5,  -5);              UT_EQ( s.c_str(), EmptyString );
    s= ToStdString(ms,  5,  -100);            UT_EQ( s.c_str(), EmptyString );
    s= ToStdString(ms,  20,  -1);             UT_EQ( s.c_str(), EmptyString );
    s= ToStdString(ms,  20,  20);             UT_EQ( s.c_str(), EmptyString );
    s= ToStdString(ms,  20,  -100);           UT_EQ( s.c_str(), EmptyString );
    s= ToStdString(ms,  20, 0);               UT_EQ( s.c_str(), EmptyString );
    s= ToStdString(ms,  20, 100);             UT_EQ( s.c_str(), EmptyString );

       ToStdString(ms, s);                    UT_EQ( s.c_str(), ASTR("0123456789") );
       ToStdString(ms, s, 5);                 UT_EQ( s.c_str(), ASTR("56789") );
       ToStdString(ms, s, 5, 100);            UT_EQ( s.c_str(), ASTR("56789") );
       ToStdString(ms, s, -5);                UT_EQ( s.c_str(), ASTR("0123456789") );
       ToStdString(ms, s, -5, 10);            UT_EQ( s.c_str(), ASTR("01234") );
       ToStdString(ms, s, -5, 100);           UT_EQ( s.c_str(), ASTR("0123456789") );

       ToStdString(ms, s, 0,  0);             UT_EQ( s.c_str(), EmptyString );
       ToStdString(ms, s, 5,  0);             UT_EQ( s.c_str(), EmptyString );
       ToStdString(ms, s, 20, 0);             UT_EQ( s.c_str(), EmptyString );
       ToStdString(ms, s, 0,  -1);            UT_EQ( s.c_str(), EmptyString );
       ToStdString(ms, s, 5,  -5);            UT_EQ( s.c_str(), EmptyString );
       ToStdString(ms, s, 5,  -100);          UT_EQ( s.c_str(), EmptyString );
       ToStdString(ms, s, 20,  -1);           UT_EQ( s.c_str(), EmptyString );
       ToStdString(ms, s, 20,  20);           UT_EQ( s.c_str(), EmptyString );
       ToStdString(ms, s, 20,  -100);         UT_EQ( s.c_str(), EmptyString );
       ToStdString(ms, s, 20, 0);             UT_EQ( s.c_str(), EmptyString );
       ToStdString(ms, s, 20, 100);           UT_EQ( s.c_str(), EmptyString );

       s.clear();
       ToStdString(ms,  s );                           UT_EQ( s.c_str(), ASTR("0123456789")            );
       ToStdString(ms,  s,         CurrentData::Keep); UT_EQ( s.c_str(), ASTR("01234567890123456789")  );
       ToStdString(ms,  s );                           UT_EQ( s.c_str(), ASTR("0123456789")            );
       ToStdString(ms,  s, 5, 100 );                   UT_EQ( s.c_str(), ASTR("56789")                 );
       ToStdString(ms,  s, -5, 10, CurrentData::Keep); UT_EQ( s.c_str(), ASTR("5678901234")            );


    std::basic_stringstream<aworx::character> strs;
    ms= ASTR("hello stringstream");
    strs << ms;
    UT_EQ( strs.str(), ASTR("hello stringstream") );

    strs << endl;
    strs << ASTR("second line");

    strs >> ms;
    UT_EQ( ASTR("hello stringstream"), ms );
    strs >> ms;
    UT_EQ( ASTR("second line")       , ms );

    strs >> ms;
    UT_EQ( ASTR("")                  , ms );
}


//--------------------------------------------------------------------------------------------------
//--- Test AssignAndAppend
//--------------------------------------------------------------------------------------------------
UT_METHOD( Append )
{
    UT_INIT();

    // const character*
    {
        AString ms;  const character* csNull= nullptr;  const character* csEmpty= ASTR("");

        ms= csNull;                UT_EQ  ( 0, ms.Length()     );  UT_TRUE( ms.IsNull()    );
        ms= csEmpty;               UT_EQ  ( 0, ms.Length()     );  UT_TRUE( ms.IsNotNull() );
        ms= ASTR("assign");        UT_EQ  ( 6, ms.Length()     );
        ms= csNull;                UT_EQ  ( 0, ms.Length()     );  UT_TRUE( ms.IsNull()    );
        ms= ASTR("assign");        UT_EQ  ( 6, ms.Length()     );
        ms= csNull;                UT_EQ  ( 0, ms.Length()     );  UT_TRUE( ms.IsNull()    );


        ms._( csNull );            UT_EQ  ( 0, ms.Length()     );  UT_TRUE( ms.IsNull()    );
        ms._( csEmpty);            UT_EQ  ( 0, ms.Length()     );  UT_TRUE( ms.IsNotNull() );
        ms.SetNull();              UT_EQ  ( 0, ms.Length()     );  UT_TRUE( ms.IsNull()    );

        ms._(ASTR("1234567"));     UT_EQ  (16, ms.Capacity()   );  UT_EQ( ASTR("1234567"),   ms );
        ms._(ASTR("89"));          UT_TRUE( ms.Length()> 7     );  UT_EQ( ASTR("123456789"), ms );

        const character* t= ASTR("0123456789");
        ms.Clear()._   ( t, 5);                     UT_EQ( ASTR("56789")      , ms );
        ms.Clear()._   ( t, 5, 100);                UT_EQ( ASTR("56789")      , ms );
        ms.Clear()._   ( t, -5);                    UT_EQ( ASTR("0123456789") , ms );
        ms.Clear()._   ( t, -5, 3);                 UT_EQ( ASTR("")           , ms );
        ms.Clear()._   ( t, 50, 3);                 UT_EQ( ASTR("")           , ms );
        ms.Clear()._   ( t, 10, 3);                 UT_EQ( ASTR("")           , ms );
        ms.Clear()._   ( t, -5, 10);                UT_EQ( ASTR("01234")      , ms );
        ms.Clear()._   ( t, -5, 100);               UT_EQ( ASTR("0123456789") , ms );

        // _NC
        ms.SetNull();            UT_EQ  ( 0, ms.Length()      );  UT_TRUE( ms.IsNull()   );
        ms._NC( csEmpty );       UT_EQ  ( 0, ms.Length()      );  UT_TRUE( ms.IsNull()   );
        ms.Clear()._NC( t, 5,3);                  UT_EQ( ASTR("567")   ,  ms    );
    }

    // const complementChar*
    {
        AString ms;  const complementChar* csNull= nullptr;
#if ALIB_NARROW_STRINGS
        const complementChar*  csEmpty= L"";
#else
        const complementChar*  csEmpty=  "";
#endif
        ms= csNull;                UT_EQ  ( 0, ms.Length()     );  UT_TRUE( ms.IsNull()    );
        ms= csEmpty;               UT_EQ  ( 0, ms.Length()     );  UT_TRUE( ms.IsNotNull() );
        ms= ASTR("assign");        UT_EQ  ( 6, ms.Length()     );
        ms= csNull;                UT_EQ  ( 0, ms.Length()     );  UT_TRUE( ms.IsNull()    );
        ms= ASTR("assign");        UT_EQ  ( 6, ms.Length()     );
        ms= csNull;                UT_EQ  ( 0, ms.Length()     );  UT_TRUE( ms.IsNull()    );


        ms._( csNull );            UT_EQ  ( 0, ms.Length()    );  UT_TRUE( ms.IsNull()    );
        ms._( csEmpty);            UT_EQ  ( 0, ms.Length()    );  UT_TRUE( ms.IsNotNull() );
        ms.SetNull();              UT_EQ  ( 0, ms.Length()    );  UT_TRUE( ms.IsNull()    );

        ms._(ASTR("1234567"));     UT_EQ  (16, ms.Capacity()  );  UT_EQ( ASTR("1234567"),   ms );
        ms._(ASTR("89"));          UT_TRUE( ms.Length()> 7    );  UT_EQ( ASTR("123456789"), ms );

        const character* t= ASTR("0123456789");
        ms.Clear()._   ( t, 5);                     UT_EQ( ASTR("56789")      , ms );
        ms.Clear()._   ( t, 5, 100);                UT_EQ( ASTR("56789")      , ms );
        ms.Clear()._   ( t, -5);                    UT_EQ( ASTR("0123456789") , ms );
        ms.Clear()._   ( t, -5, 3);                 UT_EQ( ASTR("")           , ms );
        ms.Clear()._   ( t, 50, 3);                 UT_EQ( ASTR("")           , ms );
        ms.Clear()._   ( t, 10, 3);                 UT_EQ( ASTR("")           , ms );
        ms.Clear()._   ( t, -5, 10);                UT_EQ( ASTR("01234")      , ms );
        ms.Clear()._   ( t, -5, 100);               UT_EQ( ASTR("0123456789") , ms );

        // _NC
        ms.SetNull();            UT_EQ  ( 0, ms.Length()      );  UT_TRUE( ms.IsNull()   );
        ms._NC( csEmpty );       UT_EQ  ( 0, ms.Length()      );  UT_TRUE( ms.IsNull()   );
        ms.Clear()._NC( t, 5,3);                  UT_EQ( ASTR("567")   ,  ms   );
    }

    // const strangeChar*
    {
        AString ms;  const strangeChar* csNull= nullptr;
#if ALIB_SIZEOF_WCHAR_T == 4
        const strangeChar*  csEmpty= u"";
#else
        const strangeChar*  csEmpty= U"";
#endif
        ms= csNull;                UT_EQ  ( 0, ms.Length()     );  UT_TRUE( ms.IsNull()    );
        ms= csEmpty;               UT_EQ  ( 0, ms.Length()     );  UT_TRUE( ms.IsNotNull() );
        ms= ASTR("assign");        UT_EQ  ( 6, ms.Length()     );
        ms= csNull;                UT_EQ  ( 0, ms.Length()     );  UT_TRUE( ms.IsNull()    );
        ms= ASTR("assign");        UT_EQ  ( 6, ms.Length()     );
        ms= csNull;                UT_EQ  ( 0, ms.Length()     );  UT_TRUE( ms.IsNull()    );


        ms._( csNull );            UT_EQ  ( 0, ms.Length()    );  UT_TRUE( ms.IsNull()    );
        ms._( csEmpty);            UT_EQ  ( 0, ms.Length()    );  UT_TRUE( ms.IsNotNull() );
        ms.SetNull();              UT_EQ  ( 0, ms.Length()    );  UT_TRUE( ms.IsNull()    );

        ms._(ASTR("1234567"));     UT_EQ  (16, ms.Capacity()  );  UT_EQ( ASTR("1234567"),   ms );
        ms._(ASTR("89"));          UT_TRUE( ms.Length()> 7    );  UT_EQ( ASTR("123456789"), ms );

        const character* t= ASTR("0123456789");
        ms.Clear()._   ( t, 5);                     UT_EQ( ASTR("56789")      , ms );
        ms.Clear()._   ( t, 5, 100);                UT_EQ( ASTR("56789")      , ms );
        ms.Clear()._   ( t, -5);                    UT_EQ( ASTR("0123456789") , ms );
        ms.Clear()._   ( t, -5, 3);                 UT_EQ( ASTR("")           , ms );
        ms.Clear()._   ( t, 50, 3);                 UT_EQ( ASTR("")           , ms );
        ms.Clear()._   ( t, 10, 3);                 UT_EQ( ASTR("")           , ms );
        ms.Clear()._   ( t, -5, 10);                UT_EQ( ASTR("01234")      , ms );
        ms.Clear()._   ( t, -5, 100);               UT_EQ( ASTR("0123456789") , ms );

        // _NC
        ms.SetNull();            UT_EQ  ( 0, ms.Length()      );  UT_TRUE( ms.IsNull()   );
        ms._NC( csEmpty );       UT_EQ  ( 0, ms.Length()      );  UT_TRUE( ms.IsNull()   );
        ms.Clear()._NC( t, 5,3);                  UT_EQ( ASTR("567")   ,  ms   );
    }

    // fundamental types
    {
        AString ms;
        {  int      i= 5;   ms._()._(i)._('/')._(&i);       UT_EQ ( ASTR("5/5"),    ms); }

        {  int8_t   i=  4;  ms._()._(i)._('/')._(&i);       UT_EQ ( ASTR("4/4")   , ms); }

        {  int8_t   i= -4;  ms._()._(i)._('/')._(&i);       UT_EQ ( ASTR("-4/-4") , ms); }
        {  uint8_t  i=  4;  ms._()._(i)._('/')._(&i);       UT_EQ ( ASTR("4/4")   , ms); }


        {  int16_t  i=  5;  ms._()._(i)._('/')._(&i);       UT_EQ ( ASTR("5/5")   , ms); }
        {  int16_t  i= -5;  ms._()._(i)._('/')._(&i);       UT_EQ ( ASTR("-5/-5") , ms); }
        {  uint16_t i=  5;  ms._()._(i)._('/')._(&i);       UT_EQ ( ASTR("5/5")   , ms); }

        {  int32_t  i=  6;  ms._()._(i)._('/')._(&i);       UT_EQ ( ASTR("6/6")   , ms); }
        {  int32_t  i= -6;  ms._()._(i)._('/')._(&i);       UT_EQ ( ASTR("-6/-6") , ms); }
        {  uint32_t i=  6;  ms._()._(i)._('/')._(&i);       UT_EQ ( ASTR("6/6")   , ms); }

        {  int64_t  i=  7;  ms._()._(i)._('/')._(&i);       UT_EQ ( ASTR("7/7")   , ms); }
        {  int64_t  i= -7;  ms._()._(i)._('/')._(&i);       UT_EQ ( ASTR("-7/-7") , ms); }
        {  uint64_t i=  7;  ms._()._(i)._('/')._(&i);       UT_EQ ( ASTR("7/7")   , ms); }

        {  int8_t   i=  4;  ms._()._<false>(i)._('/')._<false>(&i);       UT_EQ ( ASTR("4/4")   , ms); }
        {  int8_t   i= -4;  ms._()._<false>(i)._('/')._<false>(&i);       UT_EQ ( ASTR("-4/-4") , ms); }
        {  uint8_t  i=  4;  ms._()._<false>(i)._('/')._<false>(&i);       UT_EQ ( ASTR("4/4")   , ms); }


        {  int16_t  i=  5;  ms._()._<false>(i)._('/')._<false>(&i);       UT_EQ ( ASTR("5/5")   , ms); }
        {  int16_t  i= -5;  ms._()._<false>(i)._('/')._<false>(&i);       UT_EQ ( ASTR("-5/-5") , ms); }
        {  uint16_t i=  5;  ms._()._<false>(i)._('/')._<false>(&i);       UT_EQ ( ASTR("5/5")   , ms); }

        {  int32_t  i=  6;  ms._()._<false>(i)._('/')._<false>(&i);       UT_EQ ( ASTR("6/6")   , ms); }
        {  int32_t  i= -6;  ms._()._<false>(i)._('/')._<false>(&i);       UT_EQ ( ASTR("-6/-6") , ms); }
        {  uint32_t i=  6;  ms._()._<false>(i)._('/')._<false>(&i);       UT_EQ ( ASTR("6/6")   , ms); }

        {  int64_t  i=  7;  ms._()._<false>(i)._('/')._<false>(&i);       UT_EQ ( ASTR("7/7")   , ms); }
        {  int64_t  i= -7;  ms._()._<false>(i)._('/')._<false>(&i);       UT_EQ ( ASTR("-7/-7") , ms); }
        {  uint64_t i=  7;  ms._()._<false>(i)._('/')._<false>(&i);       UT_EQ ( ASTR("7/7")   , ms); }
    }

    // Substring
    {
        AString ms;  Substring ssNull; Substring ssEmpty(ASTR(""));  Substring t( ASTR("01234") );

        ms= ssNull;                UT_EQ  ( 0, ms.Length()        );  UT_TRUE( ms.IsNull()    );
        ms= ssEmpty;               UT_EQ  ( 0, ms.Length()        );  UT_TRUE( ms.IsNotNull() );
        ms= t;                     UT_EQ  ( 5, ms.Length()        );  UT_EQ  ( String(t), ms);
        ms= ssNull;                UT_EQ  ( 0, ms.Length()        );  UT_TRUE( ms.IsNull()    );

        ms._( ssNull );            UT_EQ  ( 0, ms.Length()        );  UT_TRUE( ms.IsNull()    );
        ms._( ssEmpty);            UT_EQ  ( 0, ms.Length()        );  UT_TRUE( ms.IsNotNull() );
        ms.SetNull();              UT_EQ  ( 0, ms.Length()        );  UT_TRUE( ms.IsNull()    );
        ms._( t );                 UT_EQ  (16, ms.Capacity()      );    UT_EQ( ASTR("01234")     , ms );
        ms._( t );                 UT_TRUE( ms.Length()> 5     );  UT_EQ( ASTR("0123401234"), ms );

        t.ConsumeChar();         ms.Clear()._( t );           UT_EQ( ASTR("1234") , ms  );
        t.ConsumeChar();         ms.Clear()._( t );           UT_EQ(  ASTR("234") , ms  );
        t.ConsumeCharFromEnd();  ms.Clear()._( t );           UT_EQ( ASTR("23")   , ms  );

        // _NC
        ms.SetNull();             UT_EQ  ( 0, ms.Length()        );  UT_TRUE( ms.IsNull()    );
        ms._NC( ssEmpty );        UT_EQ  ( 0, ms.Length()        );  UT_TRUE( ms.IsNull()   );
        ms.Clear()._NC( t);                       UT_EQ( ASTR("23")  ,  ms   );
    }

    // std::string
    {
        AString ms;   std::basic_string<character> t( ASTR("012") ); std::basic_string<character> ssEmpty(ASTR(""));
                                   UT_EQ  ( 0, ms.Length()      );  UT_TRUE( ms.IsNull()    );
        ms= ssEmpty;               UT_EQ  ( 0, ms.Length()      );  UT_TRUE( ms.IsNotNull() );
        ms= t;                     UT_EQ  ( 3, ms.Length()      );  UT_EQ  ( t.c_str(), ms  );
        ms.SetNull();

        ms._( ssEmpty);            UT_EQ  ( 0, ms.Length()      );  UT_TRUE( ms.IsNotNull() );
        ms.SetNull();              UT_EQ  ( 0, ms.Length()      );  UT_TRUE( ms.IsNull()    );
        ms._( t );                 UT_EQ  (16, ms.Capacity()    );  UT_EQ( ASTR("012")   , ms );
        ms._( t );                 UT_TRUE( ms.Length()> 3      );  UT_EQ( ASTR("012012"), ms );
        t= ASTR("0123456789");
        ms.Clear()._( t, 5);            UT_EQ( ASTR("56789")      , ms );
        ms.Clear()._( t, 5, 100);       UT_EQ( ASTR("56789")      , ms );
        ms.Clear()._( t, -5);           UT_EQ( ASTR("0123456789") , ms );
        ms.Clear()._( t, -5, 3);        UT_EQ( ASTR("")           , ms );
        ms.Clear()._( t, 50, 3);        UT_EQ( ASTR("")           , ms );
        ms.Clear()._( t, 10, 3);        UT_EQ( ASTR("")           , ms );
        ms.Clear()._( t, -5, 10);       UT_EQ( ASTR("01234")      , ms );
        ms.Clear()._( t, -5, 100);      UT_EQ( ASTR("0123456789") , ms );

        // _NC
        ms.SetNull();                   UT_EQ( 0, ms.Length());  UT_TRUE( ms.IsNull() );
        ms._NC( ssEmpty );              UT_EQ( 0, ms.Length());  UT_TRUE( ms.IsNull() );
        ms.Clear()._NC( t);             UT_EQ( ASTR("0123456789"), ms  );
        ms.Clear()._<false>( t ,2,3);   UT_EQ( ASTR("234")       , ms  );
    }

    // string literals
    {
        // zero length literal
        {
            AString ms( ASTR("") );         UT_EQ( 0, ms.Length() );   UT_TRUE( ms.IsNotNull() );
        }
        {
            AString ms;
            ms._ ( ASTR("") );              UT_EQ( 0, ms.Length() );   UT_TRUE( ms.IsNotNull() );
        }
        {
            AString ms;
            ms <<       ASTR("")  ;         UT_EQ( 0, ms.Length() );   UT_TRUE( ms.IsNotNull() );
        }

        // 1 - 6 length literals
        {
            {   AString ms( ASTR("a") );       UT_EQ( 1, ms.Length() );   UT_EQ( ASTR("a")        , ms  );}
            {   AString ms( ASTR("ab") );      UT_EQ( 2, ms.Length() );   UT_EQ( ASTR("ab")       , ms  );}
            {   AString ms( ASTR("abc") );     UT_EQ( 3, ms.Length() );   UT_EQ( ASTR("abc")      , ms  );}
            {   AString ms( ASTR("abcd") );    UT_EQ( 4, ms.Length() );   UT_EQ( ASTR("abcd")     , ms  );}
            {   AString ms( ASTR("abcde") );   UT_EQ( 5, ms.Length() );   UT_EQ( ASTR("abcde")    , ms  );}
            {   AString ms( ASTR("abcdef") );  UT_EQ( 6, ms.Length() );   UT_EQ( ASTR("abcdef")   , ms  );}

            { AString ms;
              ms.Clear()._( ASTR("a") );       UT_EQ( 1, ms.Length() );   UT_EQ( ASTR("a")        , ms  );
              ms.Clear()._( ASTR("ab") );      UT_EQ( 2, ms.Length() );   UT_EQ( ASTR("ab")       , ms  );
              ms.Clear()._( ASTR("abc") );     UT_EQ( 3, ms.Length() );   UT_EQ( ASTR("abc")      , ms  );
              ms.Clear()._( ASTR("abcd") );    UT_EQ( 4, ms.Length() );   UT_EQ( ASTR("abcd")     , ms  );
              ms.Clear()._( ASTR("abcde") );   UT_EQ( 5, ms.Length() );   UT_EQ( ASTR("abcde")    , ms  );
              ms.Clear()._( ASTR("abcdef") );  UT_EQ( 6, ms.Length() );   UT_EQ( ASTR("abcdef")   , ms  );}
            { AString ms;
              ms.Clear() << ASTR("a")  ;       UT_EQ( 1, ms.Length() );   UT_EQ( ASTR("a")        , ms  );
              ms.Clear() << ASTR("ab")  ;      UT_EQ( 2, ms.Length() );   UT_EQ( ASTR("ab")       , ms  );
              ms.Clear() << ASTR("abc")  ;     UT_EQ( 3, ms.Length() );   UT_EQ( ASTR("abc")      , ms  );
              ms.Clear() << ASTR("abcd")  ;    UT_EQ( 4, ms.Length() );   UT_EQ( ASTR("abcd")     , ms  );
              ms.Clear() << ASTR("abcde")  ;   UT_EQ( 5, ms.Length() );   UT_EQ( ASTR("abcde")    , ms  );
              ms.Clear() << ASTR("abcdef")  ;  UT_EQ( 6, ms.Length() );   UT_EQ( ASTR("abcdef")   , ms  );}
        }

    }

}


//--------------------------------------------------------------------------------------------------
//--- Test Fields
//--------------------------------------------------------------------------------------------------
UT_METHOD( Fields )
{
    UT_INIT();

    AString ms;

    // empty fields
    ms._( Format::Field( nullptr, 3, Alignment::Left,     '#' ) );
    #if ALIB_DEBUG
        #if defined(_WIN32)
            UT_EQ( ASTR("std::nullptr_t(0x0)"), ms   );
        #else
            UT_EQ( ASTR("decltype(nullptr)(0x0)"), ms   );
        #endif
    #endif

    ms.Clear();

    ms._( Format::Field( ASTR("") ,     3, Alignment::Left,     '#' ) );    UT_EQ( ASTR("###")          , ms );
    ms._( Format::Field( ASTR("") ,     4, Alignment::Center,   '*' ) );    UT_EQ( ASTR("###****")      , ms );
    ms._( Format::Field( ASTR("") ,     5, Alignment::Right,    '+' ) );    UT_EQ( ASTR("###****+++++") , ms );

    // field, alignment left
    ms.Clear();
    ms._( Format::Field( ASTR("A")       , 5,    Alignment::Left ) );
    ms._( Format::Field( ASTR("AB")      , 5,    Alignment::Left ) );
    ms._( Format::Field( ASTR("ABC")     , 5,    Alignment::Left ) );
    ms._( Format::Field( ASTR("ABCD")    , 5,    Alignment::Left ) );
    ms._( Format::Field( ASTR("ABCDE")   , 5,    Alignment::Left ) );
    ms._( Format::Field( ASTR("ABCDEF")  , 5,    Alignment::Left ) );
    ms._( Format::Field( ASTR("ABCDEFG") , 5,    Alignment::Left ) );
    UT_EQ( ASTR("A    AB   ABC  ABCD ABCDEABCDEFABCDEFG"), ms );

    // field, alignment right
    ms.Clear();
    ms._( Format::Field( ASTR("A")       , 5,    Alignment::Right ) );
    ms._( Format::Field( ASTR("AB")      , 5,    Alignment::Right ) );
    ms._( Format::Field( ASTR("ABC")     , 5                      ) ); // btw: right is default
    ms._( Format::Field( ASTR("ABCD")    , 5                      ) );
    ms._( Format::Field( ASTR("ABCDE")   , 5                      ) );
    ms._( Format::Field( ASTR("ABCDEF")  , 5,    Alignment::Right ) );
    ms._( Format::Field( ASTR("ABCDEFG") , 5,    Alignment::Right ) );
    UT_EQ( ASTR("    A   AB  ABC ABCDABCDEABCDEFABCDEFG"), ms );

    // field, alignment center
    ms.Clear();
    ms._( Format::Field( ASTR("A")       , 5,    Alignment::Center) );
    ms._( Format::Field( ASTR("AB")      , 5,    Alignment::Center) );
    ms._( Format::Field( ASTR("ABC")     , 5,    Alignment::Center) );
    ms._( Format::Field( ASTR("ABCD")    , 5,    Alignment::Center) );
    ms._( Format::Field( ASTR("ABCDE")   , 5,    Alignment::Center) );
    ms._( Format::Field( ASTR("ABCDEF")  , 5,    Alignment::Center) );
    ms._( Format::Field( ASTR("ABCDEFG") , 5,    Alignment::Center) );
    UT_EQ( ASTR("  A   AB   ABC ABCD ABCDEABCDEFABCDEFG"), ms );

    // a nested field
    ms.Clear();
    ms._(ASTR("***"));
        String32 inner  (ASTR("OS:"));    inner._(Format::Field( ASTR("Linux"), 10, Alignment::Right, '-' ) );
             inner._(ASTR(" Lang:")); inner._(Format::Field( ASTR("en_US"),  8, Alignment::Right, '-' ) );
        ms._(Format::Field( inner, 35, Alignment::Center, '#' ) );
    ms._(ASTR("***"));
    UT_EQ( ASTR("***####OS:-----Linux Lang:---en_US####***"), ms );
}




//--------------------------------------------------------------------------------------------------
//--- Test Capacity
//--------------------------------------------------------------------------------------------------
UT_METHOD( CapacityLength )
{
    UT_INIT();

    // null-allocated
    {
        AString ms( 0 );
        ms._(  ASTR("") );       UT_TRUE( ms.Capacity() > 0 );
    }

    {
        AString ms( 5 );
        ms._( 'a' );        UT_EQ( 5, ms.Capacity() );
        ms._( 'b' );        UT_EQ( 5, ms.Capacity() );
        ms._( 'c' );        UT_EQ( 5, ms.Capacity() );
        ms._( 'd' );        UT_EQ( 5, ms.Capacity() );
        ms._( 'e' );        UT_EQ( 5, ms.Capacity() );
        ms._( 'x' );        UT_TRUE( ms.Capacity() >= 6 );
        UT_EQ( ASTR("abcdex"), ms );

        ms.GrowBufferAtLeastBy( 2000 );      UT_TRUE( ms.Capacity() >= 2000 );
    }

    {
        AString   ms;
        integer as;
        ms._( ASTR("0123456789") );           UT_EQ( 10, ms.Length() );    UT_TRUE  ( ms.Capacity() >= 10 );

        as= ms.Capacity();

        lib::lang::Report::GetDefault().PushHaltFlags( false, false );
            UT_PRINT( "A warning should follow" );
            ms.SetLength(20);                    UT_EQ( 10, ms.Length() );    UT_EQ  ( ms.Capacity(), as );    UT_EQ(ASTR("0123456789")   ,  ms  );

            UT_PRINT( "No (second) warning should follow" );
            ms.SetLength(20);                    UT_EQ( 10, ms.Length() );    UT_EQ  ( ms.Capacity(), as );    UT_EQ(ASTR("0123456789")   ,  ms  );

            ALIB_WARN_ONCE_PER_TYPE_ENABLE( AString, SetLengthLonger );

            UT_PRINT( "No warning should follow" );
            ms.SetLength(10);                    UT_EQ( 10, ms.Length() );    UT_EQ  ( ms.Capacity(), as );    UT_EQ(ASTR("0123456789")   ,  ms  );

            UT_PRINT( "A warning should follow" );
            ms.SetLength(11);                    UT_EQ( 10, ms.Length() );    UT_EQ  ( ms.Capacity(), as );    UT_EQ(ASTR("0123456789")   ,  ms  );

            UT_PRINT( "No (second) warning should follow" );
            ms.SetLength(11);                    UT_EQ( 10, ms.Length() );    UT_EQ  ( ms.Capacity(), as );    UT_EQ(ASTR("0123456789")   ,  ms  );

        lib::lang::Report::GetDefault().PopHaltFlags();

        ms.SetLength(5);                         UT_EQ( 5, ms.Length() );    UT_EQ  ( ms.Capacity(), as );    UT_EQ( ASTR("01234")  , ms    );
        ms.SetBuffer(3);                         UT_EQ( 3, ms.Length() );    UT_EQ  ( ms.Capacity(), 3  );    UT_EQ( ASTR("012")    , ms    );
        ms._(ASTR("ABC"));                       UT_EQ( 6, ms.Length() );    UT_TRUE( ms.Capacity() >= 6);    UT_EQ( ASTR("012ABC") , ms    );
    }

    // external buffer, heap allocated (transfer responsibility)
    #if !ALIB_DEBUG_STRINGS
    {
        AString* ms;
        ms= new AString();
        ms->SetBuffer( new character[5], 5, 0, Responsibility::Transfer );
        ms->_(ASTR("ABC"));                  UT_EQ( 3, ms->Length() );        UT_TRUE ( ms->Capacity() == 4 );  UT_EQ( ASTR("ABC")   , *ms  );

        delete ms;

        ms= new AString();
        ms->SetBuffer( new character[5], 5, 0, Responsibility::Transfer );
        ms->_(ASTR("ABC"));
        ms->_(ASTR("ABC"));                  UT_EQ( 6, ms->Length() );        UT_TRUE ( ms->Capacity() > 4 );  UT_EQ( ASTR("ABCABC")   , *ms  );
        delete ms;
    }
    #endif

    // external buffer, stack allocated (keep responsibility)
    {
        character stackCA1[5];
        AString sMS1;
        sMS1.SetBuffer( reinterpret_cast<character*>(&stackCA1), 5 );
        sMS1._(ASTR("ABC"));                 UT_TRUE ( sMS1.Buffer() == reinterpret_cast<character*>(&stackCA1) );  UT_EQ( ASTR("ABC")   , sMS1  );

        lib::lang::Report::GetDefault().PushHaltFlags( false, false );
            character stackCA2[5];
            AString sMS2;
            sMS2.SetBuffer( reinterpret_cast<character*>(&stackCA2), 5 );
            sMS2._(ASTR("ABC"));
            UT_PRINT( "A warning should follow" );
            sMS2._(ASTR("ABC"));                UT_TRUE ( sMS2.Buffer() != reinterpret_cast<character*>(&stackCA2) );  UT_EQ( ASTR("ABCABC")   , sMS2  );

            PreallocatedString<5> msS;
            const character* orig= msS.Buffer();
            msS._(ASTR("ABC"));                 UT_TRUE ( msS.Buffer() == orig );  UT_EQ( ASTR("ABC")   , String(msS)  );
            UT_PRINT( "A warning should follow" );
            msS._(ASTR("ABC"));                 UT_TRUE ( msS.Buffer() != orig );  UT_EQ( ASTR("ABCABC"), String(msS)  );

            String64 ms64;
            orig= ms64.Buffer();
            ms64._(ASTR("ABC"));                UT_TRUE ( ms64.Buffer() == orig ); UT_EQ( ASTR("ABC")   , String(ms64) );
            ms64.Clear();
            ms64.InsertChars('@', 63 );   UT_TRUE ( ms64.Buffer() == orig );
            UT_PRINT( "A warning should follow" );
            ms64._( '@' );
            UT_TRUE ( ms64 != orig );
            UT_TRUE ( ms64.SearchAndReplace( '@', 'X' ) == 64 );
        lib::lang::Report::GetDefault().PopHaltFlags();
    }
}


//--------------------------------------------------------------------------------------------------
//--- Test Tab
//--------------------------------------------------------------------------------------------------
UT_METHOD( Tab )
{
    UT_INIT();

    AString ms;

    // test some bad input
                   ms._(Format::Tab(  0, 0, -1,        '@' ));        UT_EQ( EmptyString,  ms );
    ms.Clear();    ms._(Format::Tab(  0, 0,  0,        '@' ));        UT_EQ( EmptyString,  ms );
    ms.Clear();    ms._(Format::Tab( -1, 0,  0,        '@' ));        UT_EQ( EmptyString,  ms );
    ms.Clear();    ms._(Format::Tab( -1, 0,  0,        '@' ));        UT_EQ( EmptyString,  ms );
    ms.Clear();    ms._(Format::Tab( -1, 0,  -1,       '@' ));        UT_EQ( EmptyString,  ms );
    ms.Clear();    ms._(Format::Tab( -5, 0,  0,        '@' ));        UT_EQ( EmptyString,  ms );
    ms.Clear();    ms._(Format::Tab( -5, 0,  -100,     '@' ));        UT_EQ( EmptyString,  ms );
    ms.Clear();    ms._(Format::Tab(  2, 0,  -101,     '@' ));        UT_EQ( EmptyString,  ms );
    ms.Clear();    ms._(Format::Tab(  0, 0,  0,        '@' ));        UT_EQ( EmptyString,  ms );
    ms.Clear();    ms._(Format::Tab(  1, 0,  0,        '@' ));        UT_EQ( EmptyString,  ms );
    ms.Clear();    ms._(Format::Tab(  1, 0,  1,        '@' ));        UT_EQ( ASTR("@"), ms );
    ms.Clear();    ms._(Format::Tab(  0, 0,  1,        '@' ));        UT_EQ( ASTR("@"), ms );


    // minpad 0 (not common, default is 1, tab wont move if on tab position)
    ms.Clear();                    ms._(Format::Tab( 2, 0, 0,    '@' ) )._( '-' );       UT_EQ( ASTR("-"),   ms );
    ms.Clear();    ms._( 'x' );    ms._(Format::Tab( 2, 0, 0,    '@' ) )._( '-' );       UT_EQ( ASTR("x@-"), ms );

    // more standard tabs
    ms.Clear();
    ms._( ASTR("12") )._(Format::Tab( 2 ))
      ._(  '1' )._(Format::Tab( 2 ))
                ._(Format::Tab( 2 ))
      ._(  '@' );
    UT_EQ( ASTR("12  1   @"), ms );

    ms.Clear();
    ms._( ASTR("12") )._(Format::Tab( 2, 0, 0 ))
      ._(  '1' )._(Format::Tab( 2, 0, 0 ))
      ._(Format::Tab(2, 0, 0))
      ._(  '@' );
    UT_EQ( ASTR("121 @"), ms );


    ms.Clear();
    ms._( ASTR("12345") )._(Format::Tab( 5 ))
      ._( ASTR("1234") ) ._(Format::Tab( 5 ))
      ._( ASTR("123") )  ._(Format::Tab( 5 ))
      ._( ASTR("12") )   ._(Format::Tab( 5 ))
      ._(  '1' )   ._(Format::Tab( 5 ))
                   ._(Format::Tab( 5 ))
      ._(  '@' );
    UT_EQ( ASTR("12345     1234 123  12   1         @"), ms );

    ms.Clear();
    ms._( ASTR("12345") )._(Format::Tab( 5, 0, 0 ))
      ._( ASTR("1234") ) ._(Format::Tab( 5, 0, 0 ))
      ._( ASTR("123") )  ._(Format::Tab( 5, 0, 0 ))
      ._( ASTR("12") )   ._(Format::Tab( 5, 0, 0 ))
      ._(  '1' )   ._(Format::Tab( 5, 0, 0 ))
                   ._(Format::Tab( 5, 0, 0 ))
      ._(  '@' );
    UT_EQ( ASTR("123451234 123  12   1    @"), ms );

    ms.Clear();                   ms._(Format::Tab( 0 ))  ._( '-' );        UT_EQ( ASTR(" -")    , ms );
    ms.Clear();                   ms._(Format::Tab( 1 ))  ._( '-' );        UT_EQ( ASTR(" -")    , ms );
    ms.Clear();                   ms._(Format::Tab( 2 ))  ._( '-' );        UT_EQ( ASTR("  -")   , ms );
    ms.Clear();                   ms._(Format::Tab( 3 ))  ._( '-' );        UT_EQ( ASTR("   -")  , ms );
    ms.Clear();    ms._( 'x' );   ms._(Format::Tab( 1 ))  ._( '-' );        UT_EQ( ASTR("x -")   , ms );
    ms.Clear();    ms._( 'x' );   ms._(Format::Tab( 2 ))  ._( '-' );        UT_EQ( ASTR("x -")   , ms );
    ms.Clear();    ms._( 'x' );   ms._(Format::Tab( 3 ))  ._( '-' );        UT_EQ( ASTR("x  -")  , ms );

    // tabs with tab reference set (designed for multi line tabs)
    ms.Clear()._(ASTR("ABC")).NewLine();
    ms._( ASTR("12345") )._(Format::Tab( 5 , -1))
      ._( ASTR("1234") ) ._(Format::Tab( 5 , -1))
      ._( ASTR("123") )  ._(Format::Tab( 5 , -1))
      ._( ASTR("12") )   ._(Format::Tab( 5 , -1))
      ._(  '1' )   ._(Format::Tab( 5 , -1))
                   ._(Format::Tab( 5 , -1))
      ._(  '@' );
    UT_EQ( String64(ASTR("ABC")).NewLine()._( ASTR("12345     1234 123  12   1         @") ), ms );

    ms.Clear()._(ASTR("ABC")).NewLine();
    ms._( ASTR("12345") )._(Format::Tab( 5, -1, 0 ))
      ._( ASTR("1234") ) ._(Format::Tab( 5, -1, 0 ))
      ._( ASTR("123") )  ._(Format::Tab( 5, -1, 0 ))
      ._( ASTR("12") )   ._(Format::Tab( 5, -1, 0 ))
      ._(  '1' )   ._(Format::Tab( 5, -1, 0 ))
                   ._(Format::Tab( 5, -1, 0 ))
      ._(  '@' );
    UT_EQ( String32(ASTR("ABC")).NewLine()._( ASTR("123451234 123  12   1    @") )        , ms );


}

//--------------------------------------------------------------------------------------------------
//--- Test SearchReplace
//--------------------------------------------------------------------------------------------------
UT_METHOD( SearchAndReplace )
{
    UT_INIT();

    integer result;

    // search characters
    {
        String ms(ASTR("abcd abcd"));
        result= ms.IndexOf       ( '@', -5  );   UT_EQ(    -1,  result );
        result= ms.IndexOf       ( '@'      );   UT_EQ(    -1,  result );
        result= ms.IndexOf       ( '@', 5   );   UT_EQ(    -1,  result );
        result= ms.IndexOf       ( '@', 150 );   UT_EQ(    -1,  result );

        result= ms.IndexOf       ( 'a'      );   UT_EQ(    0,   result );
        result= ms.IndexOf       ( 'b'      );   UT_EQ(    1,   result );
        result= ms.IndexOf       ( 'c'      );   UT_EQ(    2,   result );

        result= ms.IndexOf       ( 'a', 0   );   UT_EQ(    0,   result );
        result= ms.IndexOf       ( 'b', 0   );   UT_EQ(    1,   result );
        result= ms.IndexOf       ( 'c', 0   );   UT_EQ(    2,   result );

        result= ms.IndexOf       ( 'a', 1   );   UT_EQ(    5,   result );
        result= ms.IndexOf       ( 'b', 1   );   UT_EQ(    1,   result );
        result= ms.IndexOf       ( 'c', 1   );   UT_EQ(    2,   result );

        result= ms.IndexOf       ( 'a', 2   );   UT_EQ(    5,   result );
        result= ms.IndexOf       ( 'b', 2   );   UT_EQ(    6,   result );
        result= ms.IndexOf       ( 'c', 2   );   UT_EQ(    2,   result );

        result= ms.IndexOf       ( 'a', 3   );   UT_EQ(    5,   result );
        result= ms.IndexOf       ( 'b', 3   );   UT_EQ(    6,   result );
        result= ms.IndexOf       ( 'c', 3   );   UT_EQ(    7,   result );

        result= ms.IndexOf       ( 'd', 7   );   UT_EQ(    8,   result );
        result= ms.IndexOf       ( 'd', 8   );   UT_EQ(    8,   result );
        result= ms.IndexOf       ( 'd', 9   );   UT_EQ(   -1,   result );

        result= ms.IndexOf<false>( '@', 5   );   UT_EQ(    -1,  result );

        result= ms.IndexOf<false>( 'a', 0   );   UT_EQ(    0,   result );
        result= ms.IndexOf<false>( 'b', 0   );   UT_EQ(    1,   result );
        result= ms.IndexOf<false>( 'c', 0   );   UT_EQ(    2,   result );

        result= ms.IndexOf<false>( 'a', 1   );   UT_EQ(    5,   result );
        result= ms.IndexOf<false>( 'b', 1   );   UT_EQ(    1,   result );
        result= ms.IndexOf<false>( 'c', 1   );   UT_EQ(    2,   result );

        result= ms.IndexOf<false>( 'a', 2   );   UT_EQ(    5,   result );
        result= ms.IndexOf<false>( 'b', 2   );   UT_EQ(    6,   result );
        result= ms.IndexOf<false>( 'c', 2   );   UT_EQ(    2,   result );

        result= ms.IndexOf<false>( 'a', 3   );   UT_EQ(    5,   result );
        result= ms.IndexOf<false>( 'b', 3   );   UT_EQ(    6,   result );
        result= ms.IndexOf<false>( 'c', 3   );   UT_EQ(    7,   result );

        result= ms.IndexOf<false>( 'd', 7   );   UT_EQ(    8,   result );
        result= ms.IndexOf<false>( 'd', 8   );   UT_EQ(    8,   result );
    }

    {
        AString ms(ASTR("0123456789"));
        result= ms.IndexOf       ( '1',    -5,   3 );   UT_EQ( -1,  result );
        result= ms.IndexOf       ( '1',     0,   1 );   UT_EQ( -1,  result );
        result= ms.IndexOf       ( '1',     0,   2 );   UT_EQ(  1,  result );
        result= ms.IndexOf       ( '2',     0,   2 );   UT_EQ( -1,  result );
        result= ms.IndexOf       ( '2',     1,   2 );   UT_EQ(  2,  result );
        result= ms.IndexOf       ( '2',     1,   3 );   UT_EQ(  2,  result );
        result= ms.IndexOf       ( '2',     2,   3 );   UT_EQ(  2,  result );
        result= ms.IndexOf       ( '2',     2,   1 );   UT_EQ(  2,  result );
        result= ms.IndexOf       ( '2',     2,   0 );   UT_EQ( -1,  result );
        result= ms.IndexOf       ( '2',     3,   0 );   UT_EQ( -1,  result );
        result= ms.IndexOf       ( '2',     3,   1 );   UT_EQ( -1,  result );
        result= ms.IndexOf       ( '2',   100,  20 );   UT_EQ( -1,  result );
        result= ms.IndexOf       ( '2',  -100, 102 );   UT_EQ( -1,  result );
        result= ms.IndexOf       ( '2',  -100, 103 );   UT_EQ(  2,  result );
        result= ms.IndexOf       ( '2',  -100, 202 );   UT_EQ(  2,  result );

        result= ms.IndexOf<false>( '1',     0,   1 );   UT_EQ( -1,  result );
        result= ms.IndexOf<false>( '1',     0,   2 );   UT_EQ(  1,  result );
        result= ms.IndexOf<false>( '2',     0,   2 );   UT_EQ( -1,  result );
        result= ms.IndexOf<false>( '2',     1,   2 );   UT_EQ(  2,  result );
        result= ms.IndexOf<false>( '2',     1,   3 );   UT_EQ(  2,  result );
        result= ms.IndexOf<false>( '2',     2,   3 );   UT_EQ(  2,  result );
        result= ms.IndexOf<false>( '2',     2,   1 );   UT_EQ(  2,  result );
        result= ms.IndexOf<false>( '2',     3,   1 );   UT_EQ( -1,  result );
    }

    // ------------------ search char backwards ------------------
    {
        AString ms(ASTR("abcd abcd"));
        result= ms.LastIndexOf       ( '@', -5  );   UT_EQ(    -1,  result );
        result= ms.LastIndexOf       ( '@'      );   UT_EQ(    -1,  result );
        result= ms.LastIndexOf       ( '@', 5   );   UT_EQ(    -1,  result );
        result= ms.LastIndexOf       ( '@', 150 );   UT_EQ(    -1,  result );

        result= ms.LastIndexOf       ( 'a'      );   UT_EQ(    5,   result );
        result= ms.LastIndexOf       ( 'b'      );   UT_EQ(    6,   result );
        result= ms.LastIndexOf       ( 'c'      );   UT_EQ(    7,   result );
        result= ms.LastIndexOf       ( 'd'      );   UT_EQ(    8,   result );

        result= ms.LastIndexOf       ( 'a', 0   );   UT_EQ(    0,   result );
        result= ms.LastIndexOf       ( 'b', 0   );   UT_EQ(   -1,   result );
        result= ms.LastIndexOf       ( 'c', 0   );   UT_EQ(   -1,   result );

        result= ms.LastIndexOf       ( 'a', 1   );   UT_EQ(    0,   result );
        result= ms.LastIndexOf       ( 'b', 1   );   UT_EQ(    1,   result );
        result= ms.LastIndexOf       ( 'c', 1   );   UT_EQ(   -1,   result );

        result= ms.LastIndexOf       ( 'a', 2   );   UT_EQ(    0,   result );
        result= ms.LastIndexOf       ( 'b', 2   );   UT_EQ(    1,   result );
        result= ms.LastIndexOf       ( 'c', 2   );   UT_EQ(    2,   result );

        result= ms.LastIndexOf       ( 'a', 3   );   UT_EQ(    0,   result );
        result= ms.LastIndexOf       ( 'b', 3   );   UT_EQ(    1,   result );
        result= ms.LastIndexOf       ( 'c', 3   );   UT_EQ(    2,   result );

        result= ms.LastIndexOf       ( 'd', 7   );   UT_EQ(    3,   result );
        result= ms.LastIndexOf       ( 'd', 8   );   UT_EQ(    8,   result );
        result= ms.LastIndexOf       ( 'd', 9   );   UT_EQ(    8,   result );
        result= ms.LastIndexOf       ( 'd', 10  );   UT_EQ(    8,   result );
        result= ms.LastIndexOf       ( 'd', 11  );   UT_EQ(    8,   result );
        result= ms.LastIndexOf       ( 'c', 11  );   UT_EQ(    7,   result );


        result= ms.LastIndexOf<false>( '@', 5   );   UT_EQ(    -1,  result );

        result= ms.LastIndexOf<false>( 'a', 0   );   UT_EQ(    0,   result );
        result= ms.LastIndexOf<false>( 'b', 0   );   UT_EQ(   -1,   result );
        result= ms.LastIndexOf<false>( 'c', 0   );   UT_EQ(   -1,   result );

        result= ms.LastIndexOf<false>( 'a', 1   );   UT_EQ(    0,   result );
        result= ms.LastIndexOf<false>( 'b', 1   );   UT_EQ(    1,   result );
        result= ms.LastIndexOf<false>( 'c', 1   );   UT_EQ(   -1,   result );

        result= ms.LastIndexOf<false>( 'a', 2   );   UT_EQ(    0,   result );
        result= ms.LastIndexOf<false>( 'b', 2   );   UT_EQ(    1,   result );
        result= ms.LastIndexOf<false>( 'c', 2   );   UT_EQ(    2,   result );

        result= ms.LastIndexOf<false>( 'a', 3   );   UT_EQ(    0,   result );
        result= ms.LastIndexOf<false>( 'b', 3   );   UT_EQ(    1,   result );
        result= ms.LastIndexOf<false>( 'c', 3   );   UT_EQ(    2,   result );

        result= ms.LastIndexOf<false>( 'd', 7   );   UT_EQ(    3,   result );
        result= ms.LastIndexOf<false>( 'd', 8   );   UT_EQ(    8,   result );
    }


    // ------------------ search one of several characters ------------------
    // non static version
    {
        String ms(ASTR("abcd abcde"));

        // search one of
        integer l= ms.Length();
        result= ms.IndexOfAny    <Inclusion::Include       >( ASTR("")           );    UT_EQ(  -1, result );
        result= ms.IndexOfAny    <Inclusion::Include       >( ASTR("x")          );    UT_EQ(  -1, result );
        result= ms.IndexOfAny    <Inclusion::Include       >( ASTR("xy")         );    UT_EQ(  -1, result );
        result= ms.IndexOfAny    <Inclusion::Include       >( ASTR("xyz")        );    UT_EQ(  -1, result );
        result= ms.IndexOfAny    <Inclusion::Include       >( ASTR("xyd")        );    UT_EQ(   3, result );
        result= ms.IndexOfAny    <Inclusion::Include       >( ASTR("d")          );    UT_EQ(   3, result );
        result= ms.IndexOfAny    <Inclusion::Include       >( ASTR("xyd") ,  -2  );    UT_EQ(   3, result );
        result= ms.IndexOfAny    <Inclusion::Include       >( ASTR("xyd") ,   4  );    UT_EQ(   8, result );
        result= ms.IndexOfAny    <Inclusion::Include       >( ASTR("xyd") ,  20  );    UT_EQ(  -1, result );
        result= ms.LastIndexOfAny<Inclusion::Include       >( ASTR("")           );    UT_EQ(  -1, result );
        result= ms.LastIndexOfAny<Inclusion::Include       >( ASTR("x")          );    UT_EQ(  -1, result );
        result= ms.LastIndexOfAny<Inclusion::Include       >( ASTR("xy")         );    UT_EQ(  -1, result );
        result= ms.LastIndexOfAny<Inclusion::Include       >( ASTR("xyz")        );    UT_EQ(  -1, result );
        result= ms.LastIndexOfAny<Inclusion::Include       >( ASTR("xyd")        );    UT_EQ(   8, result );
        result= ms.LastIndexOfAny<Inclusion::Include       >( ASTR("xyd") ,  -2  );    UT_EQ(  -1, result );
        result= ms.LastIndexOfAny<Inclusion::Include       >( ASTR("xyd") ,   2  );    UT_EQ(  -1, result );
        result= ms.LastIndexOfAny<Inclusion::Include       >( ASTR("xyd") ,   4  );    UT_EQ(   3, result );
        result= ms.LastIndexOfAny<Inclusion::Include       >( ASTR("a")   ,   4  );    UT_EQ(   0, result );
        result= ms.LastIndexOfAny<Inclusion::Include       >( ASTR("b")   ,   4  );    UT_EQ(   1, result );
        result= ms.LastIndexOfAny<Inclusion::Include       >( ASTR("ba")  ,   4  );    UT_EQ(   1, result );
        result= ms.LastIndexOfAny<Inclusion::Include       >( ASTR("xa")  ,   4  );    UT_EQ(   0, result );
        result= ms.LastIndexOfAny<Inclusion::Include       >( ASTR("xyd") ,  20  );    UT_EQ(   8, result );
        result= ms.LastIndexOfAny<Inclusion::Include       >( ASTR("d")   ,  20  );    UT_EQ(   8, result );
        result= ms.LastIndexOfAny<Inclusion::Include       >( ASTR("e")   ,  20  );    UT_EQ(   9, result );
        result= ms.IndexOfAny    <Inclusion::Include, false>( ASTR("x")          );    UT_EQ(  -1, result );
        result= ms.IndexOfAny    <Inclusion::Include, false>( ASTR("xy")         );    UT_EQ(  -1, result );
        result= ms.IndexOfAny    <Inclusion::Include, false>( ASTR("xyz")        );    UT_EQ(  -1, result );
        result= ms.IndexOfAny    <Inclusion::Include, false>( ASTR("xyd")        );    UT_EQ(   3, result );
        result= ms.IndexOfAny    <Inclusion::Include, false>( ASTR("d")          );    UT_EQ(   3, result );
        result= ms.IndexOfAny    <Inclusion::Include, false>( ASTR("xyd")  ,  4  );    UT_EQ(   8, result );
        result= ms.LastIndexOfAny<Inclusion::Include, false>( ASTR("xyd")  ,  2  );    UT_EQ(  -1, result );
        result= ms.LastIndexOfAny<Inclusion::Include, false>( ASTR("xyd")  ,  4  );    UT_EQ(   3, result );
        result= ms.LastIndexOfAny<Inclusion::Include, false>( ASTR("a")    ,  4  );    UT_EQ(   0, result );
        result= ms.LastIndexOfAny<Inclusion::Include, false>( ASTR("b")    ,  4  );    UT_EQ(   1, result );
        result= ms.LastIndexOfAny<Inclusion::Include, false>( ASTR("ba")   ,  4  );    UT_EQ(   1, result );
        result= ms.LastIndexOfAny<Inclusion::Include, false>( ASTR("xa")   ,  4  );    UT_EQ(   0, result );

        // search NOT one of
        result= ms.IndexOfAny    <Inclusion::Exclude       >( ASTR("")           );    UT_EQ(   0, result );
        result= ms.IndexOfAny    <Inclusion::Exclude       >( ASTR("x")          );    UT_EQ(   0, result );
        result= ms.IndexOfAny    <Inclusion::Exclude       >( ASTR("xy")         );    UT_EQ(   0, result );
        result= ms.IndexOfAny    <Inclusion::Exclude       >( ASTR("xyz")        );    UT_EQ(   0, result );
        result= ms.IndexOfAny    <Inclusion::Exclude       >( ASTR("a")          );    UT_EQ(   1, result );
        result= ms.IndexOfAny    <Inclusion::Exclude       >( ASTR("ba")         );    UT_EQ(   2, result );
        result= ms.IndexOfAny    <Inclusion::Exclude       >( ASTR("abc")        );    UT_EQ(   3, result );
        result= ms.IndexOfAny    <Inclusion::Exclude       >( ASTR("acb")        );    UT_EQ(   3, result );
        result= ms.IndexOfAny    <Inclusion::Exclude       >( ASTR("cba")        );    UT_EQ(   3, result );
        result= ms.IndexOfAny    <Inclusion::Exclude       >( ASTR("xcba")       );    UT_EQ(   3, result );
        result= ms.LastIndexOfAny<Inclusion::Exclude       >( ASTR("")           );    UT_EQ( l-1, result );
        result= ms.LastIndexOfAny<Inclusion::Exclude       >( ASTR("x")          );    UT_EQ( l-1, result );
        result= ms.LastIndexOfAny<Inclusion::Exclude       >( ASTR("xy")         );    UT_EQ( l-1, result );
        result= ms.LastIndexOfAny<Inclusion::Exclude       >( ASTR("e")          );    UT_EQ( l-2, result );
        result= ms.LastIndexOfAny<Inclusion::Exclude       >( ASTR("de")         );    UT_EQ( l-3, result );
        result= ms.LastIndexOfAny<Inclusion::Exclude       >( ASTR("cde")        );    UT_EQ( l-4, result );
        result= ms.LastIndexOfAny<Inclusion::Exclude       >( ASTR("ced")        );    UT_EQ( l-4, result );
        result= ms.LastIndexOfAny<Inclusion::Exclude       >( ASTR("ecd")        );    UT_EQ( l-4, result );
        result= ms.LastIndexOfAny<Inclusion::Exclude       >( ASTR("ecd")  ,  5  );    UT_EQ(   5, result );
        result= ms.LastIndexOfAny<Inclusion::Exclude       >( ASTR("ecd")  ,  4  );    UT_EQ(   4, result );
        result= ms.LastIndexOfAny<Inclusion::Exclude       >( ASTR("acd")  ,  3  );    UT_EQ(   1, result );
        result= ms.LastIndexOfAny<Inclusion::Exclude       >( ASTR("abc")  ,  2  );    UT_EQ(  -1, result );
        result= ms.LastIndexOfAny<Inclusion::Exclude       >( ASTR("xay")  ,  3  );    UT_EQ(   3, result );
        result= ms.LastIndexOfAny<Inclusion::Exclude       >( ASTR("d")    ,  3  );    UT_EQ(   2, result );
        result= ms.LastIndexOfAny<Inclusion::Exclude       >( ASTR("a")    ,  0  );    UT_EQ(  -1, result );
        result= ms.IndexOfAny    <Inclusion::Exclude ,false>( ASTR("x")          );    UT_EQ(   0, result );
        result= ms.IndexOfAny    <Inclusion::Exclude ,false>( ASTR("xy")         );    UT_EQ(   0, result );
        result= ms.IndexOfAny    <Inclusion::Exclude ,false>( ASTR("xyz")        );    UT_EQ(   0, result );
        result= ms.IndexOfAny    <Inclusion::Exclude ,false>( ASTR("a")          );    UT_EQ(   1, result );
        result= ms.IndexOfAny    <Inclusion::Exclude ,false>( ASTR("ba")         );    UT_EQ(   2, result );
        result= ms.IndexOfAny    <Inclusion::Exclude ,false>( ASTR("abc")        );    UT_EQ(   3, result );
        result= ms.IndexOfAny    <Inclusion::Exclude ,false>( ASTR("acb")        );    UT_EQ(   3, result );
        result= ms.IndexOfAny    <Inclusion::Exclude ,false>( ASTR("cba")        );    UT_EQ(   3, result );
        result= ms.IndexOfAny    <Inclusion::Exclude ,false>( ASTR("xcba")       );    UT_EQ(   3, result );
        result= ms.LastIndexOfAny<Inclusion::Exclude ,false>( ASTR("ecd")  ,  4  );    UT_EQ(   4, result );
        result= ms.LastIndexOfAny<Inclusion::Exclude ,false>( ASTR("acd")  ,  3  );    UT_EQ(   1, result );
        result= ms.LastIndexOfAny<Inclusion::Exclude ,false>( ASTR("abc")  ,  2  );    UT_EQ(  -1, result );
        result= ms.LastIndexOfAny<Inclusion::Exclude ,false>( ASTR("xay")  ,  3  );    UT_EQ(   3, result );
        result= ms.LastIndexOfAny<Inclusion::Exclude ,false>( ASTR("d")    ,  3  );    UT_EQ(   2, result );
        result= ms.LastIndexOfAny<Inclusion::Exclude ,false>( ASTR("a")    ,  0  );    UT_EQ(  -1, result );
    }

    // ------------------ search one of several characters TString version  ------------------
    // non static version
    {
        TString ms(ASTR("abcd abcde"));
        // search one of
        result= ms.IndexOfAny<Inclusion::Include       >( ASTR("")            );    UT_EQ(  -1, result );
        result= ms.IndexOfAny<Inclusion::Include       >( ASTR("x")           );    UT_EQ(  -1, result );
        result= ms.IndexOfAny<Inclusion::Include       >( ASTR("xy")          );    UT_EQ(  -1, result );
        result= ms.IndexOfAny<Inclusion::Include       >( ASTR("xyz")         );    UT_EQ(  -1, result );
        result= ms.IndexOfAny<Inclusion::Include       >( ASTR("xyd")         );    UT_EQ(   3, result );
        result= ms.IndexOfAny<Inclusion::Include       >( ASTR("d")           );    UT_EQ(   3, result );
        result= ms.IndexOfAny<Inclusion::Include       >( ASTR("xyd")   , -2  );    UT_EQ(   3, result );
        result= ms.IndexOfAny<Inclusion::Include       >( ASTR("xyd")   ,  4  );    UT_EQ(   8, result );
        result= ms.IndexOfAny<Inclusion::Include       >( ASTR("xyd")   , 20  );    UT_EQ(  -1, result );
        result= ms.IndexOfAny<Inclusion::Include, false>( ASTR("x")           );    UT_EQ(  -1, result );
        result= ms.IndexOfAny<Inclusion::Include, false>( ASTR("xy")          );    UT_EQ(  -1, result );
        result= ms.IndexOfAny<Inclusion::Include, false>( ASTR("xyz")         );    UT_EQ(  -1, result );
        result= ms.IndexOfAny<Inclusion::Include, false>( ASTR("xyd")         );    UT_EQ(   3, result );
        result= ms.IndexOfAny<Inclusion::Include, false>( ASTR("d")           );    UT_EQ(   3, result );
        result= ms.IndexOfAny<Inclusion::Include, false>( ASTR("xyd")   ,  4  );    UT_EQ(   8, result );
        result= ms.IndexOfAny<Inclusion::Exclude       >( ASTR("")            );    UT_EQ(   0, result );
        result= ms.IndexOfAny<Inclusion::Exclude       >( ASTR("x")           );    UT_EQ(   0, result );
        result= ms.IndexOfAny<Inclusion::Exclude       >( ASTR("xy")          );    UT_EQ(   0, result );
        result= ms.IndexOfAny<Inclusion::Exclude       >( ASTR("xyz")         );    UT_EQ(   0, result );
        result= ms.IndexOfAny<Inclusion::Exclude       >( ASTR("a")           );    UT_EQ(   1, result );
        result= ms.IndexOfAny<Inclusion::Exclude       >( ASTR("ba")          );    UT_EQ(   2, result );
        result= ms.IndexOfAny<Inclusion::Exclude       >( ASTR("abc")         );    UT_EQ(   3, result );
        result= ms.IndexOfAny<Inclusion::Exclude       >( ASTR("acb")         );    UT_EQ(   3, result );
        result= ms.IndexOfAny<Inclusion::Exclude       >( ASTR("cba")         );    UT_EQ(   3, result );
        result= ms.IndexOfAny<Inclusion::Exclude       >( ASTR("xcba")        );    UT_EQ(   3, result );
        result= ms.IndexOfAny<Inclusion::Exclude, false>( ASTR("x")           );    UT_EQ(   0, result );
        result= ms.IndexOfAny<Inclusion::Exclude, false>( ASTR("xy")          );    UT_EQ(   0, result );
        result= ms.IndexOfAny<Inclusion::Exclude, false>( ASTR("xyz")         );    UT_EQ(   0, result );
        result= ms.IndexOfAny<Inclusion::Exclude, false>( ASTR("a")           );    UT_EQ(   1, result );
        result= ms.IndexOfAny<Inclusion::Exclude, false>( ASTR("ba")          );    UT_EQ(   2, result );
        result= ms.IndexOfAny<Inclusion::Exclude, false>( ASTR("abc")         );    UT_EQ(   3, result );
        result= ms.IndexOfAny<Inclusion::Exclude, false>( ASTR("acb")         );    UT_EQ(   3, result );
        result= ms.IndexOfAny<Inclusion::Exclude, false>( ASTR("cba")         );    UT_EQ(   3, result );
        result= ms.IndexOfAny<Inclusion::Exclude, false>( ASTR("xcba")        );    UT_EQ(   3, result );
    }

    // search nullptr, empty string
    {
        TString ms(ASTR("abcd abcd"));
        result= ms.IndexOf( nullptr              );    UT_EQ( -1, result );
        result= ms.IndexOf( nullptr,          5  );    UT_EQ( -1, result );
        result= ms.IndexOf( nullptr,         50  );    UT_EQ( -1, result );
        result= ms.IndexOf( nullptr,       -  5  );    UT_EQ( -1, result );
        result= ms.IndexOf( ASTR(""),      -  5  );    UT_EQ(  0, result );
        result= ms.IndexOf( ASTR(""),         0  );    UT_EQ(  0, result );
        result= ms.IndexOf( ASTR(""),         4  );    UT_EQ(  4, result );
        result= ms.IndexOf( ASTR(""),       100  );    UT_EQ( -1, result );
    }

    // search
    {
        TString ms(ASTR("abcd abcd"));
        result= ms.IndexOf                        ( ASTR("abcd")       );  UT_EQ( result,   0 );
        result= ms.IndexOf                        ( ASTR("b")          );  UT_EQ( result,   1 );
        result= ms.IndexOf                        ( ASTR(" abcd")      );  UT_EQ( result,   4 );
        result= ms.IndexOf                        ( ASTR("abcd"),     1);  UT_EQ( result,   5 );
        result= ms.IndexOf                        ( ASTR("abcd"),   - 1);  UT_EQ( result,   0 );
        result= ms.IndexOf                        ( ASTR("xyz"),    -10);  UT_EQ( result,  -1 );
        result= ms.IndexOf<false, Case::Sensitive>( ASTR("abcd")       );  UT_EQ( result,   0 );
        result= ms.IndexOf<false, Case::Sensitive>( ASTR("b")          );  UT_EQ( result,   1 );
        result= ms.IndexOf<false, Case::Sensitive>( ASTR(" abcd")      );  UT_EQ( result,   4 );
        result= ms.IndexOf<false, Case::Sensitive>( ASTR("abcd"),     1);  UT_EQ( result,   5 );
    }

    // ignore case
    {
        AString ms( ASTR("Hello A-Worx utilXXX") );
        ms.DeleteEnd(3);
        result= ms.IndexOf<true , Case::Ignore   >( ASTR("a-worx"),    0 ); UT_EQ( result,  6 );
        result= ms.IndexOf<true , Case::Ignore   >( ASTR("a-worx"),    1 ); UT_EQ( result,  6 );
        result= ms.IndexOf<true , Case::Ignore   >( ASTR("a-worx"),  -10 ); UT_EQ( result,  6 );
        result= ms.IndexOf<true , Case::Ignore   >( ASTR("a-worx"),    6 ); UT_EQ( result,  6 );
        result= ms.IndexOf<true , Case::Ignore   >( ASTR("a-worx"),    7 ); UT_EQ( result, -1 );
        result= ms.IndexOf<true , Case::Ignore   >( ASTR("a-worx"),  100 ); UT_EQ( result, -1 );
        result= ms.IndexOf<true , Case::Ignore   >( ASTR("hel"),       0 ); UT_EQ( result,  0 );
        result= ms.IndexOf<true , Case::Ignore   >( ASTR("hel"),       1 ); UT_EQ( result, -1 );
        result= ms.IndexOf<true , Case::Ignore   >( ASTR("util"),      1 ); UT_EQ( result, 13 );
        result= ms.IndexOf<true , Case::Ignore   >( ASTR("UTIL"),      5 ); UT_EQ( result, 13 );
        result= ms.IndexOf<true , Case::Ignore   >( ASTR("UTIL"),     13 ); UT_EQ( result, 13 );
        result= ms.IndexOf<true , Case::Ignore   >( ASTR("UTIL"),     14 ); UT_EQ( result, -1 );
        result= ms.IndexOf<false, Case::Ignore   >( ASTR("a-worx"),    0 ); UT_EQ( result,  6 );
        result= ms.IndexOf<false, Case::Ignore   >( ASTR("a-worx"),    1 ); UT_EQ( result,  6 );
        result= ms.IndexOf<false, Case::Ignore   >( ASTR("a-worx"),    6 ); UT_EQ( result,  6 );
        result= ms.IndexOf<false, Case::Ignore   >( ASTR("a-worx"),    7 ); UT_EQ( result, -1 );
        result= ms.IndexOf<false, Case::Ignore   >( ASTR("hel"),       0 ); UT_EQ( result,  0 );
        result= ms.IndexOf<false, Case::Ignore   >( ASTR("hel"),       1 ); UT_EQ( result, -1 );
        result= ms.IndexOf<false, Case::Ignore   >( ASTR("util"),      1 ); UT_EQ( result, 13 );
        result= ms.IndexOf<false, Case::Ignore   >( ASTR("UTIL"),      5 ); UT_EQ( result, 13 );
        result= ms.IndexOf<false, Case::Ignore   >( ASTR("UTIL"),     13 ); UT_EQ( result, 13 );
    }

    // IndexOfFirstDifference
    {
        String as(ASTR("abcdef"));
        UT_EQ( 6, as.IndexOfFirstDifference(ASTR("abcdef")) );
        UT_EQ( 5, as.IndexOfFirstDifference(ASTR("abcde")) );
        UT_EQ( 6, as.IndexOfFirstDifference(ASTR("abcdefg")) );

        UT_EQ( 0, as.IndexOfFirstDifference(ASTR("123")) );
        UT_EQ( 0, as.IndexOfFirstDifference(ASTR("123"), Case::Ignore   ) );
        UT_EQ( 0, as.IndexOfFirstDifference(ASTR("123"), Case::Ignore, 2) );

        UT_EQ( 3, as.IndexOfFirstDifference(ASTR("abc")) );
        UT_EQ( 3, as.IndexOfFirstDifference(ASTR("abc"), Case::Ignore   ) );
        UT_EQ( 0, as.IndexOfFirstDifference(ASTR("abc"), Case::Ignore, 1) );
        UT_EQ( 0, as.IndexOfFirstDifference( ASTR("bc"), Case::Ignore, 0) );
        UT_EQ( 2, as.IndexOfFirstDifference( ASTR("bc"), Case::Ignore, 1) );
        UT_EQ( 1, as.IndexOfFirstDifference( ASTR("bd"), Case::Ignore, 1) );


        UT_EQ( 3, as.IndexOfFirstDifference(ASTR("ABC"), Case::Ignore   ) );
        UT_EQ( 0, as.IndexOfFirstDifference(ASTR("ABC"), Case::Ignore, 1) );
        UT_EQ( 0, as.IndexOfFirstDifference( ASTR("BC"), Case::Ignore, 0) );
        UT_EQ( 2, as.IndexOfFirstDifference( ASTR("BC"), Case::Ignore, 1) );
        UT_EQ( 1, as.IndexOfFirstDifference( ASTR("BD"), Case::Ignore, 1) );


        UT_EQ( 0, as.IndexOfFirstDifference(ASTR("ABC")) );
        UT_EQ( 0, as.IndexOfFirstDifference(ASTR("ABC"), Case::Sensitive   ) );
        UT_EQ( 0, as.IndexOfFirstDifference(ASTR("ABC"), Case::Sensitive, 1) );
        UT_EQ( 0, as.IndexOfFirstDifference( ASTR("BC"), Case::Sensitive, 0) );
        UT_EQ( 0, as.IndexOfFirstDifference( ASTR("BC"), Case::Sensitive, 1) );
        UT_EQ( 0, as.IndexOfFirstDifference( ASTR("BD"), Case::Sensitive, 1) );
    }

    // replace nullptr
    {
        AString ms(ASTR("Hello"));
        ms.SearchAndReplace( NullString, NullString );     UT_EQ( ASTR("Hello") , ms );
        ms.SearchAndReplace( ASTR("l"),        NullString );     UT_EQ( ASTR("Heo")   , ms );
        ms.SearchAndReplace( NullString, ASTR("xx")       );     UT_EQ( ASTR("Heo")   , ms );
    }

    // SearchAndReplace nullptr
    {
        AString ms(ASTR("Hello"));
        result= ms.SearchAndReplace( NullString,  NullString );  UT_EQ( ASTR("Hello") , ms );   UT_EQ( 0, result );
        result= ms.SearchAndReplace( ASTR("l"),         NullString );  UT_EQ( ASTR("Heo")   , ms );   UT_EQ( 2, result );
        result= ms.SearchAndReplace( NullString,  ASTR("xx")   );      UT_EQ( ASTR("Heo")   , ms );   UT_EQ( 0, result );
    }

    // replace
    {
        AString ms(ASTR("Hello W!"));
        result= ms.SearchAndReplace( ASTR("W!"),  ASTR("world!")   );    UT_EQ( ASTR("Hello world!")       ,ms );      UT_EQ( 1, result );
        result= ms.SearchAndReplace( ASTR(" "),   ASTR("* *")      );    UT_EQ( ASTR("Hello* *world!")     ,ms );      UT_EQ( 1, result );
        result= ms.SearchAndReplace( ASTR("*"),   ASTR("#"), 0, 0  );    UT_EQ( ASTR("Hello* *world!")     ,ms );      UT_EQ( 0, result );
        result= ms.SearchAndReplace( ASTR("*"),   ASTR("#"), 0, 1  );    UT_EQ( ASTR("Hello# *world!")     ,ms );      UT_EQ( 1, result );
        result= ms.SearchAndReplace( ASTR("*"),   ASTR("#")        );    UT_EQ( ASTR("Hello# #world!")     ,ms );      UT_EQ( 1, result );
        result= ms.SearchAndReplace( ASTR("#"),   ASTR("$$$")      );    UT_EQ( ASTR("Hello$$$ $$$world!") ,ms );      UT_EQ( 2, result );
        result= ms.SearchAndReplace( ASTR("$$$"), ASTR("*")        );    UT_EQ( ASTR("Hello* *world!")     ,ms );      UT_EQ( 2, result );
        result= ms.SearchAndReplace( ASTR("*"),   ASTR("")         );    UT_EQ( ASTR("Hello world!")       ,ms );      UT_EQ( 2, result );
    }

    // replace one space by two spaces in a string of spaces
    {
        AString ms(ASTR("     "));
        result= ms.SearchAndReplace( ASTR(" "),   ASTR("  ")  );    UT_EQ( ASTR("          "), ms  );        UT_EQ( 5, result );
    }

    // replace ignoreCase
    {
        AString ms;
        ms.Clear()._(ASTR("ABC")); result= ms.SearchAndReplace( ASTR("abc"),    ASTR("xxx"),0, 1, Case::Sensitive );    UT_EQ( ASTR("ABC"), ms );        UT_EQ( 0, result );
        ms.Clear()._(ASTR("ABC")); result= ms.SearchAndReplace( ASTR("abc"),    ASTR("xxx"),0, 1, Case::Ignore    );    UT_EQ( ASTR("xxx"), ms );        UT_EQ( 1, result );
        ms.Clear()._(ASTR("ABC")); result= ms.SearchAndReplace( ASTR("ABC"),    ASTR("xxx"),0, 1, Case::Ignore    );    UT_EQ( ASTR("xxx"), ms );        UT_EQ( 1, result );
        ms.Clear()._(ASTR("abc"));         ms.SearchAndReplace( ASTR("ABC"),    ASTR("xxx"),0, 1, Case::Sensitive );    UT_EQ( ASTR("abc"), ms );
        ms.Clear()._(ASTR("abc"));         ms.SearchAndReplace( ASTR("ABC"),    ASTR("xxx"),0, 1, Case::Ignore    );    UT_EQ( ASTR("xxx"), ms );
        ms.Clear()._(ASTR("abc"));         ms.SearchAndReplace( ASTR("abc"),    ASTR("xxx"),0, 1, Case::Ignore    );    UT_EQ( ASTR("xxx"), ms );
    }

}


//--------------------------------------------------------------------------------------------------
//--- Test Trim
//--------------------------------------------------------------------------------------------------
UT_METHOD( Count )
{
    UT_INIT();

    // Count
    String str;

    str= ASTR("");
    UT_EQ( 0, str.Count( NullString ) );
    UT_EQ( 0, str.Count( ASTR("")  ) );
    UT_EQ( 0, str.Count( ASTR("x") ) );

    UT_EQ( 0, str.CountChar( 'x' ) );
    UT_EQ( 0, str.CountChar( '\0') );

    str= ASTR("abcdef");
    UT_EQ( 1, str.Count( ASTR("a")  ) );   UT_EQ( 1, str.CountChar( 'a'  ) );  UT_EQ( 1, str.Count<false ALIB_COMMA Case::Sensitive >( ASTR("a")  ) );   UT_EQ( 1, str.CountChar<false>( 'a'  ) );
    UT_EQ( 1, str.Count( ASTR("b")  ) );   UT_EQ( 1, str.CountChar( 'b'  ) );  UT_EQ( 1, str.Count<false ALIB_COMMA Case::Sensitive >( ASTR("b")  ) );   UT_EQ( 1, str.CountChar<false>( 'b'  ) );
    UT_EQ( 1, str.Count( ASTR("e")  ) );   UT_EQ( 1, str.CountChar( 'e'  ) );  UT_EQ( 1, str.Count<false ALIB_COMMA Case::Sensitive >( ASTR("e")  ) );   UT_EQ( 1, str.CountChar<false>( 'e'  ) );
    UT_EQ( 1, str.Count( ASTR("f")  ) );   UT_EQ( 1, str.CountChar( 'f'  ) );  UT_EQ( 1, str.Count<false ALIB_COMMA Case::Sensitive >( ASTR("f")  ) );   UT_EQ( 1, str.CountChar<false>( 'f'  ) );
    UT_EQ( 0, str.Count( ASTR("x")  ) );   UT_EQ( 0, str.CountChar( 'x'  ) );  UT_EQ( 0, str.Count<false ALIB_COMMA Case::Sensitive >( ASTR("x")  ) );   UT_EQ( 0, str.CountChar<false>( 'x'  ) );
    UT_EQ( 0, str.Count( ASTR("")   ) );                                       UT_EQ( 0, str.Count<false ALIB_COMMA Case::Sensitive >( ASTR("")   ) );
    UT_EQ( 1, str.Count( ASTR("ab") ) );                                       UT_EQ( 1, str.Count<false ALIB_COMMA Case::Sensitive >( ASTR("ab") ) );
    UT_EQ( 1, str.Count( ASTR("bc") ) );                                       UT_EQ( 1, str.Count<false ALIB_COMMA Case::Sensitive >( ASTR("bc") ) );
    UT_EQ( 1, str.Count( ASTR("ef") ) );                                       UT_EQ( 1, str.Count<false ALIB_COMMA Case::Sensitive >( ASTR("ef") ) );

    UT_EQ( 1, str.Count( ASTR("ab"), ASTR("X") ) );                                  UT_EQ( 1, str.Count<false ALIB_COMMA Case::Sensitive >( ASTR("ab"), ASTR("X") ) );
    UT_EQ( 0, str.Count( ASTR("ab"), ASTR("c") ) );                                  UT_EQ( 0, str.Count<false ALIB_COMMA Case::Sensitive >( ASTR("ab"), ASTR("c") ) );
    UT_EQ( 0, str.Count( ASTR("ab"), ASTR("")  ) );                                  UT_EQ( 0, str.Count<false ALIB_COMMA Case::Sensitive >( ASTR("ab"), ASTR("")  ) );

    str= ASTR("abcabcabcabc");
    UT_EQ( 1, str.Count( ASTR("abcabcabcabc")       ) );                                        UT_EQ( 1, str.Count<false ALIB_COMMA Case::Sensitive >( ASTR("abcabcabcabc")    ) );
    UT_EQ( 0, str.Count( ASTR("abcabcabcabcX")      ) );                                        UT_EQ( 0, str.Count<false ALIB_COMMA Case::Sensitive >( ASTR("abcabcabcabcX")   ) );
    UT_EQ( 1, str.Count(  ASTR("bcabcabcabc")       ) );                                        UT_EQ( 1, str.Count<false ALIB_COMMA Case::Sensitive >(  ASTR("bcabcabcabc")    ) );
    UT_EQ( 1, str.Count( ASTR("abcabcabcab")        ) );                                        UT_EQ( 1, str.Count<false ALIB_COMMA Case::Sensitive >( ASTR("abcabcabcab")     ) );
    UT_EQ( 1, str.Count(  ASTR("bcabcabcab")        ) );                                        UT_EQ( 1, str.Count<false ALIB_COMMA Case::Sensitive >(  ASTR("bcabcabcab")     ) );
    UT_EQ( 1, str.Count(   ASTR("cabcabc")          ) );                                        UT_EQ( 1, str.Count<false ALIB_COMMA Case::Sensitive >(   ASTR("cabcabc")       ) );
    UT_EQ( 4, str.Count(   ASTR("abc")              ) );                                        UT_EQ( 4, str.Count<false ALIB_COMMA Case::Sensitive >(   ASTR("abc")           ) );
    UT_EQ( 3, str.Count(   ASTR("cab")              ) );                                        UT_EQ( 3, str.Count<false ALIB_COMMA Case::Sensitive >(   ASTR("cab")           ) );
    UT_EQ( 4, str.Count(   ASTR("ab")               ) );                                        UT_EQ( 4, str.Count<false ALIB_COMMA Case::Sensitive >(   ASTR("ab")            ) );
    UT_EQ( 4, str.Count(   ASTR("a")                ) ); UT_EQ( 4, str.CountChar('a'        ) );  UT_EQ( 4, str.Count<false ALIB_COMMA Case::Sensitive >(   ASTR("a")             ) ); UT_EQ( 4, str.CountChar<false>(   'a'          ) );
    UT_EQ( 0, str.Count(   ASTR("a")  ,ASTR("b")    ) ); UT_EQ( 0, str.CountChar('a', 'b', 0) );  UT_EQ( 0, str.Count<false ALIB_COMMA Case::Sensitive >(   ASTR("a") , ASTR("b") ) ); UT_EQ( 0, str.CountChar<false>(   'a' ,'b' , 0 ) );
    UT_EQ( 4, str.Count(   ASTR("a")  ,ASTR("c")    ) ); UT_EQ( 4, str.CountChar('a', 'c', 0) );  UT_EQ( 4, str.Count<false ALIB_COMMA Case::Sensitive >(   ASTR("a") , ASTR("c") ) ); UT_EQ( 4, str.CountChar<false>(   'a' ,'c' , 0 ) );
    UT_EQ( 1, str.Count(   ASTR("ab") ,ASTR("ca")   ) ); UT_EQ( 1, str.CountChar('c', 'a', 0) );  UT_EQ( 1, str.Count<false ALIB_COMMA Case::Sensitive >(   ASTR("ab") ,ASTR("ca")) ); UT_EQ( 1, str.CountChar<false>(   'c' ,'a' , 0 ) );
}

//--------------------------------------------------------------------------------------------------
//--- Test Trim
//--------------------------------------------------------------------------------------------------
UT_METHOD( Trim )
{
    UT_INIT();

    AString ms;

    // ------------------ TrimAt ------------------
    {
        for (int i= -2; i < 6; i++ )
        {
            ms.Clear()._(ASTR("abc")).TrimAt( i );
            UT_EQ( ASTR("abc"), ms );
        }

        ms.Clear()._(ASTR(" abc")).TrimAt( -1 );        UT_EQ( ASTR(" abc")        , ms );
        ms.Clear()._(ASTR(" abc")).TrimAt(  0 );        UT_EQ( ASTR("abc")         , ms );
        ms.Clear()._(ASTR(" abc")).TrimAt(  1 );        UT_EQ( ASTR(" abc")        , ms );

        ms.Clear()._(ASTR("abc ")).TrimAt( -1 );        UT_EQ( ASTR("abc ")        , ms );
        ms.Clear()._(ASTR("abc ")).TrimAt(  0 );        UT_EQ( ASTR("abc ")        , ms );
        ms.Clear()._(ASTR("abc ")).TrimAt(  2 );        UT_EQ( ASTR("abc ")        , ms );
        ms.Clear()._(ASTR("abc ")).TrimAt(  3 );        UT_EQ( ASTR("abc")         , ms );
        ms.Clear()._(ASTR("abc ")).TrimAt(  4 );        UT_EQ( ASTR("abc ")        , ms );

        ms.Clear()._(ASTR("a bc")).TrimAt( -1 );        UT_EQ( ASTR("a bc")        , ms );
        ms.Clear()._(ASTR("a bc")).TrimAt(  0 );        UT_EQ( ASTR("a bc")        , ms );
        ms.Clear()._(ASTR("a bc")).TrimAt(  1 );        UT_EQ( ASTR("abc")         , ms );
        ms.Clear()._(ASTR("a bc")).TrimAt(  2 );        UT_EQ( ASTR("a bc")        , ms );
        ms.Clear()._(ASTR("a bc")).TrimAt(  3 );        UT_EQ( ASTR("a bc")        , ms );

        ms.Clear()._(ASTR("a  c")).TrimAt( -1 );        UT_EQ( ASTR("a  c")        , ms );
        ms.Clear()._(ASTR("a  c")).TrimAt(  0 );        UT_EQ( ASTR("a  c")        , ms );
        ms.Clear()._(ASTR("a  c")).TrimAt(  1 );        UT_EQ( ASTR("ac")          , ms );
        ms.Clear()._(ASTR("a  c")).TrimAt(  2 );        UT_EQ( ASTR("ac")          , ms );
        ms.Clear()._(ASTR("a  c")).TrimAt(  3 );        UT_EQ( ASTR("a  c")        , ms );

        ms.Clear()._(ASTR(" \t abc \t ")).TrimStart();   UT_EQ( ASTR("abc \t ")     , ms );
                                 ms.TrimEnd();  UT_EQ( ASTR("abc")         , ms );
    }

    // ------------------ Trim ------------------
    {
        ms.Clear()._(ASTR("")          ).Trim();        UT_EQ( ASTR("")            , ms );
        ms.Clear()._(' '         ).Trim();        UT_EQ( ASTR("")            , ms );
        ms.Clear()._(ASTR("  ")        ).Trim();        UT_EQ( ASTR("")            , ms );
        ms.Clear()._(ASTR("abc")       ).Trim();        UT_EQ( ASTR("abc")         , ms );
        ms.Clear()._(ASTR("abc")       ).Trim();        UT_EQ( ASTR("abc")         , ms );
        ms.Clear()._(ASTR(" abc")      ).Trim();        UT_EQ( ASTR("abc")         , ms );
        ms.Clear()._(ASTR("  abc")     ).Trim();        UT_EQ( ASTR("abc")         , ms );
        ms.Clear()._(ASTR(" \tabc")    ).Trim();        UT_EQ( ASTR("abc")         , ms );
        ms.Clear()._(ASTR("\t abc")    ).Trim();        UT_EQ( ASTR("abc")         , ms );
        ms.Clear()._(ASTR("abc\r\n")   ).Trim();        UT_EQ( ASTR("abc")         , ms );
        ms.Clear()._(ASTR("\tabc\r\n") ).Trim();        UT_EQ( ASTR("abc")         , ms );
        ms.Clear()._(ASTR("\tabc\rx\n") ).Trim();       UT_EQ( ASTR("abc\rx")      , ms );
    }
}


//--------------------------------------------------------------------------------------------------
//--- Test Compare
//--------------------------------------------------------------------------------------------------
UT_METHOD( Compare )
{
    UT_INIT();

    // null string comparison
    character*   nullCString= nullptr;

    AString aString;
    UT_TRUE( aString.CompareTo( nullCString) == 0 );
    UT_TRUE( aString.Equals( nullCString) );

    aString= ASTR("");
    UT_TRUE( aString.CompareTo( nullCString) != 0 );
    UT_TRUE( !aString.Equals( nullCString) );

    aString= nullCString;
    UT_TRUE( aString.CompareTo( nullCString) == 0 );
    UT_TRUE( aString.Equals( nullCString) );


    // compare/region
    AString ms;

    int result;
    int resultX;
    int intMaxValue= std::numeric_limits<int>::max();
    basic_string<character> t; t= ASTR("abcde");
    ms._( t );
    result= ms.CompareTo( t      );                          resultX= t.compare( t );                            UT_EQ( resultX,    result );
    result= ms.CompareTo( basic_string<character>(t).append(ASTR("x"))       );     resultX= t.compare( basic_string<character>(t).append(ASTR("x")) );        UT_EQ( resultX,    result );
    result= ms.CompareTo( t.substr (0, t.size() -1 )  );     resultX= t.compare( t.substr(0, t.size() -1 ) );    UT_EQ( resultX,    result );
    result= ms.CompareTo<true, Case::Sensitive>( ASTR("pad") + t,  3            );    UT_EQ(  0,     result );
    result= ms.CompareTo<true, Case::Sensitive>( ASTR("pad") + t,  3, 2         );    UT_EQ(  1,     result );
    result= ms.CompareTo<true, Case::Sensitive>( ASTR("pad") + t,  3, 100, 0, 4 );    UT_EQ( -1,     result );
    result= ms.CompareTo<true, Case::Sensitive>( ASTR("pad") + t,  3, 100, 0, 5 );    UT_EQ(  0,     result );
    result= ms.CompareTo<true, Case::Sensitive>(         t,  2,2,   2,2   );    UT_EQ(  0,     result );
    result= ms.CompareTo<true, Case::Sensitive>(         t,  2,2,   2,1   );    UT_EQ( -1,     result );
    result= ms.CompareTo<true, Case::Sensitive>(         t,  2,2,   2,3   );    UT_EQ(  1,     result );
    result= ms.CompareTo<true, Case::Sensitive>(         t,  -2,5,   -2, 5 );                           UT_EQ( 0,   result );
    result= ms.CompareTo<true, Case::Sensitive>(         t,   2, intMaxValue,    2, intMaxValue );      UT_EQ( 0,   result );
    result= ms.CompareTo<true, Case::Sensitive>(         t,  -2, intMaxValue,   -2, intMaxValue );      UT_EQ( 0,   result );
    result= ms.CompareTo<true, Case::Sensitive>(         t,  -2, 100,           -2, 99 );               UT_EQ( 0,   result );
    result= ms.CompareTo<true, Case::Sensitive>(         t,  -2, 5,             -2, intMaxValue );      UT_EQ( 1,   result );
    result= ms.CompareTo<true, Case::Sensitive>(         t,  -2, intMaxValue,   -2, 5 );                UT_EQ( -1,  result );


    result= ms.CompareTo<false, Case::Sensitive>( ASTR("pad") + t,  3, 2         );    UT_EQ(  1,     result );
    result= ms.CompareTo<false, Case::Sensitive>( ASTR("pad") + t,  3, 5         );    UT_EQ(  0,     result );
    result= ms.CompareTo<false, Case::Sensitive>( ASTR("pad") + t,  3, 6         );    UT_EQ(  -1,    result );
    result= ms.CompareTo<false, Case::Sensitive>(         t,  2,2,   2,2   );    UT_EQ(  0,     result );
    result= ms.CompareTo<false, Case::Sensitive>(         t,  2,2,   2,1   );    UT_EQ( -1,     result );
    result= ms.CompareTo<false, Case::Sensitive>(         t,  2,2,   2,3   );    UT_EQ(  1,     result );


    // greater/smaller strings
    int scRes;
    basic_string<character> greater=  ASTR("x");
    basic_string<character> greater2= ASTR("abcdef");
    basic_string<character> smaller=  ASTR("aaa");
    basic_string<character> smaller2= ASTR("abcd");
    result= ms.CompareTo( greater  );   scRes= t.compare( greater  );    UT_TRUE( (result==0 && scRes==0) || (result<0 && scRes<0) || (result>0 && scRes>0)   );
    result= ms.CompareTo( greater2 );   scRes= t.compare( greater2 );    UT_TRUE( (result==0 && scRes==0) || (result<0 && scRes<0) || (result>0 && scRes>0)   );
    result= ms.CompareTo( smaller  );   scRes= t.compare( smaller  );    UT_TRUE( (result==0 && scRes==0) || (result<0 && scRes<0) || (result>0 && scRes>0)   );
    result= ms.CompareTo( smaller2 );   scRes= t.compare( smaller2 );    UT_TRUE( (result==0 && scRes==0) || (result<0 && scRes<0) || (result>0 && scRes>0)   );
    UT_TRUE ( ms < greater  );
    UT_TRUE ( ms < greater2 );
    UT_TRUE ( ms > smaller  );
    UT_TRUE ( ms > smaller2 );
    UT_FALSE( ms > greater  );
    UT_FALSE( ms > greater2 );
    UT_FALSE( ms < smaller  );
    UT_FALSE( ms < smaller2 );
    UT_TRUE ( ms == ms );
    UT_TRUE ( ms != greater );
    UT_FALSE( ms < ms );
    UT_FALSE( ms > ms );

    // ignore case
    basic_string<character> ABCDE= ASTR("ABCDE");
    result= ms.CompareTo                         ( ABCDE               );    UT_TRUE( result >  0 );
    result= ms.CompareTo<true, Case::Sensitive  >( ABCDE               );    UT_TRUE( result >  0 );
    result= ms.CompareTo<true, Case::Ignore     >( ABCDE               );    UT_TRUE( result == 0 );
    result= ms.CompareTo<false                  >( ABCDE               );    UT_TRUE( result >  0 );
    result= ms.CompareTo<false, Case::Sensitive >( ABCDE               );    UT_TRUE( result >  0 );
    result= ms.CompareTo<false, Case::Ignore    >( ABCDE               );    UT_TRUE( result == 0 );
    result= ms.CompareTo<true , Case::Sensitive >( ABCDE,  2,2,   2,2  );    UT_TRUE( result >  0 );
    result= ms.CompareTo<true , Case::Ignore    >( ABCDE,  2,2,   2,2  );    UT_TRUE( result == 0 );
    result= ms.CompareTo<false, Case::Sensitive >( ABCDE,  2,2,   2,2  );    UT_TRUE( result >  0 );
    result= ms.CompareTo<false, Case::Ignore    >( ABCDE,  2,2,   2,2  );    UT_TRUE( result == 0 );

    // we do just a very little testing with StringBuilder and AString parameter version of this function, as
    // the code differences are very small
    const character* cp= t.c_str(); //new StringBuilder(); tSB._( t );
    result= ms.CompareTo<true ,Case::Sensitive>( cp                          );  UT_EQ( 0,   result );
    result= ms.CompareTo<true ,Case::Sensitive>( cp,  -5, 100, -10, 100      );  UT_EQ( 0,   result );
    result= ms.CompareTo<true ,Case::Sensitive>( cp,   2,   3,   2,   3      );  UT_EQ( 0,   result );
    result= ms.CompareTo<false                >( cp,   2,   3,   2,   3      );  UT_EQ( 0,   result );

    AString tAS( t );
    result= ms.CompareTo                        ( tAS      );                     UT_EQ( 0,   result );
    result= ms.CompareTo<true , Case::Sensitive>(tAS,  -5, 100, -10, 100      );  UT_EQ( 0,   result );
    result= ms.CompareTo<true , Case::Sensitive>( cp,   2,   3,   2,   3      );  UT_EQ( 0,   result );
    result= ms.CompareTo<false, Case::Sensitive>( cp,   2,   3,   2,   3      );  UT_EQ( 0,   result );

    // different ranges
    basic_string<character> r1= ASTR("*ABCDEF*");
    basic_string<character> r2= ASTR("##*ABCDEF*##");
    ms.Clear()._( r1 );
    result= ms.CompareTo<true , Case::Sensitive>( r2,  2,8          );       UT_EQ( 0,   result );
    result= ms.CompareTo<false, Case::Sensitive>( r2,  2,7          );       UT_EQ( 1,   result );
    result= ms.CompareTo<false, Case::Sensitive>( r2,  2,8          );       UT_EQ( 0,   result );
    result= ms.CompareTo<false, Case::Sensitive>( r2,  2,9          );       UT_EQ(-1,   result );
    result= ms.CompareTo<true , Case::Sensitive>( r2,  3,6,   1,6   );       UT_EQ( 0,   result );
    result= ms.CompareTo<false, Case::Sensitive>( r2,  3,6,   1,6   );       UT_EQ( 0,   result );

    // startsWith/Endswith/ContainsAt
    basic_string<character> sub1=  ASTR("ABC");
    basic_string<character> sub2=  ASTR("abc");
    basic_string<character> s1=    ASTR("ABCDEF");
    basic_string<character> s2=    ASTR("123ABC");
    basic_string<character> s3=    ASTR("##12ABC21##");
    basic_string<character> s4=    ASTR("AB");
    ms.Clear()._( s1 );    UT_TRUE  ( ms.StartsWith                              ( sub1    ) )
                           UT_FALSE ( ms.StartsWith                              ( sub2    ) )
    ms.Clear()._( s2 );    UT_TRUE  ( ms.EndsWith                                ( sub1    ) )
                           UT_FALSE ( ms.EndsWith                                ( sub2    ) )
    ms.Clear()._( s3 );    UT_TRUE  ( ms.ContainsAt                              ( sub1, 4 ) )
                           UT_FALSE ( ms.ContainsAt                              ( sub2, 4 ) )
                           UT_TRUE  ( ms.ContainsAt<true ALIB_COMMA Case::Ignore>( sub2, 4 ) )
    ms.Clear()._( s4 );    UT_FALSE ( ms.ContainsAt                              ( sub1, 4 ) )
                           UT_FALSE ( ms.ContainsAt                              ( sub2, 4 ) )
    ms.Clear()._( s4 );    UT_FALSE ( ms.ContainsAt                              ( sub1, 0 ) )
                           UT_FALSE ( ms.ContainsAt<true ALIB_COMMA Case::Ignore>( sub2, 0 ) )

    ms.Clear()._( ASTR("Hello ALib classes") );

    UT_TRUE( ms.StartsWith<true ALIB_COMMA Case::Ignore>( ASTR("Hello ALib classes")   ) == true  );
    UT_TRUE( ms.EndsWith  <true ALIB_COMMA Case::Ignore>( ASTR("Hello ALib classes")   ) == true  );
    UT_TRUE( ms.StartsWith<true ALIB_COMMA Case::Ignore>( ASTR("Hello ALib classesx")  ) == false );
    UT_TRUE( ms.EndsWith  <true ALIB_COMMA Case::Ignore>( ASTR("xHello ALib classes")  ) == false );
    UT_TRUE( ms.StartsWith<true ALIB_COMMA Case::Ignore>( ASTR("heLLO")                ) == true  );
    UT_TRUE( ms.StartsWith<true ALIB_COMMA Case::Ignore>( ASTR("HeLLO")                ) == true  );
    UT_TRUE( ms.EndsWith  <true ALIB_COMMA Case::Ignore>( ASTR("heLLO")                ) == false );
    UT_TRUE( ms.StartsWith<true ALIB_COMMA Case::Ignore>( ASTR("CLASSES")              ) == false );
    UT_TRUE( ms.EndsWith  <true ALIB_COMMA Case::Ignore>( ASTR("CLASSES")              ) == true  );
    UT_TRUE( ms.EndsWith  <true ALIB_COMMA Case::Ignore>( ASTR("clASSes")              ) == true  );

    // contains with empty/null strings
    ms._()._(ASTR("AB"));       UT_FALSE ( ms.ContainsAt( NullString, -1 )  )
                                UT_FALSE ( ms.ContainsAt( NullString,  0 )  )
                                UT_FALSE ( ms.ContainsAt( NullString,  1 )  )
                                UT_FALSE ( ms.ContainsAt( NullString,  2 )  )
                                UT_FALSE ( ms.ContainsAt( NullString,  3 )  )

                                UT_FALSE ( ms.ContainsAt( ASTR(""),  -1 )  )
                                UT_TRUE  ( ms.ContainsAt( ASTR(""),   0 )  )
                                UT_TRUE  ( ms.ContainsAt( ASTR(""),   1 )  )
                                UT_TRUE  ( ms.ContainsAt( ASTR(""),   2 )  )
                                UT_FALSE ( ms.ContainsAt( ASTR(""),   3 )  )
                                UT_FALSE ( ms.ContainsAt( ASTR(""), 100 )  )

    ms._();                     UT_FALSE ( ms.ContainsAt( NullString, -1 )  )
                                UT_FALSE ( ms.ContainsAt( NullString,  0 )  )
                                UT_FALSE ( ms.ContainsAt( NullString,  1 )  )

                                UT_FALSE ( ms.ContainsAt( ASTR(""),  -1 )  )
                                UT_TRUE  ( ms.ContainsAt( ASTR(""),   0 )  )
                                UT_FALSE ( ms.ContainsAt( ASTR(""),   1 )  )

    // equals String
    ms.Clear()._( ASTR("") );   UT_TRUE  ( ms.Equals( basic_string<character>(ASTR(""))   )     )
                                UT_FALSE ( ms.Equals( basic_string<character>(ASTR("a"))  )     )

    ms.Clear()._( ASTR("AB") ); UT_FALSE ( ms.Equals( basic_string<character>(ASTR("")   ))     )
                                UT_FALSE ( ms.Equals( basic_string<character>(ASTR("ab") ))     )
                                UT_FALSE ( ms.Equals( basic_string<character>(ASTR("A")  ))     )
                                UT_FALSE ( ms.Equals( basic_string<character>(ASTR("B")  ))     )
                                UT_TRUE  ( ms.Equals( basic_string<character>(ASTR("AB") ))     )
                                UT_TRUE  ( ms.Equals( basic_string<character>(ASTR("AB") ))     )

    // equals AString
    ms.Clear()._( ASTR("") );         UT_TRUE  ( ms.Equals( String(ASTR("")  ))   )
                                UT_FALSE ( ms.Equals( String(ASTR("a") ))   )

    ms.Clear()._( ASTR("AB") ); UT_FALSE ( ms.Equals( String(ASTR("" ) ))   )
                                UT_FALSE ( ms.Equals( String(ASTR("ab")))   )
                                UT_FALSE ( ms.Equals( String(ASTR("A") ))   )
                                UT_FALSE ( ms.Equals( String(ASTR("B") ))   )
                                UT_TRUE  ( ms.Equals( String(ASTR("AB")))   )
                                UT_TRUE  ( ms.Equals( String(ASTR("AB")))   )
}

//--------------------------------------------------------------------------------------------------
//--- Test ConvertCase
//--------------------------------------------------------------------------------------------------
UT_METHOD( ConvertCase )
{
    UT_INIT();

    AString ms;

    {
        // test all characters in the 8 bit range
        int testCharRangeStart= 1;
        int testCharRangeEnd=   255;

        basic_string<character> sb;
        for ( int i= testCharRangeStart; i <= testCharRangeEnd ; i++ )
            sb.append( 1, static_cast<character>( i ) );

        basic_string<character> tUpper= sb;    std::transform( tUpper.begin(), tUpper.end(), tUpper.begin(), [](character c) { return  static_cast<character>( ::toupper(c) ); } );
        basic_string<character> tLower= sb;    std::transform( tLower.begin(), tLower.end(), tLower.begin(), [](character c) { return  static_cast<character>( ::tolower(c) ); } );

        ms.Clear()._( sb ).ToLower(); UT_EQ( tLower.c_str(), ms );
        ms.Clear()._( sb ).ToUpper(); UT_EQ( tUpper.c_str(), ms );
    }

    // test substring conversion
    {
        const character* t= ASTR("AAAbbb");
        ms.Clear()._( t ).ToLower(  0,  2 );        UT_EQ( ASTR("aaAbbb"), ms );
        ms.Clear()._( t ).ToLower( -2,  3 );        UT_EQ( ASTR("aAAbbb"), ms );
        ms.Clear()._( t ).ToLower( -2     );        UT_EQ( ASTR("aaabbb"), ms );
        ms.Clear()._( t ).ToUpper(  3,  2 );        UT_EQ( ASTR("AAABBb"), ms );
        ms.Clear()._( t ).ToUpper( -2,  6 );        UT_EQ( ASTR("AAABbb"), ms );
        ms.Clear()._( t ).ToUpper( -2     );        UT_EQ( ASTR("AAABBB"), ms );
    }
}


UT_CLASS_END

}; //namespace
