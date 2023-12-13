/** ************************************************************************************************
 * \file
 * This header file is part of the \aliblong.
 *
 * \emoji :copyright: 2013-2023 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 *
 * <b>Legal Notice:</b>
 * This is an optional extension header to provide compatibility between ALib and
 * the QT class library.
 * All information about QT is found at https://www.qt.io
 * ALib otherwise does not use or rely on QT.
 * The use of QT is bound to the QT license restrictions.
 **************************************************************************************************/
#ifndef HPP_ALIB_COMPATIBILITY_QT_CHARACTERS
#define HPP_ALIB_COMPATIBILITY_QT_CHARACTERS 1

#if !defined(HPP_ALIB_MODULES) && !defined(ALIB_DOX)
#   include "alib/lib/modules.hpp"
#endif

ALIB_ASSERT_MODULE(CHARACTERS)

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

#if defined(HPP_ALIB_BOXING_BOXING)
#   error "Wrong inclusion order. Header 'alib/compatibility/qt_characters.hpp' has to be included before headers of module ALib Boxing."
#endif

#if !defined(HPP_ALIB_CHARACTERS_CHARACTERS)
#   include "alib/characters/characters.hpp"
#endif

namespace aworx { namespace lib { namespace characters {

// Faking all template specializations of namespace strings for doxygen into namespace
// strings::custom to keep the documentation of namespace string clean!
#if defined(ALIB_DOX)
namespace compatibility {
/**
 * Specializations of type traits structs \alib{characters,T_CharArray} and
 * \alib{characters,T_ZTCharArray} for types found in the
 * \https{QT Class Library,www.qt.io}.
 */
namespace qt {} }

#else

#define ALIB_QTCHAR   typename aworx::lib::characters::TT_CharOfSize<2>::type
#define ALIB_QTCHAR_4 typename aworx::lib::characters::TT_CharOfSize<4>::type

ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
// QStringView
template<> struct  T_CharArray<QStringView, ALIB_QTCHAR>
{
    static constexpr AccessType          Access       =       AccessType::Implicit;
    static constexpr ConstructionType    Construction = ConstructionType::Implicit;
    static const ALIB_QTCHAR*            Buffer   (QStringView const & src)                   { return reinterpret_cast<const ALIB_QTCHAR*>(src.data()); }
    static integer                       Length   (QStringView const & src)                   { return      static_cast<integer      >(src.size()); }
    static QStringView                   Construct(const ALIB_QTCHAR* array, integer length ) { return QStringView( array, static_cast<qsizetype>(length) ); }
};

template<> struct  T_ZTCharArray<QStringView, ALIB_QTCHAR>
{
    static constexpr AccessType          Access       =       AccessType::ExplicitOnly;
    static constexpr ConstructionType    Construction = ConstructionType::Implicit;
    static const ALIB_QTCHAR*            Buffer   (QStringView const & src)                   { return reinterpret_cast<const ALIB_QTCHAR*>(src.data()); }
    static integer                       Length   (QStringView const & src)                   { return      static_cast<integer           >(src.size()); }
    static QStringView                   Construct(const ALIB_QTCHAR* array, integer length ) { return QStringView( array, static_cast<qsizetype>(length) ); }
};

// QString
template<> struct  T_CharArray<QString, ALIB_QTCHAR >
{
    static constexpr AccessType          Access       =       AccessType::Implicit;
    static constexpr ConstructionType    Construction = ConstructionType::ExplicitOnly;
    static const ALIB_QTCHAR*            Buffer   (QString const & src)                       { return reinterpret_cast<const ALIB_QTCHAR*>(src.data()); }
    static integer                       Length   (QString const & src)                       { return      static_cast<integer           >(src.size()); }
    static QString                       Construct(const ALIB_QTCHAR* array, integer length ) { return QString( reinterpret_cast<const QChar*>(array), static_cast<int>(length) ); }
};

template<> struct  T_ZTCharArray<QString, ALIB_QTCHAR>
{
    static constexpr AccessType          Access       =       AccessType::Implicit;
    static constexpr ConstructionType    Construction = ConstructionType::ExplicitOnly;
    static const ALIB_QTCHAR*            Buffer   (QString const & src)                       { return reinterpret_cast<const ALIB_QTCHAR*>(src.utf16()); }
    static integer                       Length   (QString const & src)                       { return      static_cast<integer           >(src.size() ); }
    static QString                       Construct(const ALIB_QTCHAR* array, integer length ) { return QString( reinterpret_cast<const QChar*>(array), static_cast<int>(length) ); }
};

// QLatin1String
template<> struct  T_CharArray<QLatin1String, nchar>
{
    static constexpr AccessType          Access       =       AccessType::Implicit;
    static constexpr ConstructionType    Construction = ConstructionType::Implicit;
    static const nchar*                  Buffer   (QLatin1String const & src)            { return                                 src.data();  }
    static integer                       Length   (QLatin1String const & src)            { return      static_cast<integer      >(src.size()); }
    static QLatin1String                 Construct(const nchar* array, integer length )  { return QLatin1String( array, static_cast<int>(length) ); }
};

template<> struct  T_ZTCharArray<QLatin1String, nchar>
{
    static constexpr AccessType          Access       =       AccessType::ExplicitOnly;
    static constexpr ConstructionType    Construction = ConstructionType::Implicit;
    static const nchar*                  Buffer   (QLatin1String const & src)            { return reinterpret_cast<const nchar*>(src.data()); }
    static integer                       Length   (QLatin1String const & src)            { return      static_cast<integer     >(src.size()); }
    static QLatin1String                 Construct(const nchar* array, integer length )  { return QLatin1String( array, static_cast<int>(length) ); }
};

// QByteArray
template<> struct  T_CharArray<QByteArray, nchar>
{
    static constexpr AccessType          Access       =       AccessType::Implicit;
    static constexpr ConstructionType    Construction = ConstructionType::ExplicitOnly;
    static const nchar*                  Buffer   (QByteArray const & src)               { return                                 src.data();  }
    static integer                       Length   (QByteArray const & src)               { return      static_cast<integer      >(src.size()); }
    static QByteArray                    Construct(const nchar* array, integer length )  { return QByteArray( array, static_cast<int>(length) ); }
};

template<> struct  T_ZTCharArray<QByteArray, nchar>
{
    static constexpr AccessType          Access       =       AccessType::ExplicitOnly;
    static constexpr ConstructionType    Construction = ConstructionType::ExplicitOnly;
    static const nchar*                  Buffer   (QByteArray const & src)               { return reinterpret_cast<const nchar*>(src.data()); }
    static integer                       Length   (QByteArray const & src)               { return      static_cast<integer      >(src.size()); }
    static QByteArray                    Construct(const nchar* array, integer length )  { return QByteArray( array, static_cast<int>(length) ); }
};

// QVector<uint>
template<> struct  T_CharArray<QVector<uint>, ALIB_QTCHAR_4>
{
    static constexpr AccessType          Access       =       AccessType::Implicit;
    static constexpr ConstructionType    Construction = ConstructionType::ExplicitOnly;
    static const ALIB_QTCHAR_4*          Buffer   (QVector<uint> const & src)                  { return reinterpret_cast<const ALIB_QTCHAR_4*>(src.data()); }
    static integer                       Length   (QVector<uint> const & src)                  { return      static_cast<integer      >(src.size()); }
    static QVector<uint>                 Construct(const ALIB_QTCHAR_4* array, integer length )
    {
        QVector<uint> result;
        result.reserve( static_cast<int>(length) );
        const ALIB_QTCHAR_4* end= array + length;
        while( array < end )
            result.append( static_cast<uint>( *array++ ) );
        return  result;
    }
};

template<> struct  T_ZTCharArray<QVector<uint>, ALIB_QTCHAR_4>
{
    static constexpr AccessType          Access       =       AccessType::ExplicitOnly;
    static constexpr ConstructionType    Construction = ConstructionType::ExplicitOnly;
    static const ALIB_QTCHAR_4*          Buffer   (QVector<uint> const & src)                  { return reinterpret_cast<const ALIB_QTCHAR_4*>(src.data()); }
    static integer                       Length   (QVector<uint> const & src)                  { return      static_cast<integer             >(src.size() ); }
    static QVector<uint>                 Construct(const ALIB_QTCHAR_4* array, integer length )
    {
        QVector<uint> result;
        result.reserve( static_cast<int>(length) );
        const ALIB_QTCHAR_4* end= array + length;
        while( array < end )
            result.append( static_cast<uint>( *array++ ) );
        return  result;
    }
};

ALIB_WARNINGS_RESTORE

#endif  //ALIB_DOX

}}}  // namespace [aworx::lib::characters]


#endif // !defined(ALIB_QT_LIB_NOT_FOUND) // this is only set in an automated test project
#endif // HPP_ALIB_COMPATIBILITY_QT_CHARACTERS
