//==================================================================================================
/// \file
/// This header file is part of the \aliblong. It does not belong to an \alibmod and is
/// included in any \alibdist.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_LANG_RTTRALLOCATOR
#define HPP_ALIB_LANG_RTTRALLOCATOR
#pragma once

#if ALIB_DEBUG
#   include "alib/lang/dbgtypedemangler.hpp"
#endif
#include "alib/lang/allocation.hpp"
#include "alib/lang/sidilist.hpp"

#if !DOXYGEN
#   if ALIB_DEBUG
#      include <iostream>
#      define DBGMEMOUT(output) std::cout << output << std::endl;
#   else
#      define DBGMEMOUT(output)
#   endif
#endif //if DOXYGEN

#include <memory>


#if ALIB_MONOMEM
namespace alib::monomem
{
    template<typename TAllocator>class TMonoAllocator;
}
#endif

namespace alib::lang {

//==================================================================================================
/// This structs' name stands for "run-time type recycling allocator".
/// The struct encapsulates an allocator and recycles (caches) objects whose size and alignment are
/// only determined and detected at run-time.
/// This type should \e only be used in combination with struct
/// \alib{lang;StdContainerAllocatorRecycling} or in similar (unlikely) situations when the type of
/// recyclable objects is unknown at compile time.
/// \note The reason why the size of node types of containers of the C++ standard library is known
///       only at run-time, is that the implementation of the containers is not standardized.
///       Hence, in fact the node size is of course known at compile time, it is just not accessible
///       in a platform-independent fashion.
///
/// This type is provided by \alib to support recycling of monotonically allocated memory with
/// container types provided by the C++ standard library. With those, the internal node types are unspecified.
/// A typical implementation and use of the containers will always allocate the same object size.
/// This is detected with the first allocation and from this point in time, future de-allocations
/// will recycle pieces of memory of exactly this type.
///
/// If type \alib{lang::HeapAllocator} is given as template parameter \p{TAllocator}, this recycler
/// uses <c>std::malloc</c> and <c>std::free</c> for memory allocation and de-allocation.
/// While such operation mode does not imply all performance benefits of monotonic allocation
/// scenarios, still the recycling of node objects may avoid many malloc/free operations and
/// therefore reduce memory fragmentation significantly.<br>
/// Method #RecycleChunk will not create recyclable objects in that mode, but will duly
/// use <c>std::free</c> to directly free a chunk of non-object size.
///
/// @tparam TAllocator The allocator type, as prototyped with \alib{lang;Allocator}.
//==================================================================================================
template<typename TAllocator>
struct RTTRAllocator : AllocatorMember<TAllocator>
{
    /// The allocator type that \p{TAllocator} specifies.
    using allocBase=   AllocatorMember<TAllocator>;

    /// The node type of the internal node type used for stacking  recyclables. Besides inheriting
    /// the single-list pointer, this type is empty.
    struct Node : SidiNodeBase<Node>
    {};

    /// List of destructed objects available for recycling.
    SidiListHook<Node>          stack;

    /// The object size of recyclables. Will be detected with the first invocation of #Get.
    size_t                      detectedObjectSize                                         = 0;

    /// The required object alignment. Will be detected with the first invocation of #Get.
    size_t                      detectedObjectAlignment                                    = 0;

    #if ALIB_DEBUG
        /// The detected object's run-time type information struct.<br>
        /// Available only in debug-builds.
        const std::type_info*   dbgDetectedObjectTypeInfo                                 = nullptr;

        /// Flag on raising an \alib warning. Defaults to \c true and set to \c false when a
        /// corresponding warning was given..<br>
        /// Available only in debug-builds.
        bool                    dbgWarnDifferentObjectTypeAlloc                              = true;

        /// Flag on raising an \alib warning. Defaults to \c true and set to \c false when a
        /// corresponding warning was given..<br>
        /// Available only in debug-builds.
        bool                    dbgWarnDifferentObjectTypeDealloc                            = true;

        /// Flag on raising an \alib warning. Defaults to \c true and set to \c false when a
        /// corresponding warning was given..<br>
        /// Available only in debug-builds.
        bool                    dbgWarnDeallocationPriorToAllocation                         = true;

        /// Flag on raising an \alib warning. Defaults to \c true and set to \c false when a
        /// corresponding warning was given.<br>
        /// Available only in debug-builds.
        bool                    dbgWarnRecycleChunkPriorToAllocation                         = true;
    #endif

   /// Constructor taking the monotonic allocator.
   ///
   /// If \c nullptr is given, this recycler uses <c>std::malloc</c> and <c>std::free</c> for memory
   /// allocation and de-allocation.
   ///
   /// @param pAllocator The monotonic allocator. If \c nullptr is given, still recycling of node
   ///                   objects is performed.
    RTTRAllocator( TAllocator& pAllocator )                 noexcept   : allocBase(pAllocator)    {}

    // #############################################################################################
    // ### Allocation
    // #############################################################################################
#if DOXYGEN
    //==============================================================================================
    /// Allocates or recycles memory for the dedicated object type. With the first invocation
    /// of this method, this type is determined object type.
    /// In debug-builds, this method will raise an \alib warning in case a different
    /// object type is requested.
    ///
    /// @param size         The requested size.
    /// @param alignment    The requested alignment
    /// @param dbgTypeInfo  The type information of the object to allocate.
    ///                     Available only in debug-builds.
    /// @return The requested memory.
    //==============================================================================================
    inline
    void* Get( size_t size, size_t alignment, const type_info& dbgTypeInfo);
#else
    void* Get( size_t size, size_t alignment ALIB_DBG(, const std::type_info& dbgTypeInfo) )
    {
        ALIB_DBG( (void) dbgTypeInfo; )

        // detect object size
        if(detectedObjectSize == 0)
        {
            detectedObjectSize               = size;
            detectedObjectAlignment          = alignment;
  ALIB_DBG( dbgDetectedObjectTypeInfo        = &dbgTypeInfo;   )

            DBGMEMOUT("Object type detected     : " << DbgTypeDemangler(dbgTypeInfo).Get() )
            ALIB_ASSERT_ERROR( alignment >= alignof(Node),
                               "MONOMEM/RTTRA", "Struct RTTRAllocator cannot be used to recycle types with an alignment "
                               "smaller than ", alignof(Node) )
        }

        if( (size == detectedObjectSize) && (detectedObjectAlignment== alignment) )
        {
            if( !stack.isEmpty() )
            {
                #if ALIB_DEBUG_MONOMEM
                    DBGMEMOUT( "Recycling object.    Type: " << DbgTypeDemangler(dbgTypeInfo).Get() )
                #endif
                return reinterpret_cast<char*>( stack.popFront() );
            }
        }
        #if ALIB_DEBUG
        else
        {
            if(dbgWarnDifferentObjectTypeAlloc)
            {
                DBGMEMOUT( "Warning: ALIB/RTTRA" )
                DBGMEMOUT("A different object was requested for allocation!")
                DBGMEMOUT("  Previous type : <" << DbgTypeDemangler(*dbgDetectedObjectTypeInfo).Get() << ">")
                DBGMEMOUT("  Requested type: <" << DbgTypeDemangler( dbgTypeInfo              ).Get() << ">")
                DBGMEMOUT("Note: This allocator may not be efficient when used."                                   )
                DBGMEMOUT("      If this is a use case using a 'std' library container, this message indicates"    )
                DBGMEMOUT("      that a RTTRAllocator was shared between different container instantiations."      )
                DBGMEMOUT("      If this is not the case, than an 'unusual' implementation of such C++ library may")
                DBGMEMOUT("      prevent this concept from working. See ALib manual for further information"       )
                dbgWarnDifferentObjectTypeAlloc= false;
            }
            DBGMEMOUT( "Allocating a different object type \"" << DbgTypeDemangler(dbgTypeInfo).Get() << "\"")
            DBGMEMOUT( "  Note: This object cannot be recycled." )
            return allocBase::AI().Alloc( size, alignment );
        }
        #endif

        #if ALIB_DEBUG_MONOMEM
            DBGMEMOUT( "Allocating object.   Type: \"" << DbgTypeDemangler(dbgTypeInfo).Get() << "\"" )
        #endif

        return allocBase::AI().Alloc( size, alignment );
    }
#endif // DOXYGEN

#if DOXYGEN
    //==============================================================================================
    /// Allocates memory for a type different to the dedicated, detected object type.
    /// @param size         The requested size.
    /// @param alignment    The requested alignment
    /// @param dbgTypeInfo  The type information of the object to allocate.
    ///                     Available only in debug-builds.
    /// @return The requested memory.
    //==============================================================================================
    inline
    void* AllocUnrelated( size_t size, size_t alignment, const type_info& dbgTypeInfo);
#else
    void* AllocUnrelated( size_t size, size_t alignment ALIB_DBG(, const std::type_info& dbgTypeInfo))
    {
        ALIB_DBG( (void) dbgTypeInfo; )
        #if ALIB_DEBUG_MONOMEM
            DBGMEMOUT( "Allocating other.    Type: " << DbgTypeDemangler(dbgTypeInfo).Get() )
        #endif
        return allocBase::AI().Alloc( size, alignment );
    }
#endif // DOXYGEN


    // #############################################################################################
    // ### De-allocation
    // #############################################################################################
#if DOXYGEN
    //==============================================================================================
    /// Deallocates memory for the dedicated, detected object type.
    ///
    /// In debug-builds, this method will raise an \alib warning in case a different
    /// object type is deallocated as had been detected.
    /// Furthermore, a warning is raised in case no previous call to #Get has been performed.
    ///
    /// @param mem          The object to deallocate.
    /// @param size         The size of the object to deallocate.
    /// @param alignment    The alignment of the object to deallocate.
    /// @param dbgTypeInfo  The type information of the object to de-allocate.
    ///                     Available only in debug-builds.
    //==============================================================================================
    inline
    void Recycle( void* mem, size_t size, size_t alignment, const type_info& dbgTypeInfo );
#else
    void Recycle( void* mem, size_t size, size_t alignment ALIB_DBG(, const std::type_info& dbgTypeInfo) )
    {
        if(    size      == detectedObjectSize
            && alignment == detectedObjectAlignment )
        {
            stack.pushFront( reinterpret_cast<Node*>( mem ) );
            #if ALIB_DEBUG_MONOMEM
                DBGMEMOUT( "Stacking object.     Type: " << DbgTypeDemangler(dbgTypeInfo).Get() )
            #endif
        }
        else
        {
            allocBase::GetAllocator().free(mem, size);
            #if ALIB_DEBUG
                if( detectedObjectSize == 0 )
                {
                    if( dbgWarnDeallocationPriorToAllocation)
                    {
                        DBGMEMOUT( "Warning: ALIB/RTTRA" )
                        DBGMEMOUT(  "De-allocation before a first object allocation needed to detect recyclable type!"  )
                        DBGMEMOUT(  "  De-allocated object type: <" << DbgTypeDemangler( dbgTypeInfo ).Get() << ">" )
                        DBGMEMOUT(  "Note: This allocator may not be efficient when used."                                )
                        DBGMEMOUT(  "      If this is a use case using a 'std' library container, this message indicates" )
                        DBGMEMOUT(  "      an 'unusual' implementation of such C++ standard library."                     )
                    }
                    dbgWarnDeallocationPriorToAllocation= false;
                }
                else
                {
                    if( dbgWarnDifferentObjectTypeDealloc )
                    {
                        DBGMEMOUT( "Warning: ALIB/RTTRA" )
                        DBGMEMOUT(  "A different object for was requested for de-allocoation!"                               )
                        DBGMEMOUT(  "  Previous type : <" << DbgTypeDemangler(*dbgDetectedObjectTypeInfo).Get() << ">" )
                        DBGMEMOUT(  "  Requested type: <" << DbgTypeDemangler( dbgTypeInfo              ).Get() << ">" )
                        DBGMEMOUT(  "Note: This allocator may not be efficient when used."                                   )
                        DBGMEMOUT(  "      If this is a use case using a 'std' library container, this message indicates"    )
                        DBGMEMOUT(  "      that a RTTRAllocator was shared between different container instantiations."      )
                        DBGMEMOUT(  "      If this is not the case, than an 'unusual' implementation of such C++ library may")
                        DBGMEMOUT(  "      prevent this concept from working. See ALib manual for further information"       )
                        dbgWarnDifferentObjectTypeDealloc= false;
                    }
                }
            #endif
        }
    }
#endif


#if DOXYGEN
    //==============================================================================================
    /// Deallocates memory for a type different to the dedicated, detected object type.
    ///
    /// In debug-builds, this method will raise an \alib warning no previous call to #Get
    /// has been performed.<br>
    /// Furthermore, a warning is raised in case that the provided memory chunk is too small
    /// to be sliced into at least one recyclable object.
    ///
    /// @param mem          The object to deallocate.
    /// @param size         The size of the object to deallocate.
    /// @param dbgTypeInfo  The type information of the object to de-allocate.
    ///                     Available only in debug-builds.
    //==============================================================================================
    inline
    void RecycleChunk( void* mem, size_t size, const type_info& dbgTypeInfo );
#else
    void RecycleChunk( void* memUnaligned, size_t size ALIB_DBG(, const std::type_info& dbgTypeInfo) )
    {
        // This whole exercise must only be done with MonoAllocator and if ALIB_DEBUG_ALLOCATIONS is
        // not set. (This is ensured by 'TAllocator::allowsMemSplit()')
        if( !TAllocator::allowsMemSplit() )
        {
            allocBase::GetAllocator().free(memUnaligned, size);
            return;
        }

        // if object size not detected, yet, we cannot create recyclables.
        ALIB_DBG( size_t origSize= size; )
        if( detectedObjectSize == 0)
        {
            #if ALIB_DEBUG
            if( dbgWarnRecycleChunkPriorToAllocation )
            {
                DBGMEMOUT( "Warning: ALIB/RTTRA" )
                DBGMEMOUT( "De-allocation before a first object allocation needed to detect recyclable type!"   )
                DBGMEMOUT( "  De-allocated object type: <" << DbgTypeDemangler( dbgTypeInfo ).Get() << ">." )
                DBGMEMOUT( "Note: If this recycler is used with a 'std' library container, this either"           )
                DBGMEMOUT( "      indicates an 'unusual' implementation of such C++ standard library,"            )
                DBGMEMOUT( "      or a manual shrink of the capacity without any prior object insertion."         )
                dbgWarnRecycleChunkPriorToAllocation = false ;
            }
            #endif

            return;
        }

        // align beginning of buffer
        char* mem = reinterpret_cast<char*>( (size_t(memUnaligned) + detectedObjectAlignment - 1) & ~(detectedObjectAlignment -1) );
        size-= size_t(mem - reinterpret_cast<char*>(memUnaligned));

        // create recyclables
        ALIB_DBG( size_t cntStackedObjects= 0; )
        while(size > detectedObjectSize)
        {
            stack.pushFront( reinterpret_cast<Node*>( mem ) );
            ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
            mem  =  reinterpret_cast<char*>(mem) + detectedObjectSize;
            ALIB_WARNINGS_RESTORE
            size -= detectedObjectSize;
            ALIB_DBG( ++cntStackedObjects; )
        }

        #if ALIB_DEBUG
            if( cntStackedObjects > 0 )
            {
                DBGMEMOUT( "Warning: ALIB/RTTRA" )
                DBGMEMOUT( "De-allocated chunk's size is smaller than detected object size."                                         )
                DBGMEMOUT( "  Deallocated object: Type: <" << DbgTypeDemangler( dbgTypeInfo ).Get() << ">"                     )
                DBGMEMOUT( "                      Size: "  << origSize     << " bytes"                                               )
                DBGMEMOUT( "  Detected object:    Type: <" << DbgTypeDemangler( *dbgDetectedObjectTypeInfo ).Get() << ">"      )
                DBGMEMOUT( "                      Size: "  << detectedObjectSize << " bytes, alignment: " << detectedObjectAlignment )
                DBGMEMOUT( "Note: If this recycler is used with a <std::unordered_map> or <std::unordered_set>," )
                DBGMEMOUT( "      this message may be eliminated by reserving a reasonable initial bucket size." )
            }

        #endif
        #if ALIB_DEBUG_MONOMEM
            DBGMEMOUT( "ALIB/MEMORY" )
            DBGMEMOUT( "Stacking " << cntStackedObjects << " objects from de-allocated memory of size "
                        << origSize << " (lost " << origSize - cntStackedObjects * detectedObjectSize
                        << " bytes). Deallocated type: " << DbgTypeDemangler(dbgTypeInfo).Get() )
        #endif
    }
#endif

}; // RTTRAllocator

} // namespace [alib::lang]

#undef DBGMEMOUT
#endif // HPP_ALIB_LANG_RTTRALLOCATOR

