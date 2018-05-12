// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib.hpp"

#include "alib/compatibility/std_iostream.hpp"

#if !defined (HPP_ALIB_THREADS_SMARTLOCK)
    #include "alib/threads/smartlock.hpp"
#endif

#if !defined(HPP_ALIB_STRINGS_FORMAT_FORMATTER_PYTHONSTYLE)
    #include "alib/strings/format/formatterpythonstyle.hpp"
#endif

#if !defined(HPP_ALIB_STRINGS_FORMAT_FORMATTER)
    #include "alib/strings/format/formatter.hpp"
#endif


namespace aworx { namespace lib { namespace lang {

// #################################################################################################
// static objects
// #################################################################################################
Report* Report::defaultReport                                                             = nullptr;

// #################################################################################################
// class Report
// #################################################################################################
Report::Report()
{
    lock= new ThreadLock();
    PushHaltFlags( true, false );
    PushWriter( ReportWriterStdIO::GetSingleton() );
}

Report::~Report()
{
    PopWriter( ReportWriterStdIO::GetSingleton() );
    delete lock;
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
            e.Add( message.File, message.Line, message.Func, lang::Exceptions::ErrorWritingReport );
            throw;
        }

        #if ALIB_DEBUG
            int haltFlags= haltAfterReport.top();
            bool halt=     (message.Type == 0 && ( (haltFlags & 1) != 0) )
                        || (message.Type == 1 && ( (haltFlags & 2) != 0) );

            #if defined( _WIN32 )
                if( halt )
                {
                    if ( lib::ALIB.IsDebuggerPresent() )
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


void ReportWriterStdIO::NotifyActivation( lang::Phase phase )
{
    if ( phase == Phase::Begin )
        lib::ALIB.StdOutputStreamsLock.AddAcquirer   ( nullptr );
    else
        lib::ALIB.StdOutputStreamsLock.RemoveAcquirer( nullptr );
}


void ReportWriterStdIO::Report( const Report::Message& msg )
{
    lib::ALIB.StdOutputStreamsLock.Acquire(ALIB_CALLER_PRUNED);

        String512 buffer( "ALib " );
             if (  msg.Type == 0 )   buffer._( "Error:   " );
        else if (  msg.Type == 1 )   buffer._( "Warning: " );
        else                         buffer._( "Report (type=" )._( msg.Type )._("): ");

        auto& out   = msg.Type == 0 || msg.Type == 1 ? std::cerr : std::cout;
        auto& other = msg.Type == 0 || msg.Type == 1 ? std::cout : std::cerr;

        lib::STRINGS.GetDefaultFormatter()->Format( buffer, dynamic_cast<const Boxes&>( msg ) );

        out  .flush();
        other.flush();
        out << std::endl;

        ALIB_STD_TO_NARROW_TSTRING(buffer, nBuffer);
        out.write( nBuffer.Buffer(), nBuffer.Length() );
        out << std::endl;
        out  .flush();
        other.flush();

        #if defined( _WIN32 )
            if ( lib::ALIB.IsDebuggerPresent() )
            {
                #if ALIB_NARROW_STRINGS
                    OutputDebugStringA( buffer.ToCString() );
                    OutputDebugStringA( "\r\n" );
                #else
                    OutputDebugStringW( buffer.ToCString() );
                    OutputDebugStringW( L"\r\n" );
                #endif
            }
        #endif

    lib::ALIB.StdOutputStreamsLock.Release();
}


}}}// namespace [aworx::lib::lang]

