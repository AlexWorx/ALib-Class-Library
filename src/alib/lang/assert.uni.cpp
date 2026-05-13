//##################################################################################################
//  ALib C++ Framework
//
//  Copyright 2013-2026 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
//========================================= Global Fragment ========================================
#include "alib_precompile.hpp"
#include "alib/strings/strings.prepro.hpp"
#include "alib/variables/variables.prepro.hpp"
#if ALIB_DEBUG
#   include <cstdlib>
#   include "alib/platform.prepro.hpp"
#   include <cassert>
#   include <cstring>
#   include <iostream>
#   include <iomanip>
#   include <string>
#   include <string>
#   include <stdexcept>
#   include <any>
#   if __has_include(<format>)
#      include <format>
#   else
#      include <fmt/format.h>
#   endif
#   include <codecvt>

#   if (ALIB_SINGLE_THREADED && ALIB_EXT_LIB_THREADS_AVAILABLE)
#       include <thread>
#   endif
#   if ALIB_DEBUG_ASSERTION_PRINTABLES
#       include <unordered_set>
#   endif

#   include <iostream>
#   include <iomanip>
#   include <typeindex>
#   include <functional>
#endif

//============================================= Imports ============================================
#   include    "ALib.Lang.H"
#   if ALIB_DEBUG
#      include "ALib.Bootstrap.H"
#      include "ALib.Threads.H"
#      include "ALib.Strings.H"
#      include "ALib.Strings.Token.H"
#      include "ALib.Boxing.H"
#      include "ALib.EnumRecords.H"
#      include "ALib.Resources.H"
#      include "ALib.Variables.H"
#      include "ALib.System.H"
#      include "ALib.ThreadModel.H"
#      include "ALib.Camp.Base.H"
#   endif

//========================================== Implementation ========================================
#include "assert.cpp"
