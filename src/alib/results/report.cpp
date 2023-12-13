// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2023 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)
#   if !defined(HPP_ALIB_RESULTS_REPORT)
#      include "alib/results/report.hpp"
#   endif

#   if ALIB_THREADS && !defined (HPP_ALIB_THREADS_SMARTLOCK)
#      include "alib/threads/smartlock.hpp"
#   endif

#   if !defined(HPP_ALIB_TEXT_FORMATTER_PYTHONSTYLE)
#      include "alib/text/formatterpythonstyle.hpp"
#   endif

#   if !defined(HPP_ALIB_TEXT_FORMATTER)
#      include "alib/text/formatter.hpp"
#   endif

#   if !defined (HPP_ALIB_RESULTS_RESULTS)
#      include "alib/results/results.hpp"
#   endif

#   if ALIB_DEBUG && defined( _WIN32 ) && !defined(HPP_ALIB_FS_MODULES_DISTRIBUTION)
#      include "alib/lib/fs_modules/distribution.hpp"
#   endif

#   if !defined (_GLIBCXX_IOSTREAM) && !defined (_IOSTREAM_ )
#      include <iostream>
#   endif

#   if defined( _WIN32 ) && ALIB_FILESET_MODULES
#       include "alib/lib/fs_modules/distribution.hpp"
#   endif
#endif // !defined(ALIB_DOX)


ALIB_BOXING_VTABLE_DEFINE( aworx::lib::results::Report::Types, vt_alib_report_types )


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
    PushHaltFlags( true, false );
    PushWriter( &ReportWriterStdIO::GetSingleton() );
}

Report::~Report()
{
    PopWriter( &ReportWriterStdIO::GetSingleton() );
}


void Report::PushHaltFlags( bool haltOnErrors, bool haltOnWarnings )
{
    ALIB_LOCK_WITH(lock)
    haltAfterReport.push(    (haltOnErrors   ? 1 : 0)
                           + (haltOnWarnings ? 2 : 0));
}
void Report::PopHaltFlags()
{
    ALIB_DBG(
        bool stackEmptyError;
    )

    {
        ALIB_LOCK_WITH(lock)
        haltAfterReport.pop();

        ALIB_DBG(  stackEmptyError= haltAfterReport.size() == 0; )
    }

    ALIB_DBG(
        if ( stackEmptyError )
        {
            PushHaltFlags( true, true );
            ALIB_ERROR( "RESULTS", "Stack empty, too many pop operations" );
        }
    )
}

void Report::PushWriter( ReportWriter* newReportWriter )
{
    ALIB_LOCK_WITH(lock)
    if ( writers.size() > 0 )
        writers.top()->NotifyActivation( Phase::End );
    writers.push( newReportWriter );
    newReportWriter->NotifyActivation( Phase::Begin );
}

void Report::PopWriter( ReportWriter* checkWriter )
{
    ALIB_LOCK_WITH(lock)
    if ( writers.size() == 0 )             {  ALIB_ERROR( "RESULTS", "No Writer to remove" )          return; }
    if ( writers.top()  != checkWriter )   {  ALIB_ERROR( "RESULTS", "Report Writer is not actual" )  return; }
    writers.top()->NotifyActivation( Phase::End );
    writers.pop();
    if ( writers.size() > 0 )
        writers.top()->NotifyActivation( Phase::Begin );
}

ReportWriter* Report::PeekWriter()
{
    ALIB_LOCK_WITH(lock)
    return writers.top();
}

void Report::DoReport( Message& message )
{
    ALIB_LOCK_WITH(lock)
    if (recursionBlocker)
        return;
    recursionBlocker= true;

        try
        {
            if ( writers.size() > 0 )
                writers.top()->Report( message );
        }
        catch( Exception& e )
        {
            e.Add( message.File, message.Line, message.Function, Exceptions::ErrorWritingReport );
            throw;
        }

        #if ALIB_DEBUG
            int haltFlags= haltAfterReport.top();
            bool halt=     (message.Type == Report::Types::Error   && ( (haltFlags & 1) != 0) )
                        || (message.Type == Report::Types::Warning && ( (haltFlags & 2) != 0) );

            #if defined( _WIN32 )
                if( halt )
                {
                    #if  ALIB_SYSTEM
                    if ( ALIB.IsDebuggerPresent() )
                        DebugBreak();
                    else
                    #endif
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
    #if ALIB_THREADS
        if ( phase == Phase::Begin )
            SmartLock::StdOutputStreams.AddAcquirer   ( nullptr );
        else
            SmartLock::StdOutputStreams.RemoveAcquirer( nullptr );
    #else
        (void) phase;
    #endif
}


void ReportWriterStdIO::Report( Message& msg )
{
    ALIB_IF_THREADS( SmartLock::StdOutputStreams.Acquire(ALIB_CALLER_PRUNED); )

        String1K buffer( "ALib " );
             if (  msg.Type.Integral() == 0 )   buffer._( "Error:   " );
        else if (  msg.Type.Integral() == 1 )   buffer._( "Warning: " );
        else                                    buffer._( "Report (type=" )._( msg.Type )._("): ");

        auto* out  = msg.Type.Integral() == 0 || msg.Type.Integral() == 1 ? &std::cerr : &std::cout;
        auto* other= msg.Type.Integral() == 0 || msg.Type.Integral() == 1 ? &std::cout : &std::cerr;

        // With ALox replacing this report writer (the usual thing), the first string might
        // become auto-detected as an ALox domain name. To keep this consistent wie do a similar
        // effort here (code is copied from ALoxReportWriter::Report())
        NString32 replacement;
        if(     msg.Size() > 1
            &&  msg[0].IsArrayOf<nchar>()
            &&  msg[0].UnboxLength() < 29 )
        {
            bool illegalCharacterFound= false;
            NString firstArg= msg[0].Unbox<NString>();
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
                replacement <<  firstArg << ": ";
                msg[0]= replacement;
            }
        }

        SPFormatter formatter= Formatter::GetDefault();
        if( formatter != nullptr)
        {
            formatter->Acquire( ALIB_CALLER_PRUNED );
            try
            {
                formatter->FormatArgs( buffer, msg );
            }
            catch ( Exception & e )
            {
                buffer << lib::RESULTS.GetResource( "RepFmtExc" );
                e.Format( buffer );
                out = &std::cerr;
                other = &std::cout;
            }
            formatter->Release();
        }
        else
        {
            for( auto& box : msg )
                buffer << box << " ";
            buffer << NewLine() << "(Note: Default Formatter was not available while writing Report)";
        }
        buffer << NewLine() << "At        :   " << msg.File << ':' << msg.Line << ' ' << msg.Function << "()";

        out  ->flush();
        other->flush();
        *out << std::endl;

        ALIB_STRINGS_TO_NARROW(buffer, nBuffer, 1024)
        out->write( nBuffer.Buffer(), nBuffer.Length() );
       *out << std::endl;
        out  ->flush();
        other->flush();

        #if defined( _WIN32 ) && ALIB_SYSTEM
            if ( ALIB.IsDebuggerPresent() )
            {
                #if !ALIB_CHARACTERS_WIDE
                    OutputDebugStringA( buffer );
                    OutputDebugStringA( "\r\n" );
                #else
                    OutputDebugStringW( buffer );
                    OutputDebugStringW( L"\r\n" );
                #endif
            }
        #endif

    ALIB_IF_THREADS( SmartLock::StdOutputStreams.Release(); )
}


}}} // namespace [aworx::lib::results]

