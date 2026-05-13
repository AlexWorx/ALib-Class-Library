//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
#ifndef H_ALIB_BOOTSTRAP
#define H_ALIB_BOOTSTRAP
#pragma once
#ifndef HPP_ALIB
#   include "alib/alib.prepro.hpp"
#endif

//=========================================== Dependencies =========================================
#include "alib/singletons/singletons.prepro.hpp"
#include "alib/boxing/boxing.prepro.hpp"
#include "alib/strings/strings.prepro.hpp"
#include "alib/resources/resources.prepro.hpp"
#include "alib/bitbuffer/bitbuffer.prepro.hpp"
#include "alib/alox/alox.prepro.hpp"
#include "alib/bootstrap/bootstrap.prepro.hpp"

#include "ALib.Monomem.H"
#include "ALib.Containers.List.H"
#include "ALib.Camp.H"

//============================================= Imports ============================================
#if ALIB_C20_MODULES
import ALib.Bootstrap;
#else
#include "alib/bootstrap/bootstrap.hpp"
#endif // ALIB_C20_MODULES
#endif // H_ALIB_BOOTSTRAP
