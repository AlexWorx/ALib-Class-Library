//==================================================================================================
/// \file
/// This header-file is part of the \aliblong. It does not belong to an \alibmod and is
/// included in any \alibbuild.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib {  namespace time {

//==================================================================================================
/// This class provides an interface into the system's clock values. In contrast to values of sibling
/// class \alib{time;Ticks}, the underlying system timer is not guaranteed to be steady.
/// This means that an object created or set at a later point in time of the program execution,
/// might represent an earlier point in time.
/// This might happen when the system clock gets adjusted in-between two measurements.
///
/// Therefore, the class is to be used to represent calendar clock values which usually get
/// converted to human-readable formats (calendar dates and 24/60/60 clock times).
///
/// Apart from a few system-dependent conversion methods, the class has no specific interface, but
/// the methods and operators inherited from base \alib{time;TimePointBase}.
///
/// To construct an object of this type using calendrical date and time values, helper-class
/// \alib{strings::util;CalendarDateTime} with its constructor
/// \alib{strings::util::CalendarDateTime;CalendarDateTime(int,int,int,int,int,int,int)} is
/// available in case module \alib_strings is included in the \alibbuild.
/// A typical construction with this helper could look like this:
///
///      DateTime myDate= CalendarDateTime(2025,1,31,14,5).Get( Timezone::UTC );
///
/// @see
///   For this class, a \ref alibtools_debug_helpers_gdb "pretty printer" for the
///   GNU debugger is provided.
//==================================================================================================
class DateTime : public TimePointBase<std::chrono::system_clock, DateTime>
{
  public:

//! @cond NO_DOX
// doxygen bug: generates "return type not documented"
        /// Use constructors of parent class.
        using  TimePointBase::TimePointBase;
//! @endcond

  //################################################################################################
  // Conversion to time platform/language specific values
  //################################################################################################

    /// Converts the internal value into seconds since January 1, 1970, 00:00:00 GMT.
    /// The conversion is dependent on time zone and system clock setting of the host.
    ///
    /// @return Seconds in the epoch.
    time_t      InEpochSeconds()     const { return std::chrono::system_clock::to_time_t( stamp ); }


    /// Static method that creates a \b %DateTime object representing the given system
    /// point in time measured in seconds since January 1st 1970, 00:00:00 GMT.
    ///
    /// @param epochSeconds The milliseconds in the epoch to convert.
    /// @return A time stamp object
    static
    DateTime    FromEpochSeconds ( time_t epochSeconds )
    { return DateTime( std::chrono::system_clock::from_time_t( epochSeconds ) ); }


        #if defined (_WIN32)
            //======================================================================================
            /// Converts the internal value into windows specific file time, a 64-bit value that
            /// represents the number of 100 nanosecond intervals that have elapsed since
            /// 12:00 A.M. January 1, 1601 UTC. The conversion is dependent on time zone and system
            /// clock setting of the host.
            ///
            /// \note
            ///    Microsoft Windows specific.
            ///
            /// @return The Windows OS file time value represented by this object.
            //======================================================================================
            ALIB_DLL  FILETIME          ToFileTime()                                          const;

            //======================================================================================
            /// Converts the internal value into windows specific file time, a 64-bit value that
            /// represents the number of 100 nanosecond intervals that have elapsed since
            /// 12:00 A.M. January 1, 1601 UTC. The conversion is dependent on time zone and system
            /// clock setting of the host.
            ///
            /// \note
            ///    Microsoft Windows specific.
            ///
            /// @return The Windows OS file time represented by this object as type
            ///         struct \b %ULARGE_INTEGER.
            //======================================================================================
            ALIB_DLL  ULARGE_INTEGER    ToFileTimeLI()                                        const;

            //======================================================================================
            /// Static method that creates a \b %DateTime object representing the given "file time".
            /// File time is a 64-bit value that represents the number of 100 nanosecond intervals
            /// that have elapsed since 12:00 A.M. January 1, 1601 UTC.
            ///
            /// \note
            ///    Microsoft Windows specific.
            ///
            /// @param fileTime The file time to use.
            /// @return A time stamp object
            //======================================================================================
            ALIB_DLL  static DateTime  FromFileTime( const FILETIME& fileTime );

            //======================================================================================
            /// Static method that creates a \b %DateTime object representing the given "file time".
            /// File time is a 64-bit value that represents the number of 100 nanosecond intervals
            /// that have elapsed since 12:00 A.M. January 1, 1601 UTC.
            ///
            /// \note
            ///    Microsoft Windows specific.
            ///
            /// @param fileTime The file time to use.
            /// @return A time stamp object
            //======================================================================================
            ALIB_DLL  static DateTime FromFileTime( const ULARGE_INTEGER& fileTime );

            //======================================================================================
            /// Converts the internal value into windows specific system time struct.
            ///
            /// \note
            ///    Microsoft Windows specific.
            ///
            /// @param timezone Denote if the time that is returned should be local or UTC.
            ///                 Defaults to \c TimeZone::Local.
            /// @return The time point as \b %SYSTEMTIME.
            //======================================================================================
            ALIB_DLL  SYSTEMTIME ToSystemTime( lang::Timezone timezone = lang::Timezone::Local ) const;

            //======================================================================================
            /// Static method that creates a \b %DateTime object representing the given as windows
            /// system time.
            ///
            /// \note
            ///    Microsoft Windows specific.
            ///
            /// @param systemTime Pointer to a SYSTEMTIME struct that holds the system time to use.
            /// @param timezone Denote if the time is interpreted as local or UTC.
            ///                 Defaults to \c TimeZone::Local.
            /// @return Seconds in the epoch.
            //======================================================================================
            ALIB_DLL static DateTime FromSystemTime( const SYSTEMTIME& systemTime,
                                                     lang::Timezone timezone= lang::Timezone::Local );
        #endif


    /// Equal to operator.
    /// @param other The time stamp to compare.
    /// @return The result of the comparison.
    bool   operator==( const DateTime& other )                const { return stamp == other.stamp; }


    /// Not equal to operator.
    /// @param other The time stamp to compare.
    /// @return The result of the comparison.
    bool   operator!=( const DateTime& other )                const { return stamp != other.stamp; }

    /// Less than operator.
    /// @param other The time stamp to compare.
    /// @return A reference to this object.
    bool   operator<( const DateTime& other )                 const { return stamp <  other.stamp; }

    /// Less than or equal to operator.
    /// @param other The time stamp to compare.
    /// @return The result of the comparison.
    bool   operator<=( const DateTime& other )               const { return stamp <=  other.stamp; }

    /// Greater than operator.
    /// @param other The time stamp to compare.
    /// @return The result of the comparison.
    bool   operator>( const DateTime& other )                 const { return stamp >  other.stamp; }

    /// Greater than or equal to operator.
    /// @param other The time stamp to compare.
    /// @return The result of the comparison.
    bool   operator>=( const DateTime& other )               const { return stamp >=  other.stamp; }
};

} // namespace time

/// Type alias in namespace \b alib.
using     DateTime=    time::DateTime;

} // namespace [alib]
