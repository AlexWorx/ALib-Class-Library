// #################################################################################################
//  alib::lox::loggers - ALox Logging Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)
#if !defined (HPP_ALOX_VSTUDIO_LOGGER)
#   include "alib/alox/loggers/vstudiologger.hpp"
#endif
#endif // !defined(ALIB_DOX)

#if defined(_MSC_VER) && ALIB_DEBUG

#if !defined(ALIB_DOX)
#   if !defined (HPP_ALIB_LANG_BASECAMP)
#      include "alib/lang/basecamp/basecamp.hpp"
#   endif
#endif // !defined(ALIB_DOX)


#if !defined(_IOSTREAM_)
    #include <iostream>
#endif
#if !defined (_STRING_H) && !defined(_INC_STRING)
    #include <string.h>
#endif

using namespace alib;
using namespace alib::lox;
using namespace alib::lox::detail;

// #################################################################################################
// Constructor/Destructor
// #################################################################################################

VStudioLogger::VStudioLogger( const NString&  name )
: PlainTextLogger( name, "VSTUDIO_CONSOLE", false )
{
    ALIB_ASSERT_ERROR ( BASECAMP.IsDebuggerPresent(), "ALOX", "This is not a debug session within Visual Studio" )

    // prevent cutting off filenames
    MetaInfo->Format.SearchAndReplace( A_CHAR("%SF:%SL:"), A_CHAR("%SP\\%SF(%SL):") );
}

VStudioLogger::~VStudioLogger()
{}

bool VStudioLogger::notifyLogOp(lang::Phase phase)
{
    if ( phase == lang::Phase::End )
        OutputDebugStringA( "\r\n" );
    return true;
}

integer VStudioLogger::logSubstring( const String& buffer, integer start, integer length )
{
    outputString.Reset( String(buffer.Buffer() + start, length ) );
    ALIB_STRINGS_TO_NARROW(outputString, dest, 8192);
    OutputDebugStringA ( dest );
    return outputString.WStringLength();
}


#endif //defined(_WIN32) &&  ALIB_DEBUG
