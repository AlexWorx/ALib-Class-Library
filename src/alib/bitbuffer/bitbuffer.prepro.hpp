//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
#ifndef HPP_ALIB_BITBUFFER_PP
#define HPP_ALIB_BITBUFFER_PP
#pragma once
#ifndef HPP_ALIB
#   include "alib/alib.prepro.hpp"
#endif

//##################################################################################################
// Macro  introduced by module ALib.BitBuffer
//##################################################################################################

//  ALIB_DEBUG_ARRAY_COMPRESSION
#if ALIB_BITBUFFER
#   if !defined(ALIB_DEBUG_ARRAY_COMPRESSION)
#      define   ALIB_DEBUG_ARRAY_COMPRESSION   ALIB_DEBUG
#   elif !ALIB_DEBUG && ALIB_DEBUG_ARRAY_COMPRESSION
#      undef    ALIB_DEBUG_ARRAY_COMPRESSION
#      define   ALIB_DEBUG_ARRAY_COMPRESSION 0
#      pragma message "Configuration Macro ALIB_DEBUG_ARRAY_COMPRESSION set (from outside!) while ALIB_DEBUG is not. The symbol got disabled."
#   endif
#else
#   define ALIB_DEBUG_ARRAY_COMPRESSION  0
#endif

#endif // HPP_ALIB_BITBUFFER_PP
