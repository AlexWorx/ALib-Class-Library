//##################################################################################################
//  ALib C++ Framework
//
//  Copyright 2013-2026 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
//========================================= Global Fragment ========================================
module;
#include "alib/strings/strings.prepro.hpp"
//============================================= Imports ============================================
module ALib.Strings.Token;
    import   ALib.Characters.Functions;
    import   ALib.EnumOps;
#  if ALIB_RESOURCES
     import   ALib.Resources;
#  endif
#   if ALIB_ENUMRECORDS
     import   ALib.EnumRecords;
#  endif
//========================================== Implementation ========================================
ALIB_ALLOW_IMPLEMENTATION_INCLUSION
#include "token.cpp"
ALIB_POP_ALLOWANCE
