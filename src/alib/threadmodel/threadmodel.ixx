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
#include <list>
#include <map>
#include <queue>
#include "alib/enumops/enumops.prepro.hpp"
#include "alib/enumrecords/enumrecords.prepro.hpp"
//============================================== Module ============================================
    /// This is a <em><b>C++ Module</b></em> of the \aliblong.
    /// Due to the dual-compile option (either as C++20-Modules or using legacy C++ inclusion),
    /// the C++20-Module names are not of further interest or use.<br>
    /// In general, the names equal the names of the header files listed in the chapter
    /// #"alib_manual_modules_impludes" of the \alib User Manual.
    ///
    /// @see The documentation of the <em><b>"ALib Module"</b></em> given with the corresponding
    ///      Programmer's Manual \alib_threadmodel.
    export module ALib.ThreadModel;
    import        ALib.Lang;
    import        ALib.Time;
    import        ALib.Threads;
    import        ALib.Singletons;
#  if ALIB_STRINGS
    import        ALib.Strings;
#  endif
#  if ALIB_MONOMEM
    import        ALib.Monomem;
#  endif
    import        ALib.Containers.List;
#  if ALIB_CONTAINERS
    import        ALib.Containers.HashTable;
#  endif
#  if ALIB_ENUMRECORDS
    import        ALib.EnumRecords;
#  endif

//============================================= Exports ============================================
#include "alib/threadmodel/trigger.hpp"
#include "alib/threadmodel/jobs.hpp"
#include "alib/threadmodel/dedicatedworker.hpp"
#include "alib/threadmodel/threadpool.hpp"
