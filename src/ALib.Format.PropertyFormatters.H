//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
#ifndef H_ALIB_FORMAT_PROPERTYFORMATTERS
#define H_ALIB_FORMAT_PROPERTYFORMATTERS
#pragma once
#ifndef HPP_ALIB
#   include "alib/alib.prepro.hpp"
#endif

#if ALIB_FORMAT & ALIB_VARIABLES
//=========================================== Dependencies =========================================
#include "ALib.Strings.H"
#include "ALib.EnumRecords.H"
#include "ALib.Exceptions.H"
#include "ALib.Format.PropertyFormatter.H"
#include "ALib.Variables.H"

#include <unordered_map>
//============================================= Imports ============================================
#if ALIB_C20_MODULES
import ALib.Format.PropertyFormatters;
#else
#include "alib/format/propertyformatters.hpp"

#endif // ALIB_C20_MODULES
#endif // ALIB_FORMAT
#endif // H_ALIB_FORMAT_PROPERTYFORMATTERS
