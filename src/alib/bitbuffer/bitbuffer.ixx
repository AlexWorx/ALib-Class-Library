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
#include "alib/boxing/boxing.prepro.hpp"
#include "alib/enumrecords/enumrecords.prepro.hpp"
#include "alib/bitbuffer/bitbuffer.prepro.hpp"
#include <vector>
//============================================== Module ============================================
    /// This is a <em><b>C++ Module</b></em> of the \aliblong.
    /// Due to the dual-compile option (either as C++20-Modules or using legacy C++ inclusion),
    /// the C++20-Module names are not of further interest or use.<br>
    /// In general, the names equal the names of the header files listed in the chapter
    /// #"alib_manual_modules_impludes" of the \alib User Manual.
    ///
    /// @see The documentation of the <em><b>"ALib Module"</b></em> given with the corresponding
    ///      Programmer's Manual \alib_bitbuffer.
    export module ALib.BitBuffer;
    import        ALib.Lang;
    import        ALib.Time;
    import        ALib.EnumOps;
#  if ALIB_BOXING
    import        ALib.Boxing;
#  endif
#  if ALIB_ENUMRECORDS
    import        ALib.EnumRecords;
    import        ALib.Strings;
#  endif
    import        ALib.Monomem;
    import        ALib.Monomem.StdContainers;

//============================================= Exports ============================================
#include "alib/bitbuffer/bitbuffer.hpp"
#include "alib/bitbuffer/ac_v1/huffman.hpp"
#include "alib/bitbuffer/ac_v1/ac.hpp"
#include "alib/bitbuffer/arraycompressor.hpp"
