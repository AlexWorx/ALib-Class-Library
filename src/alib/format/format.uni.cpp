//##################################################################################################
//  ALib C++ Framework
//
//  Copyright 2013-2026 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
//========================================= Global Fragment ========================================
#include "alib_precompile.hpp"
#include <cmath>
#include "alib/boxing/boxing.prepro.hpp"
//============================================= Imports ============================================
#   include "ALib.Lang.H"
#   include "ALib.Strings.H"
#   include "ALib.Format.StdImplementation.H"
#   include "ALib.Format.FormatterPythonStyle.H"
#   include "ALib.Format.FormatterJavaStyle.H"
#   include "ALib.Format.PropertyFormatter.H"
#   include "ALib.Format.Paragraphs.H"
#   include "ALib.Exceptions.H"
#   include "ALib.Strings.Calendar.H"
#if ALIB_DEBUG_BOXING
#   include "ALib.Monomem.H"
#   if ALIB_DEBUG_CONTAINERS
#      include "ALib.Containers.HashTable.H"
#   endif
#endif
#if ALIB_DEBUG_RESOURCES
#    include "ALib.Resources.H"
#endif
#if ALIB_CAMP
#    include "ALib.Camp.Base.H"
#endif
//========================================== Implementation ========================================
ALIB_ALLOW_IMPLEMENTATION_INCLUSION
#include "formatter.cpp"
#include "formatterstdimpl.cpp"
#include "formatterpythonstyle.cpp"
#include "formatterjavastyle.cpp"
#include "fmtvarious.cpp"
#include "paragraphs.cpp"
#include "propertyformatter.cpp"
#include "extensions/boxing_format_debug.cpp"
#include "extensions/resources_debug.cpp"
ALIB_POP_ALLOWANCE
