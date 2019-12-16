/** ************************************************************************************************
 * \file
 * This header file is part of the \aliblong.
 *
 * \emoji :copyright: 2013-2019 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 *
 * <b>Legal Notice:</b>
 * This is an optional extension header to provide compatibility between ALib and
 * the QT class library.
 * All information about QT is found at https://www.qt.io
 * ALib otherwise does not use or rely on QT.
 * The use of QT is bound to the QT license restrictions.
 **************************************************************************************************/
#ifndef HPP_ALIB_COMPATIBILITY_BOXING_QT
#define HPP_ALIB_COMPATIBILITY_BOXING_QT 1

#if !defined(HPP_ALIB_MODULES) && !defined(ALIB_DOX)
#   include "alib/lib/modules.hpp"
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

#if ALIB_CHARACTERS && !defined(HPP_ALIB_COMPATIBILITY_QT_CHARACTERS)
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
 * This namespace documents compatibility features of \alib_boxing_nl and the
 * \https{QT Class Library,www.qt.io}.
 */
namespace qt {



void BootstrapQTStringBoxing();
/**
 * Initializes \alib_boxing_nl in respect to boxing QT string types.
 *
 * This method is \b not automatically invoked with \alib{ALibDistribution::Bootstrap}, because support
 * for boxing QT string types is optional and provided with the inclusion of header
 * \alibheader{compatibility/qt_boxing.hpp}.
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
 *   If invoked after bootstrap and module \alib_monomem_nl is included in the \alibdist,
 *   mutex \alib{monomem,GlobalAllocatorLock} has to be locked prior to an invocation.
 */
inline void BootstrapQTStringBoxing()
{
    #if ALIB_STRINGS
        aworx::lib::boxing::BootstrapRegister<boxing::FAppend<nchar>, boxing::TMappedTo<::std::reference_wrapper<QByteArray>   >>( boxing::FAppend<nchar>::WrappedAppendable<QByteArray>    );
        aworx::lib::boxing::BootstrapRegister<boxing::FAppend<wchar>, boxing::TMappedTo<::std::reference_wrapper<QByteArray>   >>( boxing::FAppend<wchar>::WrappedAppendable<QByteArray>    );
        aworx::lib::boxing::BootstrapRegister<boxing::FAppend<nchar>, boxing::TMappedTo<::std::reference_wrapper<QLatin1String>>>( boxing::FAppend<nchar>::WrappedAppendable<QLatin1String> );
        aworx::lib::boxing::BootstrapRegister<boxing::FAppend<wchar>, boxing::TMappedTo<::std::reference_wrapper<QLatin1String>>>( boxing::FAppend<wchar>::WrappedAppendable<QLatin1String> );
        aworx::lib::boxing::BootstrapRegister<boxing::FAppend<nchar>, boxing::TMappedTo<::std::reference_wrapper<QString>      >>( boxing::FAppend<nchar>::WrappedAppendable<QString>       );
        aworx::lib::boxing::BootstrapRegister<boxing::FAppend<wchar>, boxing::TMappedTo<::std::reference_wrapper<QString>      >>( boxing::FAppend<wchar>::WrappedAppendable<QString>       );
    #endif
}
}}}}}

#endif // !defined(ALIB_QT_LIB_NOT_FOUND) // this is only set in an automated test project

#endif // HPP_ALIB_COMPATIBILITY_BOXING_QT
