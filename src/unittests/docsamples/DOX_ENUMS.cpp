// #################################################################################################
//  AWorx ALib Unit Tests
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"
#include "unittests/alib_test_selection.hpp"
#if ALIB_UT_DOCS && ALIB_UT_ENUMS

#include "alib/alox.hpp"
#include <iostream>
#include <sstream>
#include <assert.h>
#include "alib/compatibility/std_strings.hpp"
DOX_MARKER( [DOX_ENUMS_RECORDS_HEADER_COMPAT_IOSTREAM])
#include "alib/compatibility/std_strings_iostream.hpp"
DOX_MARKER( [DOX_ENUMS_RECORDS_HEADER_COMPAT_IOSTREAM])
#include "alib/enums/iterable.hpp"

DOX_MARKER( [DOX_ENUMS_BITSET_HEADER])
#include "alib/enums/iterablebitset.hpp"
DOX_MARKER( [DOX_ENUMS_BITSET_HEADER])

DOX_MARKER( [DOX_ENUMS_RECORDS_HEADER_MAIN])
#include "alib/enums/records.hpp"
DOX_MARKER( [DOX_ENUMS_RECORDS_HEADER_MAIN])

DOX_MARKER( [DOX_ENUMS_RECORDS_HEADER_INIT])
#include "alib/enums/recordbootstrap.hpp"
DOX_MARKER( [DOX_ENUMS_RECORDS_HEADER_INIT])

DOX_MARKER( [DOX_ENUMS_RECORDS_HEADER_SERIALIZATION])
#include "alib/enums/serialization.hpp"
DOX_MARKER( [DOX_ENUMS_RECORDS_HEADER_SERIALIZATION])
#include "alib/lang/resources/resources.hpp"
#   include "alib/lang/commonenums.hpp"

#define TESTCLASSNAME       UT_Dox_Enums
#include "unittests/aworx_unittests.hpp"

#if defined(__clang__)
    #pragma clang diagnostic ignored "-Wmissing-prototypes"
    #pragma clang diagnostic ignored "-Wunused-variable"
    #pragma clang diagnostic ignored "-Wold-style-cast"
    #pragma clang diagnostic ignored "-Wunused-parameter"
#elif defined(__GNUC__)
    #pragma GCC diagnostic ignored "-Wunused-parameter"
    #pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#elif defined(_MSC_VER)
    #pragma warning( disable:4100 ) // unreferenced formal parameter
#endif

//--------------------------------------------------------------------------------------------------
//--- DOCUMENTATION SAMPLES
//--------------------------------------------------------------------------------------------------

namespace std
{
    extern std::stringstream testOutputStream; // declaration (needed when clang warnings are on)
           std::stringstream testOutputStream;
}
#define cout testOutputStream
using namespace std;
using namespace alib;

extern bool compiledButNotInvoked;
       bool compiledButNotInvoked= true;

// ###################################    intGap_t    #############################################
namespace dox_lang_complete_specialization {

DOX_MARKER( [DOX_INTXX_DECLARATION])
// Template function
template <typename TInteger> const char* MyFunc(TInteger val) { return "NOT IMPLEMENTED"; }

// Specializations for integer types
template<> const char* MyFunc(  int8_t  ) { return "Type=  int8_t "; }
template<> const char* MyFunc( uint8_t  ) { return "Type= uint8_t "; }
template<> const char* MyFunc(  int16_t ) { return "Type=  int16_t"; }
template<> const char* MyFunc( uint16_t ) { return "Type= uint16_t"; }
template<> const char* MyFunc(  int32_t ) { return "Type=  int32_t"; }
template<> const char* MyFunc( uint32_t ) { return "Type= uint32_t"; }
template<> const char* MyFunc(  int64_t ) { return "Type=  int64_t"; }
template<> const char* MyFunc( uint64_t ) { return "Type= uint64_t"; }

// TestMethod
void test()
{
    // test std int types
    cout << " int8_t           : " << MyFunc( ( int8_t )           0 ) << endl; // OK
    cout << "uint8_t           : " << MyFunc( (uint8_t )           0 ) << endl; // OK
    cout << " int16_t          : " << MyFunc( ( int16_t)           0 ) << endl; // OK
    cout << "uint16_t          : " << MyFunc( (uint16_t)           0 ) << endl; // OK
    cout << " int32_t          : " << MyFunc( ( int32_t)           0 ) << endl; // OK
    cout << "uint32_t          : " << MyFunc( (uint32_t)           0 ) << endl; // OK
    cout << " int64_t          : " << MyFunc( ( int64_t)           0 ) << endl; // OK
    cout << "uint64_t          : " << MyFunc( (uint64_t)           0 ) << endl; // OK
    cout << endl;

    // test 'language' types
    cout << "         long     : " << MyFunc( (         long     ) 0 ) << endl; // Gap on common 32-bit platforms and 64-bit MSVC
    cout << "unsigned long     : " << MyFunc( (unsigned long     ) 0 ) << endl; // Gap on common 32-bit platforms and 64-bit MSVC
    cout << endl;
    cout << "         long long: " << MyFunc( (         long long) 0 ) << endl; // Gap on 64-Bit GNU C++
    cout << "unsigned long long: " << MyFunc( (unsigned long long) 0 ) << endl; // Gap on 64-Bit GNU C++
    cout << endl;

    // further info
    cout << endl;
    cout << "sizeof(         long     )= " << sizeof(         long     ) << endl;
    cout << "sizeof(unsigned long     )= " << sizeof(unsigned long     ) << endl;
    cout << endl;
    cout << "sizeof(         long long)= " << sizeof(         long long) << endl;
    cout << "sizeof(unsigned long long)= " << sizeof(unsigned long long) << endl;
}

DOX_MARKER( [DOX_INTXX_DECLARATION])

}//namespace dox_lang_complete_specialization

namespace dox_lang_complete_specialization2 {

template <typename TInteger> const char* MyFunc(TInteger val) { return "NOT IMPLEMENTED"; }

DOX_MARKER( [DOX_INTXX_DECLARATION2])
template<> const char* MyFunc( alib:: intGap_t ) { return "Type=  intGap_t"; }
template<> const char* MyFunc( alib::uintGap_t ) { return "Type= uintGap_t"; }
DOX_MARKER( [DOX_INTXX_DECLARATION2])
}//namespace dox_lang_complete_specialization2


// ###################################    Singleton    #############################################
namespace dox_lang_singleton
{
    extern void test();
}



// ###################################    enums    #############################################
namespace WindowManager {}

using namespace WindowManager;



DOX_MARKER( [DOX_ENUMS_NORMAL])
enum class Fruits
{
    Apple,
    Orange,
    Banana,
};
DOX_MARKER( [DOX_ENUMS_NORMAL])


// #################################################################################################
// ### Enum Arithmetic
// #################################################################################################
DOX_MARKER( [DOX_ENUMS_ARITHMETIC_INTRO])
enum class MyEnum
{
    One     = 1,
    Two     = 2,

    Five    = 5,
};
DOX_MARKER( [DOX_ENUMS_ARITHMETIC_INTRO])

DOX_MARKER( [DOX_ENUMS_ARITHMETIC])
ALIB_ENUMS_MAKE_ARITHMETICAL( MyEnum )
DOX_MARKER( [DOX_ENUMS_ARITHMETIC])


enum class ArithmeticEnum
{
    element0,
    element1,
    element2,
    element3,
    element4,
    element5,
};
ALIB_ENUMS_ASSIGN_RECORD( ArithmeticEnum, alib::enums::ERSerializable )

ALIB_ENUMS_MAKE_ARITHMETICAL(ArithmeticEnum)



// #################################################################################################
// ### Enum Bitwise
// #################################################################################################
DOX_MARKER( [DOX_ENUMS_BITWISE_DECLARATION])
namespace WindowManager
{
    enum class States
    {
        HorizontallyMaximized  = (1 << 0),
        VerticallyMaximized    = (1 << 1),
        Hidden                 = (1 << 2),
    };
}

ALIB_ENUMS_MAKE_BITWISE( WindowManager::States )
DOX_MARKER( [DOX_ENUMS_BITWISE_DECLARATION])

void bitwiseUseNotInvoked()
{
auto currentState= States::Hidden;

#if defined(_MSC_VER)
#   pragma warning( push )
#   pragma warning( disable:4189 ) // local variable is initialized but not referenced
#elif defined(__GNUC__)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wunused-variable"
#endif
DOX_MARKER( [DOX_ENUMS_BITWISE_2])
auto newState=   ( currentState +  (   States::HorizontallyMaximized
                                     + States::VerticallyMaximized   ) )
               & ~States::Hidden;
DOX_MARKER( [DOX_ENUMS_BITWISE_2])
#if defined(_MSC_VER)
#   pragma warning( pop )
#elif defined(__GNUC__)
#   pragma GCC diagnostic pop
#endif
}


enum class Bits
{
    NONE            =  0,
    One             =  1 << 0,
    Two             =  1 << 1,
    Three           =  1 << 2,
    Four            =  1 << 3,
    END_OF_ENUM     =  1 << 4,
};

ALIB_ENUMS_ASSIGN_RECORD( Bits, alib::enums::ERSerializable )
ALIB_ENUMS_MAKE_BITWISE(    Bits )
ALIB_ENUMS_MAKE_ITERABLE( Bits, Bits::END_OF_ENUM )

enum class BitsParsable
{
    NONE            =  0,
    One             =  1 << 0,
    Two             =  1 << 1,
    Three           =  1 << 2,
    Four            =  1 << 3,
    END_OF_ENUM     =  1 << 4,
};


ALIB_ENUMS_ASSIGN_RECORD( BitsParsable, alib::enums::ERSerializable )
ALIB_ENUMS_MAKE_BITWISE( BitsParsable )


// #################################################################################################
// ### Iterable Enum
// #################################################################################################
DOX_MARKER( [DOX_ENUMS_ITER_SAMPLE])
enum class Pets
{
    Cat,
    Dog,
    Bird,
    Snake,
};
DOX_MARKER( [DOX_ENUMS_ITER_SAMPLE])

DOX_MARKER( [DOX_ENUMS_ITER_MAKE_ITERABLE])
ALIB_ENUMS_MAKE_ITERABLE(Pets, Pets::Snake + 1 )
DOX_MARKER( [DOX_ENUMS_ITER_MAKE_ITERABLE])


DOX_MARKER( [DOX_ENUMS_ER_STATES])
ALIB_ENUMS_ASSIGN_RECORD( WindowManager::States, alib::enums::ERSerializable )
DOX_MARKER( [DOX_ENUMS_ER_STATES])

namespace iter_bitset {

void sample()
{
DOX_MARKER( [DOX_ENUMS_BITSET_DEF])
EnumBitSet<Pets> pets;
DOX_MARKER( [DOX_ENUMS_BITSET_DEF])

DOX_MARKER( [DOX_ENUMS_BITSET_FILL])
pets.Set( Pets::Cat, Pets::Dog, Pets::Bird );
DOX_MARKER( [DOX_ENUMS_BITSET_FILL])

DOX_MARKER( [DOX_ENUMS_BITSET_USE1])
cout << "Allowed pets: " << endl;
for(auto& it : pets )
    cout << "  " << static_cast<int>( it.Bit() ) << endl;
DOX_MARKER( [DOX_ENUMS_BITSET_USE1])

DOX_MARKER( [DOX_ENUMS_BITSET_USE2])
pets.Flip();

cout << endl << "Forbidden pets: " << endl;
for(auto& it : pets )
    cout << "  " << static_cast<int>( it.Bit() ) << endl;
DOX_MARKER( [DOX_ENUMS_BITSET_USE2])
}
} //namespace iter {

// #################################################################################################
// ### Enum Records
// #################################################################################################
namespace simple {
DOX_MARKER( [DOX_ENUMS_RECORDS_FRUITS])
enum class Fruits
{
    Apple,
    Orange,
    Banana,
};
DOX_MARKER( [DOX_ENUMS_RECORDS_FRUITS])

DOX_MARKER( [DOX_ENUMS_RECORDS_FRUITS_ER])
struct ERFruits
{
    String Name;

    ERFruits( const String& name )
    : Name(name)
    {}
};
DOX_MARKER( [DOX_ENUMS_RECORDS_FRUITS_ER])
}


// -To remove clash of namespaces, quickly re-def module's macro to use namespace "simple" ;-}
#undef ALIB_ENUMS_ASSIGN_RECORD
#define  ALIB_ENUMS_ASSIGN_RECORD( TEnum, TRecord )                            \
namespace alib::enums {                                                        \
template<> struct T_EnumRecords<simple::TEnum>  :  public std::true_type       \
{                                                                              \
    using  Type=  simple::TRecord;                                             \
};}

DOX_MARKER( [DOX_ENUMS_RECORDS_FRUITS_ASSIGN])
ALIB_ENUMS_ASSIGN_RECORD( Fruits, ERFruits )
DOX_MARKER( [DOX_ENUMS_RECORDS_FRUITS_ASSIGN])

#undef ALIB_ENUMS_ASSIGN_RECORD
#define  ALIB_ENUMS_ASSIGN_RECORD( TEnum, TRecord )                            \
namespace alib::enums {                                                        \
template<> struct T_EnumRecords<TEnum>  :  public std::true_type               \
{                                                                              \
    using  Type=  TRecord;                                                     \
};}


// ### Chapter 4.2.1  ##############################
namespace simple {
namespace version1 {
DOX_MARKER( [DOX_ENUMS_RECORDS_FRUITS_GETRECORD])
void printFruit(Fruits fruit)
{
    cout << alib::enums::GetRecord(fruit).Name << endl;
}
DOX_MARKER( [DOX_ENUMS_RECORDS_FRUITS_GETRECORD])
void invoke()
{
DOX_MARKER( [DOX_ENUMS_RECORDS_FRUITS_GETRECORD_INVOKE])
printFruit( Fruits::Apple );
DOX_MARKER( [DOX_ENUMS_RECORDS_FRUITS_GETRECORD_INVOKE])
}

} namespace version2 {
DOX_MARKER( [DOX_ENUMS_RECORDS_FRUITS_TRYRECORD])
void printFruit(Fruits fruit)
{
    const ERFruits* record= alib::enums::TryRecord(fruit);

    if( record != nullptr )
        cout << record->Name << endl;
    else
        cout << "Fruits(" << UnderlyingIntegral(fruit) << ")" << endl;
}
DOX_MARKER( [DOX_ENUMS_RECORDS_FRUITS_TRYRECORD])
void invoke()
{
DOX_MARKER( [DOX_ENUMS_RECORDS_FRUITS_TRYRECORD_INVOKE])
printFruit( Fruits(42) );
DOX_MARKER( [DOX_ENUMS_RECORDS_FRUITS_TRYRECORD_INVOKE])
}

// ### Chapter 4.2.3  ##############################
} namespace iterate {

void invoke()
{
DOX_MARKER( [DOX_ENUMS_RECORDS_FRUITS_ITERATE])
for( auto& fruitRecord : EnumRecords<Fruits>() )
    cout << fruitRecord.Name << endl;
DOX_MARKER( [DOX_ENUMS_RECORDS_FRUITS_ITERATE])
}

DOX_MARKER( [DOX_ENUMS_RECORDS_FRUITS_ITERATE_2])
Fruits readFruit(const String& input)
{
    for( auto it= EnumRecords<Fruits>::begin(); it != EnumRecords<Fruits>::end() ; ++it)
        if( input.Equals<CHK, lang::Case::Ignore>( it->Name ) )
            return it.Enum();

    return Fruits(-1);
}
DOX_MARKER( [DOX_ENUMS_RECORDS_FRUITS_ITERATE_2])
void invoke2()
{
DOX_MARKER( [DOX_ENUMS_RECORDS_FRUITS_ITERATE_2_INVOKE])
Fruits someFruit= readFruit( A_CHAR("Banana") );
assert( someFruit == Fruits::Banana );
DOX_MARKER( [DOX_ENUMS_RECORDS_FRUITS_ITERATE_2_INVOKE])
(void) someFruit;
}
}} // simple::version2

// ### Chapter 4.3.1  ##############################
DOX_MARKER([DOX_ENUMS_RECORDS_FRUITS_ASSIGN_ERSERIALZEABLE])
ALIB_ENUMS_ASSIGN_RECORD( Fruits, alib::enums::ERSerializable )
DOX_MARKER([DOX_ENUMS_RECORDS_FRUITS_ASSIGN_ERSERIALZEABLE])

void append()
{
DOX_MARKER([DOX_ENUMS_RECORDS_FRUITS_APPEND])
AString buffer;
buffer << Fruits::Banana;

assert( buffer.Equals( A_CHAR("Banana") ) );
DOX_MARKER([DOX_ENUMS_RECORDS_FRUITS_APPEND])
DOX_MARKER([DOX_ENUMS_RECORDS_FRUITS_PARSE])
Fruits     parsedFruit;
Substring  input  = A_CHAR("Banana");
bool       success= alib::enums::Parse( input, parsedFruit );

assert(    success && parsedFruit == Fruits::Banana   );
DOX_MARKER([DOX_ENUMS_RECORDS_FRUITS_PARSE])
(void) success;

DOX_MARKER([DOX_ENUMS_RECORDS_FRUITS_OSTREAM])
cout << Fruits::Orange;
DOX_MARKER([DOX_ENUMS_RECORDS_FRUITS_OSTREAM])
}

// ### Chapter 4.3.2  ##############################

DOX_MARKER([DOX_ENUMS_INHERITANCE])
struct ERAnything               { /* data members */ };
struct ERBase                   { /* data members */ };
struct ERDerived   : ERBase     { /* data members */ };

enum class Anything   { element };
enum class Base       { element };
enum class Derived    { element };

ALIB_ENUMS_ASSIGN_RECORD( Anything , ERAnything )
ALIB_ENUMS_ASSIGN_RECORD( Base     , ERBase     )
ALIB_ENUMS_ASSIGN_RECORD( Derived  , ERDerived  )

// A function accepting enums of type Base or a "derived enum type"
template<typename TEnum>                             typename std::enable_if<
  EnumRecords<TEnum>::template AreOfType<ERBase>()   >::type
acceptBaseOrDerived(TEnum element)
{
    const ERBase* record= alib::enums::TryRecord( element );
    (void) record; // do something...
}
DOX_MARKER([DOX_ENUMS_INHERITANCE])


// ###################################   Test Class   #############################################

namespace ut_aworx {

UT_CLASS


    UT_METHOD( lang_IntXX )
    {
        UT_INIT()
        UT_PRINT("*** Documentation Sample +**")

        dox_lang_complete_specialization::test();
        ut.WriteResultFile( "DOX_INTXX.txt", testOutputStream.str(), "" );
        testOutputStream.str("");


        dox_lang_singleton::test();
        testOutputStream.str("");
    }

    UT_METHOD( lang_enums )
    {
        UT_INIT()

        UT_PRINT("*** Documentation Sample +**")


// #################################################################################################
// ### Enum Arithmetic
// #################################################################################################
{
DOX_MARKER([DOX_ENUMS_ARITHMETIC_INTRO_2])
auto myElement_1 = MyEnum::One;
auto myElement_2 = MyEnum::Two;
auto myElement_3 = MyEnum(3);    // compiles well
auto myElement_42= MyEnum(42);   // compiles well
DOX_MARKER([DOX_ENUMS_ARITHMETIC_INTRO_2])
(void) myElement_1 ;
(void) myElement_2 ;
(void) myElement_3 ;
(void) myElement_42;

}
{
DOX_MARKER([DOX_ENUMS_ARITHMETIC_2])
auto myElement_3  = MyEnum::One  + MyEnum::Two;
auto myElement_42 = MyEnum::Five + 37;
auto myElement_43 = myElement_42++;
     myElement_42-= 1;
DOX_MARKER([DOX_ENUMS_ARITHMETIC_2])
(void) myElement_3  ;
(void) myElement_42 ;
(void) myElement_43 ;
}



// #################################################################################################
// ### Enum Iterator
// #################################################################################################

DOX_MARKER([DOX_ENUMS_ITER_SAMPLE_LOOP])
// loop over pets
for( auto element : { Pets::Cat, Pets::Dog, Pets::Bird, Pets::Snake } )
{
    // do something...
    cout << UnderlyingIntegral(element) << endl;
}
DOX_MARKER([DOX_ENUMS_ITER_SAMPLE_LOOP])

DOX_MARKER([DOX_ENUMS_ITER_SAMPLE_LOOP_NEW])
for( auto element : alib::enums::EnumIterator<Pets>() )
{
    // do something...
    cout << UnderlyingIntegral(element) << endl;
}
DOX_MARKER([DOX_ENUMS_ITER_SAMPLE_LOOP_NEW])



// #################################################################################################
// ### EnumBitSet
// #################################################################################################
// ### Chapter 3.5  ##############################

testOutputStream.str("");
iter_bitset::sample();
ut.WriteResultFile( "DOX_ENUMS_BITSET.txt", testOutputStream.str() );
testOutputStream.str("");



// #################################################################################################
// ### Enum Records
// #################################################################################################

// ### Chapter 4.1  ##############################
#if defined(__clang__)
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wdisabled-macro-expansion"
#endif

#define   Fruits simple::Fruits

{ALIB_LOCK_RECURSIVE_WITH( alib::monomem::GLOBAL_ALLOCATOR_LOCK )

DOX_MARKER([DOX_ENUMS_RECORDS_FRUITS_INIT_3CALLS])
alib::EnumRecords<Fruits>::Bootstrap( Fruits::Apple , A_CHAR("Apple" ) );
alib::EnumRecords<Fruits>::Bootstrap( Fruits::Orange, A_CHAR("Orange") );
alib::EnumRecords<Fruits>::Bootstrap( Fruits::Banana, A_CHAR("Banana") );
DOX_MARKER([DOX_ENUMS_RECORDS_FRUITS_INIT_3CALLS])
}

#undef   Fruits
#if defined(__clang__)
    #pragma clang diagnostic pop
#endif

if( !compiledButNotInvoked)
DOX_MARKER([DOX_ENUMS_RECORDS_FRUITS_INIT_1CALL])
alib::EnumRecords<Fruits>::Bootstrap(
{
    { Fruits::Apple , A_CHAR("Apple" )  },
    { Fruits::Orange, A_CHAR("Orange")  },
    { Fruits::Banana, A_CHAR("Banana")  },
} );
DOX_MARKER([DOX_ENUMS_RECORDS_FRUITS_INIT_1CALL])

if( !compiledButNotInvoked)
DOX_MARKER([DOX_ENUMS_RECORDS_FRUITS_INIT_ERSERIALZEABLE])
alib::EnumRecords<Fruits>::Bootstrap(
{
    { Fruits::Apple , A_CHAR("Apple" ), 1 },
    { Fruits::Orange, A_CHAR("Orange"), 1 },
    { Fruits::Banana, A_CHAR("Banana"), 1 },
} );
DOX_MARKER([DOX_ENUMS_RECORDS_FRUITS_INIT_ERSERIALZEABLE])

if( compiledButNotInvoked)
{ALIB_LOCK_RECURSIVE_WITH( alib::monomem::GLOBAL_ALLOCATOR_LOCK )

DOX_MARKER([DOX_ENUMS_RECORDS_FRUITS_INIT_STRING])
alib::EnumRecords<Fruits>::Bootstrap(
    A_CHAR(  "0"    ","    "Apple"   ","   "1"      ","
             "1"    ","    "Orange"  ","   "1"      ","
             "2"    ","    "Banana"  ","   "1"       )  );
DOX_MARKER([DOX_ENUMS_RECORDS_FRUITS_INIT_STRING])
}
// ### Chapter 4.2  ##############################

testOutputStream.str("");
simple::version1::invoke();
ut.WriteResultFile( "DOX_ENUMS_RECORDS_FRUITS_GETRECORD", testOutputStream.str() ,"");
testOutputStream.str("");

simple::version2::invoke();
ut.WriteResultFile( "DOX_ENUMS_RECORDS_FRUITS_TRYRECORD.txt", testOutputStream.str(), "" );
testOutputStream.str("");

simple::iterate::invoke();

ut.WriteResultFile( "DOX_ENUMS_RECORDS_FRUITS_ITERATE.txt", testOutputStream.str() );
testOutputStream.str("");

simple::iterate::invoke2();

// ### Chapter 4.3.1  ##############################
append();
assert( testOutputStream.str().compare("Orange") == 0 );
testOutputStream.str("");

// ### Chapter 4.3.2  ##############################

DOX_MARKER([DOX_ENUMS_INHERITANCE_INVOKE])
acceptBaseOrDerived(     Base::element );
acceptBaseOrDerived(  Derived::element );

#if defined( I_WANT_COMPILATION_ERRORS )
acceptBaseOrDerived( Anything::element );
#endif
DOX_MARKER([DOX_ENUMS_INHERITANCE_INVOKE])

// ### Chapter 4.?  ##############################

{ALIB_LOCK_RECURSIVE_WITH( alib::monomem::GLOBAL_ALLOCATOR_LOCK )
DOX_MARKER([DOX_ENUMS_BITWISE_DEFINITION])
alib::EnumRecords<WindowManager::States>::Bootstrap(

A_CHAR(
    // No state set
    "0"   ","   "Normal"      ","   "1"     ","

    // Combined entry. Put before single bits.
    "3"   ","   "Maximized"   ","   "1"     ","

    // The corresponding single bit entries.
    "1"   ","   "HMax"        ","   "1"     ","
    "2"   ","   "VMax"        ","   "1"     ","

    // Others...
    "4"   ","   "Hidden"       ","   "1"     )  );
DOX_MARKER([DOX_ENUMS_BITWISE_DEFINITION])


alib::EnumRecords<BitsParsable>::Bootstrap(
{
    { BitsParsable::One  ,   A_CHAR("One")     , 1  },
    { BitsParsable::Two  ,   A_CHAR("Two")     , 2  },
    { BitsParsable::Three,   A_CHAR("Three")   , 2  },
    { BitsParsable::Four ,   A_CHAR("Four")    , 1  },
} );

alib::EnumRecords<Bits>::Bootstrap(
{
    { Bits::One  ,   A_CHAR("One")    },
    { Bits::Two  ,   A_CHAR("Two")    },
    { Bits::Three,   A_CHAR("Three")  },
    { Bits::Four ,   A_CHAR("Four")   },
} );

}



{
DOX_MARKER([DOX_ENUMS_BITWISE_SAMPLE])
States stateNull      = States(0);
States stateHM        = States::HorizontallyMaximized;
States stateVM        = States::VerticallyMaximized;
States stateHMVM      = States::HorizontallyMaximized + States::VerticallyMaximized;
States stateHMHidden  = States::HorizontallyMaximized + States::Hidden;

cout << "Null:      " <<  stateNull       << endl;
cout << "HM:        " <<  stateHM         << endl;
cout << "VM:        " <<  stateVM         << endl;
cout << "VM+HM:     " <<  stateHMVM       << endl;
cout << "HM+Hidden: " <<  stateHMHidden   << endl;
DOX_MARKER([DOX_ENUMS_BITWISE_SAMPLE])

    UT_EQ( A_CHAR("Normal")      , alib::String64() << stateNull     )
    UT_EQ( A_CHAR("HMax")        , alib::String64() << stateHM       )
    UT_EQ( A_CHAR("VMax")        , alib::String64() << stateVM       )
    UT_EQ( A_CHAR("Maximized")   , alib::String64() << stateHMVM     )
    UT_EQ( A_CHAR("HMax,Hidden") , alib::String64() << stateHMHidden )
}
    ut.WriteResultFile( "DOX_ENUMS_BITWISE_OUTPUT.txt", testOutputStream.str() );
    testOutputStream.str("");


{
DOX_MARKER([DOX_ENUMS_NORMAL_SAMPLE])
Fruits fruit1= Fruits::Apple;
Fruits fruit2= Fruits::Orange;
Fruits fruit3= Fruits::Banana;

cout << "Fruit 1: " << fruit1  << endl;
cout << "Fruit 2: " << fruit2  << endl;
cout << "Fruit 3: " << fruit3  << endl;
DOX_MARKER([DOX_ENUMS_NORMAL_SAMPLE])
    UT_EQ( A_CHAR("Apple")  , alib::String64() << fruit1)
    UT_EQ( A_CHAR("Orange") , alib::String64() << fruit2)
    UT_EQ( A_CHAR("Banana") , alib::String64() << fruit3)


}

        ut.WriteResultFile( "DOX_ENUMS_NORMAL_OUTPUT.txt", testOutputStream.str() );
        testOutputStream.str("");

{


DOX_MARKER([DOX_ENUMS_OPERATORS_SAMPLE_1])
States maximized= States::HorizontallyMaximized | States::VerticallyMaximized;

// Still not allowed: compiler error "invalid operands to binary expression"
//Fruits fruit= Fruits::Apple | Fruits::Orange;
DOX_MARKER([DOX_ENUMS_OPERATORS_SAMPLE_1])
(void) maximized;
}

    }

    UT_METHOD( lang_enums_IterateBitwise )
    {
        UT_INIT()
        UT_PRINT("*** Documentation Sample +**")

        { ALIB_LOCK_RECURSIVE_WITH(monomem::GLOBAL_ALLOCATOR_LOCK)
            alib::EnumRecords<Bits>::Bootstrap(
            {
                { Bits::One   ,   A_CHAR("One"  ) },
                { Bits::Two   ,   A_CHAR("Two"  ) },
                { Bits::Three ,   A_CHAR("Three") },
                { Bits::Four  ,   A_CHAR("Four" ) },
            } );
        }


        AString buf;
        for( auto bit : EnumIterator<Bits>() )
            buf << bit ;
        UT_EQ( A_CHAR("OneTwoThreeFour")   , buf )
        buf._();

        for( auto bit= EnumIterator<Bits>().begin() ; bit < EnumIterator<Bits>().end(); ++bit )
            buf << *bit ;
        UT_EQ( A_CHAR("OneTwoThreeFour") , buf)
        buf._();

        for( auto bit= EnumIterator<Bits>().begin() ; bit < EnumIterator<Bits>().end();  )
        {
            buf << *bit ;
            bit+=2;
        }
        UT_EQ(A_CHAR("OneThree") , buf)
        buf._();

        auto bit= EnumIterator<Bits>().begin();
        UT_EQ( A_CHAR("One")   , buf << *bit     )  buf._();
        UT_EQ( A_CHAR("Two")   , buf << *(bit+1) )  buf._();
        UT_EQ( A_CHAR("Three") , buf << *(bit+2) )  buf._();
        UT_EQ( A_CHAR("Four")  , buf << *(bit+3) )  buf._();
        bit+=3;
        UT_EQ( A_CHAR("Four")  , buf << *bit     )  buf._();
        UT_EQ( A_CHAR("Three") , buf << *(bit-1) )  buf._();
        UT_EQ( A_CHAR("Two")   , buf << *(bit-2) )  buf._();
        UT_EQ( A_CHAR("One")   , buf << *(bit-3) )  buf._();

        UT_EQ( 4       , (  EnumIterator<Bits>().end  ()      -  EnumIterator<Bits>().begin()      ) )
        UT_EQ( 1       , ( (EnumIterator<Bits>().begin() +1 ) - (EnumIterator<Bits>().begin()    ) ) )
        UT_EQ( 2       , ( (EnumIterator<Bits>().begin() +3 ) - (EnumIterator<Bits>().begin() +1 ) ) )
        UT_EQ( 1       , (  EnumIterator<Bits>().end  ()      - (EnumIterator<Bits>().end  () -1 ) ) )

        bit= EnumIterator<Bits>().begin();
        UT_EQ( A_CHAR("One")   , buf << bit[0] ) buf._();
        UT_EQ( A_CHAR("Two")   , buf << bit[1] ) buf._();
        UT_EQ( A_CHAR("Three") , buf << bit[2] ) buf._();
        UT_EQ( A_CHAR("Four")  , buf << bit[3] ) buf._();
    }


    UT_METHOD( lang_enums_Parseing )
    {
        UT_INIT()
        UT_PRINT("*** Documentation Sample +**")

        { ALIB_LOCK_RECURSIVE_WITH(monomem::GLOBAL_ALLOCATOR_LOCK)
            alib::EnumRecords<BitsParsable>::Bootstrap(
            {
                { BitsParsable::One  , A_CHAR("One")     , 1 },
                { BitsParsable::Two  , A_CHAR("Two")     , 2 },
                { BitsParsable::Three, A_CHAR("Three")   , 2 },
                { BitsParsable::Four , A_CHAR("Four")    , 1 },
            } );
        }


        Substring s;

        lang::Case caseRead= lang::Case(-1);
        caseRead= lang::Case(-1);       s= A_CHAR("senitive")   ; UT_TRUE ( enums::Parse(s, caseRead ) )    UT_TRUE( caseRead== lang::Case::Sensitive)  UT_EQ( A_CHAR("itive")   , s)
        caseRead= lang::Case(-1);       s= A_CHAR("ignore")     ; UT_TRUE ( enums::Parse(s, caseRead ) )    UT_TRUE( caseRead== lang::Case::Ignore   )  UT_EQ( A_CHAR("")        , s)
        caseRead= lang::Case(-1);       s= A_CHAR("abc")        ; UT_FALSE( enums::Parse(s, caseRead ) )    UT_TRUE( caseRead== lang::Case(-1)       )  UT_EQ( A_CHAR("abc")     , s)

        lang::ContainerOp copRead;
        copRead= lang::ContainerOp(-1); s= A_CHAR("G")          ; UT_TRUE ( enums::Parse(s, copRead  ) )    UT_TRUE( copRead == lang::ContainerOp::Get       )  UT_EQ( A_CHAR("")        , s)
        copRead= lang::ContainerOp(-1); s= A_CHAR("Ge")         ; UT_TRUE ( enums::Parse(s, copRead  ) )    UT_TRUE( copRead == lang::ContainerOp::Get       )  UT_EQ( A_CHAR("")        , s)
        copRead= lang::ContainerOp(-1); s= A_CHAR("GET")        ; UT_TRUE ( enums::Parse(s, copRead  ) )    UT_TRUE( copRead == lang::ContainerOp::Get       )  UT_EQ( A_CHAR("")        , s)
        copRead= lang::ContainerOp(-1); s= A_CHAR("GETC")       ; UT_TRUE ( enums::Parse(s, copRead  ) )    UT_TRUE( copRead == lang::ContainerOp::GetCreate )  UT_EQ( A_CHAR("")        , s)
        copRead= lang::ContainerOp(-1); s= A_CHAR("GETX")       ; UT_TRUE ( enums::Parse(s, copRead  ) )    UT_TRUE( copRead == lang::ContainerOp::Get       )  UT_EQ( A_CHAR("X")       , s)

        BitsParsable bits;
        bits= BitsParsable(0);  UT_TRUE ( enums::ParseBitwise( s= A_CHAR("onex")     , bits) )       UT_TRUE( bits== BitsParsable::One   )         UT_EQ( A_CHAR("x")      , s)
        bits= BitsParsable(0);  UT_FALSE( enums::ParseBitwise( s= A_CHAR("murx")     , bits) )       UT_TRUE( bits== BitsParsable(0)     )         UT_EQ( A_CHAR("murx")   , s)
        bits= BitsParsable(0);  UT_TRUE ( enums::ParseBitwise( s= A_CHAR("one,twox") , bits) )       UT_TRUE( bits== BitsParsable::One
                                                                                                            + BitsParsable::Two           )             UT_EQ( A_CHAR("x")      , s)
        bits= BitsParsable(0);  UT_TRUE ( enums::ParseBitwise( s= A_CHAR("f")        , bits) )       UT_TRUE( bits== BitsParsable::Four  )         UT_EQ( A_CHAR("")       , s)
        bits= BitsParsable(0);  UT_TRUE ( enums::ParseBitwise( s= A_CHAR("f , murx") , bits)                                              )        UT_TRUE( bits== BitsParsable::Four   )  UT_EQ( A_CHAR(", murx") , s)
        bits= BitsParsable(0);  UT_TRUE ( enums::ParseBitwise<BitsParsable ALIB_COMMA character ALIB_COMMA lang::Case::Ignore ALIB_COMMA lang::Whitespaces::Keep ALIB_COMMA ',' ALIB_COMMA true >( s= A_CHAR("f , murx"), bits ))   UT_TRUE( bits== BitsParsable::Four   ) UT_EQ(A_CHAR(" , murx") , s)
        bits= BitsParsable(0);  UT_TRUE ( enums::ParseBitwise<BitsParsable ALIB_COMMA character ALIB_COMMA lang::Case::Ignore ALIB_COMMA lang::Whitespaces::Trim ALIB_COMMA ',' ALIB_COMMA false>( s= A_CHAR("f , murx"), bits ))   UT_TRUE( bits== BitsParsable::Four   ) UT_EQ(A_CHAR("murx") , s)
        bits= BitsParsable(0);  UT_TRUE ( enums::ParseBitwise<BitsParsable ALIB_COMMA character ALIB_COMMA lang::Case::Ignore ALIB_COMMA lang::Whitespaces::Keep ALIB_COMMA ',' ALIB_COMMA false>( s= A_CHAR("f, murx" ), bits ))   UT_TRUE( bits== BitsParsable::Four   ) UT_EQ(A_CHAR(" murx") , s)
    }

    UT_METHOD( lang_enums_Arithmetic )
    {
        UT_INIT()
        UT_PRINT("*** Documentation Sample +**")

        {ALIB_LOCK_RECURSIVE_WITH( alib::monomem::GLOBAL_ALLOCATOR_LOCK )
            alib::EnumRecords<ArithmeticEnum>::Bootstrap(
            {
                { ArithmeticEnum(0),  A_CHAR("E0") },
                { ArithmeticEnum(1),  A_CHAR("E1") },
                { ArithmeticEnum(2),  A_CHAR("E2") },
                { ArithmeticEnum(3),  A_CHAR("E3") },
                { ArithmeticEnum(4),  A_CHAR("E4") },
                { ArithmeticEnum(5),  A_CHAR("E5") },
            } );
        }


        UT_EQ   ( ArithmeticEnum::element3  , ArithmeticEnum::element1 + ArithmeticEnum::element2 )
        UT_EQ   ( ArithmeticEnum::element0  , ArithmeticEnum::element2 - ArithmeticEnum::element2 )
        UT_EQ   ( ArithmeticEnum::element4  , ArithmeticEnum::element2 + 2 )
        UT_EQ   ( ArithmeticEnum::element0  , ArithmeticEnum::element2 - 2 )
        UT_EQ   ( ArithmeticEnum::element2  , ArithmeticEnum::element2 - 0 )
        UT_EQ   ( ArithmeticEnum::element2  , ArithmeticEnum::element2 + 0 )
        UT_TRUE ( ArithmeticEnum::element0 <  ArithmeticEnum::element1 )
        UT_TRUE ( ArithmeticEnum::element0 <= ArithmeticEnum::element1 )
        UT_TRUE ( ArithmeticEnum::element1 <= ArithmeticEnum::element2 - 1 )
        UT_FALSE( ArithmeticEnum::element0 >  ArithmeticEnum::element1 )
        UT_FALSE( ArithmeticEnum::element0 >= ArithmeticEnum::element1 )
        UT_TRUE ( ArithmeticEnum::element1 >= ArithmeticEnum::element2 - 1 )

        ArithmeticEnum e= ArithmeticEnum::element0;
        e+= 1;                          UT_EQ ( ArithmeticEnum::element1 , e )
        e+= 2;                          UT_EQ ( ArithmeticEnum::element3 , e )
        e+= ArithmeticEnum::element2 ;  UT_EQ ( ArithmeticEnum::element5 , e )
        e-= ArithmeticEnum::element2 ;  UT_EQ ( ArithmeticEnum::element3 , e )
        e-= 2;                          UT_EQ ( ArithmeticEnum::element1 , e )
        e-= 1;                          UT_EQ ( ArithmeticEnum::element0 , e )
        e+= 100;  UT_EQ ( ArithmeticEnum::element0 + 100, e )
                  UT_EQ ( ArithmeticEnum::element0 + 200, e + 100 )
        e= ArithmeticEnum::element0;
        UT_EQ ( ArithmeticEnum::element1 , ++e   )
        UT_EQ ( ArithmeticEnum::element1 , e++   )
        UT_EQ ( ArithmeticEnum::element2 , e     )
        UT_EQ ( ArithmeticEnum::element2 , e--   )
        UT_EQ ( ArithmeticEnum::element0 , --e   )
        UT_EQ ( ArithmeticEnum::element2 , e+=2  )
        UT_EQ ( ArithmeticEnum::element4 , e* 2  )
        UT_EQ ( ArithmeticEnum::element4 , e*=2  )
        UT_EQ ( ArithmeticEnum::element2 , e / 2 )
        UT_EQ ( ArithmeticEnum::element2 , e/= 2 )
        UT_EQ ( ArithmeticEnum::element1 , (e+3) % 2 )
        UT_EQ ( ArithmeticEnum::element5 , e+= 3 )
        UT_EQ ( ArithmeticEnum::element1 , e%= 2 )
        UT_EQ ( ArithmeticEnum::element1 , +e    )
        UT_EQ ( ArithmeticEnum::element1 , -(-e) )
    }

#include "unittests/aworx_unittests_end.hpp"

} //namespace


#endif //  ALIB_UT_DOCS && ALIB_ENUMS
