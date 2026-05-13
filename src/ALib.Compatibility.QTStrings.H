//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
///
/// <b>Legal Notice:</b>
/// This is an optional extension header to provide compatibility between \alib and
/// the QT class library.
/// All information about QT is found at https://www.qt.io
/// \alib otherwise does not use or rely on QT.
/// The use of QT is bound to the QT license restrictions.
//==================================================================================================
#ifndef HPP_ALIB_COMPATIBILITY_QT_STRINGS
#define HPP_ALIB_COMPATIBILITY_QT_STRINGS 1
#pragma once
#ifndef HPP_ALIB
#   include "alib/alib.prepro.hpp"
#endif

#if !defined(ALIB_QT_LIB_NOT_FOUND) // this is only set in an automated test project

//=========================================== Dependencies =========================================
#include "ALib.Compatibility.QTCharacters.H"
#include "ALib.Strings.H"

//============================================= Includes ===========================================
#include "alib/strings/qtstrings.hpp"

#endif // !defined(ALIB_QT_LIB_NOT_FOUND) // this is only set in an automated test project
#endif // HPP_ALIB_COMPATIBILITY_QT_STRINGS
