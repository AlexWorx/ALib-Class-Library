// #################################################################################################
//  AWorx ALib Unit Tests
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"
#include "unittests/alib_test_selection.hpp"
#if ALIB_UT_COMPATIBILITY

#if defined(QT_CORE_LIB) && ALIB_CHARACTERS
#include "alib/compatibility/qt_characters.hpp"
#include "alib/compatibility/qt_strings.hpp"
#include "alib/compatibility/qt_boxing.hpp"
#include "alib/alox.hpp"
#include "alib/strings/util/tokenizer.hpp"
#include "alib/strings/numberformat.hpp"

#include <iostream>
#include <sstream>
#include <iomanip>


#include "unittests/aworx_unittests.hpp"

#include <qt/QtCore/QString>
#include <qt/QtCore/QVector>
#include <qt/QtCore/QRect>

#define TESTCLASSNAME       UT_Compatibility


using namespace std;
using namespace alib;

namespace ut_aworx {


UT_CLASS

//--------------------------------------------------------------------------------------------------
//--- QT_String
//--------------------------------------------------------------------------------------------------
UT_METHOD( QT_String )
{
    UT_INIT()


    QString qts( "QString" );
    AString test( qts );        UT_EQ( A_CHAR("QString"), test )
    test._()._ ( qts);          UT_EQ( A_CHAR("QString"), test )

    QByteArray qba= QString ( "QByteArray" ).toLocal8Bit();
    test._()._ (NString(qba));  UT_EQ( A_CHAR("QByteArray"), test )
    test._()._ ( qba);          UT_EQ( A_CHAR("QByteArray"), test )

    QVector<uint> qv4=  QString( "QVector<uint>" ).toUcs4();
    AString test4( qv4 );       UT_EQ( A_CHAR("QVector<uint>"), test4 )
    test4._()._ ( qv4);         UT_EQ( A_CHAR("QVector<uint>"), test4 )


    QLatin1String ql1s( "Latin1String" );
    test._()._ (NString(ql1s)); UT_EQ( A_CHAR("Latin1String"), test )
    test._()._ ( ql1s);         UT_EQ( A_CHAR("Latin1String"), test )

    QChar qc= 'c';
    test._()._ (qc);            UT_EQ( A_CHAR("c"), test )


    UT_PRINT( String128() << "QString toUtf8() " )
    UT_PRINT( String128() << "  QString: German:  " << QString("ÄÜÖäüöß").toLocal8Bit() )
    UT_PRINT( String128() << "  QString: Greek:   " << QString().fromStdWString(L"\u03B1\u03B2\u03B3\u03B4\u03B5").toLocal8Bit() )
    UT_PRINT( String128() << "  QString: Smileys: " << QString().fromStdWString(L"\U0001F609 * \U0001F607 * \U0001F603 * \U0001F60E * "
                                                                         "\U00000361\U000000b0\U0000035c\U00000296\U00000361\U000000b0").toLocal8Bit() )
    UT_PRINT( String128() << "QString directly " )
    UT_PRINT( String128() << "  QString: German:  " << QString().fromLocal8Bit("ÄÜÖäüöß") )
    UT_PRINT( String128() << "  QString: Greek:   " << QString().fromStdWString(L"\u03B1\u03B2\u03B3\u03B4\u03B5") )

}

UT_METHOD( QT_StringBoxing )
{
    UT_INIT()
    IF_ALIB_THREADS( alib::monomem::GLOBAL_ALLOCATOR_LOCK.Acquire(ALIB_CALLER_PRUNED) );
    alib::boxing::compatibility::qt::BootstrapQTStringBoxing();
    IF_ALIB_THREADS( alib::monomem::GLOBAL_ALLOCATOR_LOCK.Release() );


    UT_PRINT("") UT_PRINT( "### Boxing: Testing Equals: char*/char[]  with QByteArray ###" )
    {
        bool result;
        Box helloP( static_cast<const char*>("Hello"));
        Box helloA(                          "Hello");
        Box falseP( static_cast<const char*>("not hello"));
        Box falseA(                          "not hello");
        QByteArray qbaHello= QString ( "Hello"     ).toLocal8Bit();
        QByteArray qbaFalse= QString ( "not Hello" ).toLocal8Bit();
        Box helloQBA( qbaHello );
        Box falseQBA( qbaFalse );

        result= helloQBA.Call<FEquals>( helloP ); UT_EQ( true , result )
        result= helloQBA.Call<FEquals>( helloA ); UT_EQ( true , result )
        result= helloQBA.Call<FEquals>( falseP ); UT_EQ( false, result )
        result= helloQBA.Call<FEquals>( falseA ); UT_EQ( false, result )

        result= helloA.Call<FEquals>( helloQBA ); UT_EQ( true , result )
        result= helloP.Call<FEquals>( helloQBA ); UT_EQ( true , result )
        result= helloA.Call<FEquals>( falseQBA ); UT_EQ( false, result )
        result= helloP.Call<FEquals>( falseQBA ); UT_EQ( false, result )
    }

    UT_PRINT("") UT_PRINT( "### Boxing: Testing Equals: char*/char[]  with QLatin1String ###" )
    {
        bool result;
        Box helloP( static_cast<const char*>("Hello"));
        Box helloA(                          "Hello");
        Box falseP( static_cast<const char*>("not hello"));
        Box falseA(                          "not hello");
        QLatin1String ql1sHello( "Hello"     );
        QLatin1String ql1sFalse( "not Hello" );
        Box helloQL1S( ql1sHello );
        Box falseQL1S( ql1sFalse );

        result= helloQL1S.Call<FEquals>( helloP ); UT_EQ( true , result )
        result= helloQL1S.Call<FEquals>( helloA ); UT_EQ( true , result )
        result= helloQL1S.Call<FEquals>( falseP ); UT_EQ( false, result )
        result= helloQL1S.Call<FEquals>( falseA ); UT_EQ( false, result )

        result= helloA.Call<FEquals>( helloQL1S ); UT_EQ( true , result )
        result= helloP.Call<FEquals>( helloQL1S ); UT_EQ( true , result )
        result= helloA.Call<FEquals>( falseQL1S ); UT_EQ( false, result )
        result= helloP.Call<FEquals>( falseQL1S ); UT_EQ( false, result )
    }

    UT_PRINT("") UT_PRINT( "### Boxing: Testing Equals: char16_t*/char16_t[]  with QString ###" )
    {
        bool result;
        Box helloP( static_cast<const char16_t*>(u"Hello"));
        Box helloA(                              u"Hello");
        Box falseP( static_cast<const char16_t*>(u"not hello"));
        Box falseA(                              u"not hello");
        QString qsHello( "Hello"     );
        QString qsFalse( "not Hello" );
        Box helloQs( qsHello );
        Box falseQs( qsFalse );

        result= helloQs.Call<FEquals>( helloP ); UT_EQ( true , result )
        result= helloQs.Call<FEquals>( helloA ); UT_EQ( true , result )
        result= helloQs.Call<FEquals>( falseP ); UT_EQ( false, result )
        result= helloQs.Call<FEquals>( falseA ); UT_EQ( false, result )

        result= helloA.Call<FEquals>( helloQs ); UT_EQ( true , result )
        result= helloP.Call<FEquals>( helloQs ); UT_EQ( true , result )
        result= helloA.Call<FEquals>( falseQs ); UT_EQ( false, result )
        result= helloP.Call<FEquals>( falseQs ); UT_EQ( false, result )
    }

    UT_PRINT("") UT_PRINT( "### Boxing: Appending BoxedAs<QTxxx> ###" )
    {
        AString test;
        { QLatin1String q("QLatin1String"); test._() << Box( std::reference_wrapper<QLatin1String>( q ) ); UT_EQ( A_CHAR("QLatin1String")  , test) }
        { QByteArray    q("QByteArray"   ); test._() << Box( std::reference_wrapper<QByteArray   >( q ) ); UT_EQ( A_CHAR("QByteArray")     , test) }
        { QString       q("QString"      ); test._() << Box( std::reference_wrapper<QString      >( q ) ); UT_EQ( A_CHAR("QString")        , test) }
    }

}

#include "unittests/aworx_unittests_end.hpp"

} //namespace
#endif

#endif // ALIB_UT_COMPATIBILITY
