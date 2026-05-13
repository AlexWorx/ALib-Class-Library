//##################################################################################################
//  ALib C++ Framework
//
//  Copyright 2013-2026 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
//========================================= Global Fragment ========================================
module;
#include "ALib.Monomem.StdContainers.H"
//============================================= Imports ============================================
module ALib.Format.Paragraphs;
    import   ALib.Lang;
#  if ALIB_MONOMEM
    import   ALib.Monomem;
#  endif
    import   ALib.Strings;
    import   ALib.Exceptions;
#   if ALIB_CAMP
      import ALib.Camp.Base;
#   endif
//========================================== Implementation ========================================
ALIB_ALLOW_IMPLEMENTATION_INCLUSION
#include "paragraphs.cpp"
ALIB_POP_ALLOWANCE
