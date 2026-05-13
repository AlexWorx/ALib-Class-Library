//##################################################################################################
//  ALib C++ Framework
//
//  Copyright 2013-2026 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
//========================================= Global Fragment ========================================
#include "alib_precompile.hpp"
#include "alib/singletons/singletons.prepro.hpp"
#if !ALIB_MONOMEM || !ALIB_CONTAINERS
#    include <unordered_map>
#endif
#if !ALIB_MONOMEM && !ALIB_SINGLE_THREADED
#    include <mutex>
#endif
#include <cstring>
//============================================= Imports ============================================
#   include "ALib.Monomem.H"
#   include "ALib.Singletons.H"
//========================================== Implementation ========================================
#include "singletons.cpp"
