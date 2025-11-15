//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_BITBUFFER_PP
#define HPP_ALIB_BITBUFFER_PP
#pragma once
#ifndef INL_ALIB
#   include "alib/alib.inl"
#endif

//##################################################################################################
// Symbols introduced by module ALib.BitBuffer
//##################################################################################################

//  ALIB_DEBUG_ARRAY_COMPRESSION
#if ALIB_BITBUFFER
#   if !defined(ALIB_DEBUG_ARRAY_COMPRESSION)
#      define   ALIB_DEBUG_ARRAY_COMPRESSION   ALIB_DEBUG
#   elif !ALIB_DEBUG && ALIB_DEBUG_ARRAY_COMPRESSION
#      undef    ALIB_DEBUG_ARRAY_COMPRESSION
#      define   ALIB_DEBUG_ARRAY_COMPRESSION 0
#      pragma message "Symbol ALIB_DEBUG_ARRAY_COMPRESSION set (from outside!) while ALIB_DEBUG is not. The symbol got disabled."
#   endif
#else
#   define ALIB_DEBUG_ARRAY_COMPRESSION  0
#endif

#endif // HPP_ALIB_BITBUFFER_PP
