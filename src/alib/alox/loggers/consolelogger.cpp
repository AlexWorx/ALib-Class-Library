using namespace alib;

bool ConsoleLogger::notifyPlainTextLogOp( lang::Phase phase ) {
    if ( phase == lang::Phase::Begin )
        writer.Construct(std::cout, ma);
    if ( phase == lang::Phase::End ) {
        writer->Write( NNEW_LINE );
        writer.Destruct();
        ma.Reset();
    }
    return true;
}

integer ConsoleLogger::logPlainTextPart( const String& buffer, integer start, integer length ) {
    integer printedWidth;
    writer->Write( buffer.Substring<NC>( start, length ), &printedWidth );
    return printedWidth;
}
