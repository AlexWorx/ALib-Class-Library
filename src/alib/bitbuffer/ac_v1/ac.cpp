// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !DOXYGEN
#   include "alib/bitbuffer/ac_v1/ac.hpp"
#   if ALIB_CAMP
#      include "alib/lang/format/formatter.hpp"
#   endif
#endif // !DOXYGEN

namespace alib {  namespace bitbuffer { namespace ac_v1 {

#if ALIB_CAMP

ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
void ArrayCompressor::Statistics::Print( AString& result, const String& headline, bool printTotals)
{
    ALIB_LOCK_RECURSIVE_WITH(Formatter::DefaultLock)
    Formatter& fmt= *Formatter::Default;
    fmt.Reset();
#if ALIB_TIME
    Ticks::Duration tWrite;
    Ticks::Duration tRead;
#endif
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
#if ALIB_TIME
        if( readTimes [algoNo].InNanoseconds() )
            fmt.Format( avgReadTime, "{:>11,}",
                     readTimes [algoNo].InNanoseconds()     / (ctdReads[algoNo] ? ctdReads[algoNo] : 1 ) );
        else
#endif
            avgReadTime= "-/-";

        fmt.Format( result,
                    "{:13} {:>8,} {:>11}    {:f5.1}%    {:>6} {:f3.0}% ({:4})\n"
                  , ToBitwiseEnumeration<Algorithm>( algoNo )
#if ALIB_TIME
                  , writeTimes[algoNo].InNanoseconds()     / ctdCompressions
#else
                  , "-/-"
#endif
                  , avgReadTime
                  , double(100 * sumCompressed[algoNo]) / double(sumUncompressed)
                  , sizeWhenWon
                  , double(100 * ctdWins      [algoNo]) / ctdCompressions
                  , ctdWins      [algoNo]                                                     );

        ALIB_DBG(check+= ctdWins[algoNo];)

#if ALIB_TIME
        tWrite     +=  writeTimes      [algoNo];
        tRead      +=  readTimes       [algoNo];
#endif
        allSizes   +=  sumCompressed   [algoNo];
        winnerSizes+=  sumCompressedWon[algoNo];
    }

    ALIB_ASSERT_ERROR( check==ctdCompressions, "BITBUFFER/AC",
                       "Error in ArrayCompressor::ExecutionStats: "
                       "#algo wins do not sum up to #compressions: ",
                       NString128() << check << "!=" << ctdCompressions )
    if( printTotals)
    {
        result << "        -----------------------------------------------------------\n";

        fmt.Format( result, "        Totals:{:>7,} {:>11,}    {:f5.1}%    {:f5.1}%\n"
#if ALIB_TIME
                           , tWrite.InNanoseconds()    / ( ctdCompressions * ArrayCompressor::NumberOfAlgorithms )
                           , tRead .InNanoseconds()    /   ctdCompressions
#else
                           , "-/-", "-/-"
#endif
                           , double(100 * allSizes   ) / double( ArrayCompressor::NumberOfAlgorithms * sumUncompressed )
                           , double(100 * winnerSizes) / double(sumUncompressed)     );
    }
    result.NewLine();
}
ALIB_WARNINGS_RESTORE

#endif // #if ALIB_CAMP

}}} // namespace [alib::bitbuffer::ac_v1]

