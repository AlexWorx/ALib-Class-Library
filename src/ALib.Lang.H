//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
#ifndef H_ALIB_LANG
#define H_ALIB_LANG
#pragma once
#ifndef HPP_ALIB
#   include "alib/alib.prepro.hpp"
#endif

//=========================================== Dependencies =========================================
#if defined(_WIN32)
#include <intrin.h>
#endif
#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstring>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <typeindex>
#include <vector>

#if ALIB_DEBUG
#include <any>
#include <iostream>
#endif

//============================================= Imports ============================================
#if ALIB_C20_MODULES
import ALib.Lang;
#else
#include "alib/lang/callerinfo.hpp"
#include "alib/lang/assert.hpp"
#include "alib/lang/mainargs.hpp"
#include "alib/lang/chk_nc.hpp"
#include "alib/lang/tmp.hpp"
#include "alib/lang/placeholder.hpp"
#include "alib/lang/integers.hpp"
#include "alib/lang/dbgtypedemangler.hpp"
#include "alib/lang/dbgcriticalsections.hpp"
#include "alib/lang/sidilist.hpp"
#include "alib/lang/bidilist.hpp"
#include "alib/lang/allocation.hpp"
#include "alib/lang/commonenumdefs.hpp"
#include "alib/lang/bits.hpp"
#include "alib/lang/bitset.hpp"
#include "alib/lang/owner.hpp"
#include "alib/lang/plugins.hpp"
#include "alib/lang/stdtypeinfofunctors.hpp"

#include "alib/characters/chartypes.hpp"
#include "alib/characters/chartraits.hpp"
#include "alib/characters/chartraits_stdtypes.hpp"
#endif // ALIB_C20_MODULES
#endif // H_ALIB_LANG
