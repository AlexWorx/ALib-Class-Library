// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#if !defined(HPP_ALIB_STRING_PROPPERINCLUDE)
#   error "ALib sources with ending '.inl' must not be included from outside."
#endif

#if !defined (HPP_ALIB_CHARACTERS_CHARARRAY)
#   include "alib/characters/chararray.hpp"
#endif

namespace aworx { namespace lib { namespace strings {


#if !ALIB_DOCUMENTATION_PARSER
template<typename TChar>  struct T_SuppressAutoCast<TAString<TChar>, characters::ConstructionType::Implicit,   TCString<TChar> > : public std::true_type {};
template<typename TChar>  struct T_SuppressAutoCast<TAString<TChar>, characters::ConstructionType::Implicit, TSubstring<TChar> > : public std::true_type {};
template<typename TChar>  struct T_SuppressAutoCast<TCString<TChar>, characters::ConstructionType::Implicit, TSubstring<TChar> > : public std::true_type {};
#endif


#if !ALIB_DOCUMENTATION_PARSER

// #################################################################################################
// Specializations of T_CharArray for ALib string types
// #################################################################################################
} namespace characters {

template<typename TChar>     struct  T_CharArray<strings::TString<TChar>, TChar>
{
    static        constexpr AccessType                    Access       =       AccessType::Implicit;
    static inline constexpr const TChar*                  Buffer   (const strings::TString<TChar>& src) { return src.Buffer(); }
    static inline constexpr integer                       Length   (const strings::TString<TChar>& src) { return src.Length(); }

    static        constexpr ConstructionType              Construction = ConstructionType::Implicit;
    static inline constexpr strings::TString<TChar>       Construct(const TChar* array, integer length) { return strings::TString<TChar>(array, length); }
};

template<typename TChar>    struct  T_CharArray<strings::TSubstring<TChar>, TChar>
{
    static        constexpr AccessType                    Access       =       AccessType::Implicit;
    static inline constexpr const TChar*                  Buffer(const strings::TSubstring<TChar>& src) { return src.Buffer(); }
    static inline constexpr integer                       Length(const strings::TSubstring<TChar>& src) { return src.Length(); }

    static        constexpr ConstructionType              Construction = ConstructionType::Implicit;
    static inline constexpr strings::TSubstring<TChar>    Construct(const TChar* array, integer length) { return strings::TSubstring<TChar>(array, length); }
};

template<typename TChar>    struct  T_CharArray<strings::TCString<TChar>, TChar>
{
    static        constexpr AccessType                    Access       =       AccessType::Implicit;
    static inline constexpr const TChar*                  Buffer(const strings::TCString<TChar>& src)  { return src.Buffer(); }
    static inline constexpr integer                       Length(const strings::TCString<TChar>& src)  { return src.Length(); }

    static        constexpr ConstructionType              Construction = ConstructionType::ExplicitOnly;
    static inline constexpr strings::TCString<TChar>      Construct(const TChar* array, integer length){ return strings::TCString<TChar>(array, length); }
};


template<typename TChar>    struct  T_CharArray<strings::TAString<TChar>, TChar>
{
    static        constexpr AccessType                    Access       =       AccessType::Implicit;
    static inline constexpr const TChar*                  Buffer(const strings::TAString<TChar>& src)  { return src.Buffer(); }
    static inline constexpr integer                       Length(const strings::TAString<TChar>& src)  { return src.Length(); }

    static        constexpr ConstructionType              Construction = ConstructionType::ExplicitOnly;
    static inline constexpr strings::TAString<TChar>      Construct(const TChar* array, integer length){ return strings::TAString<TChar>().Append(array, length); }
};

template<typename TChar, integer TCapacity>
struct  T_CharArray<strings::TLocalString<TChar,TCapacity>, TChar>
{
    static        constexpr AccessType                    Access       =       AccessType::Implicit;
    static inline constexpr const TChar*                  Buffer(const strings::TLocalString<TChar, TCapacity>& src)  { return src.Buffer(); }
    static inline constexpr integer                       Length(const strings::TLocalString<TChar, TCapacity>& src)  { return src.Length(); }

    static        constexpr ConstructionType              Construction = ConstructionType::NONE;
};

template<typename TChar>     struct  T_CharArray<strings::TStringNZT<TChar>, TChar>
{
    static        constexpr AccessType                    Access       =       AccessType::Implicit;
    static inline constexpr const TChar*                  Buffer   (const strings::TString<TChar>& src) { return src.Buffer(); }
    static inline constexpr integer                       Length   (const strings::TString<TChar>& src) { return src.Length(); }

    static        constexpr ConstructionType              Construction = ConstructionType::NONE;
};


// #################################################################################################
// Specializations of T_ZTCharArray for ALib string types
// #################################################################################################

template<typename TChar>     struct  T_ZTCharArray<strings::TString<TChar>, TChar>
{
    static        constexpr AccessType                    Access       =       AccessType::ExplicitOnly;
    static inline constexpr const TChar*                  Buffer(const strings::TString<TChar>& src)       { return src.Buffer(); }
    static inline constexpr integer                       Length(const strings::TString<TChar>& src)       { return src.Length(); }

    static        constexpr ConstructionType              Construction = ConstructionType::ExplicitOnly;
    static inline constexpr strings::TString<TChar>       Construct(const TChar* array, integer length   ) { return strings::TString<TChar>(array, length); }
};

template<typename TChar>    struct  T_ZTCharArray<strings::TSubstring<TChar>, TChar>
{
    static        constexpr AccessType                    Access       =       AccessType::ExplicitOnly;
    static inline constexpr const TChar*                  Buffer(const strings::TSubstring<TChar>& src)    { return src.Buffer(); }
    static inline constexpr integer                       Length(const strings::TSubstring<TChar>& src)    { return src.Length(); }

    static        constexpr ConstructionType              Construction = ConstructionType::ExplicitOnly;
    static inline constexpr strings::TSubstring<TChar>    Construct(const TChar* array, integer length   ) { return strings::TSubstring<TChar>(array, length); }
};


template<typename TChar>    struct  T_ZTCharArray<strings::TCString<TChar>, TChar>
{
    static        constexpr AccessType                    Access       =       AccessType::Implicit;
    static inline constexpr const TChar*                  Buffer(const strings::TCString<TChar>& src)      { return src.Buffer(); }
    static inline constexpr integer                       Length(const strings::TCString<TChar>& src)      { return src.Length(); }

    static        constexpr ConstructionType              Construction = ConstructionType::ExplicitOnly;
    static inline constexpr strings::TCString<TChar>      Construct(const TChar* array, integer length )   { return strings::TCString<TChar>(array, length); }
};


template<typename TChar>    struct  T_ZTCharArray<strings::TAString<TChar>, TChar>
{
    static        constexpr AccessType                    Access       =       AccessType::Implicit;
    static inline constexpr const TChar*                  Buffer(const strings::TAString<TChar>& src)      { return src.Terminate(); }
    static inline constexpr integer                       Length(const strings::TAString<TChar>& src)      { return src.Length(); }

    static        constexpr ConstructionType              Construction = ConstructionType::ExplicitOnly;
    static inline constexpr strings::TAString<TChar>      Construct(const TChar* array, integer length )   { return strings::TAString<TChar>().Append(array, length); }
};

template<typename TChar, integer TCapacity>
struct  T_ZTCharArray<strings::TLocalString<TChar,TCapacity>, TChar>
{
    static        constexpr AccessType                    Access       =       AccessType::Implicit;
    static inline constexpr const TChar*                  Buffer(const strings::TLocalString<TChar, TCapacity>& src)  { return src.Terminate(); }
    static inline constexpr integer                       Length(const strings::TLocalString<TChar, TCapacity>& src)  { return src.Length(); }

    static        constexpr ConstructionType              Construction = ConstructionType::NONE;
};


} namespace strings {

#endif // !ALIB_DOCUMENTATION_PARSER


}}} // namespace [aworx::lib::strings]
