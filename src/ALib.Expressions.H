//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
#ifndef H_ALIB_EXPRESSIONS
#define H_ALIB_EXPRESSIONS
#pragma once
#ifndef HPP_ALIB
#   include "alib/alib.prepro.hpp"
#endif

#if ALIB_EXPRESSIONS
//=========================================== Dependencies =========================================
#include "ALib.Singletons.H"
#include "ALib.Monomem.StdContainers.H"
#include "ALib.Monomem.SharedMonoVal.H"
#include "ALib.Boxing.StdFunctors.H"
#include "ALib.Strings.StdFunctors.H"
#include "ALib.Strings.Vector.H"
#include "ALib.Strings.Token.H"
#include "ALib.Strings.StdFunctors.H"
#include "ALib.Containers.List.H"
#include "ALib.Containers.HashTable.H"
#include "ALib.EnumRecords.H"
#include "ALib.Resources.H"
#include "ALib.Format.H"
#include "ALib.Exceptions.H"
#include "ALib.Variables.H"
#include "ALib.Camp.H"

#include "alib/expressions/expressions.prepro.hpp"

#include <stack>
#include <bitset>
#include <vector>
//============================================= Imports ============================================
#if ALIB_C20_MODULES
import ALib.Expressions;
#else
#include "alib/expressions/detail/ast.hpp"
#include "alib/expressions/parser.hpp"
#include "alib/expressions/expressionscamp.hpp"
#include "alib/expressions/expression.hpp"
#include "alib/expressions/scope.hpp"
#include "alib/expressions/compiler.hpp"
#include "alib/expressions/compilerplugin.hpp"
#include "alib/expressions/standardrepository.hpp"
#include "alib/expressions/util/expressionformatter.hpp"

#endif // ALIB_C20_MODULES
#endif // ALIB_EXPRESSIONS
#endif // H_ALIB_EXPRESSIONS
