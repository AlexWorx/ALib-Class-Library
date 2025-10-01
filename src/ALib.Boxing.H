//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef H_ALIB_BOXING
#define H_ALIB_BOXING
#pragma once
#ifndef INL_ALIB
#   include "alib/alib.inl"
#endif

#include "alib/boxing/boxing.prepro.hpp"

#if ALIB_BOXING
#   if ALIB_C20_MODULES && !DOXYGEN
        import ALib.Boxing;
#   elif !defined(ALIB_INC_BOXING_BOXING_MPP)
#       define ALIB_INC_BOXING_BOXING_MPP
#       include "alib/boxing/boxing.mpp"
#   endif
#endif

#endif // H_ALIB_BOXING

