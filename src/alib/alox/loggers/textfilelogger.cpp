// #################################################################################################
//  alib::lox::loggers - ALox Logging Library
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib_precompile.hpp"
#if !defined(ALIB_C20_MODULES) || ((ALIB_C20_MODULES != 0) && (ALIB_C20_MODULES != 1))
#   error "Symbol ALIB_C20_MODULES has to be given to the compiler as either 0 or 1"
#endif
#if ALIB_C20_MODULES
    module;
#endif
// ======================================   Global Fragment   ======================================
#ifndef H_ALIB_ALOX
#include "alib/strings/strings.prepro.hpp"
#include "alib/alox/alox.prepro.hpp"
#endif
#include <fstream>
// ===========================================   Module   ==========================================
#if ALIB_C20_MODULES
    module ALib.ALox.Impl;
    import   ALib.Lang;
    import   ALib.Strings;
#else
#   include "ALib.Lang.H"
#   include "ALib.Strings.H"
#   include "ALib.ALox.H"
#   include "ALib.ALox.Impl.H"
#endif
// ======================================   Implementation   =======================================
using namespace alib;

TextFileLogger::TextFileLogger( const alib::String&  fileName,
                                const alib::NString& loggerName )
: PlainTextLogger( loggerName, "TEXTFILE", false )
{
    FileName << fileName;

    // test open file for test
    ALIB_STRINGS_TO_NARROW(FileName,nFileName,1024)
    auto* os= new std::ofstream( nFileName, std::ios::app );
    if ( !os->is_open() )
    {
        LastSystemError= SystemErrors(errno);
        ALIB_WARNING( "ALOX", "Could not open file: \"{}\". System error code: {}",
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
        LastSystemError= SystemErrors(errno);
        ALIB_WARNING( "ALOX", "Could not open file: \"{}\". System error code: {}",
                      FileName, LastSystemError )
        delete os;  os= nullptr;
        return;
    }
    else
        LastSystemError= SystemErrors::None;

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


void TextFileLogger::notifyMultiLineOp( lang::Phase phase )
{
    // save state (to have it in logText)
    currentlyInMultiLineOp= (phase == lang::Phase::Begin);

    // open/close the file
    if ( phase == lang::Phase::Begin )
        openFile();
    else if( writer.GetStream() != nullptr )
        closeFile();
}

bool TextFileLogger::notifyLogOp( lang::Phase phase )
{
    auto* stream= writer.GetStream();
    if ( stream != nullptr && phase == lang::Phase::End )
        *stream << std::endl;

    // open/close
    if( !currentlyInMultiLineOp )
    {
        if ( phase == lang::Phase::Begin )
            openFile();
        else
            closeFile();
    }

    return LastSystemError == SystemErrors::None;
}

integer TextFileLogger::logSubstring( const String& buffer, integer start, integer length )
{
    if (writer.GetStream() != nullptr && LastSystemError == SystemErrors::None)
        return writer.WriteAndGetWideLength( buffer.Substring<NC>( start, length ) );
    return 0;
}

