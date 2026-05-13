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
#include "alib/enumrecords/enumrecords.prepro.hpp"
#include "alib/variables/variables.prepro.hpp"

//============================================== Module ============================================
    /// This is a <em><b>C++ Module</b></em> of the \aliblong.
    /// Due to the dual-compile option (either as C++20-Modules or using legacy C++ inclusion),
    /// the C++20-Module names are not of further interest or use.<br>
    /// In general, the names equal the names of the header files listed in the chapter
    /// #"alib_manual_modules_impludes" of the \alib User Manual.
    ///
    /// @see The documentation of the <em><b>"ALib Module"</b></em> given with the corresponding
    ///      Programmer's Manual \alib_variables.
    export module ALib.Variables.IniFile;
    import        ALib.Lang;
    import        ALib.Monomem;
    import        ALib.Containers.List;
    import        ALib.Containers.HashTable;
    import        ALib.Strings;
    import        ALib.Boxing;
    import        ALib.EnumRecords;
#if ALIB_EXCEPTIONS
    import        ALib.Exceptions;
#endif
#if ALIB_RESOURCES
    import        ALib.Resources;
#endif
    import        ALib.System;
    import        ALib.Variables;

//============================================= Exports ============================================
#include "alib/variables/inifile.hpp"
#include "alib/variables/inifilefeeder.hpp"
