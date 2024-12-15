// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"
#include "alib/bitbuffer/bitbuffer.hpp"

namespace alib {

//==================================================================================================
/// This \alibmod implements a buffer that is write- and readable bit by bit.
/// Conversion for standard numerical types is provided. Furthermore several array compression
/// algorithms are given, which allow the bit buffers to be compressed before transported or
/// stored.
///
/// Please consult the little quick
/// \ref alib_mod_bitbuffer "ALib Module BitBuffer - Programmer's Manual" for further information.
//==================================================================================================
namespace bitbuffer {

BitBufferBase::Index BitBufferBase::Terminate( Index idx )
{
    // write termination bit
    {
        BitWriter bw(*this, idx);
        bw.Write<1>(1);
        idx= bw.GetIndex();
    }// <bw.Flush

    // fast forward to next word
    if(idx.bit)
    {
        idx.pos++;
        idx.bit= 0;
    }
    return idx;
}

BitBufferBase::Index BitBufferBase::Unterminate(Index terminationIndex )
{
    ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
    ALIB_ASSERT_ERROR(terminationIndex.pos > 0 && terminationIndex.bit == 0 && data[terminationIndex.pos - 1] != 0, "BITBUFFER",
                      "Given index is no termination index" )
    ALIB_WARNINGS_RESTORE

    // go back to previous word...
    terminationIndex.pos--;
    TStorage word= GetWord(terminationIndex);

    //...search and delete MSB
    terminationIndex.bit= lang::MSB(word );
    terminationIndex.bit--;
    word^= BitBuffer::TStorage(1) << terminationIndex.bit;

    // store word and return new reduced index
    SetWord(terminationIndex, word );
    return terminationIndex;
}

#if !DOXYGEN
// anonymous check if this platform has little endian encoding
// already (done once). If so, this method does nothing.
namespace
{
    static int                      isLittleEndianEncoding= 0;
    static BitBufferBase::TStorage  testWord;
    bool IsLittleEndianEncoding()
    {
        if( isLittleEndianEncoding == 0 )
        {
            ALIB_STATIC_ASSERT( Sizeof_int_must_be_16_32_or_64,
                                   bitsof(BitBufferBase::TStorage) == 16
                                || bitsof(BitBufferBase::TStorage) == 32
                                || bitsof(BitBufferBase::TStorage) == 64,
                               "Platform not supported")

            isLittleEndianEncoding= 2;
            if constexpr ( bitsof(BitBufferBase::TStorage) == 16 )
            {
                testWord= 0x2211;
                ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
                uint8_t* bytes= reinterpret_cast<uint8_t*>( &testWord );
                ALIB_WARNINGS_RESTORE
                if( *bytes == 0x11 && *(bytes+1) == 0x22)
                    isLittleEndianEncoding= 1;
            }

            else if constexpr ( bitsof(BitBufferBase::TStorage) == 32 )
            {
                testWord= 0x44332211;
                ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
                uint8_t* bytes= reinterpret_cast<uint8_t*>( &testWord );
                ALIB_WARNINGS_RESTORE
                if(    * bytes    == 0x11 && *(bytes+1) == 0x22
                    && *(bytes+2) == 0x33 && *(bytes+3) == 0x44 )
                    isLittleEndianEncoding= 1;
            }

            else if constexpr ( bitsof(BitBufferBase::TStorage) == 64 )
            {
                ALIB_WARNINGS_IGNORE_INTEGRAL_CONSTANT_OVERFLOW
                testWord= 0x8877665544332211u;
                ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
                uint8_t* bytes= reinterpret_cast<uint8_t*>( &testWord );
                ALIB_WARNINGS_RESTORE
                if(    * bytes    == 0x11 && *(bytes+1) == 0x22
                    && *(bytes+2) == 0x33 && *(bytes+3) == 0x44
                    && *(bytes+4) == 0x55 && *(bytes+5) == 0x66
                    && *(bytes+6) == 0x77 && *(bytes+7) == 0x88 )
                    isLittleEndianEncoding= 1;
                ALIB_WARNINGS_RESTORE

            }
        }

        return (isLittleEndianEncoding == 1);
    }
}
#endif

void  BitBufferBase::ToLittleEndianEncoding( const Index& startIndex, const Index& endIndex )
{
    ALIB_ASSERT_ERROR( startIndex.IsAligned(), "BITBUFFER",
                      "Given start index is not algined. The easiest way to get an aligned "
                      "index is to terminate the buffer." )

    if( IsLittleEndianEncoding())
        return;

    const size_t end=   endIndex.pos + (endIndex.bit != 0 );
    for (size_t  pos= startIndex.pos; pos < end; ++pos)
    {
        ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
        TStorage   word = data[pos];
        uint8_t*   bytes= reinterpret_cast<uint8_t*>( &data[pos] );
        ALIB_WARNINGS_RESTORE
                   bytes[0]= word & 0xFF;
        word>>= 8; bytes[1]= word & 0xFF;

        if constexpr ( bitsof(TStorage) > 16 )
        {
            word>>= 8; bytes[2]= word & 0xFF;
            word>>= 8; bytes[3]= word & 0xFF;
        }

        if constexpr ( bitsof(TStorage) > 32 )
        {
            word>>= 8; bytes[4]= word & 0xFF;
            word>>= 8; bytes[5]= word & 0xFF;
            word>>= 8; bytes[6]= word & 0xFF;
            word>>= 8; bytes[7]= word & 0xFF;
        }
    }
}

void  BitBufferBase::FromLittleEndianEncoding( const Index& startIndex, const Index& endIndex )
{
    if( IsLittleEndianEncoding())
        return;

    size_t end= endIndex.pos + (endIndex.bit != 0 );
    for (size_t pos= startIndex.pos; pos < end; ++pos)
    {
        TStorage word = 0;
        ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
        uint8_t* bytes= reinterpret_cast<uint8_t*>( &data[pos] );
        word|=  TStorage(bytes[0]);
        word|=  TStorage(bytes[1]) << 8;

        if constexpr ( bitsof(TStorage) > 16 )
        {
            word|=  TStorage(bytes[2]) << 16;
            word|=  TStorage(bytes[3]) << 24;
        }

        if constexpr ( bitsof(TStorage) > 32 )
        {
            ALIB_WARNINGS_IGNORE_INTEGER_OVERFLOW
            word|=  TStorage(bytes[4]) << 32;
            word|=  TStorage(bytes[5]) << 40;
            word|=  TStorage(bytes[6]) << 48;
            word|=  TStorage(bytes[7]) << 56;
            ALIB_WARNINGS_IGNORE_INTEGER_OVERFLOW
        }

        data[pos]= word;
        ALIB_WARNINGS_RESTORE
    }
}



// write 8-bit values
void BitWriter::writeUIntegral(uint8_t val)
{
    if( val < (1<<3) )
    {
        Write<4>( val << 1); // | 0
        return;
    }

    Write<9>( (val << 1) |1 ) ;
}

// write  16-bit values
void BitWriter::writeUIntegral(uint16_t val)
{
    if( val < (1<<8) )
    {
        Write<9>( val << 1);
        return;
    }

    Write<17>( ( val << 1 ) | 1 ) ;
}

// write  32-bit byte values
void BitWriter::writeUIntegral(uint32_t val)
{
         if( val < (1<< 8) )       { Write<10>(  val << 2       );  }
    else if( val < (1<<16) )       { Write<18>(( val << 2) | 1u );  }
    else if( val < (1<<24) )       { Write<26>(( val << 2) | 2u );  }
    else                           { Write< 2>(  3 );
                                     Write<32>( val );  }
}

// write  64-bit byte values
void BitWriter::writeUIntegral(uint64_t val)
{
         if( val < uint64_t(1) <<  8 )    { Write<11>(   int16_t  (val) << 3        );  }
    else if( val < uint64_t(1) << 16 )    { Write<19>( ( uint32_t (val) << 3 )  | 1u);  }
    else if( val < uint64_t(1) << 24 )    { Write<27>( ( uint32_t (val) << 3 )  | 2u);  }
    else if( val < uint64_t(1) << 32 )    { Write<35>( (           val  << 3 )  | 3u);  }
    else if( val < uint64_t(1) << 40 )    { Write<43>( (           val  << 3 )  | 4u);  }
    else if( val < uint64_t(1) << 48 )    { Write<51>( (           val  << 3 )  | 5u);  }
    else if( val < uint64_t(1) << 56 )    { Write<59>( (           val  << 3 )  | 6u);  }
    else                                  { Write< 3>(             7                );
                                            Write<64>(             val              );  }
}


// read 8-bit values
uint8_t BitReader::readUIntegral8()
{
    auto result= Read<4>();
    if( !(result & 1))
        return uint8_t(result >> 1);

    return    uint8_t(    result >> 1
                        | Read<5>() << 3  );
}

uint16_t BitReader::readUIntegral16()
{
    auto result= Read< 9>();
    if( !(result & 1))
        return uint16_t(result >> 1);

    return    uint16_t(   result    >> 1
                        | Read<8>() << 8 );
}

uint32_t BitReader::readUIntegral32()
{
    uint32_t result= Read< 10, uint32_t>();
    switch (result & 3)
    {
        case 0: return uint32_t(result >> 2);
        case 1: return uint32_t(result >> 2  | (Read<  8, uint32_t>() << 8 ) );
        case 2: return uint32_t(result >> 2  | (Read< 16, uint32_t>() << 8 ) );
       default: return uint32_t(result >> 2  | (Read< 24, uint32_t>() << 8 ) );
    }
}

uint64_t BitReader::readUIntegral64()
{
    uint64_t result= Read< 11, uint64_t>();
    switch (result & 7)
    {
        case 0: return uint64_t(result >> 3);
        case 1: return uint64_t(result >> 3 |  (Read<  8, uint64_t>() << 8 ));
        case 2: return uint64_t(result >> 3 |  (Read< 16, uint64_t>() << 8 ));
        case 3: return uint64_t(result >> 3 |  (Read< 24, uint64_t>() << 8 ));
        case 4: return uint64_t(result >> 3 |  (Read< 32, uint64_t>() << 8 ));
        case 5: return uint64_t(result >> 3 |  (Read< 40, uint64_t>() << 8 ));
        case 6: return uint64_t(result >> 3 |  (Read< 48, uint64_t>() << 8 ));
       default: return uint64_t(result >> 3 |  (Read< 56, uint64_t>() << 8 ));
    }
}

}} // namespace [alib::bitbuffer]

