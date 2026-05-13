//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
/// With supporting legacy or module builds, .mpp-files are either recognized by the build-system
/// as C++20-Module interface files, or are included by the
/// #"alib_manual_modules_impludes;import/include headers".
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
module;
//========================================= Global Fragment ========================================
#include "alib/alib.prepro.hpp"

#if !ALIB_SINGLE_THREADED
#   include <mutex>
#   include <shared_mutex>
#   include <future>
#   include <condition_variable>
#endif

//============================================== Module ============================================
    /// This is a <em><b>C++ Module</b></em> of the \aliblong.
    /// Due to the dual-compile option (either as C++20-Modules or using legacy C++ inclusion),
    /// the C++20-Module names are not of further interest or use.<br>
    /// In general, the names equal the names of the header files listed in the chapter
    /// #"alib_manual_modules_impludes" of the \alib User Manual.
    ///
    /// @see The documentation of the <em><b>"ALib Module"</b></em> given with the corresponding
    ///      Programmer's Manual \alib_threads.
    export module ALib.Threads;
    import        ALib.Lang;
    import        ALib.Time;

//============================================= Exports ============================================
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
