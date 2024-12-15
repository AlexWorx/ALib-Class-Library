//==================================================================================================
/// \file
/// This header file is part of the \aliblong.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
///
/// <b>Legal Notice:</b>
/// This is an optional extension header to provide compatibility between \alib and
/// the QT class library.
/// All information about QT is found at https://www.qt.io
/// \alib otherwise does not use or rely on QT.
/// The use of QT is bound to the QT license restrictions.
//==================================================================================================
#ifndef HPP_ALIB_COMPATIBILITY_QT_STRINGS
#define HPP_ALIB_COMPATIBILITY_QT_STRINGS 1
#pragma once
#if !defined(DOXYGEN)
#   include "alib/alib.hpp"
#endif

#if !defined(ALIB_QT_LIB_NOT_FOUND) // this is only set in an automated test project

#   include "alib/compatibility/qt_characters.hpp"
#   include "alib/strings/astring.hpp"

namespace alib::strings {

#if DOXYGEN
namespace APPENDABLES {
#endif

// #############################################################################################
// T_Append
// #############################################################################################

/// Specialization of type-traits struct \alib{strings;T_Append} for type \c QChar.
template<typename TChar, typename TAllocator> struct T_Append<QChar, TChar, TAllocator>
{
    //==============================================================================================
    /// Appends \p{src} of type \b QChar to \p{target}.
    ///
    /// @param  target The AString to append \p{src} to.
    /// @param  src    The source string.
    //==============================================================================================
    void operator()( TAString<TChar,TAllocator>& target, const QChar& src )
    {
        target._( static_cast<ALIB_QTCHAR>( src.unicode() ) );
    }
};

#if DOXYGEN
}  // namespace alib::strings[::APPENDABLES]
#endif

}  // namespace [alib::strings]


#endif // !defined(ALIB_QT_LIB_NOT_FOUND) // this is only set in an automated test project
#endif // HPP_ALIB_COMPATIBILITY_QT_STRINGS

