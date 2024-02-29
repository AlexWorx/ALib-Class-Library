/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_monomem of the \aliblong.
 *
 * \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_MONOMEM_MONOALLOCATOR
#define HPP_ALIB_MONOMEM_MONOALLOCATOR 1

#if !defined (HPP_ALIB_LANG_TMP) && !defined(ALIB_DOX)
#   include "alib/lang/tmp.hpp"
#endif

#if !defined (HPP_ALIB_MONOMEM_FWDS)
#   include "alib/monomem/fwds.hpp"
#endif

#if ALIB_THREADS
#   if  !defined(HPP_ALIB_THREADS_THREADLOCK)
#      include "alib/threads/threadlock.hpp"
#   endif
#endif

#if ALIB_STRINGS
#   if !defined(HPP_ALIB_STRINGS_STRING)
#       include "alib/strings/string.hpp"
#   endif
#endif


#if !defined(_GLIBCXX_CSTDLIB) && !defined(_CSTDLIB_)
#   include <cstdlib>
#endif

#if !defined (_GLIBCXX_MEMORY) && !defined(_MEMORY_)
#   include <memory>
#endif

#if !defined (_GLIBCXX_CSTDDEF) && !defined (_CSTDDEF_)
#   include <cstddef>
#endif



namespace alib {  namespace monomem {


/** ************************************************************************************************
 * Implements a monotonic allocator. Allocates a series of bigger memory chunks and offers
 * sequential allocation of portions of those.
 * The allocations can be reset to a certain state (see #TakeSnapshot), which allows to reuse
 * the allocated chunks for future sequential allocations.
 *
 * The size of the chunks allocated is defined with constructor parameters
 * \p{initialChunkSize}. This value is reduced by #MaxUsableSpaceLoss and stored in field
 * #nextChunksUsableSize. With each allocation of a chunk this value can be increased.
 * Constructor parameter \p{chunkGrowthInPercent} defaults to 200, which doubles the next
 * chunk size.
 *
 * If an invocation of one of the allocation methods requests memory bigger than the
 * remaining space in the actual (last) chunk, then a new chunk is created and made the actual
 * chunk.
 * The remaining space of the former actual chunk will not be used for future allocations and is
 * lost in this respect, except for the following exclamation.
 *
 * If a requested allocation size exceeds #nextChunksUsableSize, then a new chunk with the requested size
 * is created, stored in the list of allocated chunks and the current chunk remains in use.
 * With that, requested allocations are allowed to have a bigger size than the standard
 * chunk size given in the constructor.<br>
 *
 * Depending on compiler symbol \ref ALIB_DEBUG_MONOMEM some metrics on instances of this class
 * become available. Those might for example be helpful to find a reasonable value for constructor
 * parameter \p{initialChunkSize}.
 **************************************************************************************************/
class MonoAllocator
{
    #if !defined(ALIB_DOX)
        template<typename TContained>
        friend
        class SelfContained;
    #endif

    // #############################################################################################
    // inner types
    // #############################################################################################
    protected:
        /** ****************************************************************************************
         * Internal type defining an allocated chunk of memory.
         * The allocation space is found behind this object itself as it is placed at the start
         * of each allocated chunk.
         ******************************************************************************************/
        struct Chunk
        {
            Chunk*    previous;  ///< the previously allocated chunk.
            char*     act;       ///< Pointer to the free space in the chunk.
            char*     end;       ///< Pointer to the first byte behind the chunk.

            /** ************************************************************************************
             * Allocates a chunk of memory of a size so that an instance of this class as well as
             * the desired minimum size \p{minSize} plus alignment bytes are all fitting in.
             *
             * This memory is not freed with a destructor. Instead, the allocated chunk that
             * objects of this type reside in has to be deleted by invoking method #destruct.
             *
             * @param size  The minimum size (of the usable part) of the chunk of memory
             *                           allocate, taking the maximum possible alignment space
             *                           into account.
             * @return An new allocated and internally linked chunk of memory.
             **************************************************************************************/
            ALIB_FORCE_INLINE
            static
            Chunk* create( size_t size )
            {
                ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
                size+= MonoAllocator::MaxUsableSpaceLoss();
                Chunk*  chunk= new (malloc(size)) Chunk();
                chunk->act = reinterpret_cast<char*>( chunk + 1 );
                chunk->end = reinterpret_cast<char*>( chunk ) + size;
                ALIB_WARNINGS_RESTORE

                return chunk;
            }

            /** ************************************************************************************
             * Defaulted default constructor
             **************************************************************************************/
            Chunk()                                                                       = default;

            /** ************************************************************************************
             * Deletes the allocated chunk and with it, this object itself.
             **************************************************************************************/
            void destruct()
            {
                free( this );
            }

            /** ************************************************************************************
             * "Frees" all allocated memory, by simply resetting the fill marker of the
             * this chunk to the first usable byte of the allocated chunk.
             **************************************************************************************/
            void reset()
            {
                ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
                act= reinterpret_cast<char*>( this + 1 );
                ALIB_WARNINGS_RESTORE
            }

            /** ************************************************************************************
             * Returns a pointer to an aligned piece of memory of the requested size inside this
             * chunk. If there is not enough space left, \c nullptr is returned.
             *
             * @param requestedSize The size to allocate.
             * @param alignment     The necessary alignment.
             * @return \c nullptr on failure, otherwise pointer to the requested memory.
             **************************************************************************************/
            ALIB_FORCE_INLINE
            char* alloc( size_t requestedSize, size_t alignment )
            {
                void*   result= act;
                size_t freeSpace= static_cast<size_t>( end-act );
                ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
                if( !std::align( alignment, requestedSize, result, freeSpace) )
                    return nullptr;
                act= reinterpret_cast<char*>( result ) + requestedSize;
                return reinterpret_cast<char*>( result );
                ALIB_WARNINGS_RESTORE
            }

            /** ************************************************************************************
             * Templated version of #alloc(size_t, size_t). Used with sizes and alignment
             * known at compile-time.
             *
             * @tparam requestedSize The size to allocate.
             * @tparam alignment     The necessary alignment.
             * @return \c nullptr on failure, otherwise pointer to the requested memory.
             **************************************************************************************/
            template<size_t requestedSize, size_t alignment>
            ALIB_FORCE_INLINE
            char* alloc()
            {
                void*   result= act;
                size_t freeSpace= static_cast<size_t>( end-act );
                if( !std::align( alignment, requestedSize, result, freeSpace) )
                    return nullptr;
                act= reinterpret_cast<char*>( result ) + requestedSize;

                return reinterpret_cast<char*>( result );
            }

        }; // struct Chunk

        /**
         * The maximum amount of bytes that are lost by the fact that a chunk's data is
         * stored at the start of the chunk.
         * This is the sum of the chunk's size plus the waste caused by aligning an object of
         * maximum possible alignment, if allocated as a first object behind a chunk's management
         * data.
         *
         * @return The maximum space loss in allocated chunks.
         */
        static constexpr size_t MaxUsableSpaceLoss()
        {
            return     sizeof(Chunk)
                     + sizeof(Chunk) % alignof(std::max_align_t);
        }


    // #############################################################################################
    // protected fields
    // #############################################################################################
    protected:
        /** The actual chunk. Contains a link to previously allocated chunks.*/
        Chunk*              chunk;

        /** The list of chunks that are to be recycled. */
        Chunk*              recyclables;

        /**
         * The initial allocation size given in the constructor minus the maximum overhead
         * caused by storing a chunk's management data inside the chunks themselves.
         * In other words, this field stores the value given in the constructor, minus the value
         * returned by \alib{monomem::MonoAllocator,MaxUsableSpaceLoss}.
         *
         * Allocated chunks may be bigger in the case that a single allocation is larger than this
         * value.
         */
        size_t              nextChunksUsableSize;

        /**
         * Growth factor of subsequently allocated chunks.
         * Given by a construction parameter, which defaults to 200, which doubles chunk size
         * with each next chunk allocation.
         */
        unsigned int        chunkGrowthInPercent;

    public:
        #if ALIB_DEBUG_MONOMEM
            /**
             * Debug statistics.<br>
             * Availability depends on code selector symbol \ref ALIB_DEBUG_MONOMEM.
             */
            struct DbgStatistics
            {
                /** The number of allocations performed. */
                size_t          QtyAllocations                                                  = 0;

                /** The number of allocations that did not create a new chunk . */
                size_t          QtyTrivialAllocations                                           = 0;

                /** The number of created chunks.      */
                size_t          QtyChunks                                                       = 0;

                /** The number of bytes allocated at the heap.      */
                size_t          HeapSize                                                        = 0;

                /** The number of allocated space. */
                size_t          AllocSize                                                       = 0;

                /** The number of bytes lost for alignment. */
                size_t          AlignmentWaste                                                  = 0;

                /** The number of bytes remaining in chunks. */
                size_t          ChunkWaste                                                      = 0;

                /** The number of allocations that have been larger than the chunk size. */
                size_t          QtyChunkSizeExceeds                                             = 0;

                /** The number of resets performed. */
                size_t          QtyResets                                                       = 0;
            };

            /** Debug statistics measured on the whole run-time of this object.<br>
             *  Availability depends on code selector symbol \ref ALIB_DEBUG_MONOMEM.       */
            DbgStatistics    DbgStats;

            /**
             * Sub-domain for debug log output of this class. Could be changed to allow
             * fine grained log-output selection. The following domains are set by \alib
             * types:
             *
             * Type                            |  Sub-Domain
             * ------------------------------  |------------------------------------------------------
             * - \alib{monomem,GlobalAllocator}| <c>"MA/GLBL"</c>
             * - \alib{config,InMemoryPlugin}  | <c>"MA/CFG/IMPLGN"</c>
             * - \alib{config,Variable}        | <c>"MA/CFG/VAR"</c>
             * - \alib{lox,Lox}                | <c>"MA/ALOX/LOX"</c>
             * - \alib{expressions,Compiler}   | <c>"MA/EXPR/CMPLR"</c>
             * - \alib{expressions,Scope}      | <c>"MA/EXPR/SCP"</c>, respectively <c>"MA/EXPR/CTSCP"</c>
             * - \alib{expressions::plugins,Calculus}   | <c>"MA/EXPR/CLCLS"</c>
             *
             *  Availability depends on code selector symbol \ref ALIB_DEBUG_MONOMEM.
             */
            String           LogDomain                                               = A_CHAR("MA");


        #endif

    // #############################################################################################
    // Snapshot
    // #############################################################################################
    public:
        /**
         * Stores the actual state of outer class \b %MonoAllocator.
         * Retrieved method \alib{monomem,MonoAllocator::TakeSnapshot} and
         * \alib{monomem,MonoAllocator::Reset(const Snapshot&)}.
         */
        class Snapshot
        {
            private:
                #if !defined(ALIB_DOX)
                    friend class MonoAllocator;
                #endif

                /** The current chunk. */
                Chunk*      chunk;

                /** Pointer to the first free byte in the current chunk */
                char*       actFill;

            public:
                /**
                 * Internal constructor which creates a snapshot from the current allocator.
                 * @param allocator   The current chunk.
                 */
                Snapshot( MonoAllocator* allocator )
                : chunk  ( allocator->chunk )
                , actFill( chunk ? allocator->chunk->act : nullptr)
                {}

                /**
                 * Default constructor.
                 * \note
                 *   Default-constructed snapshots passed to method #Reset(const Snapshot&) reset
                 *   the whole of the monotonic allocator.
                 */
                Snapshot()
                : chunk( nullptr)
                {}

                /**
                 * Returns \c false if this snapshot was never initialized properly (default
                 * constructed and not copied over.
                 * @return \c true if this is not a valid snapshot, \c false otherwise.
                 */
                bool IsValid()
                {
                    return chunk != nullptr;
                }
        };

   // #############################################################################################
   // Constructor / Destructor
   // #############################################################################################
   public:
        /** ****************************************************************************************
         * Constructor.
         * #MaxUsableSpaceLoss is subtracted from given \p{initialChunkSize} and stored in field
         * #nextChunksUsableSize.
         * @param chunkGrowthInPercent  Optional growth factor in percent (*100), applied to each
         *                              allocation of a next chunk size.
         *                              Values provided should be greater than 100.<p>
         *                              Defaults to 200, which doubles chunk size with each
         *                              next internal chunk allocation.
         * @param initialChunkSize      The standard size of memory chunks that are dynamically allocated.
         ******************************************************************************************/
        ALIB_API    MonoAllocator( size_t initialChunkSize, unsigned int chunkGrowthInPercent= 200 );


        /** ****************************************************************************************
         * Destructor. Frees all allocated memory chunks.
         ******************************************************************************************/
        ALIB_API    ~MonoAllocator();

        /** ****************************************************************************************
         * This static method creates an object of this type inside "itself", aka inside its first
         * allocated chunk.
         * Objects created with this method, have to be deleted by only invoking the
         * destructor, which also deletes the object the returned pointer refers to.
         *
         * The parameterless version of method #Reset must not be called with objects created
         * by this method. Instead, if reset operations are desired, a snapshot has to be
         * taken (see method #TakeSnapshot) right after the invocation of this method (aka before
         * allocations are performed) which then has to be passed to the overloaded method
         * \b Reset(const Snapshot&).
         *
         * @param initialChunkSize      The size of memory chunks allocated.
         * @param chunkGrowthInPercent  Optional growth factor in percent (*100), applied to each
         *                              allocation of a next chunk size.
         *                              Values provided should be greater than 100.<p>
         *                              Defaults to 200, which doubles chunk size with each
         *                              next internal chunk allocation.
         * @return A \b MonoAllocator object residing in its first created chunk.
         ******************************************************************************************/
        ALIB_API static MonoAllocator* Create( size_t       initialChunkSize,
                                               unsigned int chunkGrowthInPercent= 200 );


        #if ALIB_THREADS && ALIB_CAMP && !defined(ALIB_DOX)
            ALIB_API void dbgCheckGlobalAllocatorLock();
        #endif

   // #############################################################################################
   // Interface
   // #############################################################################################
   public:
        /** ****************************************************************************************
         * Allocates memory of requested size and alignment.
         * This method may be used if templated method #Alloc is not applicable.
         *
         * @param size      The allocation size requested.
         * @param alignment The required alignment.
         * @return A pointer to the allocated memory.
         ******************************************************************************************/
        ALIB_FORCE_INLINE
        char*           Alloc( size_t size, size_t alignment)
        {
            #if ALIB_THREADS && ALIB_CAMP
                dbgCheckGlobalAllocatorLock();
            #endif

            #if ALIB_DEBUG_MONOMEM
                ++DbgStats.QtyAllocations;
                DbgStats.AllocSize += size;
                size_t qtyLeftBeforeAlloc= chunk ? static_cast<size_t>(chunk->end - chunk->act)
                                                 : 0;
            #endif

            char* mem;

            if( chunk )
            {
                mem = chunk->alloc(size, alignment);
                if ( mem )
                {
                    #if ALIB_DEBUG_MONOMEM
                        DbgStats.AlignmentWaste += qtyLeftBeforeAlloc
                            - static_cast<size_t>(chunk->end - chunk->act)
                            - size;
                        DbgStats.QtyTrivialAllocations++;
                    #endif
                    return mem;
                }
            }

            return getCreateChunk( size, alignment );
        }

        /** ****************************************************************************************
         * Allocates aligned memory suitable to emplace an instance of \p{T}.
         *
         * \see For a version that also constructs the type, see #Emplace.
         *
         * @tparam T The type of object to allocated memory for.
         * @return A pointer to the allocated memory.
         ******************************************************************************************/
        template<typename T>
        ALIB_FORCE_INLINE
        T*              Alloc()
        {
            return reinterpret_cast<T*>( Alloc( sizeof(T), alignof(T) ) );
        }

        /** ****************************************************************************************
         * Allocates aligned memory for an array of objects of type \p{T} of size \p{length}.
         *
         * \see
         *   Method #EmplaceArray for an alternative that initializes the memory.
         *
         * @tparam T     The array element type.
         * @tparam TSize The type of the array length value. This template parameter is provided
         *               for convenience, to avoid casts from singed types. It is deduced by the
         *               compiler and not needed to be provided.
         * @param length The capacity of the requested array.
         * @return A pointer to the first element of the allocated array.
         ******************************************************************************************/
        template<typename T, typename TSize>
        ALIB_FORCE_INLINE
        T*              AllocArray( TSize length )
        {
            return reinterpret_cast<T*>( Alloc( sizeof(T[1]) * static_cast<size_t>(length),
                                                alignof(T[])                                ) );
        }

        /** ****************************************************************************************
         * Allocates aligned memory of size and alignment suitable for type \p{T} and performs a
         * C++ "placement new", passing the given arguments to the type's constructor.
         *
         * @tparam T     Type to emplace in this monotonic allocator.
         * @tparam TArgs Types of variadic parameters given with parameter \p{args}.
         * @param  args  Variadic parameters to be forwarded to constructor of type \p{T}.
         * @return A pointer to the allocated memory.
         ******************************************************************************************/
        template<typename T, typename... TArgs>
        ALIB_FORCE_INLINE
        T*              Emplace( TArgs&& ... args )
        {
            return new (Alloc<T>())   T( std::forward<TArgs>( args )... );
        }

        /** ****************************************************************************************
         * Allocates aligned memory for an array of objects of type \p{T} of size* \p{length}.
         * Array members are initialized using a "placement new" passing the given \p{args} to
         * the type's constructor.
         *
         * \see
         *   Method #AllocArray for an alternative that does not initialize the memory.
         *
         * @tparam T      The array element type.
         * @tparam TArgs  Types of variadic parameters given with parameter \p{args}.
         * @tparam TSize  The type of the array length value. This template parameter is provided
         *                for convenience, to avoid casts from singed types. It is deduced by the
         *                compiler and not needed to be provided.
         * @param  length The capacity of the requested array.
         * @param  args   Variadic parameters to be forwarded to constructor of each array element
         *                of type \p{T}.
         * @return A pointer to the first element of the allocated array.
         ******************************************************************************************/
        template<typename T, typename TSize, typename... TArgs>
        T*              EmplaceArray( TSize length, TArgs&& ... args )
        {
            ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
            T* mem= AllocArray<T, TSize>( length );
            ALIB_WARNINGS_RESTORE

            for( TSize i= 0 ; i < length ; ++i )
                new (mem + i) T(std::forward<TArgs>( args )...);

            return mem;
        }

        /** ****************************************************************************************
         * Saves the current state of the allocator and returns this information as a \b Snapshot
         * value. Such snapshots may be passed to method #Reset(const Snapshot&).
         *
         * Note that the actual memory is \b not copied and restored. In this respect the word
         * "Snapshot" is overstating. What is stored are the current use of memory, but not it's
         * contents.
         *
         * @return A (lightweight) snapshot value object.
         ******************************************************************************************/
        Snapshot        TakeSnapshot()
        {
            return Snapshot(this);
        }

        /** ****************************************************************************************
         * Resets this allocator to the given \alib{monomem::MonoAllocator,Snapshot}.
         * Parameter \p{snapshot} is defaulted with a default-constructed \b Snapshot, which
         * completely resets the allocator.
         *
         * With a reset, the memory chunks which had been allocated after taking the given
         * \p{snapshot}, are not freed, but re-used with future monotonic allocations.
         *
         * This method is useful in cases where some permanent objects which are allocated first
         * have to be preserved with resets.
         * It can also be used to preserve a self-contained monotonic allocator created with
         * static method #Create.
         *
         * Note that snapshots taken after the given one, become invalid. This is because
         * class \b Snapshot is only a simple lightweight class that marks the currently
         * used chunk and its fill level.
         *
         * @param snapshot The snapshot to reset to.
         ******************************************************************************************/
        ALIB_API void   Reset( const Snapshot& snapshot= Snapshot() );

        #if ALIB_STRINGS
            /** ************************************************************************************
             * Returns a copy of the given string.
             *
             * \note
             *   In case \p{src} is empty, the a copy of the original string is
             *   returned (aka a string that points to the same buffer that was given).
             *   This allows to have a sort of "valid" pointer, even though the buffer of \p{src}
             *   might be a temporary.
             *
             * @tparam TChar The character type of the input and output string.
             * @param  src   The source string to copy
             * @return A string object representing the copy.
             **************************************************************************************/
            template<typename TChar>
            strings::TString<TChar>  EmplaceString( const strings::TString<TChar>& src )
            {
                if( !src.Length() )
                    return src;

                TChar* mem=  AllocArray<TChar>( src.Length() );
                src.CopyTo( mem );
                return strings::TString<TChar>(mem, src.Length() );
            }

            #if ALIB_DEBUG_MONOMEM
                /** ********************************************************************************
                 * Provides allocation statistics for manual performance optimization.
                 * This method is available only if code selector symbol \ref ALIB_DEBUG_MONOMEM is
                 * set.
                 *
                 * @return Some textual information on the allocation statistics.
                 **********************************************************************************/
                ALIB_API NAString DbgDumpStats();
            #endif

        #endif

    // #############################################################################################
    // Internals
    // #############################################################################################
    protected:
        /** ****************************************************************************************
         * Protected constructor taking a first chunk of memory that was allocated externally.
         * This constructor is used by static method #Create which uses <em>placement new</em> to
         * create this allocate inside the chunk given.
         *
         * @param firstChunk            The first chunk already created outside.
         * @param initialChunkSize      The size of the first chunk allocated.
         * @param chunkGrowthInPercent  Growth factor in percent (*100), applied to each
         *                              allocation of a next chunk size.
         ******************************************************************************************/
        ALIB_API    MonoAllocator( Chunk* firstChunk,
                                   size_t initialChunkSize, unsigned int chunkGrowthInPercent );

        /** ****************************************************************************************
         * This internal allocation method is called by the allocation interface methods, in case
         * the current request can not be trivially satisfied.
         *
         * Implements the overall strategy of this class in respect to oversized blocks,
         * recycling of blocks, etc.
         *
         * @param size      The size of the memory to allocate.
         * @param alignment The allocation alignment needed.
         * @return A pointer to the allocated memory.
         ******************************************************************************************/
        ALIB_API char*   getCreateChunk(size_t size, size_t alignment);
}; // class MonoAllocator


// #################################################################################################
// Namespace Functions
// #################################################################################################
/** ************************************************************************************************
 * This static inline namespace function calls the destructor <b>~T()</b>of given \p{object}.
 * The use of this method is recommended instead of calling the destructor directly,
 * to increase readability of the code.
 *
 * @tparam T        The object type. Deduced by the compiler and not needed to be given.
 * @param object    The object to destruct.
 **************************************************************************************************/
template<typename T>
static ALIB_FORCE_INLINE
void     Destruct(T* object)
{
    object->~T();
}



#if defined(ALIB_DOX)
/** ************************************************************************************************
 * Simple inline function that acquires \alib{monomem,GlobalAllocatorLock} and returns the object
 * singleton \alib{monomem,GlobalAllocator}.
 *
 * \see
 *  - Chapter \ref alib_monomem_globalinstance "7. The Global MonoAllocator" of the Programmer's
 *    Manual of this \alibmod_nl.
 *  - Macro \ref ALIB_CALLER_PRUNED that is recommended to be used to pass the parameters to
 *    this method.
 *
 * \see Namespace function #ReleaseGlobalAllocator.
 *
 * @param dbgFile  Caller information. Available only with debug builds and if
 *                 \alib_threads is included in the \alibdist.
 * @param dbgLine  Caller information. Available only with debug builds and if
 *                 \alib_threads is included in the \alibdist.
 * @param dbgFunc  Caller information. Available only with debug builds and if
 *                 \alib_threads is included in the \alibdist.
 * @return A reference to the global monotonic allocator instance
 *         \alib{monomem,AcquireGlobalAllocator}.
 **************************************************************************************************/
inline
MonoAllocator&          AcquireGlobalAllocator( const NCString&  dbgFile,
                                                int              dbgLine,
                                                const NCString&  dbgFunc  );

/** ************************************************************************************************
 * Simple inline function that releases \alib{monomem,GlobalAllocatorLock}, previously acquired
 * with \alib{monomem,AcquireGlobalAllocator}.
 **************************************************************************************************/
inline
void                    ReleaseGlobalAllocator();

/**
 * This is the global monotonic allocator singleton instance.
 *
 * \see
 *  - Chapter \ref alib_monomem_globalinstance "7. The Global MonoAllocator" of the Programmer's
 *    Manual of this \alibmod_nl.
 *  - \b ThreadLock instance \alib{monomem::GlobalAllocatorLock}
 *  - Functions \alib{monomem,AcquireGlobalAllocator} and \alib{monomem,ReleaseGlobalAllocator}.
 */
extern ALIB_API
MonoAllocator           GlobalAllocator;

/**
 * This <em>mutex</em> is used by methods \alib{monomem,AcquireGlobalAllocator} and
 * \alib{monomem,ReleaseGlobalAllocator} to control access over the global allocator instance
 * \alib{monomem,GlobalAllocator}.
 *
 * This object may be controlled directly as an alternative to using the functions named above, for
 * example in combination with macro \ref ALIB_LOCK_WITH, with attaches the locking/unlocking
 * mechanisms "automatically" to the current call stack.
 */
extern ALIB_API
ThreadLock              GlobalAllocatorLock;

#else // !defined(ALIB_DOX)

extern ALIB_API
MonoAllocator           GlobalAllocator;

#if ALIB_THREADS
    extern ALIB_API
    ThreadLock          GlobalAllocatorLock;

    inline
    MonoAllocator&      AcquireGlobalAllocator( ALIB_DBG(const NCString& file, int line, const NCString& func) )
    {
        GlobalAllocatorLock.Acquire( ALIB_DBG(file,line,func) );
        return GlobalAllocator;
    }

    inline void         ReleaseGlobalAllocator()
    {
        GlobalAllocatorLock.Release();
    }

#else
    inline
    MonoAllocator&      AcquireGlobalAllocator(ALIB_DBG(const char* , int , const char* ) )
    {
        ALIB_DBG( DbgCheckSingleThreaded(); )
        return GlobalAllocator;
    }
    inline
    void                ReleaseGlobalAllocator()
    {}
#endif // ALIB_THREADS

#endif // !defined(ALIB_DOX)

} // namespace alib[::monomem]

/// Type alias in namespace \b alib.
using     MonoAllocator =   monomem::MonoAllocator;

} // namespace [alib]

#endif // HPP_ALIB_MONOMEM_MONOALLOCATOR
