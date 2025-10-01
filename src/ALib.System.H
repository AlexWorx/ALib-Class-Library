//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef H_ALIB_SYSTEM
#define H_ALIB_SYSTEM
#pragma once

#ifndef INL_ALIB
#   include "alib/alib.inl"
#endif

#include "alib/system/system.prepro.hpp"

#if ALIB_SYSTEM
#   if ALIB_C20_MODULES && !DOXYGEN
        import ALib.System;
#   elif !defined(ALIB_INC_SYSTEM_SYSTEM_MPP)
#       define ALIB_INC_SYSTEM_SYSTEM_MPP
#       include "alib/system/system.mpp"
#   endif
#endif

#endif // H_ALIB_SYSTEM

