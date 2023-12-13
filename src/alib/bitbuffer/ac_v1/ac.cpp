// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2023 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)
#   if !defined(HPP_AWORX_ALIB_BITBUFFER_AC_V1)
#      include "alib/bitbuffer/ac_v1/ac.hpp"
#   endif
#   if ALIB_TEXT && !defined(HPP_ALIB_TEXT_FORMATTER)
#      include "alib/text/formatter.hpp"
#   endif
#endif // !defined(ALIB_DOX)

namespace aworx { namespace lib { namespace bitbuffer { namespace ac_v1 {

#if ALIB_CPPVER < 17
constexpr int ArrayCompressor::QtyAlgorithms;
#endif

#if ALIB_TEXT

ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
void ArrayCompressor::Statistics::Print( AString& result, const String& headline, bool printTotals)
{
    auto fmt= Formatter::AcquireDefault( ALIB_CALLER_PRUNED );
#if ALIB_TIME
    Ticks::Duration tWrite;
    Ticks::Duration tRead;
#endif
    size_t          allSizes   = 0;
    size_t          winnerSizes= 0;
    ALIB_DBG(int check= 0;)

    result << "-------------------------------------------------------------------\n";
    fmt->Format( result, "{} ({} arrays compressed)\n", headline, qtyCompressions             );
    fmt->Format( result, A_WCHAR("#Algo       \u2205writeTime   \u2205readTime    \u2205 Size  \u2205 Sz-Won        wins\n"   ));
    result << "-------------------------------------------------------------------\n";
    for( int algoNo= 0; algoNo < ArrayCompressor::QtyAlgorithms ; ++algoNo )
    {
        String128 sizeWhenWon;
        if( sumUnCompressedWon[algoNo] )
            fmt->Format( sizeWhenWon, "{:f5.1}%",
                     double(100 * sumCompressedWon[algoNo]) / double(sumUnCompressedWon[algoNo]) );
        else
            sizeWhenWon= "-/-";

        String128 avgReadTime;
#if ALIB_TIME
        if( readTimes [algoNo].InNanoseconds() )
            fmt->Format( avgReadTime, "{:>11,}",
                     readTimes [algoNo].InNanoseconds()     / (qtyReads[algoNo] ? qtyReads[algoNo] : 1 ) );
        else
#endif
            avgReadTime= "-/-";

        fmt->Format( result,
                    "{:13} {:>8,} {:>11}    {:f5.1}%    {:>6} {:f3.0}% ({:4})\n"
                  , ToBitwiseEnumeration<Algorithm>( algoNo )
#if ALIB_TIME
                  , writeTimes[algoNo].InNanoseconds()     / qtyCompressions
#else
                  , "-/-"
#endif
                  , avgReadTime
                  , double(100 * sumCompressed[algoNo]) / double(sumUncompressed)
                  , sizeWhenWon
                  , double(100 * qtyWins      [algoNo]) / qtyCompressions
                  , qtyWins      [algoNo]                                                     );

        ALIB_DBG(check+= qtyWins[algoNo];)

#if ALIB_TIME
        tWrite     +=  writeTimes      [algoNo];
        tRead      +=  readTimes       [algoNo];
#endif
        allSizes   +=  sumCompressed   [algoNo];
        winnerSizes+=  sumCompressedWon[algoNo];
    }

    ALIB_ASSERT_ERROR( check==qtyCompressions, "BITBUFFER/AC",
                       "Error in ArrayCompressor::ExecutionStats: "
                       "#algo wins do not sum up to #compressions: ",
                       NString128() << check << "!=" << qtyCompressions )
    if( printTotals)
    {
        result << "        -----------------------------------------------------------\n";

        fmt->Format( result, "        Totals:{:>7,} {:>11,}    {:f5.1}%    {:f5.1}%\n"
#if ALIB_TIME
                           , tWrite.InNanoseconds()    / ( qtyCompressions * ArrayCompressor::QtyAlgorithms )
                           , tRead .InNanoseconds()    /   qtyCompressions
#else
                           , "-/-", "-/-"
#endif
                           , double(100 * allSizes   ) / double( ArrayCompressor::QtyAlgorithms * sumUncompressed )
                           , double(100 * winnerSizes) / double(sumUncompressed)     );
    }
    result << NewLine();

    fmt->Release();
}
ALIB_WARNINGS_RESTORE

#endif // #if ALIB_TEXT

}}}} // namespace [aworx::lib::bitbuffer::ac_v1]
