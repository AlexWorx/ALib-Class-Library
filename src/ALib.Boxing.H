//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
#ifndef H_ALIB_BOXING
#define H_ALIB_BOXING
#pragma once
#ifndef HPP_ALIB
#   include "alib/alib.prepro.hpp"
#endif

#if ALIB_BOXING
//=========================================== Dependencies =========================================
#include "ALib.Singletons.H"
#include "ALib.Threads.H"
#include "ALib.Containers.HashTable.H"
#include "ALib.Monomem.H"
#include "ALib.Strings.Token.H"

#include "alib/boxing/boxing.prepro.hpp"

#if ALIB_DEBUG && (!ALIB_MONOMEM || !ALIB_CONTAINERS)
#include <unordered_set>
#include <unordered_map>
#endif

#if ALIB_DEBUG_BOXING
#include <tuple>
#endif

#include <cstring>
#include <functional>
#include <vector>
#include <typeindex>
#include <array>

//============================================= Imports ============================================
#if ALIB_C20_MODULES
import ALib.Boxing;
#else
#include "alib/boxing/placeholder.hpp"
#include "alib/boxing/boxingtraits.hpp"
#include "alib/boxing/functiondecls.hpp"
#include "alib/boxing/detail/vtable.hpp"
#include "alib/boxing/box.hpp"
#include "alib/boxing/boxes.hpp"
#include "alib/boxing/enum.hpp"
#include "alib/boxing/boxing_debug.hpp"
#include "alib/boxing/boxstrap.hpp"

#include "alib/boxing/strings_appendables.hpp"
#include "alib/boxing/extensions/strings_tfield.hpp"


#endif // ALIB_C20_MODULES
#endif // ALIB_BOXING
#endif // H_ALIB_BOXING
