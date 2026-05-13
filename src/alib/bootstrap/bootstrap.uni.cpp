//##################################################################################################
//  ALib C++ Framework
//
//  Copyright 2013-2026 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
//========================================= Global Fragment ========================================
#include "alib_precompile.hpp"
#include "alib/singletons/singletons.prepro.hpp"
#include "alib/monomem/monomem.prepro.hpp"
#include "alib/strings/strings.prepro.hpp"
#include "alib/boxing/boxing.prepro.hpp"
#include "alib/resources/resources.prepro.hpp"
#include "alib/bitbuffer/bitbuffer.prepro.hpp"
#include "alib/alox/alox.prepro.hpp"
#include "alib/bootstrap/bootstrap.prepro.hpp"
#if ALIB_DEBUG && !DOXYGEN
#   if defined(__GNUC__) || defined(__clang__)
#      include <cxxabi.h>
#      include <cassert>
#   endif
#   include <cstdlib>
#   if defined(_WIN32)
#      include <cstring>
#   endif
#endif

#if !DOXYGEN
#   include "alib/platform.prepro.hpp"
#   if (ALIB_DEBUG)
#       include <cassert>
#       include <cstring>
#       if defined( _WIN32 ) && !defined(_STRING_)
#          include <string>
#       endif
#   endif

#endif // !DOXYGEN

#include <iostream>
#include <iomanip>

#if (ALIB_SINGLE_THREADED && ALIB_EXT_LIB_THREADS_AVAILABLE)
#  include <thread>
#endif

//============================================= Imports ============================================
#   include "ALib.Time.H"
#   include "ALib.Threads.H"
#   include "ALib.Monomem.H"
#   include "ALib.Strings.H"
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
#   include "ALib.BitBuffer.H"
#   include "ALib.Camp.H"
#   include "ALib.Camp.Base.H"
#   include "ALib.Bootstrap.H"
#   include "ALib.App.H"
#   include "ALib.Expressions.H"
#   include "ALib.ALox.Impl.H"
#   include "ALib.FileTree.H"

//========================================== Implementation ========================================
#include "bootstrap.cpp"
