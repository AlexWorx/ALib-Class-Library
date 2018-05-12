// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
/** @file */ // Hello Doxygen

// check for alib.hpp already there but not us
#if !defined (HPP_ALIB)
    #error "include \"alib/alib.hpp\" before including this header"
#endif
#if defined(HPP_COM_ALIB_TEST_INCLUDES) && defined(HPP_ALIB_LANG_MEMORYBLOCKS)
    #error "Header already included"
#endif


// then, set include guard
#ifndef HPP_ALIB_LANG_MEMORYBLOCKS
//! @cond NO_DOX
#define HPP_ALIB_LANG_MEMORYBLOCKS 1
//! @endcond

#if !defined (_GLIBCXX_VECTOR) && !defined(_VECTOR_)
    #include <vector>
#endif


namespace aworx { namespace lib { namespace util {

/** ************************************************************************************************
 * Simple class that allocates a series of bigger memory blocks  and provides sequential
 * allocation of sub-ranges of those. Furthermore, the allocations can be reset and the
 * allocated blocks reused.
 *
 * The size of the blocks allocated is defined with constructor parameter \p{pSize}.
 * If an invocation of an allocation method is requesting a bigger allocation, then a new block
 * with that given size is created.
 *
 * \note
 *   This class is new with \alib. It is not considered finished, optimized and stable in design.
 **************************************************************************************************/
class MemoryBlocks
{
    // #############################################################################################
    // inner types
    // #############################################################################################
    protected:
        /** ****************************************************************************************
         * Internal type defining an allocated memory chunk.
         ******************************************************************************************/
        struct Block
        {
            char*     mem;  ///< The allocated memory. This is in fact a <c>void*</c> but
                            ///< <c>char*</c> allows arithmetics.
            size_t    size; ///< The size of the block.
            size_t    left; ///< The current fill.

            /** ************************************************************************************
             * Constructor allocating the given \p{pSize} of memory.
             * This memory is not freed with a destructor, therefore
             * objects of this type need to be deleted manually.
             *
             * @param pSize  The size of memory chunk to allocate
             **************************************************************************************/
            inline Block( size_t pSize )
            : size(pSize)
            , left(pSize)
            {
                mem= new char[pSize];
            }

            /** ************************************************************************************
             * Returns \c nullptr if the requested size is not available in the block.
             * Otherwise, the pointer to the aligned free chunk of memory inside the block is
             * returned.
             *
             * @param requestedSize The size to allocate.
             * @param alignment     The alignment.
             * @return \c nullptr on failure, otherwise pointer to the requested memory.
             **************************************************************************************/
            inline char* get( size_t requestedSize, size_t alignment )
            {
                void*   actMem= mem  + size - left;
                if( !std::align( alignment, requestedSize, actMem, left) )
                    return nullptr;
                left-=  requestedSize;

                return reinterpret_cast<char*>( actMem );
            }
        };

    // #############################################################################################
    // protected fields
    // #############################################################################################
    protected:
        /// The list of allocated memory blocks.
        std::vector<MemoryBlocks::Block>                        blocks;

        /// The actual block iterator.
        typename std::vector<MemoryBlocks::Block>::iterator     actBlock;

        /** The standard allocation size. Blocks may be bigger in the case that
         *  a string added is longer than this value. */
        size_t                                                  blockAllocationSize;

        #if ALIB_DEBUG
            /// The number of allocations performed.
            /// Available only in debug compilations.
            size_t          dbgQtyAllocations;

            /// The number of allocated space.
            /// Available only in debug compilations.
            size_t          dbgAllocSize;

            /// The number of space used for alignment.
            /// Available only in debug compilations.
            size_t          dbgAlignmentWaste;


            /// The number of bytes not used in blocks.
            /// Available only in debug compilations.
            size_t          dbgQtyUnusedByes;

            /// The number of allocations that have been larger than the block size.
            /// Available only in debug compilations.
            size_t          dbgQtyBlockSizeExceeds;
        #endif


   // #############################################################################################
   // Constructor / Destructor
   // #############################################################################################
   public:
        /** ****************************************************************************************
         * Constructor.
         * Accepts a value to manipulate #blockAllocationSize.
         *
         * @param stdBlockSize The size of memory blocks allocated.
         ******************************************************************************************/
        ALIB_API MemoryBlocks( size_t stdBlockSize );

        /** ****************************************************************************************
         * Destructor.
         * Invokes \ref Clear "Clear(true)".
         ******************************************************************************************/
        ALIB_API ~MemoryBlocks();


   // #############################################################################################
   // Interface
   // #############################################################################################
   public:
        /** ****************************************************************************************
         * Allocates aligned memory of size of type \p{T}.
         *
         * @return A pointer to the allocated memory.
         ******************************************************************************************/
        template<typename T>
        inline T*   Alloc()
        {
            return reinterpret_cast<T*>( getAlignedChunk( sizeof(T[1]), alignof(T) ) );
        }

        /** ****************************************************************************************
         * Allocates aligned memory for an array of objects \p{T} of size \p{length}.
         *
         * @param length The number of objects in of type \p{T}.
         * @tparam T     The array element type.
         * @tparam TSize The type of the array length value. This template parameter is provided
         *               for convenience, to avoid casts from singed types.
         * @return A pointer to the allocated memory.
         ******************************************************************************************/
        template<typename T, typename TSize>
        inline T*   AllocArray( TSize length )
        {
            return reinterpret_cast<T*>( getAlignedChunk( sizeof(T[1]) * static_cast<size_t>(length), alignof(T[]) ) );
        }

        /** ****************************************************************************************
         * Clears the allocation information.
         * The allocated memory blocks will be freed only if parameter \p{deallocate} is \c true.
         * In this case, future invocations of allocation methods will allocate new blocks from
         * the heap.
         *
         * @param currentData Denotes if the currently allocated memory chunks is to be freed or
         *                    to be reused with future allocations.
         ******************************************************************************************/
        ALIB_API void    Clear( CurrentData currentData = CurrentData::Keep );


        #if ALIB_MODULE_STRINGS
            /** ************************************************************************************
             * Returns a copy of the given string.
             *
             * @param src The source string to copy
             * @tparam TChar The character type of the input and output string.
             * @return A string object representing the copy.
             **************************************************************************************/
            template<typename TChar>
            inline
            strings::StringBase<TChar>  AllocAndCopy( const strings::StringBase<TChar>& src )
            {
                TChar* mem=  AllocArray<TChar>( src.Length() );
                src.CopyTo( mem );
                return strings::StringBase<TChar>(mem, src.Length() );
            }

            #if ALIB_DEBUG
                /** ********************************************************************************
                 * Provides allocation statistics for manual performance optimization.
                 *
                 * @return Some textual information on the allocation statistics.
                 **********************************************************************************/
                ALIB_API NAString DbgStats();
            #endif

        #endif
   // #############################################################################################
   // Internals
   // #############################################################################################
    protected:
        /** ****************************************************************************************
         * This internal allocation method is called by the allocation interface methods.
         * Internally, this method calls \alib{util::MemoryBlocks,Block::get}, which in turn
         * uses <c>std::align</c> to align the address.
         *
         * @param size      The size of the memory to allocate.
         * @param alignment The allocation alignment needed.
         * @return A pointer to the allocated memory.
         ******************************************************************************************/
        ALIB_API char*   getAlignedChunk(size_t size, size_t alignment);

};

}} // namespace aworx[::lib::util]

/// Type alias in namespace #aworx.
using     MemoryBlocks   =   aworx::lib::util::MemoryBlocks;

} // namespace aworx

#endif // HPP_ALIB_LANG_MEMORYBLOCKS
