//==================================================================================================
/// \file
/// This header-file is part of the \aliblong. It does not belong to an \alibmod and is
/// included in any \alibbuild.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
///
/// \note
///   To reduce complexity, this header is not shown in inclusion graphs of this documentation.
//==================================================================================================
#ifndef HPP_ALIB_LANG_PLATFORM_INCLUDES
#define HPP_ALIB_LANG_PLATFORM_INCLUDES 1
#pragma once
#if !defined(DOXYGEN)
#   include "alib/alib.inl"
#endif

// #################################################################################################
// basic system headers
// #################################################################################################

// Windows specific
#if defined (_WIN32)

    //! @cond NO_DOX
    // Include windows.h. but exclude rarely-used stuff from Windows headers. If these things are
    // needed by the software that incorporates ALib, then windows has to be
    // included before this header-file is.
#   if !defined (_INC_WINDOWS)
#      define WIN32_LEAN_AND_MEAN
#      define NOGDI
#      define NOSERVICE
#      define NOMCX
#      define NOIME
#      define NOSOUND
#      define NOCOMM
#      define NOKANJI
#      define NORPC
#      define NOPROXYSTUB
#      define NOIMAGE
#      define NOTAPE
#      include <windows.h>
#   endif
    //! @endcond

#   if !defined (_STDINT)
#      include <stdint.h>
#   endif
#   if ALIB_DEBUG
#      include <assert.h>
#   endif
#endif


#endif // HPP_ALIB_LANG_PLATFORM_INCLUDES

