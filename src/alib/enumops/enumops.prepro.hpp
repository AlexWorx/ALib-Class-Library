//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_ENUMOPS_PP
#define HPP_ALIB_ENUMOPS_PP
#pragma once

#ifndef INL_ALIB
#   include "alib/alib.inl"
#endif

//##################################################################################################
// Symbols introduced by module ALib.Enums
//##################################################################################################
#define  ALIB_ENUMS_MAKE_ARITHMETICAL(TEnum)                                                       \
namespace alib::enumops {                                                                          \
template<> struct ArithmeticalTraits<TEnum> : std::true_type {}; }                                 \

#define  ALIB_ENUMS_MAKE_BITWISE(TEnum)                                                            \
namespace alib::enumops {                                                                          \
template<>                                                                                         \
struct BitwiseTraits<TEnum> : std::true_type {}; }

#define  ALIB_ENUMS_MAKE_ITERABLE_BEGIN_END(TEnum, StartElement, StopElement )                     \
namespace alib::enumops {                                                                          \
template<> struct IterableTraits<TEnum>   : std::true_type                                         \
{                                                                                                  \
    static constexpr   TEnum    Begin    = StartElement;                                           \
    static constexpr   TEnum    End      = StopElement;                                            \
};}

#define  ALIB_ENUMS_MAKE_ITERABLE(TEnum, StopElement )                                             \
ALIB_ENUMS_MAKE_ITERABLE_BEGIN_END( TEnum, TEnum(0),  StopElement )

#endif // HPP_ALIB_ENUMOPS_PP
