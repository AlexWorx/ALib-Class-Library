//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
#ifndef H_ALIB_FILETREE
#define H_ALIB_FILETREE
#pragma once
#ifndef HPP_ALIB
#   include "alib/alib.prepro.hpp"
#endif

#if ALIB_FILETREE
//=========================================== Dependencies =========================================
#include "ALib.Monomem.StdContainers.H"
#include "ALib.Monomem.SharedMonoVal.H"
#include "ALib.Containers.List.H"
#include "ALib.Containers.HashTable.H"
#include "ALib.Containers.LRUCacheTable.H"
#include "ALib.Containers.StringTree.H"
#include "ALib.Boxing.H"
#include "ALib.Strings.StdIOStream.H"
#include "ALib.Resources.H"
#include "ALib.System.H"
#include "ALib.Format.Paragraphs.H"
#include "ALib.Exceptions.H"
#include "ALib.Camp.H"

#include "alib/filetree/filetree.prepro.hpp"

#include <stack>
#include <bitset>
#include <vector>
#include <memory>

//============================================= Imports ============================================
#if ALIB_C20_MODULES
import ALib.FileTree;
#else
#include "alib/filetree/filetreecamp.hpp"
#include "alib/filetree/ftvalue.hpp"
#include "alib/filetree/ftree.hpp"
#include "alib/filetree/ffilter.hpp"
#include "alib/filetree/fscanner.hpp"

#endif // ALIB_C20_MODULES
#endif // ALIB_FILETREE
#endif // H_ALIB_FILETREE
