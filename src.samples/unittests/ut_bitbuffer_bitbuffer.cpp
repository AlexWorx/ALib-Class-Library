// #################################################################################################
//  AWorx ALib Unit Tests
//
//  Copyright 2013-2026 A-Worx GmbH, Germany
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
            bw.WriteBits<1>(0);
        bw.Flush();

        BitReader br(bb);
        for (int i = 0; i < 10; ++i)
            br.ReadBits<1>();
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
        bw.WriteBits(true); UT_EQ(1u                    , bw.Usage()          )
                            UT_EQ(  size- 1                 , bw.RemainingSize()  )
                            UT_EQ(bw.GetIndex(), BitBuffer::Index::Decode32(bw.GetIndex().Encode32()) )
                            UT_EQ(bw.GetIndex(), BitBuffer::Index::Decode64(bw.GetIndex().Encode64()) )

                            UT_FALSE(idxZer == bw.GetIndex() )
                            UT_FALSE(idxZer >= bw.GetIndex() )
                            UT_TRUE (idxZer <= bw.GetIndex() )
                            UT_TRUE (idxZer != bw.GetIndex() )
                            UT_TRUE (idxZer <  bw.GetIndex() )
                            UT_FALSE(idxZer >  bw.GetIndex() )
        bw.WriteBits(true); UT_EQ(2u                    , bw.Usage()          )
                            UT_EQ(  size- 2                 , bw.RemainingSize()  )
                            UT_EQ(bw.GetIndex(), BitBuffer::Index::Decode32(bw.GetIndex().Encode32()) )
                            UT_EQ(bw.GetIndex(), BitBuffer::Index::Decode64(bw.GetIndex().Encode64()) )

        bw.WriteBits<5>(0 );UT_EQ(7u                    , bw.Usage()          )
        UT_EQ(  size- 7                 , bw.RemainingSize()  )
                            UT_EQ(bw.GetIndex(), BitBuffer::Index::Decode32(bw.GetIndex().Encode32()) )
                            UT_EQ(bw.GetIndex(), BitBuffer::Index::Decode64(bw.GetIndex().Encode64()) )

        bw.WriteBits<1>(0 );UT_EQ(8u                    , bw.Usage()          )
        UT_EQ(  size- 8                 , bw.RemainingSize()  )
                            UT_EQ(bw.GetIndex(), BitBuffer::Index::Decode32(bw.GetIndex().Encode32()) )
                            UT_EQ(bw.GetIndex(), BitBuffer::Index::Decode64(bw.GetIndex().Encode64()) )

        bw.WriteBits<1>(0 );UT_EQ(9u                    , bw.Usage()          )
                            UT_EQ(  size- 9                 , bw.RemainingSize()  )
                            UT_EQ(bw.GetIndex(), BitBuffer::Index::Decode32(bw.GetIndex().Encode32()) )
                            UT_EQ(bw.GetIndex(), BitBuffer::Index::Decode64(bw.GetIndex().Encode64()) )

        bw.WriteBits<bitsof(BitBuffer::TStorage)>(0ul );
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
            val= (std::numeric_limits<TyPE>::min)()    ; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<TyPE>())
            val= (std::numeric_limits<TyPE>::min)() + 1; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<TyPE>())
            val= (std::numeric_limits<TyPE>::max)()    ; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<TyPE>())
            val= (std::numeric_limits<TyPE>::max)() - 1; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<TyPE>())
        }
        #undef TyPE
        #define TyPE  uint16_t
        {
            TyPE val;
            val= (std::numeric_limits<TyPE>::min)()    ; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<TyPE>())
            val= (std::numeric_limits<TyPE>::min)() + 1; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<TyPE>())
            val= (std::numeric_limits<TyPE>::max)()    ; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<TyPE>())
            val= (std::numeric_limits<TyPE>::max)() - 1; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<TyPE>())
        }
        #undef TyPE
        #define TyPE  uint32_t
        {
            TyPE val;
            val= (std::numeric_limits<TyPE>::min)()    ; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<TyPE>())
            val= (std::numeric_limits<TyPE>::min)() + 1; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<TyPE>())
            val= (std::numeric_limits<TyPE>::max)()    ; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<TyPE>())
            val= (std::numeric_limits<TyPE>::max)() - 1; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<TyPE>())
        }
        #undef TyPE
        #define TyPE  uint64_t
        {
            TyPE val;
            val= (std::numeric_limits<TyPE>::min)()    ; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<TyPE>())
            val= (std::numeric_limits<TyPE>::min)() + 1; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<TyPE>())
            val= (std::numeric_limits<TyPE>::max)()    ; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<TyPE>())
            val= (std::numeric_limits<TyPE>::max)() - 1; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<TyPE>())
        }
        #undef TyPE

        {
            uint8_t val;
            for( ShiftOpRHS bits= 0; bits < 8; ++bits )
            {
                val=  uint8_t(1 << bits); bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<uint8_t>())
                val++                   ; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<uint8_t>())
                val-=2                  ; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<uint8_t>())
                val--                   ; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<uint8_t>())
                UT_EQ(  bw.GetIndex(), BitBuffer::Index::Decode32( bw.GetIndex().Encode32()) )
                UT_EQ(  br.GetIndex(), BitBuffer::Index::Decode64( br.GetIndex().Encode64()) )
            }
        }
        UT_PRINT("Buff usage: {:%.01}", double(bw.Usage()) / double(bb.Capacity()) )

        {
            uint16_t val;

            for( ShiftOpRHS bits= 0; bits < 16; ++bits )
            {
                val= uint16_t(1 << bits); bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<uint16_t>())
                val++                   ; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<uint16_t>())
                val-=2                  ; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<uint16_t>())
                val--                   ; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<uint16_t>())
                UT_EQ(  bw.GetIndex(), BitBuffer::Index::Decode32( bw.GetIndex().Encode32()) )
                UT_EQ(  br.GetIndex(), BitBuffer::Index::Decode64( br.GetIndex().Encode64()) )
            }
        }
        UT_PRINT("Buff usage: {:%.01}", double(bw.Usage()) / double(bb.Capacity()) )

        {
            uint32_t val;
            for( ShiftOpRHS bits= 0; bits < 32; ++bits )
            {
                val= uint32_t(1 << bits); bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<uint32_t>())
                val++                   ; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<uint32_t>())
                val-=2                  ; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<uint32_t>())
                val--                   ; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<uint32_t>())
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
                val= uint64_t(1) << bits; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<uint64_t>())
                val++                   ; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<uint64_t>())
                val-=2                  ; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<uint64_t>())
                val--                   ; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<uint64_t>())
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
                val= (std::numeric_limits<TyPE>::min)()    ; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<TyPE>())
                val= (std::numeric_limits<TyPE>::min)() + 1; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<TyPE>())
                val= -1                                    ; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<TyPE>())
                val= 0                                     ; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<TyPE>())
                val= +1                                    ; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<TyPE>())
                val= (std::numeric_limits<TyPE>::max)()    ; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<TyPE>())
                val= (std::numeric_limits<TyPE>::max)() - 1; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<TyPE>())
            }
            #undef TyPE
            #define TyPE  int16_t
            {
                TyPE val;
                val= (std::numeric_limits<TyPE>::min)()    ; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<TyPE>())
                val= (std::numeric_limits<TyPE>::min)() + 1; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<TyPE>())
                val= -1                                    ; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<TyPE>())
                val= 0                                     ; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<TyPE>())
                val= +1                                    ; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<TyPE>())
                val= (std::numeric_limits<TyPE>::max)()    ; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<TyPE>())
                val= (std::numeric_limits<TyPE>::max)() - 1; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<TyPE>())
            }
            #undef TyPE
            #define TyPE  int32_t
            {
                TyPE val;
                val= (std::numeric_limits<TyPE>::min)()    ; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<TyPE>())
                val= (std::numeric_limits<TyPE>::min)() + 1; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<TyPE>())
                val= -1                                    ; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<TyPE>())
                val= 0                                     ; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<TyPE>())
                val= +1                                    ; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<TyPE>())
                val= (std::numeric_limits<TyPE>::max)()    ; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<TyPE>())
                val= (std::numeric_limits<TyPE>::max)() - 1; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<TyPE>())
            }
            #undef TyPE
            #define TyPE  int64_t
            {
                TyPE val;
                val= (std::numeric_limits<TyPE>::min)()    ; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<TyPE>())
                val= (std::numeric_limits<TyPE>::min)() + 1; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<TyPE>())
                val= -1                                    ; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<TyPE>())
                val= 0                                     ; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<TyPE>())
                val= +1                                    ; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<TyPE>())
                val= (std::numeric_limits<TyPE>::max)()    ; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<TyPE>())
                val= (std::numeric_limits<TyPE>::max)() - 1; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<TyPE>())
            }
            #undef TyPE
        }
        {
            int8_t val;
            for( ShiftOpRHS bits= 0; bits < 8; ++bits )
            {
                val=  int8_t(1 << bits) ; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<int8_t>())
                val++                   ; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<int8_t>())
                val= int8_t(-val)       ; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<int8_t>())
                val++                   ; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<int8_t>())
                val++                   ; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<int8_t>())

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
                val= int16_t(1 << bits) ; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<int16_t>())
                val++                   ; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<int16_t>())
                val= int16_t(-val)      ; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<int16_t>())
                val++                   ; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<int16_t>())
                val++                   ; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<int16_t>())

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
                val= int32_t(1 << bits) ; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<int32_t>())
                val++                   ; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<int32_t>())
                val=-val                ; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<int32_t>())
                if( bits < 31 ) // overflow must not be done due to tricky gcc optimization
                                // (would not recognize  a negative value inside Write()!!!!)
                {
                    val++               ; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<int32_t>())
                    val++               ; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<int32_t>())
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
                val= int64_t(1) << bits  ; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<int64_t>())
                val++                    ; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<int64_t>())
                val=-val                 ; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<int64_t>())
                if( bits < 31 ) // overflow must not be done due to tricky gcc optimization
                                // (would not recognize a negative value inside Write()!!!!)
                {
                    val++                ; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<int64_t>())
                    val++                ; bw.WriteInt(val); bw.Flush(); UT_EQ(val, br.Sync().ReadInt<int64_t>())
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

        bw.WriteBits<      4, uint32_t, true>( uint32_t(         0xAAAAAAAAu) );   bw.Flush(); UT_EQ( (br.Sync().ReadBits<    4 ALIB_COMMA uint32_t>()),  LowerBits<    4>(         0xAAAAAAAAu))
        bw.WriteBits<      4, uint32_t, true>( uint32_t(         0x55555555u) );   bw.Flush(); UT_EQ( (br.Sync().ReadBits<    4 ALIB_COMMA uint32_t>()),  LowerBits<    4>(         0x55555555u))
        bw.WriteBits<     29, uint32_t, true>( uint32_t(         0xAAAAAAAAu) );   bw.Flush(); UT_EQ( (br.Sync().ReadBits<   29 ALIB_COMMA uint32_t>()),  LowerBits<   29>(         0xAAAAAAAAu))
        bw.WriteBits<     29, uint32_t, true>( uint32_t(         0x55555555u) );   bw.Flush(); UT_EQ( (br.Sync().ReadBits<   29 ALIB_COMMA uint32_t>()),  LowerBits<   29>(         0x55555555u))
        bw.WriteBits<     31, uint32_t, true>( uint32_t(         0xAAAAAAAAu) );   bw.Flush(); UT_EQ( (br.Sync().ReadBits<   31 ALIB_COMMA uint32_t>()),  LowerBits<   31>(         0xAAAAAAAAu))
        bw.WriteBits<     31, uint32_t, true>( uint32_t(         0x55555555u) );   bw.Flush(); UT_EQ( (br.Sync().ReadBits<   31 ALIB_COMMA uint32_t>()),  LowerBits<   31>(         0x55555555u))
        bw.WriteBits<     32, uint32_t, true>( uint32_t(         0xAAAAAAAAu) );   bw.Flush(); UT_EQ( (br.Sync().ReadBits<   32 ALIB_COMMA uint32_t>()),                            0xAAAAAAAAu )
        bw.WriteBits<     32, uint32_t, true>( uint32_t(         0x55555555u) );   bw.Flush(); UT_EQ( (br.Sync().ReadBits<   32 ALIB_COMMA uint32_t>()),                            0x55555555u )

        bw.WriteBits< 4 + 32, uint64_t, true>( uint64_t(0xAAAAAAAAAAAAAAAAul) );   bw.Flush(); UT_EQ( (br.Sync().ReadBits< 4+32 ALIB_COMMA uint64_t>()),  LowerBits< 4+32>(0xAAAAAAAAAAAAAAAAul))
        bw.WriteBits< 4 + 32, uint64_t, true>( uint64_t(0x5555555555555555ul) );   bw.Flush(); UT_EQ( (br.Sync().ReadBits< 4+32 ALIB_COMMA uint64_t>()),  LowerBits< 4+32>(0x5555555555555555ul))
        bw.WriteBits<29 + 32, uint64_t, true>( uint64_t(0xAAAAAAAAAAAAAAAAul) );   bw.Flush(); UT_EQ( (br.Sync().ReadBits<29+32 ALIB_COMMA uint64_t>()),  LowerBits<29+32>(0xAAAAAAAAAAAAAAAAul))
        bw.WriteBits<29 + 32, uint64_t, true>( uint64_t(0x5555555555555555ul) );   bw.Flush(); UT_EQ( (br.Sync().ReadBits<29+32 ALIB_COMMA uint64_t>()),  LowerBits<29+32>(0x5555555555555555ul))
        bw.WriteBits<31 + 32, uint64_t, true>( uint64_t(0xAAAAAAAAAAAAAAAAul) );   bw.Flush(); UT_EQ( (br.Sync().ReadBits<31+32 ALIB_COMMA uint64_t>()),  LowerBits<31+32>(0xAAAAAAAAAAAAAAAAul))
        bw.WriteBits<31 + 32, uint64_t, true>( uint64_t(0x5555555555555555ul) );   bw.Flush(); UT_EQ( (br.Sync().ReadBits<31+32 ALIB_COMMA uint64_t>()),  LowerBits<31+32>(0x5555555555555555ul))
        bw.WriteBits<32 + 32, uint64_t, true>( uint64_t(0xAAAAAAAAAAAAAAAAul) );   bw.Flush(); UT_EQ( (br.Sync().ReadBits<32+32 ALIB_COMMA uint64_t>()),                   0xAAAAAAAAAAAAAAAAul )
        bw.WriteBits<32 + 32, uint64_t, true>( uint64_t(0x5555555555555555ul) );   bw.Flush(); UT_EQ( (br.Sync().ReadBits<32+32 ALIB_COMMA uint64_t>()),                   0x5555555555555555ul )

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
                bw.WriteBits( width, val ); bw.Flush(); UT_EQ( val, br.Sync().ReadBits<int>(width) )
                UT_EQ(  bw.GetIndex(), BitBuffer::Index::Decode32( bw.GetIndex().Encode32()) )
                UT_EQ(  br.GetIndex(), BitBuffer::Index::Decode64( br.GetIndex().Encode64()) )
            }

        br.Reset();
        for(ShiftOpRHS width=0 ; width <= bitsof(int) ; ++width )
            for(int i=0 ; i < loopsize; ++i )
                UT_EQ( width < bitsof(int) ? LowerBits(width, i) : i,
                       br.Sync().ReadBits<int>(width) )

        bw.Reset();
        br.Reset();
        for(ShiftOpRHS width= bitsof(int) +1 ; width <= bitsof(uinteger) ; ++width )
            for(uinteger i=0 ; i < uinteger(loopsize); ++i )
            {
                uinteger val= width < bitsof(uinteger) ? LowerBits(width, i) : i;
                bw.WriteBits( width, val ); bw.Flush(); UT_EQ( val,br.Sync().ReadBits<uinteger>(width) )
                UT_EQ(  bw.GetIndex(), BitBuffer::Index::Decode32( bw.GetIndex().Encode32()) )
                UT_EQ(  br.GetIndex(), BitBuffer::Index::Decode64( br.GetIndex().Encode64()) )
            }

        br.Reset();
        for(ShiftOpRHS width=bitsof(int) +1 ; width <= bitsof(uinteger); ++width )
            for(uinteger i=0 ; i < uinteger(loopsize); ++i )
                UT_EQ( width < bitsof(int) ? LowerBits(width, i) : i,
                       br.Sync().ReadBits<uinteger>(width) )
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
            bw.WriteBits<3>( val ); bw.Flush(); UT_EQ( val, (br.Sync().ReadBits< 3  ALIB_COMMA int>()) )
            UT_EQ(  bw.GetIndex(), BitBuffer::Index::Decode32( bw.GetIndex().Encode32()) )
            UT_EQ(  br.GetIndex(), BitBuffer::Index::Decode64( br.GetIndex().Encode64()) )
        }
        br.Reset();
        for(int i=0 ; i < loopsize; ++i )
            UT_EQ( LowerBits<3>(i) , (br.Sync().ReadBits< 3  ALIB_COMMA int>()) )

        bw.Reset();
        br.Reset();
        for(int i=0 ; i < loopsize; ++i )
        {
            int val= LowerBits<4>(i);
            bw.WriteBits<4>( val ); bw.Flush();UT_EQ( val, (br.Sync().ReadBits< 4  ALIB_COMMA int>()) )
            UT_EQ(  bw.GetIndex(), BitBuffer::Index::Decode32( bw.GetIndex().Encode32()) )
            UT_EQ(  br.GetIndex(), BitBuffer::Index::Decode64( br.GetIndex().Encode64()) )
        }
        br.Reset();
        for(int i=0 ; i < loopsize; ++i )
            UT_EQ( LowerBits<4>(i) , (br.Sync().ReadBits< 4  ALIB_COMMA int>()) )


        bw.Reset();
        br.Reset();
        for(int i=0 ; i < loopsize; ++i )
        {
            int val= LowerBits<7>(i);
            bw.WriteBits<7>( val ); bw.Flush(); UT_EQ( val, (br.Sync().ReadBits< 7  ALIB_COMMA int>()) )
            UT_EQ(  bw.GetIndex(), BitBuffer::Index::Decode32( bw.GetIndex().Encode32()) )
            UT_EQ(  br.GetIndex(), BitBuffer::Index::Decode64( br.GetIndex().Encode64()) )
        }
        br.Reset();
        for(int i=0 ; i < loopsize; ++i )
            UT_EQ( LowerBits<7>(i) , (br.Sync().ReadBits< 7  ALIB_COMMA int>()) )

        bw.Reset();
        br.Reset();
        for(int i=0 ; i < loopsize; ++i )
        {
            int val= LowerBits<8>(i);
            bw.WriteBits<8>( val ); bw.Flush(); UT_EQ( val, (br.Sync().ReadBits< 8  ALIB_COMMA int>()) )
            UT_EQ(  bw.GetIndex(), BitBuffer::Index::Decode32( bw.GetIndex().Encode32()) )
            UT_EQ(  br.GetIndex(), BitBuffer::Index::Decode64( br.GetIndex().Encode64()) )
        }
        br.Reset();
        for(int i=0 ; i < loopsize; ++i )
            UT_EQ( LowerBits<8>(i) , (br.Sync().ReadBits< 8  ALIB_COMMA int>()) )

        bw.Reset();
        br.Reset();
        for(int i=0 ; i < loopsize; ++i )
        {
            int val= LowerBits<15>(i);
            bw.WriteBits<15>( val ); bw.Flush(); UT_EQ( val, (br.Sync().ReadBits< 15  ALIB_COMMA int>()) )
            UT_EQ(  bw.GetIndex(), BitBuffer::Index::Decode32( bw.GetIndex().Encode32()) )
            UT_EQ(  br.GetIndex(), BitBuffer::Index::Decode64( br.GetIndex().Encode64()) )
        }
        br.Reset();
        for(int i=0 ; i < loopsize; ++i )
            UT_EQ( LowerBits<15>(i) , (br.Sync().ReadBits< 15  ALIB_COMMA int>()) )

        bw.Reset();
        br.Reset();
        for(int i=0 ; i < loopsize; ++i )
        {
            int val= LowerBits<16>(i);
            bw.WriteBits<16>( val ); bw.Flush(); UT_EQ( val, (br.Sync().ReadBits< 16  ALIB_COMMA int>()) )
            UT_EQ(  bw.GetIndex(), BitBuffer::Index::Decode32( bw.GetIndex().Encode32()) )
            UT_EQ(  br.GetIndex(), BitBuffer::Index::Decode64( br.GetIndex().Encode64()) )
        }
        br.Reset();
        for(int i=0 ; i < loopsize; ++i )
            UT_EQ( LowerBits<16>(i) , (br.Sync().ReadBits< 16  ALIB_COMMA int>()) )

        bw.Reset();
        br.Reset();
        for(unsigned int i=0 ; i < static_cast<unsigned int>(loopsize); ++i )
        {
            unsigned int val= LowerBits<31, unsigned int>(i);
            bw.WriteBits<31>( val ); bw.Flush(); UT_EQ( val, (br.Sync().ReadBits< 31  ALIB_COMMA unsigned int>()) )
            UT_EQ(  bw.GetIndex(), BitBuffer::Index::Decode32( bw.GetIndex().Encode32()) )
            UT_EQ(  br.GetIndex(), BitBuffer::Index::Decode64( br.GetIndex().Encode64()) )
        }
        br.Reset();
        for(unsigned int i=0 ; i < static_cast<unsigned int>(loopsize); ++i )
            UT_EQ( LowerBits<31>(i) , (br.Sync().ReadBits< 31  ALIB_COMMA unsigned int>()) )

        bw.Reset();
        br.Reset();
        for(int i=0 ; i < loopsize; ++i )
        {
            int val= LowerBits<32>(i);
            bw.WriteBits<32>( val ); bw.Flush(); UT_EQ( val, (br.Sync().ReadBits< 32  ALIB_COMMA int>()) )
            UT_EQ(  bw.GetIndex(), BitBuffer::Index::Decode32( bw.GetIndex().Encode32()) )
            UT_EQ(  br.GetIndex(), BitBuffer::Index::Decode64( br.GetIndex().Encode64()) )
        }
        br.Reset();
        for(int i=0 ; i < loopsize; ++i )
            UT_EQ( LowerBits<32>(i) , (br.Sync().ReadBits< 32  ALIB_COMMA int>()) )

        bw.Reset();
        br.Reset();
        for(uint64_t i=0 ; i < uint64_t(loopsize); ++i )
        {
            uint64_t val= LowerBits<35>(i);
            bw.WriteBits<35>( val ); bw.Flush(); UT_EQ( val, (br.Sync().ReadBits< 35  ALIB_COMMA uint64_t>()) )
            UT_EQ(  bw.GetIndex(), BitBuffer::Index::Decode32( bw.GetIndex().Encode32()) )
            UT_EQ(  br.GetIndex(), BitBuffer::Index::Decode64( br.GetIndex().Encode64()) )
        }
        br.Reset();
        for(uint64_t i=0 ; i < uint64_t(loopsize); ++i )
            UT_EQ( LowerBits<35>(i) , (br.Sync().ReadBits< 35  ALIB_COMMA uint64_t>()) )

        bw.Reset();
        br.Reset();
        for(uint64_t i=0 ; i < uint64_t(loopsize); ++i )
        {
            uint64_t val= LowerBits<63>(i);
            bw.WriteBits<63>( val ); bw.Flush(); UT_EQ( val, (br.Sync().ReadBits< 63  ALIB_COMMA uint64_t>()) )
            UT_EQ(  bw.GetIndex(), BitBuffer::Index::Decode32( bw.GetIndex().Encode32()) )
            UT_EQ(  br.GetIndex(), BitBuffer::Index::Decode64( br.GetIndex().Encode64()) )
        }
        br.Reset();
        for(uint64_t i=0 ; i < uint64_t(loopsize); ++i )
            UT_EQ( LowerBits<63>(i) , (br.Sync().ReadBits< 63  ALIB_COMMA uint64_t>()) )

        bw.Reset();
        br.Reset();
        for(uint64_t i=0 ; i < uint64_t(loopsize); ++i )
        {
            uint64_t val= LowerBits<64>(i);
            bw.WriteBits<64>( val ); bw.Flush(); UT_EQ( val, (br.Sync().ReadBits< 64  ALIB_COMMA uint64_t>()) )
            UT_EQ(  bw.GetIndex(), BitBuffer::Index::Decode32( bw.GetIndex().Encode32()) )
            UT_EQ(  br.GetIndex(), BitBuffer::Index::Decode64( br.GetIndex().Encode64()) )
        }
        br.Reset();
        for(uint64_t i=0 ; i < uint64_t(loopsize); ++i )
            UT_EQ( LowerBits<64>(i) , (br.Sync().ReadBits< 64  ALIB_COMMA uint64_t>()) )


        UT_PRINT( "Used buffer: {}", bw.Usage()  )
    }
}

#include "aworx_unittests_end.hpp"
} // namespace ut_aworx

#endif // ALIB_UT_MONOMEM
