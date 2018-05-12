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
#if defined(HPP_COM_ALIB_TEST_INCLUDES) && defined(HPP_ALIB_TIME_TICKS)
    #error "Header already included"
#endif

// then, set include guard
#ifndef HPP_ALIB_TIME_TICKS
//! @cond NO_DOX
#define HPP_ALIB_TIME_TICKS 1
//! @endcond

#if !defined (HPP_ALIB_TIME_TIMEPOINT)
#   include "alib/time/timepointbase.hpp"
#endif


namespace aworx { namespace lib { namespace time {


/** ************************************************************************************************
 * This class provides an interface into system dependent steady, monotonic timer values, which are
 * independent from the system's clock settings.
 *
 * Usually, this class is used to measure and store durations (e.g. with inherited method
 * \alib{time::TimePointBase,Age}). The class is \e not suitable to process date and time values.
 * For this, sibling class \alib{time,DateTime} is available. However, in some situations it still
 * may be needed to convert points in time created with this class to human readable date and
 * time representations. For this, class \alib{time,TicksConverter} allows to convert objects of
 * this type to objects of sibling type \b %DateTime.
 *
 * As tick values are system dependent, they should not be stored and loaded in data files or
 * otherwise shared between systems. Even on the same machine, the values may become
 * incompatible (e.g. after a reboot).
 **************************************************************************************************/
class Ticks : public TimePointBase<std::chrono::steady_clock, Ticks>
{
    public:
//! @cond NO_DOX
// doxygen bug: generates "return type not documented"
        /// Use constructors of parent class.
        using  TimePointBase::TimePointBase;
//! @endcond

};


}} // namespace lib::time

/// Type alias in namespace #aworx.
using     Ticks   =                aworx::lib::time::Ticks;

}  // namespace aworx


//! @cond NO_DOX


#if ALIB_MODULE_STRINGS
    ALIB_STRINGS_SPECIALIZE_T_APPLY_N( aworx::lib::time::Ticks::Duration )
    ALIB_STRINGS_SPECIALIZE_T_APPLY_W( aworx::lib::time::Ticks::Duration )
#endif


#if ALIB_MODULE_BOXING

namespace aworx { namespace lib { namespace boxing  {

ALIB_BOXING_SPECIALIZE_CB_CUB( aworx::lib::time::Ticks , aworx::lib::time::Ticks , false, true );
inline  void  T_Boxing<aworx::lib::time::Ticks>::Boxing(Box& box, const aworx::lib::time::Ticks& o)
{
    box.data.Value= static_cast<boxvalue>( o.Raw() );
    box.data.Length= 0;
}

inline  aworx::lib::time::Ticks T_Boxing<aworx::lib::time::Ticks>::Unboxing( const Box& box )
{
    return Ticks::FromRaw( static_cast<Ticks::TRaw>(box.data.Value) );
}

ALIB_BOXING_SPECIALIZE_CB_CUB( aworx::lib::time::Ticks::Duration , aworx::lib::time::Ticks::Duration , false, true );
inline  void  T_Boxing<aworx::lib::time::Ticks::Duration>::Boxing(Box& box, const aworx::lib::time::Ticks::Duration& o)
{
    box.data.Value= static_cast<boxvalue>( o.Export().count() );
    box.data.Length= 0;
}

inline  aworx::lib::time::Ticks::Duration T_Boxing<aworx::lib::time::Ticks::Duration>::Unboxing( const Box& box )
{
    return aworx::lib::time::Ticks::Duration::Import(
             aworx::lib::time::Ticks::Duration::TDuration( box.data.Value  ) );
}

}}}  // namespace [aworx::lib::boxing]


#endif //ALIB_MODULE_BOXING

//! @endcond


#endif // HPP_ALIB_TIME_TICKS
