//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef H_ALIB_CONTAINERS_LIST
#define H_ALIB_CONTAINERS_LIST
#pragma once
#ifndef INL_ALIB
#   include "alib/alib.inl"
#endif

#include "alib/containers/containers.prepro.hpp"

#if ALIB_CONTAINERS
#   if ALIB_C20_MODULES && !DOXYGEN
        import ALib.Containers.List;
#   elif !defined(ALIB_INC_CONTAINERS_LIST_MPP)
#       define ALIB_INC_CONTAINERS_LIST_MPP
#       include "alib/containers/list.mpp"
#   endif
#endif

#endif // H_ALIB_CONTAINERS_LIST
