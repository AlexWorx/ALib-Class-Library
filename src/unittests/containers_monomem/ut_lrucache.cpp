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
#include "alib/containers/lrucachetable.hpp"
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

#define TESTCLASSNAME       UT_ContMonoLRUCache
#include "unittests/aworx_unittests.hpp"

#if defined(__clang__)
    #pragma clang diagnostic ignored "-Wunused-function"
#elif defined(__GNUC__)
    #pragma GCC diagnostic ignored "-Wunused-function"
#elif defined(_MSC_VER)

#endif

using namespace std;
using namespace alib;

namespace ut_aworx {
namespace {
#include "alib/lang/callerinfo_functions.hpp"

//--------------------------------------------------------------------------------------------------
//--- LRUCacheTable test preparations
//--------------------------------------------------------------------------------------------------


// ----------------   types used to test LRUCacheSet  ----------------
struct TestLRUSetValue
{
    TestLRUSetValue(int pKey)                      : key(pKey)      { value= new double(pKey*100);  }
    TestLRUSetValue(const  TestLRUSetValue&  copy) : key(copy.key)  { value= new double(copy.key*100);  }
    ~TestLRUSetValue()                                              { delete value;  }
    int      key;
    double*  value;
    bool operator==(const TestLRUSetValue& other )   const { return key == other.key; }
    bool operator!=(const TestLRUSetValue& other )   const { return key != other.key; }
};
struct TestLRUSetValueHash
{
    size_t operator()(const TestLRUSetValue& src) const
    {
        return size_t(src.key * 31 + *src.value);
    }

};

// ----------------   types used to test embedded keys  ----------------
struct TestLRUEmbeddedKeyValueDescriptor
{
    using StoredType    = TestLRUSetValue;
    using KeyType       = int;
    using MappedType    = double&;

    KeyType     Key   (TestLRUSetValue& src)           const { return src.key;    }
    MappedType  Mapped(TestLRUSetValue& src)           const { return *src.value; }
};

// ----------------   result test function  ----------------
template<typename TLRUCache>
void testCache( AWorxUnitTesting& ut, TLRUCache& cache, integer expectedSize )
{
    int cnt= 0;
    for(auto it= cache.begin() ; it != cache.end() ; ++it)
        cnt++;

    UT_EQ( expectedSize, cnt )
    UT_EQ( expectedSize, cache.Size() )
}


} //anonymous namespace

// #################################################################################################
// ### Static compile-time tests of type definitions
// #################################################################################################
struct StaticTest_LRUMap
{
    using    set1=                alib::LRUCacheMap<lang::HeapAllocator, String, int >;
    using    set2= containers::LRUCacheMap<lang::HeapAllocator, String, int >;
    using    set3= containers::LRUCacheTable< lang::HeapAllocator,
                                                  containers::TPairDescriptor<String,int>>;


    UT_TEQ(set1               , set2)
    UT_TEQ(set1               , set3)
    UT_TEQ(set1::StoredType   , std::pair<String ALIB_COMMA int>)
    UT_TEQ(set1::KeyType      , String)
    UT_TEQ(set1::MappedType   , int   )
    UT_TEQ(set1::HashType     , std::hash<String>)
    UT_TEQ(set1::EqualType    , std::equal_to<String>)
    UT_TEQ(set1::DescriptorType, containers::TPairDescriptor<String ALIB_COMMA int>)
};

struct StaticTest_LRUSet
{
    using    set1=                alib::LRUCacheSet<lang::HeapAllocator, TestLRUSetValue>;
    using    set2= containers::LRUCacheSet<lang::HeapAllocator, TestLRUSetValue>;
    using    set3= containers::LRUCacheTable< lang::HeapAllocator,
                                                  containers::TIdentDescriptor<TestLRUSetValue>>;


    UT_TEQ(set1               , set2)
    UT_TEQ(set1               , set3)
    UT_TEQ(set1::StoredType   , TestLRUSetValue)
    UT_TEQ(set1::KeyType      , TestLRUSetValue)
    UT_TEQ(set1::MappedType   , TestLRUSetValue)
    UT_TEQ(set1::HashType     , std::hash<TestLRUSetValue>)
    UT_TEQ(set1::EqualType    , std::equal_to<TestLRUSetValue>)
    UT_TEQ(set1::DescriptorType, containers::TIdentDescriptor<TestLRUSetValue>)
};


struct StaticTest_LRUCacheEmbeddedKey
{ 
    using    set1=                alib::LRUCacheTable<lang::HeapAllocator, TestLRUEmbeddedKeyValueDescriptor>;
    using    set2= containers::LRUCacheTable<lang::HeapAllocator, TestLRUEmbeddedKeyValueDescriptor>;

    UT_TEQ(set1               , set2)
    UT_TEQ(set1::StoredType   , TestLRUSetValue)
    UT_TEQ(set1::KeyType      , int)
    UT_TEQ(set1::MappedType   , double&)
    UT_TEQ(set1::HashType     , std::hash<int>)
    UT_TEQ(set1::EqualType    , std::equal_to<int>)
    UT_TEQ(set1::DescriptorType, TestLRUEmbeddedKeyValueDescriptor)
};

#include "alib/lang/callerinfo_methods.hpp"

    
// #################################################################################################
// ### UT_CLASS
// #################################################################################################
UT_CLASS

//--------------------------------------------------------------------------------------------------
//--- LRUCacheTable
//--------------------------------------------------------------------------------------------------
UT_METHOD(Base)
{
    UT_INIT()
    //-------- LRUCacheMap: Accessing the cache and printing the result --------
    using TestLRUMap= LRUCacheMap<lang::HeapAllocator, int, int>;
    {
        TestLRUMap cache(1, 3);
    }

    {
        TestLRUMap cache(1, 3);                             testCache( ut, cache, 0);
        std::pair<bool, TestLRUMap::Iterator> rp;
        rp= cache.Try(1); UT_FALSE(rp.first)  rp.second.Construct(1, 100);    testCache( ut, cache, 1);
        rp= cache.Try(1); UT_TRUE (rp.first)  UT_EQ(100, rp.second.Mapped())  testCache( ut, cache, 1);
        rp= cache.Try(2); UT_FALSE(rp.first)  rp.second.Construct(2, 200);    testCache( ut, cache, 2);
        rp= cache.Try(2); UT_TRUE (rp.first)  UT_EQ(200, rp.second.Mapped())  testCache( ut, cache, 2);
        rp= cache.Try(1); UT_TRUE (rp.first)  UT_EQ(100, rp.second.Mapped())  testCache( ut, cache, 2);
        rp= cache.Try(3); UT_FALSE(rp.first)  rp.second.Construct(3, 300);    testCache( ut, cache, 3);
        rp= cache.Try(1); UT_TRUE (rp.first)  UT_EQ(100, rp.second.Mapped())  testCache( ut, cache, 3);
        rp= cache.Try(1); UT_TRUE (rp.first)  UT_EQ(100, rp.second.Mapped())  testCache( ut, cache, 3);
        rp= cache.Try(3); UT_TRUE (rp.first)  UT_EQ(300, rp.second.Mapped())  testCache( ut, cache, 3);
        rp= cache.Try(2); UT_TRUE (rp.first)  UT_EQ(200, rp.second.Mapped())  testCache( ut, cache, 3);
        rp= cache.Try(3); UT_TRUE (rp.first)  UT_EQ(300, rp.second.Mapped())  testCache( ut, cache, 3);
        rp= cache.Try(2); UT_TRUE (rp.first)  UT_EQ(200, rp.second.Mapped())  testCache( ut, cache, 3);
        rp= cache.Try(1); UT_TRUE (rp.first)  UT_EQ(100, rp.second.Mapped())  testCache( ut, cache, 3);
    }
    //-------- LRUCacheSet Accessing the cache and printing the result --------
    using TestLRUSet= LRUCacheSet<lang::HeapAllocator, TestLRUSetValue, TestLRUSetValueHash>;
    {
        TestLRUSet cache(1, 3);
    }

    {
        TestLRUSet cache(1, 3);                                            testCache( ut, cache, 0);
        std::pair<bool, TestLRUSet::Iterator> rp;
        static_assert(ATMP_EQ(TestLRUSet::HashType, TestLRUSetValueHash), "error" );
        rp= cache.Try(1); UT_FALSE(rp.first)  rp.second.Construct(1);                        testCache( ut, cache, 1);
        rp= cache.Try(1); UT_TRUE (rp.first)  UT_EQ(TestLRUSetValue(1), rp.second.Mapped())  testCache( ut, cache, 1);
        rp= cache.Try(2); UT_FALSE(rp.first)  rp.second.Construct(2);                        testCache( ut, cache, 2);
        rp= cache.Try(2); UT_TRUE (rp.first)  UT_EQ(TestLRUSetValue(2), rp.second.Mapped())  testCache( ut, cache, 2);
        rp= cache.Try(1); UT_TRUE (rp.first)  UT_EQ(TestLRUSetValue(1), rp.second.Mapped())  testCache( ut, cache, 2);
        rp= cache.Try(3); UT_FALSE(rp.first)  rp.second.Construct(3);                        testCache( ut, cache, 3);
        rp= cache.Try(1); UT_TRUE (rp.first)  UT_EQ(TestLRUSetValue(1), rp.second.Mapped())  testCache( ut, cache, 3);
        rp= cache.Try(1); UT_TRUE (rp.first)  UT_EQ(TestLRUSetValue(1), rp.second.Mapped())  testCache( ut, cache, 3);
        rp= cache.Try(3); UT_TRUE (rp.first)  UT_EQ(TestLRUSetValue(3), rp.second.Mapped())  testCache( ut, cache, 3);
        rp= cache.Try(2); UT_TRUE (rp.first)  UT_EQ(TestLRUSetValue(2), rp.second.Mapped())  testCache( ut, cache, 3);
        rp= cache.Try(3); UT_TRUE (rp.first)  UT_EQ(TestLRUSetValue(3), rp.second.Mapped())  testCache( ut, cache, 3);
        rp= cache.Try(2); UT_TRUE (rp.first)  UT_EQ(TestLRUSetValue(2), rp.second.Mapped())  testCache( ut, cache, 3);
        rp= cache.Try(1); UT_TRUE (rp.first)  UT_EQ(TestLRUSetValue(1), rp.second.Mapped())  testCache( ut, cache, 3);
    }

    //-------- LRU Embedded Key Accessing the cache and printing the result --------
    using TestLRUEmbeddedKey = LRUCacheTable<lang::HeapAllocator, TestLRUEmbeddedKeyValueDescriptor>;
    {
        TestLRUEmbeddedKey cache(1, 3);
    }

    {
        TestLRUEmbeddedKey cache(1, 3);                           testCache( ut, cache, 0);
        std::pair<bool, TestLRUEmbeddedKey::Iterator> rp;
        rp= cache.Try(1); UT_FALSE(rp.first)  rp.second.Construct(1);          testCache( ut, cache, 1);
        rp= cache.Try(1); UT_TRUE (rp.first)  UT_EQ(100., rp.second.Mapped())  testCache( ut, cache, 1);
        rp= cache.Try(2); UT_FALSE(rp.first)  rp.second.Construct(2);          testCache( ut, cache, 2);
        rp= cache.Try(2); UT_TRUE (rp.first)  UT_EQ(200., rp.second.Mapped())  testCache( ut, cache, 2);
        rp= cache.Try(1); UT_TRUE (rp.first)  UT_EQ(100., rp.second.Mapped())  testCache( ut, cache, 2);
        rp= cache.Try(3); UT_FALSE(rp.first)  rp.second.Construct(3);          testCache( ut, cache, 3);
        rp= cache.Try(1); UT_TRUE (rp.first)  UT_EQ(100., rp.second.Mapped())  testCache( ut, cache, 3);
        rp= cache.Try(1); UT_TRUE (rp.first)  UT_EQ(100., rp.second.Mapped())  testCache( ut, cache, 3);
        rp= cache.Try(3); UT_TRUE (rp.first)  UT_EQ(300., rp.second.Mapped())  testCache( ut, cache, 3);
        rp= cache.Try(2); UT_TRUE (rp.first)  UT_EQ(200., rp.second.Mapped())  testCache( ut, cache, 3);
        rp= cache.Try(3); UT_TRUE (rp.first)  UT_EQ(300., rp.second.Mapped())  testCache( ut, cache, 3);
        rp= cache.Try(2); UT_TRUE (rp.first)  UT_EQ(200., rp.second.Mapped())  testCache( ut, cache, 3);
        rp= cache.Try(1); UT_TRUE (rp.first)  UT_EQ(100., rp.second.Mapped())  testCache( ut, cache, 3);

        cache.Reserve(1,1);                                                    testCache( ut, cache, 0);
        rp= cache.Try(1); UT_FALSE(rp.first)  rp.second.Construct(1);          testCache( ut, cache, 1);
        rp= cache.Try(1); UT_TRUE (rp.first)  UT_EQ(100., rp.second.Mapped())  testCache( ut, cache, 1);
        rp= cache.Try(2); UT_FALSE(rp.first)  rp.second.Construct(2);          testCache( ut, cache, 1);
        rp= cache.Try(2); UT_TRUE (rp.first)  UT_EQ(200., rp.second.Mapped())  testCache( ut, cache, 1);
        rp= cache.Try(1); UT_FALSE(rp.first)  rp.second.Construct(1);          testCache( ut, cache, 1);
        rp= cache.Try(3); UT_FALSE(rp.first)  rp.second.Construct(3);          testCache( ut, cache, 1);
        rp= cache.Try(1); UT_FALSE(rp.first)  rp.second.Construct(1);          testCache( ut, cache, 1);
        rp= cache.Try(3); UT_FALSE(rp.first)  rp.second.Construct(3);          testCache( ut, cache, 1);
        rp= cache.Try(2); UT_FALSE(rp.first)  rp.second.Construct(2);          testCache( ut, cache, 1);
        rp= cache.Try(3); UT_FALSE(rp.first)  rp.second.Construct(3);          testCache( ut, cache, 1);
        rp= cache.Try(2); UT_FALSE(rp.first)  rp.second.Construct(2);          testCache( ut, cache, 1);
        rp= cache.Try(1); UT_FALSE(rp.first)  rp.second.Construct(1);          testCache( ut, cache, 1);
    }
}



    
#include "unittests/aworx_unittests_end.hpp"

} //namespace [ut_aworx]

#endif // ALIB_UT_MONOMEM
