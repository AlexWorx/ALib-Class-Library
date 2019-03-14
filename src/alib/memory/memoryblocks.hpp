// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#ifndef HPP_ALIB_MEMORY_MEMORYBLOCKS
#define HPP_ALIB_MEMORY_MEMORYBLOCKS 1

#if !defined(HPP_ALIB_LIB_PREDEF_MODULES)
#   include "alib/lib/predef_modules.hpp"
#endif

#if ALIB_MODULE_STRINGS
#   if !defined(HPP_ALIB_STRINGS_STRING)
#       include "alib/strings/string.hpp"
#   endif
#else
#    if !defined(HPP_ALIB_LIB_PREDEF_COMPILER)
#       include "alib/lib/predef_compiler.hpp"
#    endif
#endif

#if !defined (_GLIBCXX_MEMORY) && !defined(_MEMORY_)
#   include <memory>
#endif
#if !defined (_GLIBCXX_VECTOR) && !defined(_VECTOR_)
    #include <vector>
#endif



#if defined(ALIB_MEMORY_DEBUG)
#   error "Code selector symbol ALIB_STRINGS_DEBUG must not be set from outside. Use  postfix '_ON' or '_OFF' for compiler symbols."
#endif
#if defined(ALIB_MEMORY_DEBUG_OFF) && defined(ALIB_MEMORY_DEBUG_ON)
    #error "Compiler symbols ALIB_MEMORY_DEBUG_OFF / ALIB_MEMORY_DEBUG_ON are both set (contradiction)."
#endif

#if defined( ALIB_MEMORY_DEBUG_ON )
    #define    ALIB_MEMORY_DEBUG 1
#else
    #define    ALIB_MEMORY_DEBUG 0
#endif

namespace aworx { namespace lib { namespace memory {

/** ************************************************************************************************
 * Simple class that allocates a series of bigger memory blocks and provides sequential
 * allocation of portions of those. Furthermore, the allocations can be reset and the
 * allocated blocks can be reused.
 *
 * The minimum usable size of the blocks allocated is defined with constructor parameter
 * \p{minSize}. In most cases, the usable size will be slightly bigger than the given value.
 * If an invocation of an allocation method is requesting a bigger allocation, then a new block
 * with that given minimum size is created.
 *
 * \note
 *   While this class is successfully used with \alib internally, it is not considered finished
 *   or best in design, yet :-)
 **************************************************************************************************/
class MemoryBlocks
{
    // #############################################################################################
    // inner types
    // #############################################################################################
    protected:
        /** ****************************************************************************************
         * Internal type defining an allocated memory chunk. The meta information about the chunk
         * and its use (aka the members of this type itself) is also stored in the allocated chunk.
         * This enables to perform only one allocation.
         ******************************************************************************************/
        struct Block
        {
            Block*    next;        ///< the next block.
            int8_t*   mem;         ///< The allocated memory. This is in fact a <c>void*</c> but using a
                                   ///< byte allows arithmetics.
            size_t    size;        ///< The size of the block.
            size_t    left;        ///< The current fill.

            /** ************************************************************************************
             * Allocates a chunk of memory of a size so that an instance of this class as well as
             * the desired minimum size \p{minSize} plus alignment bytes are all fitting in.
             *
             * This memory is not freed with a destructor. Instead, the allocated chunk that
             * objects of this type reside in has to be deleted by invoking method #destruct.
             *
             * @param minSize  The minimum size (of the usable part) of the memory chunk to
             *                 allocate.
             * @return An new allocated and internally linked chunk of memory.
             **************************************************************************************/
            ALIB_API static Block* create( size_t minSize );

            /** ************************************************************************************
             * Deletes the allocated chunk and with it, this object itself.
             **************************************************************************************/
            void destruct()
            {
                Block* block= reinterpret_cast<Block*>( mem );
                block--;
                delete[] block;
            }

            /** ************************************************************************************
             * "Frees" all allocated memory. This method simply resets the fill marker of the
             * allocated chunk.
             **************************************************************************************/
            void reset()
            {
                left= size;
            }

            /** ************************************************************************************
             * Returns a pointer to an aligned piece of memory of the requested size inside this
             * block. If there is not enough space left, \c nullptr is returned.
             *
             * @param requestedSize The size to allocate.
             * @param alignment     The necessary alignment.
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
        /** The list of allocated memory blocks. */
        Block*              firstBlock;

        /** The actual block iterator. */
        Block*              actBlock;

        /** The standard allocation size. Blocks may be bigger in the case that
         *  a string added is longer than this value. */
        size_t              blockAllocationSize;

        #if ALIB_MEMORY_DEBUG
            /** The number of allocations performed.
                Availability depends on code selector symbol \ref ALIB_MEMORY_DEBUG. */
            size_t          dbgQtyAllocations;

            /** The number of allocated space.
                Availability depends on code selector symbol \ref ALIB_MEMORY_DEBUG. */
            size_t          dbgAllocSize;

            /** The number of space used for alignment.
                Availability depends on code selector symbol \ref ALIB_MEMORY_DEBUG. */
            size_t          dbgAlignmentWaste;


            /** The number of bytes not used in blocks.
                Availability depends on code selector symbol \ref ALIB_MEMORY_DEBUG. */
            size_t          dbgQtyUnusedByes;

            /** The number of allocations that have been larger than the block size.
                Availability depends on code selector symbol \ref ALIB_MEMORY_DEBUG. */
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
        ALIB_API    MemoryBlocks( size_t stdBlockSize );

        /** ****************************************************************************************
         * Destructor. Destructs all allocated blocks.
         ******************************************************************************************/
        ALIB_API    ~MemoryBlocks();

        /** ****************************************************************************************
         * This static method creates a memory block object inside it's first allocated block.
         * Objects created with this method, have to be deleted by only invoking the
         * destructor. This also deletes the pointer returned by this static method.
         *
         * Furthermore, method #Reset must not be called with objects created "in itself" with
         * this method.
         * (While this could be implemented, it was not, because usually block allocators created
         * with this method are considered for "one-way-use" only.)
         *
         * @param stdBlockSize The size of memory blocks allocated.
         * @return A \b MemoryBlocks object residing in its first created block.
         ******************************************************************************************/
        ALIB_API static MemoryBlocks* Create( size_t stdBlockSize );


   // #############################################################################################
   // Interface
   // #############################################################################################
   public:
        /** ****************************************************************************************
         * Allocates aligned memory of a given size. This method may be used if templated
         * method #Alloc is not applicable, for example if types and alignment is only
         * known at run-time.<br>
         * Otherwise, it is recommended to use a different allocation method offered by this class.
         *
         * @param size      The allocation size requested.
         * @param alignment The required alignment.
         * @return A pointer to the allocated memory.
         ******************************************************************************************/
        char*   AllocRaw( size_t size, size_t alignment)
        {
             return get( size, alignment );
        }

        /** ****************************************************************************************
         * Allocates aligned memory of size of type \p{T}.
         * The memory is not initialized.
         *
         * \see For a version that also constructs the type, see #Construct.
         *
         * @return A pointer to the allocated memory.
         ******************************************************************************************/
        template<typename T>
        inline T*   Alloc()
        {
            return reinterpret_cast<T*>( get( sizeof(T[1]), alignof(T) ) );
        }

        /** ****************************************************************************************
         * Allocates aligned memory of size of type \p{T} and performs a C++ "placement new"
         * passing the given arguments to the type's constructor.
         *
         * @param args Variadic parameters to be forwarded to constructor of type \p{T}.
         * @return A pointer to the allocated memory.
         ******************************************************************************************/
        template<typename T, typename... ConstructionArgs>
        inline T*   Construct( ConstructionArgs&& ... args )
        {
            T* mem= reinterpret_cast<T*>( get( sizeof(T), alignof(T) ) );
            return new (mem) T( std::forward<ConstructionArgs>( args )... );
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
            return reinterpret_cast<T*>( get( sizeof(T[1]) * static_cast<size_t>(length), alignof(T[]) ) );
        }

        /** ****************************************************************************************
         * Clears the allocation information. The allocated memory blocks will remain allocated
         * and reused.
         ******************************************************************************************/
        ALIB_API void    Reset();


        #if ALIB_MODULE_STRINGS
            /** ************************************************************************************
             * Returns a copy of the given string.
             * In case the string is empty, the original string is returned.
             *
             * @param src The source string to copy
             * @tparam TChar The character type of the input and output string.
             * @return A string object representing the copy.
             **************************************************************************************/
            template<typename TChar>
            inline
            strings::TString<TChar>  Clone( const strings::TString<TChar>& src )
            {
                if( !src.Length() )
                    return src;

                TChar* mem=  AllocArray<TChar>( src.Length() );
                src.CopyTo( mem );
                return strings::TString<TChar>(mem, src.Length() );
            }

            #if ALIB_MEMORY_DEBUG
                /** ********************************************************************************
                 * Provides allocation statistics for manual performance optimization.
                 * This method is available only if code selector symbol \ref ALIB_MEMORY_DEBUG is
                 * set.
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
         * Protected constructor taking a first block.
         * This constructor is used by static method #Create which uses a placement new to
         * create this object inside the block given.
         *
         * @param firstBlock   The first block already created outside.
         * @param stdBlockSize The size of memory blocks allocated.
         ******************************************************************************************/
        ALIB_API    MemoryBlocks( Block* firstBlock, size_t stdBlockSize );

        /** ****************************************************************************************
         * This internal allocation method is called by the allocation interface methods.
         * Internally, this method calls \alib{util::MemoryBlocks,Block::get}, which in turn
         * uses <c>std::align</c> to align the address.
         *
         * @param size      The size of the memory to allocate.
         * @param alignment The allocation alignment needed.
         * @return A pointer to the allocated memory.
         ******************************************************************************************/
        ALIB_API char*   get(size_t size, size_t alignment);
};

}}// namespace aworx[::lib::memory]

/// Type alias in namespace #aworx.
using     MemoryBlocks   =   aworx::lib::memory::MemoryBlocks;

} // namespace [aworx]

#endif // HPP_ALIB_MEMORY_MEMORYBLOCKS
