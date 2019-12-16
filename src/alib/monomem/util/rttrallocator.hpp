/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_monomem of the \aliblong.
 *
 * \emoji :copyright: 2013-2019 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_MONOMEM_UTIL_RTTRALLOCATOR
#define HPP_ALIB_MONOMEM_UTIL_RTTRALLOCATOR

#if !defined(HPP_ALIB_MONOMEM_MONOALLOCATOR)
#   include "alib/monomem/monoallocator.hpp"
#endif

#if !defined(HPP_ALIB_FS_LISTS_FORWARDLIST)
#   include "alib/lib/fs_lists/forwardlist.hpp"
#endif

#if ALIB_DEBUG
#   if ALIB_STRINGS && !defined (HPP_ALIB_STRINGS_LOCALSTRING)
#      include "alib/strings/localstring.hpp"
#   endif
#   if !defined (HPP_ALIB_FS_DEBUG_TYPEDEMANGLER)
#      include "alib/lib/fs_debug/typedemangler.hpp"
#   endif
#endif

namespace aworx { namespace lib { namespace monomem {

/**
 * Utility types of module \alib_monomem.
 */
namespace util {

/** ************************************************************************************************
 * This struct's name stands for "run-time type recycling allocator".
 * The struct encapsulates an allocator and recycles (caches) objects whose size and alignment are
 * only determined and detected at run-time.<br>
 * This type should \e only be used in combination with struct \alib{monomem,StdContMARecycling}
 * or in similar (unlikely) situations when the type of recyclable objects is unknown at compile
 * time.
 *
 * It is provided by \alib to support recycling of monotonically allocated memory with container
 * types provided by the C++ standard library. With those, the internal node types are unspecified.
 * A typical implementation and use of the containers will always allocate the same object size.
 * This is detected with the first allocation and from this point in time, future de-allocations
 * will recycle pieces of memory of exactly this type.
 *
 * If no \alib{monomem,MonoAllocator} is given with construction, this recycler uses
 * <c>std::malloc</c> and <c>std::free</c> for memory allocation and de-allocation.
 * While such operation mode does not imply all performance benefits of monotonic allocation
 * scenarios, still the recycling of node objects may avoid many malloc/free operations and
 * therefore reduce memory fragmentation significantly.<br>
 * Method #RecycleChunk will not create recyclable objects in that mode, bute will duly
 * use <c>std::free</c> to directly free a chunk of non-object size.
 **************************************************************************************************/
struct RTTRAllocator
{
    /** The node type of the internal node type used for stacking  recyclables. Besides inheriting
     *  the single-list pointer, this type is empty.                                              */
    struct Node : ForwardNode<Node>
    {};

    /** The monotonic allocator */
    MonoAllocator*              allocator;

    /** List of destructed objects available for recycling. */
    ForwardList<Node>           stack;

    /** The object size of recylables. Will be detected with the first invocation of #Get. */
    size_t                      detectedObjectSize                                              = 0;

    /** The required object alignment. Will be detected with the first invocation of #Get. */
    size_t                      detectedObjectAlignment                                         = 0;

    #if ALIB_DEBUG
        /** The detected object's run-time type information struct.<br>
         *  Available only in debug builds.                                               */
        const std::type_info*   dbgDetectedObjectTypeInfo                                 = nullptr;

        /** Flag on raising an \alib warning. Defaults to \c true and set to \c false when a
         *  corresponding warning was given..<br>
         *  Available only in debug builds.                                                   */
        bool                    dbgWarnDifferentObjectTypeAlloc                              = true;

        /** Flag on raising an \alib warning. Defaults to \c true and set to \c false when a
         *  corresponding warning was given..<br>
         *  Available only in debug builds.                                                   */
        bool                    dbgWarnDifferentObjectTypeDealloc                            = true;

        /** Flag on raising an \alib warning. Defaults to \c true and set to \c false when a
         *  corresponding warning was given..<br>
         *  Available only in debug builds.                                                   */
        bool                    dbgWarnDeallocationPriorToAllocation                         = true;

        /** Flag on raising an \alib warning. Defaults to \c true and set to \c false when a
         *  corresponding warning was given..<br>
         *  Available only in debug builds.                                                   */
        bool                    dbgWarnRecycleChunkPriorToAllocation                         = true;
    #endif

   /**
    * Constructor taking the monotonic allocator.
    *
    * If \c nullptr is given, this recycler uses <c>std::malloc</c> and <c>std::free</c> for memory
    * allocation and de-allocation.
    *
    * @param pAllocator The monotonic allocator. If \c nullptr is given, still recycling of node
    *                   objects is performed.
    */
    RTTRAllocator( MonoAllocator* pAllocator)
    : allocator(pAllocator)
    {
        MONOMEM_FILE_DOMAIN("UTIL/RTTA")
    }


    // #############################################################################################
    // ### Allocation
    // #############################################################################################
#if defined(ALIB_DOX)
    /** ********************************************************************************************
     * Allocates or recycles memory for the dedicated object type. With the first invocation
     * of this method, this type is determined object type.
     * In debug builds, this method will raise an \alib warning in case a different
     * object type is requested.
     *
     * @param size         The requested size.
     * @param alignment    The requested alignment
     * @param dbgTypeInfo  The type information of the object to allocate.
     *                     Available only in debug builds.
     * @return The requested memory.
     **********************************************************************************************/
    inline
    char* Get( size_t size, size_t alignment, const type_info& dbgTypeInfo);
#else
    char* Get( size_t size, size_t alignment ALIB_DBG(, const std::type_info& dbgTypeInfo) )
    {
        ALIB_DBG( (void) dbgTypeInfo; )

        // detect object size
        if(detectedObjectSize == 0)
        {
            detectedObjectSize               = size;
            detectedObjectAlignment          = alignment;
  ALIB_DBG( dbgDetectedObjectTypeInfo        = &dbgTypeInfo;   )
            MONOMEM_INFO("Object type detected     : {!Q<>}", dbgTypeInfo  )
            ALIB_ASSERT_ERROR( alignment >= alignof(Node),
                "Struct RTTRAllocator can not be used to recycle types with an alignment "
                "smaller than ", alignof(Node) )
        }

        if( size == detectedObjectSize && detectedObjectAlignment == alignment )
        {
            if( stack.isNotEmpty() )
            {
                MONOMEM_VERBOSE( "Recycling object.    Type: {!Q<>}", dbgTypeInfo )

                return reinterpret_cast<char*>( stack.popFront() );
            }
        }
        #if ALIB_DEBUG
        else
        {
            #if ALIB_STRINGS
                ALIB_ASSERT_WARNING( !dbgWarnDifferentObjectTypeAlloc, NString1K() <<
                  "A different object was requested for allocation!\n"
                  "  Previous type : <" << DbgTypeDemangler(*dbgDetectedObjectTypeInfo).Get() << ">\n"
                  "  Requested type: <" << DbgTypeDemangler( dbgTypeInfo              ).Get() << ">\n" <<
                  "Note: This allocator may not be efficient when used.\n"
                  "      If this is a use case using a 'std' library container, this message indicates\n"
                  "      that a RTTRAllocator was shared between different container instantiations.\n"
                  "      If this is not the case, than an 'unusual' implementation of such C++ library may \n"
                  "      prevent this concept from working. See ALib manual for further information"            )
            #else
                ALIB_ASSERT_WARNING( !dbgWarnDifferentObjectTypeAlloc,
                  "A different object was requested for allocation!\n" )
            #endif

            dbgWarnDifferentObjectTypeAlloc= false;
            MONOMEM_WARNING( "Allocating a different object type {!Q<>}\\n  Note: This object can not be recycled.", dbgTypeInfo )
            return allocator ? allocator->Alloc( size, alignment )
                             : static_cast<char*>(std::malloc( size ));
        }
        #endif

        MONOMEM_VERBOSE( "Allocating object.   Type: {!Q<>}", dbgTypeInfo )

        ALIB_ASSERT_ERROR( allocator != nullptr || alignment <= alignof(std::max_align_t),
            "Struct RTTRAllocator can not be used to allocate types with an alignment "
            "greater than 'alignof(std::max_align_t)'." )
        return allocator ? allocator->Alloc( size, alignment )
                         : static_cast<char*>(std::malloc( size ));
    }
#endif // defined(ALIB_DOX)

#if defined(ALIB_DOX)
    /** ********************************************************************************************
     * Allocates memory for a type different to the dedicated, detected object type.
     * @param size         The requested size.
     * @param alignment    The requested alignment
     * @param dbgTypeInfo  The type information of the object to allocate.
     *                     Available only in debug builds.
     * @return The requested memory.
     **********************************************************************************************/
    inline
    char* AllocUnrelated( size_t size, size_t alignment, const type_info& dbgTypeInfo);
#else
    char* AllocUnrelated( size_t size, size_t alignment ALIB_DBG(, const std::type_info& dbgTypeInfo))
    {
        ALIB_DBG( (void) dbgTypeInfo; )
        MONOMEM_VERBOSE( "Allocating other.    Type: {!Q<>}", dbgTypeInfo )
        return allocator ? allocator->Alloc( size, alignment )
                         : static_cast<char*>(std::malloc( size ));
    }
#endif // defined(ALIB_DOX)


    // #############################################################################################
    // ### De-allocation
    // #############################################################################################
#if defined(ALIB_DOX)
    /** ********************************************************************************************
     * Deallocates memory for the dedicated, detected object type.
     *
     * In debug builds, this method will raise an \alib warning in case a different
     * object type is deallocated as had been detected.
     * Furthermore, a warning is raised in case no previous call to #Get has been performed.
     *
     * @param mem          The object to deallocate.
     * @param size         The size of the object to deallocate.
     * @param alignment    The alignment of the object to deallocate.
     * @param dbgTypeInfo  The type information of the object to de-allocate.
     *                     Available only in debug builds.
     **********************************************************************************************/
    inline
    void Recycle( void* mem, size_t size, size_t alignment, const type_info& dbgTypeInfo );
#else
    void Recycle( void* mem, size_t size, size_t alignment ALIB_DBG(, const std::type_info& dbgTypeInfo) )
    {
        if(    size      == detectedObjectSize
            && alignment == detectedObjectAlignment )
        {
            stack.pushFront( reinterpret_cast<Node*>( mem ) );

            MONOMEM_VERBOSE( "Stacking object.     Type: {!Q<>}", dbgTypeInfo )
        }
        else
        {
            if( allocator == nullptr )
                std::free( mem );
            #if ALIB_DEBUG
                if( detectedObjectSize == 0 )
                {
                    #if ALIB_STRINGS
                        ALIB_ASSERT_WARNING( !dbgWarnDeallocationPriorToAllocation,  NString1K() <<
                            "De-allocation prior to a first object allocation needed to detect recyclable type!\n"
                            "  De-allocated object type: <" << DbgTypeDemangler( dbgTypeInfo ).Get() << ">\n" <<
                            "Note: This allocator may not be efficient when used.\n"
                            "      If this is a use case using a 'std' library container, this message indicates\n"
                            "      an 'unusual' implementation of such C++ standard library."                   )
                    #else
                        ALIB_ASSERT_WARNING( !dbgWarnDeallocationPriorToAllocation,
                            "De-allocation prior to a first object allocation needed to detect recyclable type!" )
                        (void) dbgTypeInfo;
                    #endif
                    dbgWarnDeallocationPriorToAllocation= false;
                }
                else
                {
                    #if ALIB_STRINGS
                        ALIB_ASSERT_WARNING( !dbgWarnDifferentObjectTypeDealloc, NString1K() <<
                             "A different object for was requested for de-allocoation!\n"
                             "  Previous type : <" << DbgTypeDemangler(*dbgDetectedObjectTypeInfo).Get() << ">\n"
                             "  Requested type: <" << DbgTypeDemangler( dbgTypeInfo              ).Get() << ">\n" <<
                             "Note: This allocator may not be efficient when used.\n"
                             "      If this is a use case using a 'std' library container, this message indicates\n"
                             "      that a RTTRAllocator was shared between different container instantiations.\n"
                             "      If this is not the case, than an 'unusual' implementation of such C++ library may \n"
                             "      prevent this concept from working. See ALib manual for further information"  )
                    #else
                        ALIB_ASSERT_WARNING( !dbgWarnDifferentObjectTypeDealloc,
                             "A different object for was requested for de-allocoation!" )
                    #endif
                    dbgWarnDifferentObjectTypeDealloc= false;
                }
            #endif
        }
    }
#endif


#if defined(ALIB_DOX)
    /** ********************************************************************************************
     * Deallocates memory for a type different to the dedicated, detected object type.
     *
     * In debug builds, this method will raise an \alib warning no previous call to #Get
     * has been performed.<br>
     * Furthermore, a warning is raised in case that the provided memory chunk is too small
     * to be sliced into at least one recyclable object.
     *
     * @param mem          The object to deallocate.
     * @param size         The size of the object to deallocate.
     * @param alignment    The alignment of the object to deallocate.
     * @param dbgTypeInfo  The type information of the object to de-allocate.
     *                     Available only in debug builds.
     **********************************************************************************************/
    inline
    void RecycleChunk( char* mem, size_t size, size_t alignment, const type_info& dbgTypeInfo );
#else
    void RecycleChunk( void* mem, size_t size, size_t alignment ALIB_DBG(, const std::type_info& dbgTypeInfo) )
    {
        // in non-monomem mode, chunks can not be split.
        if ( allocator == nullptr )
        {
            std::free( mem );
            return;
        }

        // if object size not detected, yet, we can not create recyclables.
        ALIB_DBG( size_t origSize= size; )
        if( detectedObjectSize == 0)
        {
            #if ALIB_STRINGS
                ALIB_ASSERT_WARNING( !dbgWarnRecycleChunkPriorToAllocation,  NString1K() <<
                  "De-allocation prior to a first object allocation needed to detect recyclable type!\n"
                  "  De-allocated object type: <" << DbgTypeDemangler( dbgTypeInfo ).Get() << ">\n"
                  "Note: If this recycler is used with a 'std' library container, this either\n"
                  "      indicates an 'unusual' implementation of such C++ standard library,\n"
                  "      or a manual shrink of the capacity without any prior object insertion."    )
            #else
                ALIB_ASSERT_WARNING( !dbgWarnRecycleChunkPriorToAllocation,
                  "De-allocation prior to a first object allocation needed to detect recyclable type!" )
                ALIB_DBG( (void) dbgTypeInfo );
            #endif
            ALIB_DBG( dbgWarnRecycleChunkPriorToAllocation = false ;)
            return;
        }

        // align beginning of buffer (if necessary)
        if( detectedObjectAlignment > alignment )
            std::align( detectedObjectAlignment ,    // alignment: the desired alignment
                        detectedObjectSize      ,    // size     : the size of the storage to be aligned
                        mem                     ,    // ptr      : pointer to contiguous storage of at least 'space' bytes
                        size                      ); // space    : the size of the buffer in which to operate

        // create recyclables
        ALIB_DBG( size_t cntStackedObjects= 0; )
        while(size > detectedObjectSize)
        {
            stack.pushFront( reinterpret_cast<Node*>( mem ) );
            mem  =  reinterpret_cast<char*>(mem) + detectedObjectSize;
            size -= detectedObjectSize;
            ALIB_DBG( ++cntStackedObjects; )
        }

        #if ALIB_STRINGS
            ALIB_ASSERT_WARNING( cntStackedObjects > 0, NString1K() <<
              "De-allocated chunk's size is smaller than detected object size.\n"
              "  Deallocated object: Type: <" << DbgTypeDemangler( dbgTypeInfo ).Get() << ">\n"
              "                      Size: "  << origSize     << " bytes, alignment: " << alignment << "\n"
              "  Detected object:    Type: <" << DbgTypeDemangler( *dbgDetectedObjectTypeInfo ).Get() << ">\n"
              "                      Size: "  << detectedObjectSize << " bytes, alignment: " << detectedObjectAlignment << "\n"
              "Note: If this recycler is used with a <std::unordered_map> or <std::unordered_set>,\n"
              "      this message may be eliminated by reserving a reasonable initial bucket size."  )
        #else
            ALIB_ASSERT_WARNING( cntStackedObjects > 0,
              "De-allocated chunk's size is smaller than detected object size." )
            ALIB_DBG( (void) origSize );
        #endif

        MONOMEM_INFO( "Stacking {} objects from de-allocated memory of size {} (lost {} bytes). Deallocated type: {!Q<>}.",
                      cntStackedObjects,
                      origSize,
                      origSize - cntStackedObjects * detectedObjectSize,
                      dbgTypeInfo )
    }
#endif

}; // RTTRAllocator

}}}// namespace aworx[::lib::monomem::util]


/// Type alias in namespace #aworx.
using     RTTRAllocator  =   lib::monomem::util::RTTRAllocator;

} // namespace [aworx]



#endif // HPP_ALIB_MONOMEM_UTIL_RTTRALLOCATOR

