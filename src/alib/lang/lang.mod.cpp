//##################################################################################################
//  ALib C++ Framework
//
//  Copyright 2013-2026 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
//========================================= Global Fragment ========================================
module;
#include "alib/alib.prepro.hpp"
#if ALIB_DEBUG && !DOXYGEN
#   if defined(__GNUC__) || defined(__clang__)
#      include <cxxabi.h>
#      include <cassert>
#   endif
#   include <cstdlib>
#   if defined(_WIN32)
#      include <cstring>
#   endif
#endif
//============================================= Imports ============================================
module ALib.Lang;
//========================================== Implementation ========================================
ALIB_ALLOW_IMPLEMENTATION_INCLUSION
#include "dbgcriticalsections.cpp"
#include "dbgtypedemangler.cpp"
#include "mainargs.cpp"
ALIB_POP_ALLOWANCE
