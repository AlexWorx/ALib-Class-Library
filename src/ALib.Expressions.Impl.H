//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
#ifndef H_ALIB_EXPRESSIONS_IMPL
#define H_ALIB_EXPRESSIONS_IMPL
#pragma once
#ifndef HPP_ALIB
#   include "alib/alib.prepro.hpp"
#endif

#if ALIB_EXPRESSIONS
//=========================================== Dependencies =========================================
#include "ALib.Expressions.H"

//============================================= Imports ============================================
#if ALIB_C20_MODULES
import ALib.Expressions.Impl;
#else
#include "alib/expressions/plugins/calculus.hpp"
#include "alib/expressions/plugins/arithmetics.hpp"
#include "alib/expressions/plugins/autocast.hpp"
#include "alib/expressions/plugins/dateandtime.hpp"
#include "alib/expressions/plugins/elvisoperator.hpp"
#include "alib/expressions/plugins/math.hpp"
#include "alib/expressions/plugins/strings.hpp"

#include "alib/expressions/detail/ast_impl.hpp"
#include "alib/expressions/detail/virtualmachine.hpp"
#include "alib/expressions/detail/parser_impl.hpp"
#include "alib/expressions/detail/program.hpp"

#endif // ALIB_C20_MODULES
#endif // ALIB_EXPRESSIONS
#endif // H_ALIB_EXPRESSIONS_IMPL
