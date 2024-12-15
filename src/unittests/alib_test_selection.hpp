/** ************************************************************************************************
 * \file
 * This header file is part of the unit tests of the \aliblong.
 *
 * \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#if !DOXYGEN
#   include "alib/alib.hpp"
#endif

#ifndef HPP_ALIB_UNITTESTS_TEST_SELECTION
#define HPP_ALIB_UNITTESTS_TEST_SELECTION 1
#pragma once

#define ALIB_UT_COMPATIBILITY       1
#define ALIB_UT_DOCS              ( 1 )
#define ALIB_UT_LANG              ( 1                      )
#define ALIB_UT_ALOX              ( 1 & ALIB_ALOX          )
#define ALIB_UT_BITBUFFER         ( 1 & ALIB_BITBUFFER     )
#define ALIB_UT_BOXING            ( 1 & ALIB_BOXING        )
#define ALIB_UT_CAMP              ( 1 & ALIB_CAMP          )
#define ALIB_UT_CONFIGURATION     ( 1 & ALIB_CONFIGURATION )
#define ALIB_UT_CLI               ( 1 & ALIB_CLI           )
#define ALIB_UT_ENUMS             ( 1 & ALIB_ENUMS         )
#define ALIB_UT_EXPRESSIONS       ( 1 & ALIB_EXPRESSIONS   )
#define ALIB_UT_FILES             ( 1 & ALIB_FILES         )
#define ALIB_UT_MONOMEM           ( 1 & ALIB_MONOMEM       )
#define ALIB_UT_STRINGS           ( 1 & ALIB_STRINGS       )
#define ALIB_UT_THREADS           ( 1 & ALIB_THREADS       )
#define ALIB_UT_THREADMODEL       ( 1 & ALIB_THREADMODEL   )
#define ALIB_UT_TIME              ( 1 & ALIB_TIME          )

#endif // HPP_ALIB_UNITTESTS_TEST_SELECTION
