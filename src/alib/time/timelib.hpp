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
#if defined(HPP_COM_ALIB_TEST_INCLUDES) && defined(HPP_ALIB_TIME_LIB)
    #error "Header already included"
#endif

// then, set include guard
#ifndef HPP_ALIB_TIME_LIB
//! @cond NO_DOX
#define HPP_ALIB_TIME_LIB 1
//! @endcond



// #################################################################################################
// includes
// #################################################################################################
#if ALIB_MODULE_STRINGS

    #if !defined (HPP_ALIB_STRINGS_SUBSTRING)
        #include "alib/strings/substring.hpp"
    #endif
    #if !defined (_GLIBCXX_CTIME) && !defined(_CTIME_)
        #include <ctime>
    #endif

#endif


#if !defined (_GLIBCXX_CHRONO) && !defined(_CHRONO_)
#   include <chrono>
#endif



namespace aworx { namespace lib {
/**
 * This namespace of \alib provides types for calendrical date and time processing as well as
 * for non-calendrical steady and monotonic time measurement.
 *
 * Two central classes are provided to represent "point in times" or "time stamps":
 * 1. Class \alib{time,DateTime} for calendrical values
 * 2. Class \alib{time,Ticks} for steady and monotonic values
 *
 * Both classes allow to measure (probe) the current point in time, which is internally using a
 * system-dependent clock source. With class \b %DateTime, the clock used is related to the system's
 * clock setting, locale setting, daylight saving, etc., while with class \b %Ticks, the clock is
 * steady and monotonic. Besides that, both classes are derived from template type
 * \alib{time,TimePointBase} which provides basic functionality to work with clock values
 * and "durations" (see inner type \alib{time::TimePointBase,Duration} of class
 * \alib{time,TimePointBase}).
 *
 * Furthermore this namespace incorporates types used for conversion of "time points" and
 * durations to values understandable to humans, hence calendar dates and clock values in hours,
 * minutes, etc.<br>
 *
 * While for \alib{time::TimePointBase,Duration,durations} of both types (calendrical and
 * steady/monotonic), class \alib{time,CalendarDuration} can be used to convert values to and from
 * human understandable units, a similar class \alib{time,CalendarDateTime} allows only the
 * conversion of \b %DateTime values to and from units understandable by humans. The reason for
 * this is that values of type \b %Ticks can not be converted without ambiguities: Their conversion
 * can only be performed using the current system clock, which means that changes of the current
 * clock setting changes the result of the conversion!<br>
 * As it still might be suitable to convert \b Ticks to human-readable calendar dates and time,
 * class \alib{time,TicksConverter} is provided. This class allows to convert values of type
 * \b %Ticks to values of type \b %DateTime, which then can be used with \b %CalendarDateTime.
 * Of-course, it is up to the user of the library, to assure that ambiguities are solved,
 * respectively it is duly made transparent to the end-user how such translated values are to
 * be interpreted.
 *
 * \note
 *  A typical sample that makes the issues of conversion of point in time values more clear, is
 *  found in the area of activity logging of a software. Module \alibmod_alox provides tools to
 *  implement such application logging. As it is important to have a steady monotonic relationship
 *  between each log entry in respect to the moment of their occurrence, the library stores a
 *  \b %Ticks value with each entry. In many cases, end-users are only interested in two sorts of
 *  information:
 *  - The point in time given as a duration value relative to the start of the application
 *  - The point in time given as a duration value relative to the previous log statement.
 *
 * \note
 *  This means that the "absolute", calendrical point in time is not of interest in most cases.
 *  Nevertheless, the library allows to display also calendar dates and "time of day clock-times"
 *  if wished. Now, still such display should rather show "steady monotonic" times when listed.
 *  Therefore, no \b %DateTime values are stored with log entries, and if calendrical display
 *  is needed, a conversion using class \b %TicksConverter is performed. The object used for this
 *  is created only once and in favour to provide steady log dates, is not updated by the library.
 *  This means, if a software runs for a longer time, changes of the system clock are \b not
 *  reflected and therefore, the calendrical date and time values displayed may start to deviate
 *  from the true system clock!<br>
 *  A software that is using the library has to decide to either actively
 *  \alib{time,TicksConverter::SyncClocks,update the converter} (and hence introduce non steady
 *  log entries) or to acknowledge the user of the software of  the problem of interpreting the
 *  date/time values!
 */
namespace time {

template<typename TClock, typename TDerived>
class TimePointBase;
class DateTime;
class Ticks;

class CalendarDateTime;

/** ************************************************************************************************
 * The library class for namespace \ref aworx::lib::time.
 **************************************************************************************************/
class TimeLib  : public lang::Library
{
    /// Library class \b %CalendarDateTime accesses internals.
    friend class CalendarDateTime;

    protected:
        /// Table of string representations of the twelve months. Read from resource string
        /// \b "Months".
        static String                               months[12];

        /// Table of string representations of the seven days of a week. Read from resource string
        /// \b "Days".
        static String                               days  [ 7];

        /// The ticks when the library was initialized.
        Ticks*                                      creationTime;

    // #############################################################################################
    // Constructor
    // #############################################################################################
    public:
        inline  TimeLib()
        : lang::Library( ALIB_VERSION, ALIB_REVISION, ASTR("ALIB_TIME") )
        {}


    // #############################################################################################
    // Interface methods
    // #############################################################################################
    public:
        /** ****************************************************************************************
         * The time when the ticker library containing the ticker (ALIB) was initialized.
         * This might be useful to measure the time since the process was started.
         *
         * @return  Initialization time of the library or process
         ******************************************************************************************/
        inline const Ticks& CreationTime()
        {
            return *creationTime;
        }


    // #############################################################################################
    // Internal methods
    // #############################################################################################
    protected:
        /** ****************************************************************************************
         * Initializes \alib time logic.
         *
         * Dependent on \ref ALIB_MODULE_BOXING, initializes the <b>%ALib %Boxing</b> system by
         * registering interface class \alib{time,IFormat_DateTime}.
         *
         * @param phase  The initialization phase to perform.
         ******************************************************************************************/
        virtual void        init( Phases phase )                                           override;

        /** ****************************************************************************************
         * Frees resources and shuts down \alib time logic.
         ******************************************************************************************/
        virtual void        terminationCleanUp()                                           override;

}; // class time::Library

} // namespace aworx::lib[::time]

/** ************************************************************************************************
 * The static library singleton of namespace #aworx::lib::time.
 **************************************************************************************************/
ALIB_API
extern time::TimeLib TIME;

} // namespace aworx[::lib]


} // namespace [aworx]


#endif // HPP_ALIB_TIME_LIB
