//==================================================================================================
/// \file
/// This header-file is part of module \alib_expressions of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib {  namespace expressions { namespace plugins {

//==================================================================================================
/// %Compiler plug-in that provides date an time related expression terms.
///
/// The identifiers, functions and operators offered by this compiler plug-in introduce \alib
/// types \alib{time;DateTime} and \alib{time;TimePointBase::Duration} to be used with
/// module \alib_expressions_nl.
///
/// All identifier and function names are defined case-insensitive.
///
/// <b>Constants:</b><br>
///
/// Type    | Name  |Min. Abbreviation| Description
/// --------|-------|-----------------|-------------
/// Integer |\b January  | jan | Used to compare the result of functions \p{GetMonth} and \p{GetUtcMonth}.
/// Integer |\b February | feb | Used to compare the result of functions \p{GetMonth} and \p{GetUtcMonth}.
/// Integer |\b March    | mar | Used to compare the result of functions \p{GetMonth} and \p{GetUtcMonth}.
/// Integer |\b April    | apr | Used to compare the result of functions \p{GetMonth} and \p{GetUtcMonth}.
/// Integer |\b May      | may | Used to compare the result of functions \p{GetMonth} and \p{GetUtcMonth}.
/// Integer |\b June     | jun | Used to compare the result of functions \p{GetMonth} and \p{GetUtcMonth}.
/// Integer |\b July     | jul | Used to compare the result of functions \p{GetMonth} and \p{GetUtcMonth}.
/// Integer |\b August   | aug | Used to compare the result of functions \p{GetMonth} and \p{GetUtcMonth}.
/// Integer |\b September| sep | Used to compare the result of functions \p{GetMonth} and \p{GetUtcMonth}.
/// Integer |\b October  | oct | Used to compare the result of functions \p{GetMonth} and \p{GetUtcMonth}.
/// Integer |\b November | nov | Used to compare the result of functions \p{GetMonth} and \p{GetUtcMonth}.
/// Integer |\b December | dec | Used to compare the result of functions \p{GetMonth} and \p{GetUtcMonth}.
///
/// Integer |\b Sunday   | sun | Used to compare the result of functions \p{GetDayOfWeek} and \p{GetUtcDayOfWeek}.
/// Integer |\b Monday   | mon | Used to compare the result of functions \p{GetDayOfWeek} and \p{GetUtcDayOfWeek}.
/// Integer |\b Tuesday  | tue | Used to compare the result of functions \p{GetDayOfWeek} and \p{GetUtcDayOfWeek}.
/// Integer |\b Wednesday| wed | Used to compare the result of functions \p{GetDayOfWeek} and \p{GetUtcDayOfWeek}.
/// Integer |\b Thursday | thu | Used to compare the result of functions \p{GetDayOfWeek} and \p{GetUtcDayOfWeek}.
/// Integer |\b Friday   | fri | Used to compare the result of functions \p{GetDayOfWeek} and \p{GetUtcDayOfWeek}.
/// Integer |\b Saturday | sat | Used to compare the result of functions \p{GetDayOfWeek} and \p{GetUtcDayOfWeek}.
///
///
/// <br>
/// <b>Constant Constructor Functions:</b><br>
///
/// Return Type    | Name      |Min. Abbreviation| Signature | Description |
/// ---------------|-----------|-----------------|-----------|-------------|
/// DateTime |\b DateTime      | times  |int,...| Returns a time stamp representing the calendar date in local time zone, respecting daylight saving. The first parameter is required and provides the \p{year}. Further parameters are optional and provide \p{month} (defaults to \c 1), \p{dayOfMonth} (defaults to \c 1), \p{hour} (defaults to \c 0), \p{minute} (defaults to \c 0) and \p{millisecond} (defaults to \c 0).
/// DateTime |\b UtcDateTime   | utcti  |int,...| Returns a time stamp representing the calendar date in UTC time, not respecting daylight saving. The first parameter is required and provides the \p{year}. Further parameters are optional and provide \p{month} (defaults to \c 1), \p{dayOfMonth} (defaults to \c 1), \p{hour} (defaults to \c 0), \p{minute} (defaults to \c 0) and \p{millisecond} (defaults to \c 0).
/// Duration |\b NanoSecondS   | ns     |int    | Returns a time span object representing the given number of nanoseconds.
/// Duration |\b MicroSecondS  | mics   |int    | Returns a time span object representing the given (absolute) number of microseconds.
/// Duration |\b MicroSecondS  | mics   |double | Returns a time span object representing the given (rational) number of microseconds.
/// Duration |\b MillisecondS  | ms     |int    | Returns a time span object representing the given (absolute) number of milliseconds.
/// Duration |\b MillisecondS  | ms     |double | Returns a time span object representing the given (rational) number of milliseconds.
/// Duration |\b SecondS       | sec    |int    | Returns a time span object representing the given (absolute) number of seconds.
/// Duration |\b SecondS       | sec    |double | Returns a time span object representing the given (rational) number of seconds.
/// Duration |\b MinuteS       | min    |int    | Returns a time span object representing the given (absolute) number of minutes.
/// Duration |\b MinuteS       | min    |double | Returns a time span object representing the given (rational) number of minutes.
/// Duration |\b Hours         | hour   |int    | Returns a time span object representing the given (absolute) number of hours.
/// Duration |\b Hours         | hour   |double | Returns a time span object representing the given (rational) number of hours.
/// Duration |\b Days          | day    |int    | Returns a time span object representing the given (absolute) number of days.
/// Duration |\b Days          | day    |double | Returns a time span object representing the given (rational) number of days.
/// Duration |\b Weeks         | wee    |int    | Returns a time span object representing the given (absolute) number of weeks.
/// Duration |\b Weeks         | wee    |double | Returns a time span object representing the given (rational) number of weeks.
/// Duration |\b Months        | mon    |int    | Returns a time span object representing the given (absolute) number of months (30 days).
/// Duration |\b Months        | mon    |double | Returns a time span object representing the given (rational) number of months (30 days).
/// Duration |\b Years         | year   |int    | Returns a time span object representing the given (absolute) number of years (365 days).
/// Duration |\b Years         | year   |double | Returns a time span object representing the given (rational) number of years (365 days).
///
/// <br>
/// <b>Functions:</b><br>
///
/// Return Type    | Name      | Min. Abbreviation| Signature | Description |
/// ---------------|-----------|------------------|-----------|-------------|
/// DateTime  | \b Now              |now           | ./.       | Returns the actual point in time.
/// DateTime  | \b Today            |today         | ./.       | Returns the point in time that represents the current date local date at midnight, respecting the local time zone and daylight saving.
/// DateTime  | \b UtcToday         |utoday        | ./.       | Returns the point in time that represents the current date local date at midnight, respecting UTC time zone without daylight saving.
/// Duration  | \b Age              |age           | DateTime  | Returns the time span between the actual point in time and the given time stamp.
/// Boolean   | \b IsOlderThan      |isot          | DateTime ,Duration| Returns \c true if the time span between the actual point in time and the given point in time is greater than the given duration.
/// Integer   | \b GetYear          |gy            | DateTime  | Returns the calendar year of the time stamp, respecting the local time zone and daylight saving.
/// Integer   | \b GetMonth         |gmon          | DateTime  | Returns the calendar month of the time stamp, respecting the local time zone and daylight saving.
/// Integer   | \b GetDay           |gd            | DateTime  | Returns the calendar day of the time stamp, respecting the local time zone and daylight saving.
/// Integer   | \b GetDayOfWeek     |gdow          | DateTime  | Returns the calendar dayOfWeek of the time stamp, respecting the local time zone and daylight saving.
/// Integer   | \b GetHour          |gh            | DateTime  | Returns the calendar hour of the time stamp, respecting the local time zone and daylight saving.
/// Integer   | \b GetMinute        |gmin          | DateTime  | Returns the calendar Minute of the time stamp, respecting the local time zone and daylight saving.
/// Integer   | \b GetMilliSecond   |gms           | DateTime  | Returns the calendar millisecond of the time stamp, respecting the local time zone and daylight saving.
/// Integer   | \b GetUtcYear       |guy           | DateTime  | Returns the calendar year of the time stamp, respecting UTC time zone without daylight saving.
/// Integer   | \b GetUtcMonth      |gumon         | DateTime  | Returns the calendar month of the time stamp, respecting UTC time zone without daylight saving.
/// Integer   | \b GetUtcDay        |gud           | DateTime  | Returns the calendar day of the time stamp, respecting UTC time zone without daylight saving.
/// Integer   | \b GetUtcDayOfWeek  |gudow         | DateTime  | Returns the calendar dayOfWeek of the time stamp, respecting UTC time zone without daylight saving.
/// Integer   | \b GetUtcHour       |guh           | DateTime  | Returns the calendar hour of the time stamp, respecting UTC time zone without daylight saving.
/// Integer   | \b GetUtcMinute     |gumin         | DateTime  | Returns the calendar minute of the time stamp, respecting UTC time zone without daylight saving.
/// Integer   | \b GetUtcMillisecond|gums          | DateTime  | Returns the calendar millisecond of the time stamp, respecting UTC time zone without daylight saving.
/// Float     | \b InDayS           |ind           | Duration  | Returns the time span given in days.
/// Float     | \b InHourS          |inh           | Duration  | Returns the time span given in hours.
/// Float     | \b InMinuteS        |inmin         | Duration  | Returns the time span given in minutes.
/// Float     | \b InSecondS        |ins           | Duration  | Returns the time span given in seconds.
/// Float     | \b InMilliSecondS   |inms          | Duration  | Returns the time span given in milliseconds.
/// Float     | \b InMicroSecondS   |inmics        | Duration  | Returns the time span given in microseconds.
/// Float     | \b InNanoSecondS    |inns          | Duration  | Returns the time span given in nanoseconds.
/// Float     | \b InHertZ          |inhz          | Duration  | Returns the time span given in hertz (1/second).
///
/// <br>
/// <b>Binary Operators:</b><br>
///
/// Return Type    | Lhs Type | Operator | Rhs Type | Description|
/// ---------------|----------|----------|----------|------------|
/// DateTime | DateTime |<c>+</c> | Duration | Adds a time span to a time stamp.
/// DateTime | Duration |<c>+</c> | DateTime | Adds a time span to a time stamp.
/// DateTime | DateTime |<c>-</c> | DateTime | Subtracts two time stamps resulting in a time span.
/// DateTime | DateTime |<c>-</c> | Duration | Subtracts a time span from time stamp.
/// Duration | Duration |<c>+</c> | Duration | Adds two time spans.
/// Duration | Duration |<c>-</c> | Duration | Subtracts rhs-value from lhs-value.
/// Duration | Duration |<c>*</c> | Float    | Multiplies a time span by a given floating point value.
/// Duration | Float    |<c>*</c> | Duration | Multiplies a time span by a given floating point value.
/// Duration | Duration |<c>*</c> | Integer  | Multiplies a time span by a given integral value.
/// Duration | Integer  |<c>*</c> | Duration | Multiplies a time span by a given integral value.
/// Duration | Duration |<c>/</c> | Float    | Divides a time span by a given floating point value.
/// Duration | Duration |<c>/</c> | Integer  | Divides a time span by a given integral value.
/// Boolean  | DateTime |<c>==</c>| DateTime | Compares two time stamp objects.
/// Boolean  | Duration |<c>==</c>| Duration | Compares two time span objects.
/// Boolean  | DateTime |<c>!=</c>| DateTime | Compares two time stamp objects.
/// Boolean  | Duration |<c>!=</c>| Duration | Compares two time span objects.
/// Boolean  | DateTime |<c><</c> | DateTime | Compares two time stamp objects.
/// Boolean  | Duration |<c><</c> | Duration | Compares two time span objects.
/// Boolean  | DateTime |<c><=</c>| DateTime | Compares two time stamp objects.
/// Boolean  | Duration |<c><=</c>| Duration | Compares two time span objects.
/// Boolean  | DateTime |<c>></c> | DateTime | Compares two time stamp objects.
/// Boolean  | Duration |<c>></c> | Duration | Compares two time span objects.
/// Boolean  | DateTime |<c>>=</c>| DateTime | Compares two time stamp objects.
/// Boolean  | Duration |<c>>=</c>| Duration | Compares two time span objects.
///
/// # Reference Documentation #
//==================================================================================================
struct DateAndTime   : public plugins::Calculus
{
    //==============================================================================================
    /// Constructor. Creates the hash maps.
    /// @param compiler The compiler we will get attached to.
    //==============================================================================================
    ALIB_DLL    DateAndTime( Compiler& compiler );

    //==============================================================================================
    /// Virtual destructor
    //==============================================================================================
    virtual    ~DateAndTime()                                                           override  {}

    //==============================================================================================
    /// Static initialization function.
    /// Called once during \ref alib_mod_bs "library initialization".
    //==============================================================================================
    static ALIB_DLL
    void        Bootstrap();
};

}}} // namespace [alib::expressions::detail]

