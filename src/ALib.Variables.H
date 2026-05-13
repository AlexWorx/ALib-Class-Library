//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
#ifndef H_ALIB_VARIABLES
#define H_ALIB_VARIABLES
#pragma once
#ifndef HPP_ALIB
#   include "alib/alib.prepro.hpp"
#endif

#if ALIB_VARIABLES
//=========================================== Dependencies =========================================
#include "ALib.Monomem.SharedMonoVal.H"
#include "ALib.Containers.StringTree.H"
#include "ALib.Containers.List.H"
#include "ALib.Boxing.H"
#include "ALib.Strings.Token.H"
#include "ALib.Strings.StdFunctors.H"
#include "ALib.Strings.Vector.H"
#include "ALib.Strings.Escaper.H"
#include "ALib.EnumRecords.H"
#include "ALib.Resources.H"

#include "alib/variables/variables.prepro.hpp"
//============================================= Imports ============================================
#if ALIB_C20_MODULES
import ALib.Variables;
#else
#include "alib/variables/vmeta.hpp"
#include "alib/variables/erpriority.hpp"
#include "alib/variables/declaration.hpp"


#include "alib/variables/configuration.hpp"

#endif // ALIB_C20_MODULES
#endif // ALIB_VARIABLES
#endif // H_ALIB_VARIABLES
