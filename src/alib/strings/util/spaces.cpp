// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib.hpp"

#include "alib/strings/util/spaces.hpp"

#include <iostream>


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
        wSpaces.InsertChars( ' ', minLength - spacesLength );
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


}}}} // namespace [aworx::lib::strings::util]

