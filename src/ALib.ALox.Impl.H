//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef H_ALIB_ALOX_IMPL
#define H_ALIB_ALOX_IMPL
#pragma once
#ifndef INL_ALIB
#   include "alib/alib.inl"
#endif



#include "alib/alox/alox.prepro.hpp"

#if ALIB_ALOX
#   if ALIB_C20_MODULES && !DOXYGEN
        import ALib.ALox.Impl;
#   elif !defined(ALIB_INC_ALOX_ALOX_IMPL_MPP)
#       define ALIB_INC_ALOX_ALOX_IMPL_MPP
#       include "alib/alox/alox_impl.mpp"
#   endif
#endif

#endif // H_ALIB_ALOX_IMPL
