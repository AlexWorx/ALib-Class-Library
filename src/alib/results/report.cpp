// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(HPP_ALIB_RESULTS_REPORT)
#   include "alib/results/report.hpp"
#endif

#if ALIB_MODULE_THREADS && !defined (HPP_ALIB_THREADS_SMARTLOCK)
#   include "alib/threads/smartlock.hpp"
#endif

#if !defined(HPP_ALIB_STRINGFORMAT_FORMATTER_PYTHONSTYLE)
#   include "alib/stringformat/formatterpythonstyle.hpp"
#endif

#if !defined(HPP_ALIB_STRINGFORMAT_FORMATTER)
#   include "alib/stringformat/formatter.hpp"
#endif

#if !defined (HPP_ALIB_RESULTS_RESULTS)
#   include "alib/results/results.hpp"
#endif

#if ALIB_DEBUG && defined( _WIN32 ) && !defined(HPP_ALIB_LIB_ALIBMODULES)
#   include "alib/lib/alibmodules.hpp"
#endif

#if !defined (_GLIBCXX_IOSTREAM) && !defined (_IOSTREAM_ )
#   include <iostream>
#endif


namespace aworx { namespace lib { namespace results {

// #################################################################################################
// static objects
// #################################################################################################
Report* Report::defaultReport                                                             = nullptr;

// #################################################################################################
// class Report
// #################################################################################################
Report::Report()
{
    #if ALIB_MODULE_THREADS
        lock= new threads::ThreadLock();
    #endif

    PushHaltFlags( true, false );
    PushWriter( &ReportWriterStdIO::GetSingleton() );
}

Report::~Report()
{
    PopWriter( &ReportWriterStdIO::GetSingleton() );
    #if ALIB_MODULE_THREADS
        delete lock;
    #endif
}

void Report::TerminationCleanUp()
{
    if ( defaultReport != nullptr )
        delete defaultReport;
    if( ReportWriterStdIO::singleton )
        delete ReportWriterStdIO::singleton;
}


void Report::PushHaltFlags( bool haltOnErrors, bool haltOnWarnings )
{
    ALIB_LOCK_WITH(*lock)
    haltAfterReport.push(    (haltOnErrors   ? 1 : 0)
                           + (haltOnWarnings ? 2 : 0));
}
void Report::PopHaltFlags()
{
    ALIB_DBG(
        bool stackEmptyError;
    )

    {
        ALIB_LOCK_WITH(*lock)
        haltAfterReport.pop();

        ALIB_DBG(  stackEmptyError= haltAfterReport.size() == 0; )
    }

    ALIB_DBG(
        if ( stackEmptyError )
        {
            PushHaltFlags( true, true );
            ALIB_ERROR( "Stack empty, too many pop operations" );
        }
    )
}

void Report::PushWriter( ReportWriter* newReportWriter )
{
    ALIB_LOCK_WITH(*lock)
    if ( writers.size() > 0 )
        writers.top()->NotifyActivation( Phase::End );
    writers.push( newReportWriter );
    newReportWriter->NotifyActivation( Phase::Begin );
}

void Report::PopWriter( ReportWriter* checkWriter )
{
    ALIB_LOCK_WITH(*lock)
    if ( writers.size() == 0 )             {  ALIB_ERROR( "No Writer to remove" );         return; }
    if ( writers.top()  != checkWriter )   {  ALIB_ERROR( "Report Writer is not actual" ); return; }
    writers.top()->NotifyActivation( Phase::End );
    writers.pop();
    if ( writers.size() > 0 )
        writers.top()->NotifyActivation( Phase::Begin );
}

ReportWriter* Report::PeekWriter()
{
    ALIB_LOCK_WITH(*lock)
    return writers.top();
}

void Report::DoReport( const Message& message )
{
    ALIB_LOCK_WITH(*lock)
    if (recursionBlocker)
        return;
    recursionBlocker= true;

        try
        {
            if ( writers.size() > 0 )
                writers.top()->Report( message  );
        }
        catch( Exception& e )
        {
            e.Add( message.File, message.Line, message.Func, Exceptions::ErrorWritingReport );
            throw;
        }

        #if ALIB_DEBUG
            int haltFlags= haltAfterReport.top();
            bool halt=     (message.Type == Report::Types::Error   && ( (haltFlags & 1) != 0) )
                        || (message.Type == Report::Types::Warning && ( (haltFlags & 2) != 0) );

            #if defined( _WIN32 )
                if( halt )
                {
                    if ( ALIB.IsDebuggerPresent() )
                        DebugBreak();
                    else
                        assert( false );
                }
            #else
                assert( !halt );
            #endif

        #endif
    recursionBlocker= false;
}


void ReportWriterStdIO::NotifyActivation( Phase phase )
{
    #if ALIB_MODULE_THREADS
        if ( phase == Phase::Begin )
            SmartLock::StdOutputStreams.AddAcquirer   ( nullptr );
        else
            SmartLock::StdOutputStreams.RemoveAcquirer( nullptr );
    #else
        (void) phase;
    #endif
}


void ReportWriterStdIO::Report( const Message& msg )
{
    #if ALIB_MODULE_THREADS
        SmartLock::StdOutputStreams.Acquire(ALIB_CALLER_PRUNED);
    #endif

        String512 buffer( "ALib " );
             if (  msg.Type.Value() == 0 )   buffer._( "Error:   " );
        else if (  msg.Type.Value() == 1 )   buffer._( "Warning: " );
        else                                 buffer._( "Report (type=" )._( msg.Type )._("): ");

        auto& out   = msg.Type.Value() == 0 || msg.Type.Value() == 1 ? std::cerr : std::cout;
        auto& other = msg.Type.Value() == 0 || msg.Type.Value() == 1 ? std::cout : std::cerr;

        GetDefaultFormatter()->FormatArgs( buffer, msg.Args );

        out  .flush();
        other.flush();
        out << std::endl;

        ALIB_STRINGS_TO_NARROW(buffer, nBuffer, 1024);
        out.write( nBuffer.Buffer(), nBuffer.Length() );
        out << std::endl;
        out  .flush();
        other.flush();

        #if defined( _WIN32 )
            if ( ALIB.IsDebuggerPresent() )
            {
                #if ALIB_CHARACTERS_ARE_NARROW
                    OutputDebugStringA( buffer );
                    OutputDebugStringA( "\r\n" );
                #else
                    OutputDebugStringW( buffer );
                    OutputDebugStringW( L"\r\n" );
                #endif
            }
        #endif

    #if ALIB_MODULE_THREADS
        SmartLock::StdOutputStreams.Release();
    #endif
}


}}} // namespace [aworx::lib::results]

