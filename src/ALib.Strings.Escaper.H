//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef H_ALIB_STRINGS_ESCAPER
#define H_ALIB_STRINGS_ESCAPER
#pragma once
#ifndef INL_ALIB
#   include "alib/alib.inl"
#endif

#include "alib/strings/strings.prepro.hpp"

#if ALIB_STRINGS
#   if ALIB_C20_MODULES && !DOXYGEN
        import ALib.Strings.Escaper;
#   elif !defined(ALIB_INC_STRINGS_UTIL_ESCAPER_MPP)
#       define ALIB_INC_STRINGS_UTIL_ESCAPER_MPP
#       include "alib/strings/util/escaper.mpp"
#   endif
#endif

#endif // H_ALIB_STRINGS_ESCAPER


