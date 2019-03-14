// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#ifndef HPP_ALIB_LIB_COMMONENUMS_RESOURCED
#define HPP_ALIB_LIB_COMMONENUMS_RESOURCED 1

#if !defined(HPP_ALIB_LIB_COMMONENUMS)
#   include "alib/lib/commonenums.hpp"
#endif

#if !defined(HPP_ALIB_RESOURCES_ENUM_META_DATA)
#   include "alib/resources/enummetadata.hpp"
#endif

#if !defined(HPP_ALIB_LIB_ALIBMODULES)
#   include "alib/lib/alibmodules.hpp"
#endif

// #################################################################################################
// Enum meta data specializations for built-in ALib enums
// #################################################################################################

DOX_MARKER( [DOX_ALIB_ENUMS_PARSABLE_2] )
ALIB_ENUM_PARSABLE( aworx::lib::Case )
ALIB_RESOURCED_IN_MODULE(aworx::lib::Case, aworx::ALIB, "Case" )
DOX_MARKER( [DOX_ALIB_ENUMS_PARSABLE_2] )

ALIB_ENUM_PARSABLE ( aworx::lib::Bool          )
ALIB_ENUM_PARSABLE ( aworx::lib::Switch        )
ALIB_ENUM_PARSABLE ( aworx::lib::Alignment     )
ALIB_ENUM_PARSABLE ( aworx::lib::SortOrder     )
ALIB_ENUM_PARSABLE ( aworx::lib::Inclusion     )
ALIB_ENUM_PARSABLE ( aworx::lib::Reach         )
ALIB_ENUM_PARSABLE ( aworx::lib::CurrentData   )
ALIB_ENUM_PARSABLE ( aworx::lib::SourceData    )
ALIB_ENUM_PARSABLE ( aworx::lib::Safeness      )
ALIB_ENUM_PARSABLE ( aworx::lib::Responsibility)
ALIB_ENUM_PARSABLE ( aworx::lib::Timezone      )
ALIB_ENUM_PARSABLE ( aworx::lib::Whitespaces   )
ALIB_ENUM_PARSABLE ( aworx::lib::Create        )
ALIB_ENUM_PARSABLE ( aworx::lib::Propagation   )
ALIB_ENUM_PARSABLE ( aworx::lib::Phase         )
ALIB_ENUM_PARSABLE ( aworx::lib::ContainerOp   )
ALIB_ENUM_PARSABLE ( aworx::lib::Initialization)
ALIB_ENUM_PARSABLE ( aworx::lib::Timing        )

ALIB_RESOURCED_IN_MODULE( aworx::lib::Bool          , aworx::ALIB,  "Bool"           )
ALIB_RESOURCED_IN_MODULE( aworx::lib::Switch        , aworx::ALIB,  "Switch"         )
ALIB_RESOURCED_IN_MODULE( aworx::lib::Alignment     , aworx::ALIB,  "Alignment"      )
ALIB_RESOURCED_IN_MODULE( aworx::lib::SortOrder     , aworx::ALIB,  "SortOrder"      )
ALIB_RESOURCED_IN_MODULE( aworx::lib::Inclusion     , aworx::ALIB,  "Inclusion"      )
ALIB_RESOURCED_IN_MODULE( aworx::lib::Reach         , aworx::ALIB,  "Reach"          )
ALIB_RESOURCED_IN_MODULE( aworx::lib::CurrentData   , aworx::ALIB,  "CurrentData"    )
ALIB_RESOURCED_IN_MODULE( aworx::lib::SourceData    , aworx::ALIB,  "SourceData"     )
ALIB_RESOURCED_IN_MODULE( aworx::lib::Safeness      , aworx::ALIB,  "Safeness"       )
ALIB_RESOURCED_IN_MODULE( aworx::lib::Responsibility, aworx::ALIB,  "Responsibility" )
ALIB_RESOURCED_IN_MODULE( aworx::lib::Timezone      , aworx::ALIB,  "Timezone"       )
ALIB_RESOURCED_IN_MODULE( aworx::lib::Whitespaces   , aworx::ALIB,  "Whitespaces"    )
ALIB_RESOURCED_IN_MODULE( aworx::lib::Create        , aworx::ALIB,  "Create"         )
ALIB_RESOURCED_IN_MODULE( aworx::lib::Propagation   , aworx::ALIB,  "Propagation"    )
ALIB_RESOURCED_IN_MODULE( aworx::lib::Phase         , aworx::ALIB,  "Phase"          )
ALIB_RESOURCED_IN_MODULE( aworx::lib::ContainerOp   , aworx::ALIB,  "ContainerOp"    )
ALIB_RESOURCED_IN_MODULE( aworx::lib::Initialization, aworx::ALIB,  "Initialization" )
ALIB_RESOURCED_IN_MODULE( aworx::lib::Timing        , aworx::ALIB,  "Timing"         )


#endif // HPP_ALIB_LIB_COMMONENUMS_RESOURCED
