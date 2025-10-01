//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef H_ALIB_FILES_EXPRESSIONS
#define H_ALIB_FILES_EXPRESSIONS
#pragma once
#ifndef INL_ALIB
#   include "alib/alib.inl"
#endif

#if ALIB_FILES && ALIB_EXPRESSIONS
#   if ALIB_C20_MODULES && !DOXYGEN
        import ALib.Files.Expressions;
#   elif !defined(ALIB_INC_FILES_EXPRESSIONS_MPP)
#       define ALIB_INC_FILES_EXPRESSIONS_MPP
#       include "alib/files/expressions/fileexpressions.mpp"
#   endif
#endif

#endif // H_ALIB_FILES_EXPRESSIONS

