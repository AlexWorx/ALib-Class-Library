/** ************************************************************************************************
 * \file
 * This header file is part of the unit tests of the \aliblong.
 *
 * \emoji :copyright: 2013-2019 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#if !defined(HPP_ALIB_MODULES) && !defined(ALIB_DOX)
#   include "alib/lib/modules.hpp"
#endif


#ifndef HPP_ALIB_UNITTESTS_TEST_SELECTION
#define HPP_ALIB_UNITTESTS_TEST_SELECTION 1



#define ALIB_UT_MONOMEM           ( 1 & ALIB_MONOMEM       )
#define ALIB_UT_BOXING            ( 1 & ALIB_BOXING        )
#define ALIB_UT_STRINGS           ( 1 & ALIB_STRINGS       )
#define ALIB_UT_THREADS           ( 1 & ALIB_THREADS       )
#define ALIB_UT_TIME              ( 1 & ALIB_TIME          )
#define ALIB_UT_RESOURCES         ( 1 & ALIB_RESOURCES     )
#define ALIB_UT_SYSTEM            ( 1 & ALIB_SYSTEM        )
#define ALIB_UT_CONFIG            ( 1 & ALIB_CONFIGURATION )
#define ALIB_UT_ALOX              ( 1 & ALIB_ALOX          )
#define ALIB_UT_EXPRESSIONS       ( 1 & ALIB_EXPRESSIONS   )

#define ALIB_UT_COMPATIBILITY       1
#define ALIB_UT_DOCS              ( 1 & ALIB_ALOX             \
                                      & ALIB_EXPRESSIONS      \
                                      & ALIB_CLI           )

#endif // HPP_ALIB_UNITTESTS_TEST_SELECTION
