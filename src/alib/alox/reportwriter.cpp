// #################################################################################################
//  aworx::lib::lox::detail - ALox Logging Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined (HPP_ALIB_ALOX_REPORT_WRITER)
#   include "alib/alox/reportwriter.hpp"
#endif


namespace aworx { namespace lib { namespace lox {

ALoxReportWriter::ALoxReportWriter ( Lox* pLox )
{
    this->lox= pLox;

    #if ALIB_DEBUG
        pLox->Acquire( ALIB_CALLER );

            pLox->GetLogableContainer().Add( A_CHAR("ALoxReportWriter set") );
            pLox->Entry( ALoxReportWriter::LogDomain(), Verbosity::Verbose );

        pLox->Release ();
    #else

    #endif
}

void ALoxReportWriter::Report( const lib::results::Message& msg )
{
    #if ALIB_DEBUG
        lox->Acquire( msg.File, msg.Line, msg.Func );

            lox->GetLogableContainer().Add( msg.Args );
            lox->Entry( ALoxReportWriter::LogDomain(),
                        msg.Type == results::Report::Types::Error    ? Verbosity::Error   :
                        msg.Type == results::Report::Types::Warning  ? Verbosity::Warning :
                        msg.Type == results::Report::Types::Message  ? Verbosity::Info    :
                                                                     Verbosity::Verbose     );

        lox->Release ();
    #else
        (void) msg;
    #endif
}

NString16 ALoxReportWriter::reportDomain;

NString& ALoxReportWriter::LogDomain()
{
    if( reportDomain.IsEmpty() )
        reportDomain << ALox::InternalDomains << "REPORT";
    return reportDomain;
}


}}} // namespace [aworx::lib::lox]
