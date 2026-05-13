//##################################################################################################
//  ALib C++ Framework
//
//  Copyright 2013-2026 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software lbicense, see LICENSE.txt)
//##################################################################################################
module;
#include <oneapi/tbb/partitioner.h>

//========================================= Global Fragment ========================================

#include "alib/strings/strings.prepro.hpp"
#include "alib/system/system.prepro.hpp"
#include "alib/filetree/filetree.prepro.hpp"
#include "alib/alox/alox.prepro.hpp"
#include <vector>
#if  ALIB_SYSTEM_FILE_STATUS_IMPL == ALIB_SYSTEM_FILE_POSIX_STATUS
#   include <unistd.h>
#   if defined(__linux__)
#       include <asm/unistd.h>
#   endif
#   include <dirent.h>
#   if defined(__linux__)
#       include <linux/stat.h>
#       include <linux/magic.h>
#   endif
#   include <sys/stat.h>
#   include <sys/vfs.h>
#   if !defined(__APPLE__)
#       include <sys/sysmacros.h>
#   else
#       include <sys/types.h>
#       include <sys/mount.h>
#   endif
#   include <pwd.h>
#   include <fcntl.h>
#   include <pwd.h>
#   include <grp.h>
#else
#   include <filesystem>
    namespace fs = std::filesystem;
#endif
//============================================= Imports ============================================
module ALib.FileTree;
    import   ALib.Lang;
    import   ALib.Characters.Functions;
    import   ALib.Monomem;
    import   ALib.Strings;
    import   ALib.Strings.Tokenizer;
    import   ALib.System;
#  if ALIB_EXPRESSIONS
    import   ALib.Expressions;
#  endif
#  if ALIB_ALOX
    import   ALib.ALox;
    import   ALib.ALox.Impl;
#  endif
//========================================== Implementation ========================================
ALIB_ALLOW_IMPLEMENTATION_INCLUSION
#include "fscanner.cpp"
ALIB_POP_ALLOWANCE
