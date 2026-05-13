//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
#ifndef HPP_ALIB_CAMP_PP
#define HPP_ALIB_CAMP_PP
#pragma once

#ifndef HPP_ALIB
#   include "alib/alib.prepro.hpp"
#endif

//##################################################################################################
// Macro  introduced by module ALib.Camp
//##################################################################################################
#if ALIB_CAMP
#   define ALIB_RESOURCED_IN_CAMP( T, Camp, ResName )                                              \
        ALIB_RESOURCED( T, &Camp.GetResourcePool(), Camp.ResourceCategory, ResName  )

#   define ALIB_CAMP_ENUM( T, TRecord, Camp, ResName )                                             \
        ALIB_ENUMS_ASSIGN_RECORD( T, TRecord  )                                                    \
        ALIB_RESOURCED_IN_CAMP( T, Camp, ResName  )

#   if !defined(ALIB_CAMP_OMIT_DEFAULT_RESOURCES)
#      define   ALIB_CAMP_OMIT_DEFAULT_RESOURCES   0
#   endif

#   if !defined ALIB_CAMP_RESOURCE_COMPILATION
#     if ALIB_DEBUG
        /// This symbol selects the implementation of the class #"DevtimeResourceCompiler" and
        /// the availability of:
        /// - type #"ResourceListEntry",
        /// - type #"ResourceList",
        /// - function #"LoadResourceFile",
        /// - function #"PatchCPPFile", and
        /// - function #"ListErrors".
        ///
        /// With that, it controls whether resources are parsed from the `.alibrc` resource-files
        /// at runtime and - on changes - are written into a C++ file.
        ///
        /// If not pre-defined by the build system, this symbol defaults to:
        /// - `1` in debug builds.
        /// - `0` in non-debug builds.
        ///
#       define ALIB_CAMP_RESOURCE_COMPILATION 1
#     else
#       define ALIB_CAMP_RESOURCE_COMPILATION 0
#     endif
#   endif

#endif
#endif // HPP_ALIB_CAMP_PP
