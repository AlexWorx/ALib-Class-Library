//==================================================================================================
/// \file
/// This implementation file is part of module \alib_monomem of the \aliblong.
/// It may be used to instantiate custom versions of \alib{monomem;TPoolAllocator}, working with
/// different alignments or a different \ref alib_contmono_chaining "chained" allocator.
///
/// @see Manual section \ref alib_manual_appendix_t_hpp_files about the nature of ".t.hpp"-files.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#include "alib/lang/owner.hpp"
#include "alib/monomem/poolallocator.hpp"

#if !DOXYGEN

namespace alib::monomem {

// =============================   Construction/Destruction/Deletion   =============================
template<typename TAllocator, size_t TAlignment>
TPoolAllocator<TAllocator,TAlignment>::TPoolAllocator   ( TAllocator& pAllocator )
: allocMember(pAllocator)
#if ALIB_DEBUG_CRITICAL_SECTIONS
,lang::DbgCriticalSections("PoolAllocator")
#endif
ALIB_DBG(,DbgName(pAllocator.DbgName))
{
    #if ALIB_CPP_STANDARD < 20 && defined(_WIN32) // otherwise statically asserted in header
    ALIB_ASSERT_ERROR(lang::BitCount(TAlignment) == 1, "MONOMEM",
            "The fixed alignment of the pool allocator is not a power of 2. "
            "Adjust template parameter TAlignment." )
    #endif

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

template<typename TAllocator, size_t TAlignment>
TPoolAllocator<TAllocator,TAlignment>::~TPoolAllocator   ()
{
    if ( hooks == nullptr )
        return;

    deletePool();
    allocMember::AI().template FreeArray<void*>(hooks, qtyHooks());
    ALIB_DBG( hooks= nullptr; )

    #if ALIB_DEBUG_ALLOCATIONS
        ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
        for (short i = 0; i < qtyHooks(); ++i)
        {
            if ( dbgOpenAllocations[i] > 0)
                ALIB_WARNING( "MONOMEM", NString256() <<
                      "PoolAllocator '"<<DbgName<<"' destructor: There are still "
                    << dbgOpenAllocations[i] << " objects of size "
                    << GetAllocationSize(i) << " not freed. "
                    "This indicates a potential memory leak."  )
        }
        allocMember::AI().template FreeArray<int>(dbgOpenAllocations, qtyHooks() );
        ALIB_WARNINGS_RESTORE
    #endif
    #if ALIB_DEBUG_MONOMEM
        allocMember::AI().template DeleteArray<uinteger>(dbgStatAllocCounter, qtyHooks() );
    #endif

}


template<typename TAllocator, size_t TAlignment>
void TPoolAllocator<TAllocator,TAlignment>::deletePool()
{
    for (int idx = 0; idx < qtyHooks(); ++idx)
    {
        size_t allocSize= lang::DbgAlloc::extSize( size_t(1) <<  (idx + minimumHookIndex()) );
        ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
        void* elem= hooks[idx];
        while( elem )
        {
            void* next= *reinterpret_cast<void**>( elem );
            allocMember::GetAllocator().free( elem, allocSize );
            elem= next;
        }
        ALIB_WARNINGS_RESTORE
    }
}


// =======================================    Allocation    ========================================
template<typename TAllocator, size_t TAlignment>
void* TPoolAllocator<TAllocator,TAlignment>::reallocate( void* mem, size_t oldSize, size_t& newSize, size_t pAlignment )
{
    short oldSizeIdx;
    short newSizeIdx;

    {ALIB_DCS

        ALIB_ASSERT_ERROR(pAlignment <= TAlignment, "MONOMEM",
           "The requested alignment is higher than what was specified with "
           "template parameter TAlignment." )
        (void) pAlignment;

        oldSizeIdx=   hookIndex(oldSize);
        newSizeIdx=   hookIndex(newSize);
        if(  newSizeIdx == oldSizeIdx )
            return mem;

        newSize=  GetAllocationSize(newSizeIdx);
    }

    auto newMem= AllocateByAllocationInfo( newSizeIdx );
    std::memcpy( newMem, mem, oldSize );
    #if ALIB_DEBUG_ALLOCATIONS
    dbgLastRequestedSize= oldSize;
    #endif
    FreeByAllocationInfo( oldSizeIdx, mem );
    return newMem;
}

template<typename TAllocator, size_t TAlignment>
void*    TPoolAllocator<TAllocator,TAlignment>::AllocateByAllocationInfo(int allocInfo)
{ALIB_DCS
    #if ALIB_DEBUG_ALLOCATIONS
        // if not set, then this method had been called directly from outside and we store
        // the hook index instead of the true requested size.
        if( dbgLastRequestedSize == 0 )
            dbgLastRequestedSize= size_t(allocInfo);
    #endif

    ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
    #if ALIB_DEBUG_ALLOCATIONS
        ++dbgOpenAllocations[allocInfo];
    #endif
    #if ALIB_DEBUG_MONOMEM
        ++dbgStatAllocCounter[allocInfo];
    #endif


    void** hook= &hooks[allocInfo];
    ALIB_WARNINGS_RESTORE

    // found a recyclable?
    if( *hook )
    {
        void* mem= *hook;
        *hook= *reinterpret_cast<void**>(mem);

        #if ALIB_DEBUG_ALLOCATIONS
            lang::DbgAlloc::annotate( mem, dbgLastRequestedSize, MAGIC );
            dbgLastRequestedSize= 0;
        #endif
        return mem;
    }

    // create a new object, as no recyclable is available
    size_t allocSize= lang::DbgAlloc::extSize( size_t(1) <<  (allocInfo + minimumHookIndex()) );
    auto* mem= allocMember::AI().Alloc( allocSize, TAlignment );

    #if ALIB_DEBUG_ALLOCATIONS
        lang::DbgAlloc::annotate( mem, dbgLastRequestedSize, MAGIC );
        dbgLastRequestedSize= 0;
    #endif

    return mem;
}

// =========================================    Other    ===========================================

template<typename TAllocator, size_t TAlignment>
integer TPoolAllocator<TAllocator,TAlignment>::TPoolAllocator::GetPoolSize(size_t size)
{
    integer result= 0;
    ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
    void* node= hooks[hookIndex(size)];
    ALIB_WARNINGS_RESTORE
    while( node )
    {
        node= *reinterpret_cast<void**>(node);
        ++result;
    }
    return result;
}

} // namespace [alib::monomem]

#endif //  !DOXYGEN

