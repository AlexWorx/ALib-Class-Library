//==================================================================================================
/// \file
/// This header-file is part of module \alib_containers of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib {  namespace containers {

//==================================================================================================
/// This class fills the gap between standard types <c>std::array</c> and <c>std::vector</c> by
/// implementing a fixed size array, with a current filling size index.
/// With that methods #push_back and #pop_back could be implemented, which satisfies the constraints
/// given for example to underlying containers of type <c>std::priority_queue</c>.
///
/// The rationale here is to use fixed size memory allocation or even stack allocation, in situations
/// where the maximum capacity needed at runtime is known upfront.
///
/// Besides aforementioned methods \c push_back and \c pop_back, a few necessary other methods
/// of parent class <c>std::array</c> have been replaced, but are not explicitly mentioned with
/// this reference documentation. Among them are \c size(), \c end(), \c rend() or \c cend().
///
/// \attention
///   While this type inherits <c>std::array</c> publicly, not all standard methods and type
///   traits have been overwritten according to the ever more complicated C++ specification.
///   Therefore, there is not only no guarantee given that this type behaves as it is supposed to in
///   each and every complex situation (e.g template meta programing).
///   However, the use in accordance with type \c std::priority_queue is certified.
///
/// @tparam T     The type to store.
/// @tparam TSize The fixed sitze of the vector.
//==================================================================================================
template<typename T, std::size_t TSize>
class FixedCapacityVector : public std::array<T, TSize>
{
    protected:
        std::size_t fillSize= 0;       ///< The current fill

    public:
       /// Increases the size of this vector by inserting given \p value at the end.<br>
       /// In debug compilations, an \alib assertion is raised, if this fixed sized vector's
       /// capacity is exceeded.
       /// @param value The value to insert.
        void push_back( const T& value )
        {
            ALIB_ASSERT_ERROR( fillSize < TSize, "MONOMEM/UT",
                "FixedCapacityVector overflow: ", fillSize )
            (*this)[fillSize++]= value;
        }

       /// Decreases the size of this vector by destructing and removing the given \p value at the
       /// current end.<br>
       /// In debug compilations, an \alib assertion is raised, if no elements are left.
        void pop_back()
        {
            ALIB_ASSERT_ERROR( fillSize > 0   , "MONOMEM/UT",
                "FixedCapacityVector underflow: ", fillSize )
            (*this)[--fillSize].~T();
        }

#if !DOXYGEN
        [[nodiscard]]
        typename std::array<T, TSize>::size_type
        size() const
        { return fillSize; }

        [[nodiscard]]
        constexpr typename std::array<T, TSize>::iterator                   end()           noexcept
        { return std::array<T, TSize>::begin() + fillSize; }

        [[nodiscard]]
        constexpr typename std::array<T, TSize>::const_iterator             end()   const   noexcept
        { return std::array<T, TSize>::begin() + fillSize; }

        [[nodiscard]]
        constexpr
        typename std::array<T, TSize>::const_iterator                       cend()          noexcept
        { return std::array<T, TSize>::cbegin() + fillSize; }

        [[nodiscard]]
        constexpr
        typename std::array<T, TSize>::const_iterator                       cend()  const   noexcept
        { return std::array<T, TSize>::cbegin() + fillSize; }
#endif // !DOXYGEN

}; // FixedCapacityVector

/// Type alias which denotes a <c>std::priority_queue</c> using
/// a \alib{containers;FixedCapacityVector} as its underlying container type.
template<typename T, std::size_t TSize, typename TCompare= std::less<T>>
using FixedSizePriorityQueue = std::priority_queue< T, FixedCapacityVector<T, TSize>, TCompare>;

} // namespace alib[::containers]


/// Type alias in namespace \b alib.
template<typename T, std::size_t TSize>
using     FixedCapacityVector  =   containers::FixedCapacityVector<T, TSize>;


/// Type alias in namespace \b alib, which denotes a <c>std::priority_queue</c> using
/// a \alib{containers;FixedCapacityVector} as its underlying container type.
template<typename T, std::size_t TSize, typename TCompare= std::less<T>>
using FixedSizePriorityQueue = std::priority_queue< T, FixedCapacityVector<T, TSize>, TCompare>;

} // namespace [alib]

