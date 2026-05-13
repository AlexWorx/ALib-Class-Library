//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
#ifndef H_ALIB_BITBUFFER
#define H_ALIB_BITBUFFER
#pragma once
#ifndef HPP_ALIB
#   include "alib/alib.prepro.hpp"
#endif

#if ALIB_BITBUFFER
//=========================================== Dependencies =========================================
#include "ALib.Monomem.StdContainers.H"
#include "ALib.Strings.H"
#include "ALib.EnumRecords.H"

#include "alib/bitbuffer/bitbuffer.prepro.hpp"
//============================================= Imports ============================================
#if ALIB_C20_MODULES
import ALib.BitBuffer;
#else
#include "alib/bitbuffer/bitbuffer.hpp"
#include "alib/bitbuffer/ac_v1/huffman.hpp"
#include "alib/bitbuffer/ac_v1/ac.hpp"
#include "alib/bitbuffer/arraycompressor.hpp"

#endif
#endif // ALIB_C20_MODULES
#endif // H_ALIB_BITBUFFER
