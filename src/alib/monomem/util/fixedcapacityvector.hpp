/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_monomem of the \aliblong.
 *
 * \emoji :copyright: 2013-2023 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_MONOMEM_UTIL_FIXED_CAPACITY_VECTOR
#define HPP_ALIB_MONOMEM_UTIL_FIXED_CAPACITY_VECTOR

#if !defined (HPP_ALIB_TOOLS)
#   include "alib/lib/tools.hpp"
#endif

#if !defined (HPP_ALIB_MONOMEM_MONOMEM)
#   include "alib/monomem/monomem.hpp"
#endif

#if !defined (_GLIBCXX_ARRAY) && !defined(_ARRAY_)
#   include <array>
#endif

#if !defined (_GLIBCXX_QUEUE) && !defined(_QUEUE_)
#   include <queue>
#endif

namespace aworx { namespace lib { namespace monomem { namespace util {

/** ************************************************************************************************
 * This class fills the gap between standard types <c>std::array</c> and <c>std::vector</c> by
 * implementing a fixed size array, with a current filling size index.
 * With that methods #push_back and #pop_back could be implemented, which satisfies the constraints
 * given for example to underlying containers of type <c>std::priority_queue</c>.
 *
 * The rationale here is to use fixed size memory allocation or even stack allocation, in situations
 * where the maximum capacity needed at runtime is known upfront.
 *
 * Besides aforementioned methods \c push_back and \c pop_back, a few necessary other methods
 * of parent class <c>std::array</c> have been replaced, but are not explicitly mentioned with
 * this reference documentation. Among them are \c size(), \c end(), \c rend() or \c cend().
 *
 * \attention
 *   While this type inherits <c>std::array</c> publically, not all standard methods and type
 *   traits have been overwritten according to the ever more complicated C++ specification.
 *   Therefore, there is not only no guarantee given that this type behaves as it is supposed to in
 *   each and every complex situation (e.g template meta programing).
 *   However, the use in accordance with type \c std::priority_queue is certified.
 **************************************************************************************************/
template<typename T, std::size_t TSize>
class FixedCapacityVector : public std::array<T, TSize>
{
    protected:
        std::size_t fillSize= 0;       ///< The current fill

    public:
       /**
        * Increases the size of this vector by inserting given \p value at the end.<br>
        * In debug compilations, an \alib assertion is raised, if this fixed sized vector's
        * capacity is exceeded.
        * @param value The value to insert.
        */
        void push_back( const T& value )
        {
            ALIB_ASSERT_ERROR( fillSize < TSize, "MONOMEM/UT", "FixedCapacityVector overflow: ",
                                                              int(fillSize) )
            (*this)[fillSize++]= value;
        }

       /**
        * Decreases the size of this vector by destructing and removing the given \p value at the
        * current end.<br>
        * In debug compilations, an \alib assertion is raised, if no elements are left.
        */
        void pop_back()
        {
            ALIB_ASSERT_ERROR( fillSize > 0   , "MONOMEM/UT", "FixedCapacityVector underflow: ",
                                                              int(fillSize) )
            (*this)[--fillSize].~T();
        }

#if !defined(ALIB_DOX)
    ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
        ALIB_NODISCARD
        typename std::array<T, TSize>::size_type
        size() const
        { return fillSize; }

        ALIB_NODISCARD
        ALIB_CONSTEXPR17 typename std::array<T, TSize>::iterator            end()           noexcept
        { return std::array<T, TSize>::begin() + fillSize; }

        ALIB_NODISCARD
        ALIB_CONSTEXPR17 typename std::array<T, TSize>::const_iterator      end()   const   noexcept
        { return std::array<T, TSize>::begin() + fillSize; }

        ALIB_NODISCARD
        ALIB_CONSTEXPR17
        typename std::array<T, TSize>::const_iterator                       cend()          noexcept
        { return std::array<T, TSize>::cbegin() + fillSize; }

        ALIB_NODISCARD
        ALIB_CONSTEXPR17
        typename std::array<T, TSize>::const_iterator                       cend()  const   noexcept
        { return std::array<T, TSize>::cbegin() + fillSize; }
    ALIB_WARNINGS_RESTORE
#endif // !defined(ALIB_DOX)

}; // FixedCapacityVector

}}}// namespace aworx[::lib::monomem::util]


/// Type alias in namespace #aworx.
template<typename T, std::size_t TSize>
using     FixedCapacityVector  =   lib::monomem::util::FixedCapacityVector<T, TSize>;


/// Type alias in namespace #aworx which denotes a <c>std::priority_queue</c> using
/// a \alib{monomem::util,FixedCapacityVector} as its underlying container type.
template<typename T, std::size_t TSize, typename TCompare= std::less<T>>
using FixedSizePriorityQueue = std::priority_queue< T, FixedCapacityVector<T, TSize>, TCompare>;

} // namespace [aworx]



#endif // HPP_ALIB_MONOMEM_UTIL_RTTRALLOCATOR

