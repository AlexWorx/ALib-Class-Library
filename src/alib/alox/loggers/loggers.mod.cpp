//##################################################################################################
//  ALib C++ Framework
//
//  Copyright 2013-2026 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
//========================================= Global Fragment ========================================
module;
#include "alib/strings/strings.prepro.hpp"
#include "alib/alox/alox.prepro.hpp"
#include <fstream>
#include <iostream>
#include <ostream>
#include <string.h>

//============================================= Imports ============================================
module ALib.ALox.Impl;
    import   ALib.Lang;
    import   ALib.Characters.Functions;
    import   ALib.Strings;
    import   ALib.Strings.Tokenizer;
    import   ALib.Camp;
    import   ALib.Camp.Base;
//========================================== Implementation ========================================
ALIB_ALLOW_IMPLEMENTATION_INCLUSION
#include "ansilogger.cpp"
#include "consolelogger.cpp"
#include "textfilelogger.cpp"
#include "vstudiologger.cpp"
#if defined( _WIN32 )
#include "windowsconsolelogger.cpp"
#endif
ALIB_POP_ALLOWANCE
