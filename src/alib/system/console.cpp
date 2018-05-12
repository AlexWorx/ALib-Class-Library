// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib.hpp"


#if !defined(HPP_ALIB_SYSTEM_CONSOLE)
    #include "alib/system/console.hpp"
#endif

#if defined(__GLIBCXX__)
#   include <sys/ioctl.h>
#   include <unistd.h>
#elif defined(__APPLE__)
#   include <unistd.h>
#   include <sys/sysctl.h>
#elif   defined( _WIN32 )
#else
#   pragma message ("Unknown Platform in file: " __FILE__ )
#endif


namespace aworx { namespace lib { namespace system {

// static instance representing current process
int    Console::lastReceivedWidth   = -1;


int Console::GetWidth( bool forceRedetect )
{
    if( Console::lastReceivedWidth >= 0  && !forceRedetect )
        return Console::lastReceivedWidth;


    #if defined(__GLIBC__) && defined(__unix__)

        struct winsize w;
        if( ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) != 0 )
            Console::lastReceivedWidth= -1;
        else
            Console::lastReceivedWidth= w.ws_col;

    #elif defined (__APPLE__)

    #elif defined (_WIN32)

    #else
        #pragma message ("Unknown Platform in file: " __FILE__ )
    #endif

    return Console::lastReceivedWidth;
}


}}}// namespace [aworx::lib::system]

