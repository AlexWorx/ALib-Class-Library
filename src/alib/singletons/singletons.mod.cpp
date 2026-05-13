//##################################################################################################
//  ALib C++ Framework
//
//  Copyright 2013-2026 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
//========================================= Global Fragment ========================================
module;
#include "alib/singletons/singletons.prepro.hpp"
#if !ALIB_MONOMEM || !ALIB_CONTAINERS
#    include <unordered_map>
#endif
#if !ALIB_MONOMEM && !ALIB_SINGLE_THREADED
#    include <mutex>
#endif
#include <cstring>
//============================================= Imports ============================================
module ALib.Singletons;
    import   ALib.Lang;
#  if ALIB_MONOMEM
    import   ALib.Monomem;
#  endif
//========================================== Implementation ========================================
ALIB_ALLOW_IMPLEMENTATION_INCLUSION
#include "singletons.cpp"
ALIB_POP_ALLOWANCE
