//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
#ifndef H_ALIB_CAMP_BASE
#define H_ALIB_CAMP_BASE
#pragma once
#ifndef HPP_ALIB
#   include "alib/alib.prepro.hpp"
#endif

#if ALIB_CAMP
//=========================================== Dependencies =========================================
#include "ALib.System.H"
#include "ALib.Exceptions.H"
#include "ALib.Format.H"
#include "ALib.Variables.H"
#include "ALib.Variables.IniFile.H"
#include "ALib.Camp.H"

#include <stack>
//============================================= Imports ============================================
#if ALIB_C20_MODULES
import ALib.Camp.Base;
#else
#include "alib/camp/basecamp.hpp"

#endif // ALIB_C20_MODULES
#endif // ALIB_CAMP
#endif // H_ALIB_CAMP_BASE
