//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
#ifndef H_ALIB_EXCEPTIONS
#define H_ALIB_EXCEPTIONS
#pragma once
#ifndef HPP_ALIB
#   include "alib/alib.prepro.hpp"
#endif

#if ALIB_EXCEPTIONS
//=========================================== Dependencies =========================================
#include "ALib.Singletons.H"
#include "ALib.Boxing.H"
#include "ALib.Strings.H"
#include "ALib.Monomem.SharedMonoVal.H"
#include "ALib.EnumRecords.H"
#include "ALib.Resources.H"

#include <stack>
//============================================= Imports ============================================
#if ALIB_C20_MODULES
import ALib.Exceptions;
#else
#include "alib/exceptions/message.hpp"
#include "alib/exceptions/exception.hpp"

#endif // ALIB_C20_MODULES
#endif // ALIB_EXCEPTIONS
#endif // H_ALIB_EXCEPTIONS
