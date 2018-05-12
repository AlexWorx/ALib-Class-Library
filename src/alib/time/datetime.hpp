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
#if defined(HPP_COM_ALIB_TEST_INCLUDES) && defined(HPP_ALIB_TIME_DATETIME)
    #error "Header already included"
#endif

// then, set include guard
#ifndef HPP_ALIB_TIME_DATETIME
//! @cond NO_DOX
#define HPP_ALIB_TIME_DATETIME 1
//! @endcond

#if !defined (HPP_ALIB_TIME_TIMEPOINT)
#   include "alib/time/timepointbase.hpp"
#endif


namespace aworx { namespace lib { namespace time {

/** ************************************************************************************************
 * This class provides an interface into the system's clock values. In contrast to values of sibling
 * class \alib{time,DateTime}, the underlying system timer is not guaranteed to be steady.
 * This means that an object created or set at a later point in time of the program execution,
 * might represent an earlier point in time.
 * This might happen when the system clock gets adjusted in-between two measurements.
 *
 * Therefore, the class is to be used to represent calendar clock values which usually get
 * converted to human readable formats (calendar dates and 24/60/60 clock times).
 *
 * To construct an object of this type using calendrical date an time values, helper class
 * \b %CalendarDateTime with its constructor
 * \alib{time,CalendarDateTime::CalendarDateTime(int,int,int,int,int,int,int)} is available.
 * A typical construction with this helper could look like this:
 *
 *      DateTime myDate= CalendarDateTime(2018,1,31,14,5).Get( Timezone::UTC );
 **************************************************************************************************/
class DateTime : public TimePointBase<std::chrono::system_clock, DateTime>
{
    public:

//! @cond NO_DOX
// doxygen bug: generates "return type not documented"
        /// Use constructors of parent class.
        using  TimePointBase::TimePointBase;
//! @endcond

    // #############################################################################################
    // Conversion to time platform/language specific values
    // #############################################################################################

        /** ****************************************************************************************
         * Converts the internal value into seconds since January 1, 1970, 00:00:00 GMT.
         * The conversion is dependent on time zone and system clock setting of the host.
         *
         * @return Seconds in the epoch.
         ******************************************************************************************/
        inline
        time_t      InEpochSeconds()    const
        {
            return std::chrono::system_clock::to_time_t( stamp );
        }


        /** ****************************************************************************************
         * Static method that creates a \b %DateTime object representing the given system
         * point in time measured in seconds since January 1st 1970, 00:00:00 GMT.
         *
         * @param epochSeconds The milliseconds in the epoch to convert.
         * @return A time stamp object
         ******************************************************************************************/
        inline  static
        DateTime    FromEpochSeconds ( time_t epochSeconds )
        {
            return DateTime( std::chrono::system_clock::from_time_t( epochSeconds ) );
        }


        #if defined (_WIN32)
            /** ************************************************************************************
             * Converts the internal value into windows specific file time, a 64-bit value that
             * represents the number of 100 nanosecond intervals that have elapsed since
             * 12:00 A.M. January 1, 1601 UTC. The conversion is dependent on time zone and system
             * clock setting of the host.
             *
             * \note
             *    Microsoft Windows specific.
             *
             * @return The Windows OS file time value represented by this object.
             **************************************************************************************/
            ALIB_API  FILETIME          ToFileTime()                                          const;

            /** ************************************************************************************
             * Converts the internal value into windows specific file time, a 64-bit value that
             * represents the number of 100 nanosecond intervals that have elapsed since
             * 12:00 A.M. January 1, 1601 UTC. The conversion is dependent on time zone and system
             * clock setting of the host.
             *
             * \note
             *    Microsoft Windows specific.
             *
             * @return The Windows OS file time represented by this object as type
             *         struct \b %ULARGE_INTEGER.
             **************************************************************************************/
            ALIB_API  ULARGE_INTEGER    ToFileTimeLI()                                        const;

            /** ************************************************************************************
             * Static method that creates a \b %DateTime object representing the given "file time".
             * File time is a 64-bit value that represents the number of 100 nanosecond intervals
             * that have elapsed since 12:00 A.M. January 1, 1601 UTC.
             *
             * \note
             *    Microsoft Windows specific.
             *
             * @param fileTime The file time to use.
             * @return A time stamp object
             **************************************************************************************/
            ALIB_API  static DateTime  FromFileTime( const FILETIME& fileTime );

            /** ************************************************************************************
             * Static method that creates a \b %DateTime object representing the given "file time".
             * File time is a 64-bit value that represents the number of 100 nanosecond intervals
             * that have elapsed since 12:00 A.M. January 1, 1601 UTC.
             *
             * \note
             *    Microsoft Windows specific.
             *
             * @param fileTime The file time to use.
             * @return A time stamp object
             **************************************************************************************/
            ALIB_API  static DateTime FromFileTime( const ULARGE_INTEGER& fileTime );

            /** ************************************************************************************
             * Converts the internal value into windows specific system time struct.
             *
             * \note
             *    Microsoft Windows specific.
             *
             * @param timezone Denote if the time that is returned should be local or UTC.
             *                 Defaults to \c TimeZone::Local.
             * @return The time point as \b %SYSTEMTIME.
             **************************************************************************************/
            ALIB_API  SYSTEMTIME ToSystemTime( lang::Timezone timezone =lang::Timezone::Local ) const;

            /** ************************************************************************************
             * Static method that creates a \b %DateTime object representing the given as windows
             * system time.
             *
             * \note
             *    Microsoft Windows specific.
             *
             * @param systemTime Pointer to a SYSTEMTIME struct that holds the system time to use.
             * @param timezone Denote if the time is interpreted as local or UTC.
             *                 Defaults to \c TimeZone::Local.
             * @return Seconds in the epoch.
             **************************************************************************************/
            ALIB_API static DateTime FromSystemTime( const SYSTEMTIME& systemTime,
                                                   lang::Timezone timezone= lang::Timezone::Local );
        #endif

};



}} // namespace lib::time

/// Type alias in namespace #aworx.
using     DateTime=                aworx::lib::time::DateTime;

}  // namespace aworx



//! @cond NO_DOX
#if ALIB_MODULE_STRINGS
    ALIB_STRINGS_SPECIALIZE_T_APPLY_N( aworx::lib::time::DateTime::Duration )
    ALIB_STRINGS_SPECIALIZE_T_APPLY_W( aworx::lib::time::DateTime::Duration )
#endif
//! @endcond


#if ALIB_MODULE_BOXING

//! @cond NO_DOX
namespace aworx { namespace lib { namespace boxing  {


ALIB_BOXING_SPECIALIZE_CB_CUB( aworx::lib::time::DateTime , aworx::lib::time::DateTime , false, true );
inline  void  T_Boxing<aworx::lib::time::DateTime>::Boxing(Box& box, const aworx::lib::time::DateTime& o)
{
    box.data.Value= static_cast<boxvalue>( o.Raw() );
    box.data.Length= 0;
}

inline  aworx::lib::time::DateTime T_Boxing<aworx::lib::time::DateTime>::Unboxing( const Box& box )
{
    return DateTime::FromRaw( static_cast<DateTime::TRaw>(box.data.Value) );
}

ALIB_BOXING_SPECIALIZE_CB_CUB( aworx::lib::time::DateTime::Duration , aworx::lib::time::DateTime::Duration , false, true );
inline  void  T_Boxing<aworx::lib::time::DateTime::Duration>::Boxing(Box& box, const aworx::lib::time::DateTime::Duration& o)
{
    box.data.Value= static_cast<boxvalue>( o.Export().count() );
    box.data.Length= 0;
}

inline  aworx::lib::time::DateTime::Duration T_Boxing<aworx::lib::time::DateTime::Duration>::Unboxing( const Box& box )
{
    return aworx::lib::time::DateTime::Duration::Import(
             aworx::lib::time::DateTime::Duration::TDuration( box.data.Value  ) );
}

}}}  // namespace [aworx::lib::boxing]
//! @endcond

namespace aworx { namespace lib { namespace time {

/**
 * Implementation of \ref aworx::lib::strings::boxing::IFormat "IFormat" for boxable type
 * <c>DateTime</c>.<br>
 * Writes the content of \p{box} (which is of type \b %DateTime) to the given \b %AString
 * object \p{target} using a local instance of class \b %CalendarDateTime and its method
 * \ref aworx::lib::time::CalendarDateTime::Format "CalendarDateTime::Format".
 */
class IFormat_DateTime: public strings::boxing::IFormat<character>, public Singleton<IFormat_DateTime>
{
    /**
     * Interface implementation.
     * @param box        The box that the interface was invoked on.
     * @param formatSpec The specification of the format (type specific)
     * @param target     The AString object receiving the formatted string.
     */
    ALIB_API
    virtual void Invoke( const Box& box, const String& formatSpec, AString& target )       override;
};

}}} // namespace [aworx::lib::time]
#endif //ALIB_MODULE_BOXING



#endif // HPP_ALIB_TIME_DATETIME
