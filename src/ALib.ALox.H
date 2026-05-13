//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
#ifndef H_ALIB_ALOX
#define H_ALIB_ALOX
#pragma once
#ifndef HPP_ALIB
#   include "alib/alib.prepro.hpp"
#endif

#if ALIB_ALOX
//=========================================== Dependencies =========================================
#include "ALib.Boxing.StdFunctors.H"
#include "ALib.Strings.H"
#include "ALib.EnumRecords.H"
#include "ALib.Exceptions.H"
#include "ALib.Variables.H"

#include "alib/alox/alox.prepro.hpp"

//============================================= Imports ============================================
#if ALIB_C20_MODULES
import ALib.ALox;
#else
#if (ALIB_DEBUG && ALOX_DBG_LOG) || ALOX_REL_LOG
#include "alib/alox/alox_init.hpp"
#include "alib/alox/detail/loxpimpl.hpp"
#include "alib/alox/log.hpp"
#include "alib/alox/lox.hpp"
#include "alib/alox/logtools.hpp"
#endif
#endif
#endif // ALIB_C20_MODULES
#endif // H_ALIB_ALOX
