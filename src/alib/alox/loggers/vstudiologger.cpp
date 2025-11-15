//##################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
#include "alib_precompile.hpp"
#if !defined(ALIB_C20_MODULES) || ((ALIB_C20_MODULES != 0) && (ALIB_C20_MODULES != 1))
#   error "Symbol ALIB_C20_MODULES has to be given to the compiler as either 0 or 1"
#endif
#if ALIB_C20_MODULES
    module;
#endif
//========================================= Global Fragment ========================================
#include "alib/alox/alox.prepro.hpp"
#include <iostream>
#include <string.h>
//============================================== Module ============================================
#if ALIB_C20_MODULES
    module ALib.ALox.Impl;
    import   ALib.Lang;
    import   ALib.Strings;
    import   ALib.Camp.Base;
#else
#   include "ALib.Lang.H"
#   include "ALib.Strings.H"
#   include "ALib.ALox.H"
#   include "ALib.ALox.Impl.H"
#   include "ALib.Camp.Base.H"
#endif
//========================================== Implementation ========================================
#if defined(_MSC_VER) &&  ALIB_DEBUG

using namespace alib;
using namespace alib::lox;
using namespace alib::lox::detail;

//##################################################################################################
// Constructor/Destructor
//##################################################################################################

VStudioLogger::VStudioLogger( const NString&  name )
: PlainTextLogger( name, "VSTUDIO_CONSOLE" )
{
    ALIB_ASSERT_ERROR ( BASECAMP.IsDebuggerPresent(), "ALOX",
        "This is not a debug session within Visual Studio" )
}

void VStudioLogger::AcknowledgeLox( detail::LoxImpl* lox, lang::ContainerOp op ) {
    TextLogger::AcknowledgeLox( lox, op );
    if( op != lang::ContainerOp::Insert )
        return;

    // prevent cutting off filenames
    auto& fmt= GetFormatMetaInfo();
    fmt.Format.SearchAndReplace( A_CHAR("%SF:%SL:"), A_CHAR("%SP\\%SF(%SL):") );
}

bool VStudioLogger::notifyPlainTextLogOp(lang::Phase phase)
{
    if ( phase == lang::Phase::End )
        OutputDebugStringA( "\r\n" );
    return true;
}

integer VStudioLogger::logPlainTextPart( const String& buffer, integer start, integer length )
{
    outputString.Reset( String(buffer.Buffer() + start, length ) );
    ALIB_STRINGS_TO_NARROW(outputString, dest, 8192);
    OutputDebugStringA ( dest );
    return outputString.WStringLength();
}


#endif //defined(_MSC_VER) &&  ALIB_DEBUG
