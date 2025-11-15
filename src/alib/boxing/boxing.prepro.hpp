//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_BOXING_PP
#define HPP_ALIB_BOXING_PP
#pragma once
#ifndef INL_ALIB
#   include "alib/alib.inl"
#endif

//##################################################################################################
// Symbols/macros introduced by module ALib.Boxing
//##################################################################################################
#if !defined(ALIB_FEAT_BOXING_BIJECTIVE_INTEGRALS)
#   define ALIB_FEAT_BOXING_BIJECTIVE_INTEGRALS     0
#endif

#if !defined(ALIB_FEAT_BOXING_BIJECTIVE_CHARACTERS)
#   define ALIB_FEAT_BOXING_BIJECTIVE_CHARACTERS    0
#endif

#if !defined(ALIB_FEAT_BOXING_BIJECTIVE_FLOATS)
#   define ALIB_FEAT_BOXING_BIJECTIVE_FLOATS        0
#endif

#if !ALIB_BOXING
#   define ALIB_DEBUG_BOXING                        0
#else
#   if !defined(ALIB_DEBUG_BOXING)
#      define ALIB_DEBUG_BOXING                        0
#   elif !ALIB_DEBUG && ALIB_DEBUG_BOXING
#      undef    ALIB_DEBUG_BOXING
#      define   ALIB_DEBUG_BOXING                      0
#      pragma message "Symbol ALIB_DEBUG_BOXING set (from outside!) while ALIB_DEBUG is not. The symbol got disabled."
#   endif


#define ALIB_BOXING_CUSTOMIZE(TSource, TMapped, ...)                                               \
namespace alib::boxing {                                                                           \
template<>  struct BoxTraits<TSource>                                                              \
{                                                                                                  \
    using                       Mapping=  TMapped;                                                 \
    static constexpr bool       IsArray=  false;                                                   \
                                __VA_ARGS__                                                        \
}; }                                                                                               \

#define ALIB_BOXING_CUSTOMIZE_ARRAY_TYPE(TSource, TElement, ...)                                   \
namespace alib::boxing {                                                                           \
template<>  struct BoxTraits<TSource>                                                              \
{                                                                                                  \
    using                       Mapping=  TElement;                                                \
    static constexpr bool       IsArray=  true;                                                    \
                                __VA_ARGS__                                                        \
}; }


#define ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING(TSource, TMapped)                                       \
namespace alib::boxing {                                                                           \
template<>  struct BoxTraits<TSource>                                                              \
{ using                         Mapping=  TMapped;                                                 \
  static constexpr bool         IsArray=  false;                                                   \
  static constexpr void         Write( Placeholder& box,  TSource const & value )  { box.Write( static_cast<TMapped>( value ) );      } \
  static constexpr TSource      Read (const Placeholder& box)                      { return static_cast<TSource>(box.Read<TMapped>());   } \
}; }


#define ALIB_BOXING_CUSTOMIZE_NOT_UNBOXABLE(TSource, TMapped)                                      \
namespace alib::boxing {                                                                           \
template<>  struct BoxTraits<TSource>                                                              \
{ using                     Mapping=  TMapped;                                                     \
  static constexpr bool     IsArray=  false;                                                       \
  static constexpr void     Write( Placeholder& box,  TSource const & value )  { box.Write( static_cast<TMapped>( value ) ); } \
  static constexpr void     Read( const Placeholder& box);                                         \
}; }


#define ALIB_BOXING_CUSTOMIZE_ARRAY_TYPE_NON_UNBOXABLE(TSource, TElement, ...)                     \
namespace alib::boxing {                                                                           \
template<>  struct BoxTraits<TSource>                                                              \
{   using                   Mapping=  TElement;                                                    \
    static constexpr bool   IsArray=  true;                                                        \
    __VA_ARGS__                                                                                    \
    static constexpr void   Read( const Placeholder& box);                                         \
}; }


#define ALIB_BOXING_CUSTOMIZE_DENY_BOXING(TSource)                                                 \
namespace alib::boxing {                                                                           \
template<>  struct BoxTraits<TSource>                                                              \
{ using                     Mapping=  NotBoxableTag;                                               \
  static constexpr bool     IsArray=  false;                                                       \
  static constexpr void     Write( Placeholder& box,  TSource const & value );                     \
  static constexpr void     Read ( const Placeholder& box);                                        \
}; }


#define ALIB_BOXING_VTABLE_DECLARE( TMapped, Identifier )                                          \
ALIB_EXPORT namespace alib::boxing { namespace detail {                                            \
extern ALIB_DLL VTable SNGLTN_ ## Identifier; }                                                    \
template<> struct VTableOptimizationTraits< TMapped, false > {                                     \
static constexpr detail::VTable* Get() { return &detail:: SNGLTN_ ## Identifier; } };   }          \

#define ALIB_BOXING_VTABLE_DECLARE_ARRAYTYPE( TMapped, Identifier )                                \
ALIB_EXPORT namespace alib::boxing { namespace detail {                                            \
extern ALIB_DLL VTable SNGLTN_ ## Identifier; }                                                    \
template<> struct VTableOptimizationTraits< TMapped, true > {                                      \
static constexpr detail::VTable* Get() { return  &detail:: SNGLTN_ ## Identifier;}  };   }         \

#define ALIB_BOXING_VTABLE_DEFINE( TMapped, Identifier )                                           \
alib::boxing::detail::VTable alib::boxing::detail::SNGLTN_ ## Identifier                           \
( typeid(TMapped), typeid(void)  ,                                                                 \
   std::is_pointer<TMapped>::value                                                                 \
 ? VTable::MappingType::Pointer                                                                    \
 : std::is_enum<TMapped>::value                                                                    \
 ? VTable::MappingType::Enum                                                                       \
 : VTable::MappingType::Value   ,                                                                  \
 alib::boxing::SizeTraits<TMapped>);                                                               \

#define ALIB_BOXING_VTABLE_DEFINE_ARRAYTYPE( TMapped, Identifier )                                 \
alib::boxing::detail::VTable alib::boxing::detail::SNGLTN_ ## Identifier                           \
(typeid(TMapped[1]) , typeid(TMapped), VTable::MappingType(sizeof(TMapped)), sizeof(Placeholder)); \


#if ALIB_DEBUG_BOXING
#   define ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( Identifier )                                 \
alib::boxing::debug::DbgRegisterVTable( &alib::boxing::detail::SNGLTN_ ## Identifier,              \
                                       alib::boxing::detail::VTable::DbgFactoryType::Static );
#else
#   define ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( Identifier )
#endif

#define ALIB_BOXING_DEFINE_FEQUALS_FOR_COMPARABLE_TYPE( TComparable )                              \
alib::boxing::BootstrapRegister< alib::boxing::FEquals,TComparable>(FEquals::ComparableTypes<TComparable>);

#define ALIB_BOXING_DEFINE_FISLESS_FOR_COMPARABLE_TYPE( TComparable )                              \
alib::boxing::BootstrapRegister< alib::boxing::FIsLess,TComparable>(FIsLess::ComparableTypes<TComparable>);

#define ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE(TAppendable)                    \
alib::boxing::BootstrapRegister<alib::boxing::FAppend<character, alib::lang::HeapAllocator>, TAppendable>        \
    (alib::boxing::FAppend<character, alib::lang::HeapAllocator>::Appendable<TAppendable>);

#define ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE_N(TAppendable)                  \
alib::boxing::BootstrapRegister<alib::boxing::FAppend<nchar    , alib::lang::HeapAllocator>, TAppendable>        \
    (alib::boxing::FAppend<nchar    , alib::lang::HeapAllocator>::Appendable<TAppendable>);

#define ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE_W(TAppendable)                  \
alib::boxing::BootstrapRegister<alib::boxing::FAppend<wchar    , alib::lang::HeapAllocator>, TAppendable>        \
    (alib::boxing::FAppend<wchar    , alib::lang::HeapAllocator>::Appendable<TAppendable>);

#define ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE_X(TAppendable)                  \
alib::boxing::BootstrapRegister<alib::boxing::FAppend<xchar    , alib::lang::HeapAllocator>, TAppendable>        \
    (alib::boxing::FAppend<xchar    , alib::lang::HeapAllocator>::Appendable<TAppendable>);




#endif
#endif // HPP_ALIB_BOXING_PP
