/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_monomem of the \aliblong.
 *
 * \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_MONOMEM_DETAIL_RECYCLER
#define HPP_ALIB_MONOMEM_DETAIL_RECYCLER 1
#if    !defined(HPP_ALIB_MONOMEM_DETAIL_HASHTABLEBASE) \
    && !defined(HPP_ALIB_MONOMEM_LIST)
#   error "ALib sources with ending '.inl' must not be included from outside."
#endif

#if !defined(HPP_ALIB_LANG_TMP) && !defined(ALIB_DOX)
#   include "alib/lang/tmp.hpp"
#endif


#if !defined(HPP_ALIB_LANG_SIDILIST)
#   include "alib/lang/sidilist.hpp"
#endif

#if ALIB_DEBUG
#   if !defined(HPP_ALIB_LANG_DBGTYPEDEMANGLER)
#      include "alib/lang/dbgtypedemangler.hpp"
#   endif
#   if ALIB_STRINGS && !defined (HPP_ALIB_STRINGS_LOCALSTRING)
#      include "alib/strings/localstring.hpp"
#   endif
#endif

#if !defined (_GLIBCXX_VECTOR) && !defined(_VECTOR_)
#   include <vector>
#endif

#if !defined (_GLIBCXX_MEMORY) && !defined(_MEMORY_)
#   include <memory>
#endif


namespace alib {  namespace monomem { namespace detail {

#if ALIB_DEBUG_MONOMEM && !defined(ALIB_DOX)
    void dbgMonoMemRecyclingOutput(size_t a, size_t b, size_t c, const std::type_info& typeInfo, size_t d  );
#endif

/** ************************************************************************************************
 * This struct is used for recycling of "node elements" which are derived of type
 * \alib{lang::SidiNodeBase}.
 * This class provides the common base for \alib{monomem,detail::RecyclerPrivate} and
 * \alib{monomem,detail::RecyclerShared}.
 *
 * @tparam TSidiNode The type to recycle. Has to be derived of \alib{lang::SidiNodeBase}.
 * @tparam TList     The type of the list of recyclables: Either the same as \p{TSidiNode}
 *                   or a reference to it.
 **************************************************************************************************/
template<typename TSidiNode, typename TList >
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
    void            recycle( TSidiNode* elem )
    {
        recyclables.pushFront( elem );
    }

    /** Stores a list of elements for recycling.
     *  @param first The first element of the list store.
     *  @param last  The last element of the list store.           */
    void            recycle( TSidiNode* first, TSidiNode* last )
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
        if constexpr( alignof(TSidiNode) > alignof(TChunk[1])  )
            std::align( alignof(TSidiNode) ,    // alignment: the desired alignment
                        sizeof (TSidiNode) ,    // size     : the size of the storage to be aligned
                        mem                ,    // ptr      : pointer to contiguous storage of at least 'space' bytes
                        size                 ); // space    : the size of the buffer in which to operate

        // create recyclables
        ALIB_DBG( size_t cntRecycledObjects= 0; )
        while(size > sizeof(TSidiNode))
        {
            recyclables.pushFront( reinterpret_cast<TSidiNode*>( mem ) );
            ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
            mem  =  reinterpret_cast<char*>(mem) + sizeof(TSidiNode);
            size -= sizeof (TSidiNode);
            ALIB_DBG( ++cntRecycledObjects; )
            ALIB_WARNINGS_RESTORE
        }

        #if ALIB_DEBUG
            if( cntRecycledObjects <= 0 )
            {
                #if ALIB_STRINGS
                    ALIB_WARNING( "MONOMEM/RECYCLER", NString4K() <<
                      "De-allocated chunk size is smaller than node size.\n"
                      "  Chunk object:       Type: <" << lang::DbgTypeDemangler( typeid(TChunk) ).Get() << ">\n"
                      "    Size, Count, Alignment: "  <<  sizeof(TChunk[1]) << " * " << count << " = "
                                                      << (sizeof(TChunk[1]) * count) << " bytes, alignment: "
                                                      << alignof(TChunk[1]) << "\n"
                      "           Recyclable Type: <" << lang::DbgTypeDemangler( typeid(TSidiNode) ).Get() << ">\n"
                      "           Size, Alignment: "  << sizeof(TSidiNode) << " bytes, alignment: " << alignof(TSidiNode) << "\n"
                      "Note: If this recycler is used with a <monomem::HashTable>, this message may be eliminated\n"
                      "      by reserving a reasonable initial bucket size."  )
                #else
                    ALIB_WARNING( cntRecycledObjects > 0, "MONOMEM/RECYCLER",
                      "De-allocated chunk size is smaller than node size." )
                #endif
            }
        #endif

        #if ALIB_DEBUG_MONOMEM
            dbgMonoMemRecyclingOutput(
                    cntRecycledObjects,
                    sizeof(TChunk[1]) * count,
                    sizeof(TChunk[1]) * count - sizeof(TSidiNode) * cntRecycledObjects,
                    typeid(TChunk), count       );
        #endif
    }

    /** Returns a stored element. If no elements are available, \c nullptr is returned.
     *  @return A recycled element or \c nullptr if no recyclables are available.      */
    TSidiNode*   get()
    {
        return recyclables.popFront();
    }

    /** Counts the number of recyclables.<br>
     *  Attention: This method runs in linear time.
     *  @return The number of available elements.                  */
    integer         count()                                                                    const
    {
        return recyclables.count();
    }

}; // struct RecyclerPSBase

/** ************************************************************************************************
 * Implements internal recycling interface. Used by container types of module \alib_monomem_nl that
 * use private recycling (when \b their template parameter \p{TRecycling} evaluates to
 * \alib{monomem,Recycling::Private}).
 *
 * @tparam TSidiNode The type to recycle. Has to be derived of \alib{lang::SidiNodeBase}.
 **************************************************************************************************/
template<typename TSidiNode>
struct RecyclerPrivate  : RecyclerPSBase<TSidiNode, lang::SidiListHelper<TSidiNode>>
{
    /** Empty constructor, selected when \p{TRecycling} equals
     *  to \alib{monomem,Recycling::Private}.                      */
    RecyclerPrivate()
    : RecyclerPSBase<TSidiNode, lang::SidiListHelper<TSidiNode>>()
    {}

    /** Copy constructor. Does not copy the recycle node, but clears this one.              */
    RecyclerPrivate( const RecyclerPrivate&  )
    : RecyclerPSBase<TSidiNode, lang::SidiListHelper<TSidiNode>>()
    {}

    /** Move constructor. Grabs the recyclables from the moved one and sets moved to nulled.
     *  @param move The private recycler to move.                                           */
    RecyclerPrivate( RecyclerPrivate&& move )
    : RecyclerPSBase<TSidiNode, lang::SidiListHelper<TSidiNode>>( move.recyclables )
    {
        move.recyclables.reset();
    }

    /** As the method name indicates, this implementation disposes the recyclables.
     *  This method is invoked by \b Reset() methods of containers which indicate that
     *  the monotonic memory is about to be reset likewise.                             */
    void            disposeRecyclablesIfPrivate()
    {
         RecyclerPSBase<TSidiNode, lang::SidiListHelper<TSidiNode>>::recyclables.reset();
    }
};

/** ************************************************************************************************
 * Implements internal recycling interface. Used by container types of module \alib_monomem_nl that
 * use shared recycling (when \b their template parameter \p{TRecycling} evaluates to
 * \alib{monomem,Recycling::Shared}).
 *
 * @tparam TSidiNode The type to recycle. Has to be derived of \alib{lang::SidiNodeBase}.
 **************************************************************************************************/
template<typename TSidiNode>
struct RecyclerShared  : RecyclerPSBase<TSidiNode, lang::SidiListHelper<TSidiNode>&>
{
    /** Constructor, selected when \p{TRecycling} equals
     *  to \alib{monomem,Recycling::Shared}.
     *  @param sharedRecyclables The external list of recyclables. */
    RecyclerShared( lang::SidiListHelper<TSidiNode>& sharedRecyclables)
    :  RecyclerPSBase<TSidiNode, lang::SidiListHelper<TSidiNode>&>( sharedRecyclables )
    {}

    /** Copy constructor. Copies the reference to the shared recycler.
     *  @param copy The private recycler to copy.                                           */
    RecyclerShared( const RecyclerShared& copy )
    : RecyclerPSBase<TSidiNode, lang::SidiListHelper<TSidiNode>&>( copy )
    {}

    /** Move constructor. Just copies the reference but leaves original intact. (We don't care)
     *  @param move The private recycler to move.                                           */
    RecyclerShared( RecyclerShared&& move )
    : RecyclerPSBase<TSidiNode, lang::SidiListHelper<TSidiNode>&>( move )
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
 *   <c>constexpr 0</c>.<br>
 * \see
 *   Non-empty 'implementations' of this template type are given with
 *   \alib{monomem,detail::RecyclerPrivate} and \alib{monomem,detail::RecyclerShared}.
 *
 * @tparam TSidiNode The type to recycle. Has to be derived of \alib{lang::SidiNodeBase}.
 **************************************************************************************************/
template<typename TSidiNode>
struct RecyclerVoid
{
#if !defined(ALIB_DOX)
    constexpr               RecyclerVoid()                                                        {}
              void          recycle( TSidiNode*  )                     const                      {}
              void          recycle( TSidiNode* , TSidiNode* )         const                      {}
    constexpr TSidiNode* get()                                         const     { return nullptr; }
    constexpr integer       count()                                    const           { return 0; }
              void          disposeRecyclablesIfPrivate()              const                      {}
    template<typename TChunk>
              void          recycleChunk( TChunk*, size_t )            const                      {}
#endif

}; // struct RecyclerVoid


}} // namespace alib[::monomem::detail]

} // namespace [alib]

#endif // HPP_ALIB_MONOMEM_DETAIL_RECYCLER
