//##################################################################################################
//  ALib C++ Framework
//
//  Copyright 2013-2026 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
//========================================= Global Fragment ========================================
module;
#include "alib/enumrecords/enumrecords.prepro.hpp"
#include <stdexcept>
#if !ALIB_MONOMEM && ALIB_CONTAINERS
#    include <unordered_map>
#endif
//============================================= Imports ============================================
module ALib.EnumRecords.Bootstrap;
    import   ALib.Lang;
    import   ALib.EnumRecords;
#if ALIB_MONOMEM && ALIB_CONTAINERS
    import   ALib.Monomem;
    import   ALib.Containers.HashTable;
#endif
#if ALIB_FORMAT
    import   ALib.Format;
#endif
//========================================== Implementation ========================================
ALIB_ALLOW_IMPLEMENTATION_INCLUSION
#include "recordparser.cpp"
ALIB_POP_ALLOWANCE
