// #################################################################################################
//  aworx - Unit Tests
//  Private, not published in git ( I hope! )
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alox.hpp"

#include "../aworx_unittests.hpp"
#define TESTCLASSNAME       CPP_ALib_Dox_LANG


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
#include <iostream>
#include <sstream>
#include "alib/compatibility/std_string.hpp"
#include "alib/lang/enumiterators.hpp"

namespace std
{
    extern std::basic_stringstream<aworx::character> testOutputStream; // declaration (needed when clang warnings are on)
           std::basic_stringstream<aworx::character> testOutputStream;
}
#define cout testOutputStream
using namespace std;
using namespace aworx;
using namespace aworx::literals;

// ###################################    intGap_t    #############################################
namespace dox_lang_complete_specialization {

//! [DOX_ALIB_LANG_INTEGER_LITERAL]

aworx::integer myInt= 42_integer;

//! [DOX_ALIB_LANG_INTEGER_LITERAL]


//! [DOX_ALIB_LANG_INTXX_DECLARATION]

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

//! [DOX_ALIB_LANG_INTXX_DECLARATION]

}//namespace dox_lang_complete_specialization

namespace dox_lang_complete_specialization2 {

template <typename TInteger> const char* MyFunc(TInteger val) { return "NOT IMPLEMENTED"; }

//! [DOX_ALIB_LANG_INTXX_DECLARATION2]
template<> const char* MyFunc( aworx:: intGap_t ) { return "Type=  intGap_t"; }
template<> const char* MyFunc( aworx::uintGap_t ) { return "Type= uintGap_t"; }
//! [DOX_ALIB_LANG_INTXX_DECLARATION2]
}//namespace dox_lang_complete_specialization2


// ###################################    Singleton    #############################################
namespace dox_lang_singleton
{
    extern void test();
}



// ###################################    enums    #############################################
namespace WindowManager {}
namespace MyNamespace   {}

using namespace WindowManager;
using namespace MyNamespace;


//! [DOX_ALIB_LANG_ENUMS_NORMAL]
namespace MyNamespace
{
    enum class Fruits
    {
        Apple,
        Orange,
        Banana,
    };
}

ALIB_LANG_ENUM_NAMED( MyNamespace::Fruits )
//! [DOX_ALIB_LANG_ENUMS_NORMAL]



//! [DOX_ALIB_LANG_ENUMS_IRRITATING]
enum class Numbers
{
    zero,   // underlying int value: 0
    one,    // underlying int value: 1
    two,    // underlying int value: 2
};
//! [DOX_ALIB_LANG_ENUMS_IRRITATING]

//! [DOX_ALIB_LANG_ENUMS_CUSTOM]
namespace MyNamespace
{
    enum class Pets
    {
        Cat,
        Dog,
        Bird,
        Snake,
    };
}


ALIB_LANG_ENUM_META_DATA(
    MyNamespace::Pets, // The enum type.
    int ,              // tuple element 1: number of legs. (0 is underlying integer)
    int ,              // tuple element 2: number of wings.
    aworx::String      // tuple element 3: element name
)
ALIB_LANG_ENUM_READ_WRITE(
    MyNamespace::Pets, // The enum type.
    3   ,              // index of name element in the tuple.
    0                  // index of minimum read length. Set to 0, hence parsing disabled.
)
//! [DOX_ALIB_LANG_ENUMS_CUSTOM]



namespace repeat {
//! [DOX_ALIB_LANG_ENUMS_CUSTOM_REPEAT]
namespace MyNamespace
{
    enum class Pets
    {
        Cat,
        Dog,
        Bird,
        Snake,
    };
}
//! [DOX_ALIB_LANG_ENUMS_CUSTOM_REPEAT]
}

//! [DOX_ALIB_LANG_ENUMS_CUSTOM_MAKE_ITERATABLE]
ALIB_LANG_ENUM_IS_ITERATABLE(MyNamespace::Pets, MyNamespace::Pets::Snake + 1 )
//! [DOX_ALIB_LANG_ENUMS_CUSTOM_MAKE_ITERATABLE]


//! [DOX_ALIB_LANG_ENUMS_BITWISE_DECLARATION]
namespace WindowManager
{
    enum class States
    {
        HorizontallyMaximized  = (1 << 0),
        VerticallyMaximized    = (1 << 1),
        Hidden                 = (1 << 2),
    };
}

ALIB_LANG_ENUM_NAMED( WindowManager::States )
//! [DOX_ALIB_LANG_ENUMS_BITWISE_DECLARATION]

//! [DOX_ALIB_LANG_ENUMS_BITWISE_SPECIALIZE]
ALIB_LANG_ENUM_IS_BITWISE( WindowManager::States )
//! [DOX_ALIB_LANG_ENUMS_BITWISE_SPECIALIZE]



namespace MyNamespace
{
    enum class Bits
    {
        NONE            =  0,
        One             =  1 << 0,
        Two             =  1 << 1,
        Three           =  1 << 2,
        Four            =  1 << 3,
        END_OF_ENUM     =  1 << 4,
    };
}

ALIB_LANG_ENUM_NAMED(         MyNamespace::Bits )
ALIB_LANG_ENUM_IS_BITWISE(    MyNamespace::Bits )
ALIB_LANG_ENUM_IS_ITERATABLE( MyNamespace::Bits, Bits::END_OF_ENUM )

enum class BitsParseable
{
    NONE            =  0,
    One             =  1 << 0,
    Two             =  1 << 1,
    Three           =  1 << 2,
    Four            =  1 << 3,
    END_OF_ENUM     =  1 << 4,
};


ALIB_LANG_ENUM_PARSABLE(  BitsParseable )
ALIB_LANG_ENUM_IS_BITWISE(           BitsParseable )

// ###################################   ArithmeticEnum   #############################################

enum class ArithmeticEnum
{
    element0,
    element1,
    element2,
    element3,
    element4,
    element5,
};
ALIB_LANG_ENUM_NAMED(ArithmeticEnum)

ALIB_LANG_ENUM_IS_ARITHMETICAL(ArithmeticEnum)


// ###################################   Test Class   #############################################

namespace ut_aworx {

UT_CLASS()


    UT_METHOD( lang_IntXX )
    {
        UT_INIT();
        UT_PRINT("*** Documentation Sample +**")

        dox_lang_complete_specialization::test();
        ut.WriteResultFile( "DOX_ALIB_LANG_INTXX.txt", testOutputStream.str() );
        testOutputStream.str(ASTR(""));


        dox_lang_singleton::test();
        testOutputStream.str(ASTR(""));
    }

    UT_METHOD( lang_enums )
    {
        UT_INIT();

        UT_PRINT("*** Documentation Sample +**")

//! [DOX_ALIB_LANG_ENUMS_NORMAL_NAME_TABLE_DEFINITION]
EnumMetaData<MyNamespace::Fruits>::GetSingleton()->Table=
{
    { EnumValue( Fruits::Apple  ), ASTR("Apple")  },
    { EnumValue( Fruits::Orange ), ASTR("Orange") },
    { EnumValue( Fruits::Banana ), ASTR("Banana") },
};
//! [DOX_ALIB_LANG_ENUMS_NORMAL_NAME_TABLE_DEFINITION]

//! [DOX_ALIB_LANG_ENUMS_CUSTOM_NAME_TABLE_DEFINITION]
EnumMetaData<MyNamespace::Pets>::GetSingleton()->Table=
{
    //       pet enum           #legs    #wings      name
    { EnumValue( Pets::Cat   ),   4    ,   0     ,  ASTR("Cat")   },
    { EnumValue( Pets::Dog   ),   4    ,   0     ,  ASTR("Dog")   },
    { EnumValue( Pets::Bird  ),   2    ,   2     ,  ASTR("Bird")  },
    { EnumValue( Pets::Snake ),   0    ,   0     ,  ASTR("Snake") },
};
//! [DOX_ALIB_LANG_ENUMS_CUSTOM_NAME_TABLE_DEFINITION]

//! [DOX_ALIB_LANG_ENUMS_BITWISE_DEFINITION]
EnumMetaData<WindowManager::States>::GetSingleton()->Table=
{
    { 0                                                            , ASTR("Normal")   },

    // Combined entry. Put before single bits.
    {   EnumValue( WindowManager::States::HorizontallyMaximized
                 + WindowManager::States::VerticallyMaximized   )  ,ASTR("Maximized") },

    // The corresponding single bit entries.
    {   EnumValue( WindowManager::States::HorizontallyMaximized )  , ASTR("HMax")     },
    {   EnumValue( WindowManager::States::VerticallyMaximized   )  , ASTR("VMax")     },

    // Others...
    {   EnumValue( WindowManager::States::Hidden                )  , ASTR("Hidden")   },
};
//! [DOX_ALIB_LANG_ENUMS_BITWISE_DEFINITION]


EnumMetaData<BitsParseable>::GetSingleton()->Table=
{
    { EnumValue( BitsParseable::One   ),  ASTR("One")     , 1  },
    { EnumValue( BitsParseable::Two   ),  ASTR("Two")     , 2  },
    { EnumValue( BitsParseable::Three ),  ASTR("Three")   , 2  },
    { EnumValue( BitsParseable::Four  ),  ASTR("Four")    , 1  },
};

EnumMetaData<MyNamespace::Bits>::GetSingleton()->Table=
{
    { EnumValue( Bits::One   ),  ASTR("One")       },
    { EnumValue( Bits::Two   ),  ASTR("Two")       },
    { EnumValue( Bits::Three ),  ASTR("Three")     },
    { EnumValue( Bits::Four  ),  ASTR("Four")      },
};




{
//! [DOX_ALIB_LANG_ENUMS_PARSABLE_SAMPLE]
Case target;
Substring(ASTR("sensitive")).ConsumeEnum( target ); cout << target << endl;
Substring(ASTR("s")        ).ConsumeEnum( target ); cout << target << endl;
Substring(ASTR("ignore")   ).ConsumeEnum( target ); cout << target << endl;
Substring(ASTR("ign")      ).ConsumeEnum( target ); cout << target << endl;
//! [DOX_ALIB_LANG_ENUMS_PARSABLE_SAMPLE]
        UT_EQ( true , Substring(ASTR("sensitive")).ConsumeEnum( target ));   UT_TRUE( Case::Sensitive == target);
        UT_EQ( true , Substring(ASTR("s")        ).ConsumeEnum( target ));   UT_TRUE( Case::Sensitive == target);
        UT_EQ( true , Substring(ASTR("ignore")   ).ConsumeEnum( target ));   UT_TRUE( Case::Ignore    == target);
        UT_EQ( true , Substring(ASTR("ign")      ).ConsumeEnum( target ));   UT_TRUE( Case::Ignore    == target);
        UT_EQ( false, Substring(ASTR("hurz")     ).ConsumeEnum( target ));   UT_TRUE( Case::Ignore == target);
}
        ut.WriteResultFile( "DOX_ALIB_LANG_ENUMS_PARSABLE_OUTPUT.txt", testOutputStream.str() );
        testOutputStream.str(ASTR(""));



{
//! [DOX_ALIB_LANG_ENUMS_BITWISE_SAMPLE]
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
//! [DOX_ALIB_LANG_ENUMS_BITWISE_SAMPLE]
        UT_EQ( ASTR("Normal")      , aworx::String64() << stateNull     )
        UT_EQ( ASTR("HMax")        , aworx::String64() << stateHM       )
        UT_EQ( ASTR("VMax")        , aworx::String64() << stateVM       )
        UT_EQ( ASTR("Maximized")   , aworx::String64() << stateHMVM     )
        UT_EQ( ASTR("HMax,Hidden") , aworx::String64() << stateHMHidden )
}
        ut.WriteResultFile( "DOX_ALIB_LANG_ENUMS_BITWISE_OUTPUT.txt", testOutputStream.str() );
        testOutputStream.str(ASTR(""));

{
//! [DOX_ALIB_LANG_ENUMS_CUSTOM_SAMPLE_CHECKMAP]
// Initialize hash map from meta data table. Needs to be done once on bootstrap.
aworx::EnumMetaData<Pets>::GetSingleton()->CheckMap();
//! [DOX_ALIB_LANG_ENUMS_CUSTOM_SAMPLE_CHECKMAP]

//! [DOX_ALIB_LANG_ENUMS_CUSTOM_SAMPLE]
// A vector of pets
std::vector<Pets> pets= { Pets::Cat, Pets::Dog, Pets::Bird, Pets::Snake };

// loop over them
auto& enumMetaData= *aworx::EnumMetaData<Pets>::GetSingleton();
for( auto pet : pets )
{
    cout << "A "     << aworx::EnumReadWriteInfo<Pets>::Name( enumMetaData, pet )
         << " has "  << enumMetaData.Value<1>( pet ) << " legs"
         << " and "  << enumMetaData.Value<2>( pet ) << " wings!" << endl;
}
//! [DOX_ALIB_LANG_ENUMS_CUSTOM_SAMPLE]


    UT_EQ( 4       , aworx::EnumMetaData<Pets>::GetSingleton()->Value<1>( Pets::Cat   ) )
    UT_EQ( 4       , aworx::EnumMetaData<Pets>::GetSingleton()->Value<1>( Pets::Dog   ) )
    UT_EQ( 2       , aworx::EnumMetaData<Pets>::GetSingleton()->Value<1>( Pets::Bird  ) )
    UT_EQ( 0       , aworx::EnumMetaData<Pets>::GetSingleton()->Value<1>( Pets::Snake ) )
    UT_EQ( 0       , aworx::EnumMetaData<Pets>::GetSingleton()->Value<2>( Pets::Cat   ) )
    UT_EQ( 0       , aworx::EnumMetaData<Pets>::GetSingleton()->Value<2>( Pets::Dog   ) )
    UT_EQ( 2       , aworx::EnumMetaData<Pets>::GetSingleton()->Value<2>( Pets::Bird  ) )
    UT_EQ( 0       , aworx::EnumMetaData<Pets>::GetSingleton()->Value<2>( Pets::Snake ) )
    UT_EQ( ASTR("Cat")   , aworx::EnumReadWriteInfo<Pets>::Name( Pets::Cat   ))
    UT_EQ( ASTR("Dog")   , aworx::EnumReadWriteInfo<Pets>::Name( Pets::Dog   ))
    UT_EQ( ASTR("Bird")  , aworx::EnumReadWriteInfo<Pets>::Name( Pets::Bird  ))
    UT_EQ( ASTR("Snake") , aworx::EnumReadWriteInfo<Pets>::Name( Pets::Snake ))

}
        ut.WriteResultFile( "DOX_ALIB_LANG_ENUMS_CUSTOM_OUTPUT.txt", testOutputStream.str() );
        testOutputStream.str(ASTR(""));

//! [DOX_ALIB_LANG_ENUMS_CUSTOM_SAMPLE_WITH_ITERATOR]
auto& enumMetaData= *aworx::EnumMetaData<Pets>::GetSingleton();
for( auto pet : aworx::EnumIterator<Pets>() )
{
    cout << "A "     << aworx::EnumReadWriteInfo<Pets>::Name( enumMetaData, pet )
         << " has "  << enumMetaData.Value<1>( pet ) << " legs"
         << " and "  << enumMetaData.Value<2>( pet ) << " wings!" << endl;
}
//! [DOX_ALIB_LANG_ENUMS_CUSTOM_SAMPLE_WITH_ITERATOR]

//! [DOX_ALIB_LANG_ENUMS_IRRITATING_2]
Numbers aThree= Numbers(3);
//! [DOX_ALIB_LANG_ENUMS_IRRITATING_2]
(void) aThree;
        testOutputStream.str(ASTR(""));


{
//! [DOX_ALIB_LANG_ENUMS_NORMAL_SAMPLE]
Fruits fruit1= Fruits::Apple;
Fruits fruit2= Fruits::Orange;
Fruits fruit3= Fruits::Banana;

cout << "Fruit 1: " << fruit1  << endl;
cout << "Fruit 2: " << fruit2  << endl;
cout << "Fruit 3: " << fruit3  << endl;
//! [DOX_ALIB_LANG_ENUMS_NORMAL_SAMPLE]
    UT_EQ( ASTR("Apple")  , aworx::String64() << fruit1);
    UT_EQ( ASTR("Orange") , aworx::String64() << fruit2);
    UT_EQ( ASTR("Banana") , aworx::String64() << fruit3);


}

        ut.WriteResultFile( "DOX_ALIB_LANG_ENUMS_NORMAL_OUTPUT.txt", testOutputStream.str() );
        testOutputStream.str(ASTR(""));

{

{
//! [DOX_ALIB_LANG_ENUMS_BITWISE_SAMPLEERROR]
States maximized= States::HorizontallyMaximized | States::VerticallyMaximized;
//! [DOX_ALIB_LANG_ENUMS_BITWISE_SAMPLEERROR]
(void) maximized;
}

//! [DOX_ALIB_LANG_ENUMS_OPERATORS_SAMPLE_1]
// Now allowed:
States maximized= States::HorizontallyMaximized | States::VerticallyMaximized;

// Still not allowed: compiler error "invalid operands to binary expression"
//Fruits fruit= Fruits::Apple | MyNamespace::Fruits::Orange;
//! [DOX_ALIB_LANG_ENUMS_OPERATORS_SAMPLE_1]
(void) maximized;
}

    }

    UT_METHOD( lang_enums_IterateBitwise )
    {
        UT_INIT();
        UT_PRINT("*** Documentation Sample +**")

        EnumMetaData<MyNamespace::Bits>::GetSingleton()->Table=
        {
            { EnumValue( Bits::One   ),  ASTR("One")       },
            { EnumValue( Bits::Two   ),  ASTR("Two")       },
            { EnumValue( Bits::Three ),  ASTR("Three")     },
            { EnumValue( Bits::Four  ),  ASTR("Four")      },
        };


        AString buf;
        for( auto bit : EnumIterator<Bits>() )
            buf << bit ;
        UT_EQ( ASTR("OneTwoThreeFour")   , buf );
        buf._();

        for( auto bit= EnumIterator<Bits>().begin() ; bit < EnumIterator<Bits>().end(); bit++ )
            buf << *bit ;
        UT_EQ( ASTR("OneTwoThreeFour") , buf);
        buf._();

        for( auto bit= EnumIterator<Bits>().begin() ; bit < EnumIterator<Bits>().end();  )
        {
            buf << *bit ;
            bit+=2;
        }
        UT_EQ(ASTR("OneThree") , buf);
        buf._();

        auto bit= EnumIterator<Bits>().begin();
        UT_EQ( ASTR("One")   , buf << *bit     ); buf._();
        UT_EQ( ASTR("Two")   , buf << *(bit+1) ); buf._();
        UT_EQ( ASTR("Three") , buf << *(bit+2) ); buf._();
        UT_EQ( ASTR("Four")  , buf << *(bit+3) ); buf._();
        bit+=3;
        UT_EQ( ASTR("Four")  , buf << *bit     ); buf._();
        UT_EQ( ASTR("Three") , buf << *(bit-1) ); buf._();
        UT_EQ( ASTR("Two")   , buf << *(bit-2) ); buf._();
        UT_EQ( ASTR("One")   , buf << *(bit-3) ); buf._();

        UT_EQ( 4       , (  EnumIterator<Bits>().end  ()      -  EnumIterator<Bits>().begin()      ) );
        UT_EQ( 1       , ( (EnumIterator<Bits>().begin() +1 ) - (EnumIterator<Bits>().begin()    ) ) );
        UT_EQ( 2       , ( (EnumIterator<Bits>().begin() +3 ) - (EnumIterator<Bits>().begin() +1 ) ) );
        UT_EQ( 1       , (  EnumIterator<Bits>().end  ()      - (EnumIterator<Bits>().end  () -1 ) ) );

        bit= EnumIterator<Bits>().begin();
        UT_EQ( ASTR("One")   , buf << bit[0] ) buf._();
        UT_EQ( ASTR("Two")   , buf << bit[1] ) buf._();
        UT_EQ( ASTR("Three") , buf << bit[2] ) buf._();
        UT_EQ( ASTR("Four")  , buf << bit[3] ) buf._();
    }


    UT_METHOD( lang_enums_Parseing )
    {
        UT_INIT();
        UT_PRINT("*** Documentation Sample +**")

        EnumMetaData<BitsParseable>::GetSingleton()->Table=
        {
            { EnumValue( BitsParseable::One   ),  ASTR("One")     , 1  },
            { EnumValue( BitsParseable::Two   ),  ASTR("Two")     , 2  },
            { EnumValue( BitsParseable::Three ),  ASTR("Three")   , 2  },
            { EnumValue( BitsParseable::Four  ),  ASTR("Four")    , 1  },
        };


        Substring s;

        Case caseRead= Case(-1);
        caseRead= Case(-1);        s= ASTR("senitive")   ; UT_TRUE ( s.ConsumeEnum( caseRead ) );    UT_TRUE( caseRead== Case::Sensitive        ); UT_EQ( ASTR("itive")   , s);
        caseRead= Case(-1);        s= ASTR("ignore")     ; UT_TRUE ( s.ConsumeEnum( caseRead ) );    UT_TRUE( caseRead== Case::Ignore           ); UT_EQ( ASTR("")        , s);
        caseRead= Case(-1);        s= ASTR("abc")        ; UT_FALSE( s.ConsumeEnum( caseRead ) );    UT_TRUE( caseRead== Case(-1)               ); UT_EQ( ASTR("abc")     , s);

        ContainerOp copRead;
        copRead= ContainerOp(-1);  s= ASTR("G")          ; UT_TRUE ( s.ConsumeEnum( copRead  ) );    UT_TRUE( copRead == ContainerOp::Get       ); UT_EQ( ASTR("")        , s);
        copRead= ContainerOp(-1);  s= ASTR("Ge")         ; UT_TRUE ( s.ConsumeEnum( copRead  ) );    UT_TRUE( copRead == ContainerOp::Get       ); UT_EQ( ASTR("")        , s);
        copRead= ContainerOp(-1);  s= ASTR("GET")        ; UT_TRUE ( s.ConsumeEnum( copRead  ) );    UT_TRUE( copRead == ContainerOp::Get       ); UT_EQ( ASTR("")        , s);
        copRead= ContainerOp(-1);  s= ASTR("GETC")       ; UT_TRUE ( s.ConsumeEnum( copRead  ) );    UT_TRUE( copRead == ContainerOp::GetCreate ); UT_EQ( ASTR("")        , s);
        copRead= ContainerOp(-1);  s= ASTR("GETX")       ; UT_TRUE ( s.ConsumeEnum( copRead  ) );    UT_TRUE( copRead == ContainerOp::Get       ); UT_EQ( ASTR("X")       , s);

        BitsParseable bits;
        bits= BitsParseable(0); s= ASTR("onex")     ; UT_TRUE ( s.ConsumeEnumBitwise( bits) );      UT_TRUE( bits== BitsParseable::One   );      UT_EQ( ASTR("x")      , s);
        bits= BitsParseable(0); s= ASTR("murx")     ; UT_FALSE( s.ConsumeEnumBitwise( bits) );      UT_TRUE( bits== BitsParseable(0)     );      UT_EQ( ASTR("murx")   , s);
        bits= BitsParseable(0); s= ASTR("one,twox") ; UT_TRUE ( s.ConsumeEnumBitwise( bits) );      UT_TRUE( bits== BitsParseable::One
                                                                                                            + BitsParseable::Two   );      UT_EQ( ASTR("x")      , s);
        bits= BitsParseable(0); s= ASTR("f")        ; UT_TRUE ( s.ConsumeEnumBitwise( bits) );      UT_TRUE( bits== BitsParseable::Four  );      UT_EQ( ASTR("")       , s);
        bits= BitsParseable(0); s= ASTR("f , murx") ; UT_TRUE ( s.ConsumeEnumBitwise( bits)                                              );      UT_TRUE( bits== BitsParseable::Four   ); UT_EQ( ASTR(", murx") , s);
        bits= BitsParseable(0); s= ASTR("f , murx") ; UT_TRUE ( s.ConsumeEnumBitwise<BitsParseable ALIB_COMMA Case::Ignore ALIB_COMMA Whitespaces::Keep ALIB_COMMA ',' ALIB_COMMA true >( bits ));      UT_TRUE( bits== BitsParseable::Four   ); UT_EQ(ASTR(" , murx") , s);
        bits= BitsParseable(0); s= ASTR("f , murx") ; UT_TRUE ( s.ConsumeEnumBitwise<BitsParseable ALIB_COMMA Case::Ignore ALIB_COMMA Whitespaces::Trim ALIB_COMMA ',' ALIB_COMMA false>( bits ));      UT_TRUE( bits== BitsParseable::Four   ); UT_EQ(   ASTR("murx") , s);
        bits= BitsParseable(0); s= ASTR("f, murx") ;  UT_TRUE ( s.ConsumeEnumBitwise<BitsParseable ALIB_COMMA Case::Ignore ALIB_COMMA Whitespaces::Keep ALIB_COMMA ',' ALIB_COMMA false>( bits ));      UT_TRUE( bits== BitsParseable::Four   ); UT_EQ(  ASTR(" murx") , s);
    }

    UT_METHOD( lang_enums_Arithmetic )
    {
        UT_INIT();
        UT_PRINT("*** Documentation Sample +**")

        EnumMetaData<ArithmeticEnum>::GetSingleton()->Table=
        {
            { 0,  ASTR("E0") },
            { 1,  ASTR("E1") },
            { 2,  ASTR("E2") },
            { 3,  ASTR("E3") },
            { 4,  ASTR("E4") },
            { 5,  ASTR("E5") },
        };

        UT_EQ   ( ArithmeticEnum::element3  , ArithmeticEnum::element1 + ArithmeticEnum::element2 );
        UT_EQ   ( ArithmeticEnum::element0  , ArithmeticEnum::element2 - ArithmeticEnum::element2 );
        UT_EQ   ( ArithmeticEnum::element4  , ArithmeticEnum::element2 + 2 );
        UT_EQ   ( ArithmeticEnum::element0  , ArithmeticEnum::element2 - 2 );
        UT_EQ   ( ArithmeticEnum::element2  , ArithmeticEnum::element2 - 0 );
        UT_EQ   ( ArithmeticEnum::element2  , ArithmeticEnum::element2 + 0 );
        UT_TRUE ( ArithmeticEnum::element0 <  ArithmeticEnum::element1 );
        UT_TRUE ( ArithmeticEnum::element0 <= ArithmeticEnum::element1 );
        UT_TRUE ( ArithmeticEnum::element1 <= ArithmeticEnum::element2 - 1 );
        UT_FALSE( ArithmeticEnum::element0 >  ArithmeticEnum::element1 );
        UT_FALSE( ArithmeticEnum::element0 >= ArithmeticEnum::element1 );
        UT_TRUE ( ArithmeticEnum::element1 >= ArithmeticEnum::element2 - 1 );

        ArithmeticEnum e= ArithmeticEnum::element0;
        e+= 1;                          UT_EQ ( ArithmeticEnum::element1 , e );
        e+= 2;                          UT_EQ ( ArithmeticEnum::element3 , e );
        e+= ArithmeticEnum::element2 ;  UT_EQ ( ArithmeticEnum::element5 , e );
        e-= ArithmeticEnum::element2 ;  UT_EQ ( ArithmeticEnum::element3 , e );
        e-= 2;                          UT_EQ ( ArithmeticEnum::element1 , e );
        e-= 1;                          UT_EQ ( ArithmeticEnum::element0 , e );
        e+= 100;  UT_EQ ( ArithmeticEnum::element0 + 100, e );
                  UT_EQ ( ArithmeticEnum::element0 + 200, e + 100 );
        e= ArithmeticEnum::element0;
        UT_EQ ( ArithmeticEnum::element1 , ++e   );
        UT_EQ ( ArithmeticEnum::element1 , e++   );
        UT_EQ ( ArithmeticEnum::element2 , e     );
        UT_EQ ( ArithmeticEnum::element2 , e--   );
        UT_EQ ( ArithmeticEnum::element0 , --e   );
        UT_EQ ( ArithmeticEnum::element2 , e+=2  );
        UT_EQ ( ArithmeticEnum::element4 , e* 2  );
        UT_EQ ( ArithmeticEnum::element4 , e*=2  );
        UT_EQ ( ArithmeticEnum::element2 , e / 2 );
        UT_EQ ( ArithmeticEnum::element2 , e/= 2 );
        UT_EQ ( ArithmeticEnum::element1 , (e+3) % 2 );
        UT_EQ ( ArithmeticEnum::element5 , e+= 3 );
        UT_EQ ( ArithmeticEnum::element1 , e%= 2 );
        UT_EQ ( ArithmeticEnum::element1 , +e    );
        UT_EQ ( ArithmeticEnum::element1 , -(-e) );
    }



UT_CLASS_END

}; //namespace



