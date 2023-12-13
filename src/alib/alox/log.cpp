// #################################################################################################
//  aworx::lib::lox - ALox Logging Library
//
//  Copyright 2013-2023 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)
#   if !defined (HPP_ALIB_ALOX)
       #include "alib/alox/alox.hpp"
#   endif

#   if !defined (HPP_ALOX_VSTUDIO_LOGGER)
       #include "alib/alox/loggers/vstudiologger.hpp"
#   endif

#   if !defined (HPP_ALIB_ALOX_REPORT_WRITER)
#      include "alib/alox/reportwriter.hpp"
#   endif

#   if !defined (HPP_ALIB_ALOXMODULE)
#      include "alib/alox/aloxmodule.hpp"
#   endif

#   if defined(_WIN32) && ALIB_DEBUG && !defined (HPP_ALIB_FS_MODULES_DISTRIBUTION)
#      include "alib/lib/fs_modules/distribution.hpp"
#   endif
#endif // !defined(ALIB_DOX)

namespace aworx { namespace lib { namespace lox {

using namespace detail;

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
        static bool recursion= false;
        if( recursion )
            return;
        recursion= true;

        // block recursion caused by log operations in this code
        if ( DebugLogger != nullptr )
        {
            ALIB_WARNING( "ALOX", "Log::AddDebugLogger(): called twice." )
            recursion= false;
            return;
        }
        DebugLogger= reinterpret_cast<decltype(DebugLogger)>(-1);

        // add a VStudio logger if this a VStudio debug session
        #if defined(_MSC_VER) && ALIB_DEBUG
            if( ALIB.IsDebuggerPresent() )
            {
                Variable variable( Variables::NO_IDE_LOGGER );
                if( ALOX.GetConfig().Load( variable ) == Priorities::NONE || ! variable.IsTrue() )
                {
                    IDELogger= new VStudioLogger("IDE_LOGGER");

                    // add logger
                    lox->SetVerbosity( IDELogger, Verbosity::Verbose, "/"  );
                    lox->SetVerbosity( IDELogger, Verbosity::Warning, Lox::InternalDomains );
                }
           }
        #endif

        // add a default console logger
        DebugLogger= Lox::CreateConsoleLogger("DEBUG_LOGGER");

        // add logger
        lox->SetVerbosity( DebugLogger, Verbosity::Verbose );
        lox->SetVerbosity( DebugLogger, Verbosity::Warning, Lox::InternalDomains );

        // replace ALib's ReportWriter by an ALox ReportWriter
        Log::AddALibReportWriter( lox );

        recursion= false;
    }

    void Log::RemoveDebugLogger( Lox* lox )
    {
        // remove ALox specific report writer of ALib
        Log::RemoveALibReportWriter();

        // remove debug logger(s)
        ALIB_ASSERT_WARNING( DebugLogger != nullptr,
                             "Log::RemoveDebugLogger(): no debug logger to remove." )

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
                             "Log::AddReportWriter(): ALoxReportWriter already created." )

        // replace ALib's default ReportWriter (but only this!) by an ALoxReportWriter
        if ( lib::results::Report::GetDefault().PeekWriter() == &lib::results::ReportWriterStdIO::GetSingleton()  )
             lib::results::Report::GetDefault().PushWriter( DebugReportWriter= new ALoxReportWriter( lox ) );
    }

    void Log::RemoveALibReportWriter()
    {
        // replace the report writer
        if ( DebugReportWriter != nullptr )
        {
            lib::results::Report::GetDefault().PopWriter( DebugReportWriter );
            delete DebugReportWriter;
            DebugReportWriter=  nullptr;
        }
    }


#endif // ALOX_DBG_LOG

}}}// namespace [aworx::lib::lox]


