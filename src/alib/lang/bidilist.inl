//==================================================================================================
/// \file
/// This header-file is part of module \alib_lang of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib::lang {

//==================================================================================================
/// This is a generic base type that represents a node of a doubly (bidirectional) linked list.
/// The effective (instantiated) nodes of the list are to be derived from this type by specifying
/// their very own type name as template parameter \p{TElement}. For more information on this
/// design, see explanations of parent type \alib{lang::SidiNodeBase}.
///
/// By being derived from the aforementioned type, instances of \p{TElement} may also be added
/// to single-linked lists of type \alib{lang::SidiListHook}). This is, for example,
/// used by types \alib{containers;List} and \alib{containers;HashTable} to
/// collect erased list elements for recycling.
///
/// \see Types \alib{lang::SidiNodeBase},<br>
///      \alib{lang::SidiListHook}, and<br>
///      \alib{lang::BidiListHook}.
///
/// @tparam TElement The "final" node type, containing custom data fields, that is to be derived from
///                  this struct.
//==================================================================================================
template<typename TElement>
struct BidiNodeBase : public SidiNodeBase<TElement>
{
    /// Alias name for an instantiation of the base template.
    using FWDNode=  SidiNodeBase<TElement>;

    /// A pointer to the previous element in the list.
    /// \attention
    ///   If this is the first node in the list, this object will point to the list hook, which is
    ///   an instance of this type instead of template type \p{TElement}.
    TElement*  p;

    /// Default constructor. (Does not initialize the pointer!)
    BidiNodeBase()                                                                noexcept =default;

    /// Deleted copy constructor. This is deleted because it is dangerous, respectively often
    /// not possible and also mostly not wanted to be able to create copies of derived type
    /// \p{TElement}
    BidiNodeBase( const BidiNodeBase&  )                                                    =delete;

    /// Defaulted move constructor.
    BidiNodeBase(       BidiNodeBase&& )                                          noexcept =default;

    /// Deleted copy assignment operator. This is deleted because it is dangerous, respectively
    /// often not possible and also mostly not wanted to create copies of derived type \p{TElement}.
    /// @return Not defined.
    BidiNodeBase& operator=( const BidiNodeBase& )                                          =delete;

    /// Defaulted move assignment operator.
    /// @return A reference to this object.

    BidiNodeBase& operator=(       BidiNodeBase&& )                               noexcept =default;

    /// Constructor accepting a pointer to the next and previous elements.
    /// @param next Pointer to the next element. Assigned to the inherited field
    ///             \alib{lang::SidiNodeBase::n}.
    /// @param prev Pointer to the next element. Assigned to the field #p.
    BidiNodeBase( TElement* next, TElement* prev    )                                       noexcept
    : SidiNodeBase<TElement>(next)
    , p                     (prev)                                                                {}

    /// Returns the backward pointer of this node.
    /// @return Pointer to the previous element.
    [[nodiscard]]
    TElement*   prev()                                                  const noexcept { return p; }

    /// Sets the backward pointer of this node.
    /// @param previous Pointer to the previous element.
    void        prev(BidiNodeBase* previous)       noexcept { p= static_cast<TElement*>(previous); }

    /// Hooks the given element before this node.
    /// @param elem The element to add.
    void      addBefore( TElement* elem )                                                 noexcept {
        elem->next( this );
        elem->prev( prev() );
        prev()->next(elem);
        prev(elem);
    }

    /// Hooks the given element behind this node.
    /// @param elem The element to add.
    void      addBehind( TElement* elem )                                                 noexcept {
        elem->next( FWDNode::next() );
        elem->prev( this );
        FWDNode::next()->prev( elem );
        FWDNode::next( elem );
    }

    /// Unhooks this node from a list.
    void    remove()     noexcept { FWDNode::next()->prev(prev()); prev()->next( FWDNode::next()); }

    /// Unhooks the range of nodes starting with this node and ending with \p{last} a list.
    /// @param last  The last element of the range to remove.
    void remove( TElement* last )                                                         noexcept {
        last->next()->prev( prev() );
        prev()->next( last->next() );
    }

};  // struct BidiNodeBase

//==================================================================================================
/// This struct, together with sibling struct \alib{lang::BidiNodeBase} provide the tools to
/// implement a doubly linked list of \p{TElement} instances.<br>
///
/// Template type \p{TElement} has to extend struct BidiNodeBase<TElement>.
///
/// \see Types \alib{lang::SidiNodeBase},<br>
///      \alib{lang::SidiListHook}, and<br>
///      \alib{lang::BidiNodeBase}.
///
/// @tparam TElement The "final" custom type that (by contract) is derived from
///                  \alib{lang::BidiNodeBase}.
//==================================================================================================
template<typename TElement>
struct BidiListHook
{
    /// An alias for the base type of the node type of this list.
    using TFNode= SidiNodeBase<TElement>;

    /// An alias for the node type of this list.
    using TNode= BidiNodeBase<TElement>;

    /// The root node. Points twice to itself when the list is empty.
    TNode hook;

    /// Default constructor. Initializes this list to be empty.
    BidiListHook()                                  noexcept { hook.next(&hook); hook.prev(&hook); }

    /// Deleted copy constructor.
    BidiListHook( const BidiListHook& )                                                     =delete;

    /// Move constructor. Takes elements of \p{move} and sets \p{move} to empty.
    /// @param move The list to move.
    BidiListHook(       BidiListHook&& move)                                              noexcept {
        if( !move.isEmpty() ) {
            hook.next( move.hook.next() );
            hook.prev(move.hook.prev());
            move.reset();
        }
        else
            reset();
    }

    /// Deleted copy assignment operator.
    /// @return A reference to this list object.
    BidiListHook& operator=  ( const BidiListHook& )                                        =delete;

    /// Defaulted move assignment operator.
    /// @return A reference to this list object.
    BidiListHook& operator=  (       BidiListHook&&)                              noexcept =default;

    /// Constructor accepting a pointer to the first element.
    /// @param first  The element to use as the first element of this list.
    explicit BidiListHook( TElement* first  )                                             noexcept {
        hook.next( first );
        hook.prev( first );
        first->next( &hook );
        first->prev( &hook );
    }

    /// Constructor accepting a pointer to the first and last element.
    /// @param first  The element to use as the first element of this list.
    /// @param last   The element to use as the last element of this list.
    BidiListHook( TElement* first, TElement* last  )                                      noexcept {
        hook.next( first );
        hook.prev( last );
        first->prev( &hook );
         last->next( &hook );
    }

    /// Tests if this list is empty.
    /// @return \c false if the list is empty, \c true otherwise.
    [[nodiscard]]
    bool      isEmpty()                            const noexcept { return hook.pointsTo( &hook ); }

    /// Resets this list to zero elements.
    void      reset()                               noexcept { hook.next(&hook); hook.prev(&hook); }

    /// Returns a pointer to the #hook node cast to a pointer to a mutable element.
    /// This method must only be used in cases where such conversion is allowed i.e., with
    /// iterator types that use this pointer exclusively for pointer value comparison but do not
    /// allow (by contract) to dereference or otherwise use this pointer.
    /// @return The first element of this list.
    TElement* end()                                                                   const noexcept
    { return static_cast<TElement*>( &const_cast<BidiListHook*>(this)->hook ); }


    /// Returns the first element of this list.
    /// @return The first element of this list.
    [[nodiscard]]
    TElement* first()                                         const noexcept { return hook.next(); }

    /// Returns the last element of this list.
    /// @return The last element of this list.
    [[nodiscard]]
    TElement* last()                                          const noexcept { return hook.prev(); }

    /// Tests if given \p{elem} is the first element of this list.
    /// @param elem The element to test for being the first.
    /// @return \c true if \p{elem} is the first element of this list, \c false otherwise.
    [[nodiscard]]
    bool      isFirst( const TElement* elem )             const noexcept { return first() == elem; }

    /// Tests if given \p{elem} is the last element of this list.
    /// @param elem The element to test for being the last.
    /// @return \c true if \p{elem} is the last element of this list, \c false otherwise.
    [[nodiscard]]
    bool      isLast( const TElement* elem )               const noexcept { return last() == elem; }

    /// Hooks the given element to the beginning of this list.
    /// @param elem The element to insert to at the start.
    void      pushFront( TElement* elem )                       noexcept { hook.addBehind( elem ); }

    /// Hooks the given range of elements to the front of this list.
    /// @param first The first element of the range to insert.
    /// @param last  The last element of the range to insert.
    void      pushFront( TElement* first, TElement* last ) noexcept {hook.addBehind( first, last );}

    /// Hooks the given element to the end of this list.
    /// @param elem The element to insert to at the start.
    void      pushEnd( TElement* elem )                         noexcept { hook.addBefore( elem ); }

    /// Hooks the given range of elements to the end of this list.
    /// @param first The first element of the range to insert.
    /// @param last  The last element of the range to insert.
    void      pushEnd( TElement* first, TElement* last ) noexcept { hook.addBefore( first, last ); }

    /// Removes and returns the first element from this list.
    /// Must not be invoked on empty lists.
    /// @return A pointer to the first element (which was removed).
    TElement* popFront()  noexcept { TElement* first= hook.next(); first->remove();  return first; }

    /// Removes and returns the last element from this list.
    /// Must not be invoked on empty lists.
    /// @return A pointer to the last element (which was removed).
    TElement* popEnd()                                                                    noexcept {
        TElement* last= hook.prev();
        last->remove();
        return last;
    }

    /// Counts the number of elements found in the range starting with this list's first element and
    /// ending with the element before \p{end}.
    /// @param end   The element after the last one to count.
    ///              Defaults to a \c nullptr marking the end of the list.
    /// @return The number of elements in the range.
    [[nodiscard]]
    integer  count( const TNode* end= nullptr )                                     const noexcept {
        if( end == nullptr )
            end= &hook;

        integer count= 0;
        const TNode* node= hook.next();
        while( node != end ) {
            node= node->next();
            ++count;
        }
        return count;
    }
}; // struct BidiListHook


} // namespace [alib::detail]
