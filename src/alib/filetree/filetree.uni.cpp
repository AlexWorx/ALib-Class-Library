//##################################################################################################
//  ALib C++ Framework
//
//  Copyright 2013-2026 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
//========================================= Global Fragment ========================================
#include "alib_precompile.hpp"
#include "alib/alox/alox.prepro.hpp"
#include "alib/filetree/filetree.prepro.hpp"
#if !defined ( _WIN32 )
#   include <pwd.h>
#   include <grp.h>
#endif


//============================================= Imports ============================================
#   include "ALib.FileTree.H"
#   include "ALib.Containers.StringTreeIterator.H"
#   include "ALib.Strings.Calendar.H"
#if ALIB_DEBUG
#  include "ALib.Format.H"
#endif
#   include "ALib.Camp.ResourceCompiler.H"
#   include "ALib.ALox.H"
//========================================== Implementation ========================================
ALIB_ALLOW_IMPLEMENTATION_INCLUSION
#include "ftree.cpp"
#include "ftvalue.cpp"
#include "ftfile.cpp"
#include "filetreecamp.cpp"
ALIB_POP_ALLOWANCE
