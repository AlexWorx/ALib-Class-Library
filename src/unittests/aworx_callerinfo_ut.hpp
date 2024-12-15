/** ************************************************************************************************
 * \file
 * This header file is part of the \aliblong. It does not belong to an \alibmod and is
 * included in any \alibdist.
 *
 * \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 *
 * \note
 *   To reduce their complexity, this header is not shown in inclusion graphs of this documentation.
 **************************************************************************************************/
#include "alib/alib.hpp"

#undef ALIB_CALLER
#if ALIB_EXT_LIB_THREADS_AVAILABLE
#   define ALIB_CALLER      {__FILE__,__LINE__,UT_GET_TEST_NAME,::std::this_thread::get_id(),&typeid(*this)}
#else
#   define ALIB_CALLER      {__FILE__,__LINE__,UT_GET_TEST_NAME,&typeid(*this)}
#endif

