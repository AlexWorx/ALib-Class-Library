namespace alib {  namespace system {


bool EnvironmentVariables::Get( const CString&      name,
                                AString&            target,
                                lang::CurrentData   targetData ) {
    if ( targetData == lang::CurrentData::Clear )
        target.Reset();

    if ( name.IsEmpty() )
        return false;

    ALIB_STRINGS_TO_NARROW( name, nName, 512 )
    ALIB_ALLOW_UNSAFE_FUNCTION_OR_VARIABLE
    nchar* env= std::getenv( nName );
    ALIB_POP_ALLOWANCE

    if( env != nullptr ) {
        target._( NString( env ) );
        return true;
    }
    return false;
}

}} // namespace [alib::system]
