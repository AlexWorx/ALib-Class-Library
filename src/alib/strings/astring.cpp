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
#include "alib/strings/strings.prepro.hpp"
#include <cstring>
// ===========================================   Module   ==========================================
#if ALIB_C20_MODULES
    module ALib.Strings;
    import   ALib.Lang;
    #if ALIB_CAMP
    import   ALib.Camp.Base;
    #endif
#else
#   include "ALib.Lang.H"
#   include "ALib.Camp.Base.H"
#   include "ALib.Strings.H"
#endif
// ======================================   Implementation   =======================================

#define ALIB_STRINGS_TASTRING_INSTANTIATION
#   include "alib/strings/detail/tastringimpl.inl"

#define ALIB_STRINGS_TASTRING_INSTANTIATION
#   define ASTR_TALLOC_SPEC     lang::HeapAllocator
#   define ASTR_TCHAR_SPEC      ALIB_CHAR_TYPE_ID_X
#   include "alib/strings/detail/tastring_ca_impl.inl.inl"

#   define ASTR_TALLOC_SPEC     lang::HeapAllocator
#   define ASTR_TCHAR_SPEC      ALIB_CHAR_TYPE_ID_W
#   include "alib/strings/detail/tastring_ca_impl.inl.inl"

#   define ASTR_TALLOC_SPEC     lang::HeapAllocator
#   define ASTR_TCHAR_SPEC      ALIB_CHAR_TYPE_ID_N
#   include "alib/strings/detail/tastring_ca_impl.inl.inl"


#undef ALIB_STRINGS_TASTRING_INSTANTIATION

#if !DOXYGEN
namespace   alib::strings {
String DT_UNITS[size_t(DayTimeUnits::SIZE_OF_UNITS)]
#if ALIB_CAMP
    ;
#else
={
DOX_MARKER( [DT_UNITS_DEFAULTS])
    A_CHAR("zero time"),
    A_CHAR(" days"    ) ,   A_CHAR(" day"     ) ,
    A_CHAR(" hours"   ) ,   A_CHAR(" hour"    ) ,
    A_CHAR(" minutes" ) ,   A_CHAR(" minute"  ) ,
    A_CHAR(" seconds" ) ,   A_CHAR(" second"  ) ,
    A_CHAR(" ms"      ) ,   A_CHAR(" ms"      ) ,
    A_CHAR(" \u00B5s" ) ,   A_CHAR(" \u00B5s" ) ,
    A_CHAR(" ns"      ) ,   A_CHAR(" ns"      )   };
DOX_MARKER( [DT_UNITS_DEFAULTS])
#endif

}
#endif

#if !ALIB_SINGLE_THREADED
    ALIB_STRINGS_APPENDABLE_TYPE_DEF_N(alib::threads::Thread,
    { target << src.GetName()  << '(' << src.GetID() << ')';} )

    ALIB_STRINGS_APPENDABLE_TYPE_DEF_W(alib::threads::Thread,
    { target << src.GetName()  << '(' << src.GetID() << ')';} )
#endif 

