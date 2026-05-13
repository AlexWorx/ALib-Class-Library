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
