//##################################################################################################
//  ALib C++ Framework
//
//  Copyright 2013-2026 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
//========================================= Global Fragment ========================================
module;
#include "alib/boxing/boxing.prepro.hpp"
#include "alib/expressions/expressions.prepro.hpp"
#include <vector>
#include <stack>

//============================================= Imports ============================================
module ALib.Expressions.Impl;
    import   ALib.Characters.Functions;
    import   ALib.Strings;
    import   ALib.Boxing;
#if ALIB_DEBUG
    import   ALib.Format.Paragraphs;
#endif
//========================================== Implementation ========================================
ALIB_ALLOW_IMPLEMENTATION_INCLUSION
#include "virtualmachine.cpp"
ALIB_POP_ALLOWANCE
