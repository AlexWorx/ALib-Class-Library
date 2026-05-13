//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
///
/// <b>Legal Notice:</b>
/// This is an optional extension header to provide compatibility between \alib and
/// the QT class library.
/// All information about QT is found at https://www.qt.io
/// \alib otherwise does not use or rely on QT.
/// The use of QT is bound to the QT license restrictions.
//==================================================================================================
namespace alib::strings {

#if DOXYGEN
namespace APPENDABLES {
#endif

//##################################################################################################
// AppendableTraits
//##################################################################################################

/// Specialization of the type trait #"AppendableTraits" for type \c QChar.
template<typename TChar, typename TAllocator>
struct AppendableTraits<QChar, TChar, TAllocator> {
    /// Appends \p{src} of type \b QChar to \p{target}.
    ///
    /// @param  target The AString to append \p{src} to.
    /// @param  src    The source string.
    void operator()( TAString<TChar,TAllocator>& target, const QChar& src ) {
        target._( static_cast<ALIB_QTCHAR>( src.unicode() ) );
    }
};

#if DOXYGEN
}  // namespace alib::strings[::APPENDABLES]
#endif

}  // namespace [alib::strings]
