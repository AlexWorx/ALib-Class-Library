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
#include "alib/singletons/singletons.prepro.hpp"

#if ALIB_FEAT_SINGLETON_MAPPED
#   if !ALIB_MONOMEM || !ALIB_CONTAINERS
#      include <unordered_map>
#   endif
#   if !ALIB_MONOMEM
#      include <mutex>
#   endif
#endif

//============================================== Module ============================================
/// This is a C++ Module of the \aliblong. Due to the dual-compile option - as either
/// C++20-Modules or legacy C++ inclusion, the C++20-Module names are not of further interest
/// or use.<br>
/// In general, the names equal the names of the header files listed in the chapter
/// #"alib_manual_modules_impludes" of the \alib User Manual.
///
    /// @see The documentation of the <em><b>"ALib Module"</b></em> given with the corresponding
    ///      Programmer's Manual \alib_singletons.
    export module ALib.Singletons;
    import        ALib.Lang;
#   if ALIB_FEAT_SINGLETON_MAPPED
#      if ALIB_DEBUG && ALIB_STRINGS
           import ALib.Strings;
#      endif
#      if ALIB_MONOMEM
           import ALib.Monomem;
#          if ALIB_CONTAINERS
             import ALib.Containers.HashTable;
#          endif
#      endif
#   endif

//============================================= Exports ============================================
#include "alib/singletons/singletons.hpp"
