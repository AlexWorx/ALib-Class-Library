//##################################################################################################
//  ALib C++ Framework
//
//  Copyright 2013-2026 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
//========================================= Global Fragment ========================================
#include "alib_precompile.hpp"
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
#   include "ALib.Lang.H"
#   include "ALib.Threads.H"
#   include "ALib.Monomem.H"
#   include "ALib.Containers.HashTable.H"
#   include "ALib.Strings.H"
//========================================== Implementation ========================================
ALIB_ALLOW_IMPLEMENTATION_INCLUSION

#include "dbgcriticalsections.cpp"
#include "dbgtypedemangler.cpp"
#include "mainargs.cpp"

#include "../threads/thread.cpp"
#include "../threads/locks.cpp"
#include "../threads/promise.cpp"
#include "../threads/dbgasserters.cpp"

#include "../time/time.cpp"
#include "../time/datetime.cpp"
#include "../time/tickconverter.cpp"

#include "../characters/functions.cpp"

ALIB_POP_ALLOWANCE
