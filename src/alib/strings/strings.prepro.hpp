//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_STRINGS_PP
#define HPP_ALIB_STRINGS_PP
#pragma once
#ifndef INL_ALIB
#   include "alib/alib.inl"
#endif


//##################################################################################################
// Symbols introduced by module ALib.Strings
//##################################################################################################

//  ALIB_DEBUG_STRINGS
#if !ALIB_STRINGS
#   define   ALIB_DEBUG_STRINGS 0
#elif !defined(ALIB_DEBUG_STRINGS)
#   define   ALIB_DEBUG_STRINGS 0
#elif !ALIB_DEBUG && ALIB_DEBUG_STRINGS
#   undef    ALIB_DEBUG_STRINGS
#   define   ALIB_DEBUG_STRINGS 0
#   pragma message "Symbol ALIB_DEBUG_STRINGS set (from outside!) while ALIB_DEBUG is not. The symbol got disabled."
#endif

#if ALIB_STRINGS

#if ALIB_DEBUG_STRINGS
#   define ALIB_STRING_DBG_CHK(instance)  { (instance)->dbgCheck(); }
#else
#   define  ALIB_STRING_DBG_CHK(instance)
#endif


#if !ALIB_CHARACTERS_WIDE
#   define ALIB_STRINGS_TO_NARROW(        src,dest,bufSize )  decltype(src)& dest(src);
#   define ALIB_STRINGS_TO_NARROW_ARG(    src,bufSize      )  src;
#   define ALIB_STRINGS_TO_WIDE(          src,dest,bufSize )  alib::strings::TLocalString<wchar,bufSize> dest(src);
#   define ALIB_STRINGS_TO_WIDE_ARG(      src,bufSize      )  alib::strings::TLocalString<wchar,bufSize>(src);
#   define ALIB_STRINGS_FROM_NARROW(      src,dest,bufSize )  decltype(src)& dest(src);
#   define ALIB_STRINGS_FROM_NARROW_ARG(  src,bufSize      )  src;
#   define ALIB_STRINGS_FROM_WIDE(        src,dest,bufSize )  alib::strings::TLocalString<nchar,bufSize> dest(src);
#   define ALIB_STRINGS_FROM_WIDE_ARG     src,bufSize      )  alib::strings::TLocalString<nchar,bufSize>(src);
#else
#   define ALIB_STRINGS_TO_NARROW(        src,dest,bufSize )  alib::strings::TLocalString<nchar,bufSize> dest(src);
#   define ALIB_STRINGS_TO_NARROW_ARG(    src,bufSize      )  alib::strings::TLocalString<nchar,bufSize>(src);
#   define ALIB_STRINGS_TO_WIDE(          src,dest,bufSize )  decltype(src)& dest= src;
#   define ALIB_STRINGS_TO_WIDE_ARG(      src,bufSize      )  src;
#   define ALIB_STRINGS_FROM_NARROW(      src,dest,bufSize )  alib::strings::TLocalString<wchar,bufSize> dest(src);
#   define ALIB_STRINGS_FROM_NARROW_ARG(  src,bufSize      )  alib::strings::TLocalString<wchar,bufSize>(src);
#   define ALIB_STRINGS_FROM_WIDE(        src,dest,bufSize )  decltype(src)& dest= src;
#   define ALIB_STRINGS_FROM_WIDE_ARG(    src,bufSize      )  src;
#endif

#define ALIB_STRINGS_APPENDABLE_TYPE(TYPE)                                                         \
namespace alib::strings {                                                                          \
template<> struct       AppendableTraits<TYPE, alib::character, alib::lang::HeapAllocator>        {\
  ALIB_DLL void operator()(TAString<alib::character, alib::lang::HeapAllocator>&, const TYPE& );   \
};}

#define ALIB_STRINGS_APPENDABLE_TYPE_N(TYPE)                                                       \
namespace alib::strings {                                                                          \
template<> struct       AppendableTraits<TYPE, alib::nchar, alib::lang::HeapAllocator>            {\
    ALIB_DLL void operator()( TAString<alib::nchar, alib::lang::HeapAllocator>&, const TYPE& );    \
};}

#define ALIB_STRINGS_APPENDABLE_TYPE_W(TYPE)                                                       \
namespace alib::strings {                                                                          \
template<> struct       AppendableTraits<TYPE, alib::wchar, alib::lang::HeapAllocator>            {\
    ALIB_DLL void operator()( TAString<alib::wchar, alib::lang::HeapAllocator>&, const TYPE&);     \
};}

#define ALIB_STRINGS_APPENDABLE_TYPE_DEF(TYPE, IMPL)                                               \
void alib::strings::AppendableTraits<TYPE,alib::character, alib::lang::HeapAllocator>              \
  ::operator()( TAString<alib::character, alib::lang::HeapAllocator>& target, const TYPE& src){IMPL}

#define ALIB_STRINGS_APPENDABLE_TYPE_DEF_N(TYPE, IMPL)                                             \
void alib::strings::AppendableTraits<TYPE,alib::nchar, alib::lang::HeapAllocator>                  \
  ::operator()( TAString<alib::nchar, alib::lang::HeapAllocator>& target, const TYPE& src) {IMPL}

#define ALIB_STRINGS_APPENDABLE_TYPE_DEF_W(TYPE, IMPL)                                             \
void alib::strings::AppendableTraits<TYPE,alib::wchar, alib::lang::HeapAllocator>                  \
  ::operator()( TAString<alib::wchar, alib::lang::HeapAllocator>& target, const TYPE& src) {IMPL}


#define ALIB_STRINGS_APPENDABLE_TYPE_INLINE(TYPE, IMPL)                                            \
namespace alib::strings {                                                                          \
template<> struct       AppendableTraits<TYPE,alib::character, alib::lang::HeapAllocator>         {\
   void operator()(TAString<alib::character, lang::HeapAllocator>& target, const TYPE& src)  {IMPL}\
};}

#define ALIB_STRINGS_APPENDABLE_TYPE_INLINE_N(TYPE, IMPL)                                          \
namespace alib::strings {                                                                          \
template<> struct       AppendableTraits<TYPE,alib::nchar, alib::lang::HeapAllocator>             {\
   void operator()(TAString<alib::nchar, alib::lang::HeapAllocator>& target, const TYPE& src){IMPL}\
};}

#define ALIB_STRINGS_APPENDABLE_TYPE_INLINE_W(TYPE, IMPL)                                          \
namespace alib::strings {                                                                          \
template<> struct       AppendableTraits<TYPE,alib::wchar, alib::lang::HeapAllocator>             {\
   void operator()(TAString<alib::wchar, alib::lang::HeapAllocator>& target, const TYPE& src){IMPL}\
};}

#define   ALIB_STRING_RESETTER(astring)                                                            \
alib::strings::TStringLengthResetter<                                                              \
            std::remove_reference_t<decltype(astring)>::value_type,                                \
            std::remove_reference_t<decltype(astring)>::AllocatorType >                            \
                                                                   ALIB_IDENTIFIER(astring)(astring)


//##################################################################################################
// Symbols introduced by module ALib.Strings.StdIOStream
//##################################################################################################

#define ALIB_STRINGS_SUPPRESS_STD_OSTREAM_OPERATOR(TYPE)                                           \
    namespace alib::strings::compatibility::std {                                                  \
    template<> struct SuppressStdOStreamOpTraits<TYPE>  : ::std::true_type {};      }



#endif
#endif // HPP_ALIB_STRINGS_PP
