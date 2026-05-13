//##################################################################################################
//  ALib C++ Framework
//
//  Copyright 2013-2026 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
//========================================= Global Fragment ========================================
module;
#include "alib/boxing/boxing.prepro.hpp"
#include "alib/variables/variables.prepro.hpp"
#include "alib/camp/camp.prepro.hpp"
#include "alib/alox/alox.prepro.hpp"

#include "ALib.Strings.Vector.H"
//============================================= Imports ============================================
module ALib.ALox.Impl;
    import   ALib.ALox;
    import   ALib.Lang;
    import   ALib.EnumOps;
    import   ALib.Containers.List;
    import   ALib.Strings;
    import   ALib.Boxing;
    import   ALib.EnumRecords;
    import   ALib.EnumRecords.Bootstrap;
    import   ALib.Variables;
    import   ALib.Camp;
    import   ALib.Camp.Base;
    import   ALib.Camp.ResourceCompiler;
//========================================== Implementation ========================================
ALIB_ALLOW_IMPLEMENTATION_INCLUSION
#include "aloxcamp.cpp"
ALIB_POP_ALLOWANCE
