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

#include <iterator>
#include <cmath>
#include <algorithm>

#if defined( _WIN32 ) || defined(__APPLE__)
#   include <clocale>
#endif
#if defined(_WIN32)
#    include <intrin.h>
#endif
#include <clocale>
#include <limits>
#include <vector>
#include <algorithm>

//============================================== Module ============================================
    /// This is a <em><b>C++ Module</b></em> of the \aliblong.
    /// Due to the dual-compile option (either as C++20-Modules or using legacy C++ inclusion),
    /// the C++20-Module names are not of further interest or use.<br>
    /// In general, the names equal the names of the header files listed in the chapter
    /// #"alib_manual_modules_impludes" of the \alib User Manual.
    ///
    /// @see The documentation of the <em><b>"ALib Module"</b></em> given with the corresponding
    ///      Programmer's Manual \alib_containers.
    export module ALib.Containers.StringTreeIterator;
    import        ALib.Lang;
    import        ALib.Containers.StringTree;
    import        ALib.Strings;
//============================================= Exports ============================================
export import ALib.Containers.init;
#if ALIB_STRINGS
#   include "alib/containers/stringtreeiterator.hpp"
#endif
