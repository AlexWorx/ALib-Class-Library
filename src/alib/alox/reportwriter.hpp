// #################################################################################################
//  aworx::lib::lox - ALox Logging Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#ifndef HPP_ALIB_ALOX_REPORT_WRITER
#define HPP_ALIB_ALOX_REPORT_WRITER 1

#if !defined (HPP_ALIB_ALOX)
#   include "alib/alox/alox.hpp"
#endif

namespace aworx { namespace lib { namespace lox {

/** ************************************************************************************************
 * The \b %ReportWriter for \alib when using \alox. An instance of this class is created
 * in method \ref aworx::lib::lox::Log::AddDebugLogger "Log::AddDebugLogger"
 * and registered with \alib.<br>
 * Uses internal domain <c>'$/REPORT'</c> for logging.
 **************************************************************************************************/
class    ALoxReportWriter : public aworx::lib::results::ReportWriter
{
    protected:
        /** The \b Lox to report to. */
        Lox*                lox;

        /** The internal domain to report into. */
        static NString16    reportDomain;

    public:
    /** ********************************************************************************************
     * Constructs an \b %AloxReportWriter.
     * @param lox    The \b Lox to report to.
     **********************************************************************************************/
     ALIB_API
     ALoxReportWriter ( Lox* lox );

    /** ****************************************************************************************
     * Virtual destructor
     ******************************************************************************************/
    virtual ~ALoxReportWriter()              {}

    /** ********************************************************************************************
     * Notify activation/deactivation
     **********************************************************************************************/
    virtual void NotifyActivation  ( Phase ) { }


    /** ********************************************************************************************
     * Write \alib reports using \alox.
     * @param msg The message to log.
     **********************************************************************************************/
     virtual void Report  ( const lib::results::Message& msg );

    /** ********************************************************************************************
     * Returns the domain used to write reports.
     * @return The report log domain.
     **********************************************************************************************/
     ALIB_API
     static NString& LogDomain();
};// class ALoxReportWriter

}} // namespace aworx[::lib::lox]

/// Type alias in namespace #aworx.
using   ALoxReportWriter= aworx::lib::lox::ALoxReportWriter;

}  // namespace [aworx]

#endif // HPP_ALIB_ALOX_REPORT_WRITER
