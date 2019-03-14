// #################################################################################################
//  ALib C++ Library
//
//  Essential ALib types needed by every module
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#ifndef HPP_ALIB_CHARACTERS_PREDEF
#define HPP_ALIB_CHARACTERS_PREDEF 1


#if !defined(HPP_ALIB_LIB_PREDEF_MODULES)
#   include "alib/lib/predef_modules.hpp"
#endif


#if defined(ALIB_CHARACTERS_ARE_NARROW)
#   error "Code selector symbol ALIB_CHARACTERS_ARE_NARROW must not be set from outside. Use  postfix '_ON' or '_OFF' for compiler symbols."
#endif

#if defined(ALIB_CHARACTERS_FORCE_NARROW_ON) && defined(ALIB_CHARACTERS_FORCE_NARROW_OFF)
#   error "Compiler symbols ALIB_CHARACTERS_FORCE_NARROW_ON and ALIB_CHARACTERS_FORCE_NARROW_OFF are both set (contradiction)."
#endif
#if defined(ALIB_CHARACTERS_FORCE_WIDE_ON) && defined(ALIB_CHARACTERS_FORCE_WIDE_OFF)
#   error "Compiler symbols ALIB_CHARACTERS_FORCE_WIDE_ON and ALIB_CHARACTERS_FORCE_WIDE_OFF are both set (contradiction)."
#endif
#if defined(ALIB_CHARACTERS_FORCE_NARROW_ON) && defined(ALIB_CHARACTERS_FORCE_WIDE_ON)
#   error "Compiler symbols ALIB_CHARACTERS_FORCE_NARROW_ON and ALIB_CHARACTERS_FORCE_WIDE_ON are both set (contradiction)."
#endif

#if defined(ALIB_CHARACTERS_FORCE_WIDE_2_BYTES_ON) && defined(ALIB_CHARACTERS_FORCE_WIDE_2_BYTES_OFF)
#   error "Compiler symbols ALIB_CHARACTERS_FORCE_WIDE_2_BYTES_ON and ALIB_CHARACTERS_FORCE_WIDE_2_BYTES_OFF are both set (contradiction)."
#endif
#if defined(ALIB_CHARACTERS_FORCE_WIDE_4_BYTES_ON) && defined(ALIB_CHARACTERS_FORCE_WIDE_4_BYTES_OFF)
#   error "Compiler symbols ALIB_CHARACTERS_FORCE_WIDE_4_BYTES_ON and ALIB_CHARACTERS_FORCE_WIDE_4_BYTES_OFF are both set (contradiction)."
#endif
#if defined(ALIB_CHARACTERS_FORCE_WIDE_2_BYTES_ON) && defined(ALIB_CHARACTERS_FORCE_WIDE_4_BYTES_ON)
#   error "Compiler symbols ALIB_CHARACTERS_FORCE_WIDE_2_BYTES_ON and ALIB_CHARACTERS_FORCE_WIDE_4_BYTES_ON are both set (contradiction)."
#endif

#if defined(ALIB_CHARACTERS_FORCE_NARROW_ON)
#   define ALIB_CHARACTERS_ARE_NARROW 1
#endif
#if defined(ALIB_CHARACTERS_FORCE_WIDE_ON)
#   define ALIB_CHARACTERS_ARE_NARROW 0
#endif

#if !defined(ALIB_CHARACTERS_ARE_NARROW)
#   if defined(_MSC_VER)
#       define ALIB_CHARACTERS_ARE_NARROW 0
#   else
#       define ALIB_CHARACTERS_ARE_NARROW 1
#   endif
#endif

#if defined(ALIB_CHARACTERS_FORCE_WIDE_2_BYTES_ON)
#   define ALIB_CHARACTERS_SIZEOF_WCHAR   2
#elif defined(ALIB_CHARACTERS_FORCE_WIDE_4_BYTES_ON)
#   define ALIB_CHARACTERS_SIZEOF_WCHAR   4
#else
#   define ALIB_CHARACTERS_SIZEOF_WCHAR   ALIB_SIZEOF_WCHAR_T
#endif


#endif // HPP_ALIB_CHARACTERS_PREDEF

