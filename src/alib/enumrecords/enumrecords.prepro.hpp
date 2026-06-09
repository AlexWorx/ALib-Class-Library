//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
#ifndef HPP_ALIB_ENUMRECORDS_PP
#define HPP_ALIB_ENUMRECORDS_PP
#pragma once
#ifndef HPP_ALIB
#   include "alib/alib.prepro.hpp"
#endif
#if ALIB_ENUMRECORDS

//##################################################################################################
// Macro  introduced by module ALib.Enums
//##################################################################################################
#define  ALIB_ENUMS_ASSIGN_RECORD( TEnum, TRecord )                                                \
namespace alib::enumrecords {                                                                      \
template<> struct RecordsTraits<TEnum>     { using  Type=  TRecord; };   }

#endif
#endif // HPP_ALIB_ENUMRECORDS_PP
