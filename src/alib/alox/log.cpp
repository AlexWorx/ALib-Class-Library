// #################################################################################################
//  aworx::lib::lox - ALox Logging Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib.hpp"

#if !defined (HPP_ALIB_CONFIG_CONFIGURATION)
    #include "alib/config/configuration.hpp"
#endif

#if !defined (HPP_ALIB_ALOX)
    #include "alib/alox/alox.hpp"
#endif

#if !defined (HPP_ALOX_CORE_TEXTLOGGER_TEXTLOGGER)
    #include "alib/alox/core/textlogger/textlogger.hpp"
#endif

#if !defined (HPP_ALOX_VSTUDIO_LOGGER)
    #include "alib/alox/loggers/vstudiologger.hpp"
#endif


using namespace std;
namespace aworx { namespace lib { namespace lox {

using namespace core;

// #################################################################################################
// Static fields
// #################################################################################################
#if ALOX_DBG_LOG
    ALoxReportWriter*   Log::DebugReportWriter   = nullptr;


    // #################################################################################################
    // Auto detection of DEBUG environment
    // #################################################################################################

    TextLogger*   Log::DebugLogger= nullptr;
    TextLogger*   Log::IDELogger  = nullptr;


    void Log::AddDebugLogger( Lox* lox )
    {
        if ( DebugLogger != nullptr )
        {
            ALIB_WARNING( ASTR("Log::AddDebugLogger(): called twice.") );
            return;
        }

        // add a VStudio logger if this a VStudio debug session
        #if defined(_WIN32) && ALIB_DEBUG
            if( lib::ALIB.IsDebuggerPresent() )
            {
                Variable variable( Variables::NO_IDE_LOGGER );
                if( ALOX.Config->Load( variable ) == Priorities::NONE || ! variable.IsTrue() )
                {
                    IDELogger= new VStudioLogger("IDE_LOGGER");

                    // add logger
                    lox->SetVerbosity( IDELogger, Verbosity::Verbose, "/"  );
                    lox->SetVerbosity( IDELogger, Verbosity::Warning, ALox::InternalDomains );
                }
           }
        #endif

        // add a default console logger
        DebugLogger= Lox::CreateConsoleLogger("DEBUG_LOGGER");

        // add logger
        lox->SetVerbosity( DebugLogger, Verbosity::Verbose );
        lox->SetVerbosity( DebugLogger, Verbosity::Warning, ALox::InternalDomains );

        // replace ALibs' ReportWriter by an ALox ReportWriter
        Log::AddALibReportWriter( lox );

    }

    void Log::RemoveDebugLogger( Lox* lox )
    {
        // remove ALox specific report writer of ALib
        Log::RemoveALibReportWriter();

        // remove debug logger(s)
        ALIB_ASSERT_WARNING( DebugLogger != nullptr, ASTR("Log::RemoveDebugLogger(): no debug logger to remove.") );

        if ( DebugLogger != nullptr )
        {
            lox->RemoveLogger( DebugLogger );

            delete DebugLogger;
            DebugLogger= nullptr;
        }

        #if defined(_WIN32) && ALIB_DEBUG
            if ( IDELogger != nullptr )
            {
                lox->RemoveLogger( IDELogger );

                delete IDELogger;
                IDELogger= nullptr;
            }
        #endif
    }


// #################################################################################################
// ALib Report Writer installation
// #################################################################################################

    void Log::AddALibReportWriter( Lox* lox )
    {
        ALIB_ASSERT_WARNING( DebugReportWriter == nullptr,
                             ASTR("Log::AddReportWriter(): ALoxReportWriter already created.") );

        // replace ALibs' default ReportWriter (but only this!) by an ALoxReportWriter
        if ( lib::lang::Report::GetDefault().PeekWriter() == lib::lang::ReportWriterStdIO::GetSingleton()  )
            lib::lang::Report::GetDefault().PushWriter( DebugReportWriter= new ALoxReportWriter( lox ) );
    }

    void Log::RemoveALibReportWriter()
    {
        // replace the report writer
        if ( DebugReportWriter != nullptr )
        {
            lib::lang::Report::GetDefault().PopWriter( DebugReportWriter );
            delete DebugReportWriter;
            DebugReportWriter=  nullptr;
        }
    }


#endif // ALOX_DBG_LOG

}}}// namespace [aworx::lib::lox]


