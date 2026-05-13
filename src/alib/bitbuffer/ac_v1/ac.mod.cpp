//##################################################################################################
//  ALib C++ Framework
//
//  Copyright 2013-2026 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
//========================================= Global Fragment ========================================
module;
#include "alib/bitbuffer/bitbuffer.prepro.hpp"
#include "alib/boxing/boxing.prepro.hpp"

//============================================= Imports ============================================
module ALib.BitBuffer;
#  if ALIB_FORMAT
    import   ALib.Format;
#  endif
    import   ALib.Containers.FixedCapacityVector;
//========================================== Implementation ========================================
ALIB_ALLOW_IMPLEMENTATION_INCLUSION
#include "ac.cpp"
#include "huffman.cpp"
ALIB_POP_ALLOWANCE
