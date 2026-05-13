//##################################################################################################
//  ALib C++ Framework
//
//  Copyright 2013-2026 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
//========================================= Global Fragment ========================================
#include "alib_precompile.hpp"
#include "alib/strings/strings.prepro.hpp"
#if ALIB_MONOMEM && ALIB_DEBUG
#   include <cstring>
#endif
//============================================= Imports ============================================
#   include "ALib.Lang.H"
#   include "ALib.Strings.H"
#   include "ALib.Strings.StdIOStream.H"
//========================================== Implementation ========================================
ALIB_ALLOW_IMPLEMENTATION_INCLUSION
#include "string.cpp"
#include "astring.cpp"
#include "substring.cpp"
#include "numberformat.cpp"
#include "detail/numberconversion.cpp"
#include "stdiostream.cpp"
ALIB_POP_ALLOWANCE
