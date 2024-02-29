// #################################################################################################
//  AWorx ALib Unit Tests
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#include "alib/alox.hpp"
#include "alib/lang/basecamp/basecamp.hpp"
#include "alib/lang/basecamp/bootstrap.hpp"

#if ALIB_ALOX
#   if !defined (HPP_ALIB_ALOXMODULE)
#      include "alib/alox/aloxmodule.hpp"
#   endif

#   if !defined(HPP_ALOX_CONSOLE_LOGGER)
#      include "alib/alox/loggers/consolelogger.hpp"
#   endif

#   if !defined(HPP_ALOX_ANSI_LOGGER)
#      include "alib/alox/loggers/ansilogger.hpp"
#   endif

#   if !defined(HPP_ALOX_VSTUDIO_LOGGER)
#      include "alib/alox/loggers/vstudiologger.hpp"
#   endif

#   if !defined(HPP_ALOX_WINDOWS_CONSOLE_LOGGER)
#      include "alib/alox/loggers/windowsconsolelogger.hpp"
#   endif
#else
#   if !defined (HPP_ALIB_LANG_FORMAT_FORMATTER)
#       include "alib/lang/format/formatter.hpp"
#   endif
#   include "alib/compatibility/std_strings_iostream.hpp"
#endif
#if ALIB_THREADS
#   include "alib/threads/smartlock.hpp"
#endif

#include "alib/strings/substring.hpp"

#if ALIB_CAMP
#   include "alib/lang/system/environment.hpp"
#endif
#include "alib/compatibility/std_boxing.hpp"

#include "unittests/aworx_unittests.hpp"
#include <limits>
#include <assert.h>

#if defined(__clang__)
    #pragma message "Clang Compiler (not a warning, just for information)"
#elif defined(__GNUC__)
    #pragma message "GNU Compiler (not a warning, just for information)"
#elif defined(_MSC_VER)
    #pragma message ("MS Compiler (not a warning, just for information)")
#endif

using namespace std;
using namespace alib;
ALIB_IF_ALOX(
using namespace alib::lox::detail; )

namespace ut_aworx {

#if defined(_WIN32)
    class VstudioDbgStreamForCout : public std::stringbuf
    {
        public:
        ~VstudioDbgStreamForCout() { sync(); }
        int sync()
        {
            ::OutputDebugStringA( str().c_str() );
            str( std::string() ); // Clear the string buffer
            return 0;
        }
    };
    VstudioDbgStreamForCout vstudioDbgStreamForCout;
#endif


// #################################################################################################
// Fields
// #################################################################################################
String128 AWorxUnitTesting::LastAutoSizes;
NAString  AWorxUnitTesting::GeneratedSamplesDir;
String    AWorxUnitTesting::GeneratedSamplesSearchDir= A_CHAR("docs/pages");
AString   AWorxUnitTesting::CustomMetaInfoFormat;


#if defined(_WIN32)
bool       AWorxUnitTesting::fullyBootstrapped= false;
#endif

// #################################################################################################
// Constructors/destructor
// #################################################################################################
AWorxUnitTesting::AWorxUnitTesting( const NCString& testName)
#if defined(_WIN32)
// Redirect std::cout to OutputDebugString
: initializerCout(    std::cout.rdbuf( &vstudioDbgStreamForCout )
                   && std::cerr.rdbuf( &vstudioDbgStreamForCout )  )
,
#else
:
#endif
#if ALIB_ALOX
lox( "UTLox", false )
#endif
{
    this->Domain.Reset("UT/") << testName;
    this->Domain.ToUpper();
    this->ActTestName=  testName;

    #if defined(_WIN32) && !ALIB_GTEST
	if( !fullyBootstrapped )
	{ 
		fullyBootstrapped= true;
		ALIB_IF_THREADS(monomem::GlobalAllocatorLock.Acquire(ALIB_DBG(nullptr, 113, "AWorxUnitTesting")); )
		alib::boxing::compatibility::std::BootstrapStdStringBoxing();
		ALIB_IF_THREADS( monomem::GlobalAllocatorLock.Release(); )
	}
    #endif

    //needed for the assertion, as macro ALIB_CALLER is changed here
    ALIB_IF_ALOX( AWorxUnitTesting& ut= *this; )

    Log_SetSourcePathTrimRule( "*/src/", lang::Inclusion::Include )

#if ALIB_ALOX
    #if !ALIB_GTEST
        utl= new UTVStudioLogger();
    #elif defined(ALOX_UNITTESTS_QMAKE_BUILD)
        {
            utl= new ConsoleLogger("UT_LOGGER");

            // if QTCreator/ QMake was detected, then use absolute paths
            utl->MetaInfo->Format= "   Loc: [%SP/%SF(%SL)]:%A3%SM %A3{%TC +%TL}{%tN}{%D}%A1#%#: ";
            utl->MetaInfo->VerbosityError=   "{ERR}";
            utl->MetaInfo->VerbosityWarning= "{WRN}";
            utl->MetaInfo->VerbosityInfo=    "     ";
            utl->MetaInfo->VerbosityVerbose= "{***}";

            Directory moduleName( Directory::SpecialFolder::Camp );
            integer idx= moduleName.Path.IndexOf( "/ALox/" );
            if ( idx > 0 )
            {
                moduleName.Path.SetLength( idx + 5 );
                lox.ClearSourcePathTrimRules( Reach::Global, false );
                lox.SetSourcePathTrimRule( "*/src/", Inclusion::Exclude, 0, Case::Ignore, moduleName.Path,
                                           Reach::Global  );
            }

            utl->SetReplacement( "[", "{" );
            utl->SetReplacement( "]", "}" );
        }
    #else
        utl= Lox::CreateConsoleLogger("UT_LOGGER");

        // check if we are in CLion. Here it is important to switch off the use of dark/light colors
        if( utl->GetTypeName().Equals("ANSI_CONSOLE") )
        {
            if ( NString(*ArgVN).IndexOf<true, lang::Case::Ignore>( "CLion", 0) >= 0 )
                dynamic_cast<AnsiConsoleLogger*>(utl)->UseLightColors= AnsiLogger::LightColorUsage::Never;
        }
    #endif

    if ( CustomMetaInfoFormat.IsNotEmpty() )
        utl->MetaInfo->Format= CustomMetaInfoFormat;

    if ( LastAutoSizes.IsNotEmpty() )
        utl->AutoSizes.Import( LastAutoSizes, lang::CurrentData::Keep );

    lox.Acquire(ALIB_CALLER);
        lox.SetVerbosity( utl, Verbosity::Info, "/" );
        lox.SetVerbosity( utl, Verbosity::Verbose, Domain);
        lox.SetVerbosity( utl, Verbosity::Warning, Lox::InternalDomains );
        lox.SetVerbosity( utl, Verbosity::Info,    NString64() << Lox::InternalDomains << "UT_REPORT" );

    lox.Release();

    lang::Report::GetDefault().PushWriter( this );
#endif
}

AWorxUnitTesting::~AWorxUnitTesting()
{
#if ALIB_ALOX
    AWorxUnitTesting& ut= *this; //needed for the assertion, as macro ALIB_CALLER is changed here
    lox.Acquire(ALIB_CALLER);
        lox.RemoveLogger( utl );
    lox.Release();

    LastAutoSizes.Reset();
    utl->AutoSizes.Export( LastAutoSizes );

    // clean debug lox and ALox
    Log_Prune( ALOX.Reset() );
#endif

    // check if ALib SmartLock for std I/O was released properly
    #if ALIB_THREADS
        #if ALIB_ALOX
            this->EQ (__FILE__, __LINE__, 0, SmartLock::StdOutputStreams.CntAcquirers()      );
            this->EQ (__FILE__, __LINE__, 0, SmartLock::StdOutputStreams.CountAcquirements() );
        #else
            // without ALox, the standared report writer might attach
            this->IsTrue(__FILE__, __LINE__, 2 > SmartLock::StdOutputStreams.CntAcquirers()     );
            this->IsTrue(__FILE__, __LINE__, 2 > SmartLock::StdOutputStreams.CountAcquirements() );
        #endif
    #endif

ALIB_IF_ALOX(
    lang::Report::GetDefault().PopWriter( this ); )

    // check if ALib default formatter was released properly
    ALIB_IF_CAMP(
        this->EQ (__FILE__, __LINE__, 0, Formatter::GetDefault().get()->CountAcquirements() ); )

ALIB_IF_ALOX(
    delete utl; )
}


// #################################################################################################
// Print
// #################################################################################################
Boxes& AWorxUnitTesting::printPrepare (  const alib::NCString& file, int line )
{
    #if ALIB_ALOX
        lox.Acquire(file, line, ActTestName);
        return lox.GetLogableContainer();
    #else
        logablesFileAndLine.clear();
        logablesFileAndLine.Add(file, ':', line, ": ");
        logables.clear();
        return logables;
    #endif
}

void AWorxUnitTesting::printDo (  alib::Verbosity verbosity, alib::Boxes& args )
{
    #if ALIB_AVOID_ANALYZER_WARNINGS  && defined(ALIB_UT_ROUGH_EXECUTION_SPEED_TEST)
        (void) verbosity;
        (void) args;
        #if ALIB_ALOX
            lox.Release();
        #endif
    #else

    #if ALIB_ALOX
        if( args.Size() == 1 && args.back().IsType<alib::Exception*>() )
        {
            lox.Release();
            alib::LogTools::Exception( lox, *args.back().Unbox<alib::Exception*>()  );
            return;
        }

        lox.Entry( Domain, verbosity );
        lox.Release();
    #else
        (void) verbosity;

        ALIB_DBG(auto& ut= *this); // needed for ALIB_CALLER_PRUNED macro, which is modified here
        SPFormatter formatter= Formatter::AcquireDefault(ALIB_CALLER_PRUNED);
            formatter->FormatArgs( outputBuffer.Reset(), logablesFileAndLine );
            integer indent= outputBuffer.Length();
            std::cout << outputBuffer;
            formatter->FormatArgs( outputBuffer.Reset(), args );
            if( NewLine().Length() > 1 )
                outputBuffer.SearchAndReplace( NewLine(), A_CHAR("\n"));
            lines.Set(outputBuffer, '\n' );
            std::cout << lines.Next() << std::endl;
            while( lines.HasNext() )
                std::cout << alib::Spaces::GetN().Substring(0, indent ) << lines.Next() << std::endl;

        formatter->Release();
    #endif
    #endif
}

void AWorxUnitTesting::Failed( const NCString& file, int line, const Box& exp, const Box& given )
{
    Print( file, line, alib::Verbosity::Error, "UT Failure: Expected: \"{!ESC}\"\n"
                                                "               given: \"{!ESC}\"", exp, given );
    assert(!AssertOnFailure);
}

void AWorxUnitTesting::writeResultFile(const NString& name, alib::AString& output, const NString& doxyTag )
{
#if !ALIB_ALOX
    (void) name;
    (void) output;
    (void) doxyTag;
#else
    #ifndef ALIB_BASE_DIR
        #error "Dox samples output directory not given (Compiler variable 'ALIB_BASE_DIR' not properly set?)."
    #endif

    ALIB_DBG(AWorxUnitTesting& ut= *this;) //needed for the assertion, as macro ALIB_CALLER is changed here

    // if invoked the first time, search the right directory
    if ( GeneratedSamplesDir.IsNull() )
    {
        GeneratedSamplesDir.Reset() << ALIB_BASE_DIR
                                    << A_CHAR("/docs/pages/generated/");
    }

    if ( GeneratedSamplesDir.IsNotNull() && GeneratedSamplesDir.IsEmpty() )
    {
        ALIB_ERROR( "UNITTESTS", String512("Samples directory \"") << GeneratedSamplesDir <<" not found." )
        return;
    }

    NString256 fileName( GeneratedSamplesDir );
    fileName._( name );
    ofstream outputFile;
    outputFile.open ( fileName );
    if ( doxyTag.IsNotEmpty() )   outputFile << doxyTag <<endl;
    outputFile << output;
    if ( doxyTag.IsNotEmpty() )   outputFile << doxyTag <<endl;
    outputFile.close();
#endif
}

// #################################################################################################
// Error/Warning
// #################################################################################################
void AWorxUnitTesting::Report  ( lang::Message& msg )
{
    #if !ALIB_ALOX
        (void) msg;
        assert(false);
    #else
        lox.Acquire(msg.File, msg.Line, msg.Function);
            NString256 domain;
            domain << Lox::InternalDomains << "UT_REPORT";

            // detect subdomain
            if(     msg.Size() > 1
                &&  msg[0].IsArrayOf<nchar>() )
            {
                bool illegalCharacterFound= false;
                NString firstArg= msg[0].Unbox<NString>();
                for( integer idx= 0 ;  idx< firstArg.Length() ; ++idx )
                {
                    char c= firstArg[idx];
                    if (!    (    isdigit( c )
                               || ( c >= 'A' && c <= 'Z' )
                               || c == '-'
                               || c == '_'
                               || c == '/'
                               || c == '.'
                      )      )
                    {
                        illegalCharacterFound= true;
                        break;
                    }
                }

                if(!illegalCharacterFound)
                {
                    domain << '/' << firstArg;
                    msg.erase( msg.begin() );
                }
            }
            lox.GetLogableContainer().Add( msg );
            lox.Entry( domain,
                       msg.Type == alib::lang::Report::Types::Error   ? Verbosity::Error    :
                       msg.Type == alib::lang::Report::Types::Warning ? Verbosity::Warning  :
                       msg.Type == alib::lang::Report::Types::Message ? Verbosity::Info     :
                                                                                 Verbosity::Verbose    );

        lox.Release();
    #endif
}

#if !ALIB_GTEST

    UTVStudioLogger::UTVStudioLogger()
    : MemoryLogger( "VSTUDIO_UNITTEST_CONSOLE" )
    {
        // prevent cutting off filenames
        MetaInfo->Format.SearchAndReplace( A_CHAR("%Sp"), A_CHAR("%SP") );

        // we set this, to make the unit-tests compatible in respect to locking SmartLock::StdOutputStreams
        usesStdStreams= true;
    }

    UTVStudioLogger::~UTVStudioLogger()
    {
    }

    int   UTVStudioLogger::AddAcquirer( ThreadLock* newAcquirer )
    {
        // when added to a lox, we register as std output stream user. This would not be necessary, because
        // we do not write to the std output stream. But in other environments, the unit test logger does
        // so, and therefore, if we dont do it, the unit test which tests the whole concept would fail.
         SmartLock::StdOutputStreams.AddAcquirer( this );

        // call parent's implementation
        return Logger::AddAcquirer( newAcquirer );
    }

    int   UTVStudioLogger::RemoveAcquirer( ThreadLock* acquirer )
    {
         SmartLock::StdOutputStreams.RemoveAcquirer( this );

        // call parent's implementation
        return Logger::RemoveAcquirer( acquirer );
    }



    void UTVStudioLogger::logText( Domain&       domain,
                                   Verbosity     verbosity,
                                   AString&      msg,
                                   ScopeInfo&    scope,
                                   int           lineNumber )

    {
        MemoryLogger::logText( domain, verbosity, msg, scope, lineNumber );
        outputString.Reset( MemoryLog );
        Microsoft::VisualStudio::CppUnitTestFramework::Logger::WriteMessage( outputString );
        MemoryLog.Reset();
    }
#endif


// #################################################################################################
// EQ
// #################################################################################################

#if ALIB_GTEST
    void AWorxUnitTesting::EQ     ( const NCString& file, int line, float          exp, float           d )  { float  p= std::numeric_limits<float >::epsilon() * 2; if ((d < exp ? exp-d : d-exp) > p) { Failed(file,line,exp,d); EXPECT_TRUE( false );} }
    void AWorxUnitTesting::EQ     ( const NCString& file, int line, double         exp, double          d )  { double p= std::numeric_limits<double>::epsilon() * 2; if ((d < exp ? exp-d : d-exp) > p) { Failed(file,line,exp,d); EXPECT_TRUE( false );} }
    void AWorxUnitTesting::EQ     ( const NCString& file, int line, long double    exp, long double     d )  { EQ(file,line,static_cast<double>(exp), static_cast<double>(d) ); }
    void AWorxUnitTesting::EQ     ( const NCString& file, int line, const NString& exp, const NString&  s )  { if (!exp.Equals(s))                  Failed(file,line,exp,s); EXPECT_TRUE ( exp.Equals(s) ); }
    void AWorxUnitTesting::EQ     ( const NCString& file, int line, const WString& exp, const WString&  s )  { if (!exp.Equals(s))                  Failed(file,line,exp,s); EXPECT_TRUE ( exp.Equals(s) ); }
    void AWorxUnitTesting::EQ     ( const NCString& file, int line, wchar_t*       exp, wchar_t*        s )  { if (wcscmp(exp,s)!=0)                Failed(file,line,exp,s); EXPECT_STREQ( exp, s        ); }

    void AWorxUnitTesting::IsTrue ( const NCString& file, int line, bool cond )                              {                        if (!cond )   Failed(file,line, true , false ); EXPECT_TRUE ( cond ); }
    void AWorxUnitTesting::IsFalse( const NCString& file, int line, bool cond )                              {                        if (cond )    Failed(file,line, false, true  ); EXPECT_FALSE( cond ); }

#elif defined(_WIN32)
    void AWorxUnitTesting::EQ     ( const NCString& file, int line, float          exp, float           d )  { float  p= std::numeric_limits<float >::epsilon() * 2; if ((d < exp ? exp-d : d-exp) > p) { Failed(file,line,exp,d); Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsTrue (false); } }
    void AWorxUnitTesting::EQ     ( const NCString& file, int line, double         exp, double          d )  { double p= std::numeric_limits<double>::epsilon() * 2; if ((d < exp ? exp-d : d-exp) > p) { Failed(file,line,exp,d); Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsTrue (false); } }
    void AWorxUnitTesting::EQ     ( const NCString& file, int line, long double    exp, long double     d )  { EQ(file,line,static_cast<double>(exp), static_cast<double>(d) ); }
    void AWorxUnitTesting::EQ     ( const NCString& file, int line, const NString& exp, const NString&  v )  { if (!exp.Equals(v))                              Failed(file,line,exp,v);    Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsTrue  ( exp.Equals(v) ); }
    void AWorxUnitTesting::EQ     ( const NCString& file, int line, const WString& exp, const WString&  v )  { if (!exp.Equals(v))                              Failed(file,line,exp,v);    Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsTrue  ( exp.Equals(v) ); }
    void AWorxUnitTesting::EQ     ( const NCString& file, int line, wchar_t*       exp,       wchar_t*  v )  { bool c= wcscmp( v, exp )==0;              if(!c) Failed(file,line,"","Differences in wchar string."); Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsTrue  ( c      ); }

    void AWorxUnitTesting::IsTrue ( const NCString& file, int line, bool c )                                 {                                           if(!c) Failed(file,line,true ,false);Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsTrue  ( c      ); }
    void AWorxUnitTesting::IsFalse( const NCString& file, int line, bool c )                                 {                                           if( c) Failed(file,line,false,true );Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsFalse ( c      ); }

#else
    #pragma message ("Unknown Testing platform in: " __FILE__ )
#endif


}//namespace


// Had to remove this "suddenly". Is it ok, to keep this out?
// #if !ALIB_GTEST
// #if defined(_MSC_VER)
// #pragma warning( push )
// #pragma warning( disable:4505 ) // local variable is initialized but not referenced
// #endif
// namespace Microsoft {  namespace VisualStudio {   namespace CppUnitTestFramework     {
//             template<> std::wstring ToString<int64_t>(const int64_t& t)
//             {
//                 wstringstream convert;
//                 convert << t;
//                 return convert.str();
//             }
// }}}
// #if defined(_MSC_VER)
// #pragma warning( pop )
// #endif
// #endif
