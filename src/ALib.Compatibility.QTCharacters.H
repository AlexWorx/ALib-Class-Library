//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
///
/// <b>Legal Notice:</b>
/// This is an optional extension header to provide compatibility between ALib and
/// the QT class library.
/// All information about QT is found at https://www.qt.io
/// ALib otherwise does not use or rely on QT.
/// The use of QT is bound to the QT license restrictions.
//==================================================================================================
#ifndef HPP_ALIB_COMPATIBILITY_QT_CHARACTERS
#define HPP_ALIB_COMPATIBILITY_QT_CHARACTERS 1
#pragma once
#ifndef HPP_ALIB
#   include "alib/alib.prepro.hpp"
#endif

#if !defined(ALIB_QT_LIB_NOT_FOUND) // this is only set in an automated test project
//=========================================== Dependencies =========================================

// include QT
#if defined(__clang__)
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wreserved-id-macro"
    #pragma clang diagnostic ignored "-Wredundant-parens"
    #pragma clang diagnostic ignored "-Wcovered-switch-default"
    #pragma clang diagnostic ignored "-Wsign-conversion"
    #pragma clang diagnostic ignored "-Wdeprecated"
    #pragma clang diagnostic ignored "-Wunused-variable"
    #pragma clang diagnostic ignored "-Wcomma"
    #pragma clang diagnostic ignored "-Wduplicate-enum"
#endif
#include <QtCore/QString>
#include <QtCore/QVector>

#if defined(__clang__)
    #pragma clang diagnostic pop
#endif

#if defined(H_ALIB_BOXING)
#   error "Wrong inclusion order. Header 'alib/compatibility/qt_characters.hpp' has to be included before headers of module ALib Boxing."
#endif

#include "ALib.Lang.H"

//============================================= Includes ===========================================
#include "alib/characters/chartraits_qt.hpp"

#endif // !defined(ALIB_QT_LIB_NOT_FOUND) // this is only set in an automated test project
#endif // HPP_ALIB_COMPATIBILITY_QT_CHARACTERS
