//==================================================================================================
/// \file
/// This header file is part of module \alib_enums of the \aliblong.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_ENUMS_BITWISE
#define HPP_ALIB_ENUMS_BITWISE 1
#pragma once
#if !defined(DOXYGEN)
#   include "alib/alib.hpp"
#endif

ALIB_ASSERT_MODULE(ENUMS)

#include "alib/enums/arithmetical.hpp"
#include "alib/enums/underlyingintegral.hpp"
#include "alib/lang/bits.hpp"

namespace alib {

/// This is the reference documentation of sub-namespace \c enums of the \aliblink, which
/// holds types of library module \alib_enums_nl.
///
/// Extensive documentation for this module is provided with
/// \ref alib_mod_enums "ALib Module Enums - Programmer's Manual".
///
/// \attention
///   All operators are declared in the global namespace, other than this
///   namespace documentation indicates!
namespace enums {

// #################################################################################################
// struct T_EnumIsBitwise
// #################################################################################################
#if DOXYGEN
//==================================================================================================
/// Simple TMP struct that inherits <c>std::false_type</c> by default. If specialized for an
/// enumeration type \p{TEnum} to inherit <c>std::true_type</c>, the following operators  set of
/// \alib operators become applicable to elements of \p{TEnum}:
///
/// - \alib{enums::bitwise;operator&}
/// - \alib{enums::bitwise;operator&=}
/// - \alib{enums::bitwise;operator|}
/// - \alib{enums::bitwise;operator|=}
/// - \alib{enums::bitwise;operator^}
/// - \alib{enums::bitwise;operator^=}
/// - \alib{enums::bitwise;operator~}
/// - \alib{enums::bitwise;operator+}  (Alias for \alib{enums::bitwise;operator|})
/// - \alib{enums::bitwise;operator-}  (Alias for a combination of operators
///   \alib{enums::bitwise;operator&} and \alib{enums::bitwise;operator~})
/// - \alib{enums::bitwise;operator+=} (An alias for \alib{enums::bitwise;operator|=})
/// - \alib{enums::bitwise;operator-=} (Removes given bit(s) )
///
/// \attention
///   Likewise with the operators introduced with TMP struct \alib{enums;T_EnumIsArithmetical},
///   this documentation "fakes" the operators into namespace <c>alib::enums</c>,
///   while in fact they are defined in the global namespace!<br>
///   See \ref alib_enums_arithmetic_standard "corresponding note" in the Programmer's Manual
///   for details.
///
/// <b>Restrictions</b><br>
/// For technical reasons, this concept is not applicable to enum types that are defined as
/// \c private or \c protected inner types of structs or classes.
///
/// \see
///   - Macro \ref ALIB_ENUMS_MAKE_BITWISE, which specializes this TMP struct for a given
///     enumeration type.
///   - For details and a source code sample see chapter \ref alib_enums_arithmetic_bitwise
///     of the Programmer's Manual of module \alib_enums.
///
/// @tparam TEnum      The enum type to enable arithmetical operators for.
/// @tparam TEnableIf  This parameter has a default expressions and <b>must not</b> be provided
///                    with specializations of this struct.
///                    It is used to ensure that template parameter \p{TEnum} is an enumeration type.
///
///  \I{#############################################################################################}
/// # Restrictions #
/// For technical reasons, this concept is not applicable to enum types that are defined as
/// \c private or \c protected inner types of structs or classes.
///
/// # Reference Documentation #
///
/// @tparam TEnum      The enum type to enable bitwise operators for.
/// @tparam TEnableIf  This parameter has a default expressions and <b>must not</b> be provided
///                    with specializations of this struct.
///                    It is used to ensure that template parameter \p{TEnum} is an enumeration type.
//==================================================================================================
template<typename TEnum, typename TEnableIf>
struct T_EnumIsBitwise : public std::false_type {};
#else
template<typename TEnum,
         typename TEnableIf= typename std::enable_if<std::is_enum<TEnum>::value>::type>
struct T_EnumIsBitwise : public std::false_type {};
#endif

} // namespace alib[::enums]

/// Type alias in namespace \b alib.
template<typename TEnum>
using     T_EnumIsBitwise=       enums::T_EnumIsBitwise<TEnum>;

} // namespace [alib]

// #################################################################################################
// Helper Macros
// #################################################################################################
#define  ALIB_ENUMS_MAKE_BITWISE(TEnum)                                                            \
namespace alib::enums {                                                                            \
template<>                                                                                         \
struct T_EnumIsBitwise<TEnum> : public std::true_type {}; }

// #################################################################################################
// Bitwise Operators
// #################################################################################################
// For documentation, all operators and enum related template functions are faked into namespace
// alib::enums
#if DOXYGEN
namespace alib {  namespace enums {
/// Operators available to elements of enumerations if \alib{enums;T_EnumIsBitwise} is
/// specialized.
///
/// \note
///   This namespace exits only in the documentation to collect the operators.
///   When parsed by a C++ compiler, <b>the operators reside in the global namespace</b> and
///   functions \alib{enums::bitwise;HasBits}, \alib{enums::bitwise;CountElements},
///   \alib{enums::bitwise;ToBitwiseEnumeration} and \alib{enums::bitwise;ToSequentialEnumeration}
///   reside in namespace \ref alib.
///
/// As required, when parsed by a C++ compiler, the operators reside in the global namespace.
namespace bitwise {

#endif


/// Bitwise \b and operator usable with scoped enum types.<br>
/// Selected by the compiler only if \alib{enums;T_EnumIsBitwise} is specialized for
/// template enum type \p{TEnum} to inherit \c std::true_type.
///
/// @tparam TEnum      Enumeration type.
/// @param  lhs        First operand.
/// @param  rhs        Second operand.
/// @return The result of a bitwise and operation of the underlying enum values.
template<typename TEnum>
constexpr
#if DOXYGEN
TEnum
#else
typename  std::enable_if<alib::enums::T_EnumIsBitwise<TEnum>::value, TEnum>::type
#endif
operator&  (TEnum  lhs, TEnum rhs) noexcept
{
    using TBits= typename std::underlying_type<TEnum>::type;
    return TEnum(TBits(lhs) & TBits(rhs));
}

/// Bitwise assignment operator usable with scoped enum types.<br>
/// Selected by the compiler only if \alib{enums;T_EnumIsBitwise} is specialized for
/// template enum type \p{TEnum} to inherit \c std::true_type.
///
/// @tparam TEnum       Enumeration type.
/// @param[in,out]  lhs Reference to the first operand. Receives the result.
/// @param  rhs         Second operand.
/// @return The new value of \p{lhs} which is set to <c>lhs & rhs</c>.
template<typename TEnum>
constexpr
#if DOXYGEN
TEnum
#else
typename  std::enable_if<alib::enums::T_EnumIsBitwise<TEnum>::value, TEnum>::type
#endif
operator&=  (TEnum&  lhs, TEnum rhs) noexcept
{
    using TBits= typename std::underlying_type<TEnum>::type;
    return lhs= TEnum( TBits(lhs) & TBits(rhs) );
}

/// Bitwise \b or operator usable with scoped enum types.<br>
/// Selected by the compiler only if \alib{enums;T_EnumIsBitwise} is specialized for
/// template enum type \p{TEnum} to inherit \c std::true_type.
///
/// @tparam TEnum      Enumeration type.
/// @param  lhs        First operand.
/// @param  rhs        Second operand.
/// @return The result of a bitwise or operation of the underlying enum values.
template<typename TEnum>
constexpr
#if DOXYGEN
TEnum
#else
typename  std::enable_if<alib::enums::T_EnumIsBitwise<TEnum>::value, TEnum>::type
#endif
operator|  (TEnum  lhs, TEnum rhs) noexcept
{
    using TBits= typename std::underlying_type<TEnum>::type;
    return TEnum(TBits(lhs) | TBits(rhs));
}

/// Bitwise <b>or assignment</b> operator usable with scoped enum types.<br>
/// Selected by the compiler only if \alib{enums;T_EnumIsBitwise} is specialized for
/// template enum type \p{TEnum} to inherit \c std::true_type.
///
/// @tparam TEnum       Enumeration type.
/// @param[in,out]  lhs Reference to the first operand. Receives the result.
/// @param  rhs         Second operand.
/// @return The new value of \p{lhs} which is set to <c>lhs | rhs</c>.
template<typename TEnum>
constexpr
#if DOXYGEN
bool
#else
typename  std::enable_if<alib::enums::T_EnumIsBitwise<TEnum>::value, TEnum>::type
#endif
operator|=  (TEnum&  lhs, TEnum rhs) noexcept
{
    using TBits= typename std::underlying_type<TEnum>::type;
    return lhs= TEnum( TBits(lhs) | TBits(rhs) );
}

/// Bitwise \b xor operator usable with scoped enum types.<br>
/// Selected by the compiler only if \alib{enums;T_EnumIsBitwise} is specialized for
/// template enum type \p{TEnum} to inherit \c std::true_type.
///
/// @tparam TEnum      Enumeration type.
/// @param  lhs        First operand.
/// @param  rhs        Second operand.
/// @return The result of a bitwise xor operation of the underlying enum values.
template<typename TEnum>
constexpr
#if DOXYGEN
TEnum
#else
typename  std::enable_if<alib::enums::T_EnumIsBitwise<TEnum>::value, TEnum>::type
#endif
operator^  (TEnum  lhs, TEnum rhs) noexcept
{
    using TBits= typename std::underlying_type<TEnum>::type;
    return TEnum(TBits(lhs) ^ TBits(rhs));
}

/// Bitwise <b>xor assignment</b> operator usable with scoped enum types.<br>
/// Selected by the compiler only if \alib{enums;T_EnumIsBitwise} is specialized for
/// template enum type \p{TEnum} to inherit \c std::true_type.
///
/// @tparam TEnum       Enumeration type.
/// @param[in,out]  lhs Reference to the first operand. Receives the result.
/// @param  rhs         Second operand.
/// @return The new value of \p{lhs} which is set to <c>lhs ^ rhs</c>.
template<typename TEnum>
constexpr
#if DOXYGEN
TEnum
#else
typename  std::enable_if<alib::enums::T_EnumIsBitwise<TEnum>::value, TEnum>::type
#endif
operator^=  (TEnum&  lhs, TEnum rhs) noexcept
{
    using TBits= typename std::underlying_type<TEnum>::type;
    return lhs= TEnum( TBits(lhs) ^ TBits(rhs) );
}

/// Bitwise \b not operator usable with scoped enum types.<br>
/// Selected by the compiler only if \alib{enums;T_EnumIsBitwise} is specialized for
/// template enum type \p{TEnum} to inherit \c std::true_type.
///
/// \note To remove one or more bits from a scoped enum value, operator <b>&=</b> with this operator
///       applied to \p{op} can be used.
///       A shortcut to this is given with \ref operator-=.
///
/// @tparam TEnum      Enumeration type.
/// @param  op         The operand to be negated.
/// @return The result of a bitwise negation of \p{op}.
template<typename TEnum>
constexpr
#if DOXYGEN
TEnum
#else
typename  std::enable_if<alib::enums::T_EnumIsBitwise<TEnum>::value, TEnum>::type
#endif
operator~ (TEnum  op) noexcept
{
    using TBits= typename std::underlying_type<TEnum>::type;
    return TEnum( ~ TBits(op) );
}


/// Alias to bitwise \b or operator usable with scoped enum types.<br>
/// Selected by the compiler only if \alib{enums;T_EnumIsBitwise} is specialized for
/// template enum type \p{TEnum} to inherit \c std::true_type and if \alib{enums;T_EnumIsArithmetical}
/// is \b not specialized to inherit  \c std::true_type. The latter is to avoid ambiguities in
/// situations where an enum is both, arithmetical and bitwise.
///
/// @tparam TEnum      Enumeration type.
/// @param  lhs        First operand.
/// @param  rhs        Second operand.
/// @return The result of a bitwise or operation of the underlying enum values.
template<typename TEnum>
constexpr
#if DOXYGEN
TEnum
#else
typename  std::enable_if<    alib::enums::T_EnumIsBitwise     <TEnum>::value
                         && !alib::enums::T_EnumIsArithmetical<TEnum>::value, TEnum>::type
#endif
operator+  (TEnum  lhs, TEnum rhs) noexcept
{
    using TBits= typename std::underlying_type<TEnum>::type;
    return TEnum(TBits(lhs) | TBits(rhs));
}

/// Alias for bitwise <b>or assignment</b> operator usable with scoped enum types.<br>
/// Selected by the compiler only if \alib{enums;T_EnumIsBitwise} is specialized for
/// template enum type \p{TEnum} to inherit \c std::true_type  and if \alib{enums;T_EnumIsArithmetical}
/// is \b not specialized to inherit \c std::true_type. The latter is to avoid ambiguities in
/// situations where an enum is both, arithmetical and bitwise.
///
/// @tparam TEnum       Enumeration type.
/// @param[in,out]  lhs Reference to the first operand. Receives the result.
/// @param  rhs         Second operand.
/// @return The new value of \p{lhs} which is set to <c>lhs | rhs</c>.
template<typename TEnum>
constexpr
#if DOXYGEN
TEnum
#else
typename  std::enable_if<    alib::enums::T_EnumIsBitwise     <TEnum>::value
                         && !alib::enums::T_EnumIsArithmetical<TEnum>::value, TEnum>::type
#endif
operator+=  (TEnum&  lhs, TEnum rhs) noexcept
{
    using TBits= typename std::underlying_type<TEnum>::type;
    return lhs= TEnum( TBits(lhs) | TBits(rhs) );
}

/// Removes bit(s) found in \p{rhs} from \p{lhs} an returns result. This is a shortcut to:
///
///      lhs & !rhs
///
/// Selected by the compiler only if \alib{enums;T_EnumIsBitwise} is specialized for
/// template enum type \p{TEnum} to inherit \c std::true_type  and if \alib{enums;T_EnumIsArithmetical}
/// is \b not specialized to inherit  \c std::true_type. The latter is to avoid ambiguities in
/// situations where an enum is both, arithmetical and bitwise.
///
/// @tparam TEnum       Enumeration type.
/// @param  lhs         First operand.
/// @param  rhs         Second operand.
/// @return The result of <c>lhs & !rhs</c>.
template<typename TEnum>
constexpr
#if DOXYGEN
TEnum
#else
typename  std::enable_if<    alib::enums::T_EnumIsBitwise     <TEnum>::value
                         && !alib::enums::T_EnumIsArithmetical<TEnum>::value, TEnum>::type
#endif
operator-  (TEnum  lhs, TEnum rhs) noexcept
{
    using TBits= typename std::underlying_type<TEnum>::type;
    return TEnum( TBits(lhs) & (~TBits(rhs)) );
}

/// Removes bit(s) found in \p{rhs} from \p{lhs}. This is a shortcut to:
///
///      lhs &= !rhs
///
/// Selected by the compiler only if \alib{enums;T_EnumIsBitwise} is specialized for
/// template enum type \p{TEnum} to inherit \c std::true_type and if \alib{enums;T_EnumIsArithmetical}
/// is \b not specialized to inherit  \c std::true_type. The latter is to avoid ambiguities in
/// situations where an enum is both, arithmetical and bitwise.
///
/// @tparam TEnum       Enumeration type.
/// @param[in,out]  lhs Reference to the first operand. Receives the result.
/// @param  rhs         Second operand.
/// @return The new value of \p{lhs} which is set to <c>lhs & ( ~rhs )</c>.
template<typename TEnum>
constexpr
#if DOXYGEN
TEnum
#else
typename  std::enable_if<    alib::enums::T_EnumIsBitwise     <TEnum>::value
                         && !alib::enums::T_EnumIsArithmetical<TEnum>::value, TEnum>::type
#endif
operator-=  (TEnum&  lhs, TEnum rhs) noexcept
{
    using TBits= typename std::underlying_type<TEnum>::type;
    return lhs= TEnum( TBits(lhs) & (~TBits(rhs)) );
}

#if !DOXYGEN
namespace alib {
#endif

#if DOXYGEN
/// Tests if the integral value of the given enum \p{element} contains all bits set in
/// \p{selection}.
/// In other words, returns result of:
///
///       ( element & selection ) == selection
///
/// Selected by the compiler only if \alib{enums;T_EnumIsBitwise} is specialized for
/// template enum type \p{TEnum} to inherit \c std::true_type.
///
/// @tparam TEnum      Enumeration type. Deduced by the compiler.
/// @param  element    Bitset to be tested.
/// @param  selection  Second operand.
/// @return \c true if all bits of \p{selection} are set in \p{element}.
template<typename TEnum>
constexpr
bool HasBits(TEnum  element, TEnum selection)                                              noexcept;
#else
template<typename TEnum>
constexpr
ATMP_T_IF(bool, alib::enums::T_EnumIsBitwise<TEnum>::value)
HasBits(TEnum  element, TEnum selection)                           noexcept
{
    using TBits= typename std::underlying_type<TEnum>::type;
    return ( TBits(element) & TBits(selection) ) == TBits(selection);
}
#endif

#if DOXYGEN
/// Tests if the integral value of the given enum \p{element} contains at least one of the bits
/// set in \p{selection}.
/// In other words, returns result of:
///
///       ( element & selection ) != 0
///
/// Selected by the compiler only if \alib{enums;T_EnumIsBitwise} is specialized for
/// template enum type \p{TEnum} to inherit \c std::true_type.
///
/// @tparam TEnum      Enumeration type. Deduced by the compiler.
/// @param  element    Bitset to be tested.
/// @param  selection  Second operand.
/// @return \c true if one of the bits of \p{selection} are set in \p{element}.
template<typename TEnum>
constexpr
bool HasOneOf(TEnum  element, TEnum selection)                                             noexcept;
#else
template<typename TEnum>
constexpr
ATMP_T_IF(bool, alib::enums::T_EnumIsBitwise<TEnum>::value)
HasOneOf(TEnum  element, TEnum selection)                                                   noexcept
{
    using TBits= typename std::underlying_type<TEnum>::type;
    return ( TBits(element) & TBits(selection) ) != TBits(0);
}
#endif

#if DOXYGEN
/// Returns the number of bitwise enumeration elements set in the given value.
/// In other words, the bits given in \p value are counted and the number is returned.
///
/// Selected by the compiler only if \alib{enums;T_EnumIsBitwise} is specialized for
/// template enum type \p{TEnum} to inherit \c std::true_type.
///
/// @param value A single or composite selection of bits.
/// @return The result of a call to #alib::lang::BitCount().
template<typename TEnum>
static constexpr
int CountElements( TEnum value );
#else
template<typename TEnum>
constexpr
ATMP_T_IF(int, alib::enums::T_EnumIsBitwise<TEnum>::value)
CountElements( TEnum value )
{
    return lang::BitCount(UnderlyingIntegral(value));
}
#endif

#if DOXYGEN
}}} // doxygen namespace [alib::enums::bitwise]
#else
} // namespace [alib]
#endif

#endif // HPP_ALIB_ENUMS_BITWISE

