//##################################################################################################
//  ALib C++ Framework
//
//  Copyright 2013-2026 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
//========================================= Global Fragment ========================================
#include "alib_precompile.hpp"
#include "alib/strings/strings.prepro.hpp"
#include "alib/boxing/boxing.prepro.hpp"
#include "alib/system/system.prepro.hpp"


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
#   if !defined(__APPLE__)
#       include <sys/vfs.h>
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


#if !DOXYGEN
#   include <cstdlib>
#   if defined( _WIN32 )
#      include <direct.h>
#   elif defined (__GLIBCXX__) || defined(_LIBCPP_VERSION) || defined(__APPLE__) || defined(__ANDROID_NDK__)
#      include <unistd.h>
#   else
#      pragma message ("Unknown Platform in file: " __FILE__ )
#   endif
#   if ALIB_SYSTEM_FILE_STATUS_IMPL == ALIB_SYSTEM_FILE_POSIX_STATUS
#       if defined(__GLIBCXX__) || defined(_LIBCPP_VERSION) || defined(__ANDROID_NDK__)
#          include <sys/ioctl.h>
#       elif defined(__APPLE__)
#          include <sys/sysctl.h>
#       endif
#       if defined(__APPLE__)
#          include <libproc.h>
#       endif
#   endif
#   include <fstream>
#endif // !DOXYGEN

#if !defined(_WIN32)
  #include <fcntl.h>
  #include <sys/stat.h>
  #if defined(ALIB_POSIX_MAPPED_FILES) && ALIB_POSIX_MAPPED_FILES > 0
    #include <sys/mman.h>
  #endif
#else
#   include <sys/stat.h>
#endif


//============================================= Imports ============================================
#   include "ALib.Lang.H"
#   include "ALib.Threads.H"
#   include "ALib.Strings.StdIOStream.H"
#   include "ALib.System.H"
#   include "ALib.System.MappedFile.H"
//========================================== Implementation ========================================
ALIB_ALLOW_IMPLEMENTATION_INCLUSION
#include "console.cpp"
#include "environment.cpp"
#include "processinfo.cpp"
#include "shellcommand.cpp"
#include "path.cpp"
#include "filestatus.cpp"
#include "mappedfile.cpp"
ALIB_POP_ALLOWANCE
