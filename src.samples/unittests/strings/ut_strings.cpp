// #################################################################################################
//  AWorx ALib Unit Tests
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib_precompile.hpp"
#include "alib_test_selection.hpp"
#if ALIB_UT_STRINGS

#include "ALib.ALox.H"
#include "ALib.Time.H"
#include "ALib.Characters.Functions.H"
#include "ALib.Strings.H"
#include "ALib.Strings.Calendar.H"
#include "ALib.Compatibility.StdStrings.H"

#include <iostream>
#include <sstream>
#include <iomanip>

#include "aworx_unittests.hpp"
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
    ALIB_CHARACTER_ARRAY_IMPL_BUFFER( tstn::MyString, character, return         src.GetMyBuffer(); )
    ALIB_CHARACTER_ARRAY_IMPL_LENGTH( tstn::MyString, character, return integer(src.GetMyLength());)
}

#if ALIB_CAMP
namespace strings {
    template<> struct AppendableTraits<alib::time::DateTime, character, lang::HeapAllocator>
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

#include "ALib.Lang.CIFunctions.H"

//--------------------------------------------------------------------------------------------------
//--- Test CompareOperators
//--------------------------------------------------------------------------------------------------
#if !defined(ALIB_UT_REDUCED_COMPILE_TIME)
namespace {
    template<typename TALibString>
    void checkComparisonN(AWorxUnitTesting& ut, TALibString& s)
    {
        UT_TRUE (s == A_NCHAR("ABC"))    UT_TRUE(A_NCHAR("ABC") == s)
        UT_FALSE(s != A_NCHAR("ABC"))    UT_FALSE(A_NCHAR("ABC") != s)
        UT_FALSE(s <  A_NCHAR("ABC"))    UT_FALSE(A_NCHAR("ABC") < s)
        UT_TRUE (s <= A_NCHAR("ABC"))    UT_TRUE(A_NCHAR("ABC") <= s)
        UT_FALSE(s >  A_NCHAR("ABC"))    UT_FALSE(A_NCHAR("ABC") > s)
        UT_TRUE (s >= A_NCHAR("ABC"))    UT_TRUE(A_NCHAR("ABC") >= s)

        UT_FALSE(s == A_NCHAR("ABX"))    UT_FALSE(A_NCHAR("ABX") == s)
        UT_TRUE (s != A_NCHAR("ABX"))    UT_TRUE(A_NCHAR("ABX") != s)
        UT_TRUE (s <  A_NCHAR("ABX"))    UT_FALSE(A_NCHAR("ABX") < s)
        UT_TRUE (s <= A_NCHAR("ABX"))    UT_FALSE(A_NCHAR("ABX") <= s)
        UT_FALSE(s >  A_NCHAR("ABX"))    UT_TRUE(A_NCHAR("ABX") > s)
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

void testParam            ( AWorxUnitTesting& ut, const character* exp, const String& s )
{
    #if ALIB_DEBUG_STRINGS
        ALIB_STRING_DBG_CHK(&s)
    #endif
    String32 as(s);
    UT_EQ( exp, String(as)  )
    UT_TRUE( as.Equals(exp) )
}


void testParamCString( AWorxUnitTesting& ut, const character* exp, const CString& cs )
{
    #if ALIB_DEBUG_STRINGS
        ALIB_STRING_DBG_CHK(&cs)
    #endif
    String32 as(cs);
    UT_EQ( exp, String(as)  )
    UT_TRUE( as.Equals(exp) )
}

void testParamLocalString( AWorxUnitTesting& ut, const character* exp, const String64& ls )
{
    #if ALIB_DEBUG_STRINGS
        ALIB_STRING_DBG_CHK(&ls)
    #endif
    String32 as(ls);
    UT_EQ( exp, String(as)  )
    UT_TRUE( as.Equals(exp) )
}

void testParamSubstring( AWorxUnitTesting& ut, const character* exp, const Substring& ss )
{
    #if ALIB_DEBUG_STRINGS
        ALIB_STRING_DBG_CHK(&as)
    #endif
    String32 asTemp;
    asTemp._(ss);
    UT_EQ( exp, String(asTemp)  )
    UT_TRUE( ss.Equals(exp) )
}

#include "ALib.Lang.CIMethods.H"

//--------------------------------------------------------------------------------------------------
//--- Explicit constructors
//--------------------------------------------------------------------------------------------------
UT_METHOD( ConstructorsExplicit )
{
    UT_INIT()

    // before we begin...
    static_assert( std::is_nothrow_move_constructible<String>   ::value, "String has to be move constructable with no assertions");
    static_assert( std::is_nothrow_move_constructible<Substring>::value, "Substring has to be move constructable with no assertions");
    static_assert( std::is_nothrow_move_constructible<CString>  ::value, "CString has to be move constructable with no assertions");
    static_assert( std::is_nothrow_move_constructible<AString>  ::value, "AString has to be move constructable with no assertions");

    static_assert( std::same_as<AString::value_type , character>, "AString value_type not or wrongly defined");
    static_assert( std::same_as<WAString::value_type, wchar>    , "AString value_type not or wrongly defined");


    #define CLITERAL             A_CHAR("TEST\u03B1\u03B2\u03B3\u03B4\u03B5")
    #define U8LITERAL            u8"TEST\u03B1\u03B2\u03B3\u03B4\u03B5"
          character              testAChar=            '@';
    const character              testConstAChar=       '@';
    const character*             testConstACharP=      CLITERAL;
          character*             testACharP=           const_cast<character*>( testConstACharP );

    const char8_t*               testConstUCharP=      U8LITERAL;
          char8_t*               testUCharP=           const_cast<char8_t*>( testConstUCharP );
    (void) testUCharP;


    std::basic_string<character> testStdString        (testConstACharP);
    String                   testString               (testConstACharP);
    AString                  testAString              (testConstACharP);
    character                testStringLiteral[2]     (A_CHAR("1"));
    Substring                testSubstring            =testConstACharP;
    String32                 testLocalString          ( testConstACharP );
    tstn::MyString           testMyString;

    const std::basic_string<character>  testConstStdString   (testConstACharP);
    const String             testConstString                 (testConstACharP);
    const AString            testConstAString                (testConstACharP);
    const character          testConstStringLiteral[2](A_CHAR("1"));
    const Substring          testConstSubstring     =testConstACharP;
    const String32           testConstLocalString  ( testConstACharP );
    const tstn::MyString     testConstMyString;

    const character* myStringBuf= testMyString.GetMyBuffer();

    // this has to fail compilation:
    // { String     as( 42  );  }

    // String
    testParam ( ut,testConstACharP, String( CLITERAL                 ) );
    testParam ( ut,testConstACharP, String( testConstACharP          ) );
    testParam ( ut,testConstACharP, String( testACharP               ) );
    #if !ALIB_CHARACTERS_WIDE
    testParam ( ut,testConstACharP, String( U8LITERAL                ) );
    testParam ( ut,testConstACharP, String( testConstUCharP          ) );
    testParam ( ut,testConstACharP, String( testUCharP               ) );
    #endif

    testParam ( ut,testConstACharP, String( testString               ) );
    testParam ( ut,A_CHAR("1")    , String( testStringLiteral        ) );
    testParam ( ut,testConstACharP, String( testSubstring            ) );
    testParam ( ut,testConstACharP, String( testAString              ) );
    testParam ( ut,testConstACharP, String( testLocalString          ) );
    testParam ( ut,testConstACharP, String( testStdString            ) );
    testParam ( ut,myStringBuf    , String( testMyString             ) );

    testParam ( ut,testConstACharP, String( testConstString          ) );
    testParam ( ut,A_CHAR("1")    , String( testConstStringLiteral   ) );
    testParam ( ut,testConstACharP, String( testConstSubstring       ) );
    testParam ( ut,testConstACharP, String( testConstAString         ) );
    testParam ( ut,testConstACharP, String( testConstLocalString     ) );
    testParam ( ut,testConstACharP, String( testConstStdString       ) );
    testParam ( ut,myStringBuf    , String( testConstMyString        ) );

    // CString (no Strings non-terminated strings  )
    testParam ( ut,testConstACharP, CString( CLITERAL                )) ;
    testParam ( ut,testConstACharP, CString( testConstACharP         )) ;
    testParam ( ut,testConstACharP, CString( testACharP              )) ;
    #if !ALIB_CHARACTERS_WIDE
    testParam ( ut,testConstACharP, CString( U8LITERAL               )) ;
    testParam ( ut,testConstACharP, CString( testConstUCharP         )) ;
    testParam ( ut,testConstACharP, CString( testUCharP              )) ;
    #endif
    
    testParam( ut, testConstACharP, CString( testString              ) );
    testParam( ut, A_CHAR("1")    , CString( testStringLiteral       ) );
  //testParam( ut, testConstACharP, CString( testSubstring           ) );
    testParam( ut, testConstACharP, CString( testAString             ) );
    testParam( ut, testConstACharP, CString( testLocalString         ) );
    testParam( ut, testConstACharP, CString( testStdString           ) );
  //testParam( ut, myStringBuf    , CString( testMyString            ) );

    testParam( ut, testConstACharP, CString( testConstString         ) );
    testParam( ut, A_CHAR("1")    , CString( testConstStringLiteral  ) );
  //testParam( ut, testConstACharP, CString( testConstSubstring      ) );
    testParam( ut, testConstACharP, CString( testConstAString        ) );
    testParam( ut, testConstACharP, CString( testConstLocalString    ) );
    testParam( ut, testConstACharP, CString( testConstStdString      ) );
  //testParam( ut, myStringBuf    , CString( testConstMyString       ) );

    testParam( ut, testConstACharP, CString(&testString              ) );
  //testParam( ut, A_CHAR("1")    , CString(&testStringLiteral       ) );
    testParam( ut, testConstACharP, CString(&testString              ) );
    testParam( ut, testConstACharP, CString(&testAString             ) );
    testParam( ut, testConstACharP, CString(&testLocalString         ) );
    testParam( ut, testConstACharP, CString(&testStdString           ) );
  //testParam( ut, myStringBuf    , CString(&testMyString            ) );

    testParam( ut, testConstACharP, CString(&testConstString         ) );
  //testParam( ut, A_CHAR("1")    , CString(&testConstStringLiteral  ) );
  //testParam( ut, testConstACharP, CString(&testConstSubstring      ) );
    testParam( ut, testConstACharP, CString(&testConstAString        ) );
    testParam( ut, testConstACharP, CString(&testConstLocalString    ) );
    testParam( ut, testConstACharP, CString(&testConstStdString      ) );
  //testParam( ut, myStringBuf    , CString(&testConstMyString       ) );

    // Excplict CString with errors
    {
        ALIB_DBG(assert::GetHaltFlagAndCounters().HaltOnErrors=false;)

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

        ALIB_DBG(assert::GetHaltFlagAndCounters().HaltOnErrors=true;)
    }

    // AString
    testParam( ut, testConstACharP, AString(  "TEST\u03B1\u03B2\u03B3\u03B4\u03B5" ) );
    testParam( ut, testConstACharP, AString( L"TEST\u03B1\u03B2\u03B3\u03B4\u03B5" ) );
  //testParam( ut, testConstACharP, AString( testACharP              ) );
    testParam( ut, testConstACharP, AString( (String(testACharP))    ) );
    testParam( ut, testConstACharP, AString( testConstACharP         ) );
    testParam ( ut,testConstACharP, AString( U8LITERAL               ) );
    testParam ( ut,testConstACharP, AString( testConstUCharP         ) );
  //testParam ( ut,testConstACharP, AString( testUCharP              ) );
    testParam( ut, A_CHAR("@"),     AString( testAChar               ) );
    testParam( ut, testConstACharP, AString( U8LITERAL               ) );
    testParam( ut, testConstACharP, AString( testConstUCharP         ) );
  //testParam( ut, testConstACharP, AString( testUCharP              ) );
    testParam( ut, testConstACharP, AString( (String(CLITERAL))      ) );
    testParam( ut, testConstACharP, AString( testString              ) );
    testParam( ut, A_CHAR("1")    , AString( testStringLiteral       ) );
    testParam( ut, testConstACharP, AString( testSubstring           ) );
    testParam( ut, testConstACharP, AString( testAString             ) );
    testParam( ut, testConstACharP, AString( testLocalString         ) );
    testParam( ut, testConstACharP, AString( testStdString           ) );
    testParam( ut, myStringBuf    , AString( testMyString            ) );

    testParam( ut, A_CHAR("@"),     AString( testConstAChar          ) );
    testParam( ut, testConstACharP, AString( testConstString         ) );
    testParam( ut, A_CHAR("1")    , AString( testConstStringLiteral  ) );
    testParam( ut, testConstACharP, AString( testConstSubstring      ) );
    testParam( ut, testConstACharP, AString( testConstAString        ) );
    testParam( ut, testConstACharP, AString( testConstLocalString    ) );
    testParam( ut, testConstACharP, AString( testConstStdString      ) );
    testParam( ut, myStringBuf    , AString( testConstMyString       ) );

    // LocalString
    testParam( ut, testConstACharP, String64(  "TEST\u03B1\u03B2\u03B3\u03B4\u03B5"  ) );
    testParam( ut, testConstACharP, String64( L"TEST\u03B1\u03B2\u03B3\u03B4\u03B5"  ) );
  //testParam( ut, testConstACharP, String64( testACharP                  ) );
    testParam( ut, testConstACharP, String64( (String(testACharP))        ) );
    testParam( ut, testConstACharP, String64( testConstACharP             ) );
    testParam( ut, A_CHAR("@")    , String64( testAChar                   ) );
    testParam( ut, testConstACharP, String64( U8LITERAL                   ) );
    testParam( ut, testConstACharP, String64( testConstUCharP             ) );
 // testParam( ut, testConstUCharP, String64( testUCharP                  ) );
    testParam( ut, testConstACharP, String64( (String(CLITERAL))          ) );
    testParam( ut, testConstACharP, String64( testString                  ) );
    testParam( ut, A_CHAR("1")    , String64( testStringLiteral           ) );
    testParam( ut, testConstACharP, String64( testSubstring               ) );
    testParam( ut, testConstACharP, String64( testAString                 ) );
    testParam( ut, testConstACharP, String64( testLocalString             ) );
    testParam( ut, testConstACharP, String64( testStdString               ) );
    testParam( ut, myStringBuf    , String64( testMyString                ) );
                                                                             
    testParam( ut, A_CHAR("@")    , String64( testConstAChar              ) );
    testParam( ut, testConstACharP, String64( testConstString             ) );
    testParam( ut, A_CHAR("1")    , String64( testConstStringLiteral      ) );
    testParam( ut, testConstACharP, String64( testConstSubstring          ) );
    testParam( ut, testConstACharP, String64( testConstAString            ) );
    testParam( ut, testConstACharP, String64( testConstLocalString        ) );
    testParam( ut, testConstACharP, String64( testConstStdString          ) );
    testParam( ut, myStringBuf    , String64( testConstMyString           ) );

    // Substring
    testParam( ut, testConstACharP, Substring( CLITERAL                   ) );
    testParam( ut, testConstACharP, Substring( testConstACharP            ) );
    testParam( ut, testConstACharP, Substring( testACharP                 ) );
    #if !ALIB_CHARACTERS_WIDE
    testParam( ut, testConstACharP, Substring( U8LITERAL                  ) );
    testParam( ut, testConstACharP, Substring( testConstUCharP            ) );
    #endif 
    testParam( ut, testConstACharP, Substring( testString                 ) );
    testParam( ut, A_CHAR("1")    , Substring( testStringLiteral          ) );
    testParam( ut, testConstACharP, Substring( testSubstring              ) );
    testParam( ut, testConstACharP, Substring( testAString                ) );
    testParam( ut, testConstACharP, Substring( testLocalString            ) );
    testParam( ut, testConstACharP, Substring( testStdString              ) );
    testParam( ut, myStringBuf    , Substring( testMyString               ) );

    testParam( ut, testConstACharP, Substring( testConstString            ) );
    testParam( ut, A_CHAR("1")    , Substring( testConstStringLiteral     ) );
    testParam( ut, testConstACharP, Substring( testConstSubstring         ) );
    testParam( ut, testConstACharP, Substring( testConstAString           ) );
    testParam( ut, testConstACharP, Substring( testConstLocalString       ) );
    testParam( ut, testConstACharP, Substring( testConstStdString         ) );
    testParam( ut, myStringBuf    , Substring( testConstMyString          ) );
}


UT_METHOD( ConstructorsImplicit )
{
    UT_INIT()

   const character*             testConstCharP  = CLITERAL      ;
   character*                   testCharP       = const_cast<character*>( testConstCharP );
   const char8_t*               testConstUCharP = U8LITERAL;
   char8_t*                     testUCharP      = const_cast<char8_t*>( testConstUCharP );
   (void) testCharP;
   (void) testUCharP;

   std::basic_string<character> testStdString           ( testConstCharP ) ;
   AString                      testAString             ( testConstCharP ) ;
   character                    testStringLiteral[2]    ( A_CHAR("1")    ) ;
   Substring                    testSubstring           ( testConstCharP ) ;
   String32                     testLocalString         ( testConstCharP ) ;
   tstn::MyString               testMyString;

   const std::basic_string<character> testConstStdString( testConstCharP ) ;
   const AString            testConstAString            ( testConstCharP ) ;
   const character          testConstStringLiteral[2]   ( A_CHAR("1")    ) ;
   const Substring          testConstSubstring          ( testConstCharP ) ;
   const String32           testConstLocalString        ( testConstCharP ) ;
   const tstn::MyString     testConstMyString;

   const character* myStringBuf= testMyString.GetMyBuffer    (            ) ;

    // implicit String
    {
        testParam( ut, testConstCharP,     CLITERAL                     ) ;
        testParam( ut, testConstCharP,     testConstCharP               ) ;
//      testParam( ut, testConstCharP,     testCharP                    ) ;
    #if !ALIB_CHARACTERS_WIDE
        testParam( ut, testConstCharP,     testConstUCharP              ) ;
//      testParam( ut, testConstCharP,     testUCharP                   ) ;
    #endif

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
        testParamSubstring( ut, testConstCharP , CLITERAL                     );
        testParamSubstring( ut, testConstCharP , testConstCharP               );
      //testParamSubstring( ut, testConstCharP , testCharP                    );
    #if !ALIB_CHARACTERS_WIDE
        testParamSubstring( ut, testConstCharP, U8LITERAL                     );
        testParamSubstring( ut, testConstCharP, testConstUCharP               );
      //testParamSubstring( ut, testConstCharP, testUCharP                    );
    #endif

        testParamSubstring( ut, A_CHAR("1"),        testStringLiteral         );
        testParamSubstring( ut, testConstCharP,     testSubstring             );
        testParamSubstring( ut, testConstCharP,     testAString               );
        testParamSubstring( ut, testConstCharP,     testLocalString           );
        testParamSubstring( ut, testConstCharP,     testStdString             );
        testParamSubstring( ut, myStringBuf   ,     testMyString              );

        testParamSubstring( ut, A_CHAR("1"),        testConstStringLiteral    );
        testParamSubstring( ut, testConstCharP,     testConstSubstring        );
        testParamSubstring( ut, testConstCharP,     testConstAString          );
        testParamSubstring( ut, testConstCharP,     testConstLocalString      );
        testParamSubstring( ut, myStringBuf   ,     testConstMyString         );
    }

    // implicit CString
    {
        testParamCString( ut, testConstCharP,      CLITERAL                    );
        testParamCString( ut, testConstCharP,      testConstCharP              );
      //testParamCString( ut, testConstCharP,      testCharP                   );
    #if !ALIB_CHARACTERS_WIDE
        testParamCString( ut, testConstCharP,      U8LITERAL                   );
        testParamCString( ut, testConstCharP,      testConstUCharP             );
      //testParamCString( ut, testConstACharP,     testUCharP                  );
    #endif

        testParamCString( ut, A_CHAR("1"),         testStringLiteral           );
        testParamCString( ut, testConstCharP,      testAString                 );
        testParamCString( ut, testConstCharP,      testLocalString             );

        testParamCString( ut, A_CHAR("1"),         testConstStringLiteral      );
        testParamCString( ut, testConstCharP,      testConstAString            );
        testParamCString( ut, testConstCharP,      testConstLocalString        );

        //testParamCString( ut, A_CHAR("1"),       &testStringLiteral          );
        testParamCString( ut, testConstCharP,      &testAString                );
        testParamCString( ut, testConstCharP,      &testLocalString            );

        //testParamCString( ut, A_CHAR("1"),       &testConstStringLiteral     );
        testParamCString( ut, testConstCharP,      &testConstAString           );
        testParamCString( ut, testConstCharP,      &testConstLocalString);
    }
}

//--------------------------------------------------------------------------------------------------
//--- Casting back
//--------------------------------------------------------------------------------------------------
UT_METHOD( CastBack )
{
    UT_INIT()
    NString   s= "TEST\u03B1\u03B2\u03B3\u03B4\u03B5";
    NCString  c= "TEST\u03B1\u03B2\u03B3\u03B4\u03B5";
    NAString  a( "TEST\u03B1\u03B2\u03B3\u03B4\u03B5" );
    NString64 l( "TEST\u03B1\u03B2\u03B3\u03B4\u03B5" );

    // String
    //{       char*    b=s;      (void) b; }   // implicit: no
      {       char*    b(s);     (void) b; }   // explicit: yes
    //{ const char*    b=s;      (void) b; }   // implicit: no
      { const char*    b(s);     (void) b; }   // explicit: yes
    //{       char8_t* b=s;      (void) b; }   // implicit: no
      {       char8_t* b(s);     (void) b; }   // explicit: yes
    //{ const char8_t* b=s;      (void) b; }   // implicit: no
      { const char8_t* b(s);     (void) b; }   // explicit: yes

    // CString
    //{       char*    b=c;      (void) b; }   // implicit: no
      {       char*    b(c);     (void) b; }   // explicit: yes
      { const char*    b=c;      (void) b; }   // implicit: yes
      { const char*    b(c);     (void) b; }   // explicit: yes
    //{       char8_t* b=c;      (void) b; }   // implicit: no
      {       char8_t* b(c);     (void) b; }   // explicit: yes
      { const char8_t* b=c;      (void) b; }   // implicit: yes
      { const char8_t* b(c);     (void) b; }   // explicit: yes

    // AString
    //{       char*    b=a;      (void) b; }   // implicit: no
      {       char*    b(a);     (void) b; }   // explicit: yes
      { const char*    b=a;      (void) b; }   // implicit: yes
      { const char*    b(a);     (void) b; }   // explicit: yes
    //{       char8_t* b=a;      (void) b; }   // implicit: no
      {       char8_t* b(a);     (void) b; }   // explicit: yes
      { const char8_t* b=a;      (void) b; }   // implicit: yes
      { const char8_t* b(a);     (void) b; }   // explicit: yes

    // LocalString
    //{       char*    b=l;      (void) b; }   // implicit: no
      {       char*    b(l);     (void) b; }   // explicit: yes
      { const char*    b=l;      (void) b; }   // implicit: yes
      { const char*    b(l);     (void) b; }   // explicit: yes
    //{       char8_t* b=l;      (void) b; }   // implicit: no
      {       char8_t* b(l);     (void) b; }   // explicit: yes
      { const char8_t* b=l;      (void) b; }   // implicit: yes
      { const char8_t* b(l);     (void) b; }   // explicit: yes
}


//--------------------------------------------------------------------------------------------------
//--- Assignment
//--------------------------------------------------------------------------------------------------
UT_METHOD( Assignment )
{
    UT_INIT()


          character              testAChar=            '@';
    const character              testConstAChar=       '@';
    const character*             testConstACharP=      CLITERAL      ;
          character*             testACharP=           const_cast<character*>( testConstACharP );
    const char8_t*               testConstUCharP=      U8LITERAL;
          char8_t*               testUCharP=           const_cast<char8_t*>( testConstUCharP );
    (void) testUCharP;

    std::basic_string<character>        testStdString        (testConstACharP);
    String                              testString           (testConstACharP);
    CString                             testCString          (testConstACharP);
    AString                             testAString          (testConstACharP);
    character                           testStringLiteral[2] (A_CHAR("1"));
    Substring                           testSubstring        =testConstACharP;
    String32                            testLocalString       ( testConstACharP );
    tstn::MyString                      testMyString;

    const std::basic_string<character>  testConstStdString   (testConstACharP);
    const String                        testConstString      (testConstACharP);
    const CString                       testConstCString     (testConstACharP);
    const AString                       testConstAString     (testConstACharP);
    const character                     testConstStringLiteral[2](A_CHAR("1"));
    const Substring                     testConstSubstring   =testConstACharP;
    const String32                      testConstLocalString  ( testConstACharP );
    const tstn::MyString                testConstMyString;

    const character* myStringBuf= testMyString.GetMyBuffer();

    // String
    { String        s=  CLITERAL                    ; testParam( ut, testConstACharP, s) ; }
    { String        s=  testConstACharP             ; testParam( ut, testConstACharP, s) ; }
  //{ String        s=  testACharP                  ; testParam( ut, testConstACharP, s) ; }
    { String        s=  (String(testACharP))        ; testParam( ut, testConstACharP, s) ; }
    #if !ALIB_CHARACTERS_WIDE
    { String        s=  U8LITERAL                   ; testParam( ut, testConstACharP, s) ; }
    { String        s=  testConstUCharP             ; testParam( ut, testConstACharP, s) ; }
  //{ String        s=  testUCharP                  ; testParam( ut, testConstACharP, s) ; }
    { String        s=  (String(testUCharP))        ; testParam( ut, testConstACharP, s) ; }
    #endif

    { String        s=  testStringLiteral           ; testParam( ut, A_CHAR("1")    , s) ; }
    { String        s=  testString                  ; testParam( ut, testConstACharP, s) ; }
    { String        s=  testCString                 ; testParam( ut, testConstACharP, s) ; }
    { String        s=  testSubstring               ; testParam( ut, testConstACharP, s) ; }
    { String        s=  testAString                 ; testParam( ut, testConstACharP, s) ; }
    { String        s=  testLocalString             ; testParam( ut, testConstACharP, s) ; }
    { String        s=  testStdString               ; testParam( ut, testConstACharP, s) ; }
    { String        s=  testMyString                ; testParam( ut, myStringBuf    , s) ; }

    { String        s=  testConstStringLiteral      ; testParam( ut, A_CHAR("1")    , s) ; }
    { String        s=  testConstString             ; testParam( ut, testConstACharP, s) ; }
    { String        s=  testConstString             ; testParam( ut, testConstACharP, s) ; }
    { String        s=  testConstSubstring          ; testParam( ut, testConstACharP, s) ; }
    { String        s=  testConstAString            ; testParam( ut, testConstACharP, s) ; }
    { String        s=  testConstLocalString        ; testParam( ut, testConstACharP, s) ; }
    { String        s=  testConstStdString          ; testParam( ut, testConstACharP, s) ; }
    { String        s=  testConstMyString           ; testParam( ut, myStringBuf    , s) ; }

    { String    s; s=  CLITERAL                    ; testParam( ut, testConstACharP, s) ; }
    { String    s; s=  testConstACharP             ; testParam( ut, testConstACharP, s) ; }
  //{ String    s; s=  testACharP                  ; testParam( ut, testConstACharP, s) ; }
    { String    s; s=  (String(testACharP))        ; testParam( ut, testConstACharP, s) ; }

    { String    s; s=  testStringLiteral           ; testParam( ut, A_CHAR("1")    , s) ; }
    { String    s; s=  testString                  ; testParam( ut, testConstACharP, s) ; }
    { String    s; s=  testCString                 ; testParam( ut, testConstACharP, s) ; }
    { String    s; s=  testSubstring               ; testParam( ut, testConstACharP, s) ; }
    { String    s; s=  testAString                 ; testParam( ut, testConstACharP, s) ; }
    { String    s; s=  testLocalString             ; testParam( ut, testConstACharP, s) ; }
    { String    s; s=  testStdString               ; testParam( ut, testConstACharP, s) ; }
    { String    s; s=  testMyString                ; testParam( ut, myStringBuf    , s) ; }

    { String    s; s=  testConstStringLiteral      ; testParam( ut, A_CHAR("1")    , s) ; }
    { String    s; s=  testConstString             ; testParam( ut, testConstACharP, s) ; }
    { String    s; s=  testConstCString            ; testParam( ut, testConstACharP, s) ; }
    { String    s; s=  testConstSubstring          ; testParam( ut, testConstACharP, s) ; }
    { String    s; s=  testConstAString            ; testParam( ut, testConstACharP, s) ; }
    { String    s; s=  testConstLocalString        ; testParam( ut, testConstACharP, s) ; }
    { String    s; s=  testConstStdString          ; testParam( ut, testConstACharP, s) ; }
    { String    s; s=  testConstMyString           ; testParam( ut, myStringBuf    , s) ; }

    // Substring
    { Substring         ss=  CLITERAL                   ; testParam( ut, testConstACharP, ss) ; }
    { Substring         ss=  testConstACharP            ; testParam( ut, testConstACharP, ss) ; }
  //{ Substring         ss=  testACharP                 ; testParam( ut, testConstACharP, ss) ; }
    { Substring         ss=  Substring(testACharP)      ; testParam( ut, testConstACharP, ss) ; }
    #if !ALIB_CHARACTERS_WIDE
    { Substring         ss=  U8LITERAL                  ; testParam( ut, testConstACharP, ss) ; }
    { Substring         ss=  testConstUCharP            ; testParam( ut, testConstACharP, ss) ; }
  //{ Substring         ss=  testACharP                 ; testParam( ut, testConstACharP, ss) ; }
    { Substring         ss=  Substring(testUCharP)      ; testParam( ut, testConstACharP, ss) ; }
    #endif

    { Substring         ss=  testStringLiteral          ; testParam( ut, A_CHAR("1")    , ss) ; }
    { Substring         ss=  testString                 ; testParam( ut, testConstACharP, ss) ; }
    { Substring         ss=  testCString                ; testParam( ut, testConstACharP, ss) ; }
    { Substring         ss=  testSubstring              ; testParam( ut, testConstACharP, ss) ; }
    { Substring         ss=  testAString                ; testParam( ut, testConstACharP, ss) ; }
    { Substring         ss=  testLocalString            ; testParam( ut, testConstACharP, ss) ; }
    { Substring         ss=  testStdString              ; testParam( ut, testConstACharP, ss) ; }
    { Substring         ss=  testMyString               ; testParam( ut, myStringBuf    , ss) ; }

    { Substring         ss=  testConstStringLiteral     ; testParam( ut, A_CHAR("1")    , ss) ; }
    { Substring         ss=  testConstString            ; testParam( ut, testConstACharP, ss) ; }
    { Substring         ss=  testConstCString           ; testParam( ut, testConstACharP, ss) ; }
    { Substring         ss=  testConstSubstring         ; testParam( ut, testConstACharP, ss) ; }
    { Substring         ss=  testConstAString           ; testParam( ut, testConstACharP, ss) ; }
    { Substring         ss=  testConstLocalString       ; testParam( ut, testConstACharP, ss) ; }
    { Substring         ss=  testConstStdString         ; testParam( ut, testConstACharP, ss) ; }
    { Substring         ss=  testConstMyString          ; testParam( ut, myStringBuf    , ss) ; }

    { Substring     ss; ss=  CLITERAL                    ; testParam( ut, testConstACharP, ss) ; }
    { Substring     ss; ss=  testConstACharP             ; testParam( ut, testConstACharP, ss) ; }
  //{ Substring     ss; ss=  testACharP                  ; testParam( ut, testConstACharP, ss) ; }
    { Substring     ss; ss=  Substring(testACharP)       ; testParam( ut, testConstACharP, ss) ; }

    { Substring     ss; ss=  testStringLiteral           ; testParam( ut, A_CHAR("1")    , ss) ; }
    { Substring     ss; ss=  testString                  ; testParam( ut, testConstACharP, ss) ; }
    { Substring     ss; ss=  testCString                 ; testParam( ut, testConstACharP, ss) ; }
    { Substring     ss; ss=  testSubstring               ; testParam( ut, testConstACharP, ss) ; }
    { Substring     ss; ss=  testAString                 ; testParam( ut, testConstACharP, ss) ; }
    { Substring     ss; ss=  testLocalString             ; testParam( ut, testConstACharP, ss) ; }
    { Substring     ss; ss=  testStdString               ; testParam( ut, testConstACharP, ss) ; }
    { Substring     ss; ss=  testMyString                ; testParam( ut, myStringBuf    , ss) ; }

    { Substring     ss; ss=  testConstStringLiteral      ; testParam( ut, A_CHAR("1")    , ss) ; }
    { Substring     ss; ss=  testConstString             ; testParam( ut, testConstACharP, ss) ; }
    { Substring     ss; ss=  testConstCString            ; testParam( ut, testConstACharP, ss) ; }
    { Substring     ss; ss=  testConstSubstring          ; testParam( ut, testConstACharP, ss) ; }
    { Substring     ss; ss=  testConstAString            ; testParam( ut, testConstACharP, ss) ; }
    { Substring     ss; ss=  testConstLocalString        ; testParam( ut, testConstACharP, ss) ; }
    { Substring     ss; ss=  testConstStdString          ; testParam( ut, testConstACharP, ss) ; }
    { Substring     ss; ss=  testConstMyString           ; testParam( ut, myStringBuf    , ss) ; }

    // CString (no Substrings!)
    { CString     cs = CLITERAL                          ;testParam ( ut, testConstACharP,  cs ) ; }
    { CString     cs = testConstACharP                   ;testParam ( ut, testConstACharP,  cs ) ; }
    { CString     cs = CString(testACharP              ) ;testParam ( ut, testConstACharP,  cs ) ; }
    #if !ALIB_CHARACTERS_WIDE
    { CString     cs = U8LITERAL                         ;testParam ( ut, testConstACharP,  cs ) ; }
    { CString     cs = testConstUCharP                   ;testParam ( ut, testConstACharP,  cs ) ; }
    { CString     cs = CString(testUCharP              ) ;testParam ( ut, testConstACharP,  cs ) ; }
    #endif
    { CString     cs = testStringLiteral                 ;testParam ( ut, A_CHAR( "1")   ,  cs ) ; }
    { CString     cs = CString(testString              ) ;testParam ( ut, testConstACharP,  cs ) ; }
    { CString     cs = CString(testCString             ) ;testParam ( ut, testConstACharP,  cs ) ; }
  //{ CString     cs = CString(testSubstring           ) ;testParam ( ut, testConstACharP,  cs ) ; }
    { CString     cs = testAString                       ;testParam ( ut, testConstACharP,  cs ) ; }
    { CString     cs = testLocalString                   ;testParam ( ut, testConstACharP,  cs ) ; }
    { CString     cs = CString(testStdString           ) ;testParam ( ut, testConstACharP,  cs ) ; }
  //{ CString     cs = CString(testMyString            ) ;testParam ( ut, myStringBuf    ,  cs ) ; }
    { CString     cs = testConstStringLiteral            ;testParam ( ut, A_CHAR( "1")   ,  cs ) ; }
  //{ CString     cs = CString(testConstSubstring      ) ;testParam ( ut, testConstACharP,  cs ) ; }
    { CString     cs = testConstAString                  ;testParam ( ut, testConstACharP,  cs ) ; }
    { CString     cs = testConstLocalString              ;testParam ( ut, testConstACharP,  cs ) ; }
    { CString     cs = testConstStdString                ;testParam ( ut, testConstACharP,  cs ) ; }
  //{ CString     cs = CString(testConstMyString       ) ;testParam ( ut, myStringBuf    ,  cs ) ; }
  //{ CString     cs = CString(&testStringLiteral      ) ;testParam ( ut, A_CHAR( "1")   ,  cs ) ; }
    { CString     cs = CString(&testString             ) ;testParam ( ut, testConstACharP,  cs ) ; }
    { CString     cs = CString(&testCString            ) ;testParam ( ut, testConstACharP,  cs ) ; }
  //{ CString     cs = CString(&testSubstring          ) ;testParam ( ut, testConstACharP,  cs ) ; }
    { CString     cs = &testAString                      ;testParam ( ut, testConstACharP,  cs ) ; }
    { CString     cs = &testLocalString                  ;testParam ( ut, testConstACharP,  cs ) ; }
    { CString     cs = &testStdString                    ;testParam ( ut, testConstACharP,  cs ) ; }
  //{ CString     cs = CString(&testMyString           ) ;testParam ( ut, myStringBuf    ,  cs ) ; }
  //{ CString     cs = CString(&testConstStringLiteral ) ;testParam ( ut, A_CHAR( "1")   ,  cs ) ; }
  //{ CString     cs = CString(&testConstSubstring     ) ;testParam ( ut, testConstACharP,  cs ) ; }
    { CString     cs = &testConstAString                 ;testParam ( ut, testConstACharP,  cs ) ; }
    { CString     cs = &testConstLocalString             ;testParam ( ut, testConstACharP,  cs ) ; }
    { CString     cs = &testConstStdString               ;testParam ( ut, testConstACharP,  cs ) ; }
  //{ CString     cs = CString(&testConstMyString      ) ;testParam ( ut, myStringBuf    ,  cs ) ; }

    { CString cs; cs = CLITERAL                          ; testParam( ut, testConstACharP,  cs ) ; }
    { CString cs; cs = CString(testConstACharP         ) ; testParam( ut, testConstACharP,  cs ) ; }
    { CString cs; cs = CString(testACharP              ) ; testParam( ut, testConstACharP,  cs ) ; }
    { CString cs; cs = testStringLiteral                 ; testParam( ut, A_CHAR("1"   ) ,  cs ) ; }
    { CString cs; cs = CString(testString              ) ; testParam( ut, testConstACharP,  cs ) ; }
    { CString cs; cs = CString(testCString             ) ; testParam( ut, testConstACharP,  cs ) ; }
  //{ CString cs; cs = CString(testSubstring           ) ; testParam( ut, testConstACharP,  cs ) ; }
    { CString cs; cs = testAString                       ; testParam( ut, testConstACharP,  cs ) ; }
    { CString cs; cs = testLocalString                   ; testParam( ut, testConstACharP,  cs ) ; }
    { CString cs; cs = CString(testStdString           ) ; testParam( ut, testConstACharP,  cs ) ; }
  //{ CString cs; cs = CString(testMyString            ) ; testParam( ut, myStringBuf    ,  cs ) ; }
    { CString cs; cs = testConstStringLiteral            ; testParam( ut, A_CHAR("1"   ) ,  cs ) ; }
  //{ CString cs; cs = CString(testConstSubstring      ) ; testParam( ut, testConstACharP,  cs ) ; }
    { CString cs; cs = testConstAString                  ; testParam( ut, testConstACharP,  cs ) ; }
    { CString cs; cs = testConstLocalString              ; testParam( ut, testConstACharP,  cs ) ; }
    { CString cs; cs = CString(testConstStdString      ) ; testParam( ut, testConstACharP,  cs ) ; }
  //{ CString cs; cs = CString(testConstMyString       ) ; testParam( ut, myStringBuf    ,  cs ) ; }
  //{ CString cs; cs = CString(&testStringLiteral      ) ; testParam( ut, A_CHAR("1"   ) ,  cs ) ; }
    { CString cs; cs = CString(&testString             ) ; testParam( ut, testConstACharP,  cs ) ; }
    { CString cs; cs = CString(&testCString            ) ; testParam( ut, testConstACharP,  cs ) ; }
  //{ CString cs; cs = CString(&testSubstring          ) ; testParam( ut, testConstACharP,  cs ) ; }
    { CString cs; cs = &testAString                      ; testParam( ut, testConstACharP,  cs ) ; }
    { CString cs; cs = &testLocalString                  ; testParam( ut, testConstACharP,  cs ) ; }
    { CString cs; cs = &testStdString                    ; testParam( ut, testConstACharP,  cs ) ; }
  //{ CString cs; cs = CString(&testMyString           ) ; testParam( ut, myStringBuf    ,  cs ) ; }
  //{ CString cs; cs = CString(&testConstStringLiteral );; testParam( ut, A_CHAR("1"   ) ,  cs ) ; }
  //{ CString cs; cs = CString(&testConstSubstring     ) ; testParam( ut, testConstACharP,  cs ) ; }
    { CString cs; cs = &testConstAString                 ; testParam( ut, testConstACharP,  cs ) ; }
    { CString cs; cs = &testConstLocalString             ; testParam( ut, testConstACharP,  cs ) ; }
    { CString cs; cs = &testConstStdString               ; testParam( ut, testConstACharP,  cs ) ; }
  //{ CString cs; cs = CString(&testConstMyString      ) ; testParam( ut, myStringBuf    ,  cs ) ; }
    // AString

    // direct initialization not allowed! Uncomment to see compiler error
    //{ AString            as=  "TEST\U03B1\U03B2\U03B3\U03B4\U03B5"              ; testParam( ut, testConstACharP, as); }


    // LocalString
    // direct initialization not allowed!
    //{ String32             as=  "TEST\U03B1\U03B2\U03B3\U03B4\U03B5"              ; testParam( ut, testConstACharP, as); }
    //...
    //...
    { String32 ls; ls=   "TEST\u03B1\u03B2\u03B3\u03B4\u03B5" ; testParam( ut, testConstACharP, ls); }
    { String32 ls; ls=  L"TEST\u03B1\u03B2\u03B3\u03B4\u03B5" ; testParam( ut, testConstACharP, ls); }
    { String32 ls; ls=  testConstACharP             ; testParam( ut, testConstACharP, ls); }
  //{ String32 ls; ls=  testACharP                  ; testParam( ut, testConstACharP, ls); }
    { String32 ls; ls=  (String(testACharP))        ; testParam( ut, testConstACharP, ls); }

    { String32 ls; ls=  testAChar                   ; testParam( ut, A_CHAR("@")    , ls); }
    { String32 ls; ls=  testStringLiteral           ; testParam( ut, A_CHAR("1")    , ls); }
    { String32 ls; ls=  testString                  ; testParam( ut, testConstACharP, ls); }
    { String32 ls; ls=  testCString                 ; testParam( ut, testConstACharP, ls); }
    { String32 ls; ls=  testSubstring               ; testParam( ut, testConstACharP, ls); }
    { String32 ls; ls=  testAString                 ; testParam( ut, testConstACharP, ls); }
    { String32 ls; ls=  testLocalString             ; testParam( ut, testConstACharP, ls); }
    { String32 ls; ls=  testStdString               ; testParam( ut, testConstACharP, ls); }
    { String32 ls; ls=  testMyString                ; testParam( ut, myStringBuf    , ls); }

    { String32 ls; ls=  testConstAChar              ; testParam( ut, A_CHAR("@")    , ls); }
    { String32 ls; ls=  testConstStringLiteral      ; testParam( ut, A_CHAR("1")    , ls); }
    { String32 ls; ls=  testConstString             ; testParam( ut, testConstACharP, ls); }
    { String32 ls; ls=  testConstCString            ; testParam( ut, testConstACharP, ls); }
    { String32 ls; ls=  testConstSubstring          ; testParam( ut, testConstACharP, ls); }
    { String32 ls; ls=  testConstAString            ; testParam( ut, testConstACharP, ls); }
    { String32 ls; ls=  testConstLocalString        ; testParam( ut, testConstACharP, ls); }
    { String32 ls; ls=  testConstStdString          ; testParam( ut, testConstACharP, ls); }
    { String32 ls; ls=  testConstMyString           ; testParam( ut, myStringBuf    , ls); }

    // character array
    #if !ALIB_CHARACTERS_WIDE
    { character    ca[15]=  CLITERAL                ; testParam( ut, testConstACharP, ca); }
    #else
    { character    ca[10]=  CLITERAL                ; testParam( ut, testConstACharP, ca); }
    #endif
}

//--------------------------------------------------------------------------------------------------
//--- Append and operator <<
//--------------------------------------------------------------------------------------------------
UT_METHOD( AppendAndAppendOperator )
{
    UT_INIT()

    const character*                    testConstACharP=  CLITERAL      ;
    character*                          testACharP=       const_cast<character*>( testConstACharP );
    const char8_t*                      testConstUCharP=  U8LITERAL;
    char8_t*                            testUCharP=       const_cast<char8_t*>( testConstUCharP );
    std::basic_string<character>        testStdString    (testConstACharP);
     String                             testString       (testConstACharP);
    CString                             testCString      (testConstACharP);
    AString                             testAString      (testConstACharP);
    character                           testStringLiteral[2](A_CHAR("1"));
    Substring                           testSubstring    =testConstACharP;
    String32                            testLocalString  ( testConstACharP );
    tstn::MyString                      testMyString;

    const std::basic_string<character>  testConstStdString    (testConstACharP);
    const  String                       testConstString       (testConstACharP);
    const CString                       testConstCString      (testConstACharP);
    const AString                       testConstAString      (testConstACharP);
    const character                     testConstStringLiteral[2](A_CHAR("1"));
    const Substring                     testConstSubstring    =testConstACharP;
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
    { AString as; as <<  CLITERAL                     ; testParam( ut, testConstACharP, as) ; }
    { AString as; as <<  testConstACharP              ; testParam( ut, testConstACharP, as) ; }
  //{ AString as; as <<  testACharP                   ; testParam( ut, testConstACharP, as) ; }
    { AString as; as <<  (String(testACharP))         ; testParam( ut, testConstACharP, as) ; }
    { AString as; as <<  U8LITERAL                    ; testParam( ut, testConstACharP, as) ; }
    { AString as; as <<  testConstUCharP              ; testParam( ut, testConstACharP, as) ; }
  //{ AString as; as <<  testUCharP                   ; testParam( ut, testConstACharP, as) ; }
    { AString as; as <<  (NString(testUCharP))        ; testParam( ut, testConstACharP, as) ; }

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

    { AString as; as._( CLITERAL            )         ; testParam( ut, testConstACharP, as) ; }
    { AString as; as._( testConstACharP     )         ; testParam( ut, testConstACharP, as) ; }
  //{ AString as; as._( testACharP          )         ; testParam( ut, testConstACharP, as) ; }
    { AString as; as._( (String(testACharP)))         ; testParam( ut, testConstACharP, as) ; }
    { AString as; as._( U8LITERAL           )         ; testParam( ut, testConstACharP, as) ; }
    { AString as; as._( testConstUCharP     )         ; testParam( ut, testConstACharP, as) ; }
  //{ AString as; as._( testUCharP          )         ; testParam( ut, testConstACharP, as) ; }
    { AString as; as._((NString(testUCharP)))         ; testParam( ut, testConstACharP, as) ; }

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

    { AString as          ; as._<NC>( CLITERAL             )        ; testParam( ut, testConstACharP, as) ; }
    { AString as          ; as._<NC>( testConstACharP      )        ; testParam( ut, testConstACharP, as) ; }
  //{ AString as          ; as._<NC>( testACharP           )        ; testParam( ut, testConstACharP, as) ; }
    { AString as          ; as._<NC>( (String(testACharP)) )        ; testParam( ut, testConstACharP, as) ; }
    { AString as          ; as._<NC>( U8LITERAL            )        ; testParam( ut, testConstACharP, as) ; }
    { AString as          ; as._<NC>( testConstUCharP      )        ; testParam( ut, testConstACharP, as) ; }
  //{ AString as          ; as._<NC>( testUCharP           )        ; testParam( ut, testConstACharP, as) ; }
    { AString as          ; as._<NC>((NString(testUCharP)) )        ; testParam( ut, testConstACharP, as) ; }

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
    { String64    as      ; as <<  CLITERAL                            ; testParam( ut, testConstACharP, as) ; }
    { String64    as      ; as <<  testConstACharP                     ; testParam( ut, testConstACharP, as) ; }
  //{ String64    as      ; as <<  testACharP                          ; testParam( ut, testConstACharP, as) ; }
    { String64    as      ; as <<  String(testACharP)                  ; testParam( ut, testConstACharP, as) ; }
    { String64    as      ; as <<  U8LITERAL                           ; testParam( ut, testConstACharP, as) ; }
    { String64    as      ; as <<  testConstUCharP                     ; testParam( ut, testConstACharP, as) ; }
  //{ String64    as      ; as <<  testUCharP                          ; testParam( ut, testConstACharP, as) ; }
    { String64    as      ; as <<  NString(testUCharP)                 ; testParam( ut, testConstACharP, as) ; }

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

    { String64    as      ; as._( CLITERAL             )               ; testParam( ut, testConstACharP, as) ; }
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

    ALIB_DBG(assert::GetHaltFlagAndCounters().HaltOnErrors=false;)
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
    ALIB_DBG(assert::GetHaltFlagAndCounters().HaltOnErrors=true;)
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
// with GCC and enabling C++20 language standard, a quite strange compilation error occurs.
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

#include "aworx_unittests_end.hpp"

} //namespace

#undef U8LITERAL

#endif // ALIB_UT_STRINGS
