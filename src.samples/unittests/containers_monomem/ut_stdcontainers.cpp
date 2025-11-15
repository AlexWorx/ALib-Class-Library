// #################################################################################################
//  AWorx ALib Unit Tests
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib_precompile.hpp"
#include "alib_test_selection.hpp"
#if ALIB_UT_MONOMEM
#include <unordered_map>
#include <map>
#include <utility>
#include <assert.h>
#include <list>


#include "ALib.Monomem.H"
#include "ALib.Monomem.SharedMonoVal.H"
#include "ALib.Containers.HashTable.H"
#include "ALib.Containers.LRUCacheTable.H"
#include "ALib.Resources.H"
#include "ALib.Camp.Base.H"
#include "ALib.Lang.H"
#include "ALib.Monomem.StdContainers.H"

#include "ALib.Strings.StdFunctors.H"
#define TESTCLASSNAME       UT_ContMonoStdContainers
#include "aworx_unittests.hpp"

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
// ########################################################################################
// StdVector reset sample
// ########################################################################################

using MyData= int;

DOX_MARKER( [DOX_MONOMEM_STDMA_PLACEMENT_NEW] )
// field members  or global objects
alib::MonoAllocator allocator( ALIB_DBG("MyAllocator",)  4);
std::vector<MyData, StdMA<MyData>> transactionObjects( allocator );

// method using the allocator and the vector
void processTransaction( /* transaction data */)
{
    // perform transaction, collecting stuff in vector and/or allocator
    // ...
    // ...
    // ...

    // before we leave we reset the vector and the allocator:

    // 1. Destruct the container and contained objects.
    //    (In case contained objects are trivially destructible, this is not
    //    needed and usually optimized out by the compiler.)
    allocator().Delete(&transactionObjects);

    // 2. Reset allocator
    allocator.Reset();

    // 3. Placement new on the vector object
    new( &transactionObjects ) std::vector<MyData, StdMA<MyData>>( allocator );
}

DOX_MARKER( [DOX_MONOMEM_STDMA_PLACEMENT_NEW] )


} //anonymous namespace

// #################################################################################################
// ### UT_CLASS
// #################################################################################################
UT_CLASS

UT_METHOD(StdContainers)
{
UT_INIT()

// #################################################################################################
// ### StdAllocator
// #################################################################################################
{
    MonoAllocator ma( ALIB_DBG("UTStdContainers",)  1 );
    {
        UT_PRINT(  "---- std::unordered_map, strict monotonic  ----")
        StdMA<std::pair<const AString,long double>> stdMA(ma);

        std::unordered_map< AString, long double,
                            std::hash    <String>,
                            std::equal_to<String>,
                            StdMA<std::pair<const AString,long double>>>
            umap( 5  , std::hash<String>(), std::equal_to<String>(), stdMA );

        for( int i= 0; i < 20 ; ++i)
            umap.insert( std::pair<AString,int>(String128() << "Key" << i, i * 10) );

    }

    {
        UT_PRINT("-------   std::map, strict monotonic -------")
        std::map          < AString, double,
                            std::less<String>,
                            StdMA<std::pair<const AString, double> >  >
            omap( std::less<String>(), ma );


        for( int i= 0; i < 10 ; ++i)
            omap.insert( std::pair<AString,int>(String128() << "Key" << i, i * 10) );
        while( omap.begin() != omap.end() )
            omap.erase( omap.begin() );
        for( int i= 0; i < 15 ; ++i)
            omap.insert( std::pair<AString,int>(String128() << "Key" << i, i * 10) );
    }

    {
        UT_PRINT("-------   std::list, strict monotonic   -------")
        StdListMA<AString> list(ma);


        for( int i= 0; i < 5 ; ++i)
            list.push_front( String128() << "Key" << i );
        while( list.begin() != list.end() )
            list.erase( list.begin() );
        for( int i= 0; i < 7 ; ++i)
            list.push_front( String128() << "Key" << i );
    }

    {
        UT_PRINT("-------  std::vector, strict monotonic  -------")
DOX_MARKER([DOX_MONOMEM_STDMA_DECL])
struct MyStruct
{
    std::vector<int, StdMA<int>>  myField;
};
DOX_MARKER([DOX_MONOMEM_STDMA_DECL])

DOX_MARKER([DOX_MONOMEM_STDMA_DEF])
alib::MonoAllocator myAllocator(ALIB_DBG("MyAllocator",) 4);
std::vector<int, StdMA<int>>  myVector(myAllocator);
DOX_MARKER([DOX_MONOMEM_STDMA_DEF])

        for( int i= 0; i < 20 ; ++i)
            myVector.push_back( i );
    }
struct MyStructAlt
{
DOX_MARKER([DOX_MONOMEM_STDMA_DEF_ALTERNATIVE])
    StdVectorMA<int>  myField;
DOX_MARKER([DOX_MONOMEM_STDMA_DEF_ALTERNATIVE])
};
    
}

// #################################################################################################
// ### StdRecyclingAllocator
// #################################################################################################
{
    MonoAllocator ma(ALIB_DBG("UTCAR",)  1 );
    lang::RTTRAllocator<MonoAllocator> recyclerUM( ma );
    lang::RTTRAllocator<MonoAllocator> recyclerMap( ma );
    lang::RTTRAllocator<MonoAllocator> recyclerVec( ma );
    lang::RTTRAllocator<MonoAllocator> recyclerList( ma );
    lang::StdRecyclingAllocator<std::pair<const AString,long double>,MonoAllocator> rum( recyclerUM );

    {
        UT_PRINT(  "---- std::unordered_map, recycling  ----")
        std::unordered_map< AString, long double,
                            std::hash    <String>,
                            std::equal_to<String>,
                            lang::StdRecyclingAllocator<std::pair<const AString, long double>,MonoAllocator>> umap( 5, std::hash<String>(), std::equal_to<String>(), rum );
        for( int i= 0; i < 20 ; ++i)
            umap.insert( std::pair<AString,int>(String128() << "Key" << i, i * 10) );
    }

    {
        UT_PRINT("-------   std::map, recycling    -------")
        std::map          < AString, double,
                            std::less<String>,
                            lang::StdRecyclingAllocator<std::pair<const AString, double>,MonoAllocator>  > omap( std::less<String>(), (lang::StdRecyclingAllocator<std::pair<const AString, double>,MonoAllocator>(recyclerUM)) );


        for( int i= 0; i < 10 ; ++i)
            omap.insert( std::pair<AString,int>(String128() << "Key" << i, i * 10) );
        while( omap.begin() != omap.end() )
            omap.erase( omap.begin() );
        for( int i= 0; i < 15 ; ++i)
            omap.insert( std::pair<AString,int>(String128() << "Key" << i, i * 10) );
    }

    {
        UT_PRINT("-------   std::list, recycling    -------")
        std::list         < AString,
                            lang::StdRecyclingAllocator<AString,MonoAllocator>  > list( (lang::StdRecyclingAllocator<AString,MonoAllocator>(recyclerList)) );


        for( int i= 0; i < 5 ; ++i)
            list.push_front( String128() << "Key" << i );
        while( list.begin() != list.end() )
            list.erase( list.begin() );
        for( int i= 0; i < 7 ; ++i)
            list.push_front( String128() << "Key" << i );
    }

}
}

#include "aworx_unittests_end.hpp"

} //namespace [ut_aworx]

#endif // ALIB_UT_MONOMEM
