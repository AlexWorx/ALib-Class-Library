//##################################################################################################
//  ALib C++ Framework
//
//  Copyright 2013-2026 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
//========================================= Global Fragment ========================================
module;
#include <cmath>
#include <functional>
#include <cstring>
#include <typeindex>
#include <span>

#   include "ALib.Characters.Functions.H"
#   include "ALib.Time.H"
#   include "ALib.Threads.H"
#   include "ALib.Monomem.H"
#   include "ALib.Strings.H"
#   include "ALib.Strings.Token.H"
#   include "ALib.Singletons.H"
#   include "ALib.Boxing.H"
#   include "ALib.EnumRecords.Bootstrap.H"
#   include "ALib.ThreadModel.H"
#   include "ALib.Exceptions.H"
#   include "ALib.System.H"
#   include "ALib.Format.H"
#   include "ALib.Format.StdImplementation.H"
#   include "ALib.Format.FormatterPythonStyle.H"
#   include "ALib.Format.FormatterJavaStyle.H"
#   include "ALib.Variables.H"
#   include "ALib.Variables.IniFile.H"
#   include "ALib.BitBuffer.H"
#   include "ALib.Camp.H"
#   include "ALib.Camp.Base.H"
#   include "ALib.Bootstrap.H"
#   include "ALib.App.H"
#   include "ALib.Expressions.H"
#   include "ALib.ALox.Impl.H"
#   include "ALib.FileTree.H"

#if ALIB_DEBUG && !DOXYGEN
namespace alib::boxing::debug {
    // This is used by boxing::Bootstrap to do a runtime-check for compatibility of boxing
    // and long double values.
    // It was put here to prevent the compiler to optimize and remove the code.
    extern  long double LONGDOUBLE_WRITE_TEST_MEM[2];
extern  void LongDoubleTrueLengthSet();
extern  bool LongDoubleTrueLengthTest();
}
#endif

//============================================= Imports ============================================
module ALib.Boxing;
    import   ALib.Characters.Functions;
//========================================== Implementation ========================================
ALIB_ALLOW_IMPLEMENTATION_INCLUSION
#include "boxstrap.cpp"
ALIB_POP_ALLOWANCE
