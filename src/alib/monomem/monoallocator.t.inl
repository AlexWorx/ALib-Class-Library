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
namespace alib { namespace monomem {

#if !DOXYGEN
#include "ALib.Lang.CIFunctions.H"
template<typename TAllocator>
TMonoAllocator<TAllocator>* TMonoAllocator<TAllocator>::Create( ALIB_DBG(const char* pDbgName,)
                                                                TAllocator&  pAllocator,
                                                                size_t       initialBufferSizeInKB,
                                                                unsigned int pBufferGrowthInPercent )
{
    using TMA= TMonoAllocator<TAllocator>;
    ALIB_ASSERT_ERROR( initialBufferSizeInKB, "MONOMEM", "Initial buffer of 0kb requested." )

    auto size= initialBufferSizeInKB * 1024;
    auto* mem= pAllocator.allocate( size, alignof(detail::Buffer) );
    auto* firstBuffer= new (mem) detail::Buffer( initialBufferSizeInKB * 1024 );

    auto* result =  reinterpret_cast<TMA*>( firstBuffer->allocate( sizeof(TMA), alignof(TMA) ) );
    return new (result) TMA( ALIB_DBG(pDbgName,) pAllocator, firstBuffer,
                             initialBufferSizeInKB, pBufferGrowthInPercent );
}
#include "ALib.Lang.CIMethods.H"
#endif

template<typename TAllocator>
TMonoAllocator<TAllocator>::~TMonoAllocator()
{
    #if ALIB_DEBUG_CRITICAL_SECTIONS
        DbgCriticalSectionsPH.Destruct();
    #endif

    #if ALIB_DEBUG_MEMORY
        integer cntBuffers     = 0;
    #endif

    // first delete recyclable buffers
    auto* cnk= recyclables;
    while( cnk )
    {
        #if ALIB_DEBUG_MEMORY
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
        allocMember::GetAllocator().free( cnk, cnk->Size() );

        #if ALIB_DEBUG_MEMORY
            ++cntBuffers;
        #endif
        cnk= next;
    }

    #if ALIB_DEBUG_MEMORY
        ALIB_ASSERT_WARNING( cntBuffers <= 15, "MONOMEM",
                             "ALIB_DEBUG_MEMORY Warning: More than 15 buffers allocated. "
                             "Buffer size might be increased? #Buffers: ", cntBuffers   )
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


    #if ALIB_DEBUG_MEMORY
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
                // do a 'manual' reset of the buffer, without overwriting its contents.
                buffer->act= reinterpret_cast<char*>( buffer + 1 );
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
    ALIB_ASSERT( snapshot.buffer, "MONOMEM" )
    ALIB_ASSERT( snapshot.buffer == it, "MONOMEM" )
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
        #if ALIB_DEBUG_MEMORY
        ++dbgStats.QtyBufferSizeExceeds;
        #endif
        ALIB_WARNING( "MONOMEM",
                      "MonoAllocator: Allocation size exceeds the next buffers' size.\n"
                      "The allocator's buffer size should be increased.\n"
                      "Requested size: ", size                                     )
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

}} // namespace [alib::monomem]

