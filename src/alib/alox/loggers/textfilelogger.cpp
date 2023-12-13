// #################################################################################################
//  aworx::lib::lox::loggers - ALox Logging Library
//
//  Copyright 2013-2023 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)
#if !defined (HPP_ALOX_TEXT_FILE_LOGGER)
#   include "alib/alox/loggers/textfilelogger.hpp"
#endif

#if !defined(HPP_ALIB_RESULTS_REPORT)
#   include "alib/results/report.hpp"
#endif

#if !defined (_GLIBCXX_FSTREAM) && !defined(_FSTREAM_)
#   include <fstream>
#endif
#endif // !defined(ALIB_DOX)


using namespace aworx;

TextFileLogger::TextFileLogger( const aworx::String&  fileName,
                                const aworx::NString& loggerName )
: PlainTextLogger( loggerName, "TEXTFILE", false )
{
    FileName << fileName;

    // test open file for test
    ALIB_STRINGS_TO_NARROW(FileName,nFileName,1024)
    auto* os= new std::ofstream( nFileName, std::ios::app );
    if ( !os->is_open() )
    {
        LastSystemError= system::SystemErrors(errno);
        ALIB_WARNING( "ALOX", "Could not open file: {!Q}. System error code: {}",
                      FileName, LastSystemError )
    }
    else
        // test run OK
        os->close();

    delete os;
}

void TextFileLogger::openFile()
{
    ALIB_STRINGS_TO_NARROW(FileName,nFileName,1024)
    auto* os= new std::ofstream( nFileName, std::ios::app );
    if ( !os->is_open() )
    {
        writer.SetStream( nullptr );
        LastSystemError= system::SystemErrors(errno);
        ALIB_WARNING( "ALOX", "Could not open file: {!Q}. System error code: {}",
                      FileName, LastSystemError )
        delete os;  os= nullptr;
        return;
    }
    else
        LastSystemError= system::SystemErrors::None;

    writer.SetStream( os );
}

void TextFileLogger::closeFile()
{
    auto* stream= writer.GetStream();
    if( stream == nullptr )
        return;

    auto* os= dynamic_cast<std::ofstream*>(stream);
    os->close();
    delete os;
    writer.SetStream( nullptr );
}


void TextFileLogger::notifyMultiLineOp( Phase phase )
{
    // save state (to have it in logText)
    currentlyInMultiLineOp= (phase == Phase::Begin);

    // open/close the file
    if ( phase == Phase::Begin )
        openFile();
    else if( writer.GetStream() != nullptr )
        closeFile();
}

bool TextFileLogger::notifyLogOp( Phase phase )
{
    auto* stream= writer.GetStream();
    if ( stream != nullptr && phase == Phase::End )
        *stream << std::endl;

    // open/close
    if( !currentlyInMultiLineOp )
    {
        if ( phase == Phase::Begin )
            openFile();
        else
            closeFile();
    }

    return LastSystemError == system::SystemErrors::None;
}

integer TextFileLogger::logSubstring( const String& buffer, integer start, integer length )
{
    if (writer.GetStream() != nullptr && LastSystemError == system::SystemErrors::None)
        return writer.WriteAndGetWideLength( buffer.Substring<false>( start, length ) );
    return 0;
}
