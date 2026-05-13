//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
#ifndef H_ALIB_ENUMRECORDS
#define H_ALIB_ENUMRECORDS
#pragma once
#ifndef HPP_ALIB
#   include "alib/alib.prepro.hpp"
#endif

#if ALIB_ENUMRECORDS
//=========================================== Dependencies =========================================
#include "ALib.Singletons.H"
#include "ALib.Containers.HashTable.H"
#include "ALib.Monomem.H"
#include "ALib.Strings.H"
#include "ALib.Boxing.H"

#include "alib/enumrecords/enumrecords.prepro.hpp"

#if !ALIB_CONTAINERS
#include <unordered_map>
#endif

#if ALIB_BOXING && ALIB_DEBUG_BOXING
#include <vector>
#endif
//============================================= Imports ============================================
#if ALIB_C20_MODULES
import ALib.EnumRecords;
#else
#include "alib/enumrecords/detail/enumrecordmap.hpp"
#include "alib/enumrecords/records.hpp"
#include "alib/enumrecords/serialization.hpp"
#include "alib/enumrecords/builtin.hpp"

#if ALIB_BOXING
#include "alib/enumrecords/boxing_enums.hpp"
#endif

#endif // ALIB_C20_MODULES
#endif // ALIB_ENUMRECORDS
#endif // H_ALIB_ENUMRECORDS
