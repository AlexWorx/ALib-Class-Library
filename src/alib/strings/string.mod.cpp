//##################################################################################################
//  ALib C++ Framework
//
//  Copyright 2013-2026 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
//========================================= Global Fragment ========================================
module;
#include "alib/strings/strings.prepro.hpp"
#include <math.h>
//============================================= Imports ============================================
module ALib.Strings;
    import   ALib.Lang;
    import   ALib.Characters.Functions;
    #if ALIB_MONOMEM && ALIB_CONTAINERS
    import   ALib.Monomem;
    import   ALib.Containers.HashTable;
    #endif
//========================================== Implementation ========================================
ALIB_ALLOW_IMPLEMENTATION_INCLUSION
#include "string.cpp"
#include "substring.cpp"
#include "numberformat.cpp"
#include "detail/numberconversion.cpp"
ALIB_POP_ALLOWANCE
