// #################################################################################################
//  AWorx ALib Unit Tests
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib_precompile.hpp"
#include "alib_test_selection.hpp"
#if ALIB_UT_BITBUFFER

#include "ALib.BitBuffer.H"

#define TESTCLASSNAME       UT_BitBuffer
#include "aworx_unittests.hpp"

using namespace alib;
using namespace alib::lang;


namespace ut_aworx {
UT_CLASS

UT_METHOD(BitBuffer)
{
    UT_INIT()
    UT_PRINT( "" )
    UT_PRINT( "--------------------------- UT_BitBuffer() ---------------------------" )



    constexpr uinteger TCapacity= 10000 * 32;
    BitBufferLocal<TCapacity> bb;

    UT_PRINT( "0. Simple stuff to debug gdb pretty printers:" )
    {
        BitWriter bw(bb);
        for (int i = 0; i < 10; ++i)
            bw.Write<1>(0);
        bw.Flush();

        BitReader br(bb);
        for (int i = 0; i < 10; ++i)
            br.Read<1>();
    }

    UT_PRINT( "1. Length methods:" )
    {
        BitWriter bw(bb );
        auto idxZer= bw.GetIndex();
                            UT_TRUE (idxZer == bw.GetIndex() )
                            UT_TRUE (idxZer >= bw.GetIndex() )
                            UT_TRUE (idxZer <= bw.GetIndex() )
                            UT_FALSE(idxZer != bw.GetIndex() )
                            UT_FALSE(idxZer < bw.GetIndex() )
                            UT_FALSE(idxZer > bw.GetIndex() )
        auto             size   = bw.RemainingSize();
        UT_EQ( TCapacity, size   )

                            UT_EQ(0u                    , bw.Usage()          )
                            UT_EQ(size                  , bw.RemainingSize()  )
                            UT_EQ(bw.GetIndex(), BitBuffer::Index::Decode32(bw.GetIndex().Encode32()) )
                            UT_EQ(bw.GetIndex(), BitBuffer::Index::Decode64(bw.GetIndex().Encode64()) )
        bw.Write(true );    UT_EQ(1u                    , bw.Usage()          )
                            UT_EQ(  size- 1                 , bw.RemainingSize()  )
                            UT_EQ(bw.GetIndex(), BitBuffer::Index::Decode32(bw.GetIndex().Encode32()) )
                            UT_EQ(bw.GetIndex(), BitBuffer::Index::Decode64(bw.GetIndex().Encode64()) )

                            UT_FALSE(idxZer == bw.GetIndex() )
                            UT_FALSE(idxZer >= bw.GetIndex() )
                            UT_TRUE (idxZer <= bw.GetIndex() )
                            UT_TRUE (idxZer != bw.GetIndex() )
                            UT_TRUE (idxZer <  bw.GetIndex() )
                            UT_FALSE(idxZer >  bw.GetIndex() )
        bw.Write(true );   UT_EQ(2u                    , bw.Usage()          )
                            UT_EQ(  size- 2                 , bw.RemainingSize()  )
                            UT_EQ(bw.GetIndex(), BitBuffer::Index::Decode32(bw.GetIndex().Encode32()) )
                            UT_EQ(bw.GetIndex(), BitBuffer::Index::Decode64(bw.GetIndex().Encode64()) )

        bw.Write<5>(0 );   UT_EQ(7u                    , bw.Usage()          )
        UT_EQ(  size- 7                 , bw.RemainingSize()  )
                            UT_EQ(bw.GetIndex(), BitBuffer::Index::Decode32(bw.GetIndex().Encode32()) )
                            UT_EQ(bw.GetIndex(), BitBuffer::Index::Decode64(bw.GetIndex().Encode64()) )

        bw.Write<1>(0 );   UT_EQ(8u                    , bw.Usage()          )
        UT_EQ(  size- 8                 , bw.RemainingSize()  )
                            UT_EQ(bw.GetIndex(), BitBuffer::Index::Decode32(bw.GetIndex().Encode32()) )
                            UT_EQ(bw.GetIndex(), BitBuffer::Index::Decode64(bw.GetIndex().Encode64()) )

        bw.Write<1>(0 );   UT_EQ(9u                    , bw.Usage()          )
                            UT_EQ(  size- 9                 , bw.RemainingSize()  )
                            UT_EQ(bw.GetIndex(), BitBuffer::Index::Decode32(bw.GetIndex().Encode32()) )
                            UT_EQ(bw.GetIndex(), BitBuffer::Index::Decode64(bw.GetIndex().Encode64()) )

        bw.Write<bitsof(BitBuffer::TStorage)>(0ul );
                            UT_EQ(       9u + bitsof(BitBuffer::TStorage), bw.Usage()          )
                            UT_EQ(  size- 9 - bitsof(BitBuffer::TStorage), bw.RemainingSize()  )
                            UT_EQ(bw.GetIndex(), BitBuffer::Index::Decode32(bw.GetIndex().Encode32()) )
                            UT_EQ(bw.GetIndex(), BitBuffer::Index::Decode64(bw.GetIndex().Encode64()) )
        bw.Flush();

        // termination test
        auto idxBeforeTermination= bw.GetIndex();
        auto idxAfterTermination= bb.Terminate( idxBeforeTermination );
        UT_TRUE( idxAfterTermination.Pos() > idxBeforeTermination.Pos() )
        UT_TRUE( idxBeforeTermination    == bb.Unterminate(idxAfterTermination) )
    }

    UT_PRINT( "2. Writing integrals:" )
    {
        BitWriter bw(bb);
        BitReader br(bb);
        auto idxZer= br.GetIndex();
        #define TyPE  uint8_t
        {
            TyPE val;
            val= (std::numeric_limits<TyPE>::min)()    ; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<TyPE>())
            val= (std::numeric_limits<TyPE>::min)() + 1; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<TyPE>())
            val= (std::numeric_limits<TyPE>::max)()    ; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<TyPE>())
            val= (std::numeric_limits<TyPE>::max)() - 1; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<TyPE>())
        }
        #undef TyPE
        #define TyPE  uint16_t
        {
            TyPE val;
            val= (std::numeric_limits<TyPE>::min)()    ; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<TyPE>())
            val= (std::numeric_limits<TyPE>::min)() + 1; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<TyPE>())
            val= (std::numeric_limits<TyPE>::max)()    ; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<TyPE>())
            val= (std::numeric_limits<TyPE>::max)() - 1; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<TyPE>())
        }
        #undef TyPE
        #define TyPE  uint32_t
        {
            TyPE val;
            val= (std::numeric_limits<TyPE>::min)()    ; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<TyPE>())
            val= (std::numeric_limits<TyPE>::min)() + 1; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<TyPE>())
            val= (std::numeric_limits<TyPE>::max)()    ; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<TyPE>())
            val= (std::numeric_limits<TyPE>::max)() - 1; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<TyPE>())
        }
        #undef TyPE
        #define TyPE  uint64_t
        {
            TyPE val;
            val= (std::numeric_limits<TyPE>::min)()    ; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<TyPE>())
            val= (std::numeric_limits<TyPE>::min)() + 1; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<TyPE>())
            val= (std::numeric_limits<TyPE>::max)()    ; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<TyPE>())
            val= (std::numeric_limits<TyPE>::max)() - 1; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<TyPE>())
        }
        #undef TyPE

        {
            uint8_t val;
            for( ShiftOpRHS bits= 0; bits < 8; ++bits )
            {
                val=  uint8_t(1 << bits); bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<uint8_t>())
                val++                   ; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<uint8_t>())
                val-=2                  ; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<uint8_t>())
                val--                   ; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<uint8_t>())
                UT_EQ(  bw.GetIndex(), BitBuffer::Index::Decode32( bw.GetIndex().Encode32()) )
                UT_EQ(  br.GetIndex(), BitBuffer::Index::Decode64( br.GetIndex().Encode64()) )
            }
        }
        UT_PRINT("Buff usage: {:%.01}", double(bw.Usage()) / double(bb.Capacity()) )

        {
            uint16_t val;

            for( ShiftOpRHS bits= 0; bits < 16; ++bits )
            {
                val= uint16_t(1 << bits); bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<uint16_t>())
                val++                   ; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<uint16_t>())
                val-=2                  ; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<uint16_t>())
                val--                   ; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<uint16_t>())
                UT_EQ(  bw.GetIndex(), BitBuffer::Index::Decode32( bw.GetIndex().Encode32()) )
                UT_EQ(  br.GetIndex(), BitBuffer::Index::Decode64( br.GetIndex().Encode64()) )
            }
        }
        UT_PRINT("Buff usage: {:%.01}", double(bw.Usage()) / double(bb.Capacity()) )

        {
            uint32_t val;
            for( ShiftOpRHS bits= 0; bits < 32; ++bits )
            {
                val= uint32_t(1 << bits); bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<uint32_t>())
                val++                   ; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<uint32_t>())
                val-=2                  ; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<uint32_t>())
                val--                   ; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<uint32_t>())
                UT_EQ(  bw.GetIndex(), BitBuffer::Index::Decode32( bw.GetIndex().Encode32()) )
                UT_EQ(  br.GetIndex(), BitBuffer::Index::Decode64( br.GetIndex().Encode64()) )
            }
        }
        UT_PRINT("Buff usage: {:%.01}", double(bw.Usage()) / double(bb.Capacity()) )
        auto idxMid= br.GetIndex();

        {
            uint64_t val;
            for( ShiftOpRHS bits= 0; bits < 64; ++bits )
            {
                val= uint64_t(1) << bits; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<uint64_t>())
                val++                   ; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<uint64_t>())
                val-=2                  ; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<uint64_t>())
                val--                   ; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<uint64_t>())
                UT_EQ(  bw.GetIndex(), BitBuffer::Index::Decode32( bw.GetIndex().Encode32()) )
                UT_EQ(  br.GetIndex(), BitBuffer::Index::Decode64( br.GetIndex().Encode64()) )
            }
        }
        UT_PRINT("Buff usage: {:%.01}", double(bw.Usage()) / double(bb.Capacity()) )

        UT_FALSE( idxZer == br.GetIndex() )  UT_FALSE( br.GetIndex() == idxZer )
        UT_FALSE( idxZer >= br.GetIndex() )  UT_TRUE ( br.GetIndex() >= idxZer )
        UT_TRUE ( idxZer <= br.GetIndex() )  UT_FALSE( br.GetIndex() <= idxZer )
        UT_TRUE ( idxZer != br.GetIndex() )  UT_TRUE ( br.GetIndex() != idxZer )
        UT_TRUE ( idxZer <  br.GetIndex() )  UT_FALSE( br.GetIndex() <  idxZer )
        UT_FALSE( idxZer >  br.GetIndex() )  UT_TRUE ( br.GetIndex() >  idxZer )

        UT_FALSE( idxMid == br.GetIndex() )  UT_FALSE( br.GetIndex() == idxMid )
        UT_FALSE( idxMid >= br.GetIndex() )  UT_TRUE ( br.GetIndex() >= idxMid )
        UT_TRUE ( idxMid <= br.GetIndex() )  UT_FALSE( br.GetIndex() <= idxMid )
        UT_TRUE ( idxMid != br.GetIndex() )  UT_TRUE ( br.GetIndex() != idxMid )
        UT_TRUE ( idxMid <  br.GetIndex() )  UT_FALSE( br.GetIndex() <  idxMid )
        UT_FALSE( idxMid >  br.GetIndex() )  UT_TRUE ( br.GetIndex() >  idxMid )
    }

    UT_PRINT( "3. Signed integrals:" )
    {
        BitWriter bw(bb);
        BitReader br(bb);
        {
            #define TyPE  int8_t
            {
                TyPE val;
                val= (std::numeric_limits<TyPE>::min)()    ; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<TyPE>())
                val= (std::numeric_limits<TyPE>::min)() + 1; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<TyPE>())
                val= -1                                    ; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<TyPE>())
                val= 0                                     ; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<TyPE>())
                val= +1                                    ; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<TyPE>())
                val= (std::numeric_limits<TyPE>::max)()    ; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<TyPE>())
                val= (std::numeric_limits<TyPE>::max)() - 1; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<TyPE>())
            }
            #undef TyPE
            #define TyPE  int16_t
            {
                TyPE val;
                val= (std::numeric_limits<TyPE>::min)()    ; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<TyPE>())
                val= (std::numeric_limits<TyPE>::min)() + 1; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<TyPE>())
                val= -1                                    ; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<TyPE>())
                val= 0                                     ; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<TyPE>())
                val= +1                                    ; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<TyPE>())
                val= (std::numeric_limits<TyPE>::max)()    ; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<TyPE>())
                val= (std::numeric_limits<TyPE>::max)() - 1; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<TyPE>())
            }
            #undef TyPE
            #define TyPE  int32_t
            {
                TyPE val;
                val= (std::numeric_limits<TyPE>::min)()    ; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<TyPE>())
                val= (std::numeric_limits<TyPE>::min)() + 1; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<TyPE>())
                val= -1                                    ; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<TyPE>())
                val= 0                                     ; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<TyPE>())
                val= +1                                    ; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<TyPE>())
                val= (std::numeric_limits<TyPE>::max)()    ; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<TyPE>())
                val= (std::numeric_limits<TyPE>::max)() - 1; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<TyPE>())
            }
            #undef TyPE
            #define TyPE  int64_t
            {
                TyPE val;
                val= (std::numeric_limits<TyPE>::min)()    ; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<TyPE>())
                val= (std::numeric_limits<TyPE>::min)() + 1; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<TyPE>())
                val= -1                                    ; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<TyPE>())
                val= 0                                     ; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<TyPE>())
                val= +1                                    ; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<TyPE>())
                val= (std::numeric_limits<TyPE>::max)()    ; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<TyPE>())
                val= (std::numeric_limits<TyPE>::max)() - 1; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<TyPE>())
            }
            #undef TyPE
        }
        {
            int8_t val;
            for( ShiftOpRHS bits= 0; bits < 8; ++bits )
            {
                val=  int8_t(1 << bits) ; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<int8_t>())
                val++                   ; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<int8_t>())
                val=-val                ; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<int8_t>())
                val++                   ; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<int8_t>())
                val++                   ; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<int8_t>())

                // let's do an Index encoding/decoding test here
                UT_EQ(  bw.GetIndex(), BitBuffer::Index::Decode32( bw.GetIndex().Encode32()) )
                UT_EQ(  br.GetIndex(), BitBuffer::Index::Decode64( br.GetIndex().Encode64()) )

                // let's do a Termination test here
                auto idxBeforeTermination= bw.GetIndex();
                auto oldLastWord         = bb.GetWord( idxBeforeTermination );
                auto terminationIdx      = bb.Terminate(idxBeforeTermination);
                UT_TRUE( terminationIdx > idxBeforeTermination )
                auto idxAfterTermination = bb.Unterminate(terminationIdx);
                UT_TRUE( idxAfterTermination == idxBeforeTermination )
                UT_TRUE( oldLastWord == bb.GetWord( idxAfterTermination ) )
            }
        }
        UT_PRINT("Buff usage: {:%.01}", double(bw.Usage()) / double(bb.Capacity()) )

        {
            int16_t val;
            for( ShiftOpRHS bits= 0; bits < 16; ++bits )
            {
                val= int16_t(1 << bits) ; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<int16_t>())
                val++                   ; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<int16_t>())
                val=-val                ; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<int16_t>())
                val++                   ; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<int16_t>())
                val++                   ; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<int16_t>())

                // let's do an Index encoding/decoding test here
                UT_EQ(  bw.GetIndex(), BitBuffer::Index::Decode32( bw.GetIndex().Encode32()) )
                UT_EQ(  br.GetIndex(), BitBuffer::Index::Decode64( br.GetIndex().Encode64()) )

                // let's do a Termination test here
                auto idxBeforeTermination= bw.GetIndex();
                auto oldLastWord         = bb.GetWord( idxBeforeTermination );
                auto terminationIdx      = bb.Terminate(idxBeforeTermination);
                UT_TRUE( terminationIdx > idxBeforeTermination )
                auto idxAfterTermination = bb.Unterminate(terminationIdx);
                UT_TRUE( idxAfterTermination == idxBeforeTermination )
                UT_TRUE( oldLastWord == bb.GetWord( idxAfterTermination ) )
            }
        }

        UT_PRINT("Buff usage: {:%.01}", double(bw.Usage()) / double(bb.Capacity()) )
        {
            int32_t val;
            for( ShiftOpRHS bits= 31; bits < 32; ++bits )
            {
                val= int32_t(1 << bits) ; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<int32_t>())
                val++                   ; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<int32_t>())
                val=-val                ; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<int32_t>())
                if( bits < 31 ) // overflow must not be done due to tricky gcc optimization
                                // (would not recognize  a negative value inside Write()!!!!)
                {
                    val++               ; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<int32_t>())
                    val++               ; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<int32_t>())
                }

                // let's do an Index encoding/decoding test here
                UT_EQ(  bw.GetIndex(), BitBuffer::Index::Decode32( bw.GetIndex().Encode32()) )
                UT_EQ(  br.GetIndex(), BitBuffer::Index::Decode64( br.GetIndex().Encode64()) )

                // let's do a Termination test here
                auto idxBeforeTermination= bw.GetIndex();
                auto oldLastWord         = bb.GetWord( idxBeforeTermination );
                auto terminationIdx      = bb.Terminate(idxBeforeTermination);
                UT_TRUE( terminationIdx > idxBeforeTermination )
                auto idxAfterTermination = bb.Unterminate(terminationIdx);
                UT_TRUE( idxAfterTermination == idxBeforeTermination )
                UT_TRUE( oldLastWord == bb.GetWord( idxAfterTermination ) )
            }
        }

        UT_PRINT("Buff usage: {:%.01}", double(bw.Usage()) / double(bb.Capacity()) )

        {
            int64_t val;
            for( ShiftOpRHS bits= 0 ; bits < 64 ; ++bits )
            {
                val= int64_t(1) << bits  ; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<int64_t>())
                val++                    ; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<int64_t>())
                val=-val                 ; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<int64_t>())
                if( bits < 31 ) // overflow must not be done due to tricky gcc optimization
                                // (would not recognize a negative value inside Write()!!!!)
                {
                    val++                ; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<int64_t>())
                    val++                ; bw.Write(val); bw.Flush(); UT_EQ(val, br.Sync().Read<int64_t>())
                }

                // let's do an Index encoding/decoding test here
                UT_EQ(  bw.GetIndex(), BitBuffer::Index::Decode32( bw.GetIndex().Encode32()) )
                UT_EQ(  br.GetIndex(), BitBuffer::Index::Decode64( br.GetIndex().Encode64()) )

                // let's do a Termination test here
                auto idxBeforeTermination= bw.GetIndex();
                auto oldLastWord         = bb.GetWord( idxBeforeTermination );
                auto terminationIdx      = bb.Terminate(idxBeforeTermination);
                UT_TRUE( terminationIdx > idxBeforeTermination )
                auto idxAfterTermination = bb.Unterminate(terminationIdx);
                UT_TRUE( idxAfterTermination == idxBeforeTermination )
                UT_TRUE( oldLastWord == bb.GetWord( idxAfterTermination ) )
            }
        }
        UT_PRINT("Buff usage: {:%.01}", double(bw.Usage()) / double(bb.Capacity()) )
    }

    UT_PRINT( "4. Oversized integrals (maybe greater than storage size):" )
    {
        BitWriter bw(bb);
        BitReader br(bb);

        bw.Write<      4, uint32_t, true>( uint32_t(         0xAAAAAAAAu) );   bw.Flush(); UT_EQ( br.Sync().Read<    4 ALIB_COMMA uint32_t>(),  LowerBits<    4>(         0xAAAAAAAAu))
        bw.Write<      4, uint32_t, true>( uint32_t(         0x55555555u) );   bw.Flush(); UT_EQ( br.Sync().Read<    4 ALIB_COMMA uint32_t>(),  LowerBits<    4>(         0x55555555u))
        bw.Write<     29, uint32_t, true>( uint32_t(         0xAAAAAAAAu) );   bw.Flush(); UT_EQ( br.Sync().Read<   29 ALIB_COMMA uint32_t>(),  LowerBits<   29>(         0xAAAAAAAAu))
        bw.Write<     29, uint32_t, true>( uint32_t(         0x55555555u) );   bw.Flush(); UT_EQ( br.Sync().Read<   29 ALIB_COMMA uint32_t>(),  LowerBits<   29>(         0x55555555u))
        bw.Write<     31, uint32_t, true>( uint32_t(         0xAAAAAAAAu) );   bw.Flush(); UT_EQ( br.Sync().Read<   31 ALIB_COMMA uint32_t>(),  LowerBits<   31>(         0xAAAAAAAAu))
        bw.Write<     31, uint32_t, true>( uint32_t(         0x55555555u) );   bw.Flush(); UT_EQ( br.Sync().Read<   31 ALIB_COMMA uint32_t>(),  LowerBits<   31>(         0x55555555u))
        bw.Write<     32, uint32_t, true>( uint32_t(         0xAAAAAAAAu) );   bw.Flush(); UT_EQ( br.Sync().Read<   32 ALIB_COMMA uint32_t>(),                                 0xAAAAAAAAu )
        bw.Write<     32, uint32_t, true>( uint32_t(         0x55555555u) );   bw.Flush(); UT_EQ( br.Sync().Read<   32 ALIB_COMMA uint32_t>(),                                 0x55555555u )

        bw.Write< 4 + 32, uint64_t, true>( uint64_t(0xAAAAAAAAAAAAAAAAul) );   bw.Flush(); UT_EQ( br.Sync().Read< 4+32 ALIB_COMMA uint64_t>(),  LowerBits< 4+32>(0xAAAAAAAAAAAAAAAAul))
        bw.Write< 4 + 32, uint64_t, true>( uint64_t(0x5555555555555555ul) );   bw.Flush(); UT_EQ( br.Sync().Read< 4+32 ALIB_COMMA uint64_t>(),  LowerBits< 4+32>(0x5555555555555555ul))
        bw.Write<29 + 32, uint64_t, true>( uint64_t(0xAAAAAAAAAAAAAAAAul) );   bw.Flush(); UT_EQ( br.Sync().Read<29+32 ALIB_COMMA uint64_t>(),  LowerBits<29+32>(0xAAAAAAAAAAAAAAAAul))
        bw.Write<29 + 32, uint64_t, true>( uint64_t(0x5555555555555555ul) );   bw.Flush(); UT_EQ( br.Sync().Read<29+32 ALIB_COMMA uint64_t>(),  LowerBits<29+32>(0x5555555555555555ul))
        bw.Write<31 + 32, uint64_t, true>( uint64_t(0xAAAAAAAAAAAAAAAAul) );   bw.Flush(); UT_EQ( br.Sync().Read<31+32 ALIB_COMMA uint64_t>(),  LowerBits<31+32>(0xAAAAAAAAAAAAAAAAul))
        bw.Write<31 + 32, uint64_t, true>( uint64_t(0x5555555555555555ul) );   bw.Flush(); UT_EQ( br.Sync().Read<31+32 ALIB_COMMA uint64_t>(),  LowerBits<31+32>(0x5555555555555555ul))
        bw.Write<32 + 32, uint64_t, true>( uint64_t(0xAAAAAAAAAAAAAAAAul) );   bw.Flush(); UT_EQ( br.Sync().Read<32+32 ALIB_COMMA uint64_t>(),                        0xAAAAAAAAAAAAAAAAul )
        bw.Write<32 + 32, uint64_t, true>( uint64_t(0x5555555555555555ul) );   bw.Flush(); UT_EQ( br.Sync().Read<32+32 ALIB_COMMA uint64_t>(),                        0x5555555555555555ul )

        // NIY: uint128_t
    }

    UT_PRINT( "5. Fixed bit size:" )
    {
        int loopsize= 100;
        BitWriter bw(bb);
        BitReader br(bb);

        UT_PRINT("Used buffer: {}", bw.Usage()  )


        for(ShiftOpRHS width=0 ; width <= bitsof(unsigned int) ; ++width )
            for(int i=0 ; i < loopsize; ++i )
            {
                auto val= width == bitsof(int) ? i : LowerBits(width, i);
                bw.Write( width, val ); bw.Flush(); UT_EQ( val, br.Sync().Read<int>(width) )
                UT_EQ(  bw.GetIndex(), BitBuffer::Index::Decode32( bw.GetIndex().Encode32()) )
                UT_EQ(  br.GetIndex(), BitBuffer::Index::Decode64( br.GetIndex().Encode64()) )
            }

        br.Reset();
        for(ShiftOpRHS width=0 ; width <= bitsof(int) ; ++width )
            for(int i=0 ; i < loopsize; ++i )
                UT_EQ( width < bitsof(int) ? LowerBits(width, i) : i,
                       br.Sync().Read<int>(width) )

        bw.Reset();
        br.Reset();
        for(ShiftOpRHS width= bitsof(int) +1 ; width <= bitsof(uinteger) ; ++width )
            for(uinteger i=0 ; i < uinteger(loopsize); ++i )
            {
                uinteger val= width < bitsof(uinteger) ? LowerBits(width, i) : i;
                bw.Write( width, val ); bw.Flush(); UT_EQ( val,br.Sync().Read<uinteger>(width) )
                UT_EQ(  bw.GetIndex(), BitBuffer::Index::Decode32( bw.GetIndex().Encode32()) )
                UT_EQ(  br.GetIndex(), BitBuffer::Index::Decode64( br.GetIndex().Encode64()) )
            }

        br.Reset();
        for(ShiftOpRHS width=bitsof(int) +1 ; width <= bitsof(uinteger); ++width )
            for(uinteger i=0 ; i < uinteger(loopsize); ++i )
                UT_EQ( width < bitsof(int) ? LowerBits(width, i) : i,
                       br.Sync().Read<uinteger>(width) )
    }


    UT_PRINT( "6. Fixed bit size (templated methods):" )
    {
        int loopsize= 50;
        BitWriter bw(bb);
        BitReader br(bb);

        UT_PRINT("Used buffer: {}", bw.Usage()  )

        for(int i=0 ; i < loopsize; ++i )
        {
            int val= LowerBits<3>(i);
            bw.Write<3>( val ); bw.Flush(); UT_EQ( val, br.Sync().Read< 3  ALIB_COMMA int>() )
            UT_EQ(  bw.GetIndex(), BitBuffer::Index::Decode32( bw.GetIndex().Encode32()) )
            UT_EQ(  br.GetIndex(), BitBuffer::Index::Decode64( br.GetIndex().Encode64()) )
        }
        br.Reset();
        for(int i=0 ; i < loopsize; ++i )
            UT_EQ( LowerBits<3>(i) , br.Sync().Read< 3  ALIB_COMMA int>() )

        bw.Reset();
        br.Reset();
        for(int i=0 ; i < loopsize; ++i )
        {
            int val= LowerBits<4>(i);
            bw.Write<4>( val ); bw.Flush();UT_EQ( val, br.Sync().Read< 4  ALIB_COMMA int>() )
            UT_EQ(  bw.GetIndex(), BitBuffer::Index::Decode32( bw.GetIndex().Encode32()) )
            UT_EQ(  br.GetIndex(), BitBuffer::Index::Decode64( br.GetIndex().Encode64()) )
        }
        br.Reset();
        for(int i=0 ; i < loopsize; ++i )
            UT_EQ( LowerBits<4>(i) , br.Sync().Read< 4  ALIB_COMMA int>() )


        bw.Reset();
        br.Reset();
        for(int i=0 ; i < loopsize; ++i )
        {
            int val= LowerBits<7>(i);
            bw.Write<7>( val ); bw.Flush(); UT_EQ( val, br.Sync().Read< 7  ALIB_COMMA int>() )
            UT_EQ(  bw.GetIndex(), BitBuffer::Index::Decode32( bw.GetIndex().Encode32()) )
            UT_EQ(  br.GetIndex(), BitBuffer::Index::Decode64( br.GetIndex().Encode64()) )
        }
        br.Reset();
        for(int i=0 ; i < loopsize; ++i )
            UT_EQ( LowerBits<7>(i) , br.Sync().Read< 7  ALIB_COMMA int>() )

        bw.Reset();
        br.Reset();
        for(int i=0 ; i < loopsize; ++i )
        {
            int val= LowerBits<8>(i);
            bw.Write<8>( val ); bw.Flush(); UT_EQ( val, br.Sync().Read< 8  ALIB_COMMA int>() )
            UT_EQ(  bw.GetIndex(), BitBuffer::Index::Decode32( bw.GetIndex().Encode32()) )
            UT_EQ(  br.GetIndex(), BitBuffer::Index::Decode64( br.GetIndex().Encode64()) )
        }
        br.Reset();
        for(int i=0 ; i < loopsize; ++i )
            UT_EQ( LowerBits<8>(i) , br.Sync().Read< 8  ALIB_COMMA int>() )

        bw.Reset();
        br.Reset();
        for(int i=0 ; i < loopsize; ++i )
        {
            int val= LowerBits<15>(i);
            bw.Write<15>( val ); bw.Flush(); UT_EQ( val, br.Sync().Read< 15  ALIB_COMMA int>() )
            UT_EQ(  bw.GetIndex(), BitBuffer::Index::Decode32( bw.GetIndex().Encode32()) )
            UT_EQ(  br.GetIndex(), BitBuffer::Index::Decode64( br.GetIndex().Encode64()) )
        }
        br.Reset();
        for(int i=0 ; i < loopsize; ++i )
            UT_EQ( LowerBits<15>(i) , br.Sync().Read< 15  ALIB_COMMA int>() )

        bw.Reset();
        br.Reset();
        for(int i=0 ; i < loopsize; ++i )
        {
            int val= LowerBits<16>(i);
            bw.Write<16>( val ); bw.Flush(); UT_EQ( val, br.Sync().Read< 16  ALIB_COMMA int>() )
            UT_EQ(  bw.GetIndex(), BitBuffer::Index::Decode32( bw.GetIndex().Encode32()) )
            UT_EQ(  br.GetIndex(), BitBuffer::Index::Decode64( br.GetIndex().Encode64()) )
        }
        br.Reset();
        for(int i=0 ; i < loopsize; ++i )
            UT_EQ( LowerBits<16>(i) , br.Sync().Read< 16  ALIB_COMMA int>() )

        bw.Reset();
        br.Reset();
        for(unsigned int i=0 ; i < static_cast<unsigned int>(loopsize); ++i )
        {
            unsigned int val= LowerBits<31, unsigned int>(i);
            bw.Write<31>( val ); bw.Flush(); UT_EQ( val, br.Sync().Read< 31  ALIB_COMMA unsigned int>() )
            UT_EQ(  bw.GetIndex(), BitBuffer::Index::Decode32( bw.GetIndex().Encode32()) )
            UT_EQ(  br.GetIndex(), BitBuffer::Index::Decode64( br.GetIndex().Encode64()) )
        }
        br.Reset();
        for(unsigned int i=0 ; i < static_cast<unsigned int>(loopsize); ++i )
            UT_EQ( LowerBits<31>(i) , br.Sync().Read< 31  ALIB_COMMA unsigned int>() )

        bw.Reset();
        br.Reset();
        for(int i=0 ; i < loopsize; ++i )
        {
            int val= LowerBits<32>(i);
            bw.Write<32>( val ); bw.Flush(); UT_EQ( val, br.Sync().Read< 32  ALIB_COMMA int>() )
            UT_EQ(  bw.GetIndex(), BitBuffer::Index::Decode32( bw.GetIndex().Encode32()) )
            UT_EQ(  br.GetIndex(), BitBuffer::Index::Decode64( br.GetIndex().Encode64()) )
        }
        br.Reset();
        for(int i=0 ; i < loopsize; ++i )
            UT_EQ( LowerBits<32>(i) , br.Sync().Read< 32  ALIB_COMMA int>() )

        bw.Reset();
        br.Reset();
        for(uint64_t i=0 ; i < uint64_t(loopsize); ++i )
        {
            uint64_t val= LowerBits<35>(i);
            bw.Write<35>( val ); bw.Flush(); UT_EQ( val, br.Sync().Read< 35  ALIB_COMMA uint64_t>() )
            UT_EQ(  bw.GetIndex(), BitBuffer::Index::Decode32( bw.GetIndex().Encode32()) )
            UT_EQ(  br.GetIndex(), BitBuffer::Index::Decode64( br.GetIndex().Encode64()) )
        }
        br.Reset();
        for(uint64_t i=0 ; i < uint64_t(loopsize); ++i )
            UT_EQ( LowerBits<35>(i) , br.Sync().Read< 35  ALIB_COMMA uint64_t>() )

        bw.Reset();
        br.Reset();
        for(uint64_t i=0 ; i < uint64_t(loopsize); ++i )
        {
            uint64_t val= LowerBits<63>(i);
            bw.Write<63>( val ); bw.Flush(); UT_EQ( val, br.Sync().Read< 63  ALIB_COMMA uint64_t>() )
            UT_EQ(  bw.GetIndex(), BitBuffer::Index::Decode32( bw.GetIndex().Encode32()) )
            UT_EQ(  br.GetIndex(), BitBuffer::Index::Decode64( br.GetIndex().Encode64()) )
        }
        br.Reset();
        for(uint64_t i=0 ; i < uint64_t(loopsize); ++i )
            UT_EQ( LowerBits<63>(i) , br.Sync().Read< 63  ALIB_COMMA uint64_t>() )

        bw.Reset();
        br.Reset();
        for(uint64_t i=0 ; i < uint64_t(loopsize); ++i )
        {
            uint64_t val= LowerBits<64>(i);
            bw.Write<64>( val ); bw.Flush(); UT_EQ( val, br.Sync().Read< 64  ALIB_COMMA uint64_t>() )
            UT_EQ(  bw.GetIndex(), BitBuffer::Index::Decode32( bw.GetIndex().Encode32()) )
            UT_EQ(  br.GetIndex(), BitBuffer::Index::Decode64( br.GetIndex().Encode64()) )
        }
        br.Reset();
        for(uint64_t i=0 ; i < uint64_t(loopsize); ++i )
            UT_EQ( LowerBits<64>(i) , br.Sync().Read< 64  ALIB_COMMA uint64_t>() )


        UT_PRINT( "Used buffer: {}", bw.Usage()  )
    }
}

#include "aworx_unittests_end.hpp"
} // namespace ut_aworx

#endif // ALIB_UT_MONOMEM
