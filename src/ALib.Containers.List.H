//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
#ifndef H_ALIB_CONTAINERS_LIST
#define H_ALIB_CONTAINERS_LIST
#pragma once
#ifndef HPP_ALIB
#   include "alib/alib.prepro.hpp"
#endif

#if ALIB_CONTAINERS
//=========================================== Dependencies =========================================
#include "ALib.Lang.H"
#include "ALib.Monomem.H"
//============================================= Imports ============================================
#if ALIB_C20_MODULES
import ALib.Containers.List;
#else
#ifndef ALIB_GUARD_CONTAINERS_INIT
#    define ALIB_GUARD_CONTAINERS_INIT 1
#include "alib/containers/recycling.hpp"
#include "alib/containers/valuedescriptor.hpp"
#endif
#include "alib/containers/list.hpp"
#endif
#endif // ALIB_C20_MODULES
#endif // H_ALIB_CONTAINERS_LIST
