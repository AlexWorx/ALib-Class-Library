// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#include "alib/files/finfo.hpp"

#include "alib/files/ftools.hpp"

#if !defined (_WIN32)
#   include <pwd.h>
#   include <grp.h>
#endif

namespace alib::files {

//==================================================================================================
//=== OwnerAndGroupResolver
//==================================================================================================
std::pair<String,String> OwnerAndGroupResolver::Get( const FInfo& entry )
{
#if !defined ( _WIN32)
    auto owner= entry.Owner();
    if( lastUID != owner  )
    {
        lastUID= owner;
        auto* ownerPW= getpwuid(owner);
        lastUIDResult.Reset(String(ownerPW ? ownerPW->pw_name : "?"));
    }

    auto  group= entry.Group();
    if( lastGID != group )
    {
        lastGID= group;
        auto* groupPW= getgrgid(group);
        lastGIDResult.Reset(String(groupPW ? groupPW->gr_name : "?") );
    }

    return std::pair<String,String>( lastUIDResult, lastGIDResult );

#else
    (void) entry;
    {
        return std::pair<String, String>(A_CHAR("<Unknown>"), A_CHAR("<Unknown>"));
    }
#endif

}


} // namespace [alib::files]
