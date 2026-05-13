//##################################################################################################
//  ALib C++ Framework
//
//  Copyright 2013-2026 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
//========================================= Global Fragment ========================================
module;
#include "alib/strings/strings.prepro.hpp"
#include "alib/boxing/boxing.prepro.hpp"
#include "alib/system/system.prepro.hpp"
#if !DOXYGEN
#   if defined ( _WIN32 )
#      include <direct.h>
#   elif   defined(__GLIBCXX__) || defined(_LIBCPP_VERSION) || defined(__APPLE__)    || defined(__ANDROID_NDK__)
#      include <unistd.h>
#      include <dirent.h>
#      include <sys/stat.h>
#      include <pwd.h>
#   else
#      pragma message ("Unknown Platform in file: " __FILE__ )
#   endif
#   include <fstream>
#endif // !DOXYGEN
//============================================= Imports ============================================
module ALib.System;
#  if ALIB_STRINGS
    import   ALib.Strings;
#  endif
    import   ALib.Strings.StdIOStream;
#  if ALIB_BOXING
    import   ALib.Boxing;
#  endif
//========================================== Implementation ========================================
ALIB_ALLOW_IMPLEMENTATION_INCLUSION
#include "path.cpp"
ALIB_POP_ALLOWANCE
