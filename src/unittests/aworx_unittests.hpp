/** ************************************************************************************************
 * \file
 * This header file is part of the unit tests of the \aliblong.
 *
 * \emoji :copyright: 2013-2023 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 *
 * Defines some preprocessor macros and classes so that GTest and MSVC Unit Tests can live in
 * the same cpp file.
 * Relies on ALox logging library, which in turn relies on \alib. Hence, \alib's unit
 * tests can only be compiled if ALox library is present.
 *
 * Before including this header, the prepro variable "TESTCLASSNAME" has to be defined.
 **************************************************************************************************/
#ifndef HPP_AWORX_UNIT_TESTS
#define HPP_AWORX_UNIT_TESTS 1

#if !defined (HPP_ALOX)
#   include "alib/alox.hpp"
#endif


#if ALIB_ALOX
#   if !defined (HPP_ALOX_LOGTOOLS)
#       include "alib/alox/logtools.hpp"
#   endif
#   if !defined (HPP_ALOX_DETAIL_TEXTLOGGER_TEXTLOGGER)
#       include "alib/alox/detail/textlogger/textlogger.hpp"
#   endif
#else
    namespace aworx{  enum class Verbosity { Verbose, Info, Warning, Error, Off }; }
    #include "alib/strings/util/tokenizer.hpp"
    #include "alib/strings/util/spaces.hpp"
#endif

#if ALIB_SYSTEM
#   if !defined(HPP_ALIB_SYSTEM_DIRECTORY)
#       include "alib/system/directory.hpp"
#   endif
#else
    namespace aworx{  constexpr character DirectorySeparator = '/'; }
#endif

#include "alib/strings/localstring.hpp"
#include "alib/results/report.hpp"

#include <iostream>
#include <fstream>

#if !defined(ALIB_GTEST)
    #define ALIB_GTEST 0
#endif


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

    #   include "gtest/gtest.h"

    #if defined(__clang__)
        #pragma clang diagnostic pop
    #endif


    // disable warning missing prototypes for clang/gtest (we can not add prototypes as in the
    // windows code, the test functions are class methods)
    #if defined(__clang__)
        #pragma clang diagnostic ignored "-Wmissing-prototypes"
        #pragma clang diagnostic ignored "-Wmissing-variable-declarations"
    #endif


    #include <iostream>
    #include <iomanip>


    // nothing (almost) in it in GTest: every test is own class
    #if defined(__clang__)
        #define  UT_CLASS                 _Pragma("clang diagnostic push")                                       \
                                          _Pragma("clang diagnostic ignored \"-Wzero-as-null-pointer-constant\"")
        #define  UT_CLASS_END             _Pragma("clang diagnostic pop")
    #else
        #define  UT_CLASS
        #define  UT_CLASS_END
    #endif


    #define  UT_METHOD_Z(m, sm, sc)       GTEST_TEST(TESTCLASSNAME, m)


    #define UT_INIT(...)        aworx::NAString utSC (__FILE__);                                       \
                                {                                                                      \
                                    aworx::integer idx= utSC.LastIndexOf( aworx::DirectorySeparator ); \
                                    utSC.DeleteStart( idx + 1 );                                       \
                                    idx= utSC.LastIndexOf( '.' );                                      \
                                    if( idx > 0 )                                                      \
                                        utSC.Delete( idx );                                            \
                                }                                                                      \
                                AWorxUnitTesting ut( ::testing::UnitTest::GetInstance()->current_test_info()->name() );             \
                                UT_PRINT( "################### Unit Test: {}.{}() ###################", utSC, UT_GET_TEST_NAME );

// ************ Macros using Microsoft Visual Studio UnitTestFramework ************
#elif defined(_WIN32)
    #include "CppUnitTest.h"
    #include "alib/alox/loggers/memorylogger.hpp"
    #define UT_FUNC_MACRO   __FUNCTION__

    #define  UT_CLASS                   TEST_CLASS(TESTCLASSNAME)                                                 \
                                          {   private:  const char* aworxTestName= ALIB_NSTRINGIFY(TESTCLASSNAME);  \
                                              public:

    #define  UT_CLASS_END                 };

    // The prepro is never easy to understand. So do not think too long about why we have
    // to have the sub-macros "_X" and "_Y".
    #define  UT_METHOD_Z(m, sm, sc)       BEGIN_TEST_METHOD_ATTRIBUTE( m )                   \
                                             TEST_METHOD_ATTRIBUTE(  sc, L"void")            \
                                          END_TEST_METHOD_ATTRIBUTE()                        \
                                          TEST_METHOD(m)

    #define UT_INIT(...)        aworx::NAString utSC (__FILE__);                                       \
                                {                                                                      \
                                    aworx::integer idx= utSC.LastIndexOf( aworx::DirectorySeparator ); \
                                    utSC.DeleteStart( idx + 1 );                                       \
                                    idx= utSC.LastIndexOf( '.' );                                      \
                                    if( idx > 0 )                                                      \
                                        utSC.Delete( idx );                                            \
                                }                                                                      \
                                AWorxUnitTesting ut( aworxTestName );                                  \
                                UT_PRINT( "################### Unit Test: {}.{}() ###################", utSC, UT_GET_TEST_NAME );

#else
    #pragma message ("Unknown Testing platform in: " __FILE__ )
#endif

// ************ Generic macros ************

#define  UT_GET_TEST_NAME  ut.ActTestName

#define  UT_PRINT(...   )  { ut.Print (__FILE__, __LINE__, aworx::Verbosity::Info   , __VA_ARGS__ ); }
#define  UT_WARN(...    )  { ut.Print (__FILE__, __LINE__, aworx::Verbosity::Warning, __VA_ARGS__ ); }
#define  UT_EQ(    a,b  )  ut.EQ      (__FILE__, __LINE__,  a,b    );
#define  UT_NEAR( a,b,d )  ut.Near    (__FILE__, __LINE__,  a,b, d );
#define  UT_TRUE(  cond )  ut.IsTrue  (__FILE__, __LINE__,  cond   );
#define  UT_FALSE( cond )  ut.IsFalse (__FILE__, __LINE__,  cond   );


// change logging function name
#undef  ALIB_CALLER
#define ALIB_CALLER     __FILE__, __LINE__, UT_GET_TEST_NAME


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

            virtual void logText( aworx::lib::lox::detail::Domain&     domain,     aworx::lib::lox::Verbosity verbosity,
                                  aworx::AString&               msg,
                                  aworx::lib::lox::detail::ScopeInfo&  scope,      int                     lineNumber);

            virtual void notifyMultiLineOp (aworx::lib::Phase )    {  }

    };
#endif


// *************************************************************************************************
// Class AWorxUnitTesting
// *************************************************************************************************
class AWorxUnitTesting : public aworx::lib::results::ReportWriter
{
    public:
        #if defined(_WIN32)
            bool                                          initializerCout;
        #endif
        aworx::NAString         Domain;
        aworx::NCString         ActTestName;
        bool                    AssertOnFailure= true;

        static aworx::String128 LastAutoSizes;
        static aworx::NAString  GeneratedSamplesDir;
        static aworx::AString   CustomMetaInfoFormat; // used if set from outside

        // defaults to "docs/ALox.CPP/".
        // Set this to a suitable value in your bootstrap code, before using this class with
        // other projects!
        static aworx::String    GeneratedSamplesSearchDir;

    public:
        bool                                              initializer;
#if ALIB_ALOX
        aworx::lib::lox::Lox                              lox;
        aworx::lib::lox::detail::textlogger::TextLogger*  utl;
#else
        aworx::Boxes      logablesFileAndLine;
        aworx::Boxes      logables;
        aworx::AString    outputBuffer;
        aworx::Tokenizer  lines;
#endif

                 AWorxUnitTesting( const aworx::NCString& testName);
        virtual ~AWorxUnitTesting() override;

        template <typename... T>
        void Print (  const aworx::NCString& file, int line, aworx::Verbosity verbosity,  T&&... args  )
        {
            aworx::Boxes& argsBoxed= printPrepare( file, line );
            argsBoxed.Add(std::forward<T>( args )...);
            printDo( verbosity, argsBoxed );
        }

        void Failed(  const aworx::NCString& file, int line, const aworx::Box& exp, const aworx::Box& given );


        template<typename T>
        void WriteResultFile(const aworx::NString& name, const T& output, const aworx::NString& doxyTag= "//! [OUTPUT]" )
        {
            ALIB_DBG(AWorxUnitTesting& ut= *this;) //needed for the assertion, as macro ALIB_CALLER is changed here
            aworx::String4K buf; buf.DbgDisableBufferReplacementWarning();
            buf << output;
            ALIB_ASSERT_ERROR( buf.IsNotEmpty(), "UT", "Empty Doxygen sample output file." )
            writeResultFile( name, buf, doxyTag );
        }

        virtual void NotifyActivation  ( aworx::lib::Phase ) override { }
        virtual void Report  (  aworx::lib::results::Message& msg )                       override;

        template<typename TComp1, typename TComp2>
        ATMP_VOID_IF(     ( aworx::lib::characters::T_CharArray<TComp1 ALIB_COMMA aworx::character>::Access
                                                                !=aworx::lib::characters::AccessType::Implicit)
                      && !std::is_base_of<aworx::NString ALIB_COMMA TComp1>::value )
        EQ( const aworx::NCString& file, int line,  TComp1      exp , TComp2  v )
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


        void EQ( const aworx::NCString& file, int line,  const aworx::NString& exp , const aworx::NString& v );
        void EQ( const aworx::NCString& file, int line,  const aworx::WString& exp , const aworx::WString& v );
        void EQ( const aworx::NCString& file, int line,  wchar_t*              exp , wchar_t*              v );
        void EQ( const aworx::NCString& file, int line,       float            exp ,      float            v );
        void EQ( const aworx::NCString& file, int line,       double           exp ,      double           v );
        void EQ( const aworx::NCString& file, int line,  long double           exp , long double           v );

        template<typename TComp1, typename TComp2, typename TCompDiff>
        void Near   ( const aworx::NCString& file, int line,  TComp1  exp , TComp2 v, TCompDiff prec )
        {
            bool c= (v < exp ? exp-v : v-exp) <= prec;
            if (!c)
                Failed(file,line,exp,v);
            #if ALIB_GTEST
                EXPECT_NEAR ( static_cast<double>(exp), static_cast<double>(v), static_cast<double>(prec) );
            #elif defined(_WIN32)
                Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsTrue  ( c );
            #else
            #   pragma message ("Unknown Testing platform in: " __FILE__ )
            #endif
        }


        void IsTrue ( const aworx::NCString& file, int line,  bool cond );
        void IsFalse( const aworx::NCString& file, int line,  bool cond );

    protected:
        void            writeResultFile(const aworx::NString& name, aworx::AString& output, const aworx::NString& doxyTag );
        aworx::Boxes&   printPrepare   (const aworx::NCString& file, int line  );
        void            printDo        (aworx::Verbosity verbosity, aworx::Boxes& args );
};

} // namespace ut_aworx


#endif // HPP_AWORX_UNIT_TESTS
