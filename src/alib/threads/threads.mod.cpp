//##################################################################################################
//  ALib C++ Framework
//
//  Copyright 2013-2026 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
//========================================= Global Fragment ========================================
module;
#include "alib/boxing/boxing.prepro.hpp"
#if !ALIB_SINGLE_THREADED
#   include <condition_variable>
#   include <atomic>
#endif

#if !ALIB_STRINGS
#   include <format>
#endif

#include <unordered_map>
#include <future>
#if ALIB_DEBUG
#   include <vector>
#   include <any>
#endif
//============================================= Imports ============================================
module   ALib.Threads;
    import   ALib.Lang;
    #  if ALIB_STRINGS
    import   ALib.Strings;
    #  endif
    #if ALIB_MONOMEM && ALIB_CONTAINERS
    import   ALib.Monomem;
    import   ALib.Containers.HashTable;
    #endif
//========================================== Implementation ========================================
ALIB_ALLOW_IMPLEMENTATION_INCLUSION
#include "thread.cpp"
#include "locks.cpp"
#include "promise.cpp"
#include "dbgasserters.cpp"
ALIB_POP_ALLOWANCE
