// #################################################################################################
//  aworx::lib::lox - ALox Logging Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################

// Note: This file is a sort of "shortcut header" file to identically named header file within
//       the sub folder "alox".
//       The reason for its existence is that this header can conveniently be used as an alternative
//       to including "alib/alib.hpp"

#ifndef HPP_ALOX
#define HPP_ALOX 1

#if !defined(x)
#   include "alib/alox/alox.hpp"
#endif

#if !defined(HPP_ALOX_CONSOLE_LOGGER)
#   include "alib/alox/loggers/consolelogger.hpp"
#endif

#if !defined(HPP_ALOX_ANSI_LOGGER)
#   include "alib/alox/loggers/ansilogger.hpp"
#endif

#if !defined(HPP_ALOX_VSTUDIO_LOGGER)
#   include "alib/alox/loggers/vstudiologger.hpp"
#endif

#if !defined(HPP_ALOX_WINDOWS_CONSOLE_LOGGER)
#   include "alib/alox/loggers/windowsconsolelogger.hpp"
#endif


#endif // HPP_ALOX
