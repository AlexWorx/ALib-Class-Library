//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
#ifndef H_ALIB_THREADMODEL
#define H_ALIB_THREADMODEL
#pragma once
#ifndef HPP_ALIB
#   include "alib/alib.prepro.hpp"
#endif

#if ALIB_THREADMODEL
//=========================================== Dependencies =========================================
#include "ALib.Singletons.H"
#include "ALib.Monomem.H"
#include "ALib.Containers.List.H"
#include "ALib.Containers.HashTable.H"
#include "ALib.Strings.H"
#include "ALib.EnumRecords.H"

#include <list>
#include <map>
#include <queue>
//============================================= Imports ============================================
#if ALIB_C20_MODULES
import ALib.ThreadModel;
#else
#include "alib/threadmodel/trigger.hpp"
#include "alib/threadmodel/jobs.hpp"
#include "alib/threadmodel/dedicatedworker.hpp"
#include "alib/threadmodel/threadpool.hpp"

#endif // ALIB_C20_MODULES
#endif // ALIB_THREADMODEL
#endif // H_ALIB_THREADMODEL
