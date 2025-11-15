//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef H_ALIB_STRINGS_MONOMEM
#define H_ALIB_STRINGS_MONOMEM
#pragma once
#ifndef INL_ALIB
#   include "alib/alib.inl"
#endif

#include "alib/strings/strings.prepro.hpp"

#if ALIB_STRINGS && ALIB_MONOMEM
#   if ALIB_C20_MODULES && !DOXYGEN
        import ALib.Strings.Monomem;
#   elif !defined(ALIB_INC_STRINGS_MONOMEM_MPP)
#       define ALIB_INC_STRINGS_MONOMEM_MPP
#       include "alib/strings/monomem/strings_monomem.mpp"
#   endif
#endif

#endif // H_ALIB_STRINGS_MONOMEM
