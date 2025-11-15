//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef H_ALIB_ENUMOPS
#define H_ALIB_ENUMOPS
#pragma once

#ifndef INL_ALIB
#   include "alib/alib.inl"
#endif

#include "alib/enumops/enumops.prepro.hpp"

#if ALIB_C20_MODULES && !DOXYGEN
     import ALib.EnumOps;
#elif !defined(ALIB_INC_ENUMOPS_MPP)
#    define ALIB_INC_ENUMOPS_MPP
#    include "alib/enumops/enumops.mpp"
#endif

#endif // H_ALIB_ENUMOPS
