/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_monomem of the \aliblong.
 *
 * \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_MONOMEM_MONOMEM
#define HPP_ALIB_MONOMEM_MONOMEM 1


#if !defined(HPP_ALIB) && !defined(ALIB_DOX)
#   include "alib/alib.hpp"
#endif

ALIB_ASSERT_MODULE(MONOMEM)

ALIB_WARNINGS_IGNORE_UNUSED_MACRO


#if !defined(HPP_ALIB_LANG_COMMONENUMS_DEFS)
#   include "alib/lang/commonenumdefs.hpp"
#endif

#if !defined (HPP_ALIB_MONOMEM_FWDS)
#   include "alib/monomem/fwds.hpp"
#endif

#if !defined(ALIB_DOX)
#   if ALIB_DEBUG_MONOMEM
#      if !ALIB_ALOX
#          error "Module ALox needs to be selected with compiler symbol ALIB_DEBUG_MONOMEM set."
#      endif
#      include "alib/alox.hpp"

       namespace alib::monomem {
          ALIB_API bool dbgLogLock(bool lock);
       }
#      define DBG_MONOMEM_PATH_DOMAIN         { if( dbgLogLock(true) ) {Log_SetDomain( "ALIB/MONOMEM", Scope::Path ) } dbgLogLock(false); }
#      define DBG_MONOMEM_FILE_DOMAIN(domain) { if( dbgLogLock(true) ) {Log_SetDomain( domain, Scope::Filename )     } dbgLogLock(false); }
#      define DBG_MONOMEM_ERROR(...)          { if( dbgLogLock(true) ) {Log_Error(   __VA_ARGS__ )                   } dbgLogLock(false); }
#      define DBG_MONOMEM_WARNING(...)        { if( dbgLogLock(true) ) {Log_Warning( __VA_ARGS__ )                   } dbgLogLock(false); }
#      define DBG_MONOMEM_INFO(...)           { if( dbgLogLock(true) ) {Log_Info(    __VA_ARGS__ )                   } dbgLogLock(false); }
#      define DBG_MONOMEM_VERBOSE(...)        { if( dbgLogLock(true) ) {Log_Verbose( __VA_ARGS__ )                   } dbgLogLock(false); }
#   else
#      define DBG_MONOMEM_PATH_DOMAIN
#      define DBG_MONOMEM_FILE_DOMAIN(domain)
#      define DBG_MONOMEM_ERROR(...)
#      define DBG_MONOMEM_WARNING(...)
#      define DBG_MONOMEM_INFO(...)
#      define DBG_MONOMEM_VERBOSE(...)
#   endif
#endif

ALIB_WARNINGS_RESTORE

#endif // HPP_ALIB_MONOMEM_MONOMEM
