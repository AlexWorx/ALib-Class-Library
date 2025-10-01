//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef H_ALIB_SINGLETONS
#define H_ALIB_SINGLETONS
#pragma once
#ifndef INL_ALIB
#   include "alib/alib.inl"
#endif

#include "alib/singletons/singletons.prepro.hpp"

#if ALIB_SINGLETONS
#   if ALIB_C20_MODULES && !DOXYGEN
        import ALib.Singletons;
#   elif !defined(ALIB_INC_SINGLETONS_SINGLETONS_MPP)
#       define ALIB_INC_SINGLETONS_SINGLETONS_MPP
#       include "alib/singletons/singletons.mpp"
#   endif
#endif

#endif // H_ALIB_SINGLETONS


