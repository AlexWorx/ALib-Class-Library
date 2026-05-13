//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
#ifndef H_ALIB_VARIABLES_PLUGINS
#define H_ALIB_VARIABLES_PLUGINS
#pragma once
#ifndef HPP_ALIB
#   include "alib/alib.prepro.hpp"
#endif

#if ALIB_VARIABLES
//=========================================== Dependencies =========================================
#include "ALib.Variables.H"

//============================================= Imports ============================================
#if ALIB_C20_MODULES
import ALib.Variables.Plugins;
#else
#include "alib/variables/plugins.hpp"

#endif // ALIB_C20_MODULES
#endif // ALIB_VARIABLES
#endif // H_ALIB_VARIABLES_PLUGINS
