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
#include "alib/boxing/boxing.prepro.hpp"

#include <cstring>
#include <functional>
#include <vector>
#include <typeindex>
#include <array>
#if ALIB_DEBUG && (!ALIB_MONOMEM || !ALIB_CONTAINERS)
#   include <unordered_set>
#   include <unordered_map>
#endif

#if ALIB_DEBUG_BOXING
#   include <tuple>
#endif

//============================================== Module ============================================
    /// This is a <em><b>C++ Module</b></em> of the \aliblong.
    /// Due to the dual-compile option (either as C++20-Modules or using legacy C++ inclusion),
    /// the C++20-Module names are not of further interest or use.<br>
    /// In general, the names equal the names of the header files listed in the chapter
    /// #"alib_manual_modules_impludes" of the \alib User Manual.
    ///
    /// @see The documentation of the <em><b>"ALib Module"</b></em> given with the corresponding
    ///      Programmer's Manual \alib_boxing.
    export module ALib.Boxing.StdFunctors;
    import        ALib.Boxing;


//============================================= Exports ============================================
#include "alib/boxing/stdfunctors.hpp"
