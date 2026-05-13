//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
#ifndef H_ALIB_FORMAT_STDIMPLEMENTATION
#define H_ALIB_FORMAT_STDIMPLEMENTATION
#pragma once
#ifndef HPP_ALIB
#   include "alib/alib.prepro.hpp"
#endif

#if ALIB_FORMAT
//=========================================== Dependencies =========================================
#include "ALib.EnumRecords.H"
#include "ALib.Format.H"

//============================================= Imports ============================================
#if ALIB_C20_MODULES
import ALib.Format.StdImplementation;
#else
#include "alib/format/formatterstdimpl.hpp"

#endif // ALIB_C20_MODULES
#endif // ALIB_FORMAT
#endif // H_ALIB_FORMAT_STDIMPLEMENTATION
