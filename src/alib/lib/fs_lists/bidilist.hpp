/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_monomem of the \aliblong.
 *
 * \emoji :copyright: 2013-2019 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_FS_LISTS_BIDILIST
#define HPP_ALIB_FS_LISTS_BIDILIST 1

#if !defined(HPP_ALIB_FS_LISTS_FORWARDLIST)
#   include "alib/lib/fs_lists/forwardlist.hpp"
#endif

#if !defined(HPP_ALIB_TMP)
#   include "alib/lib/tmp.hpp"
#endif
#if !defined(HPP_ALIB_FS_DEBUG_ASSERT)
#   include "alib/lib/fs_debug/assert.hpp"
#endif
namespace aworx { namespace lib {

// forwards
template<typename TElement>        struct BidiList;
template<typename TElement>        struct BidiListIterator;

/** ************************************************************************************************
 * This is a generic base type that represents a node of a doubly (bidirectional) linked lists.
 * The effective (instantiated) nodes of the list are to be derived from this type with specifying
 * their very own type name as template parameter \p{TElement}. For more information on this
 * design, see explanations of parent type \alib{ForwardNode}.
 *
 * By being derived from aforementioned type, instances of \p{TElement} may also be added
 * to single-linked lists of type \alib{ForwardList}). This is for example
 * used by type \alib{monomem,List} to collect erased list elements for recycling.
 *
 * @tparam TElement The "final" node type, containing custom data fields, that is to be derived from
 *                  this struct.
 **************************************************************************************************/
template<typename TElement>
struct BidiNode : public ForwardNode<TElement>
{
    #if !defined(ALIB_DOX)
        friend   struct BidiList        <TElement>;
        friend   struct BidiListIterator<TElement>;
    #endif

    /** Alias name for the an instantiation of the base template.  */
    using TFNode=  ForwardNode<TElement>;

    /** Alias name for the an instantiation of this template.  */
    using TNode =  BidiNode<TElement>;

    private:

    /** In case of derived type \p{TElement} this is the pointer to the previous element.
     *  In case of derived class \alib{BidiList}, this is the pointer to the
     *  last element of the list. */
        TElement*  backward;

    public:

    /** Default constructor. (Does not initialize the pointer!)  */
                                BidiNode()                                    noexcept    = default;

    /** Deleted copy constructor. This is deleted because it is dangerous, respectively often
     *  not possible and also mostly not wanted to be able to create copies of derived type
     *  \p{TElement} */
                                BidiNode( const TNode&  )                                  = delete;

    /** Defaulted move constructor. */
                                BidiNode(       TNode&& )                     noexcept    = default;

    /** Deleted copy assignment operator. This is deleted because it is dangerous, respectively
     *  often not possible and also mostly not wanted to created copies of derived type
     *  \p{TElement}
     *  @return Not defined.*/
    BidiNode&            operator=      ( const TNode& )                                   = delete;

    /** Defaulted move assignment operator.
     *  @return A reference to this object.*/

    BidiNode&            operator=      (       TNode&& )                     noexcept    = default;


    /** Constructor accepting a pointer to the next element.
     *  @param next      Pointer to the next element. Assigned to inherited field
     *                   \alib{ForwardNode,forward}.
     *  @param previous  Pointer to the next element. Assigned to field #backward.  */
                                BidiNode( TElement* next, TElement* previous    )           noexcept
    : ForwardNode<TElement>(next)
    , backward(previous)
    {}

    /** Returns the successor of this node or element.
     *  @return The element following this one, respectively an non-dereferencable end-marker node,
     *          in case this is the last element of the list.                                     */
    TElement* next()                                                                           const
    {
        return TFNode::next();
    }

    /** Returns the predecessor of this node or element.
     *  @return The element before this one, respectively an non-dereferencable end-marker node,
     *          in case this is the first element of the list.                                    */
    TElement* prev()                                                                           const
    {
        return backward;
    }

    /** Returns this node statically casted to \p{TElement}.
     *  @return This casted to \p{TElement}                        */
    TElement* self()
    {
        return static_cast<TElement*>( this );
    }

    /** Hooks the given element before this node.
     *  @param elem The element to add.                            */
    void      addBefore( TElement* elem )
    {
        elem->forward     = self();
        elem->backward    = backward;
        backward->forward = elem;
        backward          = elem;
    }

    /** Hooks the given range of elements before this node.
     *  @param first The first element of the range to insert.
     *  @param last  The last element of the range to insert.      */
    void      addBefore( TElement* first, TElement* last )
    {
        last->forward     = self();
        first->backward   = backward;
        backward->forward = first;
        backward          = last;
    }

    /** Hooks the given element behind this node.
     *  @param elem The element to add.                            */
    void      addBehind( TElement* elem )
    {
                elem->forward     = next();
                elem->backward    = self();
        TFNode::forward->backward = elem;
        TFNode::forward           = elem;
    }

    /** Hooks the given range of elements behind this node.
     *  @param first The first element of the range to insert.
     *  @param last  The last element of the range to insert.      */
    void      addBehind( TElement* first, TElement* last )
    {
                last->forward     = next();
                first->backward   = self();
        TFNode::forward->backward = last;
        TFNode::forward           = first;
    }


    /** Unhooks this node from a list.                            */
    void remove()
    {
        TFNode::forward       ->backward=  TNode::backward;
        TNode::backward->forward        =  TFNode::forward;
    }

    /** Unhooks the range of nodes starting with this node and ending with \p{last} a list.
     *  @param last  The last element of the range to remove.      */
    void remove( TElement* last )
    {
        last    ->forward->backward=  self()->backward;
        backward->forward          =  last->forward;
    }

};  // struct BidiNode

/** ************************************************************************************************
 * Implementation of \c std::iterator for types
 * \alib{BidiNode} and \alib{BidiList}.
 *
 * As the name of the class indicates, this iterator satisfies the C++ standard library
 * concept \https{BidirectionalIterator,en.cppreference.com/w/cpp/named_req/BidirectionalIterator}.
 *
 * \note
 *   Iterators of this type may be received with the common <b>begin/end</b> methods found in
 *   type \alib{BidiList}, but also be created autonomously by passing
 *   a pointer to \p{TElement} to the corresponding public constructor.
 *
 * @tparam TConstOrMutableElement  The "final" element type that by contract is derived from struct
 *                                 \alib{BidiNode} in either constant or mutable
 *                                 version.
 **************************************************************************************************/
template<typename TConstOrMutableElement>
struct BidiListIterator
        : public std::iterator< std::bidirectional_iterator_tag,    // iterator_category
                                BidiNode<TConstOrMutableElement>,   // value_type
                                integer,                            // distance type
                                BidiNode<TConstOrMutableElement>*,  // pointer
                                BidiNode<TConstOrMutableElement>&   // reference
                              >
{
    /** The element type: either const or mutable \b Element. */
    using TMutableNode= BidiNode<typename std::remove_const<TConstOrMutableElement>::type>;

    private:
        /** The start of the list. */
        TConstOrMutableElement*          element;


    public:

    /** Default constructor creating an invalid iterator. */
                      BidiListIterator()                                        noexcept  = default;

    /** Constructor.
     *  @param start Pointer to the initial element.   */
                      BidiListIterator( TConstOrMutableElement* start )                     noexcept
    : element( start )
    {
        ALIB_ASSERT_ERROR( start != nullptr, "Nullptr given as bidi iterator element." )
    }

    #if defined(ALIB_DOX)
        /** Copy constructor accepting a mutable iterator.
         *  Available only for the constant version of this iterator.
         *  @tparam TMutable The type of this constructor's argument.
         *  @param mutableIt Mutable iterator to copy from.             */
        inline
        template<typename TMutable>
        BidiListIterator( const TMutable& mutableIt );
    #else
        ATMP_SELECT_IF_1TP( typename TMutable,
                            ATMP_EQ( TMutable, BidiListIterator<ATMP_RC(TConstOrMutableElement)> ) )
        BidiListIterator( const TMutable& mutableIt )
        : element( mutableIt.element )
        {}
    #endif

    /** Assign operator from element type.
      * @param elem The element to refer to.
      * @return A reference to \c this.        */
    BidiListIterator& operator=( TConstOrMutableElement* elem  )                            noexcept
    {
        ALIB_ASSERT_ERROR( elem != nullptr, "Nullptr given as bidi iterator element." )
        element= elem;
        return *this;
    }



    // ######################   To satisfy concept of  InputIterator   ######################

    /** Prefix increment operator.
     *  @return A reference to this object. */
    BidiListIterator& operator++()                                                          noexcept
    {
        element= element->next();
        return *this;
    }

     /** Postfix increment operator.
     *  @return An iterator value that is not increased, yet. */
    BidiListIterator operator++(int)                                                        noexcept
    {
        return BidiListIterator( element->next() );
    }

    /** Comparison operator.
     *  @param other  The iterator to compare ourselves to.
     *  @return \c true if this and the given iterator are pointing to the same element,
     *          \c false otherwise. */
    bool operator==(BidiListIterator other)                                           const noexcept
    {
        return element == other.element;
    }

    /** Comparison operator.
     *  @param other  The iterator to compare ourselves to.
     *  @return \c true if this and given iterator are not equal, \c false otherwise. */
    bool operator!=(BidiListIterator other)                                           const noexcept
    {
        return !(*this == other);
    }

    //##################   To satisfy concept of  BidirectionalIterator   ##################

    /** Prefix decrement operator.
     *  @return A reference to this object. */
    BidiListIterator& operator--()
    {
        element= element->backward;
        return *this;
    }

    /** Postfix decrement operator.
     *  @return The iterator value prior the decrement operation. */
    BidiListIterator operator--(int)
    {
        auto result= BidiIterator(element);
        element= element->backward;
        return result;
    }

    // ######################   Member access   ######################

    /** Retrieves a reference to the referred element.
     * @return A reference to the referred element.               */
    TConstOrMutableElement& operator*()                                                        const
    {
        return *element;
    }

    /** Retrieves a pointer to the referred element.
     * @return A pointer to the referred element.                 */
    TConstOrMutableElement* operator->()                                                       const
    {
        return element;
    }

}; // struct BidiListIterator


/** ************************************************************************************************
 * Implements a doubly linked list of \p{TElement} instances.
 * This class inherits \b BidiNode<TElement>, like \p{TElement} does (by contract).
 * This way methods that return a pointer to \b BidiNode<TElement> may return either this
 * object itself (the "start hook" of the list) or a pointer to an element instance.
 *
 * Other methods (the larger part) accept or return object(s) of  type \p{TElement}.
 * It has to be taken care that the result of methods that return type
 * \b BidiNode are not casted to pointers of \p{TElement} by the calling code
 * (which is statically possible!). Only if a method returns \p{TElement}, it is assured that
 * a custom element type is returned.
 *
 * @tparam TElement The "final" custom type that (by contract) is derived from
 *                  \alib{BidiNode} the same type as this struct is.
 **************************************************************************************************/
template<typename TElement>
struct BidiList : private BidiNode<TElement>
{
    /** An alias for the base type of the node type of this list.  */
    using TFNode= ForwardNode<TElement>;

    /** An alias for the node type of this list, which is also the type this struct is derived
     *  from.  */
    using TNode= BidiNode<TElement>;

    /** Default constructor. Initializes this list to be empty.  */
    BidiList()                                                                              noexcept
    {
        TFNode::forward=
         TNode::backward=  stopper();
    }

    /** Deleted copy constructor. */
    BidiList( const BidiList& )                                                            = delete;


    /** Defaulted move constructor.
     *  @param move The list to move.   */
    BidiList(       BidiList&& move)                                                        noexcept
    {
        if( move.isNotEmpty() )
        {
           TFNode::forward = move.forward;
            TNode::backward= move.backward;
    move.forward-> backward=
    move.backward->forward=  stopper();

            move.reset();
        }
        else
            reset();
    }

    /** Deleted copy assignment operator.
     *  @return A reference to this list object.  */
    BidiList& operator=  ( const BidiList& )                                               = delete;

    /** Defaulted move assignment operator.
     *  @return A reference to this list object.  */
    BidiList& operator=  (       BidiList&&)                                noexcept      = default;


    /** Constructor accepting a pointer to the first element.
     *  @param first  The element to use as the first element of this list.  */
                                  BidiList( TElement* first  )
    {
       TFNode::forward =
        TNode::backward= first;
        first->backward=
        first->forward =  this;
    }

    /** Constructor accepting a pointer to the first and last element.
     *  @param first  The element to use as the first element of this list.
     *  @param last   The element to use as the last element of this list.  */
                                  BidiList( TElement* first, TElement* last  )
    {
       TFNode::forward = first;
        TNode::backward= last;
        first->backward=
         last->forward=  stopper();
    }


    /** Casts this list to the node type. This method is needed because this list type inherits
     *  the node type privately.
     *
     *  \note
     *    This method simply returns <c>this</c>, which performs the static cast.
     *    If this type inherited \b BidiNode publically, a simple ampersand operator
     *    (<c>&</c>) could be used on a value of this type instead of invoking this method.
     *
     *  @return Returns this list as a pointer to base type \b BidiNode. */
    TNode*    castToNode()
    {
        return this;
    }

    /** Tests if this list is empty.
     *  @return \c false if the list is empty, \c true otherwise.      */
    bool      isEmpty()                                                                        const
    {
        return first() == stopper();
    }

    /** Tests if this list is not empty.
     *  @return \c true if the list is empty, \c false otherwise.      */
    bool      isNotEmpty()                                                                     const
    {
        return first() != stopper();
    }

    /** Resets this list to zero elements. */
    void      reset()
    {
        TFNode::forward=
         TNode::backward=  stopper();
    }

    /** Returns the first element of this list.
     *  @return The first element of this list. */
    TElement* first()                                                                          const
    {
        return TFNode::forward;
    }

    /** Returns the last element of this list.
     *  @return The last element of this list. */
    TElement* last()                                                                           const
    {
        return TNode::backward;
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

    /** Returns an "illegal" stopper element. This is illegal in respect to that it must not be
     *  dereferenced, as in fact the returned object is a node, but not an element including the
     *  custom value.
     *  @return A "stopper" element to determine the end of the list. */
    TElement* stopper()                                                                        const
    {
        return const_cast<TElement*>( reinterpret_cast<const TElement*>( this  ) );
    }


    /** Hooks the given element to the beginning of this list.
     * @param elem The element to insert to at the start.                  */
    void      pushFront( TElement* elem )
    {
        stopper()->addBehind( elem );
    }


    /** Hooks the given range of elements to the front of this list.
     *  @param first The first element of the range to insert.
     *  @param last  The last element of the range to insert.      */
    void      pushFront( TElement* first, TElement* last )
    {
        stopper()->addBehind( first, last );
    }

    /** Hooks the given element to the beginning of this list.
     *  @param elem The element to insert to at the start.                 */
    void      pushEnd( TElement* elem )
    {
        stopper()->addBefore( elem );
    }

    /** Hooks the given range of elements to the front of this list.
     *  @param first The first element of the range to insert.
     *  @param last  The last element of the range to insert.      */
    void      pushEnd( TElement* first, TElement* last )
    {
        stopper()->addBefore( first, last );
    }


    /** Removes and returns the first element from this list.
     *  Must not be invoked on empty lists.
     *  @return  A pointer to the first element (which was removed).  */
    TElement* popFront()
    {
        TElement* first= TFNode::forward;
        first->remove();
        return first;
    }

    /** Removes and returns the last element from this list.
     *  Must not be invoked on empty lists.
     *  @return  A pointer to the last element (which was removed).  */
    TElement* popEnd()
    {
        TElement* last= TNode::backward;
        last->remove();
        return last;
    }


    /** Counts the number of elements found in the range starting with this list's first element and
     *  ending with the element before \p{end}.
     *  @param end   The element after the last one to count.
     *               Defaults to a \c nullptr marking the end of the list.
     *  @return The number of elements in the range.                                           */
     integer  count( TElement* end= nullptr )                                                  const
    {
        if( end == nullptr )
            end= stopper();

        TNode* node= stopper();
        integer count= 0;
        while( node->forward != end )
        {
            node= node->forward;
            ++count;
        }
        return count;
    }


    /** The mutable iterator type. */
    using             Iterator  =        BidiListIterator<      TElement>;

    /** The constant iterator type. */
    using        ConstIterator  =        BidiListIterator<const TElement>;

    /** The mutable reverse iterator type. */
    using      ReverseIterator  = std::reverse_iterator<     Iterator>;

    /** The constant reverse iterator type. */
    using ConstReverseIterator  = std::reverse_iterator<ConstIterator>;



    /** Returns an iterator referring to a mutable element at the start of this list.
     *  @return An iterator to the first element in the list.                           */
                Iterator   begin()                   {  return              Iterator( first()   ); }

    /** Returns an iterator referring to a mutable, non-existing element.
     *  @return The end of the list.                                       */
                Iterator   end()                     {  return              Iterator( stopper() ); }

    /** Returns an iterator referring to a constant element at the start of this list.
     *  @return An iterator to the first element in the list, limited to constant element access. */
           ConstIterator   begin()    const          {  return         ConstIterator( first()   ); }

    /** Returns an iterator referring to a constant, non-existing element.
     *  @return The end of the list.                                       */
           ConstIterator   end()      const          {  return         ConstIterator( stopper() ); }

    /** Returns an iterator referring to a constant element at the start of this list.
     *  @return An iterator to the first element in the list, limited to constant element access. */
           ConstIterator  cbegin()    const          {  return         ConstIterator( first()   ); }

    /** Returns an iterator referring to a constant, non-existing element.
     *  @return The end of the list.                                       */
           ConstIterator  cend()      const          {  return         ConstIterator( stopper() ); }

    /** Returns an iterator referring to a mutable element at the start of this list.
     *  @return An iterator to the first element in the list.                           */
         ReverseIterator  rbegin()                   {  return       ReverseIterator( first()   ); }

    /** Returns an iterator referring to a mutable, non-existing element.
     *  @return The end of the list.                                       */
         ReverseIterator  rend()                     {  return       ReverseIterator( stopper() ); }

    /** Returns an iterator referring to a constant element at the start of this list.
     *  @return An iterator to the first element in the list, limited to constant element access. */
    ConstReverseIterator  rbegin()    const          {  return  ConstReverseIterator( first()   ); }

    /** Returns an iterator referring to a constant, non-existing element.
     *  @return The end of the list.                                       */
    ConstReverseIterator  rend()      const          {  return  ConstReverseIterator( stopper() ); }

    /** Returns an iterator referring to a constant element at the start of this list.
     *  @return An iterator to the first element in the list, limited to constant element access. */
    ConstReverseIterator crbegin()    const          {  return  ConstReverseIterator( first()   ); }

    /** Returns an iterator referring to a constant, non-existing element.
     *  @return The end of the list.                                       */
    ConstReverseIterator crend()      const          {  return  ConstReverseIterator( stopper() ); }

}; // struct BidiList


}} // namespace [aworx::lib]


#endif // HPP_ALIB_FS_LISTS_BIDILIST
