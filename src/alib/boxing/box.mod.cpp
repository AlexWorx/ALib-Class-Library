//##################################################################################################
//  ALib C++ Framework
//
//  Copyright 2013-2026 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
//========================================= Global Fragment ========================================
module;
#include "alib/boxing/boxing.prepro.hpp"
#if ALIB_DEBUG_BOXING
#   if !ALIB_MONOMEM
#       include <unordered_map>
#       include <unordered_set>
#   endif
#   include <vector>
#   include <algorithm>
#endif // ALIB_DEBUG_BOXING
#include <cmath>
#include <functional>
#include <cstring>
#include <typeindex>

//============================================= Imports ============================================
module ALib.Boxing;
    import   ALib.Lang;
#  if ALIB_MONOMEM
    import   ALib.Monomem;
#  endif
//========================================== Implementation ========================================
ALIB_ALLOW_IMPLEMENTATION_INCLUSION
#include "box.cpp"
#include "boxing_debug.cpp"
ALIB_POP_ALLOWANCE
