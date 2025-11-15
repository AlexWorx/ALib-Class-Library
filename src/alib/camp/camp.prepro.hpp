//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_CAMP_PP
#define HPP_ALIB_CAMP_PP
#pragma once

#ifndef INL_ALIB
#   include "alib/alib.inl"
#endif
#if ALIB_CAMP

//##################################################################################################
// Symbols introduced by module ALib.Camp
//##################################################################################################
#if ALIB_CAMP
#   define ALIB_RESOURCED_IN_MODULE( T, Camp, ResName )                                            \
        ALIB_RESOURCED( T, Camp.GetResourcePool().Get(), Camp.ResourceCategory, ResName  )

#   if !defined(ALIB_CAMP_OMIT_DEFAULT_RESOURCES)
#      define   ALIB_CAMP_OMIT_DEFAULT_RESOURCES   0
#   endif
#endif



#endif
#endif // HPP_ALIB_CAMP_PP
