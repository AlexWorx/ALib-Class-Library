//##################################################################################################
//  ALib C++ Framework
//
//  Copyright 2013-2026 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
//========================================= Global Fragment ========================================
module;
#include "alib/strings/strings.prepro.hpp"
#include "alib/boxing/boxing.prepro.hpp"
#include "alib/variables/variables.prepro.hpp"
#include <fstream>
//============================================= Imports ============================================
module ALib.Variables.IniFile;
    import   ALib.EnumOps;
    import   ALib.Strings;
    import   ALib.Strings.Tokenizer;
    import   ALib.Strings.StdIOStream;
    import   ALib.Boxing;
    import   ALib.EnumRecords;
#  if ALIB_EXCEPTIONS
    import   ALib.Exceptions;
#  endif
    import   ALib.System;
    import   ALib.Variables;
#  if ALIB_CAMP
    import   ALib.Camp.Base;
#  endif
//========================================== Implementation ========================================
ALIB_ALLOW_IMPLEMENTATION_INCLUSION
#include "inifile.cpp"
ALIB_POP_ALLOWANCE
