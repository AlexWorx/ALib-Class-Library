//##################################################################################################
//  ALib C++ Framework
//
//  Copyright 2013-2026 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
//========================================= Global Fragment ========================================
module;
#include "alib/enumrecords/enumrecords.prepro.hpp"
#if !ALIB_MONOMEM || !ALIB_CONTAINERS
#    include <unordered_map>
#endif
//============================================= Imports ============================================
module ALib.EnumRecords;
    import   ALib.EnumRecords.Bootstrap;
    import   ALib.Lang;
#   if ALIB_MONOMEM && ALIB_CONTAINERS
      import ALib.Monomem;
      import ALib.Containers.HashTable;
#   endif
//========================================== Implementation ========================================
ALIB_ALLOW_IMPLEMENTATION_INCLUSION
#include "records.cpp"
ALIB_POP_ALLOWANCE
