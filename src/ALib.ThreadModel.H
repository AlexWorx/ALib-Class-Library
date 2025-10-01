//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef H_ALIB_THREADMODEL
#define H_ALIB_THREADMODEL
#pragma once
#ifndef INL_ALIB
#   include "alib/alib.inl"
#endif

#if ALIB_THREADMODEL
#   if ALIB_C20_MODULES && !DOXYGEN
        import ALib.ThreadModel;
#   elif !defined(ALIB_INC_THREADMODEL_THREADMODEL_MPP)
#       define ALIB_INC_THREADMODEL_THREADMODEL_MPP
#       include "alib/threadmodel/threadmodel.mpp"
#   endif
#endif

#endif // H_ALIB_THREADMODEL


