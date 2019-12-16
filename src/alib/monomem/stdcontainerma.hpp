/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_monomem of the \aliblong.
 *
 * \emoji :copyright: 2013-2019 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_MONOMEM_STDCONTAINERMA
#define HPP_ALIB_MONOMEM_STDCONTAINERMA

#if !defined(HPP_ALIB_MONOMEM_UTIL_RTTRALLOCATOR)
#   include "alib/monomem/util/rttrallocator.hpp"
#endif

#if !defined(HPP_ALIB_FS_LISTS_FORWARDLIST)
#   include "alib/lib/fs_lists/forwardlist.hpp"
#endif

namespace aworx { namespace lib { namespace monomem {


namespace detail {

/** ************************************************************************************************
 * Base struct for types \alib{monomem,StdContMA} and
 * \alib{monomem,StdContMARecycling} containing entities to satisfy requirements for being a
 * <c>std::allocator</c>.
 *
 * @tparam T The type of objects to be allocated.
 **************************************************************************************************/
template<typename T>
struct StdContainerMABase
{
    // #############################################################################################
    // ### Type definitions
    // #############################################################################################
    using  size_type      =  size_t        ;  ///< Type definition as required by C++ library standards.
    using  difference_type=  ptrdiff_t     ;  ///< Type definition as required by C++ library standards.
    using  value_type     =  T             ;  ///< Type definition as required by C++ library standards.
    using  is_always_equal= std::false_type;  ///< Type definition as required by C++ library standards.

    // deprecated types
    #if ALIB_CPPVER <= 14 || defined(ALIB_DOX)
        using  pointer        =  T*       ;  ///< Type definition as required by C++ library standards.
                                             ///< Available if <b>ALIB_CPPVER <= 14</b>.
        using  const_pointer  =  const T* ;  ///< Type definition as required by C++ library standards.
                                             ///< Available if <b>ALIB_CPPVER <= 14</b>.
        using  reference      =  T&       ;  ///< Type definition as required by C++ library standards.
                                             ///< Available if <b>ALIB_CPPVER <= 14</b>.
        using  const_reference=  const T& ;  ///< Type definition as required by C++ library standards.
                                             ///< Available if <b>ALIB_CPPVER <= 14</b>.
    #endif


    // #############################################################################################
    // ### Other required members
    // #############################################################################################
    #if ALIB_CPPVER <= 17 || defined(ALIB_DOX)
        /**
         * The possible allocation size.
         * Note: Available only if <b>ALIB_CPPVER <= 14</b>.
         * @return returns the largest supported allocation size.
         */
        size_t          max_size()                                                   const  noexcept
        {
            return static_cast<size_t>(-1) / 2;
        }
    #endif


    #if ALIB_CPPVER <= 14 || defined(ALIB_DOX)
        /**
         * Constructs an object of type T in allocated uninitialized storage pointed to by p, using
         * placement-new.
         *
         * Note: Available only if <b>ALIB_CPPVER <= 14</b>.
         *
         * @tparam U     The type to construct. Deduced by the compiler from parameter \p{p}.
         * @tparam Args  Variadic argument types.
         * @param  p     Pointer to the object to construct.
         * @param  args  The arguments forward to the constructor of \p{p}.
         */
        template< typename U, typename... Args >
        void            construct( U* p, Args&&... args )
        {
            ::new(p) U(std::forward<Args>(args)...);
        }

        /**
         * Constructs an object of type T in allocated uninitialized storage pointed to by p, using
         * placement-new.
         *
         * Note: Available only if <b>ALIB_CPPVER <= 14</b>.
         *
         * @tparam U    The type to destroy. Deduced by the compiler from parameter \p{p}.
         * @param  p    Pointer to the object to destruct.
         */
        template< typename U >
        void            destroy( U* p )
        {
            p->~U();
        }

        /**
         * Returns the address of \p{x} even in presence of overloaded <c>operator&</c>.
         * @param  x The object to get the address for.
         * @return The address of \p{x} even in presence of overloaded <c>operator&</c>.
         */
        pointer         address( reference x )                                        const noexcept
        {
            return &x;
        }

        /**
         * Returns the address of \p{x} even in presence of overloaded <c>operator&</c>.
         * @param  x The object to get the address for.
         * @return The address of \p{x} even in presence of overloaded <c>operator&</c>.
         */
        const_pointer   address( const_reference x )                                  const noexcept
        {
            return &x;
        }
    #endif
}; // struct StdContainerMABase

}// namespace aworx::lib::monomem[::detail]


/** ************************************************************************************************
 * Implementation of <c>std::allocator</c> to be used with container types provided by the
 * C++ standard library.
 *
 * This allocator is suitable to use cases of
 * \ref alib_monomem_intro_strictweak "strict or weak monotonic allocation".
 * An alternative version that recycles nodes (and for example bucket arrays in case of
 * <c>std::unordered_set</c> and <c>std::unordered_map</c>), is provided with sibling type
 * \alib{monomem,StdContMARecycling}.
 *
 * A further alternative is given with type \alib{monomem,StdContMAOptional}, which accepts a
 * pointer instead of a reference to an allocator and allows this pointer to be \c nullptr, which
 * switches to dynamic allocation.
 *
 *
 * \anchor alib_namespace_monomem_StdContMA_reset
 * ### Resetting A Container ###
 * While the dedicated container types provided with this \alibmod_nl offer a method named \b Reset
 * (see for example \alib{monomem,HashTable::Reset}), the C++ \b std containers of-course do not.
 * This is a challenge, because their internal memory will be invalid with a reset of the
 * monotonic allocator used, and for example with class <c>std::vector</c> there is no interface
 * method that makes them "forget" their internal data array. Its method <c>shrink_to_fit</c>
 * by its specification is not forced to shrink anything or even dispose the data if the size
 * was \c 0 when called. This is implementation dependent.
 *
 * The way out is as simple as radical: The container is just to be reconstructed without
 * prior destruction. This can be done using a <em>C++ placement new</em>. The following
 * code snippet demonstrates this:
 * \snippet "ut_alib_monomem.cpp"     DOX_ALIB_MONOMEM_STDCONTMA_PLACEMENT_NEW
 *
 *
 * @tparam T The type of objects to be allocated.
 **************************************************************************************************/
template<typename T>
struct StdContMA : public detail::StdContainerMABase<T>
{
    // #############################################################################################
    // ### Type definition
    // #############################################################################################

    #if ALIB_CPPVER <= 14 || defined(ALIB_DOX)
       /**
        * Type definition as required by C++ library standards up to Version 14.
        * @tparam U  The originating allocation type.
        */
        template<typename U>
        struct rebind
        {
            /** The 'casted' allocator type. */
            using other= StdContMA<U>;
        };
    #endif

    // #############################################################################################
    // ### Fields
    // #############################################################################################
    /** The allocator to use. */
    MonoAllocator&  allocator;

    #if ALIB_DEBUG
        /** If \c true warnings about de-allocation of objects are suppressed. this should be set
         *  prior to destructing a container that uses this allocator. <br>
         *  Available only in debug builds.                                               */
        bool        dbgDeallocationWarning;
    #endif

    // #############################################################################################
    // ### Construction/Destruction
    // #############################################################################################
    /** Default constructor declared, but not defined.
     *
     * \attention
     *   Due to the way standard container types use template struct <c>std::allocator</c>) that
     *   this object implements, the default constructor has to be declared. Due to the fact
     *   that field #allocator is of reference type, no declaration can be made.<br>
     *   With the construction of a container type that uses this allocator, it has to be assured
     *   that a valid instance is passed. Otherwise the compiler will complain about not
     *   finding this constructor's defintion.
     */
    inline
    StdContMA();


    /** Defaulted copy constructor */
    constexpr StdContMA( const StdContMA&  )                                      noexcept =default;

    /** Defaulted move constructor */
    constexpr StdContMA(       StdContMA&& )                                      noexcept =default;

    /**
     * Copy constructor using an instance of different template type.
     * @tparam TSibling  The originating allocator's type (<b>StdContMA<X></b>).
     * @param  origin    The originating allocator of type \p{TSibling} .
     */
    template<typename TSibling>
    StdContMA( TSibling& origin)
    : allocator(origin.allocator)
ALIB_DBG( ,dbgDeallocationWarning(origin.dbgDeallocationWarning) )
    {}

#if defined(ALIB_DOX)
    /**
     * Constructor for the initial allocator instantiation.
     *
     * Parameter \p{dbgDisableDeallocationWarning}, which defaults to \c true might be given
     * after a code is tested to be stict in respect to allocation.
     * (Note: unfortunately, due to the design of <c>std::allocator</c> and its use, this flag can
     * not be changed once a container is constructed. This is why it has to be decided upfront
     * if a warning is to be raised or not).
     *
     * @param pAllocator             The recycler for allocations and de-allocations.
     * @param dbgDeallocationWarning As described with this method. Available only in debug builds.
     */
    constexpr StdContMA( MonoAllocator& pAllocator,
                         bool           dbgDeallocationWarning= true );
#else
    constexpr StdContMA( MonoAllocator& pAllocator ALIB_DBG(, bool dbgDAWarning= false) )
    : allocator(pAllocator)
ALIB_DBG( ,dbgDeallocationWarning(dbgDAWarning) )
    {}
#endif // defined(ALIB_DOX)


    // #############################################################################################
    // ### Comparision
    // #############################################################################################
    /**
     * Comparison operator.
     * @tparam U    The allocation type of the other allocator.
     * @param  rhs  The right hand side allocator.
     * @return \c true if this and \p{rhs} use the same #allocator, \c false otherwise.
     */
    template< typename U >
    bool operator==( const StdContMA<U>& rhs )                                      const   noexcept
    {
        return &allocator == &rhs.allocator;
    }

    /**
     * Comparison operator.
     * @tparam U    The allocation type of the other allocator.
     * @param  rhs  The right hand side allocator.
     * @return \c false if this and \p{rhs} use the same #allocator, \c true otherwise.
     */
    template< typename U >
    bool operator!=( const StdContMA<U>& rhs )                                      const   noexcept
    {
        return &allocator != &rhs.allocator;
    }


    // #############################################################################################
    // ### Allocate/de-allocate
    // #############################################################################################
    /** ********************************************************************************************
     * If #allocator is set, passes the allocation request to #allocator.
     * Otherwise invokes <c>std::malloc</c>.
     *
     * @param n  The number of requested objects to allocate storage for.
     * @return Pointer to the first byte of a memory block suitably aligned and sufficient to hold
     *         an array of n objects of type \p{T} .
     **********************************************************************************************/
    ALIB_NODISCARD
    T*      allocate( size_t n, const void* = nullptr )
    {
        MONOMEM_VERBOSE( "STD_CONTAINER", "Allocating object of type {!Q<>}. ", typeid(T) )
        return reinterpret_cast<T*>( allocator.AllocArray<T>( n ) );
    }

    /** ********************************************************************************************
     * If #allocator is not set, invokes <c>std::free(p)</c>. Otherwise does nothing.
     * @param p Pointer to the previously allocated memory.
     * @param n The number of objects allocated.
     **********************************************************************************************/
    void deallocate( T* p, std::size_t n )
    {
        (void) p;
        (void) n;

        ALIB_ASSERT_WARNING( !dbgDeallocationWarning || n == 1,
            "De-allocation with strict allocator. If this is container destruction, set flag \n"
            "StdContMA::dbgDeallocationWarning to disable this warning. " )
        ALIB_DBG( dbgDeallocationWarning = false; )
    }

}; // struct StdContMA

/** ************************************************************************************************
 * Implementation of <c>std::allocator</c> to be used with container types provided by the
 * C++ standard library.
 *
 * This allocator only optionally uses monotonic allocation - only if an allocator was given
 * with construction. This has a very small performance impact, hence the non-optional sibling,
 * \alib{monomem,StdContMA} is to be used with types that always use monotonic memory.
 *
 * @tparam T The type of objects to be allocated.
 **************************************************************************************************/
template<typename T>
struct StdContMAOptional : public detail::StdContainerMABase<T>
{
    // #############################################################################################
    // ### Type definition
    // #############################################################################################

    #if ALIB_CPPVER <= 14 || defined(ALIB_DOX)
       /**
        * Type definition as required by C++ library standards up to Version 14.
        * @tparam U  The originating allocation type.
        */
        template<typename U>
        struct rebind
        {
            /** The 'casted' allocator type. */
            using other= StdContMAOptional<U>;
        };
    #endif

    // #############################################################################################
    // ### Fields
    // #############################################################################################
    /** The allocator to use. */
    MonoAllocator*  allocator;

    #if ALIB_DEBUG
        /** If \c true warnings about de-allocation of objects are suppressed. this should be set
         *  prior to destructing a container that uses this allocator. <br>
         *  Available only in debug builds.                                               */
        bool        dbgDeallocationWarning;
    #endif

    // #############################################################################################
    // ### Construction/Destruction
    // #############################################################################################
    /** Default constructor */
    constexpr StdContMAOptional()
    : allocator(nullptr)
    {}


    /** Defaulted copy constructor */
    constexpr StdContMAOptional( const StdContMAOptional&  )                noexcept =default;

    /** Defaulted move constructor */
    constexpr StdContMAOptional(       StdContMAOptional&& )                noexcept =default;

    /**
     * Copy constructor using an instance of different template type.
     * @tparam TSibling  The originating allocator's type (<b>StdContMAOptional<X></b>).
     * @param  origin    The originating allocator of type \p{TSibling} .
     */
    template<typename TSibling>
    StdContMAOptional( TSibling& origin)
    : allocator(origin.allocator)
ALIB_DBG( ,dbgDeallocationWarning(origin.dbgDeallocationWarning) )
    {}

#if defined(ALIB_DOX)
    /**
     * Constructor for the initial allocator instantiation.
     *
     * Parameter \p{dbgDisableDeallocationWarning}, which defaults to \c true might be given
     * after a code is tested to be stict in respect to allocation.
     * (Note: unfortunately, due to the design of <c>std::allocator</c> and its use, this flag can
     * not be changed once a container is constructed. This is why it has to be decided upfront
     * if a warning is to be raised or not).
     *
     * @param pAllocator             The recycler for allocations and de-allocations.
     * @param dbgDeallocationWarning As described with this method. Available only in debug builds.
     */
    constexpr StdContMAOptional( MonoAllocator& pAllocator,
                                    bool           dbgDeallocationWarning= true );
#else
    constexpr StdContMAOptional( MonoAllocator* pAllocator ALIB_DBG(, bool dbgDAWarning= false) )
    : allocator(pAllocator)
ALIB_DBG( ,dbgDeallocationWarning(dbgDAWarning) )
    {}
#endif // defined(ALIB_DOX)


    // #############################################################################################
    // ### Comparision
    // #############################################################################################
    /**
     * Comparison operator.
     * @tparam U    The allocation type of the other allocator.
     * @param  rhs  The right hand side allocator.
     * @return \c true if this and \p{rhs} use the same #allocator, \c false otherwise.
     */
    template< typename U >
    bool operator==( const StdContMAOptional<U>& rhs )                           const   noexcept
    {
        return allocator == rhs.allocator;
    }

    /**
     * Comparison operator.
     * @tparam U    The allocation type of the other allocator.
     * @param  rhs  The right hand side allocator.
     * @return \c false if this and \p{rhs} use the same #allocator, \c true otherwise.
     */
    template< typename U >
    bool operator!=( const StdContMAOptional<U>& rhs )                           const   noexcept
    {
        return allocator != rhs.allocator;
    }


    // #############################################################################################
    // ### Allocate/de-allocate
    // #############################################################################################
    /** ********************************************************************************************
     * If #allocator is set, passes the allocation request to #allocator.
     * Otherwise invokes <c>std::malloc</c>.
     *
     * @param n  The number of requested objects to allocate storage for.
     * @return Pointer to the first byte of a memory block suitably aligned and sufficient to hold
     *         an array of n objects of type \p{T} .
     **********************************************************************************************/
    ALIB_NODISCARD
    T*      allocate( size_t n, const void* = nullptr )
    {
        MONOMEM_VERBOSE( "STD_CONTAINER", "Allocating object of type {!Q<>}. "
                         "Optional allocator set: ",
                         typeid(T), (allocator == nullptr ? "No" : "Yes") )
        if( allocator == nullptr )
        {
            return reinterpret_cast<T*>( std::malloc( sizeof(T) * n ) );
        }

        return reinterpret_cast<T*>( allocator->AllocArray<T>( n ) );
    }

    /** ********************************************************************************************
     * If #allocator is not set, invokes <c>std::free(p)</c>. Otherwise does nothing.
     * @param p Pointer to the previously allocated memory.
     * @param n The number of objects allocated.
     **********************************************************************************************/
    void deallocate( T* p, std::size_t n )
    {
        (void) n;
        if( allocator == nullptr )
        {
            std::free( p );
            return;
        }
        ALIB_ASSERT_WARNING( !dbgDeallocationWarning || n == 1,
            "De-allocation with strict allocator. If this is container destruction, set flag \n"
            "StdContMAOptional::dbgDeallocationWarning to disable this warning. " )
        ALIB_DBG( dbgDeallocationWarning = false; )
    }

}; // struct StdContMAOptional

/** ************************************************************************************************
 * Implementation of <c>std::allocator</c> to be used with container types provided by the
 * C++ standard library.
 *
 * As the C+++ library's specification does not include details of the container class's
 * implementation, this allocator uses type \alib{monomem,util::RTTRAllocator} which detects
 * node types automatically.
 *
 * \attention
 *   It can not be guaranteed that a certain, uncommon implementation of the C++ library
 *   allocates memory in a way that this allocator truly recycles objects, and - even if the authors
 *   of \alib are optimistic hat this type works with any implementation - it is recommended
 *   to use the alternative container types found in this \alibmod_nl.
 *
 * \see
 *   For background information about monotonic memory allocation, recycling and the issues
 *   of C++ standard container types, see the \ref alib_mod_monomem "Programmer's Manual" of
 *   this \alibmod_nl.
 *
 * @tparam T The type of objects to be allocated.
 **************************************************************************************************/
template<typename T>
struct StdContMARecycling : public detail::StdContainerMABase<T>
{
    // #############################################################################################
    // ### Type definition
    // #############################################################################################

    #if ALIB_CPPVER <= 14 || defined(ALIB_DOX)
       /**
        * Type definition as required by C++ library standards up to Version 14.
        * @tparam U  The originating allocation type.
        */
        template<typename U>
        struct rebind
        {
            /** The 'casted' allocator type. */
            using other= StdContMARecycling<U>;
        };
    #endif

    // #############################################################################################
    // ### Fields
    // #############################################################################################
    /** The only member of this allocator type used to perform all duties. */
    RTTRAllocator& recycler;

    // #############################################################################################
    // ### Construction/Destruction
    // #############################################################################################
    /** Defaulted copy constructor */
    constexpr StdContMARecycling( const StdContMARecycling&  )          noexcept =default;

    /** Defaulted move constructor */
    constexpr StdContMARecycling(       StdContMARecycling&& )          noexcept =default;

    /**
     * Copy constructor using an instance of different template type.
     * @tparam TSibling            The originating allocator's type (<b>StdContMARecycling<X></b>).
     * @param  StdContMARecycling  The originating allocator of type \p{TSibling} .
     */
    template<typename TSibling>
    StdContMARecycling( TSibling& StdContMARecycling)
    : recycler(StdContMARecycling.recycler)
    {}

    /**
     * Constructor for the initial allocator instantiation.
     * @param  pRecycler The recycler to for allocations and de-allocations.
     */
    constexpr StdContMARecycling( RTTRAllocator& pRecycler)
    : recycler(pRecycler)
    {}

    // #############################################################################################
    // ### Comparision
    // #############################################################################################
    /**
     * Comparison operator.
     * @tparam U    The allocation type of the other allocator.
     * @param  rhs  The right hand side allocator.
     * @return \c true if this and \p{rhs} use the same #recycler, \c false otherwise.
     */
    template< typename U >
    bool operator==( const StdContMARecycling<U>& rhs )                        const   noexcept
    {
        return recycler == rhs.recycler;
    }

    /**
     * Comparison operator.
     * @tparam U    The allocation type of the other allocator.
     * @param  rhs  The right hand side allocator.
     * @return \c false if this and \p{rhs} use the same #recycler, \c true otherwise.
     */
    template< typename U >
    bool operator!=( const StdContMARecycling<U>& rhs )                        const   noexcept
    {
        return recycler != rhs.recycler;
    }

    // #############################################################################################
    // ### Allocate/de-allocate
    // #############################################################################################
    /** ********************************************************************************************
     * Implementation of <c>std::allocator</c> interface.
     * Dependent on whether a single or multiple objects are requested, this method invokes
     * either \alib{monomem::util,RTTRAllocator::Get} or
     * \alib{monomem::util,RTTRAllocator::AllocUnrelated} on field #recycler.
     *
     * @param n  The number of requested objects to allocate storage for.
     * @return Pointer to the first byte of a memory block suitably aligned and sufficient to
     *         hold an array of n objects of type \p{T} .
     **********************************************************************************************/
    ALIB_NODISCARD
    T*      allocate( size_t n, const void* = nullptr )
    {
        if( n == 1 )
            return reinterpret_cast<T*>( recycler.Get( sizeof(T)    , alignof(T)
                                            ALIB_DBG(, typeid(T) )                ) );

        return reinterpret_cast<T*>( recycler.AllocUnrelated( sizeof(T) * n, alignof(T)
                                                   ALIB_DBG(, typeid(T) )               ) );
    }

    /** ********************************************************************************************
     * Implementation of <c>std::allocator</c> interface.
     * Dependent on whether a single or multiple objects are de-allocated (parameter \p{n}), this
     * method invokes either \alib{monomem::util,RTTRAllocator::Recycle} or
     * \alib{monomem::util,RTTRAllocator::RecycleChunk} on field #recycler.
     *
     * @param p  Pointer to the object to deallocate.
     * @param n  The number of objects to de-allocate.
     **********************************************************************************************/
    void deallocate( T* p, std::size_t n )
    {
        // deallocate node
        if( n == 1 )
            recycler.Recycle(       p, sizeof(T)    , alignof(T)   ALIB_DBG(, typeid(T)   ) );
        else
            recycler.RecycleChunk ( p, sizeof(T) * n, alignof(T)   ALIB_DBG(, typeid(T) ) );
    }


}; // struct StdContMARecycling

}}// namespace aworx[::lib::monomem]

/// Type alias in namespace #aworx.
template<typename T>
using     StdContMA           =   lib::monomem::StdContMA<T>;

/// Type alias in namespace #aworx.
template<typename T>
using     StdContMAOptional   =   lib::monomem::StdContMAOptional<T>;

/// Type alias in namespace #aworx.
template<typename T>
using     StdContMARecycling  =   lib::monomem::StdContMARecycling<T>;

} // namespace [aworx]



#endif // HPP_ALIB_MONOMEM_STDCONTAINERMA

