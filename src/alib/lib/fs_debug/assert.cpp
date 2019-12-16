// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)
#if !defined(HPP_ALIB_FS_DEBUG_ASSERT)
#   include "alib/lib/fs_debug/assert.hpp"
#endif

#if ALIB_DEBUG
#   include <iostream>
#   include <cassert>

    #if defined( _WIN32 ) && !defined(_STRING_)
    #   include <string>
    #endif
#endif
#endif // !defined(ALIB_DOX)


// #################################################################################################
// Debug functions
// #################################################################################################
#if ALIB_DEBUG

namespace aworx { namespace lib {


void (*DBG_SIMPLE_ALIB_MSG_PLUGIN)( const char* file, int line, const char* method,
                                    int type, int qtyMsgs, const char** msgs        )      =nullptr;


//! @cond NO_DOX
void DbgSimpleALibMsg( const char* file, int line, const char* method, int type,
                       const char* msg1, const char* msg2, const char* msg3,
                       const char* msg4, const char* msg5                         )
{

    int   qtyMsgs=  msg2 != nullptr ? (msg3 != nullptr ? (msg4 != nullptr ? (msg5 != nullptr ? 5 : 4) : 3) : 2) : 1;
    const char* msgs[5]= { msg1, msg2, msg3, msg4, msg5 };

    if( DBG_SIMPLE_ALIB_MSG_PLUGIN )
    {
        DBG_SIMPLE_ALIB_MSG_PLUGIN( file, line, method, type, qtyMsgs, msgs );
        return;
    }

    // internal implementation
    if( type == 0 )
        std::cout << "ALib Error: ";
    else
        std::cout << "ALib Warning (type=" << type << "): ";

    for (int i= 0; i != qtyMsgs; ++i )
        std::cout << msgs[i];
    std::cout << std::endl;
    std::cout << "At        : " << file << ':' << line << ' ' << method << "()" << std::endl;
    assert(type);
}

void DbgSimpleALibMsg( const char* file, int line, const char* method, int type,
                       const char* msg, integer intVal )
{
    std::basic_string<char> sIntVal=       std::to_string( intVal );
    if( DBG_SIMPLE_ALIB_MSG_PLUGIN )
    {
        const char* msgs[2]= { msg, sIntVal.c_str() };
        DBG_SIMPLE_ALIB_MSG_PLUGIN( file, line, method, type, 2, msgs );
        return;
    }

    // internal implementation
    if( type == 0 )
        std::cout << "ALib Error: ";
    else
        std::cout << "ALib Warning (type=" << type << "): ";
    std::cout << msg << sIntVal << std::endl;
    std::cout << "At        : " << file << ':' << line << ' ' << method << "()" << std::endl;
    assert(type);
}

//! @endcond

}}// namespace [aworx::lib]



// #################################################################################################
// static assertions for the platform
// #################################################################################################

static_assert(         sizeof(aworx::integer )       ==         sizeof(aworx::uinteger  ),       "Error in ALib type definitions" );
static_assert(         sizeof(aworx::integer )       ==         sizeof(std::size_t    ),         "Error in ALib type definitions" );
static_assert(std::is_signed< aworx::integer>::value == std::is_signed<std::ptrdiff_t >::value,  "Error in ALib type definitions" );
static_assert(std::is_signed< aworx::integer>::value != std::is_signed<std::size_t    >::value,  "Error in ALib type definitions" );
static_assert(std::is_signed<aworx::uinteger>::value == std::is_signed<std::size_t    >::value,  "Error in ALib type definitions" );
static_assert(std::is_signed<aworx::uinteger>::value != std::is_signed<std::ptrdiff_t >::value,  "Error in ALib type definitions" );

#endif



