//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
#ifndef HPP_ALIB_SYSTEM_PP
#define HPP_ALIB_SYSTEM_PP
#pragma once

#ifndef HPP_ALIB
#   include "alib/alib.prepro.hpp"
#endif
#if ALIB_SYSTEM

//##################################################################################################
// Macros introduced by module ALib.System
//##################################################################################################
// Configuration Macro ALIB_PATH_CHARACTERS_WIDE and macro A_PATH for literals
#if !defined(ALIB_PATH_CHARACTERS_WIDE)
#   if defined(_WIN32)
#      define ALIB_PATH_CHARACTERS_WIDE    1
#   else
#      define ALIB_PATH_CHARACTERS_WIDE    0
#   endif
#endif

#if ALIB_PATH_CHARACTERS_WIDE
#   define A_PATH(literal) L##literal
#else
#   define A_PATH(literal) literal
#endif

#if ALIB_CHARACTERS_WIDE == ALIB_PATH_CHARACTERS_WIDE
#   define ALIB_PATH_TO_STRING(        src,dest,bufSize )  decltype(src)& dest(src);
#   define ALIB_PATH_TO_STRING_ARG(    src,bufSize      )  src;
#   define ALIB_STRING_TO_PATH(        src,dest,bufSize )  decltype(src)& dest(src);
#   define ALIB_STRING_TO_PATH_ARG(    src,bufSize      )  src;
#else
#   define ALIB_PATH_TO_STRING(        src,dest,bufSize )  alib::strings::TLocalString<alib::characters::character      ,bufSize> dest(src);
#   define ALIB_PATH_TO_STRING_ARG(    src,bufSize      )  alib::strings::TLocalString<alib::characters::character      ,bufSize>(src);
#   define ALIB_STRING_TO_PATH(        src,dest,bufSize )  alib::strings::TLocalString<alib::PathCharType ,bufSize> dest(src);
#   define ALIB_STRING_TO_PATH_ARG(    src,bufSize      )  alib::strings::TLocalString<alib::PathCharType ,bufSize>(src);
#endif

#if !ALIB_PATH_CHARACTERS_WIDE
#   define ALIB_PATH_TO_NARROW(        src,dest,bufSize )  decltype(src)& dest(src);
#   define ALIB_PATH_TO_NARROW_ARG(    src,bufSize      )  src;
#else
#   define ALIB_PATH_TO_NARROW(        src,dest,bufSize )  alib::strings::TLocalString<alib::characters::nchar, bufSize> dest(src);
#   define ALIB_PATH_TO_NARROW_ARG(    src,bufSize      )  alib::strings::TLocalString<alib::characters::nchar, bufSize>(src);
#endif

// Configuration Macro ALIB_SYSTEM_FILE_STATUS_IMPL
#if !defined(ALIB_SYSTEM_FORCE_STD_FILE_STATUS)
#   define   ALIB_SYSTEM_FORCE_STD_FILE_STATUS           0
#endif

#if defined(ALIB_SYSTEM_FILE_STATUS_IMPL)
#   error "Configuration Macro ALIB_SYSTEM_FILE_STATUS_IMPL must not be set from outside! Use ALIB_SYSTEM_FORCE_STD_FILE_STATUS."
#endif

#define ALIB_SYSTEM_FILE_STD_STATUS   1
#define ALIB_SYSTEM_FILE_POSIX_STATUS   2

#if   (        ( defined(__GLIBCXX__)   && !defined(__MINGW32__) )                                 \
            || defined(__APPLE__)                                                                  \
            || defined(_LIBCPP_VERSION)                                                            \
            || defined(__ANDROID_NDK__)   )                                                        \
        &&  !ALIB_SYSTEM_FORCE_STD_FILE_STATUS
#   define ALIB_SYSTEM_FILE_STATUS_IMPL     ALIB_SYSTEM_FILE_POSIX_STATUS
#else
#   define ALIB_SYSTEM_FILE_STATUS_IMPL     ALIB_SYSTEM_FILE_STD_STATUS
#endif

// set ALIB_POSIX_MAPPED_FILES
#if !defined(_WIN32)
#   include <unistd.h>
#endif

#if defined(_POSIX_MAPPED_FILES) && _POSIX_MAPPED_FILES > 0
#   define ALIB_POSIX_MAPPED_FILES 1
#else
#   define ALIB_POSIX_MAPPED_FILES 0
#endif

#endif // ALIB_SYSTEM
#endif // HPP_ALIB_SYSTEM_PP
