//==================================================================================================
/// \file
/// This header file is part of module \alib_enums of the \aliblong.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_ENUMS_ITERABLE_BITSET
#define HPP_ALIB_ENUMS_ITERABLE_BITSET 1
#pragma once
#include "alib/enums/iterable.hpp"

ALIB_ASSERT_MODULE(ENUMS)

#include "alib/lang/bitset.hpp"

namespace alib { namespace enums {
#if DOXYGEN
//==================================================================================================
/// This is a using statement which is available (enabled) for enumerations for which traits struct
/// \alib{enums;T_EnumIsIterable} is specialized. With this, the interface methods of templated
/// class \alib{lang;TBitSet} accept and return enumeration elements.
///
/// @see For a quick tutorial on the use of this type, see chapter
/// \ref alib_enums_iter_bitset "3.5 Using Class TBitSet with Iterable Enums" of the Programmer's
/// Manual of module \alib_enums_nl.
///
/// @tparam TEnum      The enum type to use with class \alib{lang;TBitSet}.
/// \note Other than documented here, this using statement has a second template parameter
///       \p{TEnableIf}. This parameter has a default expressions and <b>must not</b> be provided
///       because it is used internally to ensure that only if a specialization of
///       \alib{enums;T_EnumIsIterable;T_EnumIsIterable<TEnum>} exists, this type definition becomes
///       available.
///       (..and this is why it was explicitly 'faked out' of this documentation).
//==================================================================================================
template<typename TEnum>
using EnumBitSet =  TBitSet<TEnum, enums::T_EnumIsIterable<TEnum>::End,
                                        enums::T_EnumIsIterable<TEnum>::Begin>;
#else
template<typename TEnum, typename TEnableIf
= ATMP_VOID_IF(ATMP_EQ(const TEnum, decltype(alib::enums::T_EnumIsIterable<TEnum>::Begin)) ) >
using EnumBitSet =  lang::TBitSet<TEnum, enums::T_EnumIsIterable<TEnum>::End,
                                         enums::T_EnumIsIterable<TEnum>::Begin>;
#endif
} // namespace alib[::enums]

#if DOXYGEN

/// Type alias in namespace \b alib.
/// @see For more information, consult type definition \alib{enums;EnumBitSet}.
template<typename TEnum>
using EnumBitSet =  lang::TBitSet<TEnum, enums::T_EnumIsIterable<TEnum>::End,
                                         enums::T_EnumIsIterable<TEnum>::Begin>;
#else
template<typename TEnum, typename TEnableIf
= ATMP_VOID_IF(ATMP_EQ(const TEnum, decltype(alib::enums::T_EnumIsIterable<TEnum>::Begin)) ) >
using EnumBitSet =  lang::TBitSet<TEnum, enums::T_EnumIsIterable<TEnum>::End,
                                         enums::T_EnumIsIterable<TEnum>::Begin>;
#endif

} // namespace [alib]

#endif // HPP_ALIB_ENUMS_ITERABLE_BITSET

