/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_time of the \aliblong.
 *
 * \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_TIME_TIME
#define HPP_ALIB_TIME_TIME 1

#if !defined(HPP_ALIB) && !defined(ALIB_DOX)
#   include "alib/alib.hpp"
#endif

namespace alib {  namespace time {

class Ticks;

/** ************************************************************************************************
 * Initializes \alib time logic.
 * Multiple invocations of this method are ignored.
 *
 * The \ref alib_manual_bootstrapping "standard bootstrap" code of \alib, hence the (overloaded)
 * functions \ref alib::Bootstrap will call this function.
 *
 * \see
 *   For information about using this method, consult chapter
 *   \ref alib_manual_bootstrapping_nocamps of the \ref alib_manual.
 **************************************************************************************************/
ALIB_API
void       Bootstrap();

/** ************************************************************************************************
 * Frees resources and shuts down \alib time logic. Multiple invocations of this method are ignored.
 * The \ref alib_manual_bootstrapping "standard bootstrap" code of \alib, hence the (overloaded)
 * functions \ref alib::Shutdown will call this function.
 *
 * \see
 *   Sibling function \alib{time::Bootstrap}.
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

}} // namespace [alib::time]

#endif // HPP_ALIB_TIME_TIME
