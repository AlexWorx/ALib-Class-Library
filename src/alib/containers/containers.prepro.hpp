//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_CONTAINERS_PP
#define HPP_ALIB_CONTAINERS_PP
#pragma once
#ifndef INL_ALIB
#   include "alib/alib.inl"
#endif

// #################################################################################################
// Symbols introduced by module ALib.Containers
// #################################################################################################

//  ALIB_DEBUG_CONTAINERS
#if !DOXYGEN
#if ALIB_CONTAINERS
#   if !defined(ALIB_DEBUG_CONTAINERS)
#      define   ALIB_DEBUG_CONTAINERS           0
#   elif !ALIB_DEBUG && ALIB_DEBUG_CONTAINERS
#      undef    ALIB_DEBUG_CONTAINERS
#      define   ALIB_DEBUG_CONTAINERS           0
#      pragma message "Symbol ALIB_DEBUG_CONTAINERS set (from outside!) while ALIB_DEBUG is not. The symbol got disabled."
#   else
#      define   ALIB_DEBUG_CONTAINERS           0
#   endif
#else
#      define   ALIB_DEBUG_CONTAINERS           0
#endif
#endif // DOXYGEN (has a typical strange error here otherwise)
#endif // HPP_ALIB_CONTAINERS_PP

