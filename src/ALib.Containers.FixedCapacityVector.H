//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef H_ALIB_CONTAINERS_FIXEDCAPACITYVECTOR
#define H_ALIB_CONTAINERS_FIXEDCAPACITYVECTOR
#pragma once
#ifndef INL_ALIB
#   include "alib/alib.inl"
#endif

#include "alib/containers/containers.prepro.hpp"

#if ALIB_CONTAINERS
#   if ALIB_C20_MODULES && !DOXYGEN
        import ALib.Containers.FixedCapacityVector;
#   elif !defined(ALIB_INC_CONTAINERS_FIXEDCAPACITYVECTOR_MPP)
#       define ALIB_INC_CONTAINERS_FIXEDCAPACITYVECTOR_MPP
#       include "alib/containers/fixedcapacityvector.mpp"
#   endif
#endif

#endif // H_ALIB_CONTAINERS_FIXEDCAPACITYVECTOR
