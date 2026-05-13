using namespace alib;

TextFileLogger::TextFileLogger( const alib::String&  fileName,
                                const alib::NString& loggerName )
: PlainTextLogger( loggerName, "TEXTFILE" )
, ma( ALIB_DBG("TextFileLogger",) 8 ) {
    FileName << fileName;

    // test open file for test
    ALIB_STRINGS_TO_NARROW(FileName,nFileName,1024)
    auto* os= new std::ofstream( nFileName, std::ios::app );
    if ( !os->is_open() ) {
        LastSystemError= std::errc(errno);
        ALIB_WARNING( "ALOX", "Could not open file: \"{}\". System error code: {}",
                      FileName, LastSystemError )
    }
    else
        // test run OK
        os->close();

    delete os;
}

void TextFileLogger::openFile() {
    ALIB_STRINGS_TO_NARROW(FileName,nFileName,1024)
    ofs= new std::ofstream( nFileName, std::ios::app );
    if ( !ofs->is_open() ) {
        LastSystemError= std::errc(errno);
        ALIB_WARNING( "ALOX", "Could not open file: \"{}\". System error code: {}",
                      FileName, LastSystemError )
        delete ofs;  ofs= nullptr;
        return;
    }

    LastSystemError= std::errc(0);
    writer.Construct( *ofs, ma );
}

void TextFileLogger::closeFile() {
    if( ofs == nullptr )
        return;

    writer.Destruct();
    ofs->close();
    delete ofs;
}


void TextFileLogger::notifyMultiLineOp( lang::Phase phase ) {
    // save state (to have it in logText)
    currentlyInMultiLineOp= (phase == lang::Phase::Begin);

    // open/close the file
    if ( phase == lang::Phase::Begin )
        openFile();
    else if( ofs != nullptr )
        closeFile();
}

bool TextFileLogger::notifyPlainTextLogOp( lang::Phase phase ) {
    if ( ofs != nullptr && phase == lang::Phase::End )
        (*ofs) << std::endl;

    // open/close
    if( !currentlyInMultiLineOp ) {
        if ( phase == lang::Phase::Begin )
            openFile();
        else
            closeFile();
    }

    return LastSystemError == std::errc(0);
}

integer TextFileLogger::logPlainTextPart( const String& buffer, integer start, integer length ) {
    if (ofs == nullptr || LastSystemError != std::errc(0))
        return 0;
    integer printedWidth;
    writer->Write( buffer.Substring<NC>( start, length ), &printedWidth );
    return printedWidth;
}
