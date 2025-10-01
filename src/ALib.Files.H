//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef H_ALIB_FILES
#define H_ALIB_FILES
#pragma once
#ifndef INL_ALIB
#   include "alib/alib.inl"
#endif

#include "alib/files/files.prepro.hpp"

#if ALIB_FILES
#   if ALIB_C20_MODULES && !DOXYGEN
        import ALib.Files;
#   elif !defined(ALIB_INC_FILES_MPP)
#       define ALIB_INC_FILES_FILES_MPP
#       include "alib/files/files.mpp"
#   endif
#endif

#endif // H_ALIB_FILES

