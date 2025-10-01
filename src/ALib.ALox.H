//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef H_ALIB_ALOX
#define H_ALIB_ALOX
#pragma once
#ifndef INL_ALIB
#   include "alib/alib.inl"
#endif



#include "alib/alox/alox.prepro.hpp"

#if ALIB_ALOX
#   if ALIB_C20_MODULES && !DOXYGEN
        import ALib.ALox;
#   elif !defined(ALIB_INC_ALOX_ALOX_MPP)
#       define ALIB_INC_ALOX_ALOX_MPP
#       include "alib/alox/alox.mpp"
#   endif
#endif

#endif // H_ALIB_ALOX



