// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib_precompile.hpp"
#if !defined(ALIB_C20_MODULES) || ((ALIB_C20_MODULES != 0) && (ALIB_C20_MODULES != 1))
#   error "Symbol ALIB_C20_MODULES has to be given to the compiler as either 0 or 1"
#endif
#if ALIB_C20_MODULES
    module;
#endif
// ======================================   Global Fragment   ======================================
#include "alib/boxing/boxing.prepro.hpp"
#include <cstring>

#if ALIB_DEBUG && !DOXYGEN
namespace alib::boxing::debug {

    // This is used by boxing::Bootstrap to do runtime-check for compatibility of boxing
    // and long double values.
    // It was put here to prevent the compiler to optimize and remove the code.
    extern  long double LONGDOUBLE_WRITE_TEST_MEM[2];
            long double LONGDOUBLE_WRITE_TEST_MEM[2];
    extern  void LongDoubleTrueLengthSet();
            void LongDoubleTrueLengthSet()
    {
        memset( LONGDOUBLE_WRITE_TEST_MEM, 0x3E, 2 * ALIB_SIZEOF_LONGDOUBLE_REPORTED);
    }
    extern  bool LongDoubleTrueLengthTest();
            bool LongDoubleTrueLengthTest()
    {
        const char* mem= reinterpret_cast<const char*>( LONGDOUBLE_WRITE_TEST_MEM );
        return    mem[ALIB_SIZEOF_LONGDOUBLE_WRITTEN - 1] != 0x3E
               && mem[ALIB_SIZEOF_LONGDOUBLE_WRITTEN    ] == 0x3E;
    }
} // namespace [alib::boxing::detail]
#endif

// ===========================================   Module   ==========================================
#if ALIB_C20_MODULES
    module ALib.Boxing;
#endif
// ======================================   Implementation   =======================================



