//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_EXPRESSIONS_PP
#define HPP_ALIB_EXPRESSIONS_PP
#pragma once
#ifndef INL_ALIB
#   include "alib/alib.inl"
#endif
#if ALIB_EXPRESSIONS

//##################################################################################################
// Symbols introduced by module ALib.Expressions
//##################################################################################################
#if ALIB_DEBUG
    #define CALCULUS_CALLBACK(func)     func, ALIB_NSTRINGIFY(func)
    #define CALCULUS_DEFAULT_AUTOCAST   nullptr, nullptr
#else
    #define CALCULUS_CALLBACK(func)     func
    #define CALCULUS_DEFAULT_AUTOCAST   nullptr
#endif

#define CALCULUS_SIGNATURE(BoxPointerArray)  BoxPointerArray, std::extent<decltype(BoxPointerArray)>::value

#endif
#endif // HPP_ALIB_EXPRESSIONS_PP
