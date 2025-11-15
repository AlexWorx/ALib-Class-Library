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
#include "alib/alib.inl"
#if ALIB_DEBUG && !DOXYGEN
#   if defined(__GNUC__) || defined(__clang__)
#      include <cxxabi.h>
#      include <cassert>
#   endif
#   include <cstdlib>
#   if defined(_WIN32)
#      include <cstring>
#   endif
#endif
//============================================== Module ============================================
#if ALIB_C20_MODULES
    module ALib.Lang;
#else
#   include "ALib.Lang.H"
#endif
//========================================== Implementation ========================================

//##################################################################################################
// Type De-mangling
//##################################################################################################
#if ALIB_DEBUG && !DOXYGEN
namespace alib::lang {

    #if defined(__GNUC__) || defined(__clang__)
DbgTypeDemangler::DbgTypeDemangler( const std::type_info& typeInfo ) {
    int status;
    name= abi::__cxa_demangle( typeInfo.name(), nullptr, nullptr, &status);
    assert( status==0 || !name );
    if ((failed= (name==nullptr)))
        name= "<DbgTypeDemangler Error>";
}

DbgTypeDemangler::~DbgTypeDemangler()
{ if (!failed) std::free(static_cast<void*>( const_cast<char*>(name) ) ); }

    #else
        lang::DbgTypeDemangler::DbgTypeDemangler( const type_info& typeInfo )
        {
            name= typeInfo.name();
            if (std::strncmp( name, "class ", 6) == 0)
                name+= 6;
        }
    #endif

const char* DbgTypeDemangler::Get()                                                 { return name; }

} // namespace [alib::lang}

#endif // ALIB_DEBUG && !DOXYGEN
