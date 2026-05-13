/** ************************************************************************************************
 * \file
 * This header-file is part of the \aliblong. It does not belong to an \alibmod and is
 * included in any \alibbuild.
 *
 * Copyright 2013-2026 A-Worx GmbH, Germany.
 * Published under #"mainpage_license".
 *
 * \note
 *   To reduce their complexity, this header is not shown in inclusion graphs of this documentation.
 **************************************************************************************************/
#undef ALIB_CALLER
#if ALIB_EXT_LIB_THREADS_AVAILABLE
#   define ALIB_CALLER      {__FILE__,__LINE__,UT_GET_TEST_NAME,::std::this_thread::get_id(),&typeid(*this)}
#else
#   define ALIB_CALLER      {__FILE__,__LINE__,UT_GET_TEST_NAME,&typeid(*this)}
#endif

