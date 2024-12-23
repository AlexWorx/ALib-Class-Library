//==================================================================================================
/// \file
/// This header file is part of module \alib_containers of the \aliblong.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_MONOMEM_CONTAINERS_LIST
#define HPP_ALIB_MONOMEM_CONTAINERS_LIST 1
#pragma once
#if !defined(DOXYGEN)
#   include "alib/alib.hpp"
#endif
ALIB_ASSERT_MODULE(CONTAINERS)
#include "alib/containers/recycler.hpp"
#include "alib/lang/bidilist.hpp"
#include "alib/lang/dbgcriticalsections.hpp"
#include <iterator>

namespace alib {

/// This is the reference documentation of namespace <c>%alib::containers</c>, which
/// holds types of module \alib_containers_nl, which in turn is part of the \aliblink.
///
/// Extensive documentation for this module is provided with the
/// \ref alib_mods_contmono "Programmer's Manual" of this module.
namespace containers {

/// Detail namespace of module \alib_containers.
namespace detail
{
    /// Extents \b BidiNodeBase by an value of type \p{T}.
    template<typename T>
    struct ListElement : public lang::BidiNodeBase<ListElement<T>>
    {
        T           data;       ///< The custom data object.
    };

} // namespace detail



//==================================================================================================
/// Implements a doubly linked list, likewise \c std::list does. Memory for inserted elements is
/// allocated using the \alib{lang;Allocator} provided with construction.
///
/// Elements that are erased from the list will be recycled with subsequent insert operations.
/// With that, remove and insert operations do not lead to leaked memory when a monotonic
/// allocator is used.
///
/// It is allowed to use different allocator objects with insertions. However, the life-cycle
/// of the allocated memory and the objects stored in this container has to be kept in sync
/// by the user of this object. For more information on this topic see methods #Clear and #Reset.
///
/// @see
///  - Chapter \ref alib_contmono_containers_types of the joint Programmer's Manuals of modules
///    \alib_containers and \alib_monomem.
///  - Chapter \ref alib_threads_intro_assert_entry of the Programmer's Manual of module
///    \alib_threads for information about debugging multithreaded access on instances of this type.
///
/// @tparam TAllocator The allocator type to use.
/// @tparam T          The type of the contained objects.
/// @tparam TRecycling Denotes the type of recycling that is to be performed. Possible values are
///                    \alib{containers;Recycling;None},
///                    \alib{containers;Recycling;Private} (the default), or
///                    \alib{containers;Recycling;Shared}.
//==================================================================================================
template<typename TAllocator, typename T, Recycling TRecycling= Recycling::Private>
class List : lang::BidiListHook<detail::ListElement<T>>
           , detail::RecyclingSelector<TRecycling>:: template Type< TAllocator,
                                                                    detail::ListElement<T> >
#if ALIB_DEBUG_CRITICAL_SECTIONS
           , public lang::DbgCriticalSections
#endif
{
    // #############################################################################################
    // Node/Element type
    // #############################################################################################
    protected:
        /// The allocator type that \p{TAllocator} specifies.
        using hook= lang::BidiListHook<detail::ListElement<T>>;

        /// The allocator type that \p{TAllocator} specifies.
        using allocBase= lang::AllocatorMember<TAllocator>;

        /// The list element type.
        using Element= detail::ListElement<T>;

        /// The recycler type.
        using recyclerType=  typename detail::RecyclingSelector<TRecycling>:: template Type< TAllocator,
                                                     detail::ListElement<T> >;

    public:
        /// The allocator type that \p{TAllocator} specifies.
        using AllocatorType=        TAllocator;

        /// This type definition may be used to define an externally managed shared recycler,
        /// which can be passed to the alternative constructor of this class when template
        /// parameter \p{TRecycling} equals \alib{containers;Recycling;Shared}.
        /// \see
        ///   Chapter \ref alib_contmono_containers_recycling_shared of the Programmer's Manual
        ///   for this \alibmod.
        using SharedRecyclerType=  typename detail::RecyclingSelector<TRecycling>
                                 ::template HookType<TAllocator, detail::ListElement<T> >;

    // #############################################################################################
    // std::iterator_traits
    // #############################################################################################
    protected:

        //==========================================================================================
        /// Implementation of \c std::iterator_traits for this container.
        ///
        /// As the name of the class indicates, this iterator satisfies the C++ standard library
        /// concept
        /// \https{BidirectionalIterator,en.cppreference.com/w/cpp/named_req/BidirectionalIterator}.
        ///
        /// @tparam TConstOrMutableElement  The custom data type as either <c>const</c> or mutable.
        //==========================================================================================
        template<typename TConstOrMutableElement>
        struct TIterator
        {
            using iterator_category = std::bidirectional_iterator_tag;  ///< Implementation of <c>std::iterator_traits</c>.
            using value_type        = TConstOrMutableElement         ;  ///< Implementation of <c>std::iterator_traits</c>.
            using difference_type   = integer                        ;  ///< Implementation of <c>std::iterator_traits</c>.
            using pointer           = TConstOrMutableElement*        ;  ///< Implementation of <c>std::iterator_traits</c>.
            using reference         = TConstOrMutableElement&        ;  ///< Implementation of <c>std::iterator_traits</c>.

            protected:
                Element*          element; ///< The actual element of the list.

                #if !DOXYGEN
                    friend   class List<TAllocator, T, TRecycling>;
                #endif

            public:

            /// Default constructor creating an invalid iterator.
            TIterator()                                                                   = default;

            /// Constructor.
            /// @param start Pointer to the initial element.
            explicit TIterator( Element* start )
            : element( start )
            {}

            #if DOXYGEN
                    /// Copy constructor accepting a mutable iterator.
                    /// Available only for the constant version of this iterator.
                    /// @tparam TMutable The type of this constructor's argument.
                    /// @param mutableIt Mutable iterator to copy from.
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

            /// Prefix increment operator.
            /// @return A reference to this object.
            TIterator& operator++()
            {
                element= element->next();
                return *this;
            }

             /// Postfix increment operator.
            /// @return An iterator value that is not increased, yet.
            TIterator operator++(int)
            {
                auto result= TIterator(element);
                element= element->next();
                return result;
            }

            /// Comparison operator.
            /// @param other  The iterator to compare ourselves to.
            /// @return \c true if this and the given iterator are pointing to the same element,
            ///         \c false otherwise.
            bool operator==(TIterator other)                                                   const
            {
                return element == other.element;
            }

            /// Comparison operator.
            /// @param other  The iterator to compare ourselves to.
            /// @return \c true if this and given iterator are not equal, \c false otherwise.
            bool operator!=(TIterator other)                                                   const
            {
                return !(*this == other);
            }

            //##################   To satisfy concept of  BidirectionalIterator   ##################

            /// Prefix decrement operator.
            /// @return A reference to this object.
            TIterator& operator--()
            {
                element= element->prev();
                return *this;
            }

            /// Postfix decrement operator.
            /// @return The iterator value prior the decrement operation.
            TIterator operator--(int)
            {
                auto result= TIterator(element);
                element= element->prev();
                return result;
            }

            // ######################   Member access   ######################

            /// Retrieves a reference to the referred element.
            /// @return A reference to the referred element.
            TConstOrMutableElement& operator*()                     const { return  element->data; }

            /// Retrieves a pointer to the referred element.
            /// @return A pointer to the referred element.
            TConstOrMutableElement* operator->()                    const { return &element->data; }

        }; // struct TIterator

    public:
        /// The constant iterator exposed by this container.
        using ConstIterator         = TIterator<const T>;

        /// The mutable iterator exposed by this container.
        using Iterator              = TIterator<      T>;

        /// The constant iterator exposed by this container.
        using ConstReverseIterator  = std::reverse_iterator<ConstIterator>;

        /// The mutable iterator exposed by this container.
        using ReverseIterator       = std::reverse_iterator<Iterator>;

    //##############################################################################################
    /// @name std::iterator_traits Interface
    //##############################################################################################
        /// Returns an iterator pointing to a mutable value at the start of this list.
        /// @return The start of this list.
        Iterator               begin()              { return             Iterator( hook::first() ); }

        /// Returns an iterator pointing to the first element behind this list.
        /// @return The end of this list.
        Iterator               end()                { return             Iterator( hook::end() );   }

        /// Returns an iterator pointing to a constant value at the start of this list.
        /// @return The start of this list.
        ConstIterator          begin()  const       { return        ConstIterator( hook::first() ); }

        /// Returns an iterator pointing to the first element behind this list.
        /// @return The end of this list.
        ConstIterator          end()    const       { return        ConstIterator( hook::end() );   }

        /// Returns an iterator pointing to a constant value at the start of this list.
        /// @return The start of this list.
        ConstIterator         cbegin()  const       { return        ConstIterator( hook::first() ); }

        /// Returns an iterator pointing to the first element behind this list.
        /// @return The end of this list.
        ConstIterator         cend()    const       { return        ConstIterator( hook::end() );   }

        /// Returns a reverse iterator pointing to a mutable value at the end of this list.
        /// @return The start of this list.
        ReverseIterator       rbegin()              { return      ReverseIterator( end() );        }

        /// Returns a reverse iterator pointing to the first element behind the start of this list.
        /// @return The end of this list.
        ReverseIterator       rend()                { return      ReverseIterator( begin() );      }

        /// Returns a reverse iterator pointing to a constant value at the end of this list.
        /// @return The start of this list.
        ConstReverseIterator  rbegin()  const       { return ConstReverseIterator( end() );        }

        /// Returns a reverse iterator pointing to the first element behind the start of this list.
        /// @return The end of this list.
        ConstReverseIterator  rend()    const       { return ConstReverseIterator( begin() );      }

        /// Returns a reverse iterator pointing to a constant value at the end of this list.
        /// @return The start of this list.
        ConstReverseIterator crbegin()  const       { return ConstReverseIterator( end() );        }

        /// Returns a reverse iterator pointing to the first element behind the start of this list.
        /// @return The end of this list.
        ConstReverseIterator crend()    const       { return ConstReverseIterator( begin() );      }


    //##############################################################################################
    // Construction/Destruction
    //##############################################################################################
    public:
        //==========================================================================================
        /// Constructor neither requiring an allocator, nor a shared recycler.
        /// \note
        ///   This constructor is not available if the template argument \p{TRecycling} equals
        ///   \alib{containers;Recycling;Shared} and if argument \p{TAllocator}
        ///   does not equal type \alib{lang;HeapAllocator}.
        //==========================================================================================
        List()
        #if ALIB_DEBUG_CRITICAL_SECTIONS
        : lang::DbgCriticalSections("List")
        #endif
        {}

        //==========================================================================================
        /// Constructor that takes an initializer list, but neither a n allocator, nor a
        /// shared recycler.
        /// \note
        ///   This constructor is not available if the template argument \p{TRecycling} equals
        ///   \alib{containers;Recycling;Shared}.
        ///
        /// @param initList   The initial lists of elements to add.
        //==========================================================================================
        List(std::initializer_list<T> initList)
        : List()
        {
            for (const auto& item : initList)
                PushBack(item);
        }

        //==========================================================================================
        /// Copy constructor.
        /// Invokes implementation dependent copy constructor of recycler, copies the pointer to the
        /// allocator and then copies each element.
        ///  @param copy The list to copy.
        //==========================================================================================
        List( const List& copy)
        : hook()
        , recyclerType( copy )
        #if ALIB_DEBUG_CRITICAL_SECTIONS
         ,lang::DbgCriticalSections("List")
        #endif
        {
            ALIB_DCS_WITH(copy)
            for( auto& element : copy )
                PushBack( element );
        }

        //==========================================================================================
        /// Move constructor.
        /// Invokes implementation dependent move constructor of recycler, moves the pointer to the
        /// allocator and then copies each element.
        ///  @param move The private recycler to move.
        //==========================================================================================
        List( List&& move)
        : hook        ( std::move( move.list      ) )
        , recyclerType( std::move( move.recycler  ) )
        {}

        //==========================================================================================
        /// Constructor accepting an allocator.
        /// \note
        ///   This constructor is not available if the template argument \p{TRecycling} equals
        ///   \alib{containers;Recycling;Shared} and if argument \p{TAllocator}
        ///   does not equal type \alib{lang;HeapAllocator}.
        ///
        /// @param pAllocator       The allocator to use.
        //==========================================================================================
        List( AllocatorType&  pAllocator )
        : hook()
        , recyclerType( pAllocator )
        #if ALIB_DEBUG_CRITICAL_SECTIONS
        ,lang::DbgCriticalSections("List")
        #endif
        {}

        //==========================================================================================
        /// Constructor that takes an allocator and an initializer list.
        /// \note
        ///   This constructor is not available if the template argument \p{TRecycling} equals
        ///   \alib{containers;Recycling;Shared}.
        ///
        /// @param pAllocator The allocator to use.
        /// @param initList   The initial lists of elements to add.
        //==========================================================================================
        List(AllocatorType&  pAllocator, std::initializer_list<T> initList)
        : List(pAllocator)
        {
            for (const auto& item : initList)
                PushBack(item);
        }

        //==========================================================================================
        /// Constructor taking a shared recycler.
        /// This constructor is not available if the template argument \p{TRecycling} does not equal
        /// \alib{containers;Recycling;Shared}.
        /// @param pSharedRecycler   The shared recycler.
        //==========================================================================================
        template<typename TSharedRecycler= SharedRecyclerType,
                 ATMP_T_IF(int, !ATMP_EQ(TSharedRecycler , void)) = 0 >
        List(TSharedRecycler& pSharedRecycler )
        : hook()
        , recyclerType( pSharedRecycler  )
        #if ALIB_DEBUG_CRITICAL_SECTIONS
        ,lang::DbgCriticalSections("List")
        #endif
        {}

        //==========================================================================================
        /// Constructor taking a shared recycler and an initializer list.
        /// This constructor is not available if the template argument \p{TRecycling} does not equal
        /// \alib{containers;Recycling;Shared}.
        /// @param pSharedRecycler   The shared recycler.
        /// @param initList          The initial lists of elements to add.
        //==========================================================================================
        template<typename TSharedRecycler= SharedRecyclerType,
                 ATMP_T_IF(int, !ATMP_EQ(TSharedRecycler , void)) = 0 >
        List(TSharedRecycler& pSharedRecycler, std::initializer_list<T> initList)
        : List(pSharedRecycler)
        {
            for (const auto& item : initList)
                PushBack(item);
        }

        //==========================================================================================
        /// Destructor. Invokes #Clear.
        //==========================================================================================
        ~List()
        {
            if (IsNotEmpty() )
                recyclerType::DisposeList( hook::first(), hook::end() );
        }

    //##############################################################################################
    /// @name Allocation
    //##############################################################################################
        //==========================================================================================
        /// Returns the allocator of this object. Usually the allocator might be used to perform
        /// allocations in respect to data found in objects stored in this object.
        /// However, such allowance is dependent on the use case and not part of this class's
        /// contract.
        ///
        /// @return The allocator that was provided in the constructor.
        //==========================================================================================
        AllocatorType& GetAllocator()                          { return allocBase::GetAllocator(); }

        //==========================================================================================
        /// Counts the number of currently allocated but unused (not contained) list elements
        /// that will be recycled with upcoming insertions.
        ///
        /// \note
        ///   This method is provided for completeness and unit-testing. It should not be of
        ///   relevance for common usage.<br>
        ///   Furthermore, this method is not available (aka does not compile) with instantiations
        ///   that specify template parameter \p{TRecycling} as
        ///   \alib{containers;Recycling;None}.
        ///
        /// @return The number of removed and not yet recycled elements.
        //==========================================================================================
        integer     RecyclablesCount()        const {ALIB_DCS_SHARED return recyclerType::Count(); }


    //##############################################################################################
    /// @name Size and Capacity
    //##############################################################################################
        //==========================================================================================
        /// Evaluates the size of the list by traversing all elements.
        /// @return The number of elements contained in this listed.
        //==========================================================================================
        integer		Count()                          const  {ALIB_DCS_SHARED return hook::count(); }

        //==========================================================================================
        /// Tests this container for emptiness.
        /// @return \c true if this list is empty, \c false otherwise.
        //==========================================================================================
        bool        IsEmpty()                       const {ALIB_DCS_SHARED return hook::isEmpty(); }

        //==========================================================================================
        /// Tests this container for emptiness.
        /// @return \c true if this list is empty, \c false otherwise.
        //==========================================================================================
        bool        IsNotEmpty()                   const {ALIB_DCS_SHARED return !hook::isEmpty(); }

        //==========================================================================================
        /// Invokes the destructor of all elements and empties the list.
        /// All allocated internal elements are kept for future recycling.
        /// Hence, the invocation of this method with this value implies that an associated
        /// monotonic allocator is \b not reset.
        ///
        /// As the life-cycle of the monotonic allocator(s) used for insertions is not under control
        /// of this object, it is the obligation of the caller to ensure that the monotonic allocator
        /// is kept in sync with this object.
        //==========================================================================================
        void        Clear()
        {ALIB_DCS
            if( IsNotEmpty() )
            {
                recyclerType::RecycleList( hook::first(), hook::end() );
                hook::reset();
            }
        }

        //==========================================================================================
        /// Same as clear, but does not recycle internal nodes. Furthermore, all recyclables
        /// are deleted. The latter is done only if recycling type is not
        /// \alib{containers;Recycling;Shared}. In this case, the elements are still
        /// recycled.
        ///
        /// This method is useful with monotonic allocators, that can be reset as well, after
        /// this instance is reset.
        /// But, because the life-cycle of the monotonic allocator(s) used for insertions is not
        /// under control of this object, it is the obligation of the caller to ensure that the
        /// monotonic allocator is kept in sync with this object.
        //==========================================================================================
        void        Reset()
        {ALIB_DCS
            if( IsNotEmpty() )
            {
                recyclerType::DisposeList( hook::first(), hook::end() );
                hook::reset();
            }
            recyclerType::Reset();
        }

        //==========================================================================================
        /// Allocates the required memory for the number of additional elements expected.
        /// @see Chapter \ref alib_contmono_containers_recycling_reserving of the Programmer's
        ///      Manual.
        ///
        /// @param qty       The expected resulting number (or increase) of elements to be stored in
        ///                  this container.
        /// @param reference Denotes whether \p{qty} is meant as an absolute size or an
        ///                  increase.
        //==========================================================================================
        void        ReserveRecyclables( integer qty, lang::ValueReference reference )
        {ALIB_DCS
            auto requiredRecyclables= (   qty
                                        - (reference == lang::ValueReference::Absolute ? Count()
                                                                                       : 0     ) )
                                        - recyclerType::Count();

            if( requiredRecyclables > 0 )
                recyclerType::Reserve( requiredRecyclables );
        }

    //##############################################################################################
    /// @name Element Access
    //##############################################################################################

        //==========================================================================================
        /// Traverses the list to return the item with the given \p{idx}.
        /// (Executes in linear time <b>O(N)</b>.)
        ///
        /// @param  idx  The index of the element to retrieve.
        /// @return A mutable reference to \p{T}.
        //==========================================================================================
        T&          ElementAt(integer idx)
        {ALIB_DCS_SHARED
            ALIB_ASSERT_ERROR( !hook::isEmpty(), "MONOMEM/LIST",
                               "Reference to element requested on empty containers::List")

            Element* act= hook::first();
            for( integer i= 0 ; i < idx ; ++i )
            {
                act= act->next();
                ALIB_ASSERT_ERROR( act != nullptr, "MONOMEM/LIST", "Element index out of bounds")
            }
            return act->data;
        }

        //==========================================================================================
        /// Traverses the list to return the item with the given \p{idx}.
        /// (Executes in linear time <b>O(N)</b>.)
        ///
        /// @param  idx  The index of the element to retrieve.
        /// @return A constant reference to \p{T}.
        //==========================================================================================
        const T&    ElementAt(integer idx)                                                     const
        {ALIB_DCS_SHARED
            ALIB_ASSERT_ERROR( hook::isNotEmpty(), "MONOMEM/LIST",
                               "Reference to element requested on empty containers::List")

            Element* act= hook::first();
            for( integer i= 0 ; i < idx ; ++i )
            {
                act= act->next();
                ALIB_ASSERT_ERROR( act != nullptr, "MONOMEM/LIST", "Element index out of bounds")
            }
            return act->data;
        }

        //==========================================================================================
        /// Returns a non-constant reference to the first object of the list.
        /// @return A mutable reference to \p{T}.
        //==========================================================================================
        T&          Front()
        {ALIB_DCS_SHARED
            ALIB_ASSERT_ERROR( !hook::isEmpty(), "MONOMEM/LIST",
                               "Reference to element requested on empty containers::List")
            return hook::first()->data;
        }


        //==========================================================================================
        /// Returns a constant reference to the first object of the list.
        /// @return A constant reference to \p{T}.
        //==========================================================================================
        const T&    Front()                                                                    const
        {ALIB_DCS_SHARED
            ALIB_ASSERT_ERROR( !hook::isEmpty(), "MONOMEM/LIST",
                               "Reference to element requested on empty containers::List")
            return hook::first()->data;
        }

        //==========================================================================================
        /// Returns a non-constant reference to the last object of the list.
        /// @return A mutable reference to \p{T}.
        //==========================================================================================
        T&          Back()
        {ALIB_DCS_SHARED
            ALIB_ASSERT_ERROR( !hook::isEmpty(), "MONOMEM/LIST",
                               "Reference to element requested on empty containers::List")
            return hook::last()->data;
        }

        //==========================================================================================
        /// Returns a constant reference to the last object of the list.
        /// @return A constant reference to \p{T}.
        //==========================================================================================
        const T&    Back()                                                                     const
        {ALIB_DCS_SHARED
            ALIB_ASSERT_ERROR( hook::isNotEmpty(), "MONOMEM/LIST",
                               "Reference to element requested on empty containers::List")
            return hook::last()->data;
        }

    //##############################################################################################
    /// @name Element Insertion
    //##############################################################################################
        //==========================================================================================
        /// Adds a new element before the given \p{position}.
        /// @param  position   The position to emplace the new element.
        /// @param  copy       The value to copy and insert.
        /// @return A constant reference to the element added.
        //==========================================================================================
        Iterator    Insert(ConstIterator position, const T& copy)
        {ALIB_DCS
            Element* elem= recyclerType::Get();
            new (&elem->data) T(copy);
            position.element->addBefore( elem );

            return Iterator(elem);
        }

        //==========================================================================================
        /// Moves a value into this container before the given \p{position}.
        /// @param  position   The position to emplace the new element.
        /// @param  move       The value to move into this container.
        /// @return A constant reference to the element moved.
        //==========================================================================================
        Iterator    Insert(ConstIterator position, T&& move)
        {ALIB_DCS
            Element* elem= recyclerType::Get();
            new (&elem->data) T(std::move(move));
            position.element->addBefore( elem );

            return Iterator(elem);
        }

        //==========================================================================================
        /// Adds a new element at the end of the list.
        /// @param  copy       The value to copy and insert.
        /// @return A reference to the element added.
        //==========================================================================================
        T&          PushBack(const T& copy)
        {ALIB_DCS
            Element* elem= recyclerType::Get();

            new (&elem->data) T(copy);
            hook::pushEnd( elem );
            return elem->data;
        }

        //==========================================================================================
        /// Moves a value to the end of this list.
        /// @param  move       The value to move into this container.
        /// @return A reference to the element moved.
        //==========================================================================================
        T&          PushBack(T&& move)
        {ALIB_DCS
            Element* elem= recyclerType::Get();

            new (&elem->data) T(std::move(move));
            hook::pushEnd( elem );
            return elem->data;
        }

        //==========================================================================================
        /// Adds a new element at the start of the list.
        /// @param  copy       The value to copy and insert.
        /// @return A reference to the element added.
        //==========================================================================================
        T&          PushFront(const T& copy)
        {ALIB_DCS
            Element* elem= recyclerType::Get();
            new (&elem->data) T(copy);
            hook::pushFront( elem );
            return elem->data;
        }

        //==========================================================================================
        /// Moves a value to the start of this list.
        /// @param  move       The value to move into this container.
        /// @return A reference to the element moved.
        //==========================================================================================
        T&          PushFront(T&& move)
        {ALIB_DCS
            Element* elem= recyclerType::Get();

            new (&elem->data) T(std::move(move));
            hook::pushFront( elem );
            return elem->data;
        }

        //==========================================================================================
        /// Adds a new element before the given \p{position}.
        /// @tparam TArgs Types of variadic parameters given with parameter \p{args}.
        /// @param  position   The position to emplace the new element.
        /// @param  args       Variadic parameters to be forwarded to the constructor of type \p{T}.
        /// @return A constant reference to the element added.
        //==========================================================================================
        template<typename... TArgs>
        Iterator    Emplace(ConstIterator position, TArgs&&... args)
        {ALIB_DCS
            Element* elem= recyclerType::Get();
            new (&elem->data) T( std::forward<TArgs>(args)... );
            position.element->addBefore( elem );

            return Iterator(elem);
        }

        //==========================================================================================
        /// Adds a new element at the end of the list.
        /// @tparam TArgs   Types of variadic parameters given with parameter \p{args}.
        /// @param  args    Variadic parameters to be forwarded to the constructor of type \p{T}.
        /// @return A reference to the element added.
        //==========================================================================================
        template<typename... TArgs>
        T&          EmplaceBack(TArgs&&... args)
        {ALIB_DCS
            Element* elem= recyclerType::Get();

            new (&elem->data) T( std::forward<TArgs>(args)... );
            hook::pushEnd( elem );
            return elem->data;
        }

        //==========================================================================================
        /// Adds a new element at the end of the list.
        /// @tparam TArgs   Types of variadic parameters given with parameter \p{args}.
        /// @param  args    Variadic parameters to be forwarded to the constructor of type \p{T}.
        /// @return A reference to the element added.
        //==========================================================================================
        template<typename... TArgs>
        T&          EmplaceFront(TArgs&&... args)
        {ALIB_DCS
            Element* elem= recyclerType::Get();

            new (&elem->data) T( std::forward<TArgs>(args)... );
            hook::pushFront( elem );
            return elem->data;
        }

    //##############################################################################################
    /// @name Element Removal
    //##############################################################################################
        //==========================================================================================
        /// Removes an element at the given position.
        ///
        /// @param position A constant iterator pointing to the element to be removed.
        ///                 Mutable iterators are inherently converted with the invocation of this
        ///                 method.
        /// @return A mutable iterator pointing behind the removed element.
        ///         If \p{position} refers to the last element of the list, iterator #end() is
        ///         returned.
        //==========================================================================================
        Iterator    Erase(ConstIterator position)
        {ALIB_DCS
            ALIB_ASSERT_ERROR( !hook::isEmpty(), "MONOMEM/LIST",
                               "Erase requested on empty containers::List")
            ALIB_ASSERT_ERROR( position != end(), "MONOMEM/LIST",
                               "Iterator end() given with containers::List::Erase")


            Element*   next   = position.element->next();
            position.element->remove();
            recyclerType::Recycle( position.element );

            return Iterator(next);
        }

        //==========================================================================================
        /// Removes a range of elements defined by iterators \p{first} and \p{last}.
        ///
        /// @param begin  The start of the range to remove.
        /// @param end    The first element behind the range to remove.
        /// @return A mutable iterator referring to the given \p{last}.
        //==========================================================================================
        Iterator    Erase(ConstIterator begin, ConstIterator end )
        {ALIB_DCS
            ALIB_ASSERT_ERROR( !hook::isEmpty(),
                               "MONOMEM/LIST", "Erase requested on empty containers::List")
            if( begin == end )
                return Iterator(const_cast<Element*>( end.element ));

            begin.element->remove( end.element->prev() );
            recyclerType::RecycleList( begin.element, end.element );

            return Iterator( end.element );
        }

        //==========================================================================================
        /// Removes the first element.
        //==========================================================================================
        void        PopFront()
        {ALIB_DCS
            ALIB_ASSERT_ERROR( !IsEmpty(), "MONOMEM/LIST", "PopFront called on empty List instance." )
            recyclerType::Recycle( hook::popFront() );
        }

        //==========================================================================================
        /// Removes the last element.
        //==========================================================================================
        void        PopBack()
        {ALIB_DCS
            ALIB_ASSERT_ERROR( !IsEmpty(), "MONOMEM/LIST", "PopBack called on empty List instance." )
            recyclerType::Recycle( hook::popEnd() );
        }
}; // class List

} // namespace alib[::containers]

/// Type alias in namespace \b alib.
template<typename TAllocator, typename T, Recycling TRecycling = containers::Recycling::Private>
using  List       = containers::List<TAllocator, T, TRecycling>;

} // namespace [alib]

#endif // HPP_ALIB_MONOMEM_CONTAINERS_LIST

