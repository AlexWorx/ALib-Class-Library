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
/// This class provides an interface into system dependent steady, monotonic timer values, which are
/// independent of the system's clock settings, hence independent of changes of the clock
/// by the user, by external clock synchronization using the internet, GPS, mobile network, etc.
///
/// Usually, this class is used to measure and store durations (e.g., with inherited method
/// \alib{time::TimePointBase;Age}).
/// As tick values are not just system dependent, but even usually change with a reboot of a system,
/// they should not be stored and loaded in data files or otherwise shared between systems.
///
/// In contrast to sibling type \alib{time;DateTime}, this class is also missing conversion methods
/// with types that represent "human" calendar dates and a clock times.
/// However, in some situations it still may be needed to convert the points in time measured with
/// this class to human-readable date and time representations.
/// For this purpose, class \alib{time;TickConverter} allows converting objects of this type to
/// objects of sibling type \b %DateTime, which then can be further converted to system-dependent
/// calendar dates and clock times.
///
/// The class has no specific interface, but the methods and operators inherited from base
/// \alib{time;TimePointBase}.
/// @see
///   For this class, a \ref alibtools_debug_helpers_gdb "pretty printer" for the
///   GNU debugger is provided.
//==================================================================================================
class Ticks : public TimePointBase<std::chrono::steady_clock, Ticks>
{
    public:
//! @cond NO_DOX
// doxygen bug: generates "return type not documented"
        /// Use constructors of parent class.
        using  TimePointBase::TimePointBase;
//! @endcond


        //==========================================================================================
        /// Equal to operator.
        /// @param other The time stamp to compare.
        /// @return The result of the comparison.
        //==========================================================================================
        bool   operator==( const Ticks& other )                                                const
        {
            return stamp == other.stamp;
        }


        //==========================================================================================
        /// Not equal to operator.
        /// @param other The time stamp to compare.
        /// @return The result of the comparison.
        //==========================================================================================
        bool   operator!=( const Ticks& other )                                                const
        {
            return stamp != other.stamp;
        }

        //==========================================================================================
        /// Less than operator.
        /// @param other The time stamp to compare.
        /// @return A reference to this object.
        //==========================================================================================
        bool   operator<( const Ticks& other )                                                 const
        {
            return stamp <  other.stamp;
        }

        //==========================================================================================
        /// Less than or equal to operator.
        /// @param other The time stamp to compare.
        /// @return The result of the comparison.
        //==========================================================================================
        bool   operator<=( const Ticks& other )                                                const
        {
            return stamp <=  other.stamp;
        }

        //==========================================================================================
        /// Greater than operator.
        /// @param other The time stamp to compare.
        /// @return The result of the comparison.
        //==========================================================================================
        bool   operator>( const Ticks& other )                                                 const
        {
            return stamp >  other.stamp;
        }

        //==========================================================================================
        /// Greater than or equal to operator.
        /// @param other The time stamp to compare.
        /// @return The result of the comparison.
        //==========================================================================================
        bool   operator>=( const Ticks& other )                                                const
        {
            return stamp >=  other.stamp;
        }
};

} // namespace alib[::time]

/// Type alias in namespace \b alib.
using     Ticks   =    time::Ticks;

}  // namespace [alib]


