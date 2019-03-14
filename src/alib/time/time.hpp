// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#ifndef HPP_ALIB_TIME_TIME
#define HPP_ALIB_TIME_TIME 1


#if !defined(HPP_ALIB_LIB_PREDEF_MODULES)
#   include "alib/lib/predef_modules.hpp"
#endif

#if !defined(HPP_ALIB_LIB_PREDEF_MODULES)
#   include "alib/lib/predef_modules.hpp"
#endif

#if !defined(HPP_ALIB_LIB_PREDEF_COMPILER)
#   include "alib/lib/predef_compiler.hpp"
#endif




namespace aworx { namespace lib { namespace time {

class Ticks;

/** ************************************************************************************************
 * Initializes \alib time logic.
 * Multiple invocations of this method are ignored.
 *
 * \see
 *   For information about using this method, consult chapter
 *   \ref alib_manual_bootstrapping_smallmods of the \ref alib_manual.
 **************************************************************************************************/
ALIB_API
void       Init();

/** ************************************************************************************************
 * Frees resources and shuts down \alib time logic.
 * Multiple invocations of this method are ignored.
 *
 * \see
 *   Sibling function \alib{time,Init}.
 **************************************************************************************************/
ALIB_API
void       TerminationCleanUp();

/** ************************************************************************************************
 * The time when the ticker module containing the ticker (ALIB) was initialized.
 * This might be useful to measure the time since the process was started.
 *
 * @return  Initialization time of the module or process
 **************************************************************************************************/
ALIB_API
Ticks&     CreationTime();

}}} // namespace [aworx::lib::time]

#endif // HPP_ALIB_TIME_TIME
