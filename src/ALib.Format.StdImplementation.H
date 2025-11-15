//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef H_ALIB_FORMAT_STDIMPLEMENTATION
#define H_ALIB_FORMAT_STDIMPLEMENTATION
#pragma once
#ifndef INL_ALIB
#   include "alib/alib.inl"
#endif

#if ALIB_FORMAT
#   if ALIB_C20_MODULES && !DOXYGEN
        import ALib.Format.StdImplementation;
#   elif !defined(ALIB_INC_FORMAT_FORMATTERSTDIMPL_MPP)
#       define ALIB_INC_FORMAT_FORMATTERSTDIMPL_MPP
#       include "alib/format/formatterstdimpl.mpp"
#   endif
#endif

#endif // H_ALIB_FORMAT_STDIMPLEMENTATION
