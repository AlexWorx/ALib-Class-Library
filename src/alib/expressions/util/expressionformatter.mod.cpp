//##################################################################################################
//  ALib C++ Framework
//
//  Copyright 2013-2026 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
//========================================= Global Fragment ========================================
module;
#include "alib/expressions/expressions.prepro.hpp"

//============================================= Imports ============================================
module ALib.Expressions;
    import   ALib.Expressions.Impl;
    import   ALib.Characters.Functions;
    import   ALib.Strings;
    import   ALib.Strings.Escaper;
#   if ALIB_CAMP
      import ALib.Camp.Base;
#   endif
//========================================== Implementation ========================================
ALIB_ALLOW_IMPLEMENTATION_INCLUSION
#include "expressionformatter.cpp"
ALIB_POP_ALLOWANCE
