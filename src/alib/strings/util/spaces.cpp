// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2023 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)
#if !defined(HPP_ALIB_STRINGS_UTIL_SPACES)
    #include "alib/strings/util/spaces.hpp"
#endif
#if !defined (HPP_ALIB_STRINGS_ASTRING)
    #include "alib/strings/astring.hpp"
#endif

#include <ostream>
#endif // !defined(ALIB_DOX)

namespace aworx { namespace lib { namespace strings { namespace util  {


//! @cond NO_DOX
namespace
{
    NAString      nSpaces;
    WAString      wSpaces;
}
//! @endcond

NString& Spaces::GetN( integer minLength )
{
    integer spacesLength= nSpaces.Length();
    if ( spacesLength < minLength )
        nSpaces.InsertChars( ' ', minLength - spacesLength );
    return nSpaces;
}

WString& Spaces::GetW( integer minLength )
{
    integer spacesLength= wSpaces.Length();
    if ( spacesLength < minLength )
        ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
        wSpaces.InsertChars( ' ', minLength - spacesLength );
        ALIB_WARNINGS_RESTORE
    return wSpaces;
}

void    Spaces::Write( std::basic_ostream<char>& os, integer qty )
{
    if( nSpaces.Length() < 128 )
        GetN( 128 );

    while ( qty > 0 )
    {
        integer size= qty < nSpaces.Length() ? qty : nSpaces.Length();
        os.write( nSpaces.Buffer(), size );
        qty-= size;
    }
}

#if ALIB_CHARACTERS_NATIVE_WCHAR
void    Spaces::Write( std::basic_ostream<wchar_t>& os, integer qty )
{
    if( wSpaces.Length() < 128 )
        GetW( 128 );

    while ( qty > 0 )
    {
        integer size= qty < wSpaces.Length() ? qty : wSpaces.Length();
        os.write( wSpaces.Buffer(), size );
        qty-= size;
    }
}
#endif

}}}} // namespace [aworx::lib::strings::util]

