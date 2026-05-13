//##################################################################################################
//  ALib C++ Framework
//
//  Copyright 2013-2026 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
//========================================= Global Fragment ========================================
module;
#include "alib/resources/resources.prepro.hpp"
#include <cstdarg>
#if ALIB_DEBUG_RESOURCES
#   include <vector>
#   include <algorithm>
#   include <iostream>
#endif
//============================================= Imports ============================================
module ALib.Resources;
    import   ALib.Lang;
#  if ALIB_STRINGS
        import   ALib.Strings;
#  endif
#if ALIB_DEBUG_RESOURCES
    import   ALib.Strings.StdIOStream;
#endif
//========================================== Implementation ========================================
ALIB_ALLOW_IMPLEMENTATION_INCLUSION
#include "localresourcepool.cpp"
ALIB_POP_ALLOWANCE
