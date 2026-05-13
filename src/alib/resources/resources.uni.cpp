//##################################################################################################
//  ALib C++ Framework
//
//  Copyright 2013-2026 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
//========================================= Global Fragment ========================================
#include "alib_precompile.hpp"
#include "alib/resources/resources.prepro.hpp"
#include <cstdarg>
#if ALIB_DEBUG_RESOURCES
#   include <vector>
#   include <algorithm>
#endif
//============================================= Imports ============================================
#if ALIB_DEBUG_RESOURCES
#   include "ALib.Strings.StdIOStream.H"
#endif
#   include "ALib.EnumRecords.Bootstrap.H"
#   include "ALib.Resources.H"
//========================================== Implementation ========================================
#include "localresourcepool.cpp"
#include "extensions/strings_loadtokens.cpp"
