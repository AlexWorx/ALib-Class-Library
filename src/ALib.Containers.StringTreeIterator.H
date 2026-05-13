//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
#ifndef H_ALIB_CONTAINERS_STRINGTREE_ITERATOR
#define H_ALIB_CONTAINERS_STRINGTREE_ITERATOR
#pragma once
#ifndef HPP_ALIB
#   include "alib/alib.prepro.hpp"
#endif

#if ALIB_CONTAINERS && ALIB_STRINGS
//=========================================== Dependencies =========================================
#include "ALib.Lang.H"
#include "ALib.Containers.StringTree.H"

//============================================= Imports ============================================
#if ALIB_C20_MODULES
import ALib.Containers.StringTreeIterator;
#else
#   ifndef ALIB_GUARD_CONTAINERS_INIT
#       define ALIB_GUARD_CONTAINERS_INIT 1
#       include "alib/containers/recycling.hpp"
#       include "alib/containers/valuedescriptor.hpp"
#   endif

#if ALIB_STRINGS
#include "alib/containers/stringtreeiterator.hpp"
#endif

#endif // ALIB_C20_MODULES
#endif // ALIB_CONTAINERS && ALIB_STRINGS
#endif // H_ALIB_CONTAINERS_STRINGTREE_ITERATOR
