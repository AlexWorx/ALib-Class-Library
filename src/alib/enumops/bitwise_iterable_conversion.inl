//==================================================================================================
/// \file
/// This header-file is part of the module \alib_enumops of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#if DOXYGEN
    ALIB_EXPORT namespace alib::enumops::bitwise {
#else
    ALIB_EXPORT namespace alib {
#endif

#include "ALib.Lang.CIFunctions.H"

/// Returns the bitwise enumeration element of \p TEnum from a given sequential enumeration.
///
/// Selected by the compiler only if \alib{enumops;BitwiseTraits} is specialized for
/// template enum type \p{TEnum} to inherit \c std::true_type.
///
/// \see The reverse function #ToSequentialEnumeration.
///
/// @tparam TEnum An enumeration type which is defined to be "bitwise".
/// @param number A sequentially enumerated number, for which the corresponding bitwise
///               enumeration element is requested.
///               \ref alib_enums_arithmetic_bitwise "bitwise enumeration".
/// @return Returns enum-element <c>1 << number</c>.
template<typename TEnum>
requires alib::enumops::IsBitwise<TEnum>
constexpr TEnum ToBitwiseEnumeration(typename std::underlying_type<TEnum>::type number )
{
    ALIB_ASSERT_ERROR( number >= 0
                       && (   !enumops::IterableTraits<TEnum>::value
                            || enumops::IterableTraits<TEnum>::End  > TEnum( 1 << number ) ),
                            "ENUMS",  "Negative enum element-number given."                     )
    return TEnum( 1 << number );
}


/// Returns the sequentially enumerated number derived from the given bitwise enumeration
/// value.
/// In other words, the positon of the most significant bit set in the underlying integral of the
/// given enum \p element is returned.<br>
/// In debug-compilations an \alib assertion is raised in case that the given value is not a
/// single enum element but a combination of bits.
///
/// Selected by the compiler only if \alib{enumops;BitwiseTraits} is specialized for
/// template enum type \p{TEnum} to inherit \c std::true_type.
///
/// \see The reverse function #ToBitwiseEnumeration.
/// @tparam TEnum  A bitwise defined enumeration type. Deduced by the compiler.
/// @param element An enumeration value.
/// @return The sequential number of an element of an enum type which is defined bitwise.
template<typename TEnum>
requires alib::enumops::IsBitwise<TEnum>
constexpr  typename std::underlying_type<TEnum>::type  ToSequentialEnumeration( TEnum element )
{
    ALIB_ASSERT_ERROR(                UnderlyingIntegral(element)  != 0, "ENUMS",
      "No bits set in given enum value"  )
    ALIB_ASSERT_ERROR( lang::BitCount(UnderlyingIntegral(element)) == 1, "ENUMS",
      "Multiple bits given with enum value"  )
    return static_cast<typename std::underlying_type<TEnum>::type>(
           lang::MSB( UnderlyingIntegral(element) ) - 1    );
}


} // namespace [alib]  (doxygen [alib::enumops::bitwise])

#include "ALib.Lang.CIMethods.H"


