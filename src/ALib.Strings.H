//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
#ifndef H_ALIB_STRINGS
#define H_ALIB_STRINGS
#pragma once
#ifndef HPP_ALIB
#   include "alib/alib.prepro.hpp"
#endif

#if ALIB_STRINGS
//=========================================== Dependencies =========================================
#include "alib/strings/strings.prepro.hpp"
#include "alib/platform.prepro.hpp"

#include "ALib.EnumOps.H"
#include "ALib.Characters.Functions.H"
#include "ALib.Threads.H"
#if ALIB_MONOMEM
#   include "ALib.Monomem.H"
#endif

#include <iterator>
#include <cmath>
#include <algorithm>
#include <cstring>

#if defined( _WIN32 ) || defined(__APPLE__)
#include <clocale>
#endif
#if defined(_WIN32)
#include <intrin.h>
#endif
#include <clocale>
#include <limits>
#include <vector>
#include <algorithm>

//============================================= Imports ============================================
#if ALIB_C20_MODULES
import ALib.Strings;
#else
#include "alib/strings/string.hpp"
#include "alib/strings/cstring.hpp"
#include "alib/strings/stringnzt.hpp"

#include "alib/strings/detail/tastring.hpp"
#include "alib/strings/appendables.hpp"
#include "alib/strings/format.hpp"
#include "alib/strings/astring.hpp"

#include "alib/strings/substring.hpp"
#include "alib/strings/localstring.hpp"

#include "alib/strings/numberformat.hpp"
#include "alib/strings/detail/numberconversion.hpp"

#if ALIB_MONOMEM
#   include "alib/strings/monomem/strings_monomem.hpp"
#endif


#endif // ALIB_C20_MODULES
#endif // ALIB_STRINGS
#endif // H_ALIB_STRINGS
