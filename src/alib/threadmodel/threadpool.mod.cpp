//##################################################################################################
//  ALib C++ Framework
//
//  Copyright 2013-2026 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
//========================================= Global Fragment ========================================
module;
#include "alib/strings/strings.prepro.hpp"
#if ALIB_DEBUG
#   include <any>
#endif
//============================================= Imports ============================================
module ALib.ThreadModel;
#  if ALIB_STRINGS
    import   ALib.Strings;
#  endif
    import   ALib.Boxing;
//========================================== Implementation ========================================
ALIB_ALLOW_IMPLEMENTATION_INCLUSION
#include "threadpool.cpp"
ALIB_POP_ALLOWANCE
