//##################################################################################################
//  ALib C++ Framework
//
//  Copyright 2013-2026 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
//========================================= Global Fragment ========================================
module;
#include "alib/variables/variables.prepro.hpp"
#include <cstdarg>

//============================================= Imports ============================================
module ALib.Variables.ResourcePool;
    import   ALib.Strings.StdIOStream;
    import   ALib.EnumRecords;
#  if ALIB_EXCEPTIONS
    import   ALib.Exceptions;
#  endif
    import   ALib.System;
#  if ALIB_VARIABLES
    import   ALib.Variables;
#  endif
//========================================== Implementation ========================================
ALIB_ALLOW_IMPLEMENTATION_INCLUSION
#include "configresourcepool.cpp"
ALIB_POP_ALLOWANCE
