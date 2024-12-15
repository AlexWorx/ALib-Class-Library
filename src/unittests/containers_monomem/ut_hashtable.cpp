// #################################################################################################
//  AWorx ALib Unit Tests
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"
#include "unittests/alib_test_selection.hpp"
#if ALIB_UT_MONOMEM

#include "alib/containers/list.hpp"
#include "alib/monomem/sharedmonoval.hpp"
#include "alib/containers/hashtable.hpp"
#include "alib/lang/resources/localresourcepool.hpp"
#include "alib/lang/basecamp/basecamp.hpp"
#include "alib/lang/rttrallocator.hpp"
#include "alib/compatibility/std_strings_functional.hpp"

#include <vector>
#include <unordered_map>
#include <map>
#include <list>
#include <utility>
#include <assert.h>

#define TESTCLASSNAME       UT_ContMonoHashtable
#include "unittests/aworx_unittests.hpp"

#if defined(__clang__)
    #pragma clang diagnostic ignored "-Wunused-function"
#elif defined(__GNUC__)
    #pragma GCC diagnostic ignored "-Wunused-function"
#elif defined(_MSC_VER)

#endif

using namespace std;
using namespace alib;

namespace ut_aworx
{
    namespace
    {

#if !defined(ALIB_UT_REDUCED_COMPILE_TIME)

#include "alib/lang/callerinfo_methods.hpp"

// ########################################################################################
// HashTable helpers
// ########################################################################################
struct DynInt
{
    int* value;
    static integer instCounter;

    DynInt()       { value= new int(0); ++instCounter;}
    DynInt( int v) { value= new int(v); ++instCounter;}

    DynInt( const DynInt&  copy) { value= new int(*copy.value); ++instCounter;}
    DynInt(       DynInt&& move) { value= move.value; move.value= nullptr; ++instCounter;}
    ~DynInt()
    {
        --instCounter;
        ALIB_ASSERT_ERROR( value != reinterpret_cast<int*>(64),
                           "Doubly destructed object!"      )
        if(value) delete value;
        value= reinterpret_cast<int*>(64);
    }

    DynInt& operator=( const DynInt&  copy) { *value= *copy.value; return *this; }
    DynInt& operator=(       DynInt&& move) { if(this != &move) { delete value; value= move.value; move.value= nullptr; } return *this; }
};
integer DynInt::instCounter= 0;


struct DynIntHash
{
    size_t operator()( const DynInt& dynInt ) {return static_cast<size_t >(*dynInt.value); }
};

// needed for MS compiler: otherwise some dubious hash value is generated for type <int>
// which is ok in general, but not for the unit tests, which rely on identity-hashing of ints.
struct IntHash
{
    size_t operator()( int value ) {return static_cast<size_t >(value); }
};

struct DynIntEqual
{
    bool operator()( const DynInt& lhs,
                     const DynInt& rhs  ) {return *lhs.value == *rhs.value; }
};

bool operator==( const DynInt& lhs, const int     rhs) { return *lhs.value ==  rhs;       }
bool operator!=( const DynInt& lhs, const int     rhs) { return *lhs.value !=  rhs;       }
bool operator==( const int     lhs, const DynInt& rhs) { return  lhs       == *rhs.value; }
bool operator!=( const int     lhs, const DynInt& rhs) { return  lhs       != *rhs.value; }
bool operator==( const DynInt& lhs, const DynInt& rhs) { return *lhs.value == *rhs.value; }
bool operator!=( const DynInt& lhs, const DynInt& rhs) { return *lhs.value != *rhs.value; }

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& s, const DynInt& v) { return s << *v.value; }

#include "alib/lang/callerinfo_functions.hpp"

template<typename THashTable>
void testConsistency( AWorxUnitTesting&  ut, THashTable& ht,
                      integer expSize, integer expBucketCount, integer expRecyclables   )

{
    if( expBucketCount == 0 )
    {
        expBucketCount= 1;
    }

    if( expSize         != -1 ) { UT_EQ(                       expSize                , ht.Size() )  }
    if( expBucketCount  != -1 ) { UT_EQ( static_cast<uinteger>(expBucketCount)        , ht.BucketCount() )  }
    if( expRecyclables  != -1 ) { UT_EQ( THashTable::IsRecycling() ? expRecyclables : 0 , ht.RecyclablesCount() )  }

    if(expSize != -1 )
    {
        // count using normal iterator
        integer countedSize= 0;
        for( auto mapPair : ht )
        {
            ++countedSize;
            (void) mapPair;
        }

        UT_EQ( expSize, countedSize )


        // count using bucket iterator
        countedSize= 0;
        for( uinteger i= 0 ; i < ht.BucketCount() ; ++i )
        {
            for( auto it= ht.begin( i ); it != ht.end( i ); ++it )
            {
                ++countedSize;
                (void) it;
            }
        }
        UT_EQ( expSize, countedSize )
    }
}

template<typename THashMap>
void testBucket( AWorxUnitTesting&  ut,
                 THashMap& map, uinteger bucketNumber, integer expSize )

{
    integer size= 0;
    for( auto bucketIt= map.begin(bucketNumber) ; bucketIt!= map.end(bucketNumber); ++bucketIt )
        ++size;

    UT_EQ(expSize, size )
}

// ########################################################################################
// HashSet
// ########################################################################################
template<typename TTable>
void UT_HashSet(AWorxUnitTesting&  ut, TTable& hashSet)
{
                                         testConsistency(ut, hashSet,     0 ,   0,  0 );

    hashSet.Emplace( 1 );                testConsistency(ut, hashSet,     1 ,  97,  0 );
                                         UT_EQ( 1l, hashSet.Erase  ( 1  ) )
                                         testConsistency(ut, hashSet,     0 ,  97,  1 );
    auto it1= hashSet.Emplace( 1 );      testConsistency(ut, hashSet,     1 ,  97,  0 );
                                         UT_EQ( it1.Value(), 1 )
    auto it2= hashSet.Emplace( 1 );      testConsistency(ut, hashSet,     2 ,  97,  0 );
                                         UT_EQ( it2.Value(), 1 )
    ++it2;                               UT_TRUE( it1 == it2 )

    UT_EQ( 2l, hashSet.Erase  ( 1  ) )   testConsistency(ut, hashSet,     0 ,  97,  2 );

    {
        hashSet.Emplace( 1 );            testConsistency(ut, hashSet,     1 ,  97,  1 );
        hashSet.Emplace( 2 );            testConsistency(ut, hashSet,     2 ,  97,  0 );
        auto
        it= hashSet.Find( 1 );           UT_FALSE( it == hashSet.end() )     UT_EQ(1, it.Value())
        it= hashSet.Find( 2 );           UT_FALSE( it == hashSet.end() )     UT_EQ(2, *it       )
        it= hashSet.Find( 3 );           UT_TRUE(  it == hashSet.end() )

    }

    // automatic rehash
    {
        hashSet.Clear();                  testConsistency(ut, hashSet,     0 ,  97,  2 );
        auto inserts= static_cast<int>(   hashSet.MaxLoadFactor() * static_cast<float>( hashSet.BucketCount() )
                                        - static_cast<float>(hashSet.Size())                                    );
        for( int i= 0 ; i < inserts -1 ; ++i )
            hashSet.Emplace( 5000 + rand() % 100  );

        auto oldBucketCount=  hashSet.BucketCount();
        hashSet.Emplace( 1000 );
        UT_TRUE( oldBucketCount < hashSet.BucketCount() )

        testConsistency(ut, hashSet,   hashSet.Size(), 199, -1 );
    }

    // EqualRange
    hashSet.Reset();
    auto cntRecylables= hashSet.RecyclablesCount();
    {
        hashSet.Emplace(  0 );            testConsistency(ut, hashSet,     1 ,  97,  cntRecylables );
        hashSet.Emplace(  0 );
        hashSet.Emplace( 97 );
        hashSet.Emplace( 97 );

        hashSet.Emplace(  1 );
        hashSet.Emplace( 12 );            testConsistency(ut, hashSet,     6 ,  97,  cntRecylables );

        auto
        result= hashSet.EqualRange(  0 ); UT_EQ(     0, result.first.Value() )
        ++result.first;                   UT_EQ(     0, result.first.Value() )
        ++result.first;                   UT_FALSE(  0  == result.first.Value() )
                                          UT_TRUE (  result.first == result.second )
        result= hashSet.EqualRange(  1 ); UT_EQ(     1, result.first.Value() )
        ++result.first;                   UT_TRUE (  result.first == result.second )
        result= hashSet.EqualRange( 97 ); UT_EQ(    97, result.first.Value() )
        ++result.first;                   UT_EQ(    97, result.first.Value() )
        ++result.first;                   UT_FALSE( 97  == result.first.Value() )
                                          UT_TRUE (  result.first == result.second )
        result= hashSet.EqualRange( 12 ); UT_EQ(    12, result.first.Value() )
        ++result.first;                   UT_TRUE (  result.first == result.second )

    }

    // Erase( pos )
    {

        hashSet.Clear();
        cntRecylables= hashSet.RecyclablesCount();
                                          testConsistency(ut, hashSet,     0 ,  97,  cntRecylables );
        hashSet.Emplace( 0);
        hashSet.Emplace( 0);
        hashSet.Emplace(97);
        hashSet.Emplace( 1);
        hashSet.Emplace(98);              testConsistency(ut, hashSet,     5 ,  97,  cntRecylables - 5 );
                                          testBucket(ut, hashSet,     0, 3 );
                                          testBucket(ut, hashSet,     1, 2 );

        auto start= hashSet.begin();      testConsistency(ut, hashSet,     5 ,  97,  cntRecylables - 5 );
                                          UT_EQ( 97,  start.Value() )

        start= hashSet.Erase( start );    testConsistency(ut, hashSet,     4 ,  97,  cntRecylables - 4 );
                                          testBucket(ut, hashSet,     0, 2 );
                                          UT_EQ(  0,  start.Value() )

        start= hashSet.Erase( start );    testConsistency(ut, hashSet,     3 ,  97,  cntRecylables - 3 );
                                          testBucket(ut, hashSet,     0, 1 );
                                          UT_EQ(  0,  start.Value() )

        start= hashSet.Erase( start );    testConsistency(ut, hashSet,     2 ,  97,  cntRecylables - 2 );
                                          testBucket(ut, hashSet,     0, 0 );
                                          UT_EQ( 98 ,  start.Value() )

        ++start;                          UT_EQ(  1 ,  start.Value() )
        start= hashSet.Erase( start );    testConsistency(ut, hashSet,     1 ,  97,  cntRecylables - 1 );
                                          testBucket(ut, hashSet,     1, 1 );
                                          UT_TRUE( hashSet.end() == start )
        start= hashSet.begin();
        start= hashSet.Erase( start );    testConsistency(ut, hashSet,     0 ,  97,  cntRecylables  );
                                          testBucket(ut, hashSet,     1, 0 );
                                          UT_TRUE( hashSet.end() == start )
    }

    // Erase( key )
    {
        hashSet.Clear();                        testConsistency ( ut, hashSet,     0 ,  97,  cntRecylables );
        hashSet.Emplace( 0);
        hashSet.Emplace( 0);
        hashSet.Emplace(97);
        hashSet.Emplace( 1);
        hashSet.Emplace(98);                    testConsistency ( ut, hashSet,     5 ,  97,  cntRecylables - 5 );
                                                testBucket      ( ut, hashSet,     0, 3 );
                                                testBucket      ( ut, hashSet,     1, 2 );

        UT_EQ( 0, hashSet.Erase(99)      )      testConsistency ( ut, hashSet,     5 ,  97,  cntRecylables - 5 );
        UT_EQ( 1, hashSet.Erase(98)      )      testConsistency ( ut, hashSet,     4 ,  97,  cntRecylables - 4 );
                                                testBucket      ( ut, hashSet,     1, 1 );
        UT_EQ( 2, hashSet.Erase(0)       )      testConsistency ( ut, hashSet,     2 ,  97,  cntRecylables - 2 );
        UT_EQ( 0, hashSet.Erase(0)       )      testConsistency ( ut, hashSet,     2 ,  97,  cntRecylables - 2 );
                                                testBucket      ( ut, hashSet,     0, 1 );

        lang::Report::GetDefault().PushHaltFlags( false, false );
        UT_PRINT( "An error should follow" )
        hashSet.Emplace( 0);
        hashSet.Emplace( 0);
        UT_EQ( true, hashSet.EraseUnique(0)  )  testConsistency(ut, hashSet,     3 ,  97,  cntRecylables - 3 );
        lang::Report::GetDefault().PopHaltFlags();
        UT_EQ( true , hashSet.EraseUnique( 0) )  testConsistency(ut, hashSet,     2 ,  97,  cntRecylables - 2 );
        UT_EQ( false, hashSet.EraseUnique( 0) )  testConsistency(ut, hashSet,     2 ,  97,  cntRecylables - 2 );
        UT_EQ( true , hashSet.EraseUnique(97) )  testConsistency(ut, hashSet,     1 ,  97,  cntRecylables - 1 );
        UT_EQ( true , hashSet.EraseUnique( 1) )  testConsistency(ut, hashSet,     0 ,  97,  cntRecylables );
        UT_EQ( false, hashSet.EraseUnique(98) )  testConsistency(ut, hashSet,     0 ,  97,  cntRecylables );
    }


    // Erase( start, end )
    {
        hashSet.Clear();                        testConsistency(ut, hashSet,     0 ,  97,  cntRecylables );
        hashSet.Emplace( 0);
        hashSet.Emplace( 0);
        hashSet.Emplace(97);
        hashSet.Emplace( 1);
        hashSet.Emplace(98);                    testConsistency(ut, hashSet,     5 ,  97,  cntRecylables - 5 );
                                                testBucket(ut, hashSet,     0, 3 );
                                                testBucket(ut, hashSet,     1, 2 );

        auto start= hashSet.begin();            UT_EQ( 97,  start.Value() )
        auto end  = hashSet.begin();            UT_EQ( 97,  end  .Value() )
        start= hashSet.Erase(start,end);        testConsistency(ut, hashSet,     5 ,  97,  cntRecylables - 5 );
                                                UT_EQ( 97,  start.Value() )

        start= hashSet.begin();                 UT_EQ( 97,  start.Value() )
        end  = hashSet.begin(); ++end;          UT_EQ(  0,  end  .Value() )
        start= hashSet.Erase(start,end);        testConsistency(ut, hashSet,     4 ,  97,  cntRecylables - 4 );
                                                UT_TRUE(  start == end )
                                                UT_EQ(  0,  start.Value() )

        start++;                                UT_EQ(  0,  start.Value() )
                                                testBucket(ut, hashSet,     0, 2 );
                                                testBucket(ut, hashSet,     1, 2 );
        end= start; ++end;  ++end;              UT_EQ(  1,  end  .Value() )
        start= hashSet.Erase( start, end );     testConsistency(ut, hashSet,     2 ,  97,  cntRecylables - 2 );
                                                testBucket(ut, hashSet,     0, 1 );
                                                testBucket(ut, hashSet,     1, 1 );
                                                UT_EQ(  1,  start.Value() )

        hashSet.Erase( hashSet.begin(),
                       hashSet.end()   );       testConsistency(ut, hashSet,     0 ,  97,  cntRecylables     );
        hashSet.Emplace( 0);
        hashSet.Emplace( 0);
        hashSet.Emplace(97);
        hashSet.Emplace( 1);
        hashSet.Emplace(98);                    testConsistency(ut, hashSet,     5 ,  97,  cntRecylables - 5 );
        hashSet.Erase( hashSet.begin(),
                       hashSet.end() );         testConsistency(ut, hashSet,     0 ,  97,  cntRecylables     );
    }


    // Erase( pos ) with bucket iterator
    {
        hashSet.Clear();                  testConsistency(ut, hashSet,     0 ,  97,  cntRecylables );
        hashSet.Emplace( 0);
        hashSet.Emplace( 0);
        hashSet.Emplace(97);
        hashSet.Emplace( 1);
        hashSet.Emplace(98);              testConsistency(ut, hashSet,     5 ,  97,  cntRecylables - 5 );
                                          testBucket(ut, hashSet,     0, 3 );
                                          testBucket(ut, hashSet,     1, 2 );

        auto start= hashSet.begin(0);     UT_EQ( 97,  start.Value() )

        start= hashSet.Erase( start );    testConsistency(ut, hashSet,     4 ,  97,  cntRecylables - 4 );
                                          testBucket(ut, hashSet,     0, 2 );
                                          UT_EQ(  0,  start.Value() )

        start= hashSet.Erase( start );    testConsistency(ut, hashSet,     3 ,  97,  cntRecylables - 3 );
                                          testBucket(ut, hashSet,     0, 1 );
                                          UT_EQ(  0,  start.Value() )

        start= hashSet.Erase( start );    testConsistency(ut, hashSet,     2 ,  97,  cntRecylables - 2 );
                                          testBucket(ut, hashSet,     0, 0 );
        hashSet.begin(0);                 UT_TRUE( hashSet.end(0) == start )

        start= hashSet.begin(1);          UT_EQ( 98  ,  start.Value() )
        ++start;                          UT_EQ(  1  ,  start.Value() )
        start= hashSet.Erase( start );    testConsistency(ut, hashSet,     1 ,  97,  cntRecylables - 1 );
                                          testBucket(ut, hashSet,     1, 1 );
                                          UT_TRUE( hashSet.end(1) == start )
        start= hashSet.begin(1);
        start= hashSet.Erase( start );    testConsistency(ut, hashSet,     0 ,  97,  cntRecylables  );
                                          testBucket(ut, hashSet,     1, 0 );
                                          UT_TRUE( hashSet.end(1) == start )
    }

    // Erase (start,end) with bucket iterators
    {
        hashSet.Clear();
        hashSet.Emplace( 0);
        hashSet.Emplace( 0);
        hashSet.Emplace(97);
        hashSet.Emplace( 1);
        hashSet.Emplace(98);                    testConsistency(ut, hashSet,     5 ,  97,  cntRecylables - 5 );
                                                testBucket(ut, hashSet,     0, 3 );
                                                testBucket(ut, hashSet,     1, 2 );

        auto start= hashSet.begin(0);           UT_EQ( 97,  start.Value() )
        auto end  = hashSet.begin(0);           UT_EQ( 97,  end  .Value() )
        start= hashSet.Erase(start,end);        testConsistency(ut, hashSet,     5 ,  97,  cntRecylables - 5 );
                                                UT_EQ( 97,  start.Value() )

        end  = hashSet.begin(0); ++end;         UT_EQ(  0,  end  .Value() )
        start= hashSet.Erase(start,end);        testConsistency(ut, hashSet,     4 ,  97,  cntRecylables - 4 );
                                                UT_TRUE(  start == end )
                                                UT_EQ(  0,  start.Value() )
                                                testBucket(ut, hashSet,     0, 2 );

        start++;                                UT_EQ(  0,  start.Value() )
        start= hashSet.Erase(start,
                             hashSet.end(0));   testConsistency(ut, hashSet,     3 ,  97,  cntRecylables - 3 );
                                                testBucket(ut, hashSet,     0, 1 );
                                                testBucket(ut, hashSet,     1, 2 );
                                                UT_TRUE(start == hashSet.end(0) )

        start= hashSet.Erase(hashSet.begin(0),
                             hashSet.end  (0)); testConsistency(ut, hashSet,     2 ,  97,  cntRecylables - 2 );
                                                testBucket(ut, hashSet,     0, 0 );
                                                testBucket(ut, hashSet,     1, 2 );
                                                UT_TRUE(start == hashSet.end(0) )



        start= hashSet.begin(1); ++start;       UT_EQ(  1,  start.Value() )
        end  = hashSet.end(1);
        start= hashSet.Erase(start,end);        testConsistency(ut, hashSet,     1 ,  97,  cntRecylables - 1 );
                                                UT_TRUE(start == hashSet.end(1) )
                                                testBucket(ut, hashSet,     1, 1 );
        start= hashSet.Erase(hashSet.begin(1),
                             hashSet.end  (1)); testConsistency(ut, hashSet,     0 ,  97,  cntRecylables  );
                                                testBucket(ut, hashSet,     0, 0 );
                                                testBucket(ut, hashSet,     1, 0 );
    }


    // Extract and Insert(handle)
    {
        hashSet.Clear();
        hashSet.Emplace( 0);
        hashSet.Emplace( 0);
        hashSet.Emplace(97);
        hashSet.Emplace( 1);
        hashSet.Emplace(98);                                testConsistency(ut, hashSet,     5 ,  97,  cntRecylables - 5 );
                                                            testBucket(ut, hashSet,     0, 3 );
                                                            testBucket(ut, hashSet,     1, 2 );

        // extract by pos and add at the same spot with same key existing
        auto start = hashSet.begin() ;                      UT_EQ( 97,  start.Value() )
        auto handle= hashSet.Extract(start);                testConsistency(ut, hashSet,     4 ,  97,  cntRecylables - 5 );
                                                            testBucket(ut, hashSet,     0, 2 );
                                                            testBucket(ut, hashSet,     1, 2 );
                                                            UT_FALSE(   handle.IsEmpty() )
                                                            UT_EQ( 97,  handle.Value  () )
        auto result= hashSet.Insert(handle);                testConsistency(ut, hashSet,     5 ,  97,  cntRecylables - 5 );
                                                            UT_TRUE(    handle.IsEmpty() )
                                                            UT_TRUE(  result == hashSet.Find(97) )
                                                            UT_TRUE(   handle.IsEmpty() )

        // extract by pos and add somewhere else
        start = hashSet.begin() ;                           UT_EQ( 97,  start.Value() )
        handle= hashSet.Extract(start);                     testConsistency(ut, hashSet,     4 ,  97,  cntRecylables - 5 );
                                                            testBucket(ut, hashSet,     0, 2 );
                                                            testBucket(ut, hashSet,     1, 2 );
                                                            UT_FALSE(   handle.IsEmpty() )
                                                            UT_EQ( 97,  handle.Value  ()  )
        handle.Value()= 5;                                  UT_EQ(  5,  handle.Value  ()   )
        result= hashSet.Insert(handle);                     testConsistency(ut, hashSet,     5 ,  97,  cntRecylables - 5 );
                                                            UT_TRUE(    handle.IsEmpty() )
                                                            UT_TRUE(  result == hashSet.Find(5) )
                                                            UT_TRUE(   handle.IsEmpty() )
                                                            testBucket(ut, hashSet,     0, 2 );
                                                            testBucket(ut, hashSet,     1, 2 );
                                                            testBucket(ut, hashSet,     5, 1 );

        // extract by key and add somewhere else
        handle= hashSet.Extract(1);                         testConsistency(ut, hashSet,     4 ,  97,  cntRecylables - 5 );
                                                            testBucket(ut, hashSet,     0, 2 );
                                                            testBucket(ut, hashSet,     1, 1 );
                                                            testBucket(ut, hashSet,     5, 1 );
                                                            UT_FALSE(   handle.IsEmpty() )
                                                            UT_EQ(  1,  handle.Value    () )
        // ...unsuccessful insert
        handle.Value()= 5;                                  UT_EQ(  5,  handle.Value  () )
        result= hashSet.InsertIfNotExistent(handle);        testConsistency(ut, hashSet,     4 ,  97,  cntRecylables - 5 );
                                                            UT_FALSE( handle.IsEmpty () )
                                                            UT_EQ(  5,  handle.Value   () )
                                                            testBucket(ut, hashSet,     0, 2 );
                                                            testBucket(ut, hashSet,     1, 1 );
                                                            testBucket(ut, hashSet,     5, 1 );
        // ...successful insert
        handle.Value()= 6;                                  UT_EQ(  6,  handle.Value  () )
        result= hashSet.InsertIfNotExistent(handle);        testConsistency(ut, hashSet,     5 ,  97,  cntRecylables - 5 );
                                                            UT_TRUE( handle.IsEmpty() )
                                                            UT_EQ(  6, result.Value()  )
                                                            testBucket(ut, hashSet,     0, 2 );
                                                            testBucket(ut, hashSet,     1, 1 );
                                                            testBucket(ut, hashSet,     5, 1 );
                                                            testBucket(ut, hashSet,     6, 1 );
    }


    // InsertUnique, EmplaceUnique
    {
        hashSet.Reset();                                    testConsistency(ut, hashSet,     0 ,  0 ,  0 );
        auto
        result= hashSet.InsertUnique(0);                    testConsistency(ut, hashSet,     1 ,  97,  0 );
                                                            UT_EQ( 0 ,  result.Value() )
        result= hashSet.InsertUnique(97);                   testConsistency(ut, hashSet,     2 ,  97,  0 );
                                                            UT_EQ( 97,  result.Value() )

        lang::Report::GetDefault().PushHaltFlags( false, false );
        UT_PRINT( "An error should follow" )

        result= hashSet.InsertUnique(0);                   testConsistency(ut, hashSet,     3 ,  97,  0 );
                                                            UT_EQ( 0 ,  result.Value() )
        lang::Report::GetDefault().PopHaltFlags();


        result= hashSet.EmplaceUnique( 2);                 testConsistency(ut, hashSet,     4 ,  97,  0 );
                                                            UT_EQ( 2  ,  result.Value() )
        result= hashSet.EmplaceUnique(98);                 testConsistency(ut, hashSet,     5 ,  97,  0 );
                                                            UT_EQ( 98 ,  result.Value() )

        lang::Report::GetDefault().PushHaltFlags( false, false );
        UT_PRINT( "An error should follow" )

        result= hashSet.EmplaceUnique(2);                  testConsistency(ut, hashSet,     6 ,  97,  0 );
                                                            UT_EQ( 2 ,  result.Value() )
        lang::Report::GetDefault().PopHaltFlags();
    }

    // EmplaceIfNotExistent
    {
        hashSet.Reset();                            testConsistency(ut, hashSet,     0 ,  0 ,  0 );
        auto
        result= hashSet.EmplaceIfNotExistent(0);    testConsistency(ut, hashSet,     1 ,  97,  0 );
                                                    UT_EQ(   0 ,  result.first.Value() )
                                                    UT_EQ(true ,  result.second  ) // is insert
                                                    UT_EQ(   0 ,  hashSet.begin().Value() )
                                                    UT_TRUE( result.first == hashSet.Find(0) )

        result= hashSet.EmplaceIfNotExistent(0);    testConsistency(ut, hashSet,     1 ,  97,  0 );
                                                    UT_EQ(   0 ,  result.first.Value() )
                                                    UT_EQ(false,  result.second  ) // nothing done
                                                    UT_EQ(   0 ,  hashSet.begin().Value() )
                                                    UT_TRUE( result.first == hashSet.Find(0) )


        result= hashSet.EmplaceIfNotExistent(1);   testConsistency(ut, hashSet,     2 ,  97,  0 );
                                                    UT_EQ(   1 ,  result.first.Value() )
                                                    UT_EQ(true ,  result.second  ) // is insert
                                                    UT_TRUE( result.first == hashSet.Find(1) )
        result= hashSet.EmplaceIfNotExistent(1);    testConsistency(ut, hashSet,     2 ,  97,  0 );
                                                    UT_EQ(   1 ,  result.first.Value() )
                                                    UT_EQ(false,  result.second  ) // nothing done
                                                    UT_TRUE( result.first == hashSet.Find(1) )

        result= hashSet.EmplaceIfNotExistent(2);    testConsistency(ut, hashSet,     3 ,  97,  0 ); // consumes recycle
                                                    UT_EQ(   2 ,  result.first.Value() )
                                                    UT_EQ(true ,  result.second  ) // is insert
                                                    UT_TRUE( result.first == hashSet.Find(2) )

        result= hashSet.EmplaceIfNotExistent(3);    testConsistency(ut, hashSet,     4 ,  97,  0 );
                                                    UT_EQ(  3  ,  result.first.Value() )
                                                    UT_EQ(true ,  result.second  ) // is insert
                                                    UT_TRUE( result.first == hashSet.Find(3) )
    }


    // InsertIfNotExistent
    {
        hashSet.Reset();                            testConsistency(ut, hashSet,     0 ,  0 ,  0 );
        auto
        result= hashSet.InsertIfNotExistent(0);     testConsistency(ut, hashSet,     1 ,  97,  0 );
                                                    UT_EQ(   0 ,  result.first.Value() )
                                                    UT_EQ(true ,  result.second  ) // is insert
                                                    UT_EQ(   0 ,  hashSet.begin().Value() )
                                                    UT_TRUE( result.first == hashSet.Find(0) )

        result= hashSet.InsertIfNotExistent(0);     testConsistency(ut, hashSet,     1 ,  97,  0 );
                                                    UT_EQ(   0 ,  result.first.Value() )
                                                    UT_EQ(false,  result.second  ) // nothing done
                                                    UT_EQ(   0 ,  hashSet.begin().Value() )
                                                    UT_TRUE( result.first == hashSet.Find(0) )


        result= hashSet.InsertIfNotExistent(1);    testConsistency(ut, hashSet,     2 ,  97,  0 );
                                                    UT_EQ(   1 ,  result.first.Value() )
                                                    UT_EQ(true ,  result.second  ) // is insert
                                                    UT_TRUE( result.first == hashSet.Find(1) )
        result= hashSet.InsertIfNotExistent(1);     testConsistency(ut, hashSet,     2 ,  97,  0 );
                                                    UT_EQ(   1 ,  result.first.Value() )
                                                    UT_EQ(false,  result.second  ) // nothing done
                                                    UT_TRUE( result.first == hashSet.Find(1) )

        result= hashSet.InsertIfNotExistent(2);     testConsistency(ut, hashSet,     3 ,  97,  0 ); // consumes recycle
                                                    UT_EQ(   2 ,  result.first.Value() )
                                                    UT_EQ(true ,  result.second  ) // is insert
                                                    UT_TRUE( result.first == hashSet.Find(2) )

        result= hashSet.InsertIfNotExistent(3);    testConsistency(ut, hashSet,     4 ,  97,  0 );
                                                    UT_EQ(  3  ,  result.first.Value() )
                                                    UT_EQ(true ,  result.second  ) // is insert
                                                    UT_TRUE( result.first == hashSet.Find(3) )
    }
}


//########################################################################################
// HashMap
//########################################################################################
template<typename TTable>
void UT_HashMap(AWorxUnitTesting&  ut, TTable& hashMap)
{
                                         testConsistency(ut, hashMap,     0 ,   0,  0 );
    hashMap.Emplace( 1, 100 );           testConsistency(ut, hashMap,     1 ,  97,  0 );
    UT_EQ( 1l, hashMap.Erase  ( 1  ) )   testConsistency(ut, hashMap,     0 ,  97,  1 );
    auto it1= hashMap.Emplace( 1, 100 ); testConsistency(ut, hashMap,     1 ,  97,  0 );
              UT_EQ( it1.Mapped(), 100 )
    auto it2= hashMap.Emplace( 1, 101 ); testConsistency(ut, hashMap,     2 ,  97,  0 );
              UT_EQ( it2.Mapped(), 101 )
    ++it2;    UT_TRUE( it1 == it2 )

    UT_EQ( 2l, hashMap.Erase  ( 1  ) )   testConsistency(ut, hashMap,     0 ,  97,  2 );


    {
        hashMap.Emplace( 1, 100 );       testConsistency(ut, hashMap,     1 ,  97,  1 );
        hashMap.Emplace( 2, 200 );
        auto it= hashMap.Find( 1 );      UT_FALSE( it == hashMap.end() )     UT_EQ(100, it.Mapped())

        it= hashMap.Find( 2 );           UT_FALSE( it == hashMap.end() )     UT_EQ(200, it->second)
        it= hashMap.Find( 3 );           UT_TRUE(  it == hashMap.end() )

    }

    // automatic rehash
    {
        hashMap.Emplace( static_cast<int>(1 + hashMap.BucketCount()), 102 );  testConsistency(ut, hashMap,     3 ,  97,  0 );
        hashMap.Emplace( 1                        , 103 );  testConsistency(ut, hashMap,     4 ,  97,  0 );

        auto inserts= static_cast<int>(   hashMap.MaxLoadFactor() * static_cast<float>(hashMap.BucketCount() )
                                        - static_cast<float>(hashMap.Size())                                     );
        for( int i= 0 ; i < inserts -1 ; ++i )
            hashMap.Emplace( 2000 + rand() % 100, 555  );

        auto oldBucketCount=  hashMap.BucketCount();
        UT_TRUE( hashMap.RecyclablesCount() == 0 )
        hashMap.Emplace( 1000, 1000 );
        UT_TRUE( oldBucketCount < hashMap.BucketCount() )

        testConsistency(ut, hashMap,   hashMap.Size(), 199, -1 );
        auto
        it= hashMap.Find( 1 );           UT_FALSE( it == hashMap.end() )     UT_TRUE( it->second == 100 || it->second== 103 )
        it= hashMap.Find( 2 );           UT_FALSE( it == hashMap.end() )     UT_EQ(200, it->second)
        it= hashMap.Find( 3 );           UT_TRUE(  it == hashMap.end() )

    }


    // EqualRange
    hashMap.Reset();
    auto cntRecylables= 0;
    {
        hashMap.Clear();                  testConsistency(ut, hashMap,     0 ,   1,  cntRecylables );
        hashMap.Emplace( 0,   0);         testConsistency(ut, hashMap,     1 ,  97,  cntRecylables );
        hashMap.Emplace( 0,   1);
        hashMap.Emplace(97,   1);
        hashMap.Emplace(97, 110);

        hashMap.Emplace( 1,   110);
        hashMap.Emplace( 98,  120);       testConsistency(ut, hashMap,     6 ,  97,  cntRecylables );

        auto
        result= hashMap.EqualRange(  0 ); UT_EQ(     0, result.first.Key() )
        ++result.first;                   UT_EQ(     0, result.first.Key() )
        ++result.first;                   UT_FALSE(  0  == result.first.Key() )
                                          UT_TRUE (  result.first == result.second )
        result= hashMap.EqualRange(  1 ); UT_EQ(     1, result.first.Key() )
        ++result.first;                   UT_TRUE (  result.first == result.second )
        result= hashMap.EqualRange( 97 ); UT_EQ(    97, result.first.Key() )
        ++result.first;                   UT_EQ(    97, result.first.Key() )
        ++result.first;                   UT_FALSE( 97  == result.first.Key() )
                                          UT_TRUE (  result.first == result.second )
        result= hashMap.EqualRange( 98 ); UT_EQ(    98, result.first.Key() )
        ++result.first;                   UT_TRUE (  result.first == result.second )
        cntRecylables= 6;
    }

    // Erase( pos )
    {
        hashMap.Clear();                  testConsistency(ut, hashMap,     0 ,  97,  cntRecylables );
        hashMap.Emplace(0, 0);
        hashMap.Emplace(0, 1);
        hashMap.Emplace(97, 110);
        hashMap.Emplace(1, 10);
        hashMap.Emplace(98, 120);         testConsistency(ut, hashMap,     5 ,  97,  cntRecylables - 5 );
                                          testBucket(ut, hashMap,     0, 3 );
                                          testBucket(ut, hashMap,     1, 2 );

        auto start= hashMap.begin();      testConsistency(ut, hashMap,     5 ,  97,  cntRecylables - 5 );
                                          UT_EQ(110,  start.Mapped() )

        start= hashMap.Erase( start );    testConsistency(ut, hashMap,     4 ,  97,  cntRecylables - 4 );
                                          testBucket(ut, hashMap,     0, 2 );
                                          UT_EQ(  1,  start.Mapped() )

        start= hashMap.Erase( start );    testConsistency(ut, hashMap,     3 ,  97,  cntRecylables - 3 );
                                          testBucket(ut, hashMap,     0, 1 );
                                          UT_EQ(  0,  start.Mapped() )

        start= hashMap.Erase( start );    testConsistency(ut, hashMap,     2 ,  97,  cntRecylables - 2 );
                                          testBucket(ut, hashMap,     0, 0 );
                                          UT_EQ(120,  start.Mapped() )

        ++start;                          UT_EQ( 10,  start.Mapped() )
        start= hashMap.Erase( start );    testConsistency(ut, hashMap,     1 ,  97,  cntRecylables - 1 );
                                          testBucket(ut, hashMap,     1, 1 );
                                          UT_TRUE( hashMap.end() == start )
        start= hashMap.begin();
        start= hashMap.Erase( start );    testConsistency(ut, hashMap,     0 ,  97,  cntRecylables - 0 );
                                          testBucket(ut, hashMap,     1, 0 );
                                          UT_TRUE( hashMap.end() == start )
    }


    // Erase( start, end )
    {
        hashMap.Clear();                        testConsistency(ut, hashMap,     0 ,  97,  cntRecylables - 0 );
        hashMap.Emplace(0, 0);
        hashMap.Emplace(0, 1);
        hashMap.Emplace(97, 110);
        hashMap.Emplace(1, 10);
        hashMap.Emplace(98, 120);               testConsistency(ut, hashMap,     5 ,  97,  cntRecylables - 5 );
                                                testBucket(ut, hashMap,     0, 3 );
                                                testBucket(ut, hashMap,     1, 2 );

        auto start= hashMap.begin();            UT_EQ(110,  start.Mapped() )
        auto end  = hashMap.begin();            UT_EQ(110,  end  .Mapped() )
        start= hashMap.Erase(start,end);        testConsistency(ut, hashMap,     5 ,  97,  cntRecylables - 5 );
                                                UT_EQ(110,  start.Mapped() )

        start= hashMap.begin();                 UT_EQ(110,  start.Mapped() )
        end  = hashMap.begin(); ++end;          UT_EQ(  1,  end  .Mapped() )
        start= hashMap.Erase(start,end);        testConsistency(ut, hashMap,     4 ,  97,  cntRecylables - 4 );
                                                UT_TRUE(  start == end )
                                                UT_EQ(  1,  start.Mapped() )

        start++;                                UT_EQ(  0,  start.Mapped() )
                                                testBucket(ut, hashMap,     0, 2 );
                                                testBucket(ut, hashMap,     1, 2 );
        end= start; ++end;  ++end;              UT_EQ( 10,  end  .Mapped() )
        start= hashMap.Erase( start, end );     testConsistency(ut, hashMap,     2 ,  97,  cntRecylables - 2 );
                                                testBucket(ut, hashMap,     0, 1 );
                                                testBucket(ut, hashMap,     1, 1 );
                                                UT_EQ( 10,  start.Mapped() )

        hashMap.Erase( hashMap.begin(),
                       hashMap.end() );         testConsistency(ut, hashMap,     0 ,  97,  cntRecylables  );
        hashMap.Emplace(0, 0);
        hashMap.Emplace(0, 1);
        hashMap.Emplace(97, 110);
        hashMap.Emplace(1, 10);
        hashMap.Emplace(98, 120);               testConsistency(ut, hashMap,     5 ,  97,  cntRecylables - 5 );
        hashMap.Erase( hashMap.begin(),
                       hashMap.end() );         testConsistency(ut, hashMap,     0 ,  97,  cntRecylables - 0 );
    }


    // Erase( pos ) with bucket iterator
    {
        hashMap.Clear();                  testConsistency(ut, hashMap,     0 ,  97,  cntRecylables );
        hashMap.Emplace(0, 0);
        hashMap.Emplace(0, 1);
        hashMap.Emplace(97, 110);
        hashMap.Emplace(1, 10);
        hashMap.Emplace(98, 120);         testConsistency(ut, hashMap,     5 ,  97,  cntRecylables - 5 );
                                          testBucket(ut, hashMap,     0, 3 );
                                          testBucket(ut, hashMap,     1, 2 );

        auto start= hashMap.begin(0);     UT_EQ(110,  start.Mapped() )

        start= hashMap.Erase( start );    testConsistency(ut, hashMap,     4 ,  97,  cntRecylables - 4 );
                                          testBucket(ut, hashMap,     0, 2 );
                                          UT_EQ(  1,  start.Mapped() )

        start= hashMap.Erase( start );    testConsistency(ut, hashMap,     3 ,  97,  cntRecylables - 3 );
                                          testBucket(ut, hashMap,     0, 1 );
                                          UT_EQ(  0,  start.Mapped() )

        start= hashMap.Erase( start );    testConsistency(ut, hashMap,     2 ,  97,  cntRecylables - 2 );
                                          testBucket(ut, hashMap,     0, 0 );
        hashMap.begin(0);                 UT_TRUE( hashMap.end(0) == start )

        start= hashMap.begin(1);          UT_EQ(120,  start.Mapped() )
        ++start;                          UT_EQ( 10,  start.Mapped() )
        start= hashMap.Erase( start );    testConsistency(ut, hashMap,     1 ,  97,  cntRecylables - 1 );
                                          testBucket(ut, hashMap,     1, 1 );
                                          UT_TRUE( hashMap.end(1) == start )
        start= hashMap.begin(1);
        start= hashMap.Erase( start );    testConsistency(ut, hashMap,     0 ,  97,  cntRecylables - 0 );
                                          testBucket(ut, hashMap,     1, 0 );
                                          UT_TRUE( hashMap.end(1) == start )
    }

    // Erase (start,end) with bucket iterators
    {
        hashMap.Clear();
        hashMap.Emplace(0, 0);
        hashMap.Emplace(0, 1);
        hashMap.Emplace(97, 110);
        hashMap.Emplace(1, 10);
        hashMap.Emplace(98, 120);               testConsistency(ut, hashMap,     5 ,  97,  cntRecylables - 5 );
                                                testBucket(ut, hashMap,     0, 3 );
                                                testBucket(ut, hashMap,     1, 2 );

        auto start= hashMap.begin(0);           UT_EQ(110,  start.Mapped() )
        auto end  = hashMap.begin(0);           UT_EQ(110,  end  .Mapped() )
        start= hashMap.Erase(start,end);        testConsistency(ut, hashMap,     5 ,  97,  cntRecylables - 5 );
                                                UT_EQ(110,  start.Mapped() )

        start= hashMap.begin(0);                UT_EQ(110,  start.Mapped() )
        end  = hashMap.begin(0); ++end;         UT_EQ(  1,  end  .Mapped() )
        start= hashMap.Erase(start,end);        testConsistency(ut, hashMap,     4 ,  97,  cntRecylables - 4 );
                                                UT_TRUE(  start == end )
                                                UT_EQ(  1,  start.Mapped() )
                                                testBucket(ut, hashMap,     0, 2 );

        start++;                                UT_EQ(  0,  start.Mapped() )
        start= hashMap.Erase(start,
                             hashMap.end(0));   testConsistency(ut, hashMap,     3 ,  97,  cntRecylables - 3 );
                                                testBucket(ut, hashMap,     0, 1 );
                                                testBucket(ut, hashMap,     1, 2 );
                                                UT_TRUE(start == hashMap.end(0) )

        start= hashMap.Erase(hashMap.begin(0),
                             hashMap.end  (0)); testConsistency(ut, hashMap,     2 ,  97,  cntRecylables - 2 );
                                                testBucket(ut, hashMap,     0, 0 );
                                                testBucket(ut, hashMap,     1, 2 );
                                                UT_TRUE(start == hashMap.end(0) )



        start= hashMap.begin(1); ++start;       UT_EQ( 10,  start.Mapped() )
        end  = hashMap.end(1);
        start= hashMap.Erase(start,end);        testConsistency(ut, hashMap,     1 ,  97,  cntRecylables - 1 );
                                                UT_TRUE(start == hashMap.end(1) )
                                                testBucket(ut, hashMap,     1, 1 );
        start= hashMap.Erase(hashMap.begin(1),
                             hashMap.end  (1)); testConsistency(ut, hashMap,     0 ,  97,  cntRecylables - 0 );
                                                testBucket(ut, hashMap,     0, 0 );
                                                testBucket(ut, hashMap,     1, 0 );
    }

    // Extract and Insert(handle)
    {
        hashMap.Clear();
        hashMap.Emplace(0, 0);
        hashMap.Emplace(0, 1);
        hashMap.Emplace(97, 110);
        hashMap.Emplace(1, 10);
        hashMap.Emplace(98, 120);                           testConsistency(ut, hashMap,     5 ,  97,  cntRecylables - 5 );
                                                            testBucket(ut, hashMap,     0, 3 );
                                                            testBucket(ut, hashMap,     1, 2 );

        // extract by pos and add at the same spot with same key existing
        auto start = hashMap.begin() ;                      UT_EQ(110,  start.Mapped() )
        auto handle= hashMap.Extract(start);                testConsistency(ut, hashMap,     4 ,  97,  cntRecylables - 5 );
                                                            testBucket(ut, hashMap,     0, 2 );
                                                            testBucket(ut, hashMap,     1, 2 );
                                                            UT_FALSE(   handle.IsEmpty() )
                                                            UT_EQ( 97,  handle.Key  ()   )
                                                            UT_EQ(110,  handle.Mapped()  )
        auto result= hashMap.Insert(handle);                testConsistency(ut, hashMap,     5 ,  97,  cntRecylables - 5 );
                                                            UT_TRUE(    handle.IsEmpty() )
                                                            UT_TRUE(  result == hashMap.Find(97) )
                                                            UT_TRUE(   handle.IsEmpty() )

        // extract by pos and add somewhere else
        start = hashMap.begin() ;                           UT_EQ(110,  start.Mapped() )
        handle= hashMap.Extract(start);                     testConsistency(ut, hashMap,     4 ,  97,  cntRecylables - 5 );
                                                            testBucket(ut, hashMap,     0, 2 );
                                                            testBucket(ut, hashMap,     1, 2 );
                                                            UT_FALSE(   handle.IsEmpty() )
                                                            UT_EQ( 97,  handle.Key  ()   )
                                                            UT_EQ(110,  handle.Mapped()  )
        handle.Key()= 5;                                    UT_EQ(  5,  handle.Key  ()   )
        result= hashMap.Insert(handle);                     testConsistency(ut, hashMap,     5 ,  97,  cntRecylables - 5 );
                                                            UT_TRUE(    handle.IsEmpty() )
                                                            UT_TRUE(  result == hashMap.Find(5) )
                                                            UT_TRUE(   handle.IsEmpty() )
                                                            testBucket(ut, hashMap,     0, 2 );
                                                            testBucket(ut, hashMap,     1, 2 );
                                                            testBucket(ut, hashMap,     5, 1 );

        // extract by key and add somewhere else
        handle= hashMap.Extract(1);                         testConsistency(ut, hashMap,     4 ,  97,  cntRecylables - 5 );
                                                            testBucket(ut, hashMap,     0, 2 );
                                                            testBucket(ut, hashMap,     1, 1 );
                                                            testBucket(ut, hashMap,     5, 1 );
                                                            UT_FALSE(   handle.IsEmpty() )
                                                            UT_EQ(  1,  handle.Key    () )
                                                            UT_EQ( 10,  handle.Mapped () )
        // ...unsuccessful insert
        handle.Key()= 5;                                    UT_EQ(  5,  handle.Key  () )
                                                            UT_EQ( 10,  handle.Mapped() )
        result= hashMap.InsertIfNotExistent(handle);        testConsistency(ut, hashMap,     4 ,  97,  cntRecylables - 5 );
                                                            UT_FALSE( handle.IsEmpty () )
                                                            UT_EQ(  5,  handle.Key   () )
                                                            UT_EQ( 10,  handle.Mapped() )
                                                            testBucket(ut, hashMap,     0, 2 );
                                                            testBucket(ut, hashMap,     1, 1 );
                                                            testBucket(ut, hashMap,     5, 1 );
        // ...successful insert
        handle.Key()= 6;                                    UT_EQ(  6,  handle.Key  () )
        result= hashMap.InsertIfNotExistent(handle);        testConsistency(ut, hashMap,     5 ,  97,  cntRecylables - 5 );
                                                            UT_TRUE( handle.IsEmpty() )
                                                            UT_EQ(  10, result.Mapped()  )
                                                            testBucket(ut, hashMap,     0, 2 );
                                                            testBucket(ut, hashMap,     1, 1 );
                                                            testBucket(ut, hashMap,     5, 1 );
                                                            testBucket(ut, hashMap,     6, 1 );
    }

    // InsertUnique, EmplaceUnique
    {
        hashMap.Reset();                                         testConsistency(ut, hashMap,     0 ,  0 ,  0 );
        auto
        result= hashMap.InsertUnique(std::make_pair( 0, 100));   testConsistency(ut, hashMap,     1 ,  97,  0 );
                                                                  UT_EQ( 100 ,  result.Mapped() )
        result= hashMap.InsertUnique(std::make_pair(97, 110));   testConsistency(ut, hashMap,     2 ,  97,  0 );
                                                                  UT_EQ( 110 ,  result.Mapped() )

        lang::Report::GetDefault().PushHaltFlags( false, false );
        UT_PRINT( "An error should follow" )

        result= hashMap.InsertUnique(std::make_pair( 0, 101));   testConsistency(ut, hashMap,     3 ,  97,  0 );
                                                                  UT_EQ( 101 ,  result.Mapped() )
        lang::Report::GetDefault().PopHaltFlags();


        result= hashMap.EmplaceUnique( 1, 200);                  testConsistency(ut, hashMap,     4 ,  97,  0 );
                                                                  UT_EQ( 200 ,  result.Mapped() )
        result= hashMap.EmplaceUnique(98, 210);                  testConsistency(ut, hashMap,     5 ,  97,  0 );
                                                                  UT_EQ( 210 ,  result.Mapped() )

        lang::Report::GetDefault().PushHaltFlags( false, false );
        UT_PRINT( "An error should follow" )

        result= hashMap.EmplaceUnique(1, 201);                   testConsistency(ut, hashMap,     6 ,  97,  0 );
                                                                  UT_EQ( 201 ,  result.Mapped() )
        lang::Report::GetDefault().PopHaltFlags();
    }


    // EmplaceOrAssign, EmplaceIfNotExistent
    {
        hashMap.Reset();                            testConsistency(ut, hashMap,     0 ,  0 ,  0 );
        auto
        result= hashMap.EmplaceOrAssign(0, 0);      testConsistency(ut, hashMap,     1 ,  97,  0 );
                                                    UT_EQ(   0 ,  result.first.Mapped() )
                                                    UT_EQ(true ,  result.second  ) // is insert
                                                    UT_EQ(   0 ,  hashMap.begin().Mapped() )
                                                    UT_TRUE( result.first == hashMap.Find(0) )

        result= hashMap.EmplaceIfNotExistent(0, 0);      testConsistency(ut, hashMap,     1 ,  97,  0 );
                                                    UT_EQ(   0 ,  result.first.Mapped() )
                                                    UT_EQ(false,  result.second  ) // nothing done
                                                    UT_EQ(   0 ,  hashMap.begin().Mapped() )
                                                    UT_TRUE( result.first == hashMap.Find(0) )


        result= hashMap.EmplaceOrAssign(0, 1);      testConsistency(ut, hashMap,     1 ,  97,  0 );
                                                    UT_EQ(false,  result.second  ) // is replace
                                                    UT_TRUE( result.first == hashMap.Find(0) )
        result= hashMap.EmplaceOrAssign(0, 2);      testConsistency(ut, hashMap,     1 ,  97,  0 );
                                                    UT_EQ(   2 ,  result.first.Mapped() )
                                                    UT_EQ(false,  result.second  ) // is replace
                                                    UT_EQ(   2 ,  hashMap.begin().Mapped() )
                                                    UT_TRUE( result.first == hashMap.Find(0) )

        result= hashMap.EmplaceOrAssign(1, 3);      testConsistency(ut, hashMap,     2 ,  97,  0 );
                                                    UT_EQ(   3 ,  result.first.Mapped() )
                                                    UT_EQ(true ,  result.second  ) // is insert
                                                    UT_TRUE( result.first == hashMap.Find(1) )
        result= hashMap.EmplaceOrAssign(1, 4);      testConsistency(ut, hashMap,     2 ,  97,  0 );
                                                    UT_EQ(   4 ,  result.first.Mapped() )
                                                    UT_EQ(false,  result.second  ) // is replace
                                                    UT_TRUE( result.first == hashMap.Find(1) )
        result= hashMap.EmplaceIfNotExistent(1, 5); testConsistency(ut, hashMap,     2 ,  97,  0 );
                                                    UT_EQ(   4 ,  result.first.Mapped() )
                                                    UT_EQ(false,  result.second  ) // nothing done
                                                    UT_TRUE( result.first == hashMap.Find(1) )

        result= hashMap.EmplaceIfNotExistent(2, 98);testConsistency(ut, hashMap,     3 ,  97,  0 );
                                                    UT_EQ(  98 ,  result.first.Mapped() )
                                                    UT_EQ(true ,  result.second  ) // is insert
                                                    UT_TRUE( result.first == hashMap.Find(2) )

        result= hashMap.EmplaceIfNotExistent(3, 13);testConsistency(ut, hashMap,     4 ,  97,  0 );
                                                    UT_EQ(  13 ,  result.first.Mapped() )
                                                    UT_EQ(true ,  result.second  ) // is insert
                                                    UT_TRUE( result.first == hashMap.Find(3) )

        // test that returned iterator survives rehash
        result= hashMap.EmplaceOrAssign(4, 14);     testConsistency(ut, hashMap,     5 ,  97,  -1 );
        result= hashMap.EmplaceOrAssign(5, 15);     testConsistency(ut, hashMap,     6 ,  97,  -1 );

        auto inserts= static_cast<int>(   hashMap.MaxLoadFactor() * static_cast<float>(hashMap.BucketCount())
                                        - static_cast<float>(hashMap.Size())                                   );
        for( int i= 0 ; i < inserts -1 ; ++i )
            hashMap.Emplace( 6000 + (rand() % 100), 1234 );
                                                    testConsistency(ut, hashMap,   193 ,  97,  -1 );

        result= hashMap.EmplaceOrAssign(0, 5);      testConsistency(ut, hashMap,   193 ,  97,  -1 );
                                                    UT_EQ(   5 ,  result.first.Mapped() )
                                                    UT_EQ(false,  result.second  ) // is replace
                                                    UT_TRUE( result.first == hashMap.Find(0) )

        result= hashMap.EmplaceOrAssign(100, 110);  testConsistency(ut, hashMap,   194 , 199,  -1 );
                                                    UT_EQ( 110 ,  result.first.Mapped() )
                                                    UT_EQ(true,  result.second  ) // is insert
                                                    UT_TRUE( result.first == hashMap.Find(100) )
    }

    // InsertOrAssign, InsertIfNotExistent
    {
        hashMap.Reset();                           testConsistency(ut, hashMap,     0 ,  0 ,  -1 );
        auto
        result= hashMap.InsertOrAssign(0, 0);      testConsistency(ut, hashMap,     1 ,  97,  -1 );
                                                    UT_EQ(   0 ,  result.first.Mapped() )
                                                    UT_EQ(true ,  result.second  ) // is insert
                                                    UT_EQ(   0 ,  hashMap.begin().Mapped() )
                                                    UT_TRUE( result.first == hashMap.Find(0) )

        result= hashMap.InsertIfNotExistent(0, 0);  testConsistency(ut, hashMap,     1 ,  97,  -1 );
                                                    UT_EQ(   0 ,  result.first.Mapped() )
                                                    UT_EQ(false,  result.second  ) // nothing done
                                                    UT_EQ(   0 ,  hashMap.begin().Mapped() )
                                                    UT_TRUE( result.first == hashMap.Find(0) )


        result= hashMap.InsertOrAssign(0, 1);       testConsistency(ut, hashMap,     1 ,  97,  -1 );
                                                    UT_EQ(false,  result.second  ) // is replace
                                                    UT_TRUE( result.first == hashMap.Find(0) )
        result= hashMap.InsertOrAssign(0, 2);       testConsistency(ut, hashMap,     1 ,  97,  -1 );
                                                    UT_EQ(   2 ,  result.first.Mapped() )
                                                    UT_EQ(false,  result.second  ) // is replace
                                                    UT_EQ(   2 ,  hashMap.begin().Mapped() )
                                                    UT_TRUE( result.first == hashMap.Find(0) )

        result= hashMap.InsertOrAssign(1, 3);       testConsistency(ut, hashMap,     2 ,  97,  -1 );
                                                    UT_EQ(   3 ,  result.first.Mapped() )
                                                    UT_EQ(true ,  result.second  ) // is insert
                                                    UT_TRUE( result.first == hashMap.Find(1) )
        result= hashMap.InsertOrAssign(1, 4);       testConsistency(ut, hashMap,     2 ,  97,  -1 );
                                                    UT_EQ(   4 ,  result.first.Mapped() )
                                                    UT_EQ(false,  result.second  ) // is replace
                                                    UT_TRUE( result.first == hashMap.Find(1) )
        result= hashMap.InsertIfNotExistent(1, 5);  testConsistency(ut, hashMap,     2 ,  97,  -1 );
                                                    UT_EQ(   4 ,  result.first.Mapped() )
                                                    UT_EQ(false,  result.second  ) // nothing done
                                                    UT_TRUE( result.first == hashMap.Find(1) )

        result= hashMap.InsertIfNotExistent(2, 98); testConsistency(ut, hashMap,     3 ,  97,  -1 );
                                                    UT_EQ(  98 ,  result.first.Mapped() )
                                                    UT_EQ(true ,  result.second  ) // is insert
                                                    UT_TRUE( result.first == hashMap.Find(2) )

        result= hashMap.InsertIfNotExistent(3, 13); testConsistency(ut, hashMap,     4 ,  97,  -1 );
                                                    UT_EQ(  13 ,  result.first.Mapped() )
                                                    UT_EQ(true ,  result.second  ) // is insert
                                                    UT_TRUE( result.first == hashMap.Find(3) )

        // test that returned iterator survives rehash
        result= hashMap.InsertOrAssign(4, 14);      testConsistency(ut, hashMap,     5 ,  97,  -1 );
        result= hashMap.InsertOrAssign(5, 15);      testConsistency(ut, hashMap,     6 ,  97,  -1 );

        auto inserts= static_cast<int>(   hashMap.MaxLoadFactor() * static_cast<float>(hashMap.BucketCount())
                                        - static_cast<float>(hashMap.Size())                                   );
        for( int i= 0 ; i < inserts -1 ; ++i )
            hashMap.Insert( std::make_pair(4000 + rand() % 100, 999) );

                                                    testConsistency(ut, hashMap,   193 ,  97,  -1 );

        result= hashMap.InsertOrAssign(0, 5);       testConsistency(ut, hashMap,   193 ,  97,  -1 );
                                                    UT_EQ(   5 ,  result.first.Mapped() )
                                                    UT_EQ(false,  result.second  ) // is replace
                                                    UT_TRUE( result.first == hashMap.Find(0) )

        result= hashMap.InsertOrAssign(100, 110);   testConsistency(ut, hashMap,   194 , 199,  -1 );
                                                    UT_EQ( 110 ,  result.first.Mapped() )
                                                    UT_EQ(true,  result.second  ) // is insert
                                                    UT_TRUE( result.first == hashMap.Find(100) )
    }
}
#endif //if !defined(ALIB_UT_REDUCED_COMPILE_TIME)

#include "alib/lang/callerinfo_methods.hpp"

} //anonymous namespace

// #################################################################################################
// #################################################################################################
// #################################################################################################
// ### UT_CLASS
// #################################################################################################
// #################################################################################################
// #################################################################################################
UT_CLASS


#if ALIB_DEBUG_CONTAINERS
// ########################################################################################
// HashTable Debug dump
// ########################################################################################
UT_METHOD(HashTable_Distribution)
{
    UT_INIT()

    // These tests do not test any output. But being able to compile the templated methods is
    // already quite good ;-)

    MonoAllocator ma(ALIB_DBG("UT" ,) 2);
    HashSet<MonoAllocator, String> hashSet(ma);
    hashSet.Emplace( A_CHAR("Hello"  ));
    hashSet.Emplace( A_CHAR("World"  ));
    hashSet.Emplace( A_CHAR(","      ));
    hashSet.Emplace( A_CHAR("how"    ));
    hashSet.Emplace( A_CHAR("are"    ));
    hashSet.Emplace( A_CHAR("you"    ));
    hashSet.Emplace( A_CHAR("today"  ));
    hashSet.Emplace( A_CHAR("?"      ));

    UT_PRINT( "Test table contents:" )
    UT_PRINT( alib::containers::DbgDumpHashtable( hashSet ) )

    UT_PRINT( "Test table statistics:" )
    UT_PRINT( alib::containers::DbgDumpDistribution(hashSet, true) )
}
#endif //  ALIB_DEBUG_CONTAINERS

//--------------------------------------------------------------------------------------------------
//--- Recycling
//--------------------------------------------------------------------------------------------------
UT_METHOD(HashTableRecycling)
{
    // Note: the main purpose of this unit tests is to ensure that the rather complicated
    //       type definitions compile well.
    //       The fact that recycling in general works is tested directly and inherently with
    //       other tests.
    UT_INIT()

    // HashSet shared recycler
    {
DOX_MARKER( [DOX_MONOMEM_RECYCLER] )
MonoAllocator monoAllocator( ALIB_DBG("MyAllocator",) 1 );

// Type definition for a hash set using a shared recycler
using MySet= HashSet< MonoAllocator,
                      int,
                      std::hash<int>,
                      std::equal_to<int>,
                      lang::Caching::Disabled,
                      Recycling::Shared      >;  // <-- shared recycling!

// The shared recycler instance
MySet::SharedRecyclerType sharedRecycler( monoAllocator );

// Two hash set instances. The shared recycler has to be passed to the constructor.
MySet  set1( sharedRecycler );
MySet  set2( sharedRecycler );

// Assert that the number of recyclables is always the same for both sets
                              UT_EQ( 0, set1.RecyclablesCount() )
                              UT_EQ( 0, set2.RecyclablesCount() )

set1.Emplace(1);              UT_EQ( 0, set1.RecyclablesCount() )
                              UT_EQ( 0, set2.RecyclablesCount() )

set1.Emplace(2);              UT_EQ( 0, set1.RecyclablesCount() )
                              UT_EQ( 0, set2.RecyclablesCount() )

set1.Erase  (1);              UT_EQ( 1, set1.RecyclablesCount() )
                              UT_EQ( 1, set2.RecyclablesCount() )

set2.Emplace(1);              UT_EQ( 0, set1.RecyclablesCount() )
                              UT_EQ( 0, set2.RecyclablesCount() )

set2.Erase  (1);              UT_EQ( 1, set1.RecyclablesCount() )
                              UT_EQ( 1, set2.RecyclablesCount() )

set1.Erase  (2);              UT_EQ( 2, set1.RecyclablesCount() )
                              UT_EQ( 2, set2.RecyclablesCount() )

// If we reserve recyclables for one container, they will also be available
// for the other.
set1.ReserveRecyclables( 10, lang::ValueReference::Absolute );
                              UT_EQ( 10, set1.RecyclablesCount() )
                              UT_EQ( 10, set2.RecyclablesCount() )

// As such, the better way is to use Reserve on the recycler:
sharedRecycler.Reserve( 10, lang::ValueReference::Relative );
                              UT_EQ( 20, set1.RecyclablesCount() )
                              UT_EQ( 20, set2.RecyclablesCount() )
DOX_MARKER( [DOX_MONOMEM_RECYCLER] )
    }

    // HashSet no recycler
    {
        MonoAllocator monoAllocator(ALIB_DBG("UTSharedHashSet",) 1);
        using MySharedSet= HashSet<MonoAllocator,
                                   int,
                                   std::hash<int>,
                                   std::equal_to<int>,
                                   lang::Caching::Disabled,
                                   Recycling::None >;
        MySharedSet  set( monoAllocator );

        set.Emplace(1);
        set.Erase(1);
    }

    // HashMap shared recycler
    {
        MonoAllocator monoAllocator(ALIB_DBG("UTHashMapSharedRec",) 1);
        using MySharedMap= HashMap<MonoAllocator,
                                   int,int,
                                   std::hash<int>,
                                   std::equal_to<int>,
                                   lang::Caching::Disabled,
                                   Recycling::Shared >;
        MySharedMap::SharedRecyclerType sharedRecycler( monoAllocator );
        MySharedMap  map1( sharedRecycler );
        MySharedMap  map2( sharedRecycler );

                               UT_EQ( 0, map1.RecyclablesCount() )
                               UT_EQ( 0, map2.RecyclablesCount() )

        map1.Emplace(1,123);   UT_EQ( 0, map1.RecyclablesCount() )
                               UT_EQ( 0, map2.RecyclablesCount() )

        map1.Erase(1);         UT_EQ( 1, map1.RecyclablesCount() )
                               UT_EQ( 1, map2.RecyclablesCount() )

        map2.Emplace(1,123);   UT_EQ( 0, map1.RecyclablesCount() )
                               UT_EQ( 0, map2.RecyclablesCount() )

        map2.Erase(1);         UT_EQ( 1, map1.RecyclablesCount() )
                               UT_EQ( 1, map2.RecyclablesCount() )
    }

    // HashMap no recycler
    {
        MonoAllocator monoAllocator(ALIB_DBG("UTHashMapNR",) 1);
        using MySharedMap= HashMap<MonoAllocator,
                                   int,int,
                                   std::hash<int>,
                                   std::equal_to<int>,
                                   lang::Caching::Disabled,
                                   Recycling::None >;
        MySharedMap  map( monoAllocator );

        map.Emplace(1,1);
        map.Erase(1);
    }

    // HeapAllocator / shared recycler
    {
        using HS= HashSet<HeapAllocator, int   , std::hash<int> , std::equal_to<int> ,lang::Caching::Disabled, Recycling::Shared>;
        HS::SharedRecyclerType sr;
        {
            HS hs1(sr);
            {
                HS hs2(sr);
                hs1.InsertUnique(11);                          UT_EQ( 0, sr.Count() )   UT_EQ( 1, hs1.Size() )  UT_EQ( 0, hs2.Size() )
                sr.Reserve(2,lang::ValueReference::Absolute);  UT_EQ( 2, sr.Count() )   UT_EQ( 1, hs1.Size() )  UT_EQ( 0, hs2.Size() )
                sr.Reserve(2,lang::ValueReference::Absolute);  UT_EQ( 2, sr.Count() )   UT_EQ( 1, hs1.Size() )  UT_EQ( 0, hs2.Size() )
                sr.Reserve(1,lang::ValueReference::Relative);  UT_EQ( 3, sr.Count() )   UT_EQ( 1, hs1.Size() )  UT_EQ( 0, hs2.Size() )
                hs1.InsertUnique(12);                          UT_EQ( 2, sr.Count() )   UT_EQ( 2, hs1.Size() )  UT_EQ( 0, hs2.Size() )
                hs2.InsertUnique(21);                          UT_EQ( 1, sr.Count() )   UT_EQ( 2, hs1.Size() )  UT_EQ( 1, hs2.Size() )
                hs2.InsertUnique(22);                          UT_EQ( 0, sr.Count() )   UT_EQ( 2, hs1.Size() )  UT_EQ( 2, hs2.Size() )
                hs1.Erase(11);                                 UT_EQ( 1, sr.Count() )   UT_EQ( 1, hs1.Size() )  UT_EQ( 2, hs2.Size() )
                hs2.Erase(21);                                 UT_EQ( 2, sr.Count() )   UT_EQ( 1, hs1.Size() )  UT_EQ( 1, hs2.Size() )
                hs1.Reset();                                   UT_EQ( 3, sr.Count() )   UT_EQ( 0, hs1.Size() )  UT_EQ( 1, hs2.Size() )
                hs1.InsertUnique(13);                          UT_EQ( 2, sr.Count() )   UT_EQ( 1, hs1.Size() )  UT_EQ( 1, hs2.Size() )
                hs1.InsertUnique(14);                          UT_EQ( 1, sr.Count() )   UT_EQ( 2, hs1.Size() )  UT_EQ( 1, hs2.Size() )
                sr.Reset();                                    UT_EQ( 0, sr.Count() )   UT_EQ( 2, hs1.Size() )  UT_EQ( 1, hs2.Size() )
                hs2.InsertUnique(23);                          UT_EQ( 0, sr.Count() )   UT_EQ( 2, hs1.Size() )  UT_EQ( 2, hs2.Size() )
                hs2.InsertUnique(24);                          UT_EQ( 0, sr.Count() )   UT_EQ( 2, hs1.Size() )  UT_EQ( 3, hs2.Size() )
            } // <- does not recycle the bucket array, because HeapAllocator::allowsMemSplit() is false
            UT_EQ( 3 , sr.Count() )
        } // <- does not recycle the bucket array, because HeapAllocator::allowsMemSplit() is false
        UT_EQ( 5 , sr.Count() )
    }

    // MonoAllocator / shared recycler
    {
        MonoAllocator ma(ALIB_DBG("UTMaSR",) 1);
        using HS= HashSet<MonoAllocator, int   , std::hash<int> , std::equal_to<int> ,lang::Caching::Disabled, Recycling::Shared>;
        HS::SharedRecyclerType sr(ma);
        integer qtyRec;
        {
            HS hs1(sr);
            {
                HS hs2(sr);
                hs1.InsertUnique(11);                          UT_EQ( 0       , sr.Count() )   UT_EQ( 1, hs1.Size() )  UT_EQ( 0, hs2.Size() )
                sr.Reserve(2,lang::ValueReference::Absolute);  UT_EQ( 2       , sr.Count() )   UT_EQ( 1, hs1.Size() )  UT_EQ( 0, hs2.Size() )
                sr.Reserve(2,lang::ValueReference::Absolute);  UT_EQ( 2       , sr.Count() )   UT_EQ( 1, hs1.Size() )  UT_EQ( 0, hs2.Size() )
                sr.Reserve(1,lang::ValueReference::Relative);  UT_EQ( 3       , sr.Count() )   UT_EQ( 1, hs1.Size() )  UT_EQ( 0, hs2.Size() )
                hs1.InsertUnique(12);                          UT_EQ( 2       , sr.Count() )   UT_EQ( 2, hs1.Size() )  UT_EQ( 0, hs2.Size() )
                hs2.InsertUnique(21);                          UT_EQ( 1       , sr.Count() )   UT_EQ( 2, hs1.Size() )  UT_EQ( 1, hs2.Size() )
                hs2.InsertUnique(22);                          UT_EQ( 0       , sr.Count() )   UT_EQ( 2, hs1.Size() )  UT_EQ( 2, hs2.Size() )
                hs1.Erase(11);                                 UT_EQ( 1       , sr.Count() )   UT_EQ( 1, hs1.Size() )  UT_EQ( 2, hs2.Size() )
                hs2.Erase(21);                                 UT_EQ( 2       , sr.Count() )   UT_EQ( 1, hs1.Size() )  UT_EQ( 1, hs2.Size() )
                hs1.Reset();  qtyRec= sr.Count();               UT_TRUE( qtyRec > 4 || ALIB_DEBUG_ALLOCATIONS)

                                                               UT_EQ( qtyRec  , sr.Count() )   UT_EQ( 0, hs1.Size() )  UT_EQ( 1, hs2.Size() )
                hs1.InsertUnique(13);                          UT_EQ( qtyRec-1, sr.Count() )   UT_EQ( 1, hs1.Size() )  UT_EQ( 1, hs2.Size() )
                hs1.InsertUnique(14);                          UT_EQ( qtyRec-2, sr.Count() )   UT_EQ( 2, hs1.Size() )  UT_EQ( 1, hs2.Size() )
                sr.Reset();                                    UT_EQ( 0       , sr.Count() )   UT_EQ( 2, hs1.Size() )  UT_EQ( 1, hs2.Size() )
                hs2.InsertUnique(23);                          UT_EQ( 0       , sr.Count() )   UT_EQ( 2, hs1.Size() )  UT_EQ( 2, hs2.Size() )
                hs2.InsertUnique(24);                          UT_EQ( 0       , sr.Count() )   UT_EQ( 2, hs1.Size() )  UT_EQ( 3, hs2.Size() )
            } // <- around 50 recyclables should be received with deleting hs2's bucket array
            UT_TRUE( sr.Count() > 30 || ALIB_DEBUG_ALLOCATIONS )
            UT_TRUE( sr.Count() < 70 || ALIB_DEBUG_ALLOCATIONS )
        } // <- around 50 recyclables should be received with deleting hs1's bucket array
        UT_TRUE( sr.Count() > 70 || ALIB_DEBUG_ALLOCATIONS )
    }

    // PoolAllocator / shared recycler
    {
        PoolAllocatorHA pa;
        using HS= HashSet<PoolAllocatorHA, int   , std::hash<int> , std::equal_to<int> ,lang::Caching::Disabled, Recycling::Shared>;
        HS::SharedRecyclerType sr(pa);
        {
            HS hs1(sr);
            {
                HS hs2(sr);
                hs1.InsertUnique(11);                          UT_EQ( 0 , sr.Count() )   UT_EQ( 1, hs1.Size() )  UT_EQ( 0, hs2.Size() )
                sr.Reserve(2,lang::ValueReference::Absolute);  UT_EQ( 2 , sr.Count() )   UT_EQ( 1, hs1.Size() )  UT_EQ( 0, hs2.Size() )
                sr.Reserve(2,lang::ValueReference::Absolute);  UT_EQ( 2 , sr.Count() )   UT_EQ( 1, hs1.Size() )  UT_EQ( 0, hs2.Size() )
                sr.Reserve(1,lang::ValueReference::Relative);  UT_EQ( 3 , sr.Count() )   UT_EQ( 1, hs1.Size() )  UT_EQ( 0, hs2.Size() )
                hs1.InsertUnique(12);                          UT_EQ( 2 , sr.Count() )   UT_EQ( 2, hs1.Size() )  UT_EQ( 0, hs2.Size() )
                hs2.InsertUnique(21);                          UT_EQ( 1 , sr.Count() )   UT_EQ( 2, hs1.Size() )  UT_EQ( 1, hs2.Size() )
                hs2.InsertUnique(22);                          UT_EQ( 0 , sr.Count() )   UT_EQ( 2, hs1.Size() )  UT_EQ( 2, hs2.Size() )
                hs1.Erase(11);                                 UT_EQ( 1 , sr.Count() )   UT_EQ( 1, hs1.Size() )  UT_EQ( 2, hs2.Size() )
                hs2.Erase(21);                                 UT_EQ( 2 , sr.Count() )   UT_EQ( 1, hs1.Size() )  UT_EQ( 1, hs2.Size() )
                hs1.Reset();                                   UT_EQ( 3 , sr.Count() )   UT_EQ( 0, hs1.Size() )  UT_EQ( 1, hs2.Size() )
                hs1.InsertUnique(13);                          UT_EQ( 2 , sr.Count() )   UT_EQ( 1, hs1.Size() )  UT_EQ( 1, hs2.Size() )
                hs1.InsertUnique(14);                          UT_EQ( 1 , sr.Count() )   UT_EQ( 2, hs1.Size() )  UT_EQ( 1, hs2.Size() )
                sr.Reset();                                    UT_EQ( 0 , sr.Count() )   UT_EQ( 2, hs1.Size() )  UT_EQ( 1, hs2.Size() )
                hs2.InsertUnique(23);                          UT_EQ( 0 , sr.Count() )   UT_EQ( 2, hs1.Size() )  UT_EQ( 2, hs2.Size() )
                hs2.InsertUnique(24);                          UT_EQ( 0 , sr.Count() )   UT_EQ( 2, hs1.Size() )  UT_EQ( 3, hs2.Size() )
            } // <- does not recycle the bucket array, because PoolAllocator::allowsMemSplit() is false
            UT_EQ( 3 , sr.Count() )
        } // <- does not recycle the bucket array, because PoolAllocator::allowsMemSplit() is false
        UT_EQ( 5 , sr.Count() )
    }

}


//--------------------------------------------------------------------------------------------------
//--- HashTable
//--------------------------------------------------------------------------------------------------
UT_METHOD(HashTable)
{
    UT_INIT()

    static_assert( HashSet<MonoAllocator, int          >::CachedHashCodes == false, "Wrong default for caching" );
    static_assert( HashSet<MonoAllocator, integer      >::CachedHashCodes == false, "Wrong default for caching" );
    static_assert( HashSet<MonoAllocator, float        >::CachedHashCodes == false, "Wrong default for caching" );
    static_assert( HashSet<MonoAllocator, double       >::CachedHashCodes == false, "Wrong default for caching" );
    static_assert( HashSet<MonoAllocator, Box          >::CachedHashCodes == true , "Wrong default for caching" );
    static_assert( HashSet<MonoAllocator, String       >::CachedHashCodes == true , "Wrong default for caching" );

    static_assert( HashSet<MonoAllocator, int    , int >::CachedHashCodes == false, "Wrong default for caching" );
    static_assert( HashSet<MonoAllocator, integer, int >::CachedHashCodes == false, "Wrong default for caching" );
    static_assert( HashSet<MonoAllocator, float  , int >::CachedHashCodes == false, "Wrong default for caching" );
    static_assert( HashSet<MonoAllocator, double , int >::CachedHashCodes == false, "Wrong default for caching" );
    static_assert( HashSet<MonoAllocator, Box    , int >::CachedHashCodes == true , "Wrong default for caching" );
    static_assert( HashSet<MonoAllocator, String , int >::CachedHashCodes == true , "Wrong default for caching" );

#if !defined(ALIB_UT_REDUCED_COMPILE_TIME)

    MonoAllocator ma(ALIB_DBG("UTHTDYNINT",) 1);
    (void) ( DynInt() == DynInt() ); // avoid unneeded constructor, == warning for this test class

    {
        UT_PRINT( "---------------------------- HashSet -------------------------------")
        ma.Reset();   { HashSet<MonoAllocator,int           , IntHash       , std::equal_to<int> ,lang::Caching::Disabled,Recycling::Private> hashSetI(ma);   UT_HashSet( ut, hashSetI ); }
        ma.Reset();   { HashSet<MonoAllocator,int           , IntHash       , std::equal_to<int> ,lang::Caching::Disabled,Recycling::None   > hashSetI(ma);   UT_HashSet( ut, hashSetI ); }
        ma.Reset();   { HashSet<MonoAllocator,DynInt        , DynIntHash    , DynIntEqual        ,lang::Caching::Disabled,Recycling::Private> hashSetI(ma);   UT_HashSet( ut, hashSetI ); }
        ma.Reset();   { HashSet<MonoAllocator,DynInt        , DynIntHash    , DynIntEqual        ,lang::Caching::Disabled,Recycling::None   > hashSetI(ma);   UT_HashSet( ut, hashSetI ); }
        UT_EQ( DynInt::instCounter, 0)
        ma.Reset();   { HashSet<MonoAllocator,int           , IntHash       , std::equal_to<int> ,lang::Caching::Enabled > hashSetI(ma);   UT_HashSet( ut, hashSetI ); }
        ma.Reset();   { HashSet<MonoAllocator,DynInt        , DynIntHash    , DynIntEqual        ,lang::Caching::Enabled > hashSetI(ma);   UT_HashSet( ut, hashSetI ); }
        UT_EQ( DynInt::instCounter, 0)

        UT_PRINT( "---------------------------- HashSet Heap Allocated -------------------------------")
        ma.Reset();   { HashSet<HeapAllocator,int           , IntHash       , std::equal_to<int> ,lang::Caching::Disabled,Recycling::Private> hashSetI;       UT_HashSet( ut, hashSetI ); }
        ma.Reset();   { HashSet<HeapAllocator,int           , IntHash       , std::equal_to<int> ,lang::Caching::Disabled,Recycling::None   > hashSetI;       UT_HashSet( ut, hashSetI ); }
        ma.Reset();   { HashSet<HeapAllocator,DynInt        , DynIntHash    , DynIntEqual        ,lang::Caching::Disabled,Recycling::Private> hashSetI;       UT_HashSet( ut, hashSetI ); }
        ma.Reset();   { HashSet<HeapAllocator,DynInt        , DynIntHash    , DynIntEqual        ,lang::Caching::Disabled,Recycling::None   > hashSetI;       UT_HashSet( ut, hashSetI ); }
        UT_EQ( DynInt::instCounter, 0)
        ma.Reset();   { HashSet<HeapAllocator,int           , IntHash       , std::equal_to<int> ,lang::Caching::Enabled                    > hashSetI;       UT_HashSet( ut, hashSetI ); }
        ma.Reset();   { HashSet<HeapAllocator,DynInt        , DynIntHash    , DynIntEqual        ,lang::Caching::Enabled                    > hashSetI;       UT_HashSet( ut, hashSetI ); }
        UT_EQ( DynInt::instCounter, 0)

        UT_PRINT( "---------------------------- HashMap -------------------------------")
        ma.Reset();   { HashMap<MonoAllocator,int   , int   , IntHash       , std::equal_to<int> ,lang::Caching::Disabled> hashMapII(ma);  UT_HashMap( ut, hashMapII ); }
        ma.Reset();   { HashMap<MonoAllocator,int   , DynInt, IntHash       , std::equal_to<int> ,lang::Caching::Disabled> hashMapID(ma);  UT_HashMap( ut, hashMapID ); }
        UT_EQ( DynInt::instCounter, 0)
        ma.Reset();   { HashMap<MonoAllocator,DynInt, int   , DynIntHash    , DynIntEqual        ,lang::Caching::Disabled> hashMapDI(ma);  UT_HashMap( ut, hashMapDI ); }
        UT_EQ( DynInt::instCounter, 0)
        ma.Reset();   { HashMap<MonoAllocator,DynInt, DynInt, DynIntHash    , DynIntEqual        ,lang::Caching::Disabled> hashMapDD(ma);  UT_HashMap( ut, hashMapDD ); }
        UT_EQ( DynInt::instCounter, 0)
        ma.Reset();   { HashMap<MonoAllocator,int   , int   , IntHash       , std::equal_to<int> ,lang::Caching::Enabled > hashMapII(ma);  UT_HashMap( ut, hashMapII ); }
        ma.Reset();   { HashMap<MonoAllocator,int   , DynInt, IntHash       , std::equal_to<int> ,lang::Caching::Enabled > hashMapID(ma);  UT_HashMap( ut, hashMapID ); }
        UT_EQ( DynInt::instCounter, 0)
        ma.Reset();   { HashMap<MonoAllocator,DynInt, int   , DynIntHash    , DynIntEqual        ,lang::Caching::Enabled > hashMapDI(ma);  UT_HashMap( ut, hashMapDI ); }
        UT_EQ( DynInt::instCounter, 0)
        ma.Reset();   { HashMap<MonoAllocator,DynInt, DynInt, DynIntHash    , DynIntEqual        ,lang::Caching::Enabled > hashMapDD(ma);  UT_HashMap( ut, hashMapDD ); }
        UT_EQ( DynInt::instCounter, 0)
    }
#endif //if !defined(ALIB_UT_REDUCED_COMPILE_TIME)
}

#include "unittests/aworx_unittests_end.hpp"

} //namespace [ut_aworx]

#endif // ALIB_UT_MONOMEM
