//==================================================================================================
/// \file
/// This "template-definition-header" file is part of module \alib_monomem of the \aliblong.
/// It may be used to instantiate custom versions of \alib{monomem;TPoolAllocator}, working with
/// a different \ref alib_contmono_chaining "chained" allocator.
/// @see Manual section \ref alib_manual_appendix_t_hpp_files about the nature of ".t.hpp"-files.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#include "alib/alib_precompile.hpp"
#if !DOXYGEN
#   include "alib/monomem/monoallocator.hpp"
#   if ALIB_ALOX
#       include "alib/alox/logtools.hpp"
#   endif
#   if ALIB_STRINGS && ALIB_DEBUG_MONOMEM
#      include "alib/strings/astring.hpp"
#   endif
#   if ALIB_THREADS && ALIB_CAMP
#      include "alib/threads/thread.hpp"
#      include "alib/lang/basecamp/basecamp.hpp"
#   endif
#   if ALIB_DEBUG_MONOMEM && ALIB_ALOX
#       include "alib/alox/aloxcamp.hpp"
#   endif
#   if ALIB_CAMP && ALIB_DEBUG
#       include "alib/strings/astring.hpp"
#       include "alib/strings/numberformat.hpp"
#   endif

#endif // !DOXYGEN

namespace alib { namespace monomem {

#if !DOXYGEN
#include "alib/lang/callerinfo_functions.hpp"
template<typename TAllocator>
TMonoAllocator<TAllocator>* TMonoAllocator<TAllocator>::Create( ALIB_DBG(const char* pDbgName,)
                                                                TAllocator&  pAllocator,
                                                                size_t       initialBufferSizeInKB,
                                                                unsigned int pBufferGrowthInPercent )
{
    using TMA= TMonoAllocator<TAllocator>;
    ALIB_ASSERT_ERROR( initialBufferSizeInKB, "MONOMEM",  "Initial buffer of 0kb requested." )

    auto size= initialBufferSizeInKB * 1024;
    auto* mem= pAllocator.allocate( size, alignof(detail::Buffer) );
    auto* firstBuffer= new (mem) detail::Buffer( initialBufferSizeInKB * 1024 );

    auto* result =  reinterpret_cast<TMA*>( firstBuffer->allocate( sizeof(TMA), alignof(TMA) ) );
    return new (result) TMA( ALIB_DBG(pDbgName,) pAllocator, firstBuffer,
                             initialBufferSizeInKB, pBufferGrowthInPercent );
}
#include "alib/lang/callerinfo_methods.hpp"
#endif

template<typename TAllocator>
TMonoAllocator<TAllocator>::~TMonoAllocator()
{
    #if ALIB_DEBUG_CRITICAL_SECTIONS
        DbgCriticalSectionsPH.Destruct();
    #endif

    #if ALIB_DEBUG_MONOMEM
        integer cntBuffers     = 0;
    #endif

    // first delete recyclable buffers
    auto* cnk= recyclables;
    while( cnk )
    {
        #if ALIB_DEBUG_MONOMEM
            ++cntBuffers;
        #endif

        auto* next= cnk->previous;
        allocMember::GetAllocator().free( cnk, cnk->Size() );
        cnk= next;
    }

    // then the active ones
    cnk= buffer;
    while( cnk )
    {
        auto* next= cnk->previous;
        if( cnk )
        {
            allocMember::GetAllocator().free( cnk, cnk->Size() );

            #if ALIB_DEBUG_MONOMEM
                ++cntBuffers;
            #endif
        }
        cnk= next;
    }

    #if ALIB_DEBUG_MONOMEM
        ALIB_ASSERT_WARNING( cntBuffers <= 15, "MONOMEM",
                             "ALIB_DEBUG_MONOMEM Warning: More than 15 buffers allocated. "
                             "Buffer size might be increased? #Buffers: ", int(cntBuffers)   )
    #endif
    //ALIB_DCS_RELEASE

}

template<typename TAllocator>
void TMonoAllocator<TAllocator>::Reset( monomem::Snapshot snapshot )
{ ALIB_DCS_WITH( *DbgCriticalSectionsPH.Get() )

    #if ALIB_DEBUG
        // check if a (forbidden!) full reset is requested on a self-contained instance
        if( snapshot.buffer == nullptr )
        {
            detail::Buffer* firstBuffer= buffer;
            while( firstBuffer->previous )
                firstBuffer= firstBuffer->previous;

            ALIB_ASSERT_ERROR( !(    this >= reinterpret_cast<TMonoAllocator*>(firstBuffer      )
                                  && this <  reinterpret_cast<TMonoAllocator*>(firstBuffer->end ) )
                               || snapshot.actFill == reinterpret_cast<char*>(1),
                "MONOMEM",
                "A full MonoAllocator::Reset() was requested for a self-contained monotonic "
                "allocator created with MonoAllocator::Create()."    )
        }
    #endif


    #if ALIB_DEBUG_MONOMEM
        Statistics stats;
        #if ALIB_DEBUG_CRITICAL_SECTIONS
            DbgCriticalSectionsPH.Get()->Release(ALIB_CALLER);
        #endif
            GetStatistics(stats);
        #if ALIB_DEBUG_CRITICAL_SECTIONS
            DbgCriticalSectionsPH.Get()->Acquire(ALIB_CALLER);
        #endif
        dbgStats.QtyAllocationsInclResets       += dbgStats.QtyAllocations;
        dbgStats.QtyTrivialAllocationsInclResets+= dbgStats.QtyTrivialAllocations;
        dbgStats.AllocSizeInclResets            += (stats.AllocSize - dbgStats.AlignmentWaste);
        ++dbgStats.QtyResets;
        dbgStats.QtyAllocations        = 0;
        dbgStats.QtyTrivialAllocations = 0;
        dbgStats.AlignmentWaste        = 0;
        dbgStats.QtyBufferSizeExceeds  = 0;
    #endif

    // recycle buffers until snapshot buffer or end is found
    detail::Buffer* it= buffer;
    while( it != snapshot.buffer )
    {
        #if !ALIB_DEBUG_ALLOCATIONS
            it->reset();
        #else
            // In debug mode, we must not reset the buffer if this is a self-contained instance and
            // this is the first buffer, because this would overwrite the self-contained content
            if ( it->previous != nullptr || snapshot.actFill != reinterpret_cast<char*>(1) )
                it->reset();
            else
                ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
                // do a 'manual' reset of the buffer, without overwriting its contents.
                buffer->act= reinterpret_cast<char*>( buffer + 1 );
                ALIB_WARNINGS_RESTORE
        #endif


        detail::Buffer* next= it->previous;
        if ( next == nullptr )
        {
            #if ALIB_DEBUG_ALLOCATIONS
                if ( snapshot.actFill != reinterpret_cast<char*>(1) )
                    memset( buffer->act, 0xD2, size_t(buffer->end - buffer->act) );
            #endif
            buffer= it;
            return;
        }
        
        it->previous= recyclables;
        recyclables= it;
        it= next;
    }

    // snapshot buffer?
    ALIB_ASSERT( snapshot.buffer )
    ALIB_ASSERT( snapshot.buffer == it )
    buffer= it;
    buffer->act= snapshot.actFill;

    #if ALIB_DEBUG_ALLOCATIONS
        memset( buffer->act, 0xD2, size_t(buffer->end - buffer->act) );
    #endif
}

template<typename TAllocator>
void TMonoAllocator<TAllocator>::destructWithExternalBuffer()
{ ALIB_DCS_WITH( *DbgCriticalSectionsPH.Get() )

    // search the pointer that has no previous and clear that pointer
    detail::Buffer** it= &buffer;
    while( (*it)->previous )
        it= &(*it)->previous;
    *it= nullptr;

    // note: we do not need to check the recycling list, because this first buffer will
    // never be recycled.
}


//==================================================================================================
// TMonoAllocator::get/Free
//==================================================================================================
template<typename TAllocator>
char*  TMonoAllocator<TAllocator>::nextBuffer(size_t size, size_t alignment)
{
    // we always have a buffer in place, and this method is called from the inlined allocation
    // function when the current buffer does not fit.

    // special treatment if size exceeds buffer size: create an own buffer and keep using current
    auto nextBUS= nextBuffersUsableSize;
    if(   lang::DbgAlloc::extSize( size )
        + detail::Buffer::firstOffset(sizeof(detail::Buffer), alignment)  > nextBuffersUsableSize )
    {
        // adjust the next buffer size to the exceeding size
        nextBUS=    lang::DbgAlloc::extSize(size)
                 +  detail::Buffer::firstOffset(sizeof(detail::Buffer), alignment)
                 +  alignment;
        #if ALIB_DEBUG_MONOMEM
        ++dbgStats.QtyBufferSizeExceeds;
        #endif
        ALIB_WARNING( "MONOMEM",
                      "MonoAllocator: Allocation size exceeds the next buffers' size.\n"
                      "The allocator's buffer size should be increased.\n"
                      "Requested size: ",
                      static_cast<int>(size )                        )
    }

    // search a recycle buffer (usually the first fits)
    detail::Buffer** previousPointer= &recyclables;
    detail::Buffer*  recyclable     =  recyclables;
    while( recyclable )
    {                                                                                DBG_ALIGNMENT_INIT(    recyclable )
        char* mem= recyclable->allocate( size, alignment ); DBG_ALIGNMENT_MEASURE( recyclable )
        if( mem )
        {
            *previousPointer= recyclable->previous;
            recyclable->previous= buffer;
            buffer= recyclable;
            return mem;
        }

        // this should almost never happen (only if requesting oversized objects after a reset)
        previousPointer= &recyclable->previous;
        recyclable     =  recyclable->previous;
    }

    // create new buffer
    detail::Buffer* previousBuffer= buffer;
    buffer= new (allocMember::GetAllocator().allocate(nextBUS, alignof(void*))) detail::Buffer( nextBUS );
    if ( nextBUS == nextBuffersUsableSize)
        nextBuffersUsableSize= (nextBuffersUsableSize * bufferGrowthInPercent) / 100;
    buffer->previous     = previousBuffer;                                   DBG_ALIGNMENT_INIT( buffer )

    char* mem= buffer->allocate( size, alignment ); DBG_ALIGNMENT_MEASURE( buffer )
    return mem;
}

//==================================================================================================
// Statistics
//==================================================================================================
template<typename TAllocator>
void  TMonoAllocator<TAllocator>::GetStatistics( Statistics& result )
{ ALIB_DCS_WITH( *DbgCriticalSectionsPH.Get() )

    result= Statistics();
    result.NextBufferSize   = nextBuffersUsableSize;


    // walk recyclable buffers
    detail::Buffer* it= buffer;
    result.CurrentBufferSize= buffer->Size();
    result.CurrentBufferFree= size_t(buffer->end - buffer->act);

    while( it != nullptr )
    {
        result.QtyBuffers++;
        result.HeapSize+=  it->Size();
        result.AllocSize+= it->Size() - size_t(it->end - it->act) - sizeof(detail::Buffer);
        if ( it != buffer )
            result.BufferWaste+= size_t(it->end - it->act);

        it= it->previous;
    }

    it= recyclables;
    while( it != nullptr )
    {
        result.QtyRecyclables++;
        result.HeapSizeRecycled+= it->Size();
        it= it->previous;
    }
}


#if ALIB_CAMP && ALIB_DEBUG
template<typename TAllocator>
NAString TMonoAllocator<TAllocator>::DbgDumpStatistics()
{
    Statistics stats;
    GetStatistics(stats);

    NAString result;
    NNumberFormat nf;
    nf.Flags |= NumberFormatFlags::WriteGroupChars;
    nf.FractionalPartWidth=2;
    result << "MonoAllocator Usage Statistics:"  << NNEW_LINE;

    result << "    Allocation Size:     "   << NFormat( stats.AllocSize                              , &nf ) << NNEW_LINE;
    result << "    Current buffer free: "   << NFormat( stats.CurrentBufferFree                      , &nf ) << NNEW_LINE;
    result << "    Current buffer used: "   << NFormat( buffer->act - reinterpret_cast<char*>(buffer), &nf ) << NNEW_LINE;
    result << "    Current buffer size: "   << NFormat( stats.CurrentBufferSize                      , &nf ) << NNEW_LINE;
    result << "    Next buffer size:    "   << NFormat( stats.NextBufferSize                         , &nf ) << NNEW_LINE;
    result << "    Buffers in use:      "   << NFormat( stats.QtyBuffers                             , &nf ) << NNEW_LINE;
    result << "    Buffers allocated:   "   << NFormat( stats.QtyBuffers + stats.QtyRecyclables      , &nf ) << NNEW_LINE;
    result << "    Heap size in use:    "   << NFormat( stats.HeapSize                               , &nf ) << NNEW_LINE;
    result << "    Heap size allocated: "   << NFormat( stats.HeapSize + stats.HeapSizeRecycled      , &nf ) << NNEW_LINE;
    result << "    Unused buffer bytes: "   << NFormat( stats.BufferWaste                            , &nf );
          if( stats.QtyBuffers )  result  << "  (per buffer: " << NFormat( stats.QtyBuffers > 1 ? (stats.BufferWaste / (stats.QtyBuffers-1)) : 0, &nf ) << ')';
    result                                  << NNEW_LINE;

    #if ALIB_DEBUG_MONOMEM
    result << "    Dbg: Allocations:    "   << NFormat( dbgStats.QtyAllocations                                , &nf )   << NNEW_LINE;
    result << "    Dbg: Non-trivial:    "   << NFormat( dbgStats.QtyAllocations- dbgStats.QtyTrivialAllocations, &nf )   << NNEW_LINE;
    result << "    Dbg: Resets:         "   << NFormat( dbgStats.QtyResets                                     , &nf )   << NNEW_LINE;

    result << "    Dbg: #Allocs/buffer: ";
    if( stats.QtyBuffers == 0 ) { result << "N/A"; ALIB_ASSERT( dbgStats.QtyAllocations == 0) }
    else                          result  << NFormat( (dbgStats.QtyAllocations / stats.QtyBuffers ), &nf );
    result << NNEW_LINE;

    result << "    Dbg: Alignm. waste: "   << NFormat( dbgStats.AlignmentWaste      , &nf )
                                  << " ("   << NFormat( double(dbgStats.AlignmentWaste)/double(stats.AllocSize) *100. , &nf ) << "%)"<< NNEW_LINE;
    result << "    Dbg: Qty exceeds:   "   << NFormat( dbgStats.QtyBufferSizeExceeds , &nf ) << NNEW_LINE;
    #endif // ALIB_DEBUG_MONOMEM

    return result;
}
#endif // ALIB_STRINGS && ALIB_DEBUG

}} // namespace [alib::monomem]

