// #################################################################################################
//  AWorx ALib Unit Tests
//  Documentation sample for module ALib CLI.
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib_precompile.hpp"
#include "alib_test_selection.hpp"

#if ALIB_UT_DOCS && ALIB_UT_CLI && ALIB_CAMP  && !defined(_WIN32)

// the namespace of our software

#define TESTCLASSNAME       UT_Dox_CLI
#include "aworx_unittests.hpp"

#include "ALib.Compatibility.StdBoxtraits.H"
#include "ALib.Format.H"
#include <array>

using namespace alib;

// #################################################################################################
// #### Unit test executing tutorial code
// #################################################################################################

namespace ut_aworx {

namespace {
#include "ALib.Lang.CIFunctions.H"

std::pair<int, NAString>     executeprocess(NString cmdWithoutOutputRedirection)
{
    NString512 cmd(cmdWithoutOutputRedirection);
    cmd << "  2>&1";

    NAString resultBuf;
    int resultCode= ShellCommand::Run(cmd, resultBuf);

    return std::make_pair(resultCode, resultBuf);
}


#define SAMPLE_EXE_DIR  A_CHAR( "/tmp/_builds_/ALib_Samples/cli_clion_debug")
std::pair<int, NAString>  utExecCLI(AWorxUnitTesting& ut, NString args, NString fileExt)
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
    {ALIB_LOCK_RECURSIVE_WITH(Formatter::DefaultLock)
     Formatter::Default->Format( buffer, "<Exit code {:>2}>{}", result.first, NEW_LINE ); }
    ut.WriteResultFile( NString256("DOX_CLI_").Append(fileExt).Append(".txt"), buffer, "" );
    return result;
}
#include "ALib.Lang.CIMethods.H"

} // anonymous namespace


UT_CLASS

UT_METHOD( CLI )
{
    UT_INIT()

    // Check if the sample application's directory exists
    if(!Path(SAMPLE_EXE_DIR).IsDirectory())
    {
        UT_PRINT( "ALib CLI Sample not available. Skipping tests. Documentation cannot be built\n"
                  "if previous runs did not already create the output files!" )
        return;
    }

    auto
    r= utExecCLI(ut, ""                            , "NOCMD"            );  UT_EQ( 0 , r.first)
                                                                            UT_EQ( 19, r.second.Length() )
    r= utExecCLI(ut, "now"                         , "CMDNOW"           );  UT_EQ( 0 , r.first)
                                                                            UT_EQ( 19, r.second.Length() )
    r= utExecCLI(ut, "--format=\"MMM dd, yyyy\""   , "CMDNOW_FORMAT"    );  UT_EQ( 0 , r.first)
                                                                            UT_EQ( 12, r.second.Length())
       utExecCLI(ut, "file /home"                  , "CMDFILE"          );

    r= utExecCLI(ut, "file"                        , "FILE_MISSING_PAR" );  UT_EQ( r.first, 102 )
                                                                            UT_EQ( 70, r.second.Length())

    r= utExecCLI(ut, "--format"                    , "FILE_MISSING_PAR2");  UT_EQ(   r.first, 10 )
                                                                            UT_TRUE( r.second.Length() > 300)

    r= utExecCLI(ut, "unknown"                     , "UNKN_CMD");           UT_EQ( r.first, 100 )
                                                                            UT_EQ( 38, r.second.Length())

    r= utExecCLI(ut, "--unknown"                   , "UNKN_OPT");           UT_EQ( r.first, 101 )
                                                                            UT_EQ( 39, r.second.Length() )

    r= utExecCLI(ut, "--help"                      , "HELP");               UT_EQ(   r.first, 0 )
                                                                            UT_TRUE( r.second.Length() > 100)

    r= utExecCLI(ut, "--help now"                  , "HELP_NOW");           UT_EQ(   r.first, 0 )
                                                                            UT_TRUE( r.second.Length()  > 400)

    r= utExecCLI(ut, "--help file"                 , "HELP_FILE");          UT_EQ(   r.first, 0 )
                                                                            UT_TRUE( r.second.Length() > 400)

    r= utExecCLI(ut, "--help format"               , "HELP_FORMAT");        UT_EQ(   r.first, 0 )
                                                                            UT_TRUE( r.second.Length() > 500)
}
#undef SAMPLE_EXE_DIR

#include "aworx_unittests_end.hpp"

} //namespace

#endif //  ALIB_UT_DOCS && ALIB_CLI
