// #################################################################################################
//  AWorx ALib Unit Tests
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"
#include "unittests/alib_test_selection.hpp"
#if ALIB_UT_BITS

#if !defined(HPP_ALIB_LANG_BITSET)
#   include "alib/lang/bitset.hpp"
#endif

#include "alib/alox.hpp"
#include "bitset"

#include <random>

#define TESTCLASSNAME       CPP_ALib_Lib
#include "unittests/aworx_unittests.hpp"


using namespace std;
using namespace alib;
using namespace alib::lang;

namespace ut_aworx {

//--------------------------------------------------------------------------------------------------
//--- BitSet preparations
//--------------------------------------------------------------------------------------------------
template<int SIZE>
void testBitsetCheckResult( AWorxUnitTesting& ut, BitSet<SIZE>& bitset, int cnt )
{
    UT_EQ( cnt      , bitset.Count() )
    UT_EQ( cnt==SIZE, bitset.All  () )
    UT_EQ( cnt>0    , bitset.Any  () )
    UT_EQ( cnt==0   , bitset.None () )
}

template<int SIZE>
void testBitSet(AWorxUnitTesting& ut)
{
    BitSet<SIZE> bitset;
    int cnt;
    for (int i = 0; i < SIZE; ++i) { UT_FALSE( bitset.Test(i) ) }               testBitsetCheckResult(ut,bitset, 0     );

    for (int i = 0; i < SIZE; ++i) { bitset.Set(i);             }               testBitsetCheckResult(ut,bitset, SIZE  );
    for (int i = 0; i < SIZE; ++i) { UT_TRUE ( bitset.Test(i) ) }

    for (int i = 0; i < SIZE; ++i) { bitset.Reset(i);           }               testBitsetCheckResult(ut,bitset, 0     );

    bitset.Set();                                                               testBitsetCheckResult(ut,bitset, SIZE  );
    for (int i = 0; i < SIZE; ++i) { UT_TRUE ( bitset.Test(i) ) }
    bitset.Reset();                                                             testBitsetCheckResult(ut,bitset, 0     );
    for (int i = 0; i < SIZE; ++i) { UT_FALSE( bitset.Test(i) ) }


    for (int i = 0; i < SIZE; ++i) { bitset.Set(i, (i & 1) != 0);  }            testBitsetCheckResult(ut,bitset, SIZE/2);
    for (int i = 0; i < SIZE; ++i) { UT_EQ( (i & 1) != 0, bitset.Test(i) )  }
    cnt= 0;  for( auto it : bitset ) {  ++cnt; UT_TRUE( (it.Bit() & 1) !=0 ) } UT_EQ(SIZE/2   , cnt)

    for (int i = 0; i < SIZE; ++i) { bitset.Flip(i);               }            testBitsetCheckResult(ut,bitset, SIZE - SIZE/2);
    for (int i = 0; i < SIZE; ++i) { UT_EQ( (i & 1) == 0, bitset.Test(i) )  }
    cnt= 0;  for( auto it : bitset ) {  ++cnt; UT_TRUE( (it.Bit() & 1) ==0 ) } UT_EQ(SIZE-SIZE/2   , cnt)
                                     bitset.Flip();                             testBitsetCheckResult(ut,bitset, SIZE/2);

    bitset.Reset();                                                             testBitsetCheckResult(ut,bitset, 0     );
    for (int i = 0; i < SIZE; ++i) { bitset.Set(i, (i % 3) == 2);  }            testBitsetCheckResult(ut,bitset, SIZE/3);
    for (int i = 0; i < SIZE; ++i) { UT_EQ( (i % 3) == 2, bitset.Test(i) )  }
    cnt= 0;  for( auto it : bitset ) {  ++cnt; UT_TRUE( (it.Bit() % 3) == 2 ) } UT_EQ(SIZE/3   , cnt)

    for (int i = 0; i < SIZE; ++i) { bitset.Flip(i);               }            testBitsetCheckResult(ut,bitset, SIZE - SIZE/3);
    for (int i = 0; i < SIZE; ++i) { UT_EQ( (i % 3) != 2, bitset.Test(i) )  }
    cnt= 0;  for( auto it : bitset ) {  ++cnt; UT_TRUE( (it.Bit() % 3) !=2 ) } UT_EQ(SIZE-SIZE/3   , cnt)
                                     bitset.Flip();                             testBitsetCheckResult(ut,bitset, SIZE/3);

    auto
    other=   bitset;  UT_TRUE( bitset == other )
    other=  ~bitset;  UT_TRUE( bitset != other )                                testBitsetCheckResult(ut,other, SIZE - SIZE/3);
    other|=  bitset;  UT_TRUE( bitset != other )                                testBitsetCheckResult(ut,other, SIZE);
    other=  ~bitset;
    other&=  bitset;                                                            testBitsetCheckResult(ut,other, 0);
    other=  ~bitset;
    other^= ~BitSet<SIZE>();  UT_TRUE( bitset == other )

    other=   bitset;  UT_TRUE( bitset == other )
    other=  ~bitset;  UT_TRUE( bitset != other )                                testBitsetCheckResult(ut,other, SIZE - SIZE/3);
    other=  other |  bitset;  UT_TRUE( bitset != other )                        testBitsetCheckResult(ut,other, SIZE);
    other=  ~bitset;
    other=  other &  bitset;                                                    testBitsetCheckResult(ut,other, 0);
    other=  ~bitset;
    other=  other ^ ~BitSet<SIZE>();  UT_TRUE( bitset == other )

    // -------- iterators ---------------
    bitset.Reset(); cnt= 0;  for( auto it : bitset                                   ) { (void) it;                           ++cnt; } UT_EQ(0   , cnt)
    bitset.Set();   cnt= 0;  for( auto it : bitset                                   ) {  UT_EQ(int(cnt), it.Bit())           ++cnt; } UT_EQ(SIZE, cnt)
    bitset.Reset(); cnt= 0;  for( auto it=bitset.rbegin() ; it!=bitset.rend() ; ++it ) { (void) *it;                          ++cnt; } UT_EQ(0   , cnt)
    bitset.Set();   cnt= 0;  for( auto it=bitset.rbegin() ; it!=bitset.rend() ; ++it ) {  UT_EQ(int(SIZE-cnt-1), (*it).Bit()) ++cnt; } UT_EQ(SIZE, cnt)
}

template<int first=-1, int...rest  >
void testBitSets(AWorxUnitTesting& ut)
{
    if constexpr ( first != -1)
    {
        testBitSet<first>(ut);
        testBitSets<rest...>(ut);
    }
}

BitSet<200>::TWord  testGetRandom64()
{
    using tword= BitSet<200>::TWord;
    constexpr int sow= sizeof(tword);
    constexpr int sor= sizeof(decltype(std::rand()));
    tword result= 0;
    for (int i = 0; i < sow; i+= sor)
    {
        tword r= tword(std::rand());
        result= r << (i*8);
    }

    return result;
}

template<int SIZE, size_t STDSIZE>
void  testCompare(AWorxUnitTesting& ut, BitSet<SIZE>& abs, std::bitset<STDSIZE> sbsCpy)
{
    static_assert( SIZE==STDSIZE, "sizes must match" );
    using tword= typename BitSet<SIZE>::TWord;
    static_assert( bitsof(tword) == 64, "only works with large bitsets" );

    constexpr int sow= sizeof(tword);

    bitset<STDSIZE> bsMask(~tword(0));


    for (int size = 0; size < SIZE; size+= sow*8 )
    {
        tword valA= abs.Export(size/(sow*8));
        tword valS= (sbsCpy & bsMask).to_ullong();

        UT_EQ(valA, valS)
        sbsCpy>>= (sow *8);
    }
}

ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
template<int SIZE, size_t TCapacity>
void testBitSetIteration(AWorxUnitTesting& ut,  int const (&bits) [TCapacity])
{
    BitSet<SIZE>    bitSet;

    for (size_t i = 0; i < TCapacity; ++i)
        bitSet.Set( bits[i] );

    size_t idx= 0;
    for( auto it : bitSet )
    {
        UT_EQ( bits[idx++], it.Bit() )
    }
    UT_EQ( idx, TCapacity )

    idx= TCapacity;
    for( auto rit= bitSet.rbegin() ; rit!= bitSet.rend() ; ++rit )
    {
        UT_EQ( bits[--idx], (*rit).Bit() )
    }
    UT_EQ( idx, size_t(0) )

    idx= 0;
    for( const auto& it : bitSet )
    {
        UT_EQ( bits[idx++], it.Bit() )
    }
    UT_EQ( idx, TCapacity )

    idx= TCapacity;
    for( auto rit= bitSet.crbegin() ; rit!= bitSet.crend() ; ++rit )
    {
        UT_EQ( bits[--idx], (*rit).Bit() )
    }
    UT_EQ( idx, size_t(0) )

}
ALIB_WARNINGS_RESTORE

UT_CLASS

//--------------------------------------------------------------------------------------------------
//--- Bits
//--------------------------------------------------------------------------------------------------
UT_METHOD(Bits)
{
    UT_INIT()
    Log_SetDomain( "UT/Bits", Scope::Method )
    UT_PRINT( "" )
    UT_PRINT( "--------------------------- UT_Bits() ---------------------------" )

    UT_PRINT( "--- bitsof() ---" )
    {
        UT_EQ( 8,  bitsof(char   ) )  UT_EQ( 8,   bitsofval(   char(1)) )
        UT_EQ( 8,  bitsof(int8_t ) )  UT_EQ( 8,   bitsofval( int8_t(1)) )
        UT_EQ(16,  bitsof(int16_t) )  UT_EQ(16,   bitsofval(int16_t(1)) )
        UT_EQ(32,  bitsof(int32_t) )  UT_EQ(32,   bitsofval(int32_t(1)) )
        UT_EQ(64,  bitsof(int64_t) )  UT_EQ(64,   bitsofval(int64_t(1)) )
    }

    UT_PRINT( "--- BitCount ---" )
    {
        std::pair<int, int> pairs[]= {
            {  0,  0}, {  1,  1}, {  2,  1}, {  3,  2}, {  4,  1}, {  5,  2}, {  6,  2}, {  7,  3},
            {  8,  1}, {255,  8}, {256,  1}, {257,  2},
        };

        // int 8
        {
            for( auto pair : pairs)
                if( pair.first <= 0xFF)
                {
                    UT_EQ(  pair.second, BitCount( uint8_t(pair.first) ) )
                }
            UT_EQ(  8, BitCount( uint8_t(255) )  )
            UT_EQ(  7, BitCount( uint8_t(254) )  )
        }

        // int 16
        {
            for( auto pair : pairs)
                if( pair.first <= 0xFFFF)
                {
                    UT_EQ(  pair.second, BitCount( uint16_t(pair.first) ) )
                }
            UT_EQ( 16, BitCount( uint16_t(0xFFFF) )  )
            UT_EQ( 15, BitCount( uint16_t(0xFFFE) )  )
        }

        // int 32
        {
            for( auto pair : pairs)
                if( pair.first <= 0xFFFF)
                {
                    UT_EQ(  pair.second, BitCount( uint32_t(pair.first) ) )
                }
            UT_EQ( 32, BitCount( uint32_t(0xFFFFFFFF) )  )
            UT_EQ( 31, BitCount( uint32_t(0xFFFFFFFE) )  )
        }

        // int 64
        {
            for( auto pair : pairs)
                if( pair.first <= 0xFFFF)
                {
                    UT_EQ(  pair.second, BitCount( uint64_t(pair.first) ) )
                }
            UT_EQ( 64, BitCount( uint64_t(0xFFFFFFFFFFFFFFFF) )  )
            UT_EQ( 63, BitCount( uint64_t(0xFFFFFFFFFFFFFFFE) )  )
        }
    }


    UT_PRINT( "--- LowerBits (includes LowerMask) ---" )
    {
        int  i;
        i= 0;           UT_EQ(  0, LowerBits( 0, i ) )
                        UT_EQ(  0, LowerBits( 1, i ) )
                        UT_EQ(  0, LowerBits( 2, i ) )
        i= 1;           UT_EQ(  0, LowerBits( 0, i ) )
                        UT_EQ(  1, LowerBits( 1, i ) )
                        UT_EQ(  1, LowerBits( 2, i ) )
        i= 2;           UT_EQ(  0, LowerBits( 0, i ) )
                        UT_EQ(  0, LowerBits( 1, i ) )
                        UT_EQ(  2, LowerBits( 2, i ) )
                        UT_EQ(  2, LowerBits( 3, i ) )
        i= 255;         UT_EQ(  0, LowerBits( 0, i ) )
                        UT_EQ(  1, LowerBits( 1, i ) )
                        UT_EQ(  3, LowerBits( 2, i ) )
                        UT_EQ(  7, LowerBits( 3, i ) )

    }
    {
        uint64_t  i;
        i= 0;           UT_EQ(  0UL, LowerBits( 0, i ) )
                        UT_EQ(  0UL, LowerBits( 1, i ) )
                        UT_EQ(  0UL, LowerBits( 2, i ) )
        i= 1;           UT_EQ(  0UL, LowerBits( 0, i ) )
                        UT_EQ(  1UL, LowerBits( 1, i ) )
                        UT_EQ(  1UL, LowerBits( 2, i ) )
        i= 2;           UT_EQ(  0UL, LowerBits( 0, i ) )
                        UT_EQ(  0UL, LowerBits( 1, i ) )
                        UT_EQ(  2UL, LowerBits( 2, i ) )
                        UT_EQ(  2UL, LowerBits( 3, i ) )
        i= 255;         UT_EQ(  0UL, LowerBits( 0, i ) )
                        UT_EQ(  1UL, LowerBits( 1, i ) )
                        UT_EQ(  3UL, LowerBits( 2, i ) )
                        UT_EQ(  7UL, LowerBits( 3, i ) )
        i= (std::numeric_limits<uint64_t>::max)();
                        UT_EQ(  0UL, LowerBits( 0, i ) )
                        UT_EQ(  1UL, LowerBits( 1, i ) )
                        UT_EQ(  3UL, LowerBits( 2, i ) )
                        UT_EQ(  7UL, LowerBits( 3, i ) )
                        UT_EQ(  (std::numeric_limits<uint32_t>::max)(),
                                   LowerBits(32, i ) )
    }


    UT_PRINT( "--------------- CLZ ---------------" )
    UT_EQ(32-  2 , CLZ( uint32_t(3)  ) )
    UT_EQ(32-  32, CLZ( uint32_t(0x80000000) ) )

    UT_EQ(8-0, CLZ0( uint8_t(0) ) )
    UT_EQ(8-1, CLZ ( uint8_t(1) ) )
    UT_EQ(8-8, CLZ ( uint8_t(~uint8_t(0))) )
    for( int i= 0 ; i < 8 ; ++i )
    {
        auto twoExpI= uint8_t( 1 << i);
        if( i > 1 )                     UT_EQ(8-  (i   ) , CLZ( uint8_t( twoExpI - 2)  ) )
        if( i > 0 )                     UT_EQ(8-  (i   ) , CLZ( uint8_t( twoExpI - 1)  ) )
                                        UT_EQ(8-  (i+ 1) , CLZ( uint8_t( twoExpI - 0)  ) )
        if( i > 0 && i < 7)             UT_EQ(8-  (i+ 1) , CLZ( uint8_t( twoExpI + 1)  ) )
        if( i > 1 && i < 7)             UT_EQ(8-  (i+ 1) , CLZ( uint8_t( twoExpI + 2)  ) )
    }

    UT_EQ(16-  0, CLZ0( uint16_t(0) ) )
    UT_EQ(16-  1, CLZ ( uint16_t(1) ) )
    UT_EQ(16- 16, CLZ ( uint16_t(~uint16_t(0))) )
    for( int i= 0 ; i < 16 ; ++i )
    {
        auto twoExpI= uint16_t( 1 << i);
        if( i > 1 )             UT_EQ(16-   (i   ) , CLZ(uint16_t(twoExpI - 2)  ) )
        if( i > 0 )             UT_EQ(16-   (i   ) , CLZ(uint16_t(twoExpI - 1)  ) )
                                UT_EQ(16-   (i+ 1) , CLZ(uint16_t(twoExpI - 0)  ) )
        if( i > 0 && i < 15)    UT_EQ(16-   (i+ 1) , CLZ(uint16_t(twoExpI + 1)  ) )
        if( i > 1 && i < 15)    UT_EQ(16-   (i+ 1) , CLZ(uint16_t(twoExpI + 2)  ) )
    }

    UT_EQ(32-  0, CLZ0( uint32_t(0) ) )
    UT_EQ(32-  1, CLZ ( uint32_t(1) ) )
    UT_EQ(32- 32, CLZ ( uint32_t(~uint32_t(0))) )
    for( int i= 0 ; i < 32 ; ++i )
    {
        auto twoExpI= uint32_t( 1 << i);
        if( i > 1 )             UT_EQ(32-  (i   ) , CLZ( uint32_t(twoExpI - 2)  ) )
        if( i > 0 )             UT_EQ(32-  (i   ) , CLZ( uint32_t(twoExpI - 1)  ) )
                                UT_EQ(32-  (i+ 1) , CLZ( uint32_t(twoExpI - 0)  ) )
        if( i > 0 && i < 31)    UT_EQ(32-  (i+ 1) , CLZ( uint32_t(twoExpI + 1)  ) )
        if( i > 1 && i < 31)    UT_EQ(32-  (i+ 1) , CLZ( uint32_t(twoExpI + 2)  ) )
    }

    UT_EQ(64-  0, CLZ0( uint64_t(0) ) )
    UT_EQ(64-  1, CLZ ( uint64_t(1) ) )
    UT_EQ(64- 64, CLZ ( uint64_t(~uint64_t(0))) )
    for( int i= 0 ; i < 64 ; ++i )
    {
        auto twoExpI= uint64_t( 1L )<< i;
        if( i > 1 )             UT_EQ(64-  (i   ) , CLZ( uint64_t(twoExpI - 2)  ) )
        if( i > 0 )             UT_EQ(64-  (i   ) , CLZ( uint64_t(twoExpI - 1)  ) )
                                UT_EQ(64-  (i+ 1) , CLZ( uint64_t(twoExpI - 0)  ) )
        if( i > 0 && i < 63)    UT_EQ(64-  (i+ 1) , CLZ( uint64_t(twoExpI + 1)  ) )
        if( i > 1 && i < 63)    UT_EQ(64-  (i+ 1) , CLZ( uint64_t(twoExpI + 2)  ) )
    }

    UT_PRINT( "--------------- CTZ ---------------" )
    UT_EQ( 0, CTZ( uint32_t(3)  ) )
    UT_EQ(31, CTZ( uint32_t(0x80000000) ) )

    UT_EQ(8, CTZ0( uint8_t(0) ) )
    UT_EQ(0, CTZ ( uint8_t(1) ) )
    UT_EQ(0, CTZ ( uint8_t(~uint8_t(0))) )
    for( int i= 0 ; i < 8 ; ++i )
    {
        auto val= uint8_t( 1 << i) + uint8_t( uint8_t(std::rand()) << (i + 1));
        UT_EQ(i, CTZ(val) )
    }

    UT_EQ(16, CTZ0( uint16_t(0) ) )
    UT_EQ( 0, CTZ ( uint16_t(1) ) )
    UT_EQ( 0, CTZ ( uint16_t(~uint16_t(0))) )
    for( int i= 0 ; i < 16 ; ++i )
    {
        auto val= uint16_t( 1 << i) + uint16_t( uint16_t(std::rand()) << (i + 1));
        UT_EQ(i, CTZ(val) )
    }

    UT_EQ(32, CTZ0( uint32_t(0) ) )
    UT_EQ( 0, CTZ ( uint32_t(1) ) )
    UT_EQ( 0, CTZ ( uint32_t(~uint32_t(0))) )
    for( int i= 0 ; i < 32 ; ++i )
    {
        auto val= uint32_t(uint32_t(1) << i) + (i==31 ? uint32_t(0) : uint32_t( uint32_t(std::rand()) << (i + 1)));
        UT_EQ(i, CTZ(val) )
    }

    UT_EQ(64, CTZ0( uint64_t(0) ) )
    UT_EQ( 0, CTZ ( uint64_t(1) ) )
    UT_EQ( 0, CTZ ( uint64_t(~uint64_t(0))) )
    for( int i= 0 ; i < 64 ; ++i )
    {
        auto val= uint64_t(uint64_t(1) << i) + (i==63 ? uint64_t(0) :  uint64_t( uint64_t(std::rand()) << (i + 1)));
        UT_EQ(i, CTZ(val) )
    }

    UT_PRINT( "--------------- MSB ---------------" )

    UT_EQ(  2 , MSB( uint32_t(3)  ) )
    UT_EQ(  32, MSB( uint32_t(0x80000000) ) )

    UT_EQ(0, MSB0( uint8_t(0) ) )
    UT_EQ(1, MSB ( uint8_t(1) ) )
    UT_EQ(8, MSB ( uint8_t(~uint8_t(0))) )
    for( int i= 0 ; i < 8 ; ++i )
    {
        auto twoExpI= uint8_t( 1 << i);
        if( i > 1 )                     UT_EQ(  i    , MSB( twoExpI - 2  ) )
        if( i > 0 )                     UT_EQ(  i    , MSB( twoExpI - 1  ) )
                                        UT_EQ(  i+ 1 , MSB( twoExpI - 0  ) )
        if( i > 0 && i < 7)             UT_EQ(  i+ 1 , MSB( twoExpI + 1  ) )
        if( i > 1 && i < 7)             UT_EQ(  i+ 1 , MSB( twoExpI + 2  ) )
    }

    UT_EQ( 0, MSB0( uint16_t(0) ) )
    UT_EQ( 1, MSB ( uint16_t(1) ) )
    UT_EQ(16, MSB ( uint16_t(~uint16_t(0))) )
    for( int i= 0 ; i < 16 ; ++i )
    {
        auto twoExpI= uint16_t( 1 << i);
        if( i > 1 )             UT_EQ(  i    , MSB( twoExpI - 2  ) )
        if( i > 0 )             UT_EQ(  i    , MSB( twoExpI - 1  ) )
                                UT_EQ(  i+ 1 , MSB( twoExpI - 0  ) )
        if( i > 0 && i < 15)    UT_EQ(  i+ 1 , MSB( twoExpI + 1  ) )
        if( i > 1 && i < 15)    UT_EQ(  i+ 1 , MSB( twoExpI + 2  ) )
    }

    UT_EQ( 0, MSB0( uint32_t(0) ) )
    UT_EQ( 1, MSB ( uint32_t(1) ) )
    UT_EQ(32, MSB ( uint32_t(~uint32_t(0))) )
    for( int i= 0 ; i < 32 ; ++i )
    {
        auto twoExpI= uint32_t( 1 << i);
        if( i > 1 )             UT_EQ(  i    , MSB( twoExpI - 2  ) )
        if( i > 0 )             UT_EQ(  i    , MSB( twoExpI - 1  ) )
                                UT_EQ(  i+ 1 , MSB( twoExpI - 0  ) )
        if( i > 0 && i < 31)    UT_EQ(  i+ 1 , MSB( twoExpI + 1  ) )
        if( i > 1 && i < 31)    UT_EQ(  i+ 1 , MSB( twoExpI + 2  ) )
    }

    UT_EQ( 0, MSB0( uint64_t(0) ) )
    UT_EQ( 1, MSB ( uint64_t(1) ) )
    UT_EQ(64, MSB ( uint64_t(~uint64_t(0))) )
    for( int i= 0 ; i < 64 ; ++i )
    {
        auto twoExpI= uint64_t( 1L )<< i;
        if( i > 1 )             UT_EQ(  i    , MSB( twoExpI - 2  ) )
        if( i > 0 )             UT_EQ(  i    , MSB( twoExpI - 1  ) )
                                UT_EQ(  i+ 1 , MSB( twoExpI - 0  ) )
        if( i > 0 && i < 63)    UT_EQ(  i+ 1 , MSB( twoExpI + 1  ) )
        if( i > 1 && i < 63)    UT_EQ(  i+ 1 , MSB( twoExpI + 2  ) )
    }
} // UT_METHOD(Bits)

//--------------------------------------------------------------------------------------------------
//--- BitSet
//--------------------------------------------------------------------------------------------------
UT_METHOD(ClassBitSet)
{
    UT_INIT()

    // ------------- Test instance size and number of words ----------
    UT_PRINT("Sizeof std::bitset/BitSet of size   1: {:>2} / {:>2}.  QtyWords= {}", sizeof(std::bitset<   1>), sizeof(BitSet<   1>), BitSet<   1>::QtyWords )
    UT_PRINT("Sizeof std::bitset/BitSet of size  63: {:>2} / {:>2}.  QtyWords= {}", sizeof(std::bitset<  63>), sizeof(BitSet<  63>), BitSet<  63>::QtyWords )
    UT_PRINT("Sizeof std::bitset/BitSet of size  64: {:>2} / {:>2}.  QtyWords= {}", sizeof(std::bitset<  64>), sizeof(BitSet<  64>), BitSet<  64>::QtyWords )
    UT_PRINT("Sizeof std::bitset/BitSet of size  65: {:>2} / {:>2}.  QtyWords= {}", sizeof(std::bitset<  65>), sizeof(BitSet<  65>), BitSet<  65>::QtyWords )
    UT_PRINT("Sizeof std::bitset/BitSet of size 127: {:>2} / {:>2}.  QtyWords= {}", sizeof(std::bitset< 127>), sizeof(BitSet< 127>), BitSet< 127>::QtyWords )
    UT_PRINT("Sizeof std::bitset/BitSet of size 128: {:>2} / {:>2}.  QtyWords= {}", sizeof(std::bitset< 128>), sizeof(BitSet< 128>), BitSet< 128>::QtyWords )
    UT_PRINT("Sizeof std::bitset/BitSet of size 129: {:>2} / {:>2}.  QtyWords= {}", sizeof(std::bitset< 129>), sizeof(BitSet< 129>), BitSet< 129>::QtyWords )

    UT_EQ(size_t( 1) , sizeof(BitSet<  0>))  UT_EQ( 1, BitSet<  1>::QtyWords )   UT_EQ( 0, BitSet<0>::Capacity  )
    UT_EQ(size_t( 1) , sizeof(BitSet<  1>))  UT_EQ( 1, BitSet<  1>::QtyWords )   UT_EQ( 1, BitSet<1>::Capacity  )
    UT_EQ(size_t( 1) , sizeof(BitSet<  8>))  UT_EQ( 1, BitSet<  8>::QtyWords )   UT_EQ( 8, BitSet<8>::Capacity  )
    UT_EQ(size_t( 2) , sizeof(BitSet<  9>))  UT_EQ( 1, BitSet<  9>::QtyWords )   UT_EQ( 9, BitSet<9>::Capacity  )
    UT_EQ(size_t( 2) , sizeof(BitSet< 16>))  UT_EQ( 1, BitSet< 16>::QtyWords )
    UT_EQ(size_t( 4) , sizeof(BitSet< 17>))  UT_EQ( 1, BitSet< 17>::QtyWords )
    UT_EQ(size_t( 4) , sizeof(BitSet< 24>))  UT_EQ( 1, BitSet< 24>::QtyWords )
    UT_EQ(size_t( 4) , sizeof(BitSet< 31>))  UT_EQ( 1, BitSet< 31>::QtyWords )
    UT_EQ(size_t( 4) , sizeof(BitSet< 32>))  UT_EQ( 1, BitSet< 32>::QtyWords )

    UT_EQ(size_t( 8) , sizeof(BitSet< 33>))  UT_EQ( 1, BitSet< 33>::QtyWords )
    UT_EQ(size_t( 8) , sizeof(BitSet< 63>))  UT_EQ( 1, BitSet< 63>::QtyWords )
    UT_EQ(size_t( 8) , sizeof(BitSet< 64>))  UT_EQ( 1, BitSet< 64>::QtyWords )
    UT_EQ(size_t(16) , sizeof(BitSet< 65>))  UT_EQ( 2, BitSet< 65>::QtyWords )
    UT_EQ(size_t(16) , sizeof(BitSet< 66>))  UT_EQ( 2, BitSet< 66>::QtyWords )
    UT_EQ(size_t(16) , sizeof(BitSet< 95>))  UT_EQ( 2, BitSet< 95>::QtyWords )
    UT_EQ(size_t(16) , sizeof(BitSet< 96>))  UT_EQ( 2, BitSet< 96>::QtyWords )
    UT_EQ(size_t(16) , sizeof(BitSet< 97>))  UT_EQ( 2, BitSet< 97>::QtyWords )
    UT_EQ(size_t(16) , sizeof(BitSet< 98>))  UT_EQ( 2, BitSet< 98>::QtyWords )
    UT_EQ(size_t(16) , sizeof(BitSet<127>))  UT_EQ( 2, BitSet<127>::QtyWords )
    UT_EQ(size_t(16) , sizeof(BitSet<128>))  UT_EQ( 2, BitSet<128>::QtyWords )
    UT_EQ(size_t(24) , sizeof(BitSet<129>))  UT_EQ( 3, BitSet<129>::QtyWords )
    UT_EQ(size_t(24) , sizeof(BitSet<130>))  UT_EQ( 3, BitSet<130>::QtyWords )

    // ------------- construction/copy/move with external preset  ----------
    {
        BitSet<7> bitSet;                           UT_EQ( 0, bitSet.Count() )
        bitSet= BitSet<7>( BitSet<7>::TWord(0x11)); UT_EQ( 2, bitSet.Count() )
        auto bsCopy= bitSet;                        UT_TRUE( bitSet == bsCopy )
        BitSet<7> bsCopy2(bitSet);                  UT_TRUE( bitSet == bsCopy2 )
        BitSet<7> bsCopy3(std::move(bitSet));       UT_TRUE( bitSet == bsCopy3 )
    }

    // ------------- Test a list of bits  ----------
    {
        BitSet<13> bitSet;              UT_EQ( 0, bitSet.Count() )
        bitSet.Set(5);                  UT_EQ( 1, bitSet.Count() )
        bitSet.Set(5, 7);               UT_EQ( 2, bitSet.Count() )
        bitSet.Set(1,5,7,8, 9, 12);     UT_EQ( 6, bitSet.Count() )
        bitSet.Reset(5);                UT_EQ( 5, bitSet.Count() )
        bitSet.Reset(1,5,7);            UT_EQ( 3, bitSet.Count() )
        bitSet.Flip(8, 9, 12);          UT_EQ( 0, bitSet.Count() )

        bitSet.Flip(  10);              UT_EQ( 1024, bitSet.Export() )
    }

    // ------------- test all methods on different sizes ----------
    testBitSets<1,2,3, 7,8,9, 15,16,17, 25, 31,32,33, 50,  63,64,65, 75, 127,128,129,
                150, 190, 191, 192, 193, 194, 200, 250>(ut);

    // ------------- import export ----------
    #define UT_ASSERT_TSIZE(V) static_assert(sizeof(ATMP_RR(decltype(ex))) == V, "Error");
    {
        BitSet<3> bitSet;

        auto& ex= bitSet.Export();
        //    ex= bitSet.Export(0); // method with parameter is not available!
        UT_ASSERT_TSIZE(1)  UT_EQ(0, ex)
        bitSet.Set(0);      UT_EQ(1, ex)
        bitSet.Set(1);      UT_EQ(3, ex)
    }
    {
        BitSet<8> bitSet;
        auto& ex= bitSet.Export();
        //    ex= bitSet.Export(0); // method with parameter is not available!
        UT_ASSERT_TSIZE(1)  UT_EQ(0, ex)
        bitSet.Set(0);      UT_EQ(1, ex)
        bitSet.Set(1);      UT_EQ(3, ex)
    }
    {
        BitSet<9> bitSet;
        auto& ex= bitSet.Export();
        //    ex= bitSet.Export(0); // method with parameter is not available!
        UT_ASSERT_TSIZE(2)  UT_EQ(0, ex)
        bitSet.Set(0);      UT_EQ(1, ex)
        bitSet.Set(1);      UT_EQ(3, ex)
    }
    {
        BitSet<17> bitSet;
        auto& ex= bitSet.Export();
        //    ex= bitSet.Export(0); // method with parameter is not available!
        UT_ASSERT_TSIZE(4)  UT_EQ(uint32_t(      0), ex)
        bitSet.Set(0);      UT_EQ(uint32_t(      1), ex)
        bitSet.Set(16);     UT_EQ(uint32_t(0x10001), ex)
    }
    {
        BitSet<33> bitSet;
        auto& ex= bitSet.Export();
        //    ex= bitSet.Export(0); // method with parameter is not available!
        UT_ASSERT_TSIZE(8)  UT_EQ(uint64_t(                  0), ex)
        bitSet.Set(0);      UT_EQ(uint64_t(                  1), ex)
        bitSet.Set(16);     UT_EQ(uint64_t(            0x10001), ex)
        bitSet.Set(32);     UT_EQ(uint64_t(        0x100010001), ex)
    }
    {
        BitSet<64> bitSet;
        auto& ex= bitSet.Export();
        //    ex= bitSet.Export(0); // method with parameter is not available!
        UT_ASSERT_TSIZE(8)  UT_EQ(uint64_t(                  0), ex)
        bitSet.Set(0);      UT_EQ(uint64_t(                  1), ex)
        bitSet.Set(16);     UT_EQ(uint64_t(            0x10001), ex)
        bitSet.Set(32);     UT_EQ(uint64_t(        0x100010001), ex)
        bitSet.Set(63);     UT_EQ(uint64_t(0x8000000100010001), ex)
    }
    {
        BitSet<65> bitSet;
        // auto& ex= bitSet.Export();
        auto& ex= bitSet.Export(0); // method without parameter is not available!
        UT_ASSERT_TSIZE(8)  UT_EQ(uint64_t(                 0), ex)
        bitSet.Set(0);      UT_EQ(uint64_t(                 1), ex)
        bitSet.Set(16);     UT_EQ(uint64_t(           0x10001), ex)
        bitSet.Set(32);     UT_EQ(uint64_t(       0x100010001), ex)
        bitSet.Set(63);     UT_EQ(uint64_t(0x8000000100010001), ex)
        bitSet.Set(64);     UT_EQ(uint64_t(0x8000000100010001), ex)

        auto& ex2   = bitSet.Export(1);
                            UT_EQ(uint64_t(                 1), ex2)
    }

    // ------------- Shifting << / >> ----------
    // simple <<
    {
        BitSet<5> bitSet;
        bitSet.Set(0); UT_EQ(  1, (bitSet << 0).Export() ) UT_EQ(  2, (bitSet << 1).Export() )  UT_EQ(  4, (bitSet << 2).Export() )  UT_EQ(     8, (bitSet << 3).Export() )
        bitSet.Set(1); UT_EQ(  3, (bitSet << 0).Export() ) UT_EQ(  6, (bitSet << 1).Export() )  UT_EQ( 12, (bitSet << 2).Export() )  UT_EQ(    24, (bitSet << 3).Export() )
        bitSet.Set(2); UT_EQ(  7, (bitSet << 0).Export() ) UT_EQ( 14, (bitSet << 1).Export() )  UT_EQ( 28, (bitSet << 2).Export() )  UT_EQ( 56%32, (bitSet << 3).Export() )
    }

    // larger <<
    {
        BitSet<248> bitSet;
        bitSet.Import(0x1122334455667788, 0);
        bitSet= bitSet << 63;  UT_EQ(0x0ull, bitSet.Export(0) )                  UT_EQ( 0x1122334455667788ull>>1, bitSet.Export(1) )UT_EQ( 0x0ull, bitSet.Export(2) )

        bitSet.Reset().Import(0x1122334455667788, 0);
        bitSet= bitSet << 64;  UT_EQ(                0x0ull, bitSet.Export(0) )  UT_EQ( 0x1122334455667788ull, bitSet.Export(1) )  UT_EQ( 0x0ull, bitSet.Export(2) )

        bitSet.Reset().Import(0x1122334455667788, 0);
        bitSet= bitSet <<  4;  UT_EQ( 0x1223344556677880ull, bitSet.Export(0) )  UT_EQ(             0x1ull, bitSet.Export(1) )
        bitSet= bitSet << 60;  UT_EQ(                0x0ull, bitSet.Export(0) )  UT_EQ( 0x1122334455667788ull, bitSet.Export(1) )  UT_EQ( 0x0ull, bitSet.Export(2) )

        bitSet.Reset().Import(0x1122334455667788, 0);
        bitSet= bitSet << 8;   UT_EQ(                0x0ull, bitSet.Export(3) )
                               UT_EQ(                0x0ull, bitSet.Export(2) )
                               UT_EQ(               0x11ull, bitSet.Export(1) )
                               UT_EQ( 0x2233445566778800ull, bitSet.Export(0) )
        bitSet.Reset().Import(0x1122334455667788, 0);
        bitSet= bitSet <<16;   UT_EQ(                0x0ull, bitSet.Export(3) )
                               UT_EQ(                0x0ull, bitSet.Export(2) )
                               UT_EQ(             0x1122ull, bitSet.Export(1) )
                               UT_EQ( 0x3344556677880000ull, bitSet.Export(0) )
        bitSet.Reset().Import(0x1122334455667788, 0);
        bitSet= bitSet <<132;  UT_EQ(                0x1ull, bitSet.Export(3) )
                               UT_EQ( 0x1223344556677880ull, bitSet.Export(2) )
                               UT_EQ(                0x0ull, bitSet.Export(1) )
                               UT_EQ(                0x0ull, bitSet.Export(0) )
        bitSet.Reset().Import(0x1122334455667788, 0);
        bitSet= bitSet <<136;  UT_EQ(               0x11ull, bitSet.Export(3) )
                               UT_EQ( 0x2233445566778800ull, bitSet.Export(2) )
                               UT_EQ(                0x0ull, bitSet.Export(1) )
                               UT_EQ(                0x0ull, bitSet.Export(0) )
        bitSet.Reset().Import(0x1122334455667788, 0);
        bitSet= bitSet <<192;  UT_EQ(   0x22334455667788ull, bitSet.Export(3) )
                               UT_EQ(                0x0ull, bitSet.Export(2) )
                               UT_EQ(                0x0ull, bitSet.Export(1) )
                               UT_EQ(                0x0ull, bitSet.Export(0) )
    }

    // larger <<=  (inplace)
    {
        BitSet<248> bitSet;
        bitSet.Import(0x1122334455667788, 0);
        bitSet<<=  4;  UT_EQ( 0x1223344556677880ull, bitSet.Export(0) )  UT_EQ(                0x1ull, bitSet.Export(1) )
        bitSet<<= 60;  UT_EQ(                0x0ull, bitSet.Export(0) )  UT_EQ( 0x1122334455667788ull, bitSet.Export(1) )  UT_EQ( 0x0ull, bitSet.Export(2) )

        bitSet.Reset().Import(0x1122334455667788, 0);
        bitSet<<=  8;   UT_EQ(                0x0ull, bitSet.Export(3) )
                        UT_EQ(                0x0ull, bitSet.Export(2) )
                        UT_EQ(               0x11ull, bitSet.Export(1) )
                        UT_EQ( 0x2233445566778800ull, bitSet.Export(0) )
        bitSet.Reset().Import(0x1122334455667788, 0);
        bitSet<<= 16;   UT_EQ(                0x0ull, bitSet.Export(3) )
                        UT_EQ(                0x0ull, bitSet.Export(2) )
                        UT_EQ(             0x1122ull, bitSet.Export(1) )
                        UT_EQ( 0x3344556677880000ull, bitSet.Export(0) )
        bitSet.Reset().Import(0x1122334455667788, 0);
        bitSet<<= 132;  UT_EQ(                0x1ull, bitSet.Export(3) )
                        UT_EQ( 0x1223344556677880ull, bitSet.Export(2) )
                        UT_EQ(                0x0ull, bitSet.Export(1) )
                        UT_EQ(                0x0ull, bitSet.Export(0) )
        bitSet.Reset().Import(0x1122334455667788, 0);
        bitSet<<= 136;  UT_EQ(               0x11ull, bitSet.Export(3) )
                        UT_EQ( 0x2233445566778800ull, bitSet.Export(2) )
                        UT_EQ(                0x0ull, bitSet.Export(1) )
                        UT_EQ(                0x0ull, bitSet.Export(0) )
        bitSet.Reset().Import(0x1122334455667788, 0);
        bitSet<<= 192;  UT_EQ(   0x22334455667788ull, bitSet.Export(3) )
                        UT_EQ(                0x0ull, bitSet.Export(2) )
                        UT_EQ(                0x0ull, bitSet.Export(1) )
                        UT_EQ(                0x0ull, bitSet.Export(0) )
    }

    // larger << (std::rand values, compared with std::bitset)
    {
        using TWord= BitSet<200>::TWord;
        constexpr int sow= bitsof(TWord);  static_assert( sow == bitsof(unsigned long long),
                                                         "Wrong assumption" );
        for (int testNo = 0; testNo < 1000; ++testNo)
        {
            std::bitset<200> sbs;
                 BitSet<200> abs;

            // fill same std::rand to both bitsets
            for (int j= BitSet<200>::QtyWords-1 ; j >=0 ; --j)
            {
                auto r= testGetRandom64();
                sbs<<= sow;
                sbs|=  r;
                abs.Import(r, j);
            }
            testCompare(ut, abs, sbs);

            // do some shift ops
            int shiftSum=0;
            while(shiftSum < 200)
            {
                int shift= std::rand() % 200;
                if( shift + shiftSum > 200)
                    shift= 200 - shiftSum;

                sbs<<= size_t(shift);
                abs<<=        shift;

                testCompare(ut, abs, sbs);
                shiftSum+= shift;
            }
        }
    }

    // simple >>
    {
        BitSet<5> bitSet;
        bitSet.Set(4); UT_EQ( 16, (bitSet >> 0).Export() )
                       UT_EQ(  8, (bitSet >> 1).Export() )
                       UT_EQ(  4, (bitSet >> 2).Export() )
                       UT_EQ(  2, (bitSet >> 3).Export() )
                       UT_EQ(  1, (bitSet >> 4).Export() )
                       UT_EQ(  0, (bitSet >> 5).Export() )
    }

    // larger >>=  (inplace)
    {
        BitSet<248> bitSet;
        bitSet.Import(0x1122334455667788, 0);
        bitSet>>=  4;  UT_EQ( 0x112233445566778ull, bitSet.Export(0) )
        bitSet>>=  4;  UT_EQ(  0x11223344556677ull, bitSet.Export(0) )
        bitSet>>= 60;  UT_EQ(                0x0ull, bitSet.Export(0) )

        bitSet.Reset().Import(0x1122334455667788ull, 1);
        bitSet>>=  8;   UT_EQ(0x8800000000000000ull, bitSet.Export(0) )
                        UT_EQ(  0x11223344556677ull, bitSet.Export(1) )
                        UT_EQ(               0x0ull, bitSet.Export(2) )

        bitSet.Reset().Import(0x1122334455667788ull, 2);
        bitSet>>=  8;   UT_EQ(               0x0ull, bitSet.Export(0) )
                        UT_EQ(0x8800000000000000ull, bitSet.Export(1) )
                        UT_EQ(  0x11223344556677ull, bitSet.Export(2) )
                        UT_EQ(               0x0ull, bitSet.Export(3) )
    }

    // larger >> (std::rand values, compared with std::bitset)
    {
        using TWord= BitSet<200>::TWord;
        constexpr int sow= bitsof(TWord);  static_assert( sow == bitsof(unsigned long long),
                                                         "Wrong assumption" );
        for (int testNo = 0; testNo < 1000; ++testNo)
        {
            std::bitset<200> sbs;
                 BitSet<200> abs;

            // fill same std::rand to both bitsets
            for (int j= BitSet<200>::QtyWords-1 ; j >=0 ; --j)
            {
                auto r= testGetRandom64();
                sbs<<= sow;
                sbs|=  r;
                abs.Import(r, j);
            }
            testCompare(ut, abs, sbs);

            // do some shift ops
            int shiftSum=0;
            while(shiftSum < 200)
            {
                int shift= std::rand() % 200;
                if( shift + shiftSum > 200)
                    shift= 200 - shiftSum;

                sbs>>= size_t(shift);
                abs>>=        shift;

                testCompare(ut, abs, sbs);
                shiftSum+= shift;
            }
        }
    }

    // ------------- Test Reference  ----------
    {
        bool actVal;
        BitSet<13> bitSet;
        auto bsr= BitSet<13>::Reference<BitSet<13>>(bitSet, 1);
        UT_EQ( false, actVal= bsr ) // implicit bool operator
        bsr.Flip();             UT_EQ( 2, bitSet.Export() )
        UT_EQ( false, ~bsr )    UT_EQ( 2, bitSet.Export() ) // does not modify the bit
        bsr= false;             UT_EQ( 0, bitSet.Export() )

        bsr= bitSet[2];         UT_EQ( false, actVal= bsr )
        bsr.Flip();             UT_EQ( 4, bitSet.Export() )
    }

    // ------------- Test iteration  ----------
    {
        {int bits[] = {0};                  testBitSetIteration< 8>(ut, bits );}
        {int bits[] = {3,4,7};              testBitSetIteration< 8>(ut, bits );}
        {int bits[] = {63};                 testBitSetIteration<96>(ut, bits );}
        {int bits[] = {64};                 testBitSetIteration<96>(ut, bits );}
        {int bits[] = {65};                 testBitSetIteration<96>(ut, bits );}
        {int bits[] = {63,64,65};           testBitSetIteration<96>(ut, bits );}
        {int bits[] = {0,63,64,65,95};      testBitSetIteration<96>(ut, bits );}

        {
            BitSet<200> bitSet;
            bitSet.Set(0, 20, 40, 60, 80, 100,  120, 140, 160, 180, 199);
            UT_EQ(  0, (*bitSet. begin(   )).Bit() )
            UT_EQ( 20, (*bitSet. begin(  1)).Bit() )
            UT_EQ( 20, (*bitSet. begin(  2)).Bit() )
            UT_EQ( 20, (*bitSet. begin( 10)).Bit() )
            UT_EQ( 20, (*bitSet. begin( 20)).Bit() )
            UT_EQ( 40, (*bitSet. begin( 21)).Bit() )
            UT_EQ(140, (*bitSet. begin(140)).Bit() )
            UT_EQ(160, (*bitSet. begin(141)).Bit() )
            UT_EQ(199, (*bitSet.rbegin(   )).Bit() )
            UT_EQ(180, (*bitSet.rbegin(  1)).Bit() )
            UT_EQ(180, (*bitSet.rbegin(  2)).Bit() )
            UT_EQ(180, (*bitSet.rbegin( 10)).Bit() )
            UT_EQ(160, (*bitSet.rbegin( 20)).Bit() )
            UT_EQ(160, (*bitSet.rbegin( 21)).Bit() )
            UT_EQ( 60, (*bitSet.rbegin(139)).Bit() )
            UT_EQ( 40, (*bitSet.rbegin(140)).Bit() )
            UT_EQ( 40, (*bitSet.rbegin(141)).Bit() )
        }
    }
} // UT_METHOD(BitSet)

#include "unittests/aworx_unittests_end.hpp"

} //namespace [ut_aworx]

#endif // ALIB_UT_BITS
