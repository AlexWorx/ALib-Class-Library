// #################################################################################################
//  ALib C++ Library
//
//  Define code selection symbols for ALib features from given compiler symbols.
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#ifndef HPP_ALIB_LIB_PREDEF_PLATFORM
#define HPP_ALIB_LIB_PREDEF_PLATFORM 1


#if !defined(HPP_ALIB_LIB_PREDEF_MODULES)
#   include "alib/lib/predef_modules.hpp"
#endif

ALIB_ASSERT_FILESET(PREDEF_PF)

#if !defined(HPP_ALIB_LIB_PREDEF_COMPILER)
#   include "alib/lib/predef_compiler.hpp"
#endif


// #################################################################################################
// basic system headers
// #################################################################################################

// GNUC specific
#if defined( __GNUC__ )


// Windows specific
#elif defined ( _MSC_VER )

    //! @cond NO_DOX
    // Include windows.h. but exclude rarely-used stuff from Windows headers. If these things are
    // needed by the software that incorporates ALib, then windows has to be
    // included before this header file is.
#   if !defined (_INC_WINDOWS)
#      define WIN32_LEAN_AND_MEAN
#      define NOGDI
#      define NOSERVICE
#      define NOMCX
#      define NOIME
#      define NOSOUND
#      define NOCOMM
#      define NOKANJI
#      define NORPC
#      define NOPROXYSTUB
#      define NOIMAGE
#      define NOTAPE
#      include <windows.h>
#   endif
    //! @endcond

#   if !defined (_STDINT)
#      include <stdint.h>
#   endif

// other platforms not tested, yet
#else
#   pragma message ("Unknown Platform in file: " __FILE__ )
#endif


// #################################################################################################
// Size of wchar_t
// #################################################################################################
#if defined(__WCHAR_MAX__)
    #if  __WCHAR_MAX__ == 0x7FFFFFFF
        #define     ALIB_SIZEOF_WCHAR_T    4
    #else
        #define     ALIB_SIZEOF_WCHAR_T    2
    #endif
#elif defined(_MSC_VER)
    #define         ALIB_SIZEOF_WCHAR_T    2
#elif
    #define         ALIB_SIZEOF_WCHAR_T    4 // guessing, will be checked below
#endif

static_assert( sizeof(wchar_t) == ALIB_SIZEOF_WCHAR_T, "Error: Platform not supported" );



#endif // HPP_ALIB_LIB_PREDEF_PLATFORM
