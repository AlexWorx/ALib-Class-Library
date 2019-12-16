/** ************************************************************************************************
 * \file
 * This header file  is part of file set \alibfs_commonenums of the \aliblong.
 *
 * \emoji :copyright: 2013-2019 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_FS_COMMONENUMS
#define HPP_ALIB_FS_COMMONENUMS 1

#if !defined(HPP_ALIB_FS_COMMONENUMS_DEFS)
#   include "alib/lib/fs_commonenums/commonenumdefs.hpp"
#endif

#if ALIB_ENUMS

#if !defined (HPP_ALIB_ENUMS_BITWISE)
#   include "alib/enums/bitwise.hpp"
#endif
ALIB_ENUMS_MAKE_BITWISE( aworx::lib::Phase )


#if ALIB_STRINGS

#if !defined(HPP_ALIB_ENUMS_RECORDS)
#   include "alib/enums/records.hpp"
#endif

// CodeMarker_CommonEnums
ALIB_ENUMS_ASSIGN_RECORD( aworx::lib::Bool              , aworx::lib::enums::ERSerializable )
ALIB_ENUMS_ASSIGN_RECORD( aworx::lib::Switch            , aworx::lib::enums::ERSerializable )
DOX_MARKER( [DOX_ALIB_ENUMS_MAKE_PARSABLE_2] )
ALIB_ENUMS_ASSIGN_RECORD( aworx::lib::Case, aworx::lib::enums::ERSerializable )
DOX_MARKER( [DOX_ALIB_ENUMS_MAKE_PARSABLE_2] )
ALIB_ENUMS_ASSIGN_RECORD( aworx::lib::Alignment         , aworx::lib::enums::ERSerializable )
ALIB_ENUMS_ASSIGN_RECORD( aworx::lib::SortOrder         , aworx::lib::enums::ERSerializable )
ALIB_ENUMS_ASSIGN_RECORD( aworx::lib::Inclusion         , aworx::lib::enums::ERSerializable )
ALIB_ENUMS_ASSIGN_RECORD( aworx::lib::Reach             , aworx::lib::enums::ERSerializable )
ALIB_ENUMS_ASSIGN_RECORD( aworx::lib::CurrentData       , aworx::lib::enums::ERSerializable )
ALIB_ENUMS_ASSIGN_RECORD( aworx::lib::SourceData        , aworx::lib::enums::ERSerializable )
ALIB_ENUMS_ASSIGN_RECORD( aworx::lib::Safeness          , aworx::lib::enums::ERSerializable )
ALIB_ENUMS_ASSIGN_RECORD( aworx::lib::Responsibility    , aworx::lib::enums::ERSerializable )
ALIB_ENUMS_ASSIGN_RECORD( aworx::lib::Side              , aworx::lib::enums::ERSerializable )
ALIB_ENUMS_ASSIGN_RECORD( aworx::lib::Timezone          , aworx::lib::enums::ERSerializable )
ALIB_ENUMS_ASSIGN_RECORD( aworx::lib::Whitespaces       , aworx::lib::enums::ERSerializable )
ALIB_ENUMS_ASSIGN_RECORD( aworx::lib::CreateIfNotExists , aworx::lib::enums::ERSerializable )
ALIB_ENUMS_ASSIGN_RECORD( aworx::lib::CreateDefaults    , aworx::lib::enums::ERSerializable )
ALIB_ENUMS_ASSIGN_RECORD( aworx::lib::Propagation       , aworx::lib::enums::ERSerializable )
ALIB_ENUMS_ASSIGN_RECORD( aworx::lib::Phase             , aworx::lib::enums::ERSerializable )
ALIB_ENUMS_ASSIGN_RECORD( aworx::lib::ContainerOp       , aworx::lib::enums::ERSerializable )
ALIB_ENUMS_ASSIGN_RECORD( aworx::lib::Initialization    , aworx::lib::enums::ERSerializable )
ALIB_ENUMS_ASSIGN_RECORD( aworx::lib::Timing            , aworx::lib::enums::ERSerializable )
ALIB_ENUMS_ASSIGN_RECORD( aworx::lib::Caching           , aworx::lib::enums::ERSerializable )

#endif // ALIB_STRINGS
#endif // ALIB_ENUMS

#endif // HPP_ALIB_FS_COMMONENUMS
