//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
#ifndef H_ALIB_THREADS
#define H_ALIB_THREADS
#pragma once
#ifndef HPP_ALIB
#   include "alib/alib.prepro.hpp"
#endif

//=========================================== Dependencies =========================================
#include "ALib.Time.H"

#if !ALIB_SINGLE_THREADED
#include <mutex>
#include <shared_mutex>
#include <future>
#include <condition_variable>
#endif

//============================================= Imports ============================================
#if ALIB_C20_MODULES
import ALib.Threads;
#else
#include "alib/threads/thread.hpp"

#if !ALIB_SINGLE_THREADED
#include "alib/threads/dbgasserters.hpp"
#include "alib/threads/lock.hpp"
#include "alib/threads/recursivelock.hpp"
#include "alib/threads/recursivetimedlock.hpp"
#include "alib/threads/sharedlock.hpp"
#include "alib/threads/sharedtimedlock.hpp"
#include "alib/threads/timedlock.hpp"

#include "alib/threads/condition.hpp"
#include "alib/threads/promise.hpp"
#endif
#endif // ALIB_C20_MODULES
#endif // H_ALIB_THREADS
