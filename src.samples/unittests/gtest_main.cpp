// #################################################################################################
//  Unit Tests - ALox Logging Library
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib_precompile.hpp"
#include "ALib.Lang.H"
#include "ALib.Singletons.H"
#include "ALib.ALox.H"
#include "ALib.Camp.Base.H"
#include "ALib.Compatibility.StdBoxtraits.H"
#include "ALib.Strings.StdIOStream.H"
#include "ALib.Format.H"

DOX_MARKER( [DOX_RESOURCES_DEBUG_BOOTSTRAP1])
#include "ALib.Resources.H"
#include "ALib.Bootstrap.H"
DOX_MARKER( [DOX_RESOURCES_DEBUG_BOOTSTRAP1])


#include "ALib.Monomem.H"

#if ALIB_CAMP
#   include "ALib.System.H"
#endif

#include "ALib.ALox.H"

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

using namespace std;
using namespace alib;



DOX_MARKER( [DOX_RESOURCES_DEBUG_BOOTSTRAP2])
#include "ALib.Lang.CIFunctions.H"

int main( int argc, const char **argv )
{
    ARG_C=   argc;
    ARG_VN=  argv;

    #if !defined(_WIN32)
        std::setlocale(LC_ALL, "en_US.UTF-8");             // C locale
        std::locale::global(std::locale("en_US.UTF-8"));   // C++ locale
        std::wcout.imbue(std::locale());
    #endif

    #if ALIB_DEBUG_RESOURCES
        resources::LocalResourcePool::DbgResourceLoadObserver= &std::cout;
    #endif

    Bootstrap();
    //...
    //...
DOX_MARKER( [DOX_RESOURCES_DEBUG_BOOTSTRAP2])

DOX_MARKER( [DOX_COMPATIBILITY_BOOTSTRAP])
{
    ALIB_LOCK_RECURSIVE_WITH( monomem::GLOBAL_ALLOCATOR_LOCK )
    alib::boxing::compatibility::std::BootstrapStdStringBoxing();
}    
DOX_MARKER( [DOX_COMPATIBILITY_BOOTSTRAP])


    ::testing::InitGoogleTest( &argc, const_cast<char**>(argv));

    std::cout << "Unit Tests compiled with: " << std::endl;
        #if defined( _MSC_VER )
            std::cout << "  MSVC Version:   " << _MSC_VER;
        #elif defined(__clang__)

            std::cout << "  Clang Version:  "  << __GNUC__ << '.' + __GNUC_MINOR__ << '.' << __GNUC_PATCHLEVEL__;
        #elif defined(__GNUC__)
            std::cout << "  GCC Version:    " << __GNUC__ << '.' + __GNUC_MINOR__ << '.' << __GNUC_PATCHLEVEL__;
        #elif
            std::cout << "  Unknown Compiler" << ALIB_CPP_STANDARD;
        #endif
        std::cout << std::endl;

        #if defined(_LIBCPP_VERSION)
            std::cout << "  CPP Library:    libc++ (LLVM)";
        #elif defined(__GLIBCXX__)
            std::cout << "  CPP Library:    libstdc++ (GNU)";
        #else
            std::cout << "  CPP Library:      Unknown";
        #endif
        std::cout << std::endl;

        std::cout << "  CPP Standard:   " << ALIB_CPP_STANDARD  << std::endl;
        std::cout << "  CPP 20 Modules: " << (ALIB_C20_MODULES ? "Yes" : "No")  << std::endl;

        std::cout << "  Platform:       ";
        #if defined(__x86_64__) || defined(_M_X64)
            std::cout << "x86_64";
        #elif defined(i386) || defined(__i386__) || defined(__i386) || defined(_M_IX86)
            std::cout << "x86_32";
        #elif defined(__ARM_ARCH_2__)
            std::cout << "ARM2";
        #elif defined(__ARM_ARCH_3__) || defined(__ARM_ARCH_3M__)
            std::cout << "ARM3";
        #elif defined(__ARM_ARCH_4T__) || defined(__TARGET_ARM_4T)
            std::cout << "ARM4T";
        #elif defined(__ARM_ARCH_5_) || defined(__ARM_ARCH_5E_)
            std::cout << "ARM5"
        #elif defined(__ARM_ARCH_6T2_) || defined(__ARM_ARCH_6T2_)
            std::cout << "ARM6T2";
        #elif defined(__ARM_ARCH_6__) || defined(__ARM_ARCH_6J__) || defined(__ARM_ARCH_6K__) || defined(__ARM_ARCH_6Z__) || defined(__ARM_ARCH_6ZK__)
            std::cout << "ARM6";
        #elif defined(__ARM_ARCH_7__) || defined(__ARM_ARCH_7A__) || defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7S__)
            std::cout << "ARM7";
        #elif defined(__ARM_ARCH_7A__) || defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7S__)
            std::cout << "ARM7A";
        #elif defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7S__)
            std::cout << "ARM7R";
        #elif defined(__ARM_ARCH_7M__)
            std::cout << "ARM7M";
        #elif defined(__ARM_ARCH_7S__)
            std::cout << "ARM7S";
        #elif defined(__aarch64__) || defined(_M_ARM64)
            std::cout << "ARM64";
        #elif defined(mips) || defined(__mips__) || defined(__mips)
            std::cout << "MIPS";
        #elif defined(__sh__)
            std::cout << "SUPERH";
        #elif defined(__powerpc) || defined(__powerpc__) || defined(__powerpc64__) || defined(__POWERPC__) || defined(__ppc__) || defined(__PPC__) || defined(_ARCH_PPC)
            std::cout << "POWERPC";
        #elif defined(__PPC64__) || defined(__ppc64__) || defined(_ARCH_PPC64)
            std::cout << "POWERPC64";
        #elif defined(__sparc__) || defined(__sparc)
            std::cout << "SPARC";
        #elif defined(__m68k__)
            std::cout << "M68K";
        #else
            std::cout << "UNKNOWN";
        #endif
        std::cout << std::endl;

    
//    ::testing::GTEST_FLAG(filter) = "UT_*";

//    ::testing::GTEST_FLAG(filter) = "UT_Lang.*";
//    ::testing::GTEST_FLAG(filter) = "UT_Lang.Bits*";
//    ::testing::GTEST_FLAG(filter) = "UT_Lang.LangMacros*";
//    ::testing::GTEST_FLAG(filter) = "UT_Dox_Enums.lang_IntXX*";

//    ::testing::GTEST_FLAG(filter) = "UT_Strings*";
//    ::testing::GTEST_FLAG(filter) = "UT_Strings_AString.*";
//    ::testing::GTEST_FLAG(filter) = "UT_Strings_AString.XAString";
//    ::testing::GTEST_FLAG(filter) = "UT_Strings_AString.Count";
//    ::testing::GTEST_FLAG(filter) = "UT_Strings_AString.Revert";
//    ::testing::GTEST_FLAG(filter) = "UT_Strings_AString.Append";
//    ::testing::GTEST_FLAG(filter) = "UT_Strings_AString.CapacityLength";
//    ::testing::GTEST_FLAG(filter) = "UT_Strings_Substring.*";
//    ::testing::GTEST_FLAG(filter) = "UT_Strings_Format.*";
//    ::testing::GTEST_FLAG(filter) = "UT_Strings_Format.ConvertFloats";
//    ::testing::GTEST_FLAG(filter) = "UT_Strings_Format.TestFormatterJavaStyle";
//    ::testing::GTEST_FLAG(filter) = "UT_Strings_Format.FormatterPythonStyle";
//    ::testing::GTEST_FLAG(filter) = "UT_Strings_Format.ConvertIntegers";
//    ::testing::GTEST_FLAG(filter) = "UT_Strings_Util.*";
//    ::testing::GTEST_FLAG(filter) = "UT_Strings_Util.OStreamWriterNLCorrection";
//    ::testing::GTEST_FLAG(filter) = "UT_Strings_Util.Text*";
//    ::testing::GTEST_FLAG(filter) = "UT_Strings_Util.StringSearch";
//    ::testing::GTEST_FLAG(filter) = "UT_Strings_Util.Token*";
//    ::testing::GTEST_FLAG(filter) = "UT_Strings_Util.OStreamWriterNLCorrection*";
//    ::testing::GTEST_FLAG(filter) = "UT_Strings_Custom*";
//    ::testing::GTEST_FLAG(filter) = "UT_Dox_Strings.*";
//    ::testing::GTEST_FLAG(filter) = "UT_Dox_Strings.PropertyFormatter";
//    ::testing::GTEST_FLAG(filter) = "UT_Dox_Strings.PropertyFormatters";
//    ::testing::GTEST_FLAG(filter) = "UT_Dox_Format.*";

//    ::testing::GTEST_FLAG(filter) = "UT_Dox_Enums.*";

//    ::testing::GTEST_FLAG(filter) = "UT_Time*";
//    ::testing::GTEST_FLAG(filter) = "UT_Time.Basics";
//    ::testing::GTEST_FLAG(filter) = "UT_Time.CalendarDate_Time";

//    ::testing::GTEST_FLAG(filter) = "UT_ContMono*";
//    ::testing::GTEST_FLAG(filter) = "UT_ContMono.AString_Ma_AStringPA";
//    ::testing::GTEST_FLAG(filter) = "UT_ContMono.LocalAllocator";
//    ::testing::GTEST_FLAG(filter) = "UT_ContMonoList.*";
//    ::testing::GTEST_FLAG(filter) = "UT_ContMonoList.TestList";
//    ::testing::GTEST_FLAG(filter) = "UT_ContMono.HashTable*";
//    ::testing::GTEST_FLAG(filter) = "UT_ContMonoHashtable.HashTableRecycling";
//    ::testing::GTEST_FLAG(filter) = "UT_ContMono_StringTree.*";
//    ::testing::GTEST_FLAG(filter) = "UT_ContMonoLRUCache*";
//    ::testing::GTEST_FLAG(filter) = "UT_ContMonoHashtable*";

//    ::testing::GTEST_FLAG(filter) = "UT_BitBuffer.*";
//    ::testing::GTEST_FLAG(filter) = "UT_BitBuffer.Bit*";
//    ::testing::GTEST_FLAG(filter) = "UT_BitBuffer.AC_16";
//    ::testing::GTEST_FLAG(filter) = "UT_BitBuffer.AC_16_LargerArrays";
//    ::testing::GTEST_FLAG(filter) = "UT_BitBuffer.ArrayCompressor";
//    ::testing::GTEST_FLAG(filter) = "UT_BitBuffer.AC_Huffman*";
//    ::testing::GTEST_FLAG(filter) = "UT_BitBuffer.AC_SignedAndMinimumSizes*";

//    ::testing::GTEST_FLAG(filter) = "UT_Threads*";
//    ::testing::GTEST_FLAG(filter) = "UT_Threads.Locks";
//    ::testing::GTEST_FLAG(filter) = "UT_Threads.LockSpeedTest";
//    ::testing::GTEST_FLAG(filter) = "UT_Threads.ThreadSimple";
//    ::testing::GTEST_FLAG(filter) = "UT_Threads.TRequest";

//    ::testing::GTEST_FLAG(filter) = "UT_ThreadModel*";

//    ::testing::GTEST_FLAG(filter) = "UT_System.Path*";
//    ::testing::GTEST_FLAG(filter) = "UT_System.ShellCommand*";
//    ::testing::GTEST_FLAG(filter) = "UT_Containers.*";
//    ::testing::GTEST_FLAG(filter) = "UT_Variables.*";
//    ::testing::GTEST_FLAG(filter) = "UT_Variables.Basics";
//    ::testing::GTEST_FLAG(filter) = "UT_Variables.ConfigIniFiles";
//    ::testing::GTEST_FLAG(filter) = "UT_Variables.ConfigSubstitution";

//    ::testing::GTEST_FLAG(filter) = "UT_Compatibility.*";

//    ::testing::GTEST_FLAG(filter) = "UT_Boxing*";
//    ::testing::GTEST_FLAG(filter) = "UT_Boxing.Boxing_Boxes";
//    ::testing::GTEST_FLAG(filter) = "UT_Boxing_DOX.*";
//    ::testing::GTEST_FLAG(filter) = "UT_Boxing_DOX.BoxingDerivedALibStrings*";

//    ::testing::GTEST_FLAG(filter) = "CPP_Dox_Tutorial.*";
//    ::testing::GTEST_FLAG(filter) = "CPP_Dox_Tutorial.ALox_Manual";
//    ::testing::GTEST_FLAG(filter) = "CPP_Dox_Tutorial.Hello_ALox";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALox*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALox_Logger.*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALox_Logger.Log_Replacements";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALox_Logger.Log_Multiline*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALox_Logger.Log_Recursive*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALox_Logger.Log_ColorsAndStyles*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALox_Logger.Log_TextLogger_RegisterStdStreamLocks*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALox_Logger.Log_TextLogger_ObjectConverter*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALox_Logger.Log_TextLogger_FormatConfig*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALox_Logger.Log_TextLogger_TimeDiff";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALox_Lox*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALox_Lox.Log_*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALox_Lox.Log_MultipleLogables";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALox_Lox.Log_DumpStateOnExit";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALox_Lox.Log_WriteVerbosities";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALox_Lox.Log_TestThreads*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALox_Lox.Log_SetSourcePathTrimRuleTest";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALox_Lox.Log_ScopeInfoCacheTest";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALox_Lox.Log_GetState*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALox_Lox.Log_SimpleScopeDomain";

//    ::testing::GTEST_FLAG(filter) = "CPP_ALox_Lox_Release.*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALox_Lox_Release.Lox_TestAssert";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALox_Lox_Release.Lox_LoggerInTwoLoxes*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALox_Lox_Release.Lox_TestVerbositySetting*";

//    ::testing::GTEST_FLAG(filter) = "CPP_ALox_Log_Domains.*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALox_Log_Domains.Log_DomainSubstitutions*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALox_Log_Domains.Log_DomainSubstitutions_IniFile*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALox_Log_Domains.Log_Domain_IniFile*";

//    ::testing::GTEST_FLAG(filter) = "CPP_ALox_Log_Scopes.*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALox_Log_Scopes.Lox_ScopeDomains*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALox_Log_Scopes.Log_ScopeDomains*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALox_Log_Scopes.Log_Once*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALox_Log_Scopes.Log_Store_Test*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALox_Log_Scopes.Log_Prefix*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALox_Log_Scopes.Log_LineFormat*";

//    ::testing::GTEST_FLAG(filter) = "CPP_Dox_Tutorial*";
//    ::testing::GTEST_FLAG(filter) = "CPP_Dox_Tutorial.ALoxTut_LogState";

//    ::testing::GTEST_FLAG(filter) = "UT_*Expr*";
//    ::testing::GTEST_FLAG(filter) = "UT_Expr*";
//    ::testing::GTEST_FLAG(filter) = "UT_Expr.MultiThreaded";
//    ::testing::GTEST_FLAG(filter) = "UT_Expr.Conditional";
//    ::testing::GTEST_FLAG(filter) = "UT_Expr.TestNormalization";
//    ::testing::GTEST_FLAG(filter) = "UT_Expr.Nested";
//    ::testing::GTEST_FLAG(filter) = "UT_Expr.NumberLiterals";
//    ::testing::GTEST_FLAG(filter) = "UT_Expr.Exceptions";
//    ::testing::GTEST_FLAG(filter) = "UT_Expr.ProgramListing";
//    ::testing::GTEST_FLAG(filter) = "UT_Expr_Builtin.*";
//    ::testing::GTEST_FLAG(filter) = "UT_Expr_Builtin.Math";
//    ::testing::GTEST_FLAG(filter) = "UT_Expr_Builtin.Strings";
//    ::testing::GTEST_FLAG(filter) = "UT_Expr_Builtin.DateTime";
//    ::testing::GTEST_FLAG(filter) = "UT_Expr_Builtin.TokenConsistency";
//    ::testing::GTEST_FLAG(filter) = "UT_Dox_Expr_Calculator.*";
//    ::testing::GTEST_FLAG(filter) = "UT_Dox_Expr_Tutorial.FileSystemIntro";
//    ::testing::GTEST_FLAG(filter) = "UT_Dox_Expr_Tutorial.Nested";
//    ::testing::GTEST_FLAG(filter) = "UT_Dox_Expr_Tutorial.VMListings";

//    ::testing::GTEST_FLAG(filter) = "UT_Files.*";
//    ::testing::GTEST_FLAG(filter) = "UT_Files.Scanning";
//    ::testing::GTEST_FLAG(filter) = "UT_Files.FileAndFTree";
//    ::testing::GTEST_FLAG(filter) = "UT_Dox_Files.filesFexFilter";

//    ::testing::GTEST_FLAG(filter) = "UT_Dox_CLI*";

    int result= RUN_ALL_TESTS();

    #if ALIB_CAMP && ALIB_DEBUG_RESOURCES
DOX_MARKER( [DOX_RESOURCES_DEBUG_SHUTDOWN])
#if ALIB_DEBUG_RESOURCES && ALIB_ALOX
    Log_Info( "---------------- Resource Pool Dump ----------------" )
        auto categoryList= alib::BASECAMP.GetResourcePool()->DbgGetCategories();
        integer sum= 0;
        for( auto& category : categoryList )
        {
            Log_Info( "Resource category {:10} has {:3} entries", category.first, category.second )
            sum+= category.second;
        }
        Log_Info( "This sums up to ", sum, " resource definitions"  )

        auto resourceList= alib::BASECAMP.GetResourcePool()->DbgGetList();
        Log_Info( resources::DbgDump( resourceList ) )
    Log_Info( "---------------- Resource Pool Dump (end) ----------" )
#endif
DOX_MARKER( [DOX_RESOURCES_DEBUG_SHUTDOWN])
    #endif

    #if ALIB_FEAT_SINGLETON_MAPPED && ALIB_DEBUG_CONTAINERS
        cout << endl;
        cout << "---------------- Mapped Singletons ----------------" << endl;
            NAString singletonList;
            alib::singletons::DbgGetSingletons(singletonList);
            cout << singletonList << endl;
            AString hashTableStats=
                    alib::containers::DbgDumpDistribution( alib::singletons::DbgGetSingletons(), true );
            cout << hashTableStats << endl;

        cout << "---------------- Mapped Singetons (end) ----------" << endl;
    #endif

    #if ALIB_DEBUG_BOXING && defined(SUPPRESS_THIS)
        cout << endl;
        cout << "---------------- Dynamic VTables (should be free of ALib types!) ----------" << endl;
        std::cout << boxing::debug::DumpVTables(false);

        cout << "---------------- Dynamic VTables (end)----------------" << endl;
    #endif


    alib::Shutdown();

    #if !ALIB_DEBUG
        cout << "\n*** Note: To generate the documentation samples, unit test have to be run in debug mode." << endl;
    #elif !ALIB_CLI || !ALIB_ALOX || !ALIB_EXPRESSIONS || !ALIB_ALOX || ALIB_SINGLE_THREADED || !ALIB_BITBUFFER
        cout << "\n*** Note: To generate the documentation samples, all ALib modules have to be enabled." << endl;
    #elif !ALIB_DEBUG_BOXING
        cout << "\n*** Note: To generate the documentation samples, CMake flag ALIB_DEBUG_BOXING has to be true for compilation." << endl;
    #else
        if(!Path(A_CHAR("/tmp/_builds_/ALib_Samples/cli_clion_debug")).IsDirectory())
            cout << "\n*** Note: To generate the documentation samples, ALib sample CLI has to be compiled to /tmp/_builds_/ALib_Samples/cli_clion_debug." << endl;
        else
            cout << "\n*** Note: Duly compiled to generate documentation after running these tests." << endl;
    #endif

    return result;
}
