// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
/** @file */ // Hello Doxygen

// check for alib.hpp already there but not us
#if !defined (HPP_ALIB)
    #error "include \"alib/alib.hpp\" before including this header"
#endif
#if defined(HPP_COM_ALIB_TEST_INCLUDES) && defined(HPP_ALIB_TIME_TICKWATCH)
    #error "Header already included"
#endif

// then, set include guard
#ifndef HPP_ALIB_TIME_STOPWATCH
//! @cond NO_DOX
#define HPP_ALIB_TIME_STOPWATCH 1
//! @endcond


#if !defined (HPP_ALIB_TIME_TICKS)
    #include "alib/time/ticks.hpp"
#endif

namespace aworx { namespace lib { namespace time {


/** ************************************************************************************************
 * This class encapsulates a system dependent timer value of type \alib{time,Ticks} and provides
 * some simple interface for measuring multiple time spans and providing their sum and average.
 **************************************************************************************************/
class StopWatch
{
    protected:

        /// The current start time.
        Ticks           startTime;

        /// The number of samples performed.
        int             cntSamples                                                               =0;

        /// The sum of the samples times.
        Ticks::Duration sum;


    public:
        /** ****************************************************************************************
         *  Creates a started StopWatch.
         ******************************************************************************************/
        inline
        StopWatch  ()
        : startTime()
        , sum()
        {}

    public:
        /** ****************************************************************************************
         * Provides access to the internal start time.
         * @return    The start time
         ******************************************************************************************/
        inline Ticks&       GetStartTime()
        {
            return startTime;
        }

        /** ****************************************************************************************
         * Sets the start time to now.
         * This affects both, the reference value for the calculation of this StopWatch's age in
         * subsequent calls, as well as subsequent sample time spans.
         ******************************************************************************************/
        inline void         Start()
        {
            startTime= Ticks::Now();
        }

        /** ****************************************************************************************
         *  Sets the internal value to current system time and clears existing sum and quantity of
         *  samples.
         ******************************************************************************************/
        inline void         Reset()
        {
            sum= Ticks::Duration::FromNanoseconds(0);
            cntSamples= 0;
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
        inline Ticks::Duration Sample()
        {
            Ticks::Duration sample= startTime.Age();
            sum+= sample;
            cntSamples++;
            startTime= Ticks::Now();

            return sample;
        }

        /** ****************************************************************************************
         * Returns the number of calls to #Sample since this instance was created or #Reset was
         * invoked.
         * @return    The number of samples.
         ******************************************************************************************/
        inline int              GetSampleCnt()  const   { return cntSamples; }

        /** ****************************************************************************************
         * Returns the cumulated time of all samples taken since this instance was created or
         * cleared.
         *
         * @return  The cumulated measured time.
         ******************************************************************************************/
        inline Ticks::Duration  GetCumulated()  const   { return sum; }

        /** ****************************************************************************************
         * Returns the average time of all samples since this instance was created or reset.
         * If no measurement was performed, the result value will be set to \c 0.
         *
         * @return  The cumulated measured time.
         ******************************************************************************************/
        inline Ticks::Duration  GetAverage()    const
        {
            return cntSamples== 0 ? Ticks::Duration()
                                  : ( sum / static_cast<int64_t>(cntSamples) );
        }
};

}} // namespace lib::time

/// Type alias in namespace #aworx.
using     StopWatch=            aworx::lib::time::StopWatch;

}  // namespace aworx

#endif // HPP_ALIB_TIME_STOPWATCH
