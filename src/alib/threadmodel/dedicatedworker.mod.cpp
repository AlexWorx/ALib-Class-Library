//##################################################################################################
//  ALib C++ Framework
//
//  Copyright 2013-2026 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
//========================================= Global Fragment ========================================
module;
#include "alib/alib.prepro.hpp"
#if ALIB_DEBUG
#   include <vector>
#   include <any>
#endif
#include <algorithm>
//============================================= Imports ============================================
module ALib.ThreadModel;
//========================================== Implementation ========================================
ALIB_ALLOW_IMPLEMENTATION_INCLUSION
#include "dedicatedworker.cpp"
ALIB_POP_ALLOWANCE
