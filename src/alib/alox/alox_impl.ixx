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
#include "alib/strings/strings.prepro.hpp"
#include "alib/enumrecords/enumrecords.prepro.hpp"
#include "alib/resources/resources.prepro.hpp"
#include "alib/variables/variables.prepro.hpp"
#include "alib/camp/camp.prepro.hpp"
#include "alib/alox/alox.prepro.hpp"
#include <vector>
#include <iostream>

//============================================== Module ============================================
    /// This is a <em><b>C++ Module</b></em> of the \aliblong.
    /// Due to the dual-compile option (either as C++20-Modules or using legacy C++ inclusion),
    /// the C++20-Module names are not of further interest or use.<br>
    /// In general, the names equal the names of the header files listed in the chapter
    /// #"alib_manual_modules_impludes" of the \alib User Manual.
    ///
    /// @see The documentation of the <em><b>"ALib Module"</b></em> given with the corresponding
    ///      Programmer's Manual \alib_alox.
    export module ALib.ALox.Impl;
    export import ALib.ALox.init;
       import     ALib.Lang;
       import     ALib.Time;
       import     ALib.Threads;
       import     ALib.Monomem;
       import     ALib.Monomem.StdContainers;
       import     ALib.Boxing;
       import     ALib.Containers.List;
       import     ALib.Containers.HashTable;
       import     ALib.Containers.StringTree;
       import     ALib.Containers.LRUCacheTable;
       import     ALib.Strings;
       import     ALib.Strings.StdIOStream;
       import     ALib.Strings.AutoSizes;
       import     ALib.Strings.Calendar;
       import     ALib.Strings.Vector;
       import     ALib.Strings.Escaper;
       import     ALib.EnumRecords;
       import     ALib.Resources;
       import     ALib.Exceptions;
       import     ALib.System;
       import     ALib.Format;
       import     ALib.Variables;
       import     ALib.Camp;

//============================================= Exports ============================================
#include "alib/alox/detail/loxpimpl.hpp"

#include "alib/alox/aloxcamp.hpp"

#include "alib/alox/detail/logger.hpp"
#include "alib/alox/detail/domain.hpp"
#include "alib/alox/detail/scopestore.hpp"
#include "alib/alox/detail/scopeinfo.hpp"
#include "alib/alox/detail/scopedump.hpp"

#include "alib/alox/textlogger/variables.hpp"
#include "alib/alox/textlogger/textlogger.hpp"
#include "alib/alox/textlogger/plaintextlogger.hpp"

#include "alib/alox/loggers/ansilogger.hpp"
#include "alib/alox/loggers/consolelogger.hpp"
#include "alib/alox/loggers/memorylogger.hpp"
#include "alib/alox/loggers/textfilelogger.hpp"
#include "alib/alox/loggers/vstudiologger.hpp"
#include "alib/alox/loggers/windowsconsolelogger.hpp"
