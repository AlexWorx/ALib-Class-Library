//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef H_ALIB_CLI
#define H_ALIB_CLI
#pragma once

#ifndef INL_ALIB
#   include "alib/alib.inl"
#endif

#if ALIB_CLI
#   if ALIB_C20_MODULES && !DOXYGEN
        import ALib.CLI;
#   elif !defined(ALIB_INC_CLI_CLI_MPP)
#       define ALIB_INC_CLI_CLI_MPP
#       include "alib/cli/cli.mpp"
#   endif
#endif

#endif // H_ALIB_CLI
