//==================================================================================================
/// \file
/// This header file is part of the \aliblong. It does not belong to an \alibmod and is
/// included in any \alibdist.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_LANG_STDCONTAINERALLOCATOR
#define HPP_ALIB_LANG_STDCONTAINERALLOCATOR
#pragma once
#include "alib/lang/rttrallocator.hpp"

namespace alib::lang {
namespace detail {

//==================================================================================================
/// Base struct for types \alib{lang;StdContainerAllocator} and
/// \alib{lang;StdContainerAllocatorRecycling} containing entities to satisfy requirements for
/// being a <c>std::allocator</c>.
///
/// @tparam T          The type of objects to be allocated.
/// @tparam TAllocator The allocator type, as prototyped with \alib{lang;Allocator}.
//==================================================================================================
template<typename T, typename TAllocator>
struct StdContainerAllocatorBase
{
    // #############################################################################################
    // ### Type definitions
    // #############################################################################################
    using  size_type      =  size_t        ;  ///< Type definition as required by C++ library standards.
    using  difference_type=  ptrdiff_t     ;  ///< Type definition as required by C++ library standards.
    using  value_type     =  T             ;  ///< Type definition as required by C++ library standards.
    using  is_always_equal= std::false_type;  ///< Type definition as required by C++ library standards.
    using  reference      =  T&            ;  ///< Type definition as required by C++ library standards.
    using  const_reference=  const T&      ;  ///< Type definition as required by C++ library standards.


    /// The allocator type that \p{TAllocator} specifies.
    using AllocatorType=        TAllocator;

    // #############################################################################################
    // ### Other required members
    // #############################################################################################
    #if ALIB_CPP_STANDARD == 17 || DOXYGEN
        /// The possible allocation size.
        /// Note: Available only if <b>ALIB_CPP_STANDARD == 17, as it was deprecated afterwards.</b>.
        /// @return Returns the largest supported allocation size.
        size_t          max_size()                                                   const  noexcept
        {
            return size_t(-1) / 2;
        }
    #endif

}; // struct StdContainerAllocatorBase

} // namespace alib::lang[::detail]


//==================================================================================================
/// This struct is an implementation of C++ standard library type <c>std::allocator</c>.
/// It is to be used with container types provided by the C++ standard library in combination
/// with \alib{lang;Allocator;ALib Allocators}.
///
/// With the inclusion of module \alib_monomem in the \alibdist, this allocator is suitable to use
/// cases of \ref alib_contmono_intro_strictweak "strict or weak monotonic allocation".
///
/// ### Shortcuts for Specific ALib Allocators ###
/// The following alias type definitions exist for this type, each addressing a specific allocator:
/// - \alib{SCAMono}, and
/// - \alib{SCAPool}.
///
/// Along with this, shortcuts to the container types are likewise provided with:
/// - \alib{StdVectorMono}, \alib{StdVectorPool},
/// - \alib{StdListMono}, \alib{StdListPool}, and
/// - \alib{StdDequeMono}, \alib{StdDequePool}.
///
/// \anchor alib_ns_monomem_scamono_reset
/// ### Resetting A Container ###
/// While the dedicated container types provided with module \alib_containers offer a method
/// named \b Reset (see for example \alib{containers;HashTable::Reset}), the C++ standard
/// containers do not. This is a challenge because their internal memory will be invalid
/// with a reset. For example, if allocator type \alib{MonoAllocator} was used with calss
/// <c>std::vector</c>, there is no interface method that makes the vector "forget" its internal
/// data array. Its method <c>shrink_to_fit</c> by its specification is not forced to shrink
/// anything or even dispose the data if the size was \c 0 when called.
/// This is implementation-dependent.
///
/// The way out is as simple as radical: The container is just to be destructed and reconstructed
/// "in place". This can be done using a <em>C++ placement-new</em>. The following
/// code snippet demonstrates this:
/// \snippet "ut_stdcontainers.cpp"     DOX_MONOMEM_SCAMONO_PLACEMENT_NEW
///
/// \see
///   - An alternative version that \ref alib_contmono_intro_recycling "recycles nodes" (and also
///     bucket arrays in case of <c>std::unordered_set</c> and <c>std::unordered_map</c>), is
///     provided with sibling type \alib{lang;StdContainerAllocatorRecycling}.
///   - Type definitions \alib{SCAMono} and \alib{SCAPool} given with module \alib_monomem.
///   - Further shortcuts for standard container types, given with \alib{StdVectorMono},
///     \alib{StdVectorPool}, \alib{StdListMono}, \alib{StdListPool}, \alib{StdDequeMono}, and
///     \alib{StdDequePool}.
///
/// @tparam T          The type of objects to be allocated.
/// @tparam TAllocator The allocator type, as prototyped with \alib{lang;Allocator}.
//==================================================================================================
template<typename T, typename TAllocator>
struct StdContainerAllocator : detail::StdContainerAllocatorBase<T, TAllocator>
                             , AllocatorMember<TAllocator>
{
    /// The allocator type that \p{TAllocator} specifies.
    using allocBase=   AllocatorMember<TAllocator>;

    /// Defaulted copy constructor
    constexpr StdContainerAllocator( const StdContainerAllocator&  )              noexcept =default;

    /// Defaulted move constructor
    constexpr StdContainerAllocator(       StdContainerAllocator&& )              noexcept =default;

    /// Copy constructor using an instance of different template type.
    /// @tparam TSibling  The originating allocator's type (<b>StdContainerAllocator<X></b>).
    /// @param  origin    The originating allocator of type \p{TSibling} .
    template<typename TSibling, typename TEnableIf=
          ATMP_T_IF(void, ATMP_ISOF(TAllocator&, typename TSibling::allocator))>
    StdContainerAllocator(const TSibling& origin)
    : allocBase(origin)
    {}

    /// Copy constructor using an instance of different template type.
    /// @tparam TSibling  The originating allocator's type (<b>StdContainerAllocator<X></b>).
    /// @param  origin    The originating allocator of type \p{TSibling} .
    template<typename TSibling>
    StdContainerAllocator( const StdContainerAllocator<TSibling,TAllocator>& origin)
    : allocBase(origin)
    {}

    /// Constructor for the initial allocator instantiation.
    /// @param pAllocator   The allocator.
    StdContainerAllocator( TAllocator& pAllocator )
    : allocBase(pAllocator)
    {}

    ///  Parameterless constructor used with heap allocation.
    StdContainerAllocator()                                                               = default;


    // #############################################################################################
    // ### Comparison
    // #############################################################################################
    /// Comparison operator.
    /// @tparam U    The allocation type of the other allocator.
    /// @param  rhs  The right hand side allocator.
    /// @return \c true if this and \p{rhs} use the same #allocator, \c false otherwise.
    template< typename U >
    bool operator==( const StdContainerAllocator<U,TAllocator>& rhs )               const   noexcept
    {
        return &allocBase::getAllocator() == &rhs.getAllocator();
    }

    /// Comparison operator.
    /// @tparam U    The allocation type of the other allocator.
    /// @param  rhs  The right hand side allocator.
    /// @return \c false if this and \p{rhs} use the same #allocator, \c true otherwise.
    template< typename U >
    bool operator!=( const StdContainerAllocator<U,TAllocator>& rhs )               const   noexcept
    {
        return &allocBase::GetAllocator() != &rhs.GetAllocator();
    }


    // #############################################################################################
    // ### Allocate/de-allocate
    // #############################################################################################
    //==============================================================================================
    /// Passes the allocation request to field #allocator.
    ///
    /// @param n  The number of requested objects to allocate storage for.
    /// @return Pointer to the start of the array of \p{n} objects of type \p{T}.
    //==============================================================================================
    [[nodiscard]]
    T*      allocate( size_t n, const void* = nullptr )
    { return allocBase::AI().template AllocArray<T>( n ); }

    //==============================================================================================
    /// Frees the given array of objects.
    /// @param p Pointer to the previously allocated memory.
    /// @param n The number of objects allocated.
    //==============================================================================================
    void deallocate( T* p, std::size_t n )
    { allocBase::AI().FreeArray( p, n ); }

}; // struct StdContainerAllocator

//==================================================================================================
/// Implementation of <c>std::allocator</c> to be used with container types provided by the
/// C++ standard library.
///
/// As the C+++ library's specification does not include details of the container class's
/// implementation, this allocator uses type \alib{lang;RTTRAllocator} which detects
/// node types automatically.
///
/// \attention
///   It cannot be guaranteed that a certain, uncommon implementation of the C++ library
///   allocates memory in a way that this allocator truly recycles objects, and - even if the authors
///   of \alib are optimistic hat this type works with any implementation - it is recommended
///   to use the alternative container types found in this \alibmod_nl.
///
/// \see
///   For background information about monotonic memory allocation, recycling, and the issues
///   of C++ standard container types, see the \ref alib_mods_contmono "Programmer's Manual" of
///   \alibmods_nl \b Containers and \b Monomem.
///
/// @tparam T          The type of objects to be allocated.
/// @tparam TAllocator The allocator type, as prototyped with \alib{lang;Allocator}.
//==================================================================================================
template<typename T, typename TAllocator>
struct StdContainerAllocatorRecycling : public detail::StdContainerAllocatorBase<T,TAllocator>
{
    // #############################################################################################
    // ### Fields
    // #############################################################################################
    /// The only member of this allocator type used to perform all duties.
    RTTRAllocator<TAllocator>& recycler;

    // #############################################################################################
    // ### Construction/Destruction
    // #############################################################################################
    /// Defaulted copy constructor
    constexpr StdContainerAllocatorRecycling( const StdContainerAllocatorRecycling& ) noexcept =default;

    /// Defaulted move constructor
    constexpr StdContainerAllocatorRecycling(       StdContainerAllocatorRecycling&& ) noexcept =default;

    /// Copy constructor using an instance of different template type.
    /// @tparam TSibling            The originating allocator's type (<b>StdContainerAllocatorRecycling<X></b>).
    /// @param  StdContainerAllocatorRecycling  The originating allocator of type \p{TSibling} .
    template<typename TSibling>
    StdContainerAllocatorRecycling( TSibling& StdContainerAllocatorRecycling)
    : recycler(StdContainerAllocatorRecycling.recycler)                                           {}

    /// Constructor for the initial allocator instantiation.
    /// @param  pRecycler The recycler to for allocations and de-allocations.
    constexpr StdContainerAllocatorRecycling( RTTRAllocator<TAllocator>& pRecycler)
    : recycler(pRecycler)
    {}

    // #############################################################################################
    // ### Comparison
    // #############################################################################################
    /// Comparison operator.
    /// @tparam U    The allocation type of the other allocator.
    /// @param  rhs  The right hand side allocator.
    /// @return \c true if this and \p{rhs} use the same #recycler, \c false otherwise.
    template< typename U >
    bool operator==( const StdContainerAllocatorRecycling<U,TAllocator>& rhs )      const   noexcept
    { return recycler == rhs.recycler; }

    /// Comparison operator.
    /// @tparam U    The allocation type of the other allocator.
    /// @param  rhs  The right hand side allocator.
    /// @return \c false if this and \p{rhs} use the same #recycler, \c true otherwise.
    template< typename U >
    bool operator!=( const StdContainerAllocatorRecycling<U,TAllocator>& rhs )      const   noexcept
    { return recycler != rhs.recycler; }

    // #############################################################################################
    // ### Allocate/de-allocate
    // #############################################################################################
    //==============================================================================================
    /// Implementation of <c>std::allocator</c> interface.
    /// Dependent on whether a single or multiple objects are requested, this method invokes
    /// either \alib{lang;RTTRAllocator::Get} or
    /// \alib{lang;RTTRAllocator::AllocUnrelated} on field #recycler.
    ///
    /// @param n  The number of requested objects to allocate storage for.
    /// @return Pointer to the start of the array of \p{n} objects of type \p{T}.
    //==============================================================================================
    [[nodiscard]]
    T*      allocate( size_t n, const void* = nullptr )
    {
        if( n == 1 )
            return reinterpret_cast<T*>( recycler.Get( sizeof(T),
                                         alignof(T)
                              ALIB_DBG(, typeid(T) )                                  ) );

        return reinterpret_cast<T*>(     recycler.AllocUnrelated( sizeof(T) * n,
                                         alignof(T)
                              ALIB_DBG(, typeid(T) )                                  ) );
    }

    //==============================================================================================
    /// Implementation of <c>std::allocator</c> interface.
    /// Dependent on whether a single or multiple objects are de-allocated (parameter \p{n}), this
    /// method invokes either \alib{lang;RTTRAllocator::Recycle} or
    /// \alib{lang;RTTRAllocator::RecycleChunk} on field #recycler.
    ///
    /// @param p  Pointer to the object to deallocate.
    /// @param n  The number of objects to de-allocate.
    //==============================================================================================
    void deallocate( T* p, std::size_t n )
    {
        // deallocate node
        if( n == 1 ) recycler.Recycle(       p, sizeof(T)    , alignof(T)  ALIB_DBG(, typeid(T) ) );
        else         recycler.RecycleChunk ( p, sizeof(T) * n              ALIB_DBG(, typeid(T) ) );
    }
}; // struct StdContainerAllocatorRecycling

} // namespace [alib::lang]


#endif // HPP_ALIB_LANG_STDCONTAINERALLOCATOR

