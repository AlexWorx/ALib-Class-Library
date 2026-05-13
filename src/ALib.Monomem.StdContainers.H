//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
#ifndef H_ALIB_MONOMEM_STDCONTAINERS
#define H_ALIB_MONOMEM_STDCONTAINERS
#pragma once
#ifndef HPP_ALIB
#   include "alib/alib.prepro.hpp"
#endif

#if ALIB_MONOMEM
//=========================================== Dependencies =========================================
#include <vector>
#include <list>
#include <deque>
#include "ALib.Monomem.H"

//============================================= Imports ============================================
#if ALIB_C20_MODULES
import ALib.Monomem.StdContainers;
#else
#include "alib/monomem/stdcontainers.hpp"

#endif // ALIB_C20_MODULES
#endif // ALIB_MONOMEM
#endif // H_ALIB_MONOMEM_STDCONTAINERS
