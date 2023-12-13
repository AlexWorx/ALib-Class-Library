/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_time of the \aliblong.
 *
 * \emoji :copyright: 2013-2023 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_TIME_STOPWATCH
#define HPP_ALIB_TIME_STOPWATCH 1

#if !defined (HPP_ALIB_TIME_TICKS)
    #include "alib/time/ticks.hpp"
#endif

namespace aworx { namespace lib { namespace time {

/** ************************************************************************************************
 * This class encapsulates a system dependent timer value of type \alib{time,Ticks} and provides
 * some simple interface for measuring multiple time spans and providing their sum, average, minimum
 * and maximum.
 **************************************************************************************************/
class StopWatch
{
    protected:

        /** The current start time. */
        Ticks           startTime;

        /** The number of samples performed. */
        int             cntSamples                                                               =0;

        /** The sum of the samples times. */
        Ticks::Duration sum;

        /** The minimum duration probed. */
        Ticks::Duration min;

        /** The maximum duration probed. */
        Ticks::Duration max;


    public:
        /** ****************************************************************************************
         *  Creates a started StopWatch.
         ******************************************************************************************/
        StopWatch  ()
        : startTime()
        , sum()
        {}

    public:
        /** ****************************************************************************************
         * Provides access to the internal start time.
         * @return    The start time
         ******************************************************************************************/
        Ticks&       GetStartTime()
        {
            return startTime;
        }

        /** ****************************************************************************************
         * Sets the start time to now.
         * This affects both, the reference value for the calculation of this StopWatch's age in
         * subsequent calls, as well as subsequent sample time spans.
         ******************************************************************************************/
        void         Start()
        {
            startTime= Ticks::Now();
        }

        /** ****************************************************************************************
         *  Sets the internal value to current system time and clears existing sum and quantity of
         *  samples.
         ******************************************************************************************/
        void         Reset()
        {
            sum= Ticks::Duration::FromNanoseconds(0);
            cntSamples= 0;
            min= (std::numeric_limits<Ticks::Duration>::max)();
            max= (std::numeric_limits<Ticks::Duration>::min)();

            Start();
        }

        /** ****************************************************************************************
         * Returns the time span between the current system time and the internal start value.
         * In addition this value is added to the sum of sample times and the sample counter is
         * increased by one. Lastly the internal reference value is set to now. Therefore, a
         * subsequent call to this function would measure the time span from this call to this
         * subsequent call (if the internal start time value was not set differently meanwhile).
         *
         * @return The time difference between the current system time and the internal
         *         reference value.
         ******************************************************************************************/
        Ticks::Duration Sample()
        {
            Ticks::Duration sample= startTime.Age();
            sum+= sample;
            if( min > sample ) min= sample;
            if( max < sample ) max= sample;
            ++cntSamples;
            startTime= Ticks::Now();

            return sample;
        }

        /** ****************************************************************************************
         * Returns the number of calls to #Sample since this instance was created or #Reset was
         * invoked.
         * @return    The number of samples.
         ******************************************************************************************/
        int              GetSampleCnt()                                                        const
        {
            return cntSamples;
        }

        /** ****************************************************************************************
         * Returns the cumulated time of all samples taken since this instance was created or
         * cleared.
         *
         * @return  The cumulated measured time.
         ******************************************************************************************/
        Ticks::Duration  GetCumulated()                                                        const
        {
            return sum;
        }

        /** ****************************************************************************************
         * Returns the average time of all samples since this instance was created or reset.
         * If no measurement was performed, the result value will be set to \c 0.
         *
         * @return  The cumulated measured time.
         ******************************************************************************************/
        Ticks::Duration  GetAverage()                                                          const
        {
            return cntSamples== 0 ? Ticks::Duration()
                                  : ( sum / static_cast<int64_t>(cntSamples) );
        }

        /** ****************************************************************************************
         * Returns the minimum duration of all samples since this instance was created or reset.
         * If no measurement was performed, the value evaluates to the minmum value storable
         * in type \b Ticks::Duration.
         *
         * @return  The minimum measured duration.
         ******************************************************************************************/
        Ticks::Duration  GetMinimum()                                                          const
        {
            return min;
        }

        /** ****************************************************************************************
         * Returns the maximum duration of all samples since this instance was created or reset.
         * If no measurement was performed, the value evaluates to the maximum value storable
         * in type \b Ticks::Duration.
         *
         * @return  The maximum measured duration.
         ******************************************************************************************/
        Ticks::Duration  GetMaximum()                                                          const
        {
            return max;
        }
};

}} // namespace lib::time

/// Type alias in namespace #aworx.
using     StopWatch=            lib::time::StopWatch;

}  // namespace [aworx]

#endif // HPP_ALIB_TIME_STOPWATCH
