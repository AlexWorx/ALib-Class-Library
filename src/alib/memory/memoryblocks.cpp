// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(HPP_ALIB_LIB_ASSERT)
#   include "alib/lib/assert.hpp"
#endif

#if !defined(HPP_ALIB_MEMORY_MEMORYBLOCKS)
#   include "alib/memory/memoryblocks.hpp"
#endif

#if !defined (_GLIBCXX_ALGORITHM) && !defined(_ALGORITHM_)
#   include <algorithm>
#endif

#if ALIB_MODULE_STRINGS && ALIB_MEMORY_DEBUG && !defined(HPP_ALIB_STRINGS_ASTRING)
#   include "alib/strings/astring.hpp"
#endif

namespace aworx { namespace lib {

/**
 * This is the namespace of \alibmod <b>"Memory"</b>. This module is extremely small,
 * it contains only one class, \alib{memory,MemoryBlocks}.
 *
 * While this class is already used in production (by other \alibmods), the design of the class
 * is considered beta state and is certainly changed with future requirements.
 */
namespace memory {

MemoryBlocks::Block*    MemoryBlocks::Block::create( size_t minSize )
{
    size_t sizeAsBlockObjects=  (minSize / sizeof(Block) + 2 );
    if (sizeAsBlockObjects & 1 )
        sizeAsBlockObjects++;

    Block* chunk= new Block[ sizeAsBlockObjects ];
    new (chunk) Block();
    chunk->mem= reinterpret_cast<int8_t*>( &chunk[1] );
    chunk->left=
    chunk->size= sizeof(Block[2]) * (sizeAsBlockObjects/2 - 1);
    chunk->next= nullptr;
    return chunk;
}

MemoryBlocks* MemoryBlocks::Create( size_t stdBlockSize )
{
    Block* firstBlock= Block::create( stdBlockSize );
    MemoryBlocks* result=  reinterpret_cast<MemoryBlocks*>( firstBlock->get( sizeof( MemoryBlocks), alignof(MemoryBlocks) ) );
    return new (result) MemoryBlocks( firstBlock, stdBlockSize );
}

MemoryBlocks::MemoryBlocks( Block* pfirstBlock, size_t stdBlockSize )
: blockAllocationSize( stdBlockSize )
{
    actBlock=
    firstBlock= pfirstBlock;

    #if ALIB_MEMORY_DEBUG
        dbgQtyAllocations        = 0;
        dbgAllocSize             = 0;
        dbgAlignmentWaste        = 0;
        dbgQtyUnusedByes         = 0;
        dbgQtyBlockSizeExceeds   = 0;
    #endif
}


MemoryBlocks::MemoryBlocks( size_t stdBlockSize )
: blockAllocationSize(stdBlockSize)
{
    actBlock=
    firstBlock= nullptr;
    #if ALIB_MEMORY_DEBUG
        dbgQtyAllocations        = 0;
        dbgAllocSize             = 0;
        dbgAlignmentWaste        = 0;
        dbgQtyUnusedByes         = 0;
        dbgQtyBlockSizeExceeds   = 0;
    #endif
}

MemoryBlocks::~MemoryBlocks()
{
    auto* block= firstBlock;
    while( block )
    {
        auto* next= block->next;
        block->destruct();
        block= next;
    }
}

void MemoryBlocks::Reset()
{
    ALIB_ASSERT_ERROR(    !firstBlock
                       || this < reinterpret_cast<MemoryBlocks*>(firstBlock->mem                   )
                       || this >=reinterpret_cast<MemoryBlocks*>(firstBlock->mem + firstBlock->size),
        "MemoryBlocks::Reset must not be invoked on objects that have been allocated\n"
        "in 'itself' with MemoryBlocks::Create()."                                                 )

    actBlock= firstBlock;
    while( actBlock )
    {
        actBlock->reset();
        actBlock= actBlock->next;
    }


    actBlock= firstBlock;
    #if ALIB_MEMORY_DEBUG
        dbgQtyAllocations        = 0;
        dbgAllocSize             = 0;
        dbgAlignmentWaste        = 0;
        dbgQtyUnusedByes         = 0;
        dbgQtyBlockSizeExceeds   = 0;
    #endif
}

char*  MemoryBlocks::get(size_t size, size_t alignment)
{
    #if ALIB_MEMORY_DEBUG
        dbgQtyAllocations++;
        dbgAllocSize     += size;
    #endif
    ALIB_ASSERT_ERROR( size, "Request for allocation of zero size." )
    for(;;)
    {
        if( actBlock == nullptr  )
        {
            actBlock=   Block::create( (std::max)( size + alignment, blockAllocationSize ) );
            if( firstBlock )
            {
                Block* block= firstBlock;
                while( block->next )
                    block= block->next;
                block->next= actBlock;
            }
            else
                firstBlock= actBlock;

            #if ALIB_MEMORY_DEBUG
                if( size > blockAllocationSize )
                    dbgQtyBlockSizeExceeds++;
            #endif
        }

        #if ALIB_MEMORY_DEBUG
            size_t qtyLeftBeforeAlloc= actBlock->left;
        #endif

        char* mem= actBlock->get( size, alignment );
        if( mem )
        {
            #if ALIB_MEMORY_DEBUG
                dbgAlignmentWaste+= qtyLeftBeforeAlloc - actBlock->left - size;
            #endif
            return mem;
        }

        #if ALIB_MEMORY_DEBUG
            dbgQtyUnusedByes+= actBlock->left;
        #endif
        actBlock= actBlock->next;
    }
}

#if ALIB_MODULE_STRINGS && ALIB_MEMORY_DEBUG

NAString MemoryBlocks::DbgStats()
{
    NAString result;
    result << "MemoryBlock Usage Statistics:"  << NNewLine();

    size_t cntBlocks= 0;
    {
        auto* block= firstBlock;
        while( block )
        {
            cntBlocks++;
            block= block->next;
        }
    }


    result << "  Allocations:        "   << dbgQtyAllocations << NNewLine();
    result << "  Blocks:             "   << cntBlocks         << " (default block size: " << blockAllocationSize << ')' <<  NNewLine();
    result << "  Allocated memory:   "   << dbgAllocSize      << NNewLine();

    result << "  Avg. alloc./block:  ";
    if( cntBlocks == 0 )     { result << "N/A"; ALIB_ASSERT( dbgQtyAllocations == 0) }
    else                       result << (dbgQtyAllocations / cntBlocks );
    result << NNewLine();

    result << "  Wasted byes:        ";
    if( cntBlocks <  2 )     { result << "N/A"; ALIB_ASSERT( dbgQtyUnusedByes == 0) }
    else                       result << dbgQtyUnusedByes << "  (per block: "   << (dbgQtyUnusedByes / (cntBlocks - 1 ) ) << ')';
    result << NNewLine();

    result << "  Alignment waste:    "   << dbgAlignmentWaste      << NNewLine();
    result << "  Block size exceeds: "   << dbgQtyBlockSizeExceeds << NNewLine();


    return result;
}
#endif



}}}// namespace [aworx::lib::memory]

