//==================================================================================================
/// \file
/// This header file is part of module \alib_alox of the \aliblong.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================

#ifndef HPP_ALIB_ALOX_REPORT_WRITER
#define HPP_ALIB_ALOX_REPORT_WRITER 1
#pragma once
#include "alib/lang/message/report.hpp"
namespace alib {  namespace lox {

class Lox;

//==================================================================================================
/// The \b %ReportWriter for \alib when using \alox. An instance of this class is created
/// in method \ref alib::lox::Log::AddDebugLogger "Log::AddDebugLogger"
/// and registered with \alib.<br>
/// Uses internal domain <c>'$/REPORT'</c> for logging.
//==================================================================================================
class    ALoxReportWriter : public alib::lang::ReportWriter
{
    protected:
        /// The \b Lox to report to.
        Lox*                lox;

        /// The internal domain to report into.
        static NString16    reportDomain;

    public:
    //==============================================================================================
    /// Constructs an \b %AloxReportWriter.
    /// @param lox    The \b Lox to report to.
    //==============================================================================================
     ALIB_API
     ALoxReportWriter ( Lox* lox );

    //==============================================================================================
    /// Virtual destructor
    //==============================================================================================
    virtual ~ALoxReportWriter()                                                         override  {}

    //==============================================================================================
    /// Notify activation/deactivation
    //==============================================================================================
    virtual void NotifyActivation  ( lang::Phase )                                      override  {}


    //==============================================================================================
    /// Write \alib reports using \alox.
    /// @param msg The message to log.
    //==============================================================================================
     virtual void Report  ( lang::Message& msg )                                   override;

    //==============================================================================================
    /// Returns the domain used to write reports.
    /// @return The report log domain.
    //==============================================================================================
     ALIB_API
     static NString& LogDomain();
};// class ALoxReportWriter

} // namespace alib[::lox]

/// Type alias in namespace \b alib.
using   ALoxReportWriter= lox::ALoxReportWriter;

}  // namespace [alib]

#endif // HPP_ALIB_ALOX_REPORT_WRITER

