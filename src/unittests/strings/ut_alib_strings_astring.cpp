// #################################################################################################
//  AWorx ALib Unit Tests
//
//  Copyright 2013-2023 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"
#include "unittests/alib_test_selection.hpp"
#include "alib/lib/fs_commonenums/commonenumdefs_aliased.hpp"
#if ALIB_UT_STRINGS

#include "alib/alox.hpp"

#if !defined (HPP_ALIB_STRINGS_SUBSTRING)
#   include "alib/strings/substring.hpp"
#endif
#if !defined(HPP_ALIB_COMPATIBILITY_STD_CHARACTERS)
#   include "alib/compatibility/std_characters.hpp"
#endif
#if !defined (HPP_ALIB_COMPATIBILITY_STD_STRINGS_IOSTREAM)
#   include "alib/compatibility/std_strings_iostream.hpp"
#endif
#if !defined (HPP_ALIB_STRINGS_FORMAT)
#    include "alib/strings/format.hpp"
#endif

#include <iostream>
#include <sstream>
#include <clocale>
#include <algorithm>

#define TESTCLASSNAME       CPP_ALib_Strings_AString
#include "unittests/aworx_unittests.hpp"

// Windows.h might bring in max/min macros
#if defined( max )
    #undef max
    #undef min
#endif


using namespace std;
using namespace aworx;

namespace ut_aworx {


UT_CLASS


//--------------------------------------------------------------------------------------------------
//--- Test Constructors
//--------------------------------------------------------------------------------------------------
UT_METHOD( Constructors )
{
    UT_INIT()

    AString* ms;
    char*     csNull= nullptr;
               ms= new AString();                        UT_EQ    ( 0 , ms->Capacity()  )    UT_EQ( ms->Length(), 0 )
    delete ms; ms= new AString( NString(csNull) );       UT_EQ    ( 0 , ms->Capacity()  )    UT_EQ( ms->Length(), 0 )
    delete ms; ms= new AString( "" );                    UT_TRUE  ( ms->Capacity() >  0 )    UT_EQ( ms->Length(), 0 )
    delete ms; ms= new AString( "Test" );                UT_TRUE  ( ms->Capacity() >= 4 )    UT_EQ( ms->Length(), 4 )

    AString tNullString;

    AString tASEmpty("");

    AString tAS( A_CHAR("0123456789") );
    std::basic_string<character> tS(A_CHAR("0123456789"));
    delete ms; ms= new AString( tS );                    UT_TRUE  ( ms->Capacity() >=  10 && ms->Length() == 10 )
                                                         UT_EQ( A_CHAR("0123456789")  , *ms )
    const character* tSBEmpty= A_CHAR("");
    delete ms; ms= new AString( tSBEmpty );              UT_TRUE  ( ms->Length() == 0 )

    const character*  tSB= A_CHAR("0123456789");
    delete ms; ms= new AString( tSB );                   UT_TRUE  ( ms->Capacity() >=  10 && ms->Length() == 10 ) UT_EQ( A_CHAR("0123456789")   , *ms )
    delete ms;
}


//--------------------------------------------------------------------------------------------------
//--- Test IsNull
//--------------------------------------------------------------------------------------------------
UT_METHOD( IsNull )
{
    UT_INIT()

    AString ms;                UT_TRUE( ms.IsNull()    )  UT_TRUE(  ms.IsNull() )   UT_TRUE( !ms.IsNotNull() )   UT_TRUE(  ms.IsEmpty() )   UT_TRUE( !ms.IsNotEmpty() )
    ms.Reset("" );             UT_TRUE( ms.IsNotNull() )  UT_TRUE( !ms.IsNull() )   UT_TRUE(  ms.IsNotNull() )   UT_TRUE(  ms.IsEmpty() )   UT_TRUE( !ms.IsNotEmpty() )
    ms.Reset("x");             UT_TRUE( ms.IsNotNull() )  UT_TRUE( !ms.IsNull() )   UT_TRUE(  ms.IsNotNull() )   UT_TRUE( !ms.IsEmpty() )   UT_TRUE(  ms.IsNotEmpty() )

    ms.SetNull();
                               UT_TRUE( ms.IsNull()    )  UT_TRUE(  ms.IsNull() )   UT_TRUE( !ms.IsNotNull() )   UT_TRUE(  ms.IsEmpty() )   UT_TRUE( !ms.IsNotEmpty() )
    ms._( A_CHAR(""));         UT_TRUE( ms.IsNotNull() )  UT_TRUE( !ms.IsNull() )   UT_TRUE(  ms.IsNotNull() )   UT_TRUE(  ms.IsEmpty() )   UT_TRUE( !ms.IsNotEmpty() )
    ms._( A_CHAR("xx"));       UT_TRUE( ms.IsNotNull() )  UT_TRUE( !ms.IsNull() )   UT_TRUE(  ms.IsNotNull() )   UT_TRUE( !ms.IsEmpty() )   UT_TRUE(  ms.IsNotEmpty() )

    ms.SetNull();              UT_TRUE( ms.IsNull()    )  UT_TRUE(  ms.IsNull() )   UT_TRUE( !ms.IsNotNull() )   UT_TRUE(  ms.IsEmpty() )   UT_TRUE( !ms.IsNotEmpty() )
    ms.SetBuffer(5);           UT_TRUE( ms.IsNotNull() )  UT_TRUE( !ms.IsNull() )   UT_TRUE(  ms.IsNotNull() )   UT_TRUE(  ms.IsEmpty() )   UT_TRUE( !ms.IsNotEmpty() )

    ms.SetNull();              UT_TRUE( ms.IsNull()    )  UT_TRUE(  ms.IsNull() )   UT_TRUE( !ms.IsNotNull() )   UT_TRUE(  ms.IsEmpty() )   UT_TRUE( !ms.IsNotEmpty() )
    ms._("");                  UT_TRUE( ms.IsNotNull() )  UT_TRUE( !ms.IsNull() )   UT_TRUE(  ms.IsNotNull() )   UT_TRUE(  ms.IsEmpty() )   UT_TRUE( !ms.IsNotEmpty() )

    ms.SetNull();              UT_TRUE( ms.IsNull()    )  UT_TRUE(  ms.IsNull() )   UT_TRUE( !ms.IsNotNull() )   UT_TRUE(  ms.IsEmpty() )   UT_TRUE( !ms.IsNotEmpty() )
    ms._("abc");               UT_TRUE( ms.IsNotNull() )  UT_TRUE( !ms.IsNull() )   UT_TRUE(  ms.IsNotNull() )   UT_TRUE( !ms.IsEmpty() )   UT_TRUE(  ms.IsNotEmpty() )
}

//--------------------------------------------------------------------------------------------------
//--- Test FirstBackCharAt
//--------------------------------------------------------------------------------------------------
UT_METHOD( CharAt )
{
    UT_INIT()

    AString ms;
    UT_EQ(  ms.CharAtStart       ()    ,    '\0' )
    UT_EQ(  ms.CharAtEnd         ()    ,    '\0' )
    UT_EQ(  ms.CharAt            ( -1 ),    '\0' )
    UT_EQ(  ms.CharAt            (  0 ),    '\0' )
    UT_EQ(  ms.CharAt            (  1 ),    '\0' )

    ms.Reset('A');

    UT_EQ(  ms.CharAtStart       ()    ,    'A'  )
    UT_EQ(  ms.CharAtStart<false>()    ,    'A'  )
    UT_EQ(  ms.CharAtEnd         ()    ,    'A'  )
    UT_EQ(  ms.CharAtEnd<false>  ()    ,    'A'  )
    UT_EQ(  ms.CharAt            ( -1 ),    '\0' )
    UT_EQ(  ms.CharAt            (  0 ),    'A'  )
    UT_EQ(  ms.CharAt            (  1 ),    '\0' )
    UT_EQ(  ms.CharAt<false>     (  0 ),    'A'  )

    UT_EQ(  ms                   [  0 ],    'A'  )

    ms.Reset("ABC");

    UT_EQ(  ms.CharAtStart       ()    ,    'A'  )
    UT_EQ(  ms.CharAtStart<false>()    ,    'A'  )
    UT_EQ(  ms.CharAtEnd         ()    ,    'C'  )
    UT_EQ(  ms.CharAtEnd<false>  ()    ,    'C'  )
    UT_EQ(  ms.CharAt            ( -1 ),    '\0' )
    UT_EQ(  ms.CharAt            (  0 ),    'A'  )
    UT_EQ(  ms.CharAt            (  1 ),    'B'  )
    UT_EQ(  ms.CharAt            (  2 ),    'C'  )
    UT_EQ(  ms.CharAt            (  3 ),    '\0' )

    UT_EQ(  ms.CharAt<false>     (  0 ),    'A'  )
    UT_EQ(  ms.CharAt<false>     (  1 ),    'B'  )
    UT_EQ(  ms.CharAt<false>     (  2 ),    'C'  )

    UT_EQ(  ms                   [  0 ],    'A'  )
    UT_EQ(  ms                   [  1 ],    'B'  )
    UT_EQ(  ms                   [  2 ],    'C'  )

    ms.Reset("ABC");
    ms.SetCharAt( -1, '@' );    UT_EQ( A_CHAR("ABC"),   ms )
    ms.SetCharAt(  0, 'X' );    UT_EQ( A_CHAR("XBC"),   ms )
    ms.SetCharAt(  1, 'Y' );    UT_EQ( A_CHAR("XYC"),   ms )
    ms.SetCharAt(  2, 'Z' );    UT_EQ( A_CHAR("XYZ"),   ms )
    ms.SetCharAt(  3, '$' );    UT_EQ( A_CHAR("XYZ"),   ms )
}


//--------------------------------------------------------------------------------------------------
//--- Test DeleteInsert
//--------------------------------------------------------------------------------------------------
UT_METHOD( DeleteInsertFillReplace )
{
    UT_INIT()

    AString ms;
    // delete
    {
        ms.Reset(A_CHAR("0123456789"));   ms.Delete                 (  5           );      UT_EQ( A_CHAR("01234"),        ms )
        ms.Reset(A_CHAR("0123456789"));   ms.Delete                 (  5,      0   );      UT_EQ( A_CHAR("0123456789"),   ms )
        ms.Reset(A_CHAR("0123456789"));   ms.Delete                 (  5,     -1   );      UT_EQ( A_CHAR("0123456789"),   ms )
        ms.Reset(A_CHAR("0123456789"));   ms.Delete                 (  5,      1   );      UT_EQ( A_CHAR("012346789"),    ms )
        ms.Reset(A_CHAR("0123456789"));   ms.Delete                 (  5,    100   );      UT_EQ( A_CHAR("01234"),        ms )
        ms.Reset(A_CHAR("0123456789"));   ms.Delete                 (  -5,     10  );      UT_EQ( A_CHAR("56789"),        ms )

        ms.Reset(A_CHAR("0123456789"));   ms.Delete<false>          (  5,    5     );      UT_EQ( A_CHAR("01234"),        ms )
        ms.Reset(A_CHAR("0123456789"));   ms.Delete<false>          (  0,    5     );      UT_EQ( A_CHAR("56789"),        ms )
        ms.Reset(A_CHAR("0123456789"));   ms.Delete<false>          (  0,    1     );      UT_EQ( A_CHAR("123456789"),    ms )
        ms.Reset(A_CHAR("0123456789"));   ms.Delete<false>          (  9,    1     );      UT_EQ( A_CHAR("012345678"),    ms )

        ms.Reset(A_CHAR("0123456789"));   ms.DeleteStart            (  -2          );      UT_EQ( A_CHAR("0123456789"),   ms )
        ms.Reset(A_CHAR("0123456789"));   ms.DeleteStart            (  -1          );      UT_EQ( A_CHAR("0123456789"),   ms )
        ms.Reset(A_CHAR("0123456789"));   ms.DeleteStart            (   0          );      UT_EQ( A_CHAR("0123456789"),   ms )
        ms.Reset(A_CHAR("0123456789"));   ms.DeleteStart            (   1          );      UT_EQ(  A_CHAR("123456789"),   ms )
        ms.Reset(A_CHAR("0123456789"));   ms.DeleteStart            (   2          );      UT_EQ(   A_CHAR("23456789"),   ms )
        ms.Reset(A_CHAR("0123456789"));   ms.DeleteStart            (   9          );      UT_EQ(          A_CHAR("9"),   ms )
        ms.Reset(A_CHAR("0123456789"));   ms.DeleteStart            (  10          );      UT_EQ(           A_CHAR(""),   ms )
        ms.Reset(A_CHAR("0123456789"));   ms.DeleteStart            (  11          );      UT_EQ(           A_CHAR(""),   ms )

        ms.Reset(A_CHAR("0123456789"));   ms.DeleteStart<false>     (   0          );      UT_EQ( A_CHAR("0123456789"),   ms )
        ms.Reset(A_CHAR("0123456789"));   ms.DeleteStart<false>     (   1          );      UT_EQ(  A_CHAR("123456789"),   ms )
        ms.Reset(A_CHAR("0123456789"));   ms.DeleteStart<false>     (   2          );      UT_EQ(   A_CHAR("23456789"),   ms )
        ms.Reset(A_CHAR("0123456789"));   ms.DeleteStart<false>     (   9          );      UT_EQ(          A_CHAR("9"),   ms )
        ms.Reset(A_CHAR("0123456789"));   ms.DeleteStart<false>     (  10          );      UT_EQ(           A_CHAR(""),   ms )

        ms.Reset(A_CHAR("0123456789"));   ms.DeleteEnd              (  -2          );      UT_EQ( A_CHAR("0123456789"),   ms )
        ms.Reset(A_CHAR("0123456789"));   ms.DeleteEnd              (  -1          );      UT_EQ( A_CHAR("0123456789"),   ms )
        ms.Reset(A_CHAR("0123456789"));   ms.DeleteEnd              (   0          );      UT_EQ( A_CHAR("0123456789"),   ms )
        ms.Reset(A_CHAR("0123456789"));   ms.DeleteEnd              (   1          );      UT_EQ( A_CHAR("012345678") ,   ms )
        ms.Reset(A_CHAR("0123456789"));   ms.DeleteEnd              (   2          );      UT_EQ( A_CHAR("01234567")  ,   ms )
        ms.Reset(A_CHAR("0123456789"));   ms.DeleteEnd              (   9          );      UT_EQ( A_CHAR("0")         ,   ms )
        ms.Reset(A_CHAR("0123456789"));   ms.DeleteEnd              (  10          );      UT_EQ( A_CHAR("")          ,   ms )
        ms.Reset(A_CHAR("0123456789"));   ms.DeleteEnd              (  11          );      UT_EQ( A_CHAR("")          ,   ms )

        ms.Reset(A_CHAR("0123456789"));   ms.DeleteEnd <false>      (   0          );      UT_EQ( A_CHAR("0123456789"),   ms )
        ms.Reset(A_CHAR("0123456789"));   ms.DeleteEnd <false>      (   1          );      UT_EQ( A_CHAR("012345678") ,   ms )
        ms.Reset(A_CHAR("0123456789"));   ms.DeleteEnd <false>      (   2          );      UT_EQ( A_CHAR("01234567")  ,   ms )
        ms.Reset(A_CHAR("0123456789"));   ms.DeleteEnd <false>      (   9          );      UT_EQ( A_CHAR("0")         ,   ms )
        ms.Reset(A_CHAR("0123456789"));   ms.DeleteEnd <false>      (  10          );      UT_EQ( A_CHAR("")          ,   ms )
    }

    // InsertChars
    {
        ms.Reset(A_CHAR("0123456789"));   ms.InsertChars            ( ' ',  1,  -1 );      UT_EQ( A_CHAR("0123456789"),   ms )
        ms.Reset(A_CHAR("0123456789"));   ms.InsertChars            ( ' ',  1,  20 );      UT_EQ( A_CHAR("0123456789"),   ms )
        ms.Reset(A_CHAR("0123456789"));   ms.InsertChars            ( ' ',  0,   0 );      UT_EQ( A_CHAR("0123456789"),   ms )

        ms.Reset(A_CHAR("0123456789"));   ms.InsertChars            ( ' ',  1,   0 );      UT_EQ( A_CHAR(" 0123456789"),  ms )
        ms.Reset(A_CHAR("0123456789"));   ms.InsertChars            ( ' ',  1,   1 );      UT_EQ( A_CHAR("0 123456789"),  ms )
        ms.Reset(A_CHAR("0123456789"));   ms.InsertChars            ( ' ',  2,   0 );      UT_EQ( A_CHAR("  0123456789"), ms )
        ms.Reset(A_CHAR("0123456789"));   ms.InsertChars            ( ' ',  2,   1 );      UT_EQ( A_CHAR("0  123456789"), ms )

        ms.Reset(A_CHAR("0123456789"));   ms.InsertChars            ( ' ',  1,  11 );      UT_EQ( A_CHAR("0123456789"),   ms )
        ms.Reset(A_CHAR("0123456789"));   ms.InsertChars            ( ' ',  1,  10 );      UT_EQ( A_CHAR("0123456789 "),  ms )
        ms.Reset(A_CHAR("0123456789"));   ms.InsertChars            ( '@',  3,   5 );      UT_EQ( A_CHAR("01234@@@56789"),ms )
    }

    // InsertAt
    {
        ms.Reset(A_CHAR("0123456789"));   ms.InsertAt               ( A_CHAR("TST"),  -2 );        UT_EQ( A_CHAR("0123456789"),    ms )
        ms.Reset(A_CHAR("0123456789"));   ms.InsertAt               ( A_CHAR("TST"),  -1 );        UT_EQ( A_CHAR("0123456789"),    ms )
        ms.Reset(A_CHAR("0123456789"));   ms.InsertAt               ( A_CHAR("TST"),   0 );        UT_EQ( A_CHAR("TST0123456789"), ms )
        ms.Reset(A_CHAR("0123456789"));   ms.InsertAt               ( A_CHAR("TST"),   1 );        UT_EQ( A_CHAR("0TST123456789"), ms )
        ms.Reset(A_CHAR("0123456789"));   ms.InsertAt               ( A_CHAR("TST"),   8 );        UT_EQ( A_CHAR("01234567TST89"), ms )
        ms.Reset(A_CHAR("0123456789"));   ms.InsertAt               ( A_CHAR("TST"),   9 );        UT_EQ( A_CHAR("012345678TST9"), ms )
        ms.Reset(A_CHAR("0123456789"));   ms.InsertAt               ( A_CHAR("TST"),  10 );        UT_EQ( A_CHAR("0123456789TST"), ms )
        ms.Reset(A_CHAR("0123456789"));   ms.InsertAt               ( A_CHAR("TST"),  11 );        UT_EQ( A_CHAR("0123456789"),    ms )
        ms.Reset(A_CHAR("0123456789"));   ms.InsertAt               ( A_CHAR("TST"),  12 );        UT_EQ( A_CHAR("0123456789"),    ms )

        ms.Reset(A_CHAR("0123456789"));   ms.InsertAt<false>        ( A_CHAR("TST"),   0 );        UT_EQ( A_CHAR("TST0123456789"), ms )
        ms.Reset(A_CHAR("0123456789"));   ms.InsertAt<false>        ( A_CHAR("TST"),   1 );        UT_EQ( A_CHAR("0TST123456789"), ms )
        ms.Reset(A_CHAR("0123456789"));   ms.InsertAt<false>        ( A_CHAR("TST"),   8 );        UT_EQ( A_CHAR("01234567TST89"), ms )
        ms.Reset(A_CHAR("0123456789"));   ms.InsertAt<false>        ( A_CHAR("TST"),   9 );        UT_EQ( A_CHAR("012345678TST9"), ms )
        ms.Reset(A_CHAR("0123456789"));   ms.InsertAt<false>        ( A_CHAR("TST"),  10 );        UT_EQ( A_CHAR("0123456789TST"), ms )

    }

    // ReplaceSubstring
    {
        String r= A_CHAR("ABC");
        ms.Reset(A_CHAR("0123456789"));   ms.ReplaceSubstring       ( r,   0,  10)  ;      UT_EQ( A_CHAR("ABC"),          ms )
        ms.Reset(A_CHAR("0123456789"));   ms.ReplaceSubstring       ( r, -10, 100)  ;      UT_EQ( A_CHAR("ABC"),          ms )
        ms.Reset(A_CHAR("0123456789"));   ms.ReplaceSubstring       ( r, -10,  5 )  ;      UT_EQ( A_CHAR("ABC0123456789"),ms )
        ms.Reset(A_CHAR("0123456789"));   ms.ReplaceSubstring       ( r, -10, 10 )  ;      UT_EQ( A_CHAR("ABC0123456789"),ms )
        ms.Reset(A_CHAR("0123456789"));   ms.ReplaceSubstring       ( r, -10, 11 )  ;      UT_EQ( A_CHAR("ABC123456789"), ms )
        ms.Reset(A_CHAR("0123456789"));   ms.ReplaceSubstring       ( r,   0,  1 )  ;      UT_EQ( A_CHAR("ABC123456789"), ms )
        ms.Reset(A_CHAR("0123456789"));   ms.ReplaceSubstring       ( r,   0,  2 )  ;      UT_EQ( A_CHAR("ABC23456789"),  ms )
        ms.Reset(A_CHAR("0123456789"));   ms.ReplaceSubstring       ( r,   1,  1 )  ;      UT_EQ( A_CHAR("0ABC23456789"), ms )
        ms.Reset(A_CHAR("0123456789"));   ms.ReplaceSubstring       ( r,   1,  2 )  ;      UT_EQ( A_CHAR("0ABC3456789"),  ms )
        ms.Reset(A_CHAR("0123456789"));   ms.ReplaceSubstring       ( r,   8,  1 )  ;      UT_EQ( A_CHAR("01234567ABC9"), ms )
        ms.Reset(A_CHAR("0123456789"));   ms.ReplaceSubstring       ( r,   8,  2 )  ;      UT_EQ( A_CHAR("01234567ABC"),  ms )
        ms.Reset(A_CHAR("0123456789"));   ms.ReplaceSubstring       ( r,   8,  3 )  ;      UT_EQ( A_CHAR("01234567ABC"),  ms )
        ms.Reset(A_CHAR("0123456789"));   ms.ReplaceSubstring       ( r,   8,  4 )  ;      UT_EQ( A_CHAR("01234567ABC"),  ms )

        ms.Reset(A_CHAR("0123456789"));   ms.ReplaceSubstring<false>( r,   0,  1 )  ;      UT_EQ( A_CHAR("ABC123456789"), ms )
        ms.Reset(A_CHAR("0123456789"));   ms.ReplaceSubstring<false>( r,   0,  2 )  ;      UT_EQ( A_CHAR("ABC23456789"),  ms )
        ms.Reset(A_CHAR("0123456789"));   ms.ReplaceSubstring<false>( r,   1,  1 )  ;      UT_EQ( A_CHAR("0ABC23456789"), ms )
        ms.Reset(A_CHAR("0123456789"));   ms.ReplaceSubstring<false>( r,   1,  2 )  ;      UT_EQ( A_CHAR("0ABC3456789"),  ms )
        ms.Reset(A_CHAR("0123456789"));   ms.ReplaceSubstring<false>( r,   8,  1 )  ;      UT_EQ( A_CHAR("01234567ABC9"), ms )
        ms.Reset(A_CHAR("0123456789"));   ms.ReplaceSubstring<false>( r,   8,  2 )  ;      UT_EQ( A_CHAR("01234567ABC"),  ms )
    }

    // ReplaceRegion
    {
        ms.Reset(A_CHAR("0123456789"));   ms.ReplaceRegion          ( '@',   0,  10)  ;    UT_EQ( A_CHAR("@@@@@@@@@@"),   ms )
        ms.Reset(A_CHAR("0123456789"));   ms.ReplaceRegion          ( '@', -10, 100)  ;    UT_EQ( A_CHAR("@@@@@@@@@@"),   ms )
        ms.Reset(A_CHAR("0123456789"));   ms.ReplaceRegion          ( '@', -10,  5 )  ;    UT_EQ( A_CHAR("0123456789"),   ms )
// GCC V.12.2.0 is afraid that internally an illegal memset is performed with the next test (which is not true)
#if defined(__clang__)
#elif defined(__GNUC__)
_Pragma("GCC diagnostic push")
_Pragma("GCC diagnostic ignored \"-Wstringop-overflow\"")
#endif
        ms.Reset(A_CHAR("0123456789"));   ms.ReplaceRegion          ( '@', -10, 10 )  ;    UT_EQ( A_CHAR("0123456789"),   ms )
#if defined(__clang__)
#elif defined(__GNUC__)
_Pragma("GCC diagnostic pop")
#endif
        ms.Reset(A_CHAR("0123456789"));   ms.ReplaceRegion          ( '@', -10, 11 )  ;    UT_EQ( A_CHAR("@123456789"),   ms )
        ms.Reset(A_CHAR("0123456789"));   ms.ReplaceRegion          ( '@',   0,  1 )  ;    UT_EQ( A_CHAR("@123456789"),   ms )
        ms.Reset(A_CHAR("0123456789"));   ms.ReplaceRegion          ( '@',   0,  2 )  ;    UT_EQ( A_CHAR("@@23456789"),   ms )
        ms.Reset(A_CHAR("0123456789"));   ms.ReplaceRegion          ( '@',   1,  1 )  ;    UT_EQ( A_CHAR("0@23456789"),   ms )
        ms.Reset(A_CHAR("0123456789"));   ms.ReplaceRegion          ( '@',   1,  2 )  ;    UT_EQ( A_CHAR("0@@3456789"),   ms )
        ms.Reset(A_CHAR("0123456789"));   ms.ReplaceRegion          ( '@',   8,  1 )  ;    UT_EQ( A_CHAR("01234567@9"),   ms )
        ms.Reset(A_CHAR("0123456789"));   ms.ReplaceRegion          ( '@',   8,  2 )  ;    UT_EQ( A_CHAR("01234567@@"),   ms )
        ms.Reset(A_CHAR("0123456789"));   ms.ReplaceRegion          ( '@',   8,  3 )  ;    UT_EQ( A_CHAR("01234567@@"),   ms )
        ms.Reset(A_CHAR("0123456789"));   ms.ReplaceRegion<false>   ( '@',   0,  1 )  ;    UT_EQ( A_CHAR("@123456789"),   ms )
        ms.Reset(A_CHAR("0123456789"));   ms.ReplaceRegion<false>   ( '@',   0,  2 )  ;    UT_EQ( A_CHAR("@@23456789"),   ms )
        ms.Reset(A_CHAR("0123456789"));   ms.ReplaceRegion<false>   ( '@',   1,  1 )  ;    UT_EQ( A_CHAR("0@23456789"),   ms )
        ms.Reset(A_CHAR("0123456789"));   ms.ReplaceRegion<false>   ( '@',   1,  2 )  ;    UT_EQ( A_CHAR("0@@3456789"),   ms )
        ms.Reset(A_CHAR("0123456789"));   ms.ReplaceRegion<false>   ( '@',   8,  1 )  ;    UT_EQ( A_CHAR("01234567@9"),   ms )
        ms.Reset(A_CHAR("0123456789"));   ms.ReplaceRegion<false>   ( '@',   8,  2 )  ;    UT_EQ( A_CHAR("01234567@@"),   ms )
    }

}


//--------------------------------------------------------------------------------------------------
//--- Test IOStream
//--------------------------------------------------------------------------------------------------
UT_METHOD( IOStream )
{
    UT_INIT()

    // narrow
    {
        NAString   ms;
        basic_string<aworx::nchar>     s;
        ms.Reset( "0123456789" );


        std::basic_stringstream<aworx::nchar> strs;
        ms.Reset( "hello stringstream" );
        strs << ms;
        UT_EQ( strs.str(), "hello stringstream" )

        strs << endl;
        strs << "second line";

        strs >> ms;
        UT_EQ( NString("hello stringstream") , static_cast<NString>(ms) )
        strs >> ms;
        UT_EQ( NString("second line"       ) , static_cast<NString>(ms) )

        strs >> ms;
        UT_EQ( NString(""                  ) , static_cast<NString>(ms) )
    }

    // wide
    {
        WAString   ms;
        basic_string<wchar_t>     s;
        ms.Reset( A_WCHAR("0123456789") );


        std::basic_stringstream<wchar_t> strs;
        ms.Reset( A_WCHAR("hello stringstream") );
        strs << ms;
        UT_EQ( strs.str(), L"hello stringstream" )

        strs << endl;
        strs << L"second line";

        strs >> ms;

        UT_EQ( WString(A_WCHAR("hello stringstream")) , static_cast<WString>(ms) )

        strs >> ms;
        UT_EQ( WString(A_WCHAR("second line"       )) , static_cast<WString>(ms) )

        strs >> ms;
        UT_EQ( WString(A_WCHAR(""                  )) , static_cast<WString>(ms) )
    }

}


//--------------------------------------------------------------------------------------------------
//--- Test AssignAndAppend
//--------------------------------------------------------------------------------------------------
UT_METHOD( Append )
{
    UT_INIT()

    // const character*
    {
        AString ms;  const character* csNull= nullptr;  const character* csEmpty= A_CHAR("");

        ms.Reset( csNull          ); UT_EQ  ( 0, ms.Length()     )   UT_TRUE( ms.IsNull()    )
        ms.Reset( csEmpty         ); UT_EQ  ( 0, ms.Length()     )   UT_TRUE( ms.IsNotNull() )
        ms.Reset( A_CHAR("assign")); UT_EQ  ( 6, ms.Length()     )
        ms.Reset( csNull          ); UT_EQ  ( 0, ms.Length()     )   UT_TRUE( ms.IsNotNull() )
        ms.SetNull();                UT_EQ  ( 0, ms.Length()     )   UT_TRUE( ms.IsNull()    )
        ms.Reset( A_CHAR("assign")); UT_EQ  ( 6, ms.Length()     )

        ms.SetNull();                UT_EQ  ( 0, ms.Length()     )   UT_TRUE( ms.IsNull()    )
        ms._( csEmpty);              UT_EQ  ( 0, ms.Length()     )   UT_TRUE( ms.IsNotNull() )
        ms.SetNull();                UT_EQ  ( 0, ms.Length()     )   UT_TRUE( ms.IsNull()    )

        ms._(A_CHAR("1234567"));     UT_EQ  (16, ms.Capacity()   )   UT_EQ( A_CHAR("1234567"),   ms )
        ms._(A_CHAR("89"));          UT_TRUE( ms.Length()> 7     )   UT_EQ( A_CHAR("123456789"), ms )

        const character* t= A_CHAR("0123456789");
        ms.Reset()._   ( t, 5);                     UT_EQ( A_CHAR("56789")      , ms )
        ms.Reset()._   ( t, 5, 100);                UT_EQ( A_CHAR("56789")      , ms )
        ms.Reset()._   ( t, -5);                    UT_EQ( A_CHAR("0123456789") , ms )
        ms.Reset()._   ( t, -5, 3);                 UT_EQ( A_CHAR("")           , ms )
        ms.Reset()._   ( t, 50, 3);                 UT_EQ( A_CHAR("")           , ms )
        ms.Reset()._   ( t, 10, 3);                 UT_EQ( A_CHAR("")           , ms )
        ms.Reset()._   ( t, -5, 10);                UT_EQ( A_CHAR("01234")      , ms )
        ms.Reset()._   ( t, -5, 100);               UT_EQ( A_CHAR("0123456789") , ms )

        // _<false>
        ms.SetNull();            UT_EQ  ( 0, ms.Length()      )  UT_TRUE( ms.IsNull()   )
        ms._<false>( csEmpty );       UT_EQ  ( 0, ms.Length()      )  UT_TRUE( ms.IsNull()   )
        ms.Reset()._<false>( t, 5,3); UT_EQ( A_CHAR("567"),  ms )
    }

    // const complementChar*
    {
        AString ms;  const complementChar* csNull= nullptr;
#if !ALIB_CHARACTERS_WIDE
        const complementChar*  csEmpty= A_WCHAR("");
#else
        const complementChar*  csEmpty=  "";
#endif
        ms.Reset( csNull          ); UT_EQ  ( 0, ms.Length()    )  UT_TRUE( ms.IsNull()    )
        ms.Reset( csEmpty         ); UT_EQ  ( 0, ms.Length()    )  UT_TRUE( ms.IsNotNull() )
        ms.Reset( A_CHAR("assign")); UT_EQ  ( 6, ms.Length()    )
        ms.Reset( csNull          ); UT_EQ  ( 0, ms.Length()    )  UT_TRUE( ms.IsNotNull() )
        ms.SetNull();                UT_EQ  ( 0, ms.Length()    )  UT_TRUE( ms.IsNull()    )
        ms.Reset( A_CHAR("assign")); UT_EQ  ( 6, ms.Length()    )

        ms.SetNull();                UT_EQ  ( 0, ms.Length()    ) UT_TRUE( ms.IsNull()    )
        ms._( csEmpty);              UT_EQ  ( 0, ms.Length()    ) UT_TRUE( ms.IsNotNull() )
        ms.SetNull();                UT_EQ  ( 0, ms.Length()    ) UT_TRUE( ms.IsNull()    )

        ms._(A_CHAR("1234567"));     UT_EQ  (16, ms.Capacity()  ) UT_EQ( A_CHAR("1234567"),   ms )
        ms._(A_CHAR("89"));          UT_TRUE( ms.Length()> 7    ) UT_EQ( A_CHAR("123456789"), ms )

        const character* t= A_CHAR("0123456789");
        ms.Reset()._   ( t, 5);                     UT_EQ( A_CHAR("56789")      , ms )
        ms.Reset()._   ( t, 5, 100);                UT_EQ( A_CHAR("56789")      , ms )
        ms.Reset()._   ( t, -5);                    UT_EQ( A_CHAR("0123456789") , ms )
        ms.Reset()._   ( t, -5, 3);                 UT_EQ( A_CHAR("")           , ms )
        ms.Reset()._   ( t, 50, 3);                 UT_EQ( A_CHAR("")           , ms )
        ms.Reset()._   ( t, 10, 3);                 UT_EQ( A_CHAR("")           , ms )
        ms.Reset()._   ( t, -5, 10);                UT_EQ( A_CHAR("01234")      , ms )
        ms.Reset()._   ( t, -5, 100);               UT_EQ( A_CHAR("0123456789") , ms )

        // _<false>
        ms.SetNull();            UT_EQ  ( 0, ms.Length()      )  UT_TRUE( ms.IsNull()   )
        ms._<false>( csEmpty );       UT_EQ  ( 0, ms.Length()      )  UT_TRUE( ms.IsNull()   )
        ms.Reset()._<false>( t, 5,3); UT_EQ( A_CHAR("567"),  ms )
    }

    // const strangeChar*
    {
        AString ms;  const strangeChar* csNull= nullptr;
        const strangeChar*  csEmpty= A_XCHAR("");
        ms.Reset( csNull          ); UT_EQ  ( 0, ms.Length()    )  UT_TRUE( ms.IsNull()    )
        ms.Reset( csEmpty         ); UT_EQ  ( 0, ms.Length()    )  UT_TRUE( ms.IsNotNull() )
        ms.Reset( A_CHAR("assign")); UT_EQ  ( 6, ms.Length()    )
        ms.Reset( csNull          ); UT_EQ  ( 0, ms.Length()    )  UT_TRUE( ms.IsNotNull() )
        ms.SetNull();                UT_EQ  ( 0, ms.Length()    )  UT_TRUE( ms.IsNull()    )
        ms.Reset( A_CHAR("assign")); UT_EQ  ( 6, ms.Length()    )

        ms.SetNull();                UT_EQ  ( 0, ms.Length()    )  UT_TRUE( ms.IsNull()    )
        ms._( csEmpty);              UT_EQ  ( 0, ms.Length()    )  UT_TRUE( ms.IsNotNull() )
        ms.SetNull();                UT_EQ  ( 0, ms.Length()    )  UT_TRUE( ms.IsNull()    )

        ms._(A_CHAR("1234567"));     UT_EQ  (16, ms.Capacity()  )  UT_EQ( A_CHAR("1234567"),   ms )
        ms._(A_CHAR("89"));          UT_TRUE( ms.Length()> 7    )  UT_EQ( A_CHAR("123456789"), ms )

        const character* t= A_CHAR("0123456789");
        ms.Reset()._   ( t, 5);                     UT_EQ( A_CHAR("56789")      , ms )
        ms.Reset()._   ( t, 5, 100);                UT_EQ( A_CHAR("56789")      , ms )
        ms.Reset()._   ( t, -5);                    UT_EQ( A_CHAR("0123456789") , ms )
        ms.Reset()._   ( t, -5, 3);                 UT_EQ( A_CHAR("")           , ms )
        ms.Reset()._   ( t, 50, 3);                 UT_EQ( A_CHAR("")           , ms )
        ms.Reset()._   ( t, 10, 3);                 UT_EQ( A_CHAR("")           , ms )
        ms.Reset()._   ( t, -5, 10);                UT_EQ( A_CHAR("01234")      , ms )
        ms.Reset()._   ( t, -5, 100);               UT_EQ( A_CHAR("0123456789") , ms )

        // _<false>
        ms.SetNull();            UT_EQ  ( 0, ms.Length()      )  UT_TRUE( ms.IsNull()   )
        ms._<false>( csEmpty );       UT_EQ  ( 0, ms.Length()      )  UT_TRUE( ms.IsNull()   )
        ms.Reset()._<false>( t, 5,3); UT_EQ( A_CHAR("567"),  ms)
    }

    // fundamental types
    {
        AString ms;
        {  int      i= 5;   ms.Reset(i);       UT_EQ ( A_CHAR("5"),    ms) }

        {  int8_t   i=  4;  ms.Reset(i);       UT_EQ ( A_CHAR("4")   , ms) }

        {  int8_t   i= -4;  ms.Reset(i);       UT_EQ ( A_CHAR("-4")  , ms) }
        {  uint8_t  i=  4;  ms.Reset(i);       UT_EQ ( A_CHAR("4")   , ms) }


        {  int16_t  i=  5;  ms.Reset(i);       UT_EQ ( A_CHAR("5")   , ms) }
        {  int16_t  i= -5;  ms.Reset(i);       UT_EQ ( A_CHAR("-5")  , ms) }
        {  uint16_t i=  5;  ms.Reset(i);       UT_EQ ( A_CHAR("5")   , ms) }

        {  int32_t  i=  6;  ms.Reset(i);       UT_EQ ( A_CHAR("6")   , ms) }
        {  int32_t  i= -6;  ms.Reset(i);       UT_EQ ( A_CHAR("-6")  , ms) }
        {  uint32_t i=  6;  ms.Reset(i);       UT_EQ ( A_CHAR("6")   , ms) }

        {  int64_t  i=  7;  ms.Reset(i);       UT_EQ ( A_CHAR("7")   , ms) }
        {  int64_t  i= -7;  ms.Reset(i);       UT_EQ ( A_CHAR("-7")  , ms) }
        {  uint64_t i=  7;  ms.Reset(i);       UT_EQ ( A_CHAR("7")   , ms) }

        {  int8_t   i=  4;  ms._()._<false>(i);       UT_EQ ( A_CHAR("4")  , ms) }
        {  int8_t   i= -4;  ms._()._<false>(i);       UT_EQ ( A_CHAR("-4") , ms) }
        {  uint8_t  i=  4;  ms._()._<false>(i);       UT_EQ ( A_CHAR("4")  , ms) }


        {  int16_t  i=  5;  ms._()._<false>(i);       UT_EQ ( A_CHAR("5")  , ms) }
        {  int16_t  i= -5;  ms._()._<false>(i);       UT_EQ ( A_CHAR("-5") , ms) }
        {  uint16_t i=  5;  ms._()._<false>(i);       UT_EQ ( A_CHAR("5")  , ms) }

        {  int32_t  i=  6;  ms._()._<false>(i);       UT_EQ ( A_CHAR("6")  , ms) }
        {  int32_t  i= -6;  ms._()._<false>(i);       UT_EQ ( A_CHAR("-6") , ms) }
        {  uint32_t i=  6;  ms._()._<false>(i);       UT_EQ ( A_CHAR("6")  , ms) }

        {  int64_t  i=  7;  ms._()._<false>(i);       UT_EQ ( A_CHAR("7")  , ms) }
        {  int64_t  i= -7;  ms._()._<false>(i);       UT_EQ ( A_CHAR("-7") , ms) }
        {  uint64_t i=  7;  ms._()._<false>(i);       UT_EQ ( A_CHAR("7")  , ms) }
    }

    // Substring
    {
        AString ms;  Substring ssNull(nullptr); Substring ssEmpty(A_CHAR(""));  Substring t( A_CHAR("01234") );

        ms.Reset(ssNull );         UT_EQ  ( 0, ms.Length()        )  UT_TRUE( ms.IsNull()    )
        ms.Reset(ssEmpty);         UT_EQ  ( 0, ms.Length()        )  UT_TRUE( ms.IsNotNull() )
        ms.Reset(t      );         UT_EQ  ( 5, ms.Length()        )  UT_EQ  ( String(t), ms  )
        ms.Reset(ssNull );         UT_EQ  ( 0, ms.Length()        )  UT_TRUE( ms.IsNotNull() )
        ms.SetNull();              UT_EQ  ( 0, ms.Length()        )  UT_TRUE( ms.IsNull()    )

        ms._( ssNull );            UT_EQ  ( 0, ms.Length()        )  UT_TRUE( ms.IsNull()    )
        ms._( ssEmpty);            UT_EQ  ( 0, ms.Length()        )  UT_TRUE( ms.IsNotNull() )
        ms.SetNull();              UT_EQ  ( 0, ms.Length()        )  UT_TRUE( ms.IsNull()    )
        ms._( t );                 UT_EQ  (16, ms.Capacity()      )    UT_EQ( A_CHAR("01234"), ms )
        ms._( t );                 UT_TRUE( ms.Length()> 5     )  UT_EQ( A_CHAR("0123401234"), ms )

        t.ConsumeChar();         ms.Reset( t );           UT_EQ( A_CHAR("1234") , ms  )
        t.ConsumeChar();         ms.Reset( t );           UT_EQ(  A_CHAR("234") , ms  )
        t.ConsumeCharFromEnd();  ms.Reset( t );           UT_EQ( A_CHAR("23")   , ms  )

        // _<false>
        ms.SetNull();             UT_EQ  ( 0, ms.Length()        )  UT_TRUE( ms.IsNull() )
        ms._<false>( ssEmpty );        UT_EQ  ( 0, ms.Length()        )  UT_TRUE( ms.IsNull() )
        ms.Reset()._<false>( t);       UT_EQ( A_CHAR("23"),  ms )
    }

    // std::string
    {
        AString ms;   std::basic_string<character> t( A_CHAR("012") ); std::basic_string<character> ssEmpty(A_CHAR(""));
                                   UT_EQ  ( 0, ms.Length()      )  UT_TRUE( ms.IsNull()    )
        ms.Reset(ssEmpty);         UT_EQ  ( 0, ms.Length()      )  UT_TRUE( ms.IsNotNull() )
        ms.Reset(t      );         UT_EQ  ( 3, ms.Length()      )  UT_EQ  ( t.c_str(), ms  )
        ms.SetNull();

        ms._( ssEmpty);            UT_EQ  ( 0, ms.Length()      )  UT_TRUE( ms.IsNotNull() )
        ms.SetNull();              UT_EQ  ( 0, ms.Length()      )  UT_TRUE( ms.IsNull()    )
        ms._( t );                 UT_EQ  (16, ms.Capacity()    )  UT_EQ( A_CHAR("012")   , ms )
        ms._( t );                 UT_TRUE( ms.Length()> 3      )  UT_EQ( A_CHAR("012012"), ms )
        t= A_CHAR("0123456789");
        ms.Reset()._( t, 5);            UT_EQ( A_CHAR("56789")      , ms )
        ms.Reset()._( t, 5, 100);       UT_EQ( A_CHAR("56789")      , ms )
        ms.Reset()._( t, -5);           UT_EQ( A_CHAR("0123456789") , ms )
        ms.Reset()._( t, -5, 3);        UT_EQ( A_CHAR("")           , ms )
        ms.Reset()._( t, 50, 3);        UT_EQ( A_CHAR("")           , ms )
        ms.Reset()._( t, 10, 3);        UT_EQ( A_CHAR("")           , ms )
        ms.Reset()._( t, -5, 10);       UT_EQ( A_CHAR("01234")      , ms )
        ms.Reset()._( t, -5, 100);      UT_EQ( A_CHAR("0123456789") , ms )

        // _<false>
        ms.SetNull();                   UT_EQ( 0, ms.Length())   UT_TRUE( ms.IsNull() )
        ms._<false>( ssEmpty );         UT_EQ( 0, ms.Length())   UT_TRUE( ms.IsNull() )
        ms.Reset()._<false>( t);        UT_EQ( A_CHAR("0123456789"), ms  )
        ms.Reset()._<false>( t ,2,3);   UT_EQ( A_CHAR("234")       , ms  )
    }

    // string literals
    {
        // zero length literal
        {
            AString ms( A_CHAR("") );         UT_EQ( 0, ms.Length() )   UT_TRUE( ms.IsNotNull() )
        }
        {
            AString ms;
            ms._ ( A_CHAR("") );              UT_EQ( 0, ms.Length() )   UT_TRUE( ms.IsNotNull() )
        }
        {
            AString ms;
            ms <<       A_CHAR("")  ;         UT_EQ( 0, ms.Length() )   UT_TRUE( ms.IsNotNull() )
        }

        // 1 - 6 length literals
        {
            {   AString ms( A_CHAR("a") );       UT_EQ( 1, ms.Length() )   UT_EQ( A_CHAR("a")        , ms  ) }
            {   AString ms( A_CHAR("ab") );      UT_EQ( 2, ms.Length() )   UT_EQ( A_CHAR("ab")       , ms  ) }
            {   AString ms( A_CHAR("abc") );     UT_EQ( 3, ms.Length() )   UT_EQ( A_CHAR("abc")      , ms  ) }
            {   AString ms( A_CHAR("abcd") );    UT_EQ( 4, ms.Length() )   UT_EQ( A_CHAR("abcd")     , ms  ) }
            {   AString ms( A_CHAR("abcde") );   UT_EQ( 5, ms.Length() )   UT_EQ( A_CHAR("abcde")    , ms  ) }
            {   AString ms( A_CHAR("abcdef") );  UT_EQ( 6, ms.Length() )   UT_EQ( A_CHAR("abcdef")   , ms  ) }

            { AString ms;
              ms.Reset( A_CHAR("a") );           UT_EQ( 1, ms.Length() )   UT_EQ( A_CHAR("a")        , ms  )
              ms.Reset( A_CHAR("ab") );          UT_EQ( 2, ms.Length() )   UT_EQ( A_CHAR("ab")       , ms  )
              ms.Reset( A_CHAR("abc") );         UT_EQ( 3, ms.Length() )   UT_EQ( A_CHAR("abc")      , ms  )
              ms.Reset( A_CHAR("abcd") );        UT_EQ( 4, ms.Length() )   UT_EQ( A_CHAR("abcd")     , ms  )
              ms.Reset( A_CHAR("abcde") );       UT_EQ( 5, ms.Length() )   UT_EQ( A_CHAR("abcde")    , ms  )
              ms.Reset( A_CHAR("abcdef") );      UT_EQ( 6, ms.Length() )   UT_EQ( A_CHAR("abcdef")   , ms  ) }
            { AString ms;
              ms.Reset( A_CHAR("a")      );      UT_EQ( 1, ms.Length() )   UT_EQ( A_CHAR("a")        , ms  )
              ms.Reset( A_CHAR("ab")     );      UT_EQ( 2, ms.Length() )   UT_EQ( A_CHAR("ab")       , ms  )
              ms.Reset( A_CHAR("abc")    );      UT_EQ( 3, ms.Length() )   UT_EQ( A_CHAR("abc")      , ms  )
              ms.Reset( A_CHAR("abcd")   );      UT_EQ( 4, ms.Length() )   UT_EQ( A_CHAR("abcd")     , ms  )
              ms.Reset( A_CHAR("abcde")  );      UT_EQ( 5, ms.Length() )   UT_EQ( A_CHAR("abcde")    , ms  )
              ms.Reset( A_CHAR("abcdef") );      UT_EQ( 6, ms.Length() )   UT_EQ( A_CHAR("abcdef")   , ms  ) }
        }
    }
}

//--------------------------------------------------------------------------------------------------
//--- Test Fields
//--------------------------------------------------------------------------------------------------
UT_METHOD( Fields )
{
    UT_INIT()

    AString ms;

    // empty fields
    ms._( Format::Field( nullptr, 3, Alignment::Left,     '#' ) );            UT_EQ( A_CHAR("###")          , ms )

    ms.Reset();
    ms._( Format::Field( A_CHAR("") ,     3, Alignment::Left,     '#' ) );    UT_EQ( A_CHAR("###")          , ms )
    ms._( Format::Field( A_CHAR("") ,     4, Alignment::Center,   '*' ) );    UT_EQ( A_CHAR("###****")      , ms )
    ms._( Format::Field( A_CHAR("") ,     5, Alignment::Right,    '+' ) );    UT_EQ( A_CHAR("###****+++++") , ms )

    // field, alignment left
    ms.Reset();
    ms._( Format::Field( A_CHAR("A")       , 5,    Alignment::Left ) );
    ms._( Format::Field( A_CHAR("AB")      , 5,    Alignment::Left ) );
    ms._( Format::Field( A_CHAR("ABC")     , 5,    Alignment::Left ) );
    ms._( Format::Field( A_CHAR("ABCD")    , 5,    Alignment::Left ) );
    ms._( Format::Field( A_CHAR("ABCDE")   , 5,    Alignment::Left ) );
    ms._( Format::Field( A_CHAR("ABCDEF")  , 5,    Alignment::Left ) );
    ms._( Format::Field( A_CHAR("ABCDEFG") , 5,    Alignment::Left ) );
    UT_EQ( A_CHAR("A    AB   ABC  ABCD ABCDEABCDEFABCDEFG"), ms )

    // field, alignment right
    ms.Reset();
    ms._( Format::Field( A_CHAR("A")       , 5,    Alignment::Right ) );
    ms._( Format::Field( A_CHAR("AB")      , 5,    Alignment::Right ) );
    ms._( Format::Field( A_CHAR("ABC")     , 5                      ) ); // btw: right is default
    ms._( Format::Field( A_CHAR("ABCD")    , 5                      ) );
    ms._( Format::Field( A_CHAR("ABCDE")   , 5                      ) );
    ms._( Format::Field( A_CHAR("ABCDEF")  , 5,    Alignment::Right ) );
    ms._( Format::Field( A_CHAR("ABCDEFG") , 5,    Alignment::Right ) );
    UT_EQ( A_CHAR("    A   AB  ABC ABCDABCDEABCDEFABCDEFG"), ms )

    // field, alignment center
    ms.Reset();
    ms._( Format::Field( A_CHAR("A")       , 5,    Alignment::Center) );
    ms._( Format::Field( A_CHAR("AB")      , 5,    Alignment::Center) );
    ms._( Format::Field( A_CHAR("ABC")     , 5,    Alignment::Center) );
    ms._( Format::Field( A_CHAR("ABCD")    , 5,    Alignment::Center) );
    ms._( Format::Field( A_CHAR("ABCDE")   , 5,    Alignment::Center) );
    ms._( Format::Field( A_CHAR("ABCDEF")  , 5,    Alignment::Center) );
    ms._( Format::Field( A_CHAR("ABCDEFG") , 5,    Alignment::Center) );
    UT_EQ( A_CHAR("  A   AB   ABC ABCD ABCDEABCDEFABCDEFG"), ms )

    // a nested field
    ms.Reset();
    ms._(A_CHAR("***"));
        String32 inner  (A_CHAR("OS:"));    inner._(Format::Field( A_CHAR("Linux"), 10, Alignment::Right, '-' ) );
             inner._(A_CHAR(" Lang:")); inner._(Format::Field( A_CHAR("en_US"),  8, Alignment::Right, '-' ) );
        ms._(Format::Field( inner, 35, Alignment::Center, '#' ) );
    ms._(A_CHAR("***"));
    UT_EQ( A_CHAR("***####OS:-----Linux Lang:---en_US####***"), ms )
}




//--------------------------------------------------------------------------------------------------
//--- Test Capacity
//--------------------------------------------------------------------------------------------------
UT_METHOD( CapacityLength )
{
    UT_INIT()

    // null-allocated
    {
        AString ms;
        ms._(  A_CHAR("") );       UT_TRUE( ms.Capacity() > 0 )
    }

    {
        AString ms;
        ms.SetBuffer(5);
        ms._( 'a' );        UT_EQ( 5, ms.Capacity() )
        ms._( 'b' );        UT_EQ( 5, ms.Capacity() )
        ms._( 'c' );        UT_EQ( 5, ms.Capacity() )
        ms._( 'd' );        UT_EQ( 5, ms.Capacity() )
        ms._( 'e' );        UT_EQ( 5, ms.Capacity() )
        ms._( 'x' );        UT_TRUE( ms.Capacity() >= 6 )
        UT_EQ( A_CHAR("abcdex"), ms )

        ms.GrowBufferAtLeastBy( 2000 );      UT_TRUE( ms.Capacity() >= 2000 )
    }

    {
        AString   ms;
        integer as;
        ms._( A_CHAR("1234567890") );           UT_EQ( 10, ms.Length() )    UT_TRUE  ( ms.Capacity() >= 10 )

        as= ms.Capacity();

        lib::results::Report::GetDefault().PushHaltFlags( false, false );
            UT_PRINT( "An error should follow" )
            ms.ShortenTo(20);                    UT_EQ( 20, ms.Length() )    UT_EQ  ( ms.Capacity(), as )

            ms.Reset( A_CHAR("123456789012345") );
            UT_PRINT( "No warning should follow" )
            ms.ShortenTo(10);                    UT_EQ( 10, ms.Length() )    UT_EQ  ( ms.Capacity(), as )    UT_EQ(A_CHAR("1234567890")   ,  ms  )

            UT_PRINT( "A warning should follow" )
            ms.ShortenTo(11);                    UT_EQ( 11, ms.Length() )    UT_EQ  ( ms.Capacity(), as )    UT_EQ(A_CHAR("12345678901")   ,  ms  )

        lib::results::Report::GetDefault().PopHaltFlags();

        ms.ShortenTo(5);                         UT_EQ( 5, ms.Length() )     UT_EQ  ( ms.Capacity(), as )    UT_EQ( A_CHAR("12345")  , ms    )
        ms.SetBuffer(3);                         UT_EQ( 3, ms.Length() )     UT_EQ  ( ms.Capacity(), 3  )    UT_EQ( A_CHAR("123")    , ms    )
        ms._(A_CHAR("ABC"));                     UT_EQ( 6, ms.Length() )     UT_TRUE( ms.Capacity() >= 6)    UT_EQ( A_CHAR("123ABC") , ms    )
    }

    // external buffer, heap allocated (transfer responsibility)
    #if !ALIB_DEBUG_STRINGS
    {
        AString* ms;
        ms= new AString();
        // we use std::malloc, as this is used inside AString. Otherwise valgrind would warn of a mismatch new[]/delete
        ms->SetBuffer( static_cast<character*>(std::malloc(static_cast<size_t>(5) * sizeof(character))), 5, 0, Responsibility::Transfer );
        ms->_(A_CHAR("ABC"));                  UT_EQ( 3, ms->Length() )       UT_TRUE ( ms->Capacity() == 4 )  UT_EQ( A_CHAR("ABC")   , *ms  )

        delete ms;

        ms= new AString();
        ms->SetBuffer( static_cast<character*>(std::malloc(static_cast<size_t>(5) * sizeof(character))), 5, 0, Responsibility::Transfer );
        ms->_(A_CHAR("ABC"));
        ms->_(A_CHAR("ABC"));                  UT_EQ( 6, ms->Length() )       UT_TRUE ( ms->Capacity() > 4 )  UT_EQ( A_CHAR("ABCABC")   , *ms  )
        delete ms;
    }
    #endif

    // external buffer, stack allocated (keep responsibility)
    {
        character stackCA1[5];
        AString sMS1;
        sMS1.SetBuffer( reinterpret_cast<character*>(&stackCA1), 5 );
        sMS1._(A_CHAR("ABC"));                 UT_TRUE ( sMS1.Buffer() == reinterpret_cast<character*>(&stackCA1) )  UT_EQ( A_CHAR("ABC")   , sMS1  )

        lib::results::Report::GetDefault().PushHaltFlags( false, false );
            character stackCA2[5];
            AString sMS2;
            sMS2.SetBuffer( reinterpret_cast<character*>(&stackCA2), 5 );
            sMS2._(A_CHAR("ABC"));
            UT_PRINT( "A warning should follow" )
            sMS2._(A_CHAR("ABC"));                UT_TRUE ( sMS2.Buffer() != reinterpret_cast<character*>(&stackCA2) )  UT_EQ( A_CHAR("ABCABC")   , sMS2  )

            LocalString<5> msS;
            const character* orig= msS.Buffer();
            msS._(A_CHAR("ABC"));                 UT_TRUE ( msS.Buffer() == orig )  UT_EQ( A_CHAR("ABC")   , String(msS)  )
            UT_PRINT( "A warning should follow" )
            msS._(A_CHAR("ABC"));                 UT_TRUE ( msS.Buffer() != orig )  UT_EQ( A_CHAR("ABCABC"), String(msS)  )

            String64 ms64;
            orig= ms64.Buffer();
            ms64._(A_CHAR("ABC"));                UT_TRUE ( ms64.Buffer() == orig)  UT_EQ( A_CHAR("ABC")   , String(ms64) )
            ms64.Reset();
            ms64.InsertChars('@', 63 );   UT_TRUE ( ms64.Buffer() == orig )
            UT_PRINT( "A warning should follow" )
            ms64._( '@' );

            UT_TRUE ( ms64 != orig )
            UT_TRUE ( ms64.SearchAndReplace( '@', 'X' ) == 64 )
        lib::results::Report::GetDefault().PopHaltFlags();
    }
}

//--------------------------------------------------------------------------------------------------
//--- Test Tab
//--------------------------------------------------------------------------------------------------
UT_METHOD( Tab )
{
    UT_INIT()

    AString ms;

    // test some bad input
        ms._(Format::Tab(  0, 0, -1,        '@' ));        UT_EQ( EmptyString(),  ms )
    ms.Reset(Format::Tab(  0, 0,  0,        '@' ));        UT_EQ( EmptyString(),  ms )
    ms.Reset(Format::Tab( -1, 0,  0,        '@' ));        UT_EQ( EmptyString(),  ms )
    ms.Reset(Format::Tab( -1, 0,  0,        '@' ));        UT_EQ( EmptyString(),  ms )
    ms.Reset(Format::Tab( -1, 0,  -1,       '@' ));        UT_EQ( EmptyString(),  ms )
    ms.Reset(Format::Tab( -5, 0,  0,        '@' ));        UT_EQ( EmptyString(),  ms )
    ms.Reset(Format::Tab( -5, 0,  -100,     '@' ));        UT_EQ( EmptyString(),  ms )
    ms.Reset(Format::Tab(  2, 0,  -101,     '@' ));        UT_EQ( EmptyString(),  ms )
    ms.Reset(Format::Tab(  0, 0,  0,        '@' ));        UT_EQ( EmptyString(),  ms )
    ms.Reset(Format::Tab(  1, 0,  0,        '@' ));        UT_EQ( EmptyString(),  ms )
    ms.Reset(Format::Tab(  1, 0,  1,        '@' ));        UT_EQ( A_CHAR("@"), ms )
    ms.Reset(Format::Tab(  0, 0,  1,        '@' ));        UT_EQ( A_CHAR("@"), ms )


    // minpad 0 (not common, default is 1, tab wont move if on tab position)
    ms.Reset();                    ms._(Format::Tab( 2, 0, 0,    '@' ) )._( '-' );       UT_EQ( A_CHAR("-"),   ms )
    ms.Reset();    ms._( 'x' );    ms._(Format::Tab( 2, 0, 0,    '@' ) )._( '-' );       UT_EQ( A_CHAR("x@-"), ms )

    // more standard tabs
    ms.Reset();
    ms._( A_CHAR("12") )._(Format::Tab( 2 ))
      ._(  '1' )._(Format::Tab( 2 ))
                ._(Format::Tab( 2 ))
      ._(  '@' );
    UT_EQ( A_CHAR("12  1   @"), ms )

    ms.Reset();
    ms._( A_CHAR("12") )._(Format::Tab( 2, 0, 0 ))
      ._(  '1' )._(Format::Tab( 2, 0, 0 ))
      ._(Format::Tab(2, 0, 0))
      ._(  '@' );
    UT_EQ( A_CHAR("121 @"), ms )


    ms.Reset();
    ms._( A_CHAR("12345") )._(Format::Tab( 5 ))
      ._( A_CHAR("1234") ) ._(Format::Tab( 5 ))
      ._( A_CHAR("123") )  ._(Format::Tab( 5 ))
      ._( A_CHAR("12") )   ._(Format::Tab( 5 ))
      ._(  '1' )   ._(Format::Tab( 5 ))
                   ._(Format::Tab( 5 ))
      ._(  '@' );
    UT_EQ( A_CHAR("12345     1234 123  12   1         @"), ms )

    ms.Reset();
    ms._( A_CHAR("12345") )._(Format::Tab( 5, 0, 0 ))
      ._( A_CHAR("1234") ) ._(Format::Tab( 5, 0, 0 ))
      ._( A_CHAR("123") )  ._(Format::Tab( 5, 0, 0 ))
      ._( A_CHAR("12") )   ._(Format::Tab( 5, 0, 0 ))
      ._(  '1' )   ._(Format::Tab( 5, 0, 0 ))
                   ._(Format::Tab( 5, 0, 0 ))
      ._(  '@' );
    UT_EQ( A_CHAR("123451234 123  12   1    @"), ms )

    ms.Reset();                   ms._(Format::Tab( 0 ))  ._( '-' );        UT_EQ( A_CHAR(" -")    , ms )
    ms.Reset();                   ms._(Format::Tab( 1 ))  ._( '-' );        UT_EQ( A_CHAR(" -")    , ms )
    ms.Reset();                   ms._(Format::Tab( 2 ))  ._( '-' );        UT_EQ( A_CHAR("  -")   , ms )
    ms.Reset();                   ms._(Format::Tab( 3 ))  ._( '-' );        UT_EQ( A_CHAR("   -")  , ms )
    ms.Reset();    ms._( 'x' );   ms._(Format::Tab( 1 ))  ._( '-' );        UT_EQ( A_CHAR("x -")   , ms )
    ms.Reset();    ms._( 'x' );   ms._(Format::Tab( 2 ))  ._( '-' );        UT_EQ( A_CHAR("x -")   , ms )
    ms.Reset();    ms._( 'x' );   ms._(Format::Tab( 3 ))  ._( '-' );        UT_EQ( A_CHAR("x  -")  , ms )

    // tabs with tab reference set (designed for multi line tabs)
    ms.Reset(A_CHAR("ABC")).NewLine();
    ms._( A_CHAR("12345") )._(Format::Tab( 5 , -1))
      ._( A_CHAR("1234") ) ._(Format::Tab( 5 , -1))
      ._( A_CHAR("123") )  ._(Format::Tab( 5 , -1))
      ._( A_CHAR("12") )   ._(Format::Tab( 5 , -1))
      ._(  '1' )   ._(Format::Tab( 5 , -1))
                   ._(Format::Tab( 5 , -1))
      ._(  '@' );
    UT_EQ( String64(A_CHAR("ABC")).NewLine()._( A_CHAR("12345     1234 123  12   1         @") ), ms )

    ms.Reset(A_CHAR("ABC")).NewLine();
    ms._( A_CHAR("12345") )._(Format::Tab( 5, -1, 0 ))
      ._( A_CHAR("1234") ) ._(Format::Tab( 5, -1, 0 ))
      ._( A_CHAR("123") )  ._(Format::Tab( 5, -1, 0 ))
      ._( A_CHAR("12") )   ._(Format::Tab( 5, -1, 0 ))
      ._(  '1' )   ._(Format::Tab( 5, -1, 0 ))
                   ._(Format::Tab( 5, -1, 0 ))
      ._(  '@' );
    UT_EQ( String32(A_CHAR("ABC")).NewLine()._( A_CHAR("123451234 123  12   1    @") )        , ms )


}

//--------------------------------------------------------------------------------------------------
//--- Test SearchReplace
//--------------------------------------------------------------------------------------------------
UT_METHOD( SearchAndReplace )
{
    UT_INIT()

    integer result;

    // search characters
    {
        String ms(A_CHAR("abcd abcd"));
        result= ms.IndexOf       ( '@', -5  );   UT_EQ(    -1,  result )
        result= ms.IndexOf       ( '@'      );   UT_EQ(    -1,  result )
        result= ms.IndexOf       ( '@', 5   );   UT_EQ(    -1,  result )
        result= ms.IndexOf       ( '@', 150 );   UT_EQ(    -1,  result )

        result= ms.IndexOf       ( 'a'      );   UT_EQ(    0,   result )
        result= ms.IndexOf       ( 'b'      );   UT_EQ(    1,   result )
        result= ms.IndexOf       ( 'c'      );   UT_EQ(    2,   result )

        result= ms.IndexOf       ( 'a', 0   );   UT_EQ(    0,   result )
        result= ms.IndexOf       ( 'b', 0   );   UT_EQ(    1,   result )
        result= ms.IndexOf       ( 'c', 0   );   UT_EQ(    2,   result )

        result= ms.IndexOf       ( 'a', 1   );   UT_EQ(    5,   result )
        result= ms.IndexOf       ( 'b', 1   );   UT_EQ(    1,   result )
        result= ms.IndexOf       ( 'c', 1   );   UT_EQ(    2,   result )

        result= ms.IndexOf       ( 'a', 2   );   UT_EQ(    5,   result )
        result= ms.IndexOf       ( 'b', 2   );   UT_EQ(    6,   result )
        result= ms.IndexOf       ( 'c', 2   );   UT_EQ(    2,   result )

        result= ms.IndexOf       ( 'a', 3   );   UT_EQ(    5,   result )
        result= ms.IndexOf       ( 'b', 3   );   UT_EQ(    6,   result )
        result= ms.IndexOf       ( 'c', 3   );   UT_EQ(    7,   result )

        result= ms.IndexOf       ( 'd', 7   );   UT_EQ(    8,   result )
        result= ms.IndexOf       ( 'd', 8   );   UT_EQ(    8,   result )
        result= ms.IndexOf       ( 'd', 9   );   UT_EQ(   -1,   result )

        result= ms.IndexOf<false>( '@', 5   );   UT_EQ(    -1,  result )

        result= ms.IndexOf<false>( 'a', 0   );   UT_EQ(    0,   result )
        result= ms.IndexOf<false>( 'b', 0   );   UT_EQ(    1,   result )
        result= ms.IndexOf<false>( 'c', 0   );   UT_EQ(    2,   result )

        result= ms.IndexOf<false>( 'a', 1   );   UT_EQ(    5,   result )
        result= ms.IndexOf<false>( 'b', 1   );   UT_EQ(    1,   result )
        result= ms.IndexOf<false>( 'c', 1   );   UT_EQ(    2,   result )

        result= ms.IndexOf<false>( 'a', 2   );   UT_EQ(    5,   result )
        result= ms.IndexOf<false>( 'b', 2   );   UT_EQ(    6,   result )
        result= ms.IndexOf<false>( 'c', 2   );   UT_EQ(    2,   result )

        result= ms.IndexOf<false>( 'a', 3   );   UT_EQ(    5,   result )
        result= ms.IndexOf<false>( 'b', 3   );   UT_EQ(    6,   result )
        result= ms.IndexOf<false>( 'c', 3   );   UT_EQ(    7,   result )

        result= ms.IndexOf<false>( 'd', 7   );   UT_EQ(    8,   result )
        result= ms.IndexOf<false>( 'd', 8   );   UT_EQ(    8,   result )
    }

    {
        AString ms(A_CHAR("0123456789"));
        result= ms.IndexOf       ( '1',    -5,   3 );   UT_EQ( -1,  result )
        result= ms.IndexOf       ( '1',     0,   1 );   UT_EQ( -1,  result )
        result= ms.IndexOf       ( '1',     0,   2 );   UT_EQ(  1,  result )
        result= ms.IndexOf       ( '2',     0,   2 );   UT_EQ( -1,  result )
        result= ms.IndexOf       ( '2',     1,   2 );   UT_EQ(  2,  result )
        result= ms.IndexOf       ( '2',     1,   3 );   UT_EQ(  2,  result )
        result= ms.IndexOf       ( '2',     2,   3 );   UT_EQ(  2,  result )
        result= ms.IndexOf       ( '2',     2,   1 );   UT_EQ(  2,  result )
        result= ms.IndexOf       ( '2',     2,   0 );   UT_EQ( -1,  result )
        result= ms.IndexOf       ( '2',     3,   0 );   UT_EQ( -1,  result )
        result= ms.IndexOf       ( '2',     3,   1 );   UT_EQ( -1,  result )
        result= ms.IndexOf       ( '2',   100,  20 );   UT_EQ( -1,  result )
        result= ms.IndexOf       ( '2',  -100, 102 );   UT_EQ( -1,  result )
        result= ms.IndexOf       ( '2',  -100, 103 );   UT_EQ(  2,  result )
        result= ms.IndexOf       ( '2',  -100, 202 );   UT_EQ(  2,  result )

        result= ms.IndexOf<false>( '1',     0,   1 );   UT_EQ( -1,  result )
        result= ms.IndexOf<false>( '1',     0,   2 );   UT_EQ(  1,  result )
        result= ms.IndexOf<false>( '2',     0,   2 );   UT_EQ( -1,  result )
        result= ms.IndexOf<false>( '2',     1,   2 );   UT_EQ(  2,  result )
        result= ms.IndexOf<false>( '2',     1,   3 );   UT_EQ(  2,  result )
        result= ms.IndexOf<false>( '2',     2,   3 );   UT_EQ(  2,  result )
        result= ms.IndexOf<false>( '2',     2,   1 );   UT_EQ(  2,  result )
        result= ms.IndexOf<false>( '2',     3,   1 );   UT_EQ( -1,  result )
    }

    // ------------------ search char backwards ------------------
    {
        AString ms(A_CHAR("abcd abcd"));
        result= ms.LastIndexOf       ( '@', -5  );   UT_EQ(    -1,  result )
        result= ms.LastIndexOf       ( '@'      );   UT_EQ(    -1,  result )
        result= ms.LastIndexOf       ( '@', 5   );   UT_EQ(    -1,  result )
        result= ms.LastIndexOf       ( '@', 150 );   UT_EQ(    -1,  result )

        result= ms.LastIndexOf       ( 'a'      );   UT_EQ(    5,   result )
        result= ms.LastIndexOf       ( 'b'      );   UT_EQ(    6,   result )
        result= ms.LastIndexOf       ( 'c'      );   UT_EQ(    7,   result )
        result= ms.LastIndexOf       ( 'd'      );   UT_EQ(    8,   result )

        result= ms.LastIndexOf       ( 'a', 0   );   UT_EQ(    0,   result )
        result= ms.LastIndexOf       ( 'b', 0   );   UT_EQ(   -1,   result )
        result= ms.LastIndexOf       ( 'c', 0   );   UT_EQ(   -1,   result )

        result= ms.LastIndexOf       ( 'a', 1   );   UT_EQ(    0,   result )
        result= ms.LastIndexOf       ( 'b', 1   );   UT_EQ(    1,   result )
        result= ms.LastIndexOf       ( 'c', 1   );   UT_EQ(   -1,   result )

        result= ms.LastIndexOf       ( 'a', 2   );   UT_EQ(    0,   result )
        result= ms.LastIndexOf       ( 'b', 2   );   UT_EQ(    1,   result )
        result= ms.LastIndexOf       ( 'c', 2   );   UT_EQ(    2,   result )

        result= ms.LastIndexOf       ( 'a', 3   );   UT_EQ(    0,   result )
        result= ms.LastIndexOf       ( 'b', 3   );   UT_EQ(    1,   result )
        result= ms.LastIndexOf       ( 'c', 3   );   UT_EQ(    2,   result )

        result= ms.LastIndexOf       ( 'd', 7   );   UT_EQ(    3,   result )
        result= ms.LastIndexOf       ( 'd', 8   );   UT_EQ(    8,   result )
        result= ms.LastIndexOf       ( 'd', 9   );   UT_EQ(    8,   result )
        result= ms.LastIndexOf       ( 'd', 10  );   UT_EQ(    8,   result )
        result= ms.LastIndexOf       ( 'd', 11  );   UT_EQ(    8,   result )
        result= ms.LastIndexOf       ( 'c', 11  );   UT_EQ(    7,   result )


        result= ms.LastIndexOf<false>( '@', 5   );   UT_EQ(    -1,  result )

        result= ms.LastIndexOf<false>( 'a', 0   );   UT_EQ(    0,   result )
        result= ms.LastIndexOf<false>( 'b', 0   );   UT_EQ(   -1,   result )
        result= ms.LastIndexOf<false>( 'c', 0   );   UT_EQ(   -1,   result )

        result= ms.LastIndexOf<false>( 'a', 1   );   UT_EQ(    0,   result )
        result= ms.LastIndexOf<false>( 'b', 1   );   UT_EQ(    1,   result )
        result= ms.LastIndexOf<false>( 'c', 1   );   UT_EQ(   -1,   result )

        result= ms.LastIndexOf<false>( 'a', 2   );   UT_EQ(    0,   result )
        result= ms.LastIndexOf<false>( 'b', 2   );   UT_EQ(    1,   result )
        result= ms.LastIndexOf<false>( 'c', 2   );   UT_EQ(    2,   result )

        result= ms.LastIndexOf<false>( 'a', 3   );   UT_EQ(    0,   result )
        result= ms.LastIndexOf<false>( 'b', 3   );   UT_EQ(    1,   result )
        result= ms.LastIndexOf<false>( 'c', 3   );   UT_EQ(    2,   result )

        result= ms.LastIndexOf<false>( 'd', 7   );   UT_EQ(    3,   result )
        result= ms.LastIndexOf<false>( 'd', 8   );   UT_EQ(    8,   result )
    }


    // ------------------ search one of several characters ------------------
    // non static version
    {
        String ms(A_CHAR("abcd abcde"));

        // search one of
        integer l= ms.Length();
        result= ms.IndexOfAny    <Inclusion::Include       >( A_CHAR("")           );    UT_EQ(  -1, result )
        result= ms.IndexOfAny    <Inclusion::Include       >( A_CHAR("x")          );    UT_EQ(  -1, result )
        result= ms.IndexOfAny    <Inclusion::Include       >( A_CHAR("xy")         );    UT_EQ(  -1, result )
        result= ms.IndexOfAny    <Inclusion::Include       >( A_CHAR("xyz")        );    UT_EQ(  -1, result )
        result= ms.IndexOfAny    <Inclusion::Include       >( A_CHAR("xyd")        );    UT_EQ(   3, result )
        result= ms.IndexOfAny    <Inclusion::Include       >( A_CHAR("d")          );    UT_EQ(   3, result )
        result= ms.IndexOfAny    <Inclusion::Include       >( A_CHAR("xyd") ,  -2  );    UT_EQ(   3, result )
        result= ms.IndexOfAny    <Inclusion::Include       >( A_CHAR("xyd") ,   4  );    UT_EQ(   8, result )
        result= ms.IndexOfAny    <Inclusion::Include       >( A_CHAR("xyd") ,  20  );    UT_EQ(  -1, result )
        result= ms.LastIndexOfAny<Inclusion::Include       >( A_CHAR("")           );    UT_EQ(  -1, result )
        result= ms.LastIndexOfAny<Inclusion::Include       >( A_CHAR("x")          );    UT_EQ(  -1, result )
        result= ms.LastIndexOfAny<Inclusion::Include       >( A_CHAR("xy")         );    UT_EQ(  -1, result )
        result= ms.LastIndexOfAny<Inclusion::Include       >( A_CHAR("xyz")        );    UT_EQ(  -1, result )
        result= ms.LastIndexOfAny<Inclusion::Include       >( A_CHAR("xyd")        );    UT_EQ(   8, result )
        result= ms.LastIndexOfAny<Inclusion::Include       >( A_CHAR("xyd") ,  -2  );    UT_EQ(  -1, result )
        result= ms.LastIndexOfAny<Inclusion::Include       >( A_CHAR("xyd") ,   2  );    UT_EQ(  -1, result )
        result= ms.LastIndexOfAny<Inclusion::Include       >( A_CHAR("xyd") ,   4  );    UT_EQ(   3, result )
        result= ms.LastIndexOfAny<Inclusion::Include       >( A_CHAR("a")   ,   4  );    UT_EQ(   0, result )
        result= ms.LastIndexOfAny<Inclusion::Include       >( A_CHAR("b")   ,   4  );    UT_EQ(   1, result )
        result= ms.LastIndexOfAny<Inclusion::Include       >( A_CHAR("ba")  ,   4  );    UT_EQ(   1, result )
        result= ms.LastIndexOfAny<Inclusion::Include       >( A_CHAR("xa")  ,   4  );    UT_EQ(   0, result )
        result= ms.LastIndexOfAny<Inclusion::Include       >( A_CHAR("xyd") ,  20  );    UT_EQ(   8, result )
        result= ms.LastIndexOfAny<Inclusion::Include       >( A_CHAR("d")   ,  20  );    UT_EQ(   8, result )
        result= ms.LastIndexOfAny<Inclusion::Include       >( A_CHAR("e")   ,  20  );    UT_EQ(   9, result )
        result= ms.IndexOfAny    <Inclusion::Include, false>( A_CHAR("x")          );    UT_EQ(  -1, result )
        result= ms.IndexOfAny    <Inclusion::Include, false>( A_CHAR("xy")         );    UT_EQ(  -1, result )
        result= ms.IndexOfAny    <Inclusion::Include, false>( A_CHAR("xyz")        );    UT_EQ(  -1, result )
        result= ms.IndexOfAny    <Inclusion::Include, false>( A_CHAR("xyd")        );    UT_EQ(   3, result )
        result= ms.IndexOfAny    <Inclusion::Include, false>( A_CHAR("d")          );    UT_EQ(   3, result )
        result= ms.IndexOfAny    <Inclusion::Include, false>( A_CHAR("xyd")  ,  4  );    UT_EQ(   8, result )
        result= ms.LastIndexOfAny<Inclusion::Include, false>( A_CHAR("xyd")  ,  2  );    UT_EQ(  -1, result )
        result= ms.LastIndexOfAny<Inclusion::Include, false>( A_CHAR("xyd")  ,  4  );    UT_EQ(   3, result )
        result= ms.LastIndexOfAny<Inclusion::Include, false>( A_CHAR("a")    ,  4  );    UT_EQ(   0, result )
        result= ms.LastIndexOfAny<Inclusion::Include, false>( A_CHAR("b")    ,  4  );    UT_EQ(   1, result )
        result= ms.LastIndexOfAny<Inclusion::Include, false>( A_CHAR("ba")   ,  4  );    UT_EQ(   1, result )
        result= ms.LastIndexOfAny<Inclusion::Include, false>( A_CHAR("xa")   ,  4  );    UT_EQ(   0, result )

        // search NOT one of
        result= ms.IndexOfAny    <Inclusion::Exclude       >( A_CHAR("")           );    UT_EQ(   0, result )
        result= ms.IndexOfAny    <Inclusion::Exclude       >( A_CHAR("x")          );    UT_EQ(   0, result )
        result= ms.IndexOfAny    <Inclusion::Exclude       >( A_CHAR("xy")         );    UT_EQ(   0, result )
        result= ms.IndexOfAny    <Inclusion::Exclude       >( A_CHAR("xyz")        );    UT_EQ(   0, result )
        result= ms.IndexOfAny    <Inclusion::Exclude       >( A_CHAR("a")          );    UT_EQ(   1, result )
        result= ms.IndexOfAny    <Inclusion::Exclude       >( A_CHAR("ba")         );    UT_EQ(   2, result )
        result= ms.IndexOfAny    <Inclusion::Exclude       >( A_CHAR("abc")        );    UT_EQ(   3, result )
        result= ms.IndexOfAny    <Inclusion::Exclude       >( A_CHAR("acb")        );    UT_EQ(   3, result )
        result= ms.IndexOfAny    <Inclusion::Exclude       >( A_CHAR("cba")        );    UT_EQ(   3, result )
        result= ms.IndexOfAny    <Inclusion::Exclude       >( A_CHAR("xcba")       );    UT_EQ(   3, result )
        result= ms.LastIndexOfAny<Inclusion::Exclude       >( A_CHAR("")           );    UT_EQ( l-1, result )
        result= ms.LastIndexOfAny<Inclusion::Exclude       >( A_CHAR("x")          );    UT_EQ( l-1, result )
        result= ms.LastIndexOfAny<Inclusion::Exclude       >( A_CHAR("xy")         );    UT_EQ( l-1, result )
        result= ms.LastIndexOfAny<Inclusion::Exclude       >( A_CHAR("e")          );    UT_EQ( l-2, result )
        result= ms.LastIndexOfAny<Inclusion::Exclude       >( A_CHAR("de")         );    UT_EQ( l-3, result )
        result= ms.LastIndexOfAny<Inclusion::Exclude       >( A_CHAR("cde")        );    UT_EQ( l-4, result )
        result= ms.LastIndexOfAny<Inclusion::Exclude       >( A_CHAR("ced")        );    UT_EQ( l-4, result )
        result= ms.LastIndexOfAny<Inclusion::Exclude       >( A_CHAR("ecd")        );    UT_EQ( l-4, result )
        result= ms.LastIndexOfAny<Inclusion::Exclude       >( A_CHAR("ecd")  ,  5  );    UT_EQ(   5, result )
        result= ms.LastIndexOfAny<Inclusion::Exclude       >( A_CHAR("ecd")  ,  4  );    UT_EQ(   4, result )
        result= ms.LastIndexOfAny<Inclusion::Exclude       >( A_CHAR("acd")  ,  3  );    UT_EQ(   1, result )
        result= ms.LastIndexOfAny<Inclusion::Exclude       >( A_CHAR("abc")  ,  2  );    UT_EQ(  -1, result )
        result= ms.LastIndexOfAny<Inclusion::Exclude       >( A_CHAR("xay")  ,  3  );    UT_EQ(   3, result )
        result= ms.LastIndexOfAny<Inclusion::Exclude       >( A_CHAR("d")    ,  3  );    UT_EQ(   2, result )
        result= ms.LastIndexOfAny<Inclusion::Exclude       >( A_CHAR("a")    ,  0  );    UT_EQ(  -1, result )
        result= ms.IndexOfAny    <Inclusion::Exclude ,false>( A_CHAR("x")          );    UT_EQ(   0, result )
        result= ms.IndexOfAny    <Inclusion::Exclude ,false>( A_CHAR("xy")         );    UT_EQ(   0, result )
        result= ms.IndexOfAny    <Inclusion::Exclude ,false>( A_CHAR("xyz")        );    UT_EQ(   0, result )
        result= ms.IndexOfAny    <Inclusion::Exclude ,false>( A_CHAR("a")          );    UT_EQ(   1, result )
        result= ms.IndexOfAny    <Inclusion::Exclude ,false>( A_CHAR("ba")         );    UT_EQ(   2, result )
        result= ms.IndexOfAny    <Inclusion::Exclude ,false>( A_CHAR("abc")        );    UT_EQ(   3, result )
        result= ms.IndexOfAny    <Inclusion::Exclude ,false>( A_CHAR("acb")        );    UT_EQ(   3, result )
        result= ms.IndexOfAny    <Inclusion::Exclude ,false>( A_CHAR("cba")        );    UT_EQ(   3, result )
        result= ms.IndexOfAny    <Inclusion::Exclude ,false>( A_CHAR("xcba")       );    UT_EQ(   3, result )
        result= ms.LastIndexOfAny<Inclusion::Exclude ,false>( A_CHAR("ecd")  ,  4  );    UT_EQ(   4, result )
        result= ms.LastIndexOfAny<Inclusion::Exclude ,false>( A_CHAR("acd")  ,  3  );    UT_EQ(   1, result )
        result= ms.LastIndexOfAny<Inclusion::Exclude ,false>( A_CHAR("abc")  ,  2  );    UT_EQ(  -1, result )
        result= ms.LastIndexOfAny<Inclusion::Exclude ,false>( A_CHAR("xay")  ,  3  );    UT_EQ(   3, result )
        result= ms.LastIndexOfAny<Inclusion::Exclude ,false>( A_CHAR("d")    ,  3  );    UT_EQ(   2, result )
        result= ms.LastIndexOfAny<Inclusion::Exclude ,false>( A_CHAR("a")    ,  0  );    UT_EQ(  -1, result )
    }

    // ------------------ search one of several characters CString version  ------------------
    // non static version
    {
        CString ms(A_CHAR("abcd abcde"));
        // search one of
        result= ms.IndexOfAny<Inclusion::Include       >( A_CHAR("")            );    UT_EQ(  -1, result )
        result= ms.IndexOfAny<Inclusion::Include       >( A_CHAR("x")           );    UT_EQ(  -1, result )
        result= ms.IndexOfAny<Inclusion::Include       >( A_CHAR("xy")          );    UT_EQ(  -1, result )
        result= ms.IndexOfAny<Inclusion::Include       >( A_CHAR("xyz")         );    UT_EQ(  -1, result )
        result= ms.IndexOfAny<Inclusion::Include       >( A_CHAR("xyd")         );    UT_EQ(   3, result )
        result= ms.IndexOfAny<Inclusion::Include       >( A_CHAR("d")           );    UT_EQ(   3, result )
        result= ms.IndexOfAny<Inclusion::Include       >( A_CHAR("xyd")   , -2  );    UT_EQ(   3, result )
        result= ms.IndexOfAny<Inclusion::Include       >( A_CHAR("xyd")   ,  4  );    UT_EQ(   8, result )
        result= ms.IndexOfAny<Inclusion::Include       >( A_CHAR("xyd")   , 20  );    UT_EQ(  -1, result )
        result= ms.IndexOfAny<Inclusion::Include, false>( A_CHAR("x")           );    UT_EQ(  -1, result )
        result= ms.IndexOfAny<Inclusion::Include, false>( A_CHAR("xy")          );    UT_EQ(  -1, result )
        result= ms.IndexOfAny<Inclusion::Include, false>( A_CHAR("xyz")         );    UT_EQ(  -1, result )
        result= ms.IndexOfAny<Inclusion::Include, false>( A_CHAR("xyd")         );    UT_EQ(   3, result )
        result= ms.IndexOfAny<Inclusion::Include, false>( A_CHAR("d")           );    UT_EQ(   3, result )
        result= ms.IndexOfAny<Inclusion::Include, false>( A_CHAR("xyd")   ,  4  );    UT_EQ(   8, result )
        result= ms.IndexOfAny<Inclusion::Exclude       >( A_CHAR("")            );    UT_EQ(   0, result )
        result= ms.IndexOfAny<Inclusion::Exclude       >( A_CHAR("x")           );    UT_EQ(   0, result )
        result= ms.IndexOfAny<Inclusion::Exclude       >( A_CHAR("xy")          );    UT_EQ(   0, result )
        result= ms.IndexOfAny<Inclusion::Exclude       >( A_CHAR("xyz")         );    UT_EQ(   0, result )
        result= ms.IndexOfAny<Inclusion::Exclude       >( A_CHAR("a")           );    UT_EQ(   1, result )
        result= ms.IndexOfAny<Inclusion::Exclude       >( A_CHAR("ba")          );    UT_EQ(   2, result )
        result= ms.IndexOfAny<Inclusion::Exclude       >( A_CHAR("abc")         );    UT_EQ(   3, result )
        result= ms.IndexOfAny<Inclusion::Exclude       >( A_CHAR("acb")         );    UT_EQ(   3, result )
        result= ms.IndexOfAny<Inclusion::Exclude       >( A_CHAR("cba")         );    UT_EQ(   3, result )
        result= ms.IndexOfAny<Inclusion::Exclude       >( A_CHAR("xcba")        );    UT_EQ(   3, result )
        result= ms.IndexOfAny<Inclusion::Exclude, false>( A_CHAR("x")           );    UT_EQ(   0, result )
        result= ms.IndexOfAny<Inclusion::Exclude, false>( A_CHAR("xy")          );    UT_EQ(   0, result )
        result= ms.IndexOfAny<Inclusion::Exclude, false>( A_CHAR("xyz")         );    UT_EQ(   0, result )
        result= ms.IndexOfAny<Inclusion::Exclude, false>( A_CHAR("a")           );    UT_EQ(   1, result )
        result= ms.IndexOfAny<Inclusion::Exclude, false>( A_CHAR("ba")          );    UT_EQ(   2, result )
        result= ms.IndexOfAny<Inclusion::Exclude, false>( A_CHAR("abc")         );    UT_EQ(   3, result )
        result= ms.IndexOfAny<Inclusion::Exclude, false>( A_CHAR("acb")         );    UT_EQ(   3, result )
        result= ms.IndexOfAny<Inclusion::Exclude, false>( A_CHAR("cba")         );    UT_EQ(   3, result )
        result= ms.IndexOfAny<Inclusion::Exclude, false>( A_CHAR("xcba")        );    UT_EQ(   3, result )
    }

    // search nullptr, empty string
    {
        CString ms(A_CHAR("abcd abcd"));
        result= ms.IndexOf( nullptr                );    UT_EQ( -1, result )
        result= ms.IndexOf( nullptr,            5  );    UT_EQ( -1, result )
        result= ms.IndexOf( nullptr,           50  );    UT_EQ( -1, result )
        result= ms.IndexOf( nullptr,         -  5  );    UT_EQ( -1, result )
        result= ms.IndexOf( A_CHAR(""),      -  5  );    UT_EQ(  0, result )
        result= ms.IndexOf( A_CHAR(""),         0  );    UT_EQ(  0, result )
        result= ms.IndexOf( A_CHAR(""),         4  );    UT_EQ(  4, result )
        result= ms.IndexOf( A_CHAR(""),       100  );    UT_EQ( -1, result )
    }

    // search
    {
        CString ms(A_CHAR("abcd abcd"));
        result= ms.IndexOf                        ( A_CHAR("abcd")       );  UT_EQ( result,   0 )
        result= ms.IndexOf                        ( A_CHAR("b")          );  UT_EQ( result,   1 )
        result= ms.IndexOf                        ( A_CHAR(" abcd")      );  UT_EQ( result,   4 )
        result= ms.IndexOf                        ( A_CHAR("abcd"),     1);  UT_EQ( result,   5 )
        result= ms.IndexOf                        ( A_CHAR("abcd"),   - 1);  UT_EQ( result,   0 )
        result= ms.IndexOf                        ( A_CHAR("xyz"),    -10);  UT_EQ( result,  -1 )
        result= ms.IndexOf<false, Case::Sensitive>( A_CHAR("abcd")       );  UT_EQ( result,   0 )
        result= ms.IndexOf<false, Case::Sensitive>( A_CHAR("b")          );  UT_EQ( result,   1 )
        result= ms.IndexOf<false, Case::Sensitive>( A_CHAR(" abcd")      );  UT_EQ( result,   4 )
        result= ms.IndexOf<false, Case::Sensitive>( A_CHAR("abcd"),     1);  UT_EQ( result,   5 )
    }

    // ignore case
    {
        AString ms( A_CHAR("Hello A-Worx utilXXX") );
        ms.DeleteEnd(3);
        result= ms.IndexOf<true , Case::Ignore   >( A_CHAR("a-worx"),    0 ); UT_EQ( result,  6 )
        result= ms.IndexOf<true , Case::Ignore   >( A_CHAR("a-worx"),    1 ); UT_EQ( result,  6 )
        result= ms.IndexOf<true , Case::Ignore   >( A_CHAR("a-worx"),  -10 ); UT_EQ( result,  6 )
        result= ms.IndexOf<true , Case::Ignore   >( A_CHAR("a-worx"),    6 ); UT_EQ( result,  6 )
        result= ms.IndexOf<true , Case::Ignore   >( A_CHAR("a-worx"),    7 ); UT_EQ( result, -1 )
        result= ms.IndexOf<true , Case::Ignore   >( A_CHAR("a-worx"),  100 ); UT_EQ( result, -1 )
        result= ms.IndexOf<true , Case::Ignore   >( A_CHAR("hel"),       0 ); UT_EQ( result,  0 )
        result= ms.IndexOf<true , Case::Ignore   >( A_CHAR("hel"),       1 ); UT_EQ( result, -1 )
        result= ms.IndexOf<true , Case::Ignore   >( A_CHAR("util"),      1 ); UT_EQ( result, 13 )
        result= ms.IndexOf<true , Case::Ignore   >( A_CHAR("UTIL"),      5 ); UT_EQ( result, 13 )
        result= ms.IndexOf<true , Case::Ignore   >( A_CHAR("UTIL"),     13 ); UT_EQ( result, 13 )
        result= ms.IndexOf<true , Case::Ignore   >( A_CHAR("UTIL"),     14 ); UT_EQ( result, -1 )
        result= ms.IndexOf<false, Case::Ignore   >( A_CHAR("a-worx"),    0 ); UT_EQ( result,  6 )
        result= ms.IndexOf<false, Case::Ignore   >( A_CHAR("a-worx"),    1 ); UT_EQ( result,  6 )
        result= ms.IndexOf<false, Case::Ignore   >( A_CHAR("a-worx"),    6 ); UT_EQ( result,  6 )
        result= ms.IndexOf<false, Case::Ignore   >( A_CHAR("a-worx"),    7 ); UT_EQ( result, -1 )
        result= ms.IndexOf<false, Case::Ignore   >( A_CHAR("hel"),       0 ); UT_EQ( result,  0 )
        result= ms.IndexOf<false, Case::Ignore   >( A_CHAR("hel"),       1 ); UT_EQ( result, -1 )
        result= ms.IndexOf<false, Case::Ignore   >( A_CHAR("util"),      1 ); UT_EQ( result, 13 )
        result= ms.IndexOf<false, Case::Ignore   >( A_CHAR("UTIL"),      5 ); UT_EQ( result, 13 )
        result= ms.IndexOf<false, Case::Ignore   >( A_CHAR("UTIL"),     13 ); UT_EQ( result, 13 )
    }

    // IndexOfFirstDifference
    {
        String as(A_CHAR("abcdef"));
        UT_EQ( 6, as.IndexOfFirstDifference(A_CHAR("abcdef")) )
        UT_EQ( 5, as.IndexOfFirstDifference(A_CHAR("abcde")) )
        UT_EQ( 6, as.IndexOfFirstDifference(A_CHAR("abcdefg")) )

        UT_EQ( 0, as.IndexOfFirstDifference(A_CHAR("123")) )
        UT_EQ( 0, as.IndexOfFirstDifference(A_CHAR("123"), Case::Ignore   ) )
        UT_EQ( 0, as.IndexOfFirstDifference(A_CHAR("123"), Case::Ignore, 2) )

        UT_EQ( 3, as.IndexOfFirstDifference(A_CHAR("abc")) )
        UT_EQ( 3, as.IndexOfFirstDifference(A_CHAR("abc"), Case::Ignore   ) )
        UT_EQ( 0, as.IndexOfFirstDifference(A_CHAR("abc"), Case::Ignore, 1) )
        UT_EQ( 0, as.IndexOfFirstDifference( A_CHAR("bc"), Case::Ignore, 0) )
        UT_EQ( 2, as.IndexOfFirstDifference( A_CHAR("bc"), Case::Ignore, 1) )
        UT_EQ( 1, as.IndexOfFirstDifference( A_CHAR("bd"), Case::Ignore, 1) )


        UT_EQ( 3, as.IndexOfFirstDifference(A_CHAR("ABC"), Case::Ignore   ) )
        UT_EQ( 0, as.IndexOfFirstDifference(A_CHAR("ABC"), Case::Ignore, 1) )
        UT_EQ( 0, as.IndexOfFirstDifference( A_CHAR("BC"), Case::Ignore, 0) )
        UT_EQ( 2, as.IndexOfFirstDifference( A_CHAR("BC"), Case::Ignore, 1) )
        UT_EQ( 1, as.IndexOfFirstDifference( A_CHAR("BD"), Case::Ignore, 1) )


        UT_EQ( 0, as.IndexOfFirstDifference(A_CHAR("ABC")) )
        UT_EQ( 0, as.IndexOfFirstDifference(A_CHAR("ABC"), Case::Sensitive   ) )
        UT_EQ( 0, as.IndexOfFirstDifference(A_CHAR("ABC"), Case::Sensitive, 1) )
        UT_EQ( 0, as.IndexOfFirstDifference( A_CHAR("BC"), Case::Sensitive, 0) )
        UT_EQ( 0, as.IndexOfFirstDifference( A_CHAR("BC"), Case::Sensitive, 1) )
        UT_EQ( 0, as.IndexOfFirstDifference( A_CHAR("BD"), Case::Sensitive, 1) )
    }

    // replace nullptr
    {
        AString ms(A_CHAR("Hello"));
        ms.SearchAndReplace( nullptr  ,   NullString() );     UT_EQ( A_CHAR("Hello") , ms )
        ms.SearchAndReplace( A_CHAR("l"), NullString() );     UT_EQ( A_CHAR("Heo")   , ms )
        ms.SearchAndReplace( nullptr    , A_CHAR("xx") );     UT_EQ( A_CHAR("Heo")   , ms )
    }

    // SearchAndReplace nullptr
    {
        AString ms(A_CHAR("Hello"));
        result= ms.SearchAndReplace( nullptr    ,  NullString() );  UT_EQ( A_CHAR("Hello") , ms )   UT_EQ( 0, result )
        result= ms.SearchAndReplace( A_CHAR("l"),  NullString() );  UT_EQ( A_CHAR("Heo")   , ms )   UT_EQ( 2, result )
        result= ms.SearchAndReplace( nullptr    ,  A_CHAR("xx") );  UT_EQ( A_CHAR("Heo")   , ms )   UT_EQ( 0, result )
    }

    // replace
    {
        AString ms(A_CHAR("Hello W!"));
        result= ms.SearchAndReplace( A_CHAR("W!"),  A_CHAR("world!")   );    UT_EQ( A_CHAR("Hello world!")       ,ms )      UT_EQ( 1, result )
        result= ms.SearchAndReplace( A_CHAR(" "),   A_CHAR("* *")      );    UT_EQ( A_CHAR("Hello* *world!")     ,ms )      UT_EQ( 1, result )
        result= ms.SearchAndReplace( A_CHAR("*"),   A_CHAR("#"), 0, 0  );    UT_EQ( A_CHAR("Hello* *world!")     ,ms )      UT_EQ( 0, result )
        result= ms.SearchAndReplace( A_CHAR("*"),   A_CHAR("#"), 0, 1  );    UT_EQ( A_CHAR("Hello# *world!")     ,ms )      UT_EQ( 1, result )
        result= ms.SearchAndReplace( A_CHAR("*"),   A_CHAR("#")        );    UT_EQ( A_CHAR("Hello# #world!")     ,ms )      UT_EQ( 1, result )
        result= ms.SearchAndReplace( A_CHAR("#"),   A_CHAR("$$$")      );    UT_EQ( A_CHAR("Hello$$$ $$$world!") ,ms )      UT_EQ( 2, result )
        result= ms.SearchAndReplace( A_CHAR("$$$"), A_CHAR("*")        );    UT_EQ( A_CHAR("Hello* *world!")     ,ms )      UT_EQ( 2, result )
        result= ms.SearchAndReplace( A_CHAR("*"),   A_CHAR("")         );    UT_EQ( A_CHAR("Hello world!")       ,ms )      UT_EQ( 2, result )
    }

    // replace one space by two spaces in a string of spaces
    {
        AString ms(A_CHAR("     "));
        result= ms.SearchAndReplace( A_CHAR(" "),   A_CHAR("  ")  );    UT_EQ( A_CHAR("          "), ms  )        UT_EQ( 5, result )
    }

    // replace ignoreCase
    {
        AString ms;
        ms.Reset(A_CHAR("ABC")); result= ms.SearchAndReplace( A_CHAR("abc"),    A_CHAR("xxx"),0, 1, Case::Sensitive );    UT_EQ( A_CHAR("ABC"), ms )  UT_EQ( 0, result )
        ms.Reset(A_CHAR("ABC")); result= ms.SearchAndReplace( A_CHAR("abc"),    A_CHAR("xxx"),0, 1, Case::Ignore    );    UT_EQ( A_CHAR("xxx"), ms )  UT_EQ( 1, result )
        ms.Reset(A_CHAR("ABC")); result= ms.SearchAndReplace( A_CHAR("ABC"),    A_CHAR("xxx"),0, 1, Case::Ignore    );    UT_EQ( A_CHAR("xxx"), ms )  UT_EQ( 1, result )
        ms.Reset(A_CHAR("abc"));         ms.SearchAndReplace( A_CHAR("ABC"),    A_CHAR("xxx"),0, 1, Case::Sensitive );    UT_EQ( A_CHAR("abc"), ms )
        ms.Reset(A_CHAR("abc"));         ms.SearchAndReplace( A_CHAR("ABC"),    A_CHAR("xxx"),0, 1, Case::Ignore    );    UT_EQ( A_CHAR("xxx"), ms )
        ms.Reset(A_CHAR("abc"));         ms.SearchAndReplace( A_CHAR("abc"),    A_CHAR("xxx"),0, 1, Case::Ignore    );    UT_EQ( A_CHAR("xxx"), ms )
    }

}


//--------------------------------------------------------------------------------------------------
//--- Test Trim
//--------------------------------------------------------------------------------------------------
UT_METHOD( Count )
{
    UT_INIT()

    // Count
    String str;

    str= A_CHAR("");
    UT_EQ( 0, str.Count( NullString() ) )
    UT_EQ( 0, str.Count( A_CHAR("")  ) )
    UT_EQ( 0, str.Count( A_CHAR("x") ) )

    UT_EQ( 0, str.CountChar( 'x' ) )
    UT_EQ( 0, str.CountChar( '\0') )

    str= A_CHAR("abcdef");
    UT_EQ( 1, str.Count( A_CHAR("a")  ) )   UT_EQ( 1, str.CountChar( 'a'  ) )   UT_EQ( 1, str.Count<false ALIB_COMMA Case::Sensitive >( A_CHAR("a")  ) )   UT_EQ( 1, str.CountChar<false>( 'a'  ) )
    UT_EQ( 1, str.Count( A_CHAR("b")  ) )   UT_EQ( 1, str.CountChar( 'b'  ) )   UT_EQ( 1, str.Count<false ALIB_COMMA Case::Sensitive >( A_CHAR("b")  ) )   UT_EQ( 1, str.CountChar<false>( 'b'  ) )
    UT_EQ( 1, str.Count( A_CHAR("e")  ) )   UT_EQ( 1, str.CountChar( 'e'  ) )   UT_EQ( 1, str.Count<false ALIB_COMMA Case::Sensitive >( A_CHAR("e")  ) )   UT_EQ( 1, str.CountChar<false>( 'e'  ) )
    UT_EQ( 1, str.Count( A_CHAR("f")  ) )   UT_EQ( 1, str.CountChar( 'f'  ) )   UT_EQ( 1, str.Count<false ALIB_COMMA Case::Sensitive >( A_CHAR("f")  ) )   UT_EQ( 1, str.CountChar<false>( 'f'  ) )
    UT_EQ( 0, str.Count( A_CHAR("x")  ) )   UT_EQ( 0, str.CountChar( 'x'  ) )   UT_EQ( 0, str.Count<false ALIB_COMMA Case::Sensitive >( A_CHAR("x")  ) )   UT_EQ( 0, str.CountChar<false>( 'x'  ) )
    UT_EQ( 0, str.Count( A_CHAR("")   ) )                                       UT_EQ( 0, str.Count<false ALIB_COMMA Case::Sensitive >( A_CHAR("")   ) )
    UT_EQ( 1, str.Count( A_CHAR("ab") ) )                                       UT_EQ( 1, str.Count<false ALIB_COMMA Case::Sensitive >( A_CHAR("ab") ) )
    UT_EQ( 1, str.Count( A_CHAR("bc") ) )                                       UT_EQ( 1, str.Count<false ALIB_COMMA Case::Sensitive >( A_CHAR("bc") ) )
    UT_EQ( 1, str.Count( A_CHAR("ef") ) )                                       UT_EQ( 1, str.Count<false ALIB_COMMA Case::Sensitive >( A_CHAR("ef") ) )

    UT_EQ( 1, str.Count( A_CHAR("ab"), A_CHAR("X") ) )                          UT_EQ( 1, str.Count<false ALIB_COMMA Case::Sensitive >( A_CHAR("ab"), A_CHAR("X") ) )
    UT_EQ( 0, str.Count( A_CHAR("ab"), A_CHAR("c") ) )                          UT_EQ( 0, str.Count<false ALIB_COMMA Case::Sensitive >( A_CHAR("ab"), A_CHAR("c") ) )
    UT_EQ( 0, str.Count( A_CHAR("ab"), A_CHAR("")  ) )                          UT_EQ( 0, str.Count<false ALIB_COMMA Case::Sensitive >( A_CHAR("ab"), A_CHAR("")  ) )

    str= A_CHAR("abcabcabcabc");
    UT_EQ( 1, str.Count( A_CHAR("abcabcabcabc")       ) )                                         UT_EQ( 1, str.Count<false ALIB_COMMA Case::Sensitive >( A_CHAR("abcabcabcabc")    ) )
    UT_EQ( 0, str.Count( A_CHAR("abcabcabcabcX")      ) )                                         UT_EQ( 0, str.Count<false ALIB_COMMA Case::Sensitive >( A_CHAR("abcabcabcabcX")   ) )
    UT_EQ( 1, str.Count(  A_CHAR("bcabcabcabc")       ) )                                         UT_EQ( 1, str.Count<false ALIB_COMMA Case::Sensitive >(  A_CHAR("bcabcabcabc")    ) )
    UT_EQ( 1, str.Count( A_CHAR("abcabcabcab")        ) )                                         UT_EQ( 1, str.Count<false ALIB_COMMA Case::Sensitive >( A_CHAR("abcabcabcab")     ) )
    UT_EQ( 1, str.Count(  A_CHAR("bcabcabcab")        ) )                                         UT_EQ( 1, str.Count<false ALIB_COMMA Case::Sensitive >(  A_CHAR("bcabcabcab")     ) )
    UT_EQ( 1, str.Count(   A_CHAR("cabcabc")          ) )                                         UT_EQ( 1, str.Count<false ALIB_COMMA Case::Sensitive >(   A_CHAR("cabcabc")       ) )
    UT_EQ( 4, str.Count(   A_CHAR("abc")              ) )                                         UT_EQ( 4, str.Count<false ALIB_COMMA Case::Sensitive >(   A_CHAR("abc")           ) )
    UT_EQ( 3, str.Count(   A_CHAR("cab")              ) )                                         UT_EQ( 3, str.Count<false ALIB_COMMA Case::Sensitive >(   A_CHAR("cab")           ) )
    UT_EQ( 4, str.Count(   A_CHAR("ab")               ) )                                         UT_EQ( 4, str.Count<false ALIB_COMMA Case::Sensitive >(   A_CHAR("ab")            ) )
    UT_EQ( 4, str.Count(   A_CHAR("a")                ) )  UT_EQ( 4, str.CountChar('a'        ) ) UT_EQ( 4, str.Count<false ALIB_COMMA Case::Sensitive >(   A_CHAR("a")             ) ) UT_EQ( 4, str.CountChar<false>(   'a'          ) )
    UT_EQ( 0, str.Count(   A_CHAR("a")  ,A_CHAR("b")  ) )  UT_EQ( 0, str.CountChar('a', 'b', 0) ) UT_EQ( 0, str.Count<false ALIB_COMMA Case::Sensitive >(   A_CHAR("a") , A_CHAR("b"))) UT_EQ( 0, str.CountChar<false>(   'a' ,'b' , 0 ) )
    UT_EQ( 4, str.Count(   A_CHAR("a")  ,A_CHAR("c")  ) )  UT_EQ( 4, str.CountChar('a', 'c', 0) ) UT_EQ( 4, str.Count<false ALIB_COMMA Case::Sensitive >(   A_CHAR("a") , A_CHAR("c"))) UT_EQ( 4, str.CountChar<false>(   'a' ,'c' , 0 ) )
    UT_EQ( 1, str.Count(   A_CHAR("ab") ,A_CHAR("ca") ) )  UT_EQ( 1, str.CountChar('c', 'a', 0) ) UT_EQ( 1, str.Count<false ALIB_COMMA Case::Sensitive >(   A_CHAR("ab"),A_CHAR("ca"))) UT_EQ( 1, str.CountChar<false>(   'c' ,'a' , 0 ) )
}

//--------------------------------------------------------------------------------------------------
//--- Test Trim
//--------------------------------------------------------------------------------------------------
UT_METHOD( Trim )
{
    UT_INIT()

    AString ms;

    // ------------------ TrimAt ------------------
    {
        for (int i= -2; i < 6; ++i )
        {
            ms.Reset(A_CHAR("abc")).TrimAt( i );
            UT_EQ( A_CHAR("abc"), ms )
        }

        ms.Reset(A_CHAR(" abc")).TrimAt( -1 );        UT_EQ( A_CHAR(" abc")        , ms )
        ms.Reset(A_CHAR(" abc")).TrimAt(  0 );        UT_EQ( A_CHAR("abc")         , ms )
        ms.Reset(A_CHAR(" abc")).TrimAt(  1 );        UT_EQ( A_CHAR(" abc")        , ms )

        ms.Reset(A_CHAR("abc ")).TrimAt( -1 );        UT_EQ( A_CHAR("abc ")        , ms )
        ms.Reset(A_CHAR("abc ")).TrimAt(  0 );        UT_EQ( A_CHAR("abc ")        , ms )
        ms.Reset(A_CHAR("abc ")).TrimAt(  2 );        UT_EQ( A_CHAR("abc ")        , ms )
        ms.Reset(A_CHAR("abc ")).TrimAt(  3 );        UT_EQ( A_CHAR("abc")         , ms )
        ms.Reset(A_CHAR("abc ")).TrimAt(  4 );        UT_EQ( A_CHAR("abc ")        , ms )

        ms.Reset(A_CHAR("a bc")).TrimAt( -1 );        UT_EQ( A_CHAR("a bc")        , ms )
        ms.Reset(A_CHAR("a bc")).TrimAt(  0 );        UT_EQ( A_CHAR("a bc")        , ms )
        ms.Reset(A_CHAR("a bc")).TrimAt(  1 );        UT_EQ( A_CHAR("abc")         , ms )
        ms.Reset(A_CHAR("a bc")).TrimAt(  2 );        UT_EQ( A_CHAR("a bc")        , ms )
        ms.Reset(A_CHAR("a bc")).TrimAt(  3 );        UT_EQ( A_CHAR("a bc")        , ms )

        ms.Reset(A_CHAR("a  c")).TrimAt( -1 );        UT_EQ( A_CHAR("a  c")        , ms )
        ms.Reset(A_CHAR("a  c")).TrimAt(  0 );        UT_EQ( A_CHAR("a  c")        , ms )
        ms.Reset(A_CHAR("a  c")).TrimAt(  1 );        UT_EQ( A_CHAR("ac")          , ms )
        ms.Reset(A_CHAR("a  c")).TrimAt(  2 );        UT_EQ( A_CHAR("ac")          , ms )
        ms.Reset(A_CHAR("a  c")).TrimAt(  3 );        UT_EQ( A_CHAR("a  c")        , ms )

        ms.Reset(A_CHAR(" \t abc \t ")).TrimStart();  UT_EQ( A_CHAR("abc \t ")     , ms )
                                     ms.TrimEnd();    UT_EQ( A_CHAR("abc")         , ms )
    }

    // ------------------ Trim ------------------
    {
        ms.Reset(A_CHAR("")          ).Trim();        UT_EQ( A_CHAR("")            , ms )
        ms.Reset(' '                 ).Trim();        UT_EQ( A_CHAR("")            , ms )
        ms.Reset(A_CHAR("  ")        ).Trim();        UT_EQ( A_CHAR("")            , ms )
        ms.Reset(A_CHAR("abc")       ).Trim();        UT_EQ( A_CHAR("abc")         , ms )
        ms.Reset(A_CHAR("abc")       ).Trim();        UT_EQ( A_CHAR("abc")         , ms )
        ms.Reset(A_CHAR(" abc")      ).Trim();        UT_EQ( A_CHAR("abc")         , ms )
        ms.Reset(A_CHAR("  abc")     ).Trim();        UT_EQ( A_CHAR("abc")         , ms )
        ms.Reset(A_CHAR(" \tabc")    ).Trim();        UT_EQ( A_CHAR("abc")         , ms )
        ms.Reset(A_CHAR("\t abc")    ).Trim();        UT_EQ( A_CHAR("abc")         , ms )
        ms.Reset(A_CHAR("abc\r\n")   ).Trim();        UT_EQ( A_CHAR("abc")         , ms )
        ms.Reset(A_CHAR("\tabc\r\n") ).Trim();        UT_EQ( A_CHAR("abc")         , ms )
        ms.Reset(A_CHAR("\tabc\rx\n") ).Trim();       UT_EQ( A_CHAR("abc\rx")      , ms )
    }
}


//--------------------------------------------------------------------------------------------------
//--- Test Compare
//--------------------------------------------------------------------------------------------------
UT_METHOD( Compare )
{
    UT_INIT()

    // null string comparison
    const character*   nullCString= nullptr;

    AString aString;
    UT_TRUE( aString.CompareTo( nullCString) == 0 )
    UT_TRUE( aString.Equals( nullCString) )

    aString.Reset( A_CHAR("") );
    UT_TRUE( aString.CompareTo( nullCString) != 0 )
    UT_TRUE( !aString.Equals( nullCString) )

    aString.SetNull();
    UT_TRUE( aString.CompareTo( nullCString) == 0 )
    UT_TRUE( aString.Equals( nullCString) )


    // compare/region
    AString ms;

    int result;
    int resultX;
    int intMaxValue= std::numeric_limits<int>::max();
    basic_string<character> t; t= A_CHAR("abcde");
    ms._( t );
    result= ms.CompareTo( t      );                          resultX= t.compare( t );                            UT_EQ( resultX,    result )
    result= ms.CompareTo( basic_string<character>(t).append(A_CHAR("x"))       );     resultX= t.compare( basic_string<character>(t).append(A_CHAR("x")) );        UT_EQ( resultX,    result )
    result= ms.CompareTo( t.substr (0, t.size() -1 )  );     resultX= t.compare( t.substr(0, t.size() -1 ) );    UT_EQ( resultX,    result )
    result= ms.CompareTo<true, Case::Sensitive>( A_CHAR("pad") + t,  3            );                    UT_EQ(  0,  result )
    result= ms.CompareTo<true, Case::Sensitive>( A_CHAR("pad") + t,  3, 2         );                    UT_EQ(  1,  result )
    result= ms.CompareTo<true, Case::Sensitive>( A_CHAR("pad") + t,  3, 100, 0, 4 );                    UT_EQ( -1,  result )
    result= ms.CompareTo<true, Case::Sensitive>( A_CHAR("pad") + t,  3, 100, 0, 5 );                    UT_EQ(  0,  result )
    result= ms.CompareTo<true, Case::Sensitive>(         t,  2,2,   2,2           );                    UT_EQ(  0,  result )
    result= ms.CompareTo<true, Case::Sensitive>(         t,  2,2,   2,1           );                    UT_EQ( -1,  result )
    result= ms.CompareTo<true, Case::Sensitive>(         t,  2,2,   2,3           );                    UT_EQ(  1,  result )
    result= ms.CompareTo<true, Case::Sensitive>(         t,  -2,5,   -2, 5        );                    UT_EQ(  0,  result )
    result= ms.CompareTo<true, Case::Sensitive>(         t,   2, intMaxValue,    2, intMaxValue );      UT_EQ(  0,  result )
    result= ms.CompareTo<true, Case::Sensitive>(         t,  -2, intMaxValue,   -2, intMaxValue );      UT_EQ(  0,  result )
    result= ms.CompareTo<true, Case::Sensitive>(         t,  -2, 100,           -2, 99 );               UT_EQ(  0,  result )
    result= ms.CompareTo<true, Case::Sensitive>(         t,  -2, 5,             -2, intMaxValue );      UT_EQ(  1,  result )
    result= ms.CompareTo<true, Case::Sensitive>(         t,  -2, intMaxValue,   -2, 5 );                UT_EQ( -1,  result )


    result= ms.CompareTo<false, Case::Sensitive>( A_CHAR("pad") + t,  3, 2         );    UT_EQ(  1,     result )
    result= ms.CompareTo<false, Case::Sensitive>( A_CHAR("pad") + t,  3, 5         );    UT_EQ(  0,     result )
    result= ms.CompareTo<false, Case::Sensitive>( A_CHAR("pad") + t,  3, 6         );    UT_EQ(  -1,    result )
    result= ms.CompareTo<false, Case::Sensitive>(         t,  2,2,   2,2           );    UT_EQ(  0,     result )
    result= ms.CompareTo<false, Case::Sensitive>(         t,  2,2,   2,1           );    UT_EQ( -1,     result )
    result= ms.CompareTo<false, Case::Sensitive>(         t,  2,2,   2,3           );    UT_EQ(  1,     result )


    // greater/smaller strings
    int scRes;
    basic_string<character> greater=  A_CHAR("x");
    basic_string<character> greater2= A_CHAR("abcdef");
    basic_string<character> smaller=  A_CHAR("aaa");
    basic_string<character> smaller2= A_CHAR("abcd");
    result= ms.CompareTo( greater  );   scRes= t.compare( greater  );    UT_TRUE( (result==0 && scRes==0) || (result<0 && scRes<0) || (result>0 && scRes>0)  )
    result= ms.CompareTo( greater2 );   scRes= t.compare( greater2 );    UT_TRUE( (result==0 && scRes==0) || (result<0 && scRes<0) || (result>0 && scRes>0)  )
    result= ms.CompareTo( smaller  );   scRes= t.compare( smaller  );    UT_TRUE( (result==0 && scRes==0) || (result<0 && scRes<0) || (result>0 && scRes>0)  )
    result= ms.CompareTo( smaller2 );   scRes= t.compare( smaller2 );    UT_TRUE( (result==0 && scRes==0) || (result<0 && scRes<0) || (result>0 && scRes>0)  )
    UT_TRUE ( ms < greater  )
    UT_TRUE ( ms < greater2 )
    UT_TRUE ( ms > smaller  )
    UT_TRUE ( ms > smaller2 )
    UT_FALSE( ms > greater  )
    UT_FALSE( ms > greater2 )
    UT_FALSE( ms < smaller  )
    UT_FALSE( ms < smaller2 )
    UT_TRUE ( ms.Equals(ms) ) // using == raises a C++ 20 warning with Clang only. Don't really get it.
    UT_TRUE ( ms != greater )
    UT_FALSE( ms < ms )
    UT_FALSE( ms > ms )

    // ignore case
    basic_string<character> ABCDE= A_CHAR("ABCDE");
    result= ms.CompareTo                         ( ABCDE               );    UT_TRUE( result >  0 )
    result= ms.CompareTo<true, Case::Sensitive  >( ABCDE               );    UT_TRUE( result >  0 )
    result= ms.CompareTo<true, Case::Ignore     >( ABCDE               );    UT_TRUE( result == 0 )
    result= ms.CompareTo<false                  >( ABCDE               );    UT_TRUE( result >  0 )
    result= ms.CompareTo<false, Case::Sensitive >( ABCDE               );    UT_TRUE( result >  0 )
    result= ms.CompareTo<false, Case::Ignore    >( ABCDE               );    UT_TRUE( result == 0 )
    result= ms.CompareTo<true , Case::Sensitive >( ABCDE,  2,2,   2,2  );    UT_TRUE( result >  0 )
    result= ms.CompareTo<true , Case::Ignore    >( ABCDE,  2,2,   2,2  );    UT_TRUE( result == 0 )
    result= ms.CompareTo<false, Case::Sensitive >( ABCDE,  2,2,   2,2  );    UT_TRUE( result >  0 )
    result= ms.CompareTo<false, Case::Ignore    >( ABCDE,  2,2,   2,2  );    UT_TRUE( result == 0 )

    // we do just a very little testing with StringBuilder and AString parameter version of this function, as
    // the code differences are very small
    const character* cp= t.c_str(); //new StringBuilder(); tSB._( t );
    result= ms.CompareTo<true ,Case::Sensitive>( cp                          );  UT_EQ( 0, result )
    result= ms.CompareTo<true ,Case::Sensitive>( cp,  -5, 100, -10, 100      );  UT_EQ( 0, result )
    result= ms.CompareTo<true ,Case::Sensitive>( cp,   2,   3,   2,   3      );  UT_EQ( 0, result )
    result= ms.CompareTo<false                >( cp,   2,   3,   2,   3      );  UT_EQ( 0, result )

    AString tAS( t );
    result= ms.CompareTo                        ( tAS      );                    UT_EQ( 0, result )
    result= ms.CompareTo<true , Case::Sensitive>(tAS,  -5, 100, -10, 100      ); UT_EQ( 0, result )
    result= ms.CompareTo<true , Case::Sensitive>( cp,   2,   3,   2,   3      ); UT_EQ( 0, result )
    result= ms.CompareTo<false, Case::Sensitive>( cp,   2,   3,   2,   3      ); UT_EQ( 0, result )

    // different ranges
    basic_string<character> r1= A_CHAR("*ABCDEF*");
    basic_string<character> r2= A_CHAR("##*ABCDEF*##");
    ms.Reset( r1 );
    result= ms.CompareTo<true , Case::Sensitive>( r2,  2,8          );       UT_EQ( 0,   result )
    result= ms.CompareTo<false, Case::Sensitive>( r2,  2,7          );       UT_EQ( 1,   result )
    result= ms.CompareTo<false, Case::Sensitive>( r2,  2,8          );       UT_EQ( 0,   result )
    result= ms.CompareTo<false, Case::Sensitive>( r2,  2,9          );       UT_EQ(-1,   result )
    result= ms.CompareTo<true , Case::Sensitive>( r2,  3,6,   1,6   );       UT_EQ( 0,   result )
    result= ms.CompareTo<false, Case::Sensitive>( r2,  3,6,   1,6   );       UT_EQ( 0,   result )

    // startsWith/Endswith/ContainsAt
    basic_string<character> sub1=  A_CHAR("ABC");
    basic_string<character> sub2=  A_CHAR("abc");
    basic_string<character> s1=    A_CHAR("ABCDEF");
    basic_string<character> s2=    A_CHAR("123ABC");
    basic_string<character> s3=    A_CHAR("##12ABC21##");
    basic_string<character> s4=    A_CHAR("AB");
    ms.Reset( s1 );    UT_TRUE  ( ms.StartsWith                              ( sub1    ) )
                       UT_FALSE ( ms.StartsWith                              ( sub2    ) )
    ms.Reset( s2 );    UT_TRUE  ( ms.EndsWith                                ( sub1    ) )
                       UT_FALSE ( ms.EndsWith                                ( sub2    ) )
    ms.Reset( s3 );    UT_TRUE  ( ms.ContainsAt                              ( sub1, 4 ) )
                       UT_FALSE ( ms.ContainsAt                              ( sub2, 4 ) )
                       UT_TRUE  ( ms.ContainsAt<true ALIB_COMMA Case::Ignore>( sub2, 4 ) )
    ms.Reset( s4 );    UT_FALSE ( ms.ContainsAt                              ( sub1, 4 ) )
                       UT_FALSE ( ms.ContainsAt                              ( sub2, 4 ) )
    ms.Reset( s4 );    UT_FALSE ( ms.ContainsAt                              ( sub1, 0 ) )
                       UT_FALSE ( ms.ContainsAt<true ALIB_COMMA Case::Ignore>( sub2, 0 ) )

    ms.Reset( A_CHAR("Hello ALib classes") );

    UT_TRUE( ms.StartsWith<true ALIB_COMMA Case::Ignore>( A_CHAR("Hello ALib classes")   ) == true  )
    UT_TRUE( ms.EndsWith  <true ALIB_COMMA Case::Ignore>( A_CHAR("Hello ALib classes")   ) == true  )
    UT_TRUE( ms.StartsWith<true ALIB_COMMA Case::Ignore>( A_CHAR("Hello ALib classesx")  ) == false )
    UT_TRUE( ms.EndsWith  <true ALIB_COMMA Case::Ignore>( A_CHAR("xHello ALib classes")  ) == false )
    UT_TRUE( ms.StartsWith<true ALIB_COMMA Case::Ignore>( A_CHAR("heLLO")                ) == true  )
    UT_TRUE( ms.StartsWith<true ALIB_COMMA Case::Ignore>( A_CHAR("HeLLO")                ) == true  )
    UT_TRUE( ms.EndsWith  <true ALIB_COMMA Case::Ignore>( A_CHAR("heLLO")                ) == false )
    UT_TRUE( ms.StartsWith<true ALIB_COMMA Case::Ignore>( A_CHAR("CLASSES")              ) == false )
    UT_TRUE( ms.EndsWith  <true ALIB_COMMA Case::Ignore>( A_CHAR("CLASSES")              ) == true  )
    UT_TRUE( ms.EndsWith  <true ALIB_COMMA Case::Ignore>( A_CHAR("clASSes")              ) == true  )

    // contains with empty/null strings
    ms.Reset(A_CHAR("AB"));       UT_FALSE ( ms.ContainsAt( NullString(), -1 )  )
                                  UT_FALSE ( ms.ContainsAt( NullString(),  0 )  )
                                  UT_FALSE ( ms.ContainsAt( NullString(),  1 )  )
                                  UT_FALSE ( ms.ContainsAt( NullString(),  2 )  )
                                  UT_FALSE ( ms.ContainsAt( NullString(),  3 )  )

                                  UT_FALSE ( ms.ContainsAt( A_CHAR(""),  -1 )  )
                                  UT_TRUE  ( ms.ContainsAt( A_CHAR(""),   0 )  )
                                  UT_TRUE  ( ms.ContainsAt( A_CHAR(""),   1 )  )
                                  UT_TRUE  ( ms.ContainsAt( A_CHAR(""),   2 )  )
                                  UT_FALSE ( ms.ContainsAt( A_CHAR(""),   3 )  )
                                  UT_FALSE ( ms.ContainsAt( A_CHAR(""), 100 )  )

    ms._();                       UT_FALSE ( ms.ContainsAt( NullString(), -1 )  )
                                  UT_FALSE ( ms.ContainsAt( NullString(),  0 )  )
                                  UT_FALSE ( ms.ContainsAt( NullString(),  1 )  )

                                  UT_FALSE ( ms.ContainsAt( A_CHAR(""),  -1 )  )
                                  UT_TRUE  ( ms.ContainsAt( A_CHAR(""),   0 )  )
                                  UT_FALSE ( ms.ContainsAt( A_CHAR(""),   1 )  )

    // equals String
    ms.Reset( A_CHAR("") );       UT_TRUE  ( ms.Equals( basic_string<character>(A_CHAR(""))   )   )
                                  UT_FALSE ( ms.Equals( basic_string<character>(A_CHAR("a"))  )   )

    ms.Reset( A_CHAR("AB") );     UT_FALSE ( ms.Equals( basic_string<character>(A_CHAR("")   ))   )
                                  UT_FALSE ( ms.Equals( basic_string<character>(A_CHAR("ab") ))   )
                                  UT_FALSE ( ms.Equals( basic_string<character>(A_CHAR("A")  ))   )
                                  UT_FALSE ( ms.Equals( basic_string<character>(A_CHAR("B")  ))   )
                                  UT_TRUE  ( ms.Equals( basic_string<character>(A_CHAR("AB") ))   )
                                  UT_TRUE  ( ms.Equals( basic_string<character>(A_CHAR("AB") ))   )

    // equals AString
    ms.Reset( A_CHAR("") );       UT_TRUE  ( ms.Equals( String(A_CHAR("")  ))   )
                                  UT_FALSE ( ms.Equals( String(A_CHAR("a") ))   )

    ms.Reset( A_CHAR("AB") );     UT_FALSE ( ms.Equals( String(A_CHAR("" ) ))   )
                                  UT_FALSE ( ms.Equals( String(A_CHAR("ab")))   )
                                  UT_FALSE ( ms.Equals( String(A_CHAR("A") ))   )
                                  UT_FALSE ( ms.Equals( String(A_CHAR("B") ))   )
                                  UT_TRUE  ( ms.Equals( String(A_CHAR("AB")))   )
                                  UT_TRUE  ( ms.Equals( String(A_CHAR("AB")))   )
}

//--------------------------------------------------------------------------------------------------
//--- Test ConvertCase
//--------------------------------------------------------------------------------------------------
UT_METHOD( ConvertCase )
{
    UT_INIT()

    AString ms;

    {
        // test all characters in the 8 bit range
        int testCharRangeStart= 1;
        int testCharRangeEnd=   255;

        basic_string<character> sb;
        for ( int i= testCharRangeStart; i <= testCharRangeEnd ; ++i )
            sb.append( 1, static_cast<character>( i ) );

        basic_string<character> tUpper= sb;    std::transform( tUpper.begin(), tUpper.end(), tUpper.begin(), [](character c)
                                                               {
                                                                   #if !ALIB_CHARACTERS_WIDE
                                                                       return  static_cast<character>( ::toupper(c) );
                                                                   #else
                                                                       return  static_cast<character>( ::towupper(static_cast<wint_t>(c)) );
                                                                   #endif

                                                               } );
        basic_string<character> tLower= sb;    std::transform( tLower.begin(), tLower.end(), tLower.begin(), [](character c)
                                                               {
                                                                   #if !ALIB_CHARACTERS_WIDE
                                                                       return  static_cast<character>( ::tolower(c) );
                                                                   #else
                                                                       return  static_cast<character>( ::towlower(static_cast<wint_t>(c)) );
                                                                   #endif
                                                               } );

        ms.Reset( sb ).ToLower(); UT_EQ( tLower.c_str(), ms )
        ms.Reset( sb ).ToUpper(); UT_EQ( tUpper.c_str(), ms )
    }

    // test sub-string conversion
    {
        const character* t= A_CHAR("AAAbbb");
        ms.Reset( t ).ToLower(  0,  2 );        UT_EQ( A_CHAR("aaAbbb"), ms )
        ms.Reset( t ).ToLower( -2,  3 );        UT_EQ( A_CHAR("aAAbbb"), ms )
        ms.Reset( t ).ToLower( -2     );        UT_EQ( A_CHAR("aaabbb"), ms )
        ms.Reset( t ).ToUpper(  3,  2 );        UT_EQ( A_CHAR("AAABBb"), ms )
        ms.Reset( t ).ToUpper( -2,  6 );        UT_EQ( A_CHAR("AAABbb"), ms )
        ms.Reset( t ).ToUpper( -2     );        UT_EQ( A_CHAR("AAABBB"), ms )
    }
}

//--------------------------------------------------------------------------------------------------
//--- Test Revert
//--------------------------------------------------------------------------------------------------
UT_METHOD( Revert )
{
    UT_INIT()

    NAString na;                        na.Reverse();           UT_TRUE( na.Equals( NullNString()          ) )
    na         << A_NCHAR(""         ); na.Reverse();           UT_TRUE( na.Equals( A_NCHAR(""           )) )
                                        na.Reverse<false>(0,0); UT_TRUE( na.Equals( A_NCHAR(""           )) )
    na.Reset() << A_NCHAR("1"        ); na.Reverse();           UT_TRUE( na.Equals( A_NCHAR("1"          )) )
    na.Reset() << A_NCHAR("12"       ); na.Reverse();           UT_TRUE( na.Equals( A_NCHAR("21"         )) )
    na.Reset() << A_NCHAR("123"      ); na.Reverse();           UT_TRUE( na.Equals( A_NCHAR("321"        )) )
    na.Reset() << A_NCHAR("1234"     ); na.Reverse();           UT_TRUE( na.Equals( A_NCHAR("4321"       )) )
    na.Reset() << A_NCHAR("12345"    ); na.Reverse();           UT_TRUE( na.Equals( A_NCHAR("54321"      )) )

    WAString wa;                        wa.Reverse();           UT_TRUE( wa.Equals( NullWString()          ) )
    wa         << A_WCHAR(""         ); wa.Reverse();           UT_TRUE( wa.Equals( A_WCHAR(""           )) )
                                        wa.Reverse<false>(0,0); UT_TRUE( wa.Equals( A_WCHAR(""           )) )
    wa.Reset() << A_WCHAR("1"        ); wa.Reverse();           UT_TRUE( wa.Equals( A_WCHAR("1"          )) )
    wa.Reset() << A_WCHAR("12"       ); wa.Reverse();           UT_TRUE( wa.Equals( A_WCHAR("21"         )) )
    wa.Reset() << A_WCHAR("123"      ); wa.Reverse();           UT_TRUE( wa.Equals( A_WCHAR("321"        )) )
    wa.Reset() << A_WCHAR("1234"     ); wa.Reverse();           UT_TRUE( wa.Equals( A_WCHAR("4321"       )) )
    wa.Reset() << A_WCHAR("12345"    ); wa.Reverse();           UT_TRUE( wa.Equals( A_WCHAR("54321"      )) )

    XAString xa;                        xa.Reverse();           UT_TRUE( xa.Equals( NullXString()          ) )
    xa         << A_XCHAR(""         ); xa.Reverse();           UT_TRUE( xa.Equals( A_XCHAR(""           )) )
                                        xa.Reverse<false>(0,0); UT_TRUE( xa.Equals( A_XCHAR(""           )) )
    xa.Reset() << A_XCHAR("1"        ); xa.Reverse();           UT_TRUE( xa.Equals( A_XCHAR("1"          )) )
    xa.Reset() << A_XCHAR("12"       ); xa.Reverse();           UT_TRUE( xa.Equals( A_XCHAR("21"         )) )
    xa.Reset() << A_XCHAR("123"      ); xa.Reverse();           UT_TRUE( xa.Equals( A_XCHAR("321"        )) )
    xa.Reset() << A_XCHAR("1234"     ); xa.Reverse();           UT_TRUE( xa.Equals( A_XCHAR("4321"       )) )
    xa.Reset() << A_XCHAR("12345"    ); xa.Reverse();           UT_TRUE( xa.Equals( A_XCHAR("54321"      )) )

}



//--------------------------------------------------------------------------------------------------
//--- Test XAString
//--------------------------------------------------------------------------------------------------
UT_METHOD( Test_XAString )
{
    UT_INIT()

    // this mainly is for testing CharArray<xchar> methods

    XAString  xstr( "BCDEF" );
    UT_TRUE ( xstr                  == A_XCHAR( "BCDEF"  ) )
    UT_TRUE ( A_XCHAR( "BCDEF")     == xstr                )
    UT_FALSE( xstr                  <  A_XCHAR( "BCDEF"  ) )
    UT_FALSE( A_XCHAR( "BCDEF")     >  xstr                )

    UT_TRUE ( xstr                  != A_XCHAR( "ABCDEF" ) )
    UT_FALSE( xstr                  == A_XCHAR( "ABCDEF" ) )
    UT_FALSE( xstr                  <  A_XCHAR( "ABCDEF" ) )
    UT_TRUE ( xstr                  >  A_XCHAR( "ABCDEF" ) )

    UT_TRUE ( xstr                  != A_XCHAR( "CBCDEF" ) )
    UT_FALSE( xstr                  == A_XCHAR( "CBCDEF" ) )
    UT_FALSE( xstr                  >  A_XCHAR( "CBCDEF" ) )
    UT_TRUE ( xstr                  <  A_XCHAR( "CBCDEF" ) )

    UT_FALSE( xstr                  == A_XCHAR( "CBCDEX" ) )
    UT_TRUE ( xstr                  != A_XCHAR( "CBCDEX" ) )

    UT_TRUE ( A_XCHAR( "ABCDEF" )   != xstr                )
    UT_FALSE( A_XCHAR( "ABCDEF" )   == xstr                )
    UT_TRUE ( A_XCHAR( "ABCDEF" )   <  xstr                )
    UT_FALSE( A_XCHAR( "ABCDEF" )   >  xstr                )

    UT_TRUE ( A_XCHAR( "CBCDEF" )   != xstr                )
    UT_FALSE( A_XCHAR( "CBCDEF" )   == xstr                )
    UT_TRUE ( A_XCHAR( "CBCDEF" )   >  xstr                )
    UT_FALSE( A_XCHAR( "CBCDEF" )   <  xstr                )

    UT_FALSE( A_XCHAR( "CBCDEX" )   == xstr                )
    UT_TRUE(  A_XCHAR( "CBCDEX" )   != xstr                )

    UT_TRUE(  xstr.CompareTo<true ALIB_COMMA Case::Ignore>(A_XCHAR( "bcdef" )) == 0 )
    UT_TRUE(  xstr.CompareTo<true ALIB_COMMA Case::Ignore>(A_XCHAR( "acdef" )) >  0 )
    UT_TRUE(  xstr.CompareTo<true ALIB_COMMA Case::Ignore>(A_XCHAR( "bcdaa" )) >  0 )
    UT_TRUE(  xstr.CompareTo<true ALIB_COMMA Case::Ignore>(A_XCHAR( "bcd"   )) >  0 )
    UT_TRUE(  xstr.CompareTo<true ALIB_COMMA Case::Ignore>(A_XCHAR( "ccdef" )) <  0 )


    xstr.InsertAt(A_XCHAR( "123"), 2 );
    UT_TRUE(  xstr == A_XCHAR( "BC123DEF" ) )

    xstr.ReplaceRegion( '@', 2, 3 );
    UT_TRUE(  xstr == A_XCHAR( "BC@@@DEF" ) )

    xstr.Reset( A_XCHAR( "  \n\r\n\tABC  \n\r\n\t" ) );
    xstr.Trim();
    UT_TRUE(  xstr == A_XCHAR( "ABC" ) )

    XCString xcstr=A_XCHAR( "123");
    UT_TRUE(  xcstr.IndexOfAny<Inclusion::Include>( A_XCHAR(""   ) )  == -1)
    UT_TRUE(  xcstr.IndexOfAny<Inclusion::Include>( A_XCHAR("X"  ) )  == -1)
    UT_TRUE(  xcstr.IndexOfAny<Inclusion::Include>( A_XCHAR("XY" ) )  == -1)
    UT_TRUE(  xcstr.IndexOfAny<Inclusion::Include>( A_XCHAR("XYZ") )  == -1)
    UT_TRUE(  xcstr.IndexOfAny<Inclusion::Include>( A_XCHAR("1XY") )  ==  0)
    UT_TRUE(  xcstr.IndexOfAny<Inclusion::Include>( A_XCHAR("X1Y") )  ==  0)
    UT_TRUE(  xcstr.IndexOfAny<Inclusion::Include>( A_XCHAR("XY1") )  ==  0)
    UT_TRUE(  xcstr.IndexOfAny<Inclusion::Include>( A_XCHAR("2XY") )  ==  1)
    UT_TRUE(  xcstr.IndexOfAny<Inclusion::Include>( A_XCHAR("X2Y") )  ==  1)
    UT_TRUE(  xcstr.IndexOfAny<Inclusion::Include>( A_XCHAR("XY2") )  ==  1)
    UT_TRUE(  xcstr.IndexOfAny<Inclusion::Include>( A_XCHAR("3XY") )  ==  2)
    UT_TRUE(  xcstr.IndexOfAny<Inclusion::Include>( A_XCHAR("X3Y") )  ==  2)
    UT_TRUE(  xcstr.IndexOfAny<Inclusion::Include>( A_XCHAR("XY3") )  ==  2)
}

#include "unittests/aworx_unittests_end.hpp"

} //namespace

#endif // ALIB_UT_STRINGS
