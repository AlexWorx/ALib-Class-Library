//==================================================================================================
/// \file
/// This header file is part of the \aliblong.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
///
/// <b>Legal Notice:</b>
/// This is an optional extension header to provide compatibility between ALib and
/// the QT class library.
/// All information about QT is found at https://www.qt.io
/// ALib otherwise does not use or rely on QT.
/// The use of QT is bound to the QT license restrictions.
//==================================================================================================
#ifndef HPP_ALIB_COMPATIBILITY_BOXING_QT
#define HPP_ALIB_COMPATIBILITY_BOXING_QT 1
#pragma once
#if !defined(DOXYGEN)
#   include "alib/alib.hpp"
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
    #pragma clang diagnostic ignored "-Wduplicate-enum"
#endif
#include <QtCore/QString>
#include <QtCore/QVector>

#if defined(__clang__)
    #pragma clang diagnostic pop
#endif

#if ALIB_CHARACTERS
#   include "alib/compatibility/qt_characters.hpp"
#endif
#include "alib/boxing/boxing.hpp"
#include <functional>

namespace alib::boxing::compatibility {
/// This namespace documents compatibility features of \alib_boxing_nl and the
/// \https{QT Class Library,www.qt.io}.
namespace qt {



void BootstrapQTStringBoxing();
/// Initializes \alib_boxing_nl in respect to boxing QT string-types.
///
/// This method is \b not automatically invoked with function \alib{Bootstrap}, because support
/// for boxing QT string-types is optional and provided with the inclusion of header
/// \alibheader{compatibility/qt_boxing.hpp}.
///
/// In general, boxing of QT string-types works well without the one-time invocation of
/// this function at the bootstrap section of a process.
/// This method registers box-function \alib{boxing;FAppend} for QT string
/// types when \ref alib_boxing_customizing_identity "custom boxing is bypassed" by wrapping the
/// types in \c std::reference_wrapper<T>.
/// The function is implemented with the help of \alib{boxing;FAppend::WrappedAppendable}
/// for wrapped types \b QByteArray, \b QQLatin1String and \b QString, each for character types
/// \b nchar and \b wchar.
///
/// \note
///   If invoked after bootstrap and module \alib_monomem_nl is included in the \alibdist,
///   mutex \alib{monomem;GLOBAL_ALLOCATOR_LOCK} has to be locked before an invocation.
inline void BootstrapQTStringBoxing()
{
    #if ALIB_STRINGS
        alib::boxing::BootstrapRegister<boxing::FAppend<nchar, lang::HeapAllocator>, boxing::TMappedTo<::std::reference_wrapper<QByteArray>   >>( boxing::FAppend<nchar, lang::HeapAllocator>::WrappedAppendable<QByteArray>    );
        alib::boxing::BootstrapRegister<boxing::FAppend<wchar, lang::HeapAllocator>, boxing::TMappedTo<::std::reference_wrapper<QByteArray>   >>( boxing::FAppend<wchar, lang::HeapAllocator>::WrappedAppendable<QByteArray>    );
        alib::boxing::BootstrapRegister<boxing::FAppend<nchar, lang::HeapAllocator>, boxing::TMappedTo<::std::reference_wrapper<QLatin1String>>>( boxing::FAppend<nchar, lang::HeapAllocator>::WrappedAppendable<QLatin1String> );
        alib::boxing::BootstrapRegister<boxing::FAppend<wchar, lang::HeapAllocator>, boxing::TMappedTo<::std::reference_wrapper<QLatin1String>>>( boxing::FAppend<wchar, lang::HeapAllocator>::WrappedAppendable<QLatin1String> );
        alib::boxing::BootstrapRegister<boxing::FAppend<nchar, lang::HeapAllocator>, boxing::TMappedTo<::std::reference_wrapper<QString>      >>( boxing::FAppend<nchar, lang::HeapAllocator>::WrappedAppendable<QString>       );
        alib::boxing::BootstrapRegister<boxing::FAppend<wchar, lang::HeapAllocator>, boxing::TMappedTo<::std::reference_wrapper<QString>      >>( boxing::FAppend<wchar, lang::HeapAllocator>::WrappedAppendable<QString>       );
    #endif
}
}}  // namespace [alib::boxing::compatibility::qt]

#endif // !defined(ALIB_QT_LIB_NOT_FOUND) // this is only set in an automated test project

#endif // HPP_ALIB_COMPATIBILITY_BOXING_QT

