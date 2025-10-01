//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef H_ALIB_ENUMS_RECORDBOOTSTRAP
#define H_ALIB_ENUMS_RECORDBOOTSTRAP
#pragma once
#ifndef INL_ALIB
#   include "alib/alib.inl"
#endif

#include "alib/enumrecords/enumrecords.prepro.hpp"

#if ALIB_ENUMRECORDS
#   if ALIB_C20_MODULES && !DOXYGEN
        import ALib.EnumRecords.Bootstrap;
#   elif !defined(ALIB_INC_ENUMS_BOOTSTRAP_RECORDBOOTSTRAP_MPP)
#       define ALIB_INC_ENUMS_BOOTSTRAP_RECORDBOOTSTRAP_MPP
#       include "alib/enumrecords/bootstrap/recordbootstrap.mpp"
#   endif
#endif

#endif // H_ALIB_ENUMS_RECORDBOOTSTRAP


