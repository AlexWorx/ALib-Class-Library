//==================================================================================================
/// \file
/// This header file is part of the \aliblong.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
///
/// Precompiled Header for ALib.
/// Each ALib <c>.cpp</c> compilation unit includes this header before doing anything else.
///
/// Compiler symbol \ref ALIB_PRECOMPILED_HEADER may be used to \e enable this header.
/// While still included, the header itself will not include anything if this variable is not
/// defined or equals \c 0.<br>
/// As of today, with GNU and clang compilers, compilation seems more efficient if this header is
/// disabled.
//==================================================================================================
#if !defined(DOXYGEN)
#include "alib/alib.hpp"

#if defined(ALIB_PRECOMPILED_HEADER) && ALIB_PRECOMPILED_HEADER

#include "alib/lang/platformincludes.hpp"

#if ALIB_CHARACTERS
#   define  ALIB_TEMP_HDRNAME "alib/characters/characters.hpp"
#   include ALIB_TEMP_HDRNAME
#   undef   ALIB_TEMP_HDRNAME

#   define  ALIB_TEMP_HDRNAME "alib/compatibility/std_strings.hpp"
#   include ALIB_TEMP_HDRNAME
#   undef   ALIB_TEMP_HDRNAME

#   if defined(QT_CORE_LIB)
#       define  ALIB_TEMP_HDRNAME "alib/compatibility/qt_characters.hpp"
#       include ALIB_TEMP_HDRNAME
#       undef   ALIB_TEMP_HDRNAME
#   endif
#endif

#if ALIB_SINGLETONS
#   define  ALIB_TEMP_HDRNAME "alib/singletons/singleton.hpp"
#   include ALIB_TEMP_HDRNAME
#   undef   ALIB_TEMP_HDRNAME
#endif

#if ALIB_THREADS
#   define  ALIB_TEMP_HDRNAME "alib/threads/recursivelock.hpp"
#   include ALIB_TEMP_HDRNAME
#   undef   ALIB_TEMP_HDRNAME
#endif

#if ALIB_TIME
#   define  ALIB_TEMP_HDRNAME "alib/time/datetime.hpp"
#   include ALIB_TEMP_HDRNAME
#   undef   ALIB_TEMP_HDRNAME
#   define  ALIB_TEMP_HDRNAME "alib/time/ticks.hpp"
#   include ALIB_TEMP_HDRNAME
#   undef   ALIB_TEMP_HDRNAME
#endif

#if ALIB_BOXING
#   define  ALIB_TEMP_HDRNAME "alib/boxing/boxing.hpp"
#   include ALIB_TEMP_HDRNAME
#   undef   ALIB_TEMP_HDRNAME
#endif

#if ALIB_STRINGS
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

#if ALIB_ENUMS
#   define  ALIB_TEMP_HDRNAME "alib/enums/records.hpp"
#   include ALIB_TEMP_HDRNAME
#   undef   ALIB_TEMP_HDRNAME
#endif

#if ALIB_CAMP
#   define  ALIB_TEMP_HDRNAME "alib/lang/resources/resources.hpp"
#   include ALIB_TEMP_HDRNAME
#   undef   ALIB_TEMP_HDRNAME
#   define  ALIB_TEMP_HDRNAME "alib/lang/format/formatter.hpp"
#   include ALIB_TEMP_HDRNAME
#   undef   ALIB_TEMP_HDRNAME
#   define  ALIB_TEMP_HDRNAME "alib/lang/message/exception.hpp"
#   include ALIB_TEMP_HDRNAME
#   undef   ALIB_TEMP_HDRNAME
#endif

#if ALIB_ALOX
#   define  ALIB_TEMP_HDRNAME "alib/alox.hpp"
#   include ALIB_TEMP_HDRNAME
#   undef   ALIB_TEMP_HDRNAME
#endif

#endif  // defined(ALIB_PRECOMPILED_HEADER) && ALIB_PRECOMPILED_HEADER
#endif  // ALIB_DOX

