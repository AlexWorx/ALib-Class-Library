//##################################################################################################
//  ALib C++ Framework
//
//  Copyright 2013-2026 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
//========================================= Global Fragment ========================================
#include "alib_precompile.hpp"
#include "alib/boxing/boxing.prepro.hpp"
#include "alib/variables/variables.prepro.hpp"
#include "alib/alox/alox.prepro.hpp"
#include <fstream>

//============================================= Imports ============================================
#   include "ALib.Containers.List.H"
#   include "ALib.Monomem.H"
#   include "ALib.Boxing.H"
#   include "ALib.EnumRecords.Bootstrap.H"
#   include "ALib.Variables.H"
#   include "ALib.Format.FormatterPythonStyle.H"
#   include "ALib.Format.FormatterJavaStyle.H"
#   include "ALib.Camp.H"
#   include "ALib.Camp.Base.H"
#   include "ALib.Camp.ResourceCompiler.H"
#   include "ALib.ALox.H"
#   include "ALib.ALox.Impl.H"

//========================================== Implementation ========================================
ALIB_ALLOW_IMPLEMENTATION_INCLUSION
#include "aloxinit.cpp"
#include "alox.cpp"
#include "logtools.cpp"
#include "aloxcamp.cpp"
#include "textlogger/textlogger.cpp"
#include "textlogger/variables.cpp"
#include "textlogger/plaintextlogger.cpp"
#include "loggers/ansilogger.cpp"
#include "loggers/consolelogger.cpp"
#include "loggers/textfilelogger.cpp"
#if defined( _WIN32 )
#include "loggers/vstudiologger.cpp"
#include "loggers/windowsconsolelogger.cpp"
#endif
ALIB_POP_ALLOWANCE
