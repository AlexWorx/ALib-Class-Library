//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef H_ALIB_VARIABLES
#define H_ALIB_VARIABLES
#pragma once
#ifndef INL_ALIB
#   include "alib/alib.inl"
#endif

#include "alib/variables/variables.prepro.hpp"

#if ALIB_VARIABLES
#   if ALIB_C20_MODULES && !DOXYGEN
        import ALib.Variables;
#   elif !defined(ALIB_INC_VARIABLES_MPP)
#       define ALIB_INC_VARIABLES_MPP
#       include "alib/variables/variables.mpp"
#   endif
#endif

#endif // H_ALIB_VARIABLES

