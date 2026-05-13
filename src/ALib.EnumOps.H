//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
#ifndef H_ALIB_ENUMOPS
#define H_ALIB_ENUMOPS
#pragma once
#ifndef HPP_ALIB
#   include "alib/alib.prepro.hpp"
#endif

//=========================================== Dependencies =========================================
#include "ALib.Lang.H"

#include "alib/enumops/enumops.prepro.hpp"

//============================================= Imports ============================================
#if ALIB_C20_MODULES
import ALib.EnumOps;
#else
#include "alib/enumops/underlyingintegral.hpp"
#include "alib/enumops/arithmetical.hpp"
#include "alib/enumops/bitwise.hpp"
#include "alib/enumops/iterable.hpp"
#include "alib/enumops/iterablebitset.hpp"
#include "alib/enumops/bitwise_iterable_conversion.hpp"

#endif // ALIB_C20_MODULES
#endif // H_ALIB_ENUMOPS
