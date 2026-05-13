//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
#ifndef H_ALIB_ALOX_IMPL
#define H_ALIB_ALOX_IMPL
#pragma once
#ifndef HPP_ALIB
#   include "alib/alib.prepro.hpp"
#endif

#if ALIB_ALOX
//=========================================== Dependencies =========================================
#include "ALib.ALox.H"
#include "ALib.Monomem.StdContainers.H"
#include "ALib.Containers.LRUCacheTable.H"
#include "ALib.Strings.StdIOStream.H"
#include "ALib.Strings.AutoSizes.H"
#include "ALib.Strings.Calendar.H"
#include "ALib.System.H"
#include "ALib.Format.H"
#include "ALib.Camp.H"


//============================================= Imports ============================================
#if ALIB_C20_MODULES
import ALib.ALox.Impl;
#else
#include "alib/alox/aloxcamp.hpp"

#include "alib/alox/detail/logger.hpp"
#include "alib/alox/detail/domain.hpp"
#include "alib/alox/detail/scopestore.hpp"
#include "alib/alox/detail/scopeinfo.hpp"
#include "alib/alox/detail/scopedump.hpp"

#include "alib/alox/textlogger/variables.hpp"
#include "alib/alox/textlogger/textlogger.hpp"
#include "alib/alox/textlogger/plaintextlogger.hpp"

#include "alib/alox/loggers/ansilogger.hpp"
#include "alib/alox/loggers/consolelogger.hpp"
#include "alib/alox/loggers/memorylogger.hpp"
#include "alib/alox/loggers/textfilelogger.hpp"
#include "alib/alox/loggers/vstudiologger.hpp"
#include "alib/alox/loggers/windowsconsolelogger.hpp"

#endif // ALIB_C20_MODULES
#endif // ALIB_ALOX
#endif // H_ALIB_ALOX_IMPL
