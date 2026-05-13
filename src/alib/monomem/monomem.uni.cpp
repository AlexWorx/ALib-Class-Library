//##################################################################################################
//  ALib C++ Framework
//
//  Copyright 2013-2026 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
//========================================= Global Fragment ========================================
#include "alib_precompile.hpp"
#include "alib/monomem/monomem.prepro.hpp"
#if ALIB_DEBUG_MEMORY
#   include <atomic>
#endif
#if ALIB_DEBUG_MEMORY && ALIB_DEBUG_ALLOCATIONS
#   pragma message "Configuration Macros ALIB_DEBUG_MEMORY and ALIB_DEBUG_ALLOCATIONS are set in parallel. Statistics on allocatoions will be fairly wrong."
#endif

#include <cstring>
//============================================= Imports ============================================
#   include "ALib.Monomem.H"
//========================================== Implementation ========================================
#include "monomem.cpp"
