/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_monomem of the \aliblong.
 *
 * \emoji :copyright: 2013-2019 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_MONOMEM_DETAIL_RECYCLER
#define HPP_ALIB_MONOMEM_DETAIL_RECYCLER 1

#if !defined (HPP_ALIB_MONOMEM_MONOMEM)
#   include "alib/monomem/monomem.hpp"
#endif

#if !defined(HPP_ALIB_TMP)
#   include "alib/lib/tmp.hpp"
#endif

#if !defined(HPP_ALIB_FS_LISTS_FORWARDLIST)
#   include "alib/lib/fs_lists/forwardlist.hpp"
#endif

#if !defined(HPP_ALIB_FS_DEBUG_ASSERT)
#   include "alib/lib/fs_debug/assert.hpp"
#endif

#if ALIB_DEBUG
#   if !defined (HPP_ALIB_FS_DEBUG_TYPEDEMANGLER)
#      include "alib/lib/fs_debug/typedemangler.hpp"
#   endif
#   if ALIB_STRINGS && !defined (HPP_ALIB_STRINGS_LOCALSTRING)
#      include "alib/strings/localstring.hpp"
#   endif
#endif

#if !defined (_GLIBCXX_MONOMEM) && !defined(_MONOMEM_)
#   include <memory>
#endif

namespace aworx { namespace lib { namespace monomem { namespace detail {

/** ************************************************************************************************
 * This struct is used for recycling of "node elements" which are derived of type
 * \alib{ForwardNode}.
 * This class provides the common base for \alib{monomem,detail::RecyclerPrivate} and
 * \alib{monomem,detail::RecyclerShared}.
 *
 * @tparam TForwardNode The type to recycle's. Has to be derived of \alib{ForwardNode}.
 * @tparam TList        The type of the list of recyclables: Either the same as \p{TForwardNode}
 *                      or a reference to it.
 **************************************************************************************************/
template<typename TForwardNode, typename TList >
struct RecyclerPSBase
{
    protected:
        /** The list of recyclables. */
        TList  recyclables;

    public:

    /** Empty constructor, used by derived type \alib{monomem,detail::RecyclerPrivate}. */
    RecyclerPSBase()                                                                      = default;


    /** Empty constructor, used by derived type \alib{monomem,detail::RecyclerShared}.
     *  @param sharedRecyclables The external list of recyclables. */
    RecyclerPSBase( TList sharedRecyclables)
    : recyclables( sharedRecyclables )
    {}

    /** Stores an element for recycling.
     *  @param elem The element to store.                          */
    void            recycle( TForwardNode* elem )
    {
        recyclables.pushFront( elem );
    }

    /** Stores a list of elements for recycling.
     *  @param first The first element of the list store.
     *  @param last  The last element of the list store.           */
    void            recycle( TForwardNode* first, TForwardNode* last )
    {
        recyclables.pushFront( first, last );
    }

    /** Recycles a chunk of memory that is \e not of the node type.
     *  Such recycling is useful for example, in combination with hash tables, which usually
     *  allocate a "bucket array" that grows over time. In this case, the former smaller bucket
     *  array may be passed to this method to recycle it as future node elements.
     *  @tparam TChunk The type of the chunk.
     *  @param  chunk  The recyclable chunk's address.
     *  @param  count  The number of objects (\c 1 for non-array types, <c>\>1</c> for arrays.  */
    template<typename TChunk>
    void            recycleChunk( TChunk* chunk, size_t count )
    {
        void* mem  = chunk;
        size_t size= sizeof(TChunk[1]) * count;

        // align beginning of buffer (if necessary)
        if ALIB_CONSTEXPR_IF( alignof(TForwardNode) > alignof(TChunk[1])  )
            std::align( alignof(TForwardNode) ,    // alignment: the desired alignment
                        sizeof (TForwardNode) ,    // size     : the size of the storage to be aligned
                        mem                   ,    // ptr      : pointer to contiguous storage of at least 'space' bytes
                        size                    ); // space    : the size of the buffer in which to operate

        // create recyclables
        ALIB_DBG( size_t cntRecycledObjects= 0; )
        while(size > sizeof(TForwardNode))
        {
            recyclables.pushFront( reinterpret_cast<TForwardNode*>( mem ) );
            mem  =  reinterpret_cast<char*>(mem) + sizeof(TForwardNode);
            size -= sizeof (TForwardNode);
            ALIB_DBG( ++cntRecycledObjects; )
        }

        #if ALIB_STRINGS
            ALIB_ASSERT_WARNING( cntRecycledObjects > 0, NString1K() <<
              "De-allocated chunk size is smaller than node size.\n"
              "  Chunk object:       Type: <" << DbgTypeDemangler( typeid(TChunk) ).Get() << ">\n"
              "    Size, Count, Alignment: "  <<  sizeof(TChunk[1]) << " * " << count << " = "
                                              << (sizeof(TChunk[1]) * count) << " bytes, alignment: "
                                              << alignof(TChunk[1]) << "\n"
              "           Recyclable Type: <" << DbgTypeDemangler( typeid(TForwardNode) ).Get() << ">\n"
              "           Size, Alignment: "  << sizeof(TForwardNode) << " bytes, alignment: " << alignof(TForwardNode) << "\n"
              "Note: If this recycler is used with a <monomem::HashTable>, this message may be eliminated\n"
              "      by reserving a reasonable initial bucket size."  )
        #else
            ALIB_ASSERT_WARNING( cntRecycledObjects > 0,
              "De-allocated chunk size is smaller than node size." )
        #endif

        MONOMEM_INFO( "RECYCLING",
                      "Recycling {} objects from de-allocated memory of size {} (lost {} bytes).\\n"
                      "Deallocated type: {!Q<>}{!Q[]}.",
                      cntRecycledObjects,
                      sizeof(TChunk[1]) * count,
                      sizeof(TChunk[1]) * count - sizeof(TForwardNode) * cntRecycledObjects,
                      typeid(TChunk), count )
    }

    /** Returns a stored element. If no elements are available, \c nullptr is returned.
     *  @return A recycled element or \c nullptr if no recyclables are available.      */
    TForwardNode*   get()
    {
        return recyclables.isNotEmpty() ?  recyclables.popFront()
                                  :  nullptr;
    }

    /** Counts the number of recyclables.<br>
     *  Attention: This method runs in linear time.
     *  @return The number of available elements.                  */
    integer         count()                                                                    const
    {
        return TForwardNode::count( recyclables.start() );
    }

}; // struct RecyclerPSBase

/** ************************************************************************************************
 * Implements internal recycling interface. Used by container types of module \alib_monomem_nl that
 * use private recycling (when \b their template parameter \p{TRecycling} evaluates to
 * \alib{monomem,Recycling::Private}).
 *
 * @tparam TForwardNode The type to recycle's. Has to be derived of \alib{ForwardNode}.
 **************************************************************************************************/
template<typename TForwardNode>
struct RecyclerPrivate  : RecyclerPSBase<TForwardNode, ForwardList<TForwardNode>>
{
    /** Empty constructor, selected when \p{TRecycling} equals
     *  to \alib{monomem,Recycling::Private}.                      */
    RecyclerPrivate()
    : RecyclerPSBase<TForwardNode, ForwardList<TForwardNode>>()
    {}

    /** Copy constructor. Does not copy the recycle node, but clears this one.              */
    RecyclerPrivate( const RecyclerPrivate&  )
    : RecyclerPSBase<TForwardNode, ForwardList<TForwardNode>>()
    {}

    /** Move constructor. Grabs the recyclables from the moved one and sets moved to nulled.
     *  @param move The private recycler to move.                                           */
    RecyclerPrivate( RecyclerPrivate&& move )
    : RecyclerPSBase<TForwardNode, ForwardList<TForwardNode>>( move.recyclables )
    {
        move.recyclables.reset();
    }

    /** As the method name indicates, this implementation disposes the recyclables.
     *  This method is invoked by \b Reset() methods of containers which indicate that
     *  the monotonic memory is about to be reset likewise.                             */
    void            disposeRecyclablesIfPrivate()
    {
         RecyclerPSBase<TForwardNode, ForwardList<TForwardNode>>::recyclables.reset();
    }
};

/** ************************************************************************************************
 * Implements internal recycling interface. Used by container types of module \alib_monomem_nl that
 * use shared recycling (when \b their template parameter \p{TRecycling} evaluates to
 * \alib{monomem,Recycling::Shared}).
 *
 * @tparam TForwardNode The type to recycle's. Has to be derived of \alib{ForwardNode}.
 **************************************************************************************************/
template<typename TForwardNode>
struct RecyclerShared  : RecyclerPSBase<TForwardNode, ForwardList<TForwardNode>&>
{
    /** Constructor, selected when \p{TRecycling} equals
     *  to \alib{monomem,Recycling::Shared}.
     *  @param sharedRecyclables The external list of recyclables. */
    RecyclerShared( ForwardList<TForwardNode>& sharedRecyclables)
    :  RecyclerPSBase<TForwardNode, ForwardList<TForwardNode>&>( sharedRecyclables )
    {}

    /** Copy constructor. Copies the reference to the shared recycler.
     *  @param copy The private recycler to copy.                                           */
    RecyclerShared( const RecyclerShared& copy )
    : RecyclerPSBase<TForwardNode, ForwardList<TForwardNode>&>( copy )
    {}

    /** Move constructor. Just copies the reference but leaves original intact. (We don't care)
     *  @param move The private recycler to move.                                           */
    RecyclerShared( RecyclerShared&& move )
    : RecyclerPSBase<TForwardNode, ForwardList<TForwardNode>&>( move )
    {}

    /** As the method name indicates, this implementation is empty.
     *  This method is invoked by \b Reset() methods of containers which indicate that
     *  the monotonic memory is about to be reset likewise.                             */
    void            disposeRecyclablesIfPrivate()
    {}
};


/** ************************************************************************************************
 * Implements internal recycling interface. Used by container types of module \alib_monomem_nl that
 * omit recycling (when \b their template parameter \p{TRecycling} evaluates to
 * \alib{monomem,Recycling::None}).
 *
 * \note
 *   The interface of this type is not documented, as all methods are emtpy, or return
 *   <c>consexpr 0</c>.<br>
 * \see
 *   Non-empty 'implementations' of this template type are given with
 *   \alib{monomem,detail::RecyclerPrivate} and \alib{monomem,detail::RecyclerShared}.
 *
 * @tparam TForwardNode The type to recycle's. Has to be derived of \alib{ForwardNode}.
 **************************************************************************************************/
template<typename TForwardNode>
struct RecyclerVoid
{
#if !defined(ALIB_DOX)
    constexpr               RecyclerVoid()                                                        {}
              void          recycle( TForwardNode*  )                  const                      {}
              void          recycle( TForwardNode* , TForwardNode*  )  const                      {}
    constexpr TForwardNode* get()                                      const     { return nullptr; }
    constexpr integer       count()                                    const           { return 0; }
              void          disposeRecyclablesIfPrivate()              const                      {}
    template<typename TChunk>
              void          recycleChunk( TChunk*, size_t )            const                      {}
#endif

}; // struct RecyclerVoid


}}}// namespace aworx[::lib::monomem::detail]

/// Type alias in namespace #aworx.
using Recycling= lib::monomem::Recycling;

} // namespace [aworx]

#endif // HPP_ALIB_MONOMEM_DETAIL_RECYCLER
