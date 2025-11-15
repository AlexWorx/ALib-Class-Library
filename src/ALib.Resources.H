//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef H_ALIB_RESOURCES
#define H_ALIB_RESOURCES
#pragma once
#ifndef INL_ALIB
#   include "alib/alib.inl"
#endif

#include "alib/resources/resources.prepro.hpp"

#if ALIB_RESOURCES
#   if ALIB_C20_MODULES && !DOXYGEN
        import ALib.Resources;
#   elif !defined(ALIB_INC_RESOURCES_RESOURCES_MPP)
#       define ALIB_INC_RESOURCES_RESOURCES_MPP
#       include "alib/resources/resources.mpp"
#   endif
#endif

#endif // H_ALIB_RESOURCES
