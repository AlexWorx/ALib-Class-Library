//##################################################################################################
//  ALib C++ Framework
//
//  Copyright 2013-2026 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
//========================================= Global Fragment ========================================
module;
#include "alib/boxing/boxing.prepro.hpp"
#include "alib/strings/strings.prepro.hpp"
#include "alib/resources/resources.prepro.hpp"
#include "alib/system/system.prepro.hpp"
#include "alib/alox/alox.prepro.hpp"
#include "alib/app/app.prepro.hpp"
#include <iostream>

#include <vector>
#include <algorithm>
//============================================= Imports ============================================
module ALib.App;
    import   ALib.Lang;
    import   ALib.Strings;
    import   ALib.Strings.Tokenizer;
    import   ALib.Strings.Calendar;
    import   ALib.Strings.StdIOStream;
    import   ALib.Boxing;
    import   ALib.EnumRecords;
    import   ALib.EnumRecords.Bootstrap;
    import   ALib.Variables;
    import   ALib.Variables.Plugins;
    import   ALib.Variables.IniFile;
    import   ALib.Camp.Base;
    import   ALib.Bootstrap;
//========================================== Implementation ========================================
ALIB_ALLOW_IMPLEMENTATION_INCLUSION
#include "app.cpp"
ALIB_POP_ALLOWANCE
