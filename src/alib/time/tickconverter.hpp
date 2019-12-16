/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_time of the \aliblong.
 *
 * \emoji :copyright: 2013-2019 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_TIME_TICKSCONVERTER
#define HPP_ALIB_TIME_TICKSCONVERTER 1

#ifndef HPP_ALIB_TIME_TICKS
#   include "alib/time/ticks.hpp"
#endif

#ifndef HPP_ALIB_TIME_DATETIME
#   include "alib/time/datetime.hpp"
#endif

namespace aworx { namespace lib { namespace time {

/** ************************************************************************************************
 * As explained in detail in the documentation of module \alib_time, a steady time model is
 * supported with class \alib{time,Ticks} and a non-steady one representing the system clock with
 * class \alib{time,DateTime}.
 * Only values of the latter type can be converted to human readable (calendar) date and time values.
 *
 * In some situations however, a software that requires steady, monotonic time points, may also be
 * required to present these time points in human readable format. It is of-course possible
 * to do some sort of conversion. For that, simply both clocks need to be probed at the same point
 * in time and then time points of both notions can be put in relation to these two probes.
 *
 * The effect however is that the conversion results will change for all values as soon as
 * the system clock is changed and the probe values of the two clocks are updated.
 * This is true also for values that are "older" than the point in time that the clock change
 * happened. The reason is quickly understood: The system clock's counter changes, while the steady
 * clock's counter does not.
 *
 * To give the user of \alib full control about how system clock changes are reflected,
 * the conversion of time points is encapsulated by this class together with one pair of clock probe
 * data. A software can use one ore more instances of this class and update (synchronize) these
 * instances independently.
 **************************************************************************************************/
class TickConverter
{
    protected:
        /** Time point of steady clock of last invocation of #SyncClocks. */
        Ticks::TTimePoint       steadyClockSyncTime;

        /** Time point of system clock of last invocation of #SyncClocks. */
        DateTime::TTimePoint    systemClockSyncTime;

    public:
        /** ****************************************************************************************
         * Constructor. Invokes #SyncClocks.
         ******************************************************************************************/
        TickConverter()
        {
            SyncClocks();
        }

        /** ****************************************************************************************
         * Generates a set of "probes" of the steady, monotonic clock and the system clock.
         * The measurement of both clocks is repeated the given number of times and the pair with
         * the smallest difference between both is chosen.
         * This approach mitigates the risk of using a pair for which thread execution had been
         * interrupted during the two measurements.
         *
         * Note that after a call to this method, the conversion methods may return slightly
         * different values than before the call, even if the system clock was not changed.
         *
         * If this method is not invoked after a change of the system clock, such change of the
         * system clock is not reflected by the conversion methods.
         * In other words, the conversion methods always work just as if the system clock had not
         * changed since the last invocation of this method.
         *
         * \note
         *   On a GNU/Linux workstation (without workload), the error observed when doing only one
         *   measurement was in the magnitude of several microseconds.
         *
         * @param qtyRepeats    The number of measurements to perform. Defaults to \c 5.
         ******************************************************************************************/
        ALIB_API
        void SyncClocks( int qtyRepeats = 5 );

        /** ****************************************************************************************
         * Sets the pair of conversion times to equal the other converter object. This is useful
         * to avoid differences in conversion across converter instances used in a software.
         *
         * @param other  Another converter object to copy the synchronization information from.
         ******************************************************************************************/
        void SetAs( const TickConverter& other )
        {
            steadyClockSyncTime= other.steadyClockSyncTime;
            systemClockSyncTime= other.systemClockSyncTime;
        }

        /** ****************************************************************************************
         * Converts a \b %Ticks object to a \b %DateTime object.
         * @param ticks The ticks object to convert.
         * @return The date time object.
         ******************************************************************************************/
        DateTime  ToDateTime( Ticks ticks )
        {
            DateTime result( Initialization::Suppress );
            result.NativeValue()= systemClockSyncTime + std::chrono::duration_cast<std::chrono::system_clock::duration>( ticks.NativeValue() - steadyClockSyncTime );
            return result;
        }

        /** ****************************************************************************************
         * Converts a \b %DateTime object to a \b %Ticks object.
         * @param dateTime The date/time object to convert.
         * @return The date time object.
         ******************************************************************************************/
        Ticks  ToTicks( DateTime dateTime )
        {
            Ticks result( Initialization::Suppress );
            result.NativeValue()= (dateTime.NativeValue() - systemClockSyncTime ) + steadyClockSyncTime;
            return result;

        }
};


}} // namespace lib::time

/// Type alias in namespace #aworx.
using   TickConverter=     lib::time::TickConverter;

}  // namespace [aworx]


#endif // HPP_ALIB_TIME_TICKSCONVERTER
