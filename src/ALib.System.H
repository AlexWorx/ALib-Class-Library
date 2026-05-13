//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
#ifndef H_ALIB_SYSTEM
#define H_ALIB_SYSTEM
#pragma once
#ifndef HPP_ALIB
#   include "alib/alib.prepro.hpp"
#endif

#if ALIB_SYSTEM
//=========================================== Dependencies =========================================
#include "ALib.Monomem.H"
#include "ALib.Containers.LRUCacheTable.H"
#include "ALib.Boxing.H"
#include "ALib.Strings.Tokenizer.H"
#include "ALib.Strings.Vector.H"
#include "ALib.EnumRecords.H"

#include "alib/system/system.prepro.hpp"

#include <filesystem>
#if defined( __GNUC__ ) || ( defined(_WIN32) && defined(_MSC_VER))
#include <errno.h>
#endif

//============================================= Imports ============================================
#if ALIB_C20_MODULES
import ALib.System;
#else
#include "alib/system/console.hpp"
#include "alib/system/environment.hpp"
#include "alib/system/path.hpp"
#include "alib/system/filestatus.hpp"
#include "alib/system/processinfo.hpp"
#include "alib/system/shellcommand.hpp"

#endif // ALIB_C20_MODULES
#endif // ALIB_SYSTEM
#endif // H_ALIB_SYSTEM
