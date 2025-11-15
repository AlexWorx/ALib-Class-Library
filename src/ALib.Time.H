//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef H_ALIB_TIME
#define H_ALIB_TIME
#pragma once
#ifndef INL_ALIB
#   include "alib/alib.inl"
#endif

#if ALIB_C20_MODULES && !DOXYGEN
    import ALib.Time;
#elif !defined(ALIB_INC_TIME_MPP)
#   define ALIB_INC_TIME_MPP
#   include "alib/time/time.mpp"
#endif

#endif // H_ALIB_TIME
