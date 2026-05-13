//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
#ifndef H_ALIB_SINGLETONS
#define H_ALIB_SINGLETONS
#pragma once
#ifndef HPP_ALIB
#   include "alib/alib.prepro.hpp"
#endif

#if ALIB_SINGLETONS
//=========================================== Dependencies =========================================
#include "ALib.Lang.H"

#include "alib/singletons/singletons.prepro.hpp"

#if ALIB_FEAT_SINGLETON_MAPPED
#   if !ALIB_MONOMEM || !ALIB_CONTAINERS
#include <unordered_map>
#   endif
#   if !ALIB_MONOMEM
#include <mutex>
#   endif
#   if ALIB_DEBUG
#       include "ALib.Strings.H"
#   endif
#   if ALIB_MONOMEM
#       include "ALib.Monomem.H"
#       if ALIB_CONTAINERS
#          include "ALib.Containers.HashTable.H"
#       endif
#   endif
#endif

//============================================= Imports ============================================
#if ALIB_C20_MODULES
import ALib.Singletons;
#else
#include "alib/singletons/singletons.hpp"

#endif // ALIB_C20_MODULES
#endif // ALIB_SINGLETONS
#endif // H_ALIB_SINGLETONS
