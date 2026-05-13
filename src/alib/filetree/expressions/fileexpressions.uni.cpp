//##################################################################################################
//  ALib C++ Framework
//
//  Copyright 2013-2026 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
//========================================= Global Fragment ========================================
#include "alib_precompile.hpp"
#include "alib/system/system.prepro.hpp"
#include "alib/expressions/expressions.prepro.hpp"
#include "alib/filetree/filetree.prepro.hpp"
#if ALIB_SYSTEM_FILE_STATUS_IMPL == ALIB_SYSTEM_FILE_POSIX_STATUS
#   include <unistd.h>
#endif

//============================================= Imports ============================================
#   include "ALib.Lang.H"
#   include "ALib.Characters.Functions.H"
#   include "ALib.Strings.H"
#   include "ALib.Strings.Token.H"
#   include "ALib.System.H"
#   include "ALib.Expressions.H"
#   include "ALib.System.H"
#   include "ALib.FileTree.H"
#   include "ALib.FileTree.Expressions.H"
//========================================== Implementation ========================================
ALIB_ALLOW_IMPLEMENTATION_INCLUSION
#include "fileexpressions.cpp"
ALIB_POP_ALLOWANCE
