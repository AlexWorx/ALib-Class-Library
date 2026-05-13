//--------------------------------------------------------------------------------------------------
//  ALib C++ Framework
//  Configuration Sample
//
//  Copyright 2025 A-Worx GmbH, Germany
//  Published under Boost Software License (a free software license, see LICENSE.txt)
//--------------------------------------------------------------------------------------------------
//========================================= Global Fragment ========================================
module;
#include "alib/boxing/boxing.prepro.hpp"
#include "alib/strings/strings.prepro.hpp"
#include "alib/camp/camp.prepro.hpp"
#include "ALib.Monomem.StdContainers.H"
//============================================= Imports ============================================
module ALib.App;
    import   ALib.Lang;
    import   ALib.Strings;
    import   ALib.Boxing;
    import   ALib.EnumRecords;
    import   ALib.EnumRecords.Bootstrap;
    import   ALib.Variables;
    import   ALib.Camp.ResourceCompiler;
//========================================== Implementation ========================================
ALIB_ALLOW_IMPLEMENTATION_INCLUSION
#include "appcamp.cpp"
ALIB_POP_ALLOWANCE
