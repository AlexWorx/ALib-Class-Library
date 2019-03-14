// #################################################################################################
//  aworx - Unit Tests
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"
#include "unittests/alib_test_selection.hpp"
#if !defined(ALIB_UT_SELECT) || defined(ALIB_UT_STRINGS)

#include "alib/alox.hpp"

#if !defined (HPP_ALIB_STRINGS)
    #include "alib/strings/util/tokenizer.hpp"
#endif
#if !defined (HPP_ALIB_STRINGS_NUMBERFORMAT)
    #include "alib/strings/numberformat.hpp"
#endif

#include "alib/stringformat/formatterpythonstyle.hpp"
#include "alib/stringformat/formatterjavastyle.hpp"
#include "alib/alox/logtools.hpp"

#undef min
#undef max


#define TESTCLASSNAME       CPP_ALib_Strings_Format
#include "unittests/aworx_unittests.hpp"

#include <iostream>
#include <iomanip>
#include <cmath>


using namespace std;
using namespace aworx;
using namespace aworx::lib::stringformat;

namespace ut_aworx {

UT_CLASS()



//--------------------------------------------------------------------------------------------------
//--- Test Simple
//--------------------------------------------------------------------------------------------------
UT_METHOD( Unsorted )
{
    UT_INIT();

    AString as;
    integer pos;
    integer posOrig;

    // parse on empty
    as._();
    {
ut.EQ( __FILE__, __LINE__,  0,  as.ParseInt()   );
        UT_EQ(   0,  as.ParseInt()   );

        posOrig= pos= 0;    UT_EQ(   0,  as.ParseInt( pos, &pos )  );     UT_EQ( pos, posOrig );
        posOrig= pos= -5;   UT_EQ(   0,  as.ParseInt( pos, &pos )  );     UT_EQ( pos, posOrig );
    }

    // parse on non number
    as.Reset( "Hello");
    {
        UT_EQ(   0,  as.ParseInt()   );

        posOrig= pos= 0;    UT_EQ(   0,  as.ParseInt( pos, &pos )  );     UT_EQ( pos, posOrig );
        posOrig= pos= -5;   UT_EQ(   0,  as.ParseInt( pos, &pos )  );     UT_EQ( pos, posOrig );
        posOrig= pos= 2;    UT_EQ(   0,  as.ParseInt( pos, &pos )  );     UT_EQ( pos, posOrig );
    }

    // check that leading whitespaces do not move pointer
    as.Reset( "   Hello");
    {
        UT_EQ(   0,  as.ParseInt()   );

        posOrig= pos= 0;    UT_EQ(   0,  as.ParseInt( pos, &pos )  );     UT_EQ( pos, posOrig );
        posOrig= pos= -5;   UT_EQ(   0,  as.ParseInt( pos, &pos )  );     UT_EQ( pos, posOrig );
        posOrig= pos= 2;    UT_EQ(   0,  as.ParseInt( pos, &pos )  );     UT_EQ( pos, posOrig );
    }

    // parse integers
    {
        as.Reset( "123456789");      UT_EQ( 123456789L,  as.ParseInt  ( integer(0), &pos ) );        UT_EQ(  0 + as.Length() , pos );
        as.Reset( "  123");          UT_EQ( 123L,        as.ParseInt  ( integer(0), &pos ) );        UT_EQ(  0 + as.Length() , pos );
        as.Reset( "  -23  ");        UT_EQ( -23L,        as.ParseInt  ( integer(0), &pos ) );        UT_EQ( -2 + as.Length() , pos );
        as.Reset(   "123  ");        UT_EQ( 123L,        as.ParseInt  ( integer(0), &pos ) );        UT_EQ( -2 + as.Length() , pos );

        as.Reset( "xxx123456789");   UT_EQ( 123456789L,  as.ParseInt  ( 3, &pos ) );        UT_EQ(  0 + as.Length() , pos );
        as.Reset( "xxx  123");       UT_EQ( 123L,        as.ParseInt  ( 3, &pos ) );        UT_EQ(  0 + as.Length() , pos );
        as.Reset( "xxx  123  ");     UT_EQ( 123L,        as.ParseInt  ( 3, &pos ) );        UT_EQ( -2 + as.Length() , pos );
        as.Reset(   "xxx123  ");     UT_EQ( 123L,        as.ParseInt  ( 3, &pos ) );        UT_EQ( -2 + as.Length() , pos );
    }

    // parse with grouping symbol
    {
        NumberFormat numberFormat;
        numberFormat.SetComputational();
        numberFormat.ThousandsGroupChar=   ',';
        numberFormat.ReadGroupChars=       true;
        as.Reset( "123,456,789");   UT_EQ( 123456789L,  as.ParseInt( 0, &numberFormat, &pos ) ); UT_EQ(  0 + as.Length() , pos );
        as.Reset( "  1,2,3"    );   UT_EQ( 123L,        as.ParseInt( 0, &numberFormat, &pos ) ); UT_EQ(  0 + as.Length() , pos );
        as.Reset( " +1,2,,4"   );   UT_EQ( 124L,        as.ParseInt( 0, &numberFormat, &pos ) ); UT_EQ(  0 + as.Length() , pos );
        as.Reset( "  -2,3  "   );   UT_EQ( -23L,        as.ParseInt( 0, &numberFormat, &pos ) ); UT_EQ( -2 + as.Length() , pos );
        as.Reset(   ",123  "   );   UT_EQ(   0L,        as.ParseInt( 0, &numberFormat, &pos ) ); UT_EQ(  0               , pos );
    }


    // int32
    {
        AString ms;

        uint32_t ui;
        int32_t   i;
        NumberFormat* nf=  &NumberFormat::Computational;
        NumberFormat  nfG; nfG.WriteGroupChars= true;

        ms._(); ui= 0;       ms._( ui    );                UT_EQ(      A_CHAR("0"), ms );
        ms._(); ui= 0;       ms._( Format( ui, 1 ,nf) );   UT_EQ(      A_CHAR("0"), ms );
        ms._(); ui= 0;       ms._( Format( ui, 3 ,nf) );   UT_EQ(    A_CHAR("000"), ms );
        ms._(); ui= 10;      ms._( ui    );                UT_EQ(     A_CHAR("10"), ms );
        ms._(); ui= 100;     ms._( Format( ui, 1 ,nf) );   UT_EQ(    A_CHAR("100"), ms );
        ms._(); ui= 100;     ms._( Format( ui, 3 ,nf) );   UT_EQ(    A_CHAR("100"), ms );
        ms._(); ui= 100;     ms._( Format( ui, 4 ,nf) );   UT_EQ(   A_CHAR("0100"), ms );
        ms._(); ui= 23;      ms._( ui    );                UT_EQ(     A_CHAR("23"), ms );
        ms._(); ui= 99;      ms._( Format( ui, 5 ,nf) );   UT_EQ(  A_CHAR("00099"), ms );
        ms._();  i= 49;      ms._( i     );                UT_EQ(     A_CHAR("49"), ms );
        ms._();  i= -5;      ms._( Format ( i, 5 ,nf) );   UT_EQ(  A_CHAR("-0005"), ms );
        ms._();  i= -5324;   ms._( Format ( i, 2 ,nf) );   UT_EQ(  A_CHAR("-5324"), ms );
        ms._(); ui= std::numeric_limits<uint32_t>::max();    ms._(         ui            );   UT_EQ(    A_CHAR("4294967295"),  ms );
        ms._(); ui= std::numeric_limits<uint32_t>::min();    ms._(         ui            );   UT_EQ(             A_CHAR("0"),  ms );
        ms._();  i= std::numeric_limits<int32_t> ::max();    ms._(         i             );   UT_EQ(    A_CHAR("2147483647"),  ms );
        ms._();  i= std::numeric_limits<int32_t> ::min();    ms._(         i             );   UT_EQ(   A_CHAR("-2147483648"),  ms );

        ms._(); ui= std::numeric_limits<uint32_t>::max();    ms._( Format( ui ,&nfG    ) );   UT_EQ( A_CHAR("4,294,967,295"),  ms );
        ms._(); ui= std::numeric_limits<uint32_t>::min();    ms._( Format( ui ,&nfG    ) );   UT_EQ(             A_CHAR("0"),  ms );
        ms._();  i= std::numeric_limits<int32_t> ::max();    ms._( Format ( i ,&nfG    ) );   UT_EQ( A_CHAR("2,147,483,647"),  ms );
        ms._();  i= std::numeric_limits<int32_t> ::min();    ms._( Format ( i ,&nfG    ) );   UT_EQ(A_CHAR("-2,147,483,648"),  ms );

        ms._(); ui= std::numeric_limits<uint32_t>::max();    ms._( Format( ui , 12 ,nf ) );   UT_EQ(  A_CHAR("004294967295"),  ms );
        ms._(); ui= std::numeric_limits<uint32_t>::min();    ms._( Format( ui , 12 ,nf ) );   UT_EQ(  A_CHAR("000000000000"),  ms );
        ms._();  i= std::numeric_limits<int32_t> ::max();    ms._( Format ( i , 12 ,nf ) );   UT_EQ(  A_CHAR("002147483647"),  ms );
        ms._();  i= std::numeric_limits<int32_t> ::min();    ms._( Format ( i , 12 ,nf ) );   UT_EQ(  A_CHAR("-02147483648"),  ms );
    }

    // int64
    {
        AString ms;

        uint64_t ui;
        int64_t   i;
        NumberFormat* nf= &NumberFormat::Computational;
        NumberFormat  nfG; nfG.WriteGroupChars= true;

        ui= 0;       ms.Reset( ui );                   UT_EQ(      A_CHAR("0"), ms );
        ui= 0;       ms.Reset( Format( ui, 1  ,nf ));  UT_EQ(      A_CHAR("0"), ms );
        ui= 0;       ms.Reset( Format( ui, 3  ,nf ));  UT_EQ(    A_CHAR("000"), ms );
        ui= 10;      ms.Reset( ui );                   UT_EQ(     A_CHAR("10"), ms );
        ui= 100;     ms.Reset( Format( ui, 1  ,nf ));  UT_EQ(    A_CHAR("100"), ms );
        ui= 100;     ms.Reset( Format( ui, 3  ,nf ));  UT_EQ(    A_CHAR("100"), ms );
        ui= 100;     ms.Reset( Format( ui, 4  ,nf ));  UT_EQ(   A_CHAR("0100"), ms );
        ui= 23;      ms.Reset( ui );                   UT_EQ(     A_CHAR("23"), ms );
        ui= 99;      ms.Reset( Format( ui, 5  ,nf ));  UT_EQ(  A_CHAR("00099"), ms );
         i= 49;      ms.Reset(                 i   );  UT_EQ(     A_CHAR("49"), ms );
         i= -5;      ms.Reset( Format ( i, 5  ,nf ));  UT_EQ(  A_CHAR("-0005"), ms );
         i= -5324;   ms.Reset( Format ( i, 2  ,nf ));  UT_EQ(  A_CHAR("-5324"), ms );
        ui= std::numeric_limits<uint32_t>::max();    ms.Reset(                ui     );  UT_EQ(    A_CHAR("4294967295"),  ms );
        ui= std::numeric_limits<uint32_t>::min();    ms.Reset(                ui     );  UT_EQ(             A_CHAR("0"),  ms );
         i= std::numeric_limits<int32_t> ::max();    ms.Reset(                i      );  UT_EQ(    A_CHAR("2147483647"),  ms );
         i= std::numeric_limits<int32_t> ::min();    ms.Reset(                i      );  UT_EQ(   A_CHAR("-2147483648"),  ms );

        ui= std::numeric_limits<uint32_t>::max();    ms.Reset( Format(   ui ,&nfG   ));  UT_EQ( A_CHAR("4,294,967,295"),  ms );
        ui= std::numeric_limits<uint32_t>::min();    ms.Reset( Format(   ui ,&nfG   ));  UT_EQ(             A_CHAR("0"),  ms );
         i= std::numeric_limits<int32_t> ::max();    ms.Reset( Format(   i  ,&nfG   ));  UT_EQ( A_CHAR("2,147,483,647"),  ms );
         i= std::numeric_limits<int32_t> ::min();    ms.Reset( Format(   i  ,&nfG   ));  UT_EQ(A_CHAR("-2,147,483,648"),  ms );

        ui= std::numeric_limits<uint32_t>::max();    ms.Reset( Format(ui, 12,nf));  UT_EQ(  A_CHAR("004294967295"),  ms );
        ui= std::numeric_limits<uint32_t>::min();    ms.Reset( Format(ui, 12,nf));  UT_EQ(  A_CHAR("000000000000"),  ms );
         i= std::numeric_limits<int32_t> ::max();    ms.Reset( Format(i , 12,nf));  UT_EQ(  A_CHAR("002147483647"),  ms );
         i= std::numeric_limits<int32_t> ::min();    ms.Reset( Format(i , 12,nf));  UT_EQ(  A_CHAR("-02147483648"),  ms );

        ui= std::numeric_limits<uint64_t>::max();    ms.Reset( Format(  ui,&nfG     ));  UT_EQ( A_CHAR("18,446,744,073,709,551,615"),  ms );
        ui= std::numeric_limits<uint64_t>::min();    ms.Reset( Format(  ui,&nfG     ));  UT_EQ(                          A_CHAR("0"),  ms );
         i= std::numeric_limits<int64_t> ::max();    ms.Reset( Format(  i ,&nfG     ));  UT_EQ(  A_CHAR("9,223,372,036,854,775,807"),  ms );
        ms.SetBuffer(20);
         i= std::numeric_limits<int64_t> ::min();    ms.Reset( Format(  i ,&nfG     ));  UT_EQ( A_CHAR("-9,223,372,036,854,775,808"),  ms );
         i= std::numeric_limits<int64_t> ::min() + 75008; ms.Reset(Format(i,&nfG    ));  UT_EQ( A_CHAR("-9,223,372,036,854,700,800"),  ms );
    }

    // plus sign
    {
        AString ms;
        int32_t   i;
        NumberFormat nf; nf.WriteGroupChars= true;
        i= 49;                            ms.Reset( i );                  UT_EQ(     A_CHAR("49"), ms );
        i= 49;      nf.PlusSign=' '; ms.Reset( Format ( i, -1, &nf  ) );  UT_EQ(    A_CHAR(" 49"), ms );
        i= 49;      nf.PlusSign='+'; ms.Reset( Format ( i, -1, &nf  ) );  UT_EQ(    A_CHAR("+49"), ms );
        i= 49;      nf.PlusSign=' '; ms.Reset( Format ( i,  5, &nf  ) );  UT_EQ(  A_CHAR("  049"), ms );
        i= 49;      nf.PlusSign='+'; ms.Reset( Format ( i,  5, &nf  ) );  UT_EQ(  A_CHAR("+ 049"), ms );
    }

    // Group character
    {
        AString ms;
        int64_t   i;
        int64_t   ui;
        NumberFormat nf; nf.WriteGroupChars= true;
        int w;

        w=  3; ms._(); ui=              100;      ms._( Format( ui , w, &nf  ) );   UT_EQ(                A_CHAR("100"),  ms ); UT_EQ( w, ms.Length() );
        w=  4; ms._(); ui=              100;      ms._( Format( ui , w, &nf  ) );   UT_EQ(               A_CHAR(" 100"),  ms ); UT_EQ( w, ms.Length() );
        w=  5; ms._(); ui=              100;      ms._( Format( ui , w, &nf  ) );   UT_EQ(              A_CHAR("0,100"),  ms ); UT_EQ( w, ms.Length() );
        w=  7; ms._(); ui=           100000;      ms._( Format( ui , w, &nf  ) );   UT_EQ(            A_CHAR("100,000"),  ms ); UT_EQ( w, ms.Length() );
        w=  8; ms._(); ui=           100000;      ms._( Format( ui , w, &nf  ) );   UT_EQ(           A_CHAR(" 100,000"),  ms ); UT_EQ( w, ms.Length() );
        w=  9; ms._(); ui=           100000;      ms._( Format( ui , w, &nf  ) );   UT_EQ(          A_CHAR("0,100,000"),  ms ); UT_EQ( w, ms.Length() );
        w= 11; ms._(); ui=        100000000;      ms._( Format( ui , w, &nf  ) );   UT_EQ(        A_CHAR("100,000,000"),  ms ); UT_EQ( w, ms.Length() );
        w= 12; ms._(); ui=        100000000;      ms._( Format( ui , w, &nf  ) );   UT_EQ(       A_CHAR(" 100,000,000"),  ms ); UT_EQ( w, ms.Length() );
        w= 13; ms._(); ui=        100000000;      ms._( Format( ui , w, &nf  ) );   UT_EQ(      A_CHAR("0,100,000,000"),  ms ); UT_EQ( w, ms.Length() );
        w= 15; ms._(); ui=     100000000000;      ms._( Format( ui , w, &nf  ) );   UT_EQ(    A_CHAR("100,000,000,000"),  ms ); UT_EQ( w, ms.Length() );
        w= 16; ms._(); ui=     100000000000;      ms._( Format( ui , w, &nf  ) );   UT_EQ(   A_CHAR(" 100,000,000,000"),  ms ); UT_EQ( w, ms.Length() );
        w= 17; ms._(); ui=     100000000000;      ms._( Format( ui , w, &nf  ) );   UT_EQ(  A_CHAR("0,100,000,000,000"),  ms ); UT_EQ( w, ms.Length() );

        w=  3; ms._(); i=     -         100;      ms._( Format(  i , w, &nf  ) );   UT_EQ(               A_CHAR("-100"),  ms ); UT_EQ( w+1, ms.Length() );
        w=  4; ms._(); i=     -         100;      ms._( Format(  i , w, &nf  ) );   UT_EQ(               A_CHAR("-100"),  ms ); UT_EQ( w  , ms.Length() );
        w=  5; ms._(); i=     -         100;      ms._( Format(  i , w, &nf  ) );   UT_EQ(              A_CHAR("- 100"),  ms ); UT_EQ( w  , ms.Length() );
        w=  6; ms._(); i=     -         100;      ms._( Format(  i , w, &nf  ) );   UT_EQ(             A_CHAR("-0,100"),  ms ); UT_EQ( w  , ms.Length() );
        w=  7; ms._(); i=     -      100000;      ms._( Format(  i , w, &nf  ) );   UT_EQ(           A_CHAR("-100,000"),  ms ); UT_EQ( w+1, ms.Length() );
        w=  8; ms._(); i=     -      100000;      ms._( Format(  i , w, &nf  ) );   UT_EQ(           A_CHAR("-100,000"),  ms ); UT_EQ( w  , ms.Length() );
        w=  9; ms._(); i=     -      100000;      ms._( Format(  i , w, &nf  ) );   UT_EQ(          A_CHAR("- 100,000"),  ms ); UT_EQ( w  , ms.Length() );
        w= 10; ms._(); i=     -      100000;      ms._( Format(  i , w, &nf  ) );   UT_EQ(         A_CHAR("-0,100,000"),  ms ); UT_EQ( w  , ms.Length() );
        w= 11; ms._(); i=     -   100000000;      ms._( Format(  i , w, &nf  ) );   UT_EQ(       A_CHAR("-100,000,000"),  ms ); UT_EQ( w+1, ms.Length() );
        w= 12; ms._(); i=     -   100000000;      ms._( Format(  i , w, &nf  ) );   UT_EQ(       A_CHAR("-100,000,000"),  ms ); UT_EQ( w  , ms.Length() );
        w= 13; ms._(); i=     -   100000000;      ms._( Format(  i , w, &nf  ) );   UT_EQ(      A_CHAR("- 100,000,000"),  ms ); UT_EQ( w  , ms.Length() );
        w= 14; ms._(); i=     -   100000000;      ms._( Format(  i , w, &nf  ) );   UT_EQ(     A_CHAR("-0,100,000,000"),  ms ); UT_EQ( w  , ms.Length() );
        w= 15; ms._(); i=     -100000000000;      ms._( Format(  i , w, &nf  ) );   UT_EQ(   A_CHAR("-100,000,000,000"),  ms ); UT_EQ( w+1, ms.Length() );
        w= 16; ms._(); i=     -100000000000;      ms._( Format(  i , w, &nf  ) );   UT_EQ(   A_CHAR("-100,000,000,000"),  ms ); UT_EQ( w  , ms.Length() );
        w= 17; ms._(); i=     -100000000000;      ms._( Format(  i , w, &nf  ) );   UT_EQ(  A_CHAR("- 100,000,000,000"),  ms ); UT_EQ( w  , ms.Length() );
        w= 18; ms._(); i=     -100000000000;      ms._( Format(  i , w, &nf  ) );   UT_EQ( A_CHAR("-0,100,000,000,000"),  ms ); UT_EQ( w  , ms.Length() );
    }
}

//--------------------------------------------------------------------------------------------------
//--- Test ConvertIntegers
//--------------------------------------------------------------------------------------------------
NumberFormat itNF;
AString itAS;

void intTestDetection( AWorxUnitTesting& ut, const String& src, int64_t exp, bool parsable, int qtyConsume )
{
    integer newIdx;
    itAS.Reset("abc  ")._(src)._("@@@");
    integer expNewIdx= 5 + ( qtyConsume >= 0 ? qtyConsume : src.Length() );
    UT_EQ( exp, itAS.ParseInt( 3, &newIdx ) );
    if( parsable )
    {
        UT_EQ( expNewIdx, newIdx );
    }
    else
    {
        UT_EQ( 3       , newIdx );
    }

    int64_t result;
    Substring itSB= itAS.Substring(3);
    if( parsable )
    {
        UT_TRUE( itSB.ConsumeInt( result ) );
        UT_EQ( exp, result );
        UT_EQ( 3 + (qtyConsume < 0 ?  0 :  src.Length() - qtyConsume), itSB.Length() );
    }
    else
    {
        UT_FALSE( itSB.ConsumeInt( result ) );
        UT_EQ( itAS.Length() -3, itSB.Length())
    }


}

void intTestRoundtrip( AWorxUnitTesting& ut, int64_t i )
{
    itAS._();
    int64_t   iBack;
    uint64_t uiBack;
    Substring itSB;
    uint64_t ui= static_cast<uint64_t>(i);

    itAS.Reset("  ")._( Format( i, &itNF ) );  if( i>=0) {uiBack= itAS.ParseDec  ( 2,     &itNF  );   UT_EQ( i, static_cast<int64_t>(uiBack )); }
                                                           iBack= itAS.ParseInt  (        &itNF  );   UT_EQ( i, iBack );
                                      itSB= Substring(itAS);     UT_TRUE(itSB.ConsumeInt( iBack, &itNF ));   UT_EQ( i, iBack ); UT_TRUE(itSB.IsEmpty())


    if( i >= 0 )
    {
        itAS.Reset("0b")._( Format::Bin  (ui, &itNF ) );  uiBack= itAS.ParseBin  ( 2,     &itNF  );   UT_EQ( i, static_cast<int64_t>(uiBack ));
                                                           iBack= itAS.ParseInt  (        &itNF  );   UT_EQ( i, iBack );
                                      itSB= itAS.Substring(2);  UT_TRUE(itSB.ConsumeBin(uiBack, &itNF ));   UT_EQ( i, static_cast<int64_t>( uiBack ) ); UT_TRUE(itSB.IsEmpty())
                                      itSB= itAS;               UT_TRUE(itSB.ConsumeInt( iBack, &itNF ));   UT_EQ( i,                        iBack   ); UT_TRUE(itSB.IsEmpty())

        itAS.Reset("0x")._( Format::Hex  (ui, &itNF ) );  uiBack= itAS.ParseHex  ( 2,     &itNF  );   UT_EQ( i, static_cast<int64_t>(uiBack ));
                                                           iBack= itAS.ParseInt  (        &itNF  );   UT_EQ( i, iBack );
                                      itSB= itAS.Substring(2);  UT_TRUE(itSB.ConsumeHex(uiBack, &itNF ));   UT_EQ( i, static_cast<int64_t>( uiBack ) ); UT_TRUE(itSB.IsEmpty())
                                      itSB= itAS;               UT_TRUE(itSB.ConsumeInt( iBack, &itNF ));   UT_EQ( i,                        iBack   ); UT_TRUE(itSB.IsEmpty())

        itAS.Reset("0o")._( Format::Oct  (ui, &itNF ) );  uiBack= itAS.ParseOct  ( 2,     &itNF  );   UT_EQ( i, static_cast<int64_t>(uiBack ));
                                                           iBack= itAS.ParseInt  (        &itNF  );   UT_EQ( i, iBack );
                                      itSB= itAS.Substring(2);  UT_TRUE(itSB.ConsumeOct(uiBack, &itNF ));   UT_EQ( i, static_cast<int64_t>( uiBack ) ); UT_TRUE(itSB.IsEmpty())
                                      itSB= itAS;               UT_TRUE(itSB.ConsumeInt( iBack, &itNF ));   UT_EQ( i,                        iBack   ); UT_TRUE(itSB.IsEmpty())
    }
}

void intTestRoundtripUS( AWorxUnitTesting& ut, uint64_t ui )
{
    itAS._();
    int64_t   iBack;
    uint64_t uiBack;
    Substring itSB;

    itAS.Reset("0b")._( Format::Bin  ( ui, &itNF ) );  uiBack= itAS.ParseBin ( 2,     &itNF  );  UT_EQ( ui, uiBack );
                                                        iBack= itAS.ParseInt (        &itNF  );  UT_EQ( ui, static_cast<uint64_t>(iBack ));
                                  itSB= itAS.Substring(2);  UT_TRUE(itSB.ConsumeBin( uiBack,&itNF ));  UT_EQ( ui, uiBack ); UT_TRUE(itSB.IsEmpty())

    itAS.Reset("0x")._( Format::Hex  ( ui, &itNF ) );  uiBack= itAS.ParseHex ( 2,     &itNF  );  UT_EQ( ui, uiBack );
                                                       iBack= itAS.ParseInt  (        &itNF  );  UT_EQ( ui, static_cast<uint64_t>(iBack ));
                                  itSB= itAS.Substring(2);  UT_TRUE(itSB.ConsumeHex( uiBack,&itNF ));  UT_EQ( ui, uiBack ); UT_TRUE(itSB.IsEmpty())

    itAS.Reset("0o")._( Format::Oct  ( ui, &itNF ) );  uiBack= itAS.ParseOct ( 2,     &itNF );   UT_EQ( ui, uiBack );
                                                       iBack= itAS.ParseInt  (        &itNF );   UT_EQ( ui, static_cast<uint64_t>(iBack ));
                                  itSB= itAS.Substring(2);  UT_TRUE(itSB.ConsumeOct( uiBack,&itNF ));  UT_EQ( ui, uiBack ); UT_TRUE(itSB.IsEmpty())
}

UT_METHOD( ConvertIntegers )
{
    UT_INIT();

    // int detection and string borders
    {
        {
            int result;
            integer newIdx;
            Substring substring(A_CHAR("0x"));
            UT_TRUE( substring.ConsumeInt( result ));
            UT_EQ( 0, result );
            UT_EQ( A_CHAR("x"), substring);

            AString astring(A_CHAR("0x"));
            UT_EQ( 0L, astring.ParseInt( &newIdx ));
            UT_EQ( 1, newIdx );
        }
        {
            int result;
            integer newIdx;
            Substring substring(A_CHAR("0xy"));
            UT_TRUE( substring.ConsumeInt( result ));
            UT_EQ( 0, result );
            UT_EQ( A_CHAR("xy"), substring);

            AString astring(A_CHAR("0xy"));
            UT_EQ( 0L, astring.ParseInt( &newIdx ));
            UT_EQ( 1, newIdx );
        }
        {
            int result;
            integer newIdx;

            Substring substring= String(A_CHAR("0x1234")).Substring<false>( 0, 2);
            UT_TRUE( substring.ConsumeInt( result ));
            UT_EQ( 0, result );
            UT_EQ( A_CHAR("x"), substring );

            AString astring(A_CHAR("0x1234")); astring.ShortenTo(2);
            UT_EQ( 0L,  astring.ParseInt( &newIdx ));
            UT_EQ( 1,  newIdx );
        }

        // the same with leading spaces
        {
            int result;
            integer newIdx;
            Substring substring(A_CHAR("  0x"));
            UT_TRUE( substring.ConsumeInt( result ));
            UT_EQ(  0, result );
            UT_EQ( A_CHAR("x"), substring);

            AString astring(A_CHAR("  0x"));
            UT_EQ( 0L, astring.ParseInt( &newIdx ));
            UT_EQ( 3, newIdx );
        }
        {
            int result;
            integer newIdx;
            Substring substring(A_CHAR("  0xy"));
            UT_TRUE( substring.ConsumeInt( result ));
            UT_EQ( 0, result );
            UT_EQ( A_CHAR("xy"), substring);

            AString astring(A_CHAR("  0xy"));
            UT_EQ( 0L, astring.ParseInt( &newIdx ));
            UT_EQ( 3, newIdx );
        }
        {
            int result;
            integer newIdx;

            Substring substring= String(A_CHAR("  0x1234")).Substring<false>( 0, 4);
            UT_TRUE( substring.ConsumeInt( result ));
            UT_EQ( 0, result );
            UT_EQ( A_CHAR("x"), substring );

            AString astring(A_CHAR("  0x1234")); astring.ShortenTo(4);
            UT_EQ( 0L,  astring.ParseInt( &newIdx ));
            UT_EQ( 3,  newIdx );
        }
    }

    // detection of the literal type
    intTestDetection( ut, A_CHAR(" - 0xF")                  , -15          , true  , -1);
    intTestDetection( ut, A_CHAR(" - 0b1")                  ,  -1          , true  , -1);
    intTestDetection( ut, A_CHAR(" 0x F12A")                ,   0          , true  ,  2);
    intTestDetection( ut, A_CHAR(" 0zF12A")                 ,   0          , true  ,  2);
    intTestDetection( ut, A_CHAR(" - 0xF")                  , -0xF         , true  , -1);
    intTestDetection( ut, A_CHAR(" - 0b1")                  ,  -1          , true  , -1);
    intTestDetection( ut, A_CHAR(" - 0b111")                ,  -7          , true  , -1);
    intTestDetection( ut, A_CHAR(" 0b111F")                 ,   7          , true  ,  6);
    intTestDetection( ut, A_CHAR("0o77")                    ,  63          , true  , -1);
    intTestDetection( ut, A_CHAR(" \t 0o777")               ,  511         , true  , -1);
    intTestDetection( ut, A_CHAR(" \t0o777")                ,  511         , true  , -1);
    intTestDetection( ut, A_CHAR(" \t-0o777")               , -511         , true  , -1);
    intTestDetection( ut, A_CHAR(" -\t0o777")               , -511         , true  , -1);
    intTestDetection( ut, A_CHAR(" -FF")                    ,  0           , false ,  0);

    // range from -4096  to + 4096
    for( int i= -0xFFF - 10; i < 0xFFF + 10 ; i++ )
        intTestRoundtrip( ut, i );

    // limits
    #if defined(__GNUC__)
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Woverflow"
    #endif
    #if defined(__clang__)
        #pragma clang diagnostic push
        #pragma clang diagnostic ignored "-Winteger-overflow"
        #
    #endif
    intTestRoundtrip  ( ut,  std::numeric_limits<int64_t> ::max()    );
    intTestRoundtrip  ( ut,  std::numeric_limits<int64_t> ::max() -1 );
    intTestRoundtrip  ( ut,  std::numeric_limits<int64_t> ::max() +1 );
    intTestRoundtrip  ( ut,  std::numeric_limits<int64_t> ::min()    );
    intTestRoundtrip  ( ut,  std::numeric_limits<int64_t> ::min() +1 );
    intTestRoundtrip  ( ut,  std::numeric_limits<int64_t> ::min() -1 );
    intTestRoundtripUS( ut,  std::numeric_limits<uint64_t>::max()    );
    intTestRoundtripUS( ut,  std::numeric_limits<uint64_t>::max() -1 );
    intTestRoundtripUS( ut,  std::numeric_limits<uint64_t>::max() +1 );
    intTestRoundtripUS( ut,  std::numeric_limits<uint64_t>::min()    );
    intTestRoundtripUS( ut,  std::numeric_limits<uint64_t>::min() +1 );
    intTestRoundtripUS( ut,  std::numeric_limits<uint64_t>::min() -1 );

    #if defined(__GNUC__)
        #pragma GCC diagnostic pop
    #endif
    #if defined(__clang__)
        #pragma clang diagnostic pop
        #
    #endif
    // random
    for( int i= 0; i < 1000 ; i++ )
        intTestRoundtrip( ut,   static_cast<int_fast64_t>(std::rand())
                              * static_cast<int_fast64_t>(std::rand())
                              * static_cast<int_fast64_t>(std::rand()) );

}

//--------------------------------------------------------------------------------------------------
//--- Test ConvertFloats
//--------------------------------------------------------------------------------------------------
void floatTest( AWorxUnitTesting& ut, double d, character decimalPoint, int8_t minDigitsBeforeDot, int8_t digitsAfterDot, const char* expectedString, bool forceScientifc= false )
{
    String64 ms;
    NumberFormat nf;
    nf.ForceScientific=   forceScientifc;
    nf.IntegralPartMinimumWidth=  minDigitsBeforeDot;
    nf.FractionalPartWidth=   digitsAfterDot;

    if ( decimalPoint == '\0' )
    {
        // reset number format to reflect current locale
        NumberFormat::Global.SetFromLocale();
        nf.DecimalPointChar=   NumberFormat::Global.DecimalPointChar;
    }
    else
        nf.DecimalPointChar=  decimalPoint;

    nf.ThousandsGroupChar= '\0';

    ms._( Format(d, &nf) );
    if ( expectedString != nullptr )
    {
        #if ALIB_CHARACTERS_ARE_NARROW
        UT_EQ(  expectedString, String(ms) );
        #else
        UT_EQ(  String128(expectedString), String(ms) );
        #endif
    }

    double precision= digitsAfterDot < 0 ?  pow ( 10, (d != 0.0 ? static_cast<int>( floor((log10( d ) )) ) : 0 )   - 14 )
                                         :  pow ( 10, digitsAfterDot ) / 2.0;

    // check with system parsing (only if system specific decimal point format was given)
    #if ALIB_CHARACTERS_ARE_NARROW
    if ( decimalPoint == '\0' )
    {
        double dSystem= ::atof( ms );
        UT_NEAR( d, dSystem, precision );
    }
    #endif

    // check with aworx parsing
    double dALib= ms.ParseFloat(&nf);
    UT_NEAR( d, dALib, precision );
}


UT_METHOD( ConvertFloats )
{
    UT_INIT();

    AString as;
    integer pos;
    integer posOrig;

    // parse on empty
    as._();
    {
                            UT_EQ( 0.0,        as.ParseFloat  (           )  );
        posOrig= pos= 0;    UT_EQ( 0.0,        as.ParseFloat  ( pos, &pos )  );     UT_EQ( posOrig, pos );
        posOrig= pos= -5;   UT_EQ( 0.0,        as.ParseFloat  ( pos, &pos )  );     UT_EQ( posOrig, pos );
    }

    // parse on non number
    as.Reset( A_CHAR("Hello"));
    {
                            UT_EQ( 0.0,        as.ParseFloat  (           )  );
        posOrig= pos= 0;    UT_EQ( 0.0,        as.ParseFloat  ( pos, &pos )  );     UT_EQ( posOrig, pos );
        posOrig= pos= -5;   UT_EQ( 0.0,        as.ParseFloat  ( pos, &pos )  );     UT_EQ( posOrig, pos );
        posOrig= pos= 2;    UT_EQ( 0.0,        as.ParseFloat  ( pos, &pos )  );     UT_EQ( posOrig, pos );
    }

    // check that leading whitespaces do not move pointer
    as.Reset( A_CHAR("   Hello"));
    {
                            UT_EQ( 0.0,        as.ParseFloat  ()             );
        posOrig= pos= 0;    UT_EQ( 0.0,        as.ParseFloat  ( pos, &pos )  );     UT_EQ( posOrig, pos );
        posOrig= pos= -5;   UT_EQ( 0.0,        as.ParseFloat  ( pos, &pos )  );     UT_EQ( posOrig, pos );
        posOrig= pos= 2;    UT_EQ( 0.0,        as.ParseFloat  ( pos, &pos )  );     UT_EQ( posOrig, pos );
    }

    // parse NaN and INF
    {
        as.Reset( A_CHAR("nan"));    UT_TRUE( isnan( as.ParseFloat( &pos ) ) );             UT_EQ( 3, pos );
        as.Reset(A_CHAR("-nan"));    UT_TRUE( isnan( as.ParseFloat( &pos ) ) );             UT_EQ( 4, pos );
        as.Reset( A_CHAR("nAN"));    UT_TRUE( isnan( as.ParseFloat( &pos ) ) );             UT_EQ( 3, pos );
        as.Reset(A_CHAR("-nAN"));    UT_TRUE( isnan( as.ParseFloat( &pos ) ) );             UT_EQ( 4, pos );

        as.Reset( A_CHAR("inf"));    UT_TRUE( isinf( as.ParseFloat( &pos ) ) );             UT_EQ( 3, pos );
        as.Reset(A_CHAR("-inf"));    UT_TRUE( isinf( as.ParseFloat( &pos ) ) );             UT_EQ( 4, pos );

        #if defined(__clang__)
            #pragma clang diagnostic push
            #pragma clang diagnostic ignored "-Wfloat-equal"
        #endif

            as.Reset( A_CHAR("INf"));    UT_TRUE( as.ParseFloat( &pos ) ==  std::numeric_limits<double>::infinity() );  UT_EQ( 3, pos );
            as.Reset(A_CHAR("-INf"));    UT_TRUE( as.ParseFloat( &pos ) == -std::numeric_limits<double>::infinity() );  UT_EQ( 4, pos );

        #if defined(__clang__)
            #pragma clang diagnostic pop
        #endif
    }

    // write NaN and INF
    {
        as.Reset(  std::numeric_limits<double>::quiet_NaN() );    UT_EQ( NumberFormat::Global.NANLiteral,  as );
        as.Reset(  std::numeric_limits<double>::infinity()  );    UT_EQ( NumberFormat::Global.INFLiteral,  as );
        as.Reset( -std::numeric_limits<double>::infinity()  );    UT_EQ( String16(A_CHAR("-"))._(NumberFormat::Global.INFLiteral), as );
    }

    // write and parse doubles
    {
        NumberFormat nf;
        nf.DecimalPointChar= '.';  as.Reset( A_CHAR("12345.789"));      UT_EQ( 12345.789,  as.ParseFloat( 0, &nf, &pos ) );   UT_EQ(  0 + as.Length() , pos );
        nf.DecimalPointChar= '@';  as.Reset( A_CHAR("12345@789"));      UT_EQ( 12345.789,  as.ParseFloat( 0, &nf, &pos ) );   UT_EQ(  0 + as.Length() , pos );
        nf.DecimalPointChar= '.';  as.Reset( A_CHAR("12345@789"));      UT_EQ( 12345.0  ,  as.ParseFloat( 0, &nf, &pos ) );   UT_EQ(  5               , pos );

        nf.DecimalPointChar= '.';
        as.Reset( "");               UT_EQ(   0.,             as.ParseFloat( 0, &nf, &pos ) );   UT_EQ(  0               , pos );
        as.Reset(  '-');             UT_EQ(   0.,             as.ParseFloat( 0, &nf, &pos ) );   UT_EQ(  0               , pos );
        as.Reset( "-X");             UT_EQ(   0.,             as.ParseFloat( 0, &nf, &pos ) );   UT_EQ(  0               , pos );
        as.Reset(  '.');             UT_EQ(   0.,             as.ParseFloat( 0, &nf, &pos ) );   UT_EQ(  1               , pos );
        as.Reset( ".0");             UT_EQ(   0.,             as.ParseFloat( 0, &nf, &pos ) );   UT_EQ(  2               , pos );
        as.Reset( "0.");             UT_EQ(   0.,             as.ParseFloat( 0, &nf, &pos ) );   UT_EQ(  2               , pos );
        as.Reset( "+.");             UT_EQ(   0.,             as.ParseFloat( 0, &nf, &pos ) );   UT_EQ(  2               , pos );
        as.Reset( "-.");             UT_EQ(   0.,             as.ParseFloat( 0, &nf, &pos ) );   UT_EQ(  2               , pos );
        as.Reset( "-.0");            UT_EQ(   0.,             as.ParseFloat( 0, &nf, &pos ) );   UT_EQ(  3               , pos );
        as.Reset( "-.08");           UT_EQ(  -0.08,           as.ParseFloat( 0, &nf, &pos ) );   UT_EQ(  4               , pos );
        as.Reset( "  123");          UT_EQ( 123.,             as.ParseFloat( 0, &nf, &pos ) );   UT_EQ(  0 + as.Length() , pos );
        as.Reset( "+42");            UT_EQ(  42.,             as.ParseFloat( 0, &nf, &pos ) );   UT_EQ(  3               , pos );
        as.Reset( "  +42  ");        UT_EQ(  42.,             as.ParseFloat( 0, &nf, &pos ) );   UT_EQ( -2 + as.Length() , pos );
        as.Reset( "  +42.  ");       UT_EQ(  42.,             as.ParseFloat( 0, &nf, &pos ) );   UT_EQ( -2 + as.Length() , pos );
        as.Reset( "-23");            UT_EQ( -23.,             as.ParseFloat( 0, &nf, &pos ) );   UT_EQ(  3               , pos );
        as.Reset( "  -23  ");        UT_EQ( -23.,             as.ParseFloat( 0, &nf, &pos ) );   UT_EQ( -2 + as.Length() , pos );
        as.Reset(   "123  ");        UT_EQ( 123.,             as.ParseFloat( 0, &nf, &pos ) );   UT_EQ( -2 + as.Length() , pos );

        as.Reset( "xxx123456789");   UT_EQ( 123456789.,       as.ParseFloat( 3, &nf, &pos ) );   UT_EQ(  0 + as.Length() , pos );
        as.Reset( "xxx  1.3");       UT_EQ( 1.3,              as.ParseFloat( 3, &nf, &pos ) );   UT_EQ(  0 + as.Length() , pos );
        as.Reset( "xxx  1.3  ");     UT_EQ( 1.3,              as.ParseFloat( 3, &nf, &pos ) );   UT_EQ( -2 + as.Length() , pos );
        as.Reset(   "xxx1.3  ");     UT_EQ( 1.3,              as.ParseFloat( 3, &nf, &pos ) );   UT_EQ( -2 + as.Length() , pos );

        as.Reset( "1.234E1");        UT_EQ( 12.34,            as.ParseFloat( 0, &nf, &pos  ) );  UT_EQ(  0 + as.Length() , pos );
        as.Reset( "1.234E0") ;       UT_EQ( 1.234,            as.ParseFloat( 0, &nf, &pos  ) );  UT_EQ(  0 + as.Length() , pos );
        as.Reset( "1.234E-1");       UT_NEAR( 0.1234,         as.ParseFloat( 0, &nf, &pos  ) , 0.0000000001);  UT_EQ(  0  + as.Length() , pos );
        as.Reset( "1.234E");         UT_EQ( 1.234,            as.ParseFloat( 0, &nf, &pos  ) );  UT_EQ(       as.Length() , pos );
        as.Reset( "1.234Ex");        UT_EQ( 1.234,            as.ParseFloat( 0, &nf, &pos  ) );  UT_EQ(       as.Length()-2,pos );
    }

    // write and parse doubles, non scientific mode
    {
        // digits after dot: -1
        floatTest( ut,     3.0              ,'.' , -1, -1 ,      "3.0"                  );
        floatTest( ut,     3.1              ,'.' , -1, -1 ,      "3.1"                  );
        floatTest( ut,     3.14             ,'.' , -1, -1 ,      "3.14"                 );
        floatTest( ut,     3.145            ,'.' , -1, -1 ,      "3.145"                );
        floatTest( ut,     3.02             ,'.' , -1, -1 ,      "3.02"                 );
        floatTest( ut,     3.001            ,'.' , -1, -1 ,      "3.001"                );
        floatTest( ut,     3.09             ,'.' , -1, -1 ,      "3.09"                 );
        floatTest( ut,     3.009            ,'.' , -1, -1 ,      "3.009"                );

        floatTest( ut,     0.               ,'.' , -1, -1 ,      "0.0"                  );
        floatTest( ut,     0.1              ,'.' , -1, -1 ,      "0.1"                  );
        floatTest( ut,     0.14             ,'.' , -1, -1 ,      "0.14"                 );
        floatTest( ut,     0.145            ,'.' , -1, -1 ,      "0.145"                );
        floatTest( ut,     0.02             ,'.' , -1, -1 ,      "0.02"                 );
        floatTest( ut,     0.001            ,'.' , -1, -1 ,      "0.001"                );
        floatTest( ut,     0.09             ,'.' , -1, -1 ,      "0.09"                 );
        floatTest( ut,     0.009            ,'.' , -1, -1 ,      "0.009"                );

        // forces scientific notation
        floatTest( ut,     3.               ,'.' , -1, -1 ,      "3.0E00"              , true );
        floatTest( ut,     3.1              ,'.' , -1, -1 ,      "3.1E00"              , true );
        floatTest( ut,     3.14             ,'.' , -1, -1 ,      "3.14E00"             , true );
        floatTest( ut,     3.145            ,'.' , -1, -1 ,      "3.145E00"            , true );
        floatTest( ut,     3.02             ,'.' , -1, -1 ,      "3.02E00"             , true );
        floatTest( ut,     3.001            ,'.' , -1, -1 ,      "3.001E00"            , true );
        floatTest( ut,     3.09             ,'.' , -1, -1 ,      "3.09E00"             , true );
        floatTest( ut,     3.009            ,'.' , -1, -1 ,      "3.009E00"            , true );

        floatTest( ut,     3.145            ,'.' , -1,  1 ,      "3.1E00"              , true  );
        floatTest( ut,     3.145            ,'.' ,  1,  1 ,      "3.1E00"              , true  );
        floatTest( ut,     3.145            ,'.' ,  5,  3 ,  "00003.145E00"            , true  );
        floatTest( ut,     123.12341234     ,'.' , -1,  6 ,      "1.231234E02"         , true  );
        floatTest( ut,       0.00123123123  ,'.' , -1,  6 ,      "1.231231E-03"        , true  );
        floatTest( ut,       0.321          ,'.' ,  0,  6 ,      "3.210000E-01"        , true  );
        floatTest( ut,     3.789            ,'.' , -1,  1 ,      "3.8E00"              , true  );
        floatTest( ut,     123.78978978     ,'.' , -1,  6 ,      "1.237898E02"         , true  );
        floatTest( ut,       0.00789789789  ,'.' , -1,  6 ,      "7.897898E-03"        , true  );



        // many digits after dot
        //                  123456789 123456789 12345
        double RoundDown= 1.123412341234123412341234;
        floatTest( ut,  RoundDown  ,'.' ,  1,  12 ,   "1.123412341234"       );
        floatTest( ut,  RoundDown  ,'.' ,  1,  13 ,   "1.1234123412341"      );
        floatTest( ut,  RoundDown  ,'.' ,  1,  14 ,   "1.12341234123412"     );
        floatTest( ut,  RoundDown  ,'.' ,  1,  15 ,   "1.123412341234123"    );

               RoundDown= 0.123412341234123412341234;
        floatTest( ut,  RoundDown  ,'.' ,  1,  12 ,   "0.123412341234"       );
        floatTest( ut,  RoundDown  ,'.' ,  1,  13 ,   "0.1234123412341"      );
        floatTest( ut,  RoundDown  ,'.' ,  1,  14 ,   "0.12341234123412"     );
        floatTest( ut,  RoundDown  ,'.' ,  1,  15 ,   "0.123412341234123"    );

        double RoundUp=   1.678967896789678967896789;
        floatTest( ut,  RoundUp    ,'.' ,  1,  12 ,   "1.678967896790"       );
        floatTest( ut,  RoundUp    ,'.' ,  1,  13 ,   "1.6789678967897"      );
        floatTest( ut,  RoundUp    ,'.' ,  1,  14 ,   "1.67896789678968"     );
        floatTest( ut,  RoundUp    ,'.' ,  1,  15 ,   "1.678967896789679"    );

               RoundUp=   0.678967896789678967896789;
        floatTest( ut,  RoundUp    ,'.' ,  1,  12 ,   "0.678967896790"       );
        floatTest( ut,  RoundUp    ,'.' ,  1,  13 ,   "0.6789678967897"      );
        floatTest( ut,  RoundUp    ,'.' ,  1,  14 ,   "0.67896789678968"     );
        floatTest( ut,  RoundUp    ,'.' ,  1,  15 ,   "0.678967896789679"    );

        //            123456789 123456789                           123456789 12345
        floatTest( ut,  0.00001234123412341234    ,'.' ,  1,  15 ,   "0.000012341234123"    );
        floatTest( ut,  0.00000123412341234123    ,'.' ,  1,  15 ,   "0.000001234123412"    );
        floatTest( ut,  0.00000012341234123412    ,'.' ,  1,  15 ,   "0.000000123412341"    );
        floatTest( ut,  0.00000001234123412341    ,'.' ,  1,  15 ,   "0.000000012341234"    );
        floatTest( ut,  0.00000000123412341234    ,'.' ,  1,  15 ,   "0.000000001234123"    );
        floatTest( ut,  0.00000000012341234123    ,'.' ,  1,  15 ,   "0.000000000123412"    );
        floatTest( ut,  0.00000000001234123412    ,'.' ,  1,  15 ,   "0.000000000012341"    );
        floatTest( ut,  0.00000000000123412341    ,'.' ,  1,  15 ,   "0.000000000001234"    );
        floatTest( ut,  0.00000000000012341234    ,'.' ,  1,  15 ,   "0.000000000000123"    );
        floatTest( ut,  0.00000000000001234123    ,'.' ,  1,  15 ,   "0.000000000000012"    );
        floatTest( ut,  0.00000000000000123412    ,'.' ,  1,  15 ,   "0.000000000000001"    );
        floatTest( ut,  0.00000000000000012341    ,'.' ,  1,  15 ,   "0.000000000000000"    );
        floatTest( ut,  0.00000000000000001234    ,'.' ,  1,  15 ,   "0.000000000000000"    );

        floatTest( ut,  0.00006789678967896789    ,'.' ,  1,  15 ,   "0.000067896789679"    );
        floatTest( ut,  0.00000678967896789678    ,'.' ,  1,  15 ,   "0.000006789678968"    );
        floatTest( ut,  0.00000067896789678967    ,'.' ,  1,  15 ,   "0.000000678967897"    );
        floatTest( ut,  0.00000006789678967896    ,'.' ,  1,  15 ,   "0.000000067896790"    );
        floatTest( ut,  0.00000000678967896789    ,'.' ,  1,  15 ,   "0.000000006789679"    );
        floatTest( ut,  0.00000000067896789678    ,'.' ,  1,  15 ,   "0.000000000678968"    );
        floatTest( ut,  0.00000000006789678967    ,'.' ,  1,  15 ,   "0.000000000067897"    );
        floatTest( ut,  0.00000000000678967896    ,'.' ,  1,  15 ,   "0.000000000006790"    );
        floatTest( ut,  0.00000000000067896789    ,'.' ,  1,  15 ,   "0.000000000000679"    );
        floatTest( ut,  0.00000000000006789678    ,'.' ,  1,  15 ,   "0.000000000000068"    );
        floatTest( ut,  0.00000000000000678967    ,'.' ,  1,  15 ,   "0.000000000000007"    );
        floatTest( ut,  0.00000000000000067896    ,'.' ,  1,  15 ,   "0.000000000000001"    );
        floatTest( ut,  0.00000000000000006789    ,'.' ,  1,  15 ,   "0.000000000000000"    );
        floatTest( ut,  0.00000000000000000678    ,'.' ,  1,  15 ,   "0.000000000000000"    );

        // digits after dot: 0 or positive
        floatTest( ut,  300.00050000001     ,'.' ,  1,  2 ,   "300.00"               );
        floatTest( ut,  300.00050000001     ,'.' ,  1,  3 ,   "300.001"              );
        floatTest( ut,  300.00050000001     ,'.' ,  1,  4 ,   "300.0005"             );
        floatTest( ut,  300.00050000001     ,'.' ,  1,  5 ,   "300.00050"            );
        floatTest( ut,  300.00050000001     ,'.' ,  1,  6 ,   "300.000500"           );
        floatTest( ut,    5.01              ,'.' ,  1,  3 ,      "5.010"             );


        floatTest( ut,     0.5              ,'.' ,  0,  0 ,       "."                );
        floatTest( ut,     0.5              ,'.' ,  0,  1 ,       ".5"               );
        floatTest( ut,     0.5              ,'.' ,  1,  0 ,      "0."                );
        floatTest( ut,     0.5              ,'.' ,  1,  1 ,      "0.5"               );
        floatTest( ut,     0.5              ,'.' ,  2,  2 ,     "00.50"              );
        floatTest( ut,     0.5              ,'.' ,  3,  3 ,    "000.500"             );
        floatTest( ut,     0.54             ,'.' ,  3,  3 ,    "000.540"             );
        floatTest( ut,     0.543            ,'.' ,  3,  3 ,    "000.543"             );
        floatTest( ut,     0.5432           ,'.' ,  3,  3 ,    "000.543"             );


        floatTest( ut,    23.37             ,'.' ,  2,  1 ,     "23.4"               );
        floatTest( ut,     1.5              ,'.' ,  0,  0 ,      "1."                );
        floatTest( ut,     1.9              ,'.' ,  0,  0 ,      "2."                );
        floatTest( ut,     1.9              ,'.' ,  0,  4 ,      "1.9000"            );
        floatTest( ut,     1.500001         ,'.' ,  0,  0 ,      "2."                );
        floatTest( ut,     1.54             ,'.' ,  3,  3 ,    "001.540"             );
        floatTest( ut,     1.543            ,'.' ,  3,  3 ,    "001.543"             );
        floatTest( ut,     1.5432           ,'.' ,  3,  3 ,    "001.543"             );
        floatTest( ut,    23.37             ,'.' ,  2,  3 ,     "23.370"             );
        floatTest( ut,    12.345            ,'.' ,  0,  3 ,     "12.345"             );
        floatTest( ut,    12.345            ,'.' ,  5,  1 ,  "00012.3"               );
        floatTest( ut,    12.36789          ,'.' ,  5,  1 ,  "00012.4"               );
        floatTest( ut,     1.5              ,'.' ,  0,  0 ,      "1."                );
        floatTest( ut,     1.5              ,'.' ,  1,  0 ,      "1."                );
        floatTest( ut,     1.5              ,'.' ,  0,  1 ,      "1.5"               );
        floatTest( ut,     1.5              ,'.' ,  1,  1 ,      "1.5"               );
        floatTest( ut,     1.5              ,'.' ,  2,  2 ,     "01.50"              );
        floatTest( ut,     1.5              ,'.' ,  3,  3 ,    "001.500"             );
        floatTest( ut,     1.54             ,'.' ,  3,  3 ,    "001.540"             );
        floatTest( ut,     1.543            ,'.' ,  3,  3 ,    "001.543"             );
        floatTest( ut,     1.5432           ,'.' ,  3,  3 ,    "001.543"             );


        floatTest( ut,     0.0              ,'.' ,  0,  0 ,       "."                );
        floatTest( ut,     0.0              ,'.' ,  1,  0 ,      "0."                );
        floatTest( ut,     0.0              ,'.' ,  0,  1 ,       ".0"               );
        floatTest( ut,     0.0              ,'.' ,  1,  1 ,      "0.0"               );
        floatTest( ut,     0.0              ,'.' ,  2,  2 ,     "00.00"              );
        floatTest( ut,    10.0              ,'.' ,  2,  2 ,     "10.00"              );
        floatTest( ut,    23.37             ,'.' ,  2,  1 ,     "23.4"               );
        floatTest( ut,    23.37             ,'.' ,  2,  3 ,     "23.370"             );
        floatTest( ut,    12.345            ,'.' ,  0,  3 ,     "12.345"             );
        floatTest( ut, -   5.1              ,'.' ,  5,  0 , "-00005."                );
        floatTest( ut, -   5.1              ,'.' ,  5,  1 , "-00005.1"               );
        floatTest( ut, -  53.24             ,'.' ,  2,  2 ,    "-53.24"              );
        floatTest( ut, -   0.2              ,'.' ,  1,  3 ,     "-0.200"             );
        floatTest( ut, -   0.6              ,'.' ,  1,  3 ,     "-0.600"             );
        floatTest( ut, -   0.999            ,'.' ,  1,  3 ,     "-0.999"             );

        floatTest( ut,     0.999            ,'.' ,  1,  3 ,      "0.999"             );
        floatTest( ut,     0.99             ,'.' ,  1,  3 ,      "0.990"             );
        floatTest( ut,     0.999            ,'.' ,  1,  3 ,      "0.999"             );
        floatTest( ut,     0.9999           ,'.' ,  1,  3 ,      "1.000"             );
        floatTest( ut,     0.0999           ,'.' ,  2,  3 ,     "00.100"             );
        floatTest( ut,     0.99999          ,'.' ,  1,  3 ,      "1.000"             );
        floatTest( ut,     0.099            ,'.' ,  2,  3 ,     "00.099"             );
        floatTest( ut,     0.0999           ,'.' ,  1,  3 ,      "0.100"             );
        floatTest( ut,     0.09999          ,'.' ,  2,  3 ,     "00.100"             );
        floatTest( ut,     0.999            ,'.' ,  2,  3 ,     "00.999"             );

        floatTest( ut,     0.0099           ,'.' ,  1,  3 ,      "0.010"             );
        floatTest( ut,     0.00999          ,'.' ,  1,  3 ,      "0.010"             );
        floatTest( ut,     0.009            ,'.' ,  1,  3 ,      "0.009"             );
        floatTest( ut,     0.00099          ,'.' ,  1,  3 ,      "0.001"             );
        floatTest( ut,     0.00009          ,'.' ,  1,  3 ,      "0.000"             );

        floatTest( ut,     1.0099           ,'.' ,  1,  3 ,      "1.010"             );
        floatTest( ut,     1.00999          ,'.' ,  1,  3 ,      "1.010"             );
        floatTest( ut,     1.009            ,'.' ,  1,  3 ,      "1.009"             );
        floatTest( ut,     1.00099          ,'.' ,  1,  3 ,      "1.001"             );
        floatTest( ut,     1.00009          ,'.' ,  1,  3 ,      "1.000"             );

        floatTest( ut,     0.00050          ,'.' ,  1,  3 ,      "0.000"             );
        floatTest( ut,     0.00051          ,'.' ,  1,  3 ,      "0.001"             );
        floatTest( ut,     0.00050000001    ,'.' ,  1,  3 ,      "0.001"             );
        floatTest( ut,  300.00050000001     ,'.' ,  1,  3 ,    "300.001"             );

        floatTest( ut,     0.10099          ,'.' ,  1,  4 ,     "0.1010"             );
        floatTest( ut,     0.10099          ,'.' ,  1,  4 ,     "0.1010"             );
        floatTest( ut,     0.00099          ,'.' ,  1,  4 ,     "0.0010"             );
        floatTest( ut,     1.099            ,'.' ,  1,  1 ,     "1.1"                );
        floatTest( ut,     0.00999          ,'.' ,  1,  4 ,     "0.0100"             );

        floatTest( ut,     2.3-1.2          ,'.' ,  -1,  -1 ,   "1.1"                );




        // roundtrip -3.0 to 3.0 step 0.001
        {
            for (int8_t digits= -1 ; digits < 5 ; digits++ )
            {
                double d= -3.0;
                double end= 3.0;
                double inc= 0.001;
                while ( d <= end )
                {
                    floatTest( ut, d, '\0' , -1, digits, nullptr );
                    d+= inc;
                }
            }
        }

        // roundtrip e-200 to e+200
        #if !ALIB_AVOID_ANALYZER_WARNINGS
        {
            for ( int exp= -200 ; exp <= 200 ; exp+=1 )
            {
                double expFactor= pow( 10, exp );

                for ( int t= 0 ; t <= 100 ; t++)
                {
                    // create a random number of type r.rrrrr *10^exp
                    //double d= drand48() * expFactor;
                    double d= ( double( rand() ) / RAND_MAX ) * expFactor;
                    if ( t & 1 )
                        d= -d;

                    floatTest( ut, d, '\0' , -1, -1, nullptr );
                }
            }
        }
        #endif

        // special exponent symbol
        {
            NumberFormat nf;
            nf.ExponentSeparator= A_CHAR("*10^");
            String64 s;
            double v= 7.5E42;
            s._( Format(v, &nf) );
            UT_EQ( A_CHAR("7.5*10^42"), String(s)  );
            double back= s.ParseFloat( &nf );
            UT_NEAR( v, back, 0.0000000001 );
        }

        // single floats
        {
            float f= 3.14f;
            as.Reset(f);
            double d= as.ParseFloat();
            UT_EQ( d, static_cast<double>(f) );
        }

    }
}

//--------------------------------------------------------------------------------------------------
//--- Formatter Tests
//--------------------------------------------------------------------------------------------------
AString     testAS;
AString     tempAS;
Formatter*  testFormatter;

template <typename... BoxedObjects>
void    checkError (AWorxUnitTesting& ut, Enum expectedException, BoxedObjects&&... args )
{
    // create argument objects using implicit constructor invocation
    Boxes boxes( std::forward<BoxedObjects>(args) ... );

    // invoke format
    bool caught= false;
    try
    {
        testAS.Reset();
        testFormatter->FormatArgs( testAS, boxes );
    }
    catch (Exception& e)
    {
        caught= true;
        UT_PRINT( "Exception caught as expected: " );
        LogTools::Exception( ut.lox, e, Verbosity::Info, ut.Domain, A_CHAR("  ") );
        if( e.Type() != expectedException  )
        {
          UT_PRINT( "But wrong type: caught: {}, expected: {}", e.Type(), expectedException );
          UT_TRUE( false );
        }
    }
    if( !caught )
    {
        UT_PRINT( "No Exception caught. Expected: ", expectedException );
        UT_PRINT( "Instead, formatting result is {!Q}", testAS );
        UT_TRUE( caught );
    }
}


template <typename... BoxedObjects>
void    checkFormat(AWorxUnitTesting& ut,  const NString& exp, BoxedObjects&&... args )
{
    // create argument objects using implicit constructor invocation
    Boxes boxes( std::forward<BoxedObjects>(args) ... );

    // clear AString buffer to test for enough capacity
    testAS._();
    testAS.SetBuffer(1);

    // invoke format
    testFormatter->FormatArgs( testAS, boxes );

#if ALIB_CHARACTERS_ARE_NARROW
    UT_EQ( exp, testAS );
#else
    UT_EQ( tempAS.Reset(exp), testAS );
#endif

}

#if defined(_MSC_VER)
#pragma warning( disable : 4146 ) //unary minus operator applied to unsigned type, result still unsigned
#endif


//--------------------------------------------------------------------------------------------------
//--- Test JavaStyle
//--------------------------------------------------------------------------------------------------
UT_METHOD( FormatterJavaStyle )
{
    UT_INIT();
    UT_PRINT(  "ALib Format Tests Java Style: Start" );

    aworx::FormatterJavaStyle          formatterJS;
    formatterJS.Next.reset( new aworx::FormatterPythonStyle() );
    testFormatter= &formatterJS;
    testFormatter->Acquire(ALIB_CALLER_PRUNED);

    formatterJS.AlternativeNumberFormat.DecimalPointChar=     ',';
    formatterJS.AlternativeNumberFormat.ThousandsGroupChar=   '.';

    NumberFormat nfBackup;
    nfBackup.Set( &formatterJS.DefaultNumberFormat );

    //===== Simple initial tests =========
    checkError (ut, Exceptions::IncompatibleTypeCode     , "FLoat as int: %d", 3.1    );

    checkFormat(ut,   "No JSF"                           , ""               , "No JSF"        );
    checkFormat(ut, "%%No JSF"                           , "%%"             , "No JSF"        );
    checkError (ut, Exceptions::UnknownConversionJS      , "% %"            , "Hello JSF"     );
    checkError (ut, Exceptions::UnknownConversionJS      , "%U"             , "Hello JSF"     );
    checkError (ut, Exceptions::ArgumentIndexIs0         , "Test %0$d %d %d", 1,2,3 );
    checkError (ut, Exceptions::ArgumentIndexOutOfBounds , "Test %4$d %d %d", 1,2,3 );
    checkFormat(ut, "Test 3 1 2"                         , "Test %3$d %d %d", 1,2,3 );
    checkFormat(ut, "Test 2 1 23"                        , "Test %2$d %d %d", 1,2,3 );
    checkFormat(ut, "Test 1 1 23"                        , "Test %1$d %d %d", 1,2,3 );
    checkError (ut, Exceptions::ArgumentIndexOutOfBounds , "Test %d %d %d"  , 1,2 );

    checkFormat(ut, "Hello JSF"                          , "%1$s"           , "Hello JSF"     );
    checkFormat(ut, "Hello JSF"                          , "%1$s %2$s"      , "Hello", "JSF"  );
    checkFormat(ut, "Hello HelloJSF"                     , "%1$s %1$s"      , "Hello", "JSF"  );
    checkFormat(ut, "JSF Hello"                          , "%2$s %1$s"      , "Hello", "JSF"  );
    checkError (ut,  Exceptions::MissingPrecisionValueJS , "%.s"            , "x"             );

    //===== replace %% and new line =========
    checkFormat(ut, "repl. percents% X"   , "repl. percents%% %s"              , "X" );
    checkFormat(ut, "repl. percents%%X"   , "repl. percents%%%%%s"             , "X" );
    checkFormat(ut, "repl. X%percents%"   , "repl. %s%%percents%%"             , "X" );


    checkFormat(ut, "x\\nxX"                , "x\\nx"                          , "X" ); // not a format string (no replacement)
    checkFormat(ut, "x\nx%sX"               , "x\nx%s"                         , "X" ); // not a format string (contains \n)
    #if defined( _WIN32)
    checkFormat(ut, "y\r\nyX"               , "y%ny%s"                         , "X" ); // a format string (contains \n)
    #else
    checkFormat(ut, "y\nyX"                 , "y%ny%s"                         , "X" ); // a format string (contains \n)
    #endif
    //===== Parameter addressing =========
    checkFormat(ut, "1 1 1END"          , "%<s %<s %<s"   ,'1', "END" );
    checkFormat(ut, "1 1 1END"          , "%<s %<s %<s"   ,'1', "END" );
    checkFormat(ut, "2 2 4 1 1 2END"    , "%2$s %<s %4$s %s %1$s %s"   ,'1','2','3','4', "END" );
    checkFormat(ut,  "abracadabra"      , "%s%s%1$s"     ,  "abra", "cad" );
    checkFormat(ut,  "abracadabra"      , "%1$s%2$s%s"   ,  "abra", "cad" );

    //================== Concatenated operations (mixed formatters)  ===============================
    checkFormat(ut,  "123"     , "%s"   ,  1,   "%s",2,     "%s",3             );
    checkFormat(ut,  "123456"  , "%s"   ,  1,   "%s%s",2,3, "%s%s",4,5  ,6     );
    checkFormat(ut,  "123456"  , "%s"   ,  1,   "%s",2,  3, "%s%s",4,5  ,6     );
    checkFormat(ut,  "13456"   , "%s"   ,  1,   "%2$s",2,3, "%s%s",4,5  ,6     );

    checkFormat(ut,  "123"     , "{}"   ,  1,   "%s",2,     "{}",3             );
    checkFormat(ut,  "123456"  , "{}"   ,  1,   "%s%s",2,3, "{}{}",4,5  ,6     );
    checkFormat(ut,  "123456"  , "{}"   ,  1,   "%s",2,  3, "{}{}",4,5  ,6     );
    checkFormat(ut,  "13456"   , "{}"   ,  1,   "{1}",2,3, "{}{}",4,5  ,6     );

    checkFormat(ut,  "123"     , "{}"   ,  1,   "{}",2,     "{}",3             );
    checkFormat(ut,  "123456"  , "{}"   ,  1,   "{}{}",2,3, "{}{}",4,5  ,6     );
    checkFormat(ut,  "123456"  , "{}"   ,  1,   "{}",2,  3, "{}{}",4,5  ,6     );
    checkFormat(ut,  "13456"   , "{}"   ,  1,   "%2$s",2,3, "{}{}",4,5  ,6     );

    checkFormat(ut,  "123"     , "{}"   ,  1,   "{}",2,     "%s",3             );
    checkFormat(ut,  "123456"  , "{}"   ,  1,   "{}{}",2,3, "%s%s",4,5  ,6     );
    checkFormat(ut,  "123456"  , "{}"   ,  1,   "{}",2,  3, "%s%s",4,5  ,6     );
    checkFormat(ut,  "13456"   , "{}"   ,  1,   "{1}",2,3 , "%s%s",4,5  ,6     );


    //===== Strings witdh/precision =========
    checkFormat(ut, "aBcDe",         "%s"       , "aBcDe"  );
    checkFormat(ut, "ABCDE",         "%S"       , "aBcDe"  );
    checkFormat(ut, "aBc",           "%.3s"     , "aBcDe"  );
    checkFormat(ut, "ABC",           "%.3S"     , "aBcDe"  );
    checkFormat(ut, "  ABC",         "%5.3S"    , "aBcDe"  );
    checkFormat(ut, "ABC   ",        "%-6.3S"   , "aBcDe"  );
    checkFormat(ut, " ABC  ",        "%^6.3S"   , "aBcDe"  );
    checkFormat(ut, "ABCDE",        "%3S"   , "aBcDe"  );


    //===== Boolean =========
    checkFormat(ut, "true false true false true", "%b %b %b %b %b"  , true, false, 1, 0, "Hello"  );
    checkFormat(ut, "true",         "%.4b"  , true  );
    checkFormat(ut, "fals",         "%.4b"  , false  );
    checkFormat(ut, "tru",          "%.3b"  , true  );
    checkFormat(ut, "fal",          "%.3b"  , false  );
    checkFormat(ut, "  tru",        "%5.3b"  , true  );
    checkFormat(ut, "  fal",        "%5.3b"  , false  );
    checkFormat(ut, " true",        "%5.6b"  , true  );
    checkFormat(ut, "false",        "%5.6b"  , false  );

    //===== Hash value 'h'/'H' =========
    NAString tExpect;
    tExpect._() << lib::strings::TFormat<char>::Hex(reinterpret_cast<uinteger>( &formatterJS ) ) ;
    checkFormat(ut, tExpect.ToLower()         , "%h" , reinterpret_cast<uinteger>( &formatterJS )  );
    checkFormat(ut, tExpect.ToUpper()         , "%H" , reinterpret_cast<uinteger>( &formatterJS )  );

    //===== Date/Time 't'/'T' =========
    CalendarDateTime ctAM;
    ctAM.Year     = 2015;
    ctAM.Day      =    3;
    ctAM.Month    =    1;
    ctAM.Hour     =   05;
    ctAM.Minute   =    6;
    ctAM.Second   =    2;
    DateTime dateTimeAM( ctAM.Get() );

    CalendarDateTime ctPM;
    ctPM.Year     = 2016;
    ctPM.Day      =   30;
    ctPM.Month    =    9;
    ctPM.Hour     =   14;
    ctPM.Minute   =   18;
    ctPM.Second   =   22;
    DateTime dateTimePM( ctPM.Get() );

    checkError (ut, Exceptions::UnknownDateTimeConversionSuffix, "Test %tX"     , dateTimeAM );

    checkFormat(ut,  "05"                         , "%tH"         ,dateTimeAM );
    checkFormat(ut,  "14"                         , "%tH"         ,dateTimePM );
    checkFormat(ut,  "5"                          , "%tk"         ,dateTimeAM );
    checkFormat(ut,  "14"                         , "%tk"         ,dateTimePM );
    checkFormat(ut,  "05 am"                      , "%tI"         ,dateTimeAM );
    checkFormat(ut,  "02 pm"                      , "%tI"         ,dateTimePM );
    checkFormat(ut,  "5 am"                       , "%tl"         ,dateTimeAM );
    checkFormat(ut,  "2 pm"                       , "%tl"         ,dateTimePM );
    checkFormat(ut,  "06"                         , "%tM"         ,dateTimeAM );
    checkFormat(ut,  "18"                         , "%tM"         ,dateTimePM );
    checkFormat(ut,  "02"                         , "%tS"         ,dateTimeAM );
    checkFormat(ut,  "22"                         , "%tS"         ,dateTimePM );

    checkFormat(ut,  "January"                    , "%tB"         ,dateTimeAM );
    checkFormat(ut,  "September"                  , "%tB"         ,dateTimePM );
    checkFormat(ut,  "Jan"                        , "%tb"         ,dateTimeAM );
    checkFormat(ut,  "Sep"                        , "%tb"         ,dateTimePM );
    checkFormat(ut,  "Saturday"                   , "%tA"         ,dateTimeAM );
    checkFormat(ut,  "Friday"                     , "%tA"         ,dateTimePM );
    checkFormat(ut,  "Sat"                        , "%ta"         ,dateTimeAM );
    checkFormat(ut,  "Fri"                        , "%ta"         ,dateTimePM );
    checkFormat(ut,  "2015"                       , "%tY"         ,dateTimeAM );
    checkFormat(ut,  "2016"                       , "%tY"         ,dateTimePM );
    checkFormat(ut,  "15"                         , "%ty"         ,dateTimeAM );
    checkFormat(ut,  "16"                         , "%ty"         ,dateTimePM );
    checkFormat(ut,  "01"                         , "%tm"         ,dateTimeAM );
    checkFormat(ut,  "09"                         , "%tm"         ,dateTimePM );
    checkFormat(ut,  "03"                         , "%td"         ,dateTimeAM );
    checkFormat(ut,  "30"                         , "%td"         ,dateTimePM );
    checkFormat(ut,  "3"                          , "%te"         ,dateTimeAM );
    checkFormat(ut,  "30"                         , "%te"         ,dateTimePM );

    checkFormat(ut,  "05:06"                      , "%tR"         ,dateTimeAM );
    checkFormat(ut,  "14:18"                      , "%tR"         ,dateTimePM );
    checkFormat(ut,  "05:06:02"                   , "%tT"         ,dateTimeAM );
    checkFormat(ut,  "14:18:22"                   , "%tT"         ,dateTimePM );

    checkFormat(ut,  "01/03/15"                   , "%tD"         ,dateTimeAM );
    checkFormat(ut,  "09/30/16"                   , "%tD"         ,dateTimePM );
    checkFormat(ut,  "2015-01-03"                 , "%tF"         ,dateTimeAM );
    checkFormat(ut,  "2016-09-30"                 , "%tF"         ,dateTimePM );


    //======================================= Characters ===========================================

    // alignment
    checkFormat(ut,  "x"                        , "%s"           , 'x'    );
    checkFormat(ut,  "#x  #"                    , "#%-3c#"       , 'x'    );
    checkFormat(ut,  "#  x#"                    , "#%3c#"        , 'x'    );
    checkFormat(ut,  "# x #"                    , "#%^3c#"       , 'x'    );
    checkFormat(ut,  "    x"                    , "%5c"          , 'x'    );

    // errors
    checkError (ut, Exceptions::NoPrecisionWithConversion    , "%5.2c"   , 'x' );
    checkError (ut, Exceptions::NoAlternateFormOfConversion  , "%#c"     , 'x' );

    // wchar
    checkFormat(ut,  NString64()._(L"\u03B1")      , "%c"         , L'\u03B1'    ); //greek alpha
    checkFormat(ut,  NString64()._(L"    \u03B1")  , "%5c"        , L'\u03B1'    ); //greek alpha
    checkFormat(ut,  NString64('a')<<L"\u03B1"<<'b' << L"\u03B2", "a%cb%c"   , L'\u03B1', L'\u03B2'    ); //greek alpha, beta
    checkFormat(ut,  "@"                          , "%c"         , 64           ); // int   ascii @
    checkFormat(ut,  "@"                          , "%c"         , 64L          ); // long  ascii @
    checkFormat(ut,  "@"                          , "%c"         , 64UL         ); // ulong ascii @
    checkFormat(ut,  NString64()._(L"\u03B1")      , "%c"         , 0x03B1       ); //greek alpha
    checkFormat(ut,  NString64()._(L"\u03B1")      , "%c"         , 0x03B1L      ); //greek alpha
    checkFormat(ut,  NString64()._(L"\u03B1")      , "%c"         , 0x03B1UL     ); //greek alpha


    //===== Hex =========
    checkFormat(ut, "1'0000"      , "%,x"       ,65536 );
    checkFormat(ut, "10000"       , "%x"        ,65536 );
    checkFormat(ut, "ffff"        , "%x"        ,65535 );
    checkFormat(ut, "fe"          , "%x"        ,254 );
    checkFormat(ut, "FE"          , "%X"        ,254 );
    checkFormat(ut, "0xfe"        , "%#x"       ,254 );
    checkFormat(ut, "FE"          , "%X"        ,254 );
    checkFormat(ut, "0XFE"        , "%#X"       ,254 );



    //======================================= Integers (Decimal) ===========================================
    checkFormat(ut,  "0"                    , "%d"              ,  0    );
    checkFormat(ut,  "1"                    , "%d"              ,  1    );
    checkFormat(ut,  "5"                    , "%d"              ,  5    );
    checkFormat(ut,  "-1"                   , "%d"              ,  -1   );
    checkFormat(ut,  "-5"                   , "%d"              ,  -5   );
    checkFormat(ut,  "0"                    , "%d"              ,  0U   );
    checkFormat(ut,  "1"                    , "%d"              ,  1U   );
    checkFormat(ut,  "5"                    , "%d"              ,  5U   );
    checkFormat(ut,  "-2147483648"          , "%d"              ,  std::numeric_limits< int32_t >::min()   );
    checkFormat(ut,  "2147483647"           , "%d"              ,  std::numeric_limits< int32_t >::max()   );
    checkFormat(ut,  "-2147483647"          , "%d"              ,  std::numeric_limits< int32_t >::min() +1);
    checkFormat(ut,  "2147483646"           , "%d"              ,  std::numeric_limits< int32_t >::max() -1);
    checkFormat(ut,  "0"                    , "%d"              ,  std::numeric_limits<uint32_t >::min()   );
    checkFormat(ut,  "4294967295"           , "%d"              ,  std::numeric_limits<uint32_t >::max()   );
    checkFormat(ut,  "1"                    , "%d"              ,  std::numeric_limits<uint32_t >::min() +1);
    checkFormat(ut,  "4294967294"           , "%d"              ,  std::numeric_limits<uint32_t >::max() -1);
    checkFormat(ut,  "-9223372036854775808" , "%d"              ,  std::numeric_limits< int64_t >::min()   );
    checkFormat(ut,  "9223372036854775807"  , "%d"              ,  std::numeric_limits< int64_t >::max()   );
    checkFormat(ut,  "-9223372036854775807" , "%d"              ,  std::numeric_limits< int64_t >::min() +1);
    checkFormat(ut,  "9223372036854775806"  , "%d"              ,  std::numeric_limits< int64_t >::max() -1);
    checkFormat(ut,  "0"                    , "%d"              ,  std::numeric_limits<uint64_t >::min()   );
    checkFormat(ut,  "18446744073709551615" , "%d"              ,  std::numeric_limits<uint64_t >::max()   );
    checkFormat(ut,  "1"                    , "%d"              ,  std::numeric_limits<uint64_t >::min() +1);
    checkFormat(ut,  "18446744073709551614" , "%d"              ,  std::numeric_limits<uint64_t >::max() -1);



    // integer sign
    checkFormat(ut,  "0"                    , "%d"             ,  0    );
    checkFormat(ut,  "1"                    , "%d"             ,  1    );
    checkFormat(ut,  "-1"                   , "%d"             , -1    );

    checkFormat(ut,  " 0"                   , "% d"            ,  0    );
    checkFormat(ut,  " 1"                   , "% d"            ,  1    );
    checkFormat(ut,  "-1"                   , "% d"            , -1    );

    checkFormat(ut,  "+0"                   , "%+d"            ,  0    );
    checkFormat(ut,  "+1"                   , "%+d"            ,  1    );
    checkFormat(ut,  "-1"                   , "%+d"            , -1    );

    // integer field width
    checkFormat(ut,  " 0"                   , "%2d"            ,  0    );

    // integer mindigits
    checkFormat(ut,  "000"                 , "%03d"           ,  0    );
    checkFormat(ut,  "001"                 , "%03d"           ,  1    );
    checkFormat(ut,  "-01"                 , "%03d"           , -1    );


    checkFormat(ut,  " 00"                 , "% 03d"          ,  0    );
    checkFormat(ut,  " 01"                 , "% 03d"          ,  1    );
    checkFormat(ut,  "-01"                 , "% 03d"          , -1    );

    checkError (ut, Exceptions::NegativeValuesInBracketsNotSupported  , "No negative: %(d"     , -1 );




    //========================================= Floats =============================================
    // mixed tests
    checkError (ut,  Exceptions::HexadecimalFloatFormatNotSupported, "Hex float: %a"      , 0.0  );
    checkFormat(ut,                     "0.0", "%s"                 , 0.0          ) ;
    checkFormat(ut,      "0.3333333333333333", "%s"                 , 1.0/3.0      ) ;
    checkFormat(ut,       "3.333333333333334", "%s"                 , 10.0/3.0     ) ;
    checkFormat(ut,       "6.666666666666667", "%s"                 , 20.0/3.0     ) ;
    checkFormat(ut,                 "4.23456", "%s"                 , 4.23456      ) ;
    checkFormat(ut,                "-4.2345" , "%s"                 , -4.2345      ) ;

    checkFormat(ut,                     "0.0", "%.5s"               , 0.0f         ) ;
    checkFormat(ut,                   "0.333", "%.5s"               , 1.0f/3.0f    ) ;
    checkFormat(ut,                   "3.333", "%.5s"               , 10.0f/3.0f   ) ;
    checkFormat(ut,                   "6.666", "%.5s"               , 20.0f/3.0f   ) ;
    checkFormat(ut,                   "4.234", "%.5s"               , 4.23456f     ) ;
    checkFormat(ut,                   "-4.23", "%.5s"               , -4.23456789f ) ;

    checkFormat(ut,            "0.000000E+00", "%E"                 , 0.0          ) ;
    checkFormat(ut,            "0.000000E+00", "%#E"                , 0.0          ) ;
    checkFormat(ut,                "1.23E-01", "%.2E"               , 0.12345      ) ;
    checkFormat(ut,                "1.23E-01", "%#.2E"              , 0.12345      ) ;
    checkFormat(ut,                   "1E-01", "%.0E"               , 0.12345      ) ;
    checkFormat(ut,                  "1.E-01", "%#.0E"              , 0.12345      ) ;

    checkFormat(ut,               "123.45"   , "%s"                 , 123.45       ) ;
    checkFormat(ut,               "1"        , "%.1s"               , 123.45       ) ;
    checkFormat(ut,               ""         , "%.0s"               , 123.45       ) ;

    checkFormat(ut,               "123.0"    , "%s"                 , 123.0        ) ;
    checkFormat(ut,               "0.45"     , "%.5s"               , 0.45         ) ;

    checkFormat(ut,  "1.234000"              , "%g"                  ,  1.234 );
    checkFormat(ut,  "1.234000"              , "%G"                  ,  1.234 );
    checkFormat(ut,  "1.234000e+20"          , "%g"                  ,  1.234e20 );
    checkFormat(ut,  "1.234000E+20"          , "%G"                  ,  1.234e20 );

    checkFormat(ut,  "1.234"                 , "%f"                  ,  1.234 );
    checkFormat(ut,  "1.0"                   , "%f"                  ,  1.0 );
    checkFormat(ut,  "1.00"                  , "%.2f"                ,  1.0 );
    checkFormat(ut,  "1.23456789"            , "%f"                  ,  1.23456789 );
    checkFormat(ut,  "1234567890.0"          , "%f"                  ,  1.23456789e9 );
    checkFormat(ut,  "12345678900000.0"      , "%f"                  ,  1.23456789e13 );

    checkFormat(ut, "5"                      , "%s"                  , 5     );
    checkFormat(ut, "12345.6"                , "%s"                  , 12345.6   );
    checkFormat(ut, "12,345.6"               , "%,s"                 , 12345.6   );
    checkFormat(ut, "12,345.6"               , "%,s"                 , 12345.6   );

    checkFormat(ut,  "1.234E+20"             , "%s"                  ,  1.234e20 );
    checkFormat(ut,  "1.234E+20"             , "%S"                  ,  1.234e20 );


    //--- a quick check on local setting (this procedure is maybe not too nice but documented)  ---


    formatterJS.DefaultNumberFormat.DecimalPointChar  = ',';
    formatterJS.DefaultNumberFormat.ThousandsGroupChar= '.';
    checkFormat(ut,            "1234,500"         ,  "%.3g"             ,  1234.5       );
    checkFormat(ut,           "1.234,500"         ,  "%,.3g"             ,  1234.5       );
    formatterJS.DefaultNumberFormat.Set(&nfBackup);


    //----------------------------- floats with width and precision ----------------------------

    // floats with field width
    checkFormat(ut,           "       0.0"       ,  "%10s"             ,  0.0         );
    checkFormat(ut,           "       0.0"       ,  "%10.5s"           ,  0.0         );
    checkFormat(ut,           "  0.000000"       ,  "%10g"             ,  0.0         );
    checkFormat(ut,           "       0.0"       ,  "%10f"             ,  0.0         );
    checkFormat(ut,           "  0.000000"       ,  "%10.6f"           ,  0.0         );
    checkFormat(ut,           "   0.00000"       ,  "%10.5g"           ,  0.0         );
    checkFormat(ut,           "  14.05000"       ,  "%10.5g"           ,  14.05       );
    checkFormat(ut,           " -14.05000"       ,  "%10.5g"           , -14.05       );
    checkFormat(ut,            " 14.05000"       ,  "%9.5g"            ,  14.05       );
    checkFormat(ut,            "-14.05000"       ,  "%9.5g"            , -14.05       );
    checkFormat(ut,             "14.05000"       ,  "%8.5g"            ,  14.05       );
    checkFormat(ut,            " 14.05000"       , "% 8.5g"            ,  14.05       );
    checkFormat(ut,            "-14.05000"       ,  "%8.5g"            , -14.05       );
    checkFormat(ut,             "14.05000"       ,  "%7.5g"            ,  14.05       );
    checkFormat(ut,            "+14.05000"       ,  "%+7.5g"           ,  14.05       );
    checkFormat(ut,            "-14.05000"       ,  "%7.5g"            , -14.05       );


    // Leading '0' in width forces padding after the sign
    //     positive without sign
    checkFormat(ut,  "0012.3456789"          , "%012.7g"          ,  12.3456789   );
    checkFormat(ut,  "012.3456789"           , "%011.7g"          ,  12.3456789   );
    checkFormat(ut,  "12.3456789"            , "%010.7g"          ,  12.3456789   );
    checkFormat(ut,  "12.3456789"            , "%09.7g"           ,  12.3456789   );

    checkFormat(ut,  "000012.3"              , "%08.1g"         ,  12.3456789   );
    checkFormat(ut,  "00012.35"              , "%08.2g"         ,  12.3456789   );
    checkFormat(ut,  "0012.346"              , "%08.3g"         ,  12.3456789   );
    checkFormat(ut,  "012.3457"              , "%08.4g"         ,  12.3456789   );

    checkFormat(ut,  " 012.3456789"          , "% 012.7g"        ,  12.3456789   );
    checkFormat(ut,  " 12.3456789"           , "% 011.7g"        ,  12.3456789   );
    checkFormat(ut,  " 12.3456789"           , "% 010.7g"        ,  12.3456789   );
    checkFormat(ut,  " 12.3456789"           , "% 09.7g"         ,  12.3456789   );
    checkFormat(ut,  " 12.3456789"           , "% 08.7g"         ,  12.3456789   );
    checkFormat(ut,  " 00012.3"              , "% 08.1g"         ,  12.3456789   );
    checkFormat(ut,  " 0012.35"              , "% 08.2g"         ,  12.3456789   );
    checkFormat(ut,  " 012.346"              , "% 08.3g"         ,  12.3456789   );
    checkFormat(ut,  " 12.3457"              , "% 08.4g"         ,  12.3456789   );

    //     negative
    checkFormat(ut,  "-012.3456789"          , "% 012.7g"        ,  -12.3456789   );
    checkFormat(ut,  "-12.3456789"           , "% 011.7g"        ,  -12.3456789   );
    checkFormat(ut,  "-12.3456789"           , "% 010.7g"        ,  -12.3456789   );
    checkFormat(ut,  "-12.3456789"           , "% 09.7g"         ,  -12.3456789   );
    checkFormat(ut,  "-12.3456789"           , "% 08.7g"         ,  -12.3456789   );
    checkFormat(ut,  "-00012.3"              , "% 08.1g"         ,  -12.3456789   );
    checkFormat(ut,  "-0012.35"              , "% 08.2g"         ,  -12.3456789   );
    checkFormat(ut,  "-012.346"              , "% 08.3g"         ,  -12.3456789   );
    checkFormat(ut,  "-12.3457"              , "% 08.4g"         ,  -12.3456789   );


    //     positive with + sign
    checkFormat(ut,  "+012.3456789"          , "%+012.7g"        ,  12.3456789   );
    checkFormat(ut,  "+12.3456789"           , "%+011.7g"        ,  12.3456789   );
    checkFormat(ut,  "+12.3456789"           , "%+010.7g"        ,  12.3456789   );
    checkFormat(ut,  "+12.3456789"           , "%+09.7g"         ,  12.3456789   );
    checkFormat(ut,  "+12.3456789"           , "%+08.7g"         ,  12.3456789   );
    checkFormat(ut,  "+00012.3"              , "%+08.1g"         ,  12.3456789   );
    checkFormat(ut,  "+0012.35"              , "%+08.2g"         ,  12.3456789   );
    checkFormat(ut,  "+012.346"              , "%+08.3g"         ,  12.3456789   );
    checkFormat(ut,  "+12.3457"              , "%+08.4g"         ,  12.3456789   );



    //-------  Nan,Inf, -0.0 -------
    checkFormat(ut,  "NaN"                  , "%s"              ,  std::nan("") );
    checkFormat(ut,  "Infinity"             , "%s"              ,  std::numeric_limits<double>::infinity() );
    checkFormat(ut,  "-Infinity"            , "%s"              , -std::numeric_limits<double>::infinity() );
    checkFormat(ut,  "0.0"                  , "%s"              ,  0.0 );
    checkFormat(ut,  "0.0"                  , "%s"              , -0.0 ); // negative zero
    checkFormat(ut,  "NaN"                  , "%+g"            ,  std::nan("") );
    checkFormat(ut,  "+Infinity"            , "%+g"            ,  std::numeric_limits<double>::infinity() );
    checkFormat(ut,  "-Infinity"            , "%+g"            , -std::numeric_limits<double>::infinity() );
    checkFormat(ut,  "+0.000000"            , "%+g"            ,  0.0 );
    checkFormat(ut,  "+0.000000"            , "%+g"            , -0.0 ); // negative zero
    checkFormat(ut,  "NaN"                  , "%+G"            ,  std::nan("") );
    checkFormat(ut,  "+INFINITY"            , "%+G"            ,  std::numeric_limits<double>::infinity() );
    checkFormat(ut,  "-INFINITY"            , "%+G"            , -std::numeric_limits<double>::infinity() );
    checkFormat(ut,  "+0.000000"            , "%+G"            ,  0.0 );
    checkFormat(ut,  "+0.000000"            , "%+G"            , -0.0 ); // negative zero


    //-------  Nan,Inf, -0.0, field width -------
    checkFormat(ut,  "       NaN"           , "%10g"           ,  std::nan("") );
    checkFormat(ut,  "  Infinity"           , "%10g"           ,  std::numeric_limits<double>::infinity() );
    checkFormat(ut,  " -Infinity"           , "%10g"           , -std::numeric_limits<double>::infinity() );
    checkFormat(ut,  "  0.000000"           , "%10g"           ,  0.0 );
    checkFormat(ut,  "  0.000000"           , "%10g"           , -0.0 ); // negative zero

    checkFormat(ut,  "       NaN"           , "%+10g"         ,  std::nan("") );
    checkFormat(ut,  " +Infinity"           , "%+10g"         ,  std::numeric_limits<double>::infinity() );
    checkFormat(ut,  " -Infinity"           , "%+10g"         , -std::numeric_limits<double>::infinity() );
    checkFormat(ut,  " +0.000000"           , "%+10g"         ,  0.0 );
    checkFormat(ut,  " +0.000000"           , "%+10g"         , -0.0 ); // negative zero

    checkFormat(ut,  "       NaN"           , "%010g"          ,  std::nan("") );
    checkFormat(ut,  "  Infinity"           , "%010g"          ,  std::numeric_limits<double>::infinity() );
    checkFormat(ut,  "- Infinity"           , "%010g"          , -std::numeric_limits<double>::infinity() );
    checkFormat(ut,  "00000000.0"           , "%010.1g"        ,  0.0 );
    checkFormat(ut,  "00000000.0"           , "%010.1g"        , -0.0 ); // negative zero

    checkFormat(ut,  "NaN       "           , "%-10g"          ,  std::nan("") );
    checkFormat(ut,  "Infinity  "           , "%-10g"          ,  std::numeric_limits<double>::infinity() );
    checkFormat(ut,  "-Infinity "           , "%-10g"          , -std::numeric_limits<double>::infinity() );
    checkFormat(ut,  "0.0       "           , "%-10.1g"        ,  0.0 );
    checkFormat(ut,  "0.0       "           , "%-10.1g"        , -0.0 ); // negative zero


    // -------------------------------- scientific -------------------------------------------

    checkFormat(ut,  "1.234568e+01"         , "%-e"            ,  12.3456789   );
    checkFormat(ut,  "1.235e+01"            , "%-.3e"          ,  12.3456789   );
    checkFormat(ut,  " 1.235e+01"           , "%10.3e"         ,  12.3456789   );
    checkFormat(ut,  "01.235e+01"           , "%010.3e"        ,  12.3456789   );
    checkFormat(ut,  "001.23e+01"           , "%010.2e"        ,  12.3456789   );
    checkFormat(ut,  "-01.23e+01"           , "%010.2e"        , -12.3456789   );
    checkFormat(ut,  "-1.235e+01"           , "%010.3e"        , -12.3456789   );
    checkFormat(ut,  "-1.2346e+01"          , "%010.4e"        , -12.3456789   );

    checkFormat(ut,  "1.234568E+01"         , "%-E"            ,  12.3456789   );
    checkFormat(ut,  "1.235E+01"            , "%-.3E"          ,  12.3456789   );
    checkFormat(ut,  " 1.235E+01"           , "%10.3E"        ,  12.3456789   );
    checkFormat(ut,  "01.235E+01"           , "%010.3E"       ,  12.3456789   );
    checkFormat(ut,  "001.23E+01"           , "%010.2E"       ,  12.3456789   );
    checkFormat(ut,  "-01.23E+01"           , "%010.2E"       , -12.3456789   );
    checkFormat(ut,  "-1.235E+01"           , "%010.3E"       , -12.3456789   );
    checkFormat(ut,  "-1.2346E+01"          , "%010.4E"       , -12.3456789   );


    //-------  Nan,Inf, -0.0 -------
    checkFormat(ut,  "NaN"                  , "%E"            ,  nan("") );
    checkFormat(ut,  "INFINITY"             , "%E"            ,  std::numeric_limits<double>::infinity() );
    checkFormat(ut,  "-INFINITY"            , "%E"            , -std::numeric_limits<double>::infinity() );
    checkFormat(ut,  "0.000000E+00"         , "%E"            ,  0.0 );
    checkFormat(ut,  "0.000000E+00"         , "%E"            , -0.0 ); // negative zero
    checkFormat(ut,  "NaN"                  , "%+e"           ,  nan("") );
    checkFormat(ut,  "+Infinity"            , "%+e"           ,  std::numeric_limits<double>::infinity() );
    checkFormat(ut,  "-Infinity"            , "%+e"           , -std::numeric_limits<double>::infinity() );
    checkFormat(ut,  "+0.000000e+00"        , "%+e"           ,  0.0 );
    checkFormat(ut,  "+0.000000e+00"        , "%+e"           , -0.0 ); // negative zero


    // -------------------------------- FixedPoint -------------------------------------------
    checkFormat(ut,  "12.345"               , "%f"            ,  12.345         );
    checkFormat(ut,  "12.3456789123"        , "%f"            ,  12.3456789123  );
    checkFormat(ut,  "-12.345"              , "%f"            , -12.345         );
    checkFormat(ut,  "-12.3456789123"       , "%f"            , -12.3456789123  );
    checkFormat(ut,  "12.345"               , "%f"            ,  12.345         );
    checkFormat(ut,  "12.3456789123"        , "%f"            ,  12.3456789123  );
    checkFormat(ut,  "-12.345"              , "%f"            , -12.345         );
    checkFormat(ut,  "-12.3456789123"       , "%f"            , -12.3456789123  );





    checkFormat(ut,             "0.00000"       , "%,06.5f"             ,  0.0         );
    checkFormat(ut,             "0.00000"       , "%,07.5f"             ,  0.0         );
    checkFormat(ut,            "00.00000"       , "%,08.5f"             ,  0.0         );
    checkFormat(ut,           "000.00000"       , "%,09.5f"             ,  0.0         );
    checkFormat(ut,          " 000.00000"       , "%,010.5f"           ,  0.0         );
    checkFormat(ut,         "0,000.00000"       , "%,011.5f"           ,  0.0         );
    checkFormat(ut,        "00,000.00000"       , "%,012.5f"           ,  0.0         );
    checkFormat(ut,       "000,000.00000"       , "%,013.5f"           ,  0.0         );
    checkFormat(ut,      " 000,000.00000"       , "%,014.5f"           ,  0.0         );
    checkFormat(ut,     "0,000,000.00000"       , "%,015.5f"           ,  0.0         );
    checkFormat(ut,    "00,000,000.00000"       , "%,016.5f"           ,  0.0         );


    checkFormat(ut,                 "0.0"       , "%,.1f"              ,          0.0 );
    checkFormat(ut,                 "1.0"       , "%,.1f"              ,          1.0 );
    checkFormat(ut,                "10.0"       , "%,.1f"              ,         10.0 );
    checkFormat(ut,               "100.0"       , "%,.1f"              ,        100.0 );
    checkFormat(ut,             "1,000.0"       , "%,.1f"              ,       1000.0 );
    checkFormat(ut,            "10,000.0"       , "%,.1f"              ,      10000.0 );
    checkFormat(ut,           "100,000.0"       , "%,.1f"              ,     100000.0 );
    checkFormat(ut,         "1,000,000.0"       , "%,.1f"              ,    1000000.0 );
    checkFormat(ut,        "10,000,000.0"       , "%,.1f"              ,   10000000.0 );
    checkFormat(ut,       "100,000,000.0"       , "%,.1f"              ,  100000000.0 );

    checkFormat(ut,             "0.00000"       ,  "%,05.5f"           ,  0.0         );
    checkFormat(ut,             "0.00000"       ,  "%,06.5f"           ,  0.0         );
    checkFormat(ut,             "0.00000"       ,  "%,07.5f"           ,  0.0         );
    checkFormat(ut,            "00.00000"       ,  "%,08.5f"           ,  0.0         );
    checkFormat(ut,           "000.00000"       ,  "%,09.5f"           ,  0.0         );
    checkFormat(ut,          " 000.00000"       , "%#,010.5f"           ,  0.0         );
    checkFormat(ut,         "0,000.00000"       , "%#,011.5f"           ,  0.0         );
    checkFormat(ut,        "00,000.00000"       , "%#,012.5f"           ,  0.0         );
    checkFormat(ut,       "000,000.00000"       , "%#,013.5f"           ,  0.0         );
    checkFormat(ut,      " 000,000.00000"       , "%#,014.5f"           ,  0.0         );
    checkFormat(ut,     "0,000,000.00000"       , "%#,015.5f"           ,  0.0         );
    checkFormat(ut,    "00,000,000.00000"       , "%#,016.5f"           ,  0.0         );

    checkFormat(ut,           "1.00000"         , "%,#5.5f"            ,  1.0         );
    checkFormat(ut,           "1.00000"         , "%,#6.5f"            ,  1.0         );
    checkFormat(ut,           "1.00000"         , "%,#7.5f"            ,  1.0         );
    checkFormat(ut,          " 1.00000"         , "%,#8.5f"            ,  1.0         );
    checkFormat(ut,         "  1.00000"         , "%,#9.5f"            ,  1.0         );
    checkFormat(ut,        "   1.00000"         , "%,#10.5f"           ,  1.0         );
    checkFormat(ut,       "    1.00000"         , "%,#11.5f"           ,  1.0         );
    checkFormat(ut,      "     1.00000"         , "%,#12.5f"           ,  1.0         );
    checkFormat(ut,     "      1.00000"         , "%,#13.5f"           ,  1.0         );
    checkFormat(ut,    "       1.00000"         , "%,#14.5f"           ,  1.0         );
    checkFormat(ut,   "        1.00000"         , "%,#15.5f"           ,  1.0         );
    checkFormat(ut,  "         1.00000"         , "%,#16.5f"           ,  1.0         );

    checkFormat(ut,           "1.00000"         , "%,#05.5f"            ,  1.0         );
    checkFormat(ut,           "1.00000"         , "%,#06.5f"            ,  1.0         );
    checkFormat(ut,           "1.00000"         , "%,#07.5f"            ,  1.0         );
    checkFormat(ut,          "01.00000"         , "%,#08.5f"            ,  1.0         );
    checkFormat(ut,         "001.00000"         , "%,#09.5f"            ,  1.0         );
    checkFormat(ut,        " 001.00000"         , "%,#010.5f"           ,  1.0         );
    checkFormat(ut,       "0,001.00000"         , "%,#011.5f"           ,  1.0         );
    checkFormat(ut,      "00,001.00000"         , "%,#012.5f"           ,  1.0         );
    checkFormat(ut,     "000,001.00000"         , "%,#013.5f"           ,  1.0         );
    checkFormat(ut,    " 000,001.00000"         , "%,#014.5f"           ,  1.0         );
    checkFormat(ut,   "0,000,001.00000"         , "%,#015.5f"           ,  1.0         );
    checkFormat(ut,  "00,000,001.00000"         , "%,#016.5f"           ,  1.0         );

    checkFormat(ut,            "0.00000"        , "%,#.5f"              ,          0.0 );
    checkFormat(ut,            "1.00000"        , "%,#.5f"              ,          1.0 );
    checkFormat(ut,           "10.00000"        , "%,#.5f"              ,         10.0 );
    checkFormat(ut,          "100.00000"        , "%,#.5f"              ,        100.0 );
    checkFormat(ut,        "1,000.00000"        , "%,#.5f"              ,       1000.0 );
    checkFormat(ut,       "10,000.00000"        , "%,#.5f"              ,      10000.0 );
    checkFormat(ut,      "100,000.00000"        , "%,#.5f"              ,     100000.0 );
    checkFormat(ut,    "1,000,000.00000"        , "%,#.5f"              ,    1000000.0 );
    checkFormat(ut,   "10,000,000.00000"        , "%,#.5f"              ,   10000000.0 );
    checkFormat(ut,  "100,000,000.00000"        , "%,#.5f"              ,  100000000.0 );


    UT_PRINT(  "ALib Format Tests Java Style: Done" );

    testFormatter->Release();
}

//--------------------------------------------------------------------------------------------------
//--- Test PythonStyle
//--------------------------------------------------------------------------------------------------
UT_METHOD( FormatterPythonStyle )
{
    UT_INIT();
    UT_PRINT( "ALib Format Tests Python Style: Start" );

    aworx::FormatterPythonStyle formatterPS;
    formatterPS.Next.reset( new aworx::FormatterJavaStyle() );
    testFormatter= &formatterPS;
    testFormatter->Acquire(ALIB_CALLER_PRUNED);

    formatterPS.AlternativeNumberFormat.DecimalPointChar=     ',';
    formatterPS.AlternativeNumberFormat.ThousandsGroupChar=   '.';


    NumberFormat nfBackup;
    nfBackup.Set( &formatterPS.DefaultNumberFormat );


    //===== Nullptr  =========
    checkFormat(ut, "Hello PX"                   , "Hello {}", 'P', nullptr, nullptr, "X", nullptr);

    //===== Conversion '!'  =========
    checkFormat(ut, "Hello world"                , "{}{!X} {}"                 , "Hello", "freaking", "world"  );
    checkFormat(ut, "world"                      , "{!X}{!X}{}"                , "Hello", "freaking", "world"  );
    checkFormat(ut, ""                           , "{!X}{!X}{!X}"              , "Hello", "freaking", "world"  );
    checkFormat(ut, "HELLO world"                , "{!U} {!L}"                 , "hELlo", "WorlD"  );
    checkFormat(ut, "HELLO hello"                , "{!U} {0!L}"                , "hELlo"           );
    checkFormat(ut, "ABC abc"                    , "{1!U} {1!L}"               , "hELlo", "abc"    );
    checkFormat(ut, "This \"is\" it"             , "This {!str} it"            , "is"              );
    checkFormat(ut, "This is \"quoted\""         , "This is {!Quote}"          , "quoted"          );
    checkFormat(ut, "This is \"quoted\""         , "This is {!Qu}"             , "quoted"          );
    checkFormat(ut, "This is \"QUOTEDUPPER\""    , "This is {!Qu!UP}"          , "quotedUpper"      );
    checkFormat(ut, "This is \"QUOTEDUPPER\""    , "This is {!Up!Qu}"          , "quotedUpper"      );
    checkFormat(ut, "This is \"quotedlower\""    , "This is {!Up!Qu!Lo}"       , "quotedLOWER"      );
    checkFormat(ut, "This is \"quotedlower\""    , "This is {!Qu!Lo}"          , "quotedLOWER"      );

    checkFormat(ut, "X   Y"                      , "X{!Fill}Y"                 , 3      );
    checkFormat(ut, "XY"                         , "X{!Fill}Y"                 , 0      );
    checkFormat(ut, "X@@@Y"                      , "X{!FillC@}Y"               , 3      );
    checkFormat(ut, "X   Y"                      , "X{!FillC}Y"                , 3      );

    checkFormat(ut, "Tab     X"                  , "Tab{!Tab}"                 , "X"      );
    checkFormat(ut, "Tab10     X"                , "Tab10{!Tab10}"             , "X"      );
    checkFormat(ut, "Tab10x    X"                , "Tab10x{!Tab10}"            , "X"      );
    checkFormat(ut, "Tab10xx   X"                , "Tab10xx{!Tab10}"           , "X"      );
    checkFormat(ut, "Tab10xxxx X"                , "Tab10xxxx{!Tab10}"         , "X"      );
    checkFormat(ut, "Tab10xxxxx          X"      , "Tab10xxxxx{!Tab10}"        , "X"      );
    checkFormat(ut, "Tab10xxxxxx         X"      , "Tab10xxxxxx{!Tab10}"       , "X"      );
    checkFormat(ut, "Tab10xxxxxx*********X"      , "Tab10xxxxxx{!TabC*10}"     , "X"      );

    checkFormat(ut, "ATab2X"                     , "ATab2{!ATab2}"             , "X"           );
    checkFormat(ut, "ATab2x  X"                  , "ATab2x{!ATab2}"            , "X"           );
    checkFormat(ut, "ATab2xx X"                  , "ATab2xx{!ATab2}"           , "X"           );
    checkFormat(ut, "ATab2xxxX"                  , "ATab2xxx{!ATab2}"          , "X"           );
    checkFormat(ut, "ATab2xxxx  X"               , "ATab2xxxx{!ATab2}"         , "X"           );
    checkFormat(ut, "ATab2x     X"               , "ATab2x{!ATab2}"            , "X"           );
    checkFormat(ut, "ATab2x     XY"              , "ATab2x{!ATab2}{!ATab3}"    , "X"   , "Y"   );
    checkFormat(ut, "ATab2x     Xxx   Y"         , "ATab2x{!ATab2}{!ATab3}"    , "Xxx" , "Y"   );
    checkFormat(ut, "ATab2x     Xxxx  Y"         , "ATab2x{!ATab2}{!ATab3}"    , "Xxxx", "Y"   );
    checkFormat(ut, "           ABC   123"       , "{!ATab2}{!ATab3}"          , "ABC" , "123" );
    checkFormat(ut, "           abc   123"       , "{!ATab2!L}{!ATab3}"        , "ABC" , "123" );
    checkFormat(ut, "===========abc---123"       , "{!ATabC=2!L}{!ATabC-3}"    , "ABC" , "123" );
    testFormatter->Release(); // this resets the formatter!
    testFormatter->Acquire(ALIB_CALLER_PRUNED);
    checkFormat(ut, "12A"                        , "{!ATab2}{!ATab3}"           , "12"  , "A"   );
    checkFormat(ut, "x  123   A"                 , "x{!ATab2}{!ATab3}"         , "123" , "A"   );
    checkFormat(ut, "x  123   A\n   456   B"     , "x{!ATab2}{!ATab3}\\n{!ATab2}{!ATab3}"  , "123" , "A", "456" , "B"   );
    checkFormat(ut, "IJ"                         , "{!ATabRes!ATab2}{!ATab3}"  , "I"   , "J"   );
    checkFormat(ut, "   X\n   Y"                 , "{!Tab3}\\n{!Tab3}"         , "X"   , "Y"   );
    testFormatter->Release(); // this resets the formatter!
    testFormatter->Acquire(ALIB_CALLER_PRUNED);

    checkFormat(ut, "\\r\\n\\t"                   , "{!ESC<}"                  , "\r\n\t" );
    checkFormat(ut, "\t\\r\\n\\t\t"               , "\t{!ESC<}\t"              , "\r\n\t" );

    checkFormat(ut, "\r"                          , "{!ESC>}"                  , "\\r"          );
    checkFormat(ut, "\r\n\t"                      , "{!ESC>}"                  , "\\r\\n\\t"    );
    checkFormat(ut, "\t\r\n\t\t"                  , "\t{!ESC>}\t"              , "\\r\\n\\t"    );


    checkFormat(ut, "This is right"              , "This is {!Repl<wrong><right>}" , "wrong"    );
    checkFormat(ut, "Hello C++ ALib dude"        , "Hello {!Repl<world><ALib>}"    , "C++ world dude" );
    checkFormat(ut, "Never empty: abc"           , "Never empty: {!Repl<><N/A>}"   , "abc"    );
    checkFormat(ut, "Never empty: N/A"           , "Never empty: {!Repl<><N/A>}"   , ""       );

    checkFormat(ut, "Auto width"                 , "Auto {!AWidth:>}"         , "width"  );
    checkFormat(ut, "Auto     w"                 , "Auto {!AWidth:>}"         , "w"      );
    testFormatter->Release(); // this resets the formatter!
    testFormatter->Acquire(ALIB_CALLER_PRUNED);

    //============================ Samples taken from Python docs ==================================
    checkFormat(ut,  "a, b, c"          , "{0}, {1}, {2}"           ,  'a', 'b', 'c' );
    checkFormat(ut,  "a, b, c"          , "{}, {}, {}"              ,  'a', 'b', 'c' );
    checkFormat(ut,  "c, b, a"          , "{2}, {1}, {0}"           ,  'a', 'b', 'c' );

    checkFormat(ut,  "abracadabra"      , "{0}{1}{0}"               ,  "abra", "cad" );

    //===== replace brackets and new line =========
    checkFormat(ut, "repl. brackets{ X"   , "repl. brackets{{ {}"              , "X" );
    checkFormat(ut, "repl. brackets{{X"   , "repl. brackets{{{{{}"             , "X" );
    checkFormat(ut, "repl. brackets} X"   , "repl. brackets}} {}"              , "X" );
    checkFormat(ut, "repl. X}brackets{"   , "repl. {}}}brackets{{"             , "X" );


    checkFormat(ut, "x\\nxX"                , "x\\nx"                          , "X" ); // not a format string (no replacement)
    checkFormat(ut, "x\nx{}X"               , "x\nx{}"                         , "X" ); // not a format string (contains \n)
    checkFormat(ut, "y\nyX"                 , "y\\ny{}"                        , "X" ); // a format string (contains \n)

    //===== Boolean =========
    checkFormat(ut, "true false true false true", "{:B} {:B} {:B} {:B} {:B}"  , true, false, 1, 0, "Hello"  );
    checkFormat(ut, "true",         "{:.4B}"   , true  );
    checkFormat(ut, "fals",         "{:.4B}"   , false );
    checkFormat(ut, "tru",          "{:.3B}"   , true  );
    checkFormat(ut, "fal",          "{:.3B}"   , false );
    checkFormat(ut, "  tru",        "{:5.3B}"  , true  );
    checkFormat(ut, "  fal",        "{:5.3B}"  , false );
    checkFormat(ut, " true",        "{:5.6B}"  , true  );
    checkFormat(ut, "false",        "{:5.6B}"  , false );

    //===== h/H Hash Code (extension to Python format spec) =========
    // Not testable since built-in box-function FHashcode was introduced.
    // checkFormat(ut,    "ff"            , "{:h}"                    , 255);
    // checkFormat(ut,  "0xff"            , "{:#h}"                   , 255);
    // checkFormat(ut,    "FF"            , "{:H}"                    , 255);
    // checkFormat(ut,  "0xFF"            , "{:#H}"                   , 255);

    //===== DateTime/calendar =========
    CalendarDateTime ct;
    ct.Year     = 2016;
    ct.Day      =    5;
    ct.Month    =    9;
    ct.Hour     =   14;
    ct.Minute   =    5;
    ct.Second   =   22;
    DateTime dateTime( ct.Get() );

    checkFormat(ut,  "2016-09-05 14:05:22"       , "{:yyyy-MM-dd HH:mm:ss}"         ,dateTime);



    //================================== Mixed tests ==============================================
    {
        checkFormat(ut,  "a  "             , "{:3}"                       ,  'a'  );
        checkFormat(ut,  "ab "             , "{:3}"                       ,  "ab" );

        checkFormat(ut,  "  3"             , "{:3}"                       ,   3 );
        checkFormat(ut,  " 13"             , "{:3}"                       ,  13 );

        checkFormat(ut,  "0,1,0,2"         , "{},{},{0},{}"               ,0,1,2);
        checkFormat(ut,  "3,0,1,0,2"       , "{3},{},{},{0},{}"           ,0,1,2,3);


        char                c=  'x';
        signed char     sChar=  'y';
        unsigned char   uChar=  'z';
        signed char     sChar2=  -5;
        AString as;   as << c << '/' << sChar << '/' << uChar << '/' << sChar2; UT_EQ( A_CHAR("x/121/122/-5"), as );

        checkFormat(ut,  "x"               , "{}"                         ,  c  );
        checkFormat(ut,  "121"             , "{}"                         , sChar  );
        checkFormat(ut,  "122"             , "{}"                         , uChar  );
        checkFormat(ut,  "-5"              , "{}"                         , sChar2 );
    }

    //======================================= Errors ===========================================

    checkError (ut,  Exceptions::DuplicateTypeCode         , "{:df}"             , 'x'  );
    checkError (ut,  Exceptions::DuplicateTypeCode         , "{:dfdf}"           , 'x'  );
    checkError (ut,  Exceptions::IncompatibleTypeCode      , "{:f}"              , 'x'  );
    checkError (ut,  Exceptions::ArgumentIndexOutOfBounds  , "{}{}"              , 'x'  );
    checkError (ut,  Exceptions::ArgumentIndexOutOfBounds  , "{2}"               , 'x'  );
    checkError (ut,  Exceptions::ArgumentIndexOutOfBounds  , "{1}"               , 'x'  );
    checkFormat(ut,  "x"                                   , "{0}"               , 'x'  );
    checkError (ut,  Exceptions::MissingClosingBracket     , "abc {-1}"          , 'x'  );
    checkError (ut,  Exceptions::MissingClosingBracket     , "abc {"             , 'x'  );
    checkError (ut,  Exceptions::MissingClosingBracket     , "abc {!Q:<"         , 'x'  );
    checkError (ut,  Exceptions::UnknownConversionPS       , "abc {!P}"          , 'x'  );
    checkError (ut,  Exceptions::ExclamationMarkExpected   , "abc {!Qack}"       , 'x'  );
    checkError (ut,  Exceptions::ExclamationMarkExpected   , "abc {!Quo!UppR}"   , 'x'  );

    checkError (ut,  Exceptions::UnknownTypeCode           , "abc {:t}"          , 'x'  );
    checkError (ut,  Exceptions::UnknownTypeCode           , "abc {:<.5t}"       , 'x'  );

    checkError (ut,  Exceptions::MissingPrecisionValuePS   , "abc {:<.g}"       , 3.154  );
    checkError (ut,  Exceptions::MissingPrecisionValuePS   , "abc {:-.<g}"      , 3.154  );


    //======================================= Characters ===========================================

    checkFormat(ut,  "x"                        , "{}"           , 'x'          );
    checkFormat(ut,  "x    "                    , "{:5c}"        , 'x'          );
    checkFormat(ut,  NString64()._(L"\u03B1")    , "{:c}"         , L'\u03B1'    ); //greek alpha
    checkFormat(ut,  NString64()._(L"\u03B1    "), "{:5c}"        , L'\u03B1'    ); //greek alpha
    checkFormat(ut,  "@"                        , "{:c}"         , 64           ); // int   ascii @
    checkFormat(ut,  "@"                        , "{:c}"         , 64L          ); // long  ascii @
    checkFormat(ut,  "@"                        , "{:c}"         , 64UL         ); // ulong ascii @

    // alignment
    checkFormat(ut,  "#x  #"                , "#{:<3}#"         , 'x'    );
    checkFormat(ut,  "# x #"                , "#{:^3}#"         , 'x'    );
    checkFormat(ut,  "#  x#"                , "#{:>3}#"         , 'x'    );



    //======================================= Strings ===========================================
    checkFormat(ut,  "x"                 , "{}"         , "x"    );
    checkFormat(ut,  "xy"                , "{}"         , "xy"    );
    checkFormat(ut,  "xy"                , "{:s}"       , "xy"    );

    checkFormat(ut,  "xyz"                , "{}z"       , "xy"    );
    checkFormat(ut,  "xyz"                , "{:2}z"     , "xy"    );
    checkFormat(ut,  "xy z"               , "{:3}z"     , "xy"    );

    // precision (max width)
    checkFormat(ut,  "xyz"                , "{:.4s}"       , "xyz"    );
    checkFormat(ut,  "xyz"                , "{:.3s}"       , "xyz"    );
    checkFormat(ut,  "xy"                 , "{:.2s}"       , "xyz"    );


    // alignment
    checkFormat(ut,  "#x  #"                , "#{:<3}#"         , "x"    );
    checkFormat(ut,  "# x #"                , "#{:^3}#"         , "x"    );
    checkFormat(ut,  "#  x#"                , "#{:>3}#"         , "x"    );
    checkFormat(ut,  "#1234#"               , "#{:^3}#"         , "1234" );

    //===== Strings width/precision =========
    checkFormat(ut, "aBcDe",         "{:s}"          , "aBcDe"  );
    checkFormat(ut, "aBc",           "{:.3s}"        , "aBcDe"  );
    checkFormat(ut, "  aBc",         "{:>5.3s}"      , "aBcDe"  );
    checkFormat(ut, "ABC   ",        "{!UP:-6.3s}"   , "aBcDe"  );
    checkFormat(ut, " ABC  ",        "{!UP:^6.3s}"   , "aBcDe"  );
    checkFormat(ut, "ABCDE",         "{!UP:3s}"      , "aBcDe"  );

    checkFormat(ut, "aBcDe",         "{:}"          , "aBcDe"  );
    checkFormat(ut, "aBc",           "{:.3}"        , "aBcDe"  );
    checkFormat(ut, "  aBc",         "{:>5.3}"      , "aBcDe"  );
    checkFormat(ut, "ABC   ",        "{!UP:-6.3}"   , "aBcDe"  );
    checkFormat(ut, " ABC  ",        "{!UP:^6.3}"   , "aBcDe"  );
    checkFormat(ut, "ABCDE",         "{!UP:3}"      , "aBcDe"  );

    //======================================= Wide Strings ===========================================
    checkFormat(ut,  "x"                 , "{}"         , L"x"    );
    checkFormat(ut,  "xy"                , "{}"         , L"xy"    );
    checkFormat(ut,  "xy"                , "{:s}"       , L"xy"    );

    // precision (max width)
    checkFormat(ut,  "xyz"                , "{:.4s}"       , L"xyz"    );
    checkFormat(ut,  "xyz"                , "{:.3s}"       , L"xyz"    );
    checkFormat(ut,  "xy"                 , "{:.2s}"       , L"xyz"    );

    // precision (max width) with utf8-encoding

    NAString testBuf; testBuf._(L"\u03B1\u03B2\u03B3");
    checkFormat(ut,  testBuf               , "{:.4s}"       , L"\u03B1\u03B2\u03B3"    );
    checkFormat(ut,  testBuf               , "{:.3s}"       , L"\u03B1\u03B2\u03B3"    );
    testBuf.Reset(L"\u03B1\u03B2");
    checkFormat(ut,  testBuf               , "{:.2s}"       , L"\u03B1\u03B2\u03B3"    );
    testBuf.Reset(L"\u03B1");
    checkFormat(ut,  testBuf               , "{:.1s}"       , L"\u03B1\u03B2\u03B3"    );


    // alignment
    checkFormat(ut,  "#x  #"                , "#{:<3}#"         , L"x"    );
    checkFormat(ut,  "# x #"                , "#{:^3}#"         , L"x"    );
    checkFormat(ut,  "#  x#"                , "#{:>3}#"         , L"x"    );
    checkFormat(ut,  "#1234#"               , "#{:^3}#"         , L"1234" );



    //===================================== C# int types  ==========================================
    { int8_t    val=-1;    checkFormat(ut,  "-1"                    , "{:2}"              ,  val    ); }
    {uint8_t    val= 2;    checkFormat(ut,  " 2"                    , "{:2}"              ,  val    ); }
    { int16_t   val=-3;    checkFormat(ut,  "-3"                    , "{:2}"              ,  val    ); }
    {uint16_t   val= 4;    checkFormat(ut,  " 4"                    , "{:2}"              ,  val    ); }
    { int32_t   val=-5;    checkFormat(ut,  "-5"                    , "{:2}"              ,  val    ); }
    {uint32_t   val= 6;    checkFormat(ut,  " 6"                    , "{:2}"              ,  val    ); }
    { int64_t   val=-7;    checkFormat(ut,  "-7"                    , "{:2}"              ,  val    ); }
    {uint64_t   val= 8;    checkFormat(ut,  " 8"                    , "{:2}"              ,  val    ); }
    { intGap_t  val=-9;    checkFormat(ut,  "-9"                    , "{:2}"              ,  val    ); }
    {uintGap_t  val=10;    checkFormat(ut,  "10"                    , "{:2}"              ,  val    ); }

    //======================================= Integers (Decimal) ===========================================

    checkFormat(ut,  "0"                    , "{}"              ,  0    );
    checkFormat(ut,  "1"                    , "{}"              ,  1    );
    checkFormat(ut,  "5"                    , "{}"              ,  5    );
    checkFormat(ut,  "-1"                   , "{}"              ,  -1   );
    checkFormat(ut,  "-5"                   , "{}"              ,  -5   );
    checkFormat(ut,  "0"                    , "{}"              ,  0U   );
    checkFormat(ut,  "1"                    , "{}"              ,  1U   );
    checkFormat(ut,  "5"                    , "{}"              ,  5U   );
    checkFormat(ut,  "-2147483648"          , "{}"              ,  std::numeric_limits< int32_t>::min()   );
    checkFormat(ut,  "2147483647"           , "{}"              ,  std::numeric_limits< int32_t>::max()   );
    checkFormat(ut,  "-2147483647"          , "{}"              ,  std::numeric_limits< int32_t>::min() +1);
    checkFormat(ut,  "2147483646"           , "{}"              ,  std::numeric_limits< int32_t>::max() -1);
    checkFormat(ut,  "0"                    , "{}"              ,  std::numeric_limits<uint32_t>::min()   );
    checkFormat(ut,  "4294967295"           , "{}"              ,  std::numeric_limits<uint32_t>::max()   );
    checkFormat(ut,  "1"                    , "{}"              ,  std::numeric_limits<uint32_t>::min() +1);
    checkFormat(ut,  "4294967294"           , "{}"              ,  std::numeric_limits<uint32_t>::max() -1);
    checkFormat(ut,  "-9223372036854775808" , "{}"              ,  std::numeric_limits< int64_t >::min()   );
    checkFormat(ut,  "9223372036854775807"  , "{}"              ,  std::numeric_limits< int64_t >::max()   );
    checkFormat(ut,  "-9223372036854775807" , "{}"              ,  std::numeric_limits< int64_t >::min() +1);
    checkFormat(ut,  "9223372036854775806"  , "{}"              ,  std::numeric_limits< int64_t >::max() -1);
    checkFormat(ut,  "0"                    , "{}"              ,  std::numeric_limits<uint64_t >::min()   );
    checkFormat(ut,  "18446744073709551615" , "{}"              ,  std::numeric_limits<uint64_t >::max()   );
    checkFormat(ut,  "1"                    , "{}"              ,  std::numeric_limits<uint64_t >::min() +1);
    checkFormat(ut,  "18446744073709551614" , "{}"              ,  std::numeric_limits<uint64_t >::max() -1);




    // integer sign
    checkFormat(ut,  "0"                    , "{:}"             ,  0    );
    checkFormat(ut,  "1"                    , "{:}"             ,  1    );
    checkFormat(ut,  "-1"                   , "{:}"             , -1    );

    checkFormat(ut,  " 0"                   , "{: }"            ,  0    );
    checkFormat(ut,  " 1"                   , "{: }"            ,  1    );
    checkFormat(ut,  "-1"                   , "{: }"            , -1    );

    checkFormat(ut,  "+0"                   , "{:+}"            ,  0    );
    checkFormat(ut,  "+1"                   , "{:+}"            ,  1    );
    checkFormat(ut,  "-1"                   , "{:+}"            , -1    );

    // integer field width
    checkFormat(ut,  " 0"                   , "{:2}"            ,  0    );
    checkFormat(ut,  "1234"                 , "{:2}"            ,  1234    );

    // integer mindigits
    checkFormat(ut,   "000"                 , "{:03}"           ,  0    );
    checkFormat(ut,   "001"                 , "{:03}"           ,  1    );
    checkFormat(ut,   "-01"                 , "{:03}"           , -1    );
    checkFormat(ut,   " 00"                 , "{:03 }"          ,  0    );
    checkFormat(ut,   " 01"                 , "{:03 }"          ,  1    );
    checkFormat(ut,   "-01"                 , "{:03 }"          , -1    );

    checkError (ut,  Exceptions::PrecisionSpecificationWithInteger, "Test {:.3}"    ,  123456);
    checkError (ut,  Exceptions::PrecisionSpecificationWithInteger, "Test {!Q:.3}"  ,  123456);


    //======================================= Binary ===========================================

    // variable length

    // without separators
    checkFormat(ut,                                                  "0", "{:b}" ,         0x00  );
    checkFormat(ut,                                                  "1", "{:b}" ,         0x01  );
    checkFormat(ut,                                                 "10", "{:b}" ,         0x02  );
    checkFormat(ut,                                               "1001", "{:b}" ,         0x09  );
    checkFormat(ut,                                               "1111", "{:b}" ,         0x0F  );
    checkFormat(ut,                                              "10001", "{:b}" ,         0x11  );
    checkFormat(ut,                                           "10001111", "{:b}" ,         0x8F  );
    checkFormat(ut,                                          "100111110", "{:b}" ,        0x13E  );
    checkFormat(ut,                                   "1111111111111111", "{:b}" ,       0xFFFF  );
    checkFormat(ut,                   "10000001100000011111111111111111", "{:b}" ,   0x8181FFFFL );

    // with separators
    // default

    formatterPS.DefaultNumberFormat.BinNibbleGroupChar=   '~';
    formatterPS.DefaultNumberFormat.BinByteGroupChar=     '\'';
    formatterPS.DefaultNumberFormat.BinWordGroupChar=     '-';
    formatterPS.DefaultNumberFormat.BinWord32GroupChar=   '#';
    checkFormat(ut,                                                  "0", "{:b,}",         0x00  );
    checkFormat(ut,                                                  "1", "{:b,}",         0x01  );
    checkFormat(ut,                                                 "10", "{:b,}",         0x02  );
    checkFormat(ut,                                               "1001", "{:b,}",         0x09  );
    checkFormat(ut,                                               "1111", "{:b,}",         0x0F  );
    checkFormat(ut,                                             "1~0001", "{:b,}",         0x11  );
    checkFormat(ut,                                          "1000~1111", "{:b,}",         0x8F  );
    checkFormat(ut,                                        "1'0011~1110", "{:b,}",        0x13E  );
    checkFormat(ut,                                "1111~1111'1111~1111", "{:b,}",       0xFFFF  );
    checkFormat(ut,            "1000~0001'1000~0001-1111~1111'1111~1111", "{:b,}",   0x8181FFFFL );

    checkFormat(ut,  "1111~1111#0001~0001'0010~0010-0011~0011'0100~0100", "{:b,}", 0xFF11223344  );

    formatterPS.DefaultNumberFormat.BinNibbleGroupChar=     '\0';
    checkFormat(ut,  "11111111#00010001'00100010-00110011'01000100", "{:b,}", 0xFF11223344L );
    formatterPS.DefaultNumberFormat.Set(&nfBackup);

    // fixed length
    checkFormat(ut,                                 "0000000100111110", "{:16b}" ,              0x13E  );
    checkFormat(ut,                                 "1001000100111110", "{:16b}" ,             0x913E  );
    checkFormat(ut,                                 "1001000100111110", "{:16b}" ,       0xABCDEF913E  );
    checkFormat(ut,                                "11001000100111110", "{:17b}" ,       0xABCDEF913E  );
    checkFormat(ut,     "1111000011110000111100001111000011110000111100001111000011110000", "{:64b}" , 0xF0F0F0F0F0F0F0F0  );

    checkFormat(ut,  "bbb1111000011110000111100001111000011110000111100001111000011110000", "{:b>67b}" , 0xF0F0F0F0F0F0F0F0  );


    // including 0b
    checkFormat(ut,                                               "0b0", "{:#b}"  ,        0x00  );
    checkFormat(ut,                                               "0b1", "{:#b}"  ,        0x01  );
    checkFormat(ut,                                              "0b10", "{:#b}"  ,        0x02  );
    checkFormat(ut,                                              "0b11", "{:#,b}" ,        0x03  );
    checkFormat(ut,                                          "0b1'0001", "{:#,b}" ,        0x11  );
    checkFormat(ut,                                              "0b0",  "{:0#b}" ,        0x00  );
    checkFormat(ut,                                                 "0", "{:1#b}" ,        0x00  );
    checkFormat(ut,                                                "0b", "{:2#b}" ,        0x00  );
    checkFormat(ut,                                               "0b0", "{:3#b}" ,        0x00  );
    checkFormat(ut,                                              "0b00", "{:4#b}" ,        0x00  );
    checkFormat(ut,                                             "0b000", "{:5#b}" ,        0x00  );
    checkFormat(ut,                                          "0b1'0001", "{:8#,b}",        0x11  );
    checkFormat(ut,                                         "0b01'0001", "{:9#,b}",        0x11  );
    checkFormat(ut,                                        "0b001'0001", "{:10#,b}",       0x11  );
    checkFormat(ut,                                          "0b 0001", "{:7#,b}",        0x11  );
    checkFormat(ut,                                           "0b0001", "{:6#,b}",        0x11  );
    checkFormat(ut,                                            "0b001", "{:5#,b}",        0x11  );


    //======================================= Hex ===========================================

    // variable length
    // without separators
    checkFormat(ut,                           "0", "{:X}"                ,                 0x00  );
    checkFormat(ut,                           "1", "{:X}"                ,                 0x01  );
    checkFormat(ut,                           "2", "{:X}"                ,                 0x02  );
    checkFormat(ut,                           "9", "{:X}"                ,                 0x09  );
    checkFormat(ut,                           "F", "{:X}"                ,                 0x0F  );
    checkFormat(ut,                          "11", "{:X}"                ,                 0x11  );
    checkFormat(ut,                          "8F", "{:X}"                ,                 0x8F  );
    checkFormat(ut,                         "13E", "{:X}"                ,                0x13E  );
    checkFormat(ut,                        "FFFF", "{:X}"                ,               0xFFFF  );
    checkFormat(ut,                    "8181FFFF", "{:X}"                ,           0x8181FFFFL );
    checkFormat(ut,                    "FFFFFFFF", "{:X}"                ,           0xFFFFFFFFL );
    checkFormat(ut,            "FFFFFFFFFFFFFFFF", "{:X}"                ,   0xFFFFFFFFFFFFFFFFL );
    checkFormat(ut,            "FFFFFFFFFFFFFFFF", "{:X}"                ,   0xFFFFFFFFFFFFFFFFUL);
    checkFormat(ut,            "FFFFFFFFFFFFFFFF", "{:X}"                ,          UINT64_C(-1) );

    // -------------- variable length, lower case --------
    // without separators
    checkFormat(ut,                           "0", "{:x}"                ,                 0x00  );
    checkFormat(ut,                           "1", "{:x}"                ,                 0x01  );
    checkFormat(ut,                           "2", "{:x}"                ,                 0x02  );
    checkFormat(ut,                           "9", "{:x}"                ,                 0x09  );
    checkFormat(ut,                           "f", "{:x}"                ,                 0x0F  );
    checkFormat(ut,                          "11", "{:x}"                ,                 0x11  );
    checkFormat(ut,                          "8f", "{:x}"                ,                 0x8F  );
    checkFormat(ut,                         "13e", "{:x}"                ,                0x13E  );
    checkFormat(ut,                        "ffff", "{:x}"                ,               0xFFFF  );
    checkFormat(ut,                    "8181ffff", "{:x}"                ,           0x8181FFFFL );
    checkFormat(ut,                    "ffffffff", "{:x}"                ,           0xFFFFFFFFL );
    checkFormat(ut,            "ffffffffffffffff", "{:x}"                ,   0xFFffFFffFFffFFffUL);


    // with separators
    // default
    checkFormat(ut,  "FF'1122'3344", "{:X,}", 0xFF11223344  );
    formatterPS.DefaultNumberFormat.HexByteGroupChar=     '\'';
    formatterPS.DefaultNumberFormat.HexWordGroupChar=     '-';
    formatterPS.DefaultNumberFormat.HexWord32GroupChar=   '#';
    checkFormat(ut,                          "8F", "{:X,}"               ,         0x8F  );
    checkFormat(ut,                        "1'3E", "{:X,}"               ,        0x13E  );
    checkFormat(ut,                       "FF'FF", "{:X,}"               ,       0xFFFF  );
    checkFormat(ut,                 "81'81-FF'FF", "{:X,}"               ,   0x8181FFFFL );
    checkFormat(ut,              "FF#11'22-33'44", "{:X,}"               , 0xFF11223344  );

    formatterPS.DefaultNumberFormat.HexByteGroupChar=     '\0';
    checkFormat(ut,  "FF#1122-3344", "{:X,}"                , 0xFF11223344  );
    formatterPS.DefaultNumberFormat.SetComputational();
    formatterPS.DefaultNumberFormat.WriteExponentPlusSign= true;



    // fixed length
    checkFormat(ut,                   "A3E", "{:0X}"    ,             0xA3E  );
    checkFormat(ut,                     "E", "{:1X}"    ,             0xA3E  );
    checkFormat(ut,                    "3E", "{:2X}"    ,             0xA3E  );
    checkFormat(ut,                   "A3E", "{:3X}"    ,             0xA3E  );
    checkFormat(ut,                  "0A3E", "{:4X}"    ,             0xA3E  );
    checkFormat(ut,                 "00A3E", "{:5X}"    ,             0xA3E  );
    checkFormat(ut,                "000A3E", "{:6X}"    ,             0xA3E  );
    checkFormat(ut,               "0000A3E", "{:7X}"    ,             0xA3E  );
    checkFormat(ut,              "00000A3E", "{:8X}"    ,             0xA3E  );
    checkFormat(ut,             "000000A3E", "{:9X}"    ,             0xA3E  );
    checkFormat(ut,       "000000000000A3E", "{:15X}"   ,             0xA3E  );
    checkFormat(ut,      "0000000000000A3E", "{:16X}"   ,             0xA3E  );
    checkFormat(ut,      "0000000000000A3E", "{:>16X}"  ,             0xA3E  );
    checkFormat(ut,     " 0000000000000A3E", "{:17X}"   ,             0xA3E  );
    checkFormat(ut,    "  0000000000000A3E", "{:18X}"   ,             0xA3E  );
    checkFormat(ut,     " 0000000000000A3E", "{:>17X}"  ,             0xA3E  );
    checkFormat(ut,    "  0000000000000A3E", "{:>18X}"  ,             0xA3E  );

    // width

    checkFormat(ut,                     "E", "{:1X}"    ,             0xA3E  );
    checkFormat(ut,                    "3E", "{:2X}"    ,             0xA3E  );
    checkFormat(ut,                   "A3E", "{:3X}"    ,             0xA3E  );

    // including 0x
    checkFormat(ut,                 "0xA3E",  "{:0#X}"  ,             0xA3E  );
    checkFormat(ut,                     "0",  "{:1#X}"  ,             0xA3E  );
    checkFormat(ut,                    "0x",  "{:2#X}"  ,             0xA3E  );
    checkFormat(ut,                   "0xE",  "{:3#X}"  ,             0xA3E  );
    checkFormat(ut,                  "0x3E",  "{:4#X}"  ,             0xA3E  );
    checkFormat(ut,                 "0xA3E",  "{:5#X}"  ,             0xA3E  );
    checkFormat(ut,                "0x0A3E", "{:6#X}"   ,             0xA3E  );
    checkFormat(ut,               "0x00A3E", "{:7#X}"   ,             0xA3E  );
    checkFormat(ut,              "0x000A3E", "{:8#X}"   ,             0xA3E  );
    checkFormat(ut,             "0x0000A3E", "{:9#X}"   ,             0xA3E  );
    checkFormat(ut,       "0x0000000000A3E", "{:15#X}"  ,             0xA3E  );
    checkFormat(ut,      "0x00000000000A3E", "{:16#X}"  ,             0xA3E  );
    checkFormat(ut,      "0x00000000000A3E", "{:>16#X}" ,             0xA3E  );
    checkFormat(ut,     "0x000000000000A3E", "{:17#X}"  ,             0xA3E  );
    checkFormat(ut,    "0x0000000000000A3E", "{:18#X}"  ,             0xA3E  );
    checkFormat(ut,    "0x0000000000000A3E", "{:>18#X}" ,             0xA3E  );
    checkFormat(ut,   "0x0000000000000A3E ", "{:<19#X}" ,             0xA3E  );
    checkFormat(ut,   " 0x0000000000000A3E", "{:>19#X}" ,             0xA3E  );
    checkFormat(ut,  "0x0000000000000A3E  ", "{:<20#X}" ,             0xA3E  );
    checkFormat(ut,  "  0x0000000000000A3E", "{:>20#X}" ,             0xA3E  );

    //======================================= Octal ===========================================

    // variable length
    // without separators
    checkFormat(ut,                             "0", "{:o}"              ,                  0  );
    checkFormat(ut,                             "1", "{:o}"              ,                  1  );
    checkFormat(ut,                             "2", "{:o}"              ,                  2  );
    checkFormat(ut,                             "7", "{:o}"              ,                  7  );
    checkFormat(ut,                            "10", "{:o}"              ,                  8  );
    checkFormat(ut,                            "11", "{:o}"              ,                  9  );
    checkFormat(ut,                            "17", "{:o}"              ,                 15  );
    checkFormat(ut,                            "20", "{:o}"              ,                 16  );
    checkFormat(ut,                            "77", "{:o}"              ,                 63  );
    checkFormat(ut,                           "100", "{:o}"              ,                 64  );
    checkFormat(ut,                           "101", "{:o}"              ,                 65  );
    checkFormat(ut,                        "177777", "{:o}"              ,             0xffff  );
    checkFormat(ut,                   "37777777777", "{:o}"              ,         0xffffffffL );
    checkFormat(ut,              "7777777777777777", "{:o}"              ,     0xffffffffffff  );
    checkFormat(ut,        "1777777777777777777777", "{:o}"              , 0xFFffFFffFFffFFffUL);
    checkFormat(ut,        "1777777777777777777777", "{:o}"              ,        UINT64_C(-1) );

    // with separator
    checkFormat(ut,                             "0", "{:,o}"             ,                  0  );
    checkFormat(ut,                             "1", "{:,o}"             ,                  1  );
    checkFormat(ut,                             "2", "{:,o}"             ,                  2  );
    checkFormat(ut,                             "7", "{:,o}"             ,                  7  );
    checkFormat(ut,                            "10", "{:,o}"             ,                  8  );
    checkFormat(ut,                            "11", "{:,o}"             ,                  9  );
    checkFormat(ut,                            "17", "{:,o}"             ,                 15  );
    checkFormat(ut,                            "20", "{:,o}"             ,                 16  );
    checkFormat(ut,                            "77", "{:,o}"             ,                 63  );
    checkFormat(ut,                           "100", "{:,o}"             ,                 64  );
    checkFormat(ut,                           "101", "{:,o}"             ,                 65  );
    checkFormat(ut,                       "177'777", "{:,o}"             ,             0xffff  );
    checkFormat(ut,                "37'777'777'777", "{:,o}"             ,         0xffffffffL );
    checkFormat(ut,         "7'777'777'777'777'777", "{:,o}"             ,     0xffffffffffff  );

    checkFormat(ut,  "1'777'777'777'777'777'777'777", "{:,o}"            , 0xFFffFFffFFffFFffUL);
    checkFormat(ut,  "1'777'777'777'777'777'777'777", "{:,o}"            ,        UINT64_C(-1) );



    // ints with thousands separator
    checkFormat(ut,            "0"               , "{:00,}"              ,  0            );
    checkFormat(ut,            "0"               , "{:01,}"              ,  0            );
    checkFormat(ut,           "00"               , "{:02,}"              ,  0            );
    checkFormat(ut,          "000"               , "{:03,}"              ,  0            );
    checkFormat(ut,         " 000"               , "{:04,}"              ,  0            );
    checkFormat(ut,        "0,000"               , "{:05,}"              ,  0            );
    checkFormat(ut,       "00,000"               , "{:06,}"              ,  0            );
    checkFormat(ut,      "000,000"               , "{:07,}"              ,  0            );
    checkFormat(ut,     " 000,000"               , "{:08,}"              ,  0            );
    checkFormat(ut,    "0,000,000"               , "{:09,}"              ,  0            );

    checkFormat(ut,            "1"               , "{:00,}"              ,  1            );
    checkFormat(ut,            "1"               , "{:01,}"              ,  1            );
    checkFormat(ut,           "01"               , "{:02,}"              ,  1            );
    checkFormat(ut,          "001"               , "{:03,}"              ,  1            );
    checkFormat(ut,         " 001"               , "{:04,}"              ,  1            );
    checkFormat(ut,        "0,001"               , "{:05,}"              ,  1            );
    checkFormat(ut,       "00,001"               , "{:06,}"              ,  1            );
    checkFormat(ut,      "000,001"               , "{:07,}"              ,  1            );
    checkFormat(ut,     " 000,001"               , "{:08,}"              ,  1            );
    checkFormat(ut,    "0,000,001"               , "{:09,}"              ,  1            );

    checkFormat(ut,            "0"               , "{:,}"                ,          0    );
    checkFormat(ut,            "1"               , "{:,}"                ,          1    );
    checkFormat(ut,           "10"               , "{:,}"                ,         10    );
    checkFormat(ut,          "100"               , "{:,}"                ,        100    );
    checkFormat(ut,        "1,000"               , "{:,}"                ,       1000    );
    checkFormat(ut,       "10,000"               , "{:,}"                ,      10000    );
    checkFormat(ut,      "100,000"               , "{:,}"                ,     100000    );
    checkFormat(ut,    "1,000,000"               , "{:,}"                ,    1000000    );
    checkFormat(ut,   "10,000,000"               , "{:,}"                ,   10000000    );
    checkFormat(ut,  "100,000,000"               , "{:,}"                ,  100000000    );

    checkFormat(ut,            "0"               , "{:,}"                ,          0    );
    checkFormat(ut,            "1"               , "{:,}"                ,          1    );
    checkFormat(ut,           "12"               , "{:,}"                ,         12    );
    checkFormat(ut,          "123"               , "{:,}"                ,        123    );
    checkFormat(ut,        "1,234"               , "{:,}"                ,       1234    );
    checkFormat(ut,       "12,345"               , "{:,}"                ,      12345    );
    checkFormat(ut,      "123,456"               , "{:,}"                ,     123456    );
    checkFormat(ut,    "1,234,567"               , "{:,}"                ,    1234567    );
    checkFormat(ut,   "12,345,678"               , "{:,}"                ,   12345678    );
    checkFormat(ut,  "123,456,789"               , "{:,}"                ,  123456789    );


    //======================================= Floats ===========================================

    // mixed tests
    checkFormat(ut,                     "0.0", "{}"              ,        0.0 );
    checkFormat(ut,      "0.3333333333333333", "{}"              ,    1.0/3.0 );
    checkFormat(ut,       "3.333333333333334", "{}"              ,   10.0/3.0 );
    checkFormat(ut,       "6.666666666666667", "{}"              ,   20.0/3.0 );
    checkFormat(ut,                 "4.23456", "{}"              ,    4.23456 );
    checkFormat(ut,                "-4.23456", "{}"              ,   -4.23456 );

    checkFormat(ut,                     "0.0", "{:.5}"              ,        0.0f );
    checkFormat(ut,                 "0.33333", "{:.5}"              ,   1.0f/3.0f );
    checkFormat(ut,                 "3.33333", "{:.5}"              ,  10.0f/3.0f );
    checkFormat(ut,                 "6.66667", "{:.5}"              ,  20.0f/3.0f );
    checkFormat(ut,                 "4.23456", "{:.5}"              ,    4.23456f );
    checkFormat(ut,                "-4.23456", "{:.5}"              ,   -4.23456f );

    checkFormat(ut,                 "0.0E+00", "{:E}"            ,        0.0 );
    checkFormat(ut,            "0.000000E+00", "{:#E}"           ,        0.0 );

    checkFormat(ut,               "123.45"   , "{}"              ,     123.45 );
    checkFormat(ut,               "123.4"    , "{:.1}"           ,     123.45 );
    checkFormat(ut,               "123"      , "{:.0}"           ,     123.45 );
    checkFormat(ut,               "123."     , "{:#.0}"          ,     123.45 );

    checkFormat(ut,               "0.45"     , "{:.5}"          ,        0.45 );

    checkFormat(ut,  "1.234e+20"       , "{}"                         ,  1.234e20 );
    checkFormat(ut,  "1.234"           , "{:g}"                       ,  1.234 );
    checkFormat(ut,  "1.234"           , "{:G}"                       ,  1.234 );
    checkFormat(ut,  "1.234e+20"       , "{:g}"                       ,  1.234e20 );
    checkFormat(ut,  "1.234e+20"       , "{:.6g}"                       ,  1.234e20 );
    checkFormat(ut,  "1.234E+20"       , "{:G}"                       ,  1.234e20 );
    checkFormat(ut,  "1.234"           , "{:#g}"                      ,  1.234 );
    checkFormat(ut,  "1.234"           , "{:#G}"                      ,  1.234 );
    checkFormat(ut,  "1.234000"        , "{:f}"                       ,  1.234 );
    checkFormat(ut,  "1.234000"        , "{:F}"                       ,  1.234 );
    checkFormat(ut,  "1.234000"        , "{:#f}"                      ,  1.234 );
    checkFormat(ut,  "1.234000"        , "{:#F}"                      ,  1.234 );
    checkFormat(ut,  "1.234568"        , "{:f}"                       ,  1.23456789 );
    checkFormat(ut,  "1.234568"        , "{:F}"                       ,  1.23456789 );
    checkFormat(ut,  "1.234568"        , "{:#f}"                      ,  1.23456789 );
    checkFormat(ut,  "1.234568"        , "{:#F}"                      ,  1.23456789 );

    // locale specific
    checkFormat(ut,   "1234.5678"         , "{:g}"           ,    1234.5678 );
    checkFormat(ut,   "1234,5678"         , "{:n}"           ,    1234.5678 );
    checkFormat(ut,  "1,234.5678"         , "{:,g}"          ,    1234.5678 );
    checkFormat(ut,  "1.234,5678"         , "{:,n}"          ,    1234.5678 );

    // integer types as floats
    checkFormat(ut,   "1234.0"            , "{:g}"           ,    1234 );
    checkFormat(ut,   "1234.0"            , "{:G}"           ,    1234 );
    checkFormat(ut,   "1234.000000"       , "{:f}"           ,    1234 );
    checkFormat(ut,   "1234.00000"        , "{:.5f}"         ,    1234 );
    checkFormat(ut,   "1234.00000"        , "{:#.5f}"        ,    1234 );
    checkFormat(ut, "  1234.00000"        , "{:12.5f}"       ,    1234 );
    checkFormat(ut,   "14.0"              , "{:.5g}"         ,      14 );
    checkFormat(ut,   "  14.00000"        , "{:#10.5g}"      ,      14 );
    checkFormat(ut,   "14.00000"          , "{:#.5g}"        ,      14 );


    //----------------------------- floats with width and precision ----------------------------

    // floats with field width
    checkFormat(ut,           "       0.0"       ,  "{:10}"             ,  0.0         );
    checkFormat(ut,           "       0.0"       ,  "{:10.5}"           ,  0.0         );
    checkFormat(ut,           "   0.00000"       , "{:#10.5}"           ,  0.0         );

    checkFormat(ut,           "     14.05"       ,  "{:10}"             ,  14.05       );
    checkFormat(ut,           "     14.05"       ,  "{:10.5}"           ,  14.05       );
    checkFormat(ut,           "  14.05000"       , "{:#10.5}"           ,  14.05       );
    checkFormat(ut,           " -14.05000"       , "{:#10.5}"           , -14.05       );
    checkFormat(ut,            "-14.05000"       , "{:#9.5}"            , -14.05       );
    checkFormat(ut,            "-14.05000"       , "{:#8.5}"            , -14.05       );

    // '='-alignment (forces padding after the sign)
    //     positive without sign
    checkFormat(ut,  "0012.3456789"          , "{:=12}"          ,  12.3456789   );
    checkFormat(ut,  "012.3456789"           , "{:=11}"          ,  12.3456789   );
    checkFormat(ut,  "12.3456789"            , "{:=10}"          ,  12.3456789   );
    checkFormat(ut,  "12.3456789"            , "{:=9}"           ,  12.3456789   );
    checkFormat(ut,  "12.3456789"            , "{:=8}"           ,  12.3456789   );
    checkFormat(ut,  "000012.3"              , "{:=8.1}"         ,  12.3456789   );
    checkFormat(ut,  "00012.35"              , "{:=8.2}"         ,  12.3456789   );
    checkFormat(ut,  "0012.346"              , "{:=8.3}"         ,  12.3456789   );
    checkFormat(ut,  "012.3457"              , "{:=8.4}"         ,  12.3456789   );

    checkFormat(ut,  " 012.3456789"          , "{:= 12}"         ,  12.3456789   );
    checkFormat(ut,  " 12.3456789"           , "{:= 11}"         ,  12.3456789   );
    checkFormat(ut,  " 12.3456789"           , "{:= 10}"         ,  12.3456789   );
    checkFormat(ut,  " 12.3456789"           , "{:= 9}"          ,  12.3456789   );
    checkFormat(ut,  " 12.3456789"           , "{:= 8}"          ,  12.3456789   );
    checkFormat(ut,  " 12.3456789"           , "{:= 8}"          ,  12.3456789   );
    checkFormat(ut,  " 00012.3"              , "{:= 8.1}"        ,  12.3456789   );
    checkFormat(ut,  " 0012.35"              , "{:= 8.2}"        ,  12.3456789   );
    checkFormat(ut,  " 012.346"              , "{:= 8.3}"        ,  12.3456789   );
    checkFormat(ut,  " 12.3457"              , "{:= 8.4}"        ,  12.3456789   );

    //     negative
    checkFormat(ut,  "-012.3456789"          , "{:= 12}"         , -12.3456789   );
    checkFormat(ut,  "-12.3456789"           , "{:= 11}"         , -12.3456789   );
    checkFormat(ut,  "-12.3456789"           , "{:= 10}"         , -12.3456789   );
    checkFormat(ut,  "-12.3456789"           , "{:= 9}"          , -12.3456789   );
    checkFormat(ut,  "-12.3456789"           , "{:= 8}"          , -12.3456789   );
    checkFormat(ut,  "-12.3456789"           , "{:= 8}"          , -12.3456789   );
    checkFormat(ut,  "-00012.3"              , "{:= 8.1}"        , -12.3456789   );
    checkFormat(ut,  "-0012.35"              , "{:= 8.2}"        , -12.3456789   );
    checkFormat(ut,  "-012.346"              , "{:= 8.3}"        , -12.3456789   );
    checkFormat(ut,  "-12.3457"              , "{:= 8.4}"        , -12.3456789   );

    //     positive with + sign
    checkFormat(ut,  "+012.3456789"          , "{:=+12}"         ,  12.3456789   );
    checkFormat(ut,  "+12.3456789"           , "{:=+11}"         ,  12.3456789   );
    checkFormat(ut,  "+12.3456789"           , "{:=+10}"         ,  12.3456789   );
    checkFormat(ut,  "+12.3456789"           , "{:=+9}"          ,  12.3456789   );
    checkFormat(ut,  "+12.3456789"           , "{:=+8}"          ,  12.3456789   );
    checkFormat(ut,  "+12.3456789"           , "{:=+8}"          ,  12.3456789   );
    checkFormat(ut,  "+00012.3"              , "{:=+8.1}"        ,  12.3456789   );
    checkFormat(ut,  "+0012.35"              , "{:=+8.2}"        ,  12.3456789   );
    checkFormat(ut,  "+012.346"              , "{:=+8.3}"        ,  12.3456789   );
    checkFormat(ut,  "+12.3457"              , "{:=+8.4}"        ,  12.3456789   );

    // Leading 0 in width name ( same as '='-alignment)
    checkFormat(ut,  "0012.3456789"          , "{:012}"         ,  12.3456789   );
    checkFormat(ut,  "012.3456789"           , "{:011}"         ,  12.3456789   );
    checkFormat(ut,  "12.3456789"            , "{:010}"         ,  12.3456789   );
    checkFormat(ut,  "12.3456789"            , "{:09}"          ,  12.3456789   );
    checkFormat(ut,  "12.3456789"            , "{:08}"          ,  12.3456789   );
    checkFormat(ut,  "000012.3"              , "{:08.1}"        ,  12.3456789   );
    checkFormat(ut,  "00012.35"              , "{:08.2}"        ,  12.3456789   );
    checkFormat(ut,  "0012.346"              , "{:08.3}"        ,  12.3456789   );
    checkFormat(ut,  "012.3457"              , "{:08.4}"        ,  12.3456789   );
    checkFormat(ut,  "-012.3456789"          , "{:=012}"        , -12.3456789   );
    checkFormat(ut,  "-0012.35"              , "{:=08.2}"       , -12.3456789   );

    //-------  Nan,Inf, -0.0 -------
    checkFormat(ut,  "nan"                  , "{}"              ,  std::nan("") );
    checkFormat(ut,  "inf"                  , "{}"              ,  std::numeric_limits<double>::infinity() );
    checkFormat(ut,  "-inf"                 , "{}"              , -std::numeric_limits<double>::infinity() );
    checkFormat(ut,  "0.0"                  , "{}"              ,  0.0 );
    checkFormat(ut,  "0.0"                  , "{}"              , -0.0 ); // negative zero
    checkFormat(ut,  "nan"                  , "{:+}"            ,  std::nan("") );
    checkFormat(ut,  "+inf"                 , "{:+}"            ,  std::numeric_limits<double>::infinity() );
    checkFormat(ut,  "-inf"                 , "{:+}"            , -std::numeric_limits<double>::infinity() );
    checkFormat(ut,  "+0.0"                 , "{:+}"            ,  0.0 );
    checkFormat(ut,  "+0.0"                 , "{:+}"            , -0.0 ); // negative zero

    //-------  Nan,Inf, -0.0 -------
    checkFormat(ut,  "       nan"           , "{:10}"           ,  std::nan("") );
    checkFormat(ut,  "       inf"           , "{:10}"           ,  std::numeric_limits<double>::infinity() );
    checkFormat(ut,  "      -inf"           , "{:10}"           , -std::numeric_limits<double>::infinity() );
    checkFormat(ut,  "       0.0"           , "{:10}"           ,  0.0 );
    checkFormat(ut,  "       0.0"           , "{:10}"           , -0.0 ); // negative zero

    checkFormat(ut,  "       nan"           , "{:+10}"          ,  std::nan("") );
    checkFormat(ut,  "      +inf"           , "{:+10}"          ,  std::numeric_limits<double>::infinity() );
    checkFormat(ut,  "      -inf"           , "{:+10}"          , -std::numeric_limits<double>::infinity() );
    checkFormat(ut,  "      +0.0"           , "{:+10}"          ,  0.0 );
    checkFormat(ut,  "      +0.0"           , "{:+10}"          , -0.0 ); // negative zero

    checkFormat(ut,  "       nan"           , "{:010}"          ,  std::nan("") );
    checkFormat(ut,  "       inf"           , "{:010}"          ,  std::numeric_limits<double>::infinity() );
    checkFormat(ut,  "-      inf"           , "{:010}"          , -std::numeric_limits<double>::infinity() );
    checkFormat(ut,  "00000000.0"           , "{:010}"          ,  0.0 );
    checkFormat(ut,  "00000000.0"           , "{:010}"          , -0.0 ); // negative zero

    checkFormat(ut,  "nan       "           , "{:<10}"          ,  std::nan("") );
    checkFormat(ut,  "inf       "           , "{:<10}"          ,  std::numeric_limits<double>::infinity() );
    checkFormat(ut,  "-inf      "           , "{:<10}"          , -std::numeric_limits<double>::infinity() );
    checkFormat(ut,  "0.0       "           , "{:<10}"          ,  0.0 );
    checkFormat(ut,  "0.0       "           , "{:<10}"          , -0.0 ); // negative zero
    checkFormat(ut,  "       nan"           , "{:>10}"          ,  std::nan("") );
    checkFormat(ut,  "       inf"           , "{:>10}"          ,  std::numeric_limits<double>::infinity() );
    checkFormat(ut,  "      -inf"           , "{:>10}"          , -std::numeric_limits<double>::infinity() );
    checkFormat(ut,  "       0.0"           , "{:>10}"          ,  0.0 );
    checkFormat(ut,  "       0.0"           , "{:>10}"          , -0.0 ); // negative zero

    // -------------------------------- scientific -------------------------------------------

    checkFormat(ut,  "1.234568e+01"         , "{:e}"            ,  12.3456789   );
    checkFormat(ut,  "1.235e+01"            , "{:.3e}"          ,  12.3456789   );
    checkFormat(ut,  " 1.235e+01"           , "{:>10.3e}"       ,  12.3456789   );
    checkFormat(ut,  "01.235e+01"           , "{:>010.3e}"      ,  12.3456789   );
    checkFormat(ut,  "001.23e+01"           , "{:>010.2e}"      ,  12.3456789   );
    checkFormat(ut,  "-01.23e+01"           , "{:>010.2e}"      , -12.3456789   );
    checkFormat(ut,  "-1.235e+01"           , "{:>010.3e}"      , -12.3456789   );
    checkFormat(ut,  "-1.2346e+01"          , "{:>010.4e}"      , -12.3456789   );

    checkFormat(ut,  "1.234568E+01"         , "{:E}"            ,  12.3456789   );
    checkFormat(ut,  "1.235E+01"            , "{:.3E}"          ,  12.3456789   );
    checkFormat(ut,  " 1.235E+01"           , "{:>10.3E}"       ,  12.3456789   );
    checkFormat(ut,  "01.235E+01"           , "{:>010.3E}"      ,  12.3456789   );
    checkFormat(ut,  "001.23E+01"           , "{:>010.2E}"      ,  12.3456789   );
    checkFormat(ut,  "-01.23E+01"           , "{:>010.2E}"      , -12.3456789   );
    checkFormat(ut,  "-1.235E+01"           , "{:>010.3E}"      , -12.3456789   );
    checkFormat(ut,  "-1.2346E+01"          , "{:>010.4E}"      , -12.3456789   );


    //-------  Nan,Inf, -0.0 -------
    checkFormat(ut,  "NAN"                  , "{:E}"            ,  nan("") );
    checkFormat(ut,  "INF"                  , "{:E}"            ,  std::numeric_limits<double>::infinity() );
    checkFormat(ut,  "-INF"                 , "{:E}"            , -std::numeric_limits<double>::infinity() );
    checkFormat(ut,  "0.0E+00"              , "{:E}"            ,  0.0 );
    checkFormat(ut,  "0.0E+00"              , "{:E}"            , -0.0 ); // negative zero
    checkFormat(ut,  "nan"                  , "{:+e}"           ,  nan("") );
    checkFormat(ut,  "+inf"                 , "{:+e}"           ,  std::numeric_limits<double>::infinity() );
    checkFormat(ut,  "-inf"                 , "{:+e}"           , -std::numeric_limits<double>::infinity() );
    checkFormat(ut,  "+0.0e+00"             , "{:+e}"           ,  0.0 );
    checkFormat(ut,  "+0.0e+00"             , "{:+e}"           , -0.0 ); // negative zero


    // -------------------------------- FixedPoint -------------------------------------------
    checkFormat(ut,  "12.345000"            , "{:f}"            ,  12.345         );
    checkFormat(ut,  "12.345679"            , "{:f}"            ,  12.3456789123  );
    checkFormat(ut,  "-12.345000"           , "{:f}"            , -12.345         );
    checkFormat(ut,  "-12.345679"           , "{:f}"            , -12.3456789123  );
    checkFormat(ut,  "12.345000"            , "{:F}"            ,  12.345         );
    checkFormat(ut,  "12.345679"            , "{:F}"            ,  12.3456789123  );
    checkFormat(ut,  "-12.345000"           , "{:F}"            , -12.345         );
    checkFormat(ut,  "-12.345679"           , "{:F}"            , -12.3456789123  );

    checkFormat(ut,  "12.345000"            , "{:#f}"            ,  12.345         );
    checkFormat(ut,  "12.345679"            , "{:#f}"            ,  12.3456789123  );
    checkFormat(ut,  "-12.345000"           , "{:#f}"            , -12.345         );
    checkFormat(ut,  "-12.345679"           , "{:#f}"            , -12.3456789123  );
    checkFormat(ut,  "12.345000"            , "{:#F}"            ,  12.345         );
    checkFormat(ut,  "12.345679"            , "{:#F}"            ,  12.3456789123  );
    checkFormat(ut,  "-12.345000"           , "{:#F}"            , -12.345         );
    checkFormat(ut,  "-12.345679"           , "{:#F}"            , -12.3456789123  );


    //-------  Nan,Inf, -0.0 -------
    checkFormat(ut,  "nan"                  , "{:f}"            ,  nan("") );
    checkFormat(ut,  "inf"                  , "{:f}"            ,  std::numeric_limits<double>::infinity() );
    checkFormat(ut,  "-inf"                 , "{:f}"            , -std::numeric_limits<double>::infinity() );
    checkFormat(ut,  "0.000000"             , "{:f}"            ,  0.0 );
    checkFormat(ut,  "0.000000"             , "{:f}"            , -0.0 ); // negative zero
    checkFormat(ut,  "NAN"                  , "{:+F}"           ,  nan("") );
    checkFormat(ut,  "+INF"                 , "{:+F}"           ,  std::numeric_limits<double>::infinity() );
    checkFormat(ut,  "-INF"                 , "{:+F}"           , -std::numeric_limits<double>::infinity() );
    checkFormat(ut,  "+0.000000"            , "{:+F}"           ,  0.0 );
    checkFormat(ut,  "+0.000000"            , "{:+F}"           , -0.0 ); // negative zero

    checkFormat(ut,  "nan"                  , "{:#f}"            ,  nan("") );
    checkFormat(ut,  "inf"                  , "{:#f}"            ,  std::numeric_limits<double>::infinity() );
    checkFormat(ut,  "-inf"                 , "{:#f}"            , -std::numeric_limits<double>::infinity() );
    checkFormat(ut,  "0.000000"             , "{:#f}"            ,  0.0 );
    checkFormat(ut,  "0.000000"             , "{:#f}"            , -0.0 ); // negative zero
    checkFormat(ut,  "NAN"                  , "{:#+F}"           ,  nan("") );
    checkFormat(ut,  "+INF"                 , "{:#+F}"           ,  std::numeric_limits<double>::infinity() );
    checkFormat(ut,  "-INF"                 , "{:#+F}"           , -std::numeric_limits<double>::infinity() );
    checkFormat(ut,  "+0.000000"            , "{:#+F}"           ,  0.0 );
    checkFormat(ut,  "+0.000000"            , "{:#+F}"           , -0.0 ); // negative zero


    checkFormat(ut,              "   0.0"       , "{:06,.5}"            ,  0.0         );
    checkFormat(ut,             "    0.0"       , "{:07,.5}"            ,  0.0         );
    checkFormat(ut,            "    00.0"       , "{:08,.5}"            ,  0.0         );
    checkFormat(ut,           "    000.0"       , "{:09,.5}"            ,  0.0         );
    checkFormat(ut,          "     000.0"       , "{:010,.5}"           ,  0.0         );
    checkFormat(ut,         "    0,000.0"       , "{:011,.5}"           ,  0.0         );
    checkFormat(ut,        "    00,000.0"       , "{:012,.5}"           ,  0.0         );
    checkFormat(ut,       "    000,000.0"       , "{:013,.5}"           ,  0.0         );
    checkFormat(ut,      "     000,000.0"       , "{:014,.5}"           ,  0.0         );
    checkFormat(ut,     "    0,000,000.0"       , "{:015,.5}"           ,  0.0         );
    checkFormat(ut,    "    00,000,000.0"       , "{:016,.5}"           ,  0.0         );

    checkFormat(ut,             "0.00000"       ,  "{:06#,.5}"            ,  0.0         );
    checkFormat(ut,             "0.00000"       ,  "{:07#,.5}"            ,  0.0         );
    checkFormat(ut,            "00.00000"       ,  "{:08#,.5}"            ,  0.0         );
    checkFormat(ut,           "000.00000"       ,  "{:09#,.5}"            ,  0.0         );
    checkFormat(ut,          " 000.00000"       , "{:010#,.5}"           ,  0.0         );
    checkFormat(ut,         "0,000.00000"       , "{:011#,.5}"           ,  0.0         );
    checkFormat(ut,        "00,000.00000"       , "{:012#,.5}"           ,  0.0         );
    checkFormat(ut,       "000,000.00000"       , "{:013#,.5}"           ,  0.0         );
    checkFormat(ut,      " 000,000.00000"       , "{:014#,.5}"           ,  0.0         );
    checkFormat(ut,     "0,000,000.00000"       , "{:015#,.5}"           ,  0.0         );
    checkFormat(ut,    "00,000,000.00000"       , "{:016#,.5}"           ,  0.0         );

    checkFormat(ut,                 "0.0"       , "{:,.5}"              ,          0.0 );
    checkFormat(ut,                 "1.0"       , "{:,.5}"              ,          1.0 );
    checkFormat(ut,                "10.0"       , "{:,.5}"              ,         10.0 );
    checkFormat(ut,               "100.0"       , "{:,.5}"              ,        100.0 );
    checkFormat(ut,             "1,000.0"       , "{:,.5}"              ,       1000.0 );
    checkFormat(ut,            "10,000.0"       , "{:,.5}"              ,      10000.0 );
    checkFormat(ut,           "100,000.0"       , "{:,.5}"              ,     100000.0 );
    checkFormat(ut,         "1,000,000.0"       , "{:,.5}"              ,    1000000.0 );
    checkFormat(ut,        "10,000,000.0"       , "{:,.5}"              ,   10000000.0 );
    checkFormat(ut,       "100,000,000.0"       , "{:,.5}"              ,  100000000.0 );

    checkFormat(ut,             "0.00000"       ,  "{:#05,.5}"           ,  0.0         );
    checkFormat(ut,             "0.00000"       ,  "{:#06,.5}"           ,  0.0         );
    checkFormat(ut,             "0.00000"       ,  "{:#07,.5}"           ,  0.0         );
    checkFormat(ut,            "00.00000"       ,  "{:#08,.5}"           ,  0.0         );
    checkFormat(ut,           "000.00000"       ,  "{:#09,.5}"           ,  0.0         );
    checkFormat(ut,          " 000.00000"       , "{:#010,.5}"           ,  0.0         );
    checkFormat(ut,         "0,000.00000"       , "{:#011,.5}"           ,  0.0         );
    checkFormat(ut,        "00,000.00000"       , "{:#012,.5}"           ,  0.0         );
    checkFormat(ut,       "000,000.00000"       , "{:#013,.5}"           ,  0.0         );
    checkFormat(ut,      " 000,000.00000"       , "{:#014,.5}"           ,  0.0         );
    checkFormat(ut,     "0,000,000.00000"       , "{:#015,.5}"           ,  0.0         );
    checkFormat(ut,    "00,000,000.00000"       , "{:#016,.5}"           ,  0.0         );

    checkFormat(ut,           "1.00000"         , "{:#5,.5}"            ,  1.0         );
    checkFormat(ut,           "1.00000"         , "{:#6,.5}"            ,  1.0         );
    checkFormat(ut,           "1.00000"         , "{:#7,.5}"            ,  1.0         );
    checkFormat(ut,          " 1.00000"         , "{:#8,.5}"            ,  1.0         );
    checkFormat(ut,         "  1.00000"         , "{:#9,.5}"            ,  1.0         );
    checkFormat(ut,        "   1.00000"         , "{:#10,.5}"           ,  1.0         );
    checkFormat(ut,       "    1.00000"         , "{:#11,.5}"           ,  1.0         );
    checkFormat(ut,      "     1.00000"         , "{:#12,.5}"           ,  1.0         );
    checkFormat(ut,     "      1.00000"         , "{:#13,.5}"           ,  1.0         );
    checkFormat(ut,    "       1.00000"         , "{:#14,.5}"           ,  1.0         );
    checkFormat(ut,   "        1.00000"         , "{:#15,.5}"           ,  1.0         );
    checkFormat(ut,  "         1.00000"         , "{:#16,.5}"           ,  1.0         );

    checkFormat(ut,           "1.00000"         , "{:#05,.5}"            ,  1.0         );
    checkFormat(ut,           "1.00000"         , "{:#06,.5}"            ,  1.0         );
    checkFormat(ut,           "1.00000"         , "{:#07,.5}"            ,  1.0         );
    checkFormat(ut,          "01.00000"         , "{:#08,.5}"            ,  1.0         );
    checkFormat(ut,         "001.00000"         , "{:#09,.5}"            ,  1.0         );
    checkFormat(ut,        " 001.00000"         , "{:#010,.5}"           ,  1.0         );
    checkFormat(ut,       "0,001.00000"         , "{:#011,.5}"           ,  1.0         );
    checkFormat(ut,      "00,001.00000"         , "{:#012,.5}"           ,  1.0         );
    checkFormat(ut,     "000,001.00000"         , "{:#013,.5}"           ,  1.0         );
    checkFormat(ut,    " 000,001.00000"         , "{:#014,.5}"           ,  1.0         );
    checkFormat(ut,   "0,000,001.00000"         , "{:#015,.5}"           ,  1.0         );
    checkFormat(ut,  "00,000,001.00000"         , "{:#016,.5}"           ,  1.0         );

    checkFormat(ut,            "0.00000"        , "{:#,.5}"              ,          0.0 );
    checkFormat(ut,            "1.00000"        , "{:#,.5}"              ,          1.0 );
    checkFormat(ut,           "10.00000"        , "{:#,.5}"              ,         10.0 );
    checkFormat(ut,          "100.00000"        , "{:#,.5}"              ,        100.0 );
    checkFormat(ut,        "1,000.00000"        , "{:#,.5}"              ,       1000.0 );
    checkFormat(ut,       "10,000.00000"        , "{:#,.5}"              ,      10000.0 );
    checkFormat(ut,      "100,000.00000"        , "{:#,.5}"              ,     100000.0 );
    checkFormat(ut,    "1,000,000.00000"        , "{:#,.5}"              ,    1000000.0 );
    checkFormat(ut,   "10,000,000.00000"        , "{:#,.5}"              ,   10000000.0 );
    checkFormat(ut,  "100,000,000.00000"        , "{:#,.5}"              ,  100000000.0 );

    // percentage
    checkFormat(ut,   "0.000000%"          , "{:%}"                ,          0.0       );
    checkFormat(ut,   "0.000000%"          , "{:#%}"                ,          0.0       );

    checkFormat(ut,       "0.00%"          , "{:.2%}"              ,          0.0       );
    checkFormat(ut,       "0.00%"          , "{:#.2%}"             ,          0.0       );
    checkFormat(ut,      "45.37%"          , "{:.2%}"              ,          0.4537    );
    checkFormat(ut,      "45%"             , "{:.0%}"              ,          0.4537    );
    checkFormat(ut,      "45.%"            , "{:#.0%}"             ,          0.4537    );

    UT_PRINT( "ALib Format Tests Python Style: Done" );

    testFormatter->Release();
}

UT_CLASS_END

} //namespace

#endif // !defined(ALIB_UT_SELECT) || defined(ALIB_UT_STRINGS)
