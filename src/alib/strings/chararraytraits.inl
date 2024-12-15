//==================================================================================================
/// \file
/// This header file is part of module \alib_strings of the \aliblong.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_STRINGS_CHARARRAYTRAITS
#define HPP_ALIB_STRINGS_CHARARRAYTRAITS 1
#pragma once
#if !defined(HPP_ALIB_STRINGS_STRING)
#   error "ALib sources with ending '.inl' must not be included from outside."
#endif
#if !DOXYGEN

namespace alib {  namespace strings {


template<typename TChar, typename TAllocator>  struct T_SuppressAutoCast<TAString<TChar, TAllocator>, characters::ConstructionType::Implicit,   TCString<TChar> > : public std::true_type {};
template<typename TChar, typename TAllocator>  struct T_SuppressAutoCast<TAString<TChar, TAllocator>, characters::ConstructionType::Implicit, TSubstring<TChar> > : public std::true_type {};
template<typename TChar>                       struct T_SuppressAutoCast<TCString<TChar>        , characters::ConstructionType::Implicit, TSubstring<TChar> > : public std::true_type {};

// #################################################################################################
// Specializations of T_CharArray for ALib string-types
// #################################################################################################
} namespace characters {

template<typename TChar>     struct  T_CharArray<strings::TString<TChar>, TChar>
{
    static constexpr AccessType                    Access       =       AccessType::Implicit;
    static constexpr const TChar*                  Buffer   (const strings::TString<TChar>& src) { return src.Buffer(); }
    static constexpr integer                       Length   (const strings::TString<TChar>& src) { return src.Length(); }

    static constexpr ConstructionType              Construction = ConstructionType::Implicit;
    static constexpr strings::TString<TChar>       Construct(const TChar* array, integer length) { return strings::TString<TChar>(array, length); }
};

template<typename TChar>    struct  T_CharArray<strings::TSubstring<TChar>, TChar>
{
    static constexpr AccessType                    Access       =       AccessType::Implicit;
    static constexpr const TChar*                  Buffer(const strings::TSubstring<TChar>& src) { return src.Buffer(); }
    static constexpr integer                       Length(const strings::TSubstring<TChar>& src) { return src.Length(); }

    static constexpr ConstructionType              Construction = ConstructionType::Implicit;
    static constexpr strings::TSubstring<TChar>    Construct(const TChar* array, integer length) { return strings::TSubstring<TChar>(array, length); }
};

template<typename TChar>    struct  T_CharArray<strings::TCString<TChar>, TChar>
{
    static constexpr AccessType                    Access       =       AccessType::Implicit;
    static constexpr const TChar*                  Buffer(const strings::TCString<TChar>& src)  { return src.Buffer(); }
    static constexpr integer                       Length(const strings::TCString<TChar>& src)  { return src.Length(); }

    static constexpr ConstructionType              Construction = ConstructionType::ExplicitOnly;
    static constexpr strings::TCString<TChar>      Construct(const TChar* array, integer length){ return strings::TCString<TChar>(array, length); }
};


template<typename TChar, typename TAllocator>    struct  T_CharArray<strings::TAString<TChar,TAllocator>, TChar>
{
    static constexpr AccessType                      Access       =       AccessType::Implicit;
    static constexpr const TChar*                    Buffer(const strings::TAString<TChar,TAllocator>& src)  { return src.Buffer(); }
    static constexpr integer                         Length(const strings::TAString<TChar,TAllocator>& src)  { return src.Length(); }

    static constexpr ConstructionType                Construction = ConstructionType::ExplicitOnly;
    static constexpr strings::TAString<TChar,TAllocator> Construct(const TChar* array, integer length){ return strings::TAString<TChar,TAllocator>().Append(array, length); }
};

template<typename TChar, integer TCapacity, typename TAllocator>
struct  T_CharArray<strings::TLocalString<TChar,TCapacity,TAllocator>, TChar>
{
    static constexpr AccessType                    Access       =       AccessType::Implicit;
    static constexpr const TChar*                  Buffer(const strings::TLocalString<TChar,TCapacity,TAllocator>& src)  { return src.Buffer(); }
    static constexpr integer                       Length(const strings::TLocalString<TChar,TCapacity,TAllocator>& src)  { return src.Length(); }

    static constexpr ConstructionType              Construction = ConstructionType::NONE;
};

template<typename TChar>     struct  T_CharArray<strings::TStringNZT<TChar>, TChar>
{
    static constexpr AccessType                    Access       =       AccessType::Implicit;
    static constexpr const TChar*                  Buffer   (const strings::TString<TChar>& src) { return src.Buffer(); }
    static constexpr integer                       Length   (const strings::TString<TChar>& src) { return src.Length(); }

    static constexpr ConstructionType              Construction = ConstructionType::NONE;
};


// #################################################################################################
// Specializations of T_ZTCharArray for ALib string-types
// #################################################################################################

template<typename TChar>     struct  T_ZTCharArray<strings::TString<TChar>, TChar>
{
    static constexpr AccessType                    Access       =       AccessType::ExplicitOnly;
    static constexpr const TChar*                  Buffer(const strings::TString<TChar>& src)       { return src.Buffer(); }
    static constexpr integer                       Length(const strings::TString<TChar>& src)       { return src.Length(); }

    static constexpr ConstructionType              Construction = ConstructionType::ExplicitOnly;
    static constexpr strings::TString<TChar>       Construct(const TChar* array, integer length   ) { return strings::TString<TChar>(array, length); }
};

template<typename TChar>    struct  T_ZTCharArray<strings::TSubstring<TChar>, TChar>
{
    static constexpr AccessType                    Access       =       AccessType::ExplicitOnly;
    static constexpr const TChar*                  Buffer(const strings::TSubstring<TChar>& src)    { return src.Buffer(); }
    static constexpr integer                       Length(const strings::TSubstring<TChar>& src)    { return src.Length(); }

    static constexpr ConstructionType              Construction = ConstructionType::ExplicitOnly;
    static constexpr strings::TSubstring<TChar>    Construct(const TChar* array, integer length   ) { return strings::TSubstring<TChar>(array, length); }
};


template<typename TChar>    struct  T_ZTCharArray<strings::TCString<TChar>, TChar>
{
    static constexpr AccessType                    Access       =       AccessType::Implicit;
    static constexpr const TChar*                  Buffer(const strings::TCString<TChar>& src)      { return src.Buffer(); }
    static constexpr integer                       Length(const strings::TCString<TChar>& src)      { return src.Length(); }

    static constexpr ConstructionType              Construction = ConstructionType::ExplicitOnly;
    static constexpr strings::TCString<TChar>      Construct(const TChar* array, integer length )   { return strings::TCString<TChar>(array, length); }
};


template<typename TChar, typename TAllocator>    struct  T_ZTCharArray<strings::TAString<TChar,TAllocator>, TChar>
{
    static constexpr AccessType                         Access       =       AccessType::Implicit;
    static constexpr const TChar*                       Buffer(const strings::TAString<TChar,TAllocator>& src)      { return src.Terminate(); }
    static constexpr integer                            Length(const strings::TAString<TChar,TAllocator>& src)      { return src.Length(); }

    static constexpr ConstructionType                   Construction = ConstructionType::ExplicitOnly;
    static constexpr strings::TAString<TChar,TAllocator>    Construct(const TChar* array, integer length )   { return strings::TAString<TChar,TAllocator>().Append(array, length); }
};

template<typename TChar, integer TCapacity, typename TAllocator>
struct  T_ZTCharArray<strings::TLocalString<TChar,TCapacity,TAllocator>, TChar>
{
    static constexpr AccessType                    Access       =       AccessType::Implicit;
    static constexpr const TChar*                  Buffer(const strings::TLocalString<TChar,TCapacity,TAllocator>& src)  { return src.Terminate(); }
    static constexpr integer                       Length(const strings::TLocalString<TChar,TCapacity,TAllocator>& src)  { return src.Length(); }

    static constexpr ConstructionType              Construction = ConstructionType::NONE;
};


} namespace strings {


}} // namespace [alib::strings]

#endif // !DOXYGEN
#endif // HPP_ALIB_STRINGS_CHARARRAYTRAITS

