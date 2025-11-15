//==================================================================================================
/// \file
/// This header-file is part of module \alib_boxing of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================

//################## Switch off documentation parser for (almost) the whole header #################
#if !DOXYGEN

//##################################################################################################
// void*, Boxes*, Box[]
//##################################################################################################
ALIB_BOXING_VTABLE_DECLARE( void*, vt_voidP )

ALIB_EXPORT namespace alib {  namespace boxing {

class Boxes;
class BoxesMA;

template<>
struct BoxTraits<void*>
{
    using                         Mapping=  void*;
    static constexpr bool         IsArray=  false;
    static constexpr Placeholder  Write( const void* value )        { return Placeholder( value ); }
    static void*                  Read ( const Placeholder& box) {
        return const_cast<void*>( box.GetVoidPointer() );
    }
};
}}

ALIB_BOXING_VTABLE_DECLARE(            Boxes*  ,  vt_boxes      )
#if ALIB_MONOMEM
ALIB_BOXING_VTABLE_DECLARE(            BoxesMA*,  vt_boxesma    )
#endif
ALIB_BOXING_VTABLE_DECLARE_ARRAYTYPE(  Box     ,  vt_boxarray   )


//##################################################################################################
// Enums
//##################################################################################################
ALIB_EXPORT namespace alib {  namespace boxing {
template<typename TEnum>
requires std::is_enum<TEnum>::value
struct BoxTraits<TEnum >
{
    using                        Mapping=  TEnum;
    static constexpr bool        IsArray=  false;

    static constexpr void        Write( Placeholder& box,  TEnum const & enumElement )
    { box.Write( integer( enumElement ) ); }

    static TEnum                 Read( const Placeholder& box)
    { return TEnum( static_cast<typename std::underlying_type_t<TEnum>>(box.Integrals.Array[0])); }
};
}}


//##################################################################################################
// Boolean
//##################################################################################################
DOX_MARKER([DOX_BOXING_OPTIMIZE_DECLARE_1])
ALIB_BOXING_VTABLE_DECLARE( bool, vt_bool )
DOX_MARKER([DOX_BOXING_OPTIMIZE_DECLARE_1])

ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING( bool, bool )


//##################################################################################################
// Integrals
//##################################################################################################
#if !ALIB_FEAT_BOXING_BIJECTIVE_INTEGRALS

    ALIB_BOXING_VTABLE_DECLARE(  integer,  vt_integer  )
    ALIB_BOXING_VTABLE_DECLARE( uinteger, vt_uinteger  )

    ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING(   integer ,  integer )
    ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING(  uinteger , uinteger )

    ALIB_BOXING_CUSTOMIZE_NOT_UNBOXABLE(    int8_t,  integer )
    ALIB_BOXING_CUSTOMIZE_NOT_UNBOXABLE(   uint8_t, uinteger )
  //ALIB_BOXING_CUSTOMIZE_NOT_UNBOXABLE(   int16_t,  integer ) // done in dox sample below
    ALIB_BOXING_CUSTOMIZE_NOT_UNBOXABLE(  uint16_t, uinteger )
    ALIB_BOXING_CUSTOMIZE_NOT_UNBOXABLE(  intGap_t,  integer )
    ALIB_BOXING_CUSTOMIZE_NOT_UNBOXABLE( uintGap_t, uinteger )

  #if ALIB_SIZEOF_INTEGER == 8
    ALIB_BOXING_CUSTOMIZE_NOT_UNBOXABLE(   int32_t, integer  )
    ALIB_BOXING_CUSTOMIZE_NOT_UNBOXABLE(  uint32_t, uinteger )
  #endif


DOX_MARKER([DOX_BOXING_CUSTOM_MANUAL])
ALIB_EXPORT namespace alib {  namespace boxing {

template<>
struct BoxTraits<int16_t>
{
    using                   Mapping=  alib::integer;
    static constexpr bool   IsArray=  false;

    // The Write function (boxing) converts the value to type alib::integer
    static constexpr void   Write( Placeholder& box,  int16_t const & value ) {
        box.Write( integer( value ) );
    }

    // The read function is decared to return 'void'. This determines that unboxing is not
    // possible/allowed.
    // Thus, no implementation is given, because this method is never called.
    static void  Read( const Placeholder& box);
};

}}
DOX_MARKER([DOX_BOXING_CUSTOM_MANUAL])

//--------------------------- documentation sample of Programmer's Manual --------------------------

#else  // ALIB_FEAT_BOXING_BIJECTIVE_INTEGRALS
    ALIB_BOXING_VTABLE_DECLARE(     int8_t ,      vt_int8_t )
    ALIB_BOXING_VTABLE_DECLARE(    uint8_t ,     vt_uint8_t )
    ALIB_BOXING_VTABLE_DECLARE(    int16_t ,     vt_int16_t )
    ALIB_BOXING_VTABLE_DECLARE(   uint16_t ,    vt_uint16_t )
    ALIB_BOXING_VTABLE_DECLARE(    int32_t ,     vt_int32_t )
    ALIB_BOXING_VTABLE_DECLARE(   uint32_t ,    vt_uint32_t )
    ALIB_BOXING_VTABLE_DECLARE(   intGap_t ,    vt_intGap_t )
    ALIB_BOXING_VTABLE_DECLARE(  uintGap_t ,   vt_uintGap_t )
  #if ALIB_SIZEOF_INTEGER == 8
    ALIB_BOXING_VTABLE_DECLARE(    int64_t ,     vt_int64_t )
    ALIB_BOXING_VTABLE_DECLARE(   uint64_t ,    vt_uint64_t )
  #endif

    ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING(    int8_t,     int8_t)
    ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING(   uint8_t,    uint8_t)
    ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING(   int16_t,    int16_t)
    ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING(  uint16_t,   uint16_t)
    ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING(   int32_t,    int32_t)
    ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING(  uint32_t,   uint32_t)
    ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING(  intGap_t,   intGap_t)
    ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING( uintGap_t,  uintGap_t)

  #if ALIB_SIZEOF_INTEGER == 8
    ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING(   int64_t,    int64_t)
    ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING(  uint64_t,   uint64_t)
  #endif

#endif // ALIB_FEAT_BOXING_BIJECTIVE_INTEGRALS



//##################################################################################################
// Floating point
//##################################################################################################
#if !ALIB_FEAT_BOXING_BIJECTIVE_FLOATS
    ALIB_BOXING_CUSTOMIZE_NOT_UNBOXABLE(  float    ,  double      )
#else
    ALIB_BOXING_VTABLE_DECLARE(      float, vt_float       )
    ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING(   float    ,  float       )
#endif

    ALIB_BOXING_VTABLE_DECLARE(                   double     , vt_double    )
    ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING( double     ,  double      )

#if ALIB_SIZEOF_LONGDOUBLE_REPORTED <= 2 * ALIB_SIZEOF_INTEGER
    ALIB_BOXING_VTABLE_DECLARE(                   long double, vt_long_double )

ALIB_EXPORT namespace alib::boxing {
template<>
struct BoxTraits<long double>
{
    using                   Mapping=  long double;
    static constexpr bool   IsArray=  false;
    static           void   Write( Placeholder& box, long double const & value ) {
        const char* src = reinterpret_cast<const char*>( &value  );
              char* dest= reinterpret_cast<      char*>( &box );
        for (int i = 0; i < ALIB_SIZEOF_LONGDOUBLE_WRITTEN; ++i)
            *dest++= *src++;
    }

    static long double                Read (const Placeholder& placeholder) {
        long double result;
        const char* src = reinterpret_cast<const char*>( &placeholder  );
              char* dest= reinterpret_cast<      char*>( &result );
        for (int i = 0; i < ALIB_SIZEOF_LONGDOUBLE_WRITTEN; ++i)
            *dest++= *src++;
        return result;
    }
}; }

#endif


//##################################################################################################
// Characters
//##################################################################################################

#if !ALIB_FEAT_BOXING_BIJECTIVE_CHARACTERS
    ALIB_BOXING_VTABLE_DECLARE( wchar, vt_wchar )

    ALIB_BOXING_CUSTOMIZE_NOT_UNBOXABLE( nchar   , wchar    )
    ALIB_BOXING_CUSTOMIZE_NOT_UNBOXABLE( char8_t , wchar    )

    #if ALIB_CHARACTERS_NATIVE_WCHAR
        ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING(       wchar_t, wchar    )
        ALIB_BOXING_CUSTOMIZE_NOT_UNBOXABLE(     char16_t, wchar    )
        ALIB_BOXING_CUSTOMIZE_NOT_UNBOXABLE(     char32_t, wchar    )
    #else
        ALIB_BOXING_CUSTOMIZE_NOT_UNBOXABLE(      wchar_t, wchar    )
    #   if ALIB_CHARACTERS_SIZEOF_WCHAR == 4
            ALIB_BOXING_CUSTOMIZE_NOT_UNBOXABLE( char16_t, wchar    )
            ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING(  char32_t, wchar    )
    #   else
            ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING(  char16_t, wchar    )
            ALIB_BOXING_CUSTOMIZE_NOT_UNBOXABLE( char32_t, wchar    )
    #   endif
    #endif
#else
    ALIB_BOXING_VTABLE_DECLARE( char     , vt_char     )
    ALIB_BOXING_VTABLE_DECLARE( wchar_t  , vt_wchar_t  )
    ALIB_BOXING_VTABLE_DECLARE( char8_t  , vt_char8_t  )
    ALIB_BOXING_VTABLE_DECLARE( char16_t , vt_char16_t )
    ALIB_BOXING_VTABLE_DECLARE( char32_t , vt_char32_t )

    ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING(char    , char     )
    ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING(wchar_t , wchar_t  )
    ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING(char8_t , char8_t  )
    ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING(char16_t, char16_t )
    ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING(char32_t, char32_t )
#endif


//##################################################################################################
// Static VTables for standard types
//##################################################################################################
ALIB_BOXING_VTABLE_DECLARE( std::type_info*              , vt_std_type_info          )

//##################################################################################################
// Static VTables for low-level ALib types
//##################################################################################################
// CodeMarker_CommonEnums
ALIB_BOXING_VTABLE_DECLARE( alib::lang::Alignment          , vt_alib_Alignment         )
ALIB_BOXING_VTABLE_DECLARE( alib::lang::Bool               , vt_alib_Bool              )
ALIB_BOXING_VTABLE_DECLARE( alib::lang::Caching            , vt_alib_Caching           )
ALIB_BOXING_VTABLE_DECLARE( alib::lang::Case               , vt_alib_Case              )
ALIB_BOXING_VTABLE_DECLARE( alib::lang::ContainerOp        , vt_alib_ContainerOp       )
ALIB_BOXING_VTABLE_DECLARE( alib::lang::CreateDefaults     , vt_alib_CreateDefaults    )
ALIB_BOXING_VTABLE_DECLARE( alib::lang::CreateIfNotExists  , vt_alib_CreateIfNotExists )
ALIB_BOXING_VTABLE_DECLARE( alib::lang::CurrentData        , vt_alib_CurrentData       )
ALIB_BOXING_VTABLE_DECLARE( alib::lang::Inclusion          , vt_alib_Inclusion         )
ALIB_BOXING_VTABLE_DECLARE( alib::lang::Initialization     , vt_alib_Initialization    )
ALIB_BOXING_VTABLE_DECLARE( alib::lang::LineFeeds          , vt_alib_LineFeeds         )
ALIB_BOXING_VTABLE_DECLARE( alib::lang::Phase              , vt_alib_Phase             )
ALIB_BOXING_VTABLE_DECLARE( alib::lang::Propagation        , vt_alib_Propagation       )
ALIB_BOXING_VTABLE_DECLARE( alib::lang::Reach              , vt_alib_Reach             )
ALIB_BOXING_VTABLE_DECLARE( alib::lang::Recursive          , vt_alib_Recursive         )
ALIB_BOXING_VTABLE_DECLARE( alib::lang::Responsibility     , vt_alib_Responsibility    )
ALIB_BOXING_VTABLE_DECLARE( alib::lang::Safeness           , vt_alib_Safeness          )
ALIB_BOXING_VTABLE_DECLARE( alib::lang::Side               , vt_alib_Side              )
ALIB_BOXING_VTABLE_DECLARE( alib::lang::SortOrder          , vt_alib_SortOrder         )
ALIB_BOXING_VTABLE_DECLARE( alib::lang::SourceData         , vt_alib_SourceData        )
ALIB_BOXING_VTABLE_DECLARE( alib::lang::Switch             , vt_alib_Switch            )
ALIB_BOXING_VTABLE_DECLARE( alib::lang::Timezone           , vt_alib_Timezone          )
ALIB_BOXING_VTABLE_DECLARE( alib::lang::Timing             , vt_alib_Timing            )
ALIB_BOXING_VTABLE_DECLARE( alib::lang::ValueReference     , vt_alib_ValueReference    )
ALIB_BOXING_VTABLE_DECLARE( alib::lang::Whitespaces        , vt_alib_Whitespaces       )

ALIB_BOXING_VTABLE_DECLARE( alib::lang::CallerInfo*        , vt_lang_callerinfo        )
ALIB_BOXING_VTABLE_DECLARE( alib::time::Ticks              , vt_time_ticks             )
ALIB_BOXING_VTABLE_DECLARE( alib::time::Ticks::Duration    , vt_time_ticks_duration    )
ALIB_BOXING_VTABLE_DECLARE( alib::time::DateTime           , vt_time_datetime          )
ALIB_BOXING_VTABLE_DECLARE( alib::time::DateTime::Duration , vt_time_datetime_duration )
#if !ALIB_SINGLE_THREADED
  ALIB_BOXING_VTABLE_DECLARE( alib::threads::Thread*       , vt_threads_tp             )
#endif

#if ALIB_STRINGS
ALIB_BOXING_VTABLE_DECLARE( std::reference_wrapper<alib::strings::TAString<nchar ALIB_COMMA lang::HeapAllocator>>, vt_alib_wrapped_tanstring )
ALIB_BOXING_VTABLE_DECLARE( std::reference_wrapper<alib::strings::TAString<wchar ALIB_COMMA lang::HeapAllocator>>, vt_alib_wrapped_tawstring )
ALIB_BOXING_VTABLE_DECLARE( std::reference_wrapper<alib::strings::TAString<xchar ALIB_COMMA lang::HeapAllocator>>, vt_alib_wrapped_taxstring )

ALIB_BOXING_VTABLE_DECLARE( alib::strings::util::Token* , vt_alib_strings_token )
#endif

#endif //DOXYGEN
