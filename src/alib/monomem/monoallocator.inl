//==================================================================================================
/// \file
/// This header-file is part of module \alib_monomem of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#if ALIB_DEBUG_ALLOCATIONS && !ALIB_STRINGS
#   error "Module ALib Strings needed in the ALib Build if Monomem is included and ALIB_DEBUG_ALLOCATIONS is set."
#endif

ALIB_EXPORT namespace alib {  namespace monomem {

// forward declaration
template<typename TAllocator>   class TMonoAllocator;

/// Details of namespace #alib::monomem
namespace detail {

//==================================================================================================
/// Internal type defining a memory buffer.
/// The allocation space is found behind this object itself as it is placed at the start
/// of each allocated buffer.
//==================================================================================================
struct Buffer
{
    /// A magic byte, used with the compiler-symbol \ref ALIB_DEBUG_ALLOCATIONS to mark
    /// memory and detect out-of-bounds writes.
    /// @see Method \alib{lang;Allocator::dbgCheckMemory}.
    static constexpr unsigned char     MAGIC= 0xA1;

    /// A magic byte written when memory is freed.
    /// @see Field #CLEAR.
    static constexpr unsigned char     CLEAR= 0xF1;

    char*     act;       ///< Pointer to the next free space in the buffer.
    char*     end;       ///< Pointer to the first byte behind the buffer.
    Buffer*   previous;  ///< the previously allocated buffer.

    /// Returns the first offset of an object with given \p{alignment} behind an object with.
    /// smaller alignment.
    /// @param firstObject The allocation size of the first object allocated in a buffer.
    /// @param alignment   The allocation alignment of the first object allocated in a buffer.
    /// @return The value to add to the allocation size to ensure that an object with a certain
    ///         alignment fits after placing \p{firstObject} at the start..
    static constexpr size_t firstOffset(size_t firstObject, size_t alignment)
    {
        return ((firstObject + alignment - 1) / alignment) * alignment;
    }

    /// Initializes the members of this buffer to reflect the given \p{size}.
    /// @param size  The size of the given \p{mem}.
    Buffer( size_t size )
    {
        act = reinterpret_cast<char*>( this + 1 );
        end = reinterpret_cast<char*>( this ) + size;
    }

    //==============================================================================================
    /// Defaulted default constructor
    //==============================================================================================
    Buffer()                                                                       = default;

    /// @return The size of this buffer.
    size_t Size()    {  return size_t(end - reinterpret_cast<char*>(this)) ;  }

    //==============================================================================================
    /// "Frees" all allocated memory, by simply resetting the fill marker of the
    /// this buffer to the first usable byte of the allocated buffer.
    //==============================================================================================
    void reset()
    {
        act= reinterpret_cast<char*>( this + 1 );
        #if ALIB_DEBUG_ALLOCATIONS
            memset( act, 0xD2, size_t(end - act));
        #endif
    }

    //==============================================================================================
    /// Returns a pointer to an aligned piece of memory of the requested size inside this
    /// buffer. If there is not enough space left, \c nullptr is returned.
    ///
    /// @param size       The size to allocate.
    /// @param alignment  The necessary alignment.
    /// @return \c nullptr on failure, otherwise pointer to the requested memory.
    //==============================================================================================
    char* allocate( size_t size, size_t alignment )
    {
        ALIB_ASSERT_ERROR( lang::BitCount(alignment) == 1, "MONOMEM",
                           "Requested alignment is not a power of 2:", alignment )

        size_t dbgSize= lang::DbgAlloc::extSize(size);

        // Calc aligned start. Fits to this buffer? If not, just return nullptr
        char*  aligned   = reinterpret_cast<char*>( (size_t(act) + alignment - 1) & ~(alignment -1) );
        if( size_t(end - aligned) < dbgSize )
            return nullptr;
        act= aligned + dbgSize;

        lang::DbgAlloc::annotate(aligned, size, MAGIC);
        return aligned;
    }

}; // struct Buffer


} // namespace alib::momomem[::detail]

//==================================================================================================
/// Stores the actual state of outer class \b %MonoAllocator.
/// Retrieved method \alib{monomem;MonoAllocator::TakeSnapshot} and
/// \alib{monomem;MonoAllocator::Reset(Snapshot)}.
//==================================================================================================
class Snapshot
{
  protected:
    #if !DOXYGEN
        template<typename TAllocator>   friend class  TMonoAllocator;
    #endif

    detail::Buffer* buffer;     ///< The current buffer.
    char*           actFill;   ///< Pointer to the first free byte in the current buffer.

    /// Internal constructor.
    /// @param pBuffer  The current buffer.
    /// @param pFill    The current fill of \p{buffer}.
    constexpr Snapshot( detail:: Buffer* pBuffer, char* pFill)                                noexcept
    : buffer  (pBuffer)
    , actFill(pFill )                                                                             {}

  public:
    /// Default constructor.
    /// \note
    ///  Default-constructed snapshots passed to method \alib{monomem;MonoAllocator::Reset(Snapshot)}
    ///  do reset the monotonic allocator to its initial state after construction. All monotonic
    ///  allocated memory is considered 'freed' then.
    constexpr Snapshot()                                                                    noexcept
    : buffer (nullptr)
    , actFill(nullptr)                                                                            {}

    /// Returns \c false if this snapshot was never initialized properly (default
    /// constructed and not copied over).
    /// @return \c true if this is not a valid snapshot, \c false otherwise.
    constexpr bool IsValid()                                                                noexcept
                                                                       { return buffer != nullptr; }

}; // class Snapshot

/// Statistics for type \alib{monomem;TMonoAllocator}, receivable with method
/// \alib{monomem::TMonoAllocator;GetStatistics}.
/// @see Struct \alib{monomem;DbgStatistics} and corresponding method
///      \alib{monomem::TMonoAllocator;DbgGetStatistics}.
struct Statistics
{
    /// The number of created buffers.
    unsigned int    QtyBuffers                                                              = 0;

    /// The number of created buffers.
    unsigned int    QtyRecyclables                                                          = 0;

    /// The number of bytes allocated at the heap.
    size_t          HeapSizeRecycled                                                        = 0;

    /// The number of bytes allocated at the heap from buffers currently used.
    /// Note: To get the total size, add #HeapSizeRecycled.
    size_t          HeapSize                                                                = 0;

    /// The overall number of bytes requested. Note that this value includes the
    /// losses due to alignment. To get the exact value, method
    /// \alib{monomem::TMonoAllocator;DbgGetStatistics} needs to be used and its
    /// information about the alignment waste has to be subtracted.
    size_t          AllocSize                                                               = 0;

    /// The number of bytes remaining in buffers, because a next object did not fit. This
    /// does not include the remaining bytes in the current buffer.
    size_t          BufferWaste                                                             = 0;

    /// The free space in the current buffer.
    size_t          CurrentBufferFree                                                       = 0;

    /// The size of the current buffer.
    size_t          CurrentBufferSize                                                       = 0;

    /// The planned size of the next buffer (that is not an oversize-allocation).
    size_t          NextBufferSize                                                          = 0;

};  // struct Statistics

#if ALIB_DEBUG_MEMORY
    /// Debug statistics for type \alib{monomem;TMonoAllocator}, receivable with method
    /// \alib{monomem::TMonoAllocator;DbgGetStatistics}.
    /// Availability depends on code selector symbol \ref ALIB_DEBUG_MEMORY.
    struct DbgStatistics
    {
        /// The number of allocations performed.
        size_t          QtyAllocations                  = 0;

        /// The number of allocations performed, cumulated over resets.
        size_t          QtyAllocationsInclResets        = 0;

        /// The number of allocations that did not create a new buffer .
        size_t          QtyTrivialAllocations           = 0;

        /// The number of allocations that did not create a new buffer, cumulated over resets.
        size_t          QtyTrivialAllocationsInclResets = 0;

        /// The number of allocated space, cumulated over resets.
        size_t          AllocSizeInclResets             = 0;

        /// The number of bytes lost for alignment.
        size_t          AlignmentWaste                  = 0;

        /// The number of allocations that have been larger than the buffer size.
        size_t          QtyBufferSizeExceeds            = 0;

        /// The number of resets performed.
        size_t          QtyResets                       = 0;
    };  // struct DbgStatistics
#endif


// =================================================================================================
/// \par Important Note
///   Please consult the \ref alib_contmono_intro "Programmer's Manual" of \alib module
///   \alib_monomem_nl for an introduction into the rather complex topic of monotonous allocation
///   and how to use this class.
///
/// \par General Behavior
/// Implements a monotonic allocator. Allocates a series of bigger memory buffers and offers
/// sequential allocation of portions of those.<br>
/// With construction, an initial memory buffer is received from the
/// \ref alib_contmono_chaining "chained allocator".
/// The size of this buffer is defined with the constructor parameter \p{initialBufferSizeInKB}.
/// With each necessary allocation of a next buffer, this value can be increased, according
/// to the optional parameter \p{bufferGrowthInPercent}, which defaults to \c 200, resulting in
/// doubling the size of each next buffer.
///
/// If an invocation of one of the allocation methods requests memory bigger than the
/// remaining space in the actual (last) buffer, then a new buffer is created and made the actual
/// buffer. The remaining space of the former actual buffer will not be used for future allocations
/// and is wasted in this respect.
///
/// \par Oversized Allocations
/// If a requested allocation size exceeds what would be the size of the next buffer, then a new
/// buffer of the extended size is created. The next allocation will then create a new buffer
/// which continues the originally defined buffer growth path. In other words:
/// - Oversized allocations are allowed, and
/// - the next buffer's size after an oversized allocation will use the size of the
///   last non-oversized buffer as the base value for calculating the next size.
///
/// \note While accepted, oversized allocations should still be avoided, because the current
///       buffer will not be used for further allocations.
///       This is a design decision, allowing a more efficient implementation of this class.
///       Former implementations kept the current buffer as the actual buffer, when an oversized
///       allocation occured, but this feature was dropped.
///
/// \par Resetting
///   The allocator can be reset to a certain state (see #TakeSnapshot and #Reset), which disposes
///   all memory behind the snapshot.
///   Disposed complete buffers will not be freed, but "recycled"  with future allocations.<br>
///   If \ref alib_contmono_class_monoalloc_snapshot "nested snapshots" are used in the right
///   places, further gains in execution performance can be achieved.
///
/// \par External Allocation of the First Buffer
/// Special constructors of this type allow to pass an existing first buffer for usage.
/// This allows two fundamental concepts:
/// - The first buffer might be stack-memory, which can lead to further (sometimes dramatical)
///   performance improvements.<br>
///   This feature is leveraged by type \alib{monomem;TLocalAllocator}.
/// - The first buffer might contain not only already emplaced objects, but even the
///   \b %TMonoAllocator itself. We name this a <em>"self-contained monotonic allocator"</em>.<br>
///   This feature is leveraged by type \alib{monomem;TSharedMonoVal}.
///
/// \par Debug Features
/// Depending on the compiler-symbol \ref ALIB_DEBUG_MEMORY, some metrics on instances of this
/// class become available.
/// Those might for example be helpful to find a reasonable value for constructor parameters
/// \p{initialBufferSize} and \p{bufferGrowthInPercent}.
///
/// \par See also
/// - The \ref alib_contmono_intro "Programmer's Manual" of module \alib_monomem_nl.
/// - The "standard" type definition of this template type \alib{MonoAllocator}, which uses
///   an \alib{lang;HeapAllocator} as the \ref alib_contmono_chaining "chained allocator".
///
/// @tparam TAllocator The underlying allocator to use.<br>
///                    With type definition \alib{MonoAllocator}, this is bound to type
///                    \alib{lang;HeapAllocator}.<br>
///                    For general information see chapter \ref alib_contmono_chaining
///                    of the Programmer's Manual.
// =================================================================================================
template<typename TAllocator>
class TMonoAllocator  : public lang::AllocatorMember<TAllocator>
{
  protected:
    /// The allocator type that \p{TAllocator} specifies.
    using allocMember= lang::AllocatorMember<TAllocator>;

    /// The actual buffer. Contains a link to previously allocated buffers.
    detail::Buffer*             buffer;

    /// The list of buffers that are to be recycled.
    detail::Buffer*             recyclables;

    /// The initial allocation size given in the constructor, multiplied with #bufferGrowthInPercent
    /// with each new buffer created. Allocated buffers may be bigger in the case that a single
    /// allocation is larger than this value.
    size_t                      nextBuffersUsableSize;

    /// Growth factor of subsequently allocated buffers.
    /// Given by a construction parameter, which in most cases defaults to \c 200,
    /// doubling the buffer size with each next buffer allocation.
    unsigned int                bufferGrowthInPercent;

    #if ALIB_DEBUG
      public:
        /// A name for this object.
        /// With debug-compilations, this name has to be given with construction.
        const char*             DbgName;

      protected:
        /// This flag to mark this allocator to not accept allocations.
        /// @see Method #DbgLock and chapter \ref alib_contmono_further_debug of the
        /// Programmer's Manual of this \alibmod_nl.
        bool                    dbgLock                                                     = false;
    #endif

    #if ALIB_DEBUG_MEMORY
        /// Debug statistics measured on the whole run-time of this object.<br>
        /// Availability depends on code selector symbol \ref ALIB_DEBUG_MEMORY.
        DbgStatistics           dbgStats;
    #endif

  public:
    #if ALIB_DEBUG_CRITICAL_SECTIONS
        /// Due to the possibility of beeing self-contained, this type needs special treatment
        /// in respect to instance \alib{lang;DbgCriticalSections}. It must be neither derived
        /// nor being a member. Instead, destruction has to be controlled.
        /// Furthermore, the \ref GrpALibMacros_mod_threads "ALib Module Threads Macros"
        /// cannot be used.
        lang::Placeholder<lang::DbgCriticalSections>    DbgCriticalSectionsPH;
    #endif

  protected:

    //==============================================================================================
    /// This internal allocation method is called by the allocation interface methods, in case
    /// the current request cannot be trivially satisfied.
    ///
    /// Implements the overall strategy of this class in respect to oversized blocks,
    /// recycling of blocks, etc.
    ///
    /// @param size      The size of the first object to allocate in the buffer.
    /// @param alignment The allocation alignment of the first object to allocate.
    /// @return A pointer to the memory allocated for the object.
    //==============================================================================================
    ALIB_DLL char*   nextBuffer(size_t size, size_t alignment);

public:
    /// The type of the allocator that this allocator uses underneath to allocate the buffers,
    /// given with template parameter \p{TAllocator}.<br>
    /// The instance can be accessed with inherited methods
    /// \alib{lang::AllocatorMember;GetAllocator} and \alib{lang::AllocatorMember;AI}.
    using ChainedAllocator = TAllocator;

    /// Evaluates to \b 1.
    /// @see Field \alib{lang;Allocator::MIN_ALIGNMENT}.
    static constexpr size_t             MIN_ALIGNMENT                                           = 1;

    /// Evaluates to <c>std::numeric_limits<size_t>::max()</c>.
    /// @see Field \alib{lang;Allocator::MAX_ALIGNMENT}.
    static constexpr size_t             MAX_ALIGNMENT         = (std::numeric_limits<size_t>::max)();

  // ###############################################################################################
  // ##### Constructors/Destructor
  // ###############################################################################################
  public:
    #if DOXYGEN
    /// Special constructor that is not initializing this type.
    /// This can be used if a value of this type is required, but only later it becomes known
    /// what the initial buffer size and growth factor is to be.
    ///
    /// Method #IsInitialized can be used to determine if an allocator was initialized on
    /// construction.
    /// If not, a placement-new calling a decent constructor is to be performed before
    /// construction.
    ///
    /// @see Chapter \ref alib_contmono_globalinstance_initialsize of the Programmer's Manual
    ///      of module \alib_monomem.
    ///
    /// @param  dbgName     Has to be specified with debug-compilations only.
    ///                     Use macro \ref ALIB_DBG to pass a constant.
    /// @tparam TRequires   Defaulted template parameter. Must not be specified.
    template<typename TRequires= lang::AllocatorMember<TAllocator>>
    requires std::default_initializable<TRequires>
    TMonoAllocator(const char* dbgName, std::nullptr_t)                                    noexcept;
    #else
        template<typename TRequires= lang::AllocatorMember<TAllocator>>
        requires std::default_initializable<TRequires>
        TMonoAllocator(ALIB_DBG(const char* dbgName,) std::nullptr_t)                       noexcept
        : allocMember(TAllocator())
        , buffer     (nullptr)
        , recyclables(nullptr)
ALIB_DBG(,DbgName(dbgName))                                                                       {}
    #endif

    #if DOXYGEN
    /// Constructor that accepts an external first buffer. If this constructor is used, and the
    /// given \p{initialBuffer} must not be freed to the heap, method
    /// #destructWithExternalBuffer has to be called right before the allocator is destructed.
    ///
    /// \note
    ///   This constructor, together with method #destructWithExternalBuffer enables the efficient
    ///   implementation of derived class \alib{monomem;TLocalAllocator}, as well as
    ///   class \alib{monomem;TSharedMonoVal}.
    ///
    /// @param dbgName                Has to be specified with debug-compilations only.
    ///                               Use macro \ref ALIB_DBG to pass a constant.
    /// @param pInitialBuffer         The initial buffer to use. Has to be aligned with at least
    ///                               <c>alignof(void*)</c>.
    /// @param pInitialBufferSizeInKB The size in kB (1024 bytes) of the given \p{initialBuffer}.
    /// @param pBufferGrowthInPercent Optional growth factor in percent, applied to each allocation
    ///                               of a next buffer size in respect to its previous size.
    ///                               Defaults to \c 200, which doubles buffer size with each
    ///                               next internal buffer allocation.
    /// @tparam TRequires             Defaulted template parameter. Must not be specified.
    template<typename TRequires= lang::AllocatorMember<TAllocator>>
    requires std::default_initializable<TRequires>
    TMonoAllocator( const char*     dbgName,
                    detail::Buffer* pInitialBuffer,
                    size_t          pInitialBufferSizeInKB,
                    unsigned int    pBufferGrowthInPercent= 200  );
    #else
    template<typename TRequires= lang::AllocatorMember<TAllocator>>
    requires std::default_initializable<TRequires>
    TMonoAllocator( ALIB_DBG(const char* dbgName,)
                    detail::Buffer* pInitialBuffer,
                    size_t          pInitialBufferSizeInKB,
                    unsigned int    pBufferGrowthInPercent= 200  )
    : buffer               ( pInitialBuffer )
    , recyclables          ( nullptr )
    , nextBuffersUsableSize( (pInitialBufferSizeInKB * 1024 * pBufferGrowthInPercent) / 100 )
    , bufferGrowthInPercent( pBufferGrowthInPercent )
ALIB_DBG(,DbgName          (dbgName))
    {
        // assert alignment
        ALIB_ASSERT_ERROR( (size_t(pInitialBuffer) & (alignof(void*)-1) )== 0, "MONOMEM",
                         "The given initial buffer is not aligned to at least 'alignof(void*)'."  )

        #if ALIB_DEBUG_CRITICAL_SECTIONS
            DbgCriticalSectionsPH.Construct("MonoAllocator");
        #endif
        buffer->previous= nullptr;
    }
    #endif


    #if DOXYGEN
    /// Alternative constructor version that in addition expects an allocator.
    ///
    /// @param dbgName                Has to be specified with debug-compilations only.
    ///                               Use macro \ref ALIB_DBG to pass a constant.
    /// @param pAllocator             The allocator to use for allocation of buffers.
    /// @param pInitialBuffer         The initial buffer to use. Has to be aligned with at least
    ///                               <c>alignof(void*)</c>.
    /// @param pInitialBufferSizeInKB The size in kB (1024 bytes) of the given \p{initialBuffer}.
    /// @param pBufferGrowthInPercent Optional growth factor in percent, applied to each allocation
    ///                               of a next buffer size in respect to its previous size.
    ///                               Defaults to \c 200, which doubles buffer size with each
    ///                               next internal buffer allocation.
    TMonoAllocator( const char*     dbgName,
                    TAllocator&     pAllocator,
                    detail::Buffer* pInitialBuffer,
                    size_t          pInitialBufferSizeInKB,
                    unsigned int    pBufferGrowthInPercent= 200  );
    #else
    TMonoAllocator( ALIB_DBG(const char* dbgName,)
                    TAllocator&     pAllocator,
                    detail::Buffer* pInitialBuffer,
                    size_t          pInitialBufferSizeInKB,
                    unsigned int    pBufferGrowthInPercent= 200  )
    : allocMember          ( pAllocator )
    , buffer               ( pInitialBuffer )
    , recyclables          ( nullptr )
    , nextBuffersUsableSize( (pInitialBufferSizeInKB * 1024 * pBufferGrowthInPercent) / 100 )
    , bufferGrowthInPercent( pBufferGrowthInPercent )
    ALIB_DBG(,DbgName      (dbgName))
    {
        // assert alignment
        ALIB_ASSERT_ERROR( (size_t(pInitialBuffer) & (alignof(void*)-1) )== 0, "MONOMEM",
                       "The given initial buffer is not aligned to at least 'alignof(void*)'."  )

        #if ALIB_DEBUG_CRITICAL_SECTIONS
            DbgCriticalSectionsPH.Construct("MonoAllocator");
        #endif
        buffer->previous= nullptr;
    }
    #endif

  public:
    #if DOXYGEN
    //==============================================================================================
    /// Constructor.
    /// A first memory buffer is allocated from #ChainedAllocator.
    ///
    /// Parameter \p{bufferGrowthInPercent} determines the growth of memory buffers. The size of a
    /// next buffer is calculated as:
    ///              newSize= (previousSize *  bufferGrowthInPercent) / 100
    ///
    /// @param dbgName                 Has to be specified with debug-compilations only.
    ///                                Use macro \ref ALIB_DBG to pass a constant.
    /// @param  pInitialBufferSizeInKB The size in kB (1024 bytes) of the first memory buffer used
    ///                                for the allocator itself as well as for the first allocations.
    /// @param  pBufferGrowthInPercent Optional growth factor in percent, applied to each allocation
    ///                                of a next buffer size in respect to its previous size.
    ///                                Defaults to \c 200, which doubles the buffer size with each
    ///                                next internal buffer allocation.
    /// @tparam TRequires              Defaulted template parameter. Must not be specified.
    //==============================================================================================
    template<typename TRequires= lang::AllocatorMember<TAllocator>>
    requires std::default_initializable<TRequires>
    TMonoAllocator( const char*          dbgName,
                    size_t               pInitialBufferSizeInKB,
                    unsigned int         pBufferGrowthInPercent= 200  );
    #else
    template<typename TRequires= lang::AllocatorMember<TAllocator>>
    requires std::default_initializable<TRequires>
    TMonoAllocator( ALIB_DBG(const char* dbgName,)
                    size_t               pInitialBufferSizeInKB,
                    unsigned int         pBufferGrowthInPercent= 200  )
    : recyclables          ( nullptr )
    , nextBuffersUsableSize( (pInitialBufferSizeInKB * 1024 * pBufferGrowthInPercent) / 100 )
    , bufferGrowthInPercent( pBufferGrowthInPercent )
ALIB_DBG(,DbgName(dbgName))
    {
        #if ALIB_DEBUG && !ALIB_DEBUG_ASSERTION_PRINTABLES
          ALIB_ASSERT_ERROR( pInitialBufferSizeInKB, "MONOMEM", "Initial buffer of 0kb requested." )
        #endif
        size_t initialBufferSize= pInitialBufferSizeInKB * 1024;
        buffer= new (TAllocator().allocate(initialBufferSize, alignof(detail::Buffer)))
                detail::Buffer( initialBufferSize );
        #if ALIB_DEBUG_CRITICAL_SECTIONS
            DbgCriticalSectionsPH.Construct("MonoAllocator");
        #endif
        buffer->previous= nullptr;
    }
    #endif

    #if DOXYGEN
    //==============================================================================================
    /// Alternative constructor accepting an allocator instance.
    /// To be used if template parameter \p{TAllocator} does not equal \alib{lang;HeapAllocator}
    /// or any (custom) default-constructible allocator type.
    /// @param dbgName                Has to be specified with debug-compilations only.
    ///                               Use macro \ref ALIB_DBG to pass a constant.
    /// @param pAllocator             The allocator to use for allocation of buffers.
    /// @param pInitialBufferSizeInKB The size in kB (1024 bytes) of the first memory buffer used
    ///                               for the allocator itself as well as for the first allocations.
    /// @param pBufferGrowthInPercent Optional growth factor in percent, applied to each allocation
    ///                               of a next buffer size in respect to its previous size.
    ///                               Defaults to \c 200, which doubles the buffer size with each
    ///                               next internal buffer allocation.
    //==============================================================================================
    ALIB_DLL
    TMonoAllocator( const char* dbgName, TAllocator& pAllocator,
                    size_t pInitialBufferSizeInKB, unsigned int pBufferGrowthInPercent= 200  );
    #else
    ALIB_DLL
    TMonoAllocator( ALIB_DBG(const char* dbgName,) TAllocator& pAllocator,
                    size_t pInitialBufferSizeInKB, unsigned int pBufferGrowthInPercent= 200  )
    : allocMember          ( pAllocator )
    , recyclables          ( nullptr )
    , nextBuffersUsableSize( (pInitialBufferSizeInKB * 1024 * pBufferGrowthInPercent) / 100 )
    , bufferGrowthInPercent( pBufferGrowthInPercent )
ALIB_DBG(,DbgName(dbgName))
    {
        ALIB_ASSERT_ERROR( pInitialBufferSizeInKB, "MONOMEM", "Initial buffer of 0kb requested." )
        size_t initialBufferSize= pInitialBufferSizeInKB * 1024;
        buffer= new (TAllocator().allocate(initialBufferSize, alignof(detail::Buffer)))
                detail::Buffer( initialBufferSize );
        #if ALIB_DEBUG_CRITICAL_SECTIONS
            DbgCriticalSectionsPH.Construct("MonoAllocator");
        #endif
        buffer->previous= nullptr;
    }
    #endif

    /// Not copyable.
    TMonoAllocator(const TMonoAllocator&)                                                  = delete;

    /// Not movable.
    TMonoAllocator(TMonoAllocator&&)                                                       = delete;

    /// Destructor. Disposes all memory allocated with #ChainedAllocator.
    ALIB_DLL
    ~TMonoAllocator();


    //##############################################################################################
    /// @name Self-Contained Creation
    //##############################################################################################

    #if DOXYGEN
    //==============================================================================================
    /// This static method creates an object of this type inside "itself", aka inside its first
    /// allocated buffer.
    /// Instances created with this method have to be deleted by only invoking the destructor,
    /// e.g., using \alib{lang;Destruct}.
    ///
    /// Method #Reset must not be called using its default parameter when an instance of this type
    /// was created by this method. Instead, if reset operations are desired, a snapshot has to be
    /// taken (see method #TakeSnapshot) right after the invocation of this method and maybe
    /// other initial members that should survive a reset, which then has to be passed to method
    /// \b Reset.<br>
    /// Alternatively, if only the monotonic allocator should survive the reset, overloaded method
    /// #Reset(size_t, size_t) might be used, passing <c>sizeof(TMonoAllocator<T>)</c> and
    /// <c>alignof(TMonoAllocator<T>)</c> as parameters.
    ///
    /// @param dbgName               Has to be specified with debug-compilations only.
    ///                              Use macro \ref ALIB_DBG to pass a constant.
    /// @param pAllocator            The allocator used for creating the first buffer.
    /// @param initialBufferSizeInKB The size of memory the buffers allocated in kilobytes.
    /// @param bufferGrowthInPercent Optional growth factor in percent, applied to the buffer size
    ///                              with each next buffer allocation.
    ///                              Values provided should be greater than 100.<br>
    ///                              Defaults to \c 200, which doubles buffer size with each
    ///                              next internal buffer allocation.
    /// @return A pointer to an instance of this type \b MonoAllocator residing in its first
    ///         created buffer.
    //==============================================================================================
    ALIB_DLL
    static TMonoAllocator* Create( const char*  dbgName,
                                   TAllocator&  pAllocator,
                                   size_t       initialBufferSizeInKB,
                                   unsigned int bufferGrowthInPercent= 200 );
    #else
    ALIB_DLL
    static TMonoAllocator* Create( ALIB_DBG(const char* dbgName,)
                                   TAllocator&  pAllocator,
                                   size_t       initialBufferSizeInKB,
                                   unsigned int bufferGrowthInPercent= 200 );
    #endif

    #if DOXYGEN
    //==============================================================================================
    /// Same as #Create(const char*, TAllocator&, size_t, unsigned int), but misses the allocator
    /// parameter.
    /// Used with chained allocators that are default-constructible (e.g., \b HeapAllocator):
    ///
    /// @param  dbgName               Has to be specified with debug-compilations only.
    ///                               Use macro \ref ALIB_DBG to pass a constant.
    /// @param  initialBufferSizeInKB The size of memory the buffers allocated in kilobytes.
    /// @param  bufferGrowthInPercent Optional growth factor in percent, applied to the buffer size
    ///                               with each next buffer allocation.
    ///                               Values provided should be greater than 100.<br>
    ///                               Defaults to \c 200, which doubles buffer size with each
    ///                               next internal buffer allocation.
    /// @tparam TRequires             Defaulted template parameter. Must not be specified.
    /// @return A pointer to an instance of this type \b MonoAllocator residing in its first
    ///         created buffer.
    //==============================================================================================
    template<typename TRequires= lang::AllocatorMember<TAllocator>>
    requires std::default_initializable<TRequires>
    static TMonoAllocator* Create( const char*  dbgName,
                                   size_t       initialBufferSizeInKB,
                                   unsigned int bufferGrowthInPercent= 200 );
    #else
    template<typename TRequires= lang::AllocatorMember<TAllocator>>
    requires std::default_initializable<TRequires>
    static TMonoAllocator* Create( ALIB_DBG( const char*  dbgName, )
                                   size_t       initialBufferSizeInKB,
                                   unsigned int bufferGrowthInPercent= 200 )
    {
        TAllocator a;
        return Create( ALIB_DBG(dbgName,) a, initialBufferSizeInKB, bufferGrowthInPercent );
    }
    #endif


    /// This method has to be called before destruction of an instance, in the case that
    /// instance was allocated using a special version of the constructors that accept an external
    /// \alib{monomem::detail;Buffer}.
    /// This will remove the initially given buffer from the list of buffers, and thus this buffer
    /// not be deleted. Only if the memory was simply heap-allocated using <c>std::malloc</c>,
    /// <c>C++ new operator</c>, or \alib{lang;HeapAllocator}, and if the memory is intended to be
    /// freed, this method may not be called. As a sample, class \alib{monomem;TLocalAllocator},
    /// which uses stack-memory for the first buffer, will call this method in its destructor.
    ///
    /// \attention
    ///   1. If this method is not called when needed, this leads to undefined behavior.
    ///   2. If this method is called without the provision of an external buffer on construction,
    ///      a memory leak occurs. (The first buffer this allocator allocated, will not be freed.)
    ///   3. After this method has been invoked, the instance becomes unusable and is to be
    ///      destructed in a next step.
    ///
    /// Note that as an exception to the rule, this method's name starts with a lowercase
    /// letter as if it was protected, while it has to be public.
    ALIB_DLL
    void destructWithExternalBuffer();


    /// Tests if non-initializing constructor #TMonoAllocator(const char*, std::nullptr_t) was used.
    /// @return \c false if this allocator needs to be initialized by performing a placement-new on
    ///            <c>this</c>.
    bool IsInitialized()                                const noexcept { return buffer != nullptr; }



    // #############################################################################################
    /// @name lang::Allocator Implementation
    // #############################################################################################

    /// Allocate memory from the internal buffer. If the buffer's size is exceeded, a next buffer
    /// is allocated and used.
    /// @param size      The size of memory the block to allocate in bytes.
    ///                  With this allocator this is not an input/output parameter.
    /// @param alignment The (minimum) alignment of the memory block to allocate in bytes.
    ///                  See Chapter \ref alib_contmono_further_alignment of the Programmer's
    ///                  Manual of this module.
    /// @return Pointer to the allocated memory.
    inline void* allocate( size_t size, size_t alignment )
    {
        #if ALIB_DEBUG_CRITICAL_SECTIONS
            DbgCriticalSectionsPH.Get()->Acquire(ALIB_CALLER);
        #endif
        ALIB_DBG(assert::SingleThreaded());

        ALIB_ASSERT_ERROR( buffer  , "MONOMEM",
                         "This MonoAllocator was constructed passing 'nullptr' and is not usable.")
        ALIB_ASSERT_ERROR( !dbgLock, "MONOMEM", "This MonoAllocator is locked.")
        ALIB_ASSERT_ERROR(lang::BitCount(alignment) == 1, "MONOMEM",
             "The requested alignment has to be a power of 2. Requested is: ", int(alignment) )

        #if ALIB_DEBUG_MEMORY
            ++dbgStats.QtyAllocations;
            DBG_ALIGNMENT_INIT( buffer )
        #endif

        #if ALIB_DEBUG_MEMORY
            if( lang::DbgAlloc::extSize( size ) > size_t(buffer->end - reinterpret_cast<char*>(buffer)) / 2 )
                ALIB_WARNING( "MONOMEM",
                       "MonoAllocator: Allocation size exceeds 1/2 of the current buffer size.\n"
                       "The allocator's buffer size should be increased.\n"
                       "Requested size: ",  size          )
        #endif


        char* mem= buffer->allocate(size, alignment);
        if ( mem )
        {
            #if ALIB_DEBUG_MEMORY
                DBG_ALIGNMENT_MEASURE(buffer)
            #endif
            #if ALIB_DEBUG_CRITICAL_SECTIONS
                DbgCriticalSectionsPH.Get()->Release(ALIB_CALLER);
            #endif
            return mem;
        }

        mem= nextBuffer( size, alignment );
        #if ALIB_DEBUG_CRITICAL_SECTIONS
            DbgCriticalSectionsPH.Get()->Release(ALIB_CALLER);
        #endif
        return mem;
    }

    /// Grows a piece of memory.
    /// If a new allocation had to be performed, the existing data is copied.
    /// Note that this allocator implementation never shrinks memory, thus
    /// if \p{oldSize} is greater than \p{newSize}, the original memory is returned.<br>
    /// @param mem       The memory to reallocate.
    /// @param oldSize   The current size of \p{mem}.
    /// @param newSize   The now required size of \p{mem} in bytes.
    ///                  With this allocator this is not an input/output parameter.
    /// @param alignment The (minimum) alignment of the memory block to allocate in bytes.
    ///                  (Has to be the same as before, but this is not tested here).
    ///                  See Chapter \ref alib_contmono_further_alignment of the Programmer's
    ///                  Manual of this module.
    /// @return Pointer to the re-allocated memory block.
    inline void* reallocate( void* mem, size_t oldSize, size_t newSize, size_t  alignment )
    {
        ALIB_ASSERT_ERROR( !dbgLock, "MONOMEM", "This MonoAllocator is locked.")
        ALIB_DBG(assert::SingleThreaded());
        ALIB_ASSERT_ERROR(lang::BitCount(alignment) == 1, "MONOMEM",
             "The requested alignment has to be a power of 2. Requested is: ", alignment )

        lang::DbgAlloc::checkMem(mem,oldSize,detail::Buffer::MAGIC, ALIB_REL_DBG(nullptr, DbgName));
        if( oldSize >= newSize )
            return mem;

        if( buffer->act - lang::DbgAlloc::extSize(oldSize) == mem )
            buffer->act-= lang::DbgAlloc::extSize(oldSize);

        auto* newMem= allocate( newSize, alignment );
        if( newMem != mem )
             std::memcpy( newMem, mem, oldSize );

        #if ALIB_DEBUG_MEMORY
        else
            --dbgStats.QtyAllocations;
        #endif
        lang::DbgAlloc::annotate(newMem, newSize, detail::Buffer::MAGIC);
        return newMem;
    }

    /// This method is empty for this allocator and optimized out. Only if the compiler-symbol
    /// \ref ALIB_DEBUG_ALLOCATIONS is set, the method will overwrite the freed memory with
    /// character <c>0xD2</c>. This is why method #allowsMemSplit returns \c false if that
    /// symbol is set and some few optimizations will not be performed in that mode, for example,
    /// the reuse of bucket arrays as recycled node objects when resizing hashtables.
    ///
    /// @param  mem   The memory to free.
    /// @param  size  The allocated size.
    inline void free( void* mem, size_t size)                                                  const
    {
        ALIB_ASSERT_ERROR( !dbgLock, "MONOMEM", "This MonoAllocator is locked.")
        lang::DbgAlloc::checkMem(mem, size, detail::Buffer::MAGIC, ALIB_REL_DBG(nullptr, DbgName) );
        lang::DbgAlloc::clearMem(mem, size, detail::Buffer::CLEAR);
    }

    /// This is an empty implementation of the prototyped method.
    /// It is empty because this allocator never returns a higher allocation size than requested.
    /// \alib{lang;Allocator::dbgAcknowledgeIncreasedAllocSize}.
    /// @tparam TSize    The type of parameter \p{allocSize}. (Deduced by the compiler.)
    template<typename TSize>
    void dbgAcknowledgeIncreasedAllocSize( void*, TSize  )                                 const  {}

    /// Returns a temporary object (which is usually optimized out together with a call to this
    /// operator) providing high-level convenience methods for allocation.
    /// @see Class \alib{lang::AllocatorInterface}
    /// @return A temporary high-level interface into the allocator.
    lang::AllocatorInterface<TMonoAllocator> operator()()
    { return lang::AllocatorInterface<TMonoAllocator>(*this); }

  #if DOXYGEN
    /// See the description of this method with prototype \alib{lang;Allocator::allowsMemSplit}.<br>
    /// (Note: This method is static. For technical reasons this cannot be reflected in this
    /// documentation)
    /// @return \c true, except if the compiler-symbol \ref ALIB_DEBUG_ALLOCATIONS is given.
           constexpr bool allowsMemSplit()                                                 noexcept;
  #else
    static inline constexpr bool allowsMemSplit()                                           noexcept
    {
        #if !ALIB_DEBUG_ALLOCATIONS
            return true;
        #else
            return false;
        #endif
    }
  #endif

    //##############################################################################################
    /// @name Snapshots and Reset
    //##############################################################################################

    //==============================================================================================
    /// Saves the current state of the allocator and returns this information as a \b Snapshot
    /// value. Such snapshots may be passed to method #Reset(Snapshot).
    ///
    /// Note that the actual memory is \b not copied and restored. In this respect the word
    /// "Snapshot" is overstating. What is stored are the current use of memory, but not it's
    /// contents.
    ///
    /// @return A (lightweight) snapshot value object.
    //==============================================================================================
    Snapshot        TakeSnapshot()
    {
        return Snapshot(buffer, buffer->act);
    }

    //==============================================================================================
    /// Resets this allocator to the given \alib{monomem;Snapshot}.
    /// Parameter \p{snapshot} is defaulted with a default-constructed \b Snapshot, which
    /// completely resets the allocator.
    ///
    /// With a reset, the memory buffers which had been allocated after taking the given
    /// \p{snapshot}, are not released back to the operating system, but re-used with future
    /// monotonic allocations.
    ///
    /// This method is useful in cases where some permanent objects which are allocated first
    /// have to be preserved with resets.
    ///
    /// Note that snapshots taken after the given one become invalid. This is because
    /// class \b Snapshot is only a simple lightweight class that marks the currently
    /// used buffer and its fill level.
    ///
    /// @param snapshot The snapshot to reset to.
    //==============================================================================================
    ALIB_DLL
    void   Reset( Snapshot snapshot= Snapshot() );

    //==============================================================================================
    /// Special version of #Reset(Snapshot) which resets this allocator to the first buffer
    /// and within that, behind the first object of the given size.<br>
    /// This method is used by class \alib{monomem;TSharedMonoVal} to avoid the need of storing
    /// a snapshot behind itself.
    ///
    /// If the compiler-symbol \ref ALIB_DEBUG_ALLOCATIONS is set, then all freed memory is
    /// overwritten with <c>0xD2</c>. This helps to identify invalid reset operations.
    /// @param firstObjectSize      The size of the first emplaced object.
    /// @param firstObjectAlignment The alignment of the first emplaced object.
    //==============================================================================================
    void   Reset( size_t firstObjectSize, size_t firstObjectAlignment )
    {
        Reset( ALIB_DBG( Snapshot(nullptr, reinterpret_cast<char*>(1)) ) );
        buffer->allocate( firstObjectSize, firstObjectAlignment );
    }

    //##############################################################################################
    /// @name Statistics and Debug-Interface
    //##############################################################################################

    /// Fills the given \p{result} record with statistical information about this allocator.
    /// @see Method #DbgGetStatistics, which delivers further information but is only available
    ///      with compilation symbol \ref ALIB_DEBUG_MEMORY set.
    /// @param result The object to write the result into. (Will be reset before use.)
    ALIB_DLL
    void            GetStatistics( Statistics& result );

    /// Lock or unlock this allocator.
    /// If locked, an ALib assertion is raised if allocations are performed.
    /// This can be quite useful to detect allocations with an allocator that is shared
    /// between different code entities and to enforce certain allocation contracts.
    ///
    /// With release-compilations, this method is empty and optimized out.
    /// @see Chapter \ref alib_contmono_further_debug of the Programmer's Manual of this
    /// \alibmod_nl.
    /// @param onOff The state to set.
    void            DbgLock(bool onOff)        noexcept { ALIB_DBG( dbgLock= onOff;) (void) onOff; }

    /// Allows access to the internal buffer.
    /// For example, this is used with function \alib{monomem;DbgDumpStatistics}.
    /// With release-compilations, this method is empty and not useable.
    /// @return A pointer to the internal buffer.
    detail::Buffer* DbgGetBuffer()       noexcept { ALIB_REL_DBG( return nullptr;, return buffer;) }

    /// Allows access to the internal buffer (const overload).
    /// For example, this is used with function \alib{monomem;DbgDumpStatistics}.
    /// With release-compilations, this method is empty and not useable.
    /// @return A pointer to the internal buffer.
    const detail::Buffer* DbgGetBuffer() const noexcept { ALIB_REL_DBG( return nullptr;, return buffer;) }

    /// If the compiler-symbol \ref ALIB_DEBUG_ALLOCATIONS is not set, this method is empty and will
    /// be optimized out. Otherwise, this will raise an \alib assertion if the piece of allocated
    /// memory is corrupted or its allocation size is not rightfully given by the using code.
    /// @see Chapter \ref alib_contmono_further_debug of the Programmer's Manual.
    ///
    /// @tparam TSize   The type of parameter \p{size}. (Deduced by the compiler.)
    /// @param mem   The address of the allocated object.
    /// @param size  The requested allocation size of the object.
    template<typename TSize>
    void dbgCheckMemory( void* mem, TSize size )
    { lang::DbgAlloc::checkMem( mem, size, detail::Buffer::MAGIC, ALIB_REL_DBG(nullptr, DbgName)); }

    #if ALIB_DEBUG_MEMORY
        //==========================================================================================
        /// Returns allocation statistics for manual performance optimization.
        /// \par Availability
        ///  This method is included only if the code selector symbol \ref ALIB_DEBUG_MEMORY is set.
        ///
        /// @return The internal statistics struct.
        //==========================================================================================
        const       DbgStatistics& DbgGetStatistics()                    const  { return dbgStats; }
     #endif

}; // class TMonoAllocator


// #############################   Template instantiation declaration   ############################
#if !DOXYGEN
    extern template ALIB_DLL class TMonoAllocator<lang::HeapAllocator>;
#endif

// ######################################   Global Allocator   #####################################

/// This is the global monotonic allocator singleton instance.
/// It's initial size defaults to \  128 kilobytes. This can be tweaked by performing a
/// placement-new on this instance before \ref alib_mod_bs "bootstrapping ALib".
///
/// \see
///  - Chapter \ref alib_contmono_globalinstance of the Programmer's Manual of this \alibmod_nl.
///  - \b Lock instance \alib{monomem::GLOBAL_ALLOCATOR_LOCK}
extern ALIB_DLL    TMonoAllocator<lang::HeapAllocator>  GLOBAL_ALLOCATOR;

#if !ALIB_SINGLE_THREADED
/// This <em>mutex</em> is used to protect the \alib{monomem;GLOBAL_ALLOCATOR} from racing
/// conditions in multithreaded software.<br>
/// If the compiler-symbol \ref ALIB_DEBUG_CRITICAL_SECTIONS is set, this lock will be attached
/// to the \alib{lang;DbgCriticalSections} instance in #GLOBAL_ALLOCATOR during bootstrap.
/// Thus, an assertion will be raised if the \alib{monomem;GLOBAL_ALLOCATOR} is used without
/// locking this mutex.<br>
/// This is a recursive lock.
/// This allows using the \alib{lang;AllocatorInterface;high-level allocation interface} with
/// allocations, which constructs objects.
/// These objects might recursively use the global allocator for their members.
extern ALIB_DLL    RecursiveLock                        GLOBAL_ALLOCATOR_LOCK;
#endif

}  // namespace alib[::monomem]

/// Type alias in namespace \b alib.
/// This alias fixes template parameter \p{TAllocator} (which defines the
/// \ref alib_contmono_chaining "chained allocator") to type \alib{lang;HeapAllocator}.
using     MonoAllocator =   monomem::TMonoAllocator<lang::HeapAllocator>;

/// Type alias in namespace \b alib to denote the use of a \alib{MonoAllocator}
/// with type \alib{lang::StdContainerAllocator}.
template<typename T>
using SCAMono= lang::StdContainerAllocator<T, MonoAllocator>;



} // namespace [alib]


