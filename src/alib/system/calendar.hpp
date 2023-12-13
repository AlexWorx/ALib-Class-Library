/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_system of the \aliblong.
 *
 * \emoji :copyright: 2013-2023 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_SYSTEM_CALENDAR
#define HPP_ALIB_SYSTEM_CALENDAR 1

#if !defined (HPP_ALIB_TIME_DATETIME)
    #include "alib/time/datetime.hpp"
#endif

ALIB_ASSERT_MODULE(SYSTEM)
ALIB_ASSERT_MODULE(TIME)

#if !defined (HPP_ALIB_TIME_TICKS)
    #include "alib/time/ticks.hpp"
#endif

#if !defined (HPP_ALIB_STRINGS_ASTRING)
#   include "alib/strings/astring.hpp"
#endif

#if !defined (HPP_ALIB_STRINGS_SUBSTRING)
#   include "alib/strings/substring.hpp"
#endif


namespace aworx { namespace lib { namespace system {

/** ************************************************************************************************
 * This class represents a point in time as a set of calendar and clock values
 * (year, month, day, hour, ...).
 * It provides methods to convert to and from objects of type \alib{time,DateTime}.
 * In addition, a method to format the date and time into human readable string value is available.
 *
 * \note
 *   The conversion from and into objects of type \alib{time,Ticks} is intentionally not supported.
 *   In the case that such objects should be used with this class, an additional conversion
 *   step has to be performed using class \alib{time,TickConverter}. In other words, conversion can
 *   be performed as follows:
 *
 *          CalendarDateTime <=> DateTime <=> Ticks
 * <p>
 * \note
 *   This class is using system specific calendar methods and relies on the locale and time zone
 *   settings of the machine.
 *
 * <p>
 * \note
 *   This class is part of module \alib_system. While it is a pure utility class for type
 *   \alib{time,DateTime} found in module \alib_time, the class was located in module \b %System
 *   to keep module \b %Time lean and free from dependencies to modules \alib_text and
 *   all its sub-dependencies.
 **************************************************************************************************/
class CalendarDateTime
{
    public:
        /** The calendar year (e.g. 2022). */
        int                     Year;

        /** The calendar month (1..12). */
        int                     Month;

        /** The calendar day (1..31). */
        int                     Day;

        /** The calendar hour (0..23). */
        int                     Hour;

        /** The calendar minute (0..59). */
        int                     Minute;

        /** The calendar second (0..59). */
        int                     Second;

        /** The calendar millisecond (0..999). */
        int                     Millisecond;

        /** The calendar day of week (0==Sunday..6==Saturday).
         *  \attention This value is only set when constructed with a \b DateTime object and
         *             set to \c -1 if constructed with single values, or if method #Clear is
         *             invoked.                                                                 */
        int                     DayOfWeek;
    /** ********************************************************************************************
     * Constructs an unset object.
     * @param init If \c Initialization::Perform, #Clear is invoked. Otherwise fields are not
     *             initialized.
     *             Defaults to \c Initialization::Perform.
     **********************************************************************************************/
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
    CalendarDateTime( const DateTime& timeStamp, Timezone timezone =Timezone::Local )
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
    CalendarDateTime( int year   , int month= 1 , int day= 1,
                      int hour= 0, int minute= 0, int second= 0, int millisecond= 0 )
    : Year        ( year       )
    , Month       ( month      )
    , Day         ( day        )
    , Hour        ( hour       )
    , Minute      ( minute     )
    , Second      ( second     )
    , Millisecond ( millisecond)
    , DayOfWeek   ( -1         )
    {}

    /** ********************************************************************************************
     * Sets the public fields according to the given timestamp object.
     * @param timeStamp The point in time to use for setting the public fields
     * @param timezone  Denotes if the time that is calculated should be local or UTC.
     *                  Defaults to \c TimeZone::Local.
     **********************************************************************************************/
    ALIB_API
    void        Set( const DateTime& timeStamp, Timezone timezone =Timezone::Local );

    /** ********************************************************************************************
     * Creates a \b DateTime object from this calendar date.
     * \attention
     * The resolution and possible time range of class \b %DateTime is platform dependent.
     * This method must not be used if inconsistent values are stored.
     *
     * @param timezone Denote if the time that is calculated should be local or UTC.
     *                 Defaults to \c TimeZone::Local.
     * @returns The point in time represented by the public fields of this class.
     **********************************************************************************************/
    ALIB_API
    DateTime    Get( Timezone timezone =Timezone::Local )                                     const;

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
    AString&    Format( Substring format,   AString& target,
                        CurrentData targetData= CurrentData::Keep)                            const;
};

/** ************************************************************************************************
 * This class represents a time span, measured in human units like days, hours, minutes and so on.
 * Besides conversion from and to nanoseconds, conversions from and to objects of types
 * \alib{time,TimePointBase::Duration,DateTime::Duration} and
 * \alib{time,TimePointBase::Duration,Ticks::Duration} is supported.
 *
 * \note
 *   This class is part of module \alib_system. While it is a pure utility class for types
 *   \alib{time,DateTime} and \alib{time,Ticks} found in module \alib_time, the class was located
 *   in module \b %System to keep module \b %Time lean and free from dependencies to module
 *   \alib_text and all its sub-dependencies.
 **************************************************************************************************/
class CalendarDuration
{
    public:
        /** The number of days within the duration */
        int                     Days;

        /** The number of hours (not the total, hence 0-23) within the duration. */
        int                     Hours;

        /** The number of minutes (not the total, hence 0-59) within the duration. */
        int                     Minutes;

        /** The number of seconds (not the total, hence 0-59) within the duration. */
        int                     Seconds;

        /** The number of milliseconds (not the total, hence 0-999) within the duration. */
        int                     Milliseconds;

        /** The number of microseconds (not the total, hence 0-999) within the duration. */
        int                     Microseconds;

        /** The number of nanoseconds (not the total, hence 0-999) within the duration. */
        int                     Nanoseconds;

    /** ********************************************************************************************
     * Constructs the object to represent a duration of 0. (Sets all public fields to 0.)
     * @param init If \c Initialization::Perform, #Clear is invoked. Otherwise fields are not
     *             initialized.
     *             Defaults to \c Initialization::Perform.
     **********************************************************************************************/
                                CalendarDuration(Initialization init= Initialization::Perform)
    {
        if(init == Initialization::Perform )
            Clear();
    }

    /** ********************************************************************************************
     * Constructs the object using the given duration measured in nanoseconds.
     * Invokes #FromNanoSeconds.
     * @param nanos The duration to use for setting the public fields.
     **********************************************************************************************/
                                 CalendarDuration( int64_t nanos )
    {
        FromNanoSeconds( nanos );
    }

    /** ********************************************************************************************
     * Constructs the object using the given duration object.
     * Invokes #FromDuration.
     * @param duration The duration to use for setting the public fields.
     **********************************************************************************************/
                                 CalendarDuration( DateTime::Duration duration )
    {
        FromDuration( duration );
    }

    /** ********************************************************************************************
     * Constructs the object using the given duration object.
     * Invokes #FromDuration.
     * @param duration The duration to use for setting the public fields.
     **********************************************************************************************/
                                 CalendarDuration( Ticks::Duration  duration )
    {
        FromDuration( duration );
    }

    /** ********************************************************************************************
     * Sets the public fields to represent the given duration value.
     * The state of the object will hereafter be the same as it was when constructed with the same
     * parameter.
     * @param duration The duration to use for setting the public fields.
     **********************************************************************************************/
    void                        FromDuration( DateTime::Duration duration )
    {
        FromNanoSeconds( duration.InNanoseconds() );
    }

    /** ********************************************************************************************
     * Sets the public fields to represent the given duration value.
     * The state of the object will hereafter be the same as it was when constructed with the same
     * parameter.
     * @param duration The duration to use for setting the public fields.
     **********************************************************************************************/
    void                        FromDuration( Ticks::Duration duration )
    {
        FromNanoSeconds( duration.InNanoseconds() );
    }

    /** ********************************************************************************************
     * Takes the current values of the public fields and returns a duration value compatible with
     * class \b %DateTime.
     * @returns The duration represented by the public fields of this class.
     **********************************************************************************************/
    DateTime::Duration          ToDateTimeDuration()
    {
        return DateTime::Duration::FromNanoseconds( ToNanoSeconds() );
    }

    /** ********************************************************************************************
     * Takes the current values of the public fields and returns a duration value compatible with
     * class \b %Ticks.
     * @returns The duration represented by the public fields of this class.
     **********************************************************************************************/
    Ticks::Duration             ToTicksDuration()
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



/** ************************************************************************************************
 * Represents a date in the systems calendar without the provision of a clock time.
 * This class internally uses types \alib{time,DateTime} and \alib{system,CalendarDateTime} to
 * perform operators <c>+</c>, <c>-</c>, <c>+=</c> and <c>-=</c>, but is much more efficient with
 * in- and decrement operators <c>++</c> and <c>--</c>. Furthermore it uses only 32-bits of storage.
 *
 * Besides storing clock-time agnostic date values, the type is useful to securely iterate over
 * dates, because there is no risk of false comparisons due to mixed time-zone and daylight
 * saving properties and in general due to arbitrarily set clock times.
 *
 * Another small difference is that field \alib{system::CalendarDate,DayOfWeek} is always kept in a
 * reliable (correct) state, which is not the case with field
 * \alib{system,CalendarDateTime::DayOfWeek}.
 *
 * Internally,  the values are stored a 32-bit storage word using the following scheme
 * - bits 1-3   encode the day of week (from 0 (= Sunday) to 6 (= Saturday)
 * - bits 4-8   encode the calendar day (1..31)
 * - bits 9-12  encode the calendar month (1..12)
 * - bits 13-32 encode the calendar year
 * consequently, for the calendar year 20 bits are available, which evaluates to a range of
 * \b 1,048,576 years.
 * For simplicity and speed, the values are \b not defined in accordance to class
 * \alib{system,CalendarDateTime}, where a value of \c 0 represents year \b 1900.
 * This class just stores the year value as is and does not allow negative values.
 * As a result, this class is capable of storing values between <b>1/1/0000</b> and
 * <b>12/31/1,048,576</b>.
 **************************************************************************************************/
class CalendarDate
{
    protected:
        uint32_t        stamp; ///< Encoded date value.

    // #############################################################################################
    // Conversion to time platform/language specific values
    // #############################################################################################
    public:
        /** Default constructor leaving this object uninitialized (random value).   ***************/
        CalendarDate()                                                                    = default;

        /** Trivial default copy constructor.  ****************************************************/
        CalendarDate( const CalendarDate&  )              noexcept                        = default;

        /** Trivial default move constructor.  ****************************************************/
        CalendarDate(       CalendarDate&& )              noexcept                        = default;

        /** Trivial default copy assign operator.
         * @return A reference to \c this.     ****************************************************/
        CalendarDate& operator=( const CalendarDate&  )   noexcept                        = default;

        /** Trivial default move assign operator.
         * @return A reference to \c this.     ****************************************************/
        CalendarDate& operator=(       CalendarDate&& )   noexcept                        = default;

        /** Trivial default destructor.        ****************************************************/
        ~CalendarDate()                           noexcept                                = default;

        /** ****************************************************************************************
         * Constructor taking the date as separated values.
         * @param year      The year to use. Must be between 0 and 1,048,575
         * @param month     The month to use. Must be between 1 and 12
         * @param day       The day to use. Must be between 1 and 31
         * @param dayOfWeek The day of week that results from the previous values.
         *                  Defaults to \c -1 wich instructs this constructor to retrieve that
         *                  correct value.<br>
         *                  If given, values must be between 0 (Sunday) and 6 (Saturday).
         *                  In debug compilations an assertion will be raised if the
         *                  value is out of range or inconsistent.
         ******************************************************************************************/
        CalendarDate(int year, int month, int day, int dayOfWeek= -1)
        {
            Set( year, month, day, dayOfWeek );
        }

        /** ****************************************************************************************
         * Constructor creating a date that represents "today".
         * @param timezone  Determines whether the local time zone should be used or UTC.
         ******************************************************************************************/
        explicit
        CalendarDate( Timezone timezone )
        {
            Set( DateTime(Initialization::Perform), timezone);
        }

        /** ****************************************************************************************
         * Constructor taking a \b DateTime value.
         * @param calendarDateTime The value to take the date from.
         ******************************************************************************************/
        ALIB_API explicit
        CalendarDate( const CalendarDateTime& calendarDateTime )
        {
            Set( calendarDateTime.Year
                ,calendarDateTime.Month
                ,calendarDateTime.Day
                ,calendarDateTime.DayOfWeek );
        }

        /** ****************************************************************************************
         * Constructor taking a \b DateTime value.
         * @param dateTime The value to take the date from.
         * @param timezone Determines whether the local time zone should be used or UTC.
         ******************************************************************************************/
        CalendarDate( const DateTime& dateTime, Timezone timezone )
        {
            Set( dateTime, timezone);
        }


        /** ****************************************************************************************
         * Sets this class to the given values.
         * @param year      The year to use. Must be between 0 and 1,048,575
         * @param month     The month to use. Must be between 1 and 12
         * @param day       The day to use. Must be between 1 and 31
         * @param dayOfWeek The day of week that results from the previous values.
         *                  Defaults to \c -1 wich instructs this constructor to retrieve that
         *                  correct value.<br>
         *                  If given, values must be between 0 (Sunday) and 6 (Saturday).
         *                  In debug compilations an assertion will be raised if the
         *                  value is out of range or inconsistent.
         ******************************************************************************************/
        ALIB_API
        void    Set( int year, int month, int day, int dayOfWeek= -1);

        /** ****************************************************************************************
         * Sets this class to the date represented by the given \b DateTime instance.
         * @param dateTime The value to take the date from.
         * @param timezone Determines whether the local time zone applies to \p dateTime or
         *                 whether it refers to UTC.
         ******************************************************************************************/
        ALIB_API
        void    Set( const DateTime& dateTime, Timezone timezone );

        /** ****************************************************************************************
         * Creates a \b DateTime object from this calendar date.
         * \attention
         * The resolution and possible time range of class \b %DateTime is platform dependent.
         * This method must not be used if inconsistent values are stored.
         *
         * @param timezone Denote if the time that is calculated should be local or UTC.
         *                 Defaults to \c TimeZone::Local.
         * @param hour     The hour of day (0..23) that the value returned should represent.
         *                 Defaults to noon time (\c 12).
         * @param minute   The minute of the hour (0..59) that the value returned should represent.
         *                 Defaults to \c 0.
         * @param second   The second of the minute (0..59) that the value returned should
         *                 represent. Defaults to \c 0.
         * @returns The point in time represented by this class and the given clock values.
         ******************************************************************************************/
        ALIB_API
        DateTime    Get( Timezone timezone =Timezone::Local,
                         int hour= 12, int minute= 0, int second= 0 )                         const;

        /** ****************************************************************************************
         * Returns a date and time value, by adding 12 o'clock noon time.
         * @return A corresponding \b DateTime value.
         ******************************************************************************************/
        ALIB_API
        CalendarDateTime    ToCalendarDateTime()                                              const;

        //--------------------------  Year(), Month(), Day(), DayOfWeek()   ------------------------
        /** Extracts the day of month from this date as a value between \c 1 and \c 31.
         * @return The calendar day of month.                                                     */
        int         Year()                          const   {  return int(  stamp >> 12       );   }

        /** Extracts the month from this date as a value between \c 1 and \c 12.
         * @return The calendar day of month.                                                     */
        int         Month()                         const   {  return int( (stamp >>  8) & 15 );   }

        /** Extracts the day of month from this date as a value between \c 1 and \c 31.
         * @return The calendar day of month.                                                     */
        int         Day()                           const   {  return int( (stamp >>  3) & 31 );   }

        /** Extracts the day of week from this date as a value between \c 0 (representing sunday)
         * and \c 6 (representing Saturday) in accordance with field
         * \alib{system,CalendarDateTime::DayOfWeek}.
         * @return The calendar day of month.                                                     */
        int         DayOfWeek()                     const   {  return int(  stamp        &  7 );   }

        //--------------------------- +/- and ++/--/+=/-= operators    ----------------------------

        /** ****************************************************************************************
         * Adds the given number of days to this date.
         * @param daysToAdd  The days to add. May be negative for subtraction.
         * @return A copy of this object after modification.
         ******************************************************************************************/
        ALIB_API
        CalendarDate  operator+ ( int daysToAdd )                                             const;

        /** ****************************************************************************************
         * Subtracts the given number of days to this date.
         * @param daysToSubtract The days to add. May be negative for additions.
         * @return A copy of this object after modification.
         ******************************************************************************************/
        CalendarDate  operator- ( int daysToSubtract )                                         const
        {
            return (*this) + (-daysToSubtract);
        }

        /** ****************************************************************************************
         * Prefix increment operator.
         *  @return A copy of this object after modification.
         ******************************************************************************************/
        ALIB_API
        CalendarDate operator++();

        /** ****************************************************************************************
         * Prefix decrement operator.
         *  @return A copy of this object after modification.
         ******************************************************************************************/
        ALIB_API
        CalendarDate operator--();

        /** ****************************************************************************************
         * Postfix increment operator.
         *  @return A copy of this object prior to its modification.
         ******************************************************************************************/
        CalendarDate operator++(int)
        {
            auto tmp = *this;
            ++(*this);
            return tmp;
        }

        /** ****************************************************************************************
         * Postfix decrement operator.
         *  @return A copy of this object prior to its modification.
         ******************************************************************************************/
        CalendarDate operator--(int)
        {
            auto tmp = *this;
            --(*this);
            return tmp;
        }

        /** ****************************************************************************************
         * Adds the given number of days to this date.
         * @param daysToAdd  The days to add. May be negative for subtraction.
         * @return A copy of this object after modification.
         ******************************************************************************************/
        CalendarDate  operator+=( int daysToAdd )
        {
            return (*this)= (*this) + daysToAdd;
        }

        /** ****************************************************************************************
         * Subtracts the given number of days to this date.
         * @param daysToSubtract The days to add. May be negative for additons.
         * @return A copy of this object after modification.
         ******************************************************************************************/
        CalendarDate  operator-=( int daysToSubtract )
        {
            return (*this)+= (- daysToSubtract);
        }


        //------------------------------------    Comparison    ------------------------------------
        /** ****************************************************************************************
         * Equal to operator.
         * @param other The date stamp to compare.
         * @return The result of the comparison.
         ******************************************************************************************/
        bool   operator==( const CalendarDate& other )                                         const
        {
            return stamp == other.stamp;
        }

        /** ****************************************************************************************
         * Not equal to operator.
         * @param other The date stamp to compare.
         * @return The result of the comparison.
         ******************************************************************************************/
        bool   operator!=( const CalendarDate& other )                                         const
        {
            return stamp != other.stamp;
        }

        /** ****************************************************************************************
         * Less than operator.
         * @param other The date stamp to compare.
         * @return A reference to this object.
         ******************************************************************************************/
        bool   operator<( const CalendarDate& other )                                          const
        {
            return stamp <  other.stamp;
        }

        /** ****************************************************************************************
         * Less than or equal to operator.
         * @param other The date stamp to compare.
         * @return The result of the comparison.
         ******************************************************************************************/
        bool   operator<=( const CalendarDate& other )                                         const
        {
            return stamp <=  other.stamp;
        }

        /** ****************************************************************************************
         * Greater than operator.
         * @param other The date stamp to compare.
         * @return The result of the comparison.
         ******************************************************************************************/
        bool   operator>( const CalendarDate& other )                                          const
        {
            return stamp >  other.stamp;
        }

        /** ****************************************************************************************
         * Greater than or equal to operator.
         * @param other The date stamp to compare.
         * @return The result of the comparison.
         ******************************************************************************************/
        bool   operator>=( const CalendarDate& other )                                        const
        {
            return stamp >=  other.stamp;
        }
};  // class CalendarDate



} namespace strings {


#if defined(ALIB_DOX)
namespace APPENDABLES { // Documentation fake namespace
#endif
/** Specialization of functor \alib{strings,T_Append} for type
 *  \alib{time,TimePointBase::Duration,DateTime::Duration}.
 *  \note
 *    This specialization is available only if module \alib_text is included in the
 *    \alibdist and a using source file includes header \alibheader{text/text.hpp}.
 */
template<typename TChar> struct T_Append<time::DateTime::Duration ,TChar>
{
    /** ********************************************************************************************
     * Appends a human readable string representation of objects of templated inner type
     * \alib{time,TimePointBase::Duration} of type \alib{time,DateTime}.
     *
     * \see
     *   For details of the conversion, see
     *   \ref anchor_T_Append_TimePointBase_Duration "documentation of sibling method"
     *   for type \b %Ticks, which shares this method's implementation.
     *
     *
     * @param target    The \b AString that \b Append was invoked on.
     * @param duration  The duration to append.
     **********************************************************************************************/
    ALIB_API void operator()( TAString<TChar>& target,  const time::DateTime::Duration duration );
};

/** Specialization of functor \alib{strings,T_Append} for type
 *  \alib{time,TimePointBase::Duration,Ticks::Duration}.
 *  \note
 *    This specialization is available only if module \alib_text is included in the
 *    \alibdist and a using source file includes header \alibheader{text/text.hpp}.
 */
template<typename TChar> struct T_Append<time::Ticks::Duration ,TChar>
{
    /** ********************************************************************************************
     * \anchor anchor_T_Append_TimePointBase_Duration
     * Appends a human readable string representation of objects of templated inner type
     * \alib{time,TimePointBase::Duration} of type \alib{time,Ticks}.
     *
     * Depending on the length of the duration, a different human readable time unit or combination
     * of it is used. The following rules are checked from top to bottom:
     * - If zero, resource string \b "TS_ZERO" is written.
     * - If negative, a minus sign <c>'-'</c> is written and the value is negated.
     * - If greater than 10 days, writes the number of days as floating point number.
     * - If between 1 and 10 days, writes the integral number of days and the additional hours as
     *   floating point number.
     * - If greater than an hour, writes the integral number of hours and integral minutes.
     * - If greater than a minute, writes the integral number of minutes and integral seconds.
     * - If greater than a second, writes the number of seconds as floating point number.
     * - If greater than a millisecond, writes the number of milliseconds as floating point number.
     * - If greater than a microsecond, writes the number of microseconds as floating point number.
     * - If greater than a nanosecond, writes the number of nanoseconds as floating point number.
     *
     * All floating point numbers are written with two digits fractional precision.<br>
     * The unit symbols are read from the \ref alib_mod_resources "resources" of module class
     * \alib{text,Text}. The list is given with resource name <b>UNITS</b>.
     *
     *
     * @param target    The \b AString that \b Append was invoked on.
     * @param duration  The duration to append.
     **********************************************************************************************/
    void operator()( TAString<TChar>& target,  const time::Ticks::Duration duration );
};

extern template ALIB_API void T_Append<time::Ticks::Duration, nchar>::operator()( TAString<nchar>&, const time::Ticks::Duration );
extern template ALIB_API void T_Append<time::Ticks::Duration, wchar>::operator()( TAString<wchar>&, const time::Ticks::Duration );
extern template ALIB_API void T_Append<time::Ticks::Duration, xchar>::operator()( TAString<xchar>&, const time::Ticks::Duration );

#if defined(ALIB_DOX)
} // APPENDABLES documentation fake namespace
#endif


}} // namespace aworx[::lib::strings]

/// Type alias in namespace #aworx.
using     CalendarDateTime  =    lib::system::CalendarDateTime;

/// Type alias in namespace #aworx.
using     CalendarDuration  =    lib::system::CalendarDuration;

/// Type alias in namespace #aworx.
using     CalendarDate      =    lib::system::CalendarDate;

}  // namespace [aworx]

#endif // HPP_ALIB_SYSTEM_CALENDAR
