//##################################################################################################
//  ALib C++ Framework
//
//  Copyright 2013-2026 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
//========================================= Global Fragment ========================================
module;
#include "alib/boxing/boxing.prepro.hpp"
#include "alib/camp/camp.prepro.hpp"
#include "alib/filetree/filetree.prepro.hpp"

//============================================= Imports ============================================
module ALib.FileTree;
    import   ALib.Lang;
    import   ALib.Characters.Functions;
    import   ALib.Strings;
    import   ALib.Boxing;
    import   ALib.Format;
#  if ALIB_EXPRESSIONS
    import   ALib.Expressions;
#  endif
    import   ALib.Camp;
    import   ALib.Camp.ResourceCompiler;

//========================================== Implementation ========================================
ALIB_ALLOW_IMPLEMENTATION_INCLUSION
#include "filetreecamp.cpp"
ALIB_POP_ALLOWANCE
