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
#if !ALIB_MONOMEM || !ALIB_CONTAINERS
#   include <unordered_map>
#   include <unordered_set>
#endif

// ===========================================   Module   ==========================================
#if ALIB_C20_MODULES
    module ALib.Boxing;
    import   ALib.Lang;
#else
#   include "ALib.Lang.H"
#   include "ALib.Boxing.H"
#endif
// ======================================   Implementation   =======================================
ALIB_BOXING_VTABLE_DEFINE(          void* , vt_voidP    )
ALIB_BOXING_VTABLE_DEFINE(          BoxesHA*, vt_boxes  )
#if ALIB_MONOMEM
ALIB_BOXING_VTABLE_DEFINE(        BoxesMA*, vt_boxesma  )
#endif
ALIB_BOXING_VTABLE_DEFINE_ARRAYTYPE( Box   , vt_boxarray )

DOX_MARKER([DOX_BOXING_OPTIMIZE_DEFINE_1])
ALIB_BOXING_VTABLE_DEFINE( bool,  vt_bool )
DOX_MARKER([DOX_BOXING_OPTIMIZE_DEFINE_1])


#if !ALIB_FEAT_BOXING_BIJECTIVE_INTEGRALS

    ALIB_BOXING_VTABLE_DEFINE(   integer  ,  vt_integer )
    ALIB_BOXING_VTABLE_DEFINE(   uinteger ,  vt_uinteger)

#else
    ALIB_BOXING_VTABLE_DEFINE(     int8_t ,    vt_int8_t)
    ALIB_BOXING_VTABLE_DEFINE(    uint8_t ,   vt_uint8_t)
    ALIB_BOXING_VTABLE_DEFINE(    int16_t ,   vt_int16_t)
    ALIB_BOXING_VTABLE_DEFINE(   uint16_t ,  vt_uint16_t)
    ALIB_BOXING_VTABLE_DEFINE(    int32_t ,  vt_int32_t)
    ALIB_BOXING_VTABLE_DEFINE(   uint32_t , vt_uint32_t)
    ALIB_BOXING_VTABLE_DEFINE(   intGap_t ,  vt_intGap_t)
    ALIB_BOXING_VTABLE_DEFINE(  uintGap_t , vt_uintGap_t)

  #if ALIB_SIZEOF_INTEGER == 8
    ALIB_BOXING_VTABLE_DEFINE(    int64_t ,  vt_int64_t)
    ALIB_BOXING_VTABLE_DEFINE(   uint64_t , vt_uint64_t)
  #endif

#endif

    ALIB_BOXING_VTABLE_DEFINE(      double, vt_double )
#if ALIB_SIZEOF_LONGDOUBLE_REPORTED <= 2 * ALIB_SIZEOF_INTEGER
    ALIB_BOXING_VTABLE_DEFINE( long double, vt_long_double )
#endif
#if ALIB_FEAT_BOXING_BIJECTIVE_FLOATS
    ALIB_BOXING_VTABLE_DEFINE(      float , vt_float  )
#endif

#if !ALIB_FEAT_BOXING_BIJECTIVE_CHARACTERS
    ALIB_BOXING_VTABLE_DEFINE( wchar    , vt_wchar )
#else
    ALIB_BOXING_VTABLE_DEFINE( char     , vt_char     )
    ALIB_BOXING_VTABLE_DEFINE( wchar_t  , vt_wchar_t  )
    ALIB_BOXING_VTABLE_DEFINE( char8_t  , vt_char8_t  )
    ALIB_BOXING_VTABLE_DEFINE( char16_t , vt_char16_t )
    ALIB_BOXING_VTABLE_DEFINE( char32_t , vt_char32_t )
#endif

DOX_MARKER([DOX_BOXING_OPTIMIZE_DEFINE_2])
ALIB_BOXING_VTABLE_DEFINE_ARRAYTYPE( char, vt_arr_char )
DOX_MARKER([DOX_BOXING_OPTIMIZE_DEFINE_2])
ALIB_BOXING_VTABLE_DEFINE_ARRAYTYPE( wchar_t  , vt_arr_wchar_t )
ALIB_BOXING_VTABLE_DEFINE_ARRAYTYPE( char8_t  , vt_arr_char8_t )
ALIB_BOXING_VTABLE_DEFINE_ARRAYTYPE( char16_t , vt_arr_char16_t)
ALIB_BOXING_VTABLE_DEFINE_ARRAYTYPE( char32_t , vt_arr_char32_t)

// #################################################################################################
// Static VTables for standard types
// #################################################################################################
ALIB_BOXING_VTABLE_DEFINE( std::type_info*              , vt_std_type_info          )

// #################################################################################################
// Static VTables for low-level ALib types
// #################################################################################################
// CodeMarker_CommonEnums
ALIB_BOXING_VTABLE_DEFINE( alib::lang::Alignment        , vt_alib_Alignment         )
ALIB_BOXING_VTABLE_DEFINE( alib::lang::Bool             , vt_alib_Bool              )
ALIB_BOXING_VTABLE_DEFINE( alib::lang::Caching          , vt_alib_Caching           )
ALIB_BOXING_VTABLE_DEFINE( alib::lang::Case             , vt_alib_Case              )
ALIB_BOXING_VTABLE_DEFINE( alib::lang::ContainerOp      , vt_alib_ContainerOp       )
ALIB_BOXING_VTABLE_DEFINE( alib::lang::CreateDefaults   , vt_alib_CreateDefaults    )
ALIB_BOXING_VTABLE_DEFINE( alib::lang::CreateIfNotExists, vt_alib_CreateIfNotExists )
ALIB_BOXING_VTABLE_DEFINE( alib::lang::CurrentData      , vt_alib_CurrentData       )
ALIB_BOXING_VTABLE_DEFINE( alib::lang::Inclusion        , vt_alib_Inclusion         )
ALIB_BOXING_VTABLE_DEFINE( alib::lang::Initialization   , vt_alib_Initialization    )
ALIB_BOXING_VTABLE_DEFINE( alib::lang::Phase            , vt_alib_Phase             )
ALIB_BOXING_VTABLE_DEFINE( alib::lang::Propagation      , vt_alib_Propagation       )
ALIB_BOXING_VTABLE_DEFINE( alib::lang::Reach            , vt_alib_Reach             )
ALIB_BOXING_VTABLE_DEFINE( alib::lang::Reach            , vt_alib_Recursive         )
ALIB_BOXING_VTABLE_DEFINE( alib::lang::Responsibility   , vt_alib_Responsibility    )
ALIB_BOXING_VTABLE_DEFINE( alib::lang::Safeness         , vt_alib_Safeness          )
ALIB_BOXING_VTABLE_DEFINE( alib::lang::Side             , vt_alib_Side              )
ALIB_BOXING_VTABLE_DEFINE( alib::lang::SortOrder        , vt_alib_SortOrder         )
ALIB_BOXING_VTABLE_DEFINE( alib::lang::SourceData       , vt_alib_SourceData        )
ALIB_BOXING_VTABLE_DEFINE( alib::lang::Switch           , vt_alib_Switch            )
ALIB_BOXING_VTABLE_DEFINE( alib::lang::Timezone         , vt_alib_Timezone          )
ALIB_BOXING_VTABLE_DEFINE( alib::lang::Timing           , vt_alib_Timing            )
ALIB_BOXING_VTABLE_DEFINE( alib::lang::ValueReference   , vt_alib_ValueReference    )
ALIB_BOXING_VTABLE_DEFINE( alib::lang::Whitespaces      , vt_alib_Whitespaces       )

ALIB_BOXING_VTABLE_DEFINE( alib::lang::CallerInfo*      , vt_lang_callerinfo )


// time
ALIB_BOXING_VTABLE_DEFINE( alib::time::Ticks              , vt_time_ticks             )
ALIB_BOXING_VTABLE_DEFINE( alib::time::Ticks::Duration    , vt_time_ticks_duration    )
ALIB_BOXING_VTABLE_DEFINE( alib::time::DateTime           , vt_time_datetime          )
ALIB_BOXING_VTABLE_DEFINE( alib::time::DateTime::Duration , vt_time_datetime_duration )

#if !ALIB_SINGLE_THREADED
  ALIB_BOXING_VTABLE_DEFINE(alib::threads::Thread*        , vt_threads_tp             )
#endif

