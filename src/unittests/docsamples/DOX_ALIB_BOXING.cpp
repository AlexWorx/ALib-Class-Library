// #################################################################################################
//  aworx - Unit Tests
//  Private, not published in git ( I hope! )
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alox.hpp"



#if defined(__clang__)
    #pragma clang diagnostic ignored "-Wmissing-prototypes"
    #pragma clang diagnostic ignored "-Wunused-variable"
    #pragma clang diagnostic ignored "-Wold-style-cast"
    #pragma clang diagnostic ignored "-Wunused-parameter"
#elif defined(__GNUC__)
    #pragma GCC diagnostic ignored "-Wunused-parameter"
    #pragma GCC diagnostic ignored "-Wunused-but-set-variable"
    #pragma GCC diagnostic ignored "-Wunused-variable"
#elif defined(_MSC_VER)
    #pragma warning( disable:4100 ) // unreferenced formal parameter
    #pragma warning( disable:4189 ) //  initialized but not referenced

#endif

#include <iostream>
#include <sstream>
#include "alib/compatibility/std_string.hpp"
#include "alib/strings/util/stringtuples.hpp"
#include "alib/strings/boxing/debug.hpp"


namespace std
{
    extern basic_stringstream<char> testOutputStreamN;
           basic_stringstream<char> testOutputStreamN;
}
#define cout testOutputStreamN

using namespace std;
using namespace aworx;


//using namespace aworx;

//##################################################################################################
namespace dox_boxing_implicit_conversion_sample {

//! [DOX_ALIB_BOXING_IMPLICIT_CONSTRUCTOR]
class IntegerBox
{
    public:
    int intValue;

    IntegerBox( int intParam ) : intValue(intParam) {}
};
//! [DOX_ALIB_BOXING_IMPLICIT_CONSTRUCTOR]

//! [DOX_ALIB_BOXING_IMPLICIT_CONSTRUCTOR_FUNCTION]
void myFunction( const IntegerBox& param )
{
    //...
}
//! [DOX_ALIB_BOXING_IMPLICIT_CONSTRUCTOR_FUNCTION]

void test()
{

//! [DOX_ALIB_BOXING_IMPLICIT_CONSTRUCTOR_INVOCATION]
IntegerBox myInteger= 5;
//! [DOX_ALIB_BOXING_IMPLICIT_CONSTRUCTOR_INVOCATION]


//! [DOX_ALIB_BOXING_IMPLICIT_FUNCTION_INVOCATION]
myFunction( 7 );
//! [DOX_ALIB_BOXING_IMPLICIT_FUNCTION_INVOCATION]
}

} // namespace   dox_boxing_implicit_conversion_sample

//##################################################################################################
namespace dox_boxing_implicit_conversion_sample2 {

//! [DOX_ALIB_BOXING_BOX_TEMPLATED]
class Box
{
    public:

    template<typename TBoxable>  Box( const TBoxable& value )
    {
        //... store the value
    }
};
//! [DOX_ALIB_BOXING_BOX_TEMPLATED]

void test()
{
//! [DOX_ALIB_BOXING_BOX_TEMPLATED_2]
Box  box1= 5;
Box  box2= "42";
Box  box3= true;

int intArray[2] = { 14, 5 };
Box  box4= intArray;
//! [DOX_ALIB_BOXING_BOX_TEMPLATED_2]
}

//! [DOX_ALIB_BOXING_BOX_TEMPLATED_3]
void myFunction( const Box& box )
{
    // do something with the box
}
//! [DOX_ALIB_BOXING_BOX_TEMPLATED_3]


void test2()
{
//! [DOX_ALIB_BOXING_BOX_TEMPLATED_4]
myFunction( 5    );
myFunction( "42" );
myFunction( true );

int intArray[4] = { 1,2,3,4 };
myFunction( intArray );
//! [DOX_ALIB_BOXING_BOX_TEMPLATED_4]
}

} // namespace  dox_boxing_implicit_conversion_sample2

// ########################################################################################
namespace dox_boxing_sample_1 {

//! [DOX_ALIB_BOXING_SAMPLE_1]
class MyClass
{
    public:
        int      value;

        MyClass( int pValue )
        : value(pValue)
        {
        }
};

void useBox( const aworx::Box& box )
{
    MyClass* myClass= box.Unbox<MyClass*>();
    std::cout << "Value: " << myClass->value << std::endl;
}
//! [DOX_ALIB_BOXING_SAMPLE_1]

void sampleFunc()
{
//! [DOX_ALIB_BOXING_SAMPLE_1_2]
MyClass mc( 42 );
useBox( mc );
//! [DOX_ALIB_BOXING_SAMPLE_1_2]
}

}//namespace dox_boxing_sample_1

// ########################################################################################
namespace dox_boxing_sample_2 {

class MyClass{ public: int value;    MyClass( int pValue )   : value(pValue)   { } };

//! [DOX_ALIB_BOXING_SAMPLE_2]
void useBox( const aworx::Box& box )
{
    if ( box.IsType<MyClass*>() )
    {
        MyClass* myClass= box.Unbox<MyClass*>();
        std::cout << "Value: " << myClass->value << std::endl;
    }
    else
    {
        std::cout << "Unknown boxed type" << std::endl;
    }
}
//! [DOX_ALIB_BOXING_SAMPLE_2]

void sampleFunc()
{
//! [DOX_ALIB_BOXING_SAMPLE_2_2]
MyClass mc( 42 );
useBox( mc );

useBox( "Ooops" );
//! [DOX_ALIB_BOXING_SAMPLE_2_2]
}

void sampleFunc2()
{
//! [DOX_ALIB_BOXING_SAMPLE_2_3]
MyClass mc( 42 );
useBox( mc );
useBox( &mc );
//! [DOX_ALIB_BOXING_SAMPLE_2_3]
}
}//namespace dox_boxing_sample_2



// ########################################################################################

//! [DOX_ALIB_BOXING_SAMPLE_BIJ]
// declare a class in our namespace
namespace dox_boxing_bijective {

    class MyBijBoxed
    {
        public:
            int      value;

            MyBijBoxed( int pValue ) : value(pValue)    {}
    };

} //namespace dox sample


// specialize template struct/functions of namespace aworx::lib::boxing
namespace aworx { namespace lib { namespace boxing {

// define boxing for types MyBijBoxed and MyBijBoxed*
ALIB_BOXING_SPECIALIZE_CB_CUB( dox_boxing_bijective::MyBijBoxed , dox_boxing_bijective::MyBijBoxed , false, true );
ALIB_BOXING_SPECIALIZE(        dox_boxing_bijective::MyBijBoxed*, dox_boxing_bijective::MyBijBoxed*, false, true );

// define T_Boxing::Boxing and T_Boxing::Unboxing for type MyBijBoxed (not needed for MyBijBoxed*)
inline  void  T_Boxing<dox_boxing_bijective::MyBijBoxed>::Boxing(Box& box, const dox_boxing_bijective::MyBijBoxed& o)
{
    box.data.Value= static_cast<boxvalue >( o.value );
    box.data.Length= 0;
}

inline  dox_boxing_bijective::MyBijBoxed T_Boxing<dox_boxing_bijective::MyBijBoxed>::Unboxing( const Box& box )
{
    return dox_boxing_bijective::MyBijBoxed( static_cast<int>( box.data.Value ) );
}

}}} // namespace [aworx::lib::boxing]

// back to our namespace
namespace dox_boxing_bijective {

// boxing test method
void useBox( const aworx::Box& box )
{
    if ( box.IsType<MyBijBoxed*>() )
    {
        std::cout << "Found a boxed pointer to MyBijBoxed";
        std::cout << " Value: "  << box.Unbox<MyBijBoxed*>()->value  << std::endl;
        return;
    }

    if ( box.IsType<MyBijBoxed>() )
    {
        std::cout << "Found a boxed reference to MyBijBoxed";
        std::cout << " Value: "  << box.Unbox<MyBijBoxed>().value  << std::endl;
        return;
    }
}

}// namespace
//! [DOX_ALIB_BOXING_SAMPLE_BIJ]


namespace dox_boxing_bijective {
void sampleFunc()
{
//! [DOX_ALIB_BOXING_SAMPLE_BIJ_2]
MyBijBoxed mb(13);
useBox( mb );
useBox( &mb );
//! [DOX_ALIB_BOXING_SAMPLE_BIJ_2]
}
}//namespace dox_boxing_bijective

// ########################################################################################
namespace dox_boxing_sample_arr {

//! [DOX_ALIB_BOXING_SAMPLE_ARR]
void useBox( const aworx::Box& box )
{
    if ( box.IsArrayOf<int>() )
    {
        std::cout << "int[" << box.Length() << "]= {";
        for( aworx::integer i= 0; i < box.Length(); ++i )
        {
            if ( i != 0 ) std::cout << ", ";
            std::cout << box.UnboxElement<int>( i );
        }
        std::cout << "}" << std::endl;
    }
}
//! [DOX_ALIB_BOXING_SAMPLE_ARR]

void sampleFunc()
{
//! [DOX_ALIB_BOXING_SAMPLE_ARR_2]
int intArray[3]= { 11, 12, 13 };
useBox( intArray );
//! [DOX_ALIB_BOXING_SAMPLE_ARR_2]
}

void sampleFunc2()
{
//! [DOX_ALIB_BOXING_SAMPLE_ARR_3]
int mArray[2][3] = {{ 1,2,3 },{ 4,5,6 } };
aworx::Box box( mArray );

std::cout << "Is int[][3]: " << box.IsArrayOf<int[3]>() << std::endl;

int (&arraySlice)[3]= box.UnboxElement<int[3]>(1);

std::cout << "array[1][2]= " << arraySlice[2] << std::endl;
//! [DOX_ALIB_BOXING_SAMPLE_ARR_3]
}

//! [DOX_ALIB_BOXING_SAMPLE_ARR_4]
void useBoxAlternativeImplementation( const aworx::Box& box )
{
    if ( box.IsArrayOf<int>() )
    {
        std::cout << "int[" << box.Length() << "]= {";
        int* intArr= &box.UnboxElement<int>( 0 );          // <- changed here...
        for( aworx::integer i= 0; i < box.Length(); ++i )
        {
            if ( i != 0 ) std::cout << ", ";
            std::cout << intArr[i];                        // <- ...and here.
        }
        std::cout << "}" << std::endl;
    }
}
//! [DOX_ALIB_BOXING_SAMPLE_ARR_4]

}//namespace dox_boxing_sample_arr

// ########################################################################################
namespace dox_boxing_sample_ftypes {

//! [DOX_ALIB_BOXING_SAMPLE_FTYPES_1]
// This method prints type and value for 11 different boxed types
void useBox( const aworx::Box& box )
{
    aworx::AString t( "  Type: " );
    aworx::AString v( "  Value: " );

         if( box.IsType<bool             >() ) { t << "bool       "; v << (box.Unbox<bool             >() ? "true" : "false" ); }
    else if( box.IsType<aworx::boxed_int >() ) { t << "boxed_int  "; v <<  box.Unbox<aworx::boxed_int >(); }
    else if( box.IsType<aworx::boxed_uint>() ) { t << "boxed_uint "; v <<  box.Unbox<aworx::boxed_uint>(); }
    else if( box.IsType<double           >() ) { t << "double     "; v <<  box.Unbox<double           >(); }
    else if( box.IsType<nchar            >() ) { t << "char       "; v <<  box.Unbox<nchar            >(); }
    else if( box.IsType<wchar            >() ) { t << "wchar_t    "; v <<  box.Unbox<wchar            >(); }
    else if( box.IsType<strangeChar      >() ) { t << "strangeChar"; v <<  box.Unbox<strangeChar      >(); }

    else if( box.IsArrayOf<char          >() ) { t << "char[]     "; v <<  box.Unbox<std::string      >(); }
    else if( box.IsArrayOf<wchar_t       >() ) { t << "wchar_t[]  "; v <<  box.Unbox<std::wstring     >(); }
#if ALIB_SIZEOF_WCHAR_T == 4
    else if( box.IsArrayOf<char16_t      >() ) { t << "char16_t[] "; v <<  box.Unbox<std::u16string   >(); }
#else
    else if( box.IsArrayOf<char32_t      >() ) { t << "char32_t[] "; v <<  box.Unbox<std::u32string   >(); }
#endif
    else
    {
        std::cout << "UNKNOWN TYPE" << std::endl;
        assert(0);
    }

    std::cout << t << "  " << v << std::endl;
}
//! [DOX_ALIB_BOXING_SAMPLE_FTYPES_1]

void sampleFunc()
{
//! [DOX_ALIB_BOXING_SAMPLE_FTYPES_1_2]
// We are boxing 33 different fundamental types here:
std::cout << "bool:" << std::endl;
{ bool      val= true;  useBox(  val ); useBox( &val ); }

std::cout << std::endl << "integer types:" << std::endl;
{         int8_t   val=  -1;   useBox(  val ); useBox( &val ); }
{         int16_t  val=  -2;   useBox(  val ); useBox( &val ); }
{         int32_t  val=  -3;   useBox(  val ); useBox( &val ); }
{         int64_t  val=  -4;   useBox(  val ); useBox( &val ); }
{  aworx::intGap_t val=  -5;   useBox(  val ); useBox( &val ); }
{        uint8_t   val=   1;   useBox(  val ); useBox( &val ); }
{        uint16_t  val=   2;   useBox(  val ); useBox( &val ); }
{        uint32_t  val=   3;   useBox(  val ); useBox( &val ); }
{        uint64_t  val=   4;   useBox(  val ); useBox( &val ); }
{ aworx::uintGap_t val=   5;   useBox(  val ); useBox( &val ); }


std::cout << std::endl << "float/double:" << std::endl;
{ float     val= 3.14f;useBox(  val );  useBox( &val ); }
{ double    val= 3.14; useBox(  val );  useBox( &val ); }


std::cout << std::endl
          << "Character types: Must not be passed as pointers!"
          << std::endl;
char      c=    'a';        useBox( c   );
wchar_t   wc=  L'\u03B2';   useBox( wc  );
#if ALIB_SIZEOF_WCHAR_T == 4
char16_t u16= u'\u03B3';    useBox( u16 );
#else
char32_t  u32= U'\u03B3';   useBox( u32 );
#endif


std::cout << std::endl
          << "...instead character pointer types get boxed to character arrays!"
          << std::endl;
const  char    *     cString=   "abc";                 useBox(    cString );
const wchar_t  *    wCString=  L"\u03B1\u03B2\u03B3";  useBox(   wCString );

#if ALIB_SIZEOF_WCHAR_T == 4
    const  char16_t*  u16CString=  u"\u03B4\u03B5\u03B6\U0001F609\U0001F607\U0001F60E";
    useBox( u16CString );
#else
    const  char32_t*  u32CString=  U"\u03B7\u03B8\u03B9\U0001F609\U0001F607\U0001F60E";
    useBox( u32CString );
#endif
//! [DOX_ALIB_BOXING_SAMPLE_FTYPES_1_2]


// sample how to change a boxes content
//! [DOX_ALIB_BOXING_BOX_SET]
aworx::Box box(5);
assert( box.IsType<boxed_int>() );

box= 3.14; // box a new value (uses implicit construction and move semantics)
assert( box.IsType<double>() );
//! [DOX_ALIB_BOXING_BOX_SET]
}

void sampleFunc2()
{
//! [DOX_ALIB_BOXING_SAMPLE_FTYPES_2]
    aworx::Box         box( (int16_t) 5 );                  // 'int16_t' gets boxed to type 'boxed_int'

    // unboxing to allowed type "aworx::boxed_int" is always OK
    aworx::boxed_int   unboxed=      box.Unbox<aworx::boxed_int>();

    // Unboxing original type is not allowed (would give compilation error):
//  int16_t     unboxed16=               box.Unbox<int16_t>();

    // unbox allowed "destination" type and then cast to type needed
    int16_t     unboxed16=    (int16_t)  box.Unbox<aworx::boxed_int>();

//! [DOX_ALIB_BOXING_SAMPLE_FTYPES_2]
}
}//namespace dox_boxing_sample_ftypes


// ########################################################################################
namespace dox_boxing_sample_boxedas {

//! [DOX_ALIB_BOXING_SAMPLE_BOXEDAS]
void useBox( const aworx::Box& box )
{
    // 'normal' boxed types
         if ( box.IsType<double>()  )                        std::cout << "double value: " << box.Unbox<double>();
    else if ( box.IsArrayOf<char>() )                        std::cout << "string value: " << box.Unbox<std::string>();

    // wrapped types
    else if ( box.IsType<aworx::BoxedAs<float>         >() ) std::cout << "float:        " << box.Unbox<aworx::BoxedAs<float>>().Get();
    else if ( box.IsType<aworx::BoxedAs<aworx::AString>>() ) std::cout << "AString:      " << box.Unbox<aworx::BoxedAs<aworx::AString>>().Get();

    cout  << std::endl;
}
//! [DOX_ALIB_BOXING_SAMPLE_BOXEDAS]

void sampleFunc()
{
//! [DOX_ALIB_BOXING_SAMPLE_BOXEDAS_2]
    useBox( 1.1f );
    useBox( aworx::BoxedAs<float>( 2.2f ) );

    aworx::AString as1("Boxed as is");
    aworx::AString as2("Boxed as AString reference");
    useBox( as1 );
    useBox( aworx::BoxedAs<aworx::AString>( as2 ) );
//! [DOX_ALIB_BOXING_SAMPLE_BOXEDAS_2]
}

}//namespace dox_boxing_sample_boxedas


// ########################################################################################
namespace dox_boxing_sample_enums {

//! [DOX_ALIB_BOXING_SAMPLE_ENUMS]
enum MyEnum
{
    EVAL1=     1,
    EVAL2=     2,
};

enum MyEnumClass
{
    ECVAL1,
    ECVAL2,
};

//! [DOX_ALIB_BOXING_SAMPLE_ENUMS]

void sampleFunc()
{
//! [DOX_ALIB_BOXING_SAMPLE_ENUMS_2]
aworx::Box myEnum=          MyEnum::EVAL1;
aworx::Box myEnumClass=     MyEnumClass::ECVAL1;

MyEnum      myEnumUB=       myEnum     .Unbox<MyEnum>();
MyEnumClass myEnumClassUB=  myEnumClass.Unbox<MyEnumClass >();
//! [DOX_ALIB_BOXING_SAMPLE_ENUMS_2]

}

}//namespace dox_boxing_sample_enums

// ########################################################################################
namespace dox_boxing_sample_if {

//! [DOX_ALIB_BOXING_SAMPLE_IF_1_1]
class IStoreInMyFormat : public aworx::Interface
{
  public:

    IStoreInMyFormat() : Interface( typeid(IStoreInMyFormat) ) {}

    virtual void Invoke( const aworx::Box& box, int fileID )   = 0;
};
//! [DOX_ALIB_BOXING_SAMPLE_IF_1_1]

//! [DOX_ALIB_BOXING_SAMPLE_IF_1_2]
class IStoreInMyFormat_TcharArr : public IStoreInMyFormat,
                                  public aworx::Singleton<IStoreInMyFormat_TcharArr>
{
  public:
    virtual void Invoke( const aworx::Box& box, int myFileID )
    {
        cout << "Writing value to file(" << myFileID << "): " << box.Unbox<std::string>()
             << std::endl;
    }
};

class IStoreInMyFormat_Tint : public IStoreInMyFormat,
                              public aworx::Singleton<IStoreInMyFormat_Tint>
{
  public:
    virtual void Invoke( const aworx::Box& box, int myFileID )
    {
        cout << "Writing value to file(" << myFileID << "): ";

        if (box.IsType<aworx::boxed_int>())  cout << box.Unbox<aworx::boxed_int>();
        else                                 cout << box.Unbox<aworx::boxed_uint>();

        cout << std::endl;
    }
};

//! [DOX_ALIB_BOXING_SAMPLE_IF_1_2]

//! [DOX_ALIB_BOXING_SAMPLE_IF_1_4]
void storeData( const aworx::Box& box )
{
    int fileID= 5;

    if ( box.HasInterface<IStoreInMyFormat>() )
        box.Invoke<IStoreInMyFormat>( fileID );

    else
        cout << "Error: Boxed type does not support interface 'IStoreInMyFormat'"
             << std::endl;
}
//! [DOX_ALIB_BOXING_SAMPLE_IF_1_4]

void sampleFunc()
{
//! [DOX_ALIB_BOXING_SAMPLE_IF_1_3]
// This is to be executed once at bootstrap, e.g. at the start of the main() function
aworx::lib::boxing::DefineInterface<aworx::boxed_int , false, IStoreInMyFormat_Tint    >();
aworx::lib::boxing::DefineInterface<aworx::boxed_uint, false, IStoreInMyFormat_Tint    >();
aworx::lib::boxing::DefineInterface<char             , true , IStoreInMyFormat_TcharArr>();
//! [DOX_ALIB_BOXING_SAMPLE_IF_1_3]

//! [DOX_ALIB_BOXING_SAMPLE_IF_1_5]
    storeData( 42 );
    storeData( "Hello Boxing" );
    storeData( 3.14 );
//! [DOX_ALIB_BOXING_SAMPLE_IF_1_5]

    aworx::Box myBox(5);
    {
//! [DOX_ALIB_BOXING_SAMPLE_IF_2_1]
    aworx::AString myAString;
    myBox.Invoke<aworx::IApply<character>>( myAString );
//! [DOX_ALIB_BOXING_SAMPLE_IF_2_1]
    }
    {
//! [DOX_ALIB_BOXING_SAMPLE_IF_2_2]
    aworx::AString myAString;
    if( myBox.HasInterface<aworx::IApply<character>>() )
        myBox.Invoke<aworx::IApply<character>>( myAString );
//! [DOX_ALIB_BOXING_SAMPLE_IF_2_2]
    }
    {
//! [DOX_ALIB_BOXING_SAMPLE_IF_2_3]
    aworx::AString myAString;
    if( myBox.HasInterface<aworx::IApply<character>>() )
        myBox.Invoke<aworx::IApply<character>>( myAString );
    else
    {
        myAString << "We do not like the default behavior. We do something else...";
        //...
    }
//! [DOX_ALIB_BOXING_SAMPLE_IF_2_3]
    }

//! [DOX_ALIB_BOXING_SAMPLE_IF_3_0]
    if( myBox.IsArrayOf<char>() )
    {
        // process string...
    }
//! [DOX_ALIB_BOXING_SAMPLE_IF_3_0]

//! [DOX_ALIB_BOXING_SAMPLE_IF_3_0_2]
    if( myBox.IsArrayOf<char>() )
    {
        aworx::String aworxString= myBox.Unbox<aworx::String>();
        std::string     stdString= myBox.Unbox<std::string  >();
    }
//! [DOX_ALIB_BOXING_SAMPLE_IF_3_0_2]

}
}//namespace dox_boxing_sample_if

// ########################################################################################
namespace dox_boxing_sample_strings {

void sampleFunc()
{
//! [DOX_ALIB_BOXING_SAMPLE_STRINGS]
    aworx::Box box1( "The answer is: " );
    aworx::Box box2( 42 );

    aworx::AString aString;
    aString << box1 << box2;

    std::cout << aString << std::endl;
//! [DOX_ALIB_BOXING_SAMPLE_STRINGS]
}

}//namespace dox_boxing_sample_strings

// ########################################################################################
namespace dox_boxing_sample_recipe {

void sampleFunc()
{
//! [DOX_ALIB_BOXING_SAMPLE_RECIPE]
    std::string stdString( "TEST_STRING" );
    aworx::Box box1(  stdString );
    aworx::Box box2( &stdString );

    // test types
    std::cout << "box1.IsArrayOf<char>: " << box1.IsArrayOf<char>()  << std::endl;
    std::cout << "box2.IsArrayOf<char>: " << box2.IsArrayOf<char>()  << std::endl;

    // compare
    std::cout << "box1.equals(box2):          "   << box1.Invoke<aworx::IEquals, bool>( box2 )          << std::endl;
    std::cout << "box2.equals(\"TEST_STRING\"): " << box2.Invoke<aworx::IEquals, bool>( "TEST_STRING" ) << std::endl;

    // get a standard string back:
    std::string std_back1= box1.Unbox<std::string>();
    std::string std_back2= box2.Unbox<std::string>();
    std::cout << "Unboxed 1 as std::string:   " << std_back1 << std::endl;
    std::cout << "Unboxed 2 as std::string:   " << std_back2 << std::endl;

    // get as aworx::NString
    aworx::NString awx_back1= box1.Unbox<aworx::NString>();
    aworx::NString awx_back2= box2.Unbox<aworx::NString>();
    std::cout << "Unboxed 1 as aworx::String: " << awx_back1 << std::endl;
    std::cout << "Unboxed 2 as aworx::String: " << awx_back2 << std::endl;

    // invoke IApply
    aworx::AString aString("IApply:         ");
    box1.Invoke<aworx::IApply<character>>( aString );
    std::cout << aString << std::endl;

    // indirectly invoke IApply by applying the Box to the AString
    aString.Clear()  << "AString << Box: " << box2;
    std::cout << aString << std::endl;
//! [DOX_ALIB_BOXING_SAMPLE_RECIPE]
}
}//namespace dox_boxing_sample_recipe


// ########################################################################################
namespace dox_boxing_sample_variadic {

//! [DOX_ALIB_BOXING_VARIADIC_SAMPLE]
template <typename... T> void VariadicFunction( const T&... args )
{
    // fetch the arguments into an array of boxes
    aworx::Box boxes[]= { args... };

    // do something
    for( size_t i= 0; i < sizeof...(T) ; ++i )
    {
        aworx::Box& box= boxes[i];
        //...
    }
};
//! [DOX_ALIB_BOXING_VARIADIC_SAMPLE]

//! [DOX_ALIB_BOXING_VARIADIC_RECIPE]
template <typename... T> void VariadicRecipe( T&&... args )
{
    // fetch the arguments into an array of boxes
    aworx::Box boxes[]= { std::forward<T>( args )... };

    // ...
};
//! [DOX_ALIB_BOXING_VARIADIC_RECIPE]


void sampleFunc()
{
//! [DOX_ALIB_BOXING_VARIADIC_SAMPLE_1]
VariadicFunction( 7, "ALib", 3.14 );
//! [DOX_ALIB_BOXING_VARIADIC_SAMPLE_1]

VariadicRecipe( 7, "ALib", 3.14 );
}


}//namespace dox_boxing_sample_variadic

// ########################################################################################
namespace dox_boxing_sample_boxes {

//! [DOX_ALIB_BOXING_BOXES_SAMPLE_VARIADIC]
template <typename... T> void VariadicFunction( T&&... args )
{
    // fetch the arguments into a Boxes object
    aworx::Boxes boxes( std::forward<T>( args )... );

    // do something
    for( aworx::Box& box : boxes )
    {
        if( box.IsType<aworx::boxed_int>() )
            std::cout << box.Unbox<aworx::boxed_int>() << "  ";
        else
            std::cout << " Unknown Argument Type ";
    }
    std::cout << std::endl;
};
//! [DOX_ALIB_BOXING_BOXES_SAMPLE_VARIADIC]


void sampleFunc()
{
    {
//! [DOX_ALIB_BOXING_BOXES_SAMPLE_1]
aworx::Boxes boxes( 7, "ALib", 3.14 );
boxes.Add( 42, "Yipee-yeah" );
//! [DOX_ALIB_BOXING_BOXES_SAMPLE_1]

//! [DOX_ALIB_BOXING_BOXES_SAMPLE_2]
boxes.clear();
boxes.Add( "New", "Values" );
//! [DOX_ALIB_BOXING_BOXES_SAMPLE_2]
    }
}

void sampleFunc2()
{
    {
//! [DOX_ALIB_BOXING_BOXES_SAMPLE_VARIADIC_CALL_1]
aworx::Boxes boxes( 2, 3 );
VariadicFunction( 1, boxes, 4 );
//! [DOX_ALIB_BOXING_BOXES_SAMPLE_VARIADIC_CALL_1]
    }
}

void sampleFunc3()
{
    {
//! [DOX_ALIB_BOXING_BOXES_SAMPLE_VARIADIC_CALL_2]
aworx::Boxes boxes( 2, 3 );
aworx::Box   box( boxes );
VariadicFunction( 1, box, 4 );
//! [DOX_ALIB_BOXING_BOXES_SAMPLE_VARIADIC_CALL_2]
    }
}
}//namespace dox_boxing_sample_boxes

// ########################################################################################
namespace dox_boxing_sample_boxes_box {

//! [DOX_ALIB_BOXING_BOXES_SAMPLE_VARIADIC_SINGLE]
void HeavilyOverloadedFunction( const aworx::Box& boxOrBoxes )
{
    // pass the single box into a Boxes object. This way, if another boxes object gets passed,
    // its elements are added to the list!
    aworx::Boxes boxes( std::forward<const aworx::Box>( boxOrBoxes ) );

    // do something
    for( aworx::Box& box : boxes )
    {
        if( box.IsType<aworx::boxed_int>() )
            std::cout << box.Unbox<aworx::boxed_int>() << "  ";
        else
            std::cout << " Unknown Argument Type ";
    }
    std::cout << std::endl;
};
//! [DOX_ALIB_BOXING_BOXES_SAMPLE_VARIADIC_SINGLE]


void sampleFunc()
{
//! [DOX_ALIB_BOXING_BOXES_SAMPLE_VARIADIC_SINGLE_1]
HeavilyOverloadedFunction( 1 );
HeavilyOverloadedFunction( aworx::Boxes(1, 2, 3) );
//! [DOX_ALIB_BOXING_BOXES_SAMPLE_VARIADIC_SINGLE_1]
}
}//namespace dox_boxing_sample_boxes_box


// ########################################################################################
namespace dox_boxing_sample_debug {

void sampleFunc()
{
//! [DOX_ALIB_BOXING_DEBUG]
aworx::Box box( "Character Array" );

#if ALIB_DEBUG
    cout << aworx::lib::debug::Boxing::GetType( box  ) << endl;
#endif
//! [DOX_ALIB_BOXING_DEBUG]
}
void sampleFunc2()
{
//! [DOX_ALIB_BOXING_DEBUG_2]
#if ALIB_DEBUG
    aworx::lib::debug::Boxing dbgBoxing;
    auto& strings= dbgBoxing.GetInterfaces( "abc" );
    for( auto& it : strings )
        cout << "  " << it << endl;
#endif
//! [DOX_ALIB_BOXING_DEBUG_2]
}
void sampleFunc3()
{
//! [DOX_ALIB_BOXING_DEBUG_3]
#if ALIB_DEBUG
    std::cout << aworx::lib::debug::Boxing().Dump() << std::endl;
#endif
//! [DOX_ALIB_BOXING_DEBUG_3]
}

void sampleFunc4()
{
//! [DOX_ALIB_BOXING_DEBUG_4]
#if ALIB_DEBUG
    std::cout << aworx::lib::debug::Boxing().GetBoxingInfo<float            >() << std::endl;
    std::cout << aworx::lib::debug::Boxing().GetBoxingInfo<char*            >() << std::endl;
    std::cout << aworx::lib::debug::Boxing().GetBoxingInfo<aworx::String    >() << std::endl;
    std::cout << aworx::lib::debug::Boxing().GetBoxingInfo<std::vector<int> >() << std::endl;
#endif
//! [DOX_ALIB_BOXING_DEBUG_4]
}


}//namespace dox_boxing_sample_debug


// ########################################################################################

namespace dox_boxing_sample_compare {

void sampleFunc()
{
//! [DOX_ALIB_BOXING_COMPARE]
std::vector<aworx::Box> myVec= { 2, 3.0, "BBB", 6, 1.0, "AAA", 4, "CCC", 5.0, 0 };

std::sort( myVec.begin(), myVec.end() );

for( auto& box : myVec )
    cout << box << endl;
//! [DOX_ALIB_BOXING_COMPARE]
}


}//namespace dox_boxing_sample_compare





