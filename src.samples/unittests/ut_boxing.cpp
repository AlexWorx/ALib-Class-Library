// #################################################################################################
//  AWorx ALib Unit Tests
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib_precompile.hpp"
#include "alib_test_selection.hpp"
#if ALIB_UT_BOXING

#if defined(QT_CORE_LIB) // needed here for unity builds to be included before boxing (!)
#   include "ALib.Compatibility.QTCharacters.H"
#endif

#include "ALib.Strings.H"
#include "ALib.ALox.H"
#include "ALib.Strings.StdIOStream.H"
#include "ALib.Compatibility.StdStrings.H"
#include "ALib.Compatibility.StdBoxtraits.H"

#include "ALib.Boxing.H"
#include "ALib.Monomem.H"
#include "ALib.Format.H"

#define TESTCLASSNAME       UT_Boxing
#include "aworx_unittests.hpp"

using namespace std;
using namespace alib;

static_assert( !std::is_trivial                         <Box>::value, "error");
static_assert( !std::is_trivially_default_constructible <Box>::value, "error");
static_assert( !std::is_trivially_constructible         <Box>::value, "error");
static_assert(  std::is_trivially_destructible          <Box>::value, "error");
static_assert(  std::is_trivially_copy_constructible    <Box>::value, "error");
static_assert(  std::is_trivially_move_constructible    <Box>::value, "error");
static_assert(  std::is_trivially_copyable              <Box>::value, "error");
static_assert(  std::is_trivially_copy_assignable       <Box>::value, "error");
static_assert(  std::is_trivially_move_assignable       <Box>::value, "error");


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

class SmallNotCopyConstr //not copy-constructible
{
    public:
        SmallNotCopyConstr(const SmallNotCopyConstr& )= delete;

        SmallNotCopyConstr(int pa, const char* pc)
        :i(pa), cp(pc) {}
            int          i;
            const char*  cp;
};


class SmallNotCopyConstrCustom //not default-constructible
{
    public:
        constexpr SmallNotCopyConstrCustom(const SmallNotCopyConstrCustom& )= delete;

        constexpr SmallNotCopyConstrCustom(int pa, const char* pc)
        :i(pa), cp(pc) {}
            int          i;
            const char*  cp;
};

namespace alib::boxing { namespace detail {
extern  VTable SNGLTN_vt_SmallNotCopyConstrCustom;
        VTable SNGLTN_vt_SmallNotCopyConstrCustom(typeid(SmallNotCopyConstrCustom), typeid(void), VTable::MappingType::Value, alib::boxing::SizeTraits<SmallNotCopyConstrCustom> );
}
template<> struct VTableOptimizationTraits< SmallNotCopyConstrCustom, false > {
static constexpr detail::VTable* Get() { return &detail:: SNGLTN_vt_SmallNotCopyConstrCustom; } };   }


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
    static constexpr void Write( Placeholder& box, SmallNotCopyConstrCustom const& value )
    {
        box.Write( value.cp, value.i );
    }
    static SmallNotCopyConstrCustom Read( const Placeholder& box)
    {
        return SmallNotCopyConstrCustom( int(box.GetLength()),  box.GetPointer<char>() )  ;
    }
)


ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING(ClassWithExplicitPointerBoxing*, ClassWithExplicitPointerBoxing*  )

ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING(ClassWithValueAndPointerMapping       , ClassWithValueAndPointerMapping         )
ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING(ClassWithValueAndPointerMapping*      , ClassWithValueAndPointerMapping*        )


ALIB_BOXING_CUSTOMIZE(ClassWithValueAndPointerCustom , ClassWithValueAndPointerCustom,
    static constexpr void Write( Placeholder& box, ClassWithValueAndPointerCustom const& value )
    {
        box.Write( value.cp, value.i );
    }

    static ClassWithValueAndPointerCustom Read( const Placeholder& box)
    {
        ClassWithValueAndPointerCustom result;
        result.cp=      box.GetPointer<char>();
        result.i=  int( box.GetLength()  );
        return result;
    }
)

ALIB_BOXING_CUSTOMIZE(ClassWithValueAndPointerCustom*, ClassWithValueAndPointerCustom*,
    static constexpr void Write( Placeholder& box, ClassWithValueAndPointerCustom* const& value )
    {
        box.Write( value );
    }
    static ClassWithValueAndPointerCustom* Read( const Placeholder& box)
    {
        return box.GetPointer<ClassWithValueAndPointerCustom>();
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

struct ClassConstexpr2P {
    const float* p1;
    int*  p2;
    constexpr ClassConstexpr2P( const float* pp1, int* pp2 ) : p1(pp1), p2(pp2)    {}
};

ALIB_BOXING_CUSTOMIZE(ClassConstexpr2P, ClassConstexpr2P,
    static constexpr void Write( Placeholder& box, ClassConstexpr2P const& value )
    {
        box.Write( value.p1, value.p2 );
    }
    static ClassConstexpr2P Read( const Placeholder& box)
    {
        return ClassConstexpr2P( box.GetPointer<float>(), box.GetPointer2<int>());
    }
)


namespace alib::boxing { namespace detail {
extern  VTable SNGLTN_vtClassConstexpr2P;
        VTable SNGLTN_vtClassConstexpr2P(typeid(ClassConstexpr2P), typeid(void), VTable::MappingType::Value, alib::boxing::SizeTraits<ClassConstexpr2P> );
}
template<> struct VTableOptimizationTraits< ClassConstexpr2P, false > {
static constexpr detail::VTable* Get() { return &detail:: SNGLTN_vtClassConstexpr2P; } };   }



#if ALIB_DEBUG_BOXING
extern int   externI;
       int   externI= 5;
extern float externF;
       float externF= 1.234f;
constexpr Box constexpr2PBox(ClassConstexpr2P(&externF, &externI));
#endif

namespace ut_aworx {

#include "ALib.Lang.CIFunctions.H"

extern  AString asTestFAppend;
        AString asTestFAppend;
extern  void testFAppend(AWorxUnitTesting& ut, const Box& box,  const String& val );
        void testFAppend(AWorxUnitTesting& ut, const Box& box,  const String& val )
{
    asTestFAppend.Reset();
    UT_TRUE( box.GetFunction<FAppend<character ALIB_COMMA lang::HeapAllocator>>( lang::Reach::Local ) )
    box.Call<FAppend<character, lang::HeapAllocator>>( asTestFAppend );
    UT_EQ( val, asTestFAppend )
}


UT_CLASS

//--------------------------------------------------------------------------------------------------
//--- FTypes Equals/IsNull/IsEmpty
//--------------------------------------------------------------------------------------------------
inline
void testEquals( AWorxUnitTesting& ut, bool exp, const Box& b1, const Box& b2 )
{
    UT_EQ( exp, ( b1.Call<FEquals>(b2) ) )
    UT_EQ( exp, ( b2.Call<FEquals>(b1) ) )
}

template<typename TBoxable>
inline void testBoxingArray(AWorxUnitTesting& ut,  TBoxable& val, const Box& box )
{
    UT_TRUE(  box.IsArrayOf<typename std::remove_extent<TBoxable>::type>() )
    UT_TRUE( ( box.Call<FEquals>(val) ) )
}

template<typename TBoxable>
inline void testBoxing(AWorxUnitTesting& ut,  const TBoxable& val, const Box& box )
{
    UT_EQ( val, box.Unbox<TBoxable>() )
    UT_TRUE( ( box.Call<FEquals>(val) ) )
}


template<typename TBoxable>
inline void testBoxing(AWorxUnitTesting& ut,  TBoxable val, const Box& box1 , const Box& box2)
{
    UT_EQ( val, box1.Unbox<TBoxable>() )
    UT_EQ( val, box2.Unbox<TBoxable>() )

    UT_TRUE( ( box1.Call<FEquals>(box2) ) )
    UT_TRUE( ( box2.Call<FEquals>(box1) ) )
}

template<>
inline void testBoxing(AWorxUnitTesting& ut,  double val, const Box& box1 , const Box& box2)
{
    UT_NEAR( val, box1.Unbox<double>(), 0.00001 )
    UT_NEAR( val, box2.Unbox<double>(), 0.00001 )

    UT_TRUE( ( box1.Call<FEquals>(box2) ) )
    UT_TRUE( ( box2.Call<FEquals>(box1) ) )
}

template<>
inline void testBoxing(AWorxUnitTesting& ut,  float val, const Box& box1 , const Box& box2)
{
    #if ALIB_FEAT_BOXING_BIJECTIVE_FLOATS
        UT_NEAR( val, box1.Unbox<float>(), 0.00001f )
        UT_NEAR( val, box2.Unbox<float>(), 0.00001f )
    #else
        UT_NEAR( val, float(box1.Unbox<double>()), 0.00001f )
        UT_NEAR( val, float(box2.Unbox<double>()), 0.00001f )
    #endif

    UT_TRUE( ( box1.Call<FEquals>(box2) ) )
    UT_TRUE( ( box2.Call<FEquals>(box1) ) )
}
#include "ALib.Lang.CIMethods.H"

UT_METHOD(Boxing_FundamentalTypes)
{
    UT_INIT()
    UT_PRINT( "### Boxing: Fundamental Types ###" )

    { bool      v=      true;   testBoxing(ut,  true, v, &v );       }

    #if !ALIB_FEAT_BOXING_BIJECTIVE_INTEGRALS
        {  int8_t   v=         1;   testBoxing(ut,    integer( 1 )   , v, &v );   }
        {  int16_t  v=         2;   testBoxing(ut,    integer( 2 )   , v, &v );   }
        {  int32_t  v=         3;   testBoxing(ut,    integer( 3 )   , v, &v );   }
        {  int64_t  v=         4;   testBoxing(ut,    int64_t( 4 )   , v, &v );   }
        {  intGap_t v=         4;   testBoxing(ut,    integer( 4 )   , v, &v );   }
        {  int8_t   v=        -1;   testBoxing(ut,    integer(-1 )   , v, &v );   }
        {  int16_t  v=        -1;   testBoxing(ut,    integer(-1 )   , v, &v );   }
        {  int32_t  v=        -1;   testBoxing(ut,    integer(-1 )   , v, &v );   }
        {  int64_t  v=        -1;   testBoxing(ut,    int64_t(-1 )   , v, &v );   }
        {  intGap_t v=        -1;   testBoxing(ut,    integer(-1 )   , v, &v );   }
        {  int8_t   v=        -5;   testBoxing(ut,    integer(-5 )   , v, &v );   }
        {  int16_t  v=        -5;   testBoxing(ut,    integer(-5 )   , v, &v );   }
        {  int32_t  v=        -5;   testBoxing(ut,    integer(-5 )   , v, &v );   }
        {  int64_t  v=        -5;   testBoxing(ut,    int64_t(-5 )   , v, &v );   }
        {  intGap_t v=        -5;   testBoxing(ut,    integer(-5 )   , v, &v );   }
        { uint8_t   v=         1;   testBoxing(ut,   uinteger( 1 )   , v, &v );   }
        { uint16_t  v=         2;   testBoxing(ut,   uinteger( 2 )   , v, &v );   }
        { uint32_t  v=         3;   testBoxing(ut,   uinteger( 3 )   , v, &v );   }
        { uint64_t  v=         4;   testBoxing(ut,   uint64_t( 4 )   , v, &v );   }
        { uintGap_t v=         4;   testBoxing(ut,   uinteger( 4 )   , v, &v );   }
    #else
        {  int8_t   v=         1;   testBoxing(ut,     int8_t( 1 )   , v, &v );   }
        {  int16_t  v=         2;   testBoxing(ut,    int16_t( 2 )   , v, &v );   }
        {  int32_t  v=         3;   testBoxing(ut,    int32_t( 3 )   , v, &v );   }
        {  int64_t  v=         4;   testBoxing(ut,    int64_t( 4 )   , v, &v );   }
        {  intGap_t v=         4;   testBoxing(ut,   intGap_t( 4 )   , v, &v );   }
        {  int8_t   v=        -1;   testBoxing(ut,     int8_t(-1 )   , v, &v );   }
        {  int16_t  v=        -1;   testBoxing(ut,    int16_t(-1 )   , v, &v );   }
        {  int32_t  v=        -1;   testBoxing(ut,    int32_t(-1 )   , v, &v );   }
        {  int64_t  v=        -1;   testBoxing(ut,    int64_t(-1 )   , v, &v );   }
        {  intGap_t v=        -1;   testBoxing(ut,   intGap_t(-1 )   , v, &v );   }
        {  int8_t   v=        -5;   testBoxing(ut,     int8_t(-5 )   , v, &v );   }
        {  int16_t  v=        -5;   testBoxing(ut,    int16_t(-5 )   , v, &v );   }
        {  int32_t  v=        -5;   testBoxing(ut,    int32_t(-5 )   , v, &v );   }
        {  int64_t  v=        -5;   testBoxing(ut,    int64_t(-5 )   , v, &v );   }
        {  intGap_t v=        -5;   testBoxing(ut,   intGap_t(-5 )   , v, &v );   }
        { uint8_t   v=         1;   testBoxing(ut,    uint8_t( 1 )   , v, &v );   }
        { uint16_t  v=         2;   testBoxing(ut,   uint16_t( 2 )   , v, &v );   }
        { uint32_t  v=         3;   testBoxing(ut,   uint32_t( 3 )   , v, &v );   }
        { uint64_t  v=         4;   testBoxing(ut,   uint64_t( 4 )   , v, &v );   }
        { uintGap_t v=         4;   testBoxing(ut,  uintGap_t( 4 )   , v, &v );   }
    #endif

    #if  !ALIB_FEAT_BOXING_BIJECTIVE_FLOATS
        { float       v=      5.1f;   testBoxing(ut,  static_cast<     double>( 5.1f ), v, &v );   }
        { double      v=      5.2 ;   testBoxing(ut,  static_cast<     double>( 5.2 ) , v, &v );   }
      #if ALIB_SIZEOF_LONGDOUBLE_REPORTED <= 2 * ALIB_SIZEOF_INTEGER
        { long double v=      5.2L ;  testBoxing(ut,  static_cast<long double>( 5.2 ) , v, &v );   }
      #endif
    #else
        { float       v=      5.1f;   testBoxing(ut,  static_cast<     float >( 5.1f ), v, &v );   }
        { double      v=      5.2 ;   testBoxing(ut,  static_cast<     double>( 5.2 ) , v, &v );   }
      #if ALIB_SIZEOF_LONGDOUBLE_REPORTED <= 2 * ALIB_SIZEOF_INTEGER
        { long double v=      5.2L ;  testBoxing(ut,  static_cast<long double>( 5.2 ) , v, &v );   }
      #endif
    #endif


    #if !ALIB_FEAT_BOXING_BIJECTIVE_CHARACTERS
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

UT_METHOD(Boxing_FTYPESDefaultFunctions)
{
    UT_INIT()

    UT_PRINT( "### Boxing: Fundamental Types Default functions (FEquals/FIsNotNull) ###" )
    testEquals( ut, true,      5,     5     );
    testEquals( ut, false,     5,     3     );
    testEquals( ut, true,      5L,    5L    );
    testEquals( ut, false,     5L,    3L    );
    testEquals( ut, true,    5.3f,    5.3f  );
    testEquals( ut, true,    5.3 ,    5.3   );
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

    UT_FALSE( Box(5)   <  3     )
    UT_FALSE( Box(5)   <  3.1   )
    UT_FALSE( Box(5.1) <  3     )
    UT_FALSE( Box(5.1) <  3.1   )
    UT_FALSE( Box(5)   <= 3     )
    UT_FALSE( Box(5)   <= 3.1   )
    UT_FALSE( Box(5.1) <= 3     )
    UT_FALSE( Box(5.1) <= 3.1   )
    UT_TRUE ( Box(5)   <= 5     )
    UT_FALSE( Box(5)   <= 4.9   )
    UT_TRUE ( Box(5)   <= 5.1   )
    UT_TRUE ( Box(4.9) <= 5     )
    UT_FALSE( Box(5.1) <= 5.0   )

    UT_TRUE ( Box(5)   > 3      )
    UT_TRUE ( Box(5)   > 3.1    )
    UT_TRUE ( Box(5.1) > 3      )
    UT_TRUE ( Box(5.1) > 3.1    )
    UT_TRUE ( Box(5)   >= 3     )
    UT_TRUE ( Box(5)   >= 3.1   )
    UT_TRUE ( Box(5.1) >= 3     )
    UT_TRUE ( Box(5.1) >= 3.1   )
    UT_TRUE ( Box(5)   >= 5     )
    UT_TRUE ( Box(5)   >= 5.0   )
    UT_TRUE ( Box(5.0) >= 4     )
    UT_FALSE( Box(5.0) >= 6     )
    UT_TRUE ( Box(5.0) >= 5.0   )
    UT_FALSE( Box(5.0) >= 5.1   )


    UT_PRINT("") UT_PRINT( "### Boxing: Testing templated FEquals/FLess for custom types big/small ###" )

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


    UT_PRINT("") UT_PRINT( "### Boxing: Testing Equals: char*/char[] ###" )
    {
        bool result;
        Box helloP( static_cast<const char*>("Hello"));
        Box helloA(                          "Hello");
        Box falseP( static_cast<const char*>("not hello"));
        Box falseA(                          "not hello");
        result= helloA.Call<FEquals>( helloA ); UT_EQ( true , result )
        result= helloA.Call<FEquals>( helloP ); UT_EQ( true , result )
        result= helloA.Call<FEquals>( falseA ); UT_EQ( false, result )
        result= helloA.Call<FEquals>( falseP ); UT_EQ( false, result )

        result= helloP.Call<FEquals>( helloP ); UT_EQ( true , result )
        result= helloP.Call<FEquals>( helloA ); UT_EQ( true , result )
        result= helloP.Call<FEquals>( falseP ); UT_EQ( false, result )
        result= helloP.Call<FEquals>( falseA ); UT_EQ( false, result )
    }

    UT_PRINT("") UT_PRINT( "### Boxing: Testing Equals: wchar_t*/wchar_[] ###" )
    {
        bool result;
        Box helloP( static_cast<const wchar_t*>( L"Hello"));
        Box helloA(                              L"Hello");
        Box falseP( static_cast<const wchar_t*>( L"not hello"));
        Box falseA(                              L"not hello");
        result= helloA.Call<FEquals>( helloA ); UT_EQ( true , result )
        result= helloA.Call<FEquals>( helloP ); UT_EQ( true , result )
        result= helloA.Call<FEquals>( falseA ); UT_EQ( false, result )
        result= helloA.Call<FEquals>( falseP ); UT_EQ( false, result )

        result= helloP.Call<FEquals>( helloP ); UT_EQ( true , result )
        result= helloP.Call<FEquals>( helloA ); UT_EQ( true , result )
        result= helloP.Call<FEquals>( falseP ); UT_EQ( false, result )
        result= helloP.Call<FEquals>( falseA ); UT_EQ( false, result )
    }
}

UT_METHOD(Boxing_Strings)
{
    UT_INIT()

    UT_PRINT("") UT_PRINT( "### Boxing: Testing Equals: wchar_t*/wchar_[]  with AString ###" )

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

        result= helloAS.Call<FEquals>( helloP ); UT_EQ( true , result )
        result= helloAS.Call<FEquals>( helloA ); UT_EQ( true , result )
        result= helloAS.Call<FEquals>( falseP ); UT_EQ( false, result )
        result= helloAS.Call<FEquals>( falseA ); UT_EQ( false, result )

        result= helloA.Call<FEquals>( helloAS ); UT_EQ( true , result )
        result= helloP.Call<FEquals>( helloAS ); UT_EQ( true , result )
        result= helloA.Call<FEquals>( falseAS ); UT_EQ( false, result )
        result= helloP.Call<FEquals>( falseAS ); UT_EQ( false, result )
    }

    AString test;
    {
        LocalString<73> pa73("73");        Box bpa73(pa73);
        test._() << bpa73;      UT_EQ( A_CHAR("73"), test )
    }

    {
        alib::LocalString<73> pa73("73");     Box bpa73(pa73);
        test._() << bpa73;      UT_EQ( A_CHAR("73"), test)
    }

    {
        character sl3[4](A_CHAR("123"));       Box bsl123(sl3);
        test._() << bsl123;     UT_EQ( A_CHAR("123"), test )
    }

}


UT_METHOD(Boxing_Std_String)
{
    UT_INIT()

    UT_PRINT("") UT_PRINT( "### Boxing: std::string ###" )
    {
        std::basic_string<character> stdstr( A_CHAR("Hello") );
        Box b( stdstr );
        UT_EQ( true, b.IsArrayOf<character>() )
        UT_EQ( 5,    b.UnboxLength()           )
        UT_EQ( 'H',  b.UnboxElement<character>(0) )
    }
}

UT_METHOD(Boxing_Std_Vector)
{
    UT_INIT()

    UT_PRINT("") UT_PRINT( "### Boxing: std::vector ###" )
    {
        std::vector<int> vec= {3,4,5};

        static_assert( boxing::IsCustomized<std::vector<int>> );


        Box b(vec);
        UT_EQ( true, b.IsArrayOf<int>() )
        UT_EQ( 3,    b.UnboxLength()    )
        UT_EQ( 3,    b.UnboxElement<int>(0) )
        UT_EQ( 4,    b.UnboxElement<int>(1) )
        UT_EQ( 5,    b.UnboxElement<int>(2) )
        int* array= &b.UnboxElement<int>(0);
        UT_EQ( 3,    array[0] )
        UT_EQ( 4,    array[1] )
        UT_EQ( 5,    array[2] )

    }
    {
        std::vector<double> vec= {3.2,4.3,5.4};
        Box b(vec);
        UT_EQ( true, b.IsArrayOf<double>() )
        UT_EQ( 3,    b.UnboxLength()       )
        UT_EQ( 3.2,  b.UnboxElement<double>(0) )
        UT_EQ( 4.3,  b.UnboxElement<double>(1) )
        UT_EQ( 5.4,  b.UnboxElement<double>(2) )
        double* array= &b.UnboxElement<double>(0);
        UT_EQ( 3.2,  array[0] )
        UT_EQ( 4.3,  array[1] )
        UT_EQ( 5.4,  array[2] )
    }
    {
        std::vector<std::basic_string<character>> vec= {A_CHAR("one"), A_CHAR("two"), A_CHAR("three")};
        Box b(vec);
        UT_EQ( true, b.IsArrayOf<std::basic_string<character>>() )
        UT_EQ( 3,    b.UnboxLength()                             )
        UT_EQ( std::basic_string<character>(A_CHAR("one")),    b.UnboxElement<std::basic_string<character>>(0) )
        UT_EQ( std::basic_string<character>(A_CHAR("two")),    b.UnboxElement<std::basic_string<character>>(1) )
        UT_EQ( std::basic_string<character>(A_CHAR("three")),  b.UnboxElement<std::basic_string<character>>(2) )
        std::basic_string<character>* array= &b.UnboxElement<std::basic_string<character>>(0);
        UT_EQ( std::basic_string<character>(A_CHAR("one")),    array[0] )
        UT_EQ( std::basic_string<character>(A_CHAR("two")),    array[1] )
        UT_EQ( std::basic_string<character>(A_CHAR("three")),  array[2] )
    }


    UT_PRINT("") UT_PRINT( "### Boxing: Testing Equals: std::vector<char>/char[] ###" )
    {
        bool result;
        std::vector<character> helloVec= {'H','e','l','l','o'};
        std::vector<character> falseVec= {'n','o','t',' ','H','e','l','l','o'};
        Box helloV( helloVec );
        Box helloA( A_CHAR("Hello"));
        Box falseP( falseVec );
        Box falseA( A_CHAR("not hello"));
        result= helloA.Call<FEquals>( helloV ); UT_EQ( true , result )
        result= helloA.Call<FEquals>( falseP ); UT_EQ( false, result )

        result= helloV.Call<FEquals>( helloV ); UT_EQ( true , result )
        result= helloV.Call<FEquals>( helloA ); UT_EQ( true , result )
        result= helloV.Call<FEquals>( falseP ); UT_EQ( false, result )
        result= helloV.Call<FEquals>( falseA ); UT_EQ( false, result )
    }
}

UT_METHOD(Boxing_Boxes)
{
    UT_INIT()

    UT_PRINT("") UT_PRINT( "### Boxing: class Boxes ###" )

    {
        Boxes boxes;
        #if !ALIB_FEAT_BOXING_BIJECTIVE_INTEGRALS
        boxes.Add( 1 );                                              UT_EQ( 1,  boxes.Size() )  UT_EQ( 1              , boxes[0].Unbox<integer>())
        boxes.Add( 2 );                                              UT_EQ( 2,  boxes.Size() )  UT_EQ( 2              , boxes[1].Unbox<integer>())
        boxes.Add( 3,4,5,6 );                                        UT_EQ( 6,  boxes.Size() )  UT_EQ( 4              , boxes[3].Unbox<integer>())
        boxes.Add( 7 );                                              UT_EQ( 7,  boxes.Size() )  UT_EQ( 7              , boxes[6].Unbox<integer>())
        boxes.clear(); boxes.Add( 5 );                               UT_EQ( 1,  boxes.Size() )  UT_EQ( 5              , boxes[0].Unbox<integer>())
        boxes.clear(); boxes.Add( 5,6 );                             UT_EQ( 2,  boxes.Size() )  UT_EQ( 6              , boxes[1].Unbox<integer>())
        #endif

        boxes.clear(); boxes.Add( 5, A_CHAR("xyz") );                UT_EQ( 2,  boxes.Size() )  UT_EQ( A_CHAR("xyz")  , boxes[1].Unbox<String >())
        String64 xyzStr(A_CHAR("xyz"));
        boxes.clear(); boxes.Add( 5, xyzStr );      UT_EQ( 2,  boxes.Size() )  UT_EQ( A_CHAR("xyz")  , boxes[1].Unbox<String >())
        boxes.clear(); boxes.Add(A_CHAR("Hello "), A_CHAR("World") );UT_EQ( 2,  boxes.Size() )  UT_EQ( A_CHAR("World"), boxes[1].Unbox<String >())

        Boxes boxes2;
        boxes2.Add(A_CHAR("List1"), A_CHAR("List2") );
        boxes.Add( boxes2 );                                         UT_EQ( 4,  boxes.Size() )
        boxes.Add( Box(boxes2) );                                    UT_EQ( 6,  boxes.Size() )
        UT_EQ( A_CHAR("Hello "), boxes[0].Unbox<String   >())
        UT_EQ( A_CHAR("World" ), boxes[1].Unbox<String   >())
        UT_EQ( A_CHAR("List1" ), boxes[2].Unbox<String   >())
        UT_EQ( A_CHAR("List2" ), boxes[3].Unbox<String   >())
        UT_EQ( A_CHAR("List1" ), boxes[4].Unbox<String   >())
        UT_EQ( A_CHAR("List2" ), boxes[5].Unbox<String   >())
    }

    // the same with local allocator
    {
        LocalAllocator2K la;
        BoxesMA boxes(la);
        #if !ALIB_FEAT_BOXING_BIJECTIVE_INTEGRALS
        boxes.Add( 1 );                                              UT_EQ( 1,  boxes.Size() )  UT_EQ( 1              , boxes[0].Unbox<integer>())
        boxes.Add( 2 );                                              UT_EQ( 2,  boxes.Size() )  UT_EQ( 2              , boxes[1].Unbox<integer>())
        boxes.Add( 3,4,5,6 );                                        UT_EQ( 6,  boxes.Size() )  UT_EQ( 4              , boxes[3].Unbox<integer>())
        boxes.Add( 7 );                                              UT_EQ( 7,  boxes.Size() )  UT_EQ( 7              , boxes[6].Unbox<integer>())
        boxes.clear(); boxes.Add( 5 );                               UT_EQ( 1,  boxes.Size() )  UT_EQ( 5              , boxes[0].Unbox<integer>())
        boxes.clear(); boxes.Add( 5,6 );                             UT_EQ( 2,  boxes.Size() )  UT_EQ( 6              , boxes[1].Unbox<integer>())
        #endif

        boxes.clear(); boxes.Add( 5, A_CHAR("xyz") );                UT_EQ( 2,  boxes.Size() )  UT_EQ( A_CHAR("xyz")  , boxes[1].Unbox<String >())
        String64 xyzStr(A_CHAR("xyz"));
        boxes.clear(); boxes.Add( 5, xyzStr);                        UT_EQ( 2,  boxes.Size() )  UT_EQ( A_CHAR("xyz")  , boxes[1].Unbox<String >())
        boxes.clear(); boxes.Add(A_CHAR("Hello "), A_CHAR("World") );UT_EQ( 2,  boxes.Size() )  UT_EQ( A_CHAR("World"), boxes[1].Unbox<String >())

        Boxes boxes2;
        boxes2.Add(A_CHAR("List1"), A_CHAR("List2") );
        boxes.Add( boxes2 );                                         UT_EQ( 4,  boxes.Size() )
        boxes.Add( Box(boxes2) );                                    UT_EQ( 6,  boxes.Size() )
        UT_EQ( A_CHAR("Hello "), boxes[0].Unbox<String   >())
        UT_EQ( A_CHAR("World" ), boxes[1].Unbox<String   >())
        UT_EQ( A_CHAR("List1" ), boxes[2].Unbox<String   >())
        UT_EQ( A_CHAR("List2" ), boxes[3].Unbox<String   >())
        UT_EQ( A_CHAR("List1" ), boxes[4].Unbox<String   >())
        UT_EQ( A_CHAR("List2" ), boxes[5].Unbox<String   >())
    }

    // import/export
    {
        Box a= A_CHAR("Hello");
        Box b( a.ExportType(), a.ExportValue() );
        UT_EQ( String(A_CHAR("Hello")), a.Unbox<String>() )
        UT_EQ( String(A_CHAR("Hello")), b.Unbox<String>() )
        Box::TypeCode type= a.ExportType();
        b.Import( type );
        UT_TRUE( b.IsType<String>() )
        UT_TRUE( b.Unbox<String>().IsNull() )
        boxing::Placeholder ph= a.ExportValue();
        b.Import( type, ph );
        UT_TRUE( b.IsType<String>() )
        UT_EQ( String(A_CHAR("Hello")), b.Unbox<String>() )
    }
}

#if ALIB_DEBUG_BOXING

UT_METHOD(Boxing_CustomClasses)
{
    UT_INIT()

    using namespace alib::boxing;

    // uncomment and watch output. Not easily testable otherwise
    #if ALIB_DEBUG_BOXING
    //UT_PRINT( boxing::debug::TypeInfo<int>()                 )
    //UT_PRINT( boxing::debug::TypeInfo<double>()              )
    //UT_PRINT( boxing::debug::TypeInfo<double*>()             )
    //UT_PRINT( boxing::debug::TypeInfo<float>()               )
    //UT_PRINT( boxing::debug::TypeInfo<float*>()              )
    //UT_PRINT( boxing::debug::TypeInfo<std::string>()         )
    //UT_PRINT( boxing::debug::TypeInfo<std::string*>()        )
    //UT_PRINT( boxing::debug::TypeInfo<std::map<int,int>>()   )
    //UT_PRINT( boxing::debug::TypeInfo<std::map<int,int>*>()  )
    //int ia[5];
    //UT_PRINT( boxing::debug::TypeInfo<int[5]>(ia)            )
    //UT_PRINT( boxing::debug::TypeInfo<std::nullptr_t>()      )
    //UT_PRINT( boxing::debug::DumpAll()                       )
    #endif

    UT_PRINT( "### Boxing: SmallClassDefaultBoxing ###" )
    {
        SmallClassDefaultBoxing mb;
        Box bMB1( mb );     UT_TRUE( bMB1.IsType<SmallClassDefaultBoxing >() )
        Box bMB2( &mb );    UT_TRUE( bMB2.IsType<SmallClassDefaultBoxing >() )

        UT_FALSE( IsCustomized<SmallClassDefaultBoxing > )
        UT_FALSE( IsCustomized<SmallClassDefaultBoxing*> )
        UT_TRUE ( IsUnboxable <SmallClassDefaultBoxing > )
        UT_FALSE( IsUnboxable <SmallClassDefaultBoxing*> )
        UT_FALSE( IsLocked    <SmallClassDefaultBoxing > )
        UT_FALSE( IsLocked    <SmallClassDefaultBoxing*> )
        UT_FALSE( IsNotBoxable<SmallClassDefaultBoxing> )
        UT_FALSE( IsNotBoxable<SmallClassDefaultBoxing*> )
    }

    UT_PRINT( "### Boxing: BigClassDefaultBoxing ###" )
    {
        BigClassDefaultBoxing mb;
        Box bMB1( mb );     UT_TRUE( bMB1.IsType<BigClassDefaultBoxing  *>() )
        Box bMB2( &mb );    UT_TRUE( bMB2.IsType<BigClassDefaultBoxing  *>() )

        UT_FALSE( IsCustomized<BigClassDefaultBoxing > )
        UT_FALSE( IsCustomized<BigClassDefaultBoxing*> )
        UT_FALSE( IsUnboxable <BigClassDefaultBoxing > )
        UT_TRUE ( IsUnboxable <BigClassDefaultBoxing*> )
        UT_FALSE( IsLocked    <BigClassDefaultBoxing > )
        UT_FALSE( IsLocked    <BigClassDefaultBoxing*> )
        UT_FALSE( IsNotBoxable<BigClassDefaultBoxing > )
        UT_FALSE( IsNotBoxable<BigClassDefaultBoxing*> )
    }

    UT_PRINT( "### Boxing: SmallNotCopyConstr ###" )
    {
        SmallNotCopyConstr mb(5,"NDFC");
        Box bMB1( mb );     UT_TRUE( bMB1.IsType<SmallNotCopyConstr*>() )
        Box bMB2( &mb );    UT_TRUE( bMB2.IsType<SmallNotCopyConstr*>() )

        UT_PRINT(  boxing::debug::TypeInfo<SmallNotCopyConstr >(bMB1) )
        UT_PRINT(  boxing::debug::TypeInfo<SmallNotCopyConstr*>()     )

        UT_EQ   ( 5,   bMB1.Unbox<SmallNotCopyConstr*>()->i   )

        UT_FALSE( IsCustomized<SmallNotCopyConstr > )
        UT_FALSE( IsCustomized<SmallNotCopyConstr*> )
        UT_FALSE( IsUnboxable <SmallNotCopyConstr > )
        UT_TRUE ( IsUnboxable <SmallNotCopyConstr*> )
        UT_FALSE( IsLocked    <SmallNotCopyConstr > )
        UT_FALSE( IsLocked    <SmallNotCopyConstr*> )
        UT_FALSE( IsNotBoxable<SmallNotCopyConstr> )
        UT_FALSE( IsNotBoxable<SmallNotCopyConstr*> )
    }


    UT_PRINT( "### Boxing: SmallNotCopyConstrCustom ###" )
    {
        constexpr Box bMBce( SmallNotCopyConstrCustom(5, nullptr) );
        (void) bMBce;

        SmallNotCopyConstrCustom mb(5,"NDFC");
        Box bMB1( mb );     UT_TRUE( bMB1.IsType<SmallNotCopyConstrCustom>() )
        Box bMB2( &mb );    UT_TRUE( bMB2.IsType<SmallNotCopyConstrCustom>() )

        UT_PRINT(  boxing::debug::TypeInfo<SmallNotCopyConstrCustom  >(bMB1) )
        UT_PRINT(  boxing::debug::TypeInfo<SmallNotCopyConstrCustom* >() )

        //UT_EQ( 5, bMB1.Unbox<SmallNotCopyConstrCustom >().i )
        //UT_EQ( 5, bMB1.Unbox<SmallNotCopyConstrCustom*>()->i  ) // has to cause static assertion

        UT_TRUE ( IsCustomized<SmallNotCopyConstrCustom > )
        UT_FALSE( IsCustomized<SmallNotCopyConstrCustom*> )
        UT_TRUE ( IsUnboxable <SmallNotCopyConstrCustom > )
        UT_FALSE( IsUnboxable <SmallNotCopyConstrCustom*> )
        UT_FALSE( IsLocked    <SmallNotCopyConstrCustom > )
        UT_FALSE( IsLocked    <SmallNotCopyConstrCustom*> )
        UT_FALSE( IsNotBoxable<SmallNotCopyConstrCustom> )
        UT_FALSE( IsNotBoxable<SmallNotCopyConstrCustom*> )
    }

    UT_PRINT( "### Boxing: ClassWithExplicitPointerBoxing ###" )
    {
        ClassWithExplicitPointerBoxing mb;
        Box bMB1( mb );     UT_TRUE( bMB1.IsType<ClassWithExplicitPointerBoxing*>() )
        Box bMB2( &mb );    UT_TRUE( bMB2.IsType<ClassWithExplicitPointerBoxing*>() )

        UT_PRINT(  boxing::debug::TypeInfo<ClassWithExplicitPointerBoxing >() )
        UT_PRINT(  boxing::debug::TypeInfo<ClassWithExplicitPointerBoxing*>() )

      //UT_EQ( 42, bMB1.Unbox<ClassWithExplicitPointerBoxing >().i  ) // has to cause static assertion
        UT_EQ( 42, bMB1.Unbox<ClassWithExplicitPointerBoxing*>()->i )

        UT_FALSE( IsCustomized<ClassWithExplicitPointerBoxing > )
        UT_TRUE ( IsCustomized<ClassWithExplicitPointerBoxing*> )
        UT_FALSE( IsUnboxable <ClassWithExplicitPointerBoxing > )
        UT_TRUE ( IsUnboxable <ClassWithExplicitPointerBoxing*> )
        UT_FALSE( IsLocked    <ClassWithExplicitPointerBoxing >)
        UT_FALSE( IsLocked    <ClassWithExplicitPointerBoxing*>)
        UT_FALSE( IsNotBoxable<ClassWithExplicitPointerBoxing> )
        UT_FALSE( IsNotBoxable<ClassWithExplicitPointerBoxing*> )
    }


    UT_PRINT( "### Boxing: ClassWithValueAndPointerMapping ###" )
    {
        ClassWithValueAndPointerMapping mb;
        Box bMB1( mb );     UT_TRUE( bMB1.IsType<ClassWithValueAndPointerMapping >() )
        Box bMB2( &mb );    UT_TRUE( bMB2.IsType<ClassWithValueAndPointerMapping*>() )

        UT_PRINT(  boxing::debug::TypeInfo<ClassWithValueAndPointerMapping >() )
        UT_PRINT(  boxing::debug::TypeInfo<ClassWithValueAndPointerMapping*>() )

        UT_EQ( 43, bMB1.Unbox<ClassWithValueAndPointerMapping >().i   )
        UT_EQ( 43, bMB2.Unbox<ClassWithValueAndPointerMapping*>()->i )

        UT_TRUE ( IsCustomized<ClassWithValueAndPointerMapping > )
        UT_TRUE ( IsCustomized<ClassWithValueAndPointerMapping*> )
        UT_TRUE ( IsUnboxable <ClassWithValueAndPointerMapping > )
        UT_TRUE ( IsUnboxable <ClassWithValueAndPointerMapping*> )
        UT_FALSE( IsLocked    <ClassWithValueAndPointerMapping > )
        UT_FALSE( IsLocked    <ClassWithValueAndPointerMapping*> )
        UT_FALSE( IsNotBoxable<ClassWithValueAndPointerMapping> )
        UT_FALSE( IsNotBoxable<ClassWithValueAndPointerMapping*> )
    }

    UT_PRINT( "### Boxing: ClassWithValueAndPointerCustom ###" )
    {
        ClassWithValueAndPointerCustom mb;
        Box bMB1( mb );     UT_TRUE( bMB1.IsType<ClassWithValueAndPointerCustom >() )
        Box bMB2( &mb );    UT_TRUE( bMB2.IsType<ClassWithValueAndPointerCustom*>() )

        UT_PRINT(  boxing::debug::TypeInfo<ClassWithValueAndPointerCustom >() )
        UT_PRINT(  boxing::debug::TypeInfo<ClassWithValueAndPointerCustom*>() )

        UT_EQ( 44, bMB1.Unbox<ClassWithValueAndPointerCustom >().i   )
        UT_EQ( 44, bMB2.Unbox<ClassWithValueAndPointerCustom*>()->i )

        UT_TRUE ( IsCustomized<ClassWithValueAndPointerCustom > )
        UT_TRUE ( IsCustomized<ClassWithValueAndPointerCustom*> )
        UT_TRUE ( IsUnboxable <ClassWithValueAndPointerCustom > )
        UT_TRUE ( IsUnboxable <ClassWithValueAndPointerCustom*> )
        UT_FALSE( IsLocked    <ClassWithValueAndPointerCustom > )
        UT_FALSE( IsLocked    <ClassWithValueAndPointerCustom*> )
        UT_FALSE( IsNotBoxable<ClassWithValueAndPointerCustom> )
        UT_FALSE( IsNotBoxable<ClassWithValueAndPointerCustom*> )
    }

    UT_PRINT( "### Boxing: ClassVNotUnboxable ###" )
    {
        ClassVNotUnboxable mb;
        Box bMB1( mb );     //UT_TRUE( bMB1.IsType<ClassVNotUnboxable >() )
        Box bMB2( &mb );    //UT_TRUE( bMB2.IsType<ClassVNotUnboxable*>() )

        UT_PRINT(  boxing::debug::TypeInfo<ClassVNotUnboxable >() )
        UT_PRINT(  boxing::debug::TypeInfo<ClassVNotUnboxable*>() )

        // UT_EQ( 44, bMB1.Unbox<ClassVNotUnboxable >().i  )
        // UT_EQ( 44, bMB2.Unbox<ClassVNotUnboxable*>()->i )

        UT_TRUE ( IsCustomized<ClassVNotUnboxable > )
        UT_FALSE( IsCustomized<ClassVNotUnboxable*> )
        UT_FALSE( IsUnboxable <ClassVNotUnboxable > )
        UT_FALSE( IsUnboxable <ClassVNotUnboxable*> )
        UT_TRUE ( IsLocked    <ClassVNotUnboxable > )
        UT_FALSE( IsLocked    <ClassVNotUnboxable*> )
        UT_FALSE( IsNotBoxable<ClassVNotUnboxable > )
        UT_FALSE( IsNotBoxable<ClassVNotUnboxable*> )
    }

    UT_PRINT( "### Boxing: ClassPNotUnboxable ###" )
    {
        ClassPNotUnboxable mb;
        Box bMB1( mb );     //UT_TRUE( bMB1.IsType<ClassPNotUnboxable >() )
        Box bMB2( &mb );    //UT_TRUE( bMB2.IsType<ClassPNotUnboxable*>() )

        UT_PRINT(  boxing::debug::TypeInfo<ClassPNotUnboxable >() )
        UT_PRINT(  boxing::debug::TypeInfo<ClassPNotUnboxable*>() )

        // UT_EQ( 44, bMB1.Unbox<ClassPNotUnboxable >().i  )
        // UT_EQ( 44, bMB2.Unbox<ClassPNotUnboxable*>()->i )

        UT_FALSE( IsCustomized<ClassPNotUnboxable > )
        UT_TRUE ( IsCustomized<ClassPNotUnboxable*> )
        UT_FALSE( IsUnboxable <ClassPNotUnboxable > )
        UT_FALSE( IsUnboxable <ClassPNotUnboxable*> )
        UT_FALSE( IsLocked    <ClassPNotUnboxable > )
        UT_TRUE ( IsLocked    <ClassPNotUnboxable*> )
        UT_FALSE( IsNotBoxable<ClassPNotUnboxable > )
        UT_FALSE( IsNotBoxable<ClassPNotUnboxable*> )
    }

    UT_PRINT( "### Boxing: ClassBothNotUnboxable ###" )
    {
        ClassBothNotUnboxable mb;
        Box bMB1( mb );     //UT_TRUE( bMB1.IsType<ClassBothNotUnboxable >() )
        Box bMB2( &mb );    //UT_TRUE( bMB2.IsType<ClassBothNotUnboxable*>() )

        UT_PRINT(  boxing::debug::TypeInfo<ClassBothNotUnboxable >() )
        UT_PRINT(  boxing::debug::TypeInfo<ClassBothNotUnboxable*>() )

        //UT_EQ( 44, bMB1.Unbox<ClassBothNotUnboxable >().   )
        //UT_EQ( 44, bMB2.Unbox<ClassBothNotUnboxable*>()->i )

        UT_TRUE ( IsCustomized<ClassBothNotUnboxable > )
        UT_TRUE ( IsCustomized<ClassBothNotUnboxable*> )
        UT_FALSE( IsUnboxable <ClassBothNotUnboxable > )
        UT_FALSE( IsUnboxable <ClassBothNotUnboxable*> )
        UT_TRUE ( IsLocked    <ClassBothNotUnboxable > )
        UT_TRUE ( IsLocked    <ClassBothNotUnboxable*> )
        UT_FALSE( IsNotBoxable<ClassBothNotUnboxable> )
        UT_FALSE( IsNotBoxable<ClassBothNotUnboxable*> )
    }

    UT_PRINT( "### Boxing: ClassVNotBoxable ###" )
    {
        //ClassVNotBoxable mb;
        //Box bMB1= mb ;    //UT_TRUE( bMB1.IsType<ClassVNotBoxable >() )
        //Box bMB2= &mb;    //UT_TRUE( bMB2.IsType<ClassVNotBoxable*>() )

        // UT_PRINT(  boxing::debug::TypeInfo<ClassVNotBoxable >() )
        // UT_PRINT(  boxing::debug::TypeInfo<ClassVNotBoxable*>() )

        //UT_EQ( 44, bMB1.Unbox<ClassVNotBoxable >().i  )
        //UT_EQ( 44, bMB2.Unbox<ClassVNotBoxable*>()->i )

        UT_TRUE ( IsCustomized<ClassVNotBoxable > )
        UT_FALSE( IsCustomized<ClassVNotBoxable*> )
        UT_FALSE( IsUnboxable <ClassVNotBoxable > )
        UT_FALSE( IsUnboxable <ClassVNotBoxable*> )
        UT_TRUE ( IsLocked    <ClassVNotBoxable > )
        UT_FALSE( IsLocked    <ClassVNotBoxable*> )
        UT_TRUE ( IsNotBoxable<ClassVNotBoxable > )
        UT_TRUE ( IsNotBoxable<ClassVNotBoxable*> )
    }

    UT_PRINT( "### Boxing: ClassPNotBoxable ###" )
    {
        //ClassPNotBoxable mb;
        //Box bMB1=  mb;    //UT_TRUE( bMB1.IsType<ClassPNotBoxable >() )
        //Box bMB2= &mb;    //UT_TRUE( bMB2.IsType<ClassPNotBoxable*>() )

        // UT_PRINT(  boxing::debug::TypeInfo<ClassPNotBoxable >() )
        // UT_PRINT(  boxing::debug::TypeInfo<ClassPNotBoxable*>() )

        //UT_EQ( 44, bMB1.Unbox<ClassPNotBoxable >().i  )
        //UT_EQ( 44, bMB2.Unbox<ClassPNotBoxable*>()->i )

        UT_FALSE( IsCustomized<ClassPNotBoxable > )
        UT_TRUE ( IsCustomized<ClassPNotBoxable*> )
        UT_FALSE( IsUnboxable <ClassPNotBoxable > )
        UT_FALSE( IsUnboxable <ClassPNotBoxable*> )
        UT_FALSE( IsLocked    <ClassPNotBoxable > )
        UT_TRUE ( IsLocked    <ClassPNotBoxable*> )
        UT_TRUE ( IsNotBoxable<ClassPNotBoxable > )
        UT_TRUE ( IsNotBoxable<ClassPNotBoxable*> )
    }

    UT_PRINT( "### Boxing: ClassBothNotBoxable ###" )
    {
        //ClassBothNotBoxable mb;
        //Box bMB1=  mb;     //UT_TRUE( bMB1.IsType<ClassBothNotBoxable >() )
        //Box bMB2= &mb;    //UT_TRUE( bMB2.IsType<ClassBothNotBoxable*>() )

        // UT_PRINT(  boxing::debug::TypeInfo<ClassBothNotBoxable >() )
        // UT_PRINT(  boxing::debug::TypeInfo<ClassBothNotBoxable*>() )

        // UT_EQ( 44, bMB1.Unbox<ClassBothNotBoxable >().i )
        //UT_EQ( 44, bMB2.Unbox<ClassBothNotBoxable*>()->i )

        UT_TRUE ( IsCustomized<ClassBothNotBoxable > )
        UT_TRUE ( IsCustomized<ClassBothNotBoxable*> )
        UT_FALSE( IsUnboxable <ClassBothNotBoxable > )
        UT_FALSE( IsUnboxable <ClassBothNotBoxable*> )
        UT_TRUE ( IsLocked    <ClassBothNotBoxable*> )
        UT_TRUE ( IsLocked    <ClassBothNotBoxable*> )
        UT_TRUE ( IsNotBoxable<ClassBothNotBoxable > )
        UT_TRUE ( IsNotBoxable<ClassBothNotBoxable*> )
    }

    UT_PRINT( "### Boxing: ClassVNotBoxablePBoxable ###" )
    {
        ClassVNotBoxablePBoxable mb;
        //Box bMB1( mb );     UT_TRUE( bMB1.IsType<ClassVNotBoxablePBoxable >() )
        Box bMB2( &mb );    UT_TRUE( bMB2.IsType<ClassVNotBoxablePBoxable*>() )

        //UT_PRINT(  boxing::debug::TypeInfo<ClassVNotBoxablePBoxable >() )
        UT_PRINT(  boxing::debug::TypeInfo<ClassVNotBoxablePBoxable*>() )

        //UT_EQ( 44, bMB1.Unbox<ClassVNotBoxablePBoxable >().i   )
        UT_EQ( 44, bMB2.Unbox<ClassVNotBoxablePBoxable*>()->i )

        UT_TRUE ( IsCustomized<ClassVNotBoxablePBoxable > )
        UT_TRUE ( IsCustomized<ClassVNotBoxablePBoxable*> )
        UT_FALSE( IsUnboxable <ClassVNotBoxablePBoxable > )
        UT_TRUE ( IsUnboxable <ClassVNotBoxablePBoxable*> )
        UT_TRUE ( IsLocked    <ClassVNotBoxablePBoxable > )
        UT_FALSE( IsLocked    <ClassVNotBoxablePBoxable*> )
        UT_TRUE ( IsNotBoxable<ClassVNotBoxablePBoxable > )
        UT_FALSE( IsNotBoxable<ClassVNotBoxablePBoxable*> )
    }

    UT_PRINT( "### Boxing: ClassPNotBoxableVBoxable ###" )
    {
        ClassPNotBoxableVBoxable mb;
        Box bMB1( mb );     UT_TRUE( bMB1.IsType<ClassPNotBoxableVBoxable >() )
        //Box bMB2( &mb );    UT_TRUE( bMB2.IsType<ClassPNotBoxableVBoxable*>() )

        UT_PRINT(  boxing::debug::TypeInfo<ClassPNotBoxableVBoxable >() )
        //UT_PRINT(  boxing::debug::TypeInfo<ClassPNotBoxableVBoxable*>() )

        UT_EQ( 44, bMB1.Unbox<ClassPNotBoxableVBoxable >().i   )
        //UT_EQ( 44, bMB2.Unbox<ClassPNotBoxableVBoxable*>()->i )

        UT_TRUE ( IsCustomized<ClassPNotBoxableVBoxable > )
        UT_TRUE ( IsCustomized<ClassPNotBoxableVBoxable*> )
        UT_TRUE ( IsUnboxable <ClassPNotBoxableVBoxable > )
        UT_FALSE( IsUnboxable <ClassPNotBoxableVBoxable*> )
        UT_FALSE( IsLocked    <ClassPNotBoxableVBoxable > )
        UT_TRUE ( IsLocked    <ClassPNotBoxableVBoxable*> )
        UT_FALSE( IsNotBoxable<ClassPNotBoxableVBoxable > )
        UT_TRUE ( IsNotBoxable<ClassPNotBoxableVBoxable*> )
    }

    UT_PRINT( "### Boxing: ClassConstexpr2P ###" )
    {
         ALIB_DBG(alib::assert::GetHaltFlagAndCounters().HaltOnErrors= false; )
         UT_PRINT( "Two errors should follow: VTable not registered." )
         UT_EQ(*constexpr2PBox.Unbox<ClassConstexpr2P>().p1, 1.234f )
         UT_EQ(*constexpr2PBox.Unbox<ClassConstexpr2P>().p2, 5 )
         ALIB_DBG(alib::assert::GetHaltFlagAndCounters().HaltOnErrors= true; )
    }

}
#endif  //ALIB_DEBUG_BOXING


UT_METHOD(Boxing_Function_FAppend)
{
    UT_INIT()
    UT_PRINT( "### Boxing: SmallClassDefaultBoxing ###" )

    testFAppend(ut, 5.2, A_CHAR("5.2") );
    testFAppend(ut, 5U,  A_CHAR("5") );
    testFAppend(ut,  5,  A_CHAR("5") );
    testFAppend(ut, A_CHAR("Hello"), A_CHAR("Hello") );
    testFAppend(ut, CString(A_CHAR("atstring")),  A_CHAR("atstring"));
    testFAppend(ut, AString(A_CHAR("anastring")),  A_CHAR("anastring"));
    testFAppend(ut, String64(A_CHAR("string64")),  A_CHAR("string64"));
}

#include "aworx_unittests_end.hpp"

} //namespace

#endif // ALIB_UT_BOXING
