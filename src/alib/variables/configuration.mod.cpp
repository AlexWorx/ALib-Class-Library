//##################################################################################################
//  ALib C++ Framework
//
//  Copyright 2013-2026 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
//========================================= Global Fragment ========================================
module;
#include "alib/boxing/boxing.prepro.hpp"
#include "alib/variables/variables.prepro.hpp"
//============================================= Imports ============================================
module ALib.Variables;
    import   ALib.EnumRecords;
    import   ALib.EnumRecords.Bootstrap;
    import   ALib.Boxing;
    import   ALib.Strings.StdFunctors;
#  if ALIB_RESOURCES
    import   ALib.Resources;
#  endif
    import   ALib.Variables.Plugins;
#  if ALIB_CAMP
    import   ALib.Camp;
    import   ALib.Camp.Base;
#  endif
//========================================== Implementation ========================================
ALIB_ALLOW_IMPLEMENTATION_INCLUSION
#include "configuration.cpp"
ALIB_POP_ALLOWANCE
