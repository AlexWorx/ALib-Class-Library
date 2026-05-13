//##################################################################################################
//  ALib C++ Framework
//
//  Copyright 2013-2026 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
//========================================= Global Fragment ========================================
module;
#include "alib/boxing/boxing.prepro.hpp"
#if ALIB_MONOMEM && ALIB_CONTAINERS
#include "alib/boxing/boxing.prepro.hpp"
#endif
#if ALIB_DEBUG && (!ALIB_MONOMEM || !ALIB_CONTAINERS )
#   include <unordered_map>
#   include <unordered_set>
#endif
#if ALIB_DEBUG_BOXING
#   include <vector>
#endif
#include <cstring>
// this is non-exported (non module) cpp debug check code
#include "dbgdoublelengthcheck.cpp"

//============================================= Imports ============================================
module ALib.Boxing;
#  if ALIB_DEBUG && ALIB_MONOMEM && ALIB_CONTAINERS
    import   ALib.Monomem;
    import   ALib.Containers.HashTable;
#  endif

//========================================== Implementation ========================================
ALIB_ALLOW_IMPLEMENTATION_INCLUSION
#include "vtable.cpp"
#include "boxingcustoms.cpp"
ALIB_POP_ALLOWANCE
