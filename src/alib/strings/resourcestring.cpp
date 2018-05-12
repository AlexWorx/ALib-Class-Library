// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib.hpp"



#if !defined (HPP_ALIB_STRINGS_RESOURCESTRING)
    #include "alib/strings/resourcestring.hpp"
#endif



namespace aworx { namespace lib {  namespace strings {

AString& ResourceString::CheckLoad()
{
    auto& usAsAString= *dynamic_cast<AString*>(this);
    if( IsNull() )
        usAsAString <<  Lib.Get( Name );

    return usAsAString;
}


}}} // namespace [aworx::lib::strings]
