//##################################################################################################
//  ALib C++ Framework
//
//  Copyright 2013-2026 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
//========================================= Global Fragment ========================================
#include "alib_precompile.hpp"
#include "alib/enumrecords/enumrecords.prepro.hpp"
#if !ALIB_MONOMEM || !ALIB_CONTAINERS
#    include <unordered_map>
#endif
//============================================= Imports ============================================
#   if ALIB_MONOMEM && ALIB_CONTAINERS
#     include "ALib.Monomem.H"
#     include "ALib.Containers.HashTable.H"
#   endif
#   include "ALib.EnumRecords.H"
#   include "ALib.EnumRecords.Bootstrap.H"
//========================================== Implementation ========================================
#include "records.cpp"
#include "bootstrap/recordparser.cpp"
