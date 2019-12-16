/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_monomem of the \aliblong.
 *
 * \emoji :copyright: 2013-2019 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_MONOMEM_LIST
#define HPP_ALIB_MONOMEM_LIST 1

#if !defined (HPP_ALIB_MONOMEM_MONOALLOCATOR)
#   include "alib/monomem/monoallocator.hpp"
#endif

#if !defined (HPP_ALIB_MONOMEM_DETAIL_RECYCLER)
#   include "alib/monomem/detail/recycler.hpp"
#endif

#if !defined(HPP_ALIB_FS_LISTS_BIDILIST)
#   include "alib/lib/fs_lists/bidilist.hpp"
#endif

namespace aworx { namespace lib { namespace monomem {

namespace detail
{
    /** Extents \b BidiNode by an value of type \p{T}.    */
    template<typename T>
    struct ListElement : public BidiNode<ListElement<T>>
    {
        T           data;       ///< The custom data object.
    };

    /**
     * Partially specialized helper struct used to determine the recycler type for struct
     * \alib{monomem,List}.
     */
    template<typename T, typename TRecycling> struct ListRecycler {};
#if !defined(ALIB_DOX)
    template<typename T> struct ListRecycler<T,Recycling::Private>{ using type= detail::RecyclerPrivate<detail::ListElement<T>>; };
    template<typename T> struct ListRecycler<T,Recycling::Shared >{ using type= detail::RecyclerShared <detail::ListElement<T>>; };
    template<typename T> struct ListRecycler<T,Recycling::None   >{ using type= detail::RecyclerVoid   <detail::ListElement<T>>; };
#endif

} // namespace detail


/** ************************************************************************************************
 * Implements a doubly linked list, likewise \c std::list does. Memory for inserted elements is
 * allocated using the \alib{monomem,MonoAllocator} provided with the constructor.
 *
 * Elements that are erased from the list will be recycled with subsequent insert operations.
 * With that, remove and insert operations do not lead to leaked memory in the monotonic allocator.
 *
 * It is allowed to use different allocator objects with insertions. However, the life-cycle
 * of the allocated memory and the objects stored in this container has to be kept in sync
 * by the user of this object. For more information on this topic see methods #Clear and #Reset.
 *
 * @tparam T          The type of the contained objects.
 * @tparam TRecycling Denotes the type of recycling that is to be performed. Possible values are
 *                    \alib{monomem,Recycling::Private} (the default),
 *                    \alib{monomem,Recycling::Shared} or \alib{monomem,Recycling::None}.
 **************************************************************************************************/
template<typename T, typename TRecycling>
class List : public detail::ListRecycler<T, TRecycling>::type
{
    // #############################################################################################
    // Node/Element type
    // #############################################################################################
    protected:
        /** The list element type.    */
        using Element= detail::ListElement<T>;

        /** The allocator assigned.  */
        MonoAllocator*          allocator;

        /** The list.     */
        BidiList<Element>       list;

        /** Shortcut to the recycler that this class is derived from.     */
        using TRecycler= typename detail::ListRecycler<T, TRecycling>::type;

    public:
        /** This type definition may be used to define an externally managed shared recycler,
         *  which can be passed to the alterative constructor of this class when template
         *  parameter \p{TRecycling} equals \alib{monomem,Recycling::Shared}.
         *  \see
         *    Chapter \ref alib_monomem_containers_recycling_shared of the Programmer's Manual
         *    for this \alibmod.                                                          */
        using TSharedRecycler=  ForwardList<Element>;

    // #############################################################################################
    // Helpers
    // #############################################################################################
    protected:
        /** Returns either a recycled or newly allocated element.
         *  @return A pointer to the element created or recycled.    */
        Element* allocElement()
        {
            Element* elem= TRecycler::get();
            if( elem != nullptr )
                return elem;

            return       allocator->Alloc<Element>();
        }

    // #############################################################################################
    // std::iterator
    // #############################################################################################
    protected:

        /** ****************************************************************************************
         * Implementation of \c std::iterator for this container.
         *
         * As the name of the class indicates, this iterator satisfies the C++ standard library
         * concept
         * \https{BidirectionalIterator,en.cppreference.com/w/cpp/named_req/BidirectionalIterator}.
         *
         * @tparam TConstOrMutableElement  The custom data type as either <c>const</c> or mutable.
         ******************************************************************************************/
        template<typename TConstOrMutableElement>
        struct TIterator
                : public std::iterator< std::bidirectional_iterator_tag, // iterator_category
                                        TConstOrMutableElement,          // value_type
                                        integer,                         // distance type
                                        TConstOrMutableElement*,         // pointer
                                        TConstOrMutableElement&          // reference
                                      >
        {
            private:
                /** The start of the list. */
                Element*          element;

                #if !defined(ALIB_DOX)
                    friend   class List<T, TRecycling>;
                #endif


            public:

            /** Default constructor creating an invalid iterator. */
            TIterator()                                                                   = default;

            /** Constructor.
             *  @param start Pointer to the initial element.   */
            explicit TIterator( Element* start )
            : element( start )
            {}

            #if defined(ALIB_DOX)
                    /** Copy constructor accepting a mutable iterator.
                     *  Available only for the constant version of this iterator.
                     *  @tparam TMutable The type of this constructor's argument.
                     *  @param mutableIt Mutable iterator to copy from.             */
                    template<typename TMutable>
                    TIterator( const TMutable& mutableIt );
             #else
                    ATMP_SELECT_IF_1TP( typename TMutable,
                    ATMP_EQ( TMutable,
                             TIterator<typename std::remove_const<TConstOrMutableElement>::type> ) )
                    TIterator( const TMutable& mutableIt )
                    : element( mutableIt.element )
                    {}
             #endif

            // ######################   To satisfy concept of  InputIterator   ######################

            /** Prefix increment operator.
             *  @return A reference to this object. */
            TIterator& operator++()
            {
                element= element->next();
                return *this;
            }

             /** Postfix increment operator.
             *  @return An iterator value that is not increased, yet. */
            TIterator operator++(int)
            {
                auto result= TIterator(element);
                element= element->next();
                return result;
            }

            /** Comparison operator.
             *  @param other  The iterator to compare ourselves to.
             *  @return \c true if this and the given iterator are pointing to the same element,
             *          \c false otherwise. */
            bool operator==(TIterator other)                                                   const
            {
                return element == other.element;
            }

            /** Comparison operator.
             *  @param other  The iterator to compare ourselves to.
             *  @return \c true if this and given iterator are not equal, \c false otherwise. */
            bool operator!=(TIterator other)                                                   const
            {
                return !(*this == other);
            }

            //##################   To satisfy concept of  BidirectionalIterator   ##################

            /** Prefix decrement operator.
             *  @return A reference to this object. */
            TIterator& operator--()
            {
                element= element->prev();
                return *this;
            }

            /** Postfix decrement operator.
             *  @return The iterator value prior the decrement operation. */
            TIterator operator--(int)
            {
                auto result= TIterator(element);
                element= element->prev();
                return result;
            }

            // ######################   Member access   ######################

            /** Retrieves a reference to the referred element.
             * @return A reference to the referred element.               */
            TConstOrMutableElement& operator*()                                                const
            {
                return element->data;
            }

            /** Retrieves a pointer to the referred element.
             * @return A pointer to the referred element.                 */
            TConstOrMutableElement* operator->()                                               const
            {
                return &element->data;
            }

        }; // struct TIterator

    public:
        /** The constant iterator exposed by this container. */
        using ConstIterator         = TIterator<const T>;

        /** The mutable iterator exposed by this container. */
        using Iterator              = TIterator<      T>;

        /** The constant iterator exposed by this container. */
        using ConstReverseIterator  = std::reverse_iterator<ConstIterator>;

        /** The mutable iterator exposed by this container. */
        using ReverseIterator       = std::reverse_iterator<Iterator>;

    /** ############################################################################################
     * @name std::iterator Interface
     ##@{ ########################################################################################*/
        /** Returns an iterator pointing to a mutable value at the start of this list.
         *  @return The start of this list.                                         */
        Iterator               begin()                   { return      Iterator( list.first()   ); }

        /** Returns an iterator pointing to the first element behind this list.
         *  @return The end of this list.                                           */
        Iterator               end()                     { return      Iterator( list.stopper() ); }

        /** Returns an iterator pointing to a constant value at the start of this list.
         *  @return The start of this list.                                         */
        ConstIterator          begin()  const            { return ConstIterator( list.first()   ); }

        /** Returns an iterator pointing to the first element behind this list.
         *  @return The end of this list.                                           */
        ConstIterator          end()    const            { return ConstIterator( list.stopper() ); }

        /** Returns an iterator pointing to a constant value at the start of this list.
         *  @return The start of this list.                                         */
        ConstIterator         cbegin()  const            { return ConstIterator( list.first()   ); }

        /** Returns an iterator pointing to the first element behind this list.
         *  @return The end of this list.                                           */
        ConstIterator         cend()    const            { return ConstIterator( list.stopper() ); }

        /** Returns a reverse iterator pointing to a mutable value at the end of this list.
         *  @return The start of this list.                                         */
        ReverseIterator       rbegin()                   { return      ReverseIterator( end()   ); }

        /** Returns a reverse iterator pointing to the first element behind the start of this list.
         * @return The end of this list.                                           */
        ReverseIterator       rend()                     { return      ReverseIterator( begin() ); }

        /** Returns a reverse iterator pointing to a constant value at the end of this list.
         *  @return The start of this list.                                        */
        ConstReverseIterator  rbegin()  const            { return ConstReverseIterator( end()   ); }

        /** Returns a reverse iterator pointing to the first element behind the start of this list.
         *  @return The end of this list.                                         */
        ConstReverseIterator  rend()    const            { return ConstReverseIterator( begin() ); }

        /** Returns a reverse iterator pointing to a constant value at the end of this list.
         *  @return The start of this list.                                        */
        ConstReverseIterator crbegin()  const            { return ConstReverseIterator( end()   ); }

        /** Returns a reverse iterator pointing to the first element behind the start of this list.
         *  @return The end of this list.                                         */
        ConstReverseIterator crend()    const            { return ConstReverseIterator( begin() ); }



    /** ############################################################################################
     * @name Construction/Destruction
     ##@{ ########################################################################################*/
    public:
        /** ****************************************************************************************
         * Copy constructor.
         * Invokes implementation dependent copy constructor of recycler, copies the pointer to the
         * allocator and then copies each element.
         *  @param copy The list to copy.
         ******************************************************************************************/
        List( const List& copy)
        : TRecycler ( copy           )
        , allocator ( copy.allocator )
        {
            for( auto& element : copy )
                PushBack( element );
        }

        /** ****************************************************************************************
         * Move constructor.
         * Invokes implementation dependent move constructor of recycler, moves the pointer to the
         * allocator and then copies each element.
         *  @param move The private recycler to move.
         ******************************************************************************************/
        List( List&& move)
        : TRecycler ( std::move( move           ) )
        , allocator ( std::move( move.allocator ) )
        , list      ( std::move( move.list      ) )
        {}

        #if defined(ALIB_DOX)
        /** ****************************************************************************************
         * Constructor accepting a pointer to the allocator.
         *
         * \note
         *   This cnstructor is not available if template argument \p{TRecycling} equals
         *   type \alib{monomem,Recycling::Shared}.
         *
         * @param pAllocator       The monotonic allocator to use.
         ******************************************************************************************/
         List( MonoAllocator*  pAllocator )
         : allocator( pAllocator )
         {}
        #else
            template<typename TEnableIf= TRecycling,
                     ATMP_T_IF(int, !ATMP_EQ(TEnableIf, Recycling::Shared)) = 0 >
            explicit
            List( MonoAllocator*  pAllocator )
            : allocator( pAllocator )
            {}
        #endif // defined(ALIB_DOX)

        /** ****************************************************************************************
         * Constructor taking a shared recycler.
         * @param pAllocator  The monotonic allocator to use.
         * @param pRecycler   The shared recycler.
         ******************************************************************************************/
        explicit
        List( MonoAllocator*  pAllocator, TSharedRecycler& pRecycler )
        : TRecycler( pRecycler  )
        , allocator( pAllocator )
        {}


        /** ****************************************************************************************
         * Destructor. Invokes #Clear.
         ******************************************************************************************/
        ~List()
        {
            if ALIB_CONSTEXPR_IF( !std::is_trivially_destructible<T>::value )
                Clear();
        }

    /** ############################################################################################
     * @name Allocation
     ##@{ ########################################################################################*/
        /** ****************************************************************************************
         * Returns the allocator of this object. Usually the allocator might be used to perform
         * allocations in respect to data found in objects stored in this object.
         * However, such allowance is dependent on the use case and not part of this class's
         * contract.
         *
         * @return The allocator that was provided in the constructor.
         ******************************************************************************************/
        MonoAllocator* GetAllocator()                                                         const
        {
            return allocator;
        }

        /** ****************************************************************************************
         * Counts the number of currently allocated but unused (not contained) list elements
         * that will be recycled with upcoming insertions.
         *
         * \note
         *   This method is provided for completeness and unit-testing. It should not be of
         *   relevance for common usage.<br>
         *   Furthermore, this method is not available (aka does not compile) with instantiations
         *   that specify template parameter \p{TRecycling} as \alib{monomem,Recycling::None}.
         *
         * @return The number of removed and not yet recycled elements.
         ******************************************************************************************/
        integer     RecyclablesCount()                                                         const
        {
            return TRecycler::count();
        }


    /** ############################################################################################
     * @name Size and Capacity
     ##@{ ########################################################################################*/
        /** ****************************************************************************************
         * Evaluates the size of the list by traversing all elements.
         * @return The number of elements contained in this listed.
         ******************************************************************************************/
        integer      Size()                                                                    const
        {
            return list.count();
        }

        /** ****************************************************************************************
         * Tests this container for emptiness.
         * @return \c true if this list is empty, \c false otherwise.
         ******************************************************************************************/
        bool        IsEmpty()                                                                  const
        {
            return list.isEmpty();
        }

        /** ****************************************************************************************
         * Tests this container for emptiness.
         * @return \c true if this list is empty, \c false otherwise.
         ******************************************************************************************/
        bool        IsNotEmpty()                                                               const
        {
            return list.isNotEmpty();
        }

        /** ****************************************************************************************
         * Invokes the destructor of all elements and empties the list.
         * All allocated internal elements are kept for future recycling.
         * Hence, the invocation of this method with this value implies that associated
         * monotonic allocator is \b not reset.
         *
         * As the life-cycle of the monotonic allocator(s) used for insertions is not under control of
         * this object, it is the obligation of the caller to ensure that the monotonic allocator is
         * kept in sync with this object.
         ******************************************************************************************/
        void        Clear()
        {
            if( list.isNotEmpty() )
            {
                for( auto& it : list )
                    it.data.~T();
                TRecycler::recycle( list.first(), list.last() );
                list.reset();
            }
        }

        /** ****************************************************************************************
         * Invokes #Clear and in case of \alib{monomem,Recycling::Private,private recycling}
         * disposes all previously allocated recyclable instances of internal element type.
         *
         * Hence, the invocation of this method with this value usually implies that
         * the monotonic allocator used is reset as well.
         *
         * As the life-cycle of the monotonic allocator(s) used for insertions is not under control of
         * this object, it is the obligation of the caller to ensure that the monotonic allocator is
         * kept in sync with this object.
         ******************************************************************************************/
        void        Reset()
        {
            Clear();
            TRecycler::disposeRecyclablesIfPrivate();
        }

        /** ****************************************************************************************
         * Allocates the required memory for the number of additional elements expected.
         *
         * This method may be helpful if the definite number of stored elements that is never
         * exceeded is known. In this case, a \alib{monomem,MonoAllocator::TakeSnapshot,snapshot}
         * of the monotonic allocator could be taken after the invocation of this method and
         * then used to reset the monotonic allocator with preserving the memory used by this container.
         *
         * \note
         *   This method is not available (aka does not compile) with instantiations
         *   that specify template parameter \p{TRecycling} as \alib{monomem,Recycling::None}.
         *
         * @param expectedSize The expected number of elements to be stored in the hash table.
         ******************************************************************************************/
        void        ReserveRecyclables( integer expectedSize )
        {
            auto requiredRecyclables=  (expectedSize - Size()) - RecyclablesCount();
            if( requiredRecyclables > 0 )
            {
                Element* newElements= allocator->template AllocArray<Element>( requiredRecyclables );
                for( auto i= requiredRecyclables -2; i >= 0   ; --i )
                    newElements[i].makePointTo( &newElements[i + 1] );

                TRecycler::recycle( &newElements[0], &newElements[requiredRecyclables - 1] );
            }
        }

    /** ############################################################################################
     * @name List Start/End Access
     ##@{ ########################################################################################*/

        /** ****************************************************************************************
         * Traverses the list to return the item with the given \p{idx}.
         * (Executes in linear time <b>O(N)</b>.)
         *
         * @param  idx  The index of the element to retrieve.
         * @return A mutable reference to \p{T}.
         ******************************************************************************************/
        T&          ElementAt(integer idx)
        {
            ALIB_ASSERT_ERROR( list.isNotEmpty(),
                               "Reference to element requested on empty monomem::List")

            Element* act= list.forward;
            for( integer i= 1 ; i < idx ; ++i )
            {
                act= act->forward;
                ALIB_ASSERT_ERROR( act != nullptr, "Element index out of bounds")
            }
            return act->data;
        }

        /** ****************************************************************************************
         * Traverses the list to return the item with the given \p{idx}.
         * (Executes in linear time <b>O(N)</b>.)
         *
         * @param  idx  The index of the element to retrieve.
         * @return A constant reference to \p{T}.
         ******************************************************************************************/
        const T&    ElementAt(integer idx)                                                     const
        {
            ALIB_ASSERT_ERROR( list.isNotEmpty(),
                               "Reference to element requested on empty monomem::List")

            Element* act= list.forward;
            for( integer i= 1 ; i < idx ; ++i )
            {
                act= act->forward;
                ALIB_ASSERT_ERROR( act != nullptr, "Element index out of bounds")
            }
            return act->data;
        }

        /** ****************************************************************************************
         * Returns a non-constant reference to the first object of the list.
         * @return A mutable reference to \p{T}.
         ******************************************************************************************/
        T&          Front()
        {
            ALIB_ASSERT_ERROR( list.isNotEmpty(),
                               "Reference to element requested on empty monomem::List")
            return list.first()->data;
        }


        /** ****************************************************************************************
         * Returns a constant reference to the first object of the list.
         * @return A constant reference to \p{T}.
         ******************************************************************************************/
        const T&    Front()                                                                    const
        {
            ALIB_ASSERT_ERROR( list.isNotEmpty(),
                               "Reference to element requested on empty monomem::List")
            return list.first()->data;
        }

        /** ****************************************************************************************
         * Returns a non-constant reference to the last object of the list.
         * @return A mutable reference to \p{T}.
         ******************************************************************************************/
        T&          Back()
        {
            ALIB_ASSERT_ERROR( list.isNotEmpty(),
                               "Reference to element requested on empty monomem::List")
            return list.last()->data;
        }

        /** ****************************************************************************************
         * Returns a constant reference to the last object of the list.
         * @return A constant reference to \p{T}.
         ******************************************************************************************/
        const T&    Back()                                                                     const
        {
            ALIB_ASSERT_ERROR( list.isNotEmpty(),
                               "Reference to element requested on empty monomem::List")
            return list.last()->data;
        }

    /** ############################################################################################
     * @name Element Insertion
     ##@{ ########################################################################################*/
        /** ****************************************************************************************
         * Adds a new element before the given \p{position}.
         * @param  position   The position to emplace the new element.
         * @param  copy       The value to copy and insert.
         * @return A constant reference to the element added.
         ******************************************************************************************/
        Iterator    Insert(ConstIterator position, const T& copy)
        {
            Element* elem= allocElement();
            new (&elem->data) T(copy);
            position.element->addBefore( elem );

            return Iterator(elem);
        }

        /** ****************************************************************************************
         * Moves a value into this container before the given \p{position}.
         * @param  position   The position to emplace the new element.
         * @param  move       The value to move into this container.
         * @return A constant reference to the element moved.
         ******************************************************************************************/
        Iterator    Insert(ConstIterator position, T&& move)
        {
            Element* elem= allocElement();
            new (&elem->data) T(std::move(move));
            position.element->addBefore( elem );

            return Iterator(elem);
        }

        /** ****************************************************************************************
         * Adds a new element at the end of the list.
         * @param  copy       The value to copy and insert.
         * @return A reference to the element added.
         ******************************************************************************************/
        T&          PushBack(const T& copy)
        {
            Element* elem= allocElement();

            new (&elem->data) T(copy);
            list.pushEnd( elem );
            return elem->data;
        }

        /** ****************************************************************************************
         * Moves a value to the end of this list.
         * @param  move       The value to move into this container.
         * @return A reference to the element moved.
         ******************************************************************************************/
        T&          PushBack(T&& move)
        {
            Element* elem= allocElement();

            new (&elem->data) T(std::move(move));
            list.pushEnd( elem );
            return elem->data;
        }

        /** ****************************************************************************************
         * Adds a new element at the start of the list.
         * @param  copy       The value to copy and insert.
         * @return A reference to the element added.
         ******************************************************************************************/
        T&          PushFront(const T& copy)
        {
            Element* elem= allocElement();
            new (&elem->data) T(copy);
            list.pushFront( elem );
            return elem->data;
        }

        /** ****************************************************************************************
         * Moves a value to the start of this list.
         * @param  move       The value to move into this container.
         * @return A reference to the element moved.
         ******************************************************************************************/
        T&          PushFront(T&& move)
        {
            Element* elem= allocElement();

            new (&elem->data) T(std::move(move));
            list.pushFront( elem );
            return elem->data;
        }

        /** ****************************************************************************************
         * Adds a new element before the given \p{position}.
         * @tparam TArgs Types of variadic parameters given with parameter \p{args}.
         * @param  position   The position to emplace the new element.
         * @param  args       Variadic parameters to be forwarded to constructor of type \p{T}.
         * @return A constant reference to the element added.
         ******************************************************************************************/
        template<typename... TArgs>
        Iterator    Emplace(ConstIterator position, TArgs&&... args)
        {
            Element* elem= allocElement();
            new (&elem->data) T( std::forward<TArgs>(args)... );
            position.element->addBefore( elem );

            return Iterator(elem);
        }

        /** ****************************************************************************************
         * Adds a new element at the end of the list.
         * @tparam TArgs        Types of variadic parameters given with parameter \p{args}.
         * @param  args         Variadic parameters to be forwarded to constructor of type \p{T}.
         * @return A reference to the element added.
         ******************************************************************************************/
        template<typename... TArgs>
        T&          EmplaceBack(TArgs&&... args)
        {
            Element* elem= allocElement();

            new (&elem->data) T( std::forward<TArgs>(args)... );
            list.pushEnd( elem );
            return elem->data;
        }

        /** ****************************************************************************************
         * Adds a new element at the end of the list.
         * @tparam TArgs        Types of variadic parameters given with parameter \p{args}.
         * @param  args         Variadic parameters to be forwarded to constructor of type \p{T}.
         * @return A reference to the element added.
         ******************************************************************************************/
        template<typename... TArgs>
        T&          EmplaceFront(TArgs&&... args)
        {
            Element* elem= allocElement();

            new (&elem->data) T( std::forward<TArgs>(args)... );
            list.pushFront( elem );
            return elem->data;
        }

    /** ############################################################################################
     * @name Element Removal
     ##@{ ########################################################################################*/
        /** ****************************************************************************************
         * Removes an element at the given position.
         *
         * @param position A constant iterator pointing to the element to be removed.
         *                 Mutable iterators are inherently converted with the invocation of this
         *                 method.
         * @return A mutable iterator pointing behind the removed element.
         *         If \p{position} refers to the last element of the list, iterator #end() is
         *         returned.
         ******************************************************************************************/
        Iterator    Erase(ConstIterator position)
        {
            ALIB_ASSERT_ERROR( list.isNotEmpty(), "Erase requested on empty monomem::List")
            ALIB_ASSERT_ERROR( position != end(), "Iterator end() given with monomem::List::Erase")


            Element*   next   = position.element->next();
            position.element->remove();
            monomem::Destruct(&position.element->data);
            TRecycler::recycle( position.element );

            return Iterator(next);
        }

        /** ****************************************************************************************
         * Removes a range of elements defined by iterators \p{first} and \p{last}.
         *
         * @param first  The start of the range to remove.
         * @param last   The first element behind the range to remove.
         * @return A mutable iterator referring to the given \p{last}.
         ******************************************************************************************/
        Iterator    Erase(ConstIterator first, ConstIterator last )
        {
            ALIB_ASSERT_ERROR( list.isNotEmpty() || ( first == begin() && last == end() ),
                               "Erase requested on empty monomem::List")
            if( first == last )
                return Iterator(const_cast<Element*>( last.element ));

            // destruct objects
            Element*   elem = const_cast<Element*>( first.element );
            while ( elem != last.element )
            {
                monomem::Destruct(&elem->data);
                Element::moveForward( elem );
            }

            Element*   lastElem= last.element->prev();
            first.element->remove( lastElem );
            TRecycler::recycle( first.element, lastElem );

            return Iterator( last.element );
        }

        /** ****************************************************************************************
         * Removes the first element.
         ******************************************************************************************/
        void        PopFront()
        {
            ALIB_ASSERT_ERROR( !IsEmpty(), "PopBack called on empty List instance." )
            Element* element= list.popFront();
            element->data.~T();
            TRecycler::recycle( element );
        }

        /** ****************************************************************************************
         * Removes the last element.
         ******************************************************************************************/
        void        PopBack()
        {
            ALIB_ASSERT_ERROR( !IsEmpty(), "PopBack called on empty List instance." )
            Element* element= list.popEnd();
            monomem::Destruct(&element->data);
            TRecycler::recycle( element );
        }
};

}}// namespace aworx[::lib::monomem]

/// Type alias in namespace #aworx.
template<typename T, typename TRecycling = lib::monomem::Recycling::Private>
using  List       = lib::monomem::List<T, TRecycling>;

} // namespace [aworx]

#endif // HPP_ALIB_MONOMEM_LIST
