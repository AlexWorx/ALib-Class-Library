// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#if !defined(HPP_ALIB_BOXING_PROPPERINCLUDE)
#   error "ALib sources with ending '.inl' must not be included from outside."
#endif


// #########  Switch off documentation parser for (almost) the whole header #####
#if !ALIB_DOCUMENTATION_PARSER

// #################################################################################################
// void*, Boxes*, Box[]
// #################################################################################################
ALIB_BOXING_VTABLE_DECLARE( void*, vt_voidP )

namespace aworx { namespace lib { namespace boxing {
template<> struct T_Boxer<void*>
{
    using                         Mapping=  TMappedTo<void*>;
    static constexpr Placeholder  Write( const void* value )      { return Placeholder( value );  }
    static void*                  Read ( const Placeholder& src)  { return const_cast<void*>( src.VoidPointer() );  }
};
}}}

ALIB_BOXING_VTABLE_DECLARE(            Boxes*,  vt_boxes    )
ALIB_BOXING_VTABLE_DECLARE_ARRAYTYPE(  Box   ,  vt_boxarray )


// #################################################################################################
// Enums
// #################################################################################################
namespace aworx { namespace lib { namespace boxing {
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
}}}


// #################################################################################################
// Boolean
// #################################################################################################
DOX_MARKER([DOX_ALIB_BOXING_OPTIMIZE_DECLARE_1])
ALIB_BOXING_VTABLE_DECLARE( bool, vt_bool )
DOX_MARKER([DOX_ALIB_BOXING_OPTIMIZE_DECLARE_1])

ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING_CONSTEXPR( bool, bool )


// #################################################################################################
// Integrals
// #################################################################################################
    ALIB_BOXING_VTABLE_DECLARE(  integer,  vt_integer  )
    ALIB_BOXING_VTABLE_DECLARE( uinteger, vt_uinteger  )


    ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING_CONSTEXPR(   integer ,  integer )
    ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING_CONSTEXPR(  uinteger , uinteger )

#if ALIB_FEAT_BOXING_NON_BIJECTIVE_INTEGRALS

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


//--------- documentation sample of Programmer's Manual --------
//#define ALIB_DOX_TEST_DOXED_VERSION // un-comment for testing documentation sample
#if !defined(ALIB_DOX_TEST_DOXED_VERSION)

ALIB_BOXING_CUSTOMIZE_NOT_UNBOXABLE_CONSTEXPR( int16_t, integer )

#else
DOX_MARKER([DOX_ALIB_BOXING_CUSTOM_MANUAL])
namespace aworx { namespace lib { namespace boxing {
template<> struct T_Boxer<int16_t>
{
    using              Mapping=    TMappedTo<integer>;

    static inline void Write( Placeholder& target, const int16_t value )
    {
        target.Write( static_cast<integer>( value ) );
    }

    static void        Read( const Placeholder& src);
};
}}}
DOX_MARKER([DOX_ALIB_BOXING_CUSTOM_MANUAL])
#endif
//--------- documentation sample of Programmer's Manual --------

#else  // ALIB_FEAT_BOXING_NON_BIJECTIVE_INTEGRALS

    ALIB_BOXING_VTABLE_DECLARE(     int8_t )
    ALIB_BOXING_VTABLE_DECLARE(    uint8_t )
    ALIB_BOXING_VTABLE_DECLARE(    int16_t )
    ALIB_BOXING_VTABLE_DECLARE(   uint16_t )
    ALIB_BOXING_VTABLE_DECLARE(   intGap_t )
    ALIB_BOXING_VTABLE_DECLARE(  uintGap_t )
  #if ALIB_SIZEOF_INTEGER == 8
    ALIB_BOXING_VTABLE_DECLARE(    int32_t )
    ALIB_BOXING_VTABLE_DECLARE(   uint32_t )
  #endif

    ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING_CONSTEXPR(    int8_t,     int8_t)
    ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING_CONSTEXPR(   uint8_t,    uint8_t)
    ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING_CONSTEXPR(   int16_t,    int16_t)
    ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING_CONSTEXPR(  uint16_t,   uint16_t)
    ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING_CONSTEXPR(  intGap_t,   intGap_t)
    ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING_CONSTEXPR( uintGap_t,  uintGap_t)

  #if ALIB_SIZEOF_INTEGER == 8
    ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING_CONSTEXPR(   int32_t,    int32_t)
    ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING_CONSTEXPR(  uint32_t,   uint32_t)
  #endif

#endif // ALIB_FEAT_BOXING_NON_BIJECTIVE_INTEGRALS



// #################################################################################################
// Floating point
// #################################################################################################
#if ALIB_FEAT_BOXING_NON_BIJECTIVE_FLOATS

    ALIB_BOXING_CUSTOMIZE_NOT_UNBOXABLE_CONSTEXPR(  float    ,  double      )
#else
    ALIB_BOXING_VTABLE_DECLARE(      float, vt_float       )
    ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING_CONSTEXPR(   float    ,  float       )
#endif

    ALIB_BOXING_VTABLE_DECLARE(                   double     , vt_double    )
    ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING_CONSTEXPR( double     ,  double      )

#if ALIB_SIZEOF_LONGDOUBLE <= 2 * ALIB_SIZEOF_INTEGER
    ALIB_BOXING_VTABLE_DECLARE(                   long double, vt_long_double )
    ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING_CONSTEXPR( long double, long double    )
#endif


// #################################################################################################
// Characters
// #################################################################################################

#if ALIB_FEAT_BOXING_NON_BIJECTIVE_CHARACTERS
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
DOX_MARKER([DOX_ALIB_BOXING_OPTIMIZE_DECLARE_2])
ALIB_BOXING_VTABLE_DECLARE_ARRAYTYPE( char, vt_arr_char )
DOX_MARKER([DOX_ALIB_BOXING_OPTIMIZE_DECLARE_2])
ALIB_BOXING_VTABLE_DECLARE_ARRAYTYPE( wchar_t   , vt_arr_wchar_t )
ALIB_BOXING_VTABLE_DECLARE_ARRAYTYPE( char16_t  , vt_arr_char16_t)
ALIB_BOXING_VTABLE_DECLARE_ARRAYTYPE( char32_t  , vt_arr_char32_t)


#endif // !ALIB_DOCUMENTATION_PARSER
namespace aworx { namespace lib { namespace boxing  {
/**
 * This type-traits struct by default inherits \c std::false_type. If specialized for
 * template type \p{TCharArray} to inherit \c std::true_type, then boxing that type will not be
 * customized automatically with a corresponding specialization of \alib{characters,T_CharArray}.
 * This keeps the customization of boxing open to be performed in a different way.
 *
 * \see
 *   See manual chapter \ref alib_boxing_strings "10. Boxing Character Strings" of the
 *   Programmer's Manual of module \alibmod_nolink_boxing.
 *
 * @tparam TCharArray The type that \alib{characters,T_CharArray} is specialized for but still no
 *                    character array boxing should be performed.
 */
template<typename TCharArray>      struct T_SuppressCharArrayBoxing    :    std::false_type      {};
}}}
#if !ALIB_DOCUMENTATION_PARSER
namespace aworx { namespace lib { namespace boxing  {


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


ALIBTMP_BOXER_WITHOUT_UNBOXING(nchar)  ALIBTMP_BOXER_WITH_UNBOXING(   nchar)
ALIBTMP_BOXER_WITHOUT_UNBOXING(wchar)  ALIBTMP_BOXER_WITH_UNBOXING(   wchar)
ALIBTMP_BOXER_WITHOUT_UNBOXING(xchar)  ALIBTMP_BOXER_WITH_UNBOXING(   xchar)

#undef ALIBTMP_BOXER_WITHOUT_UNBOXING
#undef ALIBTMP_BOXER_WITH_UNBOXING



}}}

#endif //ALIB_DOCUMENTATION_PARSER

