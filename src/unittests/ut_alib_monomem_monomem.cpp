// #################################################################################################
//  AWorx ALib Unit Tests
//
//  Copyright 2013-2023 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"
#include "unittests/alib_test_selection.hpp"
#if ALIB_UT_MONOMEM

#if !defined (HPP_ALIB_MONOMEM_LIST)
    #include "alib/monomem/list.hpp"
#endif
#if !defined (HPP_ALIB_MONOMEM_LIST)
    #include "alib/monomem/selfcontained.hpp"
#endif
#if !defined (HPP_ALIB_MONOMEM_HASHMAP)
    #include "alib/monomem/hashmap.hpp"
#endif
#if !defined (HPP_ALIB_MONOMEM_HASHSET)
    #include "alib/monomem/hashset.hpp"
#endif
#if !defined (HPP_ALIB_MONOMEM_STDCONTAINERMA)
    #include "alib/monomem/stdcontainerma.hpp"
#endif

#if !defined (HPP_ALIB_RESOURCES_LOCALRESOURCEPOOL)
    #include "alib/resources/localresourcepool.hpp"
#endif

#if !defined (HPP_ALIB_FS_MODULES_DISTRIBUTION)
    #include "alib/lib/fs_modules/distribution.hpp"
#endif

#include "alib/lib/fs_commonenums/commonenumdefs_aliased.hpp"

#if !defined (_GLIBCXX_VECTOR) && !defined(_VECTOR_)
#   include <vector>
#endif
#if !defined(_GLIBCXX_UNORDERED_MAP) && !defined(_UNORDERED_MAP_)
#   include <unordered_map>
#endif
#if !defined(_GLIBCXX_MAP) && !defined(_MAP_)
#   include <map>
#endif

#if !defined(_GLIBCXX_UTILITY) && !defined(_UTILITY_) && !defined(_LIBCPP_UTILITY)
#   include <utility>
#endif

#define TESTCLASSNAME       CPP_ALib_Monomem
#include "unittests/aworx_unittests.hpp"

#if defined(__clang__)
    #pragma clang diagnostic ignored "-Wunused-function"
#elif defined(__GNUC__)
    #pragma GCC diagnostic ignored "-Wunused-function"
#elif defined(_MSC_VER)

#endif

using namespace std;
using namespace aworx;

namespace ut_aworx
{
    namespace
    {
//#define DBGOUT(val)   cout << val << endl;
#define DBGOUT( val )

// Versions of a simple char container used to ensure correct deletion of objects
// when unit tests are run with a memory checker (e.g. valgrind).

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



// ########################################################################################
// StdVector reset sample
// ########################################################################################

        using MyData= int;

DOX_MARKER( [DOX_ALIB_MONOMEM_STDCONTMA_PLACEMENT_NEW] )
// field members  or global objects
aworx::MonoAllocator allocator( 4096 );
std::vector<MyData, aworx::StdContMA<MyData>> transactionObjects(
    ( aworx::StdContMA<MyData>( allocator ) ) );

// method using the allocator and the vector
void processTransaction( /* transaction data */)
{
    // perform transaction, collecting stuff in vector and/or allocator
    // ...
    // ...
    // ...

    // before we leave we reset the vector and the allocator:

    // 1. destruct the contained objects. Not needed if trivially destructible.
    transactionObjects.clear();

    // 2. Reset allocator
    allocator.Reset();

    // 3. Placement new on the vector object
    new( &transactionObjects ) std::vector<MyData, aworx::StdContMA<MyData>>(
        aworx::StdContMA<MyData>( allocator ) );
}

DOX_MARKER( [DOX_ALIB_MONOMEM_STDCONTMA_PLACEMENT_NEW] )


    // ########################################################################################
    // List helper function
    // ########################################################################################
    template<typename TChar>
    void testCharList( AWorxUnitTesting & ut, List<TChar> & list,
                       const NString & exp, integer recyclablesCount )
    {
        UT_EQ( exp.Length(), list.Size() )
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
    void ListTest( AWorxUnitTesting & ut )
    {
        MonoAllocator ba( 512 );

        List<TChar> list( &ba );
        testCharList( ut, list, "", 0 );

        // iterator conversion
        {
            typename List<TChar>::Iterator it = list.begin();
            typename List<TChar>::ConstIterator cit = list.begin();
            cit = it;

            const List<TChar> *cp = &list;
            auto cit2 = cp->begin();
            static_assert( ATMP_EQ( decltype( cit2 ), typename List<TChar>::ConstIterator ),
                           "Error" );
        }


        // insert/erase/iterate
        list.EmplaceBack( 'a' );                       testCharList( ut , list , "a"        , 0 );
        list.Erase( list.begin() );                    testCharList( ut , list , ""         , 1 );

        list.EmplaceBack( 'a' );                       testCharList( ut , list , "a"        , 0 );
        list.EmplaceBack( 'b' );                       testCharList( ut , list , "ab"       , 0 );
        list.Erase( list.begin() );                    testCharList( ut , list , "b"        , 1 );
        list.Erase( list.begin() );                    testCharList( ut , list , ""         , 2 );

        list.EmplaceBack( 'a' );                       testCharList( ut , list , "a"        , 1 );
        list.EmplaceBack( 'b' );                       testCharList( ut , list , "ab"       , 0 );
        list.Erase( --list.end() );                    testCharList( ut , list , "a"        , 1 );
        list.Erase( --list.end() );                    testCharList( ut , list , ""         , 2 );

        list.EmplaceBack( 'a' );                       testCharList( ut , list , "a"        , 1 );
        list.EmplaceBack( 'b' );                       testCharList( ut , list , "ab"       , 0 );
        list.EmplaceBack( 'c' );                       testCharList( ut , list , "abc"      , 0 );
        list.Erase( list.begin() );                    testCharList( ut , list , "bc"       , 1 );
        list.Erase( list.begin() );                    testCharList( ut , list , "c"        , 2 );
        list.Erase( list.begin() );                    testCharList( ut , list , ""         , 3 );

        list.EmplaceBack( 'a' );                       testCharList( ut , list , "a"        , 2 );
        list.EmplaceBack( 'b' );                       testCharList( ut , list , "ab"       , 1 );
        list.EmplaceBack( 'c' );                       testCharList( ut , list , "abc"      , 0 );
        list.Erase( --list.end() );                    testCharList( ut , list , "ab"       , 1 );
        list.Erase( --list.end() );                    testCharList( ut , list , "a"        , 2 );
        list.Erase( --list.end() );                    testCharList( ut , list , ""         , 3 );

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
        UT_EQ  ( list.Size(), 0 )
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

        list.ReserveRecyclables( 5 );                  testCharList( ut , list , ""         , 5 );
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

//needed for the assertion, as macro ALIB_CALLER is changed here
#if ALIB_DEBUG
#   undef  ALIB_CALLER
#   if defined( __GNUC__ )
#      define ALIB_CALLER    __FILE__, __LINE__, __func__
#   else
#      define ALIB_CALLER    __FILE__, __LINE__, __FUNCTION__
#   endif
#endif

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

template<typename THashMap>
void testConsistency( AWorxUnitTesting&  ut,
                      THashMap& map, integer expSize,
                                     integer expBucketCount,
                                     integer expRecyclables   )

{
    if( expBucketCount == 0 )
    {
        expBucketCount= 1;
    }

    if( expSize         != -1 ) { UT_EQ(                       expSize        , map.Size() )  }
    if( expBucketCount  != -1 ) { UT_EQ( static_cast<uinteger>(expBucketCount), map.BucketCount() )  }
    if( expRecyclables  != -1 ) { UT_EQ(                       expRecyclables , map.RecyclablesCount() )  }

    if(expSize == map.Size())
    {
        // count using normal iterator
        integer size= 0;
        for( auto mapPair : map )
        {
            ++size;
            (void) mapPair;
        }

        UT_EQ( expSize, size )


        // count using bucket iterator
        size= 0;
        for( uinteger i= 0 ; i < map.BucketCount() ; ++i )
        {
            for( auto it= map.begin( i ); it != map.end( i ); ++it )
            {
                ++size;
                (void) it;
            }
        }
        UT_EQ( expSize, size )
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

    hashSet.Emplace( 1 );                testConsistency(ut, hashSet,     1 ,  11,  0 );
                                         UT_EQ( 1l, hashSet.Erase  ( 1  ) )
                                         testConsistency(ut, hashSet,     0 ,  11,  1 );
    auto it1= hashSet.Emplace( 1 );      testConsistency(ut, hashSet,     1 ,  11,  0 );
                                         UT_EQ( it1.Value(), 1 )
    auto it2= hashSet.Emplace( 1 );      testConsistency(ut, hashSet,     2 ,  11,  0 );
                                         UT_EQ( it2.Value(), 1 )
    ++it2;                               UT_TRUE( it1 == it2 )

    UT_EQ( 2l, hashSet.Erase  ( 1  ) )   testConsistency(ut, hashSet,     0 ,  11,  2 );

    {
        hashSet.Emplace( 1 );            testConsistency(ut, hashSet,     1 ,  11,  1 );
        hashSet.Emplace( 2 );            testConsistency(ut, hashSet,     2 ,  11,  0 );
        auto
        it= hashSet.Find( 1 );           UT_FALSE( it == hashSet.end() )     UT_EQ(1, it.Value())
        it= hashSet.Find( 2 );           UT_FALSE( it == hashSet.end() )     UT_EQ(2, *it       )
        it= hashSet.Find( 3 );           UT_TRUE(  it == hashSet.end() )

    }

    // automatic rehash
    {
        hashSet.Clear();                  testConsistency(ut, hashSet,     0 ,  11,  2 );
        auto inserts= static_cast<int>(   hashSet.MaxLoadFactor() * static_cast<float>( hashSet.BucketCount() )
                                        - static_cast<float>(hashSet.Size())                                    );
        for( int i= 0 ; i < inserts -1 ; ++i )
            hashSet.Emplace( 5000 + rand() % 100  );

        auto oldBucketCount=  hashSet.BucketCount();
        hashSet.Emplace( 1000 );
        UT_TRUE( oldBucketCount < hashSet.BucketCount() )

        testConsistency(ut, hashSet,   hashSet.Size(), 23, -1 );
    }

    // EqualRange
    hashSet.Reset();
    auto cntRecylables= hashSet.RecyclablesCount();
    {
        hashSet.Emplace(  0 );            testConsistency(ut, hashSet,     1 ,  11,  cntRecylables );
        hashSet.Emplace(  0 );
        hashSet.Emplace( 11 );
        hashSet.Emplace( 11 );

        hashSet.Emplace(  1 );
        hashSet.Emplace( 12 );            testConsistency(ut, hashSet,     6 ,  11,  cntRecylables );

        auto
        result= hashSet.EqualRange(  0 ); UT_EQ(     0, result.first.Value() )
        ++result.first;                   UT_EQ(     0, result.first.Value() )
        ++result.first;                   UT_FALSE(  0  == result.first.Value() )
                                          UT_TRUE (  result.first == result.second )
        result= hashSet.EqualRange(  1 ); UT_EQ(     1, result.first.Value() )
        ++result.first;                   UT_TRUE (  result.first == result.second )
        result= hashSet.EqualRange( 11 ); UT_EQ(    11, result.first.Value() )
        ++result.first;                   UT_EQ(    11, result.first.Value() )
        ++result.first;                   UT_FALSE( 11  == result.first.Value() )
                                          UT_TRUE (  result.first == result.second )
        result= hashSet.EqualRange( 12 ); UT_EQ(    12, result.first.Value() )
        ++result.first;                   UT_TRUE (  result.first == result.second )

    }

    // Erase( pos )
    {

        hashSet.Clear();
        cntRecylables= hashSet.RecyclablesCount();
                                          testConsistency(ut, hashSet,     0 ,  11,  cntRecylables );
        hashSet.Emplace( 0);
        hashSet.Emplace( 0);
        hashSet.Emplace(11);
        hashSet.Emplace( 1);
        hashSet.Emplace(12);              testConsistency(ut, hashSet,     5 ,  11,  cntRecylables - 5 );
                                          testBucket(ut, hashSet,     0, 3 );
                                          testBucket(ut, hashSet,     1, 2 );

        auto start= hashSet.begin();      testConsistency(ut, hashSet,     5 ,  11,  cntRecylables - 5 );
                                          UT_EQ( 11,  start.Value() )

        start= hashSet.Erase( start );    testConsistency(ut, hashSet,     4 ,  11,  cntRecylables - 4 );
                                          testBucket(ut, hashSet,     0, 2 );
                                          UT_EQ(  0,  start.Value() )

        start= hashSet.Erase( start );    testConsistency(ut, hashSet,     3 ,  11,  cntRecylables - 3 );
                                          testBucket(ut, hashSet,     0, 1 );
                                          UT_EQ(  0,  start.Value() )

        start= hashSet.Erase( start );    testConsistency(ut, hashSet,     2 ,  11,  cntRecylables - 2 );
                                          testBucket(ut, hashSet,     0, 0 );
                                          UT_EQ( 12 ,  start.Value() )

        ++start;                          UT_EQ(  1 ,  start.Value() )
        start= hashSet.Erase( start );    testConsistency(ut, hashSet,     1 ,  11,  cntRecylables - 1 );
                                          testBucket(ut, hashSet,     1, 1 );
                                          UT_TRUE( hashSet.end() == start )
        start= hashSet.begin();
        start= hashSet.Erase( start );    testConsistency(ut, hashSet,     0 ,  11,  cntRecylables  );
                                          testBucket(ut, hashSet,     1, 0 );
                                          UT_TRUE( hashSet.end() == start )
    }

    // Erase( key )
    {
        hashSet.Clear();                        testConsistency ( ut, hashSet,     0 ,  11,  cntRecylables );
        hashSet.Emplace( 0);
        hashSet.Emplace( 0);
        hashSet.Emplace(11);
        hashSet.Emplace( 1);
        hashSet.Emplace(12);                    testConsistency ( ut, hashSet,     5 ,  11,  cntRecylables - 5 );
                                                testBucket      ( ut, hashSet,     0, 3 );
                                                testBucket      ( ut, hashSet,     1, 2 );

        UT_EQ( 0, hashSet.Erase(99)      )      testConsistency ( ut, hashSet,     5 ,  11,  cntRecylables - 5 );
        UT_EQ( 1, hashSet.Erase(12)      )      testConsistency ( ut, hashSet,     4 ,  11,  cntRecylables - 4 );
                                                testBucket      ( ut, hashSet,     1, 1 );
        UT_EQ( 2, hashSet.Erase(0)       )      testConsistency ( ut, hashSet,     2 ,  11,  cntRecylables - 2 );
        UT_EQ( 0, hashSet.Erase(0)       )      testConsistency ( ut, hashSet,     2 ,  11,  cntRecylables - 2 );
                                                testBucket      ( ut, hashSet,     0, 1 );

        lib::results::Report::GetDefault().PushHaltFlags( false, false );
        UT_PRINT( "An error should follow" )
        hashSet.Emplace( 0);
        hashSet.Emplace( 0);
        UT_EQ( true, hashSet.EraseUnique(0)  )  testConsistency(ut, hashSet,     3 ,  11,  cntRecylables - 3 );
        lib::results::Report::GetDefault().PopHaltFlags();
        UT_EQ( true , hashSet.EraseUnique( 0) )  testConsistency(ut, hashSet,     2 ,  11,  cntRecylables - 2 );
        UT_EQ( false, hashSet.EraseUnique( 0) )  testConsistency(ut, hashSet,     2 ,  11,  cntRecylables - 2 );
        UT_EQ( true , hashSet.EraseUnique(11) )  testConsistency(ut, hashSet,     1 ,  11,  cntRecylables - 1 );
        UT_EQ( true , hashSet.EraseUnique( 1) )  testConsistency(ut, hashSet,     0 ,  11,  cntRecylables );
        UT_EQ( false, hashSet.EraseUnique(12) )  testConsistency(ut, hashSet,     0 ,  11,  cntRecylables );
    }


    // Erase( start, end )
    {
        hashSet.Clear();                        testConsistency(ut, hashSet,     0 ,  11,  cntRecylables );
        hashSet.Emplace( 0);
        hashSet.Emplace( 0);
        hashSet.Emplace(11);
        hashSet.Emplace( 1);
        hashSet.Emplace(12);                    testConsistency(ut, hashSet,     5 ,  11,  cntRecylables - 5 );
                                                testBucket(ut, hashSet,     0, 3 );
                                                testBucket(ut, hashSet,     1, 2 );

        auto start= hashSet.begin();            UT_EQ( 11,  start.Value() )
        auto end  = hashSet.begin();            UT_EQ( 11,  end  .Value() )
        start= hashSet.Erase(start,end);        testConsistency(ut, hashSet,     5 ,  11,  cntRecylables - 5 );
                                                UT_EQ( 11,  start.Value() )

        start= hashSet.begin();                 UT_EQ( 11,  start.Value() )
        end  = hashSet.begin(); ++end;          UT_EQ(  0,  end  .Value() )
        start= hashSet.Erase(start,end);        testConsistency(ut, hashSet,     4 ,  11,  cntRecylables - 4 );
                                                UT_TRUE(  start == end )
                                                UT_EQ(  0,  start.Value() )

        start++;                                UT_EQ(  0,  start.Value() )
                                                testBucket(ut, hashSet,     0, 2 );
                                                testBucket(ut, hashSet,     1, 2 );
        end= start; ++end;  ++end;              UT_EQ(  1,  end  .Value() )
        start= hashSet.Erase( start, end );     testConsistency(ut, hashSet,     2 ,  11,  cntRecylables - 2 );
                                                testBucket(ut, hashSet,     0, 1 );
                                                testBucket(ut, hashSet,     1, 1 );
                                                UT_EQ(  1,  start.Value() )

        hashSet.Erase( hashSet.begin(),
                       hashSet.end()   );       testConsistency(ut, hashSet,     0 ,  11,  cntRecylables     );
        hashSet.Emplace( 0);
        hashSet.Emplace( 0);
        hashSet.Emplace(11);
        hashSet.Emplace( 1);
        hashSet.Emplace(12);                    testConsistency(ut, hashSet,     5 ,  11,  cntRecylables - 5 );
        hashSet.Erase( hashSet.begin(),
                       hashSet.end() );         testConsistency(ut, hashSet,     0 ,  11,  cntRecylables     );
    }


    // Erase( pos ) with bucket iterator
    {
        hashSet.Clear();                  testConsistency(ut, hashSet,     0 ,  11,  cntRecylables );
        hashSet.Emplace( 0);
        hashSet.Emplace( 0);
        hashSet.Emplace(11);
        hashSet.Emplace( 1);
        hashSet.Emplace(12);              testConsistency(ut, hashSet,     5 ,  11,  cntRecylables - 5 );
                                          testBucket(ut, hashSet,     0, 3 );
                                          testBucket(ut, hashSet,     1, 2 );

        auto start= hashSet.begin(0);     UT_EQ( 11,  start.Value() )

        start= hashSet.Erase( start );    testConsistency(ut, hashSet,     4 ,  11,  cntRecylables - 4 );
                                          testBucket(ut, hashSet,     0, 2 );
                                          UT_EQ(  0,  start.Value() )

        start= hashSet.Erase( start );    testConsistency(ut, hashSet,     3 ,  11,  cntRecylables - 3 );
                                          testBucket(ut, hashSet,     0, 1 );
                                          UT_EQ(  0,  start.Value() )

        start= hashSet.Erase( start );    testConsistency(ut, hashSet,     2 ,  11,  cntRecylables - 2 );
                                          testBucket(ut, hashSet,     0, 0 );
        hashSet.begin(0);                 UT_TRUE( hashSet.end(0) == start )

        start= hashSet.begin(1);          UT_EQ( 12  ,  start.Value() )
        ++start;                          UT_EQ(  1  ,  start.Value() )
        start= hashSet.Erase( start );    testConsistency(ut, hashSet,     1 ,  11,  cntRecylables - 1 );
                                          testBucket(ut, hashSet,     1, 1 );
                                          UT_TRUE( hashSet.end(1) == start )
        start= hashSet.begin(1);
        start= hashSet.Erase( start );    testConsistency(ut, hashSet,     0 ,  11,  cntRecylables  );
                                          testBucket(ut, hashSet,     1, 0 );
                                          UT_TRUE( hashSet.end(1) == start )
    }

    // Erase (start,end) with bucket iterators
    {
        hashSet.Clear();
        hashSet.Emplace( 0);
        hashSet.Emplace( 0);
        hashSet.Emplace(11);
        hashSet.Emplace( 1);
        hashSet.Emplace(12);                    testConsistency(ut, hashSet,     5 ,  11,  cntRecylables - 5 );
                                                testBucket(ut, hashSet,     0, 3 );
                                                testBucket(ut, hashSet,     1, 2 );

        auto start= hashSet.begin(0);           UT_EQ( 11,  start.Value() )
        auto end  = hashSet.begin(0);           UT_EQ( 11,  end  .Value() )
        start= hashSet.Erase(start,end);        testConsistency(ut, hashSet,     5 ,  11,  cntRecylables - 5 );
                                                UT_EQ( 11,  start.Value() )

        end  = hashSet.begin(0); ++end;         UT_EQ(  0,  end  .Value() )
        start= hashSet.Erase(start,end);        testConsistency(ut, hashSet,     4 ,  11,  cntRecylables - 4 );
                                                UT_TRUE(  start == end )
                                                UT_EQ(  0,  start.Value() )
                                                testBucket(ut, hashSet,     0, 2 );

        start++;                                UT_EQ(  0,  start.Value() )
        start= hashSet.Erase(start,
                             hashSet.end(0));   testConsistency(ut, hashSet,     3 ,  11,  cntRecylables - 3 );
                                                testBucket(ut, hashSet,     0, 1 );
                                                testBucket(ut, hashSet,     1, 2 );
                                                UT_TRUE(start == hashSet.end(0) )

        start= hashSet.Erase(hashSet.begin(0),
                             hashSet.end  (0)); testConsistency(ut, hashSet,     2 ,  11,  cntRecylables - 2 );
                                                testBucket(ut, hashSet,     0, 0 );
                                                testBucket(ut, hashSet,     1, 2 );
                                                UT_TRUE(start == hashSet.end(0) )



        start= hashSet.begin(1); ++start;       UT_EQ(  1,  start.Value() )
        end  = hashSet.end(1);
        start= hashSet.Erase(start,end);        testConsistency(ut, hashSet,     1 ,  11,  cntRecylables - 1 );
                                                UT_TRUE(start == hashSet.end(1) )
                                                testBucket(ut, hashSet,     1, 1 );
        start= hashSet.Erase(hashSet.begin(1),
                             hashSet.end  (1)); testConsistency(ut, hashSet,     0 ,  11,  cntRecylables  );
                                                testBucket(ut, hashSet,     0, 0 );
                                                testBucket(ut, hashSet,     1, 0 );
    }


    // Extract and Insert(handle)
    {
        hashSet.Clear();
        hashSet.Emplace( 0);
        hashSet.Emplace( 0);
        hashSet.Emplace(11);
        hashSet.Emplace( 1);
        hashSet.Emplace(12);                                testConsistency(ut, hashSet,     5 ,  11,  cntRecylables - 5 );
                                                            testBucket(ut, hashSet,     0, 3 );
                                                            testBucket(ut, hashSet,     1, 2 );

        // extract by pos and add at the same spot with same key existing
        auto start = hashSet.begin() ;                      UT_EQ( 11,  start.Value() )
        auto handle= hashSet.Extract(start);                testConsistency(ut, hashSet,     4 ,  11,  cntRecylables - 5 );
                                                            testBucket(ut, hashSet,     0, 2 );
                                                            testBucket(ut, hashSet,     1, 2 );
                                                            UT_FALSE(   handle.IsEmpty() )
                                                            UT_EQ( 11,  handle.Value  () )
        auto result= hashSet.Insert(handle);                testConsistency(ut, hashSet,     5 ,  11,  cntRecylables - 5 );
                                                            UT_TRUE(    handle.IsEmpty() )
                                                            UT_TRUE(  result == hashSet.Find(11) )
                                                            UT_TRUE(   handle.IsEmpty() )

        // extract by pos and add somewhere else
        start = hashSet.begin() ;                           UT_EQ( 11,  start.Value() )
        handle= hashSet.Extract(start);                     testConsistency(ut, hashSet,     4 ,  11,  cntRecylables - 5 );
                                                            testBucket(ut, hashSet,     0, 2 );
                                                            testBucket(ut, hashSet,     1, 2 );
                                                            UT_FALSE(   handle.IsEmpty() )
                                                            UT_EQ( 11,  handle.Value  ()  )
        handle.Value()= 5;                                  UT_EQ(  5,  handle.Value  ()   )
        result= hashSet.Insert(handle);                     testConsistency(ut, hashSet,     5 ,  11,  cntRecylables - 5 );
                                                            UT_TRUE(    handle.IsEmpty() )
                                                            UT_TRUE(  result == hashSet.Find(5) )
                                                            UT_TRUE(   handle.IsEmpty() )
                                                            testBucket(ut, hashSet,     0, 2 );
                                                            testBucket(ut, hashSet,     1, 2 );
                                                            testBucket(ut, hashSet,     5, 1 );

        // extract by key and add somewhere else
        handle= hashSet.Extract(1);                         testConsistency(ut, hashSet,     4 ,  11,  cntRecylables - 5 );
                                                            testBucket(ut, hashSet,     0, 2 );
                                                            testBucket(ut, hashSet,     1, 1 );
                                                            testBucket(ut, hashSet,     5, 1 );
                                                            UT_FALSE(   handle.IsEmpty() )
                                                            UT_EQ(  1,  handle.Value    () )
        // ...unsuccessful insert
        handle.Value()= 5;                                  UT_EQ(  5,  handle.Value  () )
        result= hashSet.InsertIfNotExistent(handle);        testConsistency(ut, hashSet,     4 ,  11,  cntRecylables - 5 );
                                                            UT_FALSE( handle.IsEmpty () )
                                                            UT_EQ(  5,  handle.Value   () )
                                                            testBucket(ut, hashSet,     0, 2 );
                                                            testBucket(ut, hashSet,     1, 1 );
                                                            testBucket(ut, hashSet,     5, 1 );
        // ...successful insert
        handle.Value()= 6;                                  UT_EQ(  6,  handle.Value  () )
        result= hashSet.InsertIfNotExistent(handle);        testConsistency(ut, hashSet,     5 ,  11,  cntRecylables - 5 );
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
        result= hashSet.InsertUnique(0);                    testConsistency(ut, hashSet,     1 ,  11,  0 );
                                                            UT_EQ( 0 ,  result.Value() )
        result= hashSet.InsertUnique(11);                   testConsistency(ut, hashSet,     2 ,  11,  0 );
                                                            UT_EQ( 11,  result.Value() )

        lib::results::Report::GetDefault().PushHaltFlags( false, false );
        UT_PRINT( "An error should follow" )

        result= hashSet.InsertUnique(0);                   testConsistency(ut, hashSet,     3 ,  11,  0 );
                                                            UT_EQ( 0 ,  result.Value() )
        lib::results::Report::GetDefault().PopHaltFlags();


        result= hashSet.EmplaceUnique( 2);                 testConsistency(ut, hashSet,     4 ,  11,  0 );
                                                            UT_EQ( 2  ,  result.Value() )
        result= hashSet.EmplaceUnique(12);                 testConsistency(ut, hashSet,     5 ,  11,  0 );
                                                            UT_EQ( 12 ,  result.Value() )

        lib::results::Report::GetDefault().PushHaltFlags( false, false );
        UT_PRINT( "An error should follow" )

        result= hashSet.EmplaceUnique(2);                  testConsistency(ut, hashSet,     6 ,  11,  0 );
                                                            UT_EQ( 2 ,  result.Value() )
        lib::results::Report::GetDefault().PopHaltFlags();
    }

    // EmplaceIfNotExistent
    {
        hashSet.Reset();                            testConsistency(ut, hashSet,     0 ,  0 ,  0 );
        auto
        result= hashSet.EmplaceIfNotExistent(0);    testConsistency(ut, hashSet,     1 ,  11,  0 );
                                                    UT_EQ(   0 ,  result.first.Value() )
                                                    UT_EQ(true ,  result.second  ) // is insert
                                                    UT_EQ(   0 ,  hashSet.begin().Value() )
                                                    UT_TRUE( result.first == hashSet.Find(0) )

        result= hashSet.EmplaceIfNotExistent(0);    testConsistency(ut, hashSet,     1 ,  11,  0 );
                                                    UT_EQ(   0 ,  result.first.Value() )
                                                    UT_EQ(false,  result.second  ) // nothing done
                                                    UT_EQ(   0 ,  hashSet.begin().Value() )
                                                    UT_TRUE( result.first == hashSet.Find(0) )


        result= hashSet.EmplaceIfNotExistent(1);   testConsistency(ut, hashSet,     2 ,  11,  0 );
                                                    UT_EQ(   1 ,  result.first.Value() )
                                                    UT_EQ(true ,  result.second  ) // is insert
                                                    UT_TRUE( result.first == hashSet.Find(1) )
        result= hashSet.EmplaceIfNotExistent(1);    testConsistency(ut, hashSet,     2 ,  11,  0 );
                                                    UT_EQ(   1 ,  result.first.Value() )
                                                    UT_EQ(false,  result.second  ) // nothing done
                                                    UT_TRUE( result.first == hashSet.Find(1) )

        result= hashSet.EmplaceIfNotExistent(2);    testConsistency(ut, hashSet,     3 ,  11,  0 ); // consumes recycle
                                                    UT_EQ(   2 ,  result.first.Value() )
                                                    UT_EQ(true ,  result.second  ) // is insert
                                                    UT_TRUE( result.first == hashSet.Find(2) )

        result= hashSet.EmplaceIfNotExistent(3);    testConsistency(ut, hashSet,     4 ,  11,  0 );
                                                    UT_EQ(  3  ,  result.first.Value() )
                                                    UT_EQ(true ,  result.second  ) // is insert
                                                    UT_TRUE( result.first == hashSet.Find(3) )
    }


    // InsertIfNotExistent
    {
        hashSet.Reset();                            testConsistency(ut, hashSet,     0 ,  0 ,  0 );
        auto
        result= hashSet.InsertIfNotExistent(0);     testConsistency(ut, hashSet,     1 ,  11,  0 );
                                                    UT_EQ(   0 ,  result.first.Value() )
                                                    UT_EQ(true ,  result.second  ) // is insert
                                                    UT_EQ(   0 ,  hashSet.begin().Value() )
                                                    UT_TRUE( result.first == hashSet.Find(0) )

        result= hashSet.InsertIfNotExistent(0);     testConsistency(ut, hashSet,     1 ,  11,  0 );
                                                    UT_EQ(   0 ,  result.first.Value() )
                                                    UT_EQ(false,  result.second  ) // nothing done
                                                    UT_EQ(   0 ,  hashSet.begin().Value() )
                                                    UT_TRUE( result.first == hashSet.Find(0) )


        result= hashSet.InsertIfNotExistent(1);    testConsistency(ut, hashSet,     2 ,  11,  0 );
                                                    UT_EQ(   1 ,  result.first.Value() )
                                                    UT_EQ(true ,  result.second  ) // is insert
                                                    UT_TRUE( result.first == hashSet.Find(1) )
        result= hashSet.InsertIfNotExistent(1);     testConsistency(ut, hashSet,     2 ,  11,  0 );
                                                    UT_EQ(   1 ,  result.first.Value() )
                                                    UT_EQ(false,  result.second  ) // nothing done
                                                    UT_TRUE( result.first == hashSet.Find(1) )

        result= hashSet.InsertIfNotExistent(2);     testConsistency(ut, hashSet,     3 ,  11,  0 ); // consumes recycle
                                                    UT_EQ(   2 ,  result.first.Value() )
                                                    UT_EQ(true ,  result.second  ) // is insert
                                                    UT_TRUE( result.first == hashSet.Find(2) )

        result= hashSet.InsertIfNotExistent(3);    testConsistency(ut, hashSet,     4 ,  11,  0 );
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
    hashMap.Emplace( 1, 100 );           testConsistency(ut, hashMap,     1 ,  11,  0 );
    UT_EQ( 1l, hashMap.Erase  ( 1  ) )   testConsistency(ut, hashMap,     0 ,  11,  1 );
    auto it1= hashMap.Emplace( 1, 100 ); testConsistency(ut, hashMap,     1 ,  11,  0 );
              UT_EQ( it1.Mapped(), 100 )
    auto it2= hashMap.Emplace( 1, 101 ); testConsistency(ut, hashMap,     2 ,  11,  0 );
              UT_EQ( it2.Mapped(), 101 )
    ++it2;    UT_TRUE( it1 == it2 )

    UT_EQ( 2l, hashMap.Erase  ( 1  ) )   testConsistency(ut, hashMap,     0 ,  11,  2 );


    {
        hashMap.Emplace( 1, 100 );       testConsistency(ut, hashMap,     1 ,  11,  1 );
        hashMap.Emplace( 2, 200 );
        auto it= hashMap.Find( 1 );      UT_FALSE( it == hashMap.end() )     UT_EQ(100, it.Mapped())

        it= hashMap.Find( 2 );           UT_FALSE( it == hashMap.end() )     UT_EQ(200, it->second)
        it= hashMap.Find( 3 );           UT_TRUE(  it == hashMap.end() )

    }

    // automatic rehash
    {
        hashMap.Emplace( static_cast<int>(1 + hashMap.BucketCount()), 102 );  testConsistency(ut, hashMap,     3 ,  11,  0 );
        hashMap.Emplace( 1                        , 103 );  testConsistency(ut, hashMap,     4 ,  11,  0 );

        auto inserts= static_cast<int>(   hashMap.MaxLoadFactor() * static_cast<float>(hashMap.BucketCount() )
                                        - static_cast<float>(hashMap.Size())                                     );
        for( int i= 0 ; i < inserts -1 ; ++i )
            hashMap.Emplace( 2000 + rand() % 100, 555  );

        auto oldBucketCount=  hashMap.BucketCount();
        UT_TRUE( hashMap.RecyclablesCount() == 0 )
        hashMap.Emplace( 1000, 1000 );
        UT_TRUE( oldBucketCount < hashMap.BucketCount() )

        testConsistency(ut, hashMap,   hashMap.Size(), 23, -1 );
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
        hashMap.Emplace( 0,   0);         testConsistency(ut, hashMap,     1 ,  11,  cntRecylables );
        hashMap.Emplace( 0,   1);
        hashMap.Emplace(11,   1);
        hashMap.Emplace(11, 110);

        hashMap.Emplace( 1,   110);
        hashMap.Emplace( 12,  120);       testConsistency(ut, hashMap,     6 ,  11,  cntRecylables );

        auto
        result= hashMap.EqualRange(  0 ); UT_EQ(     0, result.first.Key() )
        ++result.first;                   UT_EQ(     0, result.first.Key() )
        ++result.first;                   UT_FALSE(  0  == result.first.Key() )
                                          UT_TRUE (  result.first == result.second )
        result= hashMap.EqualRange(  1 ); UT_EQ(     1, result.first.Key() )
        ++result.first;                   UT_TRUE (  result.first == result.second )
        result= hashMap.EqualRange( 11 ); UT_EQ(    11, result.first.Key() )
        ++result.first;                   UT_EQ(    11, result.first.Key() )
        ++result.first;                   UT_FALSE( 11  == result.first.Key() )
                                          UT_TRUE (  result.first == result.second )
        result= hashMap.EqualRange( 12 ); UT_EQ(    12, result.first.Key() )
        ++result.first;                   UT_TRUE (  result.first == result.second )
        cntRecylables= 6;
    }

    // Erase( pos )
    {
        hashMap.Clear();                  testConsistency(ut, hashMap,     0 ,  11,  cntRecylables );
        hashMap.Emplace(0, 0);
        hashMap.Emplace(0, 1);
        hashMap.Emplace(11, 110);
        hashMap.Emplace(1, 10);
        hashMap.Emplace(12, 120);         testConsistency(ut, hashMap,     5 ,  11,  cntRecylables - 5 );
                                          testBucket(ut, hashMap,     0, 3 );
                                          testBucket(ut, hashMap,     1, 2 );

        auto start= hashMap.begin();      testConsistency(ut, hashMap,     5 ,  11,  cntRecylables - 5 );
                                          UT_EQ(110,  start.Mapped() )

        start= hashMap.Erase( start );    testConsistency(ut, hashMap,     4 ,  11,  cntRecylables - 4 );
                                          testBucket(ut, hashMap,     0, 2 );
                                          UT_EQ(  1,  start.Mapped() )

        start= hashMap.Erase( start );    testConsistency(ut, hashMap,     3 ,  11,  cntRecylables - 3 );
                                          testBucket(ut, hashMap,     0, 1 );
                                          UT_EQ(  0,  start.Mapped() )

        start= hashMap.Erase( start );    testConsistency(ut, hashMap,     2 ,  11,  cntRecylables - 2 );
                                          testBucket(ut, hashMap,     0, 0 );
                                          UT_EQ(120,  start.Mapped() )

        ++start;                          UT_EQ( 10,  start.Mapped() )
        start= hashMap.Erase( start );    testConsistency(ut, hashMap,     1 ,  11,  cntRecylables - 1 );
                                          testBucket(ut, hashMap,     1, 1 );
                                          UT_TRUE( hashMap.end() == start )
        start= hashMap.begin();
        start= hashMap.Erase( start );    testConsistency(ut, hashMap,     0 ,  11,  cntRecylables - 0 );
                                          testBucket(ut, hashMap,     1, 0 );
                                          UT_TRUE( hashMap.end() == start )
    }


    // Erase( start, end )
    {
        hashMap.Clear();                        testConsistency(ut, hashMap,     0 ,  11,  cntRecylables - 0 );
        hashMap.Emplace(0, 0);
        hashMap.Emplace(0, 1);
        hashMap.Emplace(11, 110);
        hashMap.Emplace(1, 10);
        hashMap.Emplace(12, 120);               testConsistency(ut, hashMap,     5 ,  11,  cntRecylables - 5 );
                                                testBucket(ut, hashMap,     0, 3 );
                                                testBucket(ut, hashMap,     1, 2 );

        auto start= hashMap.begin();            UT_EQ(110,  start.Mapped() )
        auto end  = hashMap.begin();            UT_EQ(110,  end  .Mapped() )
        start= hashMap.Erase(start,end);        testConsistency(ut, hashMap,     5 ,  11,  cntRecylables - 5 );
                                                UT_EQ(110,  start.Mapped() )

        start= hashMap.begin();                 UT_EQ(110,  start.Mapped() )
        end  = hashMap.begin(); ++end;          UT_EQ(  1,  end  .Mapped() )
        start= hashMap.Erase(start,end);        testConsistency(ut, hashMap,     4 ,  11,  cntRecylables - 4 );
                                                UT_TRUE(  start == end )
                                                UT_EQ(  1,  start.Mapped() )

        start++;                                UT_EQ(  0,  start.Mapped() )
                                                testBucket(ut, hashMap,     0, 2 );
                                                testBucket(ut, hashMap,     1, 2 );
        end= start; ++end;  ++end;              UT_EQ( 10,  end  .Mapped() )
        start= hashMap.Erase( start, end );     testConsistency(ut, hashMap,     2 ,  11,  cntRecylables - 2 );
                                                testBucket(ut, hashMap,     0, 1 );
                                                testBucket(ut, hashMap,     1, 1 );
                                                UT_EQ( 10,  start.Mapped() )

        hashMap.Erase( hashMap.begin(),
                       hashMap.end() );         testConsistency(ut, hashMap,     0 ,  11,  cntRecylables  );
        hashMap.Emplace(0, 0);
        hashMap.Emplace(0, 1);
        hashMap.Emplace(11, 110);
        hashMap.Emplace(1, 10);
        hashMap.Emplace(12, 120);               testConsistency(ut, hashMap,     5 ,  11,  cntRecylables - 5 );
        hashMap.Erase( hashMap.begin(),
                       hashMap.end() );         testConsistency(ut, hashMap,     0 ,  11,  cntRecylables - 0 );
    }


    // Erase( pos ) with bucket iterator
    {
        hashMap.Clear();                  testConsistency(ut, hashMap,     0 ,  11,  cntRecylables );
        hashMap.Emplace(0, 0);
        hashMap.Emplace(0, 1);
        hashMap.Emplace(11, 110);
        hashMap.Emplace(1, 10);
        hashMap.Emplace(12, 120);         testConsistency(ut, hashMap,     5 ,  11,  cntRecylables - 5 );
                                          testBucket(ut, hashMap,     0, 3 );
                                          testBucket(ut, hashMap,     1, 2 );

        auto start= hashMap.begin(0);     UT_EQ(110,  start.Mapped() )

        start= hashMap.Erase( start );    testConsistency(ut, hashMap,     4 ,  11,  cntRecylables - 4 );
                                          testBucket(ut, hashMap,     0, 2 );
                                          UT_EQ(  1,  start.Mapped() )

        start= hashMap.Erase( start );    testConsistency(ut, hashMap,     3 ,  11,  cntRecylables - 3 );
                                          testBucket(ut, hashMap,     0, 1 );
                                          UT_EQ(  0,  start.Mapped() )

        start= hashMap.Erase( start );    testConsistency(ut, hashMap,     2 ,  11,  cntRecylables - 2 );
                                          testBucket(ut, hashMap,     0, 0 );
        hashMap.begin(0);                 UT_TRUE( hashMap.end(0) == start )

        start= hashMap.begin(1);          UT_EQ(120,  start.Mapped() )
        ++start;                          UT_EQ( 10,  start.Mapped() )
        start= hashMap.Erase( start );    testConsistency(ut, hashMap,     1 ,  11,  cntRecylables - 1 );
                                          testBucket(ut, hashMap,     1, 1 );
                                          UT_TRUE( hashMap.end(1) == start )
        start= hashMap.begin(1);
        start= hashMap.Erase( start );    testConsistency(ut, hashMap,     0 ,  11,  cntRecylables - 0 );
                                          testBucket(ut, hashMap,     1, 0 );
                                          UT_TRUE( hashMap.end(1) == start )
    }

    // Erase (start,end) with bucket iterators
    {
        hashMap.Clear();
        hashMap.Emplace(0, 0);
        hashMap.Emplace(0, 1);
        hashMap.Emplace(11, 110);
        hashMap.Emplace(1, 10);
        hashMap.Emplace(12, 120);               testConsistency(ut, hashMap,     5 ,  11,  cntRecylables - 5 );
                                                testBucket(ut, hashMap,     0, 3 );
                                                testBucket(ut, hashMap,     1, 2 );

        auto start= hashMap.begin(0);           UT_EQ(110,  start.Mapped() )
        auto end  = hashMap.begin(0);           UT_EQ(110,  end  .Mapped() )
        start= hashMap.Erase(start,end);        testConsistency(ut, hashMap,     5 ,  11,  cntRecylables - 5 );
                                                UT_EQ(110,  start.Mapped() )

        start= hashMap.begin(0);                UT_EQ(110,  start.Mapped() )
        end  = hashMap.begin(0); ++end;         UT_EQ(  1,  end  .Mapped() )
        start= hashMap.Erase(start,end);        testConsistency(ut, hashMap,     4 ,  11,  cntRecylables - 4 );
                                                UT_TRUE(  start == end )
                                                UT_EQ(  1,  start.Mapped() )
                                                testBucket(ut, hashMap,     0, 2 );

        start++;                                UT_EQ(  0,  start.Mapped() )
        start= hashMap.Erase(start,
                             hashMap.end(0));   testConsistency(ut, hashMap,     3 ,  11,  cntRecylables - 3 );
                                                testBucket(ut, hashMap,     0, 1 );
                                                testBucket(ut, hashMap,     1, 2 );
                                                UT_TRUE(start == hashMap.end(0) )

        start= hashMap.Erase(hashMap.begin(0),
                             hashMap.end  (0)); testConsistency(ut, hashMap,     2 ,  11,  cntRecylables - 2 );
                                                testBucket(ut, hashMap,     0, 0 );
                                                testBucket(ut, hashMap,     1, 2 );
                                                UT_TRUE(start == hashMap.end(0) )



        start= hashMap.begin(1); ++start;       UT_EQ( 10,  start.Mapped() )
        end  = hashMap.end(1);
        start= hashMap.Erase(start,end);        testConsistency(ut, hashMap,     1 ,  11,  cntRecylables - 1 );
                                                UT_TRUE(start == hashMap.end(1) )
                                                testBucket(ut, hashMap,     1, 1 );
        start= hashMap.Erase(hashMap.begin(1),
                             hashMap.end  (1)); testConsistency(ut, hashMap,     0 ,  11,  cntRecylables - 0 );
                                                testBucket(ut, hashMap,     0, 0 );
                                                testBucket(ut, hashMap,     1, 0 );
    }

    // Extract and Insert(handle)
    {
        hashMap.Clear();
        hashMap.Emplace(0, 0);
        hashMap.Emplace(0, 1);
        hashMap.Emplace(11, 110);
        hashMap.Emplace(1, 10);
        hashMap.Emplace(12, 120);                           testConsistency(ut, hashMap,     5 ,  11,  cntRecylables - 5 );
                                                            testBucket(ut, hashMap,     0, 3 );
                                                            testBucket(ut, hashMap,     1, 2 );

        // extract by pos and add at the same spot with same key existing
        auto start = hashMap.begin() ;                      UT_EQ(110,  start.Mapped() )
        auto handle= hashMap.Extract(start);                testConsistency(ut, hashMap,     4 ,  11,  cntRecylables - 5 );
                                                            testBucket(ut, hashMap,     0, 2 );
                                                            testBucket(ut, hashMap,     1, 2 );
                                                            UT_FALSE(   handle.IsEmpty() )
                                                            UT_EQ( 11,  handle.Key  ()   )
                                                            UT_EQ(110,  handle.Mapped()  )
        auto result= hashMap.Insert(handle);                testConsistency(ut, hashMap,     5 ,  11,  cntRecylables - 5 );
                                                            UT_TRUE(    handle.IsEmpty() )
                                                            UT_TRUE(  result == hashMap.Find(11) )
                                                            UT_TRUE(   handle.IsEmpty() )

        // extract by pos and add somewhere else
        start = hashMap.begin() ;                           UT_EQ(110,  start.Mapped() )
        handle= hashMap.Extract(start);                     testConsistency(ut, hashMap,     4 ,  11,  cntRecylables - 5 );
                                                            testBucket(ut, hashMap,     0, 2 );
                                                            testBucket(ut, hashMap,     1, 2 );
                                                            UT_FALSE(   handle.IsEmpty() )
                                                            UT_EQ( 11,  handle.Key  ()   )
                                                            UT_EQ(110,  handle.Mapped()  )
        handle.Key()= 5;                                    UT_EQ(  5,  handle.Key  ()   )
        result= hashMap.Insert(handle);                     testConsistency(ut, hashMap,     5 ,  11,  cntRecylables - 5 );
                                                            UT_TRUE(    handle.IsEmpty() )
                                                            UT_TRUE(  result == hashMap.Find(5) )
                                                            UT_TRUE(   handle.IsEmpty() )
                                                            testBucket(ut, hashMap,     0, 2 );
                                                            testBucket(ut, hashMap,     1, 2 );
                                                            testBucket(ut, hashMap,     5, 1 );

        // extract by key and add somewhere else
        handle= hashMap.Extract(1);                         testConsistency(ut, hashMap,     4 ,  11,  cntRecylables - 5 );
                                                            testBucket(ut, hashMap,     0, 2 );
                                                            testBucket(ut, hashMap,     1, 1 );
                                                            testBucket(ut, hashMap,     5, 1 );
                                                            UT_FALSE(   handle.IsEmpty() )
                                                            UT_EQ(  1,  handle.Key    () )
                                                            UT_EQ( 10,  handle.Mapped () )
        // ...unsuccessful insert
        handle.Key()= 5;                                    UT_EQ(  5,  handle.Key  () )
                                                            UT_EQ( 10,  handle.Mapped() )
        result= hashMap.InsertIfNotExistent(handle);        testConsistency(ut, hashMap,     4 ,  11,  cntRecylables - 5 );
                                                            UT_FALSE( handle.IsEmpty () )
                                                            UT_EQ(  5,  handle.Key   () )
                                                            UT_EQ( 10,  handle.Mapped() )
                                                            testBucket(ut, hashMap,     0, 2 );
                                                            testBucket(ut, hashMap,     1, 1 );
                                                            testBucket(ut, hashMap,     5, 1 );
        // ...successful insert
        handle.Key()= 6;                                    UT_EQ(  6,  handle.Key  () )
        result= hashMap.InsertIfNotExistent(handle);        testConsistency(ut, hashMap,     5 ,  11,  cntRecylables - 5 );
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
        result= hashMap.InsertUnique(std::make_pair( 0, 100));   testConsistency(ut, hashMap,     1 ,  11,  0 );
                                                                  UT_EQ( 100 ,  result.Mapped() )
        result= hashMap.InsertUnique(std::make_pair(11, 110));   testConsistency(ut, hashMap,     2 ,  11,  0 );
                                                                  UT_EQ( 110 ,  result.Mapped() )

        lib::results::Report::GetDefault().PushHaltFlags( false, false );
        UT_PRINT( "An error should follow" )

        result= hashMap.InsertUnique(std::make_pair( 0, 101));   testConsistency(ut, hashMap,     3 ,  11,  0 );
                                                                  UT_EQ( 101 ,  result.Mapped() )
        lib::results::Report::GetDefault().PopHaltFlags();


        result= hashMap.EmplaceUnique( 1, 200);                  testConsistency(ut, hashMap,     4 ,  11,  0 );
                                                                  UT_EQ( 200 ,  result.Mapped() )
        result= hashMap.EmplaceUnique(12, 210);                  testConsistency(ut, hashMap,     5 ,  11,  0 );
                                                                  UT_EQ( 210 ,  result.Mapped() )

        lib::results::Report::GetDefault().PushHaltFlags( false, false );
        UT_PRINT( "An error should follow" )

        result= hashMap.EmplaceUnique(1, 201);                   testConsistency(ut, hashMap,     6 ,  11,  0 );
                                                                  UT_EQ( 201 ,  result.Mapped() )
        lib::results::Report::GetDefault().PopHaltFlags();
    }


    // EmplaceOrAssign, EmplaceIfNotExistent
    {
        hashMap.Reset();                            testConsistency(ut, hashMap,     0 ,  0 ,  0 );
        auto
        result= hashMap.EmplaceOrAssign(0, 0);      testConsistency(ut, hashMap,     1 ,  11,  0 );
                                                    UT_EQ(   0 ,  result.first.Mapped() )
                                                    UT_EQ(true ,  result.second  ) // is insert
                                                    UT_EQ(   0 ,  hashMap.begin().Mapped() )
                                                    UT_TRUE( result.first == hashMap.Find(0) )

        result= hashMap.EmplaceIfNotExistent(0, 0);      testConsistency(ut, hashMap,     1 ,  11,  0 );
                                                    UT_EQ(   0 ,  result.first.Mapped() )
                                                    UT_EQ(false,  result.second  ) // nothing done
                                                    UT_EQ(   0 ,  hashMap.begin().Mapped() )
                                                    UT_TRUE( result.first == hashMap.Find(0) )


        result= hashMap.EmplaceOrAssign(0, 1);      testConsistency(ut, hashMap,     1 ,  11,  0 );
                                                    UT_EQ(false,  result.second  ) // is replace
                                                    UT_TRUE( result.first == hashMap.Find(0) )
        result= hashMap.EmplaceOrAssign(0, 2);      testConsistency(ut, hashMap,     1 ,  11,  0 );
                                                    UT_EQ(   2 ,  result.first.Mapped() )
                                                    UT_EQ(false,  result.second  ) // is replace
                                                    UT_EQ(   2 ,  hashMap.begin().Mapped() )
                                                    UT_TRUE( result.first == hashMap.Find(0) )

        result= hashMap.EmplaceOrAssign(1, 3);      testConsistency(ut, hashMap,     2 ,  11,  0 );
                                                    UT_EQ(   3 ,  result.first.Mapped() )
                                                    UT_EQ(true ,  result.second  ) // is insert
                                                    UT_TRUE( result.first == hashMap.Find(1) )
        result= hashMap.EmplaceOrAssign(1, 4);      testConsistency(ut, hashMap,     2 ,  11,  0 );
                                                    UT_EQ(   4 ,  result.first.Mapped() )
                                                    UT_EQ(false,  result.second  ) // is replace
                                                    UT_TRUE( result.first == hashMap.Find(1) )
        result= hashMap.EmplaceIfNotExistent(1, 5); testConsistency(ut, hashMap,     2 ,  11,  0 );
                                                    UT_EQ(   4 ,  result.first.Mapped() )
                                                    UT_EQ(false,  result.second  ) // nothing done
                                                    UT_TRUE( result.first == hashMap.Find(1) )

        result= hashMap.EmplaceIfNotExistent(2, 12);testConsistency(ut, hashMap,     3 ,  11,  0 );
                                                    UT_EQ(  12 ,  result.first.Mapped() )
                                                    UT_EQ(true ,  result.second  ) // is insert
                                                    UT_TRUE( result.first == hashMap.Find(2) )

        result= hashMap.EmplaceIfNotExistent(3, 13);testConsistency(ut, hashMap,     4 ,  11,  0 );
                                                    UT_EQ(  13 ,  result.first.Mapped() )
                                                    UT_EQ(true ,  result.second  ) // is insert
                                                    UT_TRUE( result.first == hashMap.Find(3) )

        // test that returned iterator survives rehash
        result= hashMap.EmplaceOrAssign(4, 14);     testConsistency(ut, hashMap,     5 ,  11,  -1 );
        result= hashMap.EmplaceOrAssign(5, 15);     testConsistency(ut, hashMap,     6 ,  11,  -1 );

        auto inserts= static_cast<int>(   hashMap.MaxLoadFactor() * static_cast<float>(hashMap.BucketCount())
                                        - static_cast<float>(hashMap.Size())                                   );
        for( int i= 0 ; i < inserts -1 ; ++i )
            hashMap.Emplace( 6000 + (rand() % 100), 1234 );
                                                    testConsistency(ut, hashMap,    21 ,  11,  -1 );

        result= hashMap.EmplaceOrAssign(0, 5);      testConsistency(ut, hashMap,    21 ,  11,  -1 );
                                                    UT_EQ(   5 ,  result.first.Mapped() )
                                                    UT_EQ(false,  result.second  ) // is replace
                                                    UT_TRUE( result.first == hashMap.Find(0) )

        result= hashMap.EmplaceOrAssign(100, 110);  testConsistency(ut, hashMap,    22 ,  23,  -1 );
                                                    UT_EQ( 110 ,  result.first.Mapped() )
                                                    UT_EQ(true,  result.second  ) // is insert
                                                    UT_TRUE( result.first == hashMap.Find(100) )
    }

    // InsertOrAssign, InsertIfNotExistent
    {
        hashMap.Reset();                           testConsistency(ut, hashMap,     0 ,  0 ,  -1 );
        auto
        result= hashMap.InsertOrAssign(0, 0);      testConsistency(ut, hashMap,     1 ,  11,  -1 );
                                                    UT_EQ(   0 ,  result.first.Mapped() )
                                                    UT_EQ(true ,  result.second  ) // is insert
                                                    UT_EQ(   0 ,  hashMap.begin().Mapped() )
                                                    UT_TRUE( result.first == hashMap.Find(0) )

        result= hashMap.InsertIfNotExistent(0, 0);  testConsistency(ut, hashMap,     1 ,  11,  -1 );
                                                    UT_EQ(   0 ,  result.first.Mapped() )
                                                    UT_EQ(false,  result.second  ) // nothing done
                                                    UT_EQ(   0 ,  hashMap.begin().Mapped() )
                                                    UT_TRUE( result.first == hashMap.Find(0) )


        result= hashMap.InsertOrAssign(0, 1);       testConsistency(ut, hashMap,     1 ,  11,  -1 );
                                                    UT_EQ(false,  result.second  ) // is replace
                                                    UT_TRUE( result.first == hashMap.Find(0) )
        result= hashMap.InsertOrAssign(0, 2);       testConsistency(ut, hashMap,     1 ,  11,  -1 );
                                                    UT_EQ(   2 ,  result.first.Mapped() )
                                                    UT_EQ(false,  result.second  ) // is replace
                                                    UT_EQ(   2 ,  hashMap.begin().Mapped() )
                                                    UT_TRUE( result.first == hashMap.Find(0) )

        result= hashMap.InsertOrAssign(1, 3);       testConsistency(ut, hashMap,     2 ,  11,  -1 );
                                                    UT_EQ(   3 ,  result.first.Mapped() )
                                                    UT_EQ(true ,  result.second  ) // is insert
                                                    UT_TRUE( result.first == hashMap.Find(1) )
        result= hashMap.InsertOrAssign(1, 4);       testConsistency(ut, hashMap,     2 ,  11,  -1 );
                                                    UT_EQ(   4 ,  result.first.Mapped() )
                                                    UT_EQ(false,  result.second  ) // is replace
                                                    UT_TRUE( result.first == hashMap.Find(1) )
        result= hashMap.InsertIfNotExistent(1, 5);  testConsistency(ut, hashMap,     2 ,  11,  -1 );
                                                    UT_EQ(   4 ,  result.first.Mapped() )
                                                    UT_EQ(false,  result.second  ) // nothing done
                                                    UT_TRUE( result.first == hashMap.Find(1) )

        result= hashMap.InsertIfNotExistent(2, 12); testConsistency(ut, hashMap,     3 ,  11,  -1 );
                                                    UT_EQ(  12 ,  result.first.Mapped() )
                                                    UT_EQ(true ,  result.second  ) // is insert
                                                    UT_TRUE( result.first == hashMap.Find(2) )

        result= hashMap.InsertIfNotExistent(3, 13); testConsistency(ut, hashMap,     4 ,  11,  -1 );
                                                    UT_EQ(  13 ,  result.first.Mapped() )
                                                    UT_EQ(true ,  result.second  ) // is insert
                                                    UT_TRUE( result.first == hashMap.Find(3) )

        // test that returned iterator survives rehash
        result= hashMap.InsertOrAssign(4, 14);      testConsistency(ut, hashMap,     5 ,  11,  -1 );
        result= hashMap.InsertOrAssign(5, 15);      testConsistency(ut, hashMap,     6 ,  11,  -1 );

        auto inserts= static_cast<int>(   hashMap.MaxLoadFactor() * static_cast<float>(hashMap.BucketCount())
                                        - static_cast<float>(hashMap.Size())                                   );
        for( int i= 0 ; i < inserts -1 ; ++i )
            hashMap.Insert( std::make_pair(4000 + rand() % 100, 999) );

                                                    testConsistency(ut, hashMap,    21 ,  11,  -1 );

        result= hashMap.InsertOrAssign(0, 5);       testConsistency(ut, hashMap,    21 ,  11,  -1 );
                                                    UT_EQ(   5 ,  result.first.Mapped() )
                                                    UT_EQ(false,  result.second  ) // is replace
                                                    UT_TRUE( result.first == hashMap.Find(0) )

        result= hashMap.InsertOrAssign(100, 110);   testConsistency(ut, hashMap,    22 ,  23,  -1 );
                                                    UT_EQ( 110 ,  result.first.Mapped() )
                                                    UT_EQ(true,  result.second  ) // is insert
                                                    UT_TRUE( result.first == hashMap.Find(100) )
    }
}


DOX_MARKER( [DOX_ALIB_MONOMEM_SELFCONTAINED_1] )
struct FieldsOfDictionary
{
    HashMap< String32             ,
             String32             ,
             std::hash    <String>,
             std::equal_to<String> > map;

    FieldsOfDictionary()
        : map( nullptr ) // allocator not available, yet
    {}
};
DOX_MARKER( [DOX_ALIB_MONOMEM_SELFCONTAINED_1] )

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunused-member-function"
#endif

DOX_MARKER( [DOX_ALIB_MONOMEM_SELFCONTAINED_2] )
struct Dictionary : protected aworx::lib::monomem::SelfContained<FieldsOfDictionary>
{
    Dictionary()
    : SelfContained( 1024, 100 )
    {
        Self().map.SetAllocatorPostConstruction( &fields->allocator );
    }

    Dictionary( Dictionary && move )
    : SelfContained( std::move( move ) ) // this "steals" everything from object 'move'
    {}


    void Define( const String& src, const String& translation )
    {
        Self().map.EmplaceOrAssign( src, translation );
    }


    const String Translate( const String& src )
    {
        auto result = Self().map.Find( src );
        if ( result == Self().map.end() )
            return EmptyString();

        return result.Mapped();
    }
};
DOX_MARKER( [DOX_ALIB_MONOMEM_SELFCONTAINED_2] )

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

} //anonymous namespace

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

UT_METHOD(StdContainers)
{
UT_INIT()

// #################################################################################################
// ### StdContMA
// #################################################################################################
{
    lib::monomem::MonoAllocator ma( 512 );

    {
        UT_PRINT(  "---- std::unordered_map, strict monotonic  ----")
        StdContMA<std::pair<const AString,long double>> stdContMa(ma);

ALIB_DBG(   stdContMa.dbgDeallocationWarning= false;    )
        std::unordered_map< AString, long double,
                            std::hash    <String>,
                            std::equal_to<String>,
                            StdContMA    <std::pair<const AString,long double>> > umap( 5  , std::hash<String>(), std::equal_to<String>(), stdContMa );
        for( int i= 0; i < 20 ; ++i)
            umap.insert( std::pair<AString,int>(String128() << "Key" << i, i * 10) );

    }

    {
        UT_PRINT("-------   std::map, strict monotonic -------")
        std::map          < AString, double,
                            std::less<String>,
                            StdContMA<std::pair<const AString, double>>  > omap(
                                std::less<String>(),
                                (StdContMA<std::pair<const AString, double>>(ma ALIB_DBG(,true))) );


        for( int i= 0; i < 10 ; ++i)
            omap.insert( std::pair<AString,int>(String128() << "Key" << i, i * 10) );
        while( omap.begin() != omap.end() )
            omap.erase( omap.begin() );
        for( int i= 0; i < 15 ; ++i)
            omap.insert( std::pair<AString,int>(String128() << "Key" << i, i * 10) );
    }

    {
        UT_PRINT("-------   std::list, strict monotonic   -------")
        std::list         < AString,
                            StdContMA<AString>  > list( (StdContMA<AString>(ma)) );


        for( int i= 0; i < 5 ; ++i)
            list.push_front( String128() << "Key" << i );
        while( list.begin() != list.end() )
            list.erase( list.begin() );
        for( int i= 0; i < 7 ; ++i)
            list.push_front( String128() << "Key" << i );
    }

    {
        UT_PRINT("-------  std::vector, strict monotonic  -------")
DOX_MARKER([DOX_ALIB_MONOMEM_STDCONTMA_DECL])
struct MyStruct
{
    std::vector<int, aworx::StdContMA<int>>  myField;
};
DOX_MARKER([DOX_ALIB_MONOMEM_STDCONTMA_DECL])

DOX_MARKER([DOX_ALIB_MONOMEM_STDCONTMA_DEF])
aworx::MonoAllocator myAllocator( 4096 );

std::vector<int, StdContMA<int>>  myVector( (aworx::StdContMA<int>(myAllocator)) );
DOX_MARKER([DOX_ALIB_MONOMEM_STDCONTMA_DEF])

        for( int i= 0; i < 20 ; ++i)
            myVector.push_back( i );
    }
}

// #################################################################################################
// ### StdContMARecycling
// #################################################################################################

{
    lib::monomem::MonoAllocator ma( 512 );

    RTTRAllocator recyclerUM( &ma );
    RTTRAllocator recyclerMap( &ma );
    RTTRAllocator recyclerVec( &ma );
    RTTRAllocator recyclerList( &ma );
    StdContMARecycling<std::pair<const AString,long double>> rum( recyclerUM );

    {
        UT_PRINT(  "---- std::unordered_map, recycling  ----")
        std::unordered_map< AString, long double,
                            std::hash    <String>,
                            std::equal_to<String>,
                            StdContMARecycling<std::pair<const AString, long double>>> umap( 5, std::hash<String>(), std::equal_to<String>(), rum );
        for( int i= 0; i < 20 ; ++i)
            umap.insert( std::pair<AString,int>(String128() << "Key" << i, i * 10) );
    }

    {
        UT_PRINT("-------   std::map, recycling    -------")
        std::map          < AString, double,
                            std::less<String>,
                            StdContMARecycling<std::pair<const AString, double>>  > omap( std::less<String>(), (StdContMARecycling<std::pair<const AString, double>>(recyclerUM)) );


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
                            StdContMARecycling<AString>  > list( (StdContMARecycling<AString>(recyclerList)) );


        for( int i= 0; i < 5 ; ++i)
            list.push_front( String128() << "Key" << i );
        while( list.begin() != list.end() )
            list.erase( list.begin() );
        for( int i= 0; i < 7 ; ++i)
            list.push_front( String128() << "Key" << i );
    }

}
}

//--------------------------------------------------------------------------------------------------
//--- Create
//--------------------------------------------------------------------------------------------------
UT_METHOD(SelfContained)
{
    UT_INIT()

                                                    DOX_MARKER( [DOX_ALIB_MONOMEM_ALLOCATOR_CREATE] )
// create a self-contained allocator.
// This performs 1 dynamic memory allocation for the first chunk of the allocator
// The pointer returned points to the allocator object located inside this first chunk.
MonoAllocator* monoAllocator= MonoAllocator::Create(1024);

// This monotonic allocation will also use the first chunk (no second dynamic allocation).
NString clone= monoAllocator->EmplaceString( NString128() << "Result is: " << 42 );

// Destruct the allocator. Invokes monoAllocator->~MonoAllocator()
lib::monomem::Destruct( monoAllocator );
                                                    DOX_MARKER( [DOX_ALIB_MONOMEM_ALLOCATOR_CREATE] )

(void) clone;


                                                    DOX_MARKER( [DOX_ALIB_MONOMEM_SELFCONTAINED_3] )
// create an instance of the self-contained dictionary type
Dictionary germanEnglish;

// interestingly, the size of class Dictionary is that of a single pointer
assert( sizeof( Dictionary) == sizeof( void* ) );

// add vocabulary
germanEnglish.Define( A_CHAR( "Spass"        )  , A_CHAR( "fun"          ) );
germanEnglish.Define( A_CHAR( "Kindergarten" )  , A_CHAR( "kindergarten" ) );

// search a word
UT_PRINT( "Spass in English is {!Q}.", germanEnglish.Translate( A_CHAR( "Spass") )  )
                                                    DOX_MARKER( [DOX_ALIB_MONOMEM_SELFCONTAINED_3] )
}

#if ALIB_DEBUG_MONOMEM
// ########################################################################################
// HashTable Debug dump
// ########################################################################################
UT_METHOD(HashTable_Distribution)
{
    UT_INIT()

    // These tests do not test any output. But being able to compile the templated methods is
    // already quite good ;-)

    MonoAllocator ma(1024);
    HashSet<String> hashSet(&ma);
    hashSet.Emplace( "Hello" );
    hashSet.Emplace( "World" );
    hashSet.Emplace( ","   );
    hashSet.Emplace( "how"   );
    hashSet.Emplace( "are"   );
    hashSet.Emplace( "you"   );
    hashSet.Emplace( "today" );
    hashSet.Emplace( "?" );

    UT_PRINT( "Test table contents:" )
    UT_PRINT( DbgDumpHashtable( hashSet ) )

    UT_PRINT( "Test table statistics:" )
    UT_PRINT( DbgDumpDistribution(hashSet, true) )
}
#endif //  ALIB_DEBUG_MONOMEM

//--------------------------------------------------------------------------------------------------
//--- List
//--------------------------------------------------------------------------------------------------
UT_METHOD(List)
{
    UT_INIT()

    CharTriv avoidUnneededDefaultConstructorWarning;
    (void)  avoidUnneededDefaultConstructorWarning;

    ListTest<char     >( ut );

    ListTest<CharTriv >( ut );

    UT_EQ( CharDyn::InstanceCounter(), 0 )
        ListTest<CharDyn >( ut );
    UT_EQ( CharDyn::InstanceCounter(), 0 )
}

//--------------------------------------------------------------------------------------------------
//--- Recycling
//--------------------------------------------------------------------------------------------------
UT_METHOD(Recycling)
{
    // Note: the main purpose of this unit tests is to ensure that the rather complicated
    //       type definitions compile well.
    //       The fact that recycling in general works is tested directly and inherently with
    //       other tests.
    UT_INIT()

    // List private
    {
MonoAllocator monoAllocator(1024);
aworx::List<int>   list( &monoAllocator );

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
MonoAllocator monoAllocator(1024);

aworx::List<int, Recycling::Shared>::TSharedRecycler sharedRecycler;
aworx::List<int, Recycling::Shared>  list1( &monoAllocator, sharedRecycler );
aworx::List<int, Recycling::Shared>  list2( &monoAllocator, sharedRecycler );

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
MonoAllocator monoAllocator(1024);

aworx::List<int, Recycling::None>  list( &monoAllocator );

list.PushFront(1);
list.PopFront();

    }


    // HashSet shared recycler
    {
DOX_MARKER( [DOX_ALIB_MONOMEM_RECYCLER] )
MonoAllocator monoAllocator(1024);

// Type definition for a hash set using a shared recycler
using MySet= HashSet< int,
                      std::hash<int>,
                      std::equal_to<int>,
                      Caching::Disabled,
                      Recycling::Shared      >;  // <-- shared recycling!

// The shared recycler instance
MySet::TSharedRecycler sharedRecycler;

// Two hash set instances. The shared recycler has to be passed to the constructor.
MySet  set1( &monoAllocator, sharedRecycler );
MySet  set2( &monoAllocator, sharedRecycler );

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

set1.ReserveRecyclables(10, lib::ValueReference::Absolute);
                              UT_EQ( 10, set1.RecyclablesCount() )
                              UT_EQ( 10, set2.RecyclablesCount() )
DOX_MARKER( [DOX_ALIB_MONOMEM_RECYCLER] )
    }

    // HashSet no recycler
    {
MonoAllocator monoAllocator(1024);
using MySharedSet= HashSet<int,
                           std::hash<int>,
                           std::equal_to<int>,
                           Caching::Disabled,
                           Recycling::None >;
MySharedSet  set( &monoAllocator );

set.Emplace(1);
set.Erase(1);

    }

    // HashMap shared recycler
    {
MonoAllocator monoAllocator(1024);
using MySharedMap= HashMap<int,int,
                           std::hash<int>,
                           std::equal_to<int>,
                           Caching::Disabled,
                           Recycling::Shared >;
MySharedMap::TSharedRecycler sharedRecycler;
MySharedMap  map1( &monoAllocator, sharedRecycler );
MySharedMap  map2( &monoAllocator, sharedRecycler );

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
MonoAllocator monoAllocator(1024);
using MySharedMap= HashMap<int,int,
                           std::hash<int>,
                           std::equal_to<int>,
                           Caching::Disabled,
                           Recycling::None >;
MySharedMap  map( &monoAllocator );

map.Emplace(1,1);
map.Erase(1);

    }
}


//--------------------------------------------------------------------------------------------------
//--- HashTable
//--------------------------------------------------------------------------------------------------
UT_METHOD(HashTable)
{
    UT_INIT()

    static_assert( HashSet<int          >::CachedHashCodes == false, "Wrong default for caching" );
    static_assert( HashSet<integer      >::CachedHashCodes == false, "Wrong default for caching" );
    static_assert( HashSet<float        >::CachedHashCodes == false, "Wrong default for caching" );
    static_assert( HashSet<double       >::CachedHashCodes == false, "Wrong default for caching" );
    static_assert( HashSet<Box          >::CachedHashCodes == true , "Wrong default for caching" );
    static_assert( HashSet<String       >::CachedHashCodes == true , "Wrong default for caching" );

    static_assert( HashSet<int    , int >::CachedHashCodes == false, "Wrong default for caching" );
    static_assert( HashSet<integer, int >::CachedHashCodes == false, "Wrong default for caching" );
    static_assert( HashSet<float  , int >::CachedHashCodes == false, "Wrong default for caching" );
    static_assert( HashSet<double , int >::CachedHashCodes == false, "Wrong default for caching" );
    static_assert( HashSet<Box    , int >::CachedHashCodes == true , "Wrong default for caching" );
    static_assert( HashSet<String , int >::CachedHashCodes == true , "Wrong default for caching" );

    MonoAllocator ba(100);

    (void) ( DynInt() == DynInt() ); // avoid unneeded constructor, == warning for this test class

    {
        UT_PRINT( "---------------------------- HashSet -------------------------------")
        ba.Reset();   { HashSet<int           , IntHash       , std::equal_to<int> ,Caching::Disabled> hashSetI(&ba);   UT_HashSet( ut, hashSetI ); }
        ba.Reset();   { HashSet<DynInt        , DynIntHash    , DynIntEqual        ,Caching::Disabled> hashSetI(&ba);   UT_HashSet( ut, hashSetI ); }
        UT_EQ( DynInt::instCounter, 0)
        ba.Reset();   { HashSet<int           , IntHash       , std::equal_to<int> ,Caching::Enabled > hashSetI(&ba);   UT_HashSet( ut, hashSetI ); }
        ba.Reset();   { HashSet<DynInt        , DynIntHash    , DynIntEqual        ,Caching::Enabled > hashSetI(&ba);   UT_HashSet( ut, hashSetI ); }
        UT_EQ( DynInt::instCounter, 0)

        UT_PRINT( "---------------------------- HashMap -------------------------------")
        ba.Reset();   { HashMap<int   , int   , IntHash       , std::equal_to<int> ,Caching::Disabled> hashMapII(&ba);  UT_HashMap( ut, hashMapII ); }
        ba.Reset();   { HashMap<int   , DynInt, IntHash       , std::equal_to<int> ,Caching::Disabled> hashMapID(&ba);  UT_HashMap( ut, hashMapID ); }
        UT_EQ( DynInt::instCounter, 0)
        ba.Reset();   { HashMap<DynInt, int   , DynIntHash    , DynIntEqual        ,Caching::Disabled> hashMapDI(&ba);  UT_HashMap( ut, hashMapDI ); }
        UT_EQ( DynInt::instCounter, 0)
        ba.Reset();   { HashMap<DynInt, DynInt, DynIntHash    , DynIntEqual        ,Caching::Disabled> hashMapDD(&ba);  UT_HashMap( ut, hashMapDD ); }
        UT_EQ( DynInt::instCounter, 0)
        ba.Reset();   { HashMap<int   , int   , IntHash       , std::equal_to<int> ,Caching::Enabled > hashMapII(&ba);  UT_HashMap( ut, hashMapII ); }
        ba.Reset();   { HashMap<int   , DynInt, IntHash       , std::equal_to<int> ,Caching::Enabled > hashMapID(&ba);  UT_HashMap( ut, hashMapID ); }
        UT_EQ( DynInt::instCounter, 0)
        ba.Reset();   { HashMap<DynInt, int   , DynIntHash    , DynIntEqual        ,Caching::Enabled > hashMapDI(&ba);  UT_HashMap( ut, hashMapDI ); }
        UT_EQ( DynInt::instCounter, 0)
        ba.Reset();   { HashMap<DynInt, DynInt, DynIntHash    , DynIntEqual        ,Caching::Enabled > hashMapDD(&ba);  UT_HashMap( ut, hashMapDD ); }
        UT_EQ( DynInt::instCounter, 0)
    }
}

#include "unittests/aworx_unittests_end.hpp"

} //namespace [ut_aworx]

#endif // ALIB_UT_MONOMEM
