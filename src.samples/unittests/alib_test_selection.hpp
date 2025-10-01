/** ************************************************************************************************
 * \file
 * This header-file is part of the unit tests of the \aliblong.
 *
 * \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#include "ALib.Lang.H"

#ifndef HPP_ALIB_UNITTESTS_TEST_SELECTION
#define HPP_ALIB_UNITTESTS_TEST_SELECTION 1
#pragma once

#define ALIB_UT_COMPATIBILITY       1
#define ALIB_UT_DOCS              ( 1 )
#define ALIB_UT_LANG              ( 1                      )
#define ALIB_UT_TIME              ( 1                      )
#define ALIB_UT_ALOX              ( 1 & ALIB_ALOX          )
#define ALIB_UT_BITBUFFER         ( 1 & ALIB_BITBUFFER     )
#define ALIB_UT_BOXING            ( 1 & ALIB_BOXING        )
#define ALIB_UT_CAMP              ( 1 & ALIB_CAMP          )
#define ALIB_UT_VARIABLES     ( 1 & ALIB_VARIABLES )
#define ALIB_UT_CLI               ( 1 & ALIB_CLI           )
#define ALIB_UT_ENUMRECORDS             ( 1 & ALIB_ENUMRECORDS         )
#define ALIB_UT_EXPRESSIONS       ( 1 & ALIB_EXPRESSIONS   )
#define ALIB_UT_FILES             ( 1 & ALIB_FILES         )
#define ALIB_UT_MONOMEM           ( 1 & ALIB_MONOMEM       )
#define ALIB_UT_STRINGS           ( 1 & ALIB_STRINGS       )
#define ALIB_UT_THREADS           ( 1 & !ALIB_SINGLE_THREADED       )
#define ALIB_UT_THREADMODEL       ( 1 & ALIB_THREADMODEL   )

#endif // HPP_ALIB_UNITTESTS_TEST_SELECTION
