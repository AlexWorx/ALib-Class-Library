// #################################################################################################
//  AWorx ALib Unit Tests
//  Documentation sample for module ALib CLI.
//
//  Copyright 2013-2023 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"
#include "unittests/alib_test_selection.hpp"

#if ALIB_UT_DOCS && ALIB_CLI && ALIB_SYSTEM  && !defined(_WIN32)

// the namespace of our software

#define TESTCLASSNAME       CPP_ALib_Dox_CLI
#include "unittests/aworx_unittests.hpp"

#include "alib/compatibility/std_boxing.hpp"
#include <array>

using namespace aworx;

// #################################################################################################
// #### Unit test executing tutorial code
// #################################################################################################

namespace ut_aworx {

namespace {

std::pair<int, AString>     executeprocess(NString cmdWithoutOutputRedirection)
{
    NString512 cmd(cmdWithoutOutputRedirection);
    cmd << "  2>&1";


    std::array<char, 128> buffer;
    std::string result;
    auto pipe = popen(cmd, "r");

    AString output;
    if (!pipe)
        return std::make_pair(999999, output.Reset("popen() failed!"));

    while (!feof(pipe))
    {
        if (fgets(buffer.data(), 128, pipe) != nullptr)
            result += buffer.data();
    }
    int rc = pclose(pipe);
    if( rc > 256 )
        rc/= 256; // I do not know exactly why I have to dived this by 256!
                  // obviously the real result code is shifted

    return std::make_pair(rc, output.Reset(result));
}


#define SAMPLE_EXE_DIR  A_CHAR( "/tmp/_builds_/ALib_Samples/cli_clion_debug")
std::pair<int, AString>  utExecCLI(AWorxUnitTesting& ut, NString args, NString fileExt)
{
    NAString cmd( SAMPLE_EXE_DIR A_CHAR("/Sample " ));
    cmd << ' ' <<  args;
    auto result= executeprocess(cmd);
    UT_PRINT("Invocation of command: {!Q}\n"
             "Result Code: {}\n"
             "Result Output:\n"
             "**************************** output: *****************************************\n{}",
             cmd,
             result.first,
             result.second )

    String2K buffer( result.second );
    Formatter::GetDefault()->Format( buffer, "<Exit code {:2>}>{}", result.first, NewLine() );
    ut.WriteResultFile( NString256("DOX_ALIB_CLI_").Append(fileExt).Append(".txt"), buffer, "" );
    return result;
}

} // anonymous namespace


UT_CLASS

UT_METHOD( CLI )
{
    UT_INIT()

    // Check if the sample application's directory exists

    if(!Directory::Exists(SAMPLE_EXE_DIR))
    {
        UT_PRINT( "ALib CLI Sample not available. Skipping tests. Documentation can not be built\n"
                  "if previous runs did not already create the output files!" )
        return;
    }

    auto
    r= utExecCLI(ut, ""                            , "NOCMD"            );  UT_EQ( r.first, 0)
                                                                            UT_EQ( r.second.Length(), 19 + NewLine().Length())
    r= utExecCLI(ut, "now"                         , "CMDNOW"           );  UT_EQ( r.first, 0)
                                                                            UT_EQ( r.second.Length(), 19 + NewLine().Length())
    r= utExecCLI(ut, "--format=\"MMM dd, yyyy\""   , "CMDNOW_FORMAT"    );  UT_EQ( r.first, 0)
                                                                            UT_EQ( r.second.Length(), 12 + NewLine().Length())
       utExecCLI(ut, "file /home"                  , "CMDFILE"          );

    r= utExecCLI(ut, "file"                        , "FILE_MISSING_PAR" );  UT_EQ( r.first, 102)
                                                                            UT_EQ( r.second.Length(), 70 + NewLine().Length())

    r= utExecCLI(ut, "--format"                    , "FILE_MISSING_PAR2");  UT_EQ( r.first, 10)
                                                                            UT_EQ( r.second.Length(), 225 + NewLine().Length())

    r= utExecCLI(ut, "unknown"                     , "UNKN_CMD");           UT_EQ( r.first, 100)
                                                                            UT_EQ( r.second.Length(), 38 + NewLine().Length())

    r= utExecCLI(ut, "--unknown"                   , "UNKN_OPT");           UT_EQ( r.first, 101)
                                                                            UT_EQ( r.second.Length(), 39 + NewLine().Length())

    r= utExecCLI(ut, "--help"                      , "HELP");               UT_EQ( r.first, 0 )
                                                                            UT_EQ( r.second.Length(), 1258 + NewLine().Length())

    r= utExecCLI(ut, "--help now"                  , "HELP_NOW");           UT_EQ( r.first, 0 )
                                                                            UT_EQ( r.second.Length(), 507 + NewLine().Length())

    r= utExecCLI(ut, "--help file"                 , "HELP_FILE");          UT_EQ( r.first, 0 )
                                                                            UT_EQ( r.second.Length(), 531 + NewLine().Length())

    r= utExecCLI(ut, "--help format"               , "HELP_FORMAT");        UT_EQ( r.first, 0 )
                                                                            UT_EQ( r.second.Length(), 611 + NewLine().Length())
}
#undef SAMPLE_EXE_DIR

#include "unittests/aworx_unittests_end.hpp"

} //namespace

#endif //  ALIB_UT_DOCS && ALIB_CLI
