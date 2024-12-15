//==================================================================================================
/// \file
/// This header file is part of module \alib_bitbuffer of the \aliblong.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_BITBUFFER_AC_V1
#define HPP_ALIB_BITBUFFER_AC_V1
#pragma once
#include "alib/alib.hpp"
ALIB_ASSERT_MODULE(BITBUFFER)
ALIB_ASSERT_MODULE(ENUMS)
#include "alib/bitbuffer/bitbuffer.hpp"
#include "alib/enums/underlyingintegral.hpp"
#include "alib/enums/bitwise_iterable_conversion.hpp"
#include "alib/enums/records.hpp"
#if ALIB_TIME
#  include "alib/time/ticks.hpp"
#endif

namespace alib {  namespace bitbuffer {

/// This sub-namespace of \alib_bitbuffer provides algorithms to compress integral arrays.
/// Classes \alib{bitbuffer;ac_v1::ArrayCompressor} and  \alib{bitbuffer;ac_v1::HuffmanEncoder} implement
/// some data formats defined on bit-streams, which, in future versions of \alib, may be changed.
/// With such future changes, theses classes will be published in a next enumerated namespace,
/// parallel to this one.<br>
/// This approach will allow software to read existing datasets from files (explicitly using
/// older versions of the classes by selecting them via the namespace) and convert the data to the
/// new binary format.
///
/// Type aliases \ref alib::ArrayCompressor, \ref alib::HuffmanEncoder and
/// \ref alib::HuffmanDecoder will always refer to the latest version.
///
/// \note Currently, there is no other version of the class available.
namespace ac_v1 {


/// This class provides several algorithms to compress arrays of integral data and encode them
/// in \alib{bitbuffer;BitBuffer} objects.
/// Besides a standard \https{Huffman compression,en.wikipedia.org/wiki/Huffman_coding}, different
/// simple approaches are "tested" and the best compression algorithm is then chosen.
/// The general assumption of the approaches (besides the <em>Huffman coding</em>) is that the
/// data contains "signal data", which is either
/// - sparsely filled,
/// - has incremental values, or
/// - has just values of a certain smaller range.
/// Also, combinations of these attributes are matched. Such data is often found in real-world
/// applications and may be compressed much better than the generic \e Huffman approach may achieve.
class ArrayCompressor
{
    public:
        static constexpr int NumberOfAlgorithms= 6;  ///< The number of algorithms implemented.

        /// Helper-class that allows access the array data. The design goal for introducing
        /// this class (instead of providing array references in the interface methods) is
        /// to allow a minimum of flexibility in respect to the data provision, while not using
        /// callback functions (or virtual methods) to access each single array element.<p>
        /// The approach implemented here, allows the array value to be a single attribute
        /// residing in an array of structs.
        /// For this, besides a base pointer to the first value and the length of the array,
        /// the distance between two values within the array of structs (or classes) has
        /// to be given.
        ///
        /// By nature, to do this, basic pointer manipulation is needed, which imposes the need
        /// using <c>char*</c> values internally, which are cast back to the source type
        /// with setters/getters.<br>
        /// Consequently, templated constructors are given, which accept array types to
        /// restrict such pointer conversion within the type.
        ///
        /// \note
        ///   In the case an application uses a more complex data scheme for storing array
        ///   data to be compressed, which are not accessible with this simple mechanism,
        ///   such data has to be written into temporary arrays before compression.
        ///
        /// Besides this, this accessor type, provides a transparent inline conversion of
        /// signed integer values to its unsigned counterparts by performing
        /// <em>"zig zag encoding"</em>.
        ///
        /// \tparam TIntegral The integral array type.
        template <typename TIntegral>
        class Array
        {
            public:
            /// The unsigned version of template type \c TIntegral.
            using TUI = typename std::make_unsigned<TIntegral>::type;

            private:
                char*   firstVal;   ///< Pointer to the first array value
                size_t  distance;   ///< The distance in memory between two array values
                size_t  len;        ///< The length of the array

            public:
                TUI     min;        ///< Maximum value (when zig-zag encoded)
                TUI     max;        ///< Maximum value (when zig-zag encoded)
                TUI     maxInc;     ///< Maximum increase between two adjacent values.
                TUI     maxDec;     ///< Maximum decrease between two adjacent values.
                TUI     minInc;     ///< Minimum increase between two adjacent values.
                TUI     minDec;     ///< Minimum decrease between two adjacent values.

                #if !DOXYGEN && ALIB_DEBUG_ARRAY_COMPRESSION
                    bool dbgIsCheckRead= false;
                #endif

            public:
                /// This constructor may (and must only) be used when the data is stored in simple
                /// arrays, hence when the data is not nested in an array of structs.
                /// @param arrayStart    Pointer to the first value of the array.
                /// @param length        The length of the array
                Array( const TIntegral* arrayStart, size_t length )
                : len(length)
                {
                    firstVal= const_cast<char*>(
                                reinterpret_cast<const char*>(arrayStart) );
                    distance= sizeof(TUI);

                    // set min > max to indicate that this was not calculated, yet.
                    min= (std::numeric_limits<TUI>::max)();
                    max= (std::numeric_limits<TUI>::min)();
                }

                /// This constructor takes the first and the second array value as pointers.
                /// The second is used to "assume" (!) the distance in memory between each value.
                /// \attention If the assumption of such basic memory layout is wrong,
                ///            array values have to be copied to a temporary memory that satisfies
                ///            this rule.
                /// @param firstValue    Pointer to the first value of the array.
                /// @param secondValue   Pointer to the second value of the array
                /// @param length        The length of the array
                Array( const TIntegral& firstValue, const TIntegral& secondValue, size_t length )
                : len(length)
                {
                    firstVal= const_cast<char*>(
                                reinterpret_cast<const char*>(&firstValue) );
                    distance= size_t(   reinterpret_cast<const char*>(&secondValue)
                                      - reinterpret_cast<const char*>(&firstValue)) ;

                    // set min > max to indicate that this was not calculated, yet.
                    min= (std::numeric_limits<TUI>::max)();
                    max= (std::numeric_limits<TUI>::min)();
                }

            public:
                /// Returns the constant array length, given on construction.
                /// @return The length of the array to compress/decompress
                ALIB_FORCE_INLINE     size_t  length()                                         const
                {
                    return len;
                }

                /// Returns the value at the given index as an unsigned integer value (for arrays of
                /// signed values, zig-zag encoding is performed)
                /// @param idx The index of the value in the array to retrieve.
                /// @return An unsigned representation of the value at the given \p index.
                ALIB_FORCE_INLINE     TUI  get(size_t idx)                                 const
                {
                    ALIB_ASSERT_ERROR( idx < len, "BITBUFFER/AC", "Array compression: Index out of bounds" )

                    ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
                    if constexpr ( std::is_unsigned<TIntegral>::value )
                        return *(reinterpret_cast<TUI*>(firstVal + idx * distance));
                    else
                    {
                        TIntegral val= *(reinterpret_cast<TIntegral*>(firstVal + idx * distance));
                        return val >= 0 ? TUI(    val      << 1      )
                                        : TUI( ((-val - 1) << 1) | 1 );
                    }
                    ALIB_WARNINGS_RESTORE
                }

                /// Writes the given value at the given idx as an unsigned integer value (for arrays
                /// of signed values, zig-zag encoding is performed)
                /// @param idx   The index of the value in the array to set.
                /// @param value The value to set.
                ALIB_FORCE_INLINE
                void    set(size_t idx, TUI value)
                {
                    ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
                    #if ALIB_DEBUG_ARRAY_COMPRESSION
                    TUI oldVal= 0;
                    if( dbgIsCheckRead )
                        oldVal= *reinterpret_cast<TUI*>(firstVal + idx * distance );
                    #endif

                    ALIB_ASSERT_ERROR( idx < len, "BITBUFFER/AC", "Array compression: Index out of bounds" )

                    if constexpr ( std::is_unsigned<TIntegral>::value )
                        *(reinterpret_cast<TIntegral*>(firstVal + idx * distance))= TIntegral(value);
                    else
                    {
                        *(reinterpret_cast<TIntegral*>(firstVal + idx * distance)) =
                            (value & 1) == 0 ?    TIntegral( value >> 1)
                                             : -( TIntegral( value >> 1) + 1 );
                    }

                    #if ALIB_DEBUG_ARRAY_COMPRESSION
                    if( dbgIsCheckRead )
                        ALIB_ASSERT_ERROR( oldVal== *(reinterpret_cast<TUI*>(firstVal + idx * distance )),
                                           "BITBUFFER/AC", "Error reading back compressed array data" )
                    #endif
                    ALIB_WARNINGS_RESTORE
                }

                /// Loops over the data and stores minimum and maximum values as well as minimum
                /// and maximum value distances.
                void calcMinMax()
                {
                    // already done?
                    if( max >= min )
                        return;

                    maxInc= 0;
                    maxDec= 0;

                    // zero-length array
                    if( !len )
                    {
                        minInc= 0;
                        minDec= 0;
                        return;
                    }
                    maxInc= 0;
                    maxDec= 0;
                    minInc= (std::numeric_limits<TUI>::max)();
                    minDec= (std::numeric_limits<TUI>::max)();

                    TUI prevVal= get(0);
                    min= (std::min)( min, prevVal );
                    max= (std::max)( max, prevVal );

                    for(size_t i= 1; i < len; ++i)
                    {
                        TUI val= get(i);
                        min= (std::min)( min, val );
                        max= (std::max)( max, val );

                        if(val >= prevVal)
                        {
                            minInc= (std::min)( minInc, TUI( val - prevVal) );
                            maxInc= (std::max)( maxInc, TUI( val - prevVal) );
                        }
                        else
                        {
                            minDec= (std::min)( minDec, TUI( prevVal - val) );
                            maxDec= (std::max)( maxDec, TUI( prevVal - val) );
                        }

                        prevVal= val;
                    }

                    // correct minDec, if no negative distance was found.
                    if( maxDec == 0 )
                        minDec =  0;
                }
        }; // internal class Array


        /// This enumeration denotes the different algorithms provided for compression.
        /// This enum is defined to be \ref alib_enums_arithmetic "bitwise".
        /// \note
        ///   With the inclusion of at least one
        ///   \ref alib_manual_modules_dependencies "\"ALib Camp\"" in the \alibdist, this
        ///   enum is furthermore \ref alib_basecamp_resources_details_data "resourced" and values
        ///   are appendable to class \alib{strings;TAString<TChar>;AString} and logable with \alox.
        enum class Algorithm
        {
            /// No compression method selected.
            NONE               =   0,

            /// All compression methods selected.
            ALL                =   (1<< NumberOfAlgorithms) - 1,

            /// Stores the data as integer values, which includes a simple sort of possible
            ///  compression as documented with
            ///  \alib{bitbuffer;BitWriter::Write<TIntegral>(TIntegral)}.
            Uncompressed       =   1,

            /// Stores the differences between the minimum and maximum value found.
            MinMax             =   2,

            /// Writes '1' if next value is equal to previous, '0' plus next value otherwise.
            Sparse             =   4,

            /// Writes the number of following equal or non equal values.
            VerySparse         =   8,

            /// Only distances of the values are written.
            Incremental        =  16,

            /// Huffman encoding (byte based).
            Huffman            =  32,

            END_OF_ENUM        = 64,
        };

        /// Statistic struct to collect information about the performance of different array
        /// compression approaches.
        /// \note While other \alib module provide similar information only in debug compilations of
        ///       the library, the optional mechanics to collect statistics on array compression
        ///       (based on this struct) are likewise included in the release version.
        struct Statistics
        {
            #if ALIB_TIME
            /// The overall compression time of each algorithm.
            Ticks::Duration writeTimes        [NumberOfAlgorithms]                              ={};

            /// The overall decompression time of each algorithm.
            Ticks::Duration readTimes         [NumberOfAlgorithms]                              ={};

            /// The number of measured decompression runs of each algorithm.
            int             ctdReads          [NumberOfAlgorithms]                              ={};
            #endif

            /// A counter for the number of times each algorithm was chosen for compression by
            /// providing the shortest encoding. The values sum up to field #ctdCompressions.
            int             ctdWins           [NumberOfAlgorithms]                              ={};

            /// For each algorithm, the sum of resulting bytes of all compressions performed.
            size_t          sumCompressed     [NumberOfAlgorithms]                              ={};

            /// For each algorithm, the sum of resulting bytes of those compressions where the
            /// corresponding algorithm performed best. The values sum up to the overall
            /// effective compression length.
            size_t          sumCompressedWon  [NumberOfAlgorithms]                              ={};

            /// For each algorithm, the sum of original bytes of those compressions where the
            /// corresponding algorithm performed best. The values sum up to the overall
            /// uncompressed size given with sumUncompressed.
            size_t          sumUnCompressedWon[NumberOfAlgorithms]                              ={};

            /// The overall given array data to compress.
            size_t          sumUncompressed                                                      =0;

            /// The number of executed compressions.
            int             ctdCompressions                                                      =0;


            /// Adds another statistic object to this one.
            /// @param other The statistics to add to this one.
            /// @return A reference to this object.
            Statistics& operator += (const Statistics& other)
            {
                for( int algoNo= 0; algoNo < NumberOfAlgorithms ; ++algoNo )
                {
                    ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
                    #if ALIB_TIME
                    writeTimes        [algoNo]+= other.writeTimes        [algoNo];
                    readTimes         [algoNo]+= other.readTimes         [algoNo];
                    ctdReads          [algoNo]+= other.ctdReads          [algoNo];
                    #endif
                    ctdWins           [algoNo]+= other.ctdWins           [algoNo];
                    sumCompressed     [algoNo]+= other.sumCompressed     [algoNo];
                    sumCompressedWon  [algoNo]+= other.sumCompressedWon  [algoNo];
                    sumUnCompressedWon[algoNo]+= other.sumUnCompressedWon[algoNo];
                    ALIB_WARNINGS_RESTORE
                }
                sumUncompressed+= other.sumUncompressed;
                ctdCompressions+= other.ctdCompressions;
                return *this;
            }


            #if ALIB_CAMP
                /// Writes compression statistics to the given string buffer.
                /// \par Availability
                ///   This method is included only if module \alib_basecamp is included in the
                ///   \alibdist.
                ///
                /// @param result      A string buffer to collect the dump results.
                /// @param headline    A headline to integrate into the result table.
                /// @param printTotals Determines if a summary line with summed up values should be
                ///                    written.
                ALIB_API
                void Print( AString& result, const String& headline, bool printTotals);
            #endif
        };


        /// Deleted default constructor (this class cannot be created)
        ArrayCompressor() = delete;


// clang 14.0.6 (as of today 221216) falsely reports:
// "warning: '@tparam' command used in a comment that is not attached to a template declaration
#if !DOXYGEN
    ALIB_WARNINGS_IGNORE_DOCS
#endif
        /// Compresses the given array and writes the data into the given bit writer.
        /// Each algorithm included in parameter \p algorithmsToTry are executed and finally that
        /// one with the best compression result is chosen. Before the usage data, some bits that
        /// determine the chosen algorithm are written, to enable method #Decompress
        /// to deserialize the data.
        ///
        /// To gain efficiency, the number of probed algorithms can be narrowed by setting a
        /// corresponding mask in \p algorithmsToTry. However, in many use case scenarios, the
        /// execution time is a less critical design factor than the compression factor reached.
        /// The decompression speed is solely dependent on the algorithm finally chosen, not on
        /// the number of algorithms tested on compression.
        ///
        /// \attention
        ///   If only one algorithm is specified in parameter \p algorithmsToTry, then no
        ///   meta-information about the algorithm chosen is written. Consequently, when reading
        ///   back the data using #Decompress, the same single algorithm has to be provided.
        ///
        /// @tparam TValue    The integral type of array data to compress.
        /// @param  bitWriter A bit writer to compress the data to.
        /// @param  data      The array to compress.
        /// @param  algorithmsToTry The set of algorithms to be tried on compression for best
        ///                         efficiency.<br>
        ///                         Defaults to \ref Algorithm::ALL.
        /// @param statistics Pointer a struct to collect statistics for the efficiency of array
        ///                   compression related to given user data. If set, methods #Compress and
        ///                   #Decompress will measure execution performance and compression rates
        ///                   for each algorithm. With that, a software may collect information about
        ///                   which algorithm is most efficient for typical datasets found and
        ///                   a programmer may, based on such heuristics decide to exclude certain
        ///                   algorithms not efficient in a use case.
        /// @return A pair of value containing the resulting size in bits and the algorithm
        ///         chosen.

        template <typename TValue>
        static
        std::pair<size_t, Algorithm>    Compress  ( BitWriter&      bitWriter,
                                                    Array<TValue>&  data,
                                                    Algorithm       algorithmsToTry = Algorithm::ALL,
                                                    Statistics*     statistics      = nullptr
                                                    );


        /// Decompresses an integral array from the given bit reader, which previously was encoded
        /// with methods #Compress.
        /// The integral data type has to be the same as with encoding.
        /// \attention
        ///   If compression was performed with specifying only one algorithm in parameter
        ///   \p algorithmsToTry, then the same algorithm has to be exclusively set on decompression,
        ///   because in this case no meta-information about the compression algorithm is stored
        ///   in the bit stream.
        /// @tparam TValue     The integral type of array data to decompress.
        /// @param  bitReader  A bit reader to read the data from.
        /// @param  data       The array to decompress data to.
        /// @param  algorithm  The algorithm to use for read back. Must only be given, in case
        ///                    that compression was performed using a single algorithm of choice.
        ///                    Defaults to \ref Algorithm::ALL.
        /// @param  statistics An optional statistics record to store the measured de-compression
        ///                    time. See method #Compress for more information.
        template <typename TValue>
        static
        void                            Decompress( BitReader&      bitReader,
                                                    Array<TValue>&  data,
                                                    Algorithm       algorithm    = Algorithm::ALL,
                                                    Statistics*     statistics   = nullptr);

ALIB_WARNINGS_RESTORE

}; // class ArrayCompressor

}}} // namespace [alib::bitbuffer::ac_v1]

ALIB_ENUMS_ASSIGN_RECORD( alib::bitbuffer::ac_v1::ArrayCompressor::Algorithm, alib::enums::ERSerializable )
ALIB_ENUMS_MAKE_BITWISE(  alib::bitbuffer::ac_v1::ArrayCompressor::Algorithm )
ALIB_ENUMS_MAKE_ITERABLE( alib::bitbuffer::ac_v1::ArrayCompressor::Algorithm,
                          alib::bitbuffer::ac_v1::ArrayCompressor::Algorithm::END_OF_ENUM )

#define HPP_ALIB_MONOMEM_DETAIL_ARRAY_COMPRESSION_ALLOW
#pragma once
#   include "alib/bitbuffer/ac_v1/acalgos.inl"
#undef  HPP_ALIB_MONOMEM_DETAIL_ARRAY_COMPRESSION_ALLOW



namespace alib {  namespace bitbuffer { namespace ac_v1 {

#include "alib/lang/callerinfo_functions.hpp"
template <typename TValue>
std::pair<size_t, ArrayCompressor::Algorithm> ArrayCompressor::Compress(
                                                    BitWriter&      bw,
                                                    Array<TValue>&  data,
                                                    Algorithm       algorithmsToTry,
                                                    Statistics*     statistics          )
{
    ALIB_ASSERT_ERROR( data.length() * bitsof(TValue) < bw.RemainingSize(), "BITBUFFER/AC",
                       "BitBuffer is smaller than uncompressed data."
                       " No buffer overflow checks are performed during compression." )
    ALIB_ASSERT_WARNING( data.length() * bitsof(TValue) * 2 < bw.RemainingSize(), "BITBUFFER/AC",
                       "BitBuffer remaining size should be twice as large as uncompressed data."
                       " No buffer overflow checks are performed during compression." )

    auto initialBufferState= bw.GetIndex();
    auto initialBufferFill = bw.Usage();
    int  multipleAlgorithms= CountElements(algorithmsToTry) > 1;
    ALIB_ASSERT_ERROR(UnderlyingIntegral(algorithmsToTry) != 0, "BITBUFFER/AC", "No algorithms to check given" )

    auto bestAlgo  = ArrayCompressor::Algorithm::NONE;
    auto bestAlgoNo= (std::numeric_limits<int>::max)();
    auto lastAlgo  = ArrayCompressor::Algorithm::NONE;
    auto leastBits = (std::numeric_limits<size_t>::max)();

    bool    isFirstAlgo = true;
    int algoNo= -1;
    for( auto algo :  enums::EnumIterator<ArrayCompressor::Algorithm>() )
    {
        algoNo++;

        // included in write (test)?
        if ( !HasBits(algorithmsToTry, algo ) )
            continue;
        if(!isFirstAlgo)
        {
            bw.Reset(initialBufferState);
        }
        isFirstAlgo= false;

        // write algo number
        if( multipleAlgorithms )
            bw.Write<3>( algoNo );
        #if ALIB_TIME
            Ticks tm;
        #endif
        ALIB_WARNINGS_ALLOW_SPARSE_ENUM_SWITCH
        switch( lastAlgo= algo )
        {
            case Algorithm::Uncompressed: writeUncompressed(bw, data); break;
            case Algorithm::MinMax:       writeMinMax      (bw, data); break;
            case Algorithm::Sparse:       writeSparse      (bw, data); break;
            case Algorithm::VerySparse:   writeVerySparse  (bw, data); break;
            case Algorithm::Incremental:  writeIncremental (bw, data); break;
            case Algorithm::Huffman:      writeHuffman     (bw, data); break;
            default:  ALIB_ERROR("BITBUFFER/AC", "Internal error: Unknown compression algorithm number read")   break;
        }
        ALIB_WARNINGS_RESTORE
        auto bufferFill= bw.Usage();

        if( statistics )
        {
            ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
            #if ALIB_TIME
                statistics->writeTimes   [algoNo]+= tm.Age();
            #endif
            statistics->sumCompressed[algoNo]+= (bufferFill - initialBufferFill)/8;
            ALIB_WARNINGS_RESTORE
        }

        ALIB_ASSERT_ERROR( bufferFill > initialBufferFill, "BITBUFFER/AC", "Array compression: Nothing written")
        if( leastBits > bufferFill - initialBufferFill )
        {
            leastBits= bufferFill - initialBufferFill;
            bestAlgo  = algo;
            bestAlgoNo= algoNo;
        }

        // DEBUG-Test: Read back values right away and check for equal data
        #if ALIB_DEBUG_ARRAY_COMPRESSION
        {
            bw.Flush();
            BitReader br(bw.GetBuffer(), initialBufferState);
            if( multipleAlgorithms )
            {
                auto readBackAlgo=  Algorithm( 1 << br.Read<3>() );
                ALIB_ASSERT_ERROR( readBackAlgo == algo, "BITBUFFER/AC",
                                   "Wrong algorithm id was read back. This must never happen." )
            }

            data.dbgIsCheckRead= true;

                ALIB_WARNINGS_ALLOW_SPARSE_ENUM_SWITCH
                switch( algo )
                {
                    case Algorithm::Uncompressed:   readUncompressed(br, data ); break;
                    case Algorithm::MinMax:         readMinMax      (br, data ); break;
                    case Algorithm::Sparse:         readSparse      (br, data ); break;
                    case Algorithm::VerySparse:     readVerySparse  (br, data ); break;
                    case Algorithm::Incremental:    readIncremental (br, data ); break;
                    case Algorithm::Huffman:        readHuffman     (br, data ); break;
                    default:     ALIB_ERROR("Internal error: Unknown compression algorithm number read")   break;
                }
                ALIB_WARNINGS_RESTORE

            data.dbgIsCheckRead= false;
        }
        #endif

        if( !multipleAlgorithms )
            break;
    } // loop over algorithms

    if( statistics )
    {
        ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
        statistics->ctdCompressions++;
        statistics->sumUncompressed+= data.length() * sizeof(TValue);
        statistics->ctdWins           [bestAlgoNo]++;
        statistics->sumCompressedWon  [bestAlgoNo]+= leastBits/8;
        statistics->sumUnCompressedWon[bestAlgoNo]+= data.length() * sizeof(TValue);
        ALIB_WARNINGS_RESTORE
    }


    // write with best algorithm found (if this was not the last one anyhow)
    if( multipleAlgorithms && (bestAlgo != lastAlgo) )
    {
        bw.Reset( initialBufferState );
        bw.Write<3>( bestAlgoNo );
        ALIB_WARNINGS_ALLOW_SPARSE_ENUM_SWITCH
        switch( bestAlgo )
        {
            case Algorithm::Uncompressed: writeUncompressed(bw, data); break;
            case Algorithm::MinMax:       writeMinMax      (bw, data); break;
            case Algorithm::Sparse:       writeSparse      (bw, data); break;
            case Algorithm::VerySparse:   writeVerySparse  (bw, data); break;
            case Algorithm::Incremental:  writeIncremental (bw, data); break;
            case Algorithm::Huffman:      writeHuffman     (bw, data); break;
            default:  ALIB_ERROR("BITBUFFER/AC", "Internal error: Unknown compression algorithm number read")   break;
        }
        ALIB_WARNINGS_RESTORE
    }

    bw.Flush();
    return std::make_pair( leastBits, bestAlgo );
}

template <typename TValue>
void ArrayCompressor::Decompress(   BitReader&      br,
                                    Array<TValue>&  data,
                                    Algorithm       algorithmsToTry,
                                    Statistics*     statistics          )
{
    ALIB_ASSERT_ERROR(UnderlyingIntegral(algorithmsToTry) != 0, "BITBUFFER/AC",
                      "No algorithms to check given" )
    bool multipleAlgorithms= CountElements(algorithmsToTry) > 1;

#if ALIB_TIME
    Ticks tm;
#endif
    auto algo=  multipleAlgorithms  ?  Algorithm( 1 << br.Read<3>() )
                                    :  algorithmsToTry;
    ALIB_WARNINGS_ALLOW_SPARSE_ENUM_SWITCH
    switch( algo )
    {
        case Algorithm::Uncompressed:   readUncompressed(br, data ); break;
        case Algorithm::MinMax:         readMinMax      (br, data ); break;
        case Algorithm::Sparse:         readSparse      (br, data ); break;
        case Algorithm::VerySparse:     readVerySparse  (br, data ); break;
        case Algorithm::Incremental:    readIncremental (br, data ); break;
        case Algorithm::Huffman:        readHuffman     (br, data ); break;
        default:     ALIB_ERROR("Internal error: Unknown compression algorithm number read")   break;
    }
    ALIB_WARNINGS_RESTORE

    if( statistics )
    {
        ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
        #if ALIB_TIME
            auto algoNo= ToSequentialEnumeration( algo );
            statistics->readTimes[algoNo]+= tm.Age();
            statistics->ctdReads [algoNo]++;
        #endif
        ALIB_WARNINGS_RESTORE
    }
}

#include "alib/lang/callerinfo_methods.hpp"
    
}}} // namespace [alib::bitbuffer::ac_v1]

#endif // HPP_ALIB_BITBUFFER_AC_V1

