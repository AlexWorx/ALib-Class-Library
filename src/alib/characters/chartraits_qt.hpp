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
namespace alib::characters {

// Faking all template specializations of namespace strings for doxygen into namespace
// strings::custom to keep the documentation of namespace string clean!
#if DOXYGEN
namespace compatibility {
/// Specializations of type traits #"ArrayTraits" and
/// #"ZTArrayTraits" for types found in the
/// \https{QT Class Library,www.qt.io}.
namespace qt {} }

#else

#define ALIB_QTCHAR   alib::characters::TypeBySize<2>
#define ALIB_QTCHAR_4 alib::characters::TypeBySize<4>

// QStringView
template<> struct  ArrayTraits<QStringView, ALIB_QTCHAR> {
    static constexpr Policy    Access                                      = Policy::Implicit;
    static constexpr Policy    Construction                                = Policy::Implicit;
    static const ALIB_QTCHAR*  Buffer   (QStringView const & src)          { return reinterpret_cast<const ALIB_QTCHAR*>(src.data()); }
    static integer             Length   (QStringView const & src)          { return                              integer(src.size()); }
    static QStringView         Construct(const ALIB_QTCHAR* b, integer l ) { return QStringView( b, qsizetype(l) ); }
};

template<> struct  ZTArrayTraits<QStringView, ALIB_QTCHAR> {
    static constexpr Policy    Access                                      = Policy::ExplicitOnly;
    static constexpr Policy    Construction                                = Policy::Implicit;
    static const ALIB_QTCHAR*  Buffer   (QStringView const & src)          { return reinterpret_cast<const ALIB_QTCHAR*>(src.data()); }
    static integer             Length   (QStringView const & src)          { return                              integer(src.size()); }
    static QStringView         Construct(const ALIB_QTCHAR* b, integer l ) { return QStringView( b, qsizetype(l) ); }
};

// QString
template<> struct  ArrayTraits<QString, ALIB_QTCHAR > {
    static constexpr Policy    Access                                      = Policy::Implicit;
    static constexpr Policy    Construction                                = Policy::ExplicitOnly;
    static const ALIB_QTCHAR*  Buffer   (QString const & src)              { return reinterpret_cast<const ALIB_QTCHAR*>(src.data()); }
    static integer             Length   (QString const & src)              { return                              integer(src.size()); }
    static QString             Construct(const ALIB_QTCHAR* b, integer l ) { return QString( reinterpret_cast<const QChar*>(b), int(l) ); }
};

template<> struct  ZTArrayTraits<QString, ALIB_QTCHAR> {
    static constexpr Policy   Access                                       = Policy::Implicit;
    static constexpr Policy   Construction                                 = Policy::ExplicitOnly;
    static const ALIB_QTCHAR* Buffer   (QString const & src)               { return reinterpret_cast<const ALIB_QTCHAR*>(src.utf16()); }
    static integer            Length   (QString const & src)               { return                              integer(src.size() ); }
    static QString            Construct(const ALIB_QTCHAR* b, integer l ) { return QString( reinterpret_cast<const QChar*>(b), int(l) ); }
};

// QLatin1String
template<> struct  ArrayTraits<QLatin1String, nchar> {
    static constexpr Policy   Access                                = Policy::Implicit;
    static constexpr Policy   Construction                          = Policy::Implicit;
    static const nchar*       Buffer   (QLatin1String const & src)    { return         src.data(); }
    static integer            Length   (QLatin1String const & src)   { return integer(src.size()); }
    static QLatin1String      Construct(const nchar* b, integer l ) { return QLatin1String( b, int(l) ); }
};

template<> struct  ZTArrayTraits<QLatin1String, nchar> {
    static constexpr Policy   Access                                 = Policy::ExplicitOnly;
    static constexpr Policy   Construction                           = Policy::Implicit;
    static const nchar*       Buffer   (QLatin1String const & src)   { return reinterpret_cast<const nchar*>(src.data()); }
    static integer            Length   (QLatin1String const & src)   { return                        integer(src.size()); }
    static QLatin1String      Construct(const nchar* b, integer l )  { return QLatin1String( b, int(l) ); }
};

// QByteArray
template<> struct  ArrayTraits<QByteArray, nchar> {
    static constexpr Policy   Access                                 = Policy::Implicit;
    static constexpr Policy   Construction                           = Policy::ExplicitOnly;
    static const nchar*       Buffer   (QByteArray const & src)       { return         src.data(); }
    static integer            Length   (QByteArray const & src)      { return integer(src.size()); }
    static QByteArray         Construct(const nchar* b, integer l )  { return QByteArray( b, int(l) ); }
};

template<> struct  ZTArrayTraits<QByteArray, nchar> {
    static constexpr Policy   Access                                 = Policy::ExplicitOnly;
    static constexpr Policy   Construction                           = Policy::ExplicitOnly;
    static const nchar*       Buffer   (QByteArray const & src)      { return reinterpret_cast<const nchar*>(src.data()); }
    static integer            Length   (QByteArray const & src)      { return                        integer(src.size()); }
    static QByteArray         Construct(const nchar* b, integer l )  { return QByteArray( b, int(l) ); }
};

// QVector<uint>
template<> struct  ArrayTraits<QVector<uint>, ALIB_QTCHAR_4> {
    static constexpr Policy     Access                                        = Policy::Implicit;
    static constexpr Policy     Construction                                  = Policy::ExplicitOnly;
    static const ALIB_QTCHAR_4* Buffer   (QVector<uint> const & src)          { return reinterpret_cast<const ALIB_QTCHAR_4*>(src.data()); }
    static integer              Length   (QVector<uint> const & src)          { return                                integer(src.size()); }
    static QVector<uint>        Construct(const ALIB_QTCHAR_4* b, integer l ) {
        QVector<uint> result;
        result.reserve( int(l) );
        const ALIB_QTCHAR_4* end= b + l;
        while( b < end )
            result.append( uint( *b++ ) );
        return  result;
    }
};

template<> struct  ZTArrayTraits<QVector<uint>, ALIB_QTCHAR_4> {
    static constexpr Policy     Access                                                = Policy::ExplicitOnly;
    static constexpr Policy     Construction                                          = Policy::ExplicitOnly;
    static const ALIB_QTCHAR_4* Buffer   (QVector<uint> const & src)                  { return reinterpret_cast<const ALIB_QTCHAR_4*>(src.data()); }
    static integer              Length   (QVector<uint> const & src)                  { return                                integer(src.size() ); }
    static QVector<uint>        Construct(const ALIB_QTCHAR_4* b, integer l ) {
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
