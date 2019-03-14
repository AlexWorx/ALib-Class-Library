// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//
//  Precompiled Header for ALib.
//  Each ALib source file (compilation unit) includes this header first. However, it
//  the load of module dependent include headers, is not performed if compilation symbol
//  "ALIB_PRECOMPILED_HEADER_DISABLED" is passed with the compiler invocation.
// #################################################################################################


#if !defined(ALIB_PRECOMPILED_HEADER_DISABLED)

#   include "alib/lib/predef_modules.hpp"

#if ALIB_MODULE_SINGLETONS
#   define  ALIB_TEMP_HDRNAME "alib/singletons/singleton.hpp"
#   include ALIB_TEMP_HDRNAME
#   undef   ALIB_TEMP_HDRNAME
#endif

#if ALIB_MODULE_CHARACTERS
#   define  ALIB_TEMP_HDRNAME "alib/characters/chararray.hpp"
#   include ALIB_TEMP_HDRNAME
#   undef   ALIB_TEMP_HDRNAME

#   define  ALIB_TEMP_HDRNAME "alib/compatibility/std_characters.hpp"
#   include ALIB_TEMP_HDRNAME
#   undef   ALIB_TEMP_HDRNAME

#   if defined(QT_CORE_LIB)
#       define  ALIB_TEMP_HDRNAME "alib/compatibility/qt_characters.hpp"
#       include ALIB_TEMP_HDRNAME
#       undef   ALIB_TEMP_HDRNAME
#   endif
#endif

#if ALIB_MODULE_THREADS
#   define  ALIB_TEMP_HDRNAME "alib/threads/thread.hpp"
#   include ALIB_TEMP_HDRNAME
#   undef   ALIB_TEMP_HDRNAME
#   define  ALIB_TEMP_HDRNAME "alib/threads/smartlock.hpp"
#   include ALIB_TEMP_HDRNAME
#   undef   ALIB_TEMP_HDRNAME
#endif

#if ALIB_MODULE_TIME
#   define  ALIB_TEMP_HDRNAME "alib/time/datetime.hpp"
#   include ALIB_TEMP_HDRNAME
#   undef   ALIB_TEMP_HDRNAME
#   define  ALIB_TEMP_HDRNAME "alib/time/ticks.hpp"
#   include ALIB_TEMP_HDRNAME
#   undef   ALIB_TEMP_HDRNAME
#endif

#if ALIB_MODULE_MEMORY
#   define  ALIB_TEMP_HDRNAME "alib/memory/memoryblocks.hpp"
#   include ALIB_TEMP_HDRNAME
#   undef   ALIB_TEMP_HDRNAME
#endif

#if ALIB_MODULE_BOXING
#   define  ALIB_TEMP_HDRNAME "alib/boxing/boxing.hpp"
#   include ALIB_TEMP_HDRNAME
#   undef   ALIB_TEMP_HDRNAME
#endif

#if ALIB_MODULE_STRINGS
#   define  ALIB_TEMP_HDRNAME "alib/strings/astring.hpp"
#   include ALIB_TEMP_HDRNAME
#   undef   ALIB_TEMP_HDRNAME
#   define  ALIB_TEMP_HDRNAME "alib/strings/cstring.hpp"
#   include ALIB_TEMP_HDRNAME
#   undef   ALIB_TEMP_HDRNAME
#   define  ALIB_TEMP_HDRNAME "alib/strings/substring.hpp"
#   include ALIB_TEMP_HDRNAME
#   undef   ALIB_TEMP_HDRNAME
#endif

#if ALIB_MODULE_RESOURCES
#   define  ALIB_TEMP_HDRNAME "alib/resources/resources.hpp"
#   include ALIB_TEMP_HDRNAME
#   undef   ALIB_TEMP_HDRNAME
#   define  ALIB_TEMP_HDRNAME "alib/resources/enummetadata.hpp"
#   include ALIB_TEMP_HDRNAME
#   undef   ALIB_TEMP_HDRNAME
#   define  ALIB_TEMP_HDRNAME "alib/resources/enummetadataspec.hpp"
#   include ALIB_TEMP_HDRNAME
#   undef   ALIB_TEMP_HDRNAME
#endif

#if ALIB_MODULE_STRINGFORMAT
#   define  ALIB_TEMP_HDRNAME "alib/stringformat/formatterpythonstyle.hpp"
#   include ALIB_TEMP_HDRNAME
#   undef   ALIB_TEMP_HDRNAME
#   define  ALIB_TEMP_HDRNAME "alib/stringformat/text.hpp"
#   include ALIB_TEMP_HDRNAME
#   undef   ALIB_TEMP_HDRNAME
#endif

#if ALIB_MODULE_RESULTS
#   define  ALIB_TEMP_HDRNAME "alib/results/exception.hpp"
#   include ALIB_TEMP_HDRNAME
#   undef   ALIB_TEMP_HDRNAME
#endif

#if ALIB_MODULE_ALOX
#   define  ALIB_TEMP_HDRNAME "alib/alox.hpp"
#   include ALIB_TEMP_HDRNAME
#   undef   ALIB_TEMP_HDRNAME
#endif

#endif  //defined( ALIB_PRECOMPILED_HEADER_DISABLED)


