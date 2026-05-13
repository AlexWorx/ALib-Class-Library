namespace   alib::system {

// static instance representing current process
int    Console::lastReceivedWidth   = -1;


int Console::GetWidth( bool forceRedetect, int defaultWidth ) {
    if( Console::lastReceivedWidth > 0  && !forceRedetect )
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

    return Console::lastReceivedWidth > 0 ? Console::lastReceivedWidth : defaultWidth;
}

} // namespace [  alib::system]
