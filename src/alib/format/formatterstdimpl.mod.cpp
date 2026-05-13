//##################################################################################################
//  ALib C++ Framework
//
//  Copyright 2013-2026 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
//========================================= Global Fragment ========================================
module;
#include <cmath>
#include "alib/alib.prepro.hpp"
#include "alib/strings/strings.prepro.hpp"
#include "alib/boxing/boxing.prepro.hpp"
//============================================= Imports ============================================
module ALib.Format.StdImplementation;
    import   ALib.Lang;
    import   ALib.EnumOps;
    import   ALib.Strings;
    import   ALib.Boxing;
    import   ALib.Exceptions;
#   if ALIB_CAMP
      import ALib.Camp.Base;
#   endif
//========================================== Implementation ========================================
ALIB_ALLOW_IMPLEMENTATION_INCLUSION
#include "formatterstdimpl.cpp"
ALIB_POP_ALLOWANCE
