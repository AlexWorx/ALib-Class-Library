//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
#ifndef H_ALIB_STRINGS_SEARCH
#define H_ALIB_STRINGS_SEARCH
#pragma once
#ifndef HPP_ALIB
#   include "alib/alib.prepro.hpp"
#endif

#if ALIB_STRINGS
//=========================================== Dependencies =========================================
#include "ALib.Strings.H"

#if ALIB_FEAT_BOOST_REGEX && (!ALIB_CHARACTERS_WIDE || ALIB_CHARACTERS_NATIVE_WCHAR)
#include <boost/regex.hpp>
#include <string>
#endif

//============================================= Imports ============================================
#if ALIB_C20_MODULES
import ALib.Strings.Search;
#else
#include "alib/strings/util/search.hpp"
#include "alib/strings/util/wildcardmatcher.hpp"
#if ALIB_FEAT_BOOST_REGEX && (!ALIB_CHARACTERS_WIDE || ALIB_CHARACTERS_NATIVE_WCHAR)
#include "alib/strings/util/regexmatcher.hpp"
#endif

#endif // ALIB_C20_MODULES
#endif // ALIB_STRINGS
#endif // H_ALIB_STRINGS_SEARCH
