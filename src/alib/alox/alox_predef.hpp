// #################################################################################################
//  aworx::lib::lox - ALox Logging Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#ifndef HPP_ALOX_PREDEF
#define HPP_ALOX_PREDEF 1


#if !defined(HPP_ALIB_LIB_PREDEF_MODULES)
#   include "alib/lib/predef_modules.hpp"
#endif

#if !defined(HPP_ALIB_LIB_PREDEF_COMPILER)
#   include "alib/lib/predef_compiler.hpp"
#endif


#if defined(ALOX_DBG_LOG)
    #error "Code selector symbol ALOX_DBG_LOG must not be set from outside. Use postfix '_ON' or '_OFF' for compiler symbols."
#endif

#if defined(ALOX_DBG_LOG_OFF) && defined(ALOX_DBG_LOG_ON)
    #error "Compiler symbols ALOX_DBG_LOG_OFF / ALOX_DBG_LOG_ON are both set (contradiction)."
#endif

#if defined(ALOX_DBG_LOG_ON) || ( ALIB_DEBUG && !defined(ALOX_DBG_LOG_OFF) )
    #define ALOX_DBG_LOG 1
#else
    #define ALOX_DBG_LOG 0
#endif


#if defined(ALOX_REL_LOG)
    #error "Code selector symbol ALOX_REL_LOG must not be set from outside. Use postfix '_ON' or '_OFF' for compiler symbols."
#endif

#if defined(ALOX_REL_LOG_OFF) && defined(ALOX_REL_LOG_ON)
    #error "Compiler symbols ALOX_REL_LOG_OFF / ALOX_REL_LOG_ON are both set (contradiction)."
#endif

#if !defined(ALOX_REL_LOG_OFF)
    #define ALOX_REL_LOG 1
#else
    #define ALOX_REL_LOG 0
#endif


// #################################################################################################
// DBG_LOG_CI / REL_LOG_CI
// #################################################################################################

#if defined(ALOX_DBG_LOG_CI)
    #error "Code selector symbol ALOX_DBG_LOG_CI must not be set from outside. Use postfix '_ON' or '_OFF' for compiler symbols."
#endif

#if defined(ALOX_DBG_LOG_CI_OFF) && defined(ALOX_DBG_LOG_CI_ON)
    #error "Compiler symbols ALOX_DBG_LOG_CI_OFF / ALOX_DBG_LOG_CI_ON are both set (contradiction)."
#endif

#if ALOX_DBG_LOG && !defined( ALOX_DBG_LOG_CI_OFF )
    #define ALOX_DBG_LOG_CI 1
#else
    #define ALOX_DBG_LOG_CI 0
#endif



#if defined(ALOX_REL_LOG_CI)
    #error "Code selector symbol ALOX_REL_LOG_CI must not be set from outside. Use postfix '_ON' or '_OFF' for compiler symbols."
#endif

#if defined(ALOX_REL_LOG_CI_OFF) && defined(ALOX_REL_LOG_CI_ON)
    #error "Compiler symbols ALOX_REL_LOG_CI_OFF / ALOX_REL_LOG_CI_ON are both set (contradiction)."
#endif

#if ALOX_REL_LOG && defined( ALOX_REL_LOG_CI_ON )
    #define ALOX_REL_LOG_CI 1
#else
    #define ALOX_REL_LOG_CI 0
#endif



#endif // HPP_ALOX_PREDEF
