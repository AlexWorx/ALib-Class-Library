// #################################################################################################
//  aworx - Unit Tests
//  Private, not published in git ( I hope! )
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"
#include "alib/compatibility/std_characters.hpp"
#include "alib/compatibility/std_boxing.hpp"
#include "alib/compatibility/std_strings_iostream.hpp"
#include <sstream>
#include <array>
#if ALIB_CPP17
#   include <any>
#endif

namespace std
{
    extern basic_stringstream<char> testOutputStreamN;
    basic_stringstream<char> testOutputStreamN;
}
#define cout testOutputStreamN
#define main dox_boxing_tut_main

int main( int, char** );

//##################################################################################################
//#### Tutorial main()
//##################################################################################################
//! [DOX_ALIB_BOXING_TUT_MAIN]
// Include boxing (this all that is almost ever needed, appart from "enum.hpp" and "dgbboxing.hpp")
#include "alib/boxing/boxing.hpp"

// Needed for ALib initialization
#include "alib/lib/alibmodules.hpp"

// Get support for writing boxes to std::cout
#include "alib/compatibility/std_strings_iostream.hpp"

using namespace std;
using namespace aworx;

int main( int, char** )
{
    // Initialize ALib
    aworx::ALIB.Init();

    // Create a box containing a string
    Box myBox= "Hello World";

    // Write the contents of the box
    cout << "My box contains: " << myBox << endl;

    return 0;
}
//! [DOX_ALIB_BOXING_TUT_MAIN]


#undef main

#include "unittests/alib_test_selection.hpp"
#if !defined(ALIB_UT_SELECT) || defined(ALIB_UT_DOCS)

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
#include "alib/compatibility/std_strings_iostream.hpp"
#include "alib/compatibility/std_boxing_functional.hpp"
#include "alib/strings/util/stringtuples.hpp"
#include "alib/boxing/dbgboxing.hpp"
#include "alib/lib/commonenumsresourced.hpp"
#include "alib/compatibility/std_boxing.hpp"


//##################################################################################################
//#### Tutorial functions
//##################################################################################################

//! [DOX_ALIB_BOXING_FISLESS_INCLUDES]
#include "alib/compatibility/std_boxing_functional.hpp"
//! [DOX_ALIB_BOXING_FISLESS_INCLUDES]



//! [DOX_ALIB_BOXING_CUSTOM_VECTOR_POINTERS_CUSTOMIZE]
namespace aworx { namespace lib { namespace boxing {

template<typename TElem>
struct T_Boxer< std::vector<TElem>* >
{
    using                       Mapping=  TMappedTo<std::vector<TElem>*>;

    static void                 Write( Placeholder& target,  std::vector<TElem>* const & value )
    {
        target.Write( value );
    }

    static std::vector<TElem>*  Read( const Placeholder& src)
    {
        return src.Read<std::vector<TElem>*>();
    }
};

}}}
//! [DOX_ALIB_BOXING_CUSTOM_VECTOR_POINTERS_CUSTOMIZE]


// ########################################################################################
// ############################   Chapters 2-4      #######################################
// ########################################################################################
namespace dox_boxing_chpt2_4 {
namespace {
#if     ALIB_FEAT_BOXING_NON_BIJECTIVE_INTEGRALS        \
     && ALIB_FEAT_BOXING_NON_BIJECTIVE_CHARACTERS       \
     && ALIB_FEAT_BOXING_NON_BIJECTIVE_FLOATS
void main2()
{
//! [DOX_ALIB_BOXING_TUT_MAIN2]
    Box myBox= "Hello World";
    cout << "My box contains a string:     " << myBox << endl;

    myBox= 42;
    cout << "My box now contains an int:   " << myBox << endl;

    myBox= 3.1415;
    cout << "My box now contains a double: " << myBox << endl;
//! [DOX_ALIB_BOXING_TUT_MAIN2]
}
#endif


//! [DOX_ALIB_BOXING_TUT_FUNCCALL]
void TakeBox( const Box& box ) // parameter has to be a const reference to allow auto-boxing
{
    cout << "Boxed argument is: " << box << endl;
}
//! [DOX_ALIB_BOXING_TUT_FUNCCALL]

//! [DOX_ALIB_BOXING_TUT_FUNCRESULT]
Box GetBox()
{
    int random= rand();

    if( random < RAND_MAX / 2  )   return random;       // auto-boxing an integer value
    else                           return "Too high!";  // auto-boxing a C++ string literal.
}
//! [DOX_ALIB_BOXING_TUT_FUNCRESULT]
}// anonymous namespace

void callcall()
{
//! [DOX_ALIB_BOXING_TUT_FUNCCALL_CALL]
TakeBox( 1 );
TakeBox( 2.0 );
TakeBox( "three" );
//! [DOX_ALIB_BOXING_TUT_FUNCCALL_CALL]
}

void callresult()
{
//! [DOX_ALIB_BOXING_TUT_FUNCCALLRESULT]
TakeBox( GetBox() );
TakeBox( GetBox() );
TakeBox( GetBox() );
TakeBox( GetBox() );
TakeBox( GetBox() );
TakeBox( GetBox() );
//! [DOX_ALIB_BOXING_TUT_FUNCCALLRESULT]
}

void isType()
{
//! [DOX_ALIB_BOXING_TUT_ISTYPE]
Box myBox= true;
cout << "Is the type boolean? " << Bool( myBox.IsType<bool  >() ) << endl;
cout << "Is the type double? "  << Bool( myBox.IsType<double>() ) << endl;

myBox    = 5.5;
cout << "Is the type boolean? " << Bool( myBox.IsType<bool  >() ) << endl;
cout << "Is the type double? "  << Bool( myBox.IsType<double>() ) << endl;
//! [DOX_ALIB_BOXING_TUT_ISTYPE]
}

void unbox()
{
//! [DOX_ALIB_BOXING_TUT_UNBOX]
double  original= 1.2345;
Box     boxed   = original;
double  unboxed = boxed.Unbox<double>();

cout << "Original: " << original << endl;
cout << " Unboxed: " << unboxed  << endl;
//! [DOX_ALIB_BOXING_TUT_UNBOX]
}

void unboxCRASHES()
{
//! [DOX_ALIB_BOXING_TUT_UNBOX2]
    double  original= 1.2345;
    Box     boxed   = original;

    // unboxing wrong type: runtime assertion, resp. undefined behaviour
    integer unboxed = boxed.Unbox<integer>();
//! [DOX_ALIB_BOXING_TUT_UNBOX2]
}

//! [DOX_ALIB_BOXING_TUT_UNBOX3]
bool ProcessBox( const Box& box )
{
    // guessing integer?
    if( box.IsType<integer>() )
    {
        cout << "Processing given integer value: " << box.Unbox<integer>() << endl;
        return true;
    }

    // guessing double?
    if( box.IsType<double>() )
    {
        cout << "Processing given double value: " << box.Unbox<double>()   << endl;
        return true;
    }

    // Unknown type
    cout << "Warning: unknown type given!"  << endl;

    // In debug compilations, we use a helper class to display the given type name in the warning.
#if ALIB_DEBUG
    cout << "  Type given: " << aworx::DbgBoxing::TypeName( box.DbgGetVTable()) << endl;
#endif

    return false;
}
//! [DOX_ALIB_BOXING_TUT_UNBOX3]

void unbox3()
{
//! [DOX_ALIB_BOXING_TUT_UNBOX3_INVOKE]
    ProcessBox( 42 );
    ProcessBox( 3.14 );
    ProcessBox( "Hello" );
//! [DOX_ALIB_BOXING_TUT_UNBOX3_INVOKE]
}

void bijective()
{
//! [DOX_ALIB_BOXING_TUT_UNBOX3_BIJECTIVE]
    int8_t  int8 =  8;     ProcessBox( int8  );
    int16_t int16= 16;     ProcessBox( int16 );
    int32_t int32= 32;     ProcessBox( int32 );
    int64_t int64= 64;     ProcessBox( int64 );

    float   f = 1.111f;    ProcessBox( f );
    double  d = 2.222;     ProcessBox( d );

//! [DOX_ALIB_BOXING_TUT_UNBOX3_BIJECTIVE]
}

#if defined(COMPILERERROR)
void surjectiveCOMPILERERROR()
{
//! [DOX_ALIB_BOXING_TUT_SURJECTIVE_COMPILE_ERROR]
    Box box = int16_t(16);
    integer i   = box.Unbox<integer>();  // OK
    integer i16 = box.Unbox<int16_t>();  // Compiler error!
//! [DOX_ALIB_BOXING_TUT_SURJECTIVE_COMPILE_ERROR]
}
void surjectiveCOMPILERERROR2()
{
//! [DOX_ALIB_BOXING_TUT_SURJECTIVE_COMPILE_ERROR2]
    Box box = int16_t(16);
    bool result= box.IsType<int16_t>();  // Compiler error!
//! [DOX_ALIB_BOXING_TUT_SURJECTIVE_COMPILE_ERROR2]
}
#endif

void surjectiveStaticCast()
{
//! [DOX_ALIB_BOXING_TUT_SURJECTIVE_STATIC_CAST]
    Box     box = int16_t(16);
    integer i16 = static_cast<int16_t>( box.Unbox<integer>() );  // OK
//! [DOX_ALIB_BOXING_TUT_SURJECTIVE_STATIC_CAST]
}

void surjectiveStringTypes()
{
#if ALIB_CPP17
//! [DOX_ALIB_BOXING_TUT_SURJECTIVE_STRINGTYPES]
    std::string  stdString  = "Hello";
    NString      alibString = "World";
    Box          box;

    // box a std::string_view
    box        =  stdString;
    assert( box.IsType<std::string_view>() );
            box.Unbox <std::string_view>();
    assert( box.IsType<aworx::NString  >() );
            box.Unbox <aworx::NString  >();

    // box an ALib string
    box        =  alibString;
    assert( box.IsType<std::string_view>() );
            box.Unbox <std::string_view>();
    assert( box.IsType<aworx::NString  >() );
            box.Unbox <aworx::NString  >();
//! [DOX_ALIB_BOXING_TUT_SURJECTIVE_STRINGTYPES]
#endif
}

} // namespace dox_boxing_chpt2_4

// ########################################################################################
// ############################      5. Arrays      #######################################
// ########################################################################################
namespace dox_boxing_sample_arr {

//! [DOX_ALIB_BOXING_SAMPLE_ARR]
bool ProcessArray( const aworx::Box& box )
{
    // not an array type?
    if ( !box.IsArray() )
    {
        cout << "Not an array" ALIB_DBG( ", but scalar type " << box.TypeID() )     << endl;
        return false;
    }

    // guess int[]
    if ( box.IsArrayOf<int>() )
    {
        cout << "int[" << box.UnboxLength() << "]= { ";

        for( aworx::integer i= 0; i < box.UnboxLength(); ++i )
            cout << box.UnboxElement<int>( i ) << " ";

        cout << "}" << endl;

        return true;
    }

    // guess double[]
    if ( box.IsArrayOf<double>() )
    {
        // using alternative, than above by unboxing a pointer the start of the array
        cout << "double[" << box.UnboxLength() << "]= { ";

        double* array= box.UnboxArray<double>();
        for( aworx::integer i= 0; i != box.UnboxLength(); ++i )
            cout << array[ i ] << " ";

        cout << "}" << endl;

        return true;
    }

    // either no array or array element type not covered
    cout << "Unknown array element type" ALIB_DBG( ": " << box.ElementTypeID() ) << endl;
    return false;
}
//! [DOX_ALIB_BOXING_SAMPLE_ARR]

void sampleFunc()
{
//! [DOX_ALIB_BOXING_SAMPLE_ARR_2]
int    intArray   [3]= { 1  , 2  , 3 };
double doubleArray[2]= { 3.3, 4.4    };
long   longArray  [3]= { 5  , 6  , 7 };

ProcessArray( intArray    );
ProcessArray( doubleArray );
ProcessArray( longArray   );
ProcessArray( 42 );
//! [DOX_ALIB_BOXING_SAMPLE_ARR_2]
}

void sampleFunc3()
{
//! [DOX_ALIB_BOXING_SAMPLE_ARR_3]
int mArray[2][3] = {{ 1,2,3 },{ 4,5,6 } };
aworx::Box box( mArray );

std::cout << "Is int[][3]: " << box.IsArrayOf<int[3]>() << std::endl;

int (&arraySlice)[3]= box.UnboxElement<int[3]>(1);

std::cout << "array[1][2]= " << arraySlice[2] << std::endl;
//! [DOX_ALIB_BOXING_SAMPLE_ARR_3]
}

void sampleFunc4()
{
//! [DOX_ALIB_BOXING_SAMPLE_ARR_4]
            int  intArray[3] = { 1  , 2  , 3 };
std::vector<int> intVector   = { 4  , 5  , 6 };

ProcessArray( intArray  );
ProcessArray( intVector );
//! [DOX_ALIB_BOXING_SAMPLE_ARR_4]
}

void sampleFuncUnboxVector()
{
//! [DOX_ALIB_BOXING_SAMPLE_ARR_UNBOX_VECTOR]
int  intArray[3] = { 1  , 2  , 3 };

Box box= intArray;

std::vector<int> intVector;
aworx::lib::boxing::compatibility::std::CopyToVector( intVector, box );
//! [DOX_ALIB_BOXING_SAMPLE_ARR_UNBOX_VECTOR]
}

}//namespace dox_boxing_sample_arr


// ########################################################################################
// ######################      6. Structs and Classes      ################################
// ########################################################################################
namespace dox_boxing_sample_classes1 {

//! [DOX_ALIB_BOXING_CLASSES_SMALL]
class SmallClass
{
    private:
        integer      value;

    public:
        SmallClass( integer v )
        : value(v)
        {}

        integer Get()    const
        {
            return value;
        }
};
//! [DOX_ALIB_BOXING_CLASSES_SMALL]

void sampleFuncSmall()
{
//! [DOX_ALIB_BOXING_CLASSES_SMALL_USING]
SmallClass smallClass( 42 );

// boxing
Box box= smallClass;

// type checking
cout << "IsType<SmallClass>: " << box.IsType<SmallClass>() << endl;

// unboxing
cout << "Value within unboxed class: " << box.Unbox<SmallClass>().Get() << endl;
//! [DOX_ALIB_BOXING_CLASSES_SMALL_USING]
}

//! [DOX_ALIB_BOXING_CLASSES_BIG]
class BigClass
{
    private:
        integer  value1;
        integer  value2;
        integer  value3;

    public:
        BigClass( integer v1, integer v2, integer v3 )
        : value1(v1)
        , value2(v2)
        , value3(v3)
        {}

        integer Get()    const
        {
            return value1 + value2 + value3;
        }
};
//! [DOX_ALIB_BOXING_CLASSES_BIG]

#if defined(COMPILERERROR)
void sampleFuncBig()
{
//! [DOX_ALIB_BOXING_CLASSES_BIG_USING]
BigClass bigClass( 1, 2, 3 );

// boxing
Box box= bigClass;

// type checking
cout << "IsType<BigClass>: " << box.IsType<BigClass>() << endl;

// unboxing
cout << "Sum of values within unboxed class: " << box.Unbox<BigClass>().Get() << endl;
//! [DOX_ALIB_BOXING_CLASSES_BIG_USING]
}
#endif

void sampleFuncBig()
{
//! [DOX_ALIB_BOXING_CLASSES_BIG_USING_FIXED]
BigClass bigClass( 1, 2, 3 );

// boxing
Box box= bigClass;

// type checking
cout << "IsType<BigClass*>: " << box.IsType<BigClass*>() << endl;

// unboxing
cout << "Sum of values within unboxed class: " << box.Unbox<BigClass*>()->Get() << endl;
//! [DOX_ALIB_BOXING_CLASSES_BIG_USING_FIXED]
}

void sampleFuncSmallAsPointer()
{
//! [DOX_ALIB_BOXING_CLASSES_SMALL_AS_POINTER]
SmallClass smallClass( 1234 );

// boxing a pointer!
Box box= &smallClass;

// type checking for non-pointer
cout << "IsType<SmallClass>: " << box.IsType<SmallClass>() << endl;

// unboxing non-pointer
cout << "Value within unboxed class: " << box.Unbox<SmallClass>().Get() << endl;
//! [DOX_ALIB_BOXING_CLASSES_SMALL_AS_POINTER]
}

void constAndMutable()
{
Box myBox;
//! [DOX_ALIB_BOXING_CLASSES_CONST_POINTERS]

auto small= myBox.Unbox<SmallClass >();
auto big  = myBox.Unbox<  BigClass*>();

static_assert( ATMP_EQ( decltype(small),       SmallClass  ), "Error" );
static_assert( ATMP_EQ( decltype(big  ), const   BigClass* ), "Error" );
//! [DOX_ALIB_BOXING_CLASSES_CONST_POINTERS]
}


}//namespace dox_boxing_sample_classes1


// ########################################################################################
// ############################      7. Customization      ################################
// ########################################################################################

namespace dox_boxing_sample_customization {

void sampleVectorPointers()
{
//! [DOX_ALIB_BOXING_CUSTOM_VECTOR_POINTERS_SAMPLE]
std::vector<int> intVector   = { 4  , 5  , 6 };
Box box;
box=   intVector;   cout << "Unboxing int array: "   <<    box.UnboxArray< int               >()  [0] << endl;
box=  &intVector;   cout << "Unboxing vector<int>*:" << ( *box.Unbox     < std::vector<int>* >() )[1] << endl;
//! [DOX_ALIB_BOXING_CUSTOM_VECTOR_POINTERS_SAMPLE]
}


} // namespace [dox_boxing_sample_customization]

//! [DOX_ALIB_BOXING_CONDITIONAL_TYPES]
struct  MyBase
{
    integer      value1;
    integer      value2;
    MyBase( integer v1, integer v2 ) : value1(v1), value2(v2) {}
};

struct  MyDerived : public MyBase
{
    integer      ExtendedData;
    MyDerived( integer v1, integer v2, integer v3 ) : MyBase(v1, v2), ExtendedData(v3) {}
};
//! [DOX_ALIB_BOXING_CONDITIONAL_TYPES]



//! [DOX_ALIB_BOXING_CONDITIONAL_CUSTOMIZING]
namespace aworx { namespace lib { namespace boxing {

template<typename TBaseOrDerived>   struct  T_Boxer< TBaseOrDerived,

// The second, otherwise defaulted type is set to a 'legal' type (here void), in the case that given
// template type TBaseOrDerived is a MyBase or derived of it.
// Otherwise the type expression below does not exist and hence the whole specialization
// becomes illegal - and thus is not used by the compiler!
typename std::enable_if<  std::is_base_of<MyBase, TBaseOrDerived>::value  >::type
>
{
    // Type mapping is fixed to 'MyBase'.
    using Mapping=  TMappedTo<MyBase>;

    // This simple sample class fits into the placeholder. Hence, we just cast down the derived
    // type and write it to the placeholder.
    // With more complex scenarios, different things could be done. For example, virtual methods
    // might be invoked to evaluate the data that is to be boxed in a type specific way.
    static inline void Write( Placeholder& target, const TBaseOrDerived& src)
    {
        target.Write( static_cast<MyBase>( src ) );
    }

    // Read returns 'MyBase'. This implies that only this type can be unboxed, all derived types
    // are not unboxable.
    // With more complex scenarios, the return type could also be 'TBaseOrDerived', which
    // would enable to unbox any derived type. Furthermore, it could be a conditionally evaluated
    // type, which would allow unboxing for some types of the set only!
    // (A sample for the latter will be seen later in this manual.)
    static MyBase     Read( const Placeholder& src)
    {
        return src.Read<MyBase>();
    }
};

}}}
//! [DOX_ALIB_BOXING_CONDITIONAL_CUSTOMIZING]



namespace dox_boxing_sample_customization {

void sampleMyDerived()
{
//! [DOX_ALIB_BOXING_CONDITIONAL_USING]
MyBase     myBase    ( 1, 2    );
MyDerived  myDerived ( 3, 4, 5 );

Box box;
box=   myBase;      cout << "Unboxing MyBase:" << box.Unbox<MyBase>().value1 << endl;
box=  &myBase;      cout << "Unboxing MyBase:" << box.Unbox<MyBase>().value1 << endl;
box=   myDerived;   cout << "Unboxing MyBase:" << box.Unbox<MyBase>().value1 << endl;
box=  &myDerived;   cout << "Unboxing MyBase:" << box.Unbox<MyBase>().value1 << endl;
//! [DOX_ALIB_BOXING_CONDITIONAL_USING]
#if defined(COMPILERERROR)
//! [DOX_ALIB_BOXING_CONDITIONAL_USING2]
box.Unbox<MyDerived>();
//! [DOX_ALIB_BOXING_CONDITIONAL_USING2]
#endif
}


}//namespace dox_boxing_sample_customization


// ########################################################################################
#if ALIB_CPP17
namespace dox_boxing_sample_customization_bypass {

void process( const aworx::Box & box );
//! [DOX_ALIB_BOXING_CUSTOM_BYPASS_WRAPPERS]
// A wrapper for float values
struct WrappedFloat
{
    float value;
};

// A wrapper for AString objects
using WrappedAString= std::reference_wrapper<aworx::AString>;
//! [DOX_ALIB_BOXING_CUSTOM_BYPASS_WRAPPERS]

void sampleFunc()
{
//! [DOX_ALIB_BOXING_CUSTOM_BYPASS]
process(                 3.1415f   );    // boxed as double
process( WrappedFloat  { 3.1415f } );    // float value wrapped, will not be converted to double

aworx::AString astring("Hello");
process(                 astring   );    // boxed as character array
process( WrappedAString( astring ) );    // AString wrapped, the whole object "survives" boxing
//! [DOX_ALIB_BOXING_CUSTOM_BYPASS]
}


//! [DOX_ALIB_BOXING_CUSTOM_BYPASS_PROCESS]
void process( const aworx::Box & box )
{
    // 'normal' boxed types
         if ( box.IsType   <double>() )      std::cout << "double value: " << box.Unbox<double          >();
    else if ( box.IsArrayOf<char  >() )      std::cout << "string value: " << box.Unbox<std::string_view>();

    // wrapped types
    else if ( box.IsType<WrappedFloat  >() ) std::cout << "float value:  " << box.Unbox<WrappedFloat    >().value;
    else if ( box.IsType<WrappedAString>() ) std::cout << "AString:      " << box.Unbox<WrappedAString  >().get();

    cout  << std::endl;
}
//! [DOX_ALIB_BOXING_CUSTOM_BYPASS_PROCESS]

}//namespace dox_boxing_sample_customization_bypass
#endif


// ########################################################################################
// ############################       8. Functions          ###############################
// ########################################################################################

namespace dox_boxing_sample_functions {

//! [DOX_ALIB_BOXING_FUNCTIONS_DESCRIPTOR]
// Descriptor of box-function ToString.
// Implementations create a string representation of boxed values.
struct FToString
{
    // The function signature.
    //
    // @param self    The box that the function was invoked on.
    // @param buffer  A string buffer used for string creation.
    using Signature = aworx::String (*) ( const Box& self, aworx::AString& buffer );
};
//! [DOX_ALIB_BOXING_FUNCTIONS_DESCRIPTOR]

//! [DOX_ALIB_BOXING_FUNCTIONS_IMPLEMENTATION]
// anonymous namespace
namespace {

    // Implementation of FToString for boxed type 'integer'
    aworx::String FToString_integer( const Box& self, aworx::AString& buffer )
    {
        return buffer.Reset() << self.Unbox< integer>();
    }

    // Implementation of FToString for boxed type 'double'
    aworx::String FToString_double ( const Box& self, aworx::AString& buffer )
    {
        return buffer.Reset() << self.Unbox< double>();
    }

    // Templated implementation of FToString for array types
    template<typename T>
    aworx::String FToString_array( const Box& self, aworx::AString& buffer )
    {
        buffer.Reset() << "{";
        for( int i= 0 ; i < self.UnboxLength() ; ++i )
            buffer << ( i!=0 ? ", " : " " )
                   << self.UnboxElement<T>( i );

        return buffer << " }";
    }
}
//! [DOX_ALIB_BOXING_FUNCTIONS_IMPLEMENTATION]


//! [DOX_ALIB_BOXING_FUNCTIONS_REGISTER]
void RegisterMyFunctions()
{
    // registering FToString for type integer
    aworx::lib::boxing::Register<FToString, aworx::lib::boxing::TMappedTo       <integer> >( FToString_integer        );

    // registering FToString for type double
    aworx::lib::boxing::Register<FToString, aworx::lib::boxing::TMappedTo       <double > >( FToString_double         );

    // registering FToString for character arrays
    aworx::lib::boxing::Register<FToString, aworx::lib::boxing::TMappedToArrayOf<char   > >( FToString_array<char   > );

    // registering FToString for integer arrays
    aworx::lib::boxing::Register<FToString, aworx::lib::boxing::TMappedToArrayOf<integer> >( FToString_array<integer> );
}
//! [DOX_ALIB_BOXING_FUNCTIONS_REGISTER]

void invoke()
{
//! [DOX_ALIB_BOXING_FUNCTIONS_INVOKE]
// A sample array
integer intArray[4] {1,2,3,4} ;

// An array of 4 sample boxes
Box boxes[4];
boxes[0]= 5;
boxes[1]= 1.111;
boxes[2]= "Hello";
boxes[3]= intArray;

// the string buffer used with the function calls.
AString buffer;

// Generic loop over all 4 boxes
for( int i= 0 ; i < 4 ; ++i )
    cout << "box["<< i <<"].ToString(): \"" <<  boxes[i].Call<FToString>( buffer )  << '\"' << endl;
//! [DOX_ALIB_BOXING_FUNCTIONS_INVOKE]
}

void invoke2()
{
//! [DOX_ALIB_BOXING_FUNCTIONS_INVOKE2]
AString buffer;

Box box= static_cast<uinteger>( 42 );

cout << "box.ToString(): \"" <<  box.Call<FToString>( buffer )  << '\"' << endl;
//! [DOX_ALIB_BOXING_FUNCTIONS_INVOKE2]
}




} //dox_boxing_sample_functions {
#if !defined(HPP_ALIB_Lib_t)
#    include "alib/lib/typedemangler.hpp"
#endif
namespace dox_boxing_sample_functions {


//! [DOX_ALIB_BOXING_FUNCTIONS_IMPLEMENTATION3]
namespace {

    aworx::String FToString_Default( const Box& self, aworx::AString& buffer )
    {
        buffer.Reset();

        #if !ALIB_DEBUG
            if( !self.IsArray() )
                buffer << "Boxed <unknown>";
            else
                buffer << "Boxed <unknown" << '['  << self.UnboxLength() << "]>";
        #else
            if( !self.IsArray() )
                buffer << "Boxed <" << aworx::lib::DbgTypeDemangler( self.TypeID()        ).Get() << '>';
            else
                buffer << "Boxed <" << aworx::lib::DbgTypeDemangler( self.ElementTypeID() ).Get()
                       << '['  << self.UnboxLength() << "]>";

            buffer << " (missing box-function FToString)";
        #endif

        return buffer;
    }
}
//! [DOX_ALIB_BOXING_FUNCTIONS_IMPLEMENTATION3]


void RegisterMyFunctions3()
{
//! [DOX_ALIB_BOXING_FUNCTIONS_REGISTER3]
    // registering FToString default implementation
    aworx::lib::boxing::RegisterDefault<FToString>( FToString_Default );
//! [DOX_ALIB_BOXING_FUNCTIONS_REGISTER3]
}

void invoke3()
{
//! [DOX_ALIB_BOXING_FUNCTIONS_INVOKE3]
AString buffer;
double doubleArray[3] { 1.1, 2.2, 3.3 };
Box box1= static_cast<uinteger>( 42 );
Box box2= doubleArray;
Box box3= "Boxing rocks!";

cout << "box1.ToString(): \"" <<  box1.Call<FToString>( buffer )  << '\"' << endl;
cout << "box2.ToString(): \"" <<  box2.Call<FToString>( buffer )  << '\"' << endl;
cout << "box3.ToString(): \"" <<  box3.Call<FToString>( buffer )  << '\"' << endl;
//! [DOX_ALIB_BOXING_FUNCTIONS_INVOKE3]
}




} //namespace dox_boxing_sample_functions {


// ########################################################################################
// #########################        12.3 Constexpr               #########################
// ########################################################################################

namespace alib_boxing_more_constexpr {

void process( const Box& box )
{
    cout << box << endl;
}

void tryDebugging()
{
//! [DOX_ALIB_BOXING_CONSTEXPR_TRYDEBUG]
          Box  box1=  "Hello world";                   // Here you can step in with the debugger
constexpr Box  Box2=  "Constructed at compile-time!";  // Here, you can't!
//! [DOX_ALIB_BOXING_CONSTEXPR_TRYDEBUG]
}


}//namespace alib_boxing_more_constexpr


// ########################################################################################
// ################    11. Class Boxes and Variadic Function Argument    ##################
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
        if( box.IsType<aworx::integer>() )
            std::cout << box.Unbox<aworx::integer>() << "  ";
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
aworx::Boxes boxes= { 7, "ALib", 3.14 };
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
        if( box.IsType<aworx::integer>() )
            std::cout << box.Unbox<aworx::integer>() << "  ";
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
void sampleFunc2()
{
//! [DOX_ALIB_BOXING_BOXES_SAMPLE_VARIADIC_SINGLE_2]
Box array[3]= { 1, 2, 3};
HeavilyOverloadedFunction( array );
//! [DOX_ALIB_BOXING_BOXES_SAMPLE_VARIADIC_SINGLE_2]
}
}//namespace dox_boxing_sample_boxes_box



// ########################################################################################
// ##########################          12.9 Debugging             #########################
// ########################################################################################

namespace alib_boxing_more_debug {

void typeName()
{
#if ALIB_DEBUG
//! [DOX_ALIB_BOXING_DEBUG_TYPENNAME]
cout << "The mapped type is: " << aworx::DbgBoxing::TypeName( "char array" ) << endl;
//! [DOX_ALIB_BOXING_DEBUG_TYPENNAME]
#endif
}

void typeInfo()
{
#if ALIB_DEBUG
//! [DOX_ALIB_BOXING_DEBUG_TYPEINFO]
cout << aworx::DbgBoxing::TypeInfo<aworx::String>();
//! [DOX_ALIB_BOXING_DEBUG_TYPEINFO]
#endif
}

void dynamicTable()
{
#if ALIB_DEBUG
//! [DOX_ALIB_BOXING_DEBUG_DYNAMICTABLE]
cout << aworx::DbgBoxing::DumpVTables(false);
//! [DOX_ALIB_BOXING_DEBUG_DYNAMICTABLE]
#endif
}

void knownFunctions()
{
#if ALIB_DEBUG
//! [DOX_ALIB_BOXING_DEBUG_KNOWNFUNCTIONS]
cout << aworx::DbgBoxing::DumpFunctions( aworx::DbgBoxing::GetKnownFunctionTypes() );
//! [DOX_ALIB_BOXING_DEBUG_KNOWNFUNCTIONS]
#endif
}

void dumpAll()
{
#if ALIB_DEBUG
//! [DOX_ALIB_BOXING_DEBUG_DUMPALL]
cout << aworx::DbgBoxing::DumpAll();
//! [DOX_ALIB_BOXING_DEBUG_DUMPALL]
#endif
}


}//namespace alib_boxing_more_debug

//######################################################################################
// Reference Documentation Samples
//######################################################################################
namespace dox_boxing_reference_manual {

void sampleFIsLess()
{
//! [DOX_ALIB_BOXING_FISLESS]
std::vector<aworx::Box> myVec= { 2, A_CHAR('b'), 3.0, "BBB", A_WCHAR('a'), -6, 1.0,
                                 "AAA", A_WCHAR('d'), 4, "CCC", A_CHAR('c'),5.0, 0  };

std::sort( myVec.begin(), myVec.end(), std::less<Box>() );

for( auto& box : myVec )
    cout << box << endl;
//! [DOX_ALIB_BOXING_FISLESS]
}

}//namespace dox_boxing_sample_compare


//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
//--- Test class
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------

#define TESTCLASSNAME       CPP_ALib_Boxing_DOX
#include "unittests/aworx_unittests.hpp"

namespace ut_aworx {


UT_CLASS()


#if    ALIB_FEAT_BOXING_NON_BIJECTIVE_INTEGRALS   \
    && ALIB_FEAT_BOXING_NON_BIJECTIVE_CHARACTERS  \
    && ALIB_FEAT_BOXING_NON_BIJECTIVE_FLOATS

UT_METHOD(Boxing_Dox)
{
    UT_INIT();
    UT_PRINT("") UT_PRINT( "### Boxing Dox Samples ###" );

    // ########################################################################################
    // ############################   Chapters 2-4      #######################################
    // ########################################################################################
    dox_boxing_tut_main(0, nullptr);
    ut.WriteResultFile( "DOX_ALIB_BOXING_TUT_MAIN.txt", testOutputStreamN.str() );
    testOutputStreamN.str("");

    dox_boxing_chpt2_4::main2();
    ut.WriteResultFile( "DOX_ALIB_BOXING_TUT_MAIN2.txt", testOutputStreamN.str() );
    testOutputStreamN.str("");

    dox_boxing_chpt2_4::callcall();
    ut.WriteResultFile( "DOX_ALIB_BOXING_TUT_FUNCALL.txt", testOutputStreamN.str() );
    testOutputStreamN.str("");

    dox_boxing_chpt2_4::callresult();
    ut.WriteResultFile( "DOX_ALIB_BOXING_TUT_FUNCRESULT.txt", testOutputStreamN.str() );
    testOutputStreamN.str("");

    dox_boxing_chpt2_4::isType();
    ut.WriteResultFile( "DOX_ALIB_BOXING_TUT_ISTYPE.txt", testOutputStreamN.str() );
    testOutputStreamN.str("");

    dox_boxing_chpt2_4::unbox();
    ut.WriteResultFile( "DOX_ALIB_BOXING_TUT_UNBOX.txt", testOutputStreamN.str() );
    testOutputStreamN.str("");

    dox_boxing_chpt2_4::unbox3();
    ut.WriteResultFile( "DOX_ALIB_BOXING_TUT_UNBOX3.txt", testOutputStreamN.str() );
    testOutputStreamN.str("");

    dox_boxing_chpt2_4::bijective();
    ut.WriteResultFile( "DOX_ALIB_BOXING_TUT_UNBOX3_BIJECTIVE.txt", testOutputStreamN.str() );
    testOutputStreamN.str("");

    dox_boxing_chpt2_4::surjectiveStaticCast();  //no output
    dox_boxing_chpt2_4::surjectiveStringTypes(); //no output


    // ########################################################################################
    // ############################      5. Arrays      #######################################
    // ########################################################################################
    dox_boxing_sample_arr::sampleFunc();
    ut.WriteResultFile( "DOX_ALIB_BOXING_SAMPLE_ARR.txt", testOutputStreamN.str() );
    testOutputStreamN.str("");

    dox_boxing_sample_arr::sampleFunc3();
    ut.WriteResultFile( "DOX_ALIB_BOXING_SAMPLE_ARR_3.txt", testOutputStreamN.str() );
    testOutputStreamN.str("");

    dox_boxing_sample_arr::sampleFunc4();
    ut.WriteResultFile( "DOX_ALIB_BOXING_SAMPLE_ARR_4.txt", testOutputStreamN.str() );
    testOutputStreamN.str("");

    dox_boxing_sample_arr::sampleFuncUnboxVector(); //no output

    // ########################################################################################
    // ######################      6. Structs and Classes      ################################
    // ########################################################################################
    dox_boxing_sample_classes1::sampleFuncSmall();
    ut.WriteResultFile( "DOX_ALIB_BOXING_CLASSES_SMALL_USING.txt", testOutputStreamN.str() );
    testOutputStreamN.str("");

    dox_boxing_sample_classes1::sampleFuncBig();
    ut.WriteResultFile( "DOX_ALIB_BOXING_CLASSES_BIG_USING.txt", testOutputStreamN.str() );
    testOutputStreamN.str("");

    dox_boxing_sample_classes1::sampleFuncSmallAsPointer();
    ut.WriteResultFile( "DOX_ALIB_BOXING_CLASSES_SMALL_AS_POINTER.txt", testOutputStreamN.str() );
    testOutputStreamN.str("");


    // ########################################################################################
    // ############################      7. Customization      ################################
    // ########################################################################################
    dox_boxing_sample_customization::sampleVectorPointers();
    ut.WriteResultFile( "DOX_ALIB_BOXING_CUSTOM_VECTOR_POINTERS.txt", testOutputStreamN.str() );
    testOutputStreamN.str("");

    dox_boxing_sample_customization::sampleMyDerived();
    ut.WriteResultFile( "DOX_ALIB_BOXING_CONDITIONAL.txt", testOutputStreamN.str() );
    testOutputStreamN.str("");

#if ALIB_CPP17
    dox_boxing_sample_customization_bypass::sampleFunc();
    ut.WriteResultFile( "DOX_ALIB_BOXING_CUSTOM_BYPASS.txt", testOutputStreamN.str() );
    testOutputStreamN.str("");
#endif


    // ########################################################################################
    // ############################       8. Function          ################################
    // ########################################################################################
    dox_boxing_sample_functions::RegisterMyFunctions();
    dox_boxing_sample_functions::invoke();
    ut.WriteResultFile( "DOX_ALIB_BOXING_FUNCTIONS_INVOKE.txt", testOutputStreamN.str() );
    testOutputStreamN.str("");

    dox_boxing_sample_functions::invoke2();
    ut.WriteResultFile( "DOX_ALIB_BOXING_FUNCTIONS_INVOKE2.txt", testOutputStreamN.str() );
    testOutputStreamN.str("");

    dox_boxing_sample_functions::RegisterMyFunctions3();
    dox_boxing_sample_functions::invoke3();
    ut.WriteResultFile( "DOX_ALIB_BOXING_FUNCTIONS_INVOKE3.txt", testOutputStreamN.str() );
    testOutputStreamN.str("");

    // ########################################################################################
    // ################    11. Class Boxes and Variadic Function Argument    ##################
    // ########################################################################################
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

    dox_boxing_sample_boxes_box::sampleFunc2();
    ut.WriteResultFile( "DOX_ALIB_BOXING_BOXES_SAMPLE_VARIADIC_SINGLE_2.txt", testOutputStreamN.str() );
    testOutputStreamN.str("");

    // ########################################################################################
    // #########################        12.? Constexpr               #########################
    // ########################################################################################
    alib_boxing_more_constexpr::tryDebugging();

    // ########################################################################################
    // ############################     12.9  Debugging        ################################
    // ########################################################################################
    alib_boxing_more_debug::typeName();
    ut.WriteResultFile( "DOX_ALIB_BOXING_DEBUG_TYPENAME.txt", testOutputStreamN.str() );
    testOutputStreamN.str("");

    alib_boxing_more_debug::typeInfo();
    ut.WriteResultFile( "DOX_ALIB_BOXING_DEBUG_TYPEINFO.txt", testOutputStreamN.str() );
    testOutputStreamN.str("");

    alib_boxing_more_debug::dynamicTable();
    ut.WriteResultFile( "DOX_ALIB_BOXING_DEBUG_DYNAMICTABLE.txt", testOutputStreamN.str() );
    testOutputStreamN.str("");

    alib_boxing_more_debug::knownFunctions();
    ut.WriteResultFile( "DOX_ALIB_BOXING_DEBUG_KNOWNFUNCTIONS.txt", testOutputStreamN.str() );
    testOutputStreamN.str("");

    alib_boxing_more_debug::dumpAll();
    ut.WriteResultFile( "DOX_ALIB_BOXING_DEBUG_DUMPALL.txt", testOutputStreamN.str() );
    testOutputStreamN.str("");

    //######################################################################################
    // Reference Documentation Samples
    //######################################################################################
    dox_boxing_reference_manual::sampleFIsLess();
    ut.WriteResultFile( "DOX_ALIB_BOXING_FISLESS.txt", testOutputStreamN.str() );
    testOutputStreamN.str("");

}
#endif // #if ALIB_FEAT_BOXING_NON_BIJECTIVE_XXX



UT_CLASS_END

}; //namespace


#endif //!defined(ALIB_UT_SELECT) || defined(ALIB_UT_DOCS)




