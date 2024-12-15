// #################################################################################################
//  alib::lox - ALox Logging Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"
#include "loggers/ansilogger.hpp"

#if !DOXYGEN
#   include "alib/alox/alox.hpp"
#   include "alib/alox/textlogger/textlogger.hpp"
#   include "alib/alox/loggers/vstudiologger.hpp"
#   include "alib/alox/reportwriter.hpp"
#   include "alib/alox/aloxcamp.hpp"
#   if defined(_WIN32) && ALIB_DEBUG
#      include "alib/lang/basecamp/basecamp.hpp"
#   endif
#endif // !DOXYGEN

namespace alib {  namespace lox {

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

#include "alib/lang/callerinfo_functions.hpp"
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

        // add a VStudio logger if this is a VStudio debug session
        #if defined(_MSC_VER) && ALIB_DEBUG
            if( BASECAMP.IsDebuggerPresent() )
            {
                bool createIDELogger;
                {ALIB_LOCK_WITH(ALOX.GetConfigLock())
                    Variable variable(ALOX, Variables::NO_IDE_LOGGER );
                    createIDELogger= variable.IsNotDefined() || (variable.GetBool() == false);
                }
                if(createIDELogger)
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

        // add logger by setting verbosities
        lox->SetVerbosity( DebugLogger, Verbosity::Verbose );
        lox->SetVerbosity( DebugLogger, Verbosity::Warning, Lox::InternalDomains );
        lox->SetVerbosity( DebugLogger, Verbosity::Warning, "/ALIB" );

        // check various variables, if existed already externally. If not, create them empty or
        // with debug defaults (only done here, namely for debug logger)
        {ALIB_LOCK_WITH(ALOX.GetConfigLock())
            // Verbosity: If not, set 'ExportAll' flag
            Variable variable( ALOX );
            Box replacements[2]= { "LOG",  "DEBUG_LOGGER"  };
            variable.Declare( Variables::VERBOSITY, replacements );
            if( variable.IsNotDefined() )
            {
                (void) variable.Define();
                variable.Get<CVVerbosities>().ExportAll= true;
            }

            variable.Declare( Variables::SPTR_LOX           , "LOG" );   (void) variable.Define();
            variable.Declare( Variables::DOMAIN_SUBSTITUTION, "LOG" );   (void) variable.Define();
            variable.Declare( Variables::PREFIXES           , "LOG" );   (void) variable.Define();
            variable.Declare( Variables::DUMP_STATE_ON_EXIT , "LOG" );   (void) variable.Define();
            if( dynamic_cast<AnsiConsoleLogger*>(DebugLogger) != nullptr )
                variable.Declare( Variables::CONSOLE_LIGHT_COLORS );   (void) variable.Define();
        }

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
        if ( lang::Report::GetDefault().PeekWriter() == &lang::ReportWriterStdIO::GetSingleton()  )
             lang::Report::GetDefault().PushWriter( DebugReportWriter= new ALoxReportWriter( lox ) );
    }

    void Log::RemoveALibReportWriter()
    {
        // replace the report writer
        if ( DebugReportWriter != nullptr )
        {
            lang::Report::GetDefault().PopWriter( DebugReportWriter );
            delete DebugReportWriter;
            DebugReportWriter=  nullptr;
        }
    }
#include "alib/lang/callerinfo_methods.hpp"

#endif // ALOX_DBG_LOG

}}// namespace [alib::lox]

