//==================================================================================================
/// \file
/// This header file is part of module \alib_boxing of the \aliblong.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_BOXING_CUSTOMIZATIONS
#define HPP_ALIB_BOXING_CUSTOMIZATIONS 1
#pragma once
#if !defined(HPP_ALIB_BOXING_BOXING)
#   error "ALib sources with ending '.inl' must not be included from outside."
#endif


// #########  Switch off documentation parser for (almost) the whole header #####
#if !DOXYGEN

// #################################################################################################
// void*, Boxes*, Box[]
// #################################################################################################
ALIB_BOXING_VTABLE_DECLARE( void*, vt_voidP )

namespace alib {  namespace boxing {
template<> struct T_Boxer<void*>
{
    using                         Mapping=  TMappedTo<void*>;
    static constexpr Placeholder  Write( const void* value )      { return Placeholder( value );  }
    static void*                  Read ( const Placeholder& src)  { return const_cast<void*>( src.VoidPointer() );  }
};
}}

ALIB_BOXING_VTABLE_DECLARE(            BoxesHA*,  vt_boxes      )
#if ALIB_MONOMEM
ALIB_BOXING_VTABLE_DECLARE(            BoxesMA*,  vt_boxesma    )
#endif
ALIB_BOXING_VTABLE_DECLARE_ARRAYTYPE(  Box     ,  vt_boxarray   )


// #################################################################################################
// Enums
// #################################################################################################
namespace alib {  namespace boxing {
template<typename TEnum> struct T_Boxer<TEnum, ATMP_VOID_IF( std::is_enum<TEnum>::value )>
{
    using                        Mapping=    TMappedTo<TEnum>;

    static constexpr Placeholder Write(  TEnum const & enumElement )
    {
        return Placeholder( static_cast<integer>( enumElement ) );
    }

    static TEnum                 Read( const Placeholder& src)
    {
        return TEnum( static_cast<typename std::underlying_type<TEnum>::type>( src.GetInteger(0)) );
    }
};
}}


// #################################################################################################
// Boolean
// #################################################################################################
DOX_MARKER([DOX_BOXING_OPTIMIZE_DECLARE_1])
ALIB_BOXING_VTABLE_DECLARE( bool, vt_bool )
DOX_MARKER([DOX_BOXING_OPTIMIZE_DECLARE_1])

ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING_CONSTEXPR( bool, bool )


// #################################################################################################
// Integrals
// #################################################################################################
#if !ALIB_FEAT_BOXING_BIJECTIVE_INTEGRALS

    ALIB_BOXING_VTABLE_DECLARE(  integer,  vt_integer  )
    ALIB_BOXING_VTABLE_DECLARE( uinteger, vt_uinteger  )

    ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING_CONSTEXPR(   integer ,  integer )
    ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING_CONSTEXPR(  uinteger , uinteger )

    ALIB_BOXING_CUSTOMIZE_NOT_UNBOXABLE_CONSTEXPR(    int8_t,  integer )
    ALIB_BOXING_CUSTOMIZE_NOT_UNBOXABLE_CONSTEXPR(   uint8_t, uinteger )
  //ALIB_BOXING_CUSTOMIZE_NOT_UNBOXABLE_CONSTEXPR(   int16_t,  integer ) // done in dox sample below
    ALIB_BOXING_CUSTOMIZE_NOT_UNBOXABLE_CONSTEXPR(  uint16_t, uinteger )
    ALIB_BOXING_CUSTOMIZE_NOT_UNBOXABLE_CONSTEXPR(  intGap_t,  integer )
    ALIB_BOXING_CUSTOMIZE_NOT_UNBOXABLE_CONSTEXPR( uintGap_t, uinteger )

  #if ALIB_SIZEOF_INTEGER == 8
    ALIB_BOXING_CUSTOMIZE_NOT_UNBOXABLE_CONSTEXPR(   int32_t, integer  )
    ALIB_BOXING_CUSTOMIZE_NOT_UNBOXABLE_CONSTEXPR(  uint32_t, uinteger )
  #endif


DOX_MARKER([DOX_BOXING_CUSTOM_MANUAL])
namespace alib {  namespace boxing {

template<>  struct T_Boxer<int16_t>
{
    using  Mapping=    TMappedTo<integer>;

    static constexpr
    Placeholder  Write(  int16_t const & value )
    {
        return Placeholder( static_cast<integer>( value ) );
    }

    static void  Read( const Placeholder& src); // no implementation given, never called
};

}}
DOX_MARKER([DOX_BOXING_CUSTOM_MANUAL])

//--------- documentation sample of Programmer's Manual --------

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

    ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING_CONSTEXPR(    int8_t,     int8_t)
    ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING_CONSTEXPR(   uint8_t,    uint8_t)
    ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING_CONSTEXPR(   int16_t,    int16_t)
    ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING_CONSTEXPR(  uint16_t,   uint16_t)
    ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING_CONSTEXPR(   int32_t,    int32_t)
    ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING_CONSTEXPR(  uint32_t,   uint32_t)
    ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING_CONSTEXPR(  intGap_t,   intGap_t)
    ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING_CONSTEXPR( uintGap_t,  uintGap_t)

  #if ALIB_SIZEOF_INTEGER == 8
    ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING_CONSTEXPR(   int64_t,    int64_t)
    ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING_CONSTEXPR(  uint64_t,   uint64_t)
  #endif

#endif // ALIB_FEAT_BOXING_BIJECTIVE_INTEGRALS



// #################################################################################################
// Floating point
// #################################################################################################
#if !ALIB_FEAT_BOXING_BIJECTIVE_FLOATS
    ALIB_BOXING_CUSTOMIZE_NOT_UNBOXABLE_CONSTEXPR(  float    ,  double      )
#else
    ALIB_BOXING_VTABLE_DECLARE(      float, vt_float       )
    ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING_CONSTEXPR(   float    ,  float       )
#endif

    ALIB_BOXING_VTABLE_DECLARE(                   double     , vt_double    )
    ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING_CONSTEXPR( double     ,  double      )

#if ALIB_SIZEOF_LONGDOUBLE_REPORTED <= 2 * ALIB_SIZEOF_INTEGER
    ALIB_BOXING_VTABLE_DECLARE(                   long double, vt_long_double )
//    ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING_CONSTEXPR( long double, long double    )

namespace alib::boxing {
template<>  struct T_Boxer<long double>
{
    using               Mapping=    TMappedTo<long double>;
    static Placeholder  Write(  long double const & value )
    {
        ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
        Placeholder target;
        const char* src = reinterpret_cast<const char*>( &value  );
              char* dest= reinterpret_cast<      char*>( &target );
        for (int i = 0; i < ALIB_SIZEOF_LONGDOUBLE_WRITTEN; ++i)
            *dest++= *src++;
        ALIB_WARNINGS_RESTORE
        return target;
    }

    static long double                Read (const Placeholder& placeholder)
    {
        ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
        long double target;
        const char* src = reinterpret_cast<const char*>( &placeholder  );
              char* dest= reinterpret_cast<      char*>( &target );
        for (int i = 0; i < ALIB_SIZEOF_LONGDOUBLE_WRITTEN; ++i)
            *dest++= *src++;
        ALIB_WARNINGS_RESTORE
        return target;
    }
}; }

#endif


// #################################################################################################
// Characters
// #################################################################################################

#if !ALIB_FEAT_BOXING_BIJECTIVE_CHARACTERS
    ALIB_BOXING_VTABLE_DECLARE( wchar, vt_wchar )

    ALIB_BOXING_CUSTOMIZE_NOT_UNBOXABLE_CONSTEXPR( nchar    , wchar    )

    #if ALIB_CHARACTERS_NATIVE_WCHAR
        ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING_CONSTEXPR(       wchar_t, wchar    )
        ALIB_BOXING_CUSTOMIZE_NOT_UNBOXABLE_CONSTEXPR(     char16_t, wchar    )
        ALIB_BOXING_CUSTOMIZE_NOT_UNBOXABLE_CONSTEXPR(     char32_t, wchar    )
    #else
        ALIB_BOXING_CUSTOMIZE_NOT_UNBOXABLE_CONSTEXPR(      wchar_t, wchar    )
    #   if ALIB_CHARACTERS_SIZEOF_WCHAR == 4
            ALIB_BOXING_CUSTOMIZE_NOT_UNBOXABLE_CONSTEXPR( char16_t, wchar    )
            ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING_CONSTEXPR(  char32_t, wchar    )
    #   else
            ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING_CONSTEXPR(  char16_t, wchar    )
            ALIB_BOXING_CUSTOMIZE_NOT_UNBOXABLE_CONSTEXPR( char32_t, wchar    )
    #   endif
    #endif
#else
    ALIB_BOXING_VTABLE_DECLARE( char     , vt_char     )
    ALIB_BOXING_VTABLE_DECLARE( wchar_t  , vt_wchar_t  )
    ALIB_BOXING_VTABLE_DECLARE( char16_t , vt_char16_t )
    ALIB_BOXING_VTABLE_DECLARE( char32_t , vt_char32_t )

    ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING_CONSTEXPR(char    , char     )
    ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING_CONSTEXPR(wchar_t , wchar_t  )
    ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING_CONSTEXPR(char16_t, char16_t )
    ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING_CONSTEXPR(char32_t, char32_t )
#endif


// #################################################################################################
// Character Arrays (Types with T_CharArray specializations)
// #################################################################################################

// vtables
DOX_MARKER([DOX_BOXING_OPTIMIZE_DECLARE_2])
ALIB_BOXING_VTABLE_DECLARE_ARRAYTYPE( char, vt_arr_char )
DOX_MARKER([DOX_BOXING_OPTIMIZE_DECLARE_2])
ALIB_BOXING_VTABLE_DECLARE_ARRAYTYPE( wchar_t   , vt_arr_wchar_t )
ALIB_BOXING_VTABLE_DECLARE_ARRAYTYPE( char16_t  , vt_arr_char16_t)
ALIB_BOXING_VTABLE_DECLARE_ARRAYTYPE( char32_t  , vt_arr_char32_t)


#endif // !DOXYGEN
namespace alib {  namespace boxing  {
/// This type-traits struct by default inherits \c std::false_type. If specialized for
/// template type \p{TCharArray} to inherit \c std::true_type, then boxing that type will not be
/// customized automatically with a corresponding specialization of \alib{characters;T_CharArray}.
/// This keeps the customization of boxing open to be performed in a different way.
///
/// \see
///   See manual chapter \ref alib_boxing_strings "10. Boxing Character Strings" of the
///   Programmer's Manual of module \alib_boxing_nl.
///
/// @tparam TCharArray The type that \alib{characters;T_CharArray} is specialized for but still no
///                    character array boxing should be performed.
template<typename TCharArray>      struct T_SuppressCharArrayBoxing    :    std::false_type      {};
}}
#if !DOXYGEN
namespace alib {  namespace boxing  {


#define ALIBTMP_BOXER_WITHOUT_UNBOXING(TC)                                                         \
template<typename TStringLike> struct T_Boxer<TStringLike,                                         \
ATMP_VOID_IF(                                                                                      \
    !T_SuppressCharArrayBoxing<TStringLike>::value                                                 \
&&     characters::T_CharArray<TStringLike ALIB_COMMA TC>::Access                                  \
    == characters::AccessType      ::Implicit                                                      \
&&     characters::T_CharArray<TStringLike ALIB_COMMA TC>::Construction                            \
    != characters::ConstructionType::Implicit                                     ) >              \
{                                                                                                  \
    using                        Mapping=    TMappedToArrayOf<TC>;                                 \
    static constexpr Placeholder Write(  TStringLike const & value )                               \
    {                                                                                              \
        return Placeholder( characters::T_CharArray<TStringLike, TC>::Buffer( value ),             \
                            characters::T_CharArray<TStringLike, TC>::Length( value )  );          \
    }                                                                                              \
    static void                 Read( const Placeholder& src);                                  }; \


#define ALIBTMP_BOXER_WITH_UNBOXING(TC)                                                            \
template<typename TStringLike> struct T_Boxer<TStringLike,                                         \
ATMP_VOID_IF(                                                                                      \
    !T_SuppressCharArrayBoxing<TStringLike>::value                                                 \
&&     characters::T_CharArray<TStringLike ALIB_COMMA TC>::Access                                  \
    == characters::AccessType      ::Implicit                                                      \
&&     characters::T_CharArray<TStringLike ALIB_COMMA TC>::Construction                            \
    == characters::ConstructionType::Implicit                                     ) >              \
{                                                                                                  \
    using                        Mapping=    TMappedToArrayOf<TC>;                                 \
    static constexpr Placeholder Write(  TStringLike const & value )                               \
    {                                                                                              \
        return Placeholder( characters::T_CharArray<TStringLike, TC>::Buffer( value ),             \
                            characters::T_CharArray<TStringLike, TC>::Length( value )  );          \
    }                                                                                              \
    static TStringLike           Read( const Placeholder& src)                                     \
    {                                                                                              \
        return characters::T_CharArray<TStringLike, TC>::Construct( src.Pointer<TC>(),             \
                                                                    src.Length() );                \
    }                                                                                           }; \


ALIBTMP_BOXER_WITHOUT_UNBOXING(nchar)  ALIBTMP_BOXER_WITH_UNBOXING(nchar)
ALIBTMP_BOXER_WITHOUT_UNBOXING(wchar)  ALIBTMP_BOXER_WITH_UNBOXING(wchar)
ALIBTMP_BOXER_WITHOUT_UNBOXING(xchar)  ALIBTMP_BOXER_WITH_UNBOXING(xchar)

#undef ALIBTMP_BOXER_WITHOUT_UNBOXING
#undef ALIBTMP_BOXER_WITH_UNBOXING
}}

// #################################################################################################
// Static VTables for standard types
// #################################################################################################
ALIB_BOXING_VTABLE_DECLARE( std::type_info*              , vt_std_type_info          )

// #################################################################################################
// Static VTables for low-level ALib types
// #################################################################################################
// CodeMarker_CommonEnums
ALIB_BOXING_VTABLE_DECLARE( alib::lang::Alignment        , vt_alib_Alignment         )
ALIB_BOXING_VTABLE_DECLARE( alib::lang::Bool             , vt_alib_Bool              )
ALIB_BOXING_VTABLE_DECLARE( alib::lang::Caching          , vt_alib_Caching           )
ALIB_BOXING_VTABLE_DECLARE( alib::lang::Case             , vt_alib_Case              )
ALIB_BOXING_VTABLE_DECLARE( alib::lang::ContainerOp      , vt_alib_ContainerOp       )
ALIB_BOXING_VTABLE_DECLARE( alib::lang::CreateDefaults   , vt_alib_CreateDefaults    )
ALIB_BOXING_VTABLE_DECLARE( alib::lang::CreateIfNotExists, vt_alib_CreateIfNotExists )
ALIB_BOXING_VTABLE_DECLARE( alib::lang::CurrentData      , vt_alib_CurrentData       )
ALIB_BOXING_VTABLE_DECLARE( alib::lang::Inclusion        , vt_alib_Inclusion         )
ALIB_BOXING_VTABLE_DECLARE( alib::lang::Initialization   , vt_alib_Initialization    )
ALIB_BOXING_VTABLE_DECLARE( alib::lang::Phase            , vt_alib_Phase             )
ALIB_BOXING_VTABLE_DECLARE( alib::lang::Propagation      , vt_alib_Propagation       )
ALIB_BOXING_VTABLE_DECLARE( alib::lang::Reach            , vt_alib_Reach             )
ALIB_BOXING_VTABLE_DECLARE( alib::lang::Recursive        , vt_alib_Recursive         )
ALIB_BOXING_VTABLE_DECLARE( alib::lang::Responsibility   , vt_alib_Responsibility    )
ALIB_BOXING_VTABLE_DECLARE( alib::lang::Safeness         , vt_alib_Safeness          )
ALIB_BOXING_VTABLE_DECLARE( alib::lang::Side             , vt_alib_Side              )
ALIB_BOXING_VTABLE_DECLARE( alib::lang::SortOrder        , vt_alib_SortOrder         )
ALIB_BOXING_VTABLE_DECLARE( alib::lang::SourceData       , vt_alib_SourceData        )
ALIB_BOXING_VTABLE_DECLARE( alib::lang::Switch           , vt_alib_Switch            )
ALIB_BOXING_VTABLE_DECLARE( alib::lang::Timezone         , vt_alib_Timezone          )
ALIB_BOXING_VTABLE_DECLARE( alib::lang::Timing           , vt_alib_Timing            )
ALIB_BOXING_VTABLE_DECLARE( alib::lang::ValueReference   , vt_alib_ValueReference    )
ALIB_BOXING_VTABLE_DECLARE( alib::lang::Whitespaces      , vt_alib_Whitespaces       )

ALIB_BOXING_VTABLE_DECLARE( alib::lang::CallerInfo*      , vt_lang_callerinfo )
// #################################################################################################
// Static VTables for wrapped string-types
// #################################################################################################
#if ALIB_STRINGS
    ALIB_BOXING_VTABLE_DECLARE( std::reference_wrapper<alib::strings::TAString<nchar ALIB_COMMA lang::HeapAllocator>>, vt_alib_wrapped_tanstring )
    ALIB_BOXING_VTABLE_DECLARE( std::reference_wrapper<alib::strings::TAString<wchar ALIB_COMMA lang::HeapAllocator>>, vt_alib_wrapped_tawstring )
    ALIB_BOXING_VTABLE_DECLARE( std::reference_wrapper<alib::strings::TAString<xchar ALIB_COMMA lang::HeapAllocator>>, vt_alib_wrapped_taxstring )
#endif

#endif //ALIB_DOX

#endif // HPP_ALIB_BOXING_CUSTOMIZATIONS

