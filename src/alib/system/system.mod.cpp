//##################################################################################################
//  ALib C++ Framework
//
//  Copyright 2013-2026 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
//========================================= Global Fragment ========================================
module;
#include "alib/strings/strings.prepro.hpp"
#include "alib/system/system.prepro.hpp"

#if !DOXYGEN
#   include <cstdlib>
#   if defined (__GLIBCXX__) || defined(_LIBCPP_VERSION) || defined(__APPLE__) || defined(__ANDROID_NDK__)
#      include <unistd.h>
#   elif defined( _WIN32 )
#      include <direct.h>
#   else
#      pragma message ("Unknown Platform in file: " __FILE__ )
#   endif
#   if defined(__GLIBCXX__) || defined(_LIBCPP_VERSION) || defined(__ANDROID_NDK__)
#      include <sys/ioctl.h>
#   elif defined(__APPLE__)
#      include <sys/sysctl.h>
#   endif
#   if defined(__APPLE__)
#      include <libproc.h>
#   endif
#   include <fstream>
#endif // !DOXYGEN

//============================================= Imports ============================================
module ALib.System;
    import   ALib.Lang;
    import   ALib.Threads;
    import   ALib.Strings;
    import   ALib.Strings.Tokenizer;
#if ALIB_MONOMEM
    import   ALib.Monomem;
#endif
//========================================== Implementation ========================================
ALIB_ALLOW_IMPLEMENTATION_INCLUSION
#include "console.cpp"
#include "environment.cpp"
#include "processinfo.cpp"
#include "shellcommand.cpp"
ALIB_POP_ALLOWANCE
