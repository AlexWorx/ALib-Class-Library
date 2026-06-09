//##################################################################################################
//  ALib C++ Framework
//
//  Copyright 2013-2026 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
//========================================= Global Fragment ========================================
module;
#include "alib/system/system.prepro.hpp"
#include "alib/strings/strings.prepro.hpp"
#include "alib/boxing/boxing.prepro.hpp"
#include "alib/system/system.prepro.hpp"
#include <fstream>
#if !defined(_WIN32)
  #include <fcntl.h>
  #include <sys/stat.h>
  #if defined(ALIB_POSIX_MAPPED_FILES) && ALIB_POSIX_MAPPED_FILES > 0
    #include <sys/mman.h>
  #endif
#endif

//============================================= Imports ============================================
module ALib.System.MappedFile;
    import   ALib.System;
#  if ALIB_EXPRESSIONS
    import   ALib.Expressions;
#  endif
//========================================== Implementation ========================================
ALIB_ALLOW_IMPLEMENTATION_INCLUSION
#include "mappedfile.cpp"
ALIB_POP_ALLOWANCE
