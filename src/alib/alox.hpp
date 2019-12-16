/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_alox of the \aliblong.
 *
 * \note
 *   This file is always included in any \alibdist!<br>
 *   See \ref alib_manual_modules_special_alox "Special Treatment For Module ALox" for furhter
 *   explanation.
 *
 * \emoji :copyright: 2013-2019 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/

#ifndef HPP_ALOX
#define HPP_ALOX 1

#if !defined(HPP_ALIB_MODULES) && !defined(ALIB_DOX)
#   include "alib/lib/modules.hpp"
#endif

#if ALIB_ALOX
#   if !defined(HPP_ALIB_ALOX)
#      include "alib/alox/alox.hpp"
#   endif

// #############################################################################################
// If ALox not included in the build, set all alox macros to empty
// #############################################################################################
#else
#   define Log_Prune( ... )
#   define Log_SetSourcePathTrimRule(...)
#   define Log_ClearSourcePathTrimRules(...)
#   define Log_AddDebugLogger()
#   define Log_RemoveDebugLogger()
#   define Log_GetLogger( identifier, name )
#   define Log_RemoveLogger( logger)
#   define Log_SetDomain(...)
#   define Log_RemoveThreadDomain(...)
#   define Log_SetDomainSubstitutionRule(...)
#   define Log_SetVerbosity(...)
#   define Log_SetStartTime( ... )
#   define Log_MapThreadName(...)
#   define Log_LogState(...)
#   define Log_Verbose(...)
#   define Log_Info(...)
#   define Log_Warning(...)
#   define Log_Error(...)
#   define Log_Assert(...)
#   define Log_If(...)
#   define Log_Once(...)
#   define Log_Entry(...)
#   define Log_SetPrefix(...)
#   define Log_Store(...)
#   define Log_Retrieve(data,...)
#   define Log_Exception(...)

#   define Lox_Prune( ... )
#   define Lox_SetSourcePathTrimRule(...)
#   define Lox_ClearSourcePathTrimRules(...)
#   define Lox_GetLogger( identifier, name )
#   define Lox_RemoveLogger( logger )
#   define Lox_SetDomain(...)
#   define Lox_RemoveThreadDomain(...)
#   define Lox_SetDomainSubstitutionRule(...)
#   define Lox_SetVerbosity(...)
#   define Lox_SetStartTime( ... )
#   define Lox_MapThreadName(...)
#   define Lox_LogState(...)
#   define Lox_Verbose(...)
#   define Lox_Info(...)
#   define Lox_Warning(...)
#   define Lox_Error(...)
#   define Lox_Assert(...)
#   define Lox_If(...)
#   define Lox_Once(...)
#   define Lox_Entry(...)
#   define Lox_Store(...)
#   define Lox_Retrieve(data,...)
#   define Lox_SetPrefix(...)
#   define Lox_Exception(...)


#endif // ALIB_ALOX


#endif // HPP_ALOX
