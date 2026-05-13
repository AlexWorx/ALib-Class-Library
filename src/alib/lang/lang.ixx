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

#if defined(_WIN32)
#   include <intrin.h>
#endif
#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstring>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <typeindex>
#include <vector>

#if ALIB_DEBUG
#   include <any>
#   include <iostream>
#endif

//============================================== Module ============================================
    /// This is a <em><b>C++ Module</b></em> of the \aliblong.
    /// Due to the dual-compile option (either as C++20-Modules or using legacy C++ inclusion),
    /// the C++20-Module names are not of further interest or use.<br>
    /// In general, the names equal the names of the header files listed in the chapter
    /// #"alib_manual_modules_impludes" of the \alib User Manual.
    ///
    /// @see The documentation of the <em><b>"ALib Module"</b></em> given with the corresponding
    ///      Programmer's Manual \alib_lang.
    export module ALib.Lang;

//============================================= Exports ============================================
#include "alib/lang/callerinfo.hpp"
#include "alib/lang/assert.hpp"
#include "alib/lang/mainargs.hpp"
#include "alib/lang/chk_nc.hpp"
#include "alib/lang/tmp.hpp"
#include "alib/lang/placeholder.hpp"
#include "alib/lang/integers.hpp"
#include "alib/lang/dbgtypedemangler.hpp"
#include "alib/lang/dbgcriticalsections.hpp"
#include "alib/lang/sidilist.hpp"
#include "alib/lang/bidilist.hpp"
#include "alib/lang/allocation.hpp"
#include "alib/lang/commonenumdefs.hpp"
#include "alib/lang/bits.hpp"
#include "alib/lang/bitset.hpp"
#include "alib/lang/owner.hpp"
#include "alib/lang/plugins.hpp"
#include "alib/lang/stdtypeinfofunctors.hpp"

#include "alib/characters/chartypes.hpp"
#include "alib/characters/chartraits.hpp"
#include "alib/characters/chartraits_stdtypes.hpp"
