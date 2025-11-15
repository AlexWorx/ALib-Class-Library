//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef H_ALIB_MONOMEM_SHAREDMONOVAL
#define H_ALIB_MONOMEM_SHAREDMONOVAL
#pragma once
#ifndef INL_ALIB
#   include "alib/alib.inl"
#endif

#include "alib/monomem/monomem.prepro.hpp"

#if ALIB_MONOMEM
#   if ALIB_C20_MODULES && !DOXYGEN
        import ALib.Monomem.SharedMonoVal;
#   elif !defined(ALIB_INC_MONOMEM_SHAREDMONOVAL_MPP)
#       define ALIB_INC_MONOMEM_SHAREDMONOVAL_MPP
#       include "alib/monomem/sharedmonoval.mpp"
#   endif
#endif

#endif // H_ALIB_MONOMEM_SHAREDMONOVAL
