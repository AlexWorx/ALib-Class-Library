// #################################################################################################
//  Unit Tests - ALox Logging Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"
#include "alib/alox.hpp"
#include "alib/lang/basecamp/basecamp.hpp"
#include "alib/lang/basecamp/bootstrap.hpp"
#include "alib/compatibility/std_boxing.hpp"
#include "alib/boxing/dbgboxing.hpp"

//! [DOX_ALIB_RESOURCES_DEBUG_BOOTSTRAP1]
#if ALIB_DEBUG_RESOURCES
#   include "alib/lang/resources/localresourcepool.hpp"
#endif
//! [DOX_ALIB_RESOURCES_DEBUG_BOOTSTRAP1]

#if ALIB_DEBUG_MONOMEM
#   include "alib/singletons/dbgsingletons.hpp"
#   include "alib/monomem/hashtable.hpp"
#endif

#if ALIB_CAMP
#   include "alib/lang/system/directory.hpp"
#endif

#if ALIB_ALOX
#   include "alib/alox/logtools.hpp"
#endif

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

//! [DOX_ALIB_RESOURCES_DEBUG_BOOTSTRAP2]
int main( int argc, char **argv )
{
    ArgC=   argc;
    ArgVN=  const_cast<const char**>(argv);

    #if ALIB_DEBUG_RESOURCES
        resources::LocalResourcePool::DbgResourceLoadObserver= &std::cout;
    #endif

    Bootstrap();
    //...
    //...
//! [DOX_ALIB_RESOURCES_DEBUG_BOOTSTRAP2]

//! [DOX_ALIB_COMPATIBILITY_BOOTSTRAP]
ALIB_IF_THREADS( monomem::GlobalAllocatorLock.Acquire(ALIB_CALLER_PRUNED); )
alib::boxing::compatibility::std::BootstrapStdStringBoxing();
ALIB_IF_THREADS( monomem::GlobalAllocatorLock.Release(); )
//! [DOX_ALIB_COMPATIBILITY_BOOTSTRAP]


    ::testing::InitGoogleTest( &argc, argv);

    std::cout << "Unit Tests compiled with : ";
        #if defined( _MSC_VER )
            std::cout << "MSC " << _MSC_VER;
        #elif defined(__clang__)
            std::cout << "Clang" << __GNUC__ << '.' + __GNUC_MINOR__ << '.' << __GNUC_PATCHLEVEL__;
        #elif defined(__GNUC__)
            std::cout << "GCC V" << __GNUC__ << '.' + __GNUC_MINOR__ << '.' << __GNUC_PATCHLEVEL__;
        #elif
            std::cout << "Unknown Compiler" << ALIB_CPP_STANDARD;
        #endif

        std::cout << ", CPP V."     << ALIB_CPP_STANDARD;


        std::cout << ", Platform \"";

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

    std::cout << "\"." << std::endl;


    //alib::lang::Report::GetDefault().HaltOnWarning= true;

//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_*";

//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_Lib.Bits*";

//    ::testing::GTEST_FLAG(filter) = "CPP_Strings_AString*:CPP_Ticks*:CPP_Thread*:CPP_ALox*:CPP_Tutorial*";

//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_Strings*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_Strings_AString.*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_Strings_AString.XAString";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_Strings_AString.Count";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_Strings_AString.Revert";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_Strings_AString.Append";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_Strings_Substring.*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_Strings_Format.*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_Strings_Format.ConvertFloats";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_Strings_Format.FormatterJavaStyle";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_Strings_Format.FormatterPythonStyle";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_Strings_Format.ConvertIntegers";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_Strings_Util.*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_Strings_Util.Text*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_Strings_Util.SubstringSearch";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_Strings_Util.Token*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_Strings_Custom*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_Dox_Strings.*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_Dox_Strings.PropertyFormatter";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_Dox_Strings.PropertyFormatters";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_Dox_Format.*";

//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_Dox_Enums.*";

//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_Time*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_Time.CalendarDate_Time";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_Monomem*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_Monomem.List*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_Monomem.HashTable*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_Monomem_StringTree*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_BitBuffer.*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_BitBuffer.Bit*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_BitBuffer.AC_16";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_BitBuffer.AC_16_LargerArrays";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_BitBuffer.ArrayCompressor";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_BitBuffer.AC_Huffman*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_BitBuffer.AC_SignedAndMinimumSizes*";

//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_Threads*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_Threads.ThreadSimple";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_Threads.SmartLock";

//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_System.Dir*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_Containers.*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_Config.*";

//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_Compatibility.*";

//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_Boxing*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_Boxing_DOX.*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_Boxing_DOX.BoxingDerivedALibStrings*";

//    ::testing::GTEST_FLAG(filter) = "CPP_ALox*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALox_Logger.*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALox_Logger.Log_Replacements";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALox_Logger.Log_Multiline*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALox_Logger.Log_Recursive*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALox_Logger.Log_ColorsAndStyles*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALox_Logger.Log_TextLogger_RegisterStreamLocks*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALox_Logger.Log_TextLogger_ObjectConverter*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALox_Logger.Log_TextLogger_FormatConfig*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALox_Lox*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALox_Lox.Log_MultipleLogables";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALox_Lox.Log_DumpStateOnExit";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALox_Lox.Log_WriteVerbosities";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALox_Lox.Log_TestThreads*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALox_Lox.Log_SetSourcePathTrimRuleTest";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALox_Lox.Log_ScopeInfoCacheTest";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALox_Lox.Log_GetState*";

//    ::testing::GTEST_FLAG(filter) = "CPP_ALox_Lox_Release.*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALox_Lox_Release.Lox_TestAssert";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALox_Lox_Release.Lox_LoggerInTwoLoxes*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALox_Lox_Release.Lox_TestVerbositySetting*";

//    ::testing::GTEST_FLAG(filter) = "CPP_ALox_Log_Domains.*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALox_Log_Domains.Log_DomainSubstitutions*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALox_Log_Domains.Log_DomainSubstitutions_IniFile*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALox_Log_Domains.Log_Domain_IniFile*";

//    ::testing::GTEST_FLAG(filter) = "CPP_ALox_Log_Scopes.*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALox_Log_Scopes.Log_ScopeDomains*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALox_Log_Scopes.Log_Once*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALox_Log_Scopes.Log_Store_Test*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALox_Log_Scopes.Log_Prefix*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALox_Log_Scopes.Log_LineFormat*";

//    ::testing::GTEST_FLAG(filter) = "CPP_Dox_Tutorial*";
//    ::testing::GTEST_FLAG(filter) = "CPP_Dox_Tutorial.ALoxTut_LogState";

//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_*Expr*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_Expr*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_Expr.*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_Expr.Conditional";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_Expr.TestNormalization";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_Expr.Nested";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_Expr.NumberLiterals";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_Expr.Exceptions";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_Expr.ProgramListing";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_Expr_Builtin.*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_Expr_Builtin.Strings";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_Expr_Builtin.DateTime";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_Expr_Builtin.TokenConsistency";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_Dox_Expr_Calculator.*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_Dox_Expr_Tutorial.FileSystemIntro";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_Dox_Expr_Tutorial.Nested";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_Dox_Expr_Tutorial.VMListings";

//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_Dox_Files.filesFexFilter";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_Files.Basics";

//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_Dox_CLI*";

    int result= RUN_ALL_TESTS();

    #if ALIB_CAMP && ALIB_DEBUG_RESOURCES
//! [DOX_ALIB_RESOURCES_DEBUG_SHUTDOWN]
#if ALIB_DEBUG_RESOURCES && ALIB_ALOX
    Log_Info( "---------------- Resource Pool Dump ----------------" )
        auto categoryList= alib::BASECAMP.GetResourcePool().DbgGetCategories();
        integer sum= 0;
        for( auto& category : categoryList )
        {
            Log_Info( "Resource category {:10} has {:3} entries", category.first, category.second )
            sum+= category.second;
        }
        Log_Info( "This sums up to ", sum, " resource definitions"  )

        auto resourceList= alib::BASECAMP.GetResourcePool().DbgGetList();
        Log_Info( ResourcePool::DbgDump( resourceList ) )
    Log_Info( "---------------- Resource Pool Dump (end) ----------" )
#endif
//! [DOX_ALIB_RESOURCES_DEBUG_SHUTDOWN]
    #endif

    #if ALIB_FEAT_SINGLETON_MAPPED && ALIB_DEBUG_MONOMEM
        cout << endl;
        cout << "---------------- Mapped Singletons ----------------" << endl;
            AString singletonList;
            alib::singletons::DbgGetSingletons(singletonList);
            cout << singletonList << endl;
            AString hashTableStats=
                    alib::monomem::DbgDumpDistribution( alib::singletons::DbgGetSingletons(), true );
            cout << hashTableStats << endl;

        cout << "---------------- Mapped Singetons (end) ----------" << endl;
    #endif

    #if ALIB_DEBUG_BOXING && defined(SUPPRESS_THIS)
        cout << endl;
        cout << "---------------- Dynamic VTables (should be free of ALib types!) ----------" << endl;
        std::cout << alib::DbgBoxing::DumpVTables(false);

        cout << "---------------- Dynamic VTables (end)----------------" << endl;
    #endif


    alib::Shutdown();

    #if !ALIB_DEBUG
        cerr << "\n  *** Note: To generate the documentation samples, unit test have to be run in debug mode." << endl;
    #elif !ALIB_CLI || !ALIB_ALOX || !ALIB_EXPRESSIONS || !ALIB_ALOX || !ALIB_THREADS || !ALIB_BITBUFFER
        cerr << "\n  *** Note: To generate the documentation samples, all ALib modules have to be enabled." << endl;
    #elif !ALIB_DEBUG_BOXING
        cerr << "\n  *** Note: To generate the documentation samples, CMake flag ALIB_DEBUG_BOXING has to be true for compilation." << endl;
    #else
        if(!Directory::Exists(A_CHAR("/tmp/_builds_/ALib_Samples/cli_clion_debug")))
            cerr << "\n  *** Note: To generate the documentation samples, ALib sample CLI has to be compiled to /tmp/_builds_/ALib_Samples/cli_clion_debug." << endl;
        else
            cout << "*** Note: Duly compiled to generate documentation after running these tests." << endl;
    #endif

    return result;
}
