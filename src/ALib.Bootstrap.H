//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef H_ALIB_BOOTSTRAP
#define H_ALIB_BOOTSTRAP
#pragma once
#ifndef INL_ALIB
#   include "alib/alib.inl"
#endif

#include "alib/bootstrap/bootstrap.prepro.hpp"

#if ALIB_C20_MODULES && !DOXYGEN
    import ALib.Bootstrap;
#elif !defined(ALIB_INC_BOOTSTRAP_BOOTSTRAP_MPP)
#   define ALIB_INC_BOOTSTRAP_BOOTSTRAP_MPP
#   include "alib/bootstrap/bootstrap.mpp"
#endif

#endif // H_ALIB_BOOTSTRAP

