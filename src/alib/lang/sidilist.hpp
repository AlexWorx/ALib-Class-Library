/** ************************************************************************************************
 * \file
 * This header file is part of the \aliblong. It does not belong to an \alibmod and is
 * included in any \alibdist.
 *
 * \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_LANG_SIDILIST
#define HPP_ALIB_LANG_SIDILIST 1

#if !defined(HPP_ALIB_LANG_INTEGERS)
#   include "alib/lang/integers.hpp"
#endif

#if !defined (_GLIBCXX_ITERATOR) && !defined (_ITERATOR_)
#   include <iterator>
#endif

namespace alib::lang {

/** ************************************************************************************************
 * This is a generic base type that may be used to represent a node of a single linked lists.
 * The effective (instantiated) nodes of the list are to be derived from this type with specifying
 * their very own type name as template parameter \p{TElement}. The following quick sample
 * demonstrates this:
 *
 * \code{.cpp}
 * // The contents to store with each element of a list
 * class MyData
 * {
 *     //....
 * };
 *
 * // The custom list node derived from this type, giving its own type as template parameter
 * struct MyElement : public lang::SidiNodeBase<MyElement>
 * {
 *     MyData   nodeValue;
 * };
 * \endcode
 *
 * With this simple mechanism in place, pointers to nodes of this type may either point to a node
 * or to an "full" element of derived class.
 * This concept allows to have field #pnext of this class of derived type \p{TElement} which makes
 * debugging end-user code much easier, because the custom values of the derived types are
 * visible within debugging tools.
 *
 * This type provides some helper methods, which together with those of sibling
 * struct \alib{lang::SidiListHelper}, provide all basic mechanisms to implement a single linked
 * list.
 *
 * Because pointers to nodes and elements are statically castable to each other (which is a noop),
 * most helper methods accept and/or return pointers to derived type \p{TElement}. It has
 * to be assured by the caller that these pointers are only dereferenced when it is assured that
 * they do not refer to this base type! Therefore, the helper classes are designed for
 * "internal use" and reside in the \e detail namespace.
 * As an example, class \alib{monomem,List} of module \alib_monomem is built using these helpers
 * internally, but publishes a fully type safe interface, including iterator classes.
 *
 * \see Types \alib{lang::SidiListHelper},<br>
 *      \alib{lang::BidiNodeBase} and<br>
 *      \alib{lang::BidiListHelper}.
 *
 * @tparam TElement The "final" node type, containing custom data fields, that is to be derived from
 *                  this struct.
 **************************************************************************************************/
template<typename TElement>
struct SidiNodeBase
{
    /** A pointer to the next element in the list.
     *  \attention
     *    In case of using type \alib{lang::BidiListHelper}, this may point to an instance of the
     *    base type \alib{lang::BidiNodeBase}. */
    TElement*   pnext;


    /** Default constructor. (Does not initialize the pointer.)  */
    SidiNodeBase()                                                               noexcept  = default;

    /** Deleted copy constructor. This is deleted because it is dangerous, respectively often
     *  not possible and also mostly not wanted to be able to create copies of derived type
     *  \p{TElement} */
    SidiNodeBase( const SidiNodeBase&  )                                                     = delete;

    /** Defaulted move constructor. */
    SidiNodeBase(       SidiNodeBase&& )                                          noexcept  = default;

    /** Deleted copy assignment operator. This is deleted because it is dangerous, respectively
     *  often not possible and also mostly not wanted to create copies of derived type
     *  \p{TElement}
     *  @return Not defined.*/
    SidiNodeBase&  operator=( const SidiNodeBase& )                                          = delete;

    /** Defaulted move assignment operator.
     *  @return A reference to this object.*/
    SidiNodeBase&  operator=(       SidiNodeBase&& )                              noexcept  = default;

    /** Constructor accepting a pointer to the next element.
     *  @param next  Pointer to the next element. Assigned to field #pnext.  */
    SidiNodeBase( TElement* next )                                                           noexcept
    : pnext(next)
    {}

    /** Sets the successor of this node or element.
     *  @param p The node that this node should point to, respectively \c nullptr if this node
     *           should represent the last element of the list.                                 */
    void        next(SidiNodeBase* p)                    { pnext= static_cast<TElement*>(p); }

    /** Returns the successor of this node or element.
     *  @return The element following this one, respectively \c nullptr if this is the last
     *          element of the list.                                                          */
    TElement*   next()                          const   { return pnext; }

    /** Test if this node or element has a next element linked.
     *  @return \c false if field #pnext equals \c nullptr, otherwise \c true.  */
    bool      hasNext()                         const   { return pnext != nullptr; }

    /** Test if \p{elem} is the successor fo this node or element.
     *  @param elem  The element to test for being a successor of this.
     *  @return \c true if field #pnext equals \p{elem} , otherwise \c false.  */
    bool      pointsTo( const SidiNodeBase* elem ) const { return pnext == elem; }

    /** Unhooks and returns the element after this node or element.
     *  \note
     *    Field #pnext of the returned element is \b not set to \c nullptr.
     *
     *  @return The unhooked element.                                        */
    TElement* removeNext()
    {
        auto* result= next();
        next( next()->next() );
        return result;
    }

    /** Unhooks successors until \p last. If \p last equals field #pnext, only this next
     *  element is unhooked.
     *  \attention
     *     Field #pnext of given \p{last} is \b not set to \c nullptr.
     *
     *  @param last The last element of the range
     *              <b>[</b>\ref pnext "pnext"<b>, </b> \p{last}<b>]</b> which is removed.
     *
     *  @return The start of the range (the current successor of this node or element).     */
    TElement* removeRangeBehind( TElement* last )
    {
        TElement* result= next();
        next( last->next() );
        return result;
    }

    /** Hooks the given element behind this node or element.
     *  @param  elem The element to insert behind this node or element.
     *  @return The element that given \p{elem} pointed to before the insertion.             */
    TElement* addBehind( TElement* elem )
    {
        TElement* result = elem->next();
        elem->next(next());
        next( elem );
        return result;
    }

    /** Counts the number of elements found in the range starting with the next node (in
     *  consideration that this node is the 'hook' node) and ending with the element before \p{end}.
     *
     *  @param end   Pointer to the element after the last one to count.
     *               Defaults to \c nullptr, marking the end of a list.
     *  @return The number of elements in the range.                                           */
    integer    count( SidiNodeBase* end= nullptr )                                             const
    {
        integer result= 0;
        SidiNodeBase* start= next();
        while( start != end )
        {
            start= start->next();
            ++result;
        }
        return result;
    }
};  // struct SidiNodeBase


/** ************************************************************************************************
 * This class, together with sibling class \alib{lang::SidiNodeBase} provide the tools to
 * implement a single linked list of \p{TElement} instances.
 *
 * \see Types \alib{lang::SidiNodeBase},<br>
 *      \alib{lang::BidiNodeBase} and<br>
 *      \alib{lang::BidiListHelper}.
 *
 * @tparam TElement The "final" custom type that (by contract) is derived from
 *                  \alib{lang::SidiNodeBase}.
 **************************************************************************************************/
template<typename TElement>
struct SidiListHelper
{
    /** An alias for the node type.  */
    using TNode= SidiNodeBase<TElement>;

    /** Pointer to the first node.  */
    TNode hook;

    /** Default constructor. Initializes this list to be empty.  */
    SidiListHelper()                                                                        noexcept
    : hook(nullptr)
    {}

    /** Deleted copy constructor.
     * \note Copy construction is a duty of derived useable types. */
    SidiListHelper( const SidiListHelper&  copy )                                          = delete;

    /** Move constructor.
     *  Copies the link from \p{move} and sets the link of \p{move} to \c nullptr.   */
    SidiListHelper(       SidiListHelper&&  )                                    noexcept = default;

    /** Deleted copy assignment operator.
     *  @return Not applicable                                */
    SidiListHelper& operator=  ( const SidiListHelper& )                          noexcept = delete;

    /** Move assignment operator.
     *  Copies the link to the first element from \p{move} and sets the link in \p{move} to
     *  \c nullptr.
     *  @return A reference to this list object.                     */
    SidiListHelper& operator=  ( SidiListHelper&& )                              noexcept = default;

    /** Tests if this list is empty.
     *  @return \c false if the list is empty, \c true otherwise.      */
    bool      isEmpty()                                                                        const
    {
        return first() == nullptr;
    }

    /** Resets this list to zero elements. */
    void      reset()
    {
        hook.next(nullptr);
    }

    /** Returns the start element of this list.
     *  @return The first element of this list, respectively \c nullptr if this list is empty. */
    TElement* first()                                                                          const
    {
        return hook.next();
    }

    /** Hooks the given element to the beginning of this list.
     * @param elem The element to insert to at the start.                  */
    void      pushFront( TElement* elem )
    {
        elem->next( hook.next() );
        hook.next( elem );
    }

    /** Hooks the given range of elements to the front of this list.
     *  @param first The first element of the range to insert.
     *  @param last  The last element of the range to insert.      */
    void      pushFront( TElement* first, TElement* last )
    {
        last->next(this->first());
        hook.next(first);
    }

    /** Removes and returns the first element from this list.
     * @return  A pointer to the element, respectively \c nullptr if the list was empty.  */
    TElement* popFront()
    {
        TElement* result=  first();
        if( result )
            hook.next(result->next());
        return result;
    }

    /** Searches and returns the last element.
     *  \note
     *    This method must only be invoked on non-empty lists (#isEmpty returns \c false).
     *    Otherwise this method has undefined behavior (dereference of a \c nullptr).
     *    To find the pointer to the last element use #findLastBefore providing \c nullptr.
     *  @return The last element of this list.      */
    TElement* findLast()                                                                       const
    {
        TElement* elem= first();
        while( elem->hasNext() )
            elem= elem->next();
        return elem;
    }

    /** Searches and returns the last element.
     *  @param hint An element of this list used to start the search.
     *  @return The last element of this list.      */
    TElement* findLast( TElement* hint )                                                       const
    {
        TElement* elem= hint;
        while( elem->hasNext() )
            elem= elem->next();
        return elem;
    }

    /** Searches the node or element that points to the given element.
     * \attention The element has to exist. Otherwise a \c nullptr exception will occur!
     *  @param elem The element to search for.
     *  @return The node (this object) or element pointing to \p{elem}.             */
    TElement*   findLastBefore( TElement* elem )
    {
        TNode* it= &hook;
        while( !it->pointsTo(elem) )
            it= it->next();
        return static_cast<TElement*>( it );
    }

    /** Searches the predecessor of the given element using #findLastBefore and unhooks the element
     *  from the list.
     *  \attention
     *    It is not checked whether a predecessor was found, aka whether \p{elem} is an element
     *    of this list. If not, behavior is undefined (<c>nullptr access</c>).<br>
     *    Furthermore, the successor of given \p{elem} is not changed, although it is removed
     *    from the list.
     *
     *  @param elem The element to remove.
     *  @return The node (this object) or element that pointed to given \p{elem} before the
     *          invocation and now points to the successor of \p{elem}.                      */
    TNode*    findAndRemove( TElement* elem )
    {
        TNode* previous= findLastBefore(elem);
        previous->removeNext();
        return previous;
    }

    /** Counts the number of elements found in the range starting with this list's first element and
     *  ending with the element before \p{end}.
     *  @param end   The element after the last one to count.
     *               Defaults to a \c nullptr marking the end of the list.
     *  @return The number of elements in the range.                                           */
    integer   count( TElement* end= nullptr )                                                  const
    {
        return hook.count( end );
    }
}; // struct SidiListHelper

} // namespace [alib::lang]


#endif // HPP_ALIB_LANG_SIDILIST
