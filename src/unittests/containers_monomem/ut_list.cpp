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

#define TESTCLASSNAME       UT_ContMonoList
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
#include "alib/lang/callerinfo_functions.hpp"
    namespace
    {
#if !defined(ALIB_UT_REDUCED_COMPILE_TIME)
        
//#define DBGOUT(val)   cout << val << endl;
#define DBGOUT( val )

// Versions of a simple char container used to ensure correct deletion of objects
// when unit tests are run with a memory checker (e.g., valgrind).

        class CharTriv
        {
            private:
            char val;
            public:
            CharTriv()  = default;
            CharTriv( char c ) : val( c )     {}

            char Value()       const          { return val; }
        };

        class CharDyn
        {
            private:
            static integer instCounter;
            char *val;

            public:
            CharDyn( char v ) : val( new char( v ) )
            { DBGOUT( "v" ); ++instCounter; }

            CharDyn( const CharDyn & c ) noexcept : val( new char( *c.val ) )
            { DBGOUT( "cc" ); ++instCounter; }

            CharDyn( CharDyn && c ) noexcept : val( std::move( c.val ) )
            { c.val= nullptr; ++instCounter; DBGOUT( "mc" ) }

            CharDyn & operator=( const CharDyn & c ) noexcept
            {
                DBGOUT( "ca" )            *val = *c.val;
                return *this;
            }
//    CharDyn& operator= (       CharDyn&&  c) noexcept                                        {DBGOUT( "ma" ) delete val; val=  c.val; c.val= nullptr; return *this; }

            ~CharDyn()
            {
                --instCounter;
                if ( val )
                {
                    delete val;
                    val = nullptr;
                }
            }


            char Value()                      { return *val; }

            static integer InstanceCounter() { return instCounter; }
        };
integer CharDyn::instCounter= 0;

// generic access functions
        char Value( char & c )
        { return c; }

//char Value( char* c )    { return *c; }
        char Value( const CharTriv& c )
        { return c.Value(); }

//char Value( * c ) { return c->Value(); }
        char Value( CharDyn & c )
        { return c.Value(); }
//char Value( CharDyn* c ) { return c->Value(); }

#undef DBGOUT


        static_assert( std::is_trivial<CharTriv>::value, "error" );
        static_assert( std::is_trivially_default_constructible<CharTriv>::value, "error" );
        static_assert( std::is_trivially_constructible<CharTriv>::value, "error" );
        static_assert( std::is_trivially_copy_constructible<CharTriv>::value, "error" );
        static_assert( std::is_trivially_move_constructible<CharTriv>::value, "error" );
        static_assert( std::is_trivially_copyable<CharTriv>::value, "error" );
        static_assert( std::is_trivially_copy_assignable<CharTriv>::value, "error" );
        static_assert( std::is_trivially_move_assignable<CharTriv>::value, "error" );


        static_assert( std::is_copy_assignable<CharTriv>::value, "error" );
        static_assert( std::is_copy_assignable<CharDyn>::value, "error" );
        static_assert( std::is_nothrow_copy_assignable<CharTriv>::value, "error" );
        static_assert( std::is_nothrow_copy_assignable<CharDyn>::value, "error" );

        static_assert( std::is_copy_constructible<CharTriv>::value, "error" );
        static_assert( std::is_copy_constructible<CharDyn>::value, "error" );

        static_assert( std::is_nothrow_copy_constructible<CharTriv>::value, "error" );
        static_assert( std::is_nothrow_copy_constructible<CharDyn>::value, "error" );



        static_assert( std::is_move_assignable<CharTriv>::value, "error" );
        static_assert( std::is_move_assignable<CharDyn>::value, "error" );
        static_assert( std::is_nothrow_move_assignable<CharTriv>::value, "error" );
        static_assert( std::is_nothrow_move_assignable<CharDyn>::value, "error" );

        static_assert( std::is_move_constructible<CharTriv>::value, "error" );
        static_assert( std::is_move_constructible<CharDyn>::value, "error" );

        static_assert( std::is_nothrow_move_constructible<CharTriv>::value, "error" );
        static_assert( std::is_nothrow_move_constructible<CharDyn>::value, "error" );
#endif //if !defined(ALIB_UT_REDUCED_COMPILE_TIME)

    // ########################################################################################
    // List helper function
    // ########################################################################################
#if !defined(ALIB_UT_REDUCED_COMPILE_TIME)
    template<typename TChar>
    void testCharList( AWorxUnitTesting & ut, List<MonoAllocator, TChar> & list,
                       const NString & exp, integer recyclablesCount )
    {
        UT_EQ( exp.Length(), list.Count() )
        UT_EQ( recyclablesCount, list.RecyclablesCount() )
        if ( exp.IsEmpty() )
        {
            UT_TRUE( list.IsEmpty() )
            UT_TRUE( list.begin() == list.end() )
            UT_TRUE( list.rbegin() == list.rend() )
            return;
        }
        else
            UT_FALSE( list.IsEmpty() )

        // forward and reverse forward iterators
        auto It1 = list.begin();
        auto It2 = list.begin();
        auto revIt1 = list.rbegin();
        auto revIt2 = list.rbegin();
        UT_TRUE( It1 != list.end() )
        UT_TRUE( revIt1 != list.rend() )

        // iterators that are decreased in the loop
        auto BackIt1 = list.end();
        auto BackIt2 = list.end();
        auto revBackIt1 = list.rend();
        auto revBackIt2 = list.rend();
        UT_TRUE( BackIt1 != list.begin() )
        UT_TRUE( revBackIt1 != list.rbegin() )

        for ( integer i = 0; i < exp.Length(); ++i )
        {
            UT_EQ( Value( *It1 ), exp[i] )
            UT_EQ( Value( *It2 ), exp[i] )
            UT_EQ( Value( *revIt1 ), exp[exp.Length() - 1 - i] )
            UT_EQ( Value( *revIt2 ), exp[exp.Length() - 1 - i] )

            if ( i != 0 )
            {
                UT_EQ( exp[exp.Length() - i], Value( *BackIt1 ) )
                UT_EQ( exp[exp.Length() - i], Value( *BackIt2 ) )
                UT_EQ( exp[i - 1], Value( *revBackIt1 ) )
                UT_EQ( exp[i - 1], Value( *revBackIt2 ) )
            }

            UT_EQ( Value(*It1), Value(list.ElementAt( i )) )


            UT_TRUE( It1 == It2++ )
            ++It1;
            UT_TRUE( revIt1 == revIt2++ )
            ++revIt1;

            UT_TRUE( BackIt1 == BackIt2-- )
            --BackIt1;
            UT_TRUE( revBackIt1 == revBackIt2-- )
            --revBackIt1;

            UT_EQ( It1 == list.end(), i == exp.Length() - 1 )
            UT_EQ( It2 == list.end(), i == exp.Length() - 1 )
            UT_EQ( revIt1 == list.rend(), i == exp.Length() - 1 )
            UT_EQ( revIt2 == list.rend(), i == exp.Length() - 1 )

            UT_EQ( BackIt1 == list.begin(), i == exp.Length() - 1 )
            UT_EQ( BackIt2 == list.begin(), i == exp.Length() - 1 )
            UT_EQ( revBackIt1 == list.rbegin(), i == exp.Length() - 1 )
            UT_EQ( revBackIt2 == list.rbegin(), i == exp.Length() - 1 )

        }
    }

    // ########################################################################################
    // List test
    // ########################################################################################
    template<typename TChar>
    void TListTest( AWorxUnitTesting & ut )
    {
        MonoAllocator ba(ALIB_DBG("UTList",) 1);

        List<MonoAllocator, TChar> list( ba );
        testCharList( ut, list, "", 0 );

        // iterator conversion
        {
            typename List<MonoAllocator, TChar>::Iterator it = list.begin();
            typename List<MonoAllocator, TChar>::ConstIterator cit = list.begin();
            cit = it;

            const List<MonoAllocator, TChar> *cp = &list;
            auto cit2 = cp->begin();
            static_assert( ATMP_EQ( decltype( cit2 ), typename List<MonoAllocator ALIB_COMMA TChar>::ConstIterator ),
                           "Error" );
        }


        // insert/erase/iterate
        list.EmplaceBack( 'a' );                       testCharList( ut , list , "a"        , 0 );
 (void) list.Erase( list.begin() );                    testCharList( ut , list , ""         , 1 );

        list.EmplaceBack( 'a' );                       testCharList( ut , list , "a"        , 0 );
        list.EmplaceBack( 'b' );                       testCharList( ut , list , "ab"       , 0 );
 (void) list.Erase( list.begin() );                    testCharList( ut , list , "b"        , 1 );
 (void) list.Erase( list.begin() );                    testCharList( ut , list , ""         , 2 );

        list.EmplaceBack( 'a' );                       testCharList( ut , list , "a"        , 1 );
        list.EmplaceBack( 'b' );                       testCharList( ut , list , "ab"       , 0 );
 (void) list.Erase( --list.end() );                    testCharList( ut , list , "a"        , 1 );
 (void) list.Erase( --list.end() );                    testCharList( ut , list , ""         , 2 );

        list.EmplaceBack( 'a' );                       testCharList( ut , list , "a"        , 1 );
        list.EmplaceBack( 'b' );                       testCharList( ut , list , "ab"       , 0 );
        list.EmplaceBack( 'c' );                       testCharList( ut , list , "abc"      , 0 );
  (void) list.Erase( list.begin() );                    testCharList( ut , list , "bc"       , 1 );
  (void) list.Erase( list.begin() );                    testCharList( ut , list , "c"        , 2 );
  (void) list.Erase( list.begin() );                    testCharList( ut , list , ""         , 3 );

        list.EmplaceBack( 'a' );                       testCharList( ut , list , "a"        , 2 );
        list.EmplaceBack( 'b' );                       testCharList( ut , list , "ab"       , 1 );
        list.EmplaceBack( 'c' );                       testCharList( ut , list , "abc"      , 0 );
 (void) list.Erase( --list.end() );                    testCharList( ut , list , "ab"       , 1 );
 (void) list.Erase( --list.end() );                    testCharList( ut , list , "a"        , 2 );
 (void) list.Erase( --list.end() );                    testCharList( ut , list , ""         , 3 );

        list.EmplaceBack( 'a' );                       testCharList( ut , list , "a"        , 2 );
        list.EmplaceBack( 'b' );                       testCharList( ut , list , "ab"       , 1 );
        list.EmplaceBack( 'c' );                       testCharList( ut , list , "abc"      , 0 );

        auto it = list.end();
        it--;
        it = list.Erase( it );                         testCharList( ut , list , "ab"       , 1 );
        UT_TRUE( it == list.end() )
        list.EmplaceBack( 'c' );                       testCharList( ut , list , "abc"      , 0 );
        it = list.end()--;
        --it;
        --it;
        it = list.Erase( it );                         testCharList( ut , list , "ac"       , 1 );
        UT_EQ( 'c', Value( *it ) )
        it = list.Erase( it );                         testCharList( ut , list , "a"        , 2 );
        list.Clear();                                  testCharList( ut , list , ""         , 3 );
        list.EmplaceBack( 'a' );                       testCharList( ut , list , "a"        , 2 );
        list.EmplaceBack( 'b' );
        list.EmplaceBack( 'c' );                       testCharList( ut , list , "abc"      , 0 );
        list.Reset();                                  testCharList( ut , list , ""         , 0 );
        list.EmplaceBack( 'a' );
        list.EmplaceBack( 'b' );
        list.EmplaceBack( 'c' );                       testCharList( ut , list , "abc"      , 0 );
        it = list.Erase( list.begin(), list.end() );   testCharList( ut , list , ""         , 3 );
        UT_TRUE( it == list.end() )
        list.EmplaceBack( 'a' );
        list.EmplaceBack( 'b' );
        list.EmplaceBack( 'c' );                       testCharList( ut , list , "abc"      , 0 );
        it = list.Erase( list.begin(), --list.end() ); testCharList( ut , list , "c"        , 2 );

        UT_EQ( 'c', Value( *it ) )
        it = list.Erase( list.begin(), list.end() );
        UT_EQ  ( list.Count(), 0 )
        UT_EQ( 'a', Value( list.EmplaceBack( 'a' ) ) ) testCharList( ut , list , "a"        , 2 );
        UT_EQ( 'b', Value( list.EmplaceBack( 'b' ) ) ) testCharList( ut , list , "ab"       , 1 );
        UT_EQ( 'c', Value( list.EmplaceBack( 'c' ) ) ) testCharList( ut , list , "abc"      , 0 );
        it = list.Erase( ++list.begin(), list.end() ); testCharList( ut , list , "a"        , 2 );
        UT_TRUE( it == list.end() )
        it = list.Erase( list.begin() );
        it = list.Emplace( list.begin(), 'c' );        testCharList( ut , list , "c"        , 2 );   UT_EQ( 'c', Value( *it ) )
        it = list.Emplace( list.begin(), 'b' );        testCharList( ut , list , "bc"       , 1 );   UT_EQ( 'b', Value( *it ) )
        it = list.Emplace( list.begin(), 'a' );        testCharList( ut , list , "abc"      , 0 );   UT_EQ( 'a', Value( *it ) )
        list.Clear();                                  testCharList( ut , list , ""         , 3 );
        it = list.Emplace( list.begin(), 'b' );        testCharList( ut , list , "b"        , 2 );   UT_EQ( 'b', Value( *it ) )
        it = list.Emplace( list.end(), 'c' );          testCharList( ut , list , "bc"       , 1 );   UT_EQ( 'c', Value( *it ) )
        it = list.Emplace( list.begin(), 'a' );        testCharList( ut , list , "abc"      , 0 );   UT_EQ( 'a', Value( *it ) )
        it = list.Emplace( list.end(), 'd' );          testCharList( ut , list , "abcd"     , 0 );   UT_EQ( 'd', Value( *it ) )
        list.Reset();                                  testCharList( ut , list , ""         , 0 );
        it = list.Emplace( list.begin(), 'a' );        testCharList( ut , list , "a"        , 0 );  UT_EQ( 'a', Value( *it ) )
        it = list.Emplace( ++it, 'b' );                testCharList( ut , list , "ab"       , 0 );  UT_EQ( 'b', Value( *it ) )
        it = list.Emplace( ++it, 'c' );                testCharList( ut , list , "abc"      , 0 );  UT_EQ( 'c', Value( *it ) )
        list.Clear();                                  testCharList( ut , list , ""         , 3 );
        it = list.Emplace( list.begin(), 'b' );        testCharList( ut , list , "b"        , 2 );  UT_EQ( 'b', Value( *it ) )
        it = list.Emplace( ++it, 'c' );                testCharList( ut , list , "bc"       , 1 );  UT_EQ( 'c', Value( *it ) )
        it = list.Emplace( list.begin(), 'a' );        testCharList( ut , list , "abc"      , 0 );  UT_EQ( 'a', Value( *it ) )
        list.Clear();                                  testCharList( ut , list , ""         , 3 );

        list.ReserveRecyclables( 5, lang::ValueReference::Relative );
                                                       testCharList( ut , list , ""         , 5 );
        list.EmplaceBack( 'd' );                       testCharList( ut , list , "d"        , 4 );
        list.PushBack( 'e' );                          testCharList( ut , list , "de"       , 3 );
        list.PushFront( 'b' );                         testCharList( ut , list , "bde"      , 2 );
        list.Insert( ++list.begin(), 'c' );            testCharList( ut , list , "bcde"     , 1 );
        list.Insert( list.end(), 'f' );                testCharList( ut , list , "bcdef"    , 0 );
        list.EmplaceFront( 'a' );                      testCharList( ut , list , "abcdef"   , 0 );
        list.Insert( it = list.end(), 'g' );           testCharList( ut , list , "abcdefg"  , 0 );
        list.Insert( it, 'h' );                        testCharList( ut , list , "abcdefgh" , 0 );

        it = list.Erase( ++ ++list.begin(),
        -- --list.end() );                             testCharList( ut , list , "abgh"     , 4 );   UT_EQ( 'g', Value( *it ) )
    }
#endif

} //anonymous namespace
#include "alib/lang/callerinfo_methods.hpp"

// #################################################################################################
// #################################################################################################
// #################################################################################################
// #################################################################################################
// ### UT_CLASS
// #################################################################################################
// #################################################################################################
// #################################################################################################
// #################################################################################################
UT_CLASS

//--------------------------------------------------------------------------------------------------
//--- List
//--------------------------------------------------------------------------------------------------
#if !defined(ALIB_UT_REDUCED_COMPILE_TIME)
UT_METHOD(TestList)
{
    UT_INIT()

    CharTriv avoidUnneededDefaultConstructorWarning;
    (void)  avoidUnneededDefaultConstructorWarning;

    TListTest<char     >( ut );

    TListTest<CharTriv >( ut );

    UT_EQ( CharDyn::InstanceCounter(), 0 )
        TListTest<CharDyn >( ut );
    UT_EQ( CharDyn::InstanceCounter(), 0 )
}
#endif


//--------------------------------------------------------------------------------------------------
//--- Recycling
//--------------------------------------------------------------------------------------------------
UT_METHOD(TestListRecycling)
{
    // Note: the main purpose of this unit tests is to ensure that the  type definitions compile well.
    //       The fact that recycling in general works is tested directly and inherently with
    //       other tests.
    UT_INIT()

    {
        static_assert(   sizeof(List<HeapAllocator, int, Recycling::None   >)
                      <  sizeof(List<HeapAllocator, int, Recycling::Private>),
                      "Size of non-recycling instance should be smaller than recycling" );
        static_assert(   sizeof(List<HeapAllocator, int, Recycling::Private>)
                      == sizeof(List<HeapAllocator, int, Recycling::Shared >),
                      "Size of instances with private/public recycling should be equal" );

        static_assert(   sizeof(List<HeapAllocator, int, Recycling::None   >)
                      <  sizeof(List<MonoAllocator, int, Recycling::None   >),
                      "Size of list with heap allocator should be smaller than one with mono-allocator" );

        static_assert(   sizeof(List<MonoAllocator, int, Recycling::None   >)
                      == sizeof(List<PoolAllocator, int, Recycling::None   >),
                      "Size of lists with mono/pool allocators should be equal" );

        static_assert(   sizeof(List<HeapAllocator, int, Recycling::Shared   >)
                      == sizeof(List<MonoAllocator, int, Recycling::None   >),
                      "These sizes should also be equal. The first adds a recycler reference, "
                      "the other a allocator  reference." );
    }

    // List private
    {
        MonoAllocator ma(ALIB_DBG("UTListPrivate",) 1);
        alib::List<MonoAllocator, int>   list( ma );

                                UT_EQ( 0, list.RecyclablesCount() )
        list.PushFront(1);      UT_EQ( 0, list.RecyclablesCount() )
        list.PushFront(2);      UT_EQ( 0, list.RecyclablesCount() )
        list.PopFront();        UT_EQ( 1, list.RecyclablesCount() )
        list.PushFront(22);     UT_EQ( 0, list.RecyclablesCount() )

        list.PopFront();        UT_EQ( 1, list.RecyclablesCount() )
        list.PopFront();        UT_EQ( 2, list.RecyclablesCount() )
    }

    // List shared
    {
        MonoAllocator monoAllocator(ALIB_DBG("UTListShared",) 1);

        List<MonoAllocator, int, Recycling::Shared>::SharedRecyclerType sharedRecycler(monoAllocator);
        List<MonoAllocator, int, Recycling::Shared>  list1( sharedRecycler );
        List<MonoAllocator, int, Recycling::Shared>  list2( sharedRecycler );

                                 UT_EQ( 0, list1.RecyclablesCount() )
                                 UT_EQ( 0, list2.RecyclablesCount() )

        list1.PushFront(1);      UT_EQ( 0, list1.RecyclablesCount() )
                                 UT_EQ( 0, list2.RecyclablesCount() )

        list2.PushFront(1);      UT_EQ( 0, list1.RecyclablesCount() )
                                 UT_EQ( 0, list2.RecyclablesCount() )

        list1.PopFront();        UT_EQ( 1, list1.RecyclablesCount() )
                                 UT_EQ( 1, list2.RecyclablesCount() )

        list2.PopFront();        UT_EQ( 2, list1.RecyclablesCount() )
                                 UT_EQ( 2, list2.RecyclablesCount() )
    }

    // List no recycling
    {
        MonoAllocator monoAllocator(ALIB_DBG("UTListNR",) 1);

        List<MonoAllocator, int, Recycling::None>  list( monoAllocator );

        list.PushFront(1);
        list.PopFront();
    }

    //------------- The same once more, now with HeapAllocator --------------
    // List private
    {
        alib::List<HeapAllocator, int>   list;

                                UT_EQ( 0, list.RecyclablesCount() )
        list.PushFront(1);      UT_EQ( 0, list.RecyclablesCount() )
        list.PushFront(2);      UT_EQ( 0, list.RecyclablesCount() )
        list.PopFront();        UT_EQ( 1, list.RecyclablesCount() )
        list.PushFront(22);     UT_EQ( 0, list.RecyclablesCount() )

        list.PopFront();        UT_EQ( 1, list.RecyclablesCount() )
        list.PopFront();        UT_EQ( 2, list.RecyclablesCount() )
    }

    // List shared
    {
        List<HeapAllocator, int, Recycling::Shared>::SharedRecyclerType sharedRecycler;
        List<HeapAllocator, int, Recycling::Shared>  list1( sharedRecycler );
        List<HeapAllocator, int, Recycling::Shared>  list2( sharedRecycler );

                                 UT_EQ( 0, list1.RecyclablesCount() )
                                 UT_EQ( 0, list2.RecyclablesCount() )

        list1.PushFront(1);      UT_EQ( 0, list1.RecyclablesCount() )
                                 UT_EQ( 0, list2.RecyclablesCount() )

        list2.PushFront(1);      UT_EQ( 0, list1.RecyclablesCount() )
                                 UT_EQ( 0, list2.RecyclablesCount() )

        list1.PopFront();        UT_EQ( 1, list1.RecyclablesCount() )
                                 UT_EQ( 1, list2.RecyclablesCount() )

        list2.PopFront();        UT_EQ( 2, list1.RecyclablesCount() )
                                 UT_EQ( 2, list2.RecyclablesCount() )
    }

    // List no recycling
    {
        List<HeapAllocator, int, Recycling::None>  list;

        list.PushFront(1);                                              UT_EQ( 0, list.RecyclablesCount() )
        list.PopFront();                                                UT_EQ( 0, list.RecyclablesCount() )

        UT_PRINT( "One warning should follow:" )
        list.ReserveRecyclables( 5, lang::ValueReference::Relative );   UT_EQ( 0, list.RecyclablesCount() )
    }

    // List no recycling with pool allocator
    {
        PoolAllocatorHA heapPool;
        List<PoolAllocatorHA, int, Recycling::None>  list(heapPool);
                                                                        UT_EQ( 0, list.RecyclablesCount() ) UT_EQ( 0, heapPool.GetPoolSize(sizeof(containers::detail::ListElement<int>)) )
        list.PushFront(1);                                              UT_EQ( 0, list.RecyclablesCount() ) UT_EQ( 0, heapPool.GetPoolSize(sizeof(containers::detail::ListElement<int>)) )
        list.PopFront();                                                UT_EQ( 0, list.RecyclablesCount() ) UT_EQ( 1, heapPool.GetPoolSize(sizeof(containers::detail::ListElement<int>)) )
        UT_PRINT( "One warning should follow:" )
        list.ReserveRecyclables( 100, lang::ValueReference::Relative ); UT_EQ( 0, list.RecyclablesCount() ) UT_EQ( 1, heapPool.GetPoolSize(sizeof(containers::detail::ListElement<int>)) )
    }
}

#include "unittests/aworx_unittests_end.hpp"

} //namespace [ut_aworx]

#endif // ALIB_UT_MONOMEM
