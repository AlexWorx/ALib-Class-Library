//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
#ifndef H_ALIB_MONOMEM_SHAREDMONOVAL
#define H_ALIB_MONOMEM_SHAREDMONOVAL
#pragma once
#ifndef HPP_ALIB
#   include "alib/alib.prepro.hpp"
#endif

#if ALIB_MONOMEM
//=========================================== Dependencies =========================================
#include "ALib.Lang.H"
#include "ALib.Time.H"
#include "ALib.Monomem.H"

//============================================= Imports ============================================
#if ALIB_C20_MODULES
import ALib.Monomem.SharedMonoVal;
#else
#include "alib/monomem/sharedmonoval.hpp"

#endif // ALIB_C20_MODULES
#endif // ALIB_MONOMEM
#endif // H_ALIB_MONOMEM_SHAREDMONOVAL
