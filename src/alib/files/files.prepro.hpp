//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_FILES_PP
#define HPP_ALIB_FILES_PP
#pragma once
#ifndef INL_ALIB
#   include "alib/alib.inl"
#endif
#if ALIB_FILES

// #################################################################################################
// Symbols introduced by module ALib.Files
// #################################################################################################

// Symbol ALIB_FILES_SCANNER_IMPL
#if !defined(ALIB_FILES_FORCE_STD_SCANNER)
#   define   ALIB_FILES_FORCE_STD_SCANNER           0
#endif

#if defined(ALIB_FILES_SCANNER_IMPL)
#   error "Symbol ALIB_FILES_SCANNER_IMPL must not be set from outside! Use ALIB_FILES_FORCE_STD_SCANNER."
#endif

#define ALIB_FILES_SCANNER_STDFS   1
#define ALIB_FILES_SCANNER_POSIX   2

#if   (        ( defined(__GLIBCXX__)   && !defined(__MINGW32__) )                                 \
            || defined(__APPLE__)                                                                  \
            || defined(_LIBCPP_VERSION)                                                            \
            || defined(__ANDROID_NDK__)   )                                                        \
        &&  !ALIB_FILES_FORCE_STD_SCANNER
#   define ALIB_FILES_SCANNER_IMPL     ALIB_FILES_SCANNER_POSIX
#else
#   define ALIB_FILES_SCANNER_IMPL     ALIB_FILES_SCANNER_STDFS
#endif




#endif
#endif // HPP_ALIB_FILES_PP

