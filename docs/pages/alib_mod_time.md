// #################################################################################################
//  Documentation - ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################

/**
\page alib_mod_time   ALib Module Time - Programmer's Manual

\tableofcontents


 \I{################################################################################################}
# 1. Introduction # {#alib_time_intro}

This \alibmod provides types for calendrical date and time processing as well as
for non-calendrical steady and monotonic time measurement.

In essence, this module uses standard library types found in namespace \c std::chrono, and
encapsulates their functionality to provide some very common use-case of it in an easy to use and
better readable interface.

\note
  This does not impose criticism of the standard library. In contrast, it is very right for the
  standard library to be as formal as possible and its extraordinary quality in this respect is
  unrivalled by the library's of most other computer languages.<br>
  However it is right and just a consequence to have libraries like \alib that morph such a formal
  and explicit API into one of many possible concrete use cases, of course limiting the original
  functionality.


 \I{################################################################################################}
# 2. Points In Time # {#alib_time_points}
A human being imagines a "point in time" to be the combination of a calendar data and a clock time.
However, calendar dates and clock times rely on very complicated definitions, if
local time zones, daylight-savings, leap years, or even different calendar schemes are considered.<br>
Therefore, it is much less complex to define a "point in time" as the number of
time units passed since another point in time. Here, this other point in time is the single static
reference for all other values. This simple schematic allows very efficient arithmetic operations
and comparisons.
The only challenge is to convert such simple point in times to the complex, human-readable calendar
and clock values, in the case this is needed.

Class \alib{time;TimePointBase} provides an "abstract", templated base type for such time points
that store values that represent the difference in respect to a reference point in time.
The class provides inner type \alib{time::TimePointBase;Duration} which represents the difference
of two values of the type and offers an extensive set of methods and operators to work with
values of itself and that inner type.

\I{################################################################################################}
# 3. DateTime And Ticks # {#alib_time_types}

Two specializations of class \alib{time;TimePointBase} are provided with the module:

1. Class \alib{time;DateTime} for time points that are convertible to calendrical values, and
2. Class \alib{time;Ticks} for steady and monotonic time points.

The wording "steady and monotonic" means that probes (aka time measurements) taken are never influenced by
changes of the system's clock, daylight saving changes, etc. In contrast, it is always ensured that
the difference of two probes represents exactly the duration that passed between taking the probes.

Internally, type \alib{time;DateTime} uses <c>std::chrono::system_clock</c> while
type \alib{time;Ticks} uses <c>std::chrono::steady_clock</c>.

Type \alib{time;DateTime} provides methods to convert values to system dependent "date types".
For example, with GNU/Linux conversion to and from \c time_t is implemented. On Windows OS, in
addition conversion to \c FILETIME and \c SYSTEMTIME is offered.

\I{################################################################################################}
# 4. Conversion Of Ticks To DateTime Values# {#alib_time_ticks_to_dates}
So, why aren't there similar conversion methods to system dependent "date types" available with
class \alib{time;Ticks} that uses the internal monotonic timer?
The answer is that values of type \b %Ticks cannot be converted without ambiguities: Their
conversion can only be performed using the current system clock, which means that any changes of the
clock setting changes the result of the conversion! This is not the case with values of
type \alib{time;DateTime} (which, as explained in the previous section, instead takes ambiguities
between values of the type itself into account).

As it still might be suitable in special situations to convert \b Ticks to human-readable calendar
dates and clock times, class \alib{time;TickConverter} is provided.
This class allows converting values of type \b %Ticks to values of type \b %DateTime and vice versa,
which then can be converted further.
Of course, it is up to the software using this type, to ensure that these ambiguities are duly made
transparent to the end-user of a software: The end user has to know how converted monotonic clock
values are to be interpreted!

At the point of creation of an object of type \b %TickConverter, internally two "probe" values
of both timers, \b %Ticks and \b %DateTime are created. Using these probe values, the conversion
is performed. This means, that the conversion is "synchronized" with the system's clock setting
at point in time of creation of the converter. In other words, changes of the system clock
after the creation of the converter are not reflected. The converter can be updated to using the
current system clock setting by invoking \alib{time;TickConverter::SyncClocks}.


To provide an understanding for this whole issue, a sample may help: Module \alib_alox provides tools to
implement application logging. As it is important to have a steady monotonic relationship
between each log entry in respect to the moment of their occurrence, the module stores a
\b %Ticks value with each entry. In many cases, end-users are in fact just interested in two pieces
information concerning a log entry's creation time:
- The duration value relative to the start of the application
- The duration value relative to the previous log statement.

This means that the "absolute", calendrical point in time is not of interest in most cases.
Nevertheless, the module allows displaying also calendar dates and "time of day clock-times"
if wished. Now, still such display should rather show "steady monotonic" times when listed even
if the system clock was changed while a process's log file was written.

Therefore, no \b %DateTime values are stored with log entries, and if calendrical display
is needed, a conversion using class \b %TickConverter is performed. The object used for this
is created only once and in favor to provide steady log dates, is not updated by the module.
This means, if a software runs for a longer time, changes of the system clock are \b not
reflected and therefore, the calendrical date and time values displayed may start to deviate
from the true system clock!<br>
A software that is using the module has to decide to either actively
\alib{time;TickConverter::SyncClocks;update the converter} (and hence introduce non steady
log entries) or to acknowledge the user of the software of  the problem of interpreting the
date/time values! Now in this sample, a software that implements a server process, probably
running for days, weeks or even months without a reset, would synchronize the logging module's
time converter on a regular basis, e.g., once an hour, to ensure correct calendrical log dates.
In contrast to that, a software that performs a certain task and then exits would not update
the converter as break in the time flow would just be confusing. A user (reader) of the log file
has to be made aware that all calendar dates given, are to be interpreted "relative" to the
system's clock setting that the first log-entry showed.


\I{################################################################################################}
# 5. Further Support With Module BaseCamp # {#alib_time_system}

Two utility classes that are directly concerned to the types of this module have \b not been
implemented with this module but with module \alib_basecamp. The rationale for this is that these
utility classes make use extensive use of other functionality in that module and the goal was to
keep this \alib_time_nl a lean and very low level module.

The classes are:

- \alib{lang::system;CalendarDateTime}<br>
  Provides bidirectional conversions between values of class \b %DateTime and
  gregorian calendar dates and 24h clock times.<br>
  Furthermore, comprehensive features for conversion of values to human-readable strings
  (formatting) is implemented with this type.

- \alib{lang::system;CalendarDuration}
  Implements bidirectional conversions between duration values of classes \b %DateTime and
  \b Ticks on the one hand and human-readable durations (seconds, minutes, hours, days, etc.) on the
  other hand.<br>
  Furthermore, "intuitive" conversion of durations to human-readable strings (formatting) is
  implemented for this type.


\I{################################################################################################}
# 6. Utilities # {#alib_time_util}
To measure a set of durations and receive cumulated, average, minimum and maximum values of
all probes, class \alib{time;StopWatch} is provided.



Furthermore this namespace incorporates types used for conversion of "time points" and
durations to values understandable to humans, hence calendar dates and clock values in hours,
minutes, etc.<br>


<br><br><br><br><br><br> */
