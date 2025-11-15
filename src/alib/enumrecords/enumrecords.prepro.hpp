//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_ENUMRECORDS_PP
#define HPP_ALIB_ENUMRECORDS_PP
#pragma once
#ifndef INL_ALIB
#   include "alib/alib.inl"
#endif
#if ALIB_ENUMRECORDS

//##################################################################################################
// Symbols introduced by module ALib.Enums
//##################################################################################################
#define  ALIB_ENUMS_ASSIGN_RECORD( TEnum, TRecord )                                                \
namespace alib::enumrecords {                                                                            \
template<> struct RecordsTraits<TEnum>     { using  Type=  TRecord; };   }

#endif
#endif // HPP_ALIB_ENUMRECORDS_PP
