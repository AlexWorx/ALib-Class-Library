//##################################################################################################
//  ALib C++ Framework
//
//  Copyright 2013-2026 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
//========================================= Global Fragment ========================================
module;
#include <vector>
#include <algorithm>
#include <any>
#include "alib/boxing/boxing.prepro.hpp"
//============================================= Imports ============================================
module ALib.Exceptions;
    import   ALib.Lang;
    import   ALib.Strings;
    import   ALib.Strings.Tokenizer;
    import   ALib.Boxing;
    import   ALib.EnumRecords;
    import   ALib.EnumRecords.Bootstrap;
#  if ALIB_SYSTEM
    import   ALib.System;
#  endif
#  if ALIB_FORMAT
    import   ALib.Format;
    import   ALib.Format.Paragraphs;
#  endif
#  if ALIB_CAMP
    import   ALib.Camp.Base;
#  endif
//========================================== Implementation ========================================
ALIB_ALLOW_IMPLEMENTATION_INCLUSION
#include "exception.cpp"
ALIB_POP_ALLOWANCE
