// #################################################################################################
//  AWorx ALib Unit Tests
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib_precompile.hpp"
#include "alib_test_selection.hpp"
#if ALIB_UT_MONOMEM

#include "ALib.Containers.List.H"
#include "ALib.Monomem.SharedMonoVal.H"
#include "ALib.Containers.List.H"
#include "ALib.Containers.HashTable.H"
#include "ALib.Monomem.H"
#include "ALib.Resources.H"
#include "ALib.Camp.Base.H"
#include "ALib.Lang.H"

#include "ALib.Strings.StdFunctors.H"
#include <vector>
#include <unordered_map>
#include <map>
#include <list>
#include <utility>
#include <assert.h>
#include <numeric>

#define TESTCLASSNAME       UT_ContMonoList
#include "aworx_unittests.hpp"

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
#include "ALib.Lang.CIFunctions.H"
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
        UT_EQ( exp.Length(), list.size() )
        UT_EQ( recyclablesCount, list.RecyclablesCount() )
        if ( exp.IsEmpty() )
        {
            UT_TRUE( list.empty() )
            UT_TRUE( list.begin() == list.end() )
            UT_TRUE( list.rbegin() == list.rend() )
            return;
        }
        else
            UT_FALSE( list.empty() )

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
            typename List<MonoAllocator, TChar>::iterator it = list.begin();
            typename List<MonoAllocator, TChar>::const_iterator cit = list.begin();
            cit = it;

            const List<MonoAllocator, TChar> *cp = &list;
            auto cit2 = cp->begin();
            static_assert( std::same_as<decltype( cit2 ), typename List<MonoAllocator, TChar>::const_iterator>,
                           "Error" );
        }


        // insert/erase/iterate
        list.emplace_back( 'a' );                       testCharList( ut , list , "a"        , 0 );
 (void) list.erase( list.begin() );                    testCharList( ut , list , ""         , 1 );

        list.emplace_back( 'a' );                       testCharList( ut , list , "a"        , 0 );
        list.emplace_back( 'b' );                       testCharList( ut , list , "ab"       , 0 );
 (void) list.erase( list.begin() );                    testCharList( ut , list , "b"        , 1 );
 (void) list.erase( list.begin() );                    testCharList( ut , list , ""         , 2 );

        list.emplace_back( 'a' );                       testCharList( ut , list , "a"        , 1 );
        list.emplace_back( 'b' );                       testCharList( ut , list , "ab"       , 0 );
 (void) list.erase( --list.end() );                    testCharList( ut , list , "a"        , 1 );
 (void) list.erase( --list.end() );                    testCharList( ut , list , ""         , 2 );

        list.emplace_back( 'a' );                       testCharList( ut , list , "a"        , 1 );
        list.emplace_back( 'b' );                       testCharList( ut , list , "ab"       , 0 );
        list.emplace_back( 'c' );                       testCharList( ut , list , "abc"      , 0 );
  (void) list.erase( list.begin() );                    testCharList( ut , list , "bc"       , 1 );
  (void) list.erase( list.begin() );                    testCharList( ut , list , "c"        , 2 );
  (void) list.erase( list.begin() );                    testCharList( ut , list , ""         , 3 );

        list.emplace_back( 'a' );                       testCharList( ut , list , "a"        , 2 );
        list.emplace_back( 'b' );                       testCharList( ut , list , "ab"       , 1 );
        list.emplace_back( 'c' );                       testCharList( ut , list , "abc"      , 0 );
 (void) list.erase( --list.end() );                    testCharList( ut , list , "ab"       , 1 );
 (void) list.erase( --list.end() );                    testCharList( ut , list , "a"        , 2 );
 (void) list.erase( --list.end() );                    testCharList( ut , list , ""         , 3 );

        list.emplace_back( 'a' );                       testCharList( ut , list , "a"        , 2 );
        list.emplace_back( 'b' );                       testCharList( ut , list , "ab"       , 1 );
        list.emplace_back( 'c' );                       testCharList( ut , list , "abc"      , 0 );

        auto it = list.end();
        it--;
        it = list.erase( it );                         testCharList( ut , list , "ab"       , 1 );
        UT_TRUE( it == list.end() )
        list.emplace_back( 'c' );                       testCharList( ut , list , "abc"      , 0 );
        it = list.end()--;
        --it;
        --it;
        it = list.erase( it );                         testCharList( ut , list , "ac"       , 1 );
        UT_EQ( 'c', Value( *it ) )
        it = list.erase( it );                         testCharList( ut , list , "a"        , 2 );
        list.Clear();                                  testCharList( ut , list , ""         , 3 );
        list.emplace_back( 'a' );                       testCharList( ut , list , "a"        , 2 );
        list.emplace_back( 'b' );
        list.emplace_back( 'c' );                       testCharList( ut , list , "abc"      , 0 );
        list.Reset();                                  testCharList( ut , list , ""         , 0 );
        list.emplace_back( 'a' );
        list.emplace_back( 'b' );
        list.emplace_back( 'c' );                       testCharList( ut , list , "abc"      , 0 );
        it = list.erase( list.begin(), list.end() );   testCharList( ut , list , ""         , 3 );
        UT_TRUE( it == list.end() )
        list.emplace_back( 'a' );
        list.emplace_back( 'b' );
        list.emplace_back( 'c' );                       testCharList( ut , list , "abc"      , 0 );
        it = list.erase( list.begin(), --list.end() ); testCharList( ut , list , "c"        , 2 );

        UT_EQ( 'c', Value( *it ) )
        it = list.erase( list.begin(), list.end() );
        UT_EQ  ( list.size(), 0 )
        UT_EQ( 'a', Value( list.emplace_back( 'a' ) ) ) testCharList( ut , list , "a"        , 2 );
        UT_EQ( 'b', Value( list.emplace_back( 'b' ) ) ) testCharList( ut , list , "ab"       , 1 );
        UT_EQ( 'c', Value( list.emplace_back( 'c' ) ) ) testCharList( ut , list , "abc"      , 0 );
        it = list.erase( ++list.begin(), list.end() ); testCharList( ut , list , "a"        , 2 );
        UT_TRUE( it == list.end() )
        it = list.erase( list.begin() );
        it = list.emplace( list.begin(), 'c' );        testCharList( ut , list , "c"        , 2 );   UT_EQ( 'c', Value( *it ) )
        it = list.emplace( list.begin(), 'b' );        testCharList( ut , list , "bc"       , 1 );   UT_EQ( 'b', Value( *it ) )
        it = list.emplace( list.begin(), 'a' );        testCharList( ut , list , "abc"      , 0 );   UT_EQ( 'a', Value( *it ) )
        list.Clear();                                  testCharList( ut , list , ""         , 3 );
        it = list.emplace( list.begin(), 'b' );        testCharList( ut , list , "b"        , 2 );   UT_EQ( 'b', Value( *it ) )
        it = list.emplace( list.end(), 'c' );          testCharList( ut , list , "bc"       , 1 );   UT_EQ( 'c', Value( *it ) )
        it = list.emplace( list.begin(), 'a' );        testCharList( ut , list , "abc"      , 0 );   UT_EQ( 'a', Value( *it ) )
        it = list.emplace( list.end(), 'd' );          testCharList( ut , list , "abcd"     , 0 );   UT_EQ( 'd', Value( *it ) )
        list.Reset();                                  testCharList( ut , list , ""         , 0 );
        it = list.emplace( list.begin(), 'a' );        testCharList( ut , list , "a"        , 0 );  UT_EQ( 'a', Value( *it ) )
        it = list.emplace( ++it, 'b' );                testCharList( ut , list , "ab"       , 0 );  UT_EQ( 'b', Value( *it ) )
        it = list.emplace( ++it, 'c' );                testCharList( ut , list , "abc"      , 0 );  UT_EQ( 'c', Value( *it ) )
        list.Clear();                                  testCharList( ut , list , ""         , 3 );
        it = list.emplace( list.begin(), 'b' );        testCharList( ut , list , "b"        , 2 );  UT_EQ( 'b', Value( *it ) )
        it = list.emplace( ++it, 'c' );                testCharList( ut , list , "bc"       , 1 );  UT_EQ( 'c', Value( *it ) )
        it = list.emplace( list.begin(), 'a' );        testCharList( ut , list , "abc"      , 0 );  UT_EQ( 'a', Value( *it ) )
        list.Clear();                                  testCharList( ut , list , ""         , 3 );

        list.ReserveRecyclables( 5, lang::ValueReference::Relative );
                                                       testCharList( ut , list , ""         , 5 );
        list.emplace_back( 'd' );                       testCharList( ut , list , "d"        , 4 );
        list.push_back( 'e' );                          testCharList( ut , list , "de"       , 3 );
        list.push_front( 'b' );                         testCharList( ut , list , "bde"      , 2 );
        list.Insert( ++list.begin(), 'c' );            testCharList( ut , list , "bcde"     , 1 );
        list.Insert( list.end(), 'f' );                testCharList( ut , list , "bcdef"    , 0 );
        list.emplace_front( 'a' );                      testCharList( ut , list , "abcdef"   , 0 );
        list.Insert( it = list.end(), 'g' );           testCharList( ut , list , "abcdefg"  , 0 );
        list.Insert( it, 'h' );                        testCharList( ut , list , "abcdefgh" , 0 );

        it = list.erase( ++ ++list.begin(),
        -- --list.end() );                             testCharList( ut , list , "abgh"     , 4 );   UT_EQ( 'g', Value( *it ) )
    }
#endif

} //anonymous namespace
#include "ALib.Lang.CIMethods.H"

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

    // run tests with different contents
    {
        TListTest<char     >( ut );
        TListTest<CharTriv >( ut );
        UT_EQ( CharDyn::InstanceCounter(), 0 )
            TListTest<CharDyn >( ut );
        UT_EQ( CharDyn::InstanceCounter(), 0 )
    }

    // test compatibility with std
    {
        // Create a list of integers
        List<lang::HeapAllocator, int> numbers = {4, 1, 9, 2, 7, 5, 3};//{1, 2, 3, 4, 5, 7, 9 };
        List<lang::HeapAllocator, int> result;

        // Display original list
        std::cout << "Original list: ";
        std::copy(numbers.begin(), numbers.end(), std::ostream_iterator<int>(std::cout, " "));
        std::cout << "\n";

        // std::accumulate - Sum all elements
        int sum = std::accumulate(numbers.begin(), numbers.end(), 0);
        std::cout << "Sum: " << sum << "\n";

        // std::transform - Transform each element
        // Using list::value_type indirectly through iterator
        std::transform(numbers.begin(), numbers.end(),
                      std::back_inserter(result),
                      [](const auto& val) { return val * 2; });

        std::cout << "After transform (doubled): ";
        std::copy(result.begin(), result.end(),  std::ostream_iterator<int>(std::cout, " "));
        std::cout << "\n";
        int sumResult = std::accumulate(result.begin(), result.end(), 0);
        std::cout << "Sum now: " << sumResult << "\n";
        UT_EQ( sum * 2, sumResult )

        // std::remove_if and erase - Remove elements that satisfy a condition
        result.erase(
            std::remove_if(result.begin(), result.end(),
                          [](const auto& val) { return val > 10; }),
            result.end());

        std::cout << "After remove_if (elements > 10): ";
        std::copy(result.begin(), result.end(), std::ostream_iterator<int>(std::cout, " "));
        std::cout << "\n";
        sumResult = std::accumulate(result.begin(), result.end(), 0);
        UT_EQ( 30, sumResult )

        // std::for_each - Apply function to each element
        std::cout << "Using for_each: ";
        std::for_each(result.begin(), result.end(), [](const auto& val) { std::cout << val << " "; });
        std::cout << "\n";

        // std::adjacent_difference - Calculate differences between adjacent elements
        List<lang::HeapAllocator, int> diffs;
        std::adjacent_difference(numbers.begin(), numbers.end(),  std::back_inserter(diffs));
        std::cout << "Adjacent differences: ";
        std::copy(diffs.begin(), diffs.end(),  std::ostream_iterator<int>(std::cout, "  "));
        std::cout << "\n";

        // std::unique - Remove consecutive duplicates
        List<lang::HeapAllocator, int> with_dupes = {1, 1, 2, 3, 3, 3, 4, 5, 5};
        with_dupes.erase(std::unique(with_dupes.begin(), with_dupes.end()), with_dupes.end());

        std::cout << "After unique: ";
        std::copy(with_dupes.begin(), with_dupes.end(), std::ostream_iterator<int>(std::cout, " "));
        std::cout << "\n";

        // std::merge - Merge two sorted lists
        List<lang::HeapAllocator, int> list1 = {1, 3, 5, 7};
        List<lang::HeapAllocator, int> list2 = {2, 4, 6, 8};
        List<lang::HeapAllocator, int> merged;

        std::merge(list1.begin(), list1.end(),
                   list2.begin(), list2.end(), std::back_inserter(merged));

        std::cout << "Merged list: ";
        std::copy(merged.begin(), merged.end(),   std::ostream_iterator<int>(std::cout, " "));
        std::cout << "\n";
    }

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
        list.push_front(1);      UT_EQ( 0, list.RecyclablesCount() )
        list.push_front(2);      UT_EQ( 0, list.RecyclablesCount() )
        list.pop_back();        UT_EQ( 1, list.RecyclablesCount() )
        list.push_front(22);     UT_EQ( 0, list.RecyclablesCount() )

        list.pop_back();        UT_EQ( 1, list.RecyclablesCount() )
        list.pop_back();        UT_EQ( 2, list.RecyclablesCount() )
    }

    // List shared
    {
        MonoAllocator monoAllocator(ALIB_DBG("UTListShared",) 1);

        List<MonoAllocator, int, Recycling::Shared>::SharedRecyclerType sharedRecycler(monoAllocator);
        List<MonoAllocator, int, Recycling::Shared>  list1( sharedRecycler );
        List<MonoAllocator, int, Recycling::Shared>  list2( sharedRecycler );

                                 UT_EQ( 0, list1.RecyclablesCount() )
                                 UT_EQ( 0, list2.RecyclablesCount() )

        list1.push_front(1);      UT_EQ( 0, list1.RecyclablesCount() )
                                 UT_EQ( 0, list2.RecyclablesCount() )

        list2.push_front(1);      UT_EQ( 0, list1.RecyclablesCount() )
                                 UT_EQ( 0, list2.RecyclablesCount() )

        list1.pop_back();        UT_EQ( 1, list1.RecyclablesCount() )
                                 UT_EQ( 1, list2.RecyclablesCount() )

        list2.pop_back();        UT_EQ( 2, list1.RecyclablesCount() )
                                 UT_EQ( 2, list2.RecyclablesCount() )
    }

    // List no recycling
    {
        MonoAllocator monoAllocator(ALIB_DBG("UTListNR",) 1);

        List<MonoAllocator, int, Recycling::None>  list( monoAllocator );

        list.push_front(1);
        list.pop_back();
    }

    //------------- The same once more, now with HeapAllocator --------------
    // List private
    {
        alib::List<HeapAllocator, int>   list;

                                UT_EQ( 0, list.RecyclablesCount() )
        list.push_front(1);      UT_EQ( 0, list.RecyclablesCount() )
        list.push_front(2);      UT_EQ( 0, list.RecyclablesCount() )
        list.pop_back();        UT_EQ( 1, list.RecyclablesCount() )
        list.push_front(22);     UT_EQ( 0, list.RecyclablesCount() )

        list.pop_back();        UT_EQ( 1, list.RecyclablesCount() )
        list.pop_back();        UT_EQ( 2, list.RecyclablesCount() )
    }

    // List shared
    {
        List<HeapAllocator, int, Recycling::Shared>::SharedRecyclerType sharedRecycler;
        List<HeapAllocator, int, Recycling::Shared>  list1( sharedRecycler );
        List<HeapAllocator, int, Recycling::Shared>  list2( sharedRecycler );

                                 UT_EQ( 0, list1.RecyclablesCount() )
                                 UT_EQ( 0, list2.RecyclablesCount() )

        list1.push_front(1);      UT_EQ( 0, list1.RecyclablesCount() )
                                 UT_EQ( 0, list2.RecyclablesCount() )

        list2.push_front(1);      UT_EQ( 0, list1.RecyclablesCount() )
                                 UT_EQ( 0, list2.RecyclablesCount() )

        list1.pop_back();        UT_EQ( 1, list1.RecyclablesCount() )
                                 UT_EQ( 1, list2.RecyclablesCount() )

        list2.pop_back();        UT_EQ( 2, list1.RecyclablesCount() )
                                 UT_EQ( 2, list2.RecyclablesCount() )
    }

    // List no recycling
    {
        List<HeapAllocator, int, Recycling::None>  list;

        list.push_front(1);                                              UT_EQ( 0, list.RecyclablesCount() )
        list.pop_back();                                                UT_EQ( 0, list.RecyclablesCount() )

        UT_PRINT( "One warning should follow:" )
        list.ReserveRecyclables( 5, lang::ValueReference::Relative );   UT_EQ( 0, list.RecyclablesCount() )
    }

    // List no recycling with pool allocator
    {
        PoolAllocatorHA heapPool;
        List<PoolAllocatorHA, int, Recycling::None>  list(heapPool);
                                                                        UT_EQ( 0, list.RecyclablesCount() ) UT_EQ( 0, heapPool.GetPoolSize(sizeof(containers::detail::ListElement<int>)) )
        list.push_front(1);                                              UT_EQ( 0, list.RecyclablesCount() ) UT_EQ( 0, heapPool.GetPoolSize(sizeof(containers::detail::ListElement<int>)) )
        list.pop_back();                                                UT_EQ( 0, list.RecyclablesCount() ) UT_EQ( 1, heapPool.GetPoolSize(sizeof(containers::detail::ListElement<int>)) )
        UT_PRINT( "One warning should follow:" )
        list.ReserveRecyclables( 100, lang::ValueReference::Relative ); UT_EQ( 0, list.RecyclablesCount() ) UT_EQ( 1, heapPool.GetPoolSize(sizeof(containers::detail::ListElement<int>)) )
    }
}

#include "aworx_unittests_end.hpp"

} //namespace [ut_aworx]

#endif // ALIB_UT_MONOMEM
