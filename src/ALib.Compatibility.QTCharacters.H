//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
///
/// <b>Legal Notice:</b>
/// This is an optional extension header to provide compatibility between ALib and
/// the QT class library.
/// All information about QT is found at https://www.qt.io
/// ALib otherwise does not use or rely on QT.
/// The use of QT is bound to the QT license restrictions.
//==================================================================================================
#ifndef HPP_ALIB_COMPATIBILITY_QT_CHARACTERS
#define HPP_ALIB_COMPATIBILITY_QT_CHARACTERS 1
#pragma once
#if !defined(DOXYGEN)
#   include "alib/alib.inl"
#endif


#if !defined(ALIB_QT_LIB_NOT_FOUND) // this is only set in an automated test project

// include QT
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

#if defined(H_ALIB_BOXING)
#   error "Wrong inclusion order. Header 'alib/compatibility/qt_characters.hpp' has to be included before headers of module ALib Boxing."
#endif

#include "ALib.Lang.H"

namespace alib::characters {

// Faking all template specializations of namespace strings for doxygen into namespace
// strings::custom to keep the documentation of namespace string clean!
#if DOXYGEN
namespace compatibility {
/// Specializations of type traits \alib{characters;ArrayTraits} and
/// \alib{characters;ZTArrayTraits} for types found in the
/// \https{QT Class Library,www.qt.io}.
namespace qt {} }

#else

#define ALIB_QTCHAR   alib::characters::TypeBySize<2>
#define ALIB_QTCHAR_4 alib::characters::TypeBySize<4>

// QStringView
template<> struct  ArrayTraits<QStringView, ALIB_QTCHAR>
{
    static constexpr Policy    Access                                      = Policy::Implicit;
    static constexpr Policy    Construction                                = Policy::Implicit;
    static const ALIB_QTCHAR*  Buffer   (QStringView const & src)          { return reinterpret_cast<const ALIB_QTCHAR*>(src.data()); }
    static integer             Length   (QStringView const & src)          { return                              integer(src.size()); }
    static QStringView         Construct(const ALIB_QTCHAR* b, integer l ) { return QStringView( b, qsizetype(l) ); }
};

template<> struct  ZTArrayTraits<QStringView, ALIB_QTCHAR>
{
    static constexpr Policy    Access                                      = Policy::ExplicitOnly;
    static constexpr Policy    Construction                                = Policy::Implicit;
    static const ALIB_QTCHAR*  Buffer   (QStringView const & src)          { return reinterpret_cast<const ALIB_QTCHAR*>(src.data()); }
    static integer             Length   (QStringView const & src)          { return                              integer(src.size()); }
    static QStringView         Construct(const ALIB_QTCHAR* b, integer l ) { return QStringView( b, qsizetype(l) ); }
};

// QString
template<> struct  ArrayTraits<QString, ALIB_QTCHAR >
{
    static constexpr Policy    Access                                      = Policy::Implicit;
    static constexpr Policy    Construction                                = Policy::ExplicitOnly;
    static const ALIB_QTCHAR*  Buffer   (QString const & src)              { return reinterpret_cast<const ALIB_QTCHAR*>(src.data()); }
    static integer             Length   (QString const & src)              { return                              integer(src.size()); }
    static QString             Construct(const ALIB_QTCHAR* b, integer l ) { return QString( reinterpret_cast<const QChar*>(b), int(l) ); }
};

template<> struct  ZTArrayTraits<QString, ALIB_QTCHAR>
{
    static constexpr Policy   Access                                       = Policy::Implicit;
    static constexpr Policy   Construction                                 = Policy::ExplicitOnly;
    static const ALIB_QTCHAR* Buffer   (QString const & src)               { return reinterpret_cast<const ALIB_QTCHAR*>(src.utf16()); }
    static integer            Length   (QString const & src)               { return                              integer(src.size() ); }
    static QString            Construct(const ALIB_QTCHAR* b, integer l ) { return QString( reinterpret_cast<const QChar*>(b), int(l) ); }
};

// QLatin1String
template<> struct  ArrayTraits<QLatin1String, nchar>
{
    static constexpr Policy   Access                                = Policy::Implicit;
    static constexpr Policy   Construction                          = Policy::Implicit;
    static const nchar*       Buffer   (QLatin1String const & src)  { return         src.data();  }
    static integer            Length   (QLatin1String const & src)  { return integer(src.size()); }
    static QLatin1String      Construct(const nchar* b, integer l ) { return QLatin1String( b, int(l) ); }
};

template<> struct  ZTArrayTraits<QLatin1String, nchar>
{
    static constexpr Policy   Access                                 = Policy::ExplicitOnly;
    static constexpr Policy   Construction                           = Policy::Implicit;
    static const nchar*       Buffer   (QLatin1String const & src)   { return reinterpret_cast<const nchar*>(src.data()); }
    static integer            Length   (QLatin1String const & src)   { return                        integer(src.size()); }
    static QLatin1String      Construct(const nchar* b, integer l )  { return QLatin1String( b, int(l) ); }
};

// QByteArray
template<> struct  ArrayTraits<QByteArray, nchar>
{
    static constexpr Policy   Access                                 = Policy::Implicit;
    static constexpr Policy   Construction                           = Policy::ExplicitOnly;
    static const nchar*       Buffer   (QByteArray const & src)      { return         src.data();  }
    static integer            Length   (QByteArray const & src)      { return integer(src.size()); }
    static QByteArray         Construct(const nchar* b, integer l )  { return QByteArray( b, int(l) ); }
};

template<> struct  ZTArrayTraits<QByteArray, nchar>
{
    static constexpr Policy   Access                                 = Policy::ExplicitOnly;
    static constexpr Policy   Construction                           = Policy::ExplicitOnly;
    static const nchar*       Buffer   (QByteArray const & src)      { return reinterpret_cast<const nchar*>(src.data()); }
    static integer            Length   (QByteArray const & src)      { return                        integer(src.size()); }
    static QByteArray         Construct(const nchar* b, integer l )  { return QByteArray( b, int(l) ); }
};

// QVector<uint>
template<> struct  ArrayTraits<QVector<uint>, ALIB_QTCHAR_4>
{
    static constexpr Policy     Access                                        = Policy::Implicit;
    static constexpr Policy     Construction                                  = Policy::ExplicitOnly;
    static const ALIB_QTCHAR_4* Buffer   (QVector<uint> const & src)          { return reinterpret_cast<const ALIB_QTCHAR_4*>(src.data()); }
    static integer              Length   (QVector<uint> const & src)          { return                                integer(src.size()); }
    static QVector<uint>        Construct(const ALIB_QTCHAR_4* b, integer l )
    {
        QVector<uint> result;
        result.reserve( int(l) );
        const ALIB_QTCHAR_4* end= b + l;
        while( b < end )
            result.append( uint( *b++ ) );
        return  result;
    }
};

template<> struct  ZTArrayTraits<QVector<uint>, ALIB_QTCHAR_4>
{
    static constexpr Policy     Access                                                = Policy::ExplicitOnly;
    static constexpr Policy     Construction                                          = Policy::ExplicitOnly;
    static const ALIB_QTCHAR_4* Buffer   (QVector<uint> const & src)                  { return reinterpret_cast<const ALIB_QTCHAR_4*>(src.data()); }
    static integer              Length   (QVector<uint> const & src)                  { return                                integer(src.size() ); }
    static QVector<uint>        Construct(const ALIB_QTCHAR_4* b, integer l )
    {
        QVector<uint> result;
        result.reserve( int(l) );
        const ALIB_QTCHAR_4* end= b + l;
        while( b < end )
            result.append( uint(*b++) );
        return  result;
    }
};

#endif  //DOXYGEN

}  // namespace [alib::characters]


#endif // !defined(ALIB_QT_LIB_NOT_FOUND) // this is only set in an automated test project
#endif // HPP_ALIB_COMPATIBILITY_QT_CHARACTERS
