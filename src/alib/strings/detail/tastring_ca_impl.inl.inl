// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#if !DOXYGEN

#if !defined(ALIB_STRINGS_TASTRING_INSTANTIATION)
#   error "ALib sources with ending '.inl' must not be included from outside."
#endif

// #################################################################################################
// #################################################################################################
// This inlined compilation unit is to be included by compilation that specialize template
// type "strings::TAString" in respect to its two template parameters TChar and TAllocator.
// For this, before inclusion two preprocessor symbols have to be specified:
// - ASTR_TCHAR_SPEC  The character type
// - ASTR_TALLOC_SPEC The allocator type
// At the end of this header, both symbols will be set undefined, to allow a next invocation
// with new symbols (and to enforce the right usage).
//
// With "tastring_ca_decl.inl", the corresponding declarations are to be made.
// #################################################################################################
// #################################################################################################
#if !defined(ASTR_TCHAR_SPEC)
#  error "Preprocessor symbol ASTR_TCHAR_SPEC has to be given as ALIB_CHAR_TYPE_ID_XYZ before invoking this .INL"
#endif
#if !defined(ASTR_TALLOC_SPEC)
#  error "Preprocessor symbol ASTR_TCHAR_SPEC has to be given as ALIB_CHAR_TYPE_ID_XYZ before invoking this .INL"
#endif

#if    ASTR_TCHAR_SPEC == ALIB_CHAR_TYPE_ID_N
#   define TCHARNAME  nchar
#elif ASTR_TCHAR_SPEC == ALIB_CHAR_TYPE_ID_W
#   define TCHARNAME  wchar
#elif ASTR_TCHAR_SPEC == ALIB_CHAR_TYPE_ID_X
#   define TCHARNAME  xchar
#else
#   error
#endif



namespace alib::strings {
#if ALIB_DEBUG_STRINGS
template  void                                    TAString<TCHARNAME, ASTR_TALLOC_SPEC>::dbgCheck()     const;
#endif
template  void                                    TAString<TCHARNAME, ASTR_TALLOC_SPEC>::GrowBufferAtLeastBy(integer);
template  void                                    TAString<TCHARNAME, ASTR_TALLOC_SPEC>::SetBuffer          (integer);
template  void                                    TAString<TCHARNAME, ASTR_TALLOC_SPEC>::SetBuffer          (TCHARNAME*,integer,integer,lang::Responsibility);
template  integer                                 TAString<TCHARNAME, ASTR_TALLOC_SPEC>::SearchAndReplace   (TCHARNAME,TCHARNAME,integer,integer );
template  integer                                 TAString<TCHARNAME, ASTR_TALLOC_SPEC>::SearchAndReplace   (const TString<TCHARNAME>&,const TString<TCHARNAME>&,integer,integer,lang::Case,integer );
template  TAString<TCHARNAME, ASTR_TALLOC_SPEC>&  TAString<TCHARNAME, ASTR_TALLOC_SPEC>::Trim               (const TCString<TCHARNAME>& );
template  integer                                 TAString<TCHARNAME, ASTR_TALLOC_SPEC>::TrimAt             (integer,const TCString<TCHARNAME>& );


#if ALIB_DEBUG
template void AppendableTraits<std::type_info    , TCHARNAME, ASTR_TALLOC_SPEC>::operator()( TAString<TCHARNAME, ASTR_TALLOC_SPEC>&, const std::type_info&   );
#endif
#if ALIB_EXT_LIB_THREADS_AVAILABLE
template void AppendableTraits<std::thread::id   , TCHARNAME, ASTR_TALLOC_SPEC>::operator()( TAString<TCHARNAME, ASTR_TALLOC_SPEC>&, const std::thread::id&  );
#endif
template void AppendableTraits<lang::CallerInfo  , TCHARNAME, ASTR_TALLOC_SPEC>::operator()( TAString<TCHARNAME, ASTR_TALLOC_SPEC>&, const lang::CallerInfo&  );
template void AppendableTraits<DateTime::Duration, TCHARNAME, ASTR_TALLOC_SPEC>::operator()( TAString<TCHARNAME, ASTR_TALLOC_SPEC>&, const DateTime::Duration );
template void AppendableTraits<Ticks::Duration   , TCHARNAME, ASTR_TALLOC_SPEC>::operator()( TAString<TCHARNAME, ASTR_TALLOC_SPEC>&, const Ticks::Duration    );


template void AppendableTraits<bool              , TCHARNAME, ASTR_TALLOC_SPEC>::operator()( TAString<TCHARNAME, ASTR_TALLOC_SPEC>&, bool     );
template void AppendableTraits<  int64_t         , TCHARNAME, ASTR_TALLOC_SPEC>::operator()( TAString<TCHARNAME, ASTR_TALLOC_SPEC>&,  int64_t );
template void AppendableTraits< uint64_t         , TCHARNAME, ASTR_TALLOC_SPEC>::operator()( TAString<TCHARNAME, ASTR_TALLOC_SPEC>&, uint64_t );
template void AppendableTraits<float             , TCHARNAME, ASTR_TALLOC_SPEC>::operator()( TAString<TCHARNAME, ASTR_TALLOC_SPEC>&, float    );
template void AppendableTraits<double            , TCHARNAME, ASTR_TALLOC_SPEC>::operator()( TAString<TCHARNAME, ASTR_TALLOC_SPEC>&, double   );
template void AppendableTraits<TDec   <TCHARNAME>, TCHARNAME, ASTR_TALLOC_SPEC>::operator()( TAString<TCHARNAME, ASTR_TALLOC_SPEC>&, const TDec   <TCHARNAME>& );
template void AppendableTraits<TTab   <TCHARNAME>, TCHARNAME, ASTR_TALLOC_SPEC>::operator()( TAString<TCHARNAME, ASTR_TALLOC_SPEC>&, const TTab   <TCHARNAME>& );
#if !ALIB_BOXING
template void AppendableTraits<TField <TCHARNAME>, TCHARNAME, ASTR_TALLOC_SPEC>::operator()( TAString<TCHARNAME, ASTR_TALLOC_SPEC>&, const TField <TCHARNAME>& );
#endif
template void AppendableTraits<TEscape<TCHARNAME>, TCHARNAME, ASTR_TALLOC_SPEC>::operator()( TAString<TCHARNAME, ASTR_TALLOC_SPEC>&, const TEscape<TCHARNAME>& );
template void AppendableTraits<TBin   <TCHARNAME>, TCHARNAME, ASTR_TALLOC_SPEC>::operator()( TAString<TCHARNAME, ASTR_TALLOC_SPEC>&, const TBin   <TCHARNAME>& );
template void AppendableTraits<THex   <TCHARNAME>, TCHARNAME, ASTR_TALLOC_SPEC>::operator()( TAString<TCHARNAME, ASTR_TALLOC_SPEC>&, const THex   <TCHARNAME>& );
template void AppendableTraits<TOct   <TCHARNAME>, TCHARNAME, ASTR_TALLOC_SPEC>::operator()( TAString<TCHARNAME, ASTR_TALLOC_SPEC>&, const TOct   <TCHARNAME>& );
template void AppendableTraits<TFill  <TCHARNAME>, TCHARNAME, ASTR_TALLOC_SPEC>::operator()( TAString<TCHARNAME, ASTR_TALLOC_SPEC>&, const TFill  <TCHARNAME>& );

} // namespace [alib::strings]


#undef  TCHARNAME
#undef  ASTR_TALLOC_SPEC
#undef  ASTR_TCHAR_SPEC

#endif //if !DOXYGEN

