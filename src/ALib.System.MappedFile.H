//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
#ifndef H_ALIB_SYSTEM_MAPPED_FILE
#define H_ALIB_SYSTEM_MAPPED_FILE
#pragma once
#ifndef HPP_ALIB
#   include "alib/alib.prepro.hpp"
#endif

#if ALIB_SYSTEM
//=========================================== Dependencies =========================================
#include "ALib.Lang.H"
#include "ALib.System.H"

//============================================= Imports ============================================
#if ALIB_C20_MODULES
    import ALib.System;
    import ALib.System.MappedFile;
#else
#   include "alib/system/mappedfile.hpp"
#endif // ALIB_C20_MODULES

#endif // ALIB_SYSTEM
#endif // H_ALIB_SYSTEM_MAPPED_FILE
