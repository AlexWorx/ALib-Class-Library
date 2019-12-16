// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)
#   if !defined(HPP_ALIB_MONOMEM_MONOALLOCATOR)
#      include "alib/monomem/monoallocator.hpp"
#   endif

#   if ALIB_STRINGS && ALIB_DEBUG_MONOMEM && !defined(HPP_ALIB_STRINGS_ASTRING)
#      include "alib/strings/astring.hpp"
#   endif

#   if ALIB_THREADS && ALIB_FILESET_MODULES
#      include "alib/threads/thread.hpp"
#      include "alib/lib/fs_modules/distribution.hpp"
#   endif
#   if ALIB_DEBUG_MONOMEM
#      if ALIB_ALOX
#         include "alib/alox/aloxmodule.hpp"
#      endif
#      include <atomic>
#   endif
#endif // !defined(ALIB_DOX)

namespace aworx { namespace lib {

/** ************************************************************************************************
 * This \alibmod implements the concept of <em>"monotonic allocation"</em> by providing central class
 * \alib{monomem,MonoAllocator} and a few corresponding container and utility types.
 *
 * Please consult \ref alib_mod_monomem "ALib Module Monomem - Programmer's Manual"
 * for further information.
 **************************************************************************************************/
namespace monomem {


// #################################################################################################
// Globals
// #################################################################################################
                 MonoAllocator   GlobalAllocator( 8 * 1024 );
ALIB_IF_THREADS( ThreadLock      GlobalAllocatorLock;              )


MonoAllocator::Chunk   MonoAllocator::dummyChunk= MonoAllocator::Chunk(TInitializeDefaults());


#if ALIB_THREADS && ALIB_FILESET_MODULES && !defined(ALIB_DOX)

    void MonoAllocator::dbgCheckGlobalAllocatorLock()
    {
        ALIB_ASSERT_ERROR(     this != &GlobalAllocator
                           ||  !aworx::ALIB.IsBootstrapped()
                           ||  GlobalAllocatorLock.GetSafeness() == Safeness::Unsafe
                           ||  GlobalAllocatorLock.IsOwnedByCurrentThread()
            , "Global MonoAllocater not acquired."  )
    }

#endif

#if ALIB_DEBUG_MONOMEM && !defined(ALIB_DOX)
    bool dbgLogLock(bool lock)
    {
        static std::atomic_int locker;
        if (lock)
        {
            if(     locker++
                ||  !aworx::lib::DbgIsBootstrapped
                ||  aworx::lib::lox::Log::DebugLogger == nullptr )
                return false;
            return true;
        }
        GlobalAllocator.LogDomain= A_CHAR("MA/GLBL");

        locker--;
        return true;
    }
#endif



// #################################################################################################
// MonoAllocator
// #################################################################################################
MonoAllocator::MonoAllocator( Chunk* pfirstChunk, size_t stdChunkSize )
: chunk         ( pfirstChunk )
, recyclables   ( nullptr )
, stdUsableSpace( stdChunkSize - MaxUsableSpaceLoss() )
{
    #if ALIB_DEBUG_MONOMEM
        ++DbgStats.QtyChunks;
        DbgStats.HeapSize+= static_cast<size_t>( pfirstChunk->end - reinterpret_cast<char*>( pfirstChunk ) );
    #endif


    chunk->previous= nullptr;
    ALIB_ASSERT_ERROR( stdChunkSize > sizeof(Chunk),
                       "The standard allocation size has to be greater than {}.", sizeof(Chunk) )
}


MonoAllocator::MonoAllocator( size_t stdChunkSize )
: chunk         ( &dummyChunk )
, recyclables   ( nullptr )
, stdUsableSpace( stdChunkSize - MaxUsableSpaceLoss() )
{
    MONOMEM_PATHDOMAIN
    MONOMEM_VERBOSE( LogDomain, "Created MonoAllocator. Chunk size: ", stdChunkSize )
    ALIB_ASSERT_ERROR( stdChunkSize > sizeof(Chunk),
                       "The standard allocation size has to be greater than {}.", sizeof(Chunk) )

}

MonoAllocator*         MonoAllocator::Create( size_t stdChunkSize )
{
    MONOMEM_PATHDOMAIN
    MONOMEM_INFO( "MA/SELFCNT", "Creating self-contained MonoAllocator. Chunk size: ", stdChunkSize )

    ALIB_ASSERT_ERROR( stdChunkSize > MonoAllocator::MaxUsableSpaceLoss(),
                       "The standard allocation size has to be greater than {}.", sizeof(Chunk) )

    Chunk*          firstChunk= Chunk::create( stdChunkSize - MonoAllocator::MaxUsableSpaceLoss() );
    MonoAllocator* allocator =  reinterpret_cast<MonoAllocator*>( firstChunk->alloc( sizeof( MonoAllocator), alignof(MonoAllocator) ) );
    return new (allocator) MonoAllocator( firstChunk, stdChunkSize );
}

MonoAllocator::~MonoAllocator()
{
    #if ALIB_DEBUG_MONOMEM
        integer cntChunks     = 0;

        // need to save these values, as later, if self-contained, they are de-allocated
        String  logDomain          = LogDomain;
        integer qtyAllocations     = static_cast<integer>(DbgStats.QtyAllocations);
    #endif

    // first delete recyclable chunks
    auto* cnk= recyclables;
    while( cnk )
    {
        ALIB_ASSERT( cnk != &dummyChunk )
        #if ALIB_DEBUG_MONOMEM
            ++cntChunks;
        #endif

        auto* next= cnk->previous;
        cnk->destruct();
        cnk= next;
    }

    // then the active ones
    cnk= chunk;
    while( cnk )
    {
        auto* next= cnk->previous;
        if( cnk != &dummyChunk )
        {
            cnk->destruct();

            #if ALIB_DEBUG_MONOMEM
                ++cntChunks;
            #endif
        }
        cnk= next;
    }

    MONOMEM_PATHDOMAIN
    MONOMEM_INFO( logDomain, "MonoAllocator destructed. #Chunks: {}, #allocations: {}",
                             cntChunks, qtyAllocations )

    #if ALIB_DEBUG_MONOMEM
        ALIB_ASSERT_WARNING( cntChunks <= 15,
                             "ALIB_DEBUG_MONOMEM Warning: More than 15 chunks allocated. "
                             "Chunksize might be increased? #Chunks: ", cntChunks         )
    #endif
}

void MonoAllocator::Reset( const MonoAllocator::Snapshot& snapshot )
{
    if( chunk == &dummyChunk )
    {
        ALIB_ASSERT_ERROR( snapshot.chunk == nullptr,
                           "Illegal snapshot given. Chunk allocator has no allocations, yet " )
        return;
    }

    #if ALIB_DEBUG
    if( snapshot.chunk == nullptr )
    {
        Chunk* firstChunk= chunk;
        while( firstChunk->previous )
            firstChunk= firstChunk->previous;

        ALIB_ASSERT_ERROR( !(    this >= reinterpret_cast<MonoAllocator*>(firstChunk      )
                              && this <  reinterpret_cast<MonoAllocator*>(firstChunk->end ) ),
            "Parameterless version of MonoAllocator::Reset() was called for a "
            "self-contained monotonic allocator created with MonoAllocator::Create()."             )
    }
    #endif

    #if ALIB_DEBUG_MONOMEM
        ++DbgStats.QtyResets;
    #endif

    // recycle chunks until snapshot chunk or end is found
    Chunk* it= chunk;
    while( it != snapshot.chunk )
    {
        it->reset();
        Chunk* next= it->previous;
        it->previous= recyclables;
        recyclables= it;
        it= next;
    }

    // snapshot chunk?
    chunk= it;
    if( snapshot.chunk )
    {
        ALIB_ASSERT( chunk)
        chunk->act= snapshot.actFill;
    }

    if( chunk == nullptr )
        chunk= &dummyChunk;
}

#if ALIB_DEBUG_MONOMEM
    #define DBG_ALIGNMENT_INIT(chunk)     size_t qtyLeftBeforeAlloc= static_cast<size_t>(chunk->end - chunk->act);
    #define DBG_ALIGNMENT_MEASURE(chunk)  if(mem) DbgStats.AlignmentWaste+=                        \
                                                     qtyLeftBeforeAlloc                            \
                                                     - static_cast<size_t>(chunk->end - chunk->act)\
                                                     - size;
#else
    #define DBG_ALIGNMENT_INIT(...)
    #define DBG_ALIGNMENT_MEASURE(...)
#endif


// #################################################################################################
// MonoAllocator::get/Free
// #################################################################################################
char*  MonoAllocator::getCreateChunk(size_t size, size_t alignment)
{
    ALIB_ASSERT( static_cast<size_t>(chunk->end - chunk->act) < size )

    if( chunk == &dummyChunk )
        chunk=   nullptr;

    #if ALIB_DEBUG_MONOMEM
        if( chunk )
            DbgStats.ChunkWaste+=   static_cast<size_t>(chunk->end - chunk->act);
    #endif


    // special treatment if size exceeds chunk size: create an own chunk and keep using current
    if( size >= stdUsableSpace )
    {
        Chunk* newChunk= Chunk::create( size
                                        // deduct what is internally added for the worst case of max-alignment
                                        - MonoAllocator::MaxUsableSpaceLoss()

                                        // replace with overhead caused with given alignment
                                        + sizeof(Chunk) + sizeof(Chunk) % alignment
                                      );

        #if ALIB_DEBUG_MONOMEM
            ++DbgStats.QtyChunkSizeExceeds;
            ++DbgStats.QtyChunks;
            DbgStats.HeapSize+= static_cast<size_t>( newChunk->end - reinterpret_cast<char*>( newChunk ) );
            ALIB_ASSERT_WARNING( DbgStats.QtyChunkSizeExceeds == 0,
                                 "ALIB_DEBUG_MONOMEM Warning: Allocation size matches or exceeds "
                                 "chunk size. Chunksize might be increased? Requested size ",
                                 static_cast<integer>(size )                        )
        #endif


        char* mem=   newChunk->alloc( size, alignment );
        ALIB_ASSERT( mem && ( newChunk->act == newChunk->end) )

        // add new chunk to used ones
        if( chunk )
        {
            newChunk->previous= chunk->previous;
            chunk->previous= newChunk;
        }
        else
        {
            // first chunk exceeded. Store chunk as actual. Will be replaced with a new one with
            // next allocation
            chunk= newChunk;
            newChunk->previous= nullptr;
        }
        return mem;
    }


    // search a recycle chunk (usually the first fits)
    Chunk** previousPointer= &recyclables;
    Chunk*  recycable      =  recyclables;
    while( recycable )
    {                                                       DBG_ALIGNMENT_INIT(    recycable )
        char* mem= recycable->alloc( size, alignment );     DBG_ALIGNMENT_MEASURE( recycable )
        if( mem )
        {
            *previousPointer= recycable->previous;
            recycable->previous= chunk;
            chunk= recycable;
            return mem;
        }

        // this should almost never happen (only if requesting oversized objects after a reset)
        previousPointer= &recycable->previous;
        recycable= recycable->previous;
    }

    // create new chunk
    Chunk* previousChunk= chunk;
    chunk               = Chunk::create( stdUsableSpace );
    chunk->previous     = previousChunk;                    DBG_ALIGNMENT_INIT( chunk )

    #if ALIB_DEBUG_MONOMEM
        ++DbgStats.QtyChunks;
        DbgStats.HeapSize+= static_cast<size_t>( chunk->end - reinterpret_cast<char*>( chunk ) );
    #endif

    char* mem= chunk->alloc( size, alignment );             DBG_ALIGNMENT_MEASURE( chunk )
    return mem;
}


// #################################################################################################
// DbgStatistics
// #################################################################################################
#if ALIB_STRINGS && ALIB_DEBUG_MONOMEM

NAString MonoAllocator::DbgDumpStats()
{
    NAString result;
    result << "MonoAllocator Usage Statistics:"  << NNewLine();

    result << "    Allocations:         "   << DbgStats.QtyAllocations          << NNewLine();
    result << "    Trivial Allocations: "   << DbgStats.QtyTrivialAllocations   << NNewLine();
    result << "    Chunks:              "   << (DbgStats.QtyChunks )            << NNewLine();
    result << "    Resets:              "   << DbgStats.QtyResets               << NNewLine();

    result << "    Avg. alloc./chunk:   ";
    if( DbgStats.QtyChunks == 0 ) { result << "N/A"; ALIB_ASSERT( DbgStats.QtyAllocations == 0) }
    else                            result << (DbgStats.QtyAllocations / DbgStats.QtyChunks );
    result << NNewLine();

    result << "    Allocated Heap Mem.: "   << (DbgStats.HeapSize)              << NNewLine();
    result << "    Wasted bytes:         "   << DbgStats.ChunkWaste;
          if( DbgStats.QtyChunks )  result  << "  (per chunk: " << (DbgStats.ChunkWaste / DbgStats.QtyChunks ) << ')';
    result                                  << NNewLine();

    result << "    Alignment waste:     "   << DbgStats.AlignmentWaste      << NNewLine();
    result << "    Chunk size exceeds:  "   << DbgStats.QtyChunkSizeExceeds << NNewLine();

    return result;
}
#endif



}}}// namespace [aworx::lib::monomem]

