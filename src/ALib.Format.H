//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef H_ALIB_FORMAT
#define H_ALIB_FORMAT
#pragma once
#ifndef INL_ALIB
#   include "alib/alib.inl"
#endif

#if ALIB_FORMAT
#   if ALIB_C20_MODULES && !DOXYGEN
        import ALib.Format;
#   elif !defined(ALIB_INC_FORMAT_FORMAT_MPP)
#       define ALIB_INC_FORMAT_FORMAT_MPP
#       include "alib/format/format.mpp"
#   endif
#endif

#endif // H_ALIB_FORMAT
