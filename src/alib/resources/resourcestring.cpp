// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined (HPP_ALIB_RESOURCES_RESOURCESTRING)
#   include "alib/resources/resourcestring.hpp"
#endif

namespace aworx { namespace lib { namespace resources {

AString& ResourceString::CheckLoad()
{
    auto& usAsAString= *dynamic_cast<AString*>(this);
    if( IsNull() )
        usAsAString <<  ResourceModule.GetResource( Name );

    return usAsAString;
}


}}} // namespace [aworx::lib::resources]
