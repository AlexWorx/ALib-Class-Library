// #################################################################################################
//  AWorx ALib Unit Tests
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"
#include "unittests/alib_test_selection.hpp"
#if ALIB_UT_STRINGS

#include "alib/alox.hpp"
#include "alib/strings/util/tokenizer.hpp"
#include "alib/strings/astring.hpp"
#include "alib/strings/numberformat.hpp"
#include "alib/compatibility/std_strings.hpp"
#if ALIB_TIME
#   include "alib/time/datetime.hpp"
#endif
#if ALIB_CAMP
#   include "alib/lang/system/calendar.hpp"
#endif

#include <iostream>
#include <sstream>
#include <iomanip>

#include "unittests/aworx_unittests.hpp"
#define TESTCLASSNAME       UT_Strings

using namespace std;
using namespace alib;

namespace tstn
{
    class MyString
    {
        private:
            const alib::character* theString= A_CHAR("This is MyString!");

        public:
            constexpr MyString() {}
                       bool             IsNulled   () const { return false; }
            constexpr  const character* GetMyBuffer() const { return                                                       theString;   }
                       integer          GetMyLength() const { return characters::Length( theString ); }
    };

}

namespace alib {

namespace characters {
    ALIB_CHARACTER_ARRAY( tstn::MyString, character, Implicit, NONE)
    ALIB_CHARACTER_ARRAY_IMPL_BUFFER( tstn::MyString, character, return                       src.GetMyBuffer  ();   )
    ALIB_CHARACTER_ARRAY_IMPL_LENGTH( tstn::MyString, character, return static_cast<integer>( src.GetMyLength() ); )
}

#if ALIB_CAMP
namespace strings {
    template<> struct T_Append<alib::time::DateTime, character, lang::HeapAllocator>
    {
        void operator()( AString& target, const alib::time::DateTime& ticks )
        {
            alib::CalendarDateTime calendarTime;
            calendarTime.Set( ticks, lang::Timezone::UTC );
            calendarTime.Format(A_CHAR("yyyy-MM-dd HH:mm"), target );
        }
    };

}
#endif

} // namespace [alib]

namespace ut_aworx {

#include "alib/lang/callerinfo_functions.hpp"

//--------------------------------------------------------------------------------------------------
//--- Test CompareOperators
//--------------------------------------------------------------------------------------------------
#if !defined(ALIB_UT_REDUCED_COMPILE_TIME)
namespace {
    template<typename TALibString>
    void checkComparisonN(AWorxUnitTesting& ut, TALibString& s)
    {
        UT_TRUE(s == A_NCHAR("ABC"))    UT_TRUE(A_NCHAR("ABC") == s)
            UT_FALSE(s != A_NCHAR("ABC"))    UT_FALSE(A_NCHAR("ABC") != s)
            UT_FALSE(s < A_NCHAR("ABC"))    UT_FALSE(A_NCHAR("ABC") < s)
            UT_TRUE(s <= A_NCHAR("ABC"))    UT_TRUE(A_NCHAR("ABC") <= s)
            UT_FALSE(s > A_NCHAR("ABC"))    UT_FALSE(A_NCHAR("ABC") > s)
            UT_TRUE(s >= A_NCHAR("ABC"))    UT_TRUE(A_NCHAR("ABC") >= s)

            UT_FALSE(s == A_NCHAR("ABX"))    UT_FALSE(A_NCHAR("ABX") == s)
            UT_TRUE(s != A_NCHAR("ABX"))    UT_TRUE(A_NCHAR("ABX") != s)
            UT_TRUE(s < A_NCHAR("ABX"))    UT_FALSE(A_NCHAR("ABX") < s)
            UT_TRUE(s <= A_NCHAR("ABX"))    UT_FALSE(A_NCHAR("ABX") <= s)
            UT_FALSE(s > A_NCHAR("ABX"))    UT_TRUE(A_NCHAR("ABX") > s)
            UT_FALSE(s >= A_NCHAR("ABX"))    UT_TRUE(A_NCHAR("ABX") >= s)
    }

    template<typename TALibString>
    void checkComparisonW(AWorxUnitTesting& ut, TALibString& s)
    {
        UT_TRUE(s == A_WCHAR("ABC"))    UT_TRUE(A_WCHAR("ABC") == s)
            UT_FALSE(s != A_WCHAR("ABC"))    UT_FALSE(A_WCHAR("ABC") != s)
            UT_FALSE(s < A_WCHAR("ABC"))    UT_FALSE(A_WCHAR("ABC") < s)
            UT_TRUE(s <= A_WCHAR("ABC"))    UT_TRUE(A_WCHAR("ABC") <= s)
            UT_FALSE(s > A_WCHAR("ABC"))    UT_FALSE(A_WCHAR("ABC") > s)
            UT_TRUE(s >= A_WCHAR("ABC"))    UT_TRUE(A_WCHAR("ABC") >= s)

            UT_FALSE(s == A_WCHAR("ABX"))    UT_FALSE(A_WCHAR("ABX") == s)
            UT_TRUE(s != A_WCHAR("ABX"))    UT_TRUE(A_WCHAR("ABX") != s)
            UT_TRUE(s < A_WCHAR("ABX"))    UT_FALSE(A_WCHAR("ABX") < s)
            UT_TRUE(s <= A_WCHAR("ABX"))    UT_FALSE(A_WCHAR("ABX") <= s)
            UT_FALSE(s > A_WCHAR("ABX"))    UT_TRUE(A_WCHAR("ABX") > s)
            UT_FALSE(s >= A_WCHAR("ABX"))    UT_TRUE(A_WCHAR("ABX") >= s)
    }

    template<typename TALibString>
    void checkComparisonX(AWorxUnitTesting& ut, TALibString& s)
    {
        UT_TRUE(s == A_XCHAR("ABC"))    UT_TRUE(A_XCHAR("ABC") == s)
            UT_FALSE(s != A_XCHAR("ABC"))    UT_FALSE(A_XCHAR("ABC") != s)
            UT_FALSE(s < A_XCHAR("ABC"))    UT_FALSE(A_XCHAR("ABC") < s)
            UT_TRUE(s <= A_XCHAR("ABC"))    UT_TRUE(A_XCHAR("ABC") <= s)
            UT_FALSE(s > A_XCHAR("ABC"))    UT_FALSE(A_XCHAR("ABC") > s)
            UT_TRUE(s >= A_XCHAR("ABC"))    UT_TRUE(A_XCHAR("ABC") >= s)

            UT_FALSE(s == A_XCHAR("ABX"))    UT_FALSE(A_XCHAR("ABX") == s)
            UT_TRUE(s != A_XCHAR("ABX"))    UT_TRUE(A_XCHAR("ABX") != s)
            UT_TRUE(s < A_XCHAR("ABX"))    UT_FALSE(A_XCHAR("ABX") < s)
            UT_TRUE(s <= A_XCHAR("ABX"))    UT_FALSE(A_XCHAR("ABX") <= s)
            UT_FALSE(s > A_XCHAR("ABX"))    UT_TRUE(A_XCHAR("ABX") > s)
            UT_FALSE(s >= A_XCHAR("ABX"))    UT_TRUE(A_XCHAR("ABX") >= s)
    }

    template<typename TALibString, typename TALibString2>
    void checkComparison(AWorxUnitTesting& ut, TALibString& s, TALibString2& c1, TALibString2& c2)
    {
        UT_TRUE(s == c1)    UT_TRUE(c1 == s)
            UT_FALSE(s != c1)    UT_FALSE(c1 != s)
            UT_FALSE(s < c1)    UT_FALSE(c1 < s)
            UT_TRUE(s <= c1)    UT_TRUE(c1 <= s)
            UT_FALSE(s > c1)    UT_FALSE(c1 > s)
            UT_TRUE(s >= c1)    UT_TRUE(c1 >= s)

            UT_FALSE(s == c2)    UT_FALSE(c2 == s)
            UT_TRUE(s != c2)    UT_TRUE(c2 != s)
            UT_TRUE(s < c2)    UT_FALSE(c2 < s)
            UT_TRUE(s <= c2)    UT_FALSE(c2 <= s)
            UT_FALSE(s > c2)    UT_TRUE(c2 > s)
            UT_FALSE(s >= c2)    UT_TRUE(c2 >= s)
    }
}//anonymous namespace
#endif //if !defined(ALIB_UT_REDUCED_COMPILE_TIME)


// #################################################################################################
// ####### String/CString construction from Implicit/Explicit/Mutable
// #################################################################################################


UT_CLASS

void testParam            ( AWorxUnitTesting& ut, const character* exp, const String& as )
{
    #if ALIB_DEBUG_STRINGS
        ALIB_STRING_DBG_CHK(&as)
    #endif
    String32 asTemp;
    asTemp._(as);
    UT_EQ( exp, String(asTemp)  )
    UT_TRUE( as.Equals(exp) )
}

void testParamTerminatable( AWorxUnitTesting& ut, const character* exp, const CString& as )
{
    #if ALIB_DEBUG_STRINGS
        ALIB_STRING_DBG_CHK(&as)
    #endif
    String32 asTemp;
    asTemp._(as);
    UT_EQ( exp, String(asTemp)  )
    UT_TRUE( as.Equals(exp) )
}

void testParamLocalString( AWorxUnitTesting& ut, const character* exp, const String64& as )
{
    #if ALIB_DEBUG_STRINGS
        ALIB_STRING_DBG_CHK(&as)
    #endif
    String32 asTemp;
    asTemp._(as);
    UT_EQ( exp, String(asTemp)  )
    UT_TRUE( as.Equals(exp) )
}

void testParamSubstring( AWorxUnitTesting& ut, const character* exp, const Substring& as )
{
    #if ALIB_DEBUG_STRINGS
        ALIB_STRING_DBG_CHK(&as)
    #endif
    String32 asTemp;
    asTemp._(as);
    UT_EQ( exp, String(asTemp)  )
    UT_TRUE( as.Equals(exp) )
}
#include "alib/lang/callerinfo_methods.hpp"

//--------------------------------------------------------------------------------------------------
//--- Explicit constructors
//--------------------------------------------------------------------------------------------------
UT_METHOD( ConstructorsExplicit )
{
    UT_INIT()

    // before we begin...
    static_assert( std::is_nothrow_move_constructible<String>                 ::value, "String has to be move constructable with no assertions");
    static_assert( std::is_nothrow_move_constructible<Substring>              ::value, "Substring has to be move constructable with no assertions");
    static_assert( std::is_nothrow_move_constructible<CString>                ::value, "CString has to be move constructable with no assertions");
    static_assert( std::is_nothrow_move_constructible<AString>                ::value, "AString has to be move constructable with no assertions");


          character              testAChar=            '@';
    const character              testConstAChar=       '@';
    const character*             testConstACharP=      A_CHAR("TEST");
          character*             testACharP=           const_cast<character*>( testConstACharP );

    std::basic_string<character> testStdString        (testConstACharP);
    String                   testString               (testConstACharP);
    AString                  testAString              (testConstACharP);
    character                testStringLiteral[2]     (A_CHAR("1"));
    Substring                testSubstring            =String( A_CHAR("@TEST@") ).Substring<NC>(1,4);
    String32                 testLocalString          ( testConstACharP );
    tstn::MyString           testMyString;

    const std::basic_string<character>  testConstStdString   (testConstACharP);
    const String             testConstString                 (testConstACharP);
    const AString            testConstAString                (testConstACharP);
    const character          testConstStringLiteral[2](A_CHAR("1"));
    const Substring          testConstSubstring     =String( A_CHAR("@TEST@") ).Substring<NC>(1,4);
    const String32           testConstLocalString  ( testConstACharP );
    const tstn::MyString     testConstMyString;

    const character* myStringBuf= testMyString.GetMyBuffer();

    // this has to assert:
    // { String     as( 42  );  }

    // String
    { String    st( A_CHAR("TEST")             ) ;testParam ( ut,testConstACharP,st ) ;}
    { String    st( testConstACharP            ) ;testParam ( ut,testConstACharP,st ) ;}
    { String    st( testACharP                 ) ;testParam ( ut,testConstACharP,st ) ;}

    { String    st( testString                 ) ;testParam ( ut,testConstACharP,st ) ;}
    { String    st( testStringLiteral          ) ;testParam ( ut,A_CHAR("1"),st       ) ;}
    { String    st( testSubstring              ) ;testParam ( ut,testConstACharP,st ) ;}
    { String    st( testAString                ) ;testParam ( ut,testConstACharP,st ) ;}
    { String    st( testLocalString            ) ;testParam ( ut,testConstACharP,st ) ;}
    { String    st( testStdString              ) ;testParam ( ut,testConstACharP,st ) ;}
    { String    st( testMyString               ) ;testParam ( ut,myStringBuf,st     ) ;}

    { String    st( testConstString            ) ;testParam ( ut,testConstACharP,st ) ;}
    { String    st( testConstStringLiteral     ) ;testParam ( ut,A_CHAR("1"),st       ) ;}
    { String    st( testConstSubstring         ) ;testParam ( ut,testConstACharP,st ) ;}
    { String    st( testConstAString           ) ;testParam ( ut,testConstACharP,st ) ;}
    { String    st( testConstLocalString       ) ;testParam ( ut,testConstACharP,st ) ;}
    { String    st( testConstStdString         ) ;testParam ( ut,testConstACharP,st ) ;}
    { String    st( testConstMyString          ) ;testParam ( ut,myStringBuf,st     ) ;}

    // CString (no Strings non-terminated strings  )
    { CString ts( A_CHAR("TEST")               ) ; testParam( ut, testConstACharP, ts ) ; }
    { CString ts( testConstACharP              ) ; testParam( ut, testConstACharP, ts ) ; }
    { CString ts( testACharP                   ) ; testParam( ut, testConstACharP, ts ) ; }

    { CString ts( testString                   ) ; testParam( ut, testConstACharP, ts ) ; }
    { CString ts( testStringLiteral            ) ; testParam( ut, A_CHAR("1")     , ts ) ; }
  //{ CString ts( testSubstring                ) ; testParam( ut, testConstACharP, ts ) ; }
    { CString ts( testAString                  ) ; testParam( ut, testConstACharP, ts ) ; }
    { CString ts( testLocalString              ) ; testParam( ut, testConstACharP, ts ) ; }
    { CString ts( testStdString                ) ; testParam( ut, testConstACharP, ts ) ; }
  //{ CString ts( testMyString                 ) ; testParam( ut, myStringBuf    , ts ) ; }

    { CString ts( testConstString              ) ; testParam( ut, testConstACharP, ts ) ; }
    { CString ts( testConstStringLiteral       ) ; testParam( ut, A_CHAR("1")     , ts ) ; }
  //{ CString ts( testConstSubstring           ) ; testParam( ut, testConstACharP, ts ) ; }
    { CString ts( testConstAString             ) ; testParam( ut, testConstACharP, ts ) ; }
    { CString ts( testConstLocalString         ) ; testParam( ut, testConstACharP, ts ) ; }
    { CString ts( testConstStdString           ) ; testParam( ut, testConstACharP, ts ) ; }
  //{ CString ts( testConstMyString            ) ; testParam( ut, myStringBuf    , ts ) ; }

    { CString ts(&testString                   ) ; testParam( ut, testConstACharP, ts ) ; }
  //{ CString ts(&testStringLiteral            ) ; testParam( ut, A_CHAR("1")     , ts ) ; }
    { CString ts(&testString                   ) ; testParam( ut, testConstACharP, ts ) ; }
    { CString ts(&testAString                  ) ; testParam( ut, testConstACharP, ts ) ; }
    { CString ts(&testLocalString              ) ; testParam( ut, testConstACharP, ts ) ; }
    { CString ts(&testStdString                ) ; testParam( ut, testConstACharP, ts ) ; }
  //{ CString ts(&testMyString                 ) ; testParam( ut, myStringBuf    , ts ) ; }

    { CString ts(&testConstString              ) ; testParam( ut, testConstACharP, ts ) ; }
  //{ CString ts(&testConstStringLiteral       ) ; testParam( ut, A_CHAR("1")     , ts ) ; }
  //{ CString ts(&testConstSubstring           ) ; testParam( ut, testConstACharP, ts ) ; }
    { CString ts(&testConstAString             ) ; testParam( ut, testConstACharP, ts ) ; }
    { CString ts(&testConstLocalString         ) ; testParam( ut, testConstACharP, ts ) ; }
    { CString ts(&testConstStdString           ) ; testParam( ut, testConstACharP, ts ) ; }
  //{ CString ts(&testConstMyString            ) ; testParam( ut, myStringBuf    , ts ) ; }

    // AString
    { AString   as(  "TEST"                         ) ; testParam( ut, testConstACharP, as         ) ; }
    { AString   as( L"TEST"                         ) ; testParam( ut, testConstACharP, as         ) ; }
  //{ AString   as( testACharP                      ) ; testParam( ut, testConstACharP, as         ) ; }
    { AString   as( (String(testACharP))            ) ; testParam( ut, testConstACharP, as         ) ; }
    { AString   as( testConstACharP                 ) ; testParam( ut, testConstACharP, as         ) ; }
    { AString   as( testAChar                       ) ; testParam( ut, A_CHAR("@"),      as         ) ; }
    { AString   as( testString                      ) ; testParam( ut, testConstACharP, as         ) ; }
    { AString   as( testStringLiteral               ) ; testParam( ut, A_CHAR("1")     , as         ) ; }
    { AString   as( testSubstring                   ) ; testParam( ut, testConstACharP, as         ) ; }
    { AString   as( testAString                     ) ; testParam( ut, testConstACharP, as         ) ; }
    { AString   as( testLocalString                 ) ; testParam( ut, testConstACharP, as         ) ; }
    { AString   as( testStdString                   ) ; testParam( ut, testConstACharP, as         ) ; }
    { AString   as( testMyString                    ) ; testParam( ut, myStringBuf    , as         ) ; }

    { AString   as( testConstAChar                  ) ; testParam( ut, A_CHAR("@"),      as         ) ; }
    { AString   as( testConstString                 ) ; testParam( ut, testConstACharP, as         ) ; }
    { AString   as( testConstStringLiteral          ) ; testParam( ut, A_CHAR("1")     , as         ) ; }
    { AString   as( testConstSubstring              ) ; testParam( ut, testConstACharP, as         ) ; }
    { AString   as( testConstAString                ) ; testParam( ut, testConstACharP, as         ) ; }
    { AString   as( testConstLocalString            ) ; testParam( ut, testConstACharP, as         ) ; }
    { AString   as( testConstStdString              ) ; testParam( ut, testConstACharP, as         ) ; }
    { AString   as( testConstMyString               ) ; testParam( ut, myStringBuf    , as         ) ; }

    // LocalString
    { String64      ps(  "TEST"                     ) ; testParam( ut, testConstACharP, ps         ) ; }
    { String64      ps( L"TEST"                     ) ; testParam( ut, testConstACharP, ps         ) ; }
  //{ String64      ps( testACharP                  ) ; testParam( ut, testConstACharP,ps          ) ; }
    { String64      ps( (String(testACharP))        ) ; testParam( ut, testConstACharP,ps          ) ; }
    { String64      ps( testConstACharP             ) ; testParam( ut, testConstACharP,ps          ) ; }
    { String64      ps( testAChar                   ) ; testParam( ut, A_CHAR("@"),      ps          ) ; }
    { String64      ps( testString                  ) ; testParam( ut, testConstACharP, ps         ) ; }
    { String64      ps( testStringLiteral           ) ; testParam( ut, A_CHAR("1")    , ps         ) ; }
    { String64      ps( testSubstring               ) ; testParam( ut, testConstACharP, ps         ) ; }
    { String64      ps( testAString                 ) ; testParam( ut, testConstACharP, ps         ) ; }
    { String64      ps( testLocalString             ) ; testParam( ut, testConstACharP, ps         ) ; }
    { String64      ps( testStdString               ) ; testParam( ut, testConstACharP, ps         ) ; }
    { String64      ps( testMyString                ) ; testParam( ut, myStringBuf    , ps         ) ; }

    { String64      ps( testConstAChar              ) ; testParam( ut, A_CHAR("@"),      ps          ) ; }
    { String64      ps( testConstString             ) ; testParam( ut, testConstACharP, ps         ) ; }
    { String64      ps( testConstStringLiteral      ) ; testParam( ut, A_CHAR("1")    , ps         ) ; }
    { String64      ps( testConstSubstring          ) ; testParam( ut, testConstACharP, ps         ) ; }
    { String64      ps( testConstAString            ) ; testParam( ut, testConstACharP, ps         ) ; }
    { String64      ps( testConstLocalString        ) ; testParam( ut, testConstACharP, ps         ) ; }
    { String64      ps( testConstStdString          ) ; testParam( ut, testConstACharP, ps         ) ; }
    { String64      ps( testConstMyString           ) ; testParam( ut, myStringBuf    , ps         ) ; }

    // Substring
    { Substring su( A_CHAR("TEST")              ) ; testParam( ut, testConstACharP, su ) ; }
    { Substring su( testConstACharP             ) ; testParam( ut, testConstACharP, su ) ; }
    { Substring su( testACharP                  ) ; testParam( ut, testConstACharP, su ) ; }

    { Substring su( testString                  ) ; testParam( ut, testConstACharP, su ) ; }
    { Substring su( testStringLiteral           ) ; testParam( ut, A_CHAR("1")    , su ) ; }
    { Substring su( testSubstring               ) ; testParam( ut, testConstACharP, su ) ; }
    { Substring su( testAString                 ) ; testParam( ut, testConstACharP, su ) ; }
    { Substring su( testLocalString             ) ; testParam( ut, testConstACharP, su ) ; }
    { Substring su( testStdString               ) ; testParam( ut, testConstACharP, su ) ; }
    { Substring su( testMyString                ) ; testParam( ut, myStringBuf    , su ) ; }

    { Substring su( testConstString             ) ; testParam( ut, testConstACharP, su ) ; }
    { Substring su( testConstStringLiteral      ) ; testParam( ut, A_CHAR("1")    , su ) ; }
    { Substring su( testConstSubstring          ) ; testParam( ut, testConstACharP, su ) ; }
    { Substring su( testConstAString            ) ; testParam( ut, testConstACharP, su ) ; }
    { Substring su( testConstLocalString        ) ; testParam( ut, testConstACharP, su ) ; }
    { Substring su( testConstStdString          ) ; testParam( ut, testConstACharP, su ) ; }
    { Substring su( testConstMyString           ) ; testParam( ut, myStringBuf    , su ) ; }
}


UT_METHOD( ConstructorsImplicit )
{
    UT_INIT()

   const character*             testConstCharP= A_CHAR("TEST");
   character*                   testCharP=      const_cast<character*>( testConstCharP );
   std::basic_string<character> testStdString           ( testConstCharP ) ;
   AString                  testAString                 ( testConstCharP ) ;
   character                testStringLiteral[2]        ( A_CHAR("1")    ) ;
   Substring                testSubstring               ( testConstCharP ) ;
   String32                 testLocalString             ( testConstCharP ) ;
   tstn::MyString           testMyString;

   const std::basic_string<character> testConstStdString( testConstCharP ) ;
   const AString            testConstAString            ( testConstCharP ) ;
   const character           testConstStringLiteral[2]  ( A_CHAR("1")    ) ;
   const Substring          testConstSubstring          ( testConstCharP ) ;
   const String32           testConstLocalString        ( testConstCharP ) ;
   const tstn::MyString     testConstMyString;

   const character* myStringBuf= testMyString.GetMyBuffer    (            ) ;

    // implicit String
    {
        testParam( ut, testConstCharP,     A_CHAR("TEST")                 ) ;
        testParam( ut, testConstCharP,     testConstCharP              ) ;
      //testParam( ut, testConstCharP,     testCharP                   ) ;

        testParam( ut, A_CHAR("1"),        testStringLiteral            ) ;
        testParam( ut, testConstCharP,     testSubstring                ) ;
        testParam( ut, testConstCharP,     testAString                  ) ;
        testParam( ut, testConstCharP,     testLocalString              ) ;
        testParam( ut, testConstCharP,     testStdString                ) ;
        testParam( ut, myStringBuf   ,     testMyString                 ) ;

        testParam( ut, A_CHAR("1"),        testConstStringLiteral       ) ;
        testParam( ut, testConstCharP,     testConstSubstring           ) ;
        testParam( ut, testConstCharP,     testConstAString             ) ;
        testParam( ut, testConstCharP,     testConstLocalString         ) ;
        testParam( ut, myStringBuf   ,     testConstMyString            ) ;
    }

    // implicit SubString
    {
        testParamSubstring( ut, testConstCharP,    A_CHAR("TEST")                                                             );
        testParamSubstring( ut, testConstCharP,     testConstCharP              );
      //testParamSubstring( ut, testConstCharP,     testCharP                   );

        testParamSubstring( ut, A_CHAR("1"),        testStringLiteral            );
        testParamSubstring( ut, testConstCharP,     testSubstring                );
        testParamSubstring( ut, testConstCharP,     testAString                  );
        testParamSubstring( ut, testConstCharP,     testLocalString              );
        testParamSubstring( ut, testConstCharP,     testStdString                );
        testParamSubstring( ut, myStringBuf   ,     testMyString                 );

        testParamSubstring( ut, A_CHAR("1"),        testConstStringLiteral       );
        testParamSubstring( ut, testConstCharP,     testConstSubstring           );
        testParamSubstring( ut, testConstCharP,     testConstAString             );
        testParamSubstring( ut, testConstCharP,     testConstLocalString         );
        testParamSubstring( ut, myStringBuf   ,     testConstMyString            );
    }

    // implicit CString
    {
        testParamTerminatable( ut, testConstCharP,      A_CHAR("TEST")              );

        testParamTerminatable( ut, A_CHAR("1"),         testStringLiteral           );
        testParamTerminatable( ut, testConstCharP,      testAString                 );
        testParamTerminatable( ut, testConstCharP,      testLocalString             );

        testParamTerminatable( ut, A_CHAR("1"),         testConstStringLiteral      );
        testParamTerminatable( ut, testConstCharP,      testConstAString            );
        testParamTerminatable( ut, testConstCharP,      testConstLocalString        );

        //testParamTerminatable( ut, A_CHAR("1"),       &testStringLiteral          );
        testParamTerminatable( ut, testConstCharP,      &testAString                );
        testParamTerminatable( ut, testConstCharP,      &testLocalString            );

        //testParamTerminatable( ut, A_CHAR("1"),       &testConstStringLiteral     );
        testParamTerminatable( ut, testConstCharP,      &testConstAString           );
        testParamTerminatable( ut, testConstCharP,      &testConstLocalString);
    }


    // explicit CString
    {
       testParamTerminatable( ut, testConstCharP,      CString(testConstCharP     ) );
       testParamTerminatable( ut, testConstCharP,      CString(testCharP          ) );
       testParamTerminatable( ut, testConstCharP,      CString(testSubstring      ) );
       testParamTerminatable( ut, testConstCharP,      CString(testStdString      ) );
       testParamTerminatable( ut, testConstCharP,      CString(testConstSubstring ) );
       testParamTerminatable( ut, testConstCharP,      CString(testConstStdString ) );
       testParamTerminatable( ut, testConstCharP,      CString(&testSubstring     ) );
       testParamTerminatable( ut, testConstCharP,      CString(&testConstSubstring) );

       // with errors
        lang::Report::GetDefault().PushHaltFlags( false, false );

        character charArray[4]= {'A', 'B', 'C', 'D'};
        // The following implicit constructor takes wrong length, as constructor for char arrays.
        // This is still what is wanted, because implicit construction with character arrays is
        // needed for C++ literals.
        UT_PRINT( "One error should follow: " )
        CString ts=  CString( charArray );
        UT_EQ( 3, ts.Length())

        Substring fromCharArray= String(charArray).Substring( 0, 2 );
        UT_PRINT( "One error should follow: " )
        ts= CString( fromCharArray );

        lang::Report::GetDefault().PopHaltFlags();
    }
}

//--------------------------------------------------------------------------------------------------
//--- Assignment
//--------------------------------------------------------------------------------------------------
UT_METHOD( Assignment )
{
    UT_INIT()


          character                     testAChar=            '@';
    const character                     testConstAChar=       '@';
    const character*                    testConstACharP=      A_CHAR("TEST");
          character*                    testACharP=           const_cast<character*>( testConstACharP );

    std::basic_string<character>        testStdString        (testConstACharP);
    String                              testString           (testConstACharP);
    CString                             testCString          (testConstACharP);
    AString                             testAString          (testConstACharP);
    character                           testStringLiteral[2] (A_CHAR("1"));
    Substring                           testSubstring        =String( A_CHAR("@TEST@") ).Substring<NC>(1,4);
    String32                            testLocalString       ( testConstACharP );
    tstn::MyString                      testMyString;

    const std::basic_string<character>  testConstStdString   (testConstACharP);
    const String                        testConstString      (testConstACharP);
    const CString                       testConstCString     (testConstACharP);
    const AString                       testConstAString     (testConstACharP);
    const character                     testConstStringLiteral[2](A_CHAR("1"));
    const Substring                     testConstSubstring   =String( A_CHAR("@TEST@") ).Substring<NC>(1,4);
    const String32                      testConstLocalString  ( testConstACharP );
    const tstn::MyString                testConstMyString;

    const character* myStringBuf= testMyString.GetMyBuffer();

    // String
    { String        as=  A_CHAR("TEST")              ; testParam( ut, testConstACharP, as) ; }
    { String        as=  testConstACharP             ; testParam( ut, testConstACharP, as) ; }
  //{ String        as=  testACharP                  ; testParam( ut, testConstACharP, as) ; }
    { String        as=  (String(testACharP))        ; testParam( ut, testConstACharP, as) ; }

    { String        as=  testStringLiteral           ; testParam( ut, A_CHAR("1")    , as) ; }
    { String        as=  testString                  ; testParam( ut, testConstACharP, as) ; }
    { String        as=  testCString                 ; testParam( ut, testConstACharP, as) ; }
    { String        as=  testSubstring               ; testParam( ut, testConstACharP, as) ; }
    { String        as=  testAString                 ; testParam( ut, testConstACharP, as) ; }
    { String        as=  testLocalString             ; testParam( ut, testConstACharP, as) ; }
    { String        as=  testStdString               ; testParam( ut, testConstACharP, as) ; }
    { String        as=  testMyString                ; testParam( ut, myStringBuf    , as) ; }

    { String        as=  testConstStringLiteral      ; testParam( ut, A_CHAR("1")    , as) ; }
    { String        as=  testConstString             ; testParam( ut, testConstACharP, as) ; }
    { String        as=  testConstString             ; testParam( ut, testConstACharP, as) ; }
    { String        as=  testConstSubstring          ; testParam( ut, testConstACharP, as) ; }
    { String        as=  testConstAString            ; testParam( ut, testConstACharP, as) ; }
    { String        as=  testConstLocalString        ; testParam( ut, testConstACharP, as) ; }
    { String        as=  testConstStdString          ; testParam( ut, testConstACharP, as) ; }
    { String        as=  testConstMyString           ; testParam( ut, myStringBuf    , as) ; }

    { String    as; as=  A_CHAR("TEST")              ; testParam( ut, testConstACharP, as) ; }
    { String    as; as=  testConstACharP             ; testParam( ut, testConstACharP, as) ; }
  //{ String    as; as=  testACharP                  ; testParam( ut, testConstACharP, as) ; }
    { String    as; as=  (String(testACharP))        ; testParam( ut, testConstACharP, as) ; }

    { String    as; as=  testStringLiteral           ; testParam( ut, A_CHAR("1")    , as) ; }
    { String    as; as=  testString                  ; testParam( ut, testConstACharP, as) ; }
    { String    as; as=  testCString                 ; testParam( ut, testConstACharP, as) ; }
    { String    as; as=  testSubstring               ; testParam( ut, testConstACharP, as) ; }
    { String    as; as=  testAString                 ; testParam( ut, testConstACharP, as) ; }
    { String    as; as=  testLocalString             ; testParam( ut, testConstACharP, as) ; }
    { String    as; as=  testStdString               ; testParam( ut, testConstACharP, as) ; }
    { String    as; as=  testMyString                ; testParam( ut, myStringBuf    , as) ; }

    { String    as; as=  testConstStringLiteral      ; testParam( ut, A_CHAR("1")    , as) ; }
    { String    as; as=  testConstString             ; testParam( ut, testConstACharP, as) ; }
    { String    as; as=  testConstCString            ; testParam( ut, testConstACharP, as) ; }
    { String    as; as=  testConstSubstring          ; testParam( ut, testConstACharP, as) ; }
    { String    as; as=  testConstAString            ; testParam( ut, testConstACharP, as) ; }
    { String    as; as=  testConstLocalString        ; testParam( ut, testConstACharP, as) ; }
    { String    as; as=  testConstStdString          ; testParam( ut, testConstACharP, as) ; }
    { String    as; as=  testConstMyString           ; testParam( ut, myStringBuf    , as) ; }

    // Substring
    { Substring   as=  A_CHAR("TEST")                    ; testParam( ut, testConstACharP, as) ; }
    { Substring          as=  testConstACharP            ; testParam( ut, testConstACharP, as) ; }
  //{ Substring          as=  testACharP                 ; testParam( ut, testConstACharP, as) ; }
    { Substring          as=  Substring(testACharP)      ; testParam( ut, testConstACharP, as) ; }

    { Substring          as=  testStringLiteral          ; testParam( ut, A_CHAR("1")    , as) ; }
    { Substring          as=  testString                 ; testParam( ut, testConstACharP, as) ; }
    { Substring          as=  testCString                ; testParam( ut, testConstACharP, as) ; }
    { Substring          as=  testSubstring              ; testParam( ut, testConstACharP, as) ; }
    { Substring          as=  testAString                ; testParam( ut, testConstACharP, as) ; }
    { Substring          as=  testLocalString            ; testParam( ut, testConstACharP, as) ; }
    { Substring          as=  testStdString              ; testParam( ut, testConstACharP, as) ; }
    { Substring          as=  testMyString               ; testParam( ut, myStringBuf    , as) ; }

    { Substring          as=  testConstStringLiteral     ; testParam( ut, A_CHAR("1")    , as) ; }
    { Substring          as=  testConstString            ; testParam( ut, testConstACharP, as) ; }
    { Substring          as=  testConstCString           ; testParam( ut, testConstACharP, as) ; }
    { Substring          as=  testConstSubstring         ; testParam( ut, testConstACharP, as) ; }
    { Substring          as=  testConstAString           ; testParam( ut, testConstACharP, as) ; }
    { Substring          as=  testConstLocalString; testParam( ut, testConstACharP, as) ; }
    { Substring          as=  testConstStdString         ; testParam( ut, testConstACharP, as) ; }
    { Substring          as=  testConstMyString          ; testParam( ut, myStringBuf    , as) ; }

    { Substring     as; as=  A_CHAR("TEST")              ; testParam( ut, testConstACharP, as) ; }
    { Substring     as; as=  testConstACharP             ; testParam( ut, testConstACharP, as) ; }
  //{ Substring     as; as=  testACharP                  ; testParam( ut, testConstACharP, as) ; }
    { Substring     as; as=  Substring(testACharP)       ; testParam( ut, testConstACharP, as) ; }

    { Substring     as; as=  testStringLiteral           ; testParam( ut, A_CHAR("1")    , as) ; }
    { Substring     as; as=  testString                  ; testParam( ut, testConstACharP, as) ; }
    { Substring     as; as=  testCString                 ; testParam( ut, testConstACharP, as) ; }
    { Substring     as; as=  testSubstring               ; testParam( ut, testConstACharP, as) ; }
    { Substring     as; as=  testAString                 ; testParam( ut, testConstACharP, as) ; }
    { Substring     as; as=  testLocalString             ; testParam( ut, testConstACharP, as) ; }
    { Substring     as; as=  testStdString               ; testParam( ut, testConstACharP, as) ; }
    { Substring     as; as=  testMyString                ; testParam( ut, myStringBuf    , as) ; }

    { Substring     as; as=  testConstStringLiteral      ; testParam( ut, A_CHAR("1")    , as) ; }
    { Substring     as; as=  testConstString             ; testParam( ut, testConstACharP, as) ; }
    { Substring     as; as=  testConstCString            ; testParam( ut, testConstACharP, as) ; }
    { Substring     as; as=  testConstSubstring          ; testParam( ut, testConstACharP, as) ; }
    { Substring     as; as=  testConstAString            ; testParam( ut, testConstACharP, as) ; }
    { Substring     as; as=  testConstLocalString        ; testParam( ut, testConstACharP, as) ; }
    { Substring     as; as=  testConstStdString          ; testParam( ut, testConstACharP, as) ; }
    { Substring     as; as=  testConstMyString           ; testParam( ut, myStringBuf    , as) ; }

    // CString (no Substrings!)
    { CString     ts = A_CHAR("TEST")                    ;testParam ( ut, testConstACharP,  ts ) ; }
    { CString     ts = CString(testConstACharP         ) ;testParam ( ut, testConstACharP,  ts ) ; }
    { CString     ts = CString(testACharP              ) ;testParam ( ut, testConstACharP,  ts ) ; }
    { CString     ts = testStringLiteral                 ;testParam ( ut, A_CHAR( "1")   ,  ts ) ; }
    { CString     ts = CString(testString              ) ;testParam ( ut, testConstACharP,  ts ) ; }
    { CString     ts = CString(testCString             ) ;testParam ( ut, testConstACharP,  ts ) ; }
  //{ CString     ts = CString(testSubstring           ) ;testParam ( ut, testConstACharP,  ts ) ; }
    { CString     ts = testAString                       ;testParam ( ut, testConstACharP,  ts ) ; }
    { CString     ts = testLocalString                   ;testParam ( ut, testConstACharP,  ts ) ; }
    { CString     ts = CString(testStdString           ) ;testParam ( ut, testConstACharP,  ts ) ; }
  //{ CString     ts = CString(testMyString            ) ;testParam ( ut, myStringBuf    ,  ts ) ; }
    { CString     ts = testConstStringLiteral            ;testParam ( ut, A_CHAR( "1")   ,  ts ) ; }
  //{ CString     ts = CString(testConstSubstring      ) ;testParam ( ut, testConstACharP,  ts ) ; }
    { CString     ts = testConstAString                  ;testParam ( ut, testConstACharP,  ts ) ; }
    { CString     ts = testConstLocalString              ;testParam ( ut, testConstACharP,  ts ) ; }
    { CString     ts = testConstStdString                ;testParam ( ut, testConstACharP,  ts ) ; }
  //{ CString     ts = CString(testConstMyString       ) ;testParam ( ut, myStringBuf    ,  ts ) ; }
  //{ CString     ts = CString(&testStringLiteral      ) ;testParam ( ut, A_CHAR( "1")   ,  ts ) ; }
    { CString     ts = CString(&testString             ) ;testParam ( ut, testConstACharP,  ts ) ; }
    { CString     ts = CString(&testCString            ) ;testParam ( ut, testConstACharP,  ts ) ; }
  //{ CString     ts = CString(&testSubstring          ) ;testParam ( ut, testConstACharP,  ts ) ; }
    { CString     ts = &testAString                      ;testParam ( ut, testConstACharP,  ts ) ; }
    { CString     ts = &testLocalString                  ;testParam ( ut, testConstACharP,  ts ) ; }
    { CString     ts = &testStdString                    ;testParam ( ut, testConstACharP,  ts ) ; }
  //{ CString     ts = CString(&testMyString           ) ;testParam ( ut, myStringBuf    ,  ts ) ; }
  //{ CString     ts = CString(&testConstStringLiteral ) ;testParam ( ut, A_CHAR( "1")   ,  ts ) ; }
  //{ CString     ts = CString(&testConstSubstring     ) ;testParam ( ut, testConstACharP,  ts ) ; }
    { CString     ts = &testConstAString                 ;testParam ( ut, testConstACharP,  ts ) ; }
    { CString     ts = &testConstLocalString             ;testParam ( ut, testConstACharP,  ts ) ; }
    { CString     ts = &testConstStdString               ;testParam ( ut, testConstACharP,  ts ) ; }
  //{ CString     ts = CString(&testConstMyString      ) ;testParam ( ut, myStringBuf    ,  ts ) ; }

    { CString ts; ts = A_CHAR("TEST")                    ; testParam( ut, testConstACharP,  ts ) ; }
    { CString ts; ts = CString(testConstACharP         ) ; testParam( ut, testConstACharP,  ts ) ; }
    { CString ts; ts = CString(testACharP              ) ; testParam( ut, testConstACharP,  ts ) ; }
    { CString ts; ts = testStringLiteral                 ; testParam( ut, A_CHAR("1"   ) ,  ts ) ; }
    { CString ts; ts = CString(testString              ) ; testParam( ut, testConstACharP,  ts ) ; }
    { CString ts; ts = CString(testCString             ) ; testParam( ut, testConstACharP,  ts ) ; }
  //{ CString ts; ts = CString(testSubstring           ) ; testParam( ut, testConstACharP,  ts ) ; }
    { CString ts; ts = testAString                       ; testParam( ut, testConstACharP,  ts ) ; }
    { CString ts; ts = testLocalString                   ; testParam( ut, testConstACharP,  ts ) ; }
    { CString ts; ts = CString(testStdString           ) ; testParam( ut, testConstACharP,  ts ) ; }
  //{ CString ts; ts = CString(testMyString            ) ; testParam( ut, myStringBuf    ,  ts ) ; }
    { CString ts; ts = testConstStringLiteral            ; testParam( ut, A_CHAR("1"   ) ,  ts ) ; }
  //{ CString ts; ts = CString(testConstSubstring      ) ; testParam( ut, testConstACharP,  ts ) ; }
    { CString ts; ts = testConstAString                  ; testParam( ut, testConstACharP,  ts ) ; }
    { CString ts; ts = testConstLocalString              ; testParam( ut, testConstACharP,  ts ) ; }
    { CString ts; ts = CString(testConstStdString      ) ; testParam( ut, testConstACharP,  ts ) ; }
  //{ CString ts; ts = CString(testConstMyString       ) ; testParam( ut, myStringBuf    ,  ts ) ; }
  //{ CString ts; ts = CString(&testStringLiteral      ) ; testParam( ut, A_CHAR("1"   ) ,  ts ) ; }
    { CString ts; ts = CString(&testString             ) ; testParam( ut, testConstACharP,  ts ) ; }
    { CString ts; ts = CString(&testCString            ) ; testParam( ut, testConstACharP,  ts ) ; }
  //{ CString ts; ts = CString(&testSubstring          ) ; testParam( ut, testConstACharP,  ts ) ; }
    { CString ts; ts = &testAString                      ; testParam( ut, testConstACharP,  ts ) ; }
    { CString ts; ts = &testLocalString                  ; testParam( ut, testConstACharP,  ts ) ; }
    { CString ts; ts = &testStdString                    ; testParam( ut, testConstACharP,  ts ) ; }
  //{ CString ts; ts = CString(&testMyString           ) ; testParam( ut, myStringBuf    ,  ts ) ; }
  //{ CString ts; ts = CString(&testConstStringLiteral );; testParam( ut, A_CHAR("1"   ) ,  ts ) ; }
  //{ CString ts; ts = CString(&testConstSubstring     ) ; testParam( ut, testConstACharP,  ts ) ; }
    { CString ts; ts = &testConstAString                 ; testParam( ut, testConstACharP,  ts ) ; }
    { CString ts; ts = &testConstLocalString             ; testParam( ut, testConstACharP,  ts ) ; }
    { CString ts; ts = &testConstStdString               ; testParam( ut, testConstACharP,  ts ) ; }
  //{ CString ts; ts = CString(&testConstMyString      ) ; testParam( ut, myStringBuf    ,  ts ) ; }
    // AString

    // direct initialization not allowed! Uncomment to see compiler error
    //{ AString            as=  "TEST"              ; testParam( ut, testConstACharP, as); }


//    { AString        as; as=  "TEST"                      ; testParam( ut, testConstACharP, as); }
//    { AString        as; as=  L"TEST"                     ; testParam( ut, testConstACharP, as); }
//    { AString        as; as=  testConstACharP             ; testParam( ut, testConstACharP, as); }
//  //{ AString        as; as=  testACharP                  ; testParam( ut, testConstACharP, as); }
//    { AString        as; as=  (String(testACharP))        ; testParam( ut, testConstACharP, as); }
//
//    { AString        as; as=  testAChar                   ; testParam( ut, A_CHAR("@")    , as); }
//    { AString        as; as=  testStringLiteral           ; testParam( ut, A_CHAR("1")    , as); }
//    { AString        as; as=  testString                  ; testParam( ut, testConstACharP, as); }
//    { AString        as; as=  testCString                 ; testParam( ut, testConstACharP, as); }
//    { AString        as; as=  testSubstring               ; testParam( ut, testConstACharP, as); }
//    { AString        as; as=  testAString                 ; testParam( ut, testConstACharP, as); }
//    { AString        as; as=  testLocalString             ; testParam( ut, testConstACharP, as); }
//    { AString        as; as=  testStdString               ; testParam( ut, testConstACharP, as); }
//    { AString        as; as=  testMyString                ; testParam( ut, myStringBuf    , as); }
//
//    { AString        as; as=  testConstAChar              ; testParam( ut, A_CHAR("@")    , as); }
//    { AString        as; as=  testConstStringLiteral      ; testParam( ut, A_CHAR("1")    , as); }
//    { AString        as; as=  testConstString             ; testParam( ut, testConstACharP, as); }
//    { AString        as; as=  testConstCString            ; testParam( ut, testConstACharP, as); }
//    { AString        as; as=  testConstSubstring          ; testParam( ut, testConstACharP, as); }
//    { AString        as; as=  testConstAString            ; testParam( ut, testConstACharP, as); }
//    { AString        as; as=  testConstLocalString        ; testParam( ut, testConstACharP, as); }
//    { AString        as; as=  testConstStdString          ; testParam( ut, testConstACharP, as); }
//    { AString        as; as=  testConstMyString           ; testParam( ut, myStringBuf    , as); }

//    { AString        as; as= &testStringLiteral           ; testParam( ut, A_CHAR("1")    , as); }
//    { AString        as; as= &testString                  ; testParam( ut, testConstACharP, as); }
//    { AString        as; as= &testCString                 ; testParam( ut, testConstACharP, as); }
//    { AString        as; as= &testSubstring               ; testParam( ut, testConstACharP, as); }
//    { AString        as; as= &testAString                 ; testParam( ut, testConstACharP, as); }
//    { AString        as; as= &testLocalString             ; testParam( ut, testConstACharP, as); }
//    { AString        as; as= &testStdString               ; testParam( ut, testConstACharP, as); }
//    { AString        as; as= &testMyString                ; testParam( ut, myStringBuf    , as); }
//
//    { AString        as; as= &testConstStringLiteral      ; testParam( ut, A_CHAR("1")    , as); }
//    { AString        as; as= &testConstSubstring          ; testParam( ut, testConstACharP, as); }
//    { AString        as; as= &testConstString             ; testParam( ut, testConstACharP, as); }
//    { AString        as; as= &testConstCString            ; testParam( ut, testConstACharP, as); }
//    { AString        as; as= &testConstAString            ; testParam( ut, testConstACharP, as); }
//    { AString        as; as= &testConstLocalString        ; testParam( ut, testConstACharP, as); }
//    { AString        as; as= &testConstStdString          ; testParam( ut, testConstACharP, as); }
//    { AString        as; as= &testConstMyString           ; testParam( ut, myStringBuf    , as); }

    // LocalString
    // direct initialization not allowed!
    //{ String32             as=  "TEST"              ; testParam( ut, testConstACharP, as); }
    //...
    //...
    { String32 as; as=  "TEST"                      ; testParam( ut, testConstACharP, as); }
    { String32 as; as=  L"TEST"                     ; testParam( ut, testConstACharP, as); }
    { String32 as; as=  testConstACharP             ; testParam( ut, testConstACharP, as); }
  //{ String32 as; as=  testACharP                  ; testParam( ut, testConstACharP, as); }
    { String32 as; as=  (String(testACharP))        ; testParam( ut, testConstACharP, as); }

    { String32 as; as=  testAChar                   ; testParam( ut, A_CHAR("@")    , as); }
    { String32 as; as=  testStringLiteral           ; testParam( ut, A_CHAR("1")    , as); }
    { String32 as; as=  testString                  ; testParam( ut, testConstACharP, as); }
    { String32 as; as=  testCString                 ; testParam( ut, testConstACharP, as); }
    { String32 as; as=  testSubstring               ; testParam( ut, testConstACharP, as); }
    { String32 as; as=  testAString                 ; testParam( ut, testConstACharP, as); }
    { String32 as; as=  testLocalString             ; testParam( ut, testConstACharP, as); }
    { String32 as; as=  testStdString               ; testParam( ut, testConstACharP, as); }
    { String32 as; as=  testMyString                ; testParam( ut, myStringBuf    , as); }

    { String32 as; as=  testConstAChar              ; testParam( ut, A_CHAR("@")    , as); }
    { String32 as; as=  testConstStringLiteral      ; testParam( ut, A_CHAR("1")    , as); }
    { String32 as; as=  testConstString             ; testParam( ut, testConstACharP, as); }
    { String32 as; as=  testConstCString            ; testParam( ut, testConstACharP, as); }
    { String32 as; as=  testConstSubstring          ; testParam( ut, testConstACharP, as); }
    { String32 as; as=  testConstAString            ; testParam( ut, testConstACharP, as); }
    { String32 as; as=  testConstLocalString        ; testParam( ut, testConstACharP, as); }
    { String32 as; as=  testConstStdString          ; testParam( ut, testConstACharP, as); }
    { String32 as; as=  testConstMyString           ; testParam( ut, myStringBuf    , as); }

    // StringLiteral
    { character    as[5]=  A_CHAR("TEST")           ; testParam( ut, testConstACharP, as); }
}

//--------------------------------------------------------------------------------------------------
//--- Append and operator <<
//--------------------------------------------------------------------------------------------------
UT_METHOD( AppendAndAppendOperator )
{
    UT_INIT()

    const character*                    testConstACharP= A_CHAR("TEST");
    character*                          testACharP=       const_cast<character*>( testConstACharP );
    std::basic_string<character>        testStdString    (testConstACharP);
     String                             testString       (testConstACharP);
    CString                             testCString      (testConstACharP);
    AString                             testAString      (testConstACharP);
    character                           testStringLiteral[2](A_CHAR("1"));
    Substring                           testSubstring    =String( A_CHAR("@TEST@") ).Substring<NC>(1,4);
    String32                            testLocalString  ( testConstACharP );
    tstn::MyString                      testMyString;

    const std::basic_string<character>  testConstStdString    (testConstACharP);
    const  String                       testConstString       (testConstACharP);
    const CString                       testConstCString      (testConstACharP);
    const AString                       testConstAString      (testConstACharP);
    const character                     testConstStringLiteral[2](A_CHAR("1"));
    const Substring                     testConstSubstring    =String( A_CHAR("@TEST@") ).Substring<NC>(1,4);
    const String32                      testConstLocalString  ( testConstACharP );
    const tstn::MyString                testConstMyString;

    const character* myStringBuf=       testMyString.GetMyBuffer();

    // these have to fail
    // { vector<int> x;             String as(x);}
    // { vector<int> x; AString as; as._ (x);}
    // { char c   ='a';             String as(c);}

#if ALIB_CAMP
    CalendarDateTime calendar;
                     calendar.Day=     1;
                     calendar.Month=   4;
                     calendar.Year=    2011;
                     calendar.Hour=    16;
                     calendar.Minute=  00;
                     calendar.Second=  01;
    DateTime         testTicks(calendar.Get(lang::Timezone::UTC));
    const DateTime   testConstTicks  ( testTicks );
    const character* ticksResult= A_CHAR("2011-04-01 16:00");
#endif

    // AString
    { AString as; as <<  A_CHAR("TEST")               ; testParam( ut, testConstACharP, as) ; }
    { AString as; as <<  testConstACharP              ; testParam( ut, testConstACharP, as) ; }
  //{ AString as; as <<  testACharP                   ; testParam( ut, testConstACharP, as) ; }
    { AString as; as <<  (String(testACharP))         ; testParam( ut, testConstACharP, as) ; }

    { AString as; as <<  testStringLiteral            ; testParam( ut, A_CHAR("1")    , as) ; }
    { AString as; as <<  testString                   ; testParam( ut, testConstACharP, as) ; }
    { AString as; as <<  testCString                  ; testParam( ut, testConstACharP, as) ; }
    { AString as; as <<  testSubstring                ; testParam( ut, testConstACharP, as) ; }
    { AString as; as <<  testAString                  ; testParam( ut, testConstACharP, as) ; }
    { AString as; as <<  testLocalString              ; testParam( ut, testConstACharP, as) ; }
    { AString as; as <<  testStdString                ; testParam( ut, testConstACharP, as) ; }
    { AString as; as <<  testMyString                 ; testParam( ut, myStringBuf    , as) ; }
IF_ALIB_CAMP(
    { AString as; as <<  testTicks                    ; testParam( ut, ticksResult ,    as) ; } )

    { AString as; as <<  testConstStringLiteral       ; testParam( ut, A_CHAR("1")    , as) ; }
    { AString as; as <<  testConstString              ; testParam( ut, testConstACharP, as) ; }
    { AString as; as <<  testConstCString             ; testParam( ut, testConstACharP, as) ; }
    { AString as; as <<  testConstSubstring           ; testParam( ut, testConstACharP, as) ; }
    { AString as; as <<  testConstAString             ; testParam( ut, testConstACharP, as) ; }
    { AString as; as <<  testConstLocalString         ; testParam( ut, testConstACharP, as) ; }
    { AString as; as <<  testConstStdString           ; testParam( ut, testConstACharP, as) ; }
    { AString as; as <<  testConstMyString            ; testParam( ut, myStringBuf    , as) ; }
IF_ALIB_CAMP(
    { AString as; as <<  testConstTicks               ; testParam( ut, ticksResult,     as) ; } )

    { AString as; as._( A_CHAR("TEST")      )         ; testParam( ut, testConstACharP, as) ; }
    { AString as; as._( testConstACharP     )         ; testParam( ut, testConstACharP, as) ; }
  //{ AString as; as._( testACharP          )         ; testParam( ut, testConstACharP, as) ; }
    { AString as; as._( (String(testACharP)))         ; testParam( ut, testConstACharP, as) ; }

    { AString as; as._( testStringLiteral   )         ; testParam( ut, A_CHAR("1")    , as) ; }
    { AString as; as._( testString          )         ; testParam( ut, testConstACharP, as) ; }
    { AString as; as._( testCString         )         ; testParam( ut, testConstACharP, as) ; }
    { AString as; as._( testSubstring       )         ; testParam( ut, testConstACharP, as) ; }
    { AString as; as._( testAString         )         ; testParam( ut, testConstACharP, as) ; }
    { AString as; as._( testLocalString     )         ; testParam( ut, testConstACharP, as) ; }
    { AString as; as._( testStdString       )         ; testParam( ut, testConstACharP, as) ; }
    { AString as; as._( testMyString        )         ; testParam( ut, myStringBuf    , as) ; }
IF_ALIB_CAMP(
    { AString as; as._( testTicks           )         ; testParam( ut, ticksResult,     as) ; } )

    { AString as; as._( testConstStringLiteral  )     ; testParam( ut, A_CHAR("1")    , as) ; }
    { AString as; as._( testConstString     )         ; testParam( ut, testConstACharP, as) ; }
    { AString as; as._( testConstCString    )         ; testParam( ut, testConstACharP, as) ; }
    { AString as; as._( testConstSubstring  )         ; testParam( ut, testConstACharP, as) ; }
    { AString as; as._( testConstAString    )         ; testParam( ut, testConstACharP, as) ; }
    { AString as; as._( testConstLocalString        ) ; testParam( ut, testConstACharP, as) ; }
    { AString as; as._( testConstStdString  )         ; testParam( ut, testConstACharP, as) ; }
    { AString as; as._( testConstMyString   )         ; testParam( ut, myStringBuf    , as) ; }
IF_ALIB_CAMP(
    { AString as; as._( testConstTicks      )         ; testParam( ut, ticksResult,     as) ; } )

    { AString as          ; as._<NC>( A_CHAR("TEST")       )        ; testParam( ut, testConstACharP, as) ; }
    { AString as          ; as._<NC>( testConstACharP      )        ; testParam( ut, testConstACharP, as) ; }
  //{ AString as          ; as._<NC>( testACharP           )        ; testParam( ut, testConstACharP, as) ; }
    { AString as          ; as._<NC>( (String(testACharP)) )        ; testParam( ut, testConstACharP, as) ; }

    { AString as          ; as._<NC>( testStringLiteral   )         ; testParam( ut, A_CHAR("1")    , as) ; }
    { AString as          ; as._<NC>( testString          )         ; testParam( ut, testConstACharP, as) ; }
    { AString as          ; as._<NC>( testCString         )         ; testParam( ut, testConstACharP, as) ; }
    { AString as          ; as._<NC>( testSubstring       )         ; testParam( ut, testConstACharP, as) ; }
    { AString as          ; as._<NC>( testAString         )         ; testParam( ut, testConstACharP, as) ; }
    { AString as          ; as._<NC>( testLocalString     )         ; testParam( ut, testConstACharP, as) ; }
    { AString as          ; as._<NC>( testStdString       )         ; testParam( ut, testConstACharP, as) ; }
    { AString as          ; as._<NC>( testMyString        )         ; testParam( ut, myStringBuf    , as) ; }
IF_ALIB_CAMP(
    { AString as          ; as._<NC>( testTicks           )         ; testParam( ut, ticksResult,     as) ; } )

    { AString as          ; as._<NC>( testConstStringLiteral  )     ; testParam( ut, A_CHAR("1")    , as) ; }
    { AString as          ; as._<NC>( testConstString     )         ; testParam( ut, testConstACharP, as) ; }
    { AString as          ; as._<NC>( testConstCString    )         ; testParam( ut, testConstACharP, as) ; }
    { AString as          ; as._<NC>( testConstSubstring  )         ; testParam( ut, testConstACharP, as) ; }
    { AString as          ; as._<NC>( testConstAString    )         ; testParam( ut, testConstACharP, as) ; }
    { AString as          ; as._<NC>( testConstLocalString)         ; testParam( ut, testConstACharP, as) ; }
    { AString as          ; as._<NC>( testConstStdString  )         ; testParam( ut, testConstACharP, as) ; }
    { AString as          ; as._<NC>( testConstMyString   )         ; testParam( ut, myStringBuf    , as) ; }
IF_ALIB_CAMP(
    { AString as          ; as._<NC>( testConstTicks      )         ; testParam( ut, ticksResult,     as) ; } )

    // LocalString
    { String64    as      ; as <<  A_CHAR("TEST")                      ; testParam( ut, testConstACharP, as) ; }
    { String64    as      ; as <<  testConstACharP                     ; testParam( ut, testConstACharP, as) ; }
  //{ String64    as      ; as <<  testACharP                          ; testParam( ut, testConstACharP, as) ; }
    { String64    as      ; as <<  String(testACharP)                  ; testParam( ut, testConstACharP, as) ; }

    { String64    as      ; as <<  testStringLiteral                   ; testParam( ut, A_CHAR("1")    , as) ; }
    { String64    as      ; as <<  testString                          ; testParam( ut, testConstACharP, as) ; }
    { String64    as      ; as <<  testCString                         ; testParam( ut, testConstACharP, as) ; }
    { String64    as      ; as <<  testSubstring                       ; testParam( ut, testConstACharP, as) ; }
    { String64    as      ; as <<  testAString                         ; testParam( ut, testConstACharP, as) ; }
    { String64    as      ; as <<  testLocalString                     ; testParam( ut, testConstACharP, as) ; }
    { String64    as      ; as <<  testStdString                       ; testParam( ut, testConstACharP, as) ; }
    { String64    as      ; as <<  testMyString                        ; testParam( ut, myStringBuf    , as) ; }

    { String64    as      ; as <<  testConstStringLiteral              ; testParam( ut, A_CHAR("1")    , as) ; }
    { String64    as      ; as <<  testConstString                     ; testParam( ut, testConstACharP, as) ; }
    { String64    as      ; as <<  testConstCString                    ; testParam( ut, testConstACharP, as) ; }
    { String64    as      ; as <<  testConstSubstring                  ; testParam( ut, testConstACharP, as) ; }
    { String64    as      ; as <<  testConstAString                    ; testParam( ut, testConstACharP, as) ; }
    { String64    as      ; as <<  testConstLocalString                ; testParam( ut, testConstACharP, as) ; }
    { String64    as      ; as <<  testConstStdString                  ; testParam( ut, testConstACharP, as) ; }
    { String64    as      ; as <<  testConstMyString                   ; testParam( ut, myStringBuf    , as) ; }

    { String64    as      ; as._( A_CHAR("TEST")       )               ; testParam( ut, testConstACharP, as) ; }
    { String64    as      ; as._( testConstACharP      )               ; testParam( ut, testConstACharP, as) ; }
  //{ String64    as      ; as._( testACharP           )               ; testParam( ut, testConstACharP, as) ; }
    { String64    as      ; as._( (String(testACharP)) )               ; testParam( ut, testConstACharP, as) ; }

    { String64    as      ; as._( testStringLiteral   )                ; testParam( ut, A_CHAR("1")    , as) ; }
    { String64    as      ; as._( testString          )                ; testParam( ut, testConstACharP, as) ; }
    { String64    as      ; as._( testCString         )                ; testParam( ut, testConstACharP, as) ; }
    { String64    as      ; as._( testSubstring       )                ; testParam( ut, testConstACharP, as) ; }
    { String64    as      ; as._( testAString         )                ; testParam( ut, testConstACharP, as) ; }
    { String64    as      ; as._( testLocalString     )                ; testParam( ut, testConstACharP, as) ; }
    { String64    as      ; as._( testStdString       )                ; testParam( ut, testConstACharP, as) ; }
    { String64    as      ; as._( testMyString        )                ; testParam( ut, myStringBuf    , as) ; }

    { String64    as      ; as._( testConstStringLiteral  )            ; testParam( ut, A_CHAR("1")    , as) ; }
    { String64    as      ; as._( testConstString     )                ; testParam( ut, testConstACharP, as) ; }
    { String64    as      ; as._( testConstCString    )                ; testParam( ut, testConstACharP, as) ; }
    { String64    as      ; as._( testConstSubstring  )                ; testParam( ut, testConstACharP, as) ; }
    { String64    as      ; as._( testConstAString    )                ; testParam( ut, testConstACharP, as) ; }
    { String64    as      ; as._( testConstLocalString)                ; testParam( ut, testConstACharP, as) ; }
    { String64    as      ; as._( testConstStdString  )                ; testParam( ut, testConstACharP, as) ; }
    { String64    as      ; as._( testConstMyString   )                ; testParam( ut, myStringBuf    , as) ; }
}


//--------------------------------------------------------------------------------------------------
//--- Move constructors
//--------------------------------------------------------------------------------------------------
UT_METHOD( MoveConstructors )
{
    UT_INIT()

    lang::Report::GetDefault().PushHaltFlags( false, false );
    UT_PRINT( "One or more buffer warnings should follow" )


    // this test is more for debug stepping to check if the right constructors are used
    {   AString as       ( AString ( A_CHAR("Anonymous")));          as._ (A_CHAR("x"));   }

    {   String16    as   ( String16( A_CHAR("123456789_12345") ));   as._ (A_CHAR("x"));   }
    {   String16    as   ( String16( A_CHAR("123456789_123456")));   as._ (A_CHAR("x"));   }

    {   String16    as   ( AString ( A_CHAR("123456789_12345") ));   as._ (A_CHAR("x"));   }
    {   String16    as   ( AString ( A_CHAR("123456789_123456")));   as._ (A_CHAR("x"));   }

    {   String16    vola ( A_CHAR("123456789_123456"));
        String16    as   ( std::move( vola ) );
                    as._ ( A_CHAR("x"));
    }

    {
        String16    vola( A_CHAR("123456789_123456"));
        String16    as; as= std::move ( vola);
                    as._(A_CHAR("x"));
    }
    lang::Report::GetDefault().PopHaltFlags();

}

//--------------------------------------------------------------------------------------------------
//--- Test ConstructAndCompare
//--------------------------------------------------------------------------------------------------
UT_METHOD( ConstructAndCompare )
{
    UT_INIT()

    // String / std::string
    {
        std::string               std_string  =         "std::string"  ;
        std::basic_string<wchar>  std_wstring = A_WCHAR("std::wstring");
        std::basic_string<xchar>  std_xstring = A_XCHAR("std::xstring");
        NString                   nString     =         "NString"      ;
        WString                   wString     = A_WCHAR("WString"     );
        XString                   xString     = A_XCHAR("XString"     );

        std_string     = std::string(nString);     UT_TRUE ( std_string  ==  "NString"            )
                                                   UT_TRUE ( std_string  == std::string(nString)  )
                                                   UT_TRUE ( nString     == std_string            )
                                                   UT_TRUE ( std_string  == nString               )
                                                   UT_FALSE( nString     != std_string            )
                                                   UT_FALSE( std_string  != nString               )
                                                   UT_FALSE( nString     <  std_string            )
                                                   UT_FALSE( std_string  <  nString               )
                                                   UT_FALSE( nString     >  std_string            )
                                                   UT_FALSE( std_string  >  nString               )
                                                   UT_TRUE ( nString     <= std_string            )
                                                   UT_TRUE ( std_string  <= nString               )
                                                   UT_TRUE ( nString     >= std_string            )
                                                   UT_TRUE ( std_string  >= nString               )

        std_wstring    = std::basic_string<wchar>(wString);
                                                   UT_TRUE ( std_wstring == A_WCHAR("WString")    )
                                                   UT_TRUE ( std_wstring == std::basic_string<wchar>(wString) )
                                                   UT_TRUE ( wString     == std_wstring           )
                                                   UT_TRUE ( std_wstring== wString               )
                                                   UT_FALSE( wString    != std_wstring           )
                                                   UT_FALSE( std_wstring!= wString               )
                                                   UT_FALSE( wString    <  std_wstring           )
                                                   UT_FALSE( std_wstring<  wString               )
                                                   UT_FALSE( wString    >  std_wstring           )
                                                   UT_FALSE( std_wstring>  wString               )
                                                   UT_TRUE ( wString    <= std_wstring           )
                                                   UT_TRUE ( std_wstring<= wString               )
                                                   UT_TRUE ( wString    >= std_wstring           )
                                                   UT_TRUE ( std_wstring>= wString               )

        std_xstring    = std::basic_string<xchar>(xString);
                                                   UT_TRUE ( std_xstring == A_XCHAR("XString")   )
                                                   UT_TRUE ( std_xstring == std::basic_string<xchar>(xString) )
                                                   UT_TRUE ( xString     == std_xstring          )
                                                   UT_TRUE ( std_xstring  == xString             )
                                                   UT_FALSE( xString     != std_xstring          )
                                                   UT_FALSE( std_xstring  != xString             )
                                                   UT_FALSE( xString     <  std_xstring          )
                                                   UT_FALSE( std_xstring  <  xString             )
                                                   UT_FALSE( xString     >  std_xstring          )
                                                   UT_FALSE( std_xstring  >  xString             )
                                                   UT_TRUE ( xString     <= std_xstring          )
                                                   UT_TRUE ( std_xstring  <= xString             )
                                                   UT_TRUE ( xString     >= std_xstring          )
                                                   UT_TRUE ( std_xstring  >= xString             )
                                                }

    // CString / std::string
    {
        std::string               std_string  =         "std::string"  ;
        std::basic_string<wchar>  std_wstring = A_WCHAR("std::wstring");
        std::basic_string<xchar>  std_xstring = A_XCHAR("std::xstring");
        NCString                  nCString    =         "NString"      ;
        WCString                  wCString    = A_WCHAR("WString"     );
        XCString                  xCString    = A_XCHAR("XString"     );

        std_string     = nCString;                 UT_TRUE ( std_string  ==  "NString"             )
                                                   UT_TRUE ( std_string  == std::string(nCString)  )
                                                   UT_TRUE ( nCString    == std_string             )
                                                   UT_TRUE ( std_string  == nCString               )
                                                   UT_FALSE( nCString    != std_string             )
                                                   UT_FALSE( std_string  != nCString               )
                                                   UT_FALSE( nCString    <  std_string             )
                                                   UT_FALSE( std_string  <  nCString               )
                                                   UT_FALSE( nCString    >  std_string             )
                                                   UT_FALSE( std_string  >  nCString               )
                                                   UT_TRUE ( nCString    <= std_string             )
                                                   UT_TRUE ( std_string  <= nCString               )
                                                   UT_TRUE ( nCString    >= std_string             )
                                                   UT_TRUE ( std_string  >= nCString               )

        std_wstring    = wCString;                 UT_TRUE ( std_wstring == A_WCHAR("WString")     )
                                                   UT_TRUE ( std_wstring == std::basic_string<wchar>(wCString) )
                                                   UT_TRUE ( wCString    == std_wstring            )
                                                   UT_TRUE ( std_wstring == wCString               )
                                                   UT_FALSE( wCString    != std_wstring            )
                                                   UT_FALSE( std_wstring != wCString               )
                                                   UT_FALSE( wCString    <  std_wstring            )
                                                   UT_FALSE( std_wstring <  wCString               )
                                                   UT_FALSE( wCString    >  std_wstring            )
                                                   UT_FALSE( std_wstring >  wCString               )
                                                   UT_TRUE ( wCString    <= std_wstring            )
                                                   UT_TRUE ( std_wstring <= wCString               )
                                                   UT_TRUE ( wCString    >= std_wstring            )
                                                   UT_TRUE ( std_wstring >= wCString               )

        std_xstring    = xCString;                 UT_TRUE ( std_xstring == A_XCHAR("XString")     )
                                                   UT_TRUE ( std_xstring == std::basic_string<xchar>(xCString) )
                                                   UT_TRUE ( xCString    == std_xstring            )
                                                   UT_TRUE ( std_xstring == xCString               )
                                                   UT_FALSE( xCString    != std_xstring            )
                                                   UT_FALSE( std_xstring != xCString               )
                                                   UT_FALSE( xCString    <  std_xstring            )
                                                   UT_FALSE( std_xstring <  xCString               )
                                                   UT_FALSE( xCString    >  std_xstring            )
                                                   UT_FALSE( std_xstring >  xCString               )
                                                   UT_TRUE ( xCString    <= std_xstring            )
                                                   UT_TRUE ( std_xstring <= xCString               )
                                                   UT_TRUE ( xCString    >= std_xstring            )
                                                   UT_TRUE ( std_xstring >= xCString               )
    }

    // AString / std::string
    {
        std::string               std_string  =         "std::string"  ;
        std::basic_string<wchar>  std_wstring = A_WCHAR("std::wstring");
        std::basic_string<xchar>  std_xstring = A_XCHAR("std::xstring");
        NAString                  nAString    (         "NString"       );
        WAString                  wAString    ( A_WCHAR("WString"     ) );
        XAString                  xAString    ( A_XCHAR("XString"     ) );

        std_string     = nAString;                 UT_TRUE ( std_string  == "NString"              )
                                                   UT_TRUE ( std_string  == std::string(nAString)  )
                                                   UT_TRUE ( nAString    == std_string             )
                                                   UT_TRUE ( std_string  == nAString               )
                                                   UT_FALSE( nAString    != std_string             )
                                                   UT_FALSE( std_string  != nAString               )
                                                   UT_FALSE( nAString    <  std_string             )
                                                   UT_FALSE( std_string  <  nAString               )
                                                   UT_FALSE( nAString    >  std_string             )
                                                   UT_FALSE( std_string  >  nAString               )
                                                   UT_TRUE ( nAString    <= std_string             )
                                                   UT_TRUE ( std_string  <= nAString               )
                                                   UT_TRUE ( nAString    >= std_string             )
                                                   UT_TRUE ( std_string  >= nAString               )

        std_wstring    = wAString;                 UT_TRUE ( std_wstring == A_WCHAR("WString")     )
                                                   UT_TRUE ( std_wstring == std::basic_string<wchar>(wAString) )
                                                   UT_TRUE ( wAString    == std_wstring            )
                                                   UT_TRUE ( std_wstring == wAString               )
                                                   UT_FALSE( wAString    != std_wstring            )
                                                   UT_FALSE( std_wstring != wAString               )
                                                   UT_FALSE( wAString    <  std_wstring            )
                                                   UT_FALSE( std_wstring <  wAString               )
                                                   UT_FALSE( wAString    >  std_wstring            )
                                                   UT_FALSE( std_wstring >  wAString               )
                                                   UT_TRUE ( wAString    <= std_wstring            )
                                                   UT_TRUE ( std_wstring <= wAString               )
                                                   UT_TRUE ( wAString    >= std_wstring            )
                                                   UT_TRUE ( std_wstring >= wAString               )

        std_xstring    = xAString;                 UT_TRUE ( std_xstring == A_XCHAR("XString")     )
                                                   UT_TRUE ( std_xstring == std::basic_string<xchar>(xAString) )
                                                   UT_TRUE ( xAString    == std_xstring            )
                                                   UT_TRUE ( std_xstring == xAString               )
                                                   UT_TRUE ( std_xstring == xAString               )
                                                   UT_FALSE( xAString    != std_xstring            )
                                                   UT_FALSE( std_xstring != xAString               )
                                                   UT_FALSE( xAString    <  std_xstring            )
                                                   UT_FALSE( std_xstring <  xAString               )
                                                   UT_FALSE( xAString    >  std_xstring            )
                                                   UT_FALSE( std_xstring >  xAString               )
                                                   UT_TRUE ( xAString    <= std_xstring            )
                                                   UT_TRUE ( std_xstring <= xAString               )
                                                   UT_TRUE ( xAString    >= std_xstring            )
                                                   UT_TRUE ( std_xstring >= xAString               )
    }

#if defined(ALIB_GCC)
// with GCC and enabling C++ 20 language standard, a quite strange compilation error occurs.
// The code generated calls the right compare function that definitely does not involve an
// "integral constant 0", what the message claims to do...
_Pragma("GCC diagnostic push")
_Pragma("GCC diagnostic ignored \"-Wbool-compare\"")
#endif
// String / std::string_view
    {
        std::string_view               std_string_view  =         "std::string"  ;
        std::basic_string_view<wchar>  std_wstring_view = A_WCHAR("std::wstring");
        std::basic_string_view<xchar>  std_xstring_view = A_XCHAR("std::xstring");
        NString                        nString          =         "NString"      ;
        WString                        wString          = A_WCHAR("WString"     );
        XString                        xString          = A_XCHAR("XString"     );

        std_string_view     = nString;             UT_TRUE ( std_string_view ==  "NString"               )
                                                   UT_TRUE ( std_string_view == std::string_view(nString))
                                                   UT_TRUE ( nString         == std_string_view          )
                                                   UT_TRUE ( std_string_view == nString            )
                                                   UT_FALSE( nString         != std_string_view    )
                                                   UT_FALSE( std_string_view != nString            )
                                                   UT_FALSE( nString         <  std_string_view    )
                                                   UT_FALSE( std_string_view <  nString            )
                                                   UT_FALSE( nString         >  std_string_view    )
                                                   UT_FALSE( std_string_view >  nString            )
                                                   UT_TRUE ( nString         <= std_string_view    )
                                                   UT_TRUE ( std_string_view <= nString            )
                                                   UT_TRUE ( nString         >= std_string_view    )
                                                   UT_TRUE ( std_string_view >= nString            )

        std_wstring_view    = wString;             UT_TRUE ( std_wstring_view== A_WCHAR("WString") )
                                                   UT_TRUE ( std_wstring_view== wString            )
                                                   UT_TRUE ( wString         == std_wstring_view   )
                                                   UT_TRUE ( std_wstring_view== wString            )
                                                   UT_FALSE( wString         != std_wstring_view   )
                                                   UT_FALSE( std_wstring_view!= wString            )
                                                   UT_FALSE( wString         <  std_wstring_view   )
                                                   UT_FALSE( std_wstring_view<  wString            )
                                                   UT_FALSE( wString         >  std_wstring_view   )
                                                   UT_FALSE( std_wstring_view>  wString            )
                                                   UT_TRUE ( wString         <= std_wstring_view   )
                                                   UT_TRUE ( std_wstring_view<= wString            )
                                                   UT_TRUE ( wString         >= std_wstring_view   )
                                                   UT_TRUE ( std_wstring_view>= wString            )

        std_xstring_view    = xString;             UT_TRUE( std_xstring_view == A_XCHAR("XString") )
                                                   UT_TRUE( std_xstring_view == xString )
                                                   UT_TRUE( xString          == std_xstring_view   )
                                                   UT_TRUE ( std_xstring_view== xString            )
                                                   UT_FALSE( xString         != std_xstring_view   )
                                                   UT_FALSE( std_xstring_view!= xString            )
                                                   UT_FALSE( xString         <  std_xstring_view   )
                                                   UT_FALSE( std_xstring_view<  xString            )
                                                   UT_FALSE( xString         >  std_xstring_view   )
                                                   UT_FALSE( std_xstring_view>  xString            )
                                                   UT_TRUE ( xString         <= std_xstring_view   )
                                                   UT_TRUE ( std_xstring_view<= xString            )
                                                   UT_TRUE ( xString         >= std_xstring_view   )
                                                   UT_TRUE ( std_xstring_view>= xString            )

    }

    // CString / std::string_view
    {
        std::string_view               std_string_view  =         "std::string"  ;
        std::basic_string_view<wchar>  std_wstring_view = A_WCHAR("std::wstring");
        std::basic_string_view<xchar>  std_xstring_view = A_XCHAR("std::xstring");
        NCString                       nCString         =         "NString"      ;
        WCString                       wCString         = A_WCHAR("WString"     );
        XCString                       xCString         = A_XCHAR("XString"     );

        std_string_view     = nCString;            UT_TRUE ( std_string_view ==  "NString"         )
                                                   UT_TRUE ( std_string_view == nCString           )
                                                   UT_TRUE ( nCString        == std_string_view    )
                                                   UT_TRUE ( std_string_view == nCString           )
                                                   UT_FALSE( nCString        != std_string_view    )
                                                   UT_FALSE( std_string_view != nCString           )
                                                   UT_FALSE( nCString        <  std_string_view    )
                                                   UT_FALSE( std_string_view <  nCString           )
                                                   UT_FALSE( nCString        >  std_string_view    )
                                                   UT_FALSE( std_string_view >  nCString           )
                                                   UT_TRUE ( nCString        <= std_string_view    )
                                                   UT_TRUE ( std_string_view <= nCString           )
                                                   UT_TRUE ( nCString        >= std_string_view    )
                                                   UT_TRUE ( std_string_view >= nCString           )

//Err:  nCString=          std_string_view;
/*OK: */nCString= NCString(std_string_view);

        std_wstring_view    = wCString;            UT_TRUE ( std_wstring_view== A_WCHAR("WString") )
                                                   UT_TRUE ( std_wstring_view== wCString           )
                                                   UT_TRUE ( wCString        == std_wstring_view   )
                                                   UT_TRUE ( std_wstring_view== wCString           )
                                                   UT_TRUE ( wCString        == std_wstring_view   )
                                                   UT_TRUE ( std_wstring_view== wCString           )
                                                   UT_FALSE( wCString        != std_wstring_view   )
                                                   UT_FALSE( std_wstring_view!= wCString           )
                                                   UT_FALSE( wCString        <  std_wstring_view   )
                                                   UT_FALSE( std_wstring_view<  wCString           )
                                                   UT_FALSE( wCString        >  std_wstring_view   )
                                                   UT_FALSE( std_wstring_view>  wCString           )
                                                   UT_TRUE ( wCString        <= std_wstring_view   )
                                                   UT_TRUE ( std_wstring_view<= wCString           )
                                                   UT_TRUE ( wCString        >= std_wstring_view   )
                                                   UT_TRUE ( std_wstring_view>= wCString           )

//Err:  wCString=          std_wstring_view;
/*OK: */wCString= WCString(std_wstring_view);

        std_xstring_view    = xCString;            UT_TRUE ( std_xstring_view== A_XCHAR("XString") )
                                                   UT_TRUE ( std_xstring_view== xCString           )
                                                   UT_TRUE ( xCString        == std_xstring_view   )
                                                   UT_TRUE ( std_xstring_view== xCString           )
                                                   UT_FALSE( xCString        != std_xstring_view   )
                                                   UT_FALSE( std_xstring_view!= xCString           )
                                                   UT_FALSE( xCString        <  std_xstring_view   )
                                                   UT_FALSE( std_xstring_view<  xCString           )
                                                   UT_FALSE( xCString        >  std_xstring_view   )
                                                   UT_FALSE( std_xstring_view>  xCString           )
                                                   UT_TRUE ( xCString        <= std_xstring_view   )
                                                   UT_TRUE ( std_xstring_view<= xCString           )
                                                   UT_TRUE ( xCString        >= std_xstring_view   )
                                                   UT_TRUE ( std_xstring_view>= xCString           )

//Err:  xCString=          std_xstring_view;
/*OK: */xCString= XCString(std_xstring_view);
    }

    // AString / std::string_view
    {
        std::string_view               std_string_view  =         "std::string"  ;
        std::basic_string_view<wchar>  std_wstring_view = A_WCHAR("std::wstring");
        std::basic_string_view<xchar>  std_xstring_view = A_XCHAR("std::xstring");
        NAString                       nAString         (         "NString"       );
        WAString                       wAString         (        L"WString"       );
        XAString                       xAString         ( A_XCHAR("XString"     ) );

        std_string_view     = nAString;            UT_TRUE ( std_string_view ==  "NString"         )
                                                   UT_TRUE ( std_string_view == std::string_view(static_cast<NString>(nAString))  )
                                                   UT_TRUE ( nAString        == std_string_view    )
                                                   UT_TRUE ( std_string_view == nAString           )
                                                   UT_FALSE( nAString        != std_string_view    )
                                                   UT_FALSE( std_string_view != nAString           )
                                                   UT_FALSE( nAString        <  std_string_view    )
                                                   UT_FALSE( std_string_view <  nAString           )
                                                   UT_FALSE( nAString        >  std_string_view    )
                                                   UT_FALSE( std_string_view >  nAString           )
                                                   UT_TRUE ( nAString        <= std_string_view    )
                                                   UT_TRUE ( std_string_view <= nAString           )
                                                   UT_TRUE ( nAString        >= std_string_view    )
                                                   UT_TRUE ( std_string_view >= nAString           )

        std_wstring_view    = wAString;            UT_TRUE ( std_wstring_view== A_WCHAR("WString") )
                                                   UT_TRUE ( std_wstring_view== wAString           )
                                                   UT_TRUE ( wAString        == std_wstring_view   )
                                                   UT_TRUE ( std_wstring_view== wAString           )
                                                   UT_TRUE ( wAString        == std_wstring_view   )
                                                   UT_TRUE ( std_wstring_view== wAString           )
                                                   UT_FALSE( wAString        != std_wstring_view   )
                                                   UT_FALSE( std_wstring_view!= wAString           )
                                                   UT_FALSE( wAString        <  std_wstring_view   )
                                                   UT_FALSE( std_wstring_view<  wAString           )
                                                   UT_FALSE( wAString        >  std_wstring_view   )
                                                   UT_FALSE( std_wstring_view>  wAString           )
                                                   UT_TRUE ( wAString        <= std_wstring_view   )
                                                   UT_TRUE ( std_wstring_view<= wAString           )
                                                   UT_TRUE ( wAString        >= std_wstring_view   )
                                                   UT_TRUE ( std_wstring_view>= wAString           )

        std_xstring_view    = xAString;            UT_TRUE ( std_xstring_view== A_XCHAR("XString") )
                                                   UT_TRUE ( std_xstring_view== xAString           )
                                                   UT_TRUE ( xAString        == std_xstring_view   )
                                                   UT_TRUE ( std_xstring_view== xAString           )
                                                   UT_FALSE( xAString        != std_xstring_view   )
                                                   UT_FALSE( std_xstring_view!= xAString           )
                                                   UT_FALSE( xAString        <  std_xstring_view   )
                                                   UT_FALSE( std_xstring_view<  xAString           )
                                                   UT_FALSE( xAString        >  std_xstring_view   )
                                                   UT_FALSE( std_xstring_view>  xAString           )
                                                   UT_TRUE ( xAString        <= std_xstring_view   )
                                                   UT_TRUE ( std_xstring_view<= xAString           )
                                                   UT_TRUE ( xAString        >= std_xstring_view   )
                                                   UT_TRUE ( std_xstring_view>= xAString           )
    }
}

#if defined(ALIB_GCC)
 _Pragma("GCC diagnostic pop")
#endif


#if !defined(ALIB_UT_REDUCED_COMPILE_TIME)

UT_METHOD( CompareOperators )
{
    UT_INIT()
    {
        NString ns= A_NCHAR("ABC");    checkComparisonN(ut, ns);
        WString ws= A_WCHAR("ABC");    checkComparisonW(ut, ws);
        XString xs= A_XCHAR("ABC");    checkComparisonX(ut, xs);

        NCString ncs= A_NCHAR("ABC");    checkComparisonN(ut, ncs);
        WCString wcs= A_WCHAR("ABC");    checkComparisonW(ut, wcs);
        XCString xcs= A_XCHAR("ABC");    checkComparisonX(ut, xcs);

        NString32        ns32(A_NCHAR("ABC"));    checkComparisonN(ut, ns32);
        WString32        ws32(A_WCHAR("ABC"));    checkComparisonW(ut, ws32);
        XLocalString<32> xs32(A_XCHAR("ABC"));    checkComparisonX(ut, xs32);
    }

    {
        NString  ns= A_NCHAR("ABC");  NString  nsC1= A_NCHAR("ABC"); NString  nsC2= A_NCHAR("ABX");
        WString  ws= A_WCHAR("ABC");  WString  wsC1= A_WCHAR("ABC"); WString  wsC2= A_WCHAR("ABX");
        XString  xs= A_XCHAR("ABC");  XString  xsC1= A_XCHAR("ABC"); XString  xsC2= A_XCHAR("ABX");

        NCString nc= A_NCHAR("ABC");  NCString ncC1= A_NCHAR("ABC"); NCString ncC2= A_NCHAR("ABX");
        WCString wc= A_WCHAR("ABC");  WCString wcC1= A_WCHAR("ABC"); WCString wcC2= A_WCHAR("ABX");
        XCString xc= A_XCHAR("ABC");  XCString xcC1= A_XCHAR("ABC"); XCString xcC2= A_XCHAR("ABX");

        NAString na; na << A_NCHAR("ABC");  NAString naC1; naC1 << A_NCHAR("ABC"); NAString naC2;  naC2 << A_NCHAR("ABX");
        WAString wa; wa << A_WCHAR("ABC");  WAString waC1; waC1 << A_WCHAR("ABC"); WAString waC2;  waC2 << A_WCHAR("ABX");
        XAString xa; xa << A_XCHAR("ABC");  XAString xaC1; xaC1 << A_XCHAR("ABC"); XAString xaC2;  xaC2 << A_XCHAR("ABX");

        NString32 nl; nl << A_NCHAR("ABC");  NString32 nlC1; nlC1 << A_NCHAR("ABC"); NString32 nlC2;  nlC2 << A_NCHAR("ABX");
        WString32 wl; wl << A_WCHAR("ABC");  WString32 wlC1; wlC1 << A_WCHAR("ABC"); WString32 wlC2;  wlC2 << A_WCHAR("ABX");
        XLocalString<32> xl; xl << A_XCHAR("ABC");  XLocalString<32> xlC1; xlC1 << A_XCHAR("ABC"); XLocalString<32> xlC2;  xlC2 << A_XCHAR("ABX");

        // (String x String)
        checkComparison(ut, ns     , nsC1      , nsC2     );
        checkComparison(ut, ws     , wsC1      , wsC2     );
        checkComparison(ut, xs     , xsC1      , xsC2     );

        // (String x CString)
        checkComparison(ut, ns     , naC1      , naC2     );
        checkComparison(ut, ws     , waC1      , waC2     );
        checkComparison(ut, xs     , xaC1      , xaC2     );

        // (String x AString)
        checkComparison(ut, ns     , naC1      , naC2     );
        checkComparison(ut, ws     , waC1      , waC2     );
        checkComparison(ut, xs     , xaC1      , xaC2     );

        // (String x LString)
        checkComparison(ut, ns     , nlC1      , nlC2     );
        checkComparison(ut, ws     , wlC1      , wlC2     );
        checkComparison(ut, xs     , xlC1      , xlC2     );


        // (CString x CString)
        checkComparison(ut, nc     , ncC1      , ncC2     );
        checkComparison(ut, wc     , wcC1      , wcC2     );
        checkComparison(ut, xc     , xcC1      , xcC2     );

        // (CString x AString)
        checkComparison(ut, nc     , naC1      , naC2     );
        checkComparison(ut, wc     , waC1      , waC2     );
        checkComparison(ut, xc     , xaC1      , xaC2     );

        // (CString x LString)
        checkComparison(ut, nc     , nlC1      , nlC2     );
        checkComparison(ut, wc     , wlC1      , wlC2     );
        checkComparison(ut, xc     , xlC1      , xlC2     );

        // (AString x LString)
        checkComparison(ut, na     , nlC1      , nlC2     );
        checkComparison(ut, wa     , wlC1      , wlC2     );
        checkComparison(ut, xa     , xlC1      , xlC2     );

    }
} //UT_METHOD( CompareOperators )
#endif // !defined(ALIB_UT_REDUCED_COMPILE_TIME)

#include "unittests/aworx_unittests_end.hpp"

} //namespace


#endif // ALIB_UT_STRINGS
