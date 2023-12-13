/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_time of the \aliblong.
 *
 * \emoji :copyright: 2013-2023 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_TIME_TIME
#define HPP_ALIB_TIME_TIME 1

#if !defined(HPP_ALIB_COMPILERS) && !defined(ALIB_DOX)
#   include "alib/lib/compilers.hpp"
#endif

namespace aworx { namespace lib { namespace time {

class Ticks;

/** ************************************************************************************************
 * Initializes \alib time logic.
 * Multiple invocations of this method are ignored.
 *
 * The \ref alib_manual_bootstrapping "standard bootstrap" code of \alib will perform this.
 * Only if fileset fileset \alibfs_modules is not included in the \alibdist_nl, this
 * function has to be invoked "manually".
 *
 * \see
 *   For information about using this method, consult chapter
 *   \ref alib_manual_bootstrapping_smallmods of the \ref alib_manual.
 **************************************************************************************************/
ALIB_API
void       Bootstrap();

/** ************************************************************************************************
 * Frees resources and shuts down \alib time logic.
 * Multiple invocations of this method are ignored.
 *
 * \see
 *   Sibling function \alib{time,Bootstrap}.
 **************************************************************************************************/
ALIB_API
void       Shutdown();

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
