//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef H_ALIB_THREADS
#define H_ALIB_THREADS
#pragma once
#ifndef INL_ALIB
#   include "alib/alib.inl"
#endif

#if ALIB_C20_MODULES && !DOXYGEN
    import ALib.Threads;
#elif !defined(ALIB_INC_THREADS_MPP)
#   define ALIB_INC_THREADS_MPP
#   include "alib/threads/threads.mpp"
#endif

#endif // H_ALIB_THREADS
