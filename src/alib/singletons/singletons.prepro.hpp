//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_SINGLETONS_PP
#define HPP_ALIB_SINGLETONS_PP
#pragma once
#ifndef INL_ALIB
#   include "alib/alib.inl"
#endif


//##################################################################################################
// Symbols introduced by module ALib.Singletons
//##################################################################################################

// ALIB_FEAT_SINGLETON_MAPPED
#if ALIB_SINGLETONS
#   if !defined(ALIB_FEAT_SINGLETON_MAPPED)
#      if defined(_WIN32)
#         if ALIB_SINGLE_THREADED
#            define ALIB_FEAT_SINGLETON_MAPPED   0
#         else
#            define ALIB_FEAT_SINGLETON_MAPPED   1
#         endif
#      else
#         define ALIB_FEAT_SINGLETON_MAPPED   0
#      endif
#   endif
#else
#   define ALIB_FEAT_SINGLETON_MAPPED    0
#endif // ALIB_SINGLETONS

#endif // HPP_ALIB_SINGLETONS_PP
