//##################################################################################################
//  ALib C++ Framework
//
//  Copyright 2013-2026 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
//========================================= Global Fragment ========================================
module;
#include "alib/boxing/boxing.prepro.hpp"
#include "alib/system/system.prepro.hpp"
#include <filesystem>
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
    namespace fs = std::filesystem;

    // Note: MacOS is currently (as of 231210) missing C++20 library features in the area of std::clock
    #if  defined(__APPLE__)  || defined(_LIBCPP_VERSION) || defined(__ANDROID_NDK__)
    namespace
    {
        template <typename TP>
        std::time_t to_time_t(TP tp)
        {
            auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(tp - TP::clock::now()
                            + std::chrono::system_clock::now());
            return std::chrono::system_clock::to_time_t(sctp);
        }
    }
    #endif
#endif

#if !defined (_WIN32)
#   include <pwd.h>
#   include <grp.h>
#endif

//============================================= Imports ============================================
module ALib.System;
    import   ALib.Characters.Functions;
    import   ALib.Strings;
    import   ALib.Boxing;
#  if ALIB_EXPRESSIONS
    import   ALib.Expressions;
#  endif
//========================================== Implementation ========================================
ALIB_ALLOW_IMPLEMENTATION_INCLUSION
#include "filestatus.cpp"
ALIB_POP_ALLOWANCE
