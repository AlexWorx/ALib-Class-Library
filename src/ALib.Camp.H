//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
#ifndef H_ALIB_CAMP
#define H_ALIB_CAMP
#pragma once
#ifndef HPP_ALIB
#   include "alib/alib.prepro.hpp"
#endif

#if ALIB_CAMP
//=========================================== Dependencies =========================================
#include "ALib.Containers.List.H"
#include "ALib.Monomem.StdContainers.H"
#include "ALib.Monomem.H"
#include "ALib.Strings.Token.H"
#include "ALib.Boxing.H"
#include "ALib.Resources.H"
#include "ALib.Variables.H"
#include "ALib.System.H"

#include "alib/camp/camp.prepro.hpp"
//============================================= Imports ============================================
#if ALIB_C20_MODULES
import ALib.Camp;
#else
#include "alib/camp/camp.hpp"
#include "alib/camp/loadtokens.hpp"

#endif // ALIB_C20_MODULES
#endif // ALIB_CAMP
#endif // H_ALIB_CAMP
