// #################################################################################################
//  aworx::lib::lox::loggers - ALox Logging Library
//
//  Copyright 2013-2023 A-Worx GmbH, Germany
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
#   if !defined (HPP_ALIB_FS_MODULES_DISTRIBUTION)
#      include "alib/lib/fs_modules/distribution.hpp"
#   endif
#endif // !defined(ALIB_DOX)


#if !defined(_IOSTREAM_)
    #include <iostream>
#endif
#if !defined (_STRING_H) && !defined(_INC_STRING)
    #include <string.h>
#endif

using namespace aworx;
using namespace aworx::lib::lox;
using namespace aworx::lib::lox::detail;

// #################################################################################################
// Constructor/Destructor
// #################################################################################################

VStudioLogger::VStudioLogger( const NString&  name )
: PlainTextLogger( name, "VSTUDIO_CONSOLE", false )
{
    ALIB_ASSERT_ERROR ( ALIB.IsDebuggerPresent(), "ALOX", "This is not a debug session within Visual Studio" )

    // prevent cutting off filenames
    MetaInfo->Format.SearchAndReplace( A_CHAR("%SF:%SL:"), A_CHAR("%SP\\%SF(%SL):") );
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
    outputString.Reset( String(buffer.Buffer() + start, length ) );
    ALIB_STRINGS_TO_NARROW(outputString, dest, 8192);
    OutputDebugStringA ( dest );
    return outputString.WStringLength();
}


#endif //defined(_WIN32) &&  ALIB_DEBUG
