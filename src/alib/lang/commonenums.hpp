/** ************************************************************************************************
 * \file
 * This header file is part of the \aliblong. It does not belong to an \alibmod and is
 * included in any \alibdist.
 *
 * \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_LANG_COMMONENUMS
#define HPP_ALIB_LANG_COMMONENUMS 1

#if !defined(HPP_ALIB_LANG_COMMONENUMS_DEFS)
#   include "alib/lang/commonenumdefs.hpp"
#endif

#if ALIB_ENUMS

#if !defined (HPP_ALIB_ENUMS_BITWISE)
#   include "alib/enums/bitwise.hpp"
#endif
ALIB_ENUMS_MAKE_BITWISE( alib::lang::Phase )

#if !defined(HPP_ALIB_ENUMS_RECORDS)
#   include "alib/enums/records.hpp"
#endif

// CodeMarker_CommonEnums
ALIB_ENUMS_ASSIGN_RECORD( alib::lang::Alignment         , alib::enums::ERSerializable )
ALIB_ENUMS_ASSIGN_RECORD( alib::lang::Bool              , alib::enums::ERSerializable )
ALIB_ENUMS_ASSIGN_RECORD( alib::lang::Caching           , alib::enums::ERSerializable )
DOX_MARKER( [DOX_ALIB_ENUMS_MAKE_PARSABLE_2] )
ALIB_ENUMS_ASSIGN_RECORD( alib::lang::Case, alib::enums::ERSerializable )
DOX_MARKER( [DOX_ALIB_ENUMS_MAKE_PARSABLE_2] )
ALIB_ENUMS_ASSIGN_RECORD( alib::lang::ContainerOp       , alib::enums::ERSerializable )
ALIB_ENUMS_ASSIGN_RECORD( alib::lang::CreateDefaults    , alib::enums::ERSerializable )
ALIB_ENUMS_ASSIGN_RECORD( alib::lang::CreateIfNotExists , alib::enums::ERSerializable )
ALIB_ENUMS_ASSIGN_RECORD( alib::lang::CurrentData       , alib::enums::ERSerializable )
ALIB_ENUMS_ASSIGN_RECORD( alib::lang::Inclusion         , alib::enums::ERSerializable )
ALIB_ENUMS_ASSIGN_RECORD( alib::lang::Initialization    , alib::enums::ERSerializable )
ALIB_ENUMS_ASSIGN_RECORD( alib::lang::Phase             , alib::enums::ERSerializable )
ALIB_ENUMS_ASSIGN_RECORD( alib::lang::Propagation       , alib::enums::ERSerializable )
ALIB_ENUMS_ASSIGN_RECORD( alib::lang::Reach             , alib::enums::ERSerializable )
ALIB_ENUMS_ASSIGN_RECORD( alib::lang::Recursive         , alib::enums::ERSerializable )
ALIB_ENUMS_ASSIGN_RECORD( alib::lang::Responsibility    , alib::enums::ERSerializable )
ALIB_ENUMS_ASSIGN_RECORD( alib::lang::Safeness          , alib::enums::ERSerializable )
ALIB_ENUMS_ASSIGN_RECORD( alib::lang::Side              , alib::enums::ERSerializable )
ALIB_ENUMS_ASSIGN_RECORD( alib::lang::SortOrder         , alib::enums::ERSerializable )
ALIB_ENUMS_ASSIGN_RECORD( alib::lang::SourceData        , alib::enums::ERSerializable )
ALIB_ENUMS_ASSIGN_RECORD( alib::lang::Switch            , alib::enums::ERSerializable )
ALIB_ENUMS_ASSIGN_RECORD( alib::lang::Timezone          , alib::enums::ERSerializable )
ALIB_ENUMS_ASSIGN_RECORD( alib::lang::Timing            , alib::enums::ERSerializable )
ALIB_ENUMS_ASSIGN_RECORD( alib::lang::ValueReference    , alib::enums::ERSerializable )
ALIB_ENUMS_ASSIGN_RECORD( alib::lang::Whitespaces       , alib::enums::ERSerializable )

#endif // ALIB_ENUMS

#endif // HPP_ALIB_LANG_COMMONENUMS
