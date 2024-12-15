//==================================================================================================
/// \file
/// This header file is part of module \alib_monomem of the \aliblong.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_MONOMEM_POOLALLOCATOR
#define HPP_ALIB_MONOMEM_POOLALLOCATOR 1
#pragma once
#include "alib/lang/bits.hpp"
ALIB_ASSERT_MODULE(MONOMEM)
#include "alib/monomem/monoallocator.hpp"
#include "alib/lang/owner.hpp"

namespace alib {
namespace monomem {

#if !defined(ALIB_MONOMEM_POOLALLOCATOR_DEFAULT_ALIGNMENT)
#   define ALIB_MONOMEM_POOLALLOCATOR_DEFAULT_ALIGNMENT     alignof(uint64_t)
#endif

//==================================================================================================
/// \par Important Note
///   Please consult the \ref alib_contmono_intro "Programmer's Manual" of \alib module
///   \alib_monomem_nl for an introduction into the rather complex topic of monotonous and
///   pool allocation and how to use this class.
///
/// This class implements the interface prototyped with \alib{lang;Allocator}.
/// Whenever an object is freed with this allocator, the memory is stored in a stack for later
/// re-use. This means, with allocations, first it is checked if a previously freed object of
/// fitting size is available, and only if not, new memory is allocated.
/// To implement this in a performant way (preferably with compile-time calculations), the following
/// approach was taken:
/// - The size of every allocation request is increased to the next higher power of 2 bound.
///   For example, if a memory piece of 42 bytes is requested, a piece of size 64 bytes is returned.
/// - For each allocation size, a simple stack (single directed linked list) is used to dispose and
///   recycle memory.
/// - The minimum allocation size is <c>sizeof(void*)</c>, because for the recycling mechanism
///   a pointer needs to be stored in the freed pool memory, when it is added to the internal
///   "recycling stack".
/// - This results in only 61 different allocation sizes (on a 64-bit machine).
/// - A fixed alignment for all objects is used. This is also the maximum alignment allowed to pass
///   to methods #allocate and #reallocate. Its value is defined by template parameter
///   \p{TAlignment}, which defaults to what is given with compiler symbol
///   \ref ALIB_MONOMEM_POOLALLOCATOR_DEFAULT_ALIGNMENT, which in turn defaults to
///   <c>alignof(uint64_t)</c>.
/// - This approach results in only 61 different allocation sizes (on a 64-bit machine).
///
/// The consequences of this strategy are straight forward:
/// - Using this allocator type requires disposing (free) every allocated piece of memory (which is
///   in contrast to \b MonoAllocator).
/// - This type is well suited for allocation scenarios, which release each object within
///   a certain period of time, just to allocate new ones shortly afterward. A good use
///   case example is given with module \alib_threadmodel: Here, request objects are
///   pushed into priority queues, which are disposed after the request got processed. The objects
///   are rather small and of different sizes. In complex server scenarios, it can become quickly
///   millions of messages per second which are created and disposed.
/// - A statistical average of 25% memory allocation overhead (waste) is taken into account with
///   the use of this class.
///
/// Chapter \ref alib_contmono_intro_recycling of the joint Programmer's Manual of modules
/// \alib_containers_nl and \alib_monomem_nl provides an in-depth discussion of what is called
/// "recycling" in this context. Several strategies are proposed with container types provided by
/// module \alib_containers_nl. From this perspective, this allocator could be seen as a
/// "recycling proxy" in front of a \b MonoAllocator (or a different,
/// \ref  alib_contmono_chaining "chained" allocator).
///
/// @see
/// - The \ref alib_contmono_intro "Programmer's Manual" of module \alib_monomem_nl.
/// - The "standard" type definition of this template type \alib{MonoAllocator},
///   which uses an \alib{lang;HeapAllocator} as the
///   \ref alib_contmono_chaining "chained allocator".
///
/// @tparam TAllocator The underlying allocator to use.<br>
///                    With type definition \alib{PoolAllocator},this is bound to type
///                    \alib{MonoAllocator}.<br>
///                    With type definition \alib{PoolAllocatorHA},this is bound to type
///                    \alib{lang;HeapAllocator}.<br>
///                    For general information see chapter \ref alib_contmono_chaining
///                    of the Programmer's Manual.
/// @tparam TAlignment The fixed alignment to use. Corresponding method parameters are ignored.<br>
///                    Defaults to compiler symbol
///                    \ref ALIB_MONOMEM_POOLALLOCATOR_DEFAULT_ALIGNMENT, which in turn defaults
///                    to <c>alignof(uint64_t)</c>.<br>
///                    It is statically asserted that this value is not higher than
///                    field \alib{lang::Allocator;MAX_ALIGNMENT} of the given chained
///                    \p{TAllocator} and not smaller than <c>alignof(void*)</c>.<br>
///                    See Chapter \ref alib_contmono_further_alignment of the Programmer's Manual
///                    of this module.
//==================================================================================================
template<typename TAllocator, size_t TAlignment= ALIB_MONOMEM_POOLALLOCATOR_DEFAULT_ALIGNMENT>
class TPoolAllocator : public lang::AllocatorMember<TAllocator>
#if ALIB_DEBUG_CRITICAL_SECTIONS
                     , public lang::DbgCriticalSections
#endif
{
    static_assert( TAlignment >= alignof(void*),
    "The (fixed) alignment of the pool allocator has to be at least as high as alignof(void*). "
    "Adjust template parameter TAlignment." );

    static_assert( TAlignment <= TAllocator::MAX_ALIGNMENT,
    "The (fixed) alignment of the pool allocator cannot be greater than the MAX_ALIGNMENT of "
    "its chained allocator. Adjust template parameter TAlignment." );

    #if ALIB_CPP_STANDARD >= 20 || !defined(_WIN32) // not a constexpr under windows
    static_assert(lang::BitCount(TAlignment) == 1,
    "The fixed alignment of the pool allocator is not a power of 2. "
    "Adjust template parameter TAlignment." );
    #endif
    
    /// A magic byte, used with compiler-symbol \ref ALIB_DEBUG_ALLOCATIONS to mark
    /// memory and detect out-of-bounds writes.
    /// @see Method \alib{lang;Allocator::dbgCheckMemory}.
    static constexpr unsigned char     MAGIC= 0xA3;

    /// A magic byte written when memory is freed.
    /// @see Field #CLEAR.
    static constexpr unsigned char     CLEAR= 0xF3;

  protected:

    using allocMember= lang::AllocatorMember<TAllocator>;            ///< A shortcut to a base type.

    // =========================================   Fields   ========================================
    /// Array of hooks. Each hook points to previously disposed memory of the same size.
    /// Its length is determined with \c constexpr #qtyHooks and it is allocated with construction
    /// in \p{allocator}.
    void**          hooks                                                                 = nullptr;

    #if ALIB_DEBUG
      public:
        /// A name for this object.
        /// The constructors grab this name from the chained allocator, which most often is an
        /// already named \b MonoAllocator. Thus, no constructor argument is available
        /// (in contrast to class \alib{monomem;TMonoAllocator;MonoAllocator}).
        /// This name may be changed after construction by the using code as appropriate.
        const char*             DbgName;
      protected:
    #endif

    #if ALIB_DEBUG_MONOMEM
        /// The overall number of allocations for each size.
        uinteger*   dbgStatAllocCounter                                                   = nullptr;
    #endif

    #if ALIB_DEBUG_ALLOCATIONS
        /// The current number of allocations that have not been freed.
        int*        dbgOpenAllocations                                                    = nullptr;

        /// This member is used with \ref ALIB_DEBUG_ALLOCATIONS to pass the requested size from
        /// interface methods to the implementation. While this approach is not thread-safe, it
        /// avoids adding some method parameter, which would exist only in debug versions.
        /// It was intentionally implemented like this, to cause testing errors in case that
        /// multithreaded access was not locked against racing conditions properly by the user.
        size_t      dbgLastRequestedSize;
    #endif

    // ====================================   Internal Methods   ===================================
    /// To be able to store allocated memory in the recycling stacks, any allocated memory is at
    /// least of size <c>sizeof(void*)</c>. Hence sizes \c 1 and \c 2 (with 64-bit systems
    /// also \c 4) will never be allocated and no hook is available for these sizes.
    /// @return \c 3 on 64-bit systems, \c 2 on 32-bit systems.
    static constexpr short  minimumHookIndex()               { return sizeof(size_t) == 4 ? 2 : 3; }

    
    /// The number of hooks needed. Evaluates to \c 61 on a common 64-bit platform and \c 29 on a
    /// @return \c 61 on 64-bit systems, \c 29 on 32-bit systems.
    static constexpr short  qtyHooks()        { return short(bitsof(size_t)) - minimumHookIndex(); }

    /// Calculates the index of the hook in field #hooks. This corresponds to the number
    /// of the most significant bit in the next higher power of 2 value minus #minimumHookIndex.
    /// @param requestedObjectSize The object size requested by the user.
    /// @return The index of the recycler hook in #hooks.
    static constexpr
    short         hookIndex(size_t requestedObjectSize)
    {
        if( requestedObjectSize < sizeof(void*) )
            return 0;
        int msb= lang::MSB(requestedObjectSize);
        return  short( ((requestedObjectSize & lang::LowerMask<size_t>(msb-1) ) == 0 ? msb -1: msb )
                       - minimumHookIndex() );
    }

    /// Templated version of #hookIndex(size_t).
    /// @tparam TRequestedObjectSize The object size requested by the user, hence excluding the
    ///                              internal pointer.
    /// @return The index of the recycler hook in #hooks.
    template<size_t TRequestedObjectSize>
    static constexpr
    short         hookIndex()
    {
        if( TRequestedObjectSize < sizeof(void*) )
            return 0;
        constexpr int      msb      = lang::MSB(TRequestedObjectSize);
        return short(  ((TRequestedObjectSize & lang::LowerMask<msb-1, size_t>() ) == 0 ? msb-1 : msb)
                      - minimumHookIndex() );
    }

    /// Frees all recycled pool objects.
    void deletePool();

  public:
    /// The type of the allocator that this allocator uses underneath to allocate the buffers,
    /// given with template parameter \p{TAllocator}.<br>
    /// The instance can be accessed with inherited methods
    /// \alib{lang::AllocatorMember;GetAllocator} and \alib{lang::AllocatorMember;AI}.
    using ChainedAllocator = TAllocator;

    /// Evaluates to the value of template parameter \p{TAlignment}.
    /// @see Field \alib{lang;Allocator::MIN_ALIGNMENT}.
    static constexpr size_t             MIN_ALIGNMENT                                  = TAlignment;

    /// Evaluates to the value of template parameter \p{TAlignment}.
    /// @see Field \alib{lang;Allocator::MAX_ALIGNMENT}.
    static constexpr size_t             MAX_ALIGNMENT                                  = TAlignment;

    // =====================================   Construction    =====================================
    /// Constructs this type.
    /// @param pAllocator  The allocator to use for allocating pool objects.
    ALIB_API
    TPoolAllocator   ( TAllocator& pAllocator );

    /// Constructs this type without a given allocator. This constructor is applicable only
    /// if \p{TAllocator} is default-constructible (e.g., \alib{lang;HeapAllocator}).
    template<typename TEnableIf= allocMember,
             ATMP_T_IF(int, std::is_default_constructible<TEnableIf>::value )= 0>
    TPoolAllocator ()
    #if ALIB_DEBUG_CRITICAL_SECTIONS
    :lang::DbgCriticalSections("PoolAllocator")
    #endif
    {
        #if ALIB_CPP_STANDARD < 20 && defined(_WIN32) // otherwise statically asserted in header
        ALIB_ASSERT_ERROR(lang::BitCount(TAlignment) == 1, "MONOMEM",
                "The fixed alignment of the pool allocator is not a power of 2. "
                "Adjust template parameter TAlignment." )
        #endif

        ALIB_DBG(DbgName= allocMember::GetAllocator().DbgName);

        // allocate and initialize hook array
        hooks= allocMember::AI().template AllocArray<void*>(qtyHooks() );
        std::memset( hooks, 0, sizeof( void*[qtyHooks()] ) );
        #if ALIB_DEBUG_ALLOCATIONS
            dbgOpenAllocations= allocMember::AI().template AllocArray<int>(qtyHooks() );
            std::memset( dbgOpenAllocations, 0, sizeof( int[qtyHooks()] ) );
        #endif
        #if ALIB_DEBUG_MONOMEM
            dbgStatAllocCounter= allocMember::AI().template AllocArray<uinteger>(qtyHooks() );
            std::memset( dbgStatAllocCounter, 0, sizeof( uinteger[qtyHooks()] ) );
        #endif
    }

    /// Destructs this type.
    ALIB_API
    ~TPoolAllocator   ();

    // #############################################################################################
    /// @name lang::Allocator Implementation
    // #############################################################################################
    /// Allocates or re-uses a previously freed piece of memory of equal \p{size}.
    /// Any given \p{size} is rounded up to the next higher power of 2 before comparison and
    /// returned via in/output parameter \p{size}.
    /// @param[in,out] size The size of memory the block to allocate in bytes. This will be rounded up
    ///                     to the next higher power of 2 when the function returns.
    ///                     Any additional size might be used by the caller.
    /// @param  pAlignment  The (minimum) alignment of the memory block to allocate in bytes.
    ///                     This is ignored with this allocator. Instead, it uses what is specified
    ///                     by template parameter \p{TAlignment}.
    ///                     See Chapter \ref alib_contmono_further_alignment of the Programmer's
    ///                     Manual of this module.
    /// @return Pointer to the allocated memory.
    void* allocate( size_t& size, size_t pAlignment )
    {
        ALIB_ASSERT_ERROR(pAlignment <= TAlignment,  "MONOMEM",
           "The requested alignment is higher than what was specified with "
           "template parameter TAlignment." )
        (void) pAlignment;

        #if ALIB_DEBUG_ALLOCATIONS
            dbgLastRequestedSize= size;
        #endif

        return AllocateByAllocationInfo(GetAllocInformation(size));
    }

    /// Shrinks or grows a piece of memory. If a new allocation was performed the existing
    /// data is copied. Any given \p{oldSize} and \p{newSize} is rounded up to the next higher
    /// power of 2 before comparison. Note, that parameter \p{newSize} is provided as a reference.
    /// @param  mem            The memory to reallocate.
    /// @param  oldSize        The current size of \p{mem}.
    /// @param[in,out] newSize The size of memory the block to allocate in bytes. This will be rounded
    ///                        up to the next higher power of 2 when the function returns. Any
    ///                        additional size might be used by the caller.
    /// @param  pAlignment     The (minimum) alignment of the memory to allocate in bytes.
    ///                        This is ignored with this allocator. Instead, it uses what is
    ///                        specified by template parameter \p{TAlignment}.
    ///                        See Chapter \ref alib_contmono_further_alignment of the Programmer's
    ///                        Manual of this module.
    /// @return Pointer to the re-allocated memory.
    void* reallocate( void* mem, size_t oldSize, size_t& newSize, size_t pAlignment );

    /// Disposes the given memory. The fragment is stored for later reuse with method #allocate
    /// of memory sizes of the same power of 2 range.
    ///
    /// @param mem   The memory to dispose.
    /// @param size  The size of the given \p{mem}.
    void free(void* mem, size_t size)
    {
        #if ALIB_DEBUG_ALLOCATIONS
            dbgLastRequestedSize= size;
        #endif
        FreeByAllocationInfo( hookIndex(size), mem );
    }

    /// For an explanation, see \alib{lang;Allocator::dbgAcknowledgeIncreasedAllocSize}.
    /// @tparam TSize     The type of parameter \p{allocSize}. (Deduced by the compiler.)
    /// @param mem        The address of the allocated object.
    /// @param allocSize  The true allocation size returned by the method #allocate .
    template<typename TSize>
    void dbgAcknowledgeIncreasedAllocSize( void* mem, TSize allocSize )                        const
    { lang::DbgAlloc::annotate(mem, allocSize, MAGIC); }

  #if DOXYGEN
    /// See the description of this method with prototype \alib{lang;Allocator::allowsMemSplit}.<br>
    /// (Note: This method is static. For technical reasons this cannot be reflected in this
    /// documentation)
    /// @return \c false.
           constexpr bool allowsMemSplit()                                                 noexcept;
  #else
    static constexpr bool allowsMemSplit()                             noexcept    { return false; }
  #endif

    /// Returns a temporary object (which is usually optimized out together with a call to this
    /// operator) providing high-level convenience methods for allocation.
    /// @see Class \alib{lang::AllocatorInterface}
    /// @return A temporary high-level interface into the allocator.
    lang::AllocatorInterface<TPoolAllocator> operator()()
    { return lang::AllocatorInterface<TPoolAllocator>(*this); }

    // #############################################################################################
    /// @name Specific Interface (Static Methods)
    // #############################################################################################
    /// Returns the number of relevant bits of the allocation information value returned by
    /// overloaded methods #GetAllocInformation. In other words, the value returned by these
    /// methods is between \c 0 and the power of 2 of the value returned by this method.
    /// @return \c 5 on 32-bit systems and \c 6 on 64 bit systems.
    ///         #GetAllocInformation.
    static constexpr
    int AllocationInformationBitCount()                      { return bitsof(void*) == 32 ? 5 : 6; }

    /// This static \c constexpr method returns information that this allocator needs for allocation
    /// and freeing of objects. In the case that the original allocation type is not known on
    /// deletion (e.g., if virtual destructors are used), then this piece of information has to be
    /// collected with this function (probably on construction) and provided to
    /// #FreeByAllocationInfo via custom mechanics.
    ///
    /// \note
    ///   Two options are quite obvious to implement:
    ///   1. The information is stored within the object and passed on destruction.
    ///   2. The information is retrieved from a virtual function, which is implemented using this
    ///      \c constexpr type.
    /// \note
    ///   While the second version has a small impact on code size and execution speed
    ///   (because the virtual function has to be called with deletion), the first version
    ///   increases the footprint of the object. So both approaches have their advantages and
    ///   disadvantages.<br><br>
    /// \note
    ///   A third option is explained in a step-by-step sample in chapter
    ///   \ref alib_contmono_poolallocator_metatrick of the Programmer's Manual of this module.
    ///
    /// Technically, the returned value is the exponent base 2 of the next higher power of 2
    /// value, minus the minimum supported exponent. This directly gives an index into field
    /// #hooks.
    /// @tparam T The object type that allocation information is to be retrieved for.
    /// @return Allocation information.
    template<typename T>
    static constexpr
    short           GetAllocInformation()                         { return hookIndex<sizeof(T)>(); }

#if DOXYGEN
    /// Same as templated #GetAllocInformation<T>() version, but accepts method parameter \p{size}
    ///  instead of a template parameter. Thus, only potentially \c constexpr.
    ///
    /// @tparam TIntegral The type that parameter \p{size} is provided with. If not integral,
    ///         this method is not chosen by the compiler.
    /// @param  size The size of the object to be allocated or disposed in a probalble next step.
    /// @return Allocation information. See #GetAllocInformation<T>().
    template<typename TIntegral>
    static constexpr
    short           GetAllocInformation(TIntegral size);
#else
    template<typename TIntegral>
    static constexpr
    ATMP_T_IF(short , ATMP_IS_INT(TIntegral))
    GetAllocInformation(TIntegral size)                          { return hookIndex(size_t(size)); }
#endif //if DOXYGEN

    /// Returns the allocation size for a value returned by #GetAllocInformation.
    ///
    /// @param  allocInfo The alloc information received with #GetAllocInformation.
    /// @return The next higher power of 2 in respect to the size passed as parameter to
    ///         #GetAllocInformation.
    static constexpr
    size_t GetAllocationSize(short allocInfo) { return size_t(1)<<(allocInfo + minimumHookIndex());}

    // #############################################################################################
    /// @name Specific Interface
    // #############################################################################################
    /// Allocates or recycles previously freed memory suitable to emplace an instance with the
    /// given allocation information.
    /// \note This method is to be used in combination with template method #GetAllocInformation
    ///       in situations, where the type information of the allocation is not available
    ///       or has to be transferred to a non-templated library function or similar.
    /// @param allocInfo The allocation information received with #GetAllocInformation.
    /// @return A pointer to the allocated memory.
    ALIB_API
    void*    AllocateByAllocationInfo(int allocInfo);


    /// Disposes an object (or piece of memory) which has been allocated using
    /// #AllocateByAllocationInfo.
    /// If compiler symbol \ref ALIB_DEBUG_ALLOCATIONS is set, the method will overwrite the freed
    /// memory with character <c>0xD3</c>.
    /// @param allocInfo  The allocation information received with #GetAllocInformation.
    /// @param mem        The object to dispose.
    void FreeByAllocationInfo(int allocInfo, void* mem )
    {ALIB_DCS
        ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
        #if ALIB_DEBUG_ALLOCATIONS
            // if not set, then this method had been called directly from outside and we expect
            // that the hook index is stored instead of the true requested size.
            if( dbgLastRequestedSize == 0 )
                dbgLastRequestedSize= (size_t(1) << (allocInfo + minimumHookIndex()));
            lang::DbgAlloc::checkMem(mem, dbgLastRequestedSize, MAGIC, ALIB_REL_DBG(nullptr, DbgName) );
            dbgLastRequestedSize= 0;
            lang::DbgAlloc::clearMem( mem, size_t(1) << (allocInfo + minimumHookIndex()), MAGIC );
            --dbgOpenAllocations[allocInfo];
        #endif

        *reinterpret_cast<void**>(mem)= hooks[allocInfo];
        hooks[allocInfo]= mem;
        ALIB_WARNINGS_RESTORE
    }

    /// Deletes all current pool objects with the #ChainedAllocator.
    /// The state of this class equals the state after construction.
    void Reset()
    {
        deletePool();
        std::memset( hooks, 0, sizeof( void*[qtyHooks()] ) );
        #if ALIB_DEBUG_ALLOCATIONS
            std::memset( dbgOpenAllocations, 0, sizeof( int[qtyHooks()] ) );
        #endif
        #if ALIB_DEBUG_MONOMEM
            std::memset( dbgStatAllocCounter, 0, sizeof( uinteger[qtyHooks()] ) );
        #endif

    }

    /// Returns the number of available pool objects, hence those that had been allocated,
    /// freed, and not been reused with a next allocation again.
    /// @param size The size of the object requested, as given with #allocate and #free.
    /// @return The number of pool objects of the given size.
    ALIB_API
    integer GetPoolSize(size_t size);

    /// If the compiler symbol \ref ALIB_DEBUG_ALLOCATIONS is not set, this method is empty and will
    /// be optimized out. Otherwise, this will raise an \alib assertion if the piece of allocated
    /// memory is corrupted or its allocation size is not rightfully given by the using code.
    /// @see Chapter \ref alib_contmono_further_debug of the Programmer's Manual.
    ///
    /// @tparam TSize   The type of parameter \p{size}. (Deduced by the compiler.)
    /// @param mem   The address of the allocated object.
    /// @param size  The requested allocation size of the object.
    template<typename TSize>
    void dbgCheckMemory( void* mem, TSize size )
    { lang::DbgAlloc::checkMem( mem, size, MAGIC, ALIB_REL_DBG(nullptr, DbgName)); }
    
    #if ALIB_DEBUG_ALLOCATIONS
    /// Returns the number of objects of the given \p{size} currently allocated (and not freed).<br>
    /// Only functional if the  compiler symbol \ref ALIB_DEBUG_ALLOCATIONS is set.
    /// Otherwise, this is a <c>static constexpr</c> function that returns \c 0.
    /// @see Overloaded method #DbgCountedOpenAllocations() which returns the overall number.
    /// @param size The size of the object requested, as given with #allocate and #free.
    /// @return The number of non-freed allocations.
    int     DbgCountedOpenAllocations(size_t size)
    {
        ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
        return dbgOpenAllocations[hookIndex(size)];
        ALIB_WARNINGS_RESTORE
    }

    /// Returns the number of objects currently allocated (and not freed).<br>
    /// Only functional if the compiler symbol \ref ALIB_DEBUG_ALLOCATIONS is set.
    /// Otherwise, this is a <c>static constexpr</c> function that returns \c 0.
    /// @return The number of non-freed allocations.
    int     DbgCountedOpenAllocations()
    {
        ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
        int result= 0;
        for(int i= 0; i < qtyHooks(); ++i)
            result+= dbgOpenAllocations[i];
        return result;
        ALIB_WARNINGS_RESTORE
    }

    /// Actively suppresses a warning on destruction in case #DbgCountedOpenAllocations does not
    /// return \c 0.<br>
    /// Only functional if the compiler symbol \ref ALIB_DEBUG_ALLOCATIONS is set.
    /// Otherwise, this is a <c>static constexpr</c> function that returns \c 0.
    /// @return The number of non-freed allocations.
    int     DbgSuppressNonFreedObjectsWarning()
    {
        int result= DbgCountedOpenAllocations();
        std::memset( dbgOpenAllocations, 0, sizeof( int[qtyHooks()] ) );
        return result;
    }

    #else
        static constexpr int DbgCountedOpenAllocations(size_t )                            { return 0; }
        static constexpr int DbgCountedOpenAllocations()                                   { return 0; }
        static constexpr int DbgSuppressNonFreedObjectsWarning()                       { return 0; }
    #endif

    #if ALIB_DEBUG_MONOMEM
    /// Returns the overall number of allocated (and potentially freed) objects of the given
    /// \p{size}.<br>
    /// Only functional if the  compiler symbol \ref ALIB_DEBUG_MONOMEM is set.
    /// Otherwise, this is a <c>static constexpr</c> function that returns \c 0.
    /// @see Overloaded method #DbgStatAllocCounter() which returns the overall number.
    /// @param size The size of the object requested, as given with #allocate and #free.
    /// @return The number of non-freed allocations.
    uinteger     DbgStatAllocCounter(size_t size)
    {
        ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
        return dbgStatAllocCounter[hookIndex(size)];
        ALIB_WARNINGS_RESTORE
    }

    /// Returns the overall number of objects currently allocated (and not freed).<br>
    /// Only functional if the compiler symbol \ref ALIB_DEBUG_MONOMEM is set.
    /// Otherwise, this is a <c>static constexpr</c> function that returns \c 0.
    /// @return The number of non-freed allocations.
    uinteger     DbgStatAllocCounter()
    {
        ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
        uinteger result= 0;
        for(int i= 0; i < qtyHooks(); ++i)
            result+= dbgStatAllocCounter[i];
        return result;
        ALIB_WARNINGS_RESTORE
    }

    #else
        static constexpr uinteger DbgStatAllocCounter(size_t)                          { return 0; }
        static constexpr uinteger DbgStatAllocCounter()                                { return 0; }
    #endif
}; // class TPoolAllocator


#if !DOXYGEN
extern template ALIB_API class  TPoolAllocator<      MonoAllocator, ALIB_MONOMEM_POOLALLOCATOR_DEFAULT_ALIGNMENT>;
extern template ALIB_API class  TPoolAllocator<lang::HeapAllocator, ALIB_MONOMEM_POOLALLOCATOR_DEFAULT_ALIGNMENT>;
#endif

} // namespace alib[::monomem]

/// Type alias in namespace \b alib.
/// This alias fixes template parameter \p{TAllocator} (which defines the
/// \ref alib_contmono_chaining "chained allocator") to type alias #alib::MonoAllocator, which
/// in turn defines its chained allocator to type \alib{lang;HeapAllocator}.
using  PoolAllocator       = monomem::TPoolAllocator<MonoAllocator>;

/// Type alias in namespace \b alib.
/// This alias fixes template parameter \p{TAllocator} (which defines the
/// \ref alib_contmono_chaining "chained allocator") to type \alib{lang;HeapAllocator}.
using  PoolAllocatorHA     = monomem::TPoolAllocator<lang::HeapAllocator>;

/// Type alias in namespace \b alib to denote the use of a
/// \alib{PoolAllocator} with type \alib{lang::StdContainerAllocator}.
template<typename T>
using SCAPool= lang::StdContainerAllocator<T, PoolAllocator>;

} // namespace [alib]

#endif // HPP_ALIB_MONOMEM_POOLALLOCATOR

