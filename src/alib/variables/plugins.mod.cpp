//##################################################################################################
//  ALib C++ Framework
//
//  Copyright 2013-2026 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
//========================================= Global Fragment ========================================
module;
#include "alib/variables/variables.prepro.hpp"
//============================================= Imports ============================================
module ALib.Variables.Plugins;
    import   ALib.Lang;
#  if ALIB_VARIABLES
    import   ALib.Variables;
#  endif
#  if ALIB_SYSTEM
    import   ALib.System;
#  endif
#  if ALIB_CAMP
    import   ALib.Camp.Base;
#  endif
//========================================== Implementation ========================================
ALIB_ALLOW_IMPLEMENTATION_INCLUSION
#include "plugins.cpp"
ALIB_POP_ALLOWANCE
