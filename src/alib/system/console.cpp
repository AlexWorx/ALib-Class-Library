//##################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
#include "alib_precompile.hpp"
#if !defined(ALIB_C20_MODULES) || ((ALIB_C20_MODULES != 0) && (ALIB_C20_MODULES != 1))
#   error "Symbol ALIB_C20_MODULES has to be given to the compiler as either 0 or 1"
#endif
#if ALIB_C20_MODULES
    module;
#endif
//========================================= Global Fragment ========================================
#include "alib/system/system.prepro.hpp"

#if !DOXYGEN
#   include <cstdlib>
#   if   defined( _WIN32 )
#   elif defined(__GLIBCXX__) || defined(_LIBCPP_VERSION) || defined(__ANDROID_NDK__)
#      include <sys/ioctl.h>
#      include <unistd.h>
#   elif defined(__APPLE__)
#      include <unistd.h>
#      include <sys/sysctl.h>
#   else
#      pragma message ("Unknown Platform in file: " __FILE__ )
#   endif
#endif // !DOXYGEN
//============================================== Module ============================================
#if ALIB_C20_MODULES
    module ALib.System;
#else
#   include "ALib.System.H"
#endif
//========================================== Implementation ========================================
namespace   alib::system {

// static instance representing current process
int    Console::lastReceivedWidth   = -1;


int Console::GetWidth( bool forceRedetect ) {
    if( Console::lastReceivedWidth >= 0  && !forceRedetect )
        return Console::lastReceivedWidth;


    #if defined (_WIN32)

    #elif defined(__GLIBC__) && defined(__unix__)  || defined(__ANDROID_NDK__)

        struct winsize w;
        if( ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) != 0 )
            Console::lastReceivedWidth= -1;
        else
            Console::lastReceivedWidth= w.ws_col;

    #elif defined (__APPLE__)

    #else
        #pragma message ("Unknown Platform in file: " __FILE__ )
    #endif

    return Console::lastReceivedWidth;
}

} // namespace [  alib::system]
