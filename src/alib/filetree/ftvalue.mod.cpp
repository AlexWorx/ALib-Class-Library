//##################################################################################################
//  ALib C++ Framework
//
//  Copyright 2013-2026 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
//========================================= Global Fragment ========================================
module;
#include "alib/boxing/boxing.prepro.hpp"
#include "alib/filetree/filetree.prepro.hpp"
#if !defined (_WIN32)
#   include <pwd.h>
#   include <grp.h>
#endif
//============================================= Imports ============================================
module ALib.FileTree;
    import   ALib.Characters.Functions;
    import   ALib.Strings;
    import   ALib.Boxing;
#  if ALIB_EXPRESSIONS
    import   ALib.Expressions;
#  endif
//========================================== Implementation ========================================
ALIB_ALLOW_IMPLEMENTATION_INCLUSION
#include "ftvalue.cpp"
ALIB_POP_ALLOWANCE
