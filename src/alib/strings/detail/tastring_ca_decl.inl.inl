//==================================================================================================
/// \file
/// This header-file is part of module \alib_strings of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#if !defined(ALIB_STRINGS_TASTRING_INSTANTIATION)
#   error "ALib sources with ending '.inl' must not be included from outside."
#endif


// #################################################################################################
// #################################################################################################
// This inlined header unit is to be included by compilation that specialize template
// type "strings::TAString" in respect to its two template parameters TChar and TAllocator.
// For this, before inclusion two preprocessor symbols have to be specified:
// - ASTR_TCHAR_SPEC  The character type
// - ASTR_TALLOC_SPEC The allocator type
// At the end of this header, both symbols will be set undefined, to allow a next invocation
// with new symbols (and to enforce the right usage).
//
// With "tastring_ca_impl.inl", the corresponding implementations are to be made.
// #################################################################################################
// #################################################################################################

#if !defined(ASTR_TCHAR_SPEC)
#  error "Preprocessor symbol ASTR_TCHAR_SPEC has to be given as ALIB_CHAR_TYPE_ID_XYZ before invoking this .INL"
#endif
#if !defined(ASTR_TALLOC_SPEC)
#  error "Preprocessor symbol ASTR_TCHAR_SPEC has to be given as ALIB_CHAR_TYPE_ID_XYZ before invoking this .INL"
#endif


#if DOXYGEN // strangely needed for doxygen
#elif ASTR_TCHAR_SPEC == ALIB_CHAR_TYPE_ID_N
#   define TCHARNAME  nchar
#   define TCHAR2     wchar
#   define TCHAR3     xchar
#elif ASTR_TCHAR_SPEC == ALIB_CHAR_TYPE_ID_W
#   define TCHARNAME  wchar
#   define TCHAR2     nchar
#   define TCHAR3     xchar
#elif ASTR_TCHAR_SPEC == ALIB_CHAR_TYPE_ID_X
#   define TCHARNAME  xchar
#   define TCHAR2     nchar
#   define TCHAR3     wchar
#else
#   error
#endif


ALIB_EXPORT namespace alib {  namespace strings {



//! @cond NO_DOX

// #################################################################################################
// member implementations
// #################################################################################################

// instantiations
extern template       ALIB_DLL void                                   TAString<TCHARNAME, ASTR_TALLOC_SPEC>::GrowBufferAtLeastBy( integer minimumGrowth );
extern template       ALIB_DLL void                                   TAString<TCHARNAME, ASTR_TALLOC_SPEC>::SetBuffer          ( integer );
extern template       ALIB_DLL void                                   TAString<TCHARNAME, ASTR_TALLOC_SPEC>::SetBuffer          ( TCHARNAME*,integer,integer,lang::Responsibility );
extern template       ALIB_DLL integer                                TAString<TCHARNAME, ASTR_TALLOC_SPEC>::SearchAndReplace   ( TCHARNAME,TCHARNAME,integer,integer );
extern template       ALIB_DLL integer                                TAString<TCHARNAME, ASTR_TALLOC_SPEC>::SearchAndReplace   ( const TString<TCHARNAME>&,const TString<TCHARNAME>&,integer,integer,lang::Case,integer );
extern template       ALIB_DLL TAString<TCHARNAME, ASTR_TALLOC_SPEC>& TAString<TCHARNAME, ASTR_TALLOC_SPEC>::Trim               ( const TCString<TCHARNAME>& );
extern template       ALIB_DLL integer                                TAString<TCHARNAME, ASTR_TALLOC_SPEC>::TrimAt             ( integer,const TCString<TCHARNAME>& );
#if ALIB_DEBUG_STRINGS
extern template       ALIB_DLL void                                   TAString<TCHARNAME,ASTR_TALLOC_SPEC>::dbgCheck            () const;
#endif

// #################################################################################################
// AppendableTraits implementations
// #################################################################################################
extern template ALIB_DLL void AppendableTraits<  int64_t         , TCHARNAME, ASTR_TALLOC_SPEC>::operator()( TAString<TCHARNAME, ASTR_TALLOC_SPEC>&,  int64_t );
extern template ALIB_DLL void AppendableTraits< uint64_t         , TCHARNAME, ASTR_TALLOC_SPEC>::operator()( TAString<TCHARNAME, ASTR_TALLOC_SPEC>&, uint64_t );
extern template ALIB_DLL void AppendableTraits<double            , TCHARNAME, ASTR_TALLOC_SPEC>::operator()( TAString<TCHARNAME, ASTR_TALLOC_SPEC>&, double   );
#if ALIB_DEBUG
extern template ALIB_DLL void AppendableTraits<std::type_info    , TCHARNAME, ASTR_TALLOC_SPEC>::operator()( TAString<TCHARNAME, ASTR_TALLOC_SPEC>&, const std::type_info&  );
#endif
#if ALIB_EXT_LIB_THREADS_AVAILABLE
extern template ALIB_DLL void AppendableTraits<std::thread::id   , TCHARNAME, ASTR_TALLOC_SPEC>::operator()( TAString<TCHARNAME, ASTR_TALLOC_SPEC>&, const std::thread::id&  );
#endif
extern template ALIB_DLL void AppendableTraits<lang::CallerInfo  , TCHARNAME, ASTR_TALLOC_SPEC>::operator()( TAString<TCHARNAME, ASTR_TALLOC_SPEC>&, const lang::CallerInfo&   );
extern template ALIB_DLL void AppendableTraits<DateTime::Duration, TCHARNAME, ASTR_TALLOC_SPEC>::operator()( TAString<TCHARNAME, ASTR_TALLOC_SPEC>&, const DateTime::Duration  );
extern template ALIB_DLL void AppendableTraits<Ticks::Duration   , TCHARNAME, ASTR_TALLOC_SPEC>::operator()( TAString<TCHARNAME, ASTR_TALLOC_SPEC>&, const Ticks::Duration     );

extern template ALIB_DLL void AppendableTraits<TDec   <TCHARNAME>, TCHARNAME, ASTR_TALLOC_SPEC>::operator()( TAString<TCHARNAME, ASTR_TALLOC_SPEC>&, const TDec   <TCHARNAME>& );
extern template ALIB_DLL void AppendableTraits<TTab   <TCHARNAME>, TCHARNAME, ASTR_TALLOC_SPEC>::operator()( TAString<TCHARNAME, ASTR_TALLOC_SPEC>&, const TTab   <TCHARNAME>& );
extern template ALIB_DLL void AppendableTraits<TEscape<TCHARNAME>, TCHARNAME, ASTR_TALLOC_SPEC>::operator()( TAString<TCHARNAME, ASTR_TALLOC_SPEC>&, const TEscape<TCHARNAME>& );
extern template ALIB_DLL void AppendableTraits<TBin   <TCHARNAME>, TCHARNAME, ASTR_TALLOC_SPEC>::operator()( TAString<TCHARNAME, ASTR_TALLOC_SPEC>&, const TBin   <TCHARNAME>& );
extern template ALIB_DLL void AppendableTraits<THex   <TCHARNAME>, TCHARNAME, ASTR_TALLOC_SPEC>::operator()( TAString<TCHARNAME, ASTR_TALLOC_SPEC>&, const THex   <TCHARNAME>& );
extern template ALIB_DLL void AppendableTraits<TOct   <TCHARNAME>, TCHARNAME, ASTR_TALLOC_SPEC>::operator()( TAString<TCHARNAME, ASTR_TALLOC_SPEC>&, const TOct   <TCHARNAME>& );
extern template ALIB_DLL void AppendableTraits<TFill  <TCHARNAME>, TCHARNAME, ASTR_TALLOC_SPEC>::operator()( TAString<TCHARNAME, ASTR_TALLOC_SPEC>&, const TFill  <TCHARNAME>& );

#if !ALIB_BOXING
extern template ALIB_DLL void AppendableTraits<TField <TCHARNAME>, TCHARNAME, ASTR_TALLOC_SPEC>::operator()( TAString<TCHARNAME, ASTR_TALLOC_SPEC>&, const TField <TCHARNAME>& );
#endif


//! @endcond

}} // namespace [alib::strings]

#undef  TCHARNAME
#undef  ASTR_TALLOC_SPEC
#undef  ASTR_TCHAR_SPEC
#undef  TCHAR2
#undef  TCHAR3





