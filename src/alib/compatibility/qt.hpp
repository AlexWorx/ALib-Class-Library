// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//
//  Legal Notice:
//    This is an optional extension header to provide compatibility between ALib and
//    the QT class library.
//    All information about QT is found under https://www.qt.io
//    ALib otherwise does not use or rely on QT.
//    The use of QT is bound to the QT license restrictions.
// #################################################################################################
/** @file */ // Hello Doxygen

// check for alib.hpp already there but not us
#if !defined (HPP_ALIB)
    #error "include \"alib/alib.hpp\" before including this header"
#endif
#if defined(HPP_COM_ALIB_TEST_INCLUDES) && defined(HPP_ALIB_COMPATIBILITY_QT)
    #error "Header already included"
#endif

#ifndef HPP_ALIB_COMPATIBILITY_QT
//! @cond NO_DOX
#define HPP_ALIB_COMPATIBILITY_QT 1
//! @endcond

#if ALIB_MODULE_STRINGS

#include <QtCore/QString>
#include <QtCore/QVector>

// #################################################################################################
// ALib String / QT string classes compatibility support
// #################################################################################################

namespace aworx { namespace lib { namespace strings
{
// We are faking all template specializations of namespace strings for doxygen into namespace
// strings::thirdparty to keep the documentation of namespace string clean!
#if defined(DOX_PARSER)
namespace thirdparty {
/**
 * With the inclusion of header file <c>alib/compatibility/qt.hpp</c>
 * some compatibility between <b>%ALib Strings</b> and types of
 * [QT Class Library](https://www.qt.io) is given.
 *
 *
 * <b>1. Specializations of  %T_String:</b><br>
 * Specializations of \ref aworx::lib::strings::T_String is given for types
 * - \b QByteArray and
 * - \b QLatin1String
 * This allows to implicitly constructs objects of class \b %String from the named types.
 *
 * <b>2. Applicable Types:</b><br>
 * Support for applying (appending contents of objects of the types to objects of class \b %AString)
 * is given. Applicable types are:
 * - QChar
 * - QLatin1String
 * - QByteArray
 * - QVector<uint>
 * - QString
 *
 * <b>3. Conversion to QString:</b><br>
 * Several overloaded methods #ToQString are defined to convert <b>%ALib Strings</b> to
 * instances of class \b QString.
 *
 * <b>General Information On ALib And QT %String Compatibility:</b><br>
 * \alib string classes are designed to support 3rd-party string types in a non-intrusive
 * fashion. Template meta programming in two areas makes this possible:
 * - Implicit conversion of external strings to class
 *   \alib{strings,StringBase,String}, which allows to pass 3rd party strings to
 *   method accepting parameters of type <b>const %aworx::String&</b> without conversion.
 * - \e Application of arbitrary types to objects of class
 *   \alib{strings,AStringBase,AString}. Here, the term \e "application" most
 *   means concatenating a string representation of an object to the \b AString object.
 * (This is all documented in reference manual of namespace \ref aworx::lib::strings.)
 *
 * %String classes of the [QT library](https://www.qt.io) are
 * supported trough header file <em>"alib/compatibility/qt.hpp"</em>.
 *
 * <b>Implicit Conversion:</b><br>
 * Unfortunately, objects of class \b %QString can not be implicitly converted (embedded) in
 * thin and lightweight class \alib{strings,StringBase,String}. The reason
 * for this is, that \alib strings are single byte strings, while \b QString represent
 * (two byte) unicode strings. Therefore, template struct
 * \ref aworx::lib::strings::T_String "T_String" can not be implemented for class \b QString
 * and objects of this class can not be simply passed to methods accepting parameters of type
 * <b>const %aworx::String&</b>.
 *
 * The way out is the use of classes \b QByteArray and \b QLatin1String. For these QT classes,
 * an implicit conversion exists and this way objects of these types are implicitly converted
 * to \b %aworx::String types. Class \b %QString provides methods to convert to these classes.
 * Passing a \b QString object to a method expecting a <b>const %aworx::String&</b> can be
 * done as follows
 *
 *      MyMethodExpectingALibString( myQString.toLocal8Bit() );
 *      MyMethodExpectingALibString( myQString.toUtf8()      );
 *      MyMethodExpectingALibString( myQString.toLatin1()    );
 *
 * \note
 *   The encoding of strings represented by \alib strings is user defined. Therefore, it
 *   is up to the user to decide which of the above conversion methods of \b QString
 *   is to be used. In most cases, method \b toLocal8Bit should be the right choice. Also,
 *   \alib strings uses the current locale to convert characters of type wchar_t (or arrays of them)
 *   when \e applying to class \b %AString as well.
 *
 * The obvious drawback of this approach is that a conversion has to take place and
 * that this conversion implies a temporary allocation of heap memory for the data stored
 * in the (stack-allocated) \b QByteArray object.
 *
 * \attention
 *   This is a non-existing namespace, exclusively defined for the
 *   [documentation parser](http://www.stack.nl/~dimitri/doxygen).
 *   All types and functions described in this namespace are residing in namespace
 *   #aworx::lib::strings (just as they have to in respect to C++ rules).
 *   This documentation is moved here to keep the original namespace documentation clean.
 */
namespace qt {

#endif

    // #############################################################################################
    // T_String (QLatin1String, QByteArray)
    // #############################################################################################

    /** ********************************************************************************************
     * Specialization of struct
     * \ref aworx::lib::strings::T_String "T_String" for type \b QLatin1String.
     **********************************************************************************************/
     template<>   struct  T_String<QLatin1String,char>  : public std::true_type
     {
        /** ****************************************************************************************
         * Returns the  result of \b src.data.
         *
         * @param  src The source string to take construct an aworx::String from
         * @return The string buffer.
         ******************************************************************************************/
        static inline
        const char* Buffer( const QLatin1String& src )
        {
            return  src.data();
        }

        /** ****************************************************************************************
         * Returns the  result of \b src.size.
         *
         * @param  src The source string to take construct an aworx::String from
         * @return The string length.
         ******************************************************************************************/
        static inline
        integer Length( const QLatin1String& src )
        {
            return src.size();
        }
    };

    /** ********************************************************************************************
     * Specialization of struct
     * \ref aworx::lib::strings::T_String "T_String" for type \b QByteArray.
     **********************************************************************************************/
    template<>   struct  T_String<QByteArray,char>  : public std::true_type
    {
        /** ****************************************************************************************
         * Returns the  result of \b src.data.
         *
         * @param  src The source string to take construct an aworx::String from
         * @return The string buffer.
         ******************************************************************************************/
        static inline
        const char* Buffer( const QByteArray& src )
        {
            return src.data();
        }

        /** ****************************************************************************************
         * Returns the  result of \b src.size.
         *
         * @param  src The source string to take construct an aworx::String from
         * @return The string length.
         ******************************************************************************************/
        static inline
        integer Length( const QByteArray& src )
        {
            return src.length();
        }
    };

    // #############################################################################################
    // T_Apply (QChar, QVector<uint>, QString)
    // #############################################################################################

    /// Specialization of template struct \ref T_Apply for type \c QChar.
    template<typename TChar> struct T_Apply<QChar, TChar>   : public std::true_type
    {
        /** ****************************************************************************************
         * Appends \p{src} of type \b QChar to \p{target}.
         *
         * @param  target The AString to append \p{src} to.
         * @param  src    The source string.
         * @return The length of the given string \p{src} which was appended to \p{target}.
         ******************************************************************************************/
        static inline integer Apply( AStringBase<TChar>& target, const QChar& src )
        {
            target._( static_cast<wchar_t>( src.unicode() ) );
            return 1;
        }
    };

    /// Specialization of template struct \ref T_Apply for type \c QByteArray.
    template<typename TChar> struct T_Apply<QByteArray, TChar>   : public std::true_type
    {
        /** ****************************************************************************************
         * Appends \p{src} of type \b QByteArray to \p{target}.
         *
         * @param  target The AString to append \p{src} to.
         * @param  src    The source string.
         * @return The length of the given string \p{src} which was appended to \p{target}.
         ******************************************************************************************/
        static inline integer Apply( AStringBase<TChar>& target, const QByteArray& src )
        {
            int len= src.length();
            target.Append( src.data(), len );
            return len;
        }
    };

    /// Specialization of template struct \ref T_Apply for type \c QVector<uint.
    template<typename TChar> struct T_Apply<QVector<uint>, TChar>   : public std::true_type
    {
        /** ****************************************************************************************
         * Appends \p{src} of type \b QVector<uint> to \p{target}.
         * It is expected that the data in the QVector contains UCS4 data, e.g. generated with
         * method \b %QString::toUcs4.<br>
         *
         * @param  target The AString to append \p{src} to.
         * @param  src    The source string.
         * @return The length of the given string \p{src} which was appended to \p{target}.
         ******************************************************************************************/
        static inline integer Apply( AStringBase<TChar>& target, const QVector<uint>& src )
        {
            target.Append( reinterpret_cast<const wchar_t*>(src.data()), src.length() );
            return 0; // avoid null check (already done in Append())
        }
    };

    /// Specialization of template struct \ref T_Apply for type \c QString.
    template<typename TChar> struct T_Apply<QString, TChar>   : public std::true_type
    {
        /** ****************************************************************************************
         * Appends \p{src} of type \b QString to \p{target}.
         *
         * @param  target The AString to append \p{src} to.
         * @param  src    The source string.
         * @return The length of the given string \p{src} which was appended to \p{target}.
         ******************************************************************************************/
        static inline integer Apply( AStringBase<TChar>& target, const QString& src )
        {
            int len= src.length();
            wchar_t localBuffer[1024];
            wchar_t* wcBuffer= localBuffer;
            if ( len > 1024 )
                wcBuffer= new wchar_t[len];

            src.toWCharArray( wcBuffer );
            target.Append( wcBuffer, len );

            if ( wcBuffer != localBuffer )
                delete[] wcBuffer;

            return 0; // avoid null check (already done in Append())
        }
    };

    /// Specialization of template struct \ref T_Apply for type \c QByteArray.
    template<> struct T_Apply<QLatin1String, wchar_t>   : public std::true_type
    {
        /** ****************************************************************************************
         * Appends \p{src} of type \b QByteArray to \p{target}.
         *
         * @param  target The AString to append \p{src} to.
         * @param  src    The source string.
         * @return The length of the given string \p{src} which was appended to \p{target}.
         ******************************************************************************************/
        static inline integer Apply( WAString& target, const QLatin1String& src )
        {
            int len= src.size();
            target.Append( src.data(), len );
            return len;
        }
    };



    // #############################################################################################
    // ToQString
    // #############################################################################################
    /** ********************************************************************************************
     * Copies or appends a region of a given \alib{strings,StringBase,String} into a QString.
     * The region is adjusted to the length of the %String.<br>
     * See \ref aworx::lib::strings::thirdparty::qt for general information about using QT
     * strings with \alib.
     *
     * @param as            The \alib{strings,StringBase,String} to convert into a QString.
     * @param target        A result QString to copy the specified region into.
     * @param regionStart   The start index of the region to be copied.
     * @param regionLength  The maximum length of the region to be copied.
     *                      Defaults to \b MaxLen.
     * @param targetData    If \c CurrentData::Keep, the target \c QString is not cleared before
     *                      the result is written. Defaults to \c CurrentData::Clear.
     *
     * @return The (modified) QString& result that was provided (for concatenation of calls).
     **********************************************************************************************/
    inline
    QString& ToQString( const NString&    as,
                        QString&          target,
                        integer           regionStart,
                        integer           regionLength    = strings::MaxLen,
                        lang::CurrentData targetData      = lang::CurrentData::Clear )
    {
        if ( targetData == lang::CurrentData::Clear )
            target.clear();

        // adjust range, if empty return empty string
        if ( as.AdjustRegion( regionStart, regionLength ) )
            return target;

        // copy our buffer into target
        target.fromUtf8( as.Buffer() + regionStart, static_cast<int>( regionLength - regionStart ) );
        return  target;
    }

    /** ********************************************************************************************
     * Copies a region of a given \alib{strings,StringBase,String} into a QString which
     * is created.
     * The region is adjusted to the length of the %String.<br>
     * See \ref aworx::lib::strings::thirdparty::qt for general information about using QT
     * strings with \alib.
     *
     * @param as            The \alib{strings,StringBase,String} to convert into a QString.
     * @param regionStart   The start index of the region in this to create the QString from.
     * @param regionLength  The maximum length of the region to be copied.
     *                      Defaults to \b MaxLen.
     *
     * @return A string that represents the specified sub region of this object.
     **********************************************************************************************/
    inline
    QString  ToQString( const NString& as, integer regionStart, integer regionLength= strings::MaxLen )
    {
        QString result;
        ToQString( as, result, regionStart, regionLength, lang::CurrentData::Keep );
        return result;
    }

    /** ********************************************************************************************
     * Copies the contents of a given \alib{strings,StringBase,String} into the given QString.<br>
     * See \ref aworx::lib::strings::thirdparty::qt for general information about using QT
     * strings with \alib.
     *
     * @param as            The \alib{strings,StringBase,String} to convert into a QString.
     * @return A QString that represents this object.
     **********************************************************************************************/
    inline
    QString  ToQString( const NString& as )
    {
        return ToQString( as, 0, as.Length() );
    }


    /** ********************************************************************************************
     * Copies the contents of a given \alib{strings,StringBase,String} into the given QString.<br>
     * See \ref aworx::lib::strings::thirdparty::qt for general information about using QT
     * strings with \alib.
     *
     * @param as           The \alib{strings,StringBase,String} to convert into a QString.
     * @param target       A target QString to copy the contents of this %String into.
     * @param targetData   If \c CurrentData::Keep, parameter \p{target} is not cleared before
     *                     the result is written. Defaults to \c CurrentData::Clear.
     * @return The (modified) result that was provided (for concatenation of calls).
     **********************************************************************************************/
    inline
    QString& ToQString( const NString&      as,
                        QString&            target,
                        lang::CurrentData   targetData = lang::CurrentData::Clear )
    {
        return ToQString( as, target, 0, as.Length(), targetData );
    }
// We are faking all template specializations of namespace strings for doxygen into namespace
// strings::thirdparty to keep the documentation of namespace string clean!
#if defined(DOX_PARSER)
}}
#endif

}}}

#endif //ALIB_MODULE_STRINGS

// #################################################################################################
// %ALib %Boxing / QTString types compatibility support
// #################################################################################################

#if ALIB_MODULE_BOXING

#if !ALIB_FEAT_BOXING_FTYPES
    static_assert(0, "This implementation of boxing QT strings may be used only if default "
                     "boxing of fundamental types is activated."
#endif

#if !defined(DOX_PARSER)
ALIB_WARNINGS_ALLOW_TEMPLATE_META_PROGRAMMING
#endif
namespace aworx { namespace lib { namespace boxing
{

// We are faking all template specializations of namespace boxing for doxygen into namespace
// boxing::thirdparty to keep the documentation of namespace string clean!
#if defined(DOX_PARSER)
namespace thirdparty {
/**
 * With the inclusion of header file <c>alib/compatibility/qt.hpp</c>, support for
 * boxing string types of the [QT Class Library](https://www.qt.io) is provided.
 * QT provides several string types, the most prominent of-course is \c QString. Unfortunately,
 * this type uses a 2-byte character type. This is neither compatible with C++ fundamental
 * type \c char nor with \c wchar_t, as the latter has platform dependent size, e.g. is 4 bytes
 * wide on GNU/Linux using GNU C++ compiler. \b %QString is boxed to type \c char16_t[].
 *
 * Besides this, supported types are \b QByteArray and \b QLatin1String.
 * Those types are boxed to fundamental type \c char[] (the same as for example
 * \b %aworx::String and \b std::string are), and therefor can be used completely transparently
 * with any function or method that accepts boxes and processes single byte string values.
 *
 * All types are unboxable as values.
 *
 * For more information see chapter \ref alib_namespace_boxing_strings "Boxing Strings" of the
 * namespace documentation of module \ref aworx::lib::boxing "ALib Boxing".
 *
 * \note
 *   Namespace function  \ref InitQTString "aworx::lib::boxing::InitQTString" is to be invoked on
 *   bootstrap to complete the support of QT-string-type boxing.
 *
 *
 * \attention
 *   This is a non-existing namespace, exclusively defined for the
 *   [documentation parser](http://www.stack.nl/~dimitri/doxygen).
 *   This documentation is moved here to keep the original namespace documentation clean.
 */
namespace qt {

#else
    ALIB_BOXING_SPECIALIZE_CB_CUB( QByteArray,    char     , true , true);
    ALIB_BOXING_SPECIALIZE_CB_CUB( QLatin1String, char     , true , true);
    ALIB_BOXING_SPECIALIZE_CB_CUB( QString,       char16_t , true , true);



    inline   void           T_Boxing<QByteArray>::Boxing( Box& box,  const QByteArray&  value )
    {
        box.data.Value=    (boxvalue) value.data();
        box.data.Length=   static_cast<integer>(value.length());
    }

    inline   void           T_Boxing<QLatin1String>::Boxing( Box& box,  const QLatin1String&  value )
    {
        box.data.Value=    (boxvalue) value.data();
        box.data.Length=   static_cast<integer>(value.size());
    }

    inline   void           T_Boxing<QString>::Boxing( Box& box,  const QString&  value )
    {
        box.data.Value=    (boxvalue) value.data();
        box.data.Length=   static_cast<integer>(value.length());
    }

    inline  QByteArray      T_Boxing<QByteArray>::Unboxing(const Box &box)
    {
        return  QByteArray( (const char*) box.data.Value,
                              box.data.Length >= 0 ? static_cast<int>(box.data.Length)
                                                   : static_cast<int>(strlen( (const char*) box.data.Value) )
                           );

    }

    inline  QLatin1String   T_Boxing<QLatin1String>::Unboxing(const Box &box)
    {
        return  QLatin1String( (const char*) box.data.Value, static_cast<int>(box.data.Length) );
    }

    inline  QString         T_Boxing<QString>::Unboxing(const Box &box)
    {
        return  QString( (const QChar*)   box.data.Value,
                         static_cast<int>(box.data.Length) ); // QString accepts -1 for unknown size

    }


#endif

// We are faking all template specializations of namespace boxing for doxygen into namespace
// boxing::thirdparty to keep the documentation of namespace string clean!
#if defined(DOX_PARSER)
}}
#endif

void InitQTString();
/**
 * Initializes <b>%ALib %Boxing</b> system in respect to boxing QT string types.
 *
 * This method is \b not automatically invoked with
 * \ref aworx::lib::ALib::Init "ALib::Init", because the support for boxing
 * QT string types is optional and due to the inclusion of header
 * <b>"alib/compatiblity/qt.hpp"</b>.
 *
 * In general, boxing ofQT string types works well without the one-time invocation of
 * this function at the bootstrap of a process. What this method currently adds is
 * interface \ref aworx::lib::strings::boxing::IApply "IApply" for boxed types
 * - \ref aworx::lib::boxing::BoxedAs "BoxedAs<QByteArray>"
 * - \ref aworx::lib::boxing::BoxedAs "BoxedAs<QLatin1String>"
 * - \ref aworx::lib::boxing::BoxedAs "BoxedAs<QString>"
 * This is only
 * needed in some particular use cases.
 *
 */
inline void InitQTString()
{
    #if ALIB_MODULE_STRINGS
        aworx::lib::boxing::DefineInterface<BoxedAs<QByteArray>    , false , strings::boxing::IApply_BoxedAs_TApplicable<char   , QByteArray>   >();
        aworx::lib::boxing::DefineInterface<BoxedAs<QByteArray>    , false , strings::boxing::IApply_BoxedAs_TApplicable<wchar_t, QByteArray>   >();
        aworx::lib::boxing::DefineInterface<BoxedAs<QLatin1String> , false , strings::boxing::IApply_BoxedAs_TApplicable<char   , QLatin1String>>();
        aworx::lib::boxing::DefineInterface<BoxedAs<QLatin1String> , false , strings::boxing::IApply_BoxedAs_TApplicable<wchar_t, QLatin1String>>();
        aworx::lib::boxing::DefineInterface<BoxedAs<QString>       , false , strings::boxing::IApply_BoxedAs_TApplicable<char   , QString>      >();
        aworx::lib::boxing::DefineInterface<BoxedAs<QString>       , false , strings::boxing::IApply_BoxedAs_TApplicable<wchar_t, QString>      >();
    #endif
}


}}}
#if !defined(DOX_PARSER)
ALIB_WARNINGS_RESTORE
#endif

#endif // ALIB_MODULE_BOXING


#endif // HPP_ALIB_COMPATIBILITY_QT
