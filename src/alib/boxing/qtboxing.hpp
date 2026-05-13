//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
///
/// <b>Legal Notice:</b>
/// This is an optional extension header to provide compatibility between ALib and
/// the QT class library.
/// All information about QT is found at https://www.qt.io
/// ALib otherwise does not use or rely on QT.
/// The use of QT is bound to the QT license restrictions.
//==================================================================================================

namespace alib::boxing::compatibility {
/// This namespace documents compatibility features of \alib_boxing_nl and the
/// \https{QT Class Library,www.qt.io}.
namespace qt {

void BootstrapQTStringBoxing();
/// Initializes \alib_boxing_nl in respect to boxing QT string-types.
///
/// This method is \b not automatically invoked with function #"Bootstrap", because support
/// for boxing QT string-types is optional and provided with the inclusion of header
/// #"F;ALib.Compatibility.QTBoxing.H".
///
/// In general, boxing of QT string-types works well without the one-time invocation of
/// this function at the bootstrap section of a process.
/// This method registers box-function #"FAppend" for QT string
/// types when #"alib_boxing_customizing_identity;custom boxing is bypassed" by wrapping the
/// types in \c std::reference_wrapper<T>.
/// The function is implemented with the help of #"FAppend::WrappedAppendable"
/// for wrapped types \b QByteArray, \b QQLatin1String and \b QString, each for character type
/// #"characters::nchar" and #"characters::wchar".
///
/// \note
///   If invoked after bootstrap and module \alib_monomem_nl is included in the \alibbuild,
///   mutex #"GLOBAL_ALLOCATOR_LOCK" has to be locked before an invocation.
inline void BootstrapQTStringBoxing() {
    #if ALIB_STRINGS
        alib::boxing::BootstrapRegister<boxing::FAppend<nchar, lang::HeapAllocator>, ::std::reference_wrapper<QByteArray>   >( boxing::FAppend<nchar, lang::HeapAllocator>::WrappedAppendable<QByteArray>    );
        alib::boxing::BootstrapRegister<boxing::FAppend<wchar, lang::HeapAllocator>, ::std::reference_wrapper<QByteArray>   >( boxing::FAppend<wchar, lang::HeapAllocator>::WrappedAppendable<QByteArray>    );
        alib::boxing::BootstrapRegister<boxing::FAppend<nchar, lang::HeapAllocator>, ::std::reference_wrapper<QLatin1String>>( boxing::FAppend<nchar, lang::HeapAllocator>::WrappedAppendable<QLatin1String> );
        alib::boxing::BootstrapRegister<boxing::FAppend<wchar, lang::HeapAllocator>, ::std::reference_wrapper<QLatin1String>>( boxing::FAppend<wchar, lang::HeapAllocator>::WrappedAppendable<QLatin1String> );
        alib::boxing::BootstrapRegister<boxing::FAppend<nchar, lang::HeapAllocator>, ::std::reference_wrapper<QString>      >( boxing::FAppend<nchar, lang::HeapAllocator>::WrappedAppendable<QString>       );
        alib::boxing::BootstrapRegister<boxing::FAppend<wchar, lang::HeapAllocator>, ::std::reference_wrapper<QString>      >( boxing::FAppend<wchar, lang::HeapAllocator>::WrappedAppendable<QString>       );
    #endif
}
}}  // namespace [alib::boxing::compatibility::qt]
