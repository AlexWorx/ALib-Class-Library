// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib_precompile.hpp"
#if !defined(ALIB_C20_MODULES) || ((ALIB_C20_MODULES != 0) && (ALIB_C20_MODULES != 1))
#   error "Symbol ALIB_C20_MODULES has to be given to the compiler as either 0 or 1"
#endif
#if ALIB_C20_MODULES
    module;
#endif
// ======================================   Global Fragment   ======================================
#include "alib/bitbuffer/bitbuffer.prepro.hpp"

// ===========================================   Module   ==========================================
#if ALIB_C20_MODULES
    module ALib.BitBuffer;
#  if ALIB_FORMAT
    import   ALib.Format;
#  endif
#else
#   include "ALib.Format.H"
#   include "ALib.BitBuffer.H"
#endif
// ======================================   Implementation   =======================================
namespace alib {  namespace bitbuffer { namespace ac_v1 {

#if ALIB_FORMAT

void ArrayCompressor::Statistics::Print( AString& result, const String& headline, bool printTotals)
{
    ALIB_LOCK_RECURSIVE_WITH(Formatter::DefaultLock)
    Formatter& fmt= *Formatter::Default;
    fmt.Reset();

    Ticks::Duration tWrite;
    Ticks::Duration tRead;

    size_t          allSizes   = 0;
    size_t          winnerSizes= 0;
    ALIB_DBG(int check= 0;)

    result << "-------------------------------------------------------------------\n";
    fmt.Format( result, "{} ({} arrays compressed)\n", headline, ctdCompressions             );
    fmt.Format( result, A_WCHAR("#Algo       \u2205writeTime   \u2205readTime    \u2205 Size  \u2205 Sz-Won        wins\n"   ));
    result << "-------------------------------------------------------------------\n";
    for( int algoNo= 0; algoNo < ArrayCompressor::NumberOfAlgorithms ; ++algoNo )
    {
        String128 sizeWhenWon;
        if( sumUnCompressedWon[algoNo] )
            fmt.Format( sizeWhenWon, "{:f5.1}%",
                     double(100 * sumCompressedWon[algoNo]) / double(sumUnCompressedWon[algoNo]) );
        else
            sizeWhenWon= "-/-";

        String128 avgReadTime;
        if( readTimes [algoNo].InNanoseconds() )
            fmt.Format( avgReadTime, "{:>11,}",
                     readTimes [algoNo].InNanoseconds()     / (ctdReads[algoNo] ? ctdReads[algoNo] : 1 ) );
        else
            avgReadTime= "-/-";

        fmt.Format( result,
                    "{:13} {:>8,} {:>11}    {:f5.1}%    {:>6} {:f3.0}% ({:4})\n"
                  , ToBitwiseEnumeration<Algorithm>( algoNo )
                  , writeTimes[algoNo].InNanoseconds()     / ctdCompressions
                  , avgReadTime
                  , double(100 * sumCompressed[algoNo]) / double(sumUncompressed)
                  , sizeWhenWon
                  , double(100 * ctdWins      [algoNo]) / ctdCompressions
                  , ctdWins      [algoNo]                                                     );

        ALIB_DBG(check+= ctdWins[algoNo];)

        tWrite     +=  writeTimes      [algoNo];
        tRead      +=  readTimes       [algoNo];
        allSizes   +=  sumCompressed   [algoNo];
        winnerSizes+=  sumCompressedWon[algoNo];
    }

    ALIB_ASSERT_ERROR( check==ctdCompressions, "BITBUFFER/AC",
     "Error in ArrayCompressor::ExecutionStats: #algo wins do not sum up to #compressions: {} != ",
     check, ctdCompressions )
    if( printTotals)
    {
        result << "        -----------------------------------------------------------\n";

        fmt.Format( result, "        Totals:{:>7,} {:>11,}    {:f5.1}%    {:f5.1}%\n"
                           , tWrite.InNanoseconds()    / ( ctdCompressions * ArrayCompressor::NumberOfAlgorithms )
                           , tRead .InNanoseconds()    /   ctdCompressions
                           , double(100 * allSizes   ) / double( ArrayCompressor::NumberOfAlgorithms * sumUncompressed )
                           , double(100 * winnerSizes) / double(sumUncompressed)     );
    }
    result.NewLine();
}

#endif // #if ALIB_CAMP

}}} // namespace [alib::bitbuffer::ac_v1]

