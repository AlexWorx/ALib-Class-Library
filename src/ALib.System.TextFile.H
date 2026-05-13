//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
#ifndef H_ALIB_SYSTEM_TEXT_FILE
#define H_ALIB_SYSTEM_TEXT_FILE
#pragma once
#ifndef HPP_ALIB
#   include "alib/alib.prepro.hpp"
#endif

#if ALIB_SYSTEM
//=========================================== Dependencies =========================================
#include "ALib.Monomem.StdContainers.H"
#include "ALib.Strings.StdIOStream.H"
#include "ALib.System.H"
#include <fstream>

//============================================= Imports ============================================
#if ALIB_C20_MODULES
    import ALib.System.TextFile;
#else
#   include "alib/system/textfile.hpp"
#endif // ALIB_C20_MODULES

#endif // ALIB_SYSTEM
#endif // H_ALIB_SYSTEM_TEXT_FILE
