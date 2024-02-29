// #################################################################################################
//  alib::lox::detail - ALox Logging Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)
#if !defined (HPP_ALIB_ALOX_REPORT_WRITER)
#   include "alib/alox/reportwriter.hpp"
#endif

#if !defined (HPP_ALIB_ALOXMODULE)
#   include "alib/alox/aloxmodule.hpp"
#endif
#endif // !defined(ALIB_DOX)


namespace alib {  namespace lox {

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

void ALoxReportWriter::Report( lang::Message& msg )
{
    #if ALIB_DEBUG
        lox->Acquire( msg.File, msg.Line, msg.Function );

            auto& logables= lox->GetLogableContainer();
            logables.Add( msg );

            auto verbosity= msg.Type == lang::Report::Types::Error    ? Verbosity::Error   :
                            msg.Type == lang::Report::Types::Warning  ? Verbosity::Warning :
                            msg.Type == lang::Report::Types::Message  ? Verbosity::Info    :
                                                                           Verbosity::Verbose ;

            NString256 domain= ALoxReportWriter::LogDomain();

            // detect subdomain
            NString256 detectedDomain;
            if(     logables.Size() > 1
                &&  logables[0].IsArrayOf<nchar>() )
            {
                bool illegalCharacterFound= false;
                NString firstArg= logables[0].Unbox<NString>();
                for( integer idx= 0 ;  idx< firstArg.Length() ; ++idx )
                {
                    char c= firstArg[idx];
                    if (!    (    isdigit( c )
                               || ( c >= 'A' && c <= 'Z' )
                               || c == '-'
                               || c == '_'
                               || c == '/'
                               || c == '.'
                      )      )
                    {
                        illegalCharacterFound= true;
                        break;
                    }
                }

                if(!illegalCharacterFound)
                {
                    detectedDomain << domain << '/' << firstArg;
                    domain= detectedDomain;
                    logables.erase( logables.begin() );
                }
                else
                    domain= ALoxReportWriter::LogDomain();
            }

            lox->Entry( domain, verbosity );

        lox->Release ();
    #else
        (void) msg;
    #endif
}

NString16 ALoxReportWriter::reportDomain;

NString& ALoxReportWriter::LogDomain()
{
    if( reportDomain.IsEmpty() )
        reportDomain << Lox::InternalDomains << "REPORT";
    return reportDomain;
}


}} // namespace [alib::lox]
