//##################################################################################################
//  ALib C++ Framework
//
//  Copyright 2013-2026 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
//========================================= Global Fragment ========================================
module;
#include "alib/strings/strings.prepro.hpp"
#if ALIB_FEAT_BOOST_REGEX && (!ALIB_CHARACTERS_WIDE || ALIB_CHARACTERS_NATIVE_WCHAR)
#   include <boost/regex.hpp>
#   include <string>
#endif
//============================================= Imports ============================================
module ALib.Strings.Search;
    import   ALib.Characters.Functions;
//========================================== Implementation ========================================
ALIB_ALLOW_IMPLEMENTATION_INCLUSION
#include "search.cpp"
#include "regexmatcher.cpp"
#include "wildcardmatcher.cpp"
ALIB_POP_ALLOWANCE
