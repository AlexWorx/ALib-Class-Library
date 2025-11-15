//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef H_ALIB_EXPRESSIONS
#define H_ALIB_EXPRESSIONS
#pragma once
#ifndef INL_ALIB
#   include "alib/alib.inl"
#endif

#include "alib/expressions/expressions.prepro.hpp"

#if ALIB_EXPRESSIONS
#   if ALIB_C20_MODULES && !DOXYGEN
        import ALib.Expressions;
#   elif !defined(ALIB_INC_EXPRESSIONS_MPP)
#       define ALIB_INC_EXPRESSIONS_MPP
#       include "alib/expressions/expressions.mpp"
#   endif
#endif

#endif // H_ALIB_EXPRESSIONS
