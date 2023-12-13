// #################################################################################################
//  AWorx ALib Unit Tests
//
//  Copyright 2013-2023 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"
#include "unittests/alib_test_selection.hpp"
#if ALIB_UT_TIME

#if !defined(HPP_ALIB_BITS)
#   include "alib/lib/bits.hpp"
#endif

#include "alib/alox.hpp"


#define TESTCLASSNAME       CPP_ALib_Lib
#include "unittests/aworx_unittests.hpp"


using namespace std;
using namespace aworx;

namespace ut_aworx {

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

    UT_PRINT( "--- BitCount ---" )
    {
        int  i;
        i= 0;           UT_EQ(  0, lib::BitCount( i ) )
        i= 1;           UT_EQ(  1, lib::BitCount( i ) )
        i= 2;           UT_EQ(  1, lib::BitCount( i ) )
        i= 3;           UT_EQ(  2, lib::BitCount( i ) )
        i= 4;           UT_EQ(  1, lib::BitCount( i ) )
        i= 5;           UT_EQ(  2, lib::BitCount( i ) )
        i= 6;           UT_EQ(  2, lib::BitCount( i ) )
        i= 7;           UT_EQ(  3, lib::BitCount( i ) )
        i= 8;           UT_EQ(  1, lib::BitCount( i ) )
        i= 255;         UT_EQ(  8, lib::BitCount( i ) )
        i= 256;         UT_EQ(  1, lib::BitCount( i ) )
        i= 257;         UT_EQ(  2, lib::BitCount( i ) )
    }
    {
        uint64_t  i;
        i= 0;                                      UT_EQ(  0, lib::BitCount( i ) )
        i= 1;                                      UT_EQ(  1, lib::BitCount( i ) )
        i= 2;                                      UT_EQ(  1, lib::BitCount( i ) )
        i= 3;                                      UT_EQ(  2, lib::BitCount( i ) )
        i= 4;                                      UT_EQ(  1, lib::BitCount( i ) )
        i= 5;                                      UT_EQ(  2, lib::BitCount( i ) )
        i= 6;                                      UT_EQ(  2, lib::BitCount( i ) )
        i= 7;                                      UT_EQ(  3, lib::BitCount( i ) )
        i= 8;                                      UT_EQ(  1, lib::BitCount( i ) )
        i= 255;                                    UT_EQ(  8, lib::BitCount( i ) )
        i= 256;                                    UT_EQ(  1, lib::BitCount( i ) )
        i= 257;                                    UT_EQ(  2, lib::BitCount( i ) )
        i= (std::numeric_limits<uint64_t>::max)(); UT_EQ( 64, lib::BitCount( i ) )
    }

    UT_PRINT( "--- LowerBits (includes LowerMask) ---" )
    {
        int  i;
        i= 0;           UT_EQ(  0, lib::LowerBits( 0, i ) )
                        UT_EQ(  0, lib::LowerBits( 1, i ) )
                        UT_EQ(  0, lib::LowerBits( 2, i ) )
        i= 1;           UT_EQ(  0, lib::LowerBits( 0, i ) )
                        UT_EQ(  1, lib::LowerBits( 1, i ) )
                        UT_EQ(  1, lib::LowerBits( 2, i ) )
        i= 2;           UT_EQ(  0, lib::LowerBits( 0, i ) )
                        UT_EQ(  0, lib::LowerBits( 1, i ) )
                        UT_EQ(  2, lib::LowerBits( 2, i ) )
                        UT_EQ(  2, lib::LowerBits( 3, i ) )
        i= 255;         UT_EQ(  0, lib::LowerBits( 0, i ) )
                        UT_EQ(  1, lib::LowerBits( 1, i ) )
                        UT_EQ(  3, lib::LowerBits( 2, i ) )
                        UT_EQ(  7, lib::LowerBits( 3, i ) )

    }
    {
        uint64_t  i;
        i= 0;           UT_EQ(  0UL, lib::LowerBits( 0, i ) )
                        UT_EQ(  0UL, lib::LowerBits( 1, i ) )
                        UT_EQ(  0UL, lib::LowerBits( 2, i ) )
        i= 1;           UT_EQ(  0UL, lib::LowerBits( 0, i ) )
                        UT_EQ(  1UL, lib::LowerBits( 1, i ) )
                        UT_EQ(  1UL, lib::LowerBits( 2, i ) )
        i= 2;           UT_EQ(  0UL, lib::LowerBits( 0, i ) )
                        UT_EQ(  0UL, lib::LowerBits( 1, i ) )
                        UT_EQ(  2UL, lib::LowerBits( 2, i ) )
                        UT_EQ(  2UL, lib::LowerBits( 3, i ) )
        i= 255;         UT_EQ(  0UL, lib::LowerBits( 0, i ) )
                        UT_EQ(  1UL, lib::LowerBits( 1, i ) )
                        UT_EQ(  3UL, lib::LowerBits( 2, i ) )
                        UT_EQ(  7UL, lib::LowerBits( 3, i ) )
        i= (std::numeric_limits<uint64_t>::max)();
                        UT_EQ(  0UL, lib::LowerBits( 0, i ) )
                        UT_EQ(  1UL, lib::LowerBits( 1, i ) )
                        UT_EQ(  3UL, lib::LowerBits( 2, i ) )
                        UT_EQ(  7UL, lib::LowerBits( 3, i ) )
                        UT_EQ(  (std::numeric_limits<uint32_t>::max)(),
                                   lib::LowerBits(32, i ) )
                        UT_EQ(  i, lib::LowerBits(64, i ) )
    }


    UT_PRINT( "--- MSB ---" )

    UT_EQ(  2 , lib::MSB( 3  ) )
    UT_EQ(  32, lib::MSB( uint32_t(0x80000000) ) )

    for( int i= 0 ; i < 8 ; ++i )
    {
        auto twoExpI= uint8_t( 1 << i);
        if( i > 1 )                     UT_EQ(  i    , lib::MSB( twoExpI - 2  ) )
        if( i > 0 )                     UT_EQ(  i    , lib::MSB( twoExpI - 1  ) )
                                        UT_EQ(  i+ 1     , lib::MSB( twoExpI - 0  ) )
        if( i > 0 && i < 7)             UT_EQ(  i+ 1 , lib::MSB( twoExpI + 1  ) )
        if( i > 1 && i < 7)             UT_EQ(  i+ 1 , lib::MSB( twoExpI + 2  ) )
    }

    for( int i= 0 ; i < 16 ; ++i )
    {
        auto twoExpI= uint16_t( 1 << i);
        if( i > 1 )             UT_EQ(  i    , lib::MSB( twoExpI - 2  ) )
        if( i > 0 )             UT_EQ(  i    , lib::MSB( twoExpI - 1  ) )
                                UT_EQ(  i+ 1     , lib::MSB( twoExpI - 0  ) )
        if( i > 0 && i < 15)    UT_EQ(  i+ 1 , lib::MSB( twoExpI + 1  ) )
        if( i > 1 && i < 15)    UT_EQ(  i+ 1 , lib::MSB( twoExpI + 2  ) )
    }

    for( int i= 0 ; i < 32 ; ++i )
    {
        auto twoExpI= uint32_t( 1 << i);
        if( i > 1 )             UT_EQ(  i    , lib::MSB( twoExpI - 2  ) )
        if( i > 0 )             UT_EQ(  i    , lib::MSB( twoExpI - 1  ) )
                                UT_EQ(  i+ 1     , lib::MSB( twoExpI - 0  ) )
        if( i > 0 && i < 31)    UT_EQ(  i+ 1 , lib::MSB( twoExpI + 1  ) )
        if( i > 1 && i < 31)    UT_EQ(  i+ 1 , lib::MSB( twoExpI + 2  ) )
    }

    for( int i= 0 ; i < 64 ; ++i )
    {
        auto twoExpI= uint64_t( 1L )<< i;
        if( i > 1 )             UT_EQ(  i    , lib::MSB( twoExpI - 2  ) )
        if( i > 0 )             UT_EQ(  i    , lib::MSB( twoExpI - 1  ) )
                                UT_EQ(  i+ 1     , lib::MSB( twoExpI - 0  ) )
        if( i > 0 && i < 63)    UT_EQ(  i+ 1 , lib::MSB( twoExpI + 1  ) )
        if( i > 1 && i < 63)    UT_EQ(  i+ 1 , lib::MSB( twoExpI + 2  ) )
    }
}


#include "unittests/aworx_unittests_end.hpp"

} //namespace [ut_aworx]

#endif // ALIB_UT_TIME
