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
#include "alib/enumops/enumops.prepro.hpp"
#include "alib/strings/strings.prepro.hpp"

#include "alib/platform.prepro.hpp"

#include <iterator>
#include <cmath>
#include <algorithm>
#include <cstring>

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
    ///      Programmer's Manual \alib_strings.
    export module ALib.Strings;
    import        ALib.Lang;
    import        ALib.Time;
    import        ALib.EnumOps;
    import        ALib.Threads;
    import        ALib.Characters.Functions;
    #if ALIB_MONOMEM
    import        ALib.Monomem;
    #endif


//============================================= Exports ============================================
#include "alib/strings/string.hpp"
#include "alib/strings/cstring.hpp"
#include "alib/strings/stringnzt.hpp"

#include "alib/strings/detail/tastring.hpp"
#include "alib/strings/format.hpp"
#include "alib/strings/appendables.hpp"
#include "alib/strings/astring.hpp"

#include "alib/strings/substring.hpp"
#include "alib/strings/localstring.hpp"

#include "alib/strings/numberformat.hpp"
#include "alib/strings/detail/numberconversion.hpp"

#if ALIB_MONOMEM
#include "alib/strings/monomem/strings_monomem.hpp"
#endif
