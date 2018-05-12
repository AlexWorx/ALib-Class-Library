// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################

#ifndef HPP_ALIB_EXPRESSIONS_PLUGINS_DATEANDTIME
#define HPP_ALIB_EXPRESSIONS_PLUGINS_DATEANDTIME

#ifndef HPP_ALIB_EXPRESSIONS_PLUGINS_CALCULUS
#   include "../plugins/calculus.hpp"
#endif

#ifndef HPP_ALIB_TIME_DATETIME
#   include "alib/time/datetime.hpp"
#endif


namespace aworx { namespace lib { namespace expressions { namespace plugins {

/** ************************************************************************************************
 * %Compiler plug-in that provides date an time related expression terms.
 *
 * The identifiers, functions and operators offered by this compiler plug-in introduce \alib
 * types \alib{time,DateTime} and \alib{time,TimePointBase::Duration} to be used with
 * module \alibmod_nolink_expressions.
 *
 * All identifier and function names are defined case insensitive.
 *
 * <b>Constants:</b><br>
 *
 * Type    | Name  |Min. Abbreviation| Description
 * --------|-------|-----------------|-------------
 * Integer |\b January  | jan | Used to compare the result of functions \p{GetMonth} and \p{GetUTCMonth}.
 * Integer |\b February | feb | Used to compare the result of functions \p{GetMonth} and \p{GetUTCMonth}.
 * Integer |\b March    | mar | Used to compare the result of functions \p{GetMonth} and \p{GetUTCMonth}.
 * Integer |\b April    | apr | Used to compare the result of functions \p{GetMonth} and \p{GetUTCMonth}.
 * Integer |\b May      | may | Used to compare the result of functions \p{GetMonth} and \p{GetUTCMonth}.
 * Integer |\b June     | jun | Used to compare the result of functions \p{GetMonth} and \p{GetUTCMonth}.
 * Integer |\b July     | jul | Used to compare the result of functions \p{GetMonth} and \p{GetUTCMonth}.
 * Integer |\b August   | aug | Used to compare the result of functions \p{GetMonth} and \p{GetUTCMonth}.
 * Integer |\b September| sep | Used to compare the result of functions \p{GetMonth} and \p{GetUTCMonth}.
 * Integer |\b October  | oct | Used to compare the result of functions \p{GetMonth} and \p{GetUTCMonth}.
 * Integer |\b November | nov | Used to compare the result of functions \p{GetMonth} and \p{GetUTCMonth}.
 * Integer |\b December | dec | Used to compare the result of functions \p{GetMonth} and \p{GetUTCMonth}.

 * Integer |\b Sunday   | sun | Used to compare the result of functions \p{GetDayOfWeek} and \p{GetUTCDayOfWeek}.
 * Integer |\b Monday   | mon | Used to compare the result of functions \p{GetDayOfWeek} and \p{GetUTCDayOfWeek}.
 * Integer |\b Tuesday  | tue | Used to compare the result of functions \p{GetDayOfWeek} and \p{GetUTCDayOfWeek}.
 * Integer |\b Wednesday| wed | Used to compare the result of functions \p{GetDayOfWeek} and \p{GetUTCDayOfWeek}.
 * Integer |\b Thursday | thu | Used to compare the result of functions \p{GetDayOfWeek} and \p{GetUTCDayOfWeek}.
 * Integer |\b Friday   | fri | Used to compare the result of functions \p{GetDayOfWeek} and \p{GetUTCDayOfWeek}.
 * Integer |\b Saturday | sat | Used to compare the result of functions \p{GetDayOfWeek} and \p{GetUTCDayOfWeek}.
 *
 *
 * <br>
 * <b>Constant Constructor Functions:</b><br>
 *
 * Return Type    | Name      |Min. Abbreviation| Signature | Description |
 * ---------------|-----------|-----------------|-----------|-------------|
 * DateTime |\b DateTime      | times  |int,...| Returns a time stamp representing the calendar date in local time zone, respecting daylight saving. The first parameter is required and provides the \p{year}. Further parameters are optional and provide \p{month} (defaults to \c 1), \p{dayOfMonth} (defaults to \c 1), \p{hour} (defaults to \c 0), \p{minute} (defaults to \c 0) and \p{millisecond} (defaults to \c 0).
 * DateTime |\b UTCDateTime   | utcti  |int,...| Returns a time stamp representing the calendar date in UTC time, not respecting daylight saving. The first parameter is required and provides the \p{year}. Further parameters are optional and provide \p{month} (defaults to \c 1), \p{dayOfMonth} (defaults to \c 1), \p{hour} (defaults to \c 0), \p{minute} (defaults to \c 0) and \p{millisecond} (defaults to \c 0).
 * Duration |\b Nanoseconds   | nano   |int    | Returns a time span object representing the given number of nanoseconds.
 * Duration |\b Microseconds  | mic    |int    | Returns a time span object representing the given (absolute) number of microseconds.
 * Duration |\b Microseconds  | mic    |double | Returns a time span object representing the given (rational) number of microseconds.
 * Duration |\b Milliseconds  | mil    |int    | Returns a time span object representing the given (absolute) number of milliseconds.
 * Duration |\b Milliseconds  | mil    |double | Returns a time span object representing the given (rational) number of milliseconds.
 * Duration |\b Seconds       | sec    |int    | Returns a time span object representing the given (absolute) number of seconds.
 * Duration |\b Seconds       | sec    |double | Returns a time span object representing the given (rational) number of seconds.
 * Duration |\b Minutes       | min    |int    | Returns a time span object representing the given (absolute) number of minutes.
 * Duration |\b Minutes       | min    |double | Returns a time span object representing the given (rational) number of minutes.
 * Duration |\b Hours         | hour   |int    | Returns a time span object representing the given (absolute) number of hours.
 * Duration |\b Hours         | hour   |double | Returns a time span object representing the given (rational) number of hours.
 * Duration |\b Days          | day    |int    | Returns a time span object representing the given (absolute) number of days.
 * Duration |\b Days          | day    |double | Returns a time span object representing the given (rational) number of days.
 * Duration |\b Weeks         | wee    |int    | Returns a time span object representing the given (absolute) number of weeks.
 * Duration |\b Weeks         | wee    |double | Returns a time span object representing the given (rational) number of weeks.
 * Duration |\b Months        | mon    |int    | Returns a time span object representing the given (absolute) number of months (30 days).
 * Duration |\b Months        | mon    |double | Returns a time span object representing the given (rational) number of months (30 days).
 * Duration |\b Years         | year   |int    | Returns a time span object representing the given (absolute) number of years (365 days).
 * Duration |\b Years         | year   |double | Returns a time span object representing the given (rational) number of years (365 days).
 *
 * <br>
 * <b>Functions:</b><br>
 *
 * Return Type    | Name      | Min. Abbreviation| Signature | Description |
 * ---------------|-----------|------------------|-----------|-------------|
 * DateTime  | \b Now           |now           | ./.       | Returns the actual point in time.
 * DateTime  | \b Today         |today         | ./.       | Returns the point in time that represents the current date local date at midnight, respecting the local time zone and daylight saving.
 * DateTime  | \b UTCToday      |utctoday      | ./.       | Returns the point in time that represents the current date local date at midnight, respecting UTC time zone without daylight saving.
 * Duration  | \b Age           |age           | DateTime  | Returns the time span between the actual point in time and the given time stamp.
 * Boolean   | \b IsOlderThan   |iso           | DateTime ,Duration| Returns \c true if the time span between the actual point in time and the given point in time is greater than the given duration.
 * Integer   | \b Year          |y             | DateTime  | Returns the calendar year of the time stamp, respecting the local time zone and daylight saving.
 * Integer   | \b Month         |m             | DateTime  | Returns the calendar month of the time stamp, respecting the local time zone and daylight saving.
 * Integer   | \b Day           |d             | DateTime  | Returns the calendar day of the time stamp, respecting the local time zone and daylight saving.
 * Integer   | \b DayOfWeek     |dayO          | DateTime  | Returns the calendar dayOfWeek of the time stamp, respecting the local time zone and daylight saving.
 * Integer   | \b Hour          |hour          | DateTime  | Returns the calendar hour of the time stamp, respecting the local time zone and daylight saving.
 * Integer   | \b Minute        |m             | DateTime  | Returns the calendar Minute of the time stamp, respecting the local time zone and daylight saving.
 * Integer   | \b Millisecond   |mi            | DateTime  | Returns the calendar millisecond of the time stamp, respecting the local time zone and daylight saving.
 * Integer   | \b UTCYear       |utcy          | DateTime  | Returns the calendar year of the time stamp, respecting UTC time zone without daylight saving.
 * Integer   | \b UTCMonth      |utcmon        | DateTime  | Returns the calendar month of the time stamp, respecting UTC time zone without daylight saving.
 * Integer   | \b UTCDay        |utcd          | DateTime  | Returns the calendar day of the time stamp, respecting UTC time zone without daylight saving.
 * Integer   | \b UTCDayOfWeek  |utcdayO       | DateTime  | Returns the calendar dayOfWeek of the time stamp, respecting UTC time zone without daylight saving.
 * Integer   | \b UTCHour       |utch          | DateTime  | Returns the calendar hour of the time stamp, respecting UTC time zone without daylight saving.
 * Integer   | \b UTCMinute     |utcmin        | DateTime  | Returns the calendar minute of the time stamp, respecting UTC time zone without daylight saving.
 * Integer   | \b UTCMillisecond|utcmil        | DateTime  | Returns the calendar millisecond of the time stamp, respecting UTC time zone without daylight saving.
 * Float     | \b InDays        |ind           | Duration  | Returns the time span given in days.
 * Float     | \b InHours       |inh           | Duration  | Returns the time span given in hours.
 * Float     | \b InMinutes     |inm           | Duration  | Returns the time span given in minutes.
 * Float     | \b InSeconds     |ins           | Duration  | Returns the time span given in seconds.
 * Float     | \b InMilliseconds|inmil         | Duration  | Returns the time span given in milliseconds.
 * Float     | \b InMicroseconds|inmic         | Duration  | Returns the time span given in microseconds.
 * Float     | \b InNanoseconds |inn           | Duration  | Returns the time span given in nanoseconds.
 * Float     | \b InHertz       |inhe          | Duration  | Returns the time span given in hertz (1/second).
 *
 * <br>
 * <b>Binary Operators:</b><br>
 *
 * Return Type    | Lhs Type | Operator | Rhs Type | Description|
 * ---------------|----------|----------|----------|------------|
 * DateTime | DateTime |<c>'+'</c> | Duration | Adds a time span to a time stamp.
 * DateTime | Duration |<c>'+'</c> | DateTime | Adds a time span to a time stamp.
 * DateTime | DateTime |<c>'-'</c> | DateTime | Subtracts two time stamps resulting in a time span.
 * DateTime | DateTime |<c>'-'</c> | Duration | Subtracts a time span from time stamp.
 * Duration | Duration |<c>'+'</c> | Duration | Adds two time spans.
 * Duration | Duration |<c>'-'</c> | Duration | Subtracts rhs-value from lhs-value.
 * Duration | Duration |<c>'*'</c> | Float    | Multiplies a time span by a given floating point value.
 * Duration | Float    |<c>'*'</c> | Duration | Multiplies a time span by a given floating point value.
 * Duration | Duration |<c>'*'</c> | Integer  | Multiplies a time span by a given integral value.
 * Duration | Integer  |<c>'*'</c> | Duration | Multiplies a time span by a given integral value.
 * Duration | Duration |<c>'/'</c> | Float    | Divides a time span by a given floating point value.
 * Duration | Duration |<c>'/'</c> | Integer  | Divides a time span by a given integral value.
 * Boolean  | DateTime |<c>'=='</c>| DateTime | Compares two time stamp objects.
 * Boolean  | Duration |<c>'=='</c>| Duration | Compares two time span objects.
 * Boolean  | DateTime |<c>'!='</c>| DateTime | Compares two time stamp objects.
 * Boolean  | Duration |<c>'!='</c>| Duration | Compares two time span objects.
 * Boolean  | DateTime |<c>'<' </c>| DateTime | Compares two time stamp objects.
 * Boolean  | Duration |<c>'<' </c>| Duration | Compares two time span objects.
 * Boolean  | DateTime |<c>'<='</c>| DateTime | Compares two time stamp objects.
 * Boolean  | Duration |<c>'<='</c>| Duration | Compares two time span objects.
 * Boolean  | DateTime |<c>'>' </c>| DateTime | Compares two time stamp objects.
 * Boolean  | Duration |<c>'>' </c>| Duration | Compares two time span objects.
 * Boolean  | DateTime |<c>'>='</c>| DateTime | Compares two time stamp objects.
 * Boolean  | Duration |<c>'>='</c>| Duration | Compares two time span objects.
 *
 * # Reference Documentation #
 **************************************************************************************************/
struct DateAndTime   : public plugins::Calculus
{
    /** ********************************************************************************************
     * Constructor. Creates the hash maps.
     * @param compiler The compiler we will get attached to.
     **********************************************************************************************/
    ALIB_API    DateAndTime( Compiler& compiler );

    /** ********************************************************************************************
     * Virtual destructor
     **********************************************************************************************/
    virtual    ~DateAndTime()
    {}

    /** ********************************************************************************************
     * Static initialization function.
     * Called once during \alib{expressions,Expressions::init,library initialization}.
     **********************************************************************************************/
    static ALIB_API
    void        Init();
};


}}}} // namespace [aworx::lib::expressions::detail]



#endif // HPP_ALIB_EXPRESSIONS_PLUGINS_DATEANDTIME
