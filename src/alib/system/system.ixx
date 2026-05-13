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
#include "alib/enumops/enumops.prepro.hpp"
#include "alib/enumrecords/enumrecords.prepro.hpp"
#include "alib/system/system.prepro.hpp"

#include <filesystem>
#if defined( __GNUC__ ) || ( defined(_WIN32) && defined(_MSC_VER))
#   include <errno.h>
#endif
#include <vector>

//============================================== Module ============================================
    /// This is a <em><b>C++ Module</b></em> of the \aliblong.
    /// Due to the dual-compile option (either as C++20-Modules or using legacy C++ inclusion),
    /// the C++20-Module names are not of further interest or use.<br>
    /// In general, the names equal the names of the header files listed in the chapter
    /// #"alib_manual_modules_impludes" of the \alib User Manual.
    ///
    /// @see The documentation of the <em><b>"ALib Module"</b></em> given with the corresponding
    ///      Programmer's Manual \alib_system.
    export module ALib.System;
    import        ALib.Lang;
    import        ALib.Time;
    import        ALib.Characters.Functions;
    import        ALib.EnumOps;
#  if ALIB_MONOMEM
    import        ALib.Monomem;
#  endif
#  if ALIB_BOXING
    import        ALib.Boxing;
#  endif
    import        ALib.Strings;
    import        ALib.Strings.Vector;
    import        ALib.Strings.Escaper;
    import        ALib.Strings.Tokenizer;
    import        ALib.EnumRecords;

//============================================= Exports ============================================
#include "alib/system/console.hpp"
#include "alib/system/environment.hpp"
#include "alib/system/path.hpp"
#include "alib/system/filestatus.hpp"
#include "alib/system/processinfo.hpp"
#include "alib/system/shellcommand.hpp"
