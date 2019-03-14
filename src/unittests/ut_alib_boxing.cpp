// #################################################################################################
//  aworx - Unit Tests
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"
#include "unittests/alib_test_selection.hpp"
#if !defined(ALIB_UT_SELECT) || defined(ALIB_UT_BOXING)

#include "alib/compatibility/qt_characters.hpp"
#include "alib/compatibility/std_characters.hpp"
#include "alib/alox.hpp"
#include "alib/compatibility/std_strings_iostream.hpp"
#include "alib/compatibility/std_characters.hpp"
#include "alib/compatibility/std_boxing.hpp"

#if !defined (HPP_ALIB_TIME_TIMEPOINTBASE)
    #include "alib/time/timepointbase.hpp"
#endif

#define TESTCLASSNAME       CPP_ALib_Boxing
#include "unittests/aworx_unittests.hpp"
#include "alib/alox/loggers/memorylogger.hpp"
#include "alib/boxing/dbgboxing.hpp"

namespace std
{
    extern basic_stringstream<aworx::nchar>     testOutputStreamN;
    extern basic_stringstream<aworx::character> testOutputStream;
}

using namespace std;
using namespace aworx;


                                                    extern void dox_boxing_tut_main(int argc, char *argv[] );
namespace dox_boxing_tut                         {  extern void callcall();
                                                    extern void callresult();
                                                    extern void isType();  }
namespace dox_boxing_implicit_conversion_sample  {  extern void test();        }
namespace dox_boxing_implicit_conversion_sample2 {  extern void test();        }
namespace dox_boxing_implicit_conversion_sample2 {  extern void test2();       }
namespace dox_boxing_sample_1                    {  extern void sampleFunc();  }
namespace dox_boxing_sample_2                    {  extern void sampleFunc();  }
namespace dox_boxing_sample_2                    {  extern void sampleFunc2(); }
namespace dox_boxing_bijective                   {  extern void sampleFunc();  }
namespace dox_boxing_sample_arr                  {  extern void sampleFunc();  }
namespace dox_boxing_sample_arr                  {  extern void sampleFunc2(); }
namespace dox_boxing_sample_ftypes               {  extern void sampleFunc();  }
namespace dox_boxing_sample_boxedas              {  extern void sampleFunc();  }
namespace dox_boxing_sample_if                   {  extern void sampleFunc();  }
namespace dox_boxing_sample_strings              {  extern void sampleFunc();  }
namespace dox_boxing_sample_recipe               {  extern void sampleFunc();  }
namespace dox_boxing_sample_variadic             {  extern void sampleFunc();  }
namespace dox_boxing_sample_boxes                {  extern void sampleFunc();  }
namespace dox_boxing_sample_boxes                {  extern void sampleFunc2(); }
namespace dox_boxing_sample_boxes                {  extern void sampleFunc3(); }
namespace dox_boxing_sample_boxes_box            {  extern void sampleFunc();  }
namespace dox_boxing_sample_debug                {  extern void sampleFunc();  }
namespace dox_boxing_sample_debug                {  extern void sampleFunc2(); }
namespace dox_boxing_sample_debug                {  extern void sampleFunc3(); }
namespace dox_boxing_sample_debug                {  extern void sampleFunc4(); }
namespace dox_boxing_sample_compare              {  extern void sampleFunc();  }


class SmallClassDefaultBoxing
{
    public:
        int          i= 73;
        const char*  cp= "This is my a SmallClassDefaultBoxing instance";

        bool operator==(const SmallClassDefaultBoxing& rhs) const { return this->i == rhs.i; }
        bool operator< (const SmallClassDefaultBoxing& rhs) const { return this->i <  rhs.i; }
};

class BigClassDefaultBoxing
{
    public:
        int          i= 73;
        const char*  cp= "This is my a SmallClassDefaultBoxing instance";
        integer      tooBigNow= 1;

        bool operator==(const   BigClassDefaultBoxing& rhs) const { return this->i == rhs.i; }
        bool operator< (const   BigClassDefaultBoxing& rhs) const { return this->i <  rhs.i; }
};

class SmallNotCopyConstr //not copy constructible
{
    public:
        SmallNotCopyConstr(const SmallNotCopyConstr& )= delete;

        SmallNotCopyConstr(int pa, const char* pc)
        :i(pa), cp(pc) {}
            int          i;
            const char*  cp;
};


class SmallNotCopyConstrCustom //not default constructible
{
    public:
        SmallNotCopyConstrCustom(const SmallNotCopyConstrCustom& )= delete;

        SmallNotCopyConstrCustom(int pa, const char* pc)
        :i(pa), cp(pc) {}
            int          i;
            const char*  cp;
};

class ClassWithExplicitPointerBoxing
{
    public:
        int          i      =42;
        const char*  cp     ="CWEPB";
};

class ClassWithValueAndPointerMapping
{
    public:
        int          i      =43;
        const char*  cp     ="CWBOTH";
};

class ClassWithValueAndPointerCustom
{
    public:
        int          i      =44;
        const char*  cp     ="CWBOTH";
};

class ClassVNotUnboxable
{
    public:
        int          i      =44;
        const char*  cp     ="VNotUnboxable";
};

class ClassPNotUnboxable
{
    public:
        int          i      =44;
        const char*  cp     ="PNotUnboxable";
};
class ClassBothNotUnboxable
{
    public:
        int          i      =44;
        const char*  cp     ="PNotUnboxable";
};

class ClassVNotBoxable
{
    public:
        int          i      =44;
        const char*  cp     ="VNotBoxable";
};

class ClassPNotBoxable
{
    public:
        int          i      =44;
        const char*  cp     ="PNotBoxable";
};
class ClassBothNotBoxable
{
    public:
        int          i      =44;
        const char*  cp     ="PNotBoxable";
};

class ClassVNotBoxablePBoxable
{
    public:
        int          i      =44;
        const char*  cp     ="VNotBoxableVPoxable";
};

class ClassPNotBoxableVBoxable
{
    public:
        int          i      =44;
        const char*  cp     ="PNotBoxableVBoxable";
};

ALIB_BOXING_CUSTOMIZE(SmallNotCopyConstrCustom, SmallNotCopyConstrCustom,
    static inline void Write( Placeholder& target, SmallNotCopyConstrCustom const& value )
    {
        target.Write( value.cp, value.i );
    }
    static SmallNotCopyConstrCustom&& Read( const Placeholder& src)
    {
        return std::forward<SmallNotCopyConstrCustom>( SmallNotCopyConstrCustom( static_cast<int>( src.Length() ),
                                                                                 src.Pointer<char>()            )  );
    }
)


ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING(ClassWithExplicitPointerBoxing*, ClassWithExplicitPointerBoxing*  )

ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING(ClassWithValueAndPointerMapping       , ClassWithValueAndPointerMapping         )
ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING(ClassWithValueAndPointerMapping*      , ClassWithValueAndPointerMapping*        )


ALIB_BOXING_CUSTOMIZE(ClassWithValueAndPointerCustom , ClassWithValueAndPointerCustom,
    static inline void Write( Placeholder& target, ClassWithValueAndPointerCustom const& value )
    {
        target.Write( value.cp, value.i );
    }

    static ClassWithValueAndPointerCustom Read( const Placeholder& src)
    {
        ClassWithValueAndPointerCustom result;
        result.cp=                   src.Pointer<char>();
        result.i=  static_cast<int>( src.Length()  );
        return result;
    }
)

ALIB_BOXING_CUSTOMIZE(ClassWithValueAndPointerCustom*, ClassWithValueAndPointerCustom*,
    static inline void Write( Placeholder& target, ClassWithValueAndPointerCustom* const& value )
    {
        target.Write( value );
    }
    static ClassWithValueAndPointerCustom* Read( const Placeholder& src)
    {
        return src.Pointer<ClassWithValueAndPointerCustom>();
    }
)

ALIB_BOXING_CUSTOMIZE_NOT_UNBOXABLE(ClassVNotUnboxable         , ClassVNotUnboxable        )
ALIB_BOXING_CUSTOMIZE_NOT_UNBOXABLE(ClassPNotUnboxable      *  , ClassPNotUnboxable      * )
ALIB_BOXING_CUSTOMIZE_NOT_UNBOXABLE(ClassBothNotUnboxable      , ClassBothNotUnboxable     )
ALIB_BOXING_CUSTOMIZE_NOT_UNBOXABLE(ClassBothNotUnboxable   *  , ClassBothNotUnboxable   * )

ALIB_BOXING_CUSTOMIZE_DENY_BOXING ( ClassVNotBoxable           )
ALIB_BOXING_CUSTOMIZE_DENY_BOXING ( ClassPNotBoxable        *  )
ALIB_BOXING_CUSTOMIZE_DENY_BOXING ( ClassBothNotBoxable        )
ALIB_BOXING_CUSTOMIZE_DENY_BOXING ( ClassBothNotBoxable     *  )
ALIB_BOXING_CUSTOMIZE_DENY_BOXING ( ClassVNotBoxablePBoxable   )
ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING( ClassVNotBoxablePBoxable*  , ClassVNotBoxablePBoxable* )
ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING( ClassPNotBoxableVBoxable   , ClassPNotBoxableVBoxable  )
ALIB_BOXING_CUSTOMIZE_DENY_BOXING ( ClassPNotBoxableVBoxable*  )

namespace ut_aworx {


UT_CLASS()

//--------------------------------------------------------------------------------------------------
//--- FTypes Equals/IsNull/IsEmpty
//--------------------------------------------------------------------------------------------------
extern
void testEquals( AWorxUnitTesting& ut, bool exp, const Box& b1, const Box& b2 );
void testEquals( AWorxUnitTesting& ut, bool exp, const Box& b1, const Box& b2 )
{
    UT_EQ( exp, ( b1.Call<FEquals>(b2) ) );
    UT_EQ( exp, ( b2.Call<FEquals>(b1) ) );
}

template<typename TBoxable>
inline void testBoxingArray(AWorxUnitTesting& ut,  TBoxable& val, const Box& box )
{
    UT_TRUE(  box.IsArrayOf<typename std::remove_extent<TBoxable>::type>() );
    UT_TRUE( ( box.Call<FEquals>(val) ) );
}

template<typename TBoxable>
inline void testBoxing(AWorxUnitTesting& ut,  const TBoxable& val, const Box& box )
{
    UT_EQ( val, box.Unbox<TBoxable>() );
    UT_TRUE( ( box.Call<FEquals>(val) ) );
}


template<typename TBoxable>
inline void testBoxing(AWorxUnitTesting& ut,  TBoxable val, const Box& box1 , const Box& box2)
{
    UT_EQ( val, box1.Unbox<TBoxable>() );
    UT_EQ( val, box2.Unbox<TBoxable>() );

    UT_TRUE( ( box1.Call<FEquals>(box2) ) );
    UT_TRUE( ( box2.Call<FEquals>(box1) ) );
}

UT_METHOD(Boxing_FundamentalTypes)
{
    UT_INIT();
    UT_PRINT( "### Boxing: Fundamental Types ###" );

    { bool      v=      true;   testBoxing(ut,  true, v, &v );       }

    #if ALIB_FEAT_BOXING_NON_BIJECTIVE_INTEGRALS
        {  int8_t   v=         1;   testBoxing(ut,  static_cast<  integer>( 1 )   , v, &v );   }
        {  int16_t  v=         2;   testBoxing(ut,  static_cast<  integer>( 2 )   , v, &v );   }
        {  int32_t  v=         3;   testBoxing(ut,  static_cast<  integer>( 3 )   , v, &v );   }
        {  int64_t  v=         4;   testBoxing(ut,  static_cast<  int64_t>( 4 )   , v, &v );   }
        {  intGap_t v=         4;   testBoxing(ut,  static_cast<  integer>( 4 )   , v, &v );   }
        {  int8_t   v=        -1;   testBoxing(ut,  static_cast<  integer>(-1 )   , v, &v );   }
        {  int16_t  v=        -1;   testBoxing(ut,  static_cast<  integer>(-1 )   , v, &v );   }
        {  int32_t  v=        -1;   testBoxing(ut,  static_cast<  integer>(-1 )   , v, &v );   }
        {  int64_t  v=        -1;   testBoxing(ut,  static_cast<  int64_t>(-1 )   , v, &v );   }
        {  intGap_t v=        -1;   testBoxing(ut,  static_cast<  integer>(-1 )   , v, &v );   }
        {  int8_t   v=        -5;   testBoxing(ut,  static_cast<  integer>(-5 )   , v, &v );   }
        {  int16_t  v=        -5;   testBoxing(ut,  static_cast<  integer>(-5 )   , v, &v );   }
        {  int32_t  v=        -5;   testBoxing(ut,  static_cast<  integer>(-5 )   , v, &v );   }
        {  int64_t  v=        -5;   testBoxing(ut,  static_cast<  int64_t>(-5 )   , v, &v );   }
        {  intGap_t v=        -5;   testBoxing(ut,  static_cast<  integer>(-5 )   , v, &v );   }
        { uint8_t   v=         1;   testBoxing(ut,  static_cast< uinteger>( 1 )   , v, &v );   }
        { uint16_t  v=         2;   testBoxing(ut,  static_cast< uinteger>( 2 )   , v, &v );   }
        { uint32_t  v=         3;   testBoxing(ut,  static_cast< uinteger>( 3 )   , v, &v );   }
        { uint64_t  v=         4;   testBoxing(ut,  static_cast< uint64_t>( 4 )   , v, &v );   }
        { uintGap_t v=         4;   testBoxing(ut,  static_cast< uinteger>( 4 )   , v, &v );   }
    #else
        {  int8_t   v=         1;   testBoxing(ut,  static_cast<   int8_t>( 1 )   , v, &v );   }
        {  int16_t  v=         2;   testBoxing(ut,  static_cast<  int16_t>( 2 )   , v, &v );   }
        {  int32_t  v=         3;   testBoxing(ut,  static_cast<  int32_t>( 3 )   , v, &v );   }
        {  int64_t  v=         4;   testBoxing(ut,  static_cast<  int64_t>( 4 )   , v, &v );   }
        {  intGap_t v=         4;   testBoxing(ut,  static_cast< intGap_t>( 4 )   , v, &v );   }
        {  int8_t   v=        -1;   testBoxing(ut,  static_cast<   int8_t>(-1 )   , v, &v );   }
        {  int16_t  v=        -1;   testBoxing(ut,  static_cast<  int16_t>(-1 )   , v, &v );   }
        {  int32_t  v=        -1;   testBoxing(ut,  static_cast<  int32_t>(-1 )   , v, &v );   }
        {  int64_t  v=        -1;   testBoxing(ut,  static_cast<  int64_t>(-1 )   , v, &v );   }
        {  intGap_t v=        -1;   testBoxing(ut,  static_cast< intGap_t>(-1 )   , v, &v );   }
        {  int8_t   v=        -5;   testBoxing(ut,  static_cast<   int8_t>(-5 )   , v, &v );   }
        {  int16_t  v=        -5;   testBoxing(ut,  static_cast<  int16_t>(-5 )   , v, &v );   }
        {  int32_t  v=        -5;   testBoxing(ut,  static_cast<  int32_t>(-5 )   , v, &v );   }
        {  int64_t  v=        -5;   testBoxing(ut,  static_cast<  int64_t>(-5 )   , v, &v );   }
        {  intGap_t v=        -5;   testBoxing(ut,  static_cast< intGap_t>(-5 )   , v, &v );   }
        { uint8_t   v=         1;   testBoxing(ut,  static_cast<  uint8_t>( 1 )   , v, &v );   }
        { uint16_t  v=         2;   testBoxing(ut,  static_cast< uint16_t>( 2 )   , v, &v );   }
        { uint32_t  v=         3;   testBoxing(ut,  static_cast< uint32_t>( 3 )   , v, &v );   }
        { uint64_t  v=         4;   testBoxing(ut,  static_cast< uint64_t>( 4 )   , v, &v );   }
        { uintGap_t v=         4;   testBoxing(ut,  static_cast<uintGap_t>( 4 )   , v, &v );   }
    #endif

    #if  ALIB_FEAT_BOXING_NON_BIJECTIVE_FLOATS
        { float       v=      5.1f;   testBoxing(ut,  static_cast<     double  >( 5.1f ), v, &v );   }
        { double      v=      5.2 ;   testBoxing(ut,  static_cast<     double  >( 5.2 ) , v, &v );   }
      #if ALIB_SIZEOF_LONGDOUBLE <= 2 * ALIB_SIZEOF_INTEGER
        { long double v=      5.2L ;  testBoxing(ut,  static_cast<long double  >( 5.2 ) , v, &v );   }
      #endif
    #else
        { float       v=      5.1f;   testBoxing(ut,  static_cast<     float   >( 5.1f ), v, &v );   }
        { double      v=      5.2 ;   testBoxing(ut,  static_cast<     double  >( 5.2 ) , v, &v );   }
      #if ALIB_SIZEOF_LONGDOUBLE <= 2 * ALIB_SIZEOF_INTEGER
        { long double v=      5.2L ;  testBoxing(ut,  static_cast<long double  >( 5.2 ) , v, &v );   }
      #endif
    #endif


    #if  ALIB_FEAT_BOXING_NON_BIJECTIVE_CHARACTERS
        { nchar   v=              'c' ;   testBoxing(ut, A_WCHAR(      'c' ), v );  }
        { wchar   v= A_WCHAR('\u03B2');   testBoxing(ut, A_WCHAR( '\u03B2' ), v );  }
        { xchar   v= A_XCHAR('\u03B3');   testBoxing(ut, A_WCHAR( '\u03B3' ), v );  }
    #else
        { nchar   v=              'c' ;   testBoxing(ut,              'c'   , v );  }
        { wchar   v= A_WCHAR('\u03B2');   testBoxing(ut, A_WCHAR( '\u03B2' ), v );  }
        { xchar   v= A_XCHAR('\u03B3');   testBoxing(ut, A_XCHAR( '\u03B3' ), v );  }
    #endif


    { const nchar*  v=          "abc";                 testBoxingArray(ut,         "abc"                , v);  }
    { const wchar*  v=  A_WCHAR("\u03B1\u03B2\u03B3"); testBoxingArray(ut, A_WCHAR("\u03B1\u03B2\u03B3"), v);  }
    { const xchar*  v=  A_XCHAR("\u03B4\u03B5\u03B6"); testBoxingArray(ut, A_XCHAR("\u03B4\u03B5\u03B6"), v);  }

    {  int16_t  v[2]= {16,17};
       int16_t  w[2]= {16,17};                     testBoxingArray(ut,  w, v );       }

}

UT_METHOD(Boxing_FTYPESDefaultInterfaces)
{
    UT_INIT();

    UT_PRINT( "### Boxing: Fundamental Types Default interfaces (FEquals/FIsNotNull) ###" );
    testEquals( ut, true,      5,     5     );
    testEquals( ut, false,     5,     3     );
    testEquals( ut, true,      5L,    5L    );
    testEquals( ut, false,     5L,    3L    );
    testEquals( ut, true,    5.3f,    5.3f  );
    testEquals( ut, true,    5.3 ,    5.3   );
#if ALIB_FEAT_BOXING_NON_BIJECTIVE_FLOATS
    testEquals( ut, true,    5.3f,    5.3   );
#else
    testEquals( ut, false,   5.3f,    5.3   );
#endif
    testEquals( ut, false,    7.8,    5.5   );
    testEquals( ut, true,     0.0,   -0.0   );
    testEquals( ut, true,     'x',    'x'   );
    testEquals( ut, false,    'x',    'y'   );
    testEquals( ut, false,    'x',    'y'   );
    testEquals( ut, true,    L'x',   L'x'   );
    testEquals( ut, false,   L'x',   L'y'   );
    testEquals( ut, false,   L'x',   L'y'   );
    testEquals( ut, true,   false,  false   );
    testEquals( ut, true,    true,   true   );
    testEquals( ut, false,  false,   true   );

    testEquals( ut, true,   "a"  ,  "a"     );
    testEquals( ut, true,   "ab" ,  "ab"    );
    testEquals( ut, true,   "abc",  "abc"   );

    char abc1[]= { 'a', 'b', 'c', '\0' };
    char abc2[]= { 'a', 'b', 'c', '\0' };
    testEquals( ut, true,   abc1,  abc2   );
    testEquals( ut, true,  L"a"  , L"a"     );
    testEquals( ut, true,  L"ab" , L"ab"    );
    testEquals( ut, true,  L"abc", L"abc"   );

    UT_FALSE( Box(5)   <  3     );
    UT_FALSE( Box(5)   <  3.1   );
    UT_FALSE( Box(5.1) <  3     );
    UT_FALSE( Box(5.1) <  3.1   );
    UT_FALSE( Box(5)   <= 3     );
    UT_FALSE( Box(5)   <= 3.1   );
    UT_FALSE( Box(5.1) <= 3     );
    UT_FALSE( Box(5.1) <= 3.1   );
    UT_TRUE ( Box(5)   <= 5     );
    UT_FALSE( Box(5)   <= 4.9   );
    UT_TRUE ( Box(5)   <= 5.1   );
    UT_TRUE ( Box(4.9) <= 5     );
    UT_FALSE( Box(5.1) <= 5.0   );

    UT_TRUE ( Box(5)   > 3      );
    UT_TRUE ( Box(5)   > 3.1    );
    UT_TRUE ( Box(5.1) > 3      );
    UT_TRUE ( Box(5.1) > 3.1    );
    UT_TRUE ( Box(5)   >= 3     );
    UT_TRUE ( Box(5)   >= 3.1   );
    UT_TRUE ( Box(5.1) >= 3     );
    UT_TRUE ( Box(5.1) >= 3.1   );
    UT_TRUE ( Box(5)   >= 5     );
    UT_TRUE ( Box(5)   >= 5.0   );
    UT_TRUE ( Box(5.0) >= 4     );
    UT_FALSE( Box(5.0) >= 6     );
    UT_TRUE ( Box(5.0) >= 5.0   );
    UT_FALSE( Box(5.0) >= 5.1   );


    UT_PRINT("") UT_PRINT( "### Boxing: Testing templated FEquals/FLess for custom types big/small ###" );

    ALIB_BOXING_DEFINE_FEQUALS_FOR_COMPARABLE_TYPE( SmallClassDefaultBoxing )
    ALIB_BOXING_DEFINE_FEQUALS_FOR_COMPARABLE_TYPE( BigClassDefaultBoxing*  )
    ALIB_BOXING_DEFINE_FISLESS_FOR_COMPARABLE_TYPE( SmallClassDefaultBoxing )
    ALIB_BOXING_DEFINE_FISLESS_FOR_COMPARABLE_TYPE( BigClassDefaultBoxing*  )
    SmallClassDefaultBoxing smallClass1;
    SmallClassDefaultBoxing smallClass2;
      BigClassDefaultBoxing   bigClass1;
      BigClassDefaultBoxing   bigClass2;
(void) (smallClass1 == smallClass2);

    UT_TRUE ( Box(smallClass1) == Box(smallClass2) )
    UT_FALSE( Box(smallClass1) <  Box(smallClass2) )
    UT_TRUE ( Box(  bigClass1) == Box(  bigClass2) )
    UT_FALSE( Box(  bigClass1) <  Box(  bigClass2) )

    smallClass1.i= 1;
      bigClass1.i= 1;
    UT_FALSE( Box(smallClass1) == Box(smallClass2) )
    UT_TRUE ( Box(smallClass1) <  Box(smallClass2) )
    UT_FALSE( Box(  bigClass1) == Box(  bigClass2) )
    UT_TRUE ( Box(  bigClass1) <  Box(  bigClass2) )


    UT_PRINT("") UT_PRINT( "### Boxing: Testing Equals: char*/char[] ###" );
    {
        bool result;
        Box helloP( static_cast<const char*>("Hello"));
        Box helloA(                          "Hello");
        Box falseP( static_cast<const char*>("not hello"));
        Box falseA(                          "not hello");
        result= helloA.Call<FEquals>( helloA ); UT_EQ( true , result );
        result= helloA.Call<FEquals>( helloP ); UT_EQ( true , result );
        result= helloA.Call<FEquals>( falseA ); UT_EQ( false, result );
        result= helloA.Call<FEquals>( falseP ); UT_EQ( false, result );

        result= helloP.Call<FEquals>( helloP ); UT_EQ( true , result );
        result= helloP.Call<FEquals>( helloA ); UT_EQ( true , result );
        result= helloP.Call<FEquals>( falseP ); UT_EQ( false, result );
        result= helloP.Call<FEquals>( falseA ); UT_EQ( false, result );
    }

    UT_PRINT("") UT_PRINT( "### Boxing: Testing Equals: wchar_t*/wchar_[] ###" );
    {
        bool result;
        Box helloP( static_cast<const wchar_t*>( L"Hello"));
        Box helloA(                              L"Hello");
        Box falseP( static_cast<const wchar_t*>( L"not hello"));
        Box falseA(                              L"not hello");
        result= helloA.Call<FEquals>( helloA ); UT_EQ( true , result );
        result= helloA.Call<FEquals>( helloP ); UT_EQ( true , result );
        result= helloA.Call<FEquals>( falseA ); UT_EQ( false, result );
        result= helloA.Call<FEquals>( falseP ); UT_EQ( false, result );

        result= helloP.Call<FEquals>( helloP ); UT_EQ( true , result );
        result= helloP.Call<FEquals>( helloA ); UT_EQ( true , result );
        result= helloP.Call<FEquals>( falseP ); UT_EQ( false, result );
        result= helloP.Call<FEquals>( falseA ); UT_EQ( false, result );
    }
}

UT_METHOD(Boxing_Strings)
{
    UT_INIT();

    UT_PRINT("") UT_PRINT( "### Boxing: Testing Equals: wchar_t*/wchar_[]  with AString ###" );

    // Equals: char*/char[]
    {
        bool result;
        Box helloP( static_cast<const char*>("Hello"));
        Box helloA(                          "Hello");
        Box falseP( static_cast<const char*>("not hello"));
        Box falseA(                          "not hello");
        NAString asHello( "Hello" );
        NAString asFalse( "not hello" );
        Box helloAS( asHello );
        Box falseAS( asFalse );

        result= helloAS.Call<FEquals>( helloP ); UT_EQ( true , result );
        result= helloAS.Call<FEquals>( helloA ); UT_EQ( true , result );
        result= helloAS.Call<FEquals>( falseP ); UT_EQ( false, result );
        result= helloAS.Call<FEquals>( falseA ); UT_EQ( false, result );

        result= helloA.Call<FEquals>( helloAS ); UT_EQ( true , result );
        result= helloP.Call<FEquals>( helloAS ); UT_EQ( true , result );
        result= helloA.Call<FEquals>( falseAS ); UT_EQ( false, result );
        result= helloP.Call<FEquals>( falseAS ); UT_EQ( false, result );
    }

    AString test;
    {
        LocalString<64> pa64("64");        Box bpa64(&pa64);
        LocalString<73> pa73("73");        Box bpa73(pa73);

        test._() << bpa64;      UT_EQ( A_CHAR("64"), test );
        test._() << bpa73;      UT_EQ( A_CHAR("73"), test );
    }

    {
        aworx::LocalString<64> pa64("64");     Box bpa64(&pa64);
        aworx::LocalString<73> pa73("73");     Box bpa73(pa73);

        test._() << bpa64;      UT_EQ( A_CHAR("64"), test);
        test._() << bpa73;      UT_EQ( A_CHAR("73"), test);
    }

    {
        character sl3[4](A_CHAR("123"));       Box bsl123(sl3);

        test._() << bsl123;     UT_EQ( A_CHAR("123"), test );
    }

}


UT_METHOD(Boxing_Std_String)
{
    UT_INIT();

    UT_PRINT("") UT_PRINT( "### Boxing: std::string ###" );
    {
        std::basic_string<character> stdstr( A_CHAR("Hello") );
        Box b( stdstr );
        UT_EQ( true, b.IsArrayOf<character>() );
        UT_EQ( 5,    b.UnboxLength()           );
        UT_EQ( 'H',  b.UnboxElement<character>(0) );

        std::basic_string<character>* stdstrP = &stdstr;
        b= Box( stdstrP );
        UT_EQ( true, b.IsArrayOf<character>() );
        UT_EQ( 5,    b.UnboxLength()           );
        UT_EQ( 'H',  b.UnboxElement<character>(0) );

        stdstrP = nullptr;
        b= Box( stdstrP );
        UT_EQ( true, b.IsArrayOf<character>() );
        UT_EQ( 0,    b.UnboxLength()          );
    }
}

UT_METHOD(Boxing_Std_Vector)
{
    UT_INIT();

    UT_PRINT("") UT_PRINT( "### Boxing: std::vector ###" );
    {
        std::vector<int> vec= {3,4,5};
        Box b(vec);
        UT_EQ( true, b.IsArrayOf<int>() );
        UT_EQ( 3,    b.UnboxLength()    );
        UT_EQ( 3,    b.UnboxElement<int>(0) );
        UT_EQ( 4,    b.UnboxElement<int>(1) );
        UT_EQ( 5,    b.UnboxElement<int>(2) );
        int* array= &b.UnboxElement<int>(0);
        UT_EQ( 3,    array[0] );
        UT_EQ( 4,    array[1] );
        UT_EQ( 5,    array[2] );

    }
    {
        std::vector<double> vec= {3.2,4.3,5.4};
        Box b(vec);
        UT_EQ( true, b.IsArrayOf<double>() );
        UT_EQ( 3,    b.UnboxLength()       );
        UT_EQ( 3.2,  b.UnboxElement<double>(0) );
        UT_EQ( 4.3,  b.UnboxElement<double>(1) );
        UT_EQ( 5.4,  b.UnboxElement<double>(2) );
        double* array= &b.UnboxElement<double>(0);
        UT_EQ( 3.2,  array[0] );
        UT_EQ( 4.3,  array[1] );
        UT_EQ( 5.4,  array[2] );
    }
    {
        std::vector<std::basic_string<character>> vec= {A_CHAR("one"), A_CHAR("two"), A_CHAR("three")};
        Box b(vec);
        UT_EQ( true, b.IsArrayOf<std::basic_string<character>>() );
        UT_EQ( 3,    b.UnboxLength()                             );
        UT_EQ( std::basic_string<character>(A_CHAR("one")),    b.UnboxElement<std::basic_string<character>>(0) );
        UT_EQ( std::basic_string<character>(A_CHAR("two")),    b.UnboxElement<std::basic_string<character>>(1) );
        UT_EQ( std::basic_string<character>(A_CHAR("three")),  b.UnboxElement<std::basic_string<character>>(2) );
        std::basic_string<character>* array= &b.UnboxElement<std::basic_string<character>>(0);
        UT_EQ( std::basic_string<character>(A_CHAR("one")),    array[0] );
        UT_EQ( std::basic_string<character>(A_CHAR("two")),    array[1] );
        UT_EQ( std::basic_string<character>(A_CHAR("three")),  array[2] );
    }


    UT_PRINT("") UT_PRINT( "### Boxing: Testing Equals: std::vector<char>/char[] ###" );
    {
        bool result;
        std::vector<character> helloVec= {'H','e','l','l','o'};
        std::vector<character> falseVec= {'n','o','t',' ','H','e','l','l','o'};
        Box helloV( helloVec );
        Box helloA( A_CHAR("Hello"));
        Box falseP( falseVec );
        Box falseA( A_CHAR("not hello"));
        result= helloA.Call<FEquals>( helloV ); UT_EQ( true , result );
        result= helloA.Call<FEquals>( falseP ); UT_EQ( false, result );

        result= helloV.Call<FEquals>( helloV ); UT_EQ( true , result );
        result= helloV.Call<FEquals>( helloA ); UT_EQ( true , result );
        result= helloV.Call<FEquals>( falseP ); UT_EQ( false, result );
        result= helloV.Call<FEquals>( falseA ); UT_EQ( false, result );
    }
}

UT_METHOD(Boxing_Boxes)
{
    UT_INIT();

    UT_PRINT("") UT_PRINT( "### Boxing: class Boxesr ###" );

    {
        #if ALIB_FEAT_BOXING_NON_BIJECTIVE_INTEGRALS
        Boxes boxes( 1 );                                  UT_EQ( static_cast<size_t>(1),  boxes.size() );  UT_EQ(       1, boxes[0].Unbox<integer>());
        boxes.Add( 2 );                                    UT_EQ( static_cast<size_t>(2),  boxes.size() );  UT_EQ(       2, boxes[1].Unbox<integer>());
        boxes.Add( 3,4,5,6 );                              UT_EQ( static_cast<size_t>(6),  boxes.size() );  UT_EQ(       4, boxes[3].Unbox<integer>());
        boxes.Add( 7 );                                    UT_EQ( static_cast<size_t>(7),  boxes.size() );  UT_EQ(       7, boxes[6].Unbox<integer>());
        boxes.clear(); boxes.Add( 5 );                     UT_EQ( static_cast<size_t>(1),  boxes.size() );  UT_EQ(       5, boxes[0].Unbox<integer>());
        boxes.clear(); boxes.Add( 5,6 );                   UT_EQ( static_cast<size_t>(2),  boxes.size() );  UT_EQ(       6, boxes[1].Unbox<integer>());
        #else
            Boxes boxes;
        #endif

        boxes.clear(); boxes.Add( 5, A_CHAR("xyz") );              UT_EQ( static_cast<size_t>(2),  boxes.size() );  UT_EQ(   A_CHAR("xyz"), boxes[1].Unbox<String   >());
        boxes.clear(); boxes.Add( 5, String64(A_CHAR("xyz")) );    UT_EQ( static_cast<size_t>(2),  boxes.size() );  UT_EQ(   A_CHAR("xyz"), boxes[1].Unbox<String   >());
        boxes.clear(); boxes.Add(A_CHAR("Hello "), A_CHAR("World") );UT_EQ( static_cast<size_t>(2),  boxes.size() );  UT_EQ( A_CHAR("World"), boxes[1].Unbox<String   >());

        Boxes boxes2(A_CHAR("List1"), A_CHAR("List2") );
        boxes.Add( boxes2 );                               UT_EQ( static_cast<size_t>(4),  boxes.size() );
        boxes.Add( Box(boxes2) );                          UT_EQ( static_cast<size_t>(6),  boxes.size() );
        UT_EQ( A_CHAR("Hello "), boxes[0].Unbox<String   >());
        UT_EQ( A_CHAR("World") , boxes[1].Unbox<String   >());
        UT_EQ( A_CHAR("List1") , boxes[2].Unbox<String   >());
        UT_EQ( A_CHAR("List2") , boxes[3].Unbox<String   >());
        UT_EQ( A_CHAR("List1") , boxes[4].Unbox<String   >());
        UT_EQ( A_CHAR("List2") , boxes[5].Unbox<String   >());
    }
}

#if ALIB_DEBUG

UT_METHOD(Boxing_CustomClasses)
{
    UT_INIT();

    using namespace aworx::lib::boxing;

    // uncomment and watch output. Not easily testable otherwise
    //UT_PRINT(( dbgBoxing.Type<int>()                 ));
    //UT_PRINT(( dbgBoxing.Type<double>()              ));
    //UT_PRINT(( dbgBoxing.Type<double*>()             ));
    //UT_PRINT(( dbgBoxing.Type<float>()               ));
    //UT_PRINT(( dbgBoxing.Type<float*>()              ));
    //UT_PRINT(( dbgBoxing.Type<std::string>()         ));
    //UT_PRINT(( dbgBoxing.Type<std::string*>()        ));
    //UT_PRINT(( dbgBoxing.Type<std::map<int,int>>()   ));
    //UT_PRINT(( dbgBoxing.Type<std::map<int,int>*>()  ));
    //UT_PRINT(( dbgBoxing.Type<int[5]>()              ));
    //UT_PRINT(( dbgBoxing.Type<std::nullptr_t>()   ));
    //UT_PRINT(( dbgBoxing.Dump()   ));



    UT_PRINT( "### Boxing: SmallClassDefaultBoxing ###" );
    {
        SmallClassDefaultBoxing mb;
        Box bMB1( mb );     UT_TRUE( bMB1.IsType<SmallClassDefaultBoxing >() );
        Box bMB2( &mb );    UT_TRUE( bMB2.IsType<SmallClassDefaultBoxing >() );

        UT_FALSE( TT_IsCustomized<SmallClassDefaultBoxing >::value )
        UT_FALSE( TT_IsCustomized<SmallClassDefaultBoxing*>::value )
        UT_TRUE ( TT_IsUnboxable <SmallClassDefaultBoxing >::value )
        UT_FALSE( TT_IsUnboxable <SmallClassDefaultBoxing*>::value )
        UT_FALSE( TT_IsLocked    <SmallClassDefaultBoxing >::value )
        UT_FALSE( TT_IsLocked    <SmallClassDefaultBoxing*>::value )
        UT_FALSE( TT_IsNotBoxable<SmallClassDefaultBoxing>::value )
        UT_FALSE( TT_IsNotBoxable<SmallClassDefaultBoxing*>::value )
    }

    UT_PRINT( "### Boxing: SmallClassDefaultBoxing ###" );
    {
        BigClassDefaultBoxing mb;
        Box bMB1( mb );     UT_TRUE( bMB1.IsType<BigClassDefaultBoxing  *>() );
        Box bMB2( &mb );    UT_TRUE( bMB2.IsType<BigClassDefaultBoxing  *>() );

        UT_FALSE( TT_IsCustomized<BigClassDefaultBoxing >::value )
        UT_FALSE( TT_IsCustomized<BigClassDefaultBoxing*>::value )
        UT_FALSE( TT_IsUnboxable <BigClassDefaultBoxing >::value )
        UT_TRUE ( TT_IsUnboxable <BigClassDefaultBoxing*>::value )
        UT_FALSE( TT_IsLocked    <BigClassDefaultBoxing >::value )
        UT_FALSE( TT_IsLocked    <BigClassDefaultBoxing*>::value )
        UT_FALSE( TT_IsNotBoxable<BigClassDefaultBoxing>::value )
        UT_FALSE( TT_IsNotBoxable<BigClassDefaultBoxing*>::value )
    }

    {
        SmallNotCopyConstr mb(5,"NDFC");
        Box bMB1( mb );     UT_TRUE( bMB1.IsType<SmallNotCopyConstr*>() );
        Box bMB2( &mb );    UT_TRUE( bMB2.IsType<SmallNotCopyConstr*>() );

        UT_PRINT(  aworx::DbgBoxing::TypeInfo<SmallNotCopyConstr >(bMB1) );
        UT_PRINT(  aworx::DbgBoxing::TypeInfo<SmallNotCopyConstr*>()     );

        UT_EQ   ( 5,   bMB1.Unbox<SmallNotCopyConstr*>()->i   );

        UT_FALSE( TT_IsCustomized<SmallNotCopyConstr >::value )
        UT_FALSE( TT_IsCustomized<SmallNotCopyConstr*>::value )
        UT_FALSE( TT_IsUnboxable <SmallNotCopyConstr >::value )
        UT_TRUE ( TT_IsUnboxable <SmallNotCopyConstr*>::value )
        UT_FALSE( TT_IsLocked    <SmallNotCopyConstr >::value )
        UT_FALSE( TT_IsLocked    <SmallNotCopyConstr*>::value )
        UT_FALSE( TT_IsNotBoxable<SmallNotCopyConstr>::value )
        UT_FALSE( TT_IsNotBoxable<SmallNotCopyConstr*>::value )
    }

    {
        SmallNotCopyConstrCustom mb(5,"NDFC");
        Box bMB1( mb );     UT_TRUE( bMB1.IsType<SmallNotCopyConstrCustom>() );
        Box bMB2( &mb );    UT_TRUE( bMB2.IsType<SmallNotCopyConstrCustom>() );

        UT_PRINT(  aworx::DbgBoxing::TypeInfo<SmallNotCopyConstrCustom  >(bMB1) );
        UT_PRINT(  aworx::DbgBoxing::TypeInfo<SmallNotCopyConstrCustom* >() );

//        UT_EQ( 5, bMB1.Unbox<SmallNotCopyConstrCustom >().i   );
      //UT_EQ( 5, bMB1.Unbox<SmallNotCopyConstrCustom*>()->i  ); // has to cause static assertion

        UT_TRUE ( TT_IsCustomized<SmallNotCopyConstrCustom >::value )
        UT_FALSE( TT_IsCustomized<SmallNotCopyConstrCustom*>::value )
        UT_TRUE ( TT_IsUnboxable <SmallNotCopyConstrCustom >::value )
        UT_FALSE( TT_IsUnboxable <SmallNotCopyConstrCustom*>::value )
        UT_FALSE( TT_IsLocked    <SmallNotCopyConstrCustom >::value )
        UT_FALSE( TT_IsLocked    <SmallNotCopyConstrCustom*>::value )
        UT_FALSE( TT_IsNotBoxable<SmallNotCopyConstrCustom>::value )
        UT_FALSE( TT_IsNotBoxable<SmallNotCopyConstrCustom*>::value )
    }

    {
        ClassWithExplicitPointerBoxing mb;
        Box bMB1( mb );     UT_TRUE( bMB1.IsType<ClassWithExplicitPointerBoxing*>() );
        Box bMB2( &mb );    UT_TRUE( bMB2.IsType<ClassWithExplicitPointerBoxing*>() );

        UT_PRINT(  aworx::DbgBoxing::TypeInfo<ClassWithExplicitPointerBoxing >() );
        UT_PRINT(  aworx::DbgBoxing::TypeInfo<ClassWithExplicitPointerBoxing*>() );

      //UT_EQ( 42, bMB1.Unbox<ClassWithExplicitPointerBoxing >().i   ); // has to cause static assertion
        UT_EQ( 42, bMB1.Unbox<ClassWithExplicitPointerBoxing*>()->i );

        UT_FALSE( TT_IsCustomized<ClassWithExplicitPointerBoxing >::value )
        UT_TRUE ( TT_IsCustomized<ClassWithExplicitPointerBoxing*>::value )
        UT_FALSE( TT_IsUnboxable <ClassWithExplicitPointerBoxing >::value )
        UT_TRUE ( TT_IsUnboxable <ClassWithExplicitPointerBoxing*>::value )
        UT_FALSE( TT_IsLocked    <ClassWithExplicitPointerBoxing >::value )
        UT_FALSE( TT_IsLocked    <ClassWithExplicitPointerBoxing*>::value )
        UT_FALSE( TT_IsNotBoxable<ClassWithExplicitPointerBoxing>::value )
        UT_FALSE( TT_IsNotBoxable<ClassWithExplicitPointerBoxing*>::value )
    }


    {
        ClassWithValueAndPointerMapping mb;
        Box bMB1( mb );     UT_TRUE( bMB1.IsType<ClassWithValueAndPointerMapping >() );
        Box bMB2( &mb );    UT_TRUE( bMB2.IsType<ClassWithValueAndPointerMapping*>() );

        UT_PRINT(  aworx::DbgBoxing::TypeInfo<ClassWithValueAndPointerMapping >() );
        UT_PRINT(  aworx::DbgBoxing::TypeInfo<ClassWithValueAndPointerMapping*>() );

        UT_EQ( 43, bMB1.Unbox<ClassWithValueAndPointerMapping >().i   );
        UT_EQ( 43, bMB2.Unbox<ClassWithValueAndPointerMapping*>()->i );

        UT_TRUE ( TT_IsCustomized<ClassWithValueAndPointerMapping >::value )
        UT_TRUE ( TT_IsCustomized<ClassWithValueAndPointerMapping*>::value )
        UT_TRUE ( TT_IsUnboxable <ClassWithValueAndPointerMapping >::value )
        UT_TRUE ( TT_IsUnboxable <ClassWithValueAndPointerMapping*>::value )
        UT_FALSE( TT_IsLocked    <ClassWithValueAndPointerMapping >::value )
        UT_FALSE( TT_IsLocked    <ClassWithValueAndPointerMapping*>::value )
        UT_FALSE( TT_IsNotBoxable<ClassWithValueAndPointerMapping>::value )
        UT_FALSE( TT_IsNotBoxable<ClassWithValueAndPointerMapping*>::value )
    }

    {
        ClassWithValueAndPointerCustom mb;
        Box bMB1( mb );     UT_TRUE( bMB1.IsType<ClassWithValueAndPointerCustom >() );
        Box bMB2( &mb );    UT_TRUE( bMB2.IsType<ClassWithValueAndPointerCustom*>() );

        UT_PRINT(  aworx::DbgBoxing::TypeInfo<ClassWithValueAndPointerCustom >() );
        UT_PRINT(  aworx::DbgBoxing::TypeInfo<ClassWithValueAndPointerCustom*>() );

        UT_EQ( 44, bMB1.Unbox<ClassWithValueAndPointerCustom >().i   );
        UT_EQ( 44, bMB2.Unbox<ClassWithValueAndPointerCustom*>()->i );

        UT_TRUE ( TT_IsCustomized<ClassWithValueAndPointerCustom >::value )
        UT_TRUE ( TT_IsCustomized<ClassWithValueAndPointerCustom*>::value )
        UT_TRUE ( TT_IsUnboxable <ClassWithValueAndPointerCustom >::value )
        UT_TRUE ( TT_IsUnboxable <ClassWithValueAndPointerCustom*>::value )
        UT_FALSE( TT_IsLocked    <ClassWithValueAndPointerCustom >::value )
        UT_FALSE( TT_IsLocked    <ClassWithValueAndPointerCustom*>::value )
        UT_FALSE( TT_IsNotBoxable<ClassWithValueAndPointerCustom>::value )
        UT_FALSE( TT_IsNotBoxable<ClassWithValueAndPointerCustom*>::value )
    }

    {
        ClassVNotUnboxable mb;
        Box bMB1( mb );     //UT_TRUE( bMB1.IsType<ClassVNotUnboxable >() );
        Box bMB2( &mb );    //UT_TRUE( bMB2.IsType<ClassVNotUnboxable*>() );

        UT_PRINT(  aworx::DbgBoxing::TypeInfo<ClassVNotUnboxable >() );
        UT_PRINT(  aworx::DbgBoxing::TypeInfo<ClassVNotUnboxable*>() );

        // UT_EQ( 44, bMB1.Unbox<ClassVNotUnboxable >().i   );
        // UT_EQ( 44, bMB2.Unbox<ClassVNotUnboxable*>()->i );

        UT_TRUE ( TT_IsCustomized<ClassVNotUnboxable >::value )
        UT_FALSE( TT_IsCustomized<ClassVNotUnboxable*>::value )
        UT_FALSE( TT_IsUnboxable <ClassVNotUnboxable >::value )
        UT_FALSE( TT_IsUnboxable <ClassVNotUnboxable*>::value )
        UT_TRUE ( TT_IsLocked    <ClassVNotUnboxable >::value )
        UT_FALSE( TT_IsLocked    <ClassVNotUnboxable*>::value )
        UT_FALSE( TT_IsNotBoxable<ClassVNotUnboxable>::value )
        UT_FALSE( TT_IsNotBoxable<ClassVNotUnboxable*>::value )
    }

    {
        ClassPNotUnboxable mb;
        Box bMB1( mb );     //UT_TRUE( bMB1.IsType<ClassPNotUnboxable >() );
        Box bMB2( &mb );    //UT_TRUE( bMB2.IsType<ClassPNotUnboxable*>() );

        UT_PRINT(  aworx::DbgBoxing::TypeInfo<ClassPNotUnboxable >() );
        UT_PRINT(  aworx::DbgBoxing::TypeInfo<ClassPNotUnboxable*>() );

        // UT_EQ( 44, bMB1.Unbox<ClassPNotUnboxable >().i   );
        // UT_EQ( 44, bMB2.Unbox<ClassPNotUnboxable*>()->i );

        UT_FALSE( TT_IsCustomized<ClassPNotUnboxable >::value )
        UT_TRUE ( TT_IsCustomized<ClassPNotUnboxable*>::value )
        UT_FALSE( TT_IsUnboxable <ClassPNotUnboxable >::value )
        UT_FALSE( TT_IsUnboxable <ClassPNotUnboxable*>::value )
        UT_FALSE( TT_IsLocked    <ClassPNotUnboxable >::value )
        UT_TRUE ( TT_IsLocked    <ClassPNotUnboxable*>::value )
        UT_FALSE( TT_IsNotBoxable<ClassPNotUnboxable >::value )
        UT_FALSE( TT_IsNotBoxable<ClassPNotUnboxable*>::value )
    }

    {
        ClassBothNotUnboxable mb;
        Box bMB1( mb );     //UT_TRUE( bMB1.IsType<ClassBothNotUnboxable >() );
        Box bMB2( &mb );    //UT_TRUE( bMB2.IsType<ClassBothNotUnboxable*>() );

        UT_PRINT(  aworx::DbgBoxing::TypeInfo<ClassBothNotUnboxable >() );
        UT_PRINT(  aworx::DbgBoxing::TypeInfo<ClassBothNotUnboxable*>() );

        //UT_EQ( 44, bMB1.Unbox<ClassBothNotUnboxable >().i   );
        //UT_EQ( 44, bMB2.Unbox<ClassBothNotUnboxable*>()->i );

        UT_TRUE ( TT_IsCustomized<ClassBothNotUnboxable >::value )
        UT_TRUE ( TT_IsCustomized<ClassBothNotUnboxable*>::value )
        UT_FALSE( TT_IsUnboxable <ClassBothNotUnboxable >::value )
        UT_FALSE( TT_IsUnboxable <ClassBothNotUnboxable*>::value )
        UT_TRUE ( TT_IsLocked    <ClassBothNotUnboxable >::value )
        UT_TRUE ( TT_IsLocked    <ClassBothNotUnboxable*>::value )
        UT_FALSE( TT_IsNotBoxable<ClassBothNotUnboxable>::value )
        UT_FALSE( TT_IsNotBoxable<ClassBothNotUnboxable*>::value )
    }

    {
        //ClassVNotBoxable mb;
        //Box bMB1; //( mb );     //UT_TRUE( bMB1.IsType<ClassVNotBoxable >() );
        //Box bMB2; //( &mb );    //UT_TRUE( bMB2.IsType<ClassVNotBoxable*>() );

        // UT_PRINT(  aworx::DbgBoxing::TypeInfo<ClassVNotBoxable >() );
        // UT_PRINT(  aworx::DbgBoxing::TypeInfo<ClassVNotBoxable*>() );

        //UT_EQ( 44, bMB1.Unbox<ClassVNotBoxable >().i   );
        //UT_EQ( 44, bMB2.Unbox<ClassVNotBoxable*>()->i );

        UT_TRUE ( TT_IsCustomized<ClassVNotBoxable >::value )
        UT_FALSE( TT_IsCustomized<ClassVNotBoxable*>::value )
        UT_FALSE( TT_IsUnboxable <ClassVNotBoxable >::value )
        UT_FALSE( TT_IsUnboxable <ClassVNotBoxable*>::value )
        UT_TRUE ( TT_IsLocked    <ClassVNotBoxable >::value )
        UT_FALSE( TT_IsLocked    <ClassVNotBoxable*>::value )
        UT_TRUE ( TT_IsNotBoxable<ClassVNotBoxable >::value )
        UT_TRUE ( TT_IsNotBoxable<ClassVNotBoxable*>::value )
    }

    {
        //ClassPNotBoxable mb;
        //Box bMB1; //( mb );     //UT_TRUE( bMB1.IsType<ClassPNotBoxable >() );
        //Box bMB2; //( &mb );    //UT_TRUE( bMB2.IsType<ClassPNotBoxable*>() );

        // UT_PRINT(  aworx::DbgBoxing::TypeInfo<ClassPNotBoxable >() );
        // UT_PRINT(  aworx::DbgBoxing::TypeInfo<ClassPNotBoxable*>() );

        //UT_EQ( 44, bMB1.Unbox<ClassPNotBoxable >().i   );
        //UT_EQ( 44, bMB2.Unbox<ClassPNotBoxable*>()->i );

        UT_FALSE( TT_IsCustomized<ClassPNotBoxable >::value )
        UT_TRUE ( TT_IsCustomized<ClassPNotBoxable*>::value )
        UT_FALSE( TT_IsUnboxable <ClassPNotBoxable >::value )
        UT_FALSE( TT_IsUnboxable <ClassPNotBoxable*>::value )
        UT_FALSE( TT_IsLocked    <ClassPNotBoxable >::value )
        UT_TRUE ( TT_IsLocked    <ClassPNotBoxable*>::value )
        UT_TRUE ( TT_IsNotBoxable<ClassPNotBoxable >::value )
        UT_TRUE ( TT_IsNotBoxable<ClassPNotBoxable*>::value )
    }

    {
        //ClassBothNotBoxable mb;
        //Box bMB1;//( mb );     //UT_TRUE( bMB1.IsType<ClassBothNotBoxable >() );
        //Box bMB2;//( &mb );    //UT_TRUE( bMB2.IsType<ClassBothNotBoxable*>() );

        // UT_PRINT(  aworx::DbgBoxing::TypeInfo<ClassBothNotBoxable >() );
        // UT_PRINT(  aworx::DbgBoxing::TypeInfo<ClassBothNotBoxable*>() );

        // UT_EQ( 44, bMB1.Unbox<ClassBothNotBoxable >().i   );
        //UT_EQ( 44, bMB2.Unbox<ClassBothNotBoxable*>()->i );

        UT_TRUE ( TT_IsCustomized<ClassBothNotBoxable >::value )
        UT_TRUE ( TT_IsCustomized<ClassBothNotBoxable*>::value )
        UT_FALSE( TT_IsUnboxable <ClassBothNotBoxable >::value )
        UT_FALSE( TT_IsUnboxable <ClassBothNotBoxable*>::value )
        UT_TRUE ( TT_IsLocked    <ClassBothNotBoxable*>::value )
        UT_TRUE ( TT_IsLocked    <ClassBothNotBoxable*>::value )
        UT_TRUE ( TT_IsNotBoxable<ClassBothNotBoxable >::value )
        UT_TRUE ( TT_IsNotBoxable<ClassBothNotBoxable*>::value )
    }

    {
        ClassVNotBoxablePBoxable mb;
        //Box bMB1( mb );     UT_TRUE( bMB1.IsType<ClassVNotBoxablePBoxable >() );
        Box bMB2( &mb );    UT_TRUE( bMB2.IsType<ClassVNotBoxablePBoxable*>() );

        //UT_PRINT(  aworx::DbgBoxing::TypeInfo<ClassVNotBoxablePBoxable >() );
        UT_PRINT(  aworx::DbgBoxing::TypeInfo<ClassVNotBoxablePBoxable*>() );

        //UT_EQ( 44, bMB1.Unbox<ClassVNotBoxablePBoxable >().i   );
        UT_EQ( 44, bMB2.Unbox<ClassVNotBoxablePBoxable*>()->i );

        UT_TRUE ( TT_IsCustomized<ClassVNotBoxablePBoxable >::value )
        UT_TRUE ( TT_IsCustomized<ClassVNotBoxablePBoxable*>::value )
        UT_FALSE( TT_IsUnboxable <ClassVNotBoxablePBoxable >::value )
        UT_TRUE ( TT_IsUnboxable <ClassVNotBoxablePBoxable*>::value )
        UT_TRUE ( TT_IsLocked    <ClassVNotBoxablePBoxable >::value )
        UT_FALSE( TT_IsLocked    <ClassVNotBoxablePBoxable*>::value )
        UT_TRUE ( TT_IsNotBoxable<ClassVNotBoxablePBoxable >::value )
        UT_FALSE( TT_IsNotBoxable<ClassVNotBoxablePBoxable*>::value )
    }

    {
        ClassPNotBoxableVBoxable mb;
        Box bMB1( mb );     UT_TRUE( bMB1.IsType<ClassPNotBoxableVBoxable >() );
        //Box bMB2( &mb );    UT_TRUE( bMB2.IsType<ClassPNotBoxableVBoxable*>() );

        UT_PRINT(  aworx::DbgBoxing::TypeInfo<ClassPNotBoxableVBoxable >() );
        //UT_PRINT(  aworx::DbgBoxing::TypeInfo<ClassPNotBoxableVBoxable*>() );

        UT_EQ( 44, bMB1.Unbox<ClassPNotBoxableVBoxable >().i   );
        //UT_EQ( 44, bMB2.Unbox<ClassPNotBoxableVBoxable*>()->i );

        UT_TRUE ( TT_IsCustomized<ClassPNotBoxableVBoxable >::value )
        UT_TRUE ( TT_IsCustomized<ClassPNotBoxableVBoxable*>::value )
        UT_TRUE ( TT_IsUnboxable <ClassPNotBoxableVBoxable >::value )
        UT_FALSE( TT_IsUnboxable <ClassPNotBoxableVBoxable*>::value )
        UT_FALSE( TT_IsLocked    <ClassPNotBoxableVBoxable >::value )
        UT_TRUE ( TT_IsLocked    <ClassPNotBoxableVBoxable*>::value )
        UT_FALSE( TT_IsNotBoxable<ClassPNotBoxableVBoxable >::value )
        UT_TRUE ( TT_IsNotBoxable<ClassPNotBoxableVBoxable*>::value )
    }
}
#endif


extern
AString asTestFAppend;
AString asTestFAppend;
extern
void testFAppend(AWorxUnitTesting& ut, const Box& box,  const String& val );
void testFAppend(AWorxUnitTesting& ut, const Box& box,  const String& val )
{
    asTestFAppend.Reset();
    UT_TRUE( box.GetFunction<FAppend<character>>( Reach::Local ) );
    box.Call<FAppend<character>>( asTestFAppend );
    UT_EQ( val, asTestFAppend );
}

UT_METHOD(Boxing_Interface_FAppend)
{
    UT_INIT();
    UT_PRINT( "### Boxing: SmallClassDefaultBoxing ###" );

    testFAppend(ut, 5.2, A_CHAR("5.2") );
    testFAppend(ut, 5U,  A_CHAR("5") );
    testFAppend(ut,  5,  A_CHAR("5") );
    testFAppend(ut, A_CHAR("Hello"), A_CHAR("Hello") );
    testFAppend(ut, CString(A_CHAR("atstring")),  A_CHAR("atstring"));
    testFAppend(ut, AString(A_CHAR("anastring")),  A_CHAR("anastring"));
    testFAppend(ut, String64(A_CHAR("string64")),  A_CHAR("string64"));
}


UT_CLASS_END

}; //namespace

#endif // !defined(ALIB_UT_SELECT) || defined(ALIB_UT_BOXING)
