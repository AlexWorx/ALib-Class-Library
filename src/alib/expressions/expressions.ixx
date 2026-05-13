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
#include "alib/resources/resources.prepro.hpp"
#include "alib/camp/camp.prepro.hpp"
#include "alib/expressions/expressions.prepro.hpp"

#include <stack>
#include <bitset>
#include <vector>
//============================================== Module ============================================
    /// This is a <em><b>C++ Module</b></em> of the \aliblong.
    /// Due to the dual-compile option (either as C++20-Modules or using legacy C++ inclusion),
    /// the C++20-Module names are not of further interest or use.<br>
    /// In general, the names equal the names of the header files listed in the chapter
    /// #"alib_manual_modules_impludes" of the \alib User Manual.
    ///
    /// @see The documentation of the <em><b>"ALib Module"</b></em> given with the corresponding
    ///      Programmer's Manual \alib_expressions.
    export module ALib.Expressions;
    import        ALib.Lang;
    import        ALib.Time;
    import        ALib.EnumOps;
    import        ALib.Singletons;
    import        ALib.Monomem;
    import        ALib.Monomem.StdContainers;
    import        ALib.Monomem.SharedMonoVal;
    import        ALib.Containers.List;
    import        ALib.Containers.HashTable;
    import        ALib.Boxing;
    import        ALib.Strings;
    import        ALib.Strings.Token;
    import        ALib.Strings.StdFunctors;
    import        ALib.Strings.Vector;
    import        ALib.Resources;
    import        ALib.EnumRecords;
    import        ALib.Format;
    import        ALib.Exceptions;
    import        ALib.Variables;
    import        ALib.Camp;

//============================================= Exports ============================================
#include "alib/expressions/detail/ast.hpp"
#include "alib/expressions/parser.hpp"
#include "alib/expressions/expressionscamp.hpp"
#include "alib/expressions/expression.hpp"
#include "alib/expressions/scope.hpp"
#include "alib/expressions/compiler.hpp"
#include "alib/expressions/compilerplugin.hpp"
#include "alib/expressions/standardrepository.hpp"
#include "alib/expressions/util/expressionformatter.hpp"
