// #################################################################################################
//  aworx - Unit Tests
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#include "alib/alox.hpp"
#include "alib/strings/substring.hpp"
#include "alib/system/environment.hpp"

#include "unittests/aworx_unittests.hpp"
#include <limits>

#if defined(__clang__)
    #pragma message "Clang Compiler (not a warning, just for information)"
#elif defined(__GNUC__)
    #pragma message "GNU Compiler (not a warning, just for information)"
#elif defined(_MSC_VER)
    #pragma message ("MS Compiler (not a warning, just for information)")
#endif

using namespace std;
using namespace aworx;
using namespace aworx::lib::lox::detail;

namespace ut_aworx {


// #################################################################################################
// Fields
// #################################################################################################
String128 AWorxUnitTesting::LastAutoSizes;
NAString  AWorxUnitTesting::GeneratedSamplesDir;
String    AWorxUnitTesting::GeneratedSamplesSearchDir= A_CHAR("docs/pages");
AString   AWorxUnitTesting::CustomMetaInfoFormat;


// #################################################################################################
// Constructors/destructor
// #################################################################################################
AWorxUnitTesting::AWorxUnitTesting( const NCString& pdomain, const NCString& testName)
: initializer(ALIB.Init())
, lox( "UTLox", false )
{
    this->Domain.Reset( pdomain );
    this->Domain.ToUpper();
    this->ActTestName=  testName;

    Log_SetSourcePathTrimRule( "*/src/", Inclusion::Include );

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

            Directory moduleName( Directory::SpecialFolder::Module );
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
            if ( NString(*ALIB.ArgVN).IndexOf<true, Case::Ignore>( "CLion", 0) >= 0 )
                dynamic_cast<AnsiConsoleLogger*>(utl)->UseLightColors= AnsiLogger::LightColorUsage::Never;
        }
    #endif

    if ( CustomMetaInfoFormat.IsNotEmpty() )
        utl->MetaInfo->Format= CustomMetaInfoFormat;

    if ( LastAutoSizes.IsNotEmpty() )
        utl->AutoSizes.Import( LastAutoSizes, CurrentData::Keep );

    lox.Acquire(ALIB_CALLER);
        lox.SetVerbosity( utl, Verbosity::Verbose, "/" );
        lox.SetVerbosity( utl, Verbosity::Verbose, Domain);
        lox.SetVerbosity( utl, Verbosity::Warning, ALox::InternalDomains );
        lox.SetVerbosity( utl, Verbosity::Info,    NString64() << ALox::InternalDomains << "UT_REPORT" );
    lox.Release();

    lib::results::Report::GetDefault().PushWriter( this );
}

AWorxUnitTesting::~AWorxUnitTesting()
{
    lox.Acquire(ALIB_CALLER);
        lox.RemoveLogger( utl );
    lox.Release();

    LastAutoSizes.Reset();
    utl->AutoSizes.Export( LastAutoSizes );

    // clean debug lox and ALox
    Log_Prune( lib::ALOX.Reset() );

//    ALIB.Config->GetPluginTypeSafe<aworx::InMemoryPlugin>( Priorities::DefaultValues   )->Reset();
//    ALIB.Config->GetPluginTypeSafe<aworx::InMemoryPlugin>( Priorities::ProtectedValues )->Reset();
//    if( lib::ALOX.Config != ALIB.Config )
//    {
//        lib::ALOX.Config->GetPluginTypeSafe<aworx::InMemoryPlugin>( Priorities::DefaultValues   )->Reset();
//        lib::ALOX.Config->GetPluginTypeSafe<aworx::InMemoryPlugin>( Priorities::ProtectedValues )->Reset();
//    }

    // check if ALib SmartLock for std I/O was released properly
    this->EQ (__FILE__, __LINE__, 0, SmartLock::StdOutputStreams.CntAcquirers() );
    this->EQ (__FILE__, __LINE__, 0, SmartLock::StdOutputStreams.CountAcquirements() );

    lib::results::Report::GetDefault().PopWriter( this );
    delete utl;
}


// #################################################################################################
// Print
// #################################################################################################
void AWorxUnitTesting::print (  const aworx::NCString& file, int line, aworx::Verbosity verbosity, aworx::Boxes& args )
{
    if( args.size() == 1 && args.back().IsType<aworx::Exception*>() )
    {
        aworx::LogTools::Exception( lox, *args.back().Unbox<aworx::Exception*>()  );
        return;
    }

    lox.Acquire(file, line, ActTestName);
    lox.GetLogableContainer().Add( args );
    lox.Entry( Domain, verbosity );
    lox.Release();
}


void AWorxUnitTesting::Failed( const NCString& file, int line, const Box& exp, const Box& given )
{
    Print( file, line, aworx::Verbosity::Error, "UT Failure: Expected: \"{!ESC}\"\\n"
                                                "               given: \"{!ESC}\"", exp, given );
    assert(!AssertOnFailure);
}

void AWorxUnitTesting::writeResultFile(const NString& name, aworx::AString& output, const NString& doxyTag )
{
    integer start= 0;
    for(;;)
    {
        start= output.IndexOf( A_CHAR("/home"), start );
        if ( start < 0 )
            break;
        integer end= output.IndexOf( A_CHAR("/ALib"), start );
        ALIB_ASSERT_ERROR( end >0 && end - start < 25, "Error pruning home directory" )
        if (end < 0 )
            end= output.Length();
        output.Delete( start + 5, end - start - 5 );
        start++;
    }

    // if invoked the first time, search the right directory
    if ( GeneratedSamplesDir.IsNull() )
    {
        GeneratedSamplesDir.Reset( "" );
        String128 testDir; testDir << "../" << GeneratedSamplesSearchDir;
        for( int depth= 0; depth < 10 ; depth++ )
        {
            if ( Directory::Exists( testDir ) )
            {
                GeneratedSamplesDir << testDir << "/generated/";
                GeneratedSamplesDir.SearchAndReplace( "//", "/" );
#if ALIB_CHARACTERS_ARE_NARROW
                Directory::Create( GeneratedSamplesDir );
#else
                Directory::Create( String512(GeneratedSamplesDir) );
#endif
                break;
            }
            testDir.InsertAt( A_CHAR("../"), 0 );
        }
    }

    if ( GeneratedSamplesDir.IsNotNull() && GeneratedSamplesDir.IsEmpty() )
    {
        ALIB_ERROR( "Samples directory \"/generated\" not found in upward path search" )
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
}

// #################################################################################################
// Error/Warning
// #################################################################################################
void AWorxUnitTesting::Report  ( const lib::results::Message& msg )
{
        lox.Acquire(msg.File, msg.Line, msg.Func);
            lox.GetLogableContainer().Add( msg.Args );
            lox.Entry( NString16() << ALox::InternalDomains << "UT_REPORT",
                       msg.Type == aworx::lib::results::Report::Types::Error   ? Verbosity::Error    :
                       msg.Type == aworx::lib::results::Report::Types::Warning ? Verbosity::Warning  :
                       msg.Type == aworx::lib::results::Report::Types::Message ? Verbosity::Info     :
                                                                               Verbosity::Verbose    );

        lox.Release();
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


#if !ALIB_GTEST
namespace Microsoft {  namespace VisualStudio {   namespace CppUnitTestFramework     {
            template<> std::wstring ToString<int64_t>(const int64_t& t)
            {
                wstringstream convert;
                convert << t;
                return convert.str();
            }
}}}
#endif
