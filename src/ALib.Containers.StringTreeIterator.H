//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef H_ALIB_CONTAINERS_STRINGTREE_ITERATOR
#define H_ALIB_CONTAINERS_STRINGTREE_ITERATOR
#pragma once
#ifndef INL_ALIB
#   include "alib/alib.inl"
#endif

#include "alib/containers/containers.prepro.hpp"

#if ALIB_CONTAINERS && ALIB_STRINGS
#   if ALIB_C20_MODULES && !DOXYGEN
        import ALib.Containers.StringTree;
#   elif !defined(ALIB_INC_CONTAINERS_STRINGTREE_ITERATOR_MPP)
#       define    ALIB_INC_CONTAINERS_STRINGTREE_ITERATOR_MPP
#       include "alib/containers/stringtreeiterator.mpp"
#   endif
#endif

#endif // H_ALIB_CONTAINERS_STRINGTREE_ITERATOR
