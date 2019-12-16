// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)
#if !defined(HPP_ALIB_COMPILERS)
#   include "alib/lib/compilers.hpp"
#endif
#endif // !defined(ALIB_DOX)

#if ALIB_DEBUG

#if !defined(ALIB_DOX)
#if !defined (HPP_ALIB_FS_DEBUG_TYPEDEMANGLER)
#   include "alib/lib/fs_debug/typedemangler.hpp"
#endif

#if defined(__GNUC__) || defined(__clang__)
#   include <cxxabi.h>
#   include <cassert>
#endif

#if !defined(_GLIBCXX_CSTDLIB) && !defined(_CSTDLIB_)
#   include <cstdlib>
#endif

#if defined(_WIN32) && !defined(_CSTRING_)
#   include <cstring>
#endif
#endif // !defined(ALIB_DOX)

namespace aworx { namespace lib {

    #if defined(__GNUC__) || defined(__clang__)

        DbgTypeDemangler::DbgTypeDemangler( const std::type_info& typeInfo )
        {
            int status;
            name= abi::__cxa_demangle( typeInfo.name(), nullptr, nullptr, &status);
            assert( status==0 );
        }

        DbgTypeDemangler::~DbgTypeDemangler()
        {
            if ( name )
                std::free(static_cast<void*>( const_cast<char*>(name) ) );
        }

        const char* DbgTypeDemangler::Get()
        {
            return name;
        }

    #else
        DbgTypeDemangler::DbgTypeDemangler( const type_info& typeInfo )
        {
            name= typeInfo.name();
            if (std::strncmp( name, "class ", 6) == 0)
                name+= 6;
        }

        DbgTypeDemangler::~DbgTypeDemangler()
        {
        }

        const char* DbgTypeDemangler::Get()
        {
            return name;
        }
    #endif

}} // namespace [aworx::lib}
#endif

