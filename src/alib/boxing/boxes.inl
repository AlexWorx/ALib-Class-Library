//==================================================================================================
/// \file
/// This header file is part of module \alib_boxing of the \aliblong.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_BOXING_BOXES
#define HPP_ALIB_BOXING_BOXES 1
#pragma once
#if !defined(HPP_ALIB_BOXING_BOXING)
#   error "ALib sources with ending '.inl' must not be included from outside."
#endif

#include "alib/lang/stdcontainerallocator.hpp"
#include <vector>

namespace alib {

namespace monomem { template<typename TAllocator> class TMonoAllocator; }

namespace boxing {
//==================================================================================================
/// A vector of objects of type \alib{boxing;Box}.
/// Specializes class \c std::vector<Box> (publicly) with a constructor and methods to add a
/// variable number of arbitrary values with one invocation.
///
/// If another \b %Boxes object or an array of boxes, or boxed versions of such, are added, this
/// container is "flattened", so that instead of the container, the boxes are added. Such flatting
/// is performed recursively.
///
/// \see
///   Chapter \ref alib_boxing_boxes "11. Variadic Function Arguments and Class Boxes"
///   of the Programmer's Manual of module \alib_boxing.
///
/// @tparam TAllocator The allocator to use with the <c>std::vector</c>, as prototyped with
///                    \alib{lang;Allocator}.
//==================================================================================================
template<typename TAllocator>
class TBoxes : public    std::vector<Box, lang::StdContainerAllocator<Box, TAllocator>>
{
  protected:
    /// The allocator type that \p{TAllocator} specifies.
    using vectorBase=   std::vector<Box, lang::StdContainerAllocator<Box, TAllocator>>;

  public:
    /// The allocator type that \p{TAllocator} specifies.
    using AllocatorType=        TAllocator;

    /// Defaulted default constructor, usable only with heap allocation.
    TBoxes()                                                                                      {}

    ///  Constructor.
    ///  @param pAllocator The allocator to use.
    TBoxes(TAllocator& pAllocator)
    : vectorBase(pAllocator)                                                                      {}


    //==============================================================================================
    /// Deleted copy constructor.
    //==============================================================================================
    TBoxes( TBoxes& ) = delete;

    //==============================================================================================
    /// Deleted copy assignment operator.
    /// @return Nothing (deleted).
    //==============================================================================================
    TBoxes& operator=( TBoxes& ) = delete;

    //==============================================================================================
    /// Empty method. Needed to allow adding empty variadic template parameter packs.
    /// @return A reference to this object.
    //==============================================================================================
    TBoxes& Add()
    {
        return *this;
    }

    //==============================================================================================
    /// Adds one box for each given variadic argument.
    ///
    /// @tparam TBoxables The types of the variadic arguments.
    /// @param  args      The variadic arguments. Each argument is converted into one box to be
    ///                   appended.
    /// @return A reference to this object.
    //==============================================================================================
    template <typename... TBoxables>
    TBoxes& Add( TBoxables&&... args )
    {
        Box boxes[sizeof...(args)]= { std::forward<TBoxables>( args )... };
        AddArray( boxes, sizeof...(args) );

        return *this;
    }

    //==============================================================================================
    /// Adds one boxt.
    ///
    /// @param  box The box to append.
    /// @return A reference to this object.
    //==============================================================================================
    TBoxes& Add(const Box& box )
    {
        AddArray( &box, 1 );

        return *this;
    }

    //==============================================================================================
    /// Adds an array of boxes.
    ///
    /// @tparam TExtend  The size of the given array of boxes.
    /// @param  boxArray The array of boxes.
    /// @return A reference to this object.
    //==============================================================================================
    template<size_t TExtend>
    TBoxes& Add( const Box(& boxArray)[TExtend] )
    {
        AddArray( boxArray, TExtend );
        return *this;
    }

    //==============================================================================================
    /// Adds all elements of the given other \b %TBoxes object.
    ///
    /// @param  boxes Another container of boxes to add.
    /// @return A reference to this object.
    //==============================================================================================
    template<typename TAllocatorArgs>
    TBoxes& Add( const TBoxes<TAllocatorArgs>& boxes )
    {
        AddArray( boxes.data(), static_cast<integer>(boxes.size()) );
        return *this;
    }

    //==============================================================================================
    /// Adds an array of boxes. Array elements of types \b %TBoxes itself and boxed arrays of
    /// class \b %Box are recursively "flattened".
    ///
    /// This method is internally used by all overloads of #Add.
    ///
    /// @param boxArray Pointer to the start of the array of boxes.
    /// @param length   The number of boxes contained in \p{boxArray}.
    //==============================================================================================
    void        AddArray( const Box* boxArray, integer length );


    //==============================================================================================
    /// Inline operator that simply aliases method #Add.
    ///
    /// @param src The value to be boxed and added.
    /// @return Returns a mutable reference to \c this.
    //==============================================================================================
    template <typename TBoxable>
    TBoxes& operator+=( TBoxable&& src )
    {
        return Add( src );
    }

    //==============================================================================================
    /// Inline operator that simply aliases method #Add.
    ///
    /// @param src The value to be boxed and added.
    /// @return Returns a mutable reference to \c this.
    //==============================================================================================
    template <typename TBoxable>
    TBoxes& operator<<( TBoxable&& src )
    {
        return Add( src );
    }

    //==============================================================================================
    /// Returns the quantity of elements stored in ths container.
    /// @return The element count.
    //==============================================================================================
    integer Size()                                                                             const
    {
        return static_cast<integer>( vectorBase::size() );
    }

    //==============================================================================================
    /// Invokes the corresponding parent's method \c std::vector::reserve.
    ///
    /// @param newCapacity The new, higher capacity of the vector.
    //==============================================================================================
    void    Reserve(integer newCapacity)
    {
        vectorBase::reserve( size_t( newCapacity ) );
    }

    //==============================================================================================
    /// Invokes \alib{boxing;Box::Call} with each box in this list.
    /// The result of the invocations of the box-functions is ignored.
    ///
    /// @tparam TFDecl  The function type to call.
    ///                 Needs to be specified with invocations of this method.
    /// @tparam TArgs   Types of the variadic arguments \p{args}.
    ///                 Do not need to be specified.
    /// @param  args    Variadic arguments forwarded to the functions.
    //==============================================================================================
    template <typename TFDecl,  typename... TArgs>
    void CallAll(TArgs&&... args)                                                              const
    {
        for( auto& box : *this )
            box.template Call<TFDecl>( std::forward<TArgs>(args)... );
    }

    //==============================================================================================
    /// Non-constant version of method #CallAll, which likewise chooses the non-constant version
    /// of \alib{boxing;Box::Call} and hence this method is usable with functions that only
    /// accept mutable (aka not constant) boxes.<br>
    /// Technically, the only difference between this method and \b CallAll is that the latter
    /// is declared \c const.
    ///
    /// @tparam TFDecl  The function type to call.
    ///                 Needs to be specified with invocations of this method.
    /// @tparam TArgs   Types of the variadic arguments \p{args} .
    /// @param  args    Variadic arguments forwarded to the functions.
    //==============================================================================================
    template <typename TFDecl,  typename... TArgs>
    void CallAll(TArgs&&... args)
    {
        for( auto& box : *this )
            box.template Call<TFDecl>( std::forward<TArgs>(args)... );
    }

    //==============================================================================================
    /// Same as \ref CallAll "CallAll<FClone>", but uses method \alib{boxing;Box::Clone},
    /// which internally invokes \alib{boxing;FClone}.
    ///
    /// Using this version leads to shorter code, because method \b Box::Clone is not inlined.
    /// Cloning is performed 'into' the allocator used by the \b %Boxes instance.
    ///
    /// \par Availability
    ///   This method is available only if the module \alib_monomem is included in the \alibdist.
    //==============================================================================================
    void CloneAll()
    {
        for( auto& box : *this )
            box.Clone( vectorBase::get_allocator().GetAllocator() );
    }

}; // class TBoxes


extern template ALIB_API  void TBoxes<lang::HeapAllocator>::AddArray( const Box* boxArray, integer length );
#if ALIB_MONOMEM
extern template ALIB_API  void TBoxes<MonoAllocator      >::AddArray( const Box* boxArray, integer length );
#endif
}


} // namespace [alib::boxing]

#endif // HPP_ALIB_BOXING_BOXES

