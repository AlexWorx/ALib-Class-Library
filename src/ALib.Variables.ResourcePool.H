//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
#ifndef H_ALIB_VARIABLES_RESOURCEPOOL
#define H_ALIB_VARIABLES_RESOURCEPOOL
#pragma once
#ifndef HPP_ALIB
#   include "alib/alib.prepro.hpp"
#endif

#if ALIB_VARIABLES && ALIB_RESOURCES
//=========================================== Dependencies =========================================
#include "ALib.Variables.H"

//============================================= Imports ============================================
#if ALIB_C20_MODULES
import ALib.Variables.ResourcePool;
#else
#include "alib/variables/configresourcepool.hpp"

#endif // ALIB_C20_MODULES
#endif // ALIB_VARIABLES
#endif // H_ALIB_VARIABLES_RESOURCEPOOL
