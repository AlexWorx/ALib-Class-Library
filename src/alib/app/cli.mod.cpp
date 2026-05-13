//##################################################################################################
//  ALib C++ Framework
//
//  Copyright 2013-2026 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
//========================================= Global Fragment ========================================
module;
#include "alib/alib.prepro.hpp"
#include "alib/boxing/boxing.prepro.hpp"
#include <algorithm>
#include <vector>
//============================================= Imports ============================================
module ALib.App;
    import   ALib.Characters.Functions;
#  if ALIB_STRINGS
    import   ALib.Strings;
    import   ALib.Strings.Tokenizer;
    import   ALib.EnumRecords.Bootstrap;
#  endif
//========================================== Implementation ========================================
ALIB_ALLOW_IMPLEMENTATION_INCLUSION
#include "cli.cpp"
#include "cliargtypes.cpp"
#include "cliutil.cpp"
ALIB_POP_ALLOWANCE
