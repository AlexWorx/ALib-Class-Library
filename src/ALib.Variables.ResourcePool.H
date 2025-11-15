//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef H_ALIB_VARIABLES_RESOURCEPOOL
#define H_ALIB_VARIABLES_RESOURCEPOOL
#pragma once
#ifndef INL_ALIB
#   include "alib/alib.inl"
#endif

#include "alib/variables/variables.prepro.hpp"

#if ALIB_VARIABLES && ALIB_RESOURCES
#   if ALIB_C20_MODULES && !DOXYGEN
        import ALib.Variables.ResourcePool;
#   elif !defined(ALIB_INC_VARIABLES_CONFIGRESOURCEPOOL_MPP)
#       define ALIB_INC_VARIABLES_CONFIGRESOURCEPOOL_MPP
#       include "alib/variables/configresourcepool.mpp"
#   endif
#endif

#endif // H_ALIB_VARIABLES_RESOURCEPOOL
