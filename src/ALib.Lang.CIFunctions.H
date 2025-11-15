//==================================================================================================
/// \file
/// This header-file is part of the \aliblong. It does not belong to an \alibmod and is
/// included in any \alibbuild.
///
/// \note This is a special header-file which has to be included for switchng preprocessor macro
///       \ref ALIB_CALLER and a bigger list of macros that are using it.<br>
///       Please refer to manual section \ref alib_manual_appendix_callerinfo for details.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
///
/// \note
///   To reduce their complexity, this header is not shown in inclusion graphs of this documentation.
//==================================================================================================
#if !DOXYGEN
#   undef ALIB_CALLER
#   if ALIB_EXT_LIB_THREADS_AVAILABLE
#      define ALIB_CALLER      {__FILE__,__LINE__,ALIB_CALLER_FUNC,::std::this_thread::get_id(),nullptr}
#   else
#      define ALIB_CALLER      {__FILE__,__LINE__,ALIB_CALLER_FUNC,nullptr}
#   endif
#endif
