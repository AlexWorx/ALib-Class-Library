// #################################################################################################
//  aworx - Unit Tests
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alox.hpp"

#if defined(QT_CORE_LIB)
#include "alib/strings/util/tokenizer.hpp"
#include "alib/strings/numberformat.hpp"
#include "alib/compatibility/qt.hpp"

#include <iostream>
#include <sstream>
#include <iomanip>


#include "aworx_unittests.hpp"

#include <qt/QtCore/QString>
#include <qt/QtCore/QVector>
#include <qt/QtCore/QRect>

#define TESTCLASSNAME       CPP_ALib_Compatibility


using namespace std;
using namespace aworx;

namespace ut_aworx {


UT_CLASS()

//--------------------------------------------------------------------------------------------------
//--- QT_String
//--------------------------------------------------------------------------------------------------
UT_METHOD( QT_String )
{
    UT_INIT();


    QString qts( "QString" );
    AString test( qts );        UT_EQ( ASTR("QString"), test );
    test._()._ ( qts);          UT_EQ( ASTR("QString"), test );
    test._()._ (&qts);          UT_EQ( ASTR("QString"), test );

    QByteArray qba= QString ( "QByteArray" ).toLocal8Bit();
    test._()._ (NString(qba));  UT_EQ( ASTR("QByteArray"), test );
    test._()._ (NString(&qba)); UT_EQ( ASTR("QByteArray"), test );
    test._()._ ( qba);          UT_EQ( ASTR("QByteArray"), test );
    test._()._ (&qba);          UT_EQ( ASTR("QByteArray"), test );

    QVector<uint> qv4=  QString( "QVector<uint>" ).toUcs4();
    AString test4( qv4 );       UT_EQ( ASTR("QVector<uint>"), test4 );
    test4._()._ ( qv4);         UT_EQ( ASTR("QVector<uint>"), test4 );
    test4._()._ (&qv4);         UT_EQ( ASTR("QVector<uint>"), test4 );


    QLatin1String ql1s( "Latin1String" );
    test._()._ (NString(ql1s)); UT_EQ( ASTR("Latin1String"), test );
    test._()._ (NString(&ql1s));UT_EQ( ASTR("Latin1String"), test );
    test._()._ ( ql1s);         UT_EQ( ASTR("Latin1String"), test );
    test._()._ (&ql1s);         UT_EQ( ASTR("Latin1String"), test );

    QChar qc= 'c';
    test._()._ (qc);            UT_EQ( ASTR("c"), test );


    UT_PRINT( String128() << "QString toUtf8() " );
    UT_PRINT( String128() << "  QString: German:  " << QString("ÄÜÖäüöß").toLocal8Bit() );
    UT_PRINT( String128() << "  QString: Greek:   " << QString().fromStdWString(L"\u03B1\u03B2\u03B3\u03B4\u03B5").toLocal8Bit() );
    UT_PRINT( String128() << "  QString: Smileys: " << QString().fromStdWString(L"\U0001F609 * \U0001F607 * \U0001F603 * \U0001F60E * "
                                                                         "\U00000361\U000000b0\U0000035c\U00000296\U00000361\U000000b0").toLocal8Bit() );
    UT_PRINT( String128() << "QString directly " );
    UT_PRINT( String128() << "  QString: German:  " << QString().fromLocal8Bit("ÄÜÖäüöß") );
    UT_PRINT( String128() << "  QString: Greek:   " << QString().fromStdWString(L"\u03B1\u03B2\u03B3\u03B4\u03B5") );

}

UT_METHOD( QT_StringBoxing )
{
    UT_INIT();
    aworx::lib::boxing::InitQTString();

    UT_PRINT("") UT_PRINT( "### Boxing: Testing Equals: char*/char[]  with QByteArray ###" );
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

        result= helloQBA.Invoke<IEquals, bool>( helloP ); UT_EQ( true , result );
        result= helloQBA.Invoke<IEquals, bool>( helloA ); UT_EQ( true , result );
        result= helloQBA.Invoke<IEquals, bool>( falseP ); UT_EQ( false, result );
        result= helloQBA.Invoke<IEquals, bool>( falseA ); UT_EQ( false, result );

        result= helloA.Invoke<IEquals, bool>( helloQBA ); UT_EQ( true , result );
        result= helloP.Invoke<IEquals, bool>( helloQBA ); UT_EQ( true , result );
        result= helloA.Invoke<IEquals, bool>( falseQBA ); UT_EQ( false, result );
        result= helloP.Invoke<IEquals, bool>( falseQBA ); UT_EQ( false, result );
    }

    UT_PRINT("") UT_PRINT( "### Boxing: Testing Equals: char*/char[]  with QLatin1String ###" );
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

        result= helloQL1S.Invoke<IEquals, bool>( helloP ); UT_EQ( true , result );
        result= helloQL1S.Invoke<IEquals, bool>( helloA ); UT_EQ( true , result );
        result= helloQL1S.Invoke<IEquals, bool>( falseP ); UT_EQ( false, result );
        result= helloQL1S.Invoke<IEquals, bool>( falseA ); UT_EQ( false, result );

        result= helloA.Invoke<IEquals, bool>( helloQL1S ); UT_EQ( true , result );
        result= helloP.Invoke<IEquals, bool>( helloQL1S ); UT_EQ( true , result );
        result= helloA.Invoke<IEquals, bool>( falseQL1S ); UT_EQ( false, result );
        result= helloP.Invoke<IEquals, bool>( falseQL1S ); UT_EQ( false, result );
    }

    UT_PRINT("") UT_PRINT( "### Boxing: Testing Equals: char16_t*/char16_t[]  with QString ###" );
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

        result= helloQs.Invoke<IEquals, bool>( helloP ); UT_EQ( true , result );
        result= helloQs.Invoke<IEquals, bool>( helloA ); UT_EQ( true , result );
        result= helloQs.Invoke<IEquals, bool>( falseP ); UT_EQ( false, result );
        result= helloQs.Invoke<IEquals, bool>( falseA ); UT_EQ( false, result );

        result= helloA.Invoke<IEquals, bool>( helloQs ); UT_EQ( true , result );
        result= helloP.Invoke<IEquals, bool>( helloQs ); UT_EQ( true , result );
        result= helloA.Invoke<IEquals, bool>( falseQs ); UT_EQ( false, result );
        result= helloP.Invoke<IEquals, bool>( falseQs ); UT_EQ( false, result );
    }

    UT_PRINT("") UT_PRINT( "### Boxing: TApply for BoxedAs<QTxxx> ###" );
    {
        AString test;
        { QLatin1String q("QLatin1String"); test._() << Box( BoxedAs<QLatin1String>( q ) ); UT_EQ( ASTR("QLatin1String")  , test); }
        { QByteArray    q("QByteArray"   ); test._() << Box( BoxedAs<QByteArray   >( q ) ); UT_EQ( ASTR("QByteArray")     , test); }
        { QString       q("QString"      ); test._() << Box( BoxedAs<QString      >( q ) ); UT_EQ( ASTR("QString")        , test); }
    }

}

UT_CLASS_END

} //namespace
#endif
