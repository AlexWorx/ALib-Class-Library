//##################################################################################################
//  ALib C++ Framework
//
//  Copyright 2013-2026 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
//========================================= Global Fragment ========================================
module;
#include "alib/alib.prepro.hpp"
//============================================= Imports ============================================
module ALib.Time;
    #if ALIB_BOXING
      import    ALib.Boxing;
    #endif

//========================================== Implementation ========================================
ALIB_ALLOW_IMPLEMENTATION_INCLUSION
#include "time.cpp"
#include "datetime.cpp"
#include "tickconverter.cpp"
ALIB_POP_ALLOWANCE
