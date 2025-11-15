//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef H_ALIB_CAMP
#define H_ALIB_CAMP
#pragma once

#ifndef INL_ALIB
#   include "alib/alib.inl"
#endif

#include "alib/camp/camp.prepro.hpp"

#if ALIB_CAMP
#   if ALIB_C20_MODULES && !DOXYGEN
        import ALib.Camp;
#   elif !defined(ALIB_INC_CAMP_CAMP_MPP)
#       define ALIB_INC_CAMP_CAMP_MPP
#       include "alib/camp/camp.mpp"
#   endif
#endif

#endif // H_ALIB_CAMP
