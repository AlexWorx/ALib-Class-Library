//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
#ifndef H_ALIB_RESOURCES
#define H_ALIB_RESOURCES
#pragma once
#ifndef HPP_ALIB
#   include "alib/alib.prepro.hpp"
#endif

#if ALIB_RESOURCES
//=========================================== Dependencies =========================================
#include "ALib.Monomem.H"
#include "ALib.Containers.HashTable.H"
#include "ALib.Containers.SharedPtr.H"
#include "ALib.Strings.Token.H"
#include "ALib.EnumRecords.Bootstrap.H"

#include "alib/resources/resources.prepro.hpp"

#if ALIB_DEBUG_RESOURCES
#include <vector>
#endif

//============================================= Imports ============================================
#if ALIB_C20_MODULES
import ALib.Resources;
#else
#include "alib/resources/resources.hpp"
#include "alib/resources/detail/resourcemap.hpp"
#include "alib/resources/localresourcepool.hpp"
#include "alib/resources/extensions/strings_loadtokens.hpp"
#if ALIB_ENUMRECORDS
#include "alib/resources/extensions/enums_serialization.hpp"
#endif

#endif // ALIB_C20_MODULES
#endif // ALIB_RESOURCES
#endif // H_ALIB_RESOURCES
