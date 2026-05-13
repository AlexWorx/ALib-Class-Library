//##################################################################################################
//  ALib C++ Framework
//
//  Copyright 2013-2026 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
//========================================= Global Fragment ========================================
module;
#include <iostream>
#include "alib/strings/strings.prepro.hpp"
#include "alib/boxing/boxing.prepro.hpp"
#include "alib/resources/resources.prepro.hpp"
#include "alib/camp/camp.prepro.hpp"
#include "alib/camp/camp.prepro.hpp"
#include "alib/variables/variables.prepro.hpp"
//============================================= Imports ============================================
module ALib.Camp.Base;
    import        ALib.Lang;
    import        ALib.Time;
    import        ALib.EnumOps;
#   if !ALIB_SINGLE_THREADED
       import     ALib.Threads;
#   endif
    import        ALib.Monomem;
    import        ALib.Strings.Tokenizer;
    import        ALib.Strings;
    import        ALib.Strings.StdIOStream;
    import        ALib.Strings.Calendar;
    import        ALib.Strings.Tokenizer;
    import        ALib.Boxing;
    import        ALib.EnumRecords.Bootstrap;
    import        ALib.Exceptions;
    import        ALib.System;
    import        ALib.Variables;
    import        ALib.Variables.IniFile;
    #if ALIB_THREADMODEL
        import    ALib.ThreadModel;
    #endif
    #if ALIB_BITBUFFER
        import    ALib.BitBuffer;
    #endif
    import        ALib.Format;
    import        ALib.Format.StdImplementation;
    import        ALib.Format.FormatterPythonStyle;
    import        ALib.Format.FormatterJavaStyle;
    import        ALib.Camp;
    import        ALib.Camp.ResourceCompiler;
//========================================== Implementation ========================================
ALIB_ALLOW_IMPLEMENTATION_INCLUSION
#include "basecamp.cpp"
ALIB_POP_ALLOWANCE
