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
#include "alib/resources/resources.prepro.hpp"

#if ALIB_DEBUG_RESOURCES
#  include <vector>
#endif

//============================================== Module ============================================
    /// This is a <em><b>C++ Module</b></em> of the \aliblong.
    /// Due to the dual-compile option (either as C++20-Modules or using legacy C++ inclusion),
    /// the C++20-Module names are not of further interest or use.<br>
    /// In general, the names equal the names of the header files listed in the chapter
    /// #"alib_manual_modules_impludes" of the \alib User Manual.
    ///
    /// @see The documentation of the <em><b>"ALib Module"</b></em> given with the corresponding
    ///      Programmer's Manual \alib_resources.
    export module ALib.Resources;
    import        ALib.Lang;
    import        ALib.Characters.Functions;
    import        ALib.Monomem;
    import        ALib.Strings;
    import        ALib.Strings.Token;
    import        ALib.Containers.init;
    import        ALib.Containers.HashTable;
    import        ALib.Containers.SharedPtr;
#  if ALIB_ENUMRECORDS
    import        ALib.EnumOps;
    import        ALib.EnumRecords;
    import        ALib.EnumRecords.Bootstrap;
#  endif

//============================================= Exports ============================================
#include "alib/resources/resources.hpp"
#include "alib/resources/detail/resourcemap.hpp"
#include "alib/resources/localresourcepool.hpp"
#include "alib/resources/extensions/strings_loadtokens.hpp"
#if ALIB_ENUMRECORDS
#   include "alib/resources/extensions/enums_serialization.hpp"
#endif
