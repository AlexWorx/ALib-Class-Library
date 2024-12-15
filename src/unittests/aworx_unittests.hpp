/** ************************************************************************************************
 * \file
 * This header file is part of the unit tests of the \aliblong.
 *
 * \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 *
 * Defines some preprocessor macros and classes so that GTest and MSVC Unit Tests can live in
 * the same cpp file.
 * Relies on ALox logging library, which in turn relies on \alib. Hence, \alib's unit
 * tests can only be compiled if ALox library is present.
 *
 * Before including this header, the prepro variable "TESTCLASSNAME" has to be defined.
 **************************************************************************************************/
#ifndef HPP_ALIB_UNIT_TESTS
#define HPP_ALIB_UNIT_TESTS 1
#pragma once
#if !defined (HPP_ALIB_LOX)
#   include "alib/alox.hpp"
#endif

#if ALIB_ALOX
#   include "alib/alox/logtools.hpp"
#   include "alib/alox/textlogger/textlogger.hpp"
#else
    namespace alib{  enum class Verbosity { Verbose, Info, Warning, Error, Off }; }
#   include "alib/strings/util/tokenizer.hpp"
#endif
#if ALIB_CAMP
#   include "alib/lang/system/path.hpp"
#else
    namespace alib{  constexpr character DIRECTORY_SEPARATOR = '/'; }
#endif
#include "alib/strings/localstring.hpp"
#include "alib/lang/message/report.hpp"
#include <iostream>
#include <fstream>

#if !defined(ALIB_GTEST)
    #define ALIB_GTEST 0
#endif

// Avoid analyzer (valgrind) warnings
#if !defined(ALIB_UT_AVOID_ANALYZER_WARNINGS )
#   define   ALIB_UT_AVOID_ANALYZER_WARNINGS   0
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


    // disable warning missing prototypes for clang/gtest (we cannot add prototypes as in the
    // windows code, the test functions are class methods)
    #if defined(__clang__)
        #pragma clang diagnostic ignored "-Wmissing-prototypes"
        #pragma clang diagnostic ignored "-Wmissing-variable-declarations"
    #endif


    #include <iostream>
    #include <iomanip>


    // nothing (almost) in it in GTest: every test is an own class
    #if defined(__clang__)
        #define  UT_CLASS                 _Pragma("clang diagnostic push")                                       \
                                          _Pragma("clang diagnostic ignored \"-Wzero-as-null-pointer-constant\"")
        #define  UT_CLASS_END             _Pragma("clang diagnostic pop")
    #else
        #define  UT_CLASS
        #define  UT_CLASS_END
    #endif


    #define  UT_METHOD_Z(m, sm, sc)       GTEST_TEST(TESTCLASSNAME, m)


  #if ALIB_EXT_LIB_THREADS_AVAILABLE
    #define UT_INIT(...)                                                                           \
        alib::NAString utSC (__FILE__);                                                            \
        {                                                                                          \
            alib::integer idx= utSC.LastIndexOf( alib::lang::system::DIRECTORY_SEPARATOR );        \
            utSC.DeleteStart( idx + 1 );                                                           \
            idx= utSC.LastIndexOf( '.' );                                                          \
            if( idx > 0 )                                                                          \
                utSC.Delete( idx );                                                                \
        }                                                                                          \
        {alib::lang::Owner<alib::threads::SharedLock> ocfg(ALOX.GetConfigLock()                    \
                  ALIB_DBG(, {nullptr,-1,nullptr, ::std::this_thread::get_id(),&typeid(*this)}));  \
           ALOX.GetConfig().DeletePath(A_CHAR("ALOX")); }                                          \
        AWorxUnitTesting ut( ::testing::UnitTest::GetInstance()->current_test_info()->name() );    \
        UT_PRINT( "################### Unit Test: {}.{}() ###################", utSC, UT_GET_TEST_NAME );
  #else
    #define UT_INIT(...)                                                                           \
        alib::NAString utSC (__FILE__);                                                            \
        {                                                                                          \
            alib::integer idx= utSC.LastIndexOf( alib::lang::system::DIRECTORY_SEPARATOR );        \
            utSC.DeleteStart( idx + 1 );                                                           \
            idx= utSC.LastIndexOf( '.' );                                                          \
            if( idx > 0 )                                                                          \
                utSC.Delete( idx );                                                                \
        }                                                                                          \
        {alib::lang::Owner<alib::threads::SharedLock> ocfg(ALOX.GetConfigLock()                    \
                                                 ALIB_DBG(, {nullptr,-1,nullptr,&typeid(*this)})); \
           ALOX.GetConfig().DeletePath(A_CHAR("ALOX")); }                                          \
        AWorxUnitTesting ut( ::testing::UnitTest::GetInstance()->current_test_info()->name() );    \
        UT_PRINT( "################### Unit Test: {}.{}() ###################", utSC, UT_GET_TEST_NAME );
  #endif

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

    #define UT_INIT(...) alib::NAString utSC (__FILE__);                                        \
                         {                                                                      \
                             alib::Bootstrap();                                                 \
                             alib::integer idx= utSC.LastIndexOf( alib::lang::system::DIRECTORY_SEPARATOR );  \
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

#define  UT_PRINT(...   )  { ut.Print (ALIB_CALLER, alib::Verbosity::Info   , __VA_ARGS__ ); }
#define  UT_WARN(...    )  { ut.Print (ALIB_CALLER, alib::Verbosity::Warning, __VA_ARGS__ ); }
#define  UT_EQ(    a,b  )  ut.EQ      (ALIB_CALLER,  a,b    );
#define  UT_NEAR( a,b,d )  ut.Near    (ALIB_CALLER,  a,b, d );
#define  UT_TRUE(  cond )  ut.IsTrue  (ALIB_CALLER,  cond   );
#define  UT_FALSE( cond )  ut.IsFalse (ALIB_CALLER,  cond   );

#define UT_TEQ(T1,T2)       static_assert( std::is_same<T1 , T2>::value , "Unit test error: types not equal"); 
#define UT_STRUE( exp )     static_assert(  exp                         , "Is not true");                      
#define UT_SFALSE( exp )    static_assert( !exp                         , "Is not false");                     


// change logging function name in caller-info
#undef  ALIB_CALLER
#if ALIB_EXT_LIB_THREADS_AVAILABLE
#   define ALIB_CALLER     {__FILE__, __LINE__, UT_GET_TEST_NAME,::std::this_thread::get_id(),&typeid(*this) }
#else
#   define ALIB_CALLER     {__FILE__, __LINE__, UT_GET_TEST_NAME,&typeid(*this) }
#endif


namespace ut_aworx {


// *************************************************************************************************
// Class UTVStudioLogger
// *************************************************************************************************
#if !ALIB_GTEST
    class UTVStudioLogger : public alib::lox::loggers::MemoryLogger
    {
        alib::WAString          outputString;

        public:

                      UTVStudioLogger();
            virtual  ~UTVStudioLogger();

            virtual void logText( alib::lox::detail::Domain&     domain,     alib::lox::Verbosity verbosity,
                                  alib::AString&               msg,
                                  alib::lox::detail::ScopeInfo&  scope,      int                     lineNumber);

            virtual void notifyMultiLineOp (alib::lang::Phase )    {  }

    };
#endif


// *************************************************************************************************
// Class AWorxUnitTesting
// *************************************************************************************************
class AWorxUnitTesting : public alib::lang::ReportWriter
{
    public:
        #if defined(_WIN32)
            bool              initializerCout;
            static bool       fullyBootstrapped;
        #endif
        alib::NAString         Domain;
        alib::NCString         ActTestName;
        bool                   AssertOnFailure= true;

        static alib::String128 LastAutoSizes;
        static alib::NAString  GeneratedSamplesDir;
        static alib::AString   CustomMetaInfoFormat; // used if set from outside

        // defaults to "docs/ALox.CPP/".
        // Set this to a suitable value in your bootstrap code, before using this class with
        // other projects!
        static alib::String    GeneratedSamplesSearchDir;

    public:
#if ALIB_ALOX
        alib::lox::Lox                      lox;
        alib::lox::textlogger::TextLogger*  utl;
#else
        alib::Boxes      logablesFileAndLine;
        alib::Boxes      logables;
        alib::AString    outputBuffer;
        alib::Tokenizer  lines;
#endif

                 AWorxUnitTesting( const alib::NCString& testName);
        virtual ~AWorxUnitTesting() override;

        template <typename... T>
        void Print (  const alib::CallerInfo& ci, alib::Verbosity verbosity,  T&&... args  )
        {
            alib::BoxesMA& argsBoxed= printPrepare( ci );
            argsBoxed.Add(std::forward<T>( args )...);
            printDo( verbosity, argsBoxed );
        }

        void Failed( const alib::CallerInfo& ci, const alib::Box& exp, const alib::Box& given );


        template<typename T>
        void WriteResultFile(const alib::NString& name, const T& output, const alib::NString& doxyTag= "DOX_MARKER( [OUTPUT])" )
        {
            ALIB_DBG(AWorxUnitTesting& ut= *this;) //needed for the assertion, as macro ALIB_CALLER is changed here
            alib::String4K buf; buf.DbgDisableBufferReplacementWarning();
            buf << output;
            ALIB_ASSERT_ERROR( buf.IsNotEmpty(), "UT", "Empty Doxygen sample output file." )
            writeResultFile( name, buf, doxyTag );
        }

        virtual void NotifyActivation  ( alib::lang::Phase ) override { }
        virtual void Report  (  alib::lang::Message& msg )                       override;

        template<typename TComp1, typename TComp2>
        ATMP_VOID_IF(     ( alib::characters::T_CharArray<TComp1 ALIB_COMMA alib::character>::Access
                                                                !=alib::characters::AccessType::Implicit)
                      && !std::is_base_of<alib::NString ALIB_COMMA TComp1>::value )
        EQ( const alib::CallerInfo& ci,  TComp1      exp , TComp2  v )
        {
            if ( v != exp)
                Failed(ci, exp, v);

            #if ALIB_GTEST
                EXPECT_EQ   ( exp, v );
            #elif defined(_WIN32)
                Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsTrue( exp == v );
            #else
            #   pragma message ("Unknown Testing platform in: " __FILE__ )
            #endif
        }


        void EQ( const alib::CallerInfo& ci,  const alib::NString& exp , const alib::NString& v );
        void EQ( const alib::CallerInfo& ci,  const alib::WString& exp , const alib::WString& v );
        void EQ( const alib::CallerInfo& ci,  wchar_t*              exp , wchar_t*              v );
        void EQ( const alib::CallerInfo& ci,       float            exp ,      float            v );
        void EQ( const alib::CallerInfo& ci,       double           exp ,      double           v );
        void EQ( const alib::CallerInfo& ci,  long double           exp , long double           v );

        template<typename TComp1, typename TComp2, typename TCompDiff>
        void Near   ( const alib::CallerInfo& ci,  TComp1  exp , TComp2 v, TCompDiff prec )
        {
            bool c= (v < exp ? exp-v : v-exp) <= prec;
            if (!c)
                Failed(ci,exp,v);
            #if ALIB_GTEST
                EXPECT_NEAR ( static_cast<double>(exp), static_cast<double>(v), static_cast<double>(prec) );
            #elif defined(_WIN32)
                Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsTrue  ( c );
            #else
            #   pragma message ("Unknown Testing platform in: " __FILE__ )
            #endif
        }


        void IsTrue ( const alib::CallerInfo& ci,  bool cond );
        void IsFalse( const alib::CallerInfo& ci,  bool cond );

    protected:
        void            writeResultFile(const alib::NString& name, alib::AString& output, const alib::NString& doxyTag );
        alib::BoxesMA&  printPrepare   (const alib::lang::CallerInfo& ci  );
        void            printDo        (alib::Verbosity verbosity, alib::BoxesMA& args );
};

} // namespace ut_aworx


#endif // HPP_ALIB_UNIT_TESTS
