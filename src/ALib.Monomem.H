//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
#ifndef H_ALIB_MONOMEM
#define H_ALIB_MONOMEM
#pragma once
#ifndef HPP_ALIB
#   include "alib/alib.prepro.hpp"
#endif

#if ALIB_MONOMEM
//=========================================== Dependencies =========================================
#include "ALib.Threads.H"

#include "alib/monomem/monomem.prepro.hpp"

#include <cstdlib>
#include <memory>
#include <cstddef>
#include <cstring>

//============================================= Imports ============================================
#if ALIB_C20_MODULES
import ALib.Monomem;
#else
#include "alib/monomem/monoallocator.hpp"
#include "alib/monomem/localallocator.hpp"
#include "alib/monomem/poolallocator.hpp"

#endif // ALIB_C20_MODULES
#endif // ALIB_MONOMEM
#endif // H_ALIB_MONOMEM
