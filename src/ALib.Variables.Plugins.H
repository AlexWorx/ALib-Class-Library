//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef H_ALIB_VARIABLES_PLUGINS
#define H_ALIB_VARIABLES_PLUGINS
#pragma once
#ifndef INL_ALIB
#   include "alib/alib.inl"
#endif

#include "alib/variables/variables.prepro.hpp"

#if ALIB_VARIABLES
#   if ALIB_C20_MODULES && !DOXYGEN
        import ALib.Variables.Plugins;
#   elif !defined(ALIB_INC_VARIABLES_PLUGINS_MPP)
#       define ALIB_INC_VARIABLES_PLUGINS_MPP
#       include "alib/variables/plugins.mpp"
#   endif
#endif

#endif // H_ALIB_VARIABLES_PLUGINS
