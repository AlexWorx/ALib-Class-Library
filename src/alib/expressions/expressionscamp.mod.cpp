//##################################################################################################
//  ALib C++ Framework
//
//  Copyright 2013-2026 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
//========================================= Global Fragment ========================================
module;
#include "alib/boxing/boxing.prepro.hpp"
#include "alib/resources/resources.prepro.hpp"
#include "alib/camp/camp.prepro.hpp"
#include "alib/expressions/expressions.prepro.hpp"

//============================================= Imports ============================================
module ALib.Expressions;
    import   ALib.Expressions.Impl;
    import   ALib.Lang;
    import   ALib.Boxing;
    import   ALib.EnumRecords;
    import   ALib.EnumRecords.Bootstrap;
    import   ALib.Variables;
    import   ALib.Camp;
    import   ALib.Camp.ResourceCompiler;
//========================================== Implementation ========================================
ALIB_ALLOW_IMPLEMENTATION_INCLUSION
#include "expressionscamp.cpp"
ALIB_POP_ALLOWANCE
