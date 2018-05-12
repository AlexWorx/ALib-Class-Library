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
#if defined(HPP_COM_ALIB_TEST_INCLUDES) && defined(HPP_ALIB_TIME_CALENDAR)
    #error "Header already included"
#endif

// then, set include guard
#ifndef HPP_ALIB_TIME_CALENDAR
//! @cond NO_DOX
#define HPP_ALIB_TIME_CALENDAR 1
//! @endcond


#if !defined (HPP_ALIB_TIME_DATETIME)
    #include "alib/time/datetime.hpp"
#endif

#if !defined (HPP_ALIB_TIME_TICKS)
    #include "alib/time/ticks.hpp"
#endif

namespace aworx { namespace lib { namespace time  {

/** ************************************************************************************************
 * This class represents a point in time as a set of calendar and clock values
 * (year, month, day, hour, ...).
 * It provides methods to convert to and from objects of type \alib{time,DateTime}.
 * In addition, a method to format the date and time into human readable string value is available.
 *
 * \note
 *   The conversion from and into objects of type \alib{time,Ticks} is intentionally not supported.
 *   In the case that such objects should be used with this class, an additional conversion
 *   step has to be performed using class \alib{time,TicksConverter}. In other words, conversion can
 *   be performed as follows:
 *
 *          CalendarDateTime <=> DateTime <=> Ticks
 *
 * \note This class is using system specific calendar methods and relies on the locale and time zone
 *       settings of the machine.
 **************************************************************************************************/
class CalendarDateTime
{
    public:
        /// The calendar year (e.g. 2018)
        int                     Year;

        /// The calendar month (1..12)
        int                     Month;

        /// The calendar day (1..31)
        int                     Day;

        /// The calendar day of week (this is ignored, when converted back to a time value)
        int                     DayOfWeek;

        /// The calendar hour (0..23)
        int                     Hour;

        /// The calendar minute (0..59)
        int                     Minute;

        /// The calendar second (0..59)
        int                     Second;

        /// The calendar millisecond (0..999)
        int                     Millisecond;

    /** ********************************************************************************************
     * Constructs an unset object.
     * @param init If \c Initialization::Perform, #Clear is invoked. Otherwise fields are not
     *             initialized.
     *             Defaults to \c Initialization::Perform.
     **********************************************************************************************/
    inline
    CalendarDateTime(Initialization init= Initialization::Perform)
    {
        if(init == Initialization::Perform )
            Clear();
    }

    /** ********************************************************************************************
     * Constructs the object according to the given timestamp object and time zone.
     * @param timeStamp The point in time to use for setting the public fields
     * @param timezone  Denotes if the time that is calculated should be local or UTC.
     *                  Defaults to \c TimeZone::Local.
     **********************************************************************************************/
    inline
    CalendarDateTime( const DateTime& timeStamp, lang::Timezone timezone =lang::Timezone::Local )
    {
        Set( timeStamp, timezone );
    }

    /** ********************************************************************************************
     * Constructs the object according to the given date and time values.
     * @param year        The year of the calendar time.
     * @param month       The month of the calendar time.
     * @param day         The day of the calendar time.
     * @param hour        The hour of the calendar time.
     * @param minute      The minute of the calendar time.
     * @param second      The second of the calendar time.
     * @param millisecond The millisecond of the calendar time.
     **********************************************************************************************/
    inline
    CalendarDateTime( int year, int month= 1, int day= 1,
                  int hour= 0, int minute= 0, int second= 0, int millisecond= 0 )
    : Year        ( year       )
    , Month       ( month      )
    , Day         ( day        )
    , Hour        ( hour       )
    , Minute      ( minute     )
    , Second      ( second     )
    , Millisecond ( millisecond)
    {}

    /** ********************************************************************************************
     * Sets the public fields according to the given timestamp object.
     * @param timeStamp The point in time to use for setting the public fields
     * @param timezone  Denotes if the time that is calculated should be local or UTC.
     *                  Defaults to \c TimeZone::Local.
     **********************************************************************************************/
    ALIB_API
    void        Set( const DateTime& timeStamp, lang::Timezone timezone =lang::Timezone::Local );

    /** ********************************************************************************************
     * Takes the current values of the public fields and calculates a time stamp object.
     * \attention
     * The resolution and possible time range of class \b %DateTime is platform dependent.
     * This method must not be used to create time stamp for dates which are out of range.
     *
     * @param timezone Denote if the time that is calculated should be local or UTC.
     *                 Defaults to \c TimeZone::Local.
     * @returns The point in time represented by the public fields of this class.
     **********************************************************************************************/
    ALIB_API
    DateTime    Get( lang::Timezone timezone =lang::Timezone::Local );

    /** ********************************************************************************************
     * Sets all public values to \c 0.
     **********************************************************************************************/
    ALIB_API
    void        Clear();

    /** ********************************************************************************************
     * Formats the date using a given pattern string. Within the pattern string, different symbols
     * are interpreted as tokens. The format is compatible with C# time format strings, as well as
     * with class SimpleDateFormat of the Java APIs.<br>
     * Strings within the format text that should not be interpreted as tokens may be surrounded
     * by single quotes. Two single quotes in a row, will be replaced by one single quote.<br>
     * The following tokens are supported:
     *
     *  <center>Token</center>  | <center>Description</center>
     *  - - - - -| - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
     *   y       |The year with as many digits as it has (for current dates this is 4).</TD> </TR>
     *   yy      |The year, truncated to 2 digits (modulo 100).</TD> </TR>
     *   yyy...y |The year with a minimum amount of digits as amount of y-characters given.</TD> </TR>
     *   M       |The month as numbers from 1..12.</TD> </TR>
     *   MM      |The month as numbers from 01..12.</TD> </TR>
     *   MMM     |The month as abbreviated, 3-digit word defined by resourced strings (defaults to English language).</TD> </TR>
     *   MMMM    |The month as word defined by resourced strings (defaults to English language).</TD> </TR>
     *   d       |The day as numbers from 1..31.</TD> </TR>
     *   dd      |The day as numbers from 01..31.</TD> </TR>
     *   ddd     |The day as abbreviated, 3-digit word defined by resourced strings (defaults to English language).</TD> </TR>
     *   dddd    |The day as word defined by resourced strings (defaults to English language).</TD> </TR>
     *   H       |The hour as numbers from 0..23.</TD> </TR>
     *   HH      |The hour as numbers from 00..23.</TD> </TR>
     *   K       |The hour as numbers from 0..11 am/pm.</TD> </TR>
     *   KK      |The hour as numbers from 00..11 am/pm.</TD> </TR>
     *   m       |The minute as numbers from 0..59.</TD> </TR>
     *   mm      |The minute as numbers from 00..59.</TD> </TR>
     *   s       |The second as numbers from 0..59.</TD> </TR>
     *   ss      |The second as numbers from 00..59.</TD> </TR>
     *
     * @param format     The format pattern string.
     * @param target     A reference to an AString that gets the result of the format processing
     *                   appended.
     * @param targetData If \c CurrentData::Keep (the default) the string is appended to \p{target}.
     *                   if \c CurrentData::Clear, \p{target} is cleared.
     * @returns \p{target} (for convenience).
     **********************************************************************************************/
    ALIB_API
    AString&    Format( Substring format, AString& target,
                        lib::lang::CurrentData targetData= lib::lang::CurrentData::Keep  );
};

/** ************************************************************************************************
 * This class represents a time span, measured in human units like days, hours, minutes and so on.
 * Besides conversion from and to nanoseconds, conversions from and to objects of types
 * \alib{time,TimePointBase::Duration,DateTime::Duration} and
 * \alib{time,TimePointBase::Duration,Ticks::Duration} is supported.
 **************************************************************************************************/
class CalendarDuration
{
    public:
        /// The number of days within the duration
        int                     Days;

        /// The number of hours (not the total, hence 0-23) within the duration.
        int                     Hours;

        /// The number of minutes (not the total, hence 0-59) within the duration.
        int                     Minutes;

        /// The number of seconds (not the total, hence 0-59) within the duration.
        int                     Seconds;

        /// The number of milliseconds (not the total, hence 0-999) within the duration.
        int                     Milliseconds;

        /// The number of microseconds (not the total, hence 0-999) within the duration.
        int                     Microseconds;

        /// The number of nanoseconds (not the total, hence 0-999) within the duration.
        int                     Nanoseconds;

    /** ********************************************************************************************
     * Constructs the object to represent a duration of 0. (Sets all public fields to 0.)
     * @param init If \c Initialization::Perform, #Clear is invoked. Otherwise fields are not
     *             initialized.
     *             Defaults to \c Initialization::Perform.
     **********************************************************************************************/
    inline                      CalendarDuration(Initialization init= Initialization::Perform)
    {
        if(init == Initialization::Perform )
            Clear();
    }

    /** ********************************************************************************************
     * Constructs the object using the given duration measured in nanoseconds.
     * Invokes #FromNanoSeconds.
     * @param nanos The duration to use for setting the public fields.
     **********************************************************************************************/
    inline                      CalendarDuration( int64_t nanos )
    {
        FromNanoSeconds( nanos );
    }

    /** ********************************************************************************************
     * Constructs the object using the given duration object.
     * Invokes #FromDuration.
     * @param duration The duration to use for setting the public fields.
     **********************************************************************************************/
    inline                      CalendarDuration( DateTime::Duration duration )
    {
        FromDuration( duration );
    }

    /** ********************************************************************************************
     * Constructs the object using the given duration object.
     * Invokes #FromDuration.
     * @param duration The duration to use for setting the public fields.
     **********************************************************************************************/
    inline                      CalendarDuration( Ticks::Duration  duration )
    {
        FromDuration( duration );
    }

    /** ********************************************************************************************
     * Sets the public fields to represent the given duration value.
     * The state of the object will hereafter be the same as it was when constructed with the same
     * parameter.
     * @param duration The duration to use for setting the public fields.
     **********************************************************************************************/
    inline void                 FromDuration( DateTime::Duration duration )
    {
        FromNanoSeconds( duration.InNanoseconds() );
    }

    /** ********************************************************************************************
     * Sets the public fields to represent the given duration value.
     * The state of the object will hereafter be the same as it was when constructed with the same
     * parameter.
     * @param duration The duration to use for setting the public fields.
     **********************************************************************************************/
    inline void                 FromDuration( Ticks::Duration duration )
    {
        FromNanoSeconds( duration.InNanoseconds() );
    }

    /** ********************************************************************************************
     * Takes the current values of the public fields and returns a duration value compatible with
     * class \b %DateTime.
     * @returns The duration represented by the public fields of this class.
     **********************************************************************************************/
    inline  DateTime::Duration  ToDateTimeDuration()
    {
        return DateTime::Duration::FromNanoseconds( ToNanoSeconds() );
    }

    /** ********************************************************************************************
     * Takes the current values of the public fields and returns a duration value compatible with
     * class \b %Ticks.
     * @returns The duration represented by the public fields of this class.
     **********************************************************************************************/
    inline  Ticks::Duration     ToTicksDuration()
    {
        return Ticks::Duration::FromNanoseconds( ToNanoSeconds() );
    }

    /** ********************************************************************************************
     * Sets the public fields to represent the given duration value.
     * The state of the object will hereafter be the same as it was when constructed with the same
     * parameter.
     * @param nanos The duration to use for setting the public fields.
     **********************************************************************************************/
    ALIB_API void               FromNanoSeconds( int64_t nanos );

    /** ********************************************************************************************
     * Takes the current values of the public fields and returns the duration.
     * @returns The duration represented by the public fields of this class in nanoseconds.
     **********************************************************************************************/
    ALIB_API int64_t            ToNanoSeconds();
    /** ********************************************************************************************
     * Sets all public values to 0.
     **********************************************************************************************/
    ALIB_API void               Clear();
};

}} // namespace lib::time

/// Type alias in namespace #aworx.
using     CalendarDateTime  =    aworx::lib::time::CalendarDateTime;

/// Type alias in namespace #aworx.
using     CalendarDuration  =    aworx::lib::time::CalendarDuration;

}  // namespace aworx

#endif // HPP_ALIB_TIME_CALENDAR
