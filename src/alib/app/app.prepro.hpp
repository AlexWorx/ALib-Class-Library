//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
#ifndef HPP_ALIB_APP_PP
#define HPP_ALIB_APP_PP
#pragma once
#ifndef HPP_ALIB
#   include "alib/alib.prepro.hpp"
#endif

//##################################################################################################
// Macro  introduced by module ALib.APP
//##################################################################################################

#if !DOXYGEN
#   undef  LOX_LOX
#   define LOX_LOX  (*::alib::app::APP_SINGLETON->GetRLox())
#endif


#endif // HPP_ALIB_APP_PP
