// #################################################################################################
//  aworx::lib::lox::loggers - ALox Logging Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib.hpp"

#if !defined (HPP_ALIB_CONFIG_CONFIGURATION)
#   include "alib/config/configuration.hpp"
#endif
#if !defined (HPP_ALIB_COMPATIBILITY_STD_IOSTREAM)
#   include "alib/compatibility/std_iostream.hpp"
#endif
#if !defined (HPP_ALOX_TEXT_FILE_LOGGER)
#   include "alib/alox/loggers/textfilelogger.hpp"
#endif

#if !defined (_GLIBCXX_FSTREAM) && !defined(_FSTREAM_)
#include <fstream>
#endif


using namespace aworx;

void TextFileLogger::openFile()
{
    ALIB_STD_TO_NARROW_TSTRING(FileName,nFileName)

    auto* os= new std::ofstream( nFileName.ToCString(), std::ios::app );

    if ( !os->is_open() )
    {
        ALIB_WARNING( "Could not open file: '{}'.", FileName);
        delete os;  os= nullptr;
        hasIoError= true;
        return;
    }
    writer.SetStream( os );
    hasIoError= false;
}

void TextFileLogger::closeFile()
{
    ALIB_ASSERT( writer.GetStream() != nullptr );
    auto* os= dynamic_cast<std::ofstream*>(writer.GetStream());

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
    else
        closeFile();
}

bool TextFileLogger::notifyLogOp( Phase phase )
{
    if ( phase == Phase::End )
        *writer.GetStream() << std::endl;

    // open/close
    if( !currentlyInMultiLineOp )
    {
        if ( phase == Phase::Begin )
            openFile();
        else
            closeFile();
    }

    return !hasIoError;
}

integer TextFileLogger::logSubstring( const String& buffer, integer start, integer length )
{
    if (!hasIoError)
        return writer.Write( buffer.Substring<false>( start, length ) );

    return 0;
}
