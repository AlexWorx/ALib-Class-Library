//##################################################################################################
//  ALib C++ Framework
//
//  Copyright 2013-2026 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
//========================================= Global Fragment ========================================
module;
#include "alib/alox/alox.prepro.hpp"
#include "alib/filetree/filetree.prepro.hpp"
#if !defined ( _WIN32 )
#   include <pwd.h>
#   include <grp.h>
#endif


//============================================= Imports ============================================
module ALib.FileTree;
    import   ALib.Lang;
    import   ALib.Characters.Functions;
    import   ALib.Strings;
    import   ALib.Containers.StringTreeIterator;
#  if ALIB_ALOX
    import   ALib.ALox;
    import   ALib.ALox.Impl;
#  endif
#  if ALIB_EXPRESSIONS
    import   ALib.Expressions;
#  endif
#  if ALIB_DEBUG
    import   ALib.Format;
#  endif
//========================================== Implementation ========================================
ALIB_ALLOW_IMPLEMENTATION_INCLUSION
#include "ftree.cpp"
ALIB_POP_ALLOWANCE
