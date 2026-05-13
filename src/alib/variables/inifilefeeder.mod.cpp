//##################################################################################################
//  ALib C++ Framework
//
//  Copyright 2013-2026 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
//========================================= Global Fragment ========================================
module;
#include "alib/variables/variables.prepro.hpp"
#include <fstream>
//============================================= Imports ============================================
module ALib.Variables.IniFile;
    import   ALib.Containers.StringTree;
    import   ALib.Containers.StringTreeIterator;
    import   ALib.Strings.StdIOStream;
#  if ALIB_EXCEPTIONS
    import   ALib.Exceptions;
#  endif
    import   ALib.System;
    import   ALib.Variables;
#  if ALIB_FORMAT
    import   ALib.Format;
    import   ALib.Format.Paragraphs;
#  endif
//========================================== Implementation ========================================
ALIB_ALLOW_IMPLEMENTATION_INCLUSION
#include "inifilefeeder.cpp"
ALIB_POP_ALLOWANCE
