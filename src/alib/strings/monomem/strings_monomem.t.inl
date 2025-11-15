//==================================================================================================
/// \file
/// This "template-definition-header" file is part of module \alib_monomem of the \aliblong.
/// It may be used to instantiate custom versions of \alib{monomem;TPoolAllocator}, working with
/// a different \ref alib_contmono_chaining "chained" allocator.
/// @see Manual section \ref alib_manual_appendix_t_inl_files about the nature of ".t.inl"-files.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#if ALIB_STRINGS && ALIB_DEBUG && !DOXYGEN
namespace alib::monomem {
#include "ALib.Lang.CIFunctions.H"

template<typename TAllocator>
NAString DbgDumpStatistics(TMonoAllocator<TAllocator>& ma) {
    Statistics stats;
    ma.GetStatistics(stats);

    NAString result;
    NNumberFormat nf;
    nf.Flags=  NumberFormatFlags(uint8_t(nf.Flags) | uint8_t(NumberFormatFlags::WriteGroupChars));
    nf.FractionalPartWidth=2;
    result << "MonoAllocator Usage Statistics:"  << NNEW_LINE;

    result << "    Allocation Size:     "   << NDec( stats.AllocSize                              , &nf ) << NNEW_LINE;
    result << "    Current buffer free: "   << NDec( stats.CurrentBufferFree                      , &nf ) << NNEW_LINE;
    result << "    Current buffer used: "   << NDec( ma.DbgGetBuffer()->act
                                                       - reinterpret_cast<char*>(ma.DbgGetBuffer())  , &nf ) << NNEW_LINE;
    result << "    Current buffer size: "   << NDec( stats.CurrentBufferSize                      , &nf ) << NNEW_LINE;
    result << "    Next buffer size:    "   << NDec( stats.NextBufferSize                         , &nf ) << NNEW_LINE;
    result << "    Buffers in use:      "   << NDec( stats.QtyBuffers                             , &nf ) << NNEW_LINE;
    result << "    Buffers allocated:   "   << NDec( stats.QtyBuffers + stats.QtyRecyclables      , &nf ) << NNEW_LINE;
    result << "    Heap size in use:    "   << NDec( stats.HeapSize                               , &nf ) << NNEW_LINE;
    result << "    Heap size allocated: "   << NDec( stats.HeapSize + stats.HeapSizeRecycled      , &nf ) << NNEW_LINE;
    result << "    Unused buffer bytes: "   << NDec( stats.BufferWaste                            , &nf );
          if( stats.QtyBuffers )  result  << "  (per buffer: " << NDec( stats.QtyBuffers > 1 ? (stats.BufferWaste / (stats.QtyBuffers-1)) : 0, &nf ) << ')';
    result                                  << NNEW_LINE;

    #if ALIB_DEBUG_MEMORY
    const DbgStatistics& dbgStats=  ma.DbgGetStatistics();
    result << "    Dbg: Allocations:    "   << NDec( dbgStats.QtyAllocations                                , &nf )   << NNEW_LINE;
    result << "    Dbg: Non-trivial:    "   << NDec( dbgStats.QtyAllocations- dbgStats.QtyTrivialAllocations, &nf )   << NNEW_LINE;
    result << "    Dbg: Resets:         "   << NDec( dbgStats.QtyResets                                     , &nf )   << NNEW_LINE;

    result << "    Dbg: #Allocs/buffer: ";
    if( stats.QtyBuffers == 0 ) { result << "N/A"; ALIB_ASSERT( dbgStats.QtyAllocations == 0, "MONOMEM") }
    else                          result  << NDec( (dbgStats.QtyAllocations / stats.QtyBuffers ), &nf );
    result << NNEW_LINE;

    result << "    Dbg: Alignm. waste: "   << NDec( dbgStats.AlignmentWaste      , &nf )
                                  << " ("  << NDec( double(dbgStats.AlignmentWaste)/double(stats.AllocSize) *100. , &nf ) << "%)"<< NNEW_LINE;
    result << "    Dbg: Qty exceeds:   "   << NDec( dbgStats.QtyBufferSizeExceeds , &nf ) << NNEW_LINE;
    #endif // ALIB_DEBUG_MEMORY

    return result;
}
#include "ALib.Lang.CIMethods.H"
} // namespace [alib::monomem]
#endif // ALIB_STRINGS && ALIB_DEBUG
