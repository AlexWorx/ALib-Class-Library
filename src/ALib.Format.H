//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
#ifndef H_ALIB_FORMAT
#define H_ALIB_FORMAT
#pragma once
#ifndef HPP_ALIB
#   include "alib/alib.prepro.hpp"
#endif

#if ALIB_FORMAT
//=========================================== Dependencies =========================================
#include "ALib.Monomem.StdContainers.H"
#include "ALib.Boxing.H"
#include "ALib.Containers.SharedPtr.H"
#include "ALib.EnumRecords.H"
#include "ALib.Resources.H"
#include "ALib.Exceptions.H"

#include <stack>

//============================================= Imports ============================================
#if ALIB_C20_MODULES
import ALib.Format;
#else
#include "alib/format/formatter.hpp"
#include "alib/format/fmtexceptions.hpp"
#include "alib/format/fmtcallerinfo.hpp"
#include "alib/format/fmtdatetime.hpp"
#include "alib/format/bytesize.hpp"

#include "alib/format/extensions/containers_hashtable.hpp"
#include "alib/format/extensions/boxing_format_debug.hpp"
#include "alib/format/extensions/resources_debug.hpp"

#endif // ALIB_C20_MODULES
#endif // ALIB_FORMAT
#endif // H_ALIB_FORMAT
