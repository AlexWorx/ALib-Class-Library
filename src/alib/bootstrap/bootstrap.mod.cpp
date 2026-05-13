//##################################################################################################
//  ALib C++ Framework
//
//  Copyright 2013-2026 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
//========================================= Global Fragment ========================================
module;
#include "alib/singletons/singletons.prepro.hpp"
#include "alib/monomem/monomem.prepro.hpp"
#include "alib/strings/strings.prepro.hpp"
#include "alib/boxing/boxing.prepro.hpp"
#include "alib/resources/resources.prepro.hpp"
#include "alib/variables/variables.prepro.hpp"
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

#if ALIB_CAMP && ALIB_DEBUG
#   include <algorithm>
#endif

#endif // !DOXYGEN

#include <iostream>
#include <iomanip>

#if (ALIB_SINGLE_THREADED && ALIB_EXT_LIB_THREADS_AVAILABLE)
#  include <thread>
#endif

//============================================= Imports ============================================
  module ALib.Bootstrap;
      import   ALib.Lang;
      import   ALib.Time;
      import   ALib.Threads;
#   if ALIB_MONOMEM
      import   ALib.Monomem;
      import   ALib.Monomem.StdContainers;
#   endif
#   if ALIB_STRINGS
      import   ALib.Strings;
#   endif
#   if ALIB_SINGLETONS
      import   ALib.Singletons;
#   endif
#   if ALIB_BOXING
      import   ALib.Boxing;
#   endif
#   if ALIB_ENUMRECORDS
      import   ALib.EnumRecords.Bootstrap;
#   endif
#   if ALIB_THREADMODEL
      import   ALib.ThreadModel;
#   endif
#   if ALIB_EXCEPTIONS
      import   ALib.Exceptions;
#   endif
#   if ALIB_SYSTEM
      import   ALib.System;
#   endif
#   if ALIB_VARIABLES
      import ALib.Variables;
#   endif
#   if ALIB_FORMAT
      import   ALib.Format;
      import   ALib.Format.StdImplementation;
      import   ALib.Format.FormatterPythonStyle;
      import   ALib.Format.FormatterJavaStyle;
#   endif
#   if ALIB_CAMP
      import   ALib.Camp;
      import   ALib.Camp.Base;
      import   ALib.Resources;
      import   ALib.Variables;
#   endif
#   if ALIB_BITBUFFER
      import   ALib.BitBuffer;
#   endif
#   if ALIB_ALOX
      import ALib.ALox;
      import ALib.ALox.Impl;
#   endif
#   if ALIB_EXPRESSIONS
      import ALib.Expressions;
#   endif
#   if ALIB_APP
      import ALib.App;
#   endif
#   if ALIB_FILETREE
      import ALib.FileTree;
#   endif


//========================================== Implementation ========================================
ALIB_ALLOW_IMPLEMENTATION_INCLUSION
#include "bootstrap.cpp"
ALIB_POP_ALLOWANCE
