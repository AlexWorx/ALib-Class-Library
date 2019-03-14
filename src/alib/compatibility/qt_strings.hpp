// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//
//  Legal Notice:
//    This is an optional extension header to provide compatibility between ALib and
//    the QT class library.
//    All information about QT is found under https://www.qt.io
//    ALib otherwise does not use or rely on QT.
//    The use of QT is bound to the QT license restrictions.
// #################################################################################################
#ifndef HPP_ALIB_COMPATIBILITY_QT_STRINGS
#define HPP_ALIB_COMPATIBILITY_QT_STRINGS 1

#if !defined(HPP_ALIB_LIB_PREDEF_MODULES)
#   include "alib/lib/predef_modules.hpp"
#endif

#if !defined(ALIB_QT_LIB_NOT_FOUND) // this is only set in an automated test project

ALIB_ASSERT_MODULE(STRINGS)


#if !defined(HPP_ALIB_COMPATIBILITY_QT_CHARACTERS)
#   include "alib/compatibility/qt_characters.hpp"
#endif

#if !defined(HPP_ALIB_STRINGS_ASTRING)
#   include "alib/strings/astring.hpp"
#endif

namespace aworx { namespace lib { namespace strings {

#if ALIB_DOCUMENTATION_PARSER
namespace APPENDABLES {
#endif

// #############################################################################################
// T_Append
// #############################################################################################

/** Specialization of type-traits struct \alib{strings,T_Append} for type \c QChar. */
template<typename TChar> struct T_Append<QChar, TChar>
{
    /** ****************************************************************************************
     * Appends \p{src} of type \b QChar to \p{target}.
     *
     * @param  target The AString to append \p{src} to.
     * @param  src    The source string.
     ******************************************************************************************/
    inline void operator()( TAString<TChar>& target, const QChar& src )
    {
        target._( static_cast<ALIB_QTCHAR>( src.unicode() ) );
    }
};

#if ALIB_DOCUMENTATION_PARSER
}  // namespace aworx::lib::strings[::APPENDABLES]
#endif

}}}  // namespace [aworx::lib::strings]


#endif // !defined(ALIB_QT_LIB_NOT_FOUND) // this is only set in an automated test project
#endif // HPP_ALIB_COMPATIBILITY_QT_STRINGS
