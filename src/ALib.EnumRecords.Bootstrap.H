//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
#ifndef H_ALIB_ENUMS_RECORDBOOTSTRAP
#define H_ALIB_ENUMS_RECORDBOOTSTRAP
#pragma once
#ifndef HPP_ALIB
#   include "alib/alib.prepro.hpp"
#endif


#if ALIB_ENUMRECORDS
//=========================================== Dependencies =========================================
#include "ALib.Monomem.H"
#include "ALib.Strings.H"
#include "ALib.EnumRecords.H"

#include <initializer_list>
//============================================= Imports ============================================
#if ALIB_C20_MODULES
import ALib.EnumRecords.Bootstrap;
#else
#include "alib/enumrecords/bootstrap/recordparser.hpp"
#include "alib/enumrecords/bootstrap/recordbootstrap.hpp"

#endif // ALIB_C20_MODULES
#endif // ALIB_ENUMS_RECORDS
#endif // H_ALIB_ENUMS_RECORDBOOTSTRAP
