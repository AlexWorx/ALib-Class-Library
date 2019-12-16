/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_monomem of the \aliblong.
 *
 * \emoji :copyright: 2013-2019 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_FS_LISTS_FORWARDLIST
#define HPP_ALIB_FS_LISTS_FORWARDLIST 1

#if !defined(HPP_ALIB_FS_INTEGERS_INTEGERS)
#   include "alib/lib/fs_integers/integers.hpp"
#endif

#if !defined (_GLIBCXX_ITERATOR) && !defined (_ITERATOR_)
#   include <iterator>
#endif

namespace aworx { namespace lib {

// forwards
template<typename TElement>        struct ForwardList;
template<typename TElement>        struct ForwardListIterator;
template<typename TElement>        struct BidiNode;
template<typename TElement>        struct BidiList;
template<typename TElement>        struct BidiListIterator;

/** ************************************************************************************************
 * This is a generic base type that represents a node of a single linked lists.
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
 * struct MyElement : public ForwardNode<MyElement> 4
 * {
 *     MyData   nodeValue;
 * };
 * \endcode
 *
 * This documentation, as well as the documentation of types \alib{ForwardList}
 * and \alib{ForwardListIterator}, uses the terms <b>"node"</b> for this type
 * and <b>"element"</b> for the custom derived type \p{TElement} that contains the custom
 * (container) data.
 *
 * With this simple mechanism in place, node instances may be used as "anchors" to the first element
 * of a list. Pointers to nodes of this type may either point to a node or to an element. Instances
 * of nodes always point to an element instance.
 *
 * While pointers to nodes and elements are statically castable to each other (which is a "noop"
 * because it does not change the pointer's address on common platforms/compilers), most methods
 * accept and/or return pointers to derived type \p{TElement}, and if so, it has to be assured by
 * the caller that the input parameters in fact point to those derived instances. Likewise it is
 * assured by this type and types \alib{ForwardList} and
 * \alib{ForwardListIterator}, that a returned pointer to an element does so.
 *
 * This "contract" provides maximum type safety while dealing with start pointers to element
 * lists (which are usually non-castable instances of this base type as provided with
 * \alib{ForwardList}), while avoiding the need of code clutter by
 * inserting <c>static_cast</c> to \p{TElement} in the using code as much as possible.
 * Furthermore, this concept allows to have field #forward of type \p{TElement} which makes
 * debugging much easier, because the custom values of the derived types are always visible.
 *
 * @tparam TElement The "final" node type, containing custom data fields, that is to be derived from
 *                  this struct.
 **************************************************************************************************/
template<typename TElement>
struct ForwardNode
{
    #if !defined(ALIB_DOX)
        friend   struct ForwardList        <TElement>;
        friend   struct ForwardListIterator<TElement>;
        friend   struct BidiNode           <TElement>;
        friend   struct BidiList           <TElement>;
        friend   struct BidiListIterator   <TElement>;
    #endif

    /** Alias name for the an instantiation of this template.  */
    using TNode=  ForwardNode<TElement>;

    private:

    /** In case of derived type \p{TElement} this is the pointer to the next element.
     *  In case of derived class \alib{ForwardList}, this is the pointer to the
     *  first element of the list. */
        TElement*  forward;

    public:

    /** Default constructor. (Does not initialize the pointer!)  */
                                ForwardNode()                                   noexcept  = default;

    /** Deleted copy constructor. This is deleted because it is dangerous, respectively often
     *  not possible and also mostly not wanted to be able to create copies of derived type
     *  \p{TElement} */
                                ForwardNode( const TNode&  )                               = delete;

    /** Defaulted move constructor. */
                                ForwardNode(       TNode&& )                    noexcept  = default;

    /** Deleted copy assignment operator. This is deleted because it is dangerous, respectively
     *  often not possible and also mostly not wanted to created copies of derived type
     *  \p{TElement}
     *  @return Not defined.*/
                      ForwardNode&  operator=( const TNode& )                              = delete;

    /** Defaulted move assignment operator.
     *  @return A reference to this object.*/
                      ForwardNode&  operator=(       TNode&& )                  noexcept  = default;


    /** Constructor accepting a pointer to the next element.
     *  @param next  Pointer to the next element. Assigned to field #forward.  */
                                ForwardNode( TElement* next    )                noexcept
    : forward(next)
    {}


    /** Test if this element is the last element of the list.
     *  @return \c true if field #forward equals \c nullptr, otherwise \c false.  */
    bool      isLast()                                                                         const
    {
        return forward == nullptr;
    }

    /** Test if this node or element has a next element linked.
     *  @return \c false if field #forward equals \c nullptr, otherwise \c true.  */
    bool      hasNext()                                                                        const
    {
        return forward != nullptr;
    }

    /** Test if \p{elem} is the successor fo this node or element.
     *  @param elem  The element to test for being a successor of this.
     *  @return \c true if field #forward equals \p{elem} , otherwise \c false.  */
    bool      pointsTo( TElement* elem )                                                       const
    {
        return forward == elem;
    }

    /** Sets field #forward to \c nullptr.
     *  @return The element that this node or element pointed to before the invocation.  */
    TElement* makeLast()
    {
        TElement* result= forward;
        forward= nullptr;
        return result;
    }

    /** Sets field #forward to \p{elem}.
     *  @param elem  The element that this node or element is to point to.
     *  @return The element that this node or element pointed to before the invocation.  */
    TElement* makePointTo( TElement* elem )
    {
        TElement* result= forward;
        forward= elem;
        return result;
    }


    /** Returns the successor of this node or element.
     *  @return The element following this one, respectively \c nullptr if this is the last
     *          element of the list.                                                          */
    TElement* next()                                                                           const
    {
        return forward;
    }

    /** Unhooks and returns the element after this node or element.
     *  \note
     *    Field #forward of the returned element is \b not set to \c nullptr.
     *
     *  @return The unhooked element.                                        */
    TElement* removeNext()
    {
        auto* result= forward;
        forward     = forward->forward;
        return result;
    }

    /** Unhooks successors until \p{last}. If \p{last} equals field #forward, only one
     *  element is unhooked.
     *  \attention
     *     Field #forward of given \p{last} is \b not set to \c nullptr.
     *
     *  @param last The last element of the range
     *              <b>[</b>\ref forward "forward"<b>, </b> \p{last}<b>]</b> which is removed.
     *
     *  @return The start of the range (the current successor of this node or element).     */
    TElement* removeRangeBehind( TElement* last )
    {
        TElement* result= forward;
        forward         = last->forward;
        return result;
    }

    /** Hooks the given element behind this node or element.
     *  @param  elem The element to insert behind this node or element.
     *  @return The element that given \p{elem} pointed to before the insertion.             */
    TElement* addBehind( TElement* elem )
    {
        TElement* result = elem->forward;
        elem->forward=  forward;
        forward      =  elem;
        return result;
    }


    /** Hooks the given range of elements behind this node or element.
     *  @param first The first element of the range to insert.
     *  @param last  The last element of the range to insert.
     *  @return The element that given \p{last} pointed to before the insertion.             */
    TElement* addBehind( TElement* first, TElement* last )
    {
        TElement* result = last->forward;
        last->forward= forward;
        forward      = first;
        return result;
    }

    // #############################################################################################
    // ### Iterator interface
    // #############################################################################################
    /** The mutable <c>std::iterator</c> type. */
    using Iterator     =        ForwardListIterator<TElement>;

    /** The constant <c>std::iterator</c> type. */
    using ConstIterator=        ForwardListIterator<const TElement>;

    /** Interprets (casts) this node to an element and returns an iterator referring to it
     *  constituting a start of a range of elements.
     *  @return The first of element in the list.                          */
         Iterator      begin()          {  return      Iterator( static_cast<TElement*>(this)  );  }

    /** Returns an iterator referring to the end of a range of elements (aka a \c nullptr).
     *  @return The end of the list.                                       */
         Iterator      end()            {  return      Iterator( nullptr );  }

    /** Interprets (casts) this node to an element and returns an iterator referring to it
     *  constituting a start of a range of elements.
     *  @return The first of element in the list.                          */
    ConstIterator      begin() const    {  return ConstIterator( static_cast<TElement*>(this)  );  }

    /** Returns an iterator referring to the end of a range of elements (aka a \c nullptr).
     *  @return The end of the list.                                       */
    ConstIterator      end()   const    {  return ConstIterator( nullptr );  }

    /** Interprets (casts) this node to an element and returns an iterator referring to it
     *  constituting a start of a range of elements.
     *  @return The first of element in the list.                          */
    ConstIterator     cbegin() const    {  return ConstIterator( static_cast<TElement*>(this)  );  }

    /** Returns an iterator referring to the end of a range of elements (aka a \c nullptr).
     *  @return The end of the list.                                       */
    ConstIterator     cend()   const    {  return ConstIterator( nullptr );  }

    // #############################################################################################
    // ### static helpers
    // #############################################################################################

    /** Counts the number of elements found in the range starting with \p{start} and ending with
     *  the element before \p{end}.
     *  @param start Pointer to the first element to count.
     *  @param end   Pointer to the element after the last one to count.
     *               Defaults to \c nullptr, marking the end of a list.
     *  @return The number of elements in the range.                                           */
    static integer    count( TElement* start, TElement* end= nullptr )
    {
         integer result= 0;
         for( ConstIterator it= ConstIterator(start) ; it != ConstIterator(end) ; ++it )
             ++result;
         return result;
    }

    /** Moves the given node pointer reference to point to the next node.
     *  The result is the moved pointer, casted to an element.
     *
     *  \note
     *    It is matter of taste to use this static method for better code readability.
     *
     *  @param pointer A reference to the pointer to move forward.
     *  @return The moved pointer, casted to an element. */
    static TElement* moveForward( TNode*& pointer )
    {
        return static_cast<TElement*>(pointer= pointer->forward);
    }

    /** Moves the given element pointer reference to point to the next element.
     *  \note
     *    It is matter of taste to use this static method for better code readability.
     *
     *  @param pointer A reference to the pointer to move forward.    */
    static void   moveForward( TElement*& pointer )
    {
        pointer= pointer->forward;
    }
};  // struct ForwardNode

/** ************************************************************************************************
 * Implementation of \c std::iterator for types
 * \alib{ForwardNode} and \alib{ForwardList}.
 *
 * As the name of the class indicates, this iterator satisfies the C++ standard library concept
 * \https{ForwardIterator,en.cppreference.com/w/cpp/concept/ForwardIterator}.
 *
 * \note
 *   Iterators of this type may be received with the common <b>begin/end</b> methods found in
 *   types \b ForwardNode and \b ForwardList, but also be created autonomously by passing
 *   a pointer to \p{TElement} to the corresponding public constructor.
 *
 * @tparam TElement The "final" element type that by contract is derived from struct
 *                  \alib{ForwardNode}.
 **************************************************************************************************/
template<typename TElement>
struct ForwardListIterator
        : public std::iterator< std::forward_iterator_tag, // iterator_category
                                ForwardNode<TElement>,     // value_type
                                integer,                   // distance type
                                ForwardNode<TElement>*,    // pointer
                                ForwardNode<TElement>&     // reference
                              >
{
    private:
        /** The current element this iterator refers to. */
        TElement*          element;

    public:

    /** Constructor.
     *  @param start Pointer to the initial element.   */
    ForwardListIterator( TElement* start )                                                  noexcept
    : element( start )
    {}

    // ######################   To satisfy concept of  InputIterator   ######################

    /** Prefix increment operator.
     *  @return A reference to this object. */
    ForwardListIterator& operator++()
    {
        element= element->next();
        return *this;
    }

     /** Postfix increment operator.
     *  @return An iterator value that is not increased, yet. */
    ForwardListIterator operator++(int)
    {
        return ForwardListIterator( element->next() );
    }

    /** Comparison operator.
     *  @param other  The iterator to compare ourselves to.
     *  @return \c true if this and the given iterator are pointing to the same element,
     *          \c false otherwise. */
    bool operator==(ForwardListIterator other)                                                 const
    {
        return element == other.element;
    }

    /** Comparison operator.
     *  @param other  The iterator to compare ourselves to.
     *  @return \c true if this and given iterator are not equal, \c false otherwise. */
    bool operator!=(ForwardListIterator other)                                                 const
    {
        return !(*this == other);
    }

    // ######################   Member access   ######################

    /** Retrieves a reference to the referred element.
     * @return A reference to the referred element.               */
    TElement& operator*()                                                                      const
    {
        return *element;
    }

    /** Retrieves a pointer to the referred element.
     * @return A pointer to the referred element.                 */
    TElement* operator->()                                                                     const
    {
        return *element;
    }

}; // struct ForwardListIterator


/** ************************************************************************************************
 * Implements a single linked list of \p{TElement} instances.
 * This class inherits \b ForwardNode<TElement>, like \p{TElement} does (by contract).
 * This way methods that return a pointer to \b ForwardNode<TElement> may return either this
 * object itself (the "start hook" of the list) or a pointer to an element instance.
 *
 * Other methods (the larger part) return an object of \p{TElement}.
 * It has to be taken care that the result of methods that return type
 * \b ForwardNode are not casted to pointers of \p{TElement} by the calling code
 * (which is statically possible!). Only if a method returns \p{TElement}, it is assured that
 * a custom element type is returned.
 *
 * \note
 *   A good sample that demonstrates this difference in return types are methods #findLast and
 *   #findLastBefore.
 *   The first returns a pointer to type \p{TElement}. In contrast to this, the second
 *   returns a pointer to type \b ForwardNode because the result may be a pointer
 *   to this list instance itself - which has no custom data attached!
 *
 * @tparam TElement The "final" custom type that (by contract) is derived from
 *                  \alib{ForwardNode} the same type as this struct is.
 **************************************************************************************************/
template<typename TElement>
struct ForwardList : private ForwardNode<TElement>
{
    /** An alias for the node type of this list, which is also the type this struct is derived
     *  from.  */
    using TNode= ForwardNode<TElement>;

    using ForwardNode<TElement>::moveForward;

    /** Default constructor. Initializes this list to be empty.  */
                 ForwardList()                                                              noexcept
    {
        TNode::forward= nullptr;
    }

    /** Copy constructor.
     *  After construction, this instance points to the first start object as \p{copy} does.
     *  @param copy  The list to copy the link from.  */
                 ForwardList( const ForwardList&  copy )                                    noexcept
    {
        TNode::forward= copy.forward;
    }


    /** Move constructor.
     *  Copies the link from \p{move} and sets the link of \p{move} to \c nullptr.
     *  @param move  The list to move.  */
                 ForwardList(       ForwardList&& move )                                    noexcept
    {
        TNode::forward= move.forward;
          move.forward= nullptr;
    }

    /** Copy assignment operator.
     *  Copies the link to the first element from \p{copy}.
     *  @param copy  The list to copy the link from.
     *  @return A reference to this list object.                     */
    ForwardList& operator=  ( const ForwardList&  copy )                                    noexcept
    {
        TNode::forward= copy.forward;
    }

    /** Move assignment operator.
     *  Copies the link to the first element from \p{move} and sets the link in \p{move} to
     *  \c nullptr.
     *  @param move  The list to move the link from.
     *  @return A reference to this list object.                     */
    ForwardList& operator=  (       ForwardList&& move )                                    noexcept
    {
        TNode::forward= move.forward;
          move.forward= nullptr;
    }


    /** Constructor accepting a pointer to the first element.
     *  @param first  The element to use as the first element of this list.  */
                 ForwardList( TElement* first    )                                          noexcept
    {
        TNode::forward= first;
    }

    /** Casts this list to the node type. This method is needed because this list type inherits
     *  the node type privately.
     *
     *  \note
     *    This method simply returns <c>this</c>, which performs the static cast.
     *    If this type inherited \b ForwardNode publically, a simple ampersand operator
     *    (<c>&</c>) could be used on a value of this type instead of invoking this method.
     *
     *  @return Returns this list as a pointer to base type \b ForwardNode. */
    TNode*    castToNode()
    {
        return this;
    }

    /** Tests if this list is empty.
     *  @return \c false if the list is empty, \c true otherwise.      */
    bool      isEmpty()                                                                        const
    {
        return start() == nullptr;
    }

    /** Tests if this list is not empty.
     *  @return \c true if the list is empty, \c false otherwise.      */
    bool      isNotEmpty()                                                                     const
    {
        return start() != nullptr;
    }

    /** Resets this list to zero elements. */
    void      reset()
    {
        TNode::forward=  nullptr;
    }

    /** Returns the start element of this list.
     *  @return The first element of this list, respectively \c nullptr if this list is empty. */
    TElement* start()                                                                          const
    {
        return TNode::forward;
    }

    /** Sets given element \p{elem} as the start of this list.
     *  @param elem  The element to set as the new start of this list.
     */
    void      setStart( TElement* elem )
    {
        TNode::forward= elem;
    }

    /** Hooks the given element to the beginning of this list.
     * @param elem The element to insert to at the start.                  */
    void      pushFront( TElement* elem )
    {
        elem->forward=  start();
        setStart( elem );
    }

    /** Hooks the given range of elements to the front of this list.
     *  @param first The first element of the range to insert.
     *  @param last  The last element of the range to insert.      */
    void      pushFront( TElement* first, TElement* last )
    {
        last->forward= start();
        setStart(first);
    }

    /** Removes and returns the first element from this list.
     * @return  A pointer to the element, respectively \c nullptr if the list was empty.  */
    TElement* popFront()
    {
        if( isEmpty() )
            return nullptr;

        auto* result=  start();
        setStart(start()->forward);
        return result;
    }

    /** Searches and returns the last element.
     *  \note
     *    This method must only be invoked on non-empty lists (#isNotEmpty returns \c true).
     *    Otherwise this method has undefined behavior (dereference of a \c nullptr).
     *    To find the pointer to the last element use #findLastBefore providing \c nullptr.
     *  @return The last element of this list.      */
    TElement* findLast()                                                                       const
    {
        TElement* elem= start();
        while( elem->hasNext() )
            TNode::moveForward(elem);
        return elem;
    }

    /** Searches and returns the last element.
     *  @param hint An element of this list used to start the search.
     *  @return The last element of this list.      */
    TElement* findLast( TElement* hint )                                                       const
    {
        TElement* elem= hint;
        while( elem->hasNext() )
            TNode::moveForward(elem);
        return elem;
    }

    /** Searches the node or element that points to the given element.
     *  @param elem The element to search for.
     *  @return The node (this object) or element pointing to \p{elem}.             */
     TNode*   findLastBefore( TElement* elem )
    {
        TNode* it= this;
        while( !it->pointsTo(elem) )
            TNode::moveForward(it);
        return it;
    }

    /** Searches the predecessor of the given element using #findLastBefore and unhooks the element
     *  from the list.
     *  \note
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
        return TNode::count( start(), end );
    }


    /** The mutable iterator type. */
    using Iterator     =        ForwardListIterator<      TElement>;

    /** The constant iterator type. */
    using ConstIterator=        ForwardListIterator<const TElement>;

    /** Returns an iterator referring to a mutable element at the start of this list.
     *  @return An iterator to the first element in the list.                           */
         Iterator  begin()                                   {  return       Iterator( start() );  }

    /** Returns an iterator referring to a mutable, non-existing element.
     *  @return The end of the list.                                       */
         Iterator  end()                                     {  return       Iterator( nullptr );  }

    /** Returns an iterator referring to a constant element at the start of this list.
     *  @return An iterator to the first element in the list, limited to constant element access. */
    ConstIterator  begin()     const                         {  return  ConstIterator( start() );  }

    /** Returns an iterator referring to a constant, non-existing element.
     *  @return The end of the list.                                       */
    ConstIterator  end()       const                         {  return  ConstIterator( nullptr );  }

    /** Returns an iterator referring to a constant element at the start of this list.
     *  @return An iterator to the first element in the list, limited to constant element access. */
    ConstIterator  cbegin()    const                         {  return  ConstIterator( start() );  }

    /** Returns an iterator referring to a constant, non-existing element.
     *  @return The end of the list.                                       */
    ConstIterator  cend()      const                         {  return  ConstIterator( nullptr );  }

}; // struct ForwardList

}} // namespace [aworx::lib]


#endif // HPP_ALIB_FS_LISTS_FORWARDLIST
