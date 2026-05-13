//##################################################################################################
//  ALib C++ Framework
//
//  Copyright 2013-2026 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
//========================================= Global Fragment ========================================
#include "alib_precompile.hpp"
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
#   include "ALib.Lang.H"
#   include "ALib.Boxing.H"
#   if ALIB_DEBUG && ALIB_MONOMEM && ALIB_CONTAINERS
#      include "ALib.Monomem.H"
#      include "ALib.Containers.HashTable.H"
#   endif
//========================================== Implementation ========================================
ALIB_ALLOW_IMPLEMENTATION_INCLUSION
#include "box.cpp"
#include "boxing_debug.cpp"
#include "boxes.cpp"
#include "detail/vtable.cpp"
#include "detail/boxingcustoms.cpp"
#include "detail/dbgdoublelengthcheck.cpp"
#include "extensions/strings_tfield.cpp"

ALIB_POP_ALLOWANCE
