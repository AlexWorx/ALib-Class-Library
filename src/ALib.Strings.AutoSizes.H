//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
#ifndef H_ALIB_STRINGS_AUTOSIZES
#define H_ALIB_STRINGS_AUTOSIZES
#pragma once
#ifndef HPP_ALIB
#   include "alib/alib.prepro.hpp"
#endif


#if ALIB_STRINGS
//=========================================== Dependencies =========================================
#include "ALib.Strings.H"

#include <vector>

//============================================= Imports ============================================
#if ALIB_C20_MODULES
import ALib.Strings.AutoSizes;
#else
#include "alib/strings/util/autosizes.hpp"


#endif // ALIB_C20_MODULES
#endif // ALIB_STRINGS
#endif // H_ALIB_STRINGS_AUTOSIZES
