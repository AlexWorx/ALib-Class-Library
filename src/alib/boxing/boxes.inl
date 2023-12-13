/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_boxing of the \aliblong.
 *
 * \emoji :copyright: 2013-2023 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_BOXING_BOXES
#define HPP_ALIB_BOXING_BOXES 1

#if !defined(HPP_ALIB_BOXING_BOXING)
#   error "ALib sources with ending '.inl' must not be included from outside."
#endif


#if !defined (_GLIBCXX_VECTOR) && !defined(_VECTOR_)
#   include <vector>
#endif

#if ALIB_MONOMEM
namespace aworx { namespace lib { namespace monomem { class MonoAllocator; }}}

namespace aworx { namespace lib { namespace boxing { namespace detail {

//
/**
 * This detail namespace function just calls \alib{monomem,MonoAllocator::Alloc} with the given
 * parameters. The use of this function avoids the need to include headers of module
 * \alib_monomem, by headers of module \alib_boxing_nl.
 * @param allocator  The allocator to use.
 * @param size       The requested size.
 * @param alignment  The requested alignment
 * @return The allocated piece of memory.
 */
ALIB_API  char*   monoAlloc( monomem::MonoAllocator& allocator, size_t size, size_t alignment);


/** ************************************************************************************************
 * Implementation of <c>std::allocator</c> used with class \alib{boxing,Boxes}.
 * This allocator takes an optional pointer to type \alib{monomem,MonoAllocator}.
 * If this pointer is \e nulled, then standard allocation is used.
 *
 * \note The internals of this type implementing the interface required by <c>std::allocator</c>
 *       are not documented.
 *
 * @tparam T The type of objects to be allocated.
 **************************************************************************************************/
template<typename T>
struct BoxesAllocator
{
#if !defined(ALIB_DOX)
    using  size_type      =  size_t        ;
    using  difference_type=  ptrdiff_t     ;
    using  value_type     =  T             ;
    using  is_always_equal= std::false_type;

    // deprecated types
    #if ALIB_CPPVER <= 14 || defined(ALIB_DOX)
        using  pointer        =  T*       ;
        using  const_pointer  =  const T* ;
        using  reference      =  T&       ;
        using  const_reference=  const T& ;
    #endif

    #if ALIB_CPPVER <= 17 || defined(ALIB_DOX)
        size_t          max_size()     const  noexcept  { return static_cast<size_t>(-1) / 2; }
    #endif

    #if ALIB_CPPVER <= 14 || defined(ALIB_DOX)
        template< typename U, typename... Args >
        void            construct( U* p, Args&&... args ) {::new(p) U(std::forward<Args>(args)...);}
        template< typename U >
        void            destroy( U* p )                              { p->~U();   }
        pointer         address( reference x )        const noexcept { return &x; }
        const_pointer   address( const_reference x )  const noexcept { return &x; }
    #endif

    monomem::MonoAllocator*  allocator;
    constexpr            BoxesAllocator()       : allocator(nullptr)   {}
    constexpr            BoxesAllocator( const BoxesAllocator&  )                 noexcept =default;
    constexpr            BoxesAllocator(       BoxesAllocator&& )                 noexcept =default;
    template<typename U> BoxesAllocator( U& origin              ) : allocator(origin.allocator)   {}
    constexpr            BoxesAllocator( monomem::MonoAllocator* pAllocator)
                                                                  : allocator(pAllocator)         {}

    template<typename T2>
    bool operator==( const aworx::lib::boxing::detail::BoxesAllocator<T2>& rhs )    noexcept
    { return allocator == rhs.allocator; }

    template<typename T2>
    bool operator!=( const aworx::lib::boxing::detail::BoxesAllocator<T2>& rhs )    noexcept
    { return (allocator != rhs.allocator); }

    ALIB_NODISCARD
    T*   allocate(   size_t n, const void* = nullptr )
    {
        if( allocator == nullptr )
            return reinterpret_cast<T*>( std::malloc( sizeof(T) * n ) );

        return reinterpret_cast<T*>( detail::monoAlloc( *allocator,
                                                       sizeof(T[1]) * n, alignof(T[]) )  );
    }

    void deallocate( T* p    , std::size_t           )
    {
        if( allocator == nullptr )
            std::free( p );
    }

#endif
}; // struct BoxesAllocator

}}}} // namespace [aworx::lib::boxing::detail]


#endif

namespace aworx { namespace lib { namespace boxing {
/** ************************************************************************************************
 * A vector of objects of type \alib{boxing,Box}.
 * Specializes class \c std::vector<Box> (publicly) with a constructor and methods to add a
 * variable amount of arbitrary values with one invocation.
 *
 * If another \b %Boxes object or an array of boxes, or boxed versions of such, are added, this
 * container is "flattened", so that instead of the container, the boxes are added. Such flatting
 * is performed recursively.
 *
 * \see
 *   Chapter \ref alib_boxing_boxes "11. Variadic Function Arguments and Class Boxes"
 *   of the Programmer's Manual of module \alib_boxing.
 **************************************************************************************************/
class Boxes
  #if ALIB_MONOMEM
        : public std::vector<Box, detail::BoxesAllocator<Box>>
  #else
        : public std::vector<Box>
  #endif
{
    public:
        /** ****************************************************************************************
         * Defaulted default constructor.
         ******************************************************************************************/
        Boxes()                                                                           = default;

        /** ****************************************************************************************
         * Deleted copy constructor.
         ******************************************************************************************/
        Boxes( Boxes& ) = delete;

        /** ****************************************************************************************
         * Deleted copy assignment operator.
         * @return Nothing (deleted).
         ******************************************************************************************/
        Boxes& operator=( Boxes& ) = delete;

        #if ALIB_MONOMEM
            /** ************************************************************************************
             * Constructor accepting a monotonic allocator.<br>
             * Note: This constructor is available only if module \alib_monomem is included in the
             * \alibdist.
             *
             * @param monoAllocator The allocator to use with the underlying <c>std::vector</c>.
             **************************************************************************************/
            Boxes( monomem::MonoAllocator*   monoAllocator )
            : vector<Box, detail::BoxesAllocator<Box>>( detail::BoxesAllocator<Box>(monoAllocator) )
            {}
        #endif

        /** ****************************************************************************************
         * Empty method. Needed to allow adding empty variadic template parameter packs.
         * @return A reference to this object.
         ******************************************************************************************/
        Boxes& Add()
        {
            return *this;
        }

        /** ****************************************************************************************
         * Adds one box for each given variadic argument.
         *
         * @tparam TBoxables The types of the variadic arguments.
         * @param  args      The variadic arguments. Each argument is converted into one box to be
         *                   appended.
         * @return A reference to this object.
         ******************************************************************************************/
        template <typename... TBoxables>
        Boxes& Add( TBoxables&&... args )
        {
            Box boxes[sizeof...(args)]= { std::forward<TBoxables>( args )... };
            AddArray( boxes, sizeof...(args) );

            return *this;
        }

        /** ****************************************************************************************
         * Adds one boxt.
         *
         * @param  box The box to append.
         * @return A reference to this object.
         ******************************************************************************************/
        Boxes& Add(const Box& box )
        {
            AddArray( &box, 1 );

            return *this;
        }

        /** ****************************************************************************************
         * Adds an array of boxes.
         *
         * @tparam TExtend  The size of the given array of boxes.
         * @param  boxArray The array of boxes.
         * @return A reference to this object.
         ******************************************************************************************/
        template<size_t TExtend>
        Boxes& Add( const Box(& boxArray)[TExtend] )
        {
            AddArray( boxArray, TExtend );
            return *this;
        }

        /** ****************************************************************************************
         * Adds all elements of the given other \b %Boxes object.
         *
         * @param  boxes Another container of boxes to add.
         * @return A reference to this object.
         ******************************************************************************************/
        Boxes& Add( const Boxes& boxes )
        {
            AddArray( boxes.data(), static_cast<integer>(boxes.size()) );
            return *this;
        }

        /** ****************************************************************************************
         * Adds an array of boxes. Array elements of types \b %Boxes itself and boxed arrays of
         * class \b %Box are recursively "flattened".
         *
         * This method is internally used by all overloads of #Add.
         *
         * @param boxArray Pointer to the start of the array of boxes.
         * @param length   The number of boxes contained in \p{boxArray}.
         ******************************************************************************************/
        ALIB_API  void AddArray( const Box* boxArray, integer length );


        /** ****************************************************************************************
         * Inline operator that simply aliases method #Add.
         *
         * @param src The value to be boxed and added.
         * @return Returns a mutable reference to \c this.
         *****************************************************************************************/
        template <typename TBoxable>
        Boxes& operator+=( TBoxable&& src )
        {
            return Add( src );
        }

        /** ****************************************************************************************
         * Inline operator that simply aliases method #Add.
         *
         * @param src The value to be boxed and added.
         * @return Returns a mutable reference to \c this.
         *****************************************************************************************/
        template <typename TBoxable>
        Boxes& operator<<( TBoxable&& src )
        {
            return Add( src );
        }

        /** ****************************************************************************************
         * Returns the quantity of elements stored in ths container.
         * @return The element count.
         *****************************************************************************************/
        integer Size()                                                                         const
        {
            return static_cast<integer>( size() );
        }

        /** ****************************************************************************************
         * Invokes the corresponding parent's method \c std::vector::reserve.
         *
         * @param newCapacity The new, higher capacity of the vector.
         ******************************************************************************************/
        void    Reserve(integer newCapacity)
        {
            reserve( static_cast<size_t>( newCapacity ) );
        }

        /** ****************************************************************************************
         * Invokes \alib{boxing,Box::Call} with each box in this list.
         * The result of the invocations of the box-functions is ignored.
         *
         * @tparam TFDecl  The function type to call.
         *                 Needs to be specified with invocations of this method.
         * @tparam TArgs   Types of the variadic arguments \p{args}.
         *                 Do not need to be specified.
         * @param  args    Variadic arguments forwarded to the functions.
         ******************************************************************************************/
        template <typename TFDecl,  typename... TArgs>
        void CallAll(TArgs&&... args)                                                          const
        {
            for( auto& box : *this )
                box.Call<TFDecl>( std::forward<TArgs>(args)... );
        }

        /** ****************************************************************************************
         * Non-constant version of method #CallAll, which likewise chooses the non-constant version
         * of \alib{boxing,Box::Call} and hence this method is usable with functions that only
         * accept mutable (aka not constant) boxes.<br>
         * Technically, the only difference between this method and \b CallAll is that the latter
         * is declared \c const.
         *
         * @tparam TFDecl  The function type to call.
         *                 Needs to be specified with invocations of this method.
         * @tparam TArgs   Types of the variadic arguments \p{args} .
         * @param  args    Variadic arguments forwarded to the functions.
         ******************************************************************************************/
        template <typename TFDecl,  typename... TArgs>
        void CallAll(TArgs&&... args)
        {
            for( auto& box : *this )
                box.Call<TFDecl>( std::forward<TArgs>(args)... );
        }

    #if ALIB_MONOMEM
        /** ****************************************************************************************
         * Same as \ref CallAll "CallAll<FClone>", but uses method \alib{boxing,Box::Clone},
         * which internally invokes \alib{boxing,FClone}.
         *
         * Using this version leads to shorter code, because method \b Box::Clone is not inlined.
         *
         * #### Availability ####
         * This method is available only if module \alib_monomem is included in the \alibdist.
         *
         * @param memory  A monotonic allocator used for storing cloned data.
         ******************************************************************************************/
        ALIB_API
        void CloneAll(monomem::MonoAllocator& memory);
    #endif

}; // class Boxes




}}} // namespace [aworx::lib::boxing]

#endif // HPP_ALIB_BOXING_BOXES

