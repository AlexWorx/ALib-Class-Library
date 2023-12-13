/** ************************************************************************************************
 * \file
* This header file is part of file set \alibfs_lists of the \aliblong.
  *
 * \emoji :copyright: 2013-2023 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_FS_LISTS_BIDILIST
#define HPP_ALIB_FS_LISTS_BIDILIST 1

#if !defined(HPP_ALIB_FS_LISTS_SIDILIST)
#   include "alib/lib/fs_lists/sidilist.hpp"
#endif

#if !defined(HPP_ALIB_TMP)
#   include "alib/lib/tmp.hpp"
#endif

#if !defined (HPP_ALIB_TOOLS)
#   include "alib/lib/tools.hpp"
#endif
namespace aworx { namespace lib { namespace detail {

/** ************************************************************************************************
 * This is a generic base type that represents a node of a doubly (bidirectional) linked lists.
 * The effective (instantiated) nodes of the list are to be derived from this type with specifying
 * their very own type name as template parameter \p{TElement}. For more information on this
 * design, see explanations of parent type \alib{detail::SidiNodeBase}.
 *
 * By being derived from aforementioned type, instances of \p{TElement} may also be added
 * to single-linked lists of type \alib{detail::SidiListHelper}). This is for example
 * used by types \alib{monomem,List} and \alib{monomem,HashTable} to collect erased list elements
 * for recycling.
 *
 * \see Types \alib{detail::SidiNodeBase},<br>
 *      \alib{detail::SidiListHelper} and<br>
 *      \alib{detail::BidiListHelper}.
 *
 * @tparam TElement The "final" node type, containing custom data fields, that is to be derived from
 *                  this struct.
 **************************************************************************************************/
template<typename TElement>
struct BidiNodeBase : public SidiNodeBase<TElement>
{
    /** Alias name for the an instantiation of the base template.  */
    using FWDNode=  SidiNodeBase<TElement>;

    /** A pointer to the previous element in the list.
     *  \attention
     *    If this is the first node in the list, this object will point to the list hook, which is
     *    an instance of this type instead of template type \p{TElement}. */
    TElement*  pprev;

    /** Default constructor. (Does not initialize the pointer!)  */
    BidiNodeBase()                                                            noexcept    = default;

    /** Deleted copy constructor. This is deleted because it is dangerous, respectively often
     *  not possible and also mostly not wanted to be able to create copies of derived type
     *  \p{TElement} */
    BidiNodeBase( const BidiNodeBase&  )                                                   = delete;

    /** Defaulted move constructor. */
    BidiNodeBase(       BidiNodeBase&& )                                      noexcept    = default;

    /** Deleted copy assignment operator. This is deleted because it is dangerous, respectively
     *  often not possible and also mostly not wanted to created copies of derived type
     *  \p{TElement}
     *  @return Not defined.*/
    BidiNodeBase& operator=( const BidiNodeBase& )                                         = delete;

    /** Defaulted move assignment operator.
     *  @return A reference to this object.*/

    BidiNodeBase& operator=(       BidiNodeBase&& )                             noexcept  = default;

    /** Constructor accepting a pointer to the next and previous elements.
     *  @param next Pointer to the next element. Assigned to inherited field
     *              \alib{detail::SidiNodeBase::pnext}.
     *  @param prev Pointer to the next element. Assigned to field #pprev.  */
    BidiNodeBase( TElement* next, TElement* prev    )           noexcept
    : SidiNodeBase<TElement>(next)
    , pprev                (prev)
    {}

    /** Returns the backward pointer of this node.
     *  @return Pointer to the previous element.    */
    TElement*   prev()                          const  { return pprev; }

    /** Sets the backward pointer of this node.
     *  @param newPrev Pointer to the previous element.    */
    void        prev(BidiNodeBase*    newPrev)         { pprev  = static_cast<TElement*>(newPrev); }

    /** Hooks the given element before this node.
     *  @param elem The element to add.                            */
    void      addBefore( TElement* elem )
    {
        elem->next( this );
        elem->prev( prev() );
        prev()->next(elem);
        prev(elem);
    }

    /** Hooks the given element behind this node.
     *  @param elem The element to add.                            */
    void      addBehind( TElement* elem )
    {
        elem->next( FWDNode::next() );
        elem->prev( this );
        FWDNode::next()->prev( elem );
        FWDNode::next( elem );
    }

    /** Unhooks this node from a list.                            */
    void remove()
    {
        FWDNode::next()->prev(prev());
        prev()->next( FWDNode::next());
    }

    /** Unhooks the range of nodes starting with this node and ending with \p{last} a list.
     *  @param last  The last element of the range to remove.      */
    void remove( TElement* last )
    {
        last->next()->prev( prev() );
        prev()->next( last->next() );
    }

};  // struct BidiNodeBase

/** ************************************************************************************************
 * This struct, together with sibling struct \alib{detail::BidiNodeBase} provide the tools to
 * implement a doubly linked list of \p{TElement} instances.<br>
 *
 * Template type \p{TElement} have to extend struct BidiNodeBase<TElement>.
 *
 * \see Types \alib{detail::SidiNodeBase},<br>
 *      \alib{detail::SidiListHelper} and<br>
 *      \alib{detail::BidiNodeBase}.
 *
 * @tparam TElement The "final" custom type that (by contract) is derived from
 *                  \alib{detail::BidiNodeBase}.
 **************************************************************************************************/
template<typename TElement>
struct BidiListHelper
{
    /** An alias for the base type of the node type of this list.  */
    using TFNode= SidiNodeBase<TElement>;

    /** An alias for the node type of this list.  */
    using TNode= BidiNodeBase<TElement>;

    /** The root node. Points twice to itself when list is empty.  */
    TNode hook;

    /** Default constructor. Initializes this list to be empty.  */
    BidiListHelper()                                                                        noexcept
    {
        hook.next(&hook);
        hook.prev(&hook);
    }

    /** Deleted copy constructor. */
    BidiListHelper( const BidiListHelper& )                                                = delete;

    /** Move constructor. Takes elements of \p move and sets \p move to empty.
     *  @param move The list to move.
     */
    BidiListHelper(       BidiListHelper&& move)                                            noexcept
    {
        if( !move.isEmpty() )
        {
            hook.next( move.hook.next() );
            hook.prev(move.hook.prev());
            move.reset();
        }
        else
            reset();
    }

    /** Deleted copy assignment operator.
     *  @return A reference to this list object.  */
    BidiListHelper& operator=  ( const BidiListHelper& )                                   = delete;

    /** Defaulted move assignment operator.
     *  @return A reference to this list object.  */
    BidiListHelper& operator=  (       BidiListHelper&&)                    noexcept      = default;

    /** Constructor accepting a pointer to the first element.
     *  @param first  The element to use as the first element of this list.  */
    BidiListHelper( TElement* first  )
    {
        hook.next( first );
        hook.prev( first );
        first->next( &hook );
        first->prev( &hook );
    }

    /** Constructor accepting a pointer to the first and last element.
     *  @param first  The element to use as the first element of this list.
     *  @param last   The element to use as the last element of this list.  */
    BidiListHelper( TElement* first, TElement* last  )
    {
        hook.next( first );
        hook.prev( last );
        first->prev( &hook );
         last->next( &hook );
    }

    /** Tests if this list is empty.
     *  @return \c false if the list is empty, \c true otherwise.      */
    bool      isEmpty()                                                                        const
    {
        return hook.pointsTo( &hook );
    }

    /** Resets this list to zero elements. */
    void      reset()
    {
        hook.next(&hook);
        hook.prev(&hook);
    }

    /** Returns a pointer to the #hook node casted to a pointer to a mutable element.
     *  This method must only be used in cases where such conversion is allowed, i.e. with
     *  iterator types that use this pointer exclusively for pointer value comparison, but do not
     *  allow (by contract) to dereference or otherwise use this pointer.
     *  @return The first element of this list. */
    TElement* end()                                                                            const
    {
        return static_cast<TElement*>( &const_cast<BidiListHelper*>(this)->hook );
    }


    /** Returns the first element of this list.
     *  @return The first element of this list. */
    TElement* first()                                                                          const
    {
        return hook.next();
    }

    /** Returns the last element of this list.
     *  @return The last element of this list. */
    TElement* last()                                                                           const
    {
        return hook.prev();
    }

    /** Tests if given \p{elem} is the first element of this list.
     *  @param elem The element to test for being the first.
     *  @return \c true if \p{elem} is the first element of this list, \c false otherwise. */
    bool      isFirst( TElement* elem )                                                        const
    {
        return first() == elem;
    }

    /** Tests if given \p{elem} is the last element of this list.
     *  @param elem The element to test for being the last.
     *  @return \c true if \p{elem} is the last element of this list, \c false otherwise. */
    bool      isLast( TElement* elem )                                                         const
    {
        return last() == elem;
    }

    /** Hooks the given element to the beginning of this list.
     * @param elem The element to insert to at the start.                  */
    void      pushFront( TElement* elem )
    {
        hook.addBehind( elem );
    }

    /** Hooks the given range of elements to the front of this list.
     *  @param first The first element of the range to insert.
     *  @param last  The last element of the range to insert.      */
    void      pushFront( TElement* first, TElement* last )
    {
        hook.addBehind( first, last );
    }

    /** Hooks the given element to the end of this list.
     *  @param elem The element to insert to at the start.         */
    void      pushEnd( TElement* elem )
    {
        hook.addBefore( elem );
    }

    /** Hooks the given range of elements to the end of this list.
     *  @param first The first element of the range to insert.
     *  @param last  The last element of the range to insert.      */
    void      pushEnd( TElement* first, TElement* last )
    {
        hook.addBefore( first, last );
    }

    /** Removes and returns the first element from this list.
     *  Must not be invoked on empty lists.
     *  @return  A pointer to the first element (which was removed).  */
    TElement* popFront()
    {
        TElement* first= hook.next();
        first->remove();
        return first;
    }

    /** Removes and returns the last element from this list.
     *  Must not be invoked on empty lists.
     *  @return  A pointer to the last element (which was removed).  */
    TElement* popEnd()
    {
        TElement* last= hook.prev();
        last->remove();
        return last;
    }

    /** Counts the number of elements found in the range starting with this list's first element and
     *  ending with the element before \p{end}.
     *  @param end   The element after the last one to count.
     *               Defaults to a \c nullptr marking the end of the list.
     *  @return The number of elements in the range.                                           */
    integer  count( const TNode* end= nullptr )                                               const
    {
        if( end == nullptr )
            end= &hook;

        integer count= 0;
        const TNode* node= hook.next();
        while( node != end )
        {
            node= node->next();
            ++count;
        }
        return count;
    }
}; // struct BidiListHelper


}}} // namespace [aworx::lib::detail]


#endif // HPP_ALIB_FS_LISTS_BIDILIST
