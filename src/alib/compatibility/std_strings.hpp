/** ************************************************************************************************
 * \file
 * This header file is part of the \aliblong.<br>
 * With the inclusion of this header compatibility features between \alib and the C++ standard
 * library are provided.
 *
 * \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_COMPATIBILITY_STD_STRINGS
#define HPP_ALIB_COMPATIBILITY_STD_STRINGS 1


#if ALIB_STRINGS

#if !defined (HPP_ALIB_COMPATIBILITY_STD_CHARACTERS)
    #include "alib/compatibility/std_characters.hpp"
#endif

#ifndef HPP_ALIB_STRINGS_ASTRING
#   include "alib/strings/astring.hpp"
#endif


#if defined(ALIB_DOX)
namespace alib::strings::compatibility::std {
#else
namespace alib::strings {
#endif

// ####################################   std::string_view    ######################################
#if (ALIB_CPP_STANDARD >= 20) || defined(ALIB_DOX)

template<typename  TStringView> ATMP_T_IF(bool, ATMP_EQ( TStringView, std:: string_view)            )  operator==  (const   String& lhs, const   TStringView& rhs) { return  lhs.CompareTo<true,lang::Case::Sensitive>(  String(rhs.data(), integer(rhs.length())) ) == 0; }
template<typename  TStringView> ATMP_T_IF(bool, ATMP_EQ( TStringView, std:: string_view)            )  operator<=> (const   String& lhs, const   TStringView& rhs) { return  lhs.CompareTo<true,lang::Case::Sensitive>(  String(rhs.data(), integer(rhs.length())) );      }
template<typename TWStringView> ATMP_T_IF(bool, ATMP_EQ(TWStringView, std::wstring_view)            )  operator==  (const  WString& lhs, const  TWStringView& rhs) { return  lhs.CompareTo<true,lang::Case::Sensitive>( WString(rhs.data(), integer(rhs.length())) ) == 0; }
template<typename TWStringView> ATMP_T_IF(bool, ATMP_EQ(TWStringView, std::wstring_view)            )  operator<=> (const  WString& lhs, const  TWStringView& rhs) { return  lhs.CompareTo<true,lang::Case::Sensitive>( WString(rhs.data(), integer(rhs.length())) );      }
template<typename TXStringView> ATMP_T_IF(bool, ATMP_EQ(TXStringView, std::basic_string_view<xchar>))  operator==  (const  XString& lhs, const  TXStringView& rhs) { return  lhs.CompareTo<true,lang::Case::Sensitive>( XString(rhs.data(), integer(rhs.length())) ) == 0; }
template<typename TXStringView> ATMP_T_IF(bool, ATMP_EQ(TXStringView, std::basic_string_view<xchar>))  operator<=> (const  XString& lhs, const  TXStringView& rhs) { return  lhs.CompareTo<true,lang::Case::Sensitive>( XString(rhs.data(), integer(rhs.length())) );      }

template<typename  TStringView> ATMP_T_IF(bool, ATMP_EQ( TStringView, std:: string_view)            )  operator==  (const   CString& lhs, const   TStringView& rhs) { return  lhs.CompareTo<true,lang::Case::Sensitive>(  String(rhs.data(), integer(rhs.length())) ) == 0; }
template<typename  TStringView> ATMP_T_IF(bool, ATMP_EQ( TStringView, std:: string_view)            )  operator<=> (const   CString& lhs, const   TStringView& rhs) { return  lhs.CompareTo<true,lang::Case::Sensitive>(  String(rhs.data(), integer(rhs.length())) );      }
template<typename TWStringView> ATMP_T_IF(bool, ATMP_EQ(TWStringView, std::wstring_view)            )  operator==  (const  WCString& lhs, const  TWStringView& rhs) { return  lhs.CompareTo<true,lang::Case::Sensitive>( WString(rhs.data(), integer(rhs.length())) ) == 0; }
template<typename TWStringView> ATMP_T_IF(bool, ATMP_EQ(TWStringView, std::wstring_view)            )  operator<=> (const  WCString& lhs, const  TWStringView& rhs) { return  lhs.CompareTo<true,lang::Case::Sensitive>( WString(rhs.data(), integer(rhs.length())) );      }
template<typename TXStringView> ATMP_T_IF(bool, ATMP_EQ(TXStringView, std::basic_string_view<xchar>))  operator==  (const  XCString& lhs, const  TXStringView& rhs) { return  lhs.CompareTo<true,lang::Case::Sensitive>( XString(rhs.data(), integer(rhs.length())) ) == 0; }
template<typename TXStringView> ATMP_T_IF(bool, ATMP_EQ(TXStringView, std::basic_string_view<xchar>))  operator<=> (const  XCString& lhs, const  TXStringView& rhs) { return  lhs.CompareTo<true,lang::Case::Sensitive>( XString(rhs.data(), integer(rhs.length())) );      }

template<typename  TStringView> ATMP_T_IF(bool, ATMP_EQ( TStringView, std:: string_view)            )  operator==  (const   AString& lhs, const   TStringView& rhs) { return  lhs.CompareTo<true,lang::Case::Sensitive>(  String(rhs.data(), integer(rhs.length())) ) == 0; }
template<typename  TStringView> ATMP_T_IF(bool, ATMP_EQ( TStringView, std:: string_view)            )  operator<=> (const   AString& lhs, const   TStringView& rhs) { return  lhs.CompareTo<true,lang::Case::Sensitive>(  String(rhs.data(), integer(rhs.length())) );      }
template<typename TWStringView> ATMP_T_IF(bool, ATMP_EQ(TWStringView, std::wstring_view)            )  operator==  (const  WAString& lhs, const  TWStringView& rhs) { return  lhs.CompareTo<true,lang::Case::Sensitive>( WString(rhs.data(), integer(rhs.length())) ) == 0; }
template<typename TWStringView> ATMP_T_IF(bool, ATMP_EQ(TWStringView, std::wstring_view)            )  operator<=> (const  WAString& lhs, const  TWStringView& rhs) { return  lhs.CompareTo<true,lang::Case::Sensitive>( WString(rhs.data(), integer(rhs.length())) );      }
template<typename TXStringView> ATMP_T_IF(bool, ATMP_EQ(TXStringView, std::basic_string_view<xchar>))  operator==  (const  XAString& lhs, const  TXStringView& rhs) { return  lhs.CompareTo<true,lang::Case::Sensitive>( XString(rhs.data(), integer(rhs.length())) ) == 0; }
template<typename TXStringView> ATMP_T_IF(bool, ATMP_EQ(TXStringView, std::basic_string_view<xchar>))  operator<=> (const  XAString& lhs, const  TXStringView& rhs) { return  lhs.CompareTo<true,lang::Case::Sensitive>( XString(rhs.data(), integer(rhs.length())) );      }


#endif


} // namespace [alib::strings]


#endif // ALIB_CHARACTERS
#endif // HPP_ALIB_COMPATIBILITY_STD_STRINGS
