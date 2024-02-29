// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#include "alib/files/finfo.hpp"

#if ALIB_DEBUG
#   if !defined(HPP_ALIB_LANG_FORMAT_FORMATTER)
#      include "alib/lang/format/formatter.hpp"
#   endif
#endif

namespace alib::files {

//==================================================================================================
//=== FInfo::WriteTypeAndAccess
//==================================================================================================
AString&  FInfo::WriteTypeAndAccess(AString& target)                                    const
{
    auto type = Type();
    auto perms= Perms();
    char typeChar= '?';
    ALIB_WARNINGS_ALLOW_SPARSE_ENUM_SWITCH
    switch( type )
    {
        case Types::REGULAR          : typeChar= '-'; break;
        case Types::DIRECTORY        : typeChar= 'd'; break;
        case Types::SYMBOLIC_LINK    : typeChar= 'l'; break;
        case Types::SYMBOLIC_LINK_DIR: typeChar= 'L'; break;
        case Types::BLOCK            : typeChar= 'b'; break;
        case Types::CHARACTER        : typeChar= 'c'; break;
        case Types::FIFO             : typeChar= 'p'; break;
        case Types::SOCKET           : typeChar= 'S'; break;
    }
    ALIB_WARNINGS_RESTORE
    target << typeChar
           << (  (perms & Permissions::OWNER_READ  ) == Permissions::OWNER_READ    ? 'r'   : '-' )
           << (  (perms & Permissions::OWNER_WRITE ) == Permissions::OWNER_WRITE   ? 'w'   : '-' )
           << (  (perms & Permissions::OWNER_EXEC  ) == Permissions::OWNER_EXEC    ? 'x'   : '-' )
           << (  (perms & Permissions::GROUP_READ  ) == Permissions::GROUP_READ    ? 'r'   : '-' )
           << (  (perms & Permissions::GROUP_WRITE ) == Permissions::GROUP_WRITE   ? 'w'   : '-' )
           << (  (perms & Permissions::GROUP_EXEC  ) == Permissions::GROUP_EXEC    ? 'x'   : '-' )
           << (  (perms & Permissions::OTHERS_READ ) == Permissions::OTHERS_READ   ? 'r'   : '-' )
           << (  (perms & Permissions::OTHERS_WRITE) == Permissions::OTHERS_WRITE  ? 'w'   : '-' )
           << (  (perms & Permissions::OTHERS_EXEC ) == Permissions::OTHERS_EXEC   ? 'x'   : '-' )
    ;
    return target;
}


void   FInfo::SetLinkTarget(const String& target, const String& realTarget)
{
    EISymLinkFile& ei= *static_cast<EISymLinkFile*>(extendedInfo);

    ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
    // delete old?
    if(ei.    Target.IsNotNull() && ei.Target.Buffer() != ei.internalBuf  )
        delete[] ei.Target.Buffer();
    if(ei.RealTarget.IsNotNull() && ei.RealTarget.Buffer() >= ei.internalBuf
                                 && ei.RealTarget.Buffer() <  ei.internalBuf + ei.InternalBufSize  )
        delete[] ei.RealTarget.Buffer();


    // copy target to buffer or external memory
    integer    tLen= target.Length();
    character* tp  = (tLen +1 < EISymLinkFile::InternalBufSize ) ?  ei.internalBuf
                                                                 :  new character[size_t(tLen+1)];
    target.CopyTo( tp );
    tp[tLen]= '\0';
    ei.Target= CString( tp, tLen );

    // real target is same?
    if( realTarget.Equals(target) )
    {
        ei.RealTarget= ei.Target;
        return;
    }

    // copy realTarget to remaining buffer or external memory
    integer bufStart= (ei.internalBuf == ei.Target.Buffer()) ? tLen + 1
                                                             : 0;
    tLen=  realTarget.Length();
    tp= (tLen +1 < EISymLinkFile::InternalBufSize - bufStart) ?  ei.internalBuf + bufStart
                                                              :  new character[size_t(tLen+1)];
    realTarget.CopyTo( tp );
    tp[tLen]= '\0';
    ei.RealTarget= CString( tp, tLen );
    ALIB_WARNINGS_RESTORE
}

} // namespace [alib::files]
