//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
#ifndef H_ALIB_STRINGS_STDFORMATTER
#define H_ALIB_STRINGS_STDFORMATTER
#pragma once
#ifndef HPP_ALIB
#   include "alib/alib.prepro.hpp"
#endif

#if ALIB_STRINGS
//=========================================== Dependencies =========================================
#include "ALib.Lang.H"
#include "ALib.Strings.H"

#   if __has_include(<format>)
#include <format>
#   else
#include <fmt/format.h>
#   endif
//============================================= Includes ===========================================
#include "alib/strings/stdformatter.hpp"

#endif // ALIB_STRINGS
#endif // H_ALIB_STRINGS_STDFORMATTER
