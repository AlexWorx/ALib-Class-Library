//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_SYSTEM_PP
#define HPP_ALIB_SYSTEM_PP
#pragma once

#ifndef INL_ALIB
#   include "alib/alib.inl"
#endif
#if ALIB_SYSTEM

// #################################################################################################
// Symbols introduced by module ALib.System
// #################################################################################################
// Symbol ALIB_PATH_CHARACTERS_WIDE and macro A_PATH for literals
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
#   define ALIB_STRING_TO_PATH(        src,dest,bufSize )  alib::strings::TLocalString<alib::system::PathCharType ,bufSize> dest(src);
#   define ALIB_STRING_TO_PATH_ARG(    src,bufSize      )  alib::strings::TLocalString<alib::system::PathCharType ,bufSize>(src);
#endif

#if !ALIB_PATH_CHARACTERS_WIDE
#   define ALIB_PATH_TO_NARROW(        src,dest,bufSize )  decltype(src)& dest(src);
#   define ALIB_PATH_TO_NARROW_ARG(    src,bufSize      )  src;
#else
#   define ALIB_PATH_TO_NARROW(        src,dest,bufSize )  alib::strings::TLocalString<alib::characters::nchar, bufSize> dest(src);
#   define ALIB_PATH_TO_NARROW_ARG(    src,bufSize      )  alib::strings::TLocalString<alib::characters::nchar, bufSize>(src);
#endif




#endif
#endif // HPP_ALIB_SYSTEM_PP

