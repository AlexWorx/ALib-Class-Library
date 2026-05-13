//##################################################################################################
//  ALib C++ Framework
//
//  Copyright 2013-2026 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
//========================================= Global Fragment ========================================
module;
#include "alib/strings/strings.prepro.hpp"
#include "alib/variables/variables.prepro.hpp"
#if ALIB_DEBUG
#   include <cstdlib>
#   include "alib/platform.prepro.hpp"
#   include <cassert>
#   include <cstring>
#   include <iostream>
#   include <iomanip>
#   include <string>
#   include <string>
#   include <stdexcept>
#   include <any>
#   if __has_include(<format>)
#      include <format>
#   else
#      include <fmt/format.h>
#   endif
#   include <codecvt>

#   if (ALIB_SINGLE_THREADED && ALIB_EXT_LIB_THREADS_AVAILABLE)
#       include <thread>
#   endif
#   if ALIB_DEBUG_ASSERTION_PRINTABLES
#       include <unordered_set>
#   endif

#   include <iostream>
#   include <iomanip>
#   include <typeindex>
#   include <functional>
#endif

//============================================= Imports ============================================
module   ALib.Lang;
#  if ALIB_DEBUG
        import   ALib.Threads;
        import   ALib.Bootstrap;
#      if ALIB_STRINGS
        import   ALib.Strings;
        import   ALib.Strings.Token;
#      endif
#      if ALIB_BOXING
        import   ALib.Boxing;
#      endif
#       if ALIB_ENUMRECORDS
        import   ALib.EnumRecords;
#       endif
#       if ALIB_RESOURCES
        import   ALib.Resources;
#       endif
#       if ALIB_VARIABLES
        import   ALib.Variables;
#       endif
#       if ALIB_SYSTEM
        import   ALib.System;
#       endif
#       if ALIB_THREADMODEL
        import   ALib.ThreadModel;
#       endif
#       if ALIB_CAMP
        import   ALib.Camp.Base;
#       endif

//========================================== Implementation ========================================
ALIB_ALLOW_IMPLEMENTATION_INCLUSION
#include "assert.cpp"
ALIB_POP_ALLOWANCE

#   endif // ALIB_DEBUG
