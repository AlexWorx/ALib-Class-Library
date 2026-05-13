//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
#ifndef H_ALIB_TIME
#define H_ALIB_TIME
#pragma once
#ifndef HPP_ALIB
#   include "alib/alib.prepro.hpp"
#endif

//=========================================== Dependencies =========================================
#include "ALib.Lang.H"
#include "alib/platform.prepro.hpp"
#include <chrono>
#include <cmath>
//============================================= Imports ============================================
#if ALIB_C20_MODULES
import ALib.Time;
#else
#include "alib/time/timepointbase.hpp"
#include "alib/time/datetime.hpp"
#include "alib/time/ticks.hpp"
#include "alib/time/tickconverter.hpp"
#include "alib/time/stopwatch.hpp"
#endif // ALIB_C20_MODULES
#endif // H_ALIB_TIME
