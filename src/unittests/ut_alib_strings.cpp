// #################################################################################################
//  aworx - Unit Tests
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alox.hpp"
#include "alib/strings/util/tokenizer.hpp"
#include "alib/strings/numberformat.hpp"
#include "alib/compatibility/std_string.hpp"

#include <iostream>
#include <sstream>
#include <iomanip>


#include "aworx_unittests.hpp"


#define TESTCLASSNAME       CPP_ALib_Strings


using namespace std;
using namespace aworx;

namespace tstn
{
    class MyString
    {
        private:
            const aworx::character* theString= ASTR("This is my string!");

        public:
            constexpr MyString() {}
            inline     bool                IsNulled   () const { return false; }
            constexpr  const aworx::character* GetMyBuffer() const { return               theString;   }
            inline     integer             GetMyLength() const { return aworx::lib::strings::CString<aworx::character>::Length( theString ); }
    };

}

namespace aworx { namespace lib { namespace strings {

    template<> struct   T_String<tstn::MyString,character> : public std::true_type
    {
        static inline const character* Buffer( const tstn::MyString& src ) { return  src.GetMyBuffer(); }
        static inline integer      Length( const tstn::MyString& src ) { return  src.GetMyLength(); }
    };

    template<> struct T_Apply<tstn::MyString, character> : public std::true_type
    {
        static inline integer Apply( AString& target, const tstn::MyString& src )
        {
            if ( src.IsNulled() )
                return -1;
            target.Append<false>( src.GetMyBuffer(), static_cast<integer>( src.GetMyLength() ) );
            return static_cast<integer>( src.GetMyLength() );
        }
    };

    template<> struct T_Apply<aworx::lib::time::DateTime, character> : public std::true_type
    {
        static inline integer Apply( AString& target, const aworx::lib::time::DateTime& ticks )
        {
            time::CalendarDateTime calendarTime;
            calendarTime.Set( ticks, Timezone::UTC );
            calendarTime.Format(ASTR("yyyy-MM-dd HH:mm"), target );
            return 16;
        }
    };

}}} // namespace [aworx::lib::strings]


namespace ut_aworx {

UT_CLASS()

void testParam            ( AWorxUnitTesting& ut, const character* exp, const String& as )
{
    #if ALIB_DEBUG_STRINGS
        ALIB_STRING_DBG_CHK(&as);
    #endif
    String32 asTemp;
    asTemp._(as);
    UT_EQ( exp, String(asTemp)  );
    UT_TRUE( as.Equals(exp) );
}

void testParamTerminatable( AWorxUnitTesting& ut, const character* exp, const TString& as )
{
    #if ALIB_DEBUG_STRINGS
        ALIB_STRING_DBG_CHK(&as);
    #endif
    String32 asTemp;
    asTemp._(as);
    UT_EQ( exp, String(asTemp)  );
    UT_TRUE( as.Equals(exp) );
}

void testParamPreallocatedString( AWorxUnitTesting& ut, const character* exp, const String64& as )
{
    #if ALIB_DEBUG_STRINGS
        ALIB_STRING_DBG_CHK(&as);
    #endif
    String32 asTemp;
    asTemp._(as);
    UT_EQ( exp, String(asTemp)  );
    UT_TRUE( as.Equals(exp) );
}

void testParamSubstring( AWorxUnitTesting& ut, const character* exp, const Substring& as )
{
    #if ALIB_DEBUG_STRINGS
        ALIB_STRING_DBG_CHK(&as);
    #endif
    String32 asTemp;
    asTemp._(as);
    UT_EQ( exp, String(asTemp)  );
    UT_TRUE( as.Equals(exp) );
}


//--------------------------------------------------------------------------------------------------
//--- Explicit constructors
//--------------------------------------------------------------------------------------------------
UT_METHOD( ConstructorsExplicit )
{
    UT_INIT();

    // before we begin...
    static_assert( std::is_nothrow_move_constructible<String>                 ::value, "String has to be move constructable with no assertions");
    static_assert( std::is_nothrow_move_constructible<Substring>              ::value, "Substring has to be move constructable with no assertions");
    static_assert( std::is_nothrow_move_constructible<TString>                ::value, "TString has to be move constructable with no assertions");
    static_assert( std::is_nothrow_move_constructible<AString>                ::value, "AString has to be move constructable with no assertions");


          character              testAChar=            '@';
    const character              testConstAChar=       '@';
    const character*             testConstACharP=      ASTR("TEST");
          character*             testACharP=           const_cast<character*>( testConstACharP );

    std::basic_string<character> testStdString        (testConstACharP);
    String                   testString               (testConstACharP);
    AString                  testAString              (testConstACharP);
    SLiteral<1>              testStringLiteral        (ASTR("1"));
    Substring                testSubstring            =String( ASTR("@TEST@") ).Substring<false>(1,4);
    String32                 testPreallocatedString       ( testConstACharP );
    tstn::MyString           testMyString;

    const std::basic_string<character>  testConstStdString   (testConstACharP);
    const String             testConstString                 (testConstACharP);
    const AString            testConstAString                (testConstACharP);
    const SLiteral<1>        testConstStringLiteral(ASTR("1"));
    const Substring          testConstSubstring     =String( ASTR("@TEST@") ).Substring<false>(1,4);
    const String32           testConstPreallocatedString  ( testConstACharP );
    const tstn::MyString     testConstMyString;

    const character* myStringBuf= testMyString.GetMyBuffer();

    // this has to assert:
    // { String     as( 42  );  }

    // String
    { String    as( ASTR("TEST")               ) ;testParam ( ut,testConstACharP,as ) ;}
    { String    as( testConstACharP            ) ;testParam ( ut,testConstACharP,as ) ;}
    { String    as( testACharP                 ) ;testParam ( ut,testConstACharP,as ) ;}

    { String    as( testString                 ) ;testParam ( ut,testConstACharP,as ) ;}
    { String    as( testStringLiteral          ) ;testParam ( ut,ASTR("1"),as       ) ;}
    { String    as( testSubstring              ) ;testParam ( ut,testConstACharP,as ) ;}
    { String    as( testAString                ) ;testParam ( ut,testConstACharP,as ) ;}
    { String    as( testPreallocatedString     ) ;testParam ( ut,testConstACharP,as ) ;}
    { String    as( testStdString              ) ;testParam ( ut,testConstACharP,as ) ;}
    { String    as( testMyString               ) ;testParam ( ut,myStringBuf,as     ) ;}

    { String    as( testConstString            ) ;testParam ( ut,testConstACharP,as ) ;}
    { String    as( testConstStringLiteral     ) ;testParam ( ut,ASTR("1"),as       ) ;}
    { String    as( testConstSubstring         ) ;testParam ( ut,testConstACharP,as ) ;}
    { String    as( testConstAString           ) ;testParam ( ut,testConstACharP,as ) ;}
    { String    as( testConstPreallocatedString) ;testParam ( ut,testConstACharP,as ) ;}
    { String    as( testConstStdString         ) ;testParam ( ut,testConstACharP,as ) ;}
    { String    as( testConstMyString          ) ;testParam ( ut,myStringBuf,as     ) ;}

    { String    as(&testString                 ) ;testParam ( ut,testConstACharP,as ) ;}
    { String    as(&testStringLiteral          ) ;testParam ( ut,ASTR("1"),as       ) ;}
    { String    as(&testSubstring              ) ;testParam ( ut,testConstACharP,as ) ;}
    { String    as(&testAString                ) ;testParam ( ut,testConstACharP,as ) ;}
    { String    as(&testPreallocatedString     ) ;testParam ( ut,testConstACharP,as ) ;}
    { String    as(&testStdString              ) ;testParam ( ut,testConstACharP,as ) ;}
    { String    as(&testMyString               ) ;testParam ( ut,myStringBuf,as     ) ;}

    { String    as(&testConstString            ) ;testParam ( ut,testConstACharP,as ) ;}
    { String    as(&testConstStringLiteral     ) ;testParam ( ut,ASTR("1"),as       ) ;}
    { String    as(&testConstSubstring         ) ;testParam ( ut,testConstACharP,as ) ;}
    { String    as(&testConstAString           ) ;testParam ( ut,testConstACharP,as ) ;}
    { String    as(&testConstPreallocatedString) ;testParam ( ut,testConstACharP,as ) ;}
    { String    as(&testConstStdString         ) ;testParam ( ut,testConstACharP,as ) ;}
    { String    as(&testConstMyString          ) ;testParam ( ut,myStringBuf,as     ) ;}

    // TString (no Substrings!  )
    { TString as( ASTR("TEST")                      ) ; testParam( ut, testConstACharP, as ) ; }
    { TString as( testConstACharP              ) ; testParam( ut, testConstACharP, as ) ; }
    { TString as( testACharP                   ) ; testParam( ut, testConstACharP, as ) ; }

    { TString as( testString                  ) ; testParam( ut, testConstACharP, as ) ; }
    { TString as( testStringLiteral           ) ; testParam( ut, ASTR("1")      , as ) ; }
    //{ TString as( testSubstring             ) ; testParam( ut, testConstACharP, as ) ; }
    { TString as( testAString                 ) ; testParam( ut, testConstACharP, as ) ; }
    { TString as( testPreallocatedString      ) ; testParam( ut, testConstACharP, as ) ; }
    { TString as( testStdString               ) ; testParam( ut, testConstACharP, as ) ; }
    { TString as( testMyString                ) ; testParam( ut, myStringBuf    , as ) ; }

    { TString as( testConstString             ) ; testParam( ut, testConstACharP, as ) ; }
    { TString as( testConstStringLiteral      ) ; testParam( ut, ASTR("1")      , as ) ; }
    //{ TString as( testConstSubstring        ) ; testParam( ut, testConstACharP, as ) ; }
    { TString as( testConstAString            ) ; testParam( ut, testConstACharP, as ) ; }
    { TString as( testConstPreallocatedString ) ; testParam( ut, testConstACharP, as ) ; }
    { TString as( testConstStdString          ) ; testParam( ut, testConstACharP, as ) ; }
    { TString as( testConstMyString           ) ; testParam( ut, myStringBuf    , as ) ; }

    { TString as(&testString                  ) ; testParam( ut, testConstACharP, as ) ; }
    { TString as(&testStringLiteral           ) ; testParam( ut, ASTR("1")      , as ) ; }
    //{ TString as(&testString                ) ; testParam( ut, testConstACharP, as ) ; }
    { TString as(&testAString                 ) ; testParam( ut, testConstACharP, as ) ; }
    { TString as(&testPreallocatedString      ) ; testParam( ut, testConstACharP, as ) ; }
    { TString as(&testStdString               ) ; testParam( ut, testConstACharP, as ) ; }
    { TString as(&testMyString                ) ; testParam( ut, myStringBuf    , as ) ; }

    { TString as(&testConstString             ) ; testParam( ut, testConstACharP, as ) ; }
    { TString as(&testConstStringLiteral      ) ; testParam( ut, ASTR("1")      , as ) ; }
    //{ TString as(&testConstSubstring        ) ; testParam( ut, testConstACharP, as ) ; }
    { TString as(&testConstAString            ) ; testParam( ut, testConstACharP, as ) ; }
    { TString as(&testConstPreallocatedString ) ; testParam( ut, testConstACharP, as ) ; }
    { TString as(&testConstStdString          ) ; testParam( ut, testConstACharP, as ) ; }
    { TString as(&testConstMyString           ) ; testParam( ut, myStringBuf    , as ) ; }

    // AString
    { AString   as(  "TEST"                         ) ; testParam( ut, testConstACharP, as         ) ; }
    { AString   as( L"TEST"                         ) ; testParam( ut, testConstACharP, as         ) ; }
    { AString   as( testACharP                      ) ; testParam( ut, testConstACharP, as         ) ; }
    { AString   as( testConstACharP                 ) ; testParam( ut, testConstACharP, as         ) ; }
    { AString   as( testAChar                        ) ; testParam( ut, ASTR("@"),      as         ) ; }
    { AString   as( testString                      ) ; testParam( ut, testConstACharP, as         ) ; }
    { AString   as( testStringLiteral               ) ; testParam( ut, ASTR("1")      , as         ) ; }
    { AString   as( testSubstring                   ) ; testParam( ut, testConstACharP, as         ) ; }
    { AString   as( testAString                     ) ; testParam( ut, testConstACharP, as         ) ; }
    { AString   as( testPreallocatedString          ) ; testParam( ut, testConstACharP, as         ) ; }
    { AString   as( testStdString                   ) ; testParam( ut, testConstACharP, as         ) ; }
    { AString   as( testMyString                    ) ; testParam( ut, myStringBuf    , as         ) ; }

    { AString   as( testConstAChar                   ) ; testParam( ut, ASTR("@"),      as         ) ; }
    { AString   as( testConstString                 ) ; testParam( ut, testConstACharP, as         ) ; }
    { AString   as( testConstStringLiteral          ) ; testParam( ut, ASTR("1")      , as         ) ; }
    { AString   as( testConstSubstring              ) ; testParam( ut, testConstACharP, as         ) ; }
    { AString   as( testConstAString                ) ; testParam( ut, testConstACharP, as         ) ; }
    { AString   as( testConstPreallocatedString     ) ; testParam( ut, testConstACharP, as         ) ; }
    { AString   as( testConstStdString              ) ; testParam( ut, testConstACharP, as         ) ; }
    { AString   as( testConstMyString               ) ; testParam( ut, myStringBuf    , as         ) ; }

    { AString   as(&testString                      ) ; testParam( ut, testConstACharP, as         ) ; }
    { AString   as(&testStringLiteral               ) ; testParam( ut, ASTR("1")      , as         ) ; }
    { AString   as(&testSubstring                   ) ; testParam( ut, testConstACharP, as         ) ; }
    { AString   as(&testAString                     ) ; testParam( ut, testConstACharP, as         ) ; }
    { AString   as(&testPreallocatedString          ) ; testParam( ut, testConstACharP, as         ) ; }
    { AString   as(&testStdString                   ) ; testParam( ut, testConstACharP, as         ) ; }
    { AString   as(&testMyString                    ) ; testParam( ut, myStringBuf    , as         ) ; }

    { AString   as(&testConstString                 ) ; testParam( ut, testConstACharP, as         ) ; }
    { AString   as(&testConstStringLiteral          ) ; testParam( ut, ASTR("1")      , as         ) ; }
    { AString   as(&testConstSubstring              ) ; testParam( ut, testConstACharP, as         ) ; }
    { AString   as(&testConstAString                ) ; testParam( ut, testConstACharP, as         ) ; }
    { AString   as(&testConstPreallocatedString     ) ; testParam( ut, testConstACharP, as         ) ; }
    { AString   as(&testConstStdString              ) ; testParam( ut, testConstACharP, as         ) ; }
    { AString   as(&testConstMyString               ) ; testParam( ut, myStringBuf    , as         ) ; }

    // PreallocatedString
    { String64      as(  "TEST"                     ) ; testParam( ut, testConstACharP, as         ) ; }
    { String64      as( L"TEST"                     ) ; testParam( ut, testConstACharP, as         ) ; }
    { String64      as( testACharP                   ) ; testParam( ut, testConstACharP,as         ) ; }
    { String64      as( testConstACharP              ) ; testParam( ut, testConstACharP,as         ) ; }
    { String64      as( testAChar                    ) ; testParam( ut, ASTR("@"),      as         ) ; }
    { String64      as( testString                  ) ; testParam( ut, testConstACharP, as         ) ; }
    { String64      as( testStringLiteral           ) ; testParam( ut, ASTR("1")      , as         ) ; }
    { String64      as( testSubstring               ) ; testParam( ut, testConstACharP, as         ) ; }
    { String64      as( testAString                 ) ; testParam( ut, testConstACharP, as         ) ; }
    { String64      as( testPreallocatedString      ) ; testParam( ut, testConstACharP, as         ) ; }
    { String64      as( testStdString               ) ; testParam( ut, testConstACharP, as         ) ; }
    { String64      as( testMyString                ) ; testParam( ut, myStringBuf    , as         ) ; }

    { String64      as( testConstAChar               ) ; testParam( ut, ASTR("@"),      as         ) ; }
    { String64      as( testConstString             ) ; testParam( ut, testConstACharP, as         ) ; }
    { String64      as( testConstStringLiteral      ) ; testParam( ut, ASTR("1")      , as         ) ; }
    { String64      as( testConstSubstring          ) ; testParam( ut, testConstACharP, as         ) ; }
    { String64      as( testConstAString            ) ; testParam( ut, testConstACharP, as         ) ; }
    { String64      as( testConstPreallocatedString ) ; testParam( ut, testConstACharP, as         ) ; }
    { String64      as( testConstStdString          ) ; testParam( ut, testConstACharP, as         ) ; }
    { String64      as( testConstMyString           ) ; testParam( ut, myStringBuf    , as         ) ; }

    { String64      as(&testString                  ) ; testParam( ut, testConstACharP, as         ) ; }
    { String64      as(&testStringLiteral           ) ; testParam( ut, ASTR("1")      , as         ) ; }
    { String64      as(&testSubstring               ) ; testParam( ut, testConstACharP, as         ) ; }
    { String64      as(&testAString                 ) ; testParam( ut, testConstACharP, as         ) ; }
    { String64      as(&testPreallocatedString      ) ; testParam( ut, testConstACharP, as         ) ; }
    { String64      as(&testStdString               ) ; testParam( ut, testConstACharP, as         ) ; }
    { String64      as(&testMyString                ) ; testParam( ut, myStringBuf    , as         ) ; }

    { String64      as(&testConstString             ) ; testParam( ut, testConstACharP, as         ) ; }
    { String64      as(&testConstStringLiteral      ) ; testParam( ut, ASTR("1")      , as         ) ; }
    { String64      as(&testConstSubstring          ) ; testParam( ut, testConstACharP, as         ) ; }
    { String64      as(&testConstAString            ) ; testParam( ut, testConstACharP, as         ) ; }
    { String64      as(&testConstPreallocatedString ) ; testParam( ut, testConstACharP, as         ) ; }
    { String64      as(&testConstStdString          ) ; testParam( ut, testConstACharP, as         ) ; }
    { String64      as(&testConstMyString           ) ; testParam( ut, myStringBuf    , as         ) ; }

    // Substring
    { Substring as( ASTR("TEST")                 ) ; testParam( ut, testConstACharP, as ) ; }
    { Substring as( testConstACharP              ) ; testParam( ut, testConstACharP, as ) ; }
    { Substring as( testACharP                   ) ; testParam( ut, testConstACharP, as ) ; }

    { Substring as( testString                  ) ; testParam( ut, testConstACharP, as ) ; }
    { Substring as( testStringLiteral           ) ; testParam( ut, ASTR("1")      , as ) ; }
    { Substring as( testSubstring               ) ; testParam( ut, testConstACharP, as ) ; }
    { Substring as( testAString                 ) ; testParam( ut, testConstACharP, as ) ; }
    { Substring as( testPreallocatedString      ) ; testParam( ut, testConstACharP, as ) ; }
    { Substring as( testStdString               ) ; testParam( ut, testConstACharP, as ) ; }
    { Substring as( testMyString                ) ; testParam( ut, myStringBuf   , as ) ; }

    { Substring as( testConstString             ) ; testParam( ut, testConstACharP, as ) ; }
    { Substring as( testConstStringLiteral      ) ; testParam( ut, ASTR("1")      , as ) ; }
    { Substring as( testConstSubstring          ) ; testParam( ut, testConstACharP, as ) ; }
    { Substring as( testConstAString            ) ; testParam( ut, testConstACharP, as ) ; }
    { Substring as( testConstPreallocatedString ) ; testParam( ut, testConstACharP, as ) ; }
    { Substring as( testConstStdString          ) ; testParam( ut, testConstACharP, as ) ; }
    { Substring as( testConstMyString           ) ; testParam( ut, myStringBuf    , as ) ; }

    { Substring as(&testString                  ) ; testParam( ut, testConstACharP, as ) ; }
    { Substring as(&testStringLiteral           ) ; testParam( ut, ASTR("1")      , as ) ; }
    { Substring as(&testSubstring               ) ; testParam( ut, testConstACharP, as ) ; }
    { Substring as(&testAString                 ) ; testParam( ut, testConstACharP, as ) ; }
    { Substring as(&testPreallocatedString      ) ; testParam( ut, testConstACharP, as ) ; }
    { Substring as(&testStdString               ) ; testParam( ut, testConstACharP, as ) ; }
    { Substring as(&testMyString                ) ; testParam( ut, myStringBuf    , as ) ; }

    { Substring as(&testConstString             ) ; testParam( ut, testConstACharP, as ) ; }
    { Substring as(&testConstStringLiteral      ) ; testParam( ut, ASTR("1")      , as ) ; }
    { Substring as(&testConstSubstring          ) ; testParam( ut, testConstACharP, as ) ; }
    { Substring as(&testConstAString            ) ; testParam( ut, testConstACharP, as ) ; }
    { Substring as(&testConstPreallocatedString ) ; testParam( ut, testConstACharP, as ) ; }
    { Substring as(&testConstStdString          ) ; testParam( ut, testConstACharP, as ) ; }
    { Substring as(&testConstMyString           ) ; testParam( ut, myStringBuf    , as ) ; }
}


UT_METHOD( ConstructorsImplicit )
{
    UT_INIT();

   const character*             testConstACharP= ASTR("TEST");
   character*                   testACharP=    const_cast<character*>( testConstACharP );
   std::basic_string<character> testStdString           ( testConstACharP ) ;
   AString                  testAString                 ( testConstACharP ) ;
   SLiteral<1>              testStringLiteral           ( ASTR("1")       ) ;
   Substring                testSubstring               ( testConstACharP ) ;
   String32                 testPreallocatedString      ( testConstACharP ) ;
   tstn::MyString           testMyString;

   const std::basic_string<character>   testConstStdString          ( testConstACharP ) ;
   const AString            testConstAString            ( testConstACharP ) ;
   const SLiteral<1>        testConstStringLiteral      ( ASTR("1")       ) ;
   const Substring          testConstSubstring          ( testConstACharP ) ;
   const String32           testConstPreallocatedString ( testConstACharP ) ;
   const tstn::MyString     testConstMyString;

   const character* myStringBuf= testMyString.GetMyBuffer    (            ) ;

    // implicit String
    {
        testParam( ut, testConstACharP,    ASTR("TEST")                 ) ;
        testParam( ut, testConstACharP,    testConstACharP              ) ;
        testParam( ut, testConstACharP,    testACharP                   ) ;

        testParam( ut, ASTR("1"),          testStringLiteral            ) ;
        testParam( ut, testConstACharP,    testSubstring                ) ;
        testParam( ut, testConstACharP,    testAString                  ) ;
        testParam( ut, testConstACharP,    testPreallocatedString       ) ;
        testParam( ut, testConstACharP,    testStdString                ) ;
        testParam( ut, myStringBuf   ,     testMyString                 ) ;

        testParam( ut, ASTR("1"),          testConstStringLiteral       ) ;
        testParam( ut, testConstACharP,    testConstSubstring           ) ;
        testParam( ut, testConstACharP,    testConstAString             ) ;
        testParam( ut, testConstACharP,    testConstPreallocatedString  ) ;
        testParam( ut, myStringBuf   ,     testConstMyString            ) ;

        testParam( ut, ASTR("1"),          &testStringLiteral           ) ;
        testParam( ut, testConstACharP,    &testSubstring               ) ;
        testParam( ut, testConstACharP,    &testAString                 ) ;
        testParam( ut, testConstACharP,    &testPreallocatedString      ) ;
        testParam( ut, testConstACharP,    &testStdString               ) ;
        testParam( ut, myStringBuf   ,     &testMyString                ) ;

        testParam( ut, ASTR("1"),          &testConstStringLiteral      ) ;
        testParam( ut, testConstACharP,    &testConstSubstring          ) ;
        testParam( ut, testConstACharP,    &testConstAString            ) ;
        testParam( ut, testConstACharP,    &testConstPreallocatedString ) ;
        testParam( ut, testConstACharP,    &testConstStdString          ) ;
        testParam( ut, myStringBuf   ,     &testConstMyString           ) ;
    }

    // implicit SubString
    {
       testParamSubstring( ut, testConstACharP,   ASTR("TEST")                                                             );
        testParamSubstring( ut, testConstACharP,    testConstACharP              );
        testParamSubstring( ut, testConstACharP,    testACharP                   );

        testParamSubstring( ut, ASTR("1"),          testStringLiteral            );
        testParamSubstring( ut, testConstACharP,    testSubstring                );
        testParamSubstring( ut, testConstACharP,    testAString                  );
        testParamSubstring( ut, testConstACharP,    testPreallocatedString       );
        testParamSubstring( ut, testConstACharP,    testStdString                );
        testParamSubstring( ut, myStringBuf   ,     testMyString                 );

        testParamSubstring( ut, ASTR("1"),          testConstStringLiteral       );
        testParamSubstring( ut, testConstACharP,    testConstSubstring           );
        testParamSubstring( ut, testConstACharP,    testConstAString             );
        testParamSubstring( ut, testConstACharP,    testConstPreallocatedString  );
        testParamSubstring( ut, myStringBuf   ,     testConstMyString            );

        testParamSubstring( ut, ASTR("1"),          &testStringLiteral           );
        testParamSubstring( ut, testConstACharP,    &testSubstring               );
        testParamSubstring( ut, testConstACharP,    &testAString                 );
        testParamSubstring( ut, testConstACharP,    &testPreallocatedString      );
        testParamSubstring( ut, testConstACharP,    &testStdString               );
        testParamSubstring( ut, myStringBuf   ,     &testMyString                );

        testParamSubstring( ut, ASTR("1"),          &testConstStringLiteral      );
        testParamSubstring( ut, testConstACharP,    &testConstSubstring          );
        testParamSubstring( ut, testConstACharP,    &testConstAString            );
        testParamSubstring( ut, testConstACharP,    &testConstPreallocatedString );
        testParamSubstring( ut, testConstACharP,    &testConstStdString          );
        testParamSubstring( ut, myStringBuf   , &testConstMyString    );
    }

    // implicit TString
    {
       testParamTerminatable( ut, testConstACharP,     ASTR("TEST")          );
       testParamTerminatable( ut, testConstACharP,     testConstACharP       );
       testParamTerminatable( ut, testConstACharP,     testACharP            );

       testParamTerminatable( ut, ASTR("1"),           testStringLiteral    );
       //Substrings are not allowed!  testParamTerminatable( ut, testConstACharP,     testSubstring        );
       testParamTerminatable( ut, testConstACharP,     testAString          );
       testParamTerminatable( ut, testConstACharP,     testPreallocatedString       );
       testParamTerminatable( ut, testConstACharP,     testStdString        );
       testParamTerminatable( ut, myStringBuf    ,     testMyString         );

       testParamTerminatable( ut, ASTR("1"),           testConstStringLiteral);
       //Substrings are not allowed!  testParamTerminatable( ut, testConstACharP,     testConstSubstring   );
       testParamTerminatable( ut, testConstACharP,     testConstAString     );
       testParamTerminatable( ut, testConstACharP,     testConstPreallocatedString  );
       testParamTerminatable( ut, testConstACharP,     testConstStdString   );
       testParamTerminatable( ut, myStringBuf    ,     testConstMyString    );

       testParamTerminatable( ut, ASTR("1"),           &testStringLiteral    );
       //Substrings are not allowed!  testParamTerminatable( ut, testConstACharP,    &testSubstring        );
       testParamTerminatable( ut, testConstACharP,     &testAString          );
       testParamTerminatable( ut, testConstACharP,     &testPreallocatedString       );
       testParamTerminatable( ut, testConstACharP,     &testStdString        );
       testParamTerminatable( ut, myStringBuf    ,     &testMyString         );

       testParamTerminatable( ut, ASTR("1"),           &testConstStringLiteral);
       testParamTerminatable( ut, testConstACharP,     &testConstAString     );
       //Substrings are not allowed!  testParamTerminatable( ut, testConstACharP,    &testConstSubstring   );
       testParamTerminatable( ut, testConstACharP,     &testConstPreallocatedString  );
       testParamTerminatable( ut, testConstACharP,     &testConstStdString   );
       testParamTerminatable( ut, myStringBuf    ,     &testConstMyString    );
    }

}

//--------------------------------------------------------------------------------------------------
//--- Assignment
//--------------------------------------------------------------------------------------------------
UT_METHOD( Assignment )
{
    UT_INIT();


          character                     testAChar=            '@';
    const character                     testConstAChar=       '@';
    const character*                    testConstACharP=      ASTR("TEST");
          character*                    testACharP=           const_cast<character*>( testConstACharP );

    std::basic_string<character>        testStdString        (testConstACharP);
    AString                             testAString          (testConstACharP);
    SLiteral<1>                         testStringLiteral    (ASTR("1"));
    Substring                           testSubstring        =String( ASTR("@TEST@") ).Substring<false>(1,4);
    String32                            testPreallocatedString       ( testConstACharP );
    tstn::MyString                      testMyString;

    const std::basic_string<character>  testConstStdString   (testConstACharP);
    const AString                       testConstAString     (testConstACharP);
    const SLiteral<1>                   testConstStringLiteral(ASTR("1"));
    const Substring                     testConstSubstring   =String( ASTR("@TEST@") ).Substring<false>(1,4);
    const String32                      testConstPreallocatedString  ( testConstACharP );
    const tstn::MyString                testConstMyString;

    const character* myStringBuf= testMyString.GetMyBuffer();

    // String
    { String        as=  ASTR("TEST")                ; testParam( ut, testConstACharP, as) ; }
    { String        as=  testConstACharP             ; testParam( ut, testConstACharP, as) ; }
    { String        as=  testACharP                  ; testParam( ut, testConstACharP, as) ; }

    { String        as=  testStringLiteral           ; testParam( ut, ASTR("1")      , as) ; }
    { String        as=  testSubstring               ; testParam( ut, testConstACharP, as) ; }
    { String        as=  testAString                 ; testParam( ut, testConstACharP, as) ; }
    { String        as=  testPreallocatedString      ; testParam( ut, testConstACharP, as) ; }
    { String        as=  testStdString               ; testParam( ut, testConstACharP, as) ; }
    { String        as=  testMyString                ; testParam( ut, myStringBuf    , as) ; }

    { String        as=  testConstStringLiteral      ; testParam( ut, ASTR("1")      , as) ; }
    { String        as=  testConstSubstring          ; testParam( ut, testConstACharP, as) ; }
    { String        as=  testConstAString            ; testParam( ut, testConstACharP, as) ; }
    { String        as=  testConstPreallocatedString ; testParam( ut, testConstACharP, as) ; }
    { String        as=  testConstStdString          ; testParam( ut, testConstACharP, as) ; }
    { String        as=  testConstMyString           ; testParam( ut, myStringBuf    , as) ; }

    { String        as= &testStringLiteral           ; testParam( ut, ASTR("1")      , as) ; }
    { String        as= &testSubstring               ; testParam( ut, testConstACharP, as) ; }
    { String        as= &testAString                 ; testParam( ut, testConstACharP, as) ; }
    { String        as= &testPreallocatedString      ; testParam( ut, testConstACharP, as) ; }
    { String        as= &testStdString               ; testParam( ut, testConstACharP, as) ; }
    { String        as= &testMyString                ; testParam( ut, myStringBuf    , as) ; }

    { String        as= &testConstStringLiteral      ; testParam( ut, ASTR("1")      , as) ; }
    { String        as= &testConstSubstring          ; testParam( ut, testConstACharP, as) ; }
    { String        as= &testConstAString            ; testParam( ut, testConstACharP, as) ; }
    { String        as= &testConstPreallocatedString ; testParam( ut, testConstACharP, as) ; }
    { String        as= &testConstStdString          ; testParam( ut, testConstACharP, as) ; }
    { String        as= &testConstMyString           ; testParam( ut, myStringBuf    , as) ; }

    { String    as; as=  ASTR("TEST")                ; testParam( ut, testConstACharP, as) ; }
    { String    as; as=  testConstACharP             ; testParam( ut, testConstACharP, as) ; }
    { String    as; as=  testACharP                  ; testParam( ut, testConstACharP, as) ; }

    { String    as; as=  testStringLiteral           ; testParam( ut, ASTR("1")      , as) ; }
    { String    as; as=  testSubstring               ; testParam( ut, testConstACharP, as) ; }
    { String    as; as=  testAString                 ; testParam( ut, testConstACharP, as) ; }
    { String    as; as=  testPreallocatedString      ; testParam( ut, testConstACharP, as) ; }
    { String    as; as=  testStdString               ; testParam( ut, testConstACharP, as) ; }
    { String    as; as=  testMyString                ; testParam( ut, myStringBuf    , as) ; }

    { String    as; as=  testConstStringLiteral      ; testParam( ut, ASTR("1")      , as) ; }
    { String    as; as=  testConstSubstring          ; testParam( ut, testConstACharP, as) ; }
    { String    as; as=  testConstAString            ; testParam( ut, testConstACharP, as) ; }
    { String    as; as=  testConstPreallocatedString ; testParam( ut, testConstACharP, as) ; }
    { String    as; as=  testConstStdString          ; testParam( ut, testConstACharP, as) ; }
    { String    as; as=  testConstMyString           ; testParam( ut, myStringBuf    , as) ; }

    { String    as; as= &testStringLiteral           ; testParam( ut, ASTR("1")      , as) ; }
    { String    as; as= &testSubstring               ; testParam( ut, testConstACharP, as) ; }
    { String    as; as= &testAString                 ; testParam( ut, testConstACharP, as) ; }
    { String    as; as= &testPreallocatedString      ; testParam( ut, testConstACharP, as) ; }
    { String    as; as= &testStdString               ; testParam( ut, testConstACharP, as) ; }
    { String    as; as= &testMyString                ; testParam( ut, myStringBuf    , as) ; }

    { String    as; as= &testConstStringLiteral      ; testParam( ut, ASTR("1")      , as) ; }
    { String    as; as= &testConstSubstring          ; testParam( ut, testConstACharP, as) ; }
    { String    as; as= &testConstAString            ; testParam( ut, testConstACharP, as) ; }
    { String    as; as= &testConstPreallocatedString ; testParam( ut, testConstACharP, as) ; }
    { String    as; as= &testConstStdString          ; testParam( ut, testConstACharP, as) ; }
    { String    as; as= &testConstMyString           ; testParam( ut, myStringBuf    , as) ; }


    // Substring
    { Substring   as=  ASTR("TEST")                      ; testParam( ut, testConstACharP, as) ; }
    { Substring          as=  testConstACharP            ; testParam( ut, testConstACharP, as) ; }
    { Substring          as=  testACharP                 ; testParam( ut, testConstACharP, as) ; }

    { Substring          as=  testStringLiteral          ; testParam( ut, ASTR("1")      , as) ; }
    { Substring          as=  testSubstring              ; testParam( ut, testConstACharP, as) ; }
    { Substring          as=  testAString                ; testParam( ut, testConstACharP, as) ; }
    { Substring          as=  testPreallocatedString     ; testParam( ut, testConstACharP, as) ; }
    { Substring          as=  testStdString              ; testParam( ut, testConstACharP, as) ; }
    { Substring          as=  testMyString               ; testParam( ut, myStringBuf    , as) ; }

    { Substring          as=  testConstStringLiteral     ; testParam( ut, ASTR("1")      , as) ; }
    { Substring          as=  testConstSubstring         ; testParam( ut, testConstACharP, as) ; }
    { Substring          as=  testConstAString           ; testParam( ut, testConstACharP, as) ; }
    { Substring          as=  testConstPreallocatedString; testParam( ut, testConstACharP, as) ; }
    { Substring          as=  testConstStdString         ; testParam( ut, testConstACharP, as) ; }
    { Substring          as=  testConstMyString          ; testParam( ut, myStringBuf    , as) ; }

    { Substring          as= &testStringLiteral          ; testParam( ut, ASTR("1")      , as) ; }
    { Substring          as= &testSubstring              ; testParam( ut, testConstACharP, as) ; }
    { Substring          as= &testAString                ; testParam( ut, testConstACharP, as) ; }
    { Substring          as= &testPreallocatedString     ; testParam( ut, testConstACharP, as) ; }
    { Substring          as= &testStdString              ; testParam( ut, testConstACharP, as) ; }
    { Substring          as= &testMyString               ; testParam( ut, myStringBuf    , as) ; }

    { Substring          as= &testConstStringLiteral     ; testParam( ut, ASTR("1")      , as) ; }
    { Substring          as= &testConstSubstring         ; testParam( ut, testConstACharP, as) ; }
    { Substring          as= &testConstAString           ; testParam( ut, testConstACharP, as) ; }
    { Substring          as= &testConstPreallocatedString; testParam( ut, testConstACharP, as) ; }
    { Substring          as= &testConstStdString         ; testParam( ut, testConstACharP, as) ; }
    { Substring          as= &testConstMyString          ; testParam( ut, myStringBuf    , as) ; }

    { Substring     as; as=  ASTR("TEST")                ; testParam( ut, testConstACharP, as) ; }
    { Substring     as; as=  testConstACharP             ; testParam( ut, testConstACharP, as) ; }
    { Substring     as; as=  testACharP                  ; testParam( ut, testConstACharP, as) ; }

    { Substring     as; as=  testStringLiteral           ; testParam( ut, ASTR("1")      , as) ; }
    { Substring     as; as=  testSubstring               ; testParam( ut, testConstACharP, as) ; }
    { Substring     as; as=  testAString                 ; testParam( ut, testConstACharP, as) ; }
    { Substring     as; as=  testPreallocatedString      ; testParam( ut, testConstACharP, as) ; }
    { Substring     as; as=  testStdString               ; testParam( ut, testConstACharP, as) ; }
    { Substring     as; as=  testMyString                ; testParam( ut, myStringBuf    , as) ; }

    { Substring     as; as=  testConstStringLiteral      ; testParam( ut, ASTR("1")      , as) ; }
    { Substring     as; as=  testConstSubstring          ; testParam( ut, testConstACharP, as) ; }
    { Substring     as; as=  testConstAString            ; testParam( ut, testConstACharP, as) ; }
    { Substring     as; as=  testConstPreallocatedString ; testParam( ut, testConstACharP, as) ; }
    { Substring     as; as=  testConstStdString          ; testParam( ut, testConstACharP, as) ; }
    { Substring     as; as=  testConstMyString           ; testParam( ut, myStringBuf    , as) ; }

    { Substring     as; as= &testStringLiteral           ; testParam( ut, ASTR("1")      , as) ; }
    { Substring     as; as= &testSubstring               ; testParam( ut, testConstACharP, as) ; }
    { Substring     as; as= &testAString                 ; testParam( ut, testConstACharP, as) ; }
    { Substring     as; as= &testPreallocatedString      ; testParam( ut, testConstACharP, as) ; }
    { Substring     as; as= &testStdString               ; testParam( ut, testConstACharP, as) ; }
    { Substring     as; as= &testMyString                ; testParam( ut, myStringBuf    , as) ; }

    { Substring     as; as= &testConstStringLiteral      ; testParam( ut, ASTR("1")      , as) ; }
    { Substring     as; as= &testConstSubstring          ; testParam( ut, testConstACharP, as) ; }
    { Substring     as; as= &testConstAString            ; testParam( ut, testConstACharP, as) ; }
    { Substring     as; as= &testConstPreallocatedString ; testParam( ut, testConstACharP, as) ; }
    { Substring     as; as= &testConstStdString          ; testParam( ut, testConstACharP, as) ; }
    { Substring     as; as= &testConstMyString           ; testParam( ut, myStringBuf    , as) ; }

    // TString (no Substrings!)
    { TString     as=  ASTR("TEST")                ; testParam( ut, testConstACharP, as) ; }
    { TString     as=  testConstACharP             ; testParam( ut, testConstACharP, as) ; }
    { TString     as=  testACharP                  ; testParam( ut, testConstACharP, as) ; }

    { TString     as=  testStringLiteral           ; testParam( ut, ASTR("1")      , as) ; }
    //{ TString   as=  testSubstring               ; testParam( ut, testConstACharP, as) ; }
    { TString     as=  testAString                 ; testParam( ut, testConstACharP, as) ; }
    { TString     as=  testPreallocatedString      ; testParam( ut, testConstACharP, as) ; }
    { TString     as=  testStdString               ; testParam( ut, testConstACharP, as) ; }
    { TString     as=  testMyString                ; testParam( ut, myStringBuf    , as) ; }

    { TString     as=  testConstStringLiteral      ; testParam( ut, ASTR("1")      , as) ; }
    //{ TString   as=  testConstSubstring          ; testParam( ut, testConstACharP, as) ; }
    { TString     as=  testConstAString            ; testParam( ut, testConstACharP, as) ; }
    { TString     as=  testConstPreallocatedString ; testParam( ut, testConstACharP, as) ; }
    { TString     as=  testConstStdString          ; testParam( ut, testConstACharP, as) ; }
    { TString     as=  testConstMyString           ; testParam( ut, myStringBuf    , as) ; }

    { TString     as= &testStringLiteral           ; testParam( ut, ASTR("1")      , as) ; }
    //{ TString   as= &testSubstring               ; testParam( ut, testConstACharP, as) ; }
    { TString     as= &testAString                 ; testParam( ut, testConstACharP, as) ; }
    { TString     as= &testPreallocatedString      ; testParam( ut, testConstACharP, as) ; }
    { TString     as= &testStdString               ; testParam( ut, testConstACharP, as) ; }
    { TString     as= &testMyString                ; testParam( ut, myStringBuf    , as) ; }

    { TString     as= &testConstStringLiteral      ; testParam( ut, ASTR("1")      , as) ; }
    //{ TString   as= &testConstSubstring          ; testParam( ut, testConstACharP, as) ; }
    { TString     as= &testConstAString            ; testParam( ut, testConstACharP, as) ; }
    { TString     as= &testConstPreallocatedString ; testParam( ut, testConstACharP, as) ; }
    { TString     as= &testConstStdString          ; testParam( ut, testConstACharP, as) ; }
    { TString     as= &testConstMyString           ; testParam( ut, myStringBuf    , as) ; }

    { TString as; as=  ASTR("TEST")                ; testParam( ut, testConstACharP, as) ; }
    { TString as; as=  testConstACharP             ; testParam( ut, testConstACharP, as) ; }
    { TString as; as=  testACharP                  ; testParam( ut, testConstACharP, as) ; }

    { TString as; as=  testStringLiteral           ; testParam( ut, ASTR("1")      , as) ; }
    //{ TString as; as=testSubstring                 ; testParam( ut, testConstACharP, as) ; }
    { TString as; as=  testAString                 ; testParam( ut, testConstACharP, as) ; }
    { TString as; as=  testPreallocatedString      ; testParam( ut, testConstACharP, as) ; }
    { TString as; as=  testStdString               ; testParam( ut, testConstACharP, as) ; }
    { TString as; as=  testMyString                ; testParam( ut, myStringBuf    , as) ; }

    { TString as; as=  testConstStringLiteral      ; testParam( ut, ASTR("1")      , as) ; }
    //{ TString as; as=testConstSubstring            ; testParam( ut, testConstACharP, as) ; }
    { TString as; as=  testConstAString            ; testParam( ut, testConstACharP, as) ; }
    { TString as; as=  testConstPreallocatedString ; testParam( ut, testConstACharP, as) ; }
    { TString as; as=  testConstStdString          ; testParam( ut, testConstACharP, as) ; }
    { TString as; as=  testConstMyString           ; testParam( ut, myStringBuf    , as) ; }

    { TString as; as= &testStringLiteral           ; testParam( ut, ASTR("1")      , as) ; }
    //{ TString as; as=&testSubstring                ; testParam( ut, testConstACharP, as) ; }
    { TString as; as= &testAString                 ; testParam( ut, testConstACharP, as) ; }
    { TString as; as= &testPreallocatedString      ; testParam( ut, testConstACharP, as) ; }
    { TString as; as= &testStdString               ; testParam( ut, testConstACharP, as) ; }
    { TString as; as= &testMyString                ; testParam( ut, myStringBuf    , as) ; }

    { TString as; as= &testConstStringLiteral      ; testParam( ut, ASTR("1")      , as) ; }
    //{ TString as; as=&testConstSubstring           ; testParam( ut, testConstACharP, as) ; }
    { TString as; as= &testConstAString            ; testParam( ut, testConstACharP, as) ; }
    { TString as; as= &testConstPreallocatedString ; testParam( ut, testConstACharP, as) ; }
    { TString as; as= &testConstStdString          ; testParam( ut, testConstACharP, as) ; }
    { TString as; as= &testConstMyString           ; testParam( ut, myStringBuf    , as) ; }

    // AString

    // direct initialization not allowed!
    //{ AString            as=  "TEST"              ; testParam( ut, testConstACharP, as); }
    //...
    //...

    { AString        as; as=  "TEST"                      ; testParam( ut, testConstACharP, as); }
    { AString        as; as=  L"TEST"                     ; testParam( ut, testConstACharP, as); }
    { AString        as; as=  testConstACharP             ; testParam( ut, testConstACharP, as); }
    { AString        as; as=  testACharP                  ; testParam( ut, testConstACharP, as); }

    { AString        as; as=  testAChar                   ; testParam( ut, ASTR("@"),       as); }
    { AString        as; as=  testStringLiteral           ; testParam( ut, ASTR("1")      , as); }
    { AString        as; as=  testSubstring               ; testParam( ut, testConstACharP, as); }
    { AString        as; as=  testAString                 ; testParam( ut, testConstACharP, as); }
    { AString        as; as=  testPreallocatedString      ; testParam( ut, testConstACharP, as); }
    { AString        as; as=  testStdString               ; testParam( ut, testConstACharP, as); }
    { AString        as; as=  testMyString                ; testParam( ut, myStringBuf    , as); }

    { AString        as; as=  testConstAChar              ; testParam( ut, ASTR("@"),       as); }
    { AString        as; as=  testConstStringLiteral      ; testParam( ut, ASTR("1")      , as); }
    { AString        as; as=  testConstSubstring          ; testParam( ut, testConstACharP, as); }
    { AString        as; as=  testConstAString            ; testParam( ut, testConstACharP, as); }
    { AString        as; as=  testConstPreallocatedString ; testParam( ut, testConstACharP, as); }
    { AString        as; as=  testConstStdString          ; testParam( ut, testConstACharP, as); }
    { AString        as; as=  testConstMyString           ; testParam( ut, myStringBuf    , as); }

    { AString        as; as= &testStringLiteral           ; testParam( ut, ASTR("1")      , as); }
    { AString        as; as= &testSubstring               ; testParam( ut, testConstACharP, as); }
    { AString        as; as= &testAString                 ; testParam( ut, testConstACharP, as); }
    { AString        as; as= &testPreallocatedString      ; testParam( ut, testConstACharP, as); }
    { AString        as; as= &testStdString               ; testParam( ut, testConstACharP, as); }
    { AString        as; as= &testMyString                ; testParam( ut, myStringBuf    , as); }

    { AString        as; as= &testConstStringLiteral      ; testParam( ut, ASTR("1")      , as); }
    { AString        as; as= &testConstSubstring          ; testParam( ut, testConstACharP, as); }
    { AString        as; as= &testConstAString            ; testParam( ut, testConstACharP, as); }
    { AString        as; as= &testConstPreallocatedString ; testParam( ut, testConstACharP, as); }
    { AString        as; as= &testConstStdString          ; testParam( ut, testConstACharP, as); }
    { AString        as; as= &testConstMyString           ; testParam( ut, myStringBuf    , as); }

    // PreallocatedString
    // direct initialization not allowed!
    //{ String32             as=  "TEST"              ; testParam( ut, testConstACharP, as); }
    //...
    //...
    { String32 as; as=  "TEST"                      ; testParam( ut, testConstACharP, as); }
    { String32 as; as=  L"TEST"                     ; testParam( ut, testConstACharP, as); }
    { String32 as; as=  testConstACharP             ; testParam( ut, testConstACharP, as); }
    { String32 as; as=  testACharP                  ; testParam( ut, testConstACharP, as); }

    { String32 as; as=  testAChar                   ; testParam( ut, ASTR("@"),       as); }
    { String32 as; as=  testStringLiteral           ; testParam( ut, ASTR("1")      , as); }
    { String32 as; as=  testSubstring               ; testParam( ut, testConstACharP, as); }
    { String32 as; as=  testAString                 ; testParam( ut, testConstACharP, as); }
    { String32 as; as=  testPreallocatedString      ; testParam( ut, testConstACharP, as); }
    { String32 as; as=  testStdString               ; testParam( ut, testConstACharP, as); }
    { String32 as; as=  testMyString                ; testParam( ut, myStringBuf    , as); }

    { String32 as; as=  testConstAChar              ; testParam( ut, ASTR("@"),       as); }
    { String32 as; as=  testConstStringLiteral      ; testParam( ut, ASTR("1")      , as); }
    { String32 as; as=  testConstSubstring          ; testParam( ut, testConstACharP, as); }
    { String32 as; as=  testConstAString            ; testParam( ut, testConstACharP, as); }
    { String32 as; as=  testConstPreallocatedString ; testParam( ut, testConstACharP, as); }
    { String32 as; as=  testConstStdString          ; testParam( ut, testConstACharP, as); }
    { String32 as; as=  testConstMyString           ; testParam( ut, myStringBuf    , as); }

    { String32 as; as= &testStringLiteral           ; testParam( ut, ASTR("1")      , as); }
    { String32 as; as= &testSubstring               ; testParam( ut, testConstACharP, as); }
    { String32 as; as= &testAString                 ; testParam( ut, testConstACharP, as); }
    { String32 as; as= &testPreallocatedString      ; testParam( ut, testConstACharP, as); }
    { String32 as; as= &testStdString               ; testParam( ut, testConstACharP, as); }
    { String32 as; as= &testMyString                ; testParam( ut, myStringBuf    , as); }

    { String32 as; as= &testConstStringLiteral      ; testParam( ut, ASTR("1")      , as); }
    { String32 as; as= &testConstSubstring          ; testParam( ut, testConstACharP, as); }
    { String32 as; as= &testConstAString            ; testParam( ut, testConstACharP, as); }
    { String32 as; as= &testConstPreallocatedString ; testParam( ut, testConstACharP, as); }
    { String32 as; as= &testConstStdString          ; testParam( ut, testConstACharP, as); }
    { String32 as; as= &testConstMyString           ; testParam( ut, myStringBuf    , as); }

    // StringLiteral
    { SLiteral<4>        as=  ASTR("TEST")          ; testParam( ut, testConstACharP, as); }
}

//--------------------------------------------------------------------------------------------------
//--- Append and operator <<
//--------------------------------------------------------------------------------------------------
UT_METHOD( AppendAndAppendOperator )
{
    UT_INIT();

    const character*                    testConstACharP= ASTR("TEST");
    character*                          testACharP=       const_cast<character*>( testConstACharP );
    std::basic_string<character>        testStdString    (testConstACharP);
    AString                             testAString      (testConstACharP);
    SLiteral<1>                         testStringLiteral(ASTR("1"));
    Substring                           testSubstring    =String( ASTR("@TEST@") ).Substring<false>(1,4);
    String32                            testPreallocatedString  ( testConstACharP );
    tstn::MyString                      testMyString;

    const std::basic_string<character>  testConstStdString    (testConstACharP);
    const AString                       testConstAString      (testConstACharP);
    const SLiteral<1>                   testConstStringLiteral(ASTR("1"));
    const Substring                     testConstSubstring    =String( ASTR("@TEST@") ).Substring<false>(1,4);
    const String32                      testConstPreallocatedString  ( testConstACharP );
    const tstn::MyString                testConstMyString;

    const character* myStringBuf=       testMyString.GetMyBuffer();

    // these have to fail
    // { vector<int> x;             String as(x);}
    // { vector<int> x; AString as; as._ (x);}
    // { char c   ='a';             String as(c);}

    CalendarDateTime calendar;
    calendar.Day=     1;
    calendar.Month=   4;
    calendar.Year=    2011;
    calendar.Hour=    16;
    calendar.Minute=  00;
    calendar.Second=  01;
    DateTime       testTicks(calendar.Get(Timezone::UTC));
    const DateTime testConstTicks  ( testTicks );
    const character* ticksResult= ASTR("2011-04-01 16:00");

    // AString
    { AString as; as <<  ASTR("TEST")                 ; testParam( ut, testConstACharP, as) ; }
    { AString as; as <<  testConstACharP              ; testParam( ut, testConstACharP, as) ; }
    { AString as; as <<  testACharP                   ; testParam( ut, testConstACharP, as) ; }

    { AString as; as <<  testStringLiteral            ; testParam( ut, ASTR("1")      , as) ; }
    { AString as; as <<  testSubstring                ; testParam( ut, testConstACharP, as) ; }
    { AString as; as <<  testAString                  ; testParam( ut, testConstACharP, as) ; }
    { AString as; as <<  testPreallocatedString       ; testParam( ut, testConstACharP, as) ; }
    { AString as; as <<  testStdString                ; testParam( ut, testConstACharP, as) ; }
    { AString as; as <<  testMyString                 ; testParam( ut, myStringBuf    , as) ; }
    { AString as; as <<  testTicks                    ; testParam( ut, ticksResult ,    as) ; }

    { AString as; as <<  testConstStringLiteral       ; testParam( ut, ASTR("1")      , as) ; }
    { AString as; as <<  testConstSubstring           ; testParam( ut, testConstACharP, as) ; }
    { AString as; as <<  testConstAString             ; testParam( ut, testConstACharP, as) ; }
    { AString as; as <<  testConstPreallocatedString  ; testParam( ut, testConstACharP, as) ; }
    { AString as; as <<  testConstStdString           ; testParam( ut, testConstACharP, as) ; }
    { AString as; as <<  testConstMyString            ; testParam( ut, myStringBuf    , as) ; }
    { AString as; as <<  testConstTicks               ; testParam( ut, ticksResult,     as) ; }

    { AString as; as << &testStringLiteral            ; testParam( ut, ASTR("1")      , as) ; }
    { AString as; as << &testSubstring                ; testParam( ut, testConstACharP, as) ; }
    { AString as; as << &testAString                  ; testParam( ut, testConstACharP, as) ; }
    { AString as; as << &testPreallocatedString       ; testParam( ut, testConstACharP, as) ; }
    { AString as; as << &testStdString                ; testParam( ut, testConstACharP, as) ; }
    { AString as; as << &testMyString                 ; testParam( ut, myStringBuf    , as) ; }
    { AString as; as << &testTicks                    ; testParam( ut, ticksResult,     as) ; }

    { AString as; as << &testConstStringLiteral       ; testParam( ut, ASTR("1")      , as) ; }
    { AString as; as << &testConstSubstring           ; testParam( ut, testConstACharP, as) ; }
    { AString as; as << &testConstAString             ; testParam( ut, testConstACharP, as) ; }
    { AString as; as << &testConstPreallocatedString  ; testParam( ut, testConstACharP, as) ; }
    { AString as; as << &testConstStdString           ; testParam( ut, testConstACharP, as) ; }
    { AString as; as << &testConstMyString            ; testParam( ut, myStringBuf    , as) ; }
    { AString as; as << &testConstTicks               ; testParam( ut, ticksResult,    as) ; }

    { AString as; as._( ASTR("TEST")         )         ; testParam( ut, testConstACharP, as) ; }
    { AString as; as._( testConstACharP      )         ; testParam( ut, testConstACharP, as) ; }
    { AString as; as._( testACharP           )         ; testParam( ut, testConstACharP, as) ; }

    { AString as; as._( testStringLiteral   )         ; testParam( ut, ASTR("1")      , as) ; }
    { AString as; as._( testSubstring       )         ; testParam( ut, testConstACharP, as) ; }
    { AString as; as._( testAString         )         ; testParam( ut, testConstACharP, as) ; }
    { AString as; as._( testPreallocatedString      ) ; testParam( ut, testConstACharP, as) ; }
    { AString as; as._( testStdString       )         ; testParam( ut, testConstACharP, as) ; }
    { AString as; as._( testMyString        )         ; testParam( ut, myStringBuf    , as) ; }
    { AString as; as._( testTicks           )         ; testParam( ut, ticksResult,     as) ; }

    { AString as; as._( testConstStringLiteral  )     ; testParam( ut, ASTR("1")      , as) ; }
    { AString as; as._( testConstSubstring  )         ; testParam( ut, testConstACharP, as) ; }
    { AString as; as._( testConstAString    )         ; testParam( ut, testConstACharP, as) ; }
    { AString as; as._( testConstPreallocatedString ) ; testParam( ut, testConstACharP, as) ; }
    { AString as; as._( testConstStdString  )         ; testParam( ut, testConstACharP, as) ; }
    { AString as; as._( testConstMyString   )         ; testParam( ut, myStringBuf    , as) ; }
    { AString as; as._( testConstTicks      )         ; testParam( ut, ticksResult,     as) ; }

    { AString as; as._(&testStringLiteral   )         ; testParam( ut, ASTR("1")      , as) ; }
    { AString as; as._(&testSubstring       )         ; testParam( ut, testConstACharP, as) ; }
    { AString as; as._(&testAString         )         ; testParam( ut, testConstACharP, as) ; }
    { AString as; as._(&testPreallocatedString      ) ; testParam( ut, testConstACharP, as) ; }
    { AString as; as._(&testStdString       )         ; testParam( ut, testConstACharP, as) ; }
    { AString as; as._(&testMyString        )         ; testParam( ut, myStringBuf    , as) ; }
    { AString as; as._(&testTicks           )         ; testParam( ut, ticksResult,     as) ; }

    { AString as; as._(&testConstStringLiteral  )     ; testParam( ut, ASTR("1")      , as) ; }
    { AString as; as._(&testConstSubstring  )         ; testParam( ut, testConstACharP, as) ; }
    { AString as; as._(&testConstAString    )         ; testParam( ut, testConstACharP, as) ; }
    { AString as; as._(&testConstPreallocatedString ) ; testParam( ut, testConstACharP, as) ; }
    { AString as; as._(&testConstStdString  )         ; testParam( ut, testConstACharP, as) ; }
    { AString as; as._(&testConstMyString   )         ; testParam( ut, myStringBuf   , as) ; }
    { AString as; as._(&testConstTicks      )         ; testParam( ut, ticksResult,    as) ; }

    { AString as          ; as._<false>( ASTR("TEST")         )        ; testParam( ut, testConstACharP, as) ; }
    { AString as          ; as._<false>( testConstACharP      )        ; testParam( ut, testConstACharP, as) ; }
    { AString as          ; as._<false>( testACharP           )        ; testParam( ut, testConstACharP, as) ; }

    { AString as          ; as._<false>( testStringLiteral   )         ; testParam( ut, ASTR("1")      , as) ; }
    { AString as          ; as._<false>( testSubstring       )         ; testParam( ut, testConstACharP, as) ; }
    { AString as          ; as._<false>( testAString         )         ; testParam( ut, testConstACharP, as) ; }
    { AString as          ; as._<false>( testPreallocatedString      ) ; testParam( ut, testConstACharP, as) ; }
    { AString as          ; as._<false>( testStdString       )         ; testParam( ut, testConstACharP, as) ; }
    { AString as          ; as._<false>( testMyString        )         ; testParam( ut, myStringBuf    , as) ; }
    { AString as          ; as._<false>( testTicks           )         ; testParam( ut, ticksResult,     as) ; }

    { AString as          ; as._<false>( testConstStringLiteral  )     ; testParam( ut, ASTR("1")      , as) ; }
    { AString as          ; as._<false>( testConstSubstring  )         ; testParam( ut, testConstACharP, as) ; }
    { AString as          ; as._<false>( testConstAString    )         ; testParam( ut, testConstACharP, as) ; }
    { AString as          ; as._<false>( testConstPreallocatedString ) ; testParam( ut, testConstACharP, as) ; }
    { AString as          ; as._<false>( testConstStdString  )         ; testParam( ut, testConstACharP, as) ; }
    { AString as          ; as._<false>( testConstMyString   )         ; testParam( ut, myStringBuf    , as) ; }
    { AString as          ; as._<false>( testConstTicks      )         ; testParam( ut, ticksResult,     as) ; }

    { AString as          ; as._<false>(&testStringLiteral   )         ; testParam( ut, ASTR("1")      , as) ; }
    { AString as          ; as._<false>(&testSubstring       )         ; testParam( ut, testConstACharP, as) ; }
    { AString as          ; as._<false>(&testAString         )         ; testParam( ut, testConstACharP, as) ; }
    { AString as          ; as._<false>(&testPreallocatedString      ) ; testParam( ut, testConstACharP, as) ; }
    { AString as          ; as._<false>(&testStdString       )         ; testParam( ut, testConstACharP, as) ; }
    { AString as          ; as._<false>(&testMyString        )         ; testParam( ut, myStringBuf    , as) ; }
    { AString as          ; as._<false>(&testTicks           )         ; testParam( ut, ticksResult,     as) ; }

    { AString as          ; as._<false>(&testConstStringLiteral  )     ; testParam( ut, ASTR("1")      , as) ; }
    { AString as          ; as._<false>(&testConstSubstring  )         ; testParam( ut, testConstACharP, as) ; }
    { AString as          ; as._<false>(&testConstAString    )         ; testParam( ut, testConstACharP, as) ; }
    { AString as          ; as._<false>(&testConstPreallocatedString ) ; testParam( ut, testConstACharP, as) ; }
    { AString as          ; as._<false>(&testConstStdString  )         ; testParam( ut, testConstACharP, as) ; }
    { AString as          ; as._<false>(&testConstMyString   )         ; testParam( ut, myStringBuf    , as) ; }
    { AString as          ; as._<false>(&testConstTicks      )         ; testParam( ut, ticksResult,     as) ; }

    // PreallocatedString
    { String64    as      ; as <<  ASTR("TEST")                        ; testParam( ut, testConstACharP, as) ; }
    { String64    as      ; as <<  testConstACharP                     ; testParam( ut, testConstACharP, as) ; }
    { String64    as      ; as <<  testACharP                          ; testParam( ut, testConstACharP, as) ; }

    { String64    as      ; as <<  testStringLiteral                   ; testParam( ut, ASTR("1")      , as) ; }
    { String64    as      ; as <<  testSubstring                       ; testParam( ut, testConstACharP, as) ; }
    { String64    as      ; as <<  testAString                         ; testParam( ut, testConstACharP, as) ; }
    { String64    as      ; as <<  testPreallocatedString              ; testParam( ut, testConstACharP, as) ; }
    { String64    as      ; as <<  testStdString                       ; testParam( ut, testConstACharP, as) ; }
    { String64    as      ; as <<  testMyString                        ; testParam( ut, myStringBuf    , as) ; }

    { String64    as      ; as <<  testConstStringLiteral              ; testParam( ut, ASTR("1")      , as) ; }
    { String64    as      ; as <<  testConstSubstring                  ; testParam( ut, testConstACharP, as) ; }
    { String64    as      ; as <<  testConstAString                    ; testParam( ut, testConstACharP, as) ; }
    { String64    as      ; as <<  testConstPreallocatedString         ; testParam( ut, testConstACharP, as) ; }
    { String64    as      ; as <<  testConstStdString                  ; testParam( ut, testConstACharP, as) ; }
    { String64    as      ; as <<  testConstMyString                   ; testParam( ut, myStringBuf    , as) ; }

    { String64    as      ; as << &testStringLiteral                   ; testParam( ut, ASTR("1")      , as) ; }
    { String64    as      ; as << &testSubstring                       ; testParam( ut, testConstACharP, as) ; }
    { String64    as      ; as << &testAString                         ; testParam( ut, testConstACharP, as) ; }
    { String64    as      ; as << &testPreallocatedString              ; testParam( ut, testConstACharP, as) ; }
    { String64    as      ; as << &testStdString                       ; testParam( ut, testConstACharP, as) ; }
    { String64    as      ; as << &testMyString                        ; testParam( ut, myStringBuf    , as) ; }

    { String64    as      ; as << &testConstStringLiteral              ; testParam( ut, ASTR("1")      , as) ; }
    { String64    as      ; as << &testConstSubstring                  ; testParam( ut, testConstACharP, as) ; }
    { String64    as      ; as << &testConstAString                    ; testParam( ut, testConstACharP, as) ; }
    { String64    as      ; as << &testConstPreallocatedString         ; testParam( ut, testConstACharP, as) ; }
    { String64    as      ; as << &testConstStdString                  ; testParam( ut, testConstACharP, as) ; }
    { String64    as      ; as << &testConstMyString                   ; testParam( ut, myStringBuf    , as) ; }

    { String64    as      ; as._( ASTR("TEST")              )          ; testParam( ut, testConstACharP, as) ; }
    { String64    as      ; as._( testConstACharP      )               ; testParam( ut, testConstACharP, as) ; }
    { String64    as      ; as._( testACharP           )               ; testParam( ut, testConstACharP, as) ; }

    { String64    as      ; as._( testStringLiteral   )                ; testParam( ut, ASTR("1")      , as) ; }
    { String64    as      ; as._( testSubstring       )                ; testParam( ut, testConstACharP, as) ; }
    { String64    as      ; as._( testAString         )                ; testParam( ut, testConstACharP, as) ; }
    { String64    as      ; as._( testPreallocatedString      )        ; testParam( ut, testConstACharP, as) ; }
    { String64    as      ; as._( testStdString       )                ; testParam( ut, testConstACharP, as) ; }
    { String64    as      ; as._( testMyString        )                ; testParam( ut, myStringBuf    , as) ; }

    { String64    as      ; as._( testConstStringLiteral  )            ; testParam( ut, ASTR("1")      , as) ; }
    { String64    as      ; as._( testConstSubstring  )                ; testParam( ut, testConstACharP, as) ; }
    { String64    as      ; as._( testConstAString    )                ; testParam( ut, testConstACharP, as) ; }
    { String64    as      ; as._( testConstPreallocatedString )        ; testParam( ut, testConstACharP, as) ; }
    { String64    as      ; as._( testConstStdString  )                ; testParam( ut, testConstACharP, as) ; }
    { String64    as      ; as._( testConstMyString   )                ; testParam( ut, myStringBuf    , as) ; }

    { String64    as      ; as._(&testStringLiteral   )                ; testParam( ut, ASTR("1")      , as) ; }
    { String64    as      ; as._(&testSubstring       )                ; testParam( ut, testConstACharP, as) ; }
    { String64    as      ; as._(&testAString         )                ; testParam( ut, testConstACharP, as) ; }
    { String64    as      ; as._(&testPreallocatedString      )        ; testParam( ut, testConstACharP, as) ; }
    { String64    as      ; as._(&testStdString       )                ; testParam( ut, testConstACharP, as) ; }
    { String64    as      ; as._(&testMyString        )                ; testParam( ut, myStringBuf    , as) ; }

    { String64    as      ; as._(&testConstStringLiteral  )            ; testParam( ut, ASTR("1")      , as) ; }
    { String64    as      ; as._(&testConstSubstring  )                ; testParam( ut, testConstACharP, as) ; }
    { String64    as      ; as._(&testConstAString    )                ; testParam( ut, testConstACharP, as) ; }
    { String64    as      ; as._(&testConstPreallocatedString )        ; testParam( ut, testConstACharP, as) ; }
    { String64    as      ; as._(&testConstStdString  )                ; testParam( ut, testConstACharP, as) ; }
    { String64    as      ; as._(&testConstMyString   )                ; testParam( ut, myStringBuf    , as) ; }

}


//--------------------------------------------------------------------------------------------------
//--- Move constructors
//--------------------------------------------------------------------------------------------------
UT_METHOD( MoveConstructors )
{
    UT_INIT();

    lib::lang::Report::GetDefault().PushHaltFlags( false, false );
    UT_PRINT( "One or more buffer warnings should follow" );


    // this test is more for debug stepping to check if the right constructors are used
    {   AString as       ( AString ( ASTR("Anonymous")));          as._ (ASTR("x"));   }

    {   String16    as   ( String16( ASTR("123456789_12345") ));   as._ (ASTR("x"));   }
    {   String16    as   ( String16( ASTR("123456789_123456")));   as._ (ASTR("x"));   }

    {   String16    as   ( AString ( ASTR("123456789_12345") ));   as._ (ASTR("x"));   }
    {   String16    as   ( AString ( ASTR("123456789_123456")));   as._ (ASTR("x"));   }

    {   String16    vola ( ASTR("123456789_123456"));
        String16    as   ( std::move( vola ) );
                    as._ ( ASTR("x"));
    }

    {
        String16    vola( ASTR("123456789_123456"));
        String16    as; as= std::move ( vola);
                    as._(ASTR("x"));
    }
    lib::lang::Report::GetDefault().PopHaltFlags();

}
UT_CLASS_END

} //namespace
