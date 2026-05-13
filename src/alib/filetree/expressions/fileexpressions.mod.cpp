//##################################################################################################
//  ALib C++ Framework
//
//  Copyright 2013-2026 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
//========================================= Global Fragment ========================================
module;
#include "alib/system/system.prepro.hpp"
#include "alib/expressions/expressions.prepro.hpp"
#include "alib/filetree/filetree.prepro.hpp"
#if ALIB_SYSTEM_FILE_STATUS_IMPL == ALIB_SYSTEM_FILE_POSIX_STATUS
#   include <unistd.h>
#endif

//============================================= Imports ============================================
module ALib.FileTree.Expressions;
    import   ALib.Lang;
    import   ALib.Characters.Functions;
    import   ALib.Strings;
    import   ALib.Strings.Token;
    import   ALib.System;
    import   ALib.Expressions;
//========================================== Implementation ========================================
ALIB_ALLOW_IMPLEMENTATION_INCLUSION
#include "fileexpressions.cpp"
ALIB_POP_ALLOWANCE
