// #################################################################################################
//  ut_aworx - AWorx Unit Test Support using ALib and ALox
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//
//  Defines some preprocessor macros and classes so that GTest and MSVC Unit Tests can live in
//  the same cpp file.
//  Relies on ALox logging library, which in turn relies on ALib. Hence, ALibs' unit
//  tests can only be compiled if ALox library is present.
//
//  Before including this header, the prepro variable "TESTCLASSNAME" has to be defined.
// #################################################################################################


// include ALox main header first...
#if !defined (HPP_ALOX)
    #include "alib/alox.hpp"
#endif

// then, set include guard
#ifndef HPP_ALIB_ALOX_UNIT_TESTS
#define HPP_ALIB_ALOX_UNIT_TESTS 1

#include "alib/alox/logtools.hpp"

#if !defined(HPP_ALIB_SYSTEM_DIRECTORY)
    #include "alib/system/directory.hpp"
#endif

#include <iostream>
#include <fstream>

/**
 * @addtogroup GrpALibCompilerSymbols
 * @{ \def  ALIB_GTEST_ON
 *  This symbol may be passed to the compiler to define code selection symbol
 *  \ref ALIB_GTEST which selects unit test code in accordance to the Google Test libraries.
 * @}
 *
 * @addtogroup GrpALibCodeSelectorSymbols
 * @{ \def  ALIB_GTEST
 *  Selects unit test code in accordance to the Google Test libraries.
 *  Use \ref ALIB_GTEST_ON to define this symbol.
 * @}
 */
#if defined(DOX_PARSER)
    #define  ALIB_GTEST
    #define  ALIB_GTEST_ON
#else
    #if defined(ALIB_GTEST)
        #error "ALIB_GTEST must not be set from outside. Use ALIB_GTEST_ON instead!"
    #endif

    #if defined(ALIB_GTEST_ON)
        #define ALIB_GTEST 1
    #else
        #define ALIB_GTEST 0
    #endif
#endif //DOX_PARSER


// *************************************************************************************************
// Unit test macros
// *************************************************************************************************
#define  UT_METHOD(name)         UT_METHOD_X(name, TESTCLASSNAME)
#define  UT_METHOD_X(m, sc)      UT_METHOD_Y(m, ALIB_STRINGIFY( m ), ALIB_STRINGIFY( sc ))
#define  UT_METHOD_Y(m, sm, sc)  UT_METHOD_Z(m, sm, sc)

// ************ Macros using GTest library ************
#if ALIB_GTEST

    // include gtest
    #if defined(__clang__)
        #pragma clang diagnostic push
        #pragma clang diagnostic ignored "-Wundef"
        #pragma clang diagnostic ignored "-Wdeprecated"
        #pragma clang diagnostic ignored "-Wmissing-noreturn"
        #pragma clang diagnostic ignored "-Wshift-sign-overflow"
        #pragma clang diagnostic ignored "-Wused-but-marked-unused"
        #pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"
    #endif

        #include "gtest/gtest.h"

    #if defined(__clang__)
        #pragma clang diagnostic pop
    #endif


    // disable warning missing prototypes for clang/gtest (we can not add prototypes as in the
    // windows code, the test functions are class methods)
    #if defined(__clang__)
        #pragma clang diagnostic ignored "-Wmissing-prototypes"
        #pragma clang diagnostic ignored "-Wmissing-variable-declarations"
    #endif


    #include "iostream"
    #include "iomanip"


    // nothing (almost) in it in GTest: every test is is own class
    #if defined(__clang__)
        #define  UT_CLASS(name)           _Pragma("clang diagnostic push")                                       \
                                          _Pragma("clang diagnostic ignored \"-Wzero-as-null-pointer-constant\"")
        #define  UT_CLASS_END             _Pragma("clang diagnostic pop")
    #else
        #define  UT_CLASS(name)
        #define  UT_CLASS_END
    #endif


    #define  UT_METHOD_Z(m, sm, sc)       GTEST_TEST(TESTCLASSNAME, m)

    #define  UT_GET_TEST_NAME             ::testing::UnitTest::GetInstance()->current_test_info()->name()

// ************ Macros using Microsoft Visual Studio UnitTestFramework ************
#elif defined(_WIN32)
    #include "CppUnitTest.h"
    #include "alib/alox/loggers/memorylogger.hpp"
    #define UT_FUNC_MACRO   __FUNCTION__

    #define  UT_CLASS()                   TEST_CLASS(TESTCLASSNAME)                                                 \
                                          {   private:  const char* aworxTestName= ALIB_NSTRINGIFY(TESTCLASSNAME);  \
                                              public:

    #define  UT_CLASS_END                 };

    // The prepro is never easy to understand. So do not think too long about why we have
    // to have the sub-macros "_X" and "_Y".
    #define  UT_METHOD_Z(m, sm, sc)       BEGIN_TEST_METHOD_ATTRIBUTE( m )                   \
                                             TEST_METHOD_ATTRIBUTE(  sc, L"void")            \
                                          END_TEST_METHOD_ATTRIBUTE()                        \
                                          TEST_METHOD(m)


    #define  UT_GET_TEST_NAME             aworxTestName

#else
    #pragma message ("Unknown Testing platform in: " __FILE__ )
#endif

// ************ Generic macros ************

#define UT_INIT(...)        aworx::AString utSC (__FILE__);                                        \
                            {                                                                      \
                                aworx::integer idx= utSC.LastIndexOf( aworx::DirectorySeparator ); \
                                utSC.DeleteStart( idx + 1 );                                       \
                                idx= utSC.LastIndexOf( '.' );                                      \
                                if( idx > 0 )                                                      \
                                    utSC.Delete( idx );                                            \
                            }                                                                      \
                            AWorxUnitTesting ut( UT_GET_TEST_NAME, UT_GET_TEST_NAME );             \
                            UT_PRINT( "################### Unit Test: {}.{}() ###################", utSC, UT_GET_TEST_NAME );


#define  UT_PRINT(...   )   { ut.Print (__FILE__, __LINE__, aworx::Verbosity::Info, __VA_ARGS__ ); }
#define  UT_EQ(    a,b  )   ut.EQ      (__FILE__, __LINE__,  a,b    );
#define  UT_NEAR( a,b,d )   ut.Near    (__FILE__, __LINE__,  a,b, d );
#define  UT_TRUE(  cond )   ut.IsTrue  (__FILE__, __LINE__,  cond   );
#define  UT_FALSE( cond )   ut.IsFalse (__FILE__, __LINE__,  cond   );


namespace ut_aworx {


// *************************************************************************************************
// Class UTVStudioLogger
// *************************************************************************************************
#if !ALIB_GTEST
    class UTVStudioLogger : public aworx::lib::lox::loggers::MemoryLogger
    {
        aworx::WAString          outputString;

        public:

                      UTVStudioLogger();
            virtual  ~UTVStudioLogger();

            virtual int   AddAcquirer( ThreadLock* newAcquirer );
            virtual int   RemoveAcquirer( ThreadLock* acquirer );

            virtual void logText( aworx::lib::lox::core::Domain&     domain,     aworx::lib::lox::Verbosity verbosity,
                                  aworx::AString&               msg,
                                  aworx::lib::lox::core::ScopeInfo&  scope,      int                     lineNumber);

            virtual void notifyMultiLineOp (aworx::lib::lang::Phase )    {  }

    };
#endif


// *************************************************************************************************
// Class AWorxUnitTesting
// *************************************************************************************************
class AWorxUnitTesting : public aworx::lib::lang::ReportWriter
{
    public:
        aworx::NAString         Domain;
        aworx::NString          ActTestName;
        bool                    AssertOnFailure= true;

        static aworx::String128 LastAutoSizes;
        static aworx::NAString  GeneratedSamplesDir;
        static aworx::AString   CustomMetaInfoFormat; // used if set from outside

        // defaults to "docs/ALox.CPP/".
        // Set this to a suitable value in your bootstrap code, before using this class with
        // other projects!
        static aworx::String    GeneratedSamplesSearchDir;

    public:
        bool                                         initializer;
        aworx::lib::lox::Lox                              lox;
        aworx::lib::lox::core::textlogger::TextLogger*    utl;

                 AWorxUnitTesting( const aworx::NTString& domain,  const aworx::NTString& testName);
        virtual ~AWorxUnitTesting();

        template <typename... T>
        void Print (  const aworx::NString& file, int line, aworx::Verbosity verbosity,  T&&... args  )
        {
            aworx::Boxes argsBoxed(std::forward<T>( args )...);
            if( argsBoxed.size() == 1 && argsBoxed.back().IsType<aworx::Exception*>() )
            {
                aworx::LogTools::Exception( lox, *argsBoxed.back().Unbox<aworx::Exception*>()  );
                return;
            }

            lox.Acquire(file, line, ActTestName);
                lox.GetLogableContainer().Add( argsBoxed );
                lox.Entry( Domain, verbosity );
            lox.Release();
        }

        void Failed(  const aworx::NString& file, int line, const aworx::Box& exp, const aworx::Box& given );


        template<typename T>
        void WriteResultFile(const aworx::NString& name, const T& output, const aworx::NString& doxyTag= "//! [OUTPUT]" )
        {
            aworx::AString buf;
            buf << output;
            writeResultFile( name, buf, doxyTag );
        }

        virtual void NotifyActivation  ( aworx::Phase ) { }
        virtual void Report  (  const aworx::lib::lang::Report::Message& msg );

        template<typename TComp1, typename TComp2,
                  typename TEnableIf= typename std::enable_if<!(     aworx::lib::strings::T_String<TComp1,aworx::character>::value
                                                                  || std::is_base_of<aworx::String, TComp1>::value )
                                                             >::type>
        void EQ( const aworx::NTString& file, int line,  TComp1      exp , TComp2  v )
        {
            if ( v != exp)
                Failed(file,line, exp, v);

            #if ALIB_GTEST
                EXPECT_EQ   ( exp, v );
            #elif defined(_WIN32)
                Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsTrue( exp == v );
            #else
            #   pragma message ("Unknown Testing platform in: " __FILE__ )
            #endif

        }


        void EQ( const aworx::NTString& file, int line,  const aworx::String&  exp , const aworx::String&  v );
        void EQ( const aworx::NTString& file, int line,  wchar_t*    exp , wchar_t*   v );
        void EQ( const aworx::NTString& file, int line,  double exp , double v );

        template<typename TComp1, typename TComp2, typename TCompDiff>
        void Near   ( const aworx::NTString& file, int line,  TComp1  exp , TComp2 v, TCompDiff prec )
        {
            bool c= (v < exp ? exp-v : v-exp) <= prec;
            if (!c)
                Failed(file,line,exp,v);
            #if ALIB_GTEST
                EXPECT_NEAR ( exp, v, prec     );
            #elif defined(_WIN32)
                Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsTrue  ( c );
            #else
            #   pragma message ("Unknown Testing platform in: " __FILE__ )
            #endif
        }


        void IsTrue ( const aworx::NTString& file, int line,  bool cond );
        void IsFalse( const aworx::NTString& file, int line,  bool cond );

    protected:
        void writeResultFile(const aworx::NString& name, aworx::AString& output, const aworx::NString& doxyTag );

};

} // namespace ut_aworx


#endif // HPP_ALIB_ALOX_UNIT_TESTS
