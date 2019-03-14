// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#ifndef HPP_ALIB_BOXING_PREDEF
#define HPP_ALIB_BOXING_PREDEF 1


#if !defined(HPP_ALIB_LIB_PREDEF_MODULES)
#   include "alib/lib/predef_modules.hpp"
#endif

#if !defined(HPP_ALIB_LIB_PREDEF_COMPILER)
#   include "alib/lib/predef_compiler.hpp"
#endif


// read compiler symbols and create corresponding code selector symbols
#if defined(ALIB_FEAT_BOXING_NON_BIJECTIVE_INTEGRALS)
#   error "Code selector symbol ALIB_FEAT_BOXING_NON_BIJECTIVE_INTEGRALS must not be set from outside. Use postfix '_ON' or '_OFF' for compiler symbols."
#endif

#if defined(ALIB_FEAT_BOXING_NON_BIJECTIVE_INTEGRALS_ON) && defined(ALIB_FEAT_BOXING_NON_BIJECTIVE_INTEGRALS_OFF)
#   error "Compiler symbols ALIB_FEAT_BOXING_NON_BIJECTIVE_INTEGRALS_ON and ALIB_FEAT_BOXING_NON_BIJECTIVE_INTEGRALS_OFF are both set (contradiction)."
#endif

#if defined(ALIB_FEAT_BOXING_NON_BIJECTIVE_INTEGRALS_OFF)
#   define ALIB_FEAT_BOXING_NON_BIJECTIVE_INTEGRALS   0
#else
#   define ALIB_FEAT_BOXING_NON_BIJECTIVE_INTEGRALS   1
#endif


#if defined(ALIB_FEAT_BOXING_NON_BIJECTIVE_CHARACTERS)
#   error "Code selector symbol ALIB_FEAT_BOXING_NON_BIJECTIVE_CHARACTERS must not be set from outside. Use postfix '_ON' or '_OFF' for compiler symbols."
#endif

#if defined(ALIB_FEAT_BOXING_NON_BIJECTIVE_CHARACTERS_ON) && defined(ALIB_FEAT_BOXING_NON_BIJECTIVE_CHARACTERS_OFF)
#   error "Compiler symbols ALIB_FEAT_BOXING_NON_BIJECTIVE_CHARACTERS_ON and ALIB_FEAT_BOXING_NON_BIJECTIVE_CHARACTERS_OFF are both set (contradiction)."
#endif

#if defined(ALIB_FEAT_BOXING_NON_BIJECTIVE_CHARACTERS_OFF)
#   define ALIB_FEAT_BOXING_NON_BIJECTIVE_CHARACTERS   0
#else
#   define ALIB_FEAT_BOXING_NON_BIJECTIVE_CHARACTERS   1
#endif


#if defined(ALIB_FEAT_BOXING_NON_BIJECTIVE_FLOATS)
#   error "Code selector symbol ALIB_FEAT_BOXING_NON_BIJECTIVE_FLOATS must not be set from outside. Use postfix '_ON' or '_OFF' for compiler symbols."
#endif

#if defined(ALIB_FEAT_BOXING_NON_BIJECTIVE_FLOATS_ON) && defined(ALIB_FEAT_BOXING_NON_BIJECTIVE_FLOATS_OFF)
#   error "Compiler symbols ALIB_FEAT_BOXING_NON_BIJECTIVE_FLOATS_ON and ALIB_FEAT_BOXING_NON_BIJECTIVE_FLOATS_OFF are both set (contradiction)."
#endif

#if defined(ALIB_FEAT_BOXING_NON_BIJECTIVE_FLOATS_OFF)
#   define ALIB_FEAT_BOXING_NON_BIJECTIVE_FLOATS   0
#else
#   define ALIB_FEAT_BOXING_NON_BIJECTIVE_FLOATS   1
#endif



#endif // HPP_ALIB_BOXING_PREDEF

