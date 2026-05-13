//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
#ifndef H_ALIB_CONTAINERS_STRINGTREE
#define H_ALIB_CONTAINERS_STRINGTREE
#pragma once
#ifndef HPP_ALIB
#   include "alib/alib.prepro.hpp"
#endif

#if ALIB_CONTAINERS && ALIB_STRINGS
//=========================================== Dependencies =========================================
#include "ALib.Containers.HashTable.H"
#include "ALib.Strings.H"

#include <iterator>
#include <cmath>
#include <algorithm>

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
import ALib.Containers.StringTree;
#else
#ifndef ALIB_GUARD_CONTAINERS_INIT
#    define ALIB_GUARD_CONTAINERS_INIT 1
#include "alib/containers/recycling.hpp"
#include "alib/containers/valuedescriptor.hpp"
#endif
#if ALIB_STRINGS
#include "alib/containers/detail/stringtreebase.hpp"
#include "alib/containers/stringtree.hpp"
#endif
#endif // ALIB_C20_MODULES
#endif // ALIB_CONTAINERS && ALIB_STRINGS
#endif // H_ALIB_CONTAINERS_STRINGTREE
