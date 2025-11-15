//##################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
#include "alib_precompile.hpp"
#if !defined(ALIB_C20_MODULES) || ((ALIB_C20_MODULES != 0) && (ALIB_C20_MODULES != 1))
#   error "Symbol ALIB_C20_MODULES has to be given to the compiler as either 0 or 1"
#endif
#if ALIB_C20_MODULES
    module;
#endif
//========================================= Global Fragment ========================================
#include "alib/system/system.prepro.hpp"
#if !DOXYGEN
#   if   defined(__GLIBCXX__) || defined(_LIBCPP_VERSION) || defined(__ANDROID_NDK__)
#   elif defined(__APPLE__)
#   elif defined( _WIN32 )
#   else
#      pragma message ("Unknown Platform in file: " __FILE__ )
#   endif
#   include <fstream>
#endif // !DOXYGEN
//============================================== Module ============================================
#if ALIB_C20_MODULES
    module ALib.System;
    import   ALib.Lang;
    import   ALib.Strings;
    import   ALib.Strings.Tokenizer;
#else
#   include "ALib.System.H"
#   if ALIB_MONOMEM
#       include "ALib.Strings.Monomem.H"
#   endif
#endif
//========================================== Implementation ========================================
namespace alib {  namespace system {


#if !DOXYGEN
// template instantiations
template int alib::system::TShellCommand<lang::HeapAllocator>::Run(
    const NCString&,
    strings::TAString<nchar, lang::HeapAllocator>&,
    strings::util::TStringVector<nchar, lang::HeapAllocator>*     );

#if ALIB_MONOMEM
template int alib::system::TShellCommand<MonoAllocator>::Run(
    const NCString&,
    strings::TAString<nchar, MonoAllocator>&,
    strings::util::TStringVector<nchar, MonoAllocator>*           );
#endif
#endif // !DOXYGEN

}} // namespace [alib::system]
