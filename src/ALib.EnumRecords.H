//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef H_ALIB_ENUMRECORDS
#define H_ALIB_ENUMRECORDS
#pragma once

#ifndef INL_ALIB
#   include "alib/alib.inl"
#endif

#include "alib/enumrecords/enumrecords.prepro.hpp"

#if ALIB_ENUMRECORDS
#   if ALIB_C20_MODULES && !DOXYGEN
        import ALib.EnumRecords;
#   elif !defined(ALIB_INC_ENUMRECORDS_MPP)
#       define ALIB_INC_ENUMRECORDS_MPP
#       include "alib/enumrecords/enumrecords.mpp"
#   endif
#endif

#endif // H_ALIB_ENUMRECORDS

