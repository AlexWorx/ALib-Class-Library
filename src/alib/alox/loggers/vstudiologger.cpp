// #################################################################################################
//  aworx::lib::lox::loggers - ALox Logging Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib.hpp"

#if defined(_WIN32) && ALIB_DEBUG

#if !defined (HPP_ALIB_CONFIG_CONFIGURATION)
    #include "alib/config/configuration.hpp"
#endif

#if !defined (HPP_ALIB_STRINGS_UTIL_TOKENIZER)
    #include "alib/strings/util/tokenizer.hpp"
#endif

#include "alib/alox/loggers/vstudiologger.hpp"


#if !defined (_GLIBCXX_IOSTREAM) && !defined(_IOSTREAM_)
    #include <iostream>
#endif
#if !defined (_STRING_H) && !defined(_INC_STRING)
    #include <string.h>
#endif


using namespace std;
using namespace aworx;
using namespace aworx::lib::lox::core;


// #################################################################################################
// Constructor/Destructor
// #################################################################################################

VStudioLogger::VStudioLogger( const NString&  name )
: PlainTextLogger( name, "VSTUDIO_CONSOLE", false )
{
    ALIB_ASSERT_ERROR ( lib::ALIB.IsDebuggerPresent(), ASTR("This is not a debug session within Visual Studio") )

    // prevent cutting off filenames
    MetaInfo->Format.SearchAndReplace( ASTR("%SF:%SL:"), ASTR("%SP\\%SF(%SL):") );
}

VStudioLogger::~VStudioLogger()
{}

bool VStudioLogger::notifyLogOp( Phase phase)
{
    if ( phase == Phase::End )
        OutputDebugStringA( "\r\n" );
    return true;
}

integer VStudioLogger::logSubstring( const String& buffer, integer start, integer length )
{
    outputString.Clear() << String(buffer.Buffer() + start, length );
    OutputDebugString ( outputString.ToCString() );
    return outputString.Length();
}


#endif //defined(_WIN32) &&  ALIB_DEBUG
