//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
#ifndef H_ALIB_CONTAINERS_SHAREDPTR
#define H_ALIB_CONTAINERS_SHAREDPTR
#pragma once
#ifndef HPP_ALIB
#   include "alib/alib.prepro.hpp"
#endif

#if ALIB_CONTAINERS
//=========================================== Dependencies =========================================
#include "ALib.Lang.H"
//============================================= Imports ============================================
#if ALIB_C20_MODULES
import ALib.Containers.SharedPtr;
#else
#include "alib/containers/sharedptr.hpp"

#endif // ALIB_C20_MODULES
#endif // ALIB_CONTAINERS
#endif // H_ALIB_CONTAINERS_SHAREDPTR
