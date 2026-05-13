//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
#ifndef H_ALIB_CAMP_RESOURCE_COMPILER
#define H_ALIB_CAMP_RESOURCE_COMPILER
#pragma once

#ifndef HPP_ALIB
#   include "alib/alib.prepro.hpp"
#endif

#if ALIB_CAMP
//=========================================== Dependencies =========================================
#include "ALib.Threads.H"
#include "ALib.Camp.H"
#include "ALib.Format.Paragraphs.H"

//============================================= Imports ============================================
#if ALIB_C20_MODULES
import ALib.Camp.ResourceCompiler;
#else
#include "alib/camp/resourcecompiler.hpp"

#endif // ALIB_C20_MODULES
#endif // ALIB_CAMP
#endif // H_ALIB_CAMP_RESOURCE_COMPILER
