//==================================================================================================
/// \file
/// This header file is part of module \alib_strings of the \aliblong.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#if !defined(HPP_ALIB_STRINGS_TASTRING_INLINING)
#   error "ALib sources with ending '.inl' must not be included from outside."
#endif
#include "alib/characters/chartraits.hpp"


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


#if    ASTR_TCHAR_SPEC == ALIB_CHAR_TYPE_ID_N
#   define TCHARNAME  nchar
#elif ASTR_TCHAR_SPEC == ALIB_CHAR_TYPE_ID_W
#   define TCHARNAME  wchar
#elif ASTR_TCHAR_SPEC == ALIB_CHAR_TYPE_ID_X
#   define TCHARNAME  xchar
#else
#   error
#endif

namespace alib {  namespace strings {

template<typename TChar>
class TFormat;


//! @cond NO_DOX

// #################################################################################################
// member implementations
// #################################################################################################

// instantiations
extern template       ALIB_API void                                   TAString<TCHARNAME, ASTR_TALLOC_SPEC>::GrowBufferAtLeastBy( integer minimumGrowth );
extern template       ALIB_API void                                   TAString<TCHARNAME, ASTR_TALLOC_SPEC>::SetBuffer          ( integer );
extern template       ALIB_API void                                   TAString<TCHARNAME, ASTR_TALLOC_SPEC>::SetBuffer          ( TCHARNAME*,integer,integer,lang::Responsibility );
extern template       ALIB_API integer                                TAString<TCHARNAME, ASTR_TALLOC_SPEC>::SearchAndReplace   ( TCHARNAME,TCHARNAME,integer,integer );
extern template       ALIB_API integer                                TAString<TCHARNAME, ASTR_TALLOC_SPEC>::SearchAndReplace   ( const TString<TCHARNAME>&,const TString<TCHARNAME>&,integer,integer,lang::Case,integer );
extern template       ALIB_API TAString<TCHARNAME, ASTR_TALLOC_SPEC>& TAString<TCHARNAME, ASTR_TALLOC_SPEC>::Trim               ( const TCString<TCHARNAME>& );
extern template       ALIB_API integer                                TAString<TCHARNAME, ASTR_TALLOC_SPEC>::TrimAt             ( integer,const TCString<TCHARNAME>& );
#if ALIB_DEBUG_STRINGS
extern template       ALIB_API void                                   TAString<TCHARNAME,ASTR_TALLOC_SPEC>::dbgCheck            () const;
#endif

// specializations
#if ASTR_TCHAR_SPEC == ALIB_CHAR_TYPE_ID_N
template<> template<> ALIB_API TAString<TCHARNAME, ASTR_TALLOC_SPEC>&  TAString<TCHARNAME, ASTR_TALLOC_SPEC>::Append<CHK,wchar>( const wchar*, integer);
template<> template<> ALIB_API TAString<TCHARNAME, ASTR_TALLOC_SPEC>&  TAString<TCHARNAME, ASTR_TALLOC_SPEC>::Append<NC ,wchar>( const wchar*, integer);
template<> template<> ALIB_API TAString<TCHARNAME, ASTR_TALLOC_SPEC>&  TAString<TCHARNAME, ASTR_TALLOC_SPEC>::Append<CHK,xchar>( const xchar*, integer);
template<> template<> ALIB_API TAString<TCHARNAME, ASTR_TALLOC_SPEC>&  TAString<TCHARNAME, ASTR_TALLOC_SPEC>::Append<NC ,xchar>( const xchar*, integer);
#endif
#if ASTR_TCHAR_SPEC == ALIB_CHAR_TYPE_ID_W
template<> template<> ALIB_API TAString<TCHARNAME, ASTR_TALLOC_SPEC>&  TAString<TCHARNAME, ASTR_TALLOC_SPEC>::Append<CHK,nchar>( const nchar*, integer);
template<> template<> ALIB_API TAString<TCHARNAME, ASTR_TALLOC_SPEC>&  TAString<TCHARNAME, ASTR_TALLOC_SPEC>::Append<NC ,nchar>( const nchar*, integer);
template<> template<> ALIB_API TAString<TCHARNAME, ASTR_TALLOC_SPEC>&  TAString<TCHARNAME, ASTR_TALLOC_SPEC>::Append<CHK,xchar>( const xchar*, integer);
template<> template<> ALIB_API TAString<TCHARNAME, ASTR_TALLOC_SPEC>&  TAString<TCHARNAME, ASTR_TALLOC_SPEC>::Append<NC ,xchar>( const xchar*, integer);
#endif
#if ASTR_TCHAR_SPEC == ALIB_CHAR_TYPE_ID_X
template<> template<> ALIB_API TAString<TCHARNAME, ASTR_TALLOC_SPEC>&  TAString<TCHARNAME, ASTR_TALLOC_SPEC>::Append<CHK,nchar>( const nchar*, integer);
template<> template<> ALIB_API TAString<TCHARNAME, ASTR_TALLOC_SPEC>&  TAString<TCHARNAME, ASTR_TALLOC_SPEC>::Append<NC ,nchar>( const nchar*, integer);
template<> template<> ALIB_API TAString<TCHARNAME, ASTR_TALLOC_SPEC>&  TAString<TCHARNAME, ASTR_TALLOC_SPEC>::Append<CHK,wchar>( const wchar*, integer);
template<> template<> ALIB_API TAString<TCHARNAME, ASTR_TALLOC_SPEC>&  TAString<TCHARNAME, ASTR_TALLOC_SPEC>::Append<NC ,wchar>( const wchar*, integer);
#endif



// #################################################################################################
// T_Append implementations
// #################################################################################################
extern template ALIB_API void T_Append<  int64_t                  , TCHARNAME, ASTR_TALLOC_SPEC>::operator()( TAString<TCHARNAME, ASTR_TALLOC_SPEC>&,  int64_t );
extern template ALIB_API void T_Append< uint64_t                  , TCHARNAME, ASTR_TALLOC_SPEC>::operator()( TAString<TCHARNAME, ASTR_TALLOC_SPEC>&, uint64_t );
extern template ALIB_API void T_Append<double                     , TCHARNAME, ASTR_TALLOC_SPEC>::operator()( TAString<TCHARNAME, ASTR_TALLOC_SPEC>&, double   );
#if ALIB_DEBUG
extern template ALIB_API void T_Append<std::type_info             , TCHARNAME, ASTR_TALLOC_SPEC>::operator()( TAString<TCHARNAME, ASTR_TALLOC_SPEC>&, const std::type_info&  );
#endif
#if ALIB_EXT_LIB_THREADS_AVAILABLE
extern template ALIB_API void T_Append<std::thread::id            , TCHARNAME, ASTR_TALLOC_SPEC>::operator()( TAString<TCHARNAME, ASTR_TALLOC_SPEC>&, const std::thread::id&  );
#endif
extern template ALIB_API void T_Append<lang::CallerInfo           , TCHARNAME, ASTR_TALLOC_SPEC>::operator()( TAString<TCHARNAME, ASTR_TALLOC_SPEC>&, const lang::CallerInfo&  );

extern template ALIB_API void T_Append<TFormat<TCHARNAME>         , TCHARNAME, ASTR_TALLOC_SPEC>::operator()( TAString<TCHARNAME, ASTR_TALLOC_SPEC>&, const TFormat<TCHARNAME>&  );
extern template ALIB_API void T_Append<TFormat<TCHARNAME>::Tab    , TCHARNAME, ASTR_TALLOC_SPEC>::operator()( TAString<TCHARNAME, ASTR_TALLOC_SPEC>&, const TFormat<TCHARNAME>::Tab&  );
extern template ALIB_API void T_Append<TFormat<TCHARNAME>::Field  , TCHARNAME, ASTR_TALLOC_SPEC>::operator()( TAString<TCHARNAME, ASTR_TALLOC_SPEC>&, const TFormat<TCHARNAME>::Field&  );
extern template ALIB_API void T_Append<TFormat<TCHARNAME>::Escape , TCHARNAME, ASTR_TALLOC_SPEC>::operator()( TAString<TCHARNAME, ASTR_TALLOC_SPEC>&, const TFormat<TCHARNAME>::Escape&  );
extern template ALIB_API void T_Append<TFormat<TCHARNAME>::Bin    , TCHARNAME, ASTR_TALLOC_SPEC>::operator()( TAString<TCHARNAME, ASTR_TALLOC_SPEC>&, const TFormat<TCHARNAME>::Bin&  );
extern template ALIB_API void T_Append<TFormat<TCHARNAME>::Hex    , TCHARNAME, ASTR_TALLOC_SPEC>::operator()( TAString<TCHARNAME, ASTR_TALLOC_SPEC>&, const TFormat<TCHARNAME>::Hex&  );
extern template ALIB_API void T_Append<TFormat<TCHARNAME>::Oct    , TCHARNAME, ASTR_TALLOC_SPEC>::operator()( TAString<TCHARNAME, ASTR_TALLOC_SPEC>&, const TFormat<TCHARNAME>::Oct&  );
extern template ALIB_API void T_Append<TFormat<TCHARNAME>::Fill   , TCHARNAME, ASTR_TALLOC_SPEC>::operator()( TAString<TCHARNAME, ASTR_TALLOC_SPEC>&, const TFormat<TCHARNAME>::Fill& );


//! @endcond

}} // namespace [alib::strings]

#undef  TCHARNAME
#undef  ASTR_TALLOC_SPEC
#undef  ASTR_TCHAR_SPEC





