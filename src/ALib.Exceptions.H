//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef H_ALIB_EXCEPTIONS
#define H_ALIB_EXCEPTIONS
#pragma once
#ifndef INL_ALIB
#   include "alib/alib.inl"
#endif

#if ALIB_EXCEPTIONS
#   if ALIB_C20_MODULES && !DOXYGEN
        import ALib.Exceptions;
#   elif !defined(ALIB_INC_EXCEPTIONS_EXCEPTIONS_MPP)
#       define ALIB_INC_EXCEPTIONS_EXCEPTIONS_MPP
#       include "alib/exceptions/exceptions.mpp"
#   endif
#endif

#endif // H_ALIB_EXCEPTIONS
