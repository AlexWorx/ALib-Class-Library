//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
#ifndef H_ALIB_VARIABLES_INIFILE
#define H_ALIB_VARIABLES_INIFILE
#pragma once
#ifndef HPP_ALIB
#   include "alib/alib.prepro.hpp"
#endif

#if ALIB_VARIABLES
//=========================================== Dependencies =========================================
#include "ALib.Monomem.H"
#include "ALib.Containers.List.H"
#include "ALib.Containers.HashTable.H"
#include "ALib.Boxing.H"
#include "ALib.EnumRecords.H"
#include "ALib.Exceptions.H"
#include "ALib.Resources.H"
#include "ALib.System.H"
#include "ALib.Variables.H"

//============================================= Imports ============================================
#if ALIB_C20_MODULES
import ALib.Variables.IniFile;
#else
#include "alib/variables/inifile.hpp"
#include "alib/variables/inifilefeeder.hpp"

#endif // ALIB_C20_MODULES
#endif // ALIB_VARIABLES
#endif // H_ALIB_VARIABLES_INIFILE
