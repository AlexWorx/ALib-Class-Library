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
#include <cmath>
#include <cstring>
#include <vector>
#include <algorithm>
//============================================= Imports ============================================
module ALib.Format;
    import   ALib.Lang;
    import   ALib.Time;
    import   ALib.Strings;
    import   ALib.Boxing;
    import   ALib.Strings.Calendar;
#if ALIB_DEBUG_BOXING
    import   ALib.Monomem;
#    if ALIB_DEBUG_CONTAINERS
        import   ALib.Containers.HashTable;
#    endif
#endif
#if ALIB_DEBUG_RESOURCES && ALIB_FORMAT
   import ALib.Resources;
#endif
#if ALIB_CAMP
   import ALib.Camp.Base;
#endif

//========================================== Implementation ========================================
ALIB_ALLOW_IMPLEMENTATION_INCLUSION
#include "formatter.cpp"
#include "fmtvarious.cpp"
#include "extensions/boxing_format_debug.cpp"
#include "extensions/resources_debug.cpp"
ALIB_POP_ALLOWANCE
