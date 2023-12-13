// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2023 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)
#if !defined(HPP_ALIB_SYSTEM_CONSOLE)
    #include "alib/system/console.hpp"
#endif

#if !defined(_GLIBCXX_CSTDLIB) && !defined(_CSTDLIB_)
#   include <cstdlib>
#endif

#if   defined( _WIN32 )
#elif defined(__GLIBCXX__)  || defined(__ANDROID_NDK__)
#   include <sys/ioctl.h>
#   include <unistd.h>
#elif defined(__APPLE__)
#   include <unistd.h>
#   include <sys/sysctl.h>
#else
#   pragma message ("Unknown Platform in file: " __FILE__ )
#endif
#endif // !defined(ALIB_DOX)


namespace aworx { namespace lib { namespace system {

// static instance representing current process
int    Console::lastReceivedWidth   = -1;


int Console::GetWidth( bool forceRedetect )
{
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


}}}// namespace [aworx::lib::system]

