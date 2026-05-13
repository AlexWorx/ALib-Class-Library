//##################################################################################################
//  ALib C++ Framework
//
//  Copyright 2013-2026 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
//========================================= Global Fragment ========================================
module;
#include <vector>
#include "alib/alib.prepro.hpp"
//============================================= Imports ============================================
module ALib.Format.PropertyFormatter;
    import   ALib.Lang;
    import   ALib.Strings;
    import   ALib.Strings.Escaper;
    import   ALib.Exceptions;
#   if ALIB_CAMP
      import ALib.Camp.Base;
#   endif
//========================================== Implementation ========================================
ALIB_ALLOW_IMPLEMENTATION_INCLUSION
#include "propertyformatter.cpp"
ALIB_POP_ALLOWANCE
