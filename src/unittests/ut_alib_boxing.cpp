// #################################################################################################
//  aworx - Unit Tests
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alox.hpp"
#include "alib/compatibility/std_string.hpp"

#if !defined (HPP_ALIB_TIME_TIMEPOINT)
    #include "alib/time/timestamp.hpp"
#endif

#define TESTCLASSNAME       CPP_ALib_Boxing
#include "aworx_unittests.hpp"
#include "alib/alox/loggers/memorylogger.hpp"
#include "alib/strings/boxing/debug.hpp"


namespace std
{
    extern basic_stringstream<aworx::nchar>     testOutputStreamN;
    extern basic_stringstream<aworx::character> testOutputStream;
}


using namespace std;
using namespace aworx;



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


class ClassWithStdBoxing
{
    public:
        int          i= 73;
        const char*  cp= "This is my a ClassWithStdBoxing instance";
};

class ClassWithValueBoxingNDFC //not default constructible
{
    public:
        ClassWithValueBoxingNDFC(int pa, const char* pc)
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

class ClassWithValueAndPointer
{
    public:
        int          i      =43;
        const char*  cp     ="CWBOTH";
};

class ClassWithValueAndPointerUnboxing
{
    public:
        int          i      =44;
        const char*  cp     ="CWBOTH";
};

namespace aworx { namespace lib { namespace boxing {

ALIB_BOXING_SPECIALIZE_CB_CUB(ClassWithValueBoxingNDFC, ClassWithValueBoxingNDFC, false, true)

void T_Boxing<ClassWithValueBoxingNDFC>::Boxing( Box& box, ClassWithValueBoxingNDFC const& value )
{
    box.data.Value= reinterpret_cast<boxvalue>( value.cp );
    box.data.Length= value.i;
}

ClassWithValueBoxingNDFC T_Boxing<ClassWithValueBoxingNDFC>::Unboxing( const Box& box )
{
    ClassWithValueBoxingNDFC result( 0, nullptr );
    result.cp= reinterpret_cast<const char*>( box.data.Value );
    result.i=       static_cast<int>        ( box.data.Length );
    return result;
}

ALIB_BOXING_SPECIALIZE(ClassWithExplicitPointerBoxing*, ClassWithExplicitPointerBoxing*, false, true)
ALIB_BOXING_SPECIALIZE(ClassWithValueAndPointer*, ClassWithValueAndPointer*, false, true)
ALIB_BOXING_SPECIALIZE_CB_CUB(ClassWithValueAndPointer , ClassWithValueAndPointer , false, true)
void T_Boxing<ClassWithValueAndPointer>::Boxing( Box& box, ClassWithValueAndPointer const& value )
{
    box.data.Value= reinterpret_cast<boxvalue>( value.cp );
    box.data.Length= value.i;
}

ClassWithValueAndPointer T_Boxing<ClassWithValueAndPointer>::Unboxing( const Box& box )
{
    ClassWithValueAndPointer result;
    result.cp= reinterpret_cast<const char*>( box.data.Value );
    result.i=       static_cast<int>        ( box.data.Length );
    return result;
}

ALIB_BOXING_SPECIALIZE_CB_CUB(ClassWithValueAndPointerUnboxing , ClassWithValueAndPointerUnboxing , false, true)
ALIB_BOXING_SPECIALIZE_CB_CUB(ClassWithValueAndPointerUnboxing*, ClassWithValueAndPointerUnboxing*, false, true)
void T_Boxing<ClassWithValueAndPointerUnboxing>::Boxing( Box& box, ClassWithValueAndPointerUnboxing const& value )
{
    box.data.Value= reinterpret_cast<boxvalue>( value.cp );
    box.data.Length= value.i;
}
ClassWithValueAndPointerUnboxing T_Boxing<ClassWithValueAndPointerUnboxing>::Unboxing( const Box& box )
{
    ClassWithValueAndPointerUnboxing result;
    result.cp= reinterpret_cast<const char*>( box.data.Value );
    result.i=       static_cast<int>        ( box.data.Length );
    return result;
}

void T_Boxing<ClassWithValueAndPointerUnboxing*>::Boxing( Box& box, ClassWithValueAndPointerUnboxing* const& value )
{
    box.data.Value= reinterpret_cast<boxvalue>( value );
    box.data.Length= 0;
}
ClassWithValueAndPointerUnboxing* T_Boxing<ClassWithValueAndPointerUnboxing*>::Unboxing( const Box& box )
{
    return reinterpret_cast<ClassWithValueAndPointerUnboxing*>( box.data.Value );
}


}}}

namespace ut_aworx {


UT_CLASS()

//--------------------------------------------------------------------------------------------------
//--- From Dox
//--------------------------------------------------------------------------------------------------
#if ALIB_DEBUG

UT_METHOD(Boxing_Dox)
{
    UT_INIT();
    UT_PRINT("") UT_PRINT( "### Boxing Dox Samples ###" );

    dox_boxing_implicit_conversion_sample::test();
    dox_boxing_implicit_conversion_sample2::test();
    dox_boxing_implicit_conversion_sample2::test2();

    dox_boxing_sample_1::sampleFunc();
    ut.WriteResultFile( "DOX_ALIB_BOXING_SAMPLE_1.txt", testOutputStreamN.str() );
    testOutputStreamN.str("");

    dox_boxing_sample_2::sampleFunc();
    ut.WriteResultFile( "DOX_ALIB_BOXING_SAMPLE_2.txt", testOutputStreamN.str() );
    testOutputStreamN.str("");

    dox_boxing_sample_2::sampleFunc2();
    ut.WriteResultFile( "DOX_ALIB_BOXING_SAMPLE_2_3.txt", testOutputStreamN.str() );
    testOutputStreamN.str("");


    dox_boxing_bijective::sampleFunc();
    ut.WriteResultFile( "DOX_ALIB_BOXING_SAMPLE_BIJ.txt", testOutputStreamN.str() );
    testOutputStreamN.str("");


    dox_boxing_sample_arr::sampleFunc();
    ut.WriteResultFile( "DOX_ALIB_BOXING_SAMPLE_ARR.txt", testOutputStreamN.str() );
    testOutputStreamN.str("");

    dox_boxing_sample_arr::sampleFunc2();
    ut.WriteResultFile( "DOX_ALIB_BOXING_SAMPLE_ARR_2.txt", testOutputStreamN.str() );
    testOutputStreamN.str("");

    dox_boxing_sample_ftypes::sampleFunc();
    ut.WriteResultFile( "DOX_ALIB_BOXING_SAMPLE_FTYPES_1.txt", testOutputStreamN.str() );
    testOutputStreamN.str("");

    dox_boxing_sample_boxedas::sampleFunc();
    ut.WriteResultFile( "DOX_ALIB_BOXING_SAMPLE_BOXEDAS.txt", testOutputStreamN.str() );
    testOutputStreamN.str("");

    dox_boxing_sample_if::sampleFunc();
    ut.WriteResultFile( "DOX_ALIB_BOXING_SAMPLE_IF_1.txt", testOutputStreamN.str() );
    testOutputStreamN.str("");

    dox_boxing_sample_strings::sampleFunc();
    ut.WriteResultFile( "DOX_ALIB_BOXING_SAMPLE_STRINGS.txt", testOutputStreamN.str() );
    testOutputStreamN.str("");

    dox_boxing_sample_recipe::sampleFunc();
    ut.WriteResultFile( "DOX_ALIB_BOXING_SAMPLE_RECIPE_1.txt", testOutputStreamN.str() );
    testOutputStreamN.str("");

    dox_boxing_sample_variadic::sampleFunc();

    dox_boxing_sample_boxes::sampleFunc();
    testOutputStreamN.str("");
    dox_boxing_sample_boxes::sampleFunc2();
    ut.WriteResultFile( "DOX_ALIB_BOXING_BOXES_SAMPLE_VARIADIC_CALL_1.txt", testOutputStreamN.str() );
    testOutputStreamN.str("");
    dox_boxing_sample_boxes::sampleFunc3();
    ut.WriteResultFile( "DOX_ALIB_BOXING_BOXES_SAMPLE_VARIADIC_CALL_2.txt", testOutputStreamN.str() );
    testOutputStreamN.str("");

    dox_boxing_sample_boxes_box::sampleFunc();
    ut.WriteResultFile( "DOX_ALIB_BOXING_BOXES_SAMPLE_VARIADIC_SINGLE.txt", testOutputStreamN.str() );
    testOutputStreamN.str("");


    dox_boxing_sample_debug::sampleFunc();
    ut.WriteResultFile( "DOX_ALIB_BOXING_DEBUG_1.txt", testOutputStreamN.str() );
    testOutputStreamN.str("");

    dox_boxing_sample_debug::sampleFunc2();
    ut.WriteResultFile( "DOX_ALIB_BOXING_DEBUG_2.txt", testOutputStreamN.str() );
    testOutputStreamN.str("");

//TODO: back in    dox_boxing_sample_debug::sampleFunc3();
//TODO: back in    ut.WriteResultFile( "DOX_ALIB_BOXING_DEBUG_3.txt", testOutputStreamN.str() );
//TODO: back in    testOutputStreamN.str("");

    dox_boxing_sample_debug::sampleFunc4();
    ut.WriteResultFile( "DOX_ALIB_BOXING_DEBUG_4.txt", testOutputStreamN.str() );
    testOutputStreamN.str("");


    dox_boxing_sample_compare::sampleFunc();
    ut.WriteResultFile( "DOX_ALIB_BOXING_COMPARE.txt", testOutputStreamN.str() );
    testOutputStreamN.str("");

}
#endif

//--------------------------------------------------------------------------------------------------
//--- FTypes Equals/IsNull/IsEmpty
//--------------------------------------------------------------------------------------------------
void testEquals( AWorxUnitTesting& ut, bool exp, const Box& b1, const Box& b2 )
{
    UT_EQ( exp, ( b1.Invoke<IEquals, bool>(b2) ) );
    UT_EQ( exp, ( b2.Invoke<IEquals, bool>(b1) ) );
}

template<typename TBoxable>
inline void testBoxingArray(AWorxUnitTesting& ut,  const TBoxable& val, const Box& box )
{
    UT_TRUE(  box.IsArrayOf<typename std::remove_extent<TBoxable>::type>() );
    UT_TRUE( ( box.Invoke<IEquals, bool>(val) ) );
}

template<typename TBoxable>
inline void testBoxing(AWorxUnitTesting& ut,  const TBoxable& val, const Box& box )
{
    UT_EQ( val, box.Unbox<TBoxable>() );
    UT_TRUE( ( box.Invoke<IEquals, bool>(val) ) );
}


template<typename TBoxable>
inline void testBoxing(AWorxUnitTesting& ut,  TBoxable val, const Box& box1 , const Box& box2)
{
    UT_EQ( val, box1.Unbox<TBoxable>() );
    UT_EQ( val, box2.Unbox<TBoxable>() );

    UT_TRUE( ( box1.Invoke<IEquals, bool>(box2) ) );
    UT_TRUE( ( box2.Invoke<IEquals, bool>(box1) ) );
}

UT_METHOD(Boxing_FundamentalTypes)
{
    UT_INIT();
    UT_PRINT( "### Boxing: Fundamental Types ###" );

    { bool      v=      true;   testBoxing(ut,  true, v, &v );       }
    {  int8_t   v=         1;   testBoxing(ut,  static_cast< int64_t>( 1 )   , v, &v );       }
    {  int16_t  v=         2;   testBoxing(ut,  static_cast< int64_t>( 2 )   , v, &v );       }
    {  int32_t  v=         3;   testBoxing(ut,  static_cast< int64_t>( 3 )   , v, &v );       }
    {  int64_t  v=         4;   testBoxing(ut,  static_cast< int64_t>( 4 )   , v, &v );       }
    { uint8_t   v=         1;   testBoxing(ut,  static_cast<uint64_t>( 1 )   , v, &v );       }
    { uint16_t  v=         2;   testBoxing(ut,  static_cast<uint64_t>( 2 )   , v, &v );       }
    { uint32_t  v=         3;   testBoxing(ut,  static_cast<uint64_t>( 3 )   , v, &v );       }
    { uint64_t  v=         4;   testBoxing(ut,  static_cast<uint64_t>( 4 )   , v, &v );       }
    { float     v=      5.1f;   testBoxing(ut,  static_cast<double  >( 5.1f ), v, &v );       }
    { double    v=      5.2 ;   testBoxing(ut,  static_cast<double  >( 5.2 ) , v, &v );       }

    { char      v=       'c';   testBoxing(ut,      'c' , v );  }
    { wchar_t   v= L'\u03B2';   testBoxing(ut, L'\u03B2', v );  }

#if ALIB_SIZEOF_WCHAR_T == 4
    { char16_t  v= u'\u03B3';   testBoxing(ut, u'\u03B3', v );  }
#else
    { char32_t  v= U'\u03B4';   testBoxing(ut, U'\u03B4', v );  }
#endif

    { const  char    *  v=   "abc";                testBoxingArray(ut,  "abc"               , v);  }
    { const wchar_t  *  v=  L"\u03B1\u03B2\u03B3"; testBoxingArray(ut, L"\u03B1\u03B2\u03B3", v);  }
#if ALIB_SIZEOF_WCHAR_T == 4
    { const  char16_t*  v=  u"\u03B4\u03B5\u03B6"; testBoxingArray(ut, u"\u03B4\u03B5\u03B6", v);  }
#else
    { const  char32_t*  v=  U"\u03B7\u03B8\u03B9"; testBoxingArray(ut, U"\u03B7\u03B8\u03B9", v);  }
#endif

    {  int16_t  v[2]= {16,17};
       int16_t  w[2]= {16,17};                     testBoxingArray(ut,  w, v );       }

}

UT_METHOD(Boxing_FTYPESDefaultInterfaces)
{
    UT_INIT();

    UT_PRINT( "### Boxing: Fundamental Types Default interfaces (Equals/IsNull/IsEmpty) ###" );
    testEquals( ut, true,       5,      5   );
    testEquals( ut, false,      5,      3   );
    testEquals( ut, true,       5L,     5L  );
    testEquals( ut, false,      5L,     3L  );
    testEquals( ut, true,    5.3f,    5.3f  );
    testEquals( ut, true,    5.3f,    5.3   );
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


    UT_PRINT("") UT_PRINT( "### Boxing: Testing Equals: char*/char[] ###" );
    {
        bool result;
        Box helloP( static_cast<const char*>("Hello"));
        Box helloA(                          "Hello");
        Box falseP( static_cast<const char*>("not hello"));
        Box falseA(                          "not hello");
        result= helloA.Invoke<IEquals, bool>( helloA ); UT_EQ( true , result );
        result= helloA.Invoke<IEquals, bool>( helloP ); UT_EQ( true , result );
        result= helloA.Invoke<IEquals, bool>( falseA ); UT_EQ( false, result );
        result= helloA.Invoke<IEquals, bool>( falseP ); UT_EQ( false, result );

        result= helloP.Invoke<IEquals, bool>( helloP ); UT_EQ( true , result );
        result= helloP.Invoke<IEquals, bool>( helloA ); UT_EQ( true , result );
        result= helloP.Invoke<IEquals, bool>( falseP ); UT_EQ( false, result );
        result= helloP.Invoke<IEquals, bool>( falseA ); UT_EQ( false, result );
    }

    UT_PRINT("") UT_PRINT( "### Boxing: Testing Equals: wchar_t*/wchar_[] ###" );
    {
        bool result;
        Box helloP( static_cast<const wchar_t*>( L"Hello"));
        Box helloA(                              L"Hello");
        Box falseP( static_cast<const wchar_t*>( L"not hello"));
        Box falseA(                              L"not hello");
        result= helloA.Invoke<IEquals, bool>( helloA ); UT_EQ( true , result );
        result= helloA.Invoke<IEquals, bool>( helloP ); UT_EQ( true , result );
        result= helloA.Invoke<IEquals, bool>( falseA ); UT_EQ( false, result );
        result= helloA.Invoke<IEquals, bool>( falseP ); UT_EQ( false, result );

        result= helloP.Invoke<IEquals, bool>( helloP ); UT_EQ( true , result );
        result= helloP.Invoke<IEquals, bool>( helloA ); UT_EQ( true , result );
        result= helloP.Invoke<IEquals, bool>( falseP ); UT_EQ( false, result );
        result= helloP.Invoke<IEquals, bool>( falseA ); UT_EQ( false, result );
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

        result= helloAS.Invoke<IEquals, bool>( helloP ); UT_EQ( true , result );
        result= helloAS.Invoke<IEquals, bool>( helloA ); UT_EQ( true , result );
        result= helloAS.Invoke<IEquals, bool>( falseP ); UT_EQ( false, result );
        result= helloAS.Invoke<IEquals, bool>( falseA ); UT_EQ( false, result );

        result= helloA.Invoke<IEquals, bool>( helloAS ); UT_EQ( true , result );
        result= helloP.Invoke<IEquals, bool>( helloAS ); UT_EQ( true , result );
        result= helloA.Invoke<IEquals, bool>( falseAS ); UT_EQ( false, result );
        result= helloP.Invoke<IEquals, bool>( falseAS ); UT_EQ( false, result );
    }

    AString test;
    {
        PreallocatedString<64> pa64("64");        Box bpa64(&pa64);
        PreallocatedString<73> pa73("73");        Box bpa73(pa73);

        test._() << bpa64;      UT_EQ( ASTR("64"), test );
        test._() << bpa73;      UT_EQ( ASTR("73"), test );
    }

    {
        aworx::PreallocatedString<64> pa64("64");     Box bpa64(&pa64);
        aworx::PreallocatedString<73> pa73("73");     Box bpa73(pa73);

        test._() << bpa64;      UT_EQ( ASTR("64"), test);
        test._() << bpa73;      UT_EQ( ASTR("73"), test);
    }

    {
        SLiteral<2> sl2(ASTR("64"));        Box bsl64(&sl2);
        SLiteral<3> sl3(ASTR("123"));       Box bsl123(sl3);

        test._() << bsl64;      UT_EQ( ASTR("64") , test );
        test._() << bsl123;     UT_EQ( ASTR("123"), test );
    }

}


UT_METHOD(Boxing_Std_String)
{
    UT_INIT();

    UT_PRINT("") UT_PRINT( "### Boxing: std::string ###" );
    {
        std::basic_string<character> stdstr( ASTR("Hello") );
        Box b( stdstr );
        UT_EQ( true, b.IsArrayOf<character>() );
        UT_EQ( 5,    b.Length()           );
        UT_EQ( 'H',  b.UnboxElement<character>(0) );

        std::basic_string<character>* stdstrP = &stdstr;
        b= Box( stdstrP );
        UT_EQ( true, b.IsArrayOf<character>() );
        UT_EQ( 5,    b.Length()           );
        UT_EQ( 'H',  b.UnboxElement<character>(0) );

        stdstrP = nullptr;
        b= Box( stdstrP );
        UT_EQ( true, b.IsArrayOf<character>() );
        UT_EQ( 0,    b.Length()          );
    }
}

#if ALIB_FEAT_BOXING_STD_VECTOR
UT_METHOD(Boxing_Std_Vector)
{
    UT_INIT();

    UT_PRINT("") UT_PRINT( "### Boxing: std::vector ###" );
    {
        std::vector<int> vec= {3,4,5};
        Box b(vec);
        UT_EQ( true, b.IsArrayOf<int>() );
        UT_EQ( 3,    b.Length()         );
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
        UT_EQ( 3,    b.Length()         );
        UT_EQ( 3.2,  b.UnboxElement<double>(0) );
        UT_EQ( 4.3,  b.UnboxElement<double>(1) );
        UT_EQ( 5.4,  b.UnboxElement<double>(2) );
        double* array= &b.UnboxElement<double>(0);
        UT_EQ( 3.2,  array[0] );
        UT_EQ( 4.3,  array[1] );
        UT_EQ( 5.4,  array[2] );
    }
    {
        std::vector<std::basic_string<character>> vec= {ASTR("one"), ASTR("two"), ASTR("three")};
        Box b(vec);
        UT_EQ( true, b.IsArrayOf<std::basic_string<character>>() );
        UT_EQ( 3,    b.Length()         );
        UT_EQ( std::basic_string<character>(ASTR("one")),    b.UnboxElement<std::basic_string<character>>(0) );
        UT_EQ( std::basic_string<character>(ASTR("two")),    b.UnboxElement<std::basic_string<character>>(1) );
        UT_EQ( std::basic_string<character>(ASTR("three")),  b.UnboxElement<std::basic_string<character>>(2) );
        std::basic_string<character>* array= &b.UnboxElement<std::basic_string<character>>(0);
        UT_EQ( std::basic_string<character>(ASTR("one")),    array[0] );
        UT_EQ( std::basic_string<character>(ASTR("two")),    array[1] );
        UT_EQ( std::basic_string<character>(ASTR("three")),  array[2] );
    }


    UT_PRINT("") UT_PRINT( "### Boxing: Testing Equals: std::vector<char>/char[] ###" );
    {
        bool result;
        std::vector<character> helloVec= {'H','e','l','l','o'};
        std::vector<character> falseVec= {'n','o','t',' ','H','e','l','l','o'};
        Box helloV( helloVec );
        Box helloA( ASTR("Hello"));
        Box falseP( falseVec );
        Box falseA( ASTR("not hello"));
        result= helloA.Invoke<IEquals, bool>( helloV ); UT_EQ( true , result );
        result= helloA.Invoke<IEquals, bool>( falseP ); UT_EQ( false, result );

        result= helloV.Invoke<IEquals, bool>( helloV ); UT_EQ( true , result );
        result= helloV.Invoke<IEquals, bool>( helloA ); UT_EQ( true , result );
        result= helloV.Invoke<IEquals, bool>( falseP ); UT_EQ( false, result );
        result= helloV.Invoke<IEquals, bool>( falseA ); UT_EQ( false, result );
    }
}

#endif

UT_METHOD(Boxing_Boxes)
{
    UT_INIT();

    UT_PRINT("") UT_PRINT( "### Boxing: class Boxesr ###" );

    {
        Boxes boxes( 1 );                                  UT_EQ( static_cast<size_t>(1),  boxes.size() );  UT_EQ(       1, boxes[0].Unbox<boxed_int>());
        boxes.Add( 2 );                                    UT_EQ( static_cast<size_t>(2),  boxes.size() );  UT_EQ(       2, boxes[1].Unbox<boxed_int>());
        boxes.Add( 3,4,5,6 );                              UT_EQ( static_cast<size_t>(6),  boxes.size() );  UT_EQ(       4, boxes[3].Unbox<boxed_int>());
        boxes.Add( 7 );                                    UT_EQ( static_cast<size_t>(7),  boxes.size() );  UT_EQ(       7, boxes[6].Unbox<boxed_int>());
        boxes.clear(); boxes.Add( 5 );                     UT_EQ( static_cast<size_t>(1),  boxes.size() );  UT_EQ(       5, boxes[0].Unbox<boxed_int>());
        boxes.clear(); boxes.Add( 5,6 );                   UT_EQ( static_cast<size_t>(2),  boxes.size() );  UT_EQ(       6, boxes[1].Unbox<boxed_int>());
        boxes.clear(); boxes.Add( 5, ASTR("xyz") );        UT_EQ( static_cast<size_t>(2),  boxes.size() );  UT_EQ(   ASTR("xyz"), boxes[1].Unbox<String   >());
        boxes.clear(); boxes.Add( 5, String64(ASTR("xyz")) );    UT_EQ( static_cast<size_t>(2),  boxes.size() );  UT_EQ(   ASTR("xyz"), boxes[1].Unbox<String   >());
        boxes.clear(); boxes.Add(ASTR("Hello "), ASTR("World") );      UT_EQ( static_cast<size_t>(2),  boxes.size() );  UT_EQ( ASTR("World"), boxes[1].Unbox<String   >());

        Boxes boxes2(ASTR("List1"), ASTR("List2") );
        boxes.Add( boxes2 );                               UT_EQ( static_cast<size_t>(4),  boxes.size() );
        boxes.Add( Box(boxes2) );                          UT_EQ( static_cast<size_t>(6),  boxes.size() );
        UT_EQ( ASTR("Hello "), boxes[0].Unbox<String   >());
        UT_EQ( ASTR("World") , boxes[1].Unbox<String   >());
        UT_EQ( ASTR("List1") , boxes[2].Unbox<String   >());
        UT_EQ( ASTR("List2") , boxes[3].Unbox<String   >());
        UT_EQ( ASTR("List1") , boxes[4].Unbox<String   >());
        UT_EQ( ASTR("List2") , boxes[5].Unbox<String   >());
    }
}

#if ALIB_DEBUG

UT_METHOD(Boxing_CustomClasses)
{
    UT_INIT();

    aworx::lib::debug::Boxing dbgBoxing;

    // uncomment and watch output. Not easily testable otherwise
    //UT_PRINT(( dbgBoxing.GetBoxingInfo<int>()                 ));
    //UT_PRINT(( dbgBoxing.GetBoxingInfo<double>()              ));
    //UT_PRINT(( dbgBoxing.GetBoxingInfo<double*>()             ));
    //UT_PRINT(( dbgBoxing.GetBoxingInfo<float>()               ));
    //UT_PRINT(( dbgBoxing.GetBoxingInfo<float*>()              ));
    //UT_PRINT(( dbgBoxing.GetBoxingInfo<std::string>()         ));
    //UT_PRINT(( dbgBoxing.GetBoxingInfo<std::string*>()        ));
    //UT_PRINT(( dbgBoxing.GetBoxingInfo<std::map<int,int>>()   ));
    //UT_PRINT(( dbgBoxing.GetBoxingInfo<std::map<int,int>*>()  ));
    //UT_PRINT(( dbgBoxing.GetBoxingInfo<int[5]>()              ));
    //UT_PRINT(( dbgBoxing.GetBoxingInfo<decltype(nullptr)>()   ));
    //UT_PRINT(( dbgBoxing.Dump()   ));



    UT_PRINT( "### Boxing: ClassWithStdBoxing ###" );
    {
        ClassWithStdBoxing mb;
        Box bMB1( mb );     UT_TRUE( bMB1.IsType<ClassWithStdBoxing*>() );
        Box bMB2( &mb );    UT_TRUE( bMB2.IsType<ClassWithStdBoxing*>() );
    }

    {
        ClassWithValueBoxingNDFC mb(5,"NDFC");
        Box bMB1( mb );     UT_TRUE( bMB1.IsType<ClassWithValueBoxingNDFC>() );
        Box bMB2( &mb );    UT_TRUE( bMB2.IsType<ClassWithValueBoxingNDFC>() );

        UT_PRINT(  dbgBoxing.GetBoxingInfo<ClassWithValueBoxingNDFC >() );
        UT_PRINT(  dbgBoxing.GetBoxingInfo<ClassWithValueBoxingNDFC* >() );

        UT_EQ( 5, bMB1.Unbox<ClassWithValueBoxingNDFC>().i   );
      //UT_EQ( 5, bMB1.Unbox<ClassWithValueBoxingNDFC*>()->i ); // has to cause static assertion
    }

    {
        ClassWithExplicitPointerBoxing mb;
        Box bMB1( mb );     UT_TRUE( bMB1.IsType<ClassWithExplicitPointerBoxing*>() );
        Box bMB2( &mb );    UT_TRUE( bMB2.IsType<ClassWithExplicitPointerBoxing*>() );

        UT_PRINT(  dbgBoxing.GetBoxingInfo<ClassWithExplicitPointerBoxing >() );
        UT_PRINT(  dbgBoxing.GetBoxingInfo<ClassWithExplicitPointerBoxing*>() );

      //UT_EQ( 42, bMB1.Unbox<ClassWithExplicitPointerBoxing >().i   ); // has to cause static assertion
        UT_EQ( 42, bMB1.Unbox<ClassWithExplicitPointerBoxing*>()->i );
    }

    {
        ClassWithValueAndPointer mb;
        Box bMB1( mb );     UT_TRUE( bMB1.IsType<ClassWithValueAndPointer >() );
        Box bMB2( &mb );    UT_TRUE( bMB2.IsType<ClassWithValueAndPointer*>() );

        UT_PRINT(  dbgBoxing.GetBoxingInfo<ClassWithValueAndPointer >() );
        UT_PRINT(  dbgBoxing.GetBoxingInfo<ClassWithValueAndPointer*>() );

        UT_EQ( 43, bMB1.Unbox<ClassWithValueAndPointer >().i   );
        UT_EQ( 43, bMB2.Unbox<ClassWithValueAndPointer*>()->i );
    }

    {
        ClassWithValueAndPointerUnboxing mb;
        Box bMB1( mb );     UT_TRUE( bMB1.IsType<ClassWithValueAndPointerUnboxing >() );
        Box bMB2( &mb );    UT_TRUE( bMB2.IsType<ClassWithValueAndPointerUnboxing*>() );

        UT_PRINT(  dbgBoxing.GetBoxingInfo<ClassWithValueAndPointerUnboxing >() );
        UT_PRINT(  dbgBoxing.GetBoxingInfo<ClassWithValueAndPointerUnboxing*>() );

        UT_EQ( 44, bMB1.Unbox<ClassWithValueAndPointerUnboxing >().i   );
        UT_EQ( 44, bMB2.Unbox<ClassWithValueAndPointerUnboxing*>()->i );
    }
}
#endif


AString asTestIApply;
void testIApply(AWorxUnitTesting& ut, const Box& box,  const String& val )
{
    asTestIApply.Clear();
    UT_TRUE( box.HasInterface<IApply<character>>() );
    box.Invoke<IApply<character>>( asTestIApply );
    UT_EQ( val, asTestIApply );
}

UT_METHOD(Boxing_Interface_IApply)
{
    UT_INIT();
    UT_PRINT( "### Boxing: ClassWithStdBoxing ###" );

    testIApply(ut, 5.2, ASTR("5.2") );
    testIApply(ut, 5U,  ASTR("5") );
    testIApply(ut,  5,  ASTR("5") );
    testIApply(ut, ASTR("Hello"), ASTR("Hello") );
    testIApply(ut, TString(ASTR("atstring")),  ASTR("atstring"));
    testIApply(ut, AString(ASTR("anastring")),  ASTR("anastring"));
    testIApply(ut, String64(ASTR("string64")),  ASTR("string64"));
}


UT_CLASS_END

}; //namespace
