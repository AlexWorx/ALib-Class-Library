//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
#if !defined(ALIB_C20_MODULES) && !defined(DOXYGEN)
#   define ALIB_C20_MODULES 0
#endif
#if !ALIB_C20_MODULES
#ifndef H_ALIB_PRECOMPILE
#define H_ALIB_PRECOMPILE
#if defined(_MSC_VER)
#  define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#endif
#ifndef HPP_ALIB
#   include "alib/alib.prepro.hpp"
#endif
#if !ALIB_C20_MODULES && defined(ALIB_PRECOMPILED_HEADER) && ALIB_PRECOMPILED_HEADER
#   include "ALib.Lang.H"
#   include "ALib.Time.H"
#   include "ALib.EnumOps.H"
#   if defined(QT_CORE_LIB)
#       include "ALib.Compatibility.QTCharacters.H"
#   endif

#   include "ALib.Strings.H"
#   include "ALib.Boxing.StdFunctors.H"
#   include "ALib.Boxing.H"
#   include "ALib.EnumRecords.H"
#   include "ALib.Resources.H"
#   include "ALib.Format.H"
#   include "ALib.Exceptions.H"
#   include "ALib.ALox.H"
#endif

#endif // H_ALIB_PRECOMPILE
#endif // ALIB_C20_MODULES
