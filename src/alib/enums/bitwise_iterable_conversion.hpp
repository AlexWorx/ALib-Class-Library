//==================================================================================================
/// \file
/// This header file is part of module \alib_enums of the \aliblong.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_ENUMS_BW_IT_CONVERSION
#define HPP_ALIB_ENUMS_BW_IT_CONVERSION 1
#pragma once
#if !defined(DOXYGEN)
#   include "alib/alib.hpp"
#endif

ALIB_ASSERT_MODULE(ENUMS)

#include "alib/enums/iterable.hpp"
#include "alib/lang/bits.hpp"

#if DOXYGEN
    namespace alib::enums::bitwise {
#else
    namespace alib {
#endif

#include "alib/lang/callerinfo_functions.hpp"
#if DOXYGEN
/// Returns the bitwise enumeration element of \p TEnum from a given sequential enumeration.
///
/// Selected by the compiler only if \alib{enums;T_EnumIsBitwise} is specialized for
/// template enum type \p{TEnum} to inherit \c std::true_type.
///
/// \see The reverse function #ToSequentialEnumeration.
///
/// @tparam TEnum An enumeration type which is defined to be "bitwise".
/// @param number A sequentially enumerated number, for which the corresponding bitwise
///               enumeration element is requested.
///               \ref alib_enums_arithmetic_bitwise "bitwise enumeration".
/// @return Returns <c>1 << number</c>.
template<typename TEnum>
constexpr inline
TEnum ToBitwiseEnumeration(typename std::underlying_type<TEnum>::type number );

#else

template<typename TEnum>
constexpr
ATMP_T_IF(TEnum, alib::enums::T_EnumIsBitwise<TEnum>::value)
ToBitwiseEnumeration(typename std::underlying_type<TEnum>::type number )
{
    #if !defined (HPP_ALIB_ENUMS_ITERABLE)
        ALIB_ASSERT_ERROR( number >= 0
                           && (   !enums::T_EnumIsIterable<TEnum>::value
                                || enums::T_EnumIsIterable<TEnum>::End  > TEnum( 1 << number ) ),
                                "ENUMS",  "Number out of bounds."                                      )
    #endif
    return TEnum( 1 << number );
}
#endif

#if DOXYGEN

/// Returns the sequentially enumerated number derived from the given bitwise enumeration
/// value.
/// In other words, the positon of the most significant bit set in the underlying integral of the
/// given enum \p element is returned.<br>
/// In debug-compilations an \alib assertion is raised in case that the given value is not a
/// single enum element but a combination of bits.
///
/// Selected by the compiler only if \alib{enums;T_EnumIsBitwise} is specialized for
/// template enum type \p{TEnum} to inherit \c std::true_type.
///
/// \see The reverse function #ToBitwiseEnumeration.
/// @tparam TEnum  A bitwise defined enumeration type. Deduced by the compiler.
/// @param element An enumeration value.
/// @return The sequential number of an element of an enum type which is defined bitwise.
template<typename TEnum>
static constexpr inline
typename std::underlying_type<TEnum>::type
ToSequentialEnumeration( TEnum element );
#else
template<typename TEnum>
constexpr
ATMP_T_IF(typename std::underlying_type<TEnum>::type, alib::enums::T_EnumIsBitwise<TEnum>::value)
ToSequentialEnumeration( TEnum element )
{
    ALIB_ASSERT_ERROR(                UnderlyingIntegral(element)  != 0, "ENUMS",
      "No bits set in given enum value"  )
    ALIB_ASSERT_ERROR( lang::BitCount(UnderlyingIntegral(element)) == 1, "ENUMS",
      "Multiple bits given with enum value"  )
    return static_cast<typename std::underlying_type<TEnum>::type>(
           lang::MSB( UnderlyingIntegral(element) ) - 1    );
}
#endif


} // namespace [alib]  (doxygen [alib::enums::bitwise])

#include "alib/lang/callerinfo_methods.hpp"

#endif // HPP_ALIB_ENUMS_BW_IT_CONVERSION

