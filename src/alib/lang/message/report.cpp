// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !DOXYGEN
#   include "alib/lang/message/report.hpp"
#   include "alib/lang/format/formatterpythonstyle.hpp"
#   include "alib/lang/format/formatter.hpp"
#   include <iostream>
#   if defined( _WIN32 )
#       include "alib/lang/basecamp/basecamp.hpp"
#   endif
#   include "alib/lang/basecamp/camp_inlines.hpp"
#   if ALIB_THREADS
#     include "alib/threads/lock.hpp"
#   endif
#endif // !DOXYGEN


ALIB_BOXING_VTABLE_DEFINE( alib::lang::Report::Types, vt_alib_report_types )


namespace alib::lang {

// #################################################################################################
// static objects
// #################################################################################################
Report* Report::defaultReport                                                             = nullptr;

// #################################################################################################
// class Report
// #################################################################################################
Report::Report()
{
    IF_ALIB_THREADS(  ALIB_DBG(lock.Dbg.Name= "alib::lang::Report";) )

    PushHaltFlags( true, false );
    PushWriter( &ReportWriterStdIO::GetSingleton() );
}

Report::~Report()
{
    PopWriter( &ReportWriterStdIO::GetSingleton() );
}


void Report::PushHaltFlags( bool haltOnErrors, bool haltOnWarnings )
{
    ALIB_LOCK_RECURSIVE_WITH(lock)
    haltAfterReport.push(    (haltOnErrors   ? 1 : 0)
                           + (haltOnWarnings ? 2 : 0));
}
void Report::PopHaltFlags()
{
    ALIB_DBG(
        bool stackEmptyError;
    )

    {
        ALIB_LOCK_RECURSIVE_WITH(lock)
        haltAfterReport.pop();

        ALIB_DBG(  stackEmptyError= haltAfterReport.size() == 0; )
    }

    ALIB_DBG(
        if ( stackEmptyError )
        {
            PushHaltFlags( true, true );
            ALIB_ERROR( "REPORT", "Stack empty, too many pop operations" );
        }
    )
}

void Report::PushWriter( ReportWriter* newReportWriter )
{
    ALIB_LOCK_RECURSIVE_WITH(lock)
    if ( writers.size() > 0 )
        writers.top()->NotifyActivation( lang::Phase::End );
    writers.push( newReportWriter );
    newReportWriter->NotifyActivation( lang::Phase::Begin );
}

void Report::PopWriter( ReportWriter* checkWriter )
{
    ALIB_LOCK_RECURSIVE_WITH(lock)
    if ( writers.size() == 0 )             {  ALIB_ERROR( "REPORT", "No Writer to remove" )          return; }
    if ( writers.top()  != checkWriter )   {  ALIB_ERROR( "REPORT", "Report Writer is not actual" )  return; }
    writers.top()->NotifyActivation( lang::Phase::End );
    writers.pop();
    if ( writers.size() > 0 )
        writers.top()->NotifyActivation( lang::Phase::Begin );
}

ReportWriter* Report::PeekWriter()
{
    ALIB_LOCK_RECURSIVE_WITH(lock)
    return writers.top();
}

void Report::DoReport( Message& message )
{
    ALIB_LOCK_RECURSIVE_WITH(lock)
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
            e.Add( message.CI, ReportExceptions::ErrorWritingReport );
            throw;
        }

        #if ALIB_DEBUG
            int haltFlags= haltAfterReport.top();
            bool halt=     (message.Type == Report::Types::Error   && ( (haltFlags & 1) != 0) )
                        || (message.Type == Report::Types::Warning && ( (haltFlags & 2) != 0) );
            (void) halt; // for release compiles with ALIB_DEBUG set
            #if defined( _WIN32 )
                if( halt )
                {
                    #if  ALIB_CAMP
                    if ( BASECAMP.IsDebuggerPresent() )
                        DebugBreak();
                    else
                    #endif
                        assert( false );
                }
            #else
                #if defined(__GNUC__) || defined(__clang__)
                    if (halt)  __builtin_trap();
                #elif defined ( _MSC_VER )
                    if (halt)  __debugbreak();
                #else
                    assert( !halt );
                #endif

            #endif

        #endif
    recursionBlocker= false;
}

void ReportWriterStdIO::Report( Message& msg )
{
        String1K buffer( "ALib " );
        buffer.DbgDisableBufferReplacementWarning();
             if (  msg.Type.Integral() == 0 )   buffer._( "Error " );
        else if (  msg.Type.Integral() == 1 )   buffer._( "Warning " );
        else                                    buffer._( "Report (type=" )._( msg.Type )._(") ");

        auto* out  = msg.Type.Integral() == 0 || msg.Type.Integral() == 1 ? &std::cerr : &std::cout;
        auto* other= msg.Type.Integral() == 0 || msg.Type.Integral() == 1 ? &std::cout : &std::cerr;

        // With ALox replacing this report writer (the usual thing), the first string might
        // become auto-detected as an ALox domain name. To keep this consistent we do a similar
        // effort here (code is copied from ALoxReportWriter::Report())
        NString64 replacement;
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
                replacement <<  "in " << firstArg << ": ";
                msg[0]= replacement;
            }
        }


        ALIB_LOCK_RECURSIVE_WITH(Formatter::DefaultLock)
        if( Formatter::Default != nullptr)
        {
            try
            {
                Formatter::Default->FormatArgs( buffer, msg );
            }
            catch ( Exception & e )
            {
                buffer << BASECAMP.GetResource( "RepFmtExc" );
                e.Format( buffer );
                out = &std::cerr;
                other = &std::cout;
            }
        }
        else
        {
            for( auto& box : msg )
                buffer << box << " ";
            buffer.NewLine() << "(Note: Default Formatter was not available while writing Report)";
        }
        buffer.NewLine() << "At: " << msg.CI;

    {ALIB_LOCK_WITH( threads::STD_IOSTREAMS_LOCK )

        out  ->flush();
            other->flush();
            *out << std::endl;

            ALIB_STRINGS_TO_NARROW(buffer, nBuffer, 16*1024)
            out->write( nBuffer.Buffer(), nBuffer.Length() );
            *    out << std::endl;
            out  ->flush();
            other->flush();

            #if defined( _WIN32 ) && ALIB_CAMP
            if ( BASECAMP.IsDebuggerPresent() )
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
    }
}


} // namespace [alib::lang]
