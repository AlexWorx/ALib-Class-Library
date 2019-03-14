// #################################################################################################
//  Unit Tests - ALox Logging Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boođst Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"
#include "alib/alox.hpp"

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
    #pragma clang diagnostic pop  đ
#endif


#include "unittests/aworx_unittests.hpp"


using namespace std;
using namespace aworx;

int main( int argc, char **argv )
{
    ::testing::InitGoogleTest( &argc, argv);
    aworx::ALIB.Init( argc, argv );
    aworx::ALIB.CheckDistribution();

    //aworx::lib::results::Report::GetDefault().HaltOnWarning= true;

//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_*";

//    ::testing::GTEST_FLAG(filter) = "CPP_Strings_AString*:CPP_Ticks*:CPP_Thread*:CPP_ALox*:CPP_Tutorial*";

//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_Strings*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_Strings_AString.*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_Strings_AString.XAString";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_Strings_AString.Count";
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
//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_util_StringTree*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_Dox_Strings.*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_Dox_Strings.PropertyFormatter";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_Dox_Strings.PropertyFormatters";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_Dox_Stringformat.*";

//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_Dox_Enums.*";

//    ::testing::GTEST_FLAG(filter) = "CPP_ALib_Time*";

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

//    ::testing::GTEST_FLAG(filter) = "CPP_ALox_Log_Domains.*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALox_Log_Domains.Log_DomainSubstitutions*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALox_Log_Domains.Log_DomainSubstitutions_IniFile*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALox_Log_Domains.Log_Domain_IniFile*";

//    ::testing::GTEST_FLAG(filter) = "CPP_ALox_Log_Scopes.*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALox_Log_Scopes.Log_ScopeDomains*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALox_Log_Scopes.Log_Once*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALox_Log_Scopes.Log_Store_Test*";
//    ::testing::GTEST_FLAG(filter) = "CPP_ALox_Log_Scopes.Log_Prefix*";

//    ::testing::GTEST_FLAG(filter) = "CPP_Dox_Tutorial*";
//    ::testing::GTEST_FLAG(filter) = "CPP_Dox_Tutorial.Tut_LogState";

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

    auto result= RUN_ALL_TESTS();

    ALIB.TerminationCleanUp();

    return result;
}



