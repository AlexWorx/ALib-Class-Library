//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
#ifndef H_ALIB_FILETREE_EXPRESSIONS
#define H_ALIB_FILETREE_EXPRESSIONS
#pragma once
#ifndef HPP_ALIB
#   include "alib/alib.prepro.hpp"
#endif

#if ALIB_FILETREE && ALIB_EXPRESSIONS
//=========================================== Dependencies =========================================
#include "ALib.Monomem.StdContainers.H"
#include "ALib.Expressions.Impl.H"
#include "ALib.FileTree.H"
#include "ALib.Expressions.H"

#include <fstream>

//============================================= Imports ============================================
#if ALIB_C20_MODULES
import ALib.FileTree.Expressions;
#else
#include "alib/filetree/expressions/fileexpressions.hpp"


#endif // ALIB_C20_MODULES
#endif // ALIB_FILETREE && ALIB_EXPRESSIONS
#endif // H_ALIB_FILETREE_EXPRESSIONS
