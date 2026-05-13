//##################################################################################################
//  ALib C++ Framework
//
//  Copyright 2013-2026 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
//========================================= Global Fragment ========================================
module;
#include "alib/monomem/monomem.prepro.hpp"
#include "alib/strings/strings.prepro.hpp"
#if ALIB_MONOMEM && ALIB_DEBUG
#   include <cstring>
#endif
//============================================= Imports ============================================
module ALib.Strings;
    import   ALib.Lang;
    import   ALib.Monomem;

//========================================== Implementation ========================================
ALIB_ALLOW_IMPLEMENTATION_INCLUSION
#include "strings_monomem.cpp"
ALIB_POP_ALLOWANCE
