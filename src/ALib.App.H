//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
#ifndef H_ALIB_APP
#define H_ALIB_APP
#pragma once
#ifndef HPP_ALIB
#   include "alib/alib.prepro.hpp"
#endif

#if ALIB_APP
//=========================================== Dependencies =========================================
#include "ALib.Singletons.H"
#include "ALib.Monomem.StdContainers.H"
#include "ALib.Boxing.StdFunctors.H"
#include "ALib.Containers.List.H"
#include "ALib.Containers.HashTable.H"
#include "ALib.Strings.StdFunctors.H"
#include "ALib.Strings.Vector.H"
#include "ALib.Strings.Token.H"
#include "ALib.Strings.StdFunctors.H"
#include "ALib.Monomem.SharedMonoVal.H"
#include "ALib.Resources.H"
#include "ALib.Format.Paragraphs.H"
#include "ALib.Exceptions.H"
#include "ALib.System.H"
#include "ALib.Camp.H"
#include "ALib.ALox.H"

#include "alib/app/app.prepro.hpp"

#include <stack>
#include <bitset>
#include <vector>
//============================================= Imports ============================================
#if ALIB_C20_MODULES
import ALib.App;
#else
#include "alib/app/appcamp.hpp"
#include "alib/app/cliargtypes.hpp"
#include "alib/app/cli.hpp"
#include "alib/app/cliutil.hpp"
#include "alib/app/app.hpp"

#endif
#endif // ALIB_C20_MODULES
#endif // H_ALIB_APP
