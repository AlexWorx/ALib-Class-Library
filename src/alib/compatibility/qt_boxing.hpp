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
#ifndef HPP_ALIB_COMPATIBILITY_BOXING_QT
#define HPP_ALIB_COMPATIBILITY_BOXING_QT 1

#if !defined(HPP_ALIB_LIB_PREDEF_MODULES)
#   include "alib/lib/predef_modules.hpp"
#endif

ALIB_ASSERT_MODULE(BOXING)

#if !defined(ALIB_QT_LIB_NOT_FOUND) // this is only set in an automated test project

#if defined(__clang__)
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wreserved-id-macro"
    #pragma clang diagnostic ignored "-Wredundant-parens"
    #pragma clang diagnostic ignored "-Wcovered-switch-default"
    #pragma clang diagnostic ignored "-Wsign-conversion"
    #pragma clang diagnostic ignored "-Wdeprecated"
    #pragma clang diagnostic ignored "-Wunused-variable"
    #pragma clang diagnostic ignored "-Wcomma"
#endif
#include <QtCore/QString>
#include <QtCore/QVector>

#if defined(__clang__)
    #pragma clang diagnostic pop
#endif

#if ALIB_MODULE_CHARACTERS && !defined(HPP_ALIB_COMPATIBILITY_QT_CHARACTERS)
#   include "alib/compatibility/qt_characters.hpp"
#endif


#if !defined(HPP_ALIB_BOXING_BOXING)
#   include "alib/boxing/boxing.hpp"
#endif

#if !defined(_GLIBCXX_FUNCTIONAL) && !defined(_FUNCTIONAL_)
    #include <functional>
#endif


namespace aworx { namespace lib { namespace boxing { namespace compatibility {
/**
 * This namespace documents compatibility features of \alibmod_nolink_boxing and the
 * [QT Class Library](https://www.qt.io).
 */
namespace qt {



void InitQTStringBoxing();
/**
 * Initializes \alibmod_nolink_boxing in respect to boxing QT string types.
 *
 * This method is \b not automatically invoked with \alib{ALibModules::Init}, because support
 * for boxing QT string types is optional and provided with the inclusion of header
 * \ref alib/compatibility/qt_boxing.hpp.
 *
 * In general, boxing of QT string types works well without the one-time invocation of
 * this function at the bootstrap section of a process.
 * This method registers box-function \alib{boxing,FAppend} for QT string
 * types when \ref alib_boxing_customizing_identity "custom boxing is bypassed" by wrapping the
 * types in \c std::reference_wrapper<T>.
 * The function is implemented with the help of \alib{boxing,FAppend::WrappedAppendable}
 * for wrapped types \b QByteArray, \b QQLatin1String and \b QString, each for character types
 * \b nchar and \b wchar.
 *
 * \note
 *   Although the method must be invoked only once, it is defined inline to avoid the need of
 *   including a corresponding compilation unit.
 */
inline void InitQTStringBoxing()
{
    #if ALIB_MODULE_STRINGS
        aworx::lib::boxing::Register<boxing::FAppend<nchar>, boxing::TMappedTo<::std::reference_wrapper<QByteArray>   >>( boxing::FAppend<nchar>::WrappedAppendable<QByteArray>    );
        aworx::lib::boxing::Register<boxing::FAppend<wchar>, boxing::TMappedTo<::std::reference_wrapper<QByteArray>   >>( boxing::FAppend<wchar>::WrappedAppendable<QByteArray>    );
        aworx::lib::boxing::Register<boxing::FAppend<nchar>, boxing::TMappedTo<::std::reference_wrapper<QLatin1String>>>( boxing::FAppend<nchar>::WrappedAppendable<QLatin1String> );
        aworx::lib::boxing::Register<boxing::FAppend<wchar>, boxing::TMappedTo<::std::reference_wrapper<QLatin1String>>>( boxing::FAppend<wchar>::WrappedAppendable<QLatin1String> );
        aworx::lib::boxing::Register<boxing::FAppend<nchar>, boxing::TMappedTo<::std::reference_wrapper<QString>      >>( boxing::FAppend<nchar>::WrappedAppendable<QString>       );
        aworx::lib::boxing::Register<boxing::FAppend<wchar>, boxing::TMappedTo<::std::reference_wrapper<QString>      >>( boxing::FAppend<wchar>::WrappedAppendable<QString>       );
    #endif
}
}}}}}

#endif // !defined(ALIB_QT_LIB_NOT_FOUND) // this is only set in an automated test project

#endif // HPP_ALIB_COMPATIBILITY_BOXING_QT
