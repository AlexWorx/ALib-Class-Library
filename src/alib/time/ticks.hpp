/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_time of the \aliblong.
 *
 * \emoji :copyright: 2013-2019 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_TIME_TICKS
#define HPP_ALIB_TIME_TICKS 1

#if !defined (HPP_ALIB_TIME_TIMEPOINT)
#   include "alib/time/timepointbase.hpp"
#endif

namespace aworx { namespace lib { namespace time {


/** ************************************************************************************************
 * This class provides an interface into system dependent steady, monotonic timer values, which are
 * independent from the system's clock settings, hence independent from changes of the clock
 * by the user, by external clock synchronization using the internet, GPS, mobile network, etc.
 *
 * Usually, this class is used to measure and store durations (e.g. with inherited method
 * \alib{time::TimePointBase,Age}).
 * As tick values are not just system dependent, but even usually change with a reboot of a system,
 * they should not be stored and loaded in data files or otherwise shared between systems.
 *
 * In contrast to sibling type \alib{time,DateTime}, this class is also missing conversion methods
 * with types that represent "human" calendar dates and a clock times.
 * However, in some situations it still may be needed to convert the points in time measured with
 * this class to human readable date and time representations.
 * For this purpose, class \alib{time,TickConverter} allows to convert objects of this type to
 * objects of sibling type \b %DateTime, which then can be further converted to system-dependent
 * calendar dates and clock times.
 *
 * The class has no specific interface, but the methods and operators inherited from base
 * \alib{time,TimePointBase}.
 **************************************************************************************************/
class Ticks : public TimePointBase<std::chrono::steady_clock, Ticks>
{
    public:
//! @cond NO_DOX
// doxygen bug: generates "return type not documented"
        /// Use constructors of parent class.
        using  TimePointBase::TimePointBase;
//! @endcond
};


}} // namespace lib::time

/// Type alias in namespace #aworx.
using     Ticks   =                lib::time::Ticks;

}  // namespace [aworx]

#if ALIB_BOXING
    #if !defined(HPP_ALIB_BOXING_BOXING)
    #   include "alib/boxing/boxing.hpp"
    #endif

    ALIB_BOXING_VTABLE_DECLARE( aworx::Ticks           , vt_time_ticks          )
    ALIB_BOXING_VTABLE_DECLARE( aworx::Ticks::Duration , vt_time_ticks_duration )
#endif


#endif // HPP_ALIB_TIME_TICKS
