//==================================================================================================
/// \file
/// This header-file is part of the \alib_enumrecords. It does not belong to an \alibmod and is
/// included in any \alibbuild.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================

// CodeMarker_CommonEnums
ALIB_ENUMS_ASSIGN_RECORD( alib::lang::Alignment         , alib::enumrecords::ERSerializable )
ALIB_ENUMS_ASSIGN_RECORD( alib::lang::Bool              , alib::enumrecords::ERSerializable )
ALIB_ENUMS_ASSIGN_RECORD( alib::lang::Caching           , alib::enumrecords::ERSerializable )
DOX_MARKER( [DOX_ENUMS_MAKE_PARSABLE_2] )
ALIB_ENUMS_ASSIGN_RECORD( alib::lang::Case, alib::enumrecords::ERSerializable )
DOX_MARKER( [DOX_ENUMS_MAKE_PARSABLE_2] )
ALIB_ENUMS_ASSIGN_RECORD( alib::lang::ContainerOp       , alib::enumrecords::ERSerializable )
ALIB_ENUMS_ASSIGN_RECORD( alib::lang::CreateDefaults    , alib::enumrecords::ERSerializable )
ALIB_ENUMS_ASSIGN_RECORD( alib::lang::CreateIfNotExists , alib::enumrecords::ERSerializable )
ALIB_ENUMS_ASSIGN_RECORD( alib::lang::CurrentData       , alib::enumrecords::ERSerializable )
ALIB_ENUMS_ASSIGN_RECORD( alib::lang::Inclusion         , alib::enumrecords::ERSerializable )
ALIB_ENUMS_ASSIGN_RECORD( alib::lang::Initialization    , alib::enumrecords::ERSerializable )
ALIB_ENUMS_ASSIGN_RECORD( alib::lang::LineFeeds         , alib::enumrecords::ERSerializable )
ALIB_ENUMS_ASSIGN_RECORD( alib::lang::Phase             , alib::enumrecords::ERSerializable )
ALIB_ENUMS_ASSIGN_RECORD( alib::lang::Propagation       , alib::enumrecords::ERSerializable )
ALIB_ENUMS_ASSIGN_RECORD( alib::lang::Reach             , alib::enumrecords::ERSerializable )
ALIB_ENUMS_ASSIGN_RECORD( alib::lang::Recursive         , alib::enumrecords::ERSerializable )
ALIB_ENUMS_ASSIGN_RECORD( alib::lang::Responsibility    , alib::enumrecords::ERSerializable )
ALIB_ENUMS_ASSIGN_RECORD( alib::lang::Safeness          , alib::enumrecords::ERSerializable )
ALIB_ENUMS_ASSIGN_RECORD( alib::lang::Side              , alib::enumrecords::ERSerializable )
ALIB_ENUMS_ASSIGN_RECORD( alib::lang::SortOrder         , alib::enumrecords::ERSerializable )
ALIB_ENUMS_ASSIGN_RECORD( alib::lang::SourceData        , alib::enumrecords::ERSerializable )
ALIB_ENUMS_ASSIGN_RECORD( alib::lang::Switch            , alib::enumrecords::ERSerializable )
ALIB_ENUMS_ASSIGN_RECORD( alib::lang::Timezone          , alib::enumrecords::ERSerializable )
ALIB_ENUMS_ASSIGN_RECORD( alib::lang::Timing            , alib::enumrecords::ERSerializable )
ALIB_ENUMS_ASSIGN_RECORD( alib::lang::ValueReference    , alib::enumrecords::ERSerializable )
ALIB_ENUMS_ASSIGN_RECORD( alib::lang::Whitespaces       , alib::enumrecords::ERSerializable )

#if !ALIB_SINGLE_THREADED
  ALIB_ENUMS_ASSIGN_RECORD(alib::threads::Thread::State, alib::enumrecords::ERSerializable )
#endif
