//##################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
#include "alib_precompile.hpp"
#if !defined(ALIB_C20_MODULES) || ((ALIB_C20_MODULES != 0) && (ALIB_C20_MODULES != 1))
#   error "Symbol ALIB_C20_MODULES has to be given to the compiler as either 0 or 1"
#endif
#if ALIB_C20_MODULES
    module;
#endif
//========================================= Global Fragment ========================================
#include "alib/boxing/boxing.prepro.hpp"
#include "alib/files/files.prepro.hpp"
#if !defined (_WIN32)
#   include <pwd.h>
#   include <grp.h>
#endif
//============================================== Module ============================================
#if ALIB_C20_MODULES
    module ALib.Files;
    import   ALib.Characters.Functions;
    import   ALib.Strings;
    import   ALib.Boxing;
#  if ALIB_EXPRESSIONS
    import   ALib.Expressions;
#  endif
#else
#   include "ALib.Characters.Functions.H"
#   include "ALib.Strings.H"
#   include "ALib.Boxing.H"
#   include "ALib.Expressions.H"
#   include "ALib.Files.H"
#endif
//========================================== Implementation ========================================

using namespace alib::system;
namespace alib::files {

void   FInfo::SetLinkTarget(FTree& tree, const PathString& target, const PathString& realTarget) {
    EISymLinkFile& ei= *static_cast<EISymLinkFile*>(extendedInfo);
    auto& pool= tree.Pool;

    // delete old values
    if(    ei.RealTarget.Buffer() != ei.Target.Buffer()
        && ei.RealTarget.Buffer() != nullptr             )
        ei.RealTarget.Free(pool);
    ei.Target.Free(pool);

    // allocate target
    ei.Target.Allocate(pool, target);

    // real target is same?
    if( realTarget.Equals(target) )
        ei.RealTarget= ei.Target;
    else
        ei.RealTarget.Allocate(pool, realTarget);
}

//==================================================================================================
//=== OwnerAndGroupResolver
//==================================================================================================
#if defined ( _WIN32) && !DOXYGEN
    namespace { NString unknown("<Unknown>"); }
#endif

const NString&  OwnerAndGroupResolver::GetOwnerName( const FInfo& fInfo ) {
    #if !defined ( _WIN32)
        auto key= fInfo.Owner();
        auto resultPair= ownerCache.Try( key );
        if( !resultPair.first ) {
            auto* result= getpwuid(key);
            resultPair.second.Construct( key, NString(result ? result->pw_name : "?"));
        }
        return resultPair.second.Mapped();
    #else
        (void) fInfo;
        return unknown;
    #endif
}

const NString&  OwnerAndGroupResolver::GetGroupName( const FInfo& fInfo ) {
    #if !defined ( _WIN32)
        auto key= fInfo.Group();
        auto resultPair= groupCache.Try( key );
        if( !resultPair.first ) {
            auto* result= getgrgid(key);
            resultPair.second.Construct( key, NString(result ? result->gr_name : "?"));
        }
        return resultPair.second.Mapped();
    #else
        (void) fInfo;
        return unknown;
    #endif
}

} // namespace [alib::files]

ALIB_BOXING_VTABLE_DEFINE( alib::files::FInfo::Permissions         , vt_files_perms  )
ALIB_BOXING_VTABLE_DEFINE( alib::files::FInfo::Types               , vt_files_type   )
ALIB_BOXING_VTABLE_DEFINE( alib::files::FInfo::TypeNames1Letter    , vt_files_type1  )
ALIB_BOXING_VTABLE_DEFINE( alib::files::FInfo::TypeNames2Letters   , vt_files_type2  )
ALIB_BOXING_VTABLE_DEFINE( alib::files::FInfo::TypeNames3Letters   , vt_files_type3  )
ALIB_BOXING_VTABLE_DEFINE( alib::files::FInfo::Qualities           , vt_files_qual   )
ALIB_BOXING_VTABLE_DEFINE( alib::files::FInfo::Qualities3Letters   , vt_files_qual3  )
