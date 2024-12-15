// #################################################################################################
//  AWorx ALib Unit Tests
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"
#include "unittests/alib_test_selection.hpp"
#include "alib/containers/sharedval.hpp"
#include "alib/monomem/globalallocator.hpp"

#if ALIB_UT_MONOMEM

// we cannot do the following within the unit tests project, which imports the ALIB_DLL.
// It has to be done in an external lib.
#if !defined(_WIN32)

DOX_MARKER([ALIB_MANUAL_T_HPP_FILE_USAGE2] )
// #################################################################################################
// Instantiation of TMonoAllocator for PoolAllocatorHA and its inner type
// detail::MAFields<PoolAllocatorHA>.
// #################################################################################################
#include "alib/monomem/poolallocator.hpp"
#include "alib/monomem/monoallocator.t.hpp"

namespace alib::monomem {

template class TMonoAllocator<alib::PoolAllocatorHA>;

}  // namespace [alib[::monomem]
DOX_MARKER([ALIB_MANUAL_T_HPP_FILE_USAGE2])
#endif

#include "alib/monomem/aliases/astringma.hpp"
#include "alib/monomem/aliases/astringpa.hpp"
#include "alib/monomem/localallocator.hpp"

#include "alib/enums/serialization.hpp"


#define TESTCLASSNAME       UT_ContMono

#include "unittests/aworx_unittests.hpp"
#include <numeric>
#include <memory_resource>

using namespace std;
using namespace alib;
using namespace alib::lang::system;



#include "alib/lang/callerinfo_functions.hpp"
namespace ut_aworx {

namespace {

struct Test1 { char c1; };
struct Test9 { integer i1; char c1; };
struct PADerived : public PoolAllocator
{

    static
    void testHookIndexFunc(AWorxUnitTesting& ut )
    {
        // test hook index calculation
        //              constexpr version                        runtime version

        #if ALIB_CPP_STANDARD >= 20 || !defined(_WIN32) // not a constexpr under windows
        #if ALIB_SIZEOF_INTEGER == 8
            static_assert(  0 == hookIndex< 1>(), "error");     UT_EQ(0, hookIndex( 1) )
            static_assert(  0 == hookIndex< 2>(), "error");     UT_EQ(0, hookIndex( 2) )
            static_assert(  0 == hookIndex< 7>(), "error");     UT_EQ(0, hookIndex( 7) )
            static_assert(  0 == hookIndex< 8>(), "error");     UT_EQ(0, hookIndex( 8) )
            static_assert(  1 == hookIndex< 9>(), "error");     UT_EQ(1, hookIndex( 9) )
            static_assert(  1 == hookIndex<15>(), "error");     UT_EQ(1, hookIndex(15) )
            static_assert(  1 == hookIndex<16>(), "error");     UT_EQ(1, hookIndex(16) )
            static_assert(  2 == hookIndex<17>(), "error");     UT_EQ(2, hookIndex(17) )
            static_assert(  2 == hookIndex<25>(), "error");     UT_EQ(2, hookIndex(25) )
            static_assert(  2 == hookIndex<32>(), "error");     UT_EQ(2, hookIndex(32) )
            static_assert(  3 == hookIndex<33>(), "error");     UT_EQ(3, hookIndex(33) )
            static_assert(  3 == hookIndex<56>(), "error");     UT_EQ(3, hookIndex(56) )
            static_assert(  3 == hookIndex<64>(), "error");     UT_EQ(3, hookIndex(64) )
            static_assert(  4 == hookIndex<65>(), "error");     UT_EQ(4, hookIndex(65) )
        #else
            static_assert(  0 == hookIndex< 1>(), "error");     UT_EQ(0, hookIndex( 1) )
            static_assert(  0 == hookIndex< 2>(), "error");     UT_EQ(0, hookIndex( 2) )
            static_assert(  1 == hookIndex< 7>(), "error");     UT_EQ(1, hookIndex( 7) )
            static_assert(  1 == hookIndex< 8>(), "error");     UT_EQ(1, hookIndex( 8) )
            static_assert(  2 == hookIndex< 9>(), "error");     UT_EQ(2, hookIndex( 9) )
            static_assert(  2 == hookIndex<15>(), "error");     UT_EQ(2, hookIndex(15) )
            static_assert(  2 == hookIndex<16>(), "error");     UT_EQ(2, hookIndex(16) )
            static_assert(  3 == hookIndex<17>(), "error");     UT_EQ(3, hookIndex(17) )
            static_assert(  3 == hookIndex<25>(), "error");     UT_EQ(3, hookIndex(25) )
            static_assert(  3 == hookIndex<32>(), "error");     UT_EQ(3, hookIndex(32) )
            static_assert(  4 == hookIndex<33>(), "error");     UT_EQ(4, hookIndex(33) )
            static_assert(  4 == hookIndex<56>(), "error");     UT_EQ(4, hookIndex(56) )
            static_assert(  4 == hookIndex<64>(), "error");     UT_EQ(4, hookIndex(64) )
            static_assert(  5 == hookIndex<65>(), "error");     UT_EQ(5, hookIndex(65) )
        #endif
#endif
    }
};

DOX_MARKER( [DOX_MONOMEM_ALLOCATOR_INTERFACE_TYPE] )
// sample type
struct MyType
{
    // the member
    alib::String member;

    // constructor
    MyType( const alib::String& pMember)
    : member(pMember)
    {}
};
DOX_MARKER( [DOX_MONOMEM_ALLOCATOR_INTERFACE_TYPE] )

DOX_MARKER( [DOX_MONOMEM_SMV_1] )
class Dictionary
{
  protected:
    // the only member: a hash map String->String
    HashMap< MonoAllocator,                  // allocator type to use
             String32, String32,             // key/value type
             std::hash    <String>,          // hash functor
             std::equal_to<String>,          // comparison functor
             lang::Caching::Enabled,         // store hash value of keys with entry
             Recycling::Private      >       // use local recycling
        map;  // compare functor

  public:
    // Constructor taking a monotonous allocator
    Dictionary(MonoAllocator& ma)    : map( ma )    {}

    // Inserts a translation.
    void Learn( const String& word, const String& translation )
    {
        map.EmplaceOrAssign( word, translation );
    }

    // Searches a translation. If not found, returns a nulled string.
    const String Translate( const String& word )
    {
        auto result=   map.Find( word );
        if ( result == map.end() )
            return NULL_STRING;

        return result.Mapped();
    }

};
DOX_MARKER( [DOX_MONOMEM_SMV_1] )

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunused-member-function"
#endif

DOX_MARKER( [DOX_MONOMEM_SMV_2] )
// 1. Derive SharedXyz class  from TSharedMonoVal, passing the original type as the template parameter
class SharedDictionary  : public alib::monomem::TSharedMonoVal<Dictionary, HeapAllocator, void>
{
  public:
    // 2. Constructor.
    SharedDictionary( size_t initialBufferSizeInKB )
    : TSharedMonoVal( initialBufferSizeInKB, 200 ) // Initializes parent TSharedMonoVal to hold 4Kb,
                                                   // doubling when a new buffer is needed.
    {
        // Constructing the contained type "Dictionary".
        // We use GetAllocator() to fetch the mono allocator of TSharedMonoVal and pass it to
        // the constructor. This way, the HashMap uses the same allocator that already holds
        // the allocator as well as the dictionary type itself!
        ConstructT( GetAllocator() );
    }

    // Default Constructor
    SharedDictionary()                       = default;

     // Constructor taking a nullptr, same as default constructor.
    SharedDictionary(std::nullptr_t)    noexcept : SharedDictionary() {}
};
DOX_MARKER( [DOX_MONOMEM_SMV_2] )


class DictionaryPool
{
  protected:
    PoolAllocator pool;
    HashMap< PoolAllocator,
             String32, String32,
             std::hash    <String>,
             std::equal_to<String>,
             lang::Caching::Enabled,
             Recycling::None      >
        map;  // compare functor


  public:
    DictionaryPool(MonoAllocator& ma)
    : pool( ma )
    , map( pool )
    {}
    
    void Learn( const String& word, const String& translation )
    { map.EmplaceOrAssign( word, translation );  }
    const String Translate( const String& word )
    {
        auto result=   map.Find( word );
        if ( result == map.end() )
            return NULL_STRING;

        return result.Mapped();
    }
};

class SharedDictionaryPool  : public alib::monomem::TSharedMonoVal<DictionaryPool, HeapAllocator, void>
{
  public:
    SharedDictionaryPool( size_t initialBufferSizeInKB )
    : TSharedMonoVal(initialBufferSizeInKB, 200 )
    {
        ConstructT( GetAllocator() );
    }

    SharedDictionaryPool()                       = default;

    SharedDictionaryPool(std::nullptr_t)    noexcept : SharedDictionaryPool() {}
};

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif


DOX_MARKER( [DOX_MONOMEM_PMR_BRIDGE_MONOMEM] )
// A bridge to allow ALib's MonoAllocator to work with std::pmr
class PmrMonoAllocator : public std::pmr::memory_resource
{
  protected:
    // Reference to the ALib MonoAllocator
    alib::MonoAllocator& monoAllocator;

    // Allocate memory using the underlying MonoAllocator
    void* do_allocate(std::size_t bytes, std::size_t alignment) override
    {
        return monoAllocator.allocate(bytes, alignment);
    }

    // Deallocate memory: With MonoAllocator, this is a no-op
    void do_deallocate(void* , std::size_t , std::size_t ) override    {}

    // Compare memory resources
    bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override
    {
        auto* cOther = dynamic_cast<const PmrMonoAllocator*>(&other);
        if ( cOther == nullptr )
            return false;
        
        return &monoAllocator == &cOther->monoAllocator;
    }

  public:
    // constructor
    PmrMonoAllocator(alib::MonoAllocator& ma)
    : monoAllocator(ma) {}
};
DOX_MARKER( [DOX_MONOMEM_PMR_BRIDGE_MONOMEM] )

}// anonymous namespace
#include "alib/lang/callerinfo_methods.hpp"

UT_CLASS

//--------------------------------------------------------------------------------------------------
//--- std::pmr Bridge
//--------------------------------------------------------------------------------------------------
UT_METHOD(PMRBridge)
{
    UT_INIT()
    {
DOX_MARKER( [DOX_MONOMEM_PMR_BRIDGE_MONOMEM_USAGE] )
// Create an ALib MonoAllocator
MonoAllocator monoAllocator(ALIB_DBG("Bridged",) 4);

// Bridge it into std::pmr using our PmrMonoAllocator class
PmrMonoAllocator pmrMonoAllocator(monoAllocator);

// Create a polymorphic allocator from the PmrMonoAllocator
std::pmr::polymorphic_allocator<int> pmrAlloc(&pmrMonoAllocator);

// Now, create a std::pmr::vector using the polymorphic allocator
std::pmr::vector<int> myVector(pmrAlloc);

// Use the vector as normal, while allocation go to MonoAllocator
myVector.push_back(42);
myVector.push_back(13);

for ( auto it : myVector )
    UT_PRINT("Vector contains: ", it )
DOX_MARKER( [DOX_MONOMEM_PMR_BRIDGE_MONOMEM_USAGE] )
    }
}

//--------------------------------------------------------------------------------------------------
//--- MonoAllocator
//--------------------------------------------------------------------------------------------------
UT_METHOD(TestMonoAllocator)
{
    UT_INIT()
    
{
DOX_MARKER( [DOX_MONOMEM_LOCK_GLOBALALLOCATOR] )
{ ALIB_LOCK_RECURSIVE_WITH( alib::monomem::GLOBAL_ALLOCATOR_LOCK )
    //...
    //... your code goes here
    //...
}
DOX_MARKER( [DOX_MONOMEM_LOCK_GLOBALALLOCATOR] )

DOX_MARKER( [DOX_MONOMEM_ALLOCATOR_INTERFACE] )
// Create an allocator (with 4kB initial buffer).
MonoAllocator allocator(ALIB_DBG("MyAllocator",) 4);

// Use operator->() on the allocator and invoke high-level interface
MyType* myObject= allocator().New<MyType>(A_CHAR("Hello"));

///...
///...

// Destruct the object and free the memory
allocator().Delete(myObject);
DOX_MARKER( [DOX_MONOMEM_ALLOCATOR_INTERFACE] )
}

{
#if !defined(_WIN32)
DOX_MARKER( [ALIB_MANUAL_T_HPP_FILE_USAGE3] )
// Create a pool allocator that uses heap allocation
PoolAllocatorHA myPool;
{
    // Create a mono allocator that receives its memory from the pool.
    // Its growth factor is 100%, which keeps each next allocation at
    // the initial size, which is a good strategy with an underlying pool!
    monomem::TMonoAllocator<PoolAllocatorHA> myMono(ALIB_DBG("MyPaMa",) myPool, 1, 100);

    // Allocate and construct an object.
    MyType* myObject= myMono().New<MyType>(A_CHAR("Hello"));

    ///...
    ///...

    // Destruct the object and free the memory
    myMono().Delete(myObject);
}// destruction of the mono allocator, which passes the memory buffer back to the pool
DOX_MARKER( [ALIB_MANUAL_T_HPP_FILE_USAGE3] )
#endif // !defined(_WIN32)
}
}

//--------------------------------------------------------------------------------------------------
//--- SharedPtr
//--------------------------------------------------------------------------------------------------
UT_METHOD(TestSharedVal)
{
    UT_INIT()

    // assert sizes of SharedVal, SharedPtr and TSharedMonoVal
    // (Note: A first implementation of SharedVal did not use EBO in case that the contained
    //        type T was derived from an empty lang::AllocMember. This was fixed, but therefore
    //        these tests once and for all!)
    static_assert( sizeof(AString)   <  sizeof(AStringMA) );
    static_assert( sizeof(AStringMA) == sizeof(AStringPA) );

    static_assert( SharedVal    <int      ,HeapAllocator>::SizeOfAllocation()  <  SharedVal    <int      ,MonoAllocator>::SizeOfAllocation() );
    static_assert( SharedVal    <integer  ,HeapAllocator>::SizeOfAllocation()  <  SharedVal    <AString  ,HeapAllocator>::SizeOfAllocation() );
    static_assert( SharedVal    <AString  ,HeapAllocator>::SizeOfAllocation()  <  SharedVal    <AString  ,MonoAllocator>::SizeOfAllocation() );
    static_assert( SharedVal    <AString  ,PoolAllocator>::SizeOfAllocation()  == SharedVal    <AString  ,MonoAllocator>::SizeOfAllocation() );
    static_assert( SharedVal    <AStringMA,HeapAllocator>::SizeOfAllocation()  <  SharedVal    <AStringMA,MonoAllocator>::SizeOfAllocation() );
    static_assert( SharedVal    <AStringMA,PoolAllocator>::SizeOfAllocation()  == SharedVal    <AStringMA,MonoAllocator>::SizeOfAllocation() );

    static_assert( SharedPtr    <int      ,HeapAllocator>::SizeOfAllocation()  <  SharedPtr    <int      ,MonoAllocator>::SizeOfAllocation() );
    static_assert( SharedPtr    <integer  ,HeapAllocator>::SizeOfAllocation()  <  SharedPtr    <AString  ,HeapAllocator>::SizeOfAllocation() );
    static_assert( SharedPtr    <AString  ,HeapAllocator>::SizeOfAllocation()  <  SharedPtr    <AString  ,MonoAllocator>::SizeOfAllocation() );
    static_assert( SharedPtr    <AString  ,PoolAllocator>::SizeOfAllocation()  == SharedPtr    <AString  ,MonoAllocator>::SizeOfAllocation() );
    static_assert( SharedPtr    <AStringMA,HeapAllocator>::SizeOfAllocation()  <  SharedPtr    <AStringMA,MonoAllocator>::SizeOfAllocation() );
    static_assert( SharedPtr    <AStringMA,PoolAllocator>::SizeOfAllocation()  == SharedPtr    <AStringMA,MonoAllocator>::SizeOfAllocation() );

    static_assert( TSharedMonoVal<int      ,HeapAllocator>::SizeOfAllocation()  <  TSharedMonoVal<int      ,MonoAllocator>::SizeOfAllocation() );
    static_assert( TSharedMonoVal<integer  ,HeapAllocator>::SizeOfAllocation()  <  TSharedMonoVal<AString  ,HeapAllocator>::SizeOfAllocation() );
    static_assert( TSharedMonoVal<AString  ,HeapAllocator>::SizeOfAllocation()  <  TSharedMonoVal<AString  ,MonoAllocator>::SizeOfAllocation() );
    static_assert( TSharedMonoVal<AString  ,PoolAllocator>::SizeOfAllocation()  == TSharedMonoVal<AString  ,MonoAllocator>::SizeOfAllocation() );
    static_assert( TSharedMonoVal<AStringMA,HeapAllocator>::SizeOfAllocation()  <  TSharedMonoVal<AStringMA,MonoAllocator>::SizeOfAllocation() );
    static_assert( TSharedMonoVal<AStringMA,PoolAllocator>::SizeOfAllocation()  == TSharedMonoVal<AStringMA,MonoAllocator>::SizeOfAllocation() );

    // some tests using heap allocator
    {
        using SPAS= SharedVal<AString>;

        SPAS sp1;
        static_assert( sizeof( SPAS ) == sizeof( void* ), "This is never false!" );
        UT_FALSE( sp1.Unique()        )
        UT_TRUE ( sp1.UseCount() == 0 )
        sp1= SPAS("TestString");

        UT_EQ( String(A_CHAR("TestString")), *sp1  )
        UT_TRUE ( sp1.Unique()        )
        UT_TRUE ( sp1.UseCount() == 1 )

        {
            SPAS sp2(sp1);
            UT_FALSE( sp1.Unique()        )
            UT_FALSE( sp2.Unique()        )
            UT_TRUE ( sp1.UseCount() == 2 )
            UT_TRUE ( sp2.UseCount() == 2 )
        }
        UT_TRUE ( sp1.Unique()        )
        UT_TRUE ( sp1.UseCount() == 1 )

        SPAS sp2= sp1;
        UT_FALSE( sp1.Unique()        )
        UT_FALSE( sp2.Unique()        )
        UT_TRUE ( sp2.UseCount() == 2  )
        UT_TRUE ( sp1.UseCount() == 2  )

        sp1= nullptr;
        UT_FALSE( sp1.Unique()        )
        UT_TRUE ( sp2.Unique()        )
        UT_TRUE ( sp1.UseCount() == 0  )
        UT_TRUE ( sp2.UseCount() == 1  )

        sp1= std::move( sp2 );
        UT_TRUE ( sp1.Unique()        )
        UT_FALSE( sp2.Unique()        )
        UT_TRUE ( sp1.UseCount() == 1  )
        UT_TRUE ( sp2.UseCount() == 0  )

        sp1= nullptr;
        UT_FALSE( sp1.Unique()        )
        UT_TRUE ( sp1.UseCount() == 0  )
    }

    #if ALIB_MONOMEM
    // same tests using PoolAllocator
    {
        using SPAS= SharedVal<AString, PoolAllocator>;

        MonoAllocator ma(ALIB_DBG("UTSharedVal",) 1);
        PoolAllocator pa(ma);

        SPAS sp1;
        static_assert( sizeof( SPAS ) == sizeof( void* ), "This is never false!" );
        UT_FALSE( sp1.Unique()        )
        UT_TRUE ( sp1.UseCount() == 0 )
        sp1= SPAS(pa, "TestString");

        UT_EQ( String(A_CHAR("TestString")), *sp1  )
        UT_TRUE ( sp1.Unique()        )
        UT_TRUE ( sp1.UseCount() == 1 )

        {
            SPAS sp2(sp1);
            UT_FALSE( sp1.Unique()        )
            UT_FALSE( sp2.Unique()        )
            UT_TRUE ( sp1.UseCount() == 2 )
            UT_TRUE ( sp2.UseCount() == 2 )
        }
        UT_TRUE ( sp1.Unique()        )
        UT_TRUE ( sp1.UseCount() == 1 )

        SPAS sp2= sp1;
        UT_FALSE( sp1.Unique()        )
        UT_FALSE( sp2.Unique()        )
        UT_TRUE ( sp2.UseCount() == 2  )
        UT_TRUE ( sp1.UseCount() == 2  )

        sp1= nullptr;
        UT_FALSE( sp1.Unique()        )
        UT_TRUE ( sp2.Unique()        )
        UT_TRUE ( sp1.UseCount() == 0  )
        UT_TRUE ( sp2.UseCount() == 1  )

        sp1= std::move( sp2 );
        UT_TRUE ( sp1.Unique()        )
        UT_FALSE( sp2.Unique()        )
        UT_TRUE ( sp1.UseCount() == 1  )
        UT_TRUE ( sp2.UseCount() == 0  )

        sp1= nullptr;
        UT_FALSE( sp1.Unique()        )
        UT_TRUE ( sp1.UseCount() == 0  )
    }
    #endif

}

//--------------------------------------------------------------------------------------------------
//--- TSharedMonoVal
//--------------------------------------------------------------------------------------------------
UT_METHOD(TestSharedMonoVal)
{
    UT_INIT()
{
// ---- creating non-TSharedMonoVal ------
{
                                                    DOX_MARKER( [DOX_MONOMEM_SMV_11] )
MonoAllocator allocator      ( ALIB_DBG("Dictionary",) 4 );
Dictionary    germanToEnglish( allocator );
                                                    DOX_MARKER( [DOX_MONOMEM_SMV_11] )
}
{
// ---- creating a TSharedMonoVal ------
                                                    DOX_MARKER( [DOX_MONOMEM_SMV_3] )
// Create an instance of the self-contained, automatic dictionary type
SharedDictionary germanEnglish(4);

// Interestingly, the size of class SharedDictionary is that of a single pointer
static_assert( sizeof( SharedDictionary ) == sizeof( void* ), "This is never false!" );
                                                    DOX_MARKER( [DOX_MONOMEM_SMV_3] )

                                                    DOX_MARKER( [DOX_MONOMEM_SMV_4] )
// Add vocabulary. We need to use operator->() to access the contained type
germanEnglish->Learn( A_CHAR( "Spass"        )  , A_CHAR( "fun"          ) );
germanEnglish->Learn( A_CHAR( "Kindergarten" )  , A_CHAR( "kindergarten" ) );

// Search a word
UT_EQ( A_CHAR("fun"), germanEnglish->Translate( A_CHAR( "Spass") )  )
                                                    DOX_MARKER( [DOX_MONOMEM_SMV_4] )

                                                    DOX_MARKER( [DOX_MONOMEM_SMV_RESET] )
// Resetting the dictionary
germanEnglish.Reset( germanEnglish.GetAllocator() );

// All is forgotten
UT_EQ( NULL_STRING, germanEnglish->Translate( A_CHAR( "Spass") )  )
                                                    DOX_MARKER( [DOX_MONOMEM_SMV_RESET] )

                                                    DOX_MARKER( [DOX_MONOMEM_SMV_SHARED] )
// Our dictionary is still unique, and has a share-counter of 1
UT_TRUE( germanEnglish.Unique()        )
UT_TRUE( germanEnglish.UseCount() == 1 )

// A default constructed dictionary is empty
SharedDictionary other;
UT_TRUE( other.UseCount() == 0 )

//  Copying the type
other= germanEnglish;
UT_TRUE ( other        .UseCount() == 2  )
UT_TRUE ( germanEnglish.UseCount() == 2  )

//  Nulling the original
germanEnglish= nullptr;
UT_TRUE ( other        .UseCount() == 1  )
UT_TRUE ( germanEnglish.UseCount() == 0  )

//  Moving the copy to the original
germanEnglish= std::move( other );
UT_TRUE ( other        .UseCount() == 0  )
UT_TRUE ( germanEnglish.UseCount() == 1  )

// Nulling the now last instance: this destructs the dictionary, the mono allocator
// and frees all memory.
germanEnglish= nullptr;
                                                    DOX_MARKER( [DOX_MONOMEM_SMV_SHARED] )
}
}


// -------------------------------------------------------------------------------------------------
// We repeat this exercise using a different chained allocator. This is mainly done to find
// compilation issues and memory leaks
// -------------------------------------------------------------------------------------------------
    {
        MonoAllocator     ma      ( ALIB_DBG("UTChained",)  4 );
        DictionaryPool    germanToEnglish( ma );
    }
    {
        SharedDictionaryPool germanEnglish(4);
        static_assert( sizeof( SharedDictionary ) == sizeof( void* ), "This is never false!" );
        germanEnglish->Learn( A_CHAR( "Spass"        )  , A_CHAR( "fun"          ) );
        germanEnglish->Learn( A_CHAR( "Kindergarten" )  , A_CHAR( "kindergarten" ) );
        UT_EQ( A_CHAR("fun"), germanEnglish->Translate( A_CHAR( "Spass") )  )
        germanEnglish.Reset( germanEnglish.GetAllocator() );
        UT_EQ( NULL_STRING, germanEnglish->Translate( A_CHAR( "Spass") )  )
        UT_TRUE( germanEnglish.Unique()        )
        UT_TRUE( germanEnglish.UseCount() == 1 )
        SharedDictionaryPool other;
        UT_TRUE( other.UseCount() == 0 )
        other= germanEnglish;
        UT_TRUE ( other        .UseCount() == 2  )
        UT_TRUE ( germanEnglish.UseCount() == 2  )
        germanEnglish= nullptr;
        UT_TRUE ( other        .UseCount() == 1  )
        UT_TRUE ( germanEnglish.UseCount() == 0  )
        germanEnglish= std::move( other );
        UT_TRUE ( other        .UseCount() == 0  )
        UT_TRUE ( germanEnglish.UseCount() == 1  )
        germanEnglish= nullptr;
    }
}

//--------------------------------------------------------------------------------------------------
//--- PoolAllocator
//--------------------------------------------------------------------------------------------------
UT_METHOD(TestPoolAllocator)
{
    UT_INIT()

    UT_PRINT("") UT_PRINT( "### PoolAllocator ###")

    PADerived::testHookIndexFunc(ut);

    MonoAllocator  ma(ALIB_DBG("UTPoolAllocator",) 1);
    PoolAllocator mr(ma);


    // basic tests
    {
        auto* o1= mr().New<Test1>();            // allocate 3 objects
        auto* o2= mr().New<Test1>();
        auto* o3= mr().New<Test1>();

        auto* oldVal= o1;                       // Delete one and get it back, check for same
        mr().Delete(o1);
        o1= mr().New<Test1>();
        UT_EQ(oldVal, o1)

               oldVal= o1;                      // Delete 2 and get them back, check for same (reversed order)
        auto* oldVal2= o2;
        mr().Delete(o1);
        mr().Delete(o2);
        o1= mr().New<Test1>();
        o2= mr().New<Test1>();
        UT_EQ(oldVal2, o1)
        UT_EQ(oldVal , o2)
        auto* o4= mr().New<Test1>();            // allocate 4th object and assert that they are different to the first 3
        UT_TRUE(   o4 != o1
                && o4 != o2
                && o4 != o3 )

        mr().Delete(o1);                        // Delete 2 and get one back, check that this is the least recent deleted
        mr().Delete(o2);
        o1= mr().New<Test1>();
        UT_EQ(o1 , o2)
    }

    // same as above, with a different type
    {
        auto* o1= mr().New<Test9>();
        auto* o2= mr().New<Test9>();
        auto* o3= mr().New<Test9>();

        auto* oldVal= o1;
        mr().Delete(o1);
        o1= mr().New<Test9>();
        UT_EQ(oldVal, o1)

               oldVal= o1;
        auto* oldVal2= o2;
        mr().Delete(o1);
        mr().Delete(o2);
        o1= mr().New<Test9>();
        o2= mr().New<Test9>();
        UT_EQ(oldVal2, o1)
        UT_EQ(oldVal , o2)
        auto* o4= mr().New<Test9>();
        UT_TRUE(o4 != o1
                && o4 != o2
                && o4 != o3 )

        mr().Delete(o1);
        mr().Delete(o2);
        o1= mr().New<Test9>();
        UT_EQ(o1 , o2)
    }

    UT_PRINT("...done")
}

//--------------------------------------------------------------------------------------------------
//--- AStrings using mono/pool allocator
//--------------------------------------------------------------------------------------------------
UT_METHOD(AString_Ma_AStringPA)
{
    UT_INIT()
    // this is rather just for testing if every string is included in the library
    {
        MonoAllocator   ma(ALIB_DBG("UTPaAs",) 4);
        PoolAllocator   pa(ma);

        NAStringMA            nAStringMA         (ma);
        NAStringPA            nAStringPA         (pa);
        WAStringMA            wAStringMA         (ma);
        WAStringPA            wAStringPA         (pa);
        XAStringMA            xAStringMA         (ma);
        XAStringPA            xAStringPA         (pa);
        AStringMA             aStringMA          (ma);
        AStringPA             aStringAStringPA   (pa);
        ComplementAStringMA   complementAStringMA(ma);
        ComplementAStringPA   complementAStringPA(pa);
        StrangeAStringMA      strangeAStringMA   (ma);
        StrangeAStringPA      strangeAStringPA   (pa);

        nAStringMA         << A_NCHAR( "abc" )<< A_WCHAR( "def" )<< A_XCHAR( "ghi" );  UT_TRUE( nAStringMA         . Equals( A_NCHAR("abcdefghi") ) )
        nAStringPA         << A_NCHAR( "abc" )<< A_WCHAR( "def" )<< A_XCHAR( "ghi" );  UT_TRUE( nAStringPA         . Equals( A_NCHAR("abcdefghi") ) )
        wAStringMA         << A_NCHAR( "abc" )<< A_WCHAR( "def" )<< A_XCHAR( "ghi" );  UT_TRUE( wAStringMA         . Equals( A_WCHAR("abcdefghi") ) )
        wAStringPA         << A_NCHAR( "abc" )<< A_WCHAR( "def" )<< A_XCHAR( "ghi" );  UT_TRUE( wAStringPA         . Equals( A_WCHAR("abcdefghi") ) )
        xAStringMA         << A_NCHAR( "abc" )<< A_WCHAR( "def" )<< A_XCHAR( "ghi" );  UT_TRUE( xAStringMA         . Equals( A_XCHAR("abcdefghi") ) )
        xAStringPA         << A_NCHAR( "abc" )<< A_WCHAR( "def" )<< A_XCHAR( "ghi" );  UT_TRUE( xAStringPA         . Equals( A_XCHAR("abcdefghi") ) )
        aStringMA          << A_NCHAR( "abc" )<< A_WCHAR( "def" )<< A_XCHAR( "ghi" );  UT_TRUE( aStringMA          . Equals( A_CHAR ("abcdefghi") ) )
        aStringAStringPA   << A_NCHAR( "abc" )<< A_WCHAR( "def" )<< A_XCHAR( "ghi" );  UT_TRUE( aStringAStringPA   . Equals( A_CHAR ("abcdefghi") ) )
        complementAStringMA<< A_NCHAR( "abc" )<< A_WCHAR( "def" )<< A_XCHAR( "ghi" );  UT_TRUE( complementAStringMA. Equals( A_CCHAR("abcdefghi") ) )
        complementAStringPA<< A_NCHAR( "abc" )<< A_WCHAR( "def" )<< A_XCHAR( "ghi" );  UT_TRUE( complementAStringPA. Equals( A_CCHAR("abcdefghi") ) )
        strangeAStringMA   << A_NCHAR( "abc" )<< A_WCHAR( "def" )<< A_XCHAR( "ghi" );  UT_TRUE( strangeAStringMA   . Equals( A_XCHAR("abcdefghi") ) )
        strangeAStringPA   << A_NCHAR( "abc" )<< A_WCHAR( "def" )<< A_XCHAR( "ghi" );  UT_TRUE( strangeAStringPA   . Equals( A_XCHAR("abcdefghi") ) )

        NString           nString           = nAStringMA           ;
        NString           nString2          = nAStringPA           ;
        WString           wString           = wAStringMA           ;
        WString           wString2          = wAStringPA           ;
        XString           xString           = xAStringMA           ;
        XString           xString2          = xAStringPA           ;
        String            string            = aStringMA            ;
        String            string2           = aStringAStringPA     ;
        ComplementString  complementString  = complementAStringMA  ;
        ComplementString  complementString2 = complementAStringPA  ;
        StrangeString     strangeString     = strangeAStringMA     ;
        StrangeString     strangeString2    = strangeAStringPA     ;


        UT_TRUE( nString           == nAStringMA          )
        UT_TRUE( nString2          == nAStringPA          )
        UT_TRUE( wString           == wAStringMA          )
        UT_TRUE( wString2          == wAStringPA          )
        UT_TRUE( xString           == xAStringMA          )
        UT_TRUE( xString2          == xAStringPA          )
        UT_TRUE( string            == aStringMA           )
        UT_TRUE( string2           == aStringAStringPA    )
        UT_TRUE( complementString  == complementAStringMA )
        UT_TRUE( complementString2 == complementAStringPA )
        UT_TRUE( strangeString     == strangeAStringMA    )
        UT_TRUE( strangeString2    == strangeAStringPA    )

        UT_TRUE( nAStringMA          .Reset(lang::Inclusion::Exclude).Equals( A_NCHAR("Exclude") ) )
        UT_TRUE( nAStringPA          .Reset(lang::Inclusion::Exclude).Equals( A_NCHAR("Exclude") ) )
        UT_TRUE( wAStringMA          .Reset(lang::Inclusion::Exclude).Equals( A_WCHAR("Exclude") ) )
        UT_TRUE( wAStringPA          .Reset(lang::Inclusion::Exclude).Equals( A_WCHAR("Exclude") ) )
        UT_TRUE( xAStringMA          .Reset(lang::Inclusion::Exclude).Equals( A_XCHAR("Exclude") ) )
        UT_TRUE( xAStringPA          .Reset(lang::Inclusion::Exclude).Equals( A_XCHAR("Exclude") ) )
        UT_TRUE( aStringMA           .Reset(lang::Inclusion::Exclude).Equals( A_CHAR ("Exclude") ) )
        UT_TRUE( aStringAStringPA    .Reset(lang::Inclusion::Exclude).Equals( A_CHAR ("Exclude") ) )
        UT_TRUE( complementAStringMA .Reset(lang::Inclusion::Exclude).Equals( A_CCHAR("Exclude") ) )
        UT_TRUE( complementAStringPA .Reset(lang::Inclusion::Exclude).Equals( A_CCHAR("Exclude") ) )
        UT_TRUE( strangeAStringMA    .Reset(lang::Inclusion::Exclude).Equals( A_XCHAR("Exclude") ) )
        UT_TRUE( strangeAStringPA    .Reset(lang::Inclusion::Exclude).Equals( A_XCHAR("Exclude") ) )

        // test LengthResetter (mostly if each version is declared rightfully)
        {
            NStringMALengthResetter              nAStringMALR             (nAStringMA);
            NStringPALengthResetter              nAStringPALR             (nAStringPA);
            WStringMALengthResetter              wAStringMALR             (wAStringMA);
            WStringPALengthResetter              wAStringPALR             (wAStringPA);
            XStringMALengthResetter              xAStringMALR             (xAStringMA);
            XStringPALengthResetter              xAStringPALR             (xAStringPA);
            StringMALengthResetter               aStringMALR              (aStringMA);
            StringPALengthResetter               aStringAStringPALR       (aStringAStringPA);
            ComplementStringMALengthResetter     complementAStringMALR    (complementAStringMA);
            ComplementStringPALengthResetter     complementAStringPALR    (complementAStringPA);
            StrangeStringMALengthResetter        strangeAStringMALR       (strangeAStringMA);
            StrangeStringPALengthResetter        strangeAStringPALR       (strangeAStringPA);

            nAStringMA          <<  "Added"; // using narrow strings 
            nAStringPA          <<  "Added";
            wAStringMA          <<  "Added";
            wAStringPA          <<  "Added";
            xAStringMA          <<  "Added";
            xAStringPA          <<  "Added";
            aStringMA           <<  "Added";
            aStringAStringPA    <<  "Added";
            complementAStringMA <<  "Added";
            complementAStringPA <<  "Added";
            strangeAStringMA    <<  "Added";
            strangeAStringPA    <<  "Added";

            UT_TRUE( nAStringMA          .Equals( A_NCHAR("ExcludeAdded") ) )
            UT_TRUE( nAStringPA          .Equals( A_NCHAR("ExcludeAdded") ) )
            UT_TRUE( wAStringMA          .Equals( A_WCHAR("ExcludeAdded") ) )
            UT_TRUE( wAStringPA          .Equals( A_WCHAR("ExcludeAdded") ) )
            UT_TRUE( xAStringMA          .Equals( A_XCHAR("ExcludeAdded") ) )
            UT_TRUE( xAStringPA          .Equals( A_XCHAR("ExcludeAdded") ) )
            UT_TRUE( aStringMA           .Equals( A_CHAR ("ExcludeAdded") ) )
            UT_TRUE( aStringAStringPA    .Equals( A_CHAR ("ExcludeAdded") ) )
            UT_TRUE( complementAStringMA .Equals( A_CCHAR("ExcludeAdded") ) )
            UT_TRUE( complementAStringPA .Equals( A_CCHAR("ExcludeAdded") ) )
            UT_TRUE( strangeAStringMA    .Equals( A_XCHAR("ExcludeAdded") ) )
            UT_TRUE( strangeAStringPA    .Equals( A_XCHAR("ExcludeAdded") ) )
        }

        UT_TRUE( nAStringMA          .Equals( A_NCHAR("Exclude") ) )
        UT_TRUE( nAStringPA          .Equals( A_NCHAR("Exclude") ) )
        UT_TRUE( wAStringMA          .Equals( A_WCHAR("Exclude") ) )
        UT_TRUE( wAStringPA          .Equals( A_WCHAR("Exclude") ) )
        UT_TRUE( xAStringMA          .Equals( A_XCHAR("Exclude") ) )
        UT_TRUE( xAStringPA          .Equals( A_XCHAR("Exclude") ) )
        UT_TRUE( aStringMA           .Equals( A_CHAR ("Exclude") ) )
        UT_TRUE( aStringAStringPA    .Equals( A_CHAR ("Exclude") ) )
        UT_TRUE( complementAStringMA .Equals( A_CCHAR("Exclude") ) )
        UT_TRUE( complementAStringPA .Equals( A_CCHAR("Exclude") ) )
        UT_TRUE( strangeAStringMA    .Equals( A_XCHAR("Exclude") ) )
        UT_TRUE( strangeAStringPA    .Equals( A_XCHAR("Exclude") ) )
    }


    // test if realloc works with MonoAllocator
    #if !ALIB_DEBUG_STRINGS
     {
        MonoAllocator ma(ALIB_DBG("UTAsMa",) 1);
        NAStringMA as(ma);
        nchar* buf= as.VBuffer();                     UT_TRUE (nullptr==  as.Buffer())
        as.InsertChars('A', 5); buf= as.VBuffer();
        as.InsertChars('A', 5);                       UT_TRUE (buf    ==  as.Buffer())
        as.InsertChars('A', 300);                     UT_TRUE (buf    ==  as.Buffer())
        UT_PRINT("One warning should follow: Allocation Size exceeds 1/2 of the current buffer size" )
        as.InsertChars('A', 300);                     UT_TRUE (buf    ==  as.Buffer())
        UT_PRINT("Two warnings should follow: Allocation Size exceeds current buffer size" )
        as.InsertChars('A', 300);                     UT_FALSE(buf    ==  as.Buffer())
    }
    #endif

    // test capacity of pool-allocated AString
    #if !ALIB_DEBUG_STRINGS
    {
        MonoAllocator ma(ALIB_DBG("UTAsPa",) 1);
        PoolAllocator pool(ma);
        NAStringPA asPA(pool);

        asPA << "Hello";
        Log_Info("AStringPA: {:2}/{:2} {!Q}", asPA.Length(), asPA.Capacity(), asPA )

        asPA.Reset("World");                 UT_EQ(  5, asPA.Length())  UT_EQ(  15, asPA.Capacity())
        asPA.Reset( "12345678");             UT_EQ(  8, asPA.Length())  UT_EQ(  15, asPA.Capacity())
        asPA.Reset( "123456789");            UT_EQ(  9, asPA.Length())  UT_EQ(  15, asPA.Capacity())
        asPA.Reset( "123456789012345");      UT_EQ( 15, asPA.Length())  UT_EQ(  15, asPA.Capacity())
        asPA.Terminate();                    UT_EQ( 15, asPA.Length())  UT_EQ(  15, asPA.Capacity())
        asPA.Reset( "1234567890123456");     UT_EQ( 16, asPA.Length())  UT_EQ(  31, asPA.Capacity())
        asPA.Terminate();                    UT_EQ( 16, asPA.Length())  UT_EQ(  31, asPA.Capacity())
        asPA.Reset( "12345678901234567");    UT_EQ( 17, asPA.Length())  UT_EQ(  31, asPA.Capacity())
        asPA.Reset( "1234567890123456");     UT_EQ( 16, asPA.Length())  UT_EQ(  31, asPA.Capacity())
        asPA.Reset( "World");                UT_EQ( 5 , asPA.Length())  UT_EQ(  31, asPA.Capacity())
    }
    #endif
}

//--------------------------------------------------------------------------------------------------
//--- LocalAllocator
//--------------------------------------------------------------------------------------------------
UT_METHOD(LocalAllocator)
{
    UT_INIT()

    UT_PRINT("") UT_PRINT( "### LocalAllocator ###")

    // destruction with only the internal buffer
    {
        LocalAllocator1K la;
        AStringMA aStringMa(la.AsMonoAllocator());
        aStringMa<< A_CHAR("hello");
        UT_TRUE(aStringMa == A_CHAR("hello"))
        la().Alloc<int>();
    }

    // destruction with a second buffer
    {
        LocalAllocator1K la;
        AStringMA aStringMa(la.AsMonoAllocator());
        UT_PRINT("One warning should follow: Allocation Size exceeds 1/2 of current buffer size" )
        la().AllocArray<char>(600);
        UT_PRINT("One warning should follow: Allocation Size exceeds 1/2 of current buffer size" )
        la().AllocArray<char>(600);  //second buffer allocated
    }

    // destruction with a second buffer, which is a special-size buffer and the currently
    // used buffer is remained
    {
        LocalAllocator1K la;
        static_assert( sizeof(LocalAllocator1K) == 1024 + sizeof(MonoAllocator),
          "LocalAllocator1K should have the same size as its buffer size plus a MonoAllocator" );

        AStringMA aStringMa(la.AsMonoAllocator());
        UT_PRINT("One warning should follow: Allocation Size exceeds 1/2 of current buffer size" )
        la().AllocArray<char>(600);
        UT_PRINT("Two warnings should follow: Allocation Size exceeds current buffer size" )
        la().AllocArray<char>(3000);
        #if ALIB_DEBUG_MONOMEM
           UT_EQ(size_t(1), la.DbgGetStatistics().QtyBufferSizeExceeds )
        #endif
    }

    // destruction with two additional buffers, while the local remains used.
    {
        LocalAllocator1K la;
        AStringMA aStringMa(la.AsMonoAllocator());
        UT_PRINT("One warning should follow: Allocation Size exceeds 1/2 of current buffer size" )
        la().AllocArray<char>(600);
        UT_PRINT("Two warnings should follow: Allocation Size exceeds current buffer size" )
        la().AllocArray<char>(2500);
        UT_PRINT("Two warnings should follow: Allocation Size exceeds current buffer size" )
        la().AllocArray<char>(2500);
        #if ALIB_DEBUG_MONOMEM
           UT_EQ(size_t(2), la.DbgGetStatistics().QtyBufferSizeExceeds )
        #endif
    }

    // destruction after a resets and reuse
    {
        LocalAllocator1K la;
        AStringMA aStringMa(la.AsMonoAllocator());
        UT_PRINT("One warning should follow: Allocation Size exceeds 1/2 of current buffer size" )
        la().AllocArray<char>(600);
        UT_PRINT("One warning should follow: Allocation Size exceeds 1/2 of current buffer size" )
        la().AllocArray<char>(600);  //second buffer allocated
        UT_PRINT("One warning should follow: Allocation Size exceeds 1/2 of current buffer size" )
        la().AllocArray<char>(600);
        la().AllocArray<char>(600);  //third buffer buffer allocated
        la.Reset();
    }

    // destruction with a second buffer, which is a special-size buffer and the currently
    // used buffer is remained...and a reset
    {
        LocalAllocator1K la;
        AStringMA aStringMa(la.AsMonoAllocator());
        UT_PRINT("One warning should follow: Allocation Size exceeds 1/2 of the current buffer size" )
        la().AllocArray<char>(600);
        UT_PRINT("Two warnings should follow: Allocation Size exceeds current buffer size" )
        la().AllocArray<char>(2500);
        #if ALIB_DEBUG_MONOMEM
           UT_EQ(size_t(1), la.DbgGetStatistics().QtyBufferSizeExceeds )
        #endif
        la.Reset();
    }

    {
        LocalAllocator1K la;
        AStringMA aStringMa(la.AsMonoAllocator());
        UT_PRINT("One warning should follow: Allocation Size exceeds 1/2 of the current buffer size" )
        la().AllocArray<char>(600);
        UT_PRINT("One warning should follow: Allocation Size exceeds 1/2 of the current buffer size" )
        la().AllocArray<char>(600);  //second buffer allocated
        UT_PRINT("One warning should follow: Allocation Size exceeds 1/2 of the current buffer size" )
        la().AllocArray<char>(600);
        la().AllocArray<char>(600);  //third buffer buffer allocated
        la.Reset();
        UT_PRINT("One warning should follow: Allocation Size exceeds 1/2 of the current buffer size" )
        la().AllocArray<char>(600);
        UT_PRINT("One warning should follow: Allocation Size exceeds 1/2 of the current buffer size" )
        la().AllocArray<char>(600);  //second buffer allocated
    }


    // we cannot do the following within the unit tests project, which imports the ALIB_DLL.
    // It has to be done in an external lib.
    #if !defined(_WIN32)
    // Use local allocator with underlying pool
    {
        // this is more or less a compilation test.
        // Works because at the beginning of this file we defined a MonoAllocator that uses
        // a pool
        PoolAllocatorHA myPool;
        monomem::TLocalAllocator<1, PoolAllocatorHA> myLocalMono(myPool, 100);

        MyType* myObject= myLocalMono().New<MyType>(A_CHAR("Hello"));
        myLocalMono().Delete(myObject);
    }
    #endif //!defined(_WIN32)

}

#include "unittests/aworx_unittests_end.hpp"

} //namespace [ut_aworx]

#endif // ALIB_UT_TIME
