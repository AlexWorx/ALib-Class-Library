// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib.hpp"

#if !defined (HPP_ALIB_LANG_MEMORYBLOCKS)
#   include "memoryblocks.hpp"
#endif
#if !defined (_GLIBCXX_ALGORITHM) && !defined(_ALGORITHM_)
#   include <algorithm>
#endif



namespace aworx { namespace lib { namespace util {

MemoryBlocks::MemoryBlocks( size_t stdBlockSize )
: blockAllocationSize( stdBlockSize )
{
    Clear();
}

MemoryBlocks::~MemoryBlocks()
{
    Clear( CurrentData::Clear );
}

void MemoryBlocks::Clear( CurrentData currentData )
{
    if( currentData == CurrentData::Clear )
    {
        for ( auto& block : blocks )
            delete[] block.mem;
        blocks.clear();
    }
    else
    {
        for ( auto& block : blocks )
            block.left= block.size;
    }

    actBlock= blocks.begin();

    #if ALIB_DEBUG
        dbgQtyAllocations        = 0;
        dbgAllocSize             = 0;
        dbgAlignmentWaste        = 0;
        dbgQtyUnusedByes         = 0;
        dbgQtyBlockSizeExceeds   = 0;
    #endif
}

char*  MemoryBlocks::getAlignedChunk(size_t size, size_t alignment)
{
    ALIB_DBG( dbgQtyAllocations++;
              dbgAllocSize     += size; )

    for(;;)
    {
        if( actBlock == blocks.end()  )
        {
            blocks.emplace_back( Block( (std::max)( size + alignment, blockAllocationSize ) ) );
            actBlock= blocks.end() - 1;

            #if ALIB_DEBUG
                if( actBlock->size > blockAllocationSize )
                    dbgQtyBlockSizeExceeds++;
            #endif
        }

        ALIB_DBG( size_t qtyLeftBeforeAlloc= actBlock->left );

        char* mem= actBlock->get( size, alignment );
        if( mem )
        {
            ALIB_DBG( dbgAlignmentWaste+= qtyLeftBeforeAlloc - actBlock->left - size );
            return mem;
        }

        ALIB_DBG( dbgQtyUnusedByes+= actBlock->left ) ;
        actBlock++;
    }
}

#if ALIB_DEBUG
NAString MemoryBlocks::DbgStats()
{
    NAString result;
    result << "MemoryBlock Usage Statistics:"  << NewLine;

    result << "  Allocations:        "   << dbgQtyAllocations << NewLine;
    result << "  Blocks:             "   << blocks.size()     << " (default block size: " << blockAllocationSize << ')' <<  NewLine;
    result << "  Allocated memory:   "   << dbgAllocSize      << NewLine;

    result << "  Avg. alloc./block:  ";
    if( blocks.size() == 0 ) { result << "N/A"; ALIB_ASSERT( dbgQtyAllocations == 0) }
    else                       result << (dbgQtyAllocations / blocks.size() );
    result << NewLine;

    result << "  Wasted byes:        ";
    if( blocks.size() <  2 ) { result << "N/A"; ALIB_ASSERT( dbgQtyUnusedByes == 0) }
    else                       result << dbgQtyUnusedByes << "  (per block: "   << (dbgQtyUnusedByes / (blocks.size() - 1 ) ) << ')';
    result << NewLine;

    result << "  Alignment waste:    "   << dbgAlignmentWaste      << NewLine;
    result << "  Block size exceeds: "   << dbgQtyBlockSizeExceeds << NewLine;


    return result;
}
#endif



}}}// namespace [aworx::lib::util]

