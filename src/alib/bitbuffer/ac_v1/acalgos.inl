/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_bitbuffer of the \aliblong.
 *
 * \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_BITBUFFER_AC_V1_ALGOS
#define HPP_ALIB_BITBUFFER_AC_V1_ALGOS 1

#if !defined(HPP_ALIB_MONOMEM_DETAIL_ARRAY_COMPRESSION_ALLOW)
#   error "ALib sources with ending '.inl' must not be included from outside."
#endif

#if !defined(HPP_ALIB_BITBUFFER_AC_V1_HUFFMAN)
#   include "alib/bitbuffer/ac_v1/huffman.hpp"
#endif

namespace alib {  namespace bitbuffer { namespace ac_v1 {


/**
 * Writes data compressed using class \alib{bitbuffer::ac_v1,HuffmanEncoder}.
 * @tparam TI    The integral type of the array to write.
 * @param  bw    The bit writer to use.
 * @param  data  The array to read the data from.
 */
template<typename TI>
void writeHuffman( BitWriter& bw, ArrayCompressor::Array<TI>& data )
{
    using TUI= typename std::make_unsigned<TI>::type;
    if( !data.length() )
        return;

    HuffmanEncoder he( bw );

    // count all occurrences of bytes
    for(size_t i= 0; i < data.length(); ++i)
    {
        TUI val= data.get( i );
        int bytes= sizeof(TUI);

        // using a loop here (instead of "if constexpr..." because hd.read() is inlined
        // (two times something similar below)
        COUNTNEXT:
        he.CountSymbol(uint8_t (val) );
        if( --bytes )
        {
            ALIB_WARNINGS_ALLOW_SHIFT_COUNT_OVERFLOW
            val>>= 8;
            ALIB_WARNINGS_RESTORE
            goto COUNTNEXT;
        }
    }
    // build huffman code
    he.Generate();

    // write out
    for(size_t i= 0; i < data.length(); ++i)
    {
        TUI val= data.get( i );
        ShiftOpRHS bits= bitsof(TUI) - 8;
        while( bits >= 0 )
        {
            ALIB_WARNINGS_ALLOW_SHIFT_COUNT_OVERFLOW
            he.Write(uint8_t (val >> bits ) );
            ALIB_WARNINGS_RESTORE

            bits-= 8;
        }
    }
}

/**
 * Reads data compressed using class \alib{bitbuffer::ac_v1,HuffmanDecoder}.
 * @tparam TI   The integral type of the array to read back.
 * @param  br   The bit reader to use.
 * @param  data The array to read the data to.
 */
template<typename TI>
void readHuffman( BitReader& br, ArrayCompressor::Array<TI>& data )
{
    using TUI= typename std::make_unsigned<TI>::type;
    if( !data.length() )
        return;
    HuffmanDecoder hd( br );
    hd.ReadTree();
    for(size_t i= 0; i < data.length(); ++i)
    {
        TUI val= 0;
        int bytes= sizeof(TUI);
        READNEXT:
        val|= hd.Read();
        if( --bytes )
        {
            ALIB_WARNINGS_ALLOW_SHIFT_COUNT_OVERFLOW
            val<<= 8;
            ALIB_WARNINGS_RESTORE
            goto READNEXT;
        }
        data.set(i, val);
    }
}


/**
 * Writes array data by simply using the mechanics provided with class \alib{bitbuffer,BitWriter},
 * which tries to shorten integrals on writing.
 * @tparam TI    The integral type of the array to write.
 * @param  bw    The bit writer to use.
 * @param  data  The array to read the data from.
 */
template<typename TI>
void writeUncompressed( BitWriter& bw, ArrayCompressor::Array<TI>& data )
{
    for(size_t i= 0; i < data.length(); ++i)
    {
        bw.Write( data.get( i ) );
    }
}

/**
 * Reads data compressed using simply the mechanics provided with class \alib{bitbuffer,BitWriter}
 * which tries to shorten integrals on writing.
 * @tparam TI   The integral type of the array to read back.
 * @param br    The bit reader to use.
 * @param data  The array to read the data to.
 */
template<typename TI>
void readUncompressed( BitReader& br, ArrayCompressor::Array<TI>& data )
{
    using TUI= typename std::make_unsigned<TI>::type;
    for(size_t i= 0; i < data.length(); ++i)
        data.set(i, br.Read<TUI>() );
}

/**
 * Writes array data by writing only the difference to the minimum found value.
 * @tparam TI    The integral type of the array to write.
 * @param  bw    The bit writer to use.
 * @param  data  The array to read the data from.
 */
template<typename TI>
void writeMinMax(BitWriter& bw, ArrayCompressor::Array<TI>& data )
{
    using TUI= typename std::make_unsigned<TI>::type;

    // calc min/max
    data.calcMinMax();

    // get bits needed
    int bitCnt= lang::MSB0(TUI(data.max - data.min));

    // write data
    bw.Write<lang::Log2OfSize<TUI>() + 1>( bitCnt );
    bw.Write( data.min );
    if( !bitCnt )
        return;

    for(size_t i= 0; i < data.length(); ++i )
        bw.Write( bitCnt, TUI( data.get(i) - data.min ) );
}

/**
 * Reads data compressed with method \alib{bitbuffer::ac_v1,writeMinMax}.
 * @tparam TI   The integral type of the array to read back.
 * @param br    The bit reader to use.
 * @param data  The array to read the data to.
 */
template<typename TI>
void readMinMax( BitReader& br, ArrayCompressor::Array<TI>& data )
{
    using TUI= typename std::make_unsigned<TI>::type;
    auto bitCnt=  br.Read<lang::Log2OfSize<TUI>() + 1>();
    TUI  min   =  br.Read<TUI>();

    if( !bitCnt )
    {
        for(size_t i= 0; i < data.length(); ++i)
            data.set(i, min );
        return;
    }
    for(size_t i= 0; i < data.length(); ++i)
        data.set(i, br.Read<TUI>( bitCnt ) + min );
}

/**
 * Writes array data assuming it is sparsely set.
 * @tparam TI    The integral type of the array to write.
 * @param  bw    The bit writer to use.
 * @param  data  The array to read the data from.
 */
template<typename TI>
void writeSparse(BitWriter& bw, ArrayCompressor::Array<TI>& data)
{
    if( !data.length() )
        return;
    using TUI= typename std::make_unsigned<TI>::type;
    TUI first= data.get(0);
    bw.Write(first);
    for(size_t i= 1; i < data.length(); ++i)
    {
        TUI second= data.get(i);
        if( second==first)
            bw.Write<1>( 1) ;
        else
        {
            bw.Write<1>( 0 );
            bw.Write(second);
        }
        first= second;
    }
}

/**
 * Reads data compressed with method \alib{bitbuffer::ac_v1,writeSparse}.
 * @tparam TI   The integral type of the array to read back.
 * @param br    The bit reader to use.
 * @param data  The array to read the data to.
 */
template<typename TI>
void readSparse( BitReader& br, ArrayCompressor::Array<TI>& data  )
{
    if( !data.length() )
        return;
    using TUI= typename std::make_unsigned<TI>::type;
    auto  prevVal= br.Read<TUI>();
    data.set(0, prevVal);
    for(size_t i= 1; i < data.length(); ++i)
        data.set(i, prevVal= br.Read<1>() ? prevVal
                                          : br.Read<TUI>()  );
}

/**
 * Writes array data assuming it is very sparsely set.
 * @tparam TI    The integral type of the array to write.
 * @param  bw    The bit writer to use.
 * @param  data  The array to read the data from.
 */
template<typename TI>
void writeVerySparse(BitWriter& bw, ArrayCompressor::Array<TI>& data )
{
    using TUI= typename std::make_unsigned<TI>::type;
   if( !data.length() )
        return;

    // calc min/max
    data.calcMinMax();

    // loop twice:
    //   1st pass: calc longest repetition
    //   2nd pass: write data
    auto  maxSegLen = (std::numeric_limits<size_t>::min)();
    auto  bitCntVal = 0;
    auto  bitCntRep = 0;
    for( int pass= 0 ; pass < 2 ; ++ pass )
    {
        size_t segStart= 0;
        TUI val    = data.get(0);
        TUI prevVal;
        do
        {
            // find next segment [segStart - segEnd[
            bool    sparse;
            size_t  segEnd= segStart + 1;
            prevVal= val;

            // only one value left?
            if( segEnd == data.length() )
            {
                sparse= false; // could also be set to true
            }
            else
            {
                val= data.get(segEnd);

                sparse= (prevVal == val);

                // find end
                while(      ( (prevVal == val) == sparse )
                       &&   ++segEnd < data.length())
                {
                    prevVal= val;
                    val    = data.get(segEnd);
                }
                if( !sparse )  // in diff mode, we have to go one back!
                    segEnd--;
            }

            // Pass 0: just note maximum segment length
            if( pass== 0)
                maxSegLen= (std::max)( maxSegLen, segEnd - segStart );
            // Pass 1: write segment
            else
            {
                // write
                if( sparse )
                {                                  // write an extra 0 to indicate sparse mode
                    bw.Write( bitCntRep +1,  (segEnd - segStart) << 1      );
                    bw.Write( bitCntVal   , TUI(prevVal - data.min ) );
                }
                else
                {                                  // write an extra 1 to indicate non-sparse mode
                    bw.Write( bitCntRep +1, ((segEnd - segStart) << 1) | 1 );
                    for( size_t j= segStart; j < segEnd; ++j)
                        bw.Write( bitCntVal, TUI(data.get(j) - data.min ) );
                }
            }

            // next segment
            segStart= segEnd;
        }
        while( segStart < data.length() );

        // pass 0 done: calc and write bits needed for value and repetition
        if( pass == 0)
        {
            bitCntVal= lang::MSB0( TUI(data.max - data.min) );
            bitCntRep= lang::MSB (maxSegLen);

            bw.Write<lang::Log2OfSize<uint32_t>()+1 + lang::Log2OfSize<TUI>()+1>(
                          bitCntRep
                        | bitCntVal << (lang::Log2OfSize<uint32_t>()+1) );
            bw.Write( data.min );
        }
    } // 2 pass loop
}

/**
 * Reads data compressed with method \alib{bitbuffer::ac_v1,writeVerySparse}.
 * @tparam TI   The integral type of the array to read back.
 * @param br    The bit reader to use.
 * @param data  The array to read the data to.
 */
template<typename TI>
void readVerySparse( BitReader& br, ArrayCompressor::Array<TI>& data )
{
    using TUI= typename std::make_unsigned<TI>::type;
    if( !data.length() )                                       return;

    auto bitCntRep =  br.Read<lang::Log2OfSize<uint32_t>()+1 + lang::Log2OfSize<TUI>()+1>();
    auto bitCntVal =  bitCntRep >> (lang::Log2OfSize<uint32_t>() + 1);
         bitCntRep&=  lang::LowerMask<lang::Log2OfSize<uint32_t>() + 1, int>();

    TUI  minVal   = br.Read<TUI>();

    size_t segStart= 0;
    while( segStart < data.length() )
    {
        size_t cntRep= br.Read<size_t>( bitCntRep + 1 );
        size_t segEnd= segStart + (cntRep >> 1);
        if( (cntRep & 1) == 0 ) // sparse
        {
            TUI val = br.Read<TUI>( bitCntVal ) + minVal;
            for( size_t i= segStart ; i < segEnd ; ++i )
                data.set(i, val);
        }
        else                    // different values
        {
            for( size_t i= segStart ; i < segEnd ; ++i )
                data.set(i, br.Read<TUI>( bitCntVal ) + minVal);
        }
        segStart= segEnd;
    }
}

/**
 * Writes array data incrementally.
 * @tparam TI    The integral type of the array to write.
 * @param  bw    The bit writer to use.
 * @param  data  The array to read the data from.
 */
template<typename TI>
void writeIncremental(BitWriter& bw, ArrayCompressor::Array<TI>& data )
{
    if( !data.length() )
        return;

    using TUI= typename std::make_unsigned<TI>::type;

    if( data.length() == 1 )
    {
        bw.Write(data.get(0));
        return;
    }

    // calc min diff bits needed to write diff
    data.calcMinMax();
    auto bitCntPos= lang::MSB0(TUI( data.maxInc - data.minInc));
    auto bitCntNeg= lang::MSB0(TUI( data.maxDec - data.minDec));

    // write data
    bw.Write<2 * (lang::Log2OfSize<TUI>() + 1)>(   bitCntPos
                                                 | bitCntNeg << (lang::Log2OfSize<TUI>() + 1)  );

    bw.Write( data.minInc );
    bw.Write( data.minDec );
    TUI first= data.get(0);
    bw.Write(first);
    for(size_t i= 1; i < data.length(); ++i)
    {
        // send one bit indicating a positive (1) or negative (0) difference
        // and then the difference as an unsigned value
        TUI second= data.get(i);
        if( second==first)
            bw.Write( true );
        else
        {
            bw.Write( false );
            bool posNeg= second >= first;
            bw.Write( posNeg );
            bw.Write(posNeg ? bitCntPos  : bitCntNeg,
                     posNeg ? TUI( second - first - data.minInc )
                            : TUI( first - second - data.minDec )   );
        }
        first= second;
    }
}

/**
 * Reads data compressed with \alib{bitbuffer::ac_v1,writeIncremental}.
 * @tparam TI   The integral type of the array to read back.
 * @param  br   The bit reader to use.
 * @param  data The array to read the data to.
 */
template<typename TI>
void readIncremental( BitReader& br, ArrayCompressor::Array<TI>& data)
{
    if( !data.length() )
        return;

    using TUI= typename std::make_unsigned<TI>::type;
    if( data.length() == 1 )
    {
        data.set(0, br.Read<TUI>() );
        return;
    }

    auto bitCntPos =  br.Read<2 * (lang::Log2OfSize<TUI>()+1)>();
    auto bitCntNeg =  bitCntPos >> (lang::Log2OfSize<TUI>()+1);
         bitCntPos&=  lang::LowerMask<(lang::Log2OfSize<TUI>()+1), int>();
    auto minDiffPos=  br.Read<TUI>();
    auto minDiffNeg=  br.Read<TUI>();
    auto prevVal   =  br.Read<TUI>();
    data.set(0, prevVal);
    for(size_t i= 1; i < data.length(); ++i)
    {
        TUI    val;
        if( br.Read<1>() )
            val= prevVal;
        else
        {
            bool   posNeg= br.Read<1>();
            TUI    diff  = br.Read<TUI>( posNeg ? bitCntPos : bitCntNeg );
                      val   = posNeg ? TUI( prevVal + minDiffPos + diff )
                                     : TUI( prevVal - minDiffNeg - diff );
        }
        data.set(i, prevVal= val);
    }
}



}}} // namespace [alib::bitbuffer::ac_v1]

#endif // HPP_ALIB_BITBUFFER_AC_V1_ALGOS
