// #################################################################################################
//  AWorx ALib Unit Tests
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib_precompile.hpp"
#include "alib_test_selection.hpp"
#if ALIB_UT_CAMP

#include "ALib.ALox.H"
#include "ALib.System.H"
#include "ALib.Format.H"

#define TESTCLASSNAME       UT_System

#include "aworx_unittests.hpp"

using namespace std;
using namespace alib;

namespace ut_aworx {

UT_CLASS

//--------------------------------------------------------------------------------------------------
//--- Path
//--------------------------------------------------------------------------------------------------
UT_METHOD(Path)
{
    UT_INIT()

    // Special Directories
    UT_PRINT("") UT_PRINT( "### Directory::SpecialFolders ###" )
    {
        Path dir( SystemFolders::Current );
        UT_PRINT( String512() << "The current directory is:     "  << dir )
        UT_TRUE( dir.IsNotEmpty() )   UT_TRUE( dir.IsDirectory() )
    }

    {
        Path dir( SystemFolders::Current );
        UT_PRINT( String512() << "The current directory is:     " << dir )
        UT_TRUE( dir.IsNotEmpty() )    UT_TRUE( dir.IsDirectory() )
    }

    {
        Path dir( SystemFolders::Home );
        UT_PRINT( String512() << "The home directory is:        " << dir )
        UT_TRUE( dir.IsNotEmpty() )    UT_TRUE( dir.IsDirectory() )
    }

    {
        Path dir( SystemFolders::HomeConfig );
        UT_PRINT( String512() << "The HomeConfig directory is:  " << dir )
        UT_TRUE( dir.IsNotEmpty() )    UT_TRUE( dir.IsDirectory() )
    }

    {
        Path dir( SystemFolders::Module );
        UT_PRINT( String512() << "The Module directory is:      " << dir )
        UT_TRUE( dir.IsNotEmpty() )    UT_TRUE( dir.IsDirectory() )
    }

    {
        Path dir( SystemFolders::Root );
        UT_PRINT( String512() << "The Root directory is:        " << dir )
        UT_TRUE( dir.IsNotEmpty() )    UT_TRUE( dir.IsDirectory() )
    }

    {
        Path dir( SystemFolders::Temp );
        UT_PRINT( String512() << "The Temp directory is:        " << dir )
        UT_TRUE( dir.IsNotEmpty() )    UT_TRUE( dir.IsDirectory() )
    }

    {
        Path dir( SystemFolders::VarTemp );
        UT_PRINT( String512() << "The VarTemp directory is:     " << dir )
        UT_TRUE( dir.IsNotEmpty() )    UT_TRUE( dir.IsDirectory() )
    }

    // Various

    #if !defined(_WIN32)
    {  Path p= A_PATH( "/"    ) ; UT_EQ( A_PATH("/") , p.Parent(  ) ) }
    {  Path p= A_PATH( "/test") ; UT_EQ( A_PATH("/") , p.Parent(  ) ) }
    {  Path p= A_PATH( "test" ) ; UT_EQ( A_PATH("" ) , p.Parent(  ) ) }

    {  Path p= A_PATH("/"    ) ; UT_FALSE( p.ChangeToParent( ) ) UT_EQ( system::PathString(A_PATH("/"   )), p ) }
    {  Path p= A_PATH("/test") ; UT_TRUE(  p.ChangeToParent( ) ) UT_EQ( system::PathString(A_PATH("/"   )), p ) }
    {  Path p= A_PATH("test" ) ; UT_FALSE( p.ChangeToParent( ) ) UT_EQ( system::PathString(A_PATH("test")), p ) }
    #endif
}



//--------------------------------------------------------------------------------------------------
//--- GetVariable
//--------------------------------------------------------------------------------------------------
UT_METHOD(GetVariable)
{
    UT_INIT()

    UT_PRINT("") UT_PRINT( "### Environment::GetVariable###" )
    AString path;
    bool result;
    #if defined(_WIN32)
        result=    EnvironmentVariables::Get( A_CHAR("HOMEDRIVE"), path )
                 | EnvironmentVariables::Get( A_CHAR("HOMEPATH") , path, lang::CurrentData::Keep );
    #else
        result=  EnvironmentVariables::Get( A_CHAR("HOME"),  path );
    #endif

    UT_PRINT("The home directory is: ", path )
    UT_TRUE( Path(path).IsDirectory() )
    UT_TRUE( result )

    result=  EnvironmentVariables::Get( A_CHAR("Nonexistingenvvar")  , path, lang::CurrentData::Keep );
    UT_FALSE( result )   UT_FALSE( path.IsEmpty() )
    result=  EnvironmentVariables::Get( A_CHAR("Nonexistingenvvar")  , path );
    UT_FALSE( result )   UT_TRUE ( path.IsEmpty() )
}

//--------------------------------------------------------------------------------------------------
//--- Processes
//--------------------------------------------------------------------------------------------------
UT_METHOD(Processes)
{
    UT_INIT()

    UT_PRINT("") UT_PRINT( "### Environment::GetProcessInfo###" )


    String2K output;
    const ProcessInfo& currentProcess= ProcessInfo::Current();
    UT_TRUE( currentProcess.PID != 0 )

    #if defined (__GLIBC__) || defined(__APPLE__) || defined(__ANDROID_NDK__)
        // print process tree of us
        int indent= 0;
        uinteger nextPID= currentProcess.PPID;
        while ( nextPID != 0 )
        {
            ProcessInfo pi( nextPID );
            output.Reset().InsertChars(' ', 2* indent); output  << "PID:          " << pi.PID;            UT_PRINT( output )
            output.Reset().InsertChars(' ', 2* indent); output  << "PPID:         " << pi.PPID;           UT_PRINT( output )
            output.Reset().InsertChars(' ', 2* indent); output  << "Name:         " << pi.Name;           UT_PRINT( output )
            output.Reset().InsertChars(' ', 2* indent); output  << "ExecFileName: " << pi.ExecFileName;   UT_PRINT( output )
            output.Reset().InsertChars(' ', 2* indent); output  << "ExecFilePath: " << pi.ExecFilePath;   UT_PRINT( output )
            output.Reset().InsertChars(' ', 2* indent); output  << "CmdLine:      " << pi.CmdLine;        UT_PRINT( output )
            #if !defined(__APPLE__)
            output.Reset().InsertChars(' ', 2* indent); output  << "StatState:    " << pi.StatState;      UT_PRINT( output )
            output.Reset().InsertChars(' ', 2* indent); output  << "StatPGRP:     " << pi.StatPGRP;       UT_PRINT( output )
            #endif
            //output.Reset(' ', 2* indent); output  << "Stat:      " << pi.Stat;      UT_PRINT( output )

            ++indent;
            nextPID= pi.PPID;
        }

    #elif defined(_WIN32)

        output.Reset( "PID:               " ) << currentProcess.PID;                      UT_PRINT( output )
        output.Reset( "CmdLine:           " ) << currentProcess.CmdLine;                  UT_PRINT( output )
        output.Reset( "ConsoleTitle:      " ) << currentProcess.ConsoleTitle;             UT_PRINT( output )

    #else
        #pragma message ("Unknown Platform in file: " __FILE__ )
    #endif
}

//--------------------------------------------------------------------------------------------------
//--- ShellCommand
//--------------------------------------------------------------------------------------------------
UT_METHOD(ShellCommand)
{
    UT_INIT()

    UT_PRINT("") UT_PRINT( "### Class ShellCommand ###" )

    // MonoAllocator version
    {
        LocalAllocator4K ma;
        ShellCommandMA shellCmd(ma);

        UT_PRINT("Passing invalid command")
        NCString cmd= "notacommand";
        int cmdResult= shellCmd.Run(cmd);
    #if !defined(_WIN32)
        UT_EQ( 127, cmdResult)
    #else
        UT_EQ(   1, cmdResult)
    #endif
        UT_EQ( 0UL, shellCmd.size())

    #if !defined(_WIN32)
        cmd= "ls -la " ALIB_BASE_DIR "/src/alib/system";
    #else
        NString256 cmdBuf("dir " ALIB_BASE_DIR "\\src\\alib\\system" );
        cmdBuf.SearchAndReplace('/', '\\'),
        cmd= cmdBuf;
    #endif
        UT_PRINT("Cmd: {!Q'}", cmd)
        cmdResult= shellCmd.Run(cmd);
        UT_EQ(   0, cmdResult)
    #if !defined(_WIN32)
        UT_EQ(17UL, shellCmd.size())
    #else
        UT_EQ(21UL, shellCmd.size())
    #endif
    #if !defined(_WIN32)
        if ( cmdResult == 0 && shellCmd.ReadBuffer.IsNotEmpty()) {
            Log_Info("Cmd executed. Result={:>03}, lines: {}, cmd: {!Q'}", cmdResult, shellCmd.size(), cmd )
            Log_Prune(
                int lineNo= 0;
                for ( auto& line : shellCmd )
                    Log_Info("{:>2}: {}", ++lineNo, line )
            )
        }

        UT_PRINT("Cmd: {!Q'}  (repeated)", cmd)
        cmdResult= shellCmd.Run(cmd);
        UT_EQ(   0, cmdResult)
        if ( cmdResult == 0 && shellCmd.ReadBuffer.IsNotEmpty()) {
            Log_Info("Cmd executed. Result={:>03}, lines: {}, cmd: {!Q'}", cmdResult, shellCmd.size(), cmd )
            Log_Prune(
                int lineNo= 0;
                for ( auto& line : shellCmd )
                    Log_Info("{:>2}: {}", ++lineNo, line )
            )
            UT_EQ(17UL, shellCmd.size())
        }

        cmd= "ls -la " ALIB_BASE_DIR "/src/alib/threadmodel";
        UT_PRINT("Cmd: {!Q'}  (without clearing old)", cmd)
        cmdResult= shellCmd.Run(cmd, lang::CurrentData::Keep);
        UT_EQ(   0, cmdResult)
        if ( cmdResult == 0 && shellCmd.ReadBuffer.IsNotEmpty()) {
            Log_Info("Cmd executed. Result={:>03}, lines: {}, cmd: {!Q'}", cmdResult, shellCmd.size(), cmd )
            Log_Prune(
                int lineNo= 0;
                for ( auto& line : shellCmd )
                    Log_Info("{:>2}: {}", ++lineNo, line )
            )
            UT_EQ(28UL, shellCmd.size())
        }
    #endif
    }

    #if !defined(_WIN32)
    // HeapAllocator version (mainly for testing compilation)
    {
        ShellCommand shellCmd;

        UT_PRINT("Passing invalid command")
        NCString cmd= "notacommand";
        int cmdResult= shellCmd.Run(cmd);
        UT_EQ( 127, cmdResult)
        UT_EQ( 0UL, shellCmd.size())

        cmd= "ls -la " ALIB_BASE_DIR "/src/alib/system";
        UT_PRINT("Cmd: {!Q'}", cmd)
        cmdResult= shellCmd.Run(cmd);
        UT_EQ(   0, cmdResult)
        UT_EQ(17UL, shellCmd.size())
        if ( cmdResult == 0 && shellCmd.ReadBuffer.IsNotEmpty()) {
            Log_Info("Cmd executed. Result={:>03}, lines: {}, cmd: {!Q'}", cmdResult, shellCmd.size(), cmd )
            Log_Prune(
                int lineNo= 0;
                for ( auto& line : shellCmd )
                    Log_Info("{:>2}: {}", ++lineNo, line )
            )
            UT_EQ(17UL, shellCmd.size())
        }
    }

    // PoolAllocator version (mainly for testing compilation, has no template instantiation)
    {
        LocalAllocator8K ma;
        PoolAllocator pa(ma);
        system::TShellCommand<PoolAllocator> shellCmd(pa);

        UT_PRINT("Passing invalid command")
        NCString cmd= "notacommand";
        int cmdResult= shellCmd.Run(cmd);
        UT_EQ( 127, cmdResult)
        UT_EQ( 0UL, shellCmd.size())

        cmd= "ls -la " ALIB_BASE_DIR "/src/alib/system";
        UT_PRINT("Cmd: {!Q'}", cmd)
        cmdResult= shellCmd.Run(cmd);
        UT_EQ(   0, cmdResult)
        UT_EQ(17UL, shellCmd.size())
        if ( cmdResult == 0 && shellCmd.ReadBuffer.IsNotEmpty()) {
            Log_Info("Cmd executed. Result={:>03}, lines: {}, cmd: {!Q'}", cmdResult, shellCmd.size(), cmd )
            Log_Prune(
                int lineNo= 0;
                for ( auto& line : shellCmd )
                    Log_Info("{:>2}: {}", ++lineNo, line )
            )
            UT_EQ(17UL, shellCmd.size())
        }
    }

    // Static usage version
    {
        LocalAllocator4K ma;
        NAStringMA as(ma.AsMonoAllocator());
        NStringVectorMA sv(ma);

        UT_PRINT("Passing invalid command")
        NCString cmd= "notacommand";
        int cmdResult= system::TShellCommand<MonoAllocator>::Run(cmd, as, &sv);
        UT_EQ( 127, cmdResult)
        UT_EQ( 0UL, sv.size())

        // read directory
        cmd= "ls -la " ALIB_BASE_DIR "/src/alib/system";
        UT_PRINT("Cmd: {!Q'}", cmd)
        cmdResult= system::TShellCommand<MonoAllocator>::Run(cmd, as, &sv);
        UT_EQ(   0, cmdResult)
        UT_EQ(17UL, sv.size())
        if ( cmdResult == 0 && as.IsNotEmpty()) {
            Log_Info("Cmd executed. Result={:>03}, lines: {}, cmd: {!Q'}", cmdResult, sv.size(), cmd )
            Log_Prune(
                int lineNo= 0;
                for ( auto& line : sv )
                    Log_Info("{:>2}: {}", ++lineNo, line )
            )
            UT_EQ(17UL, sv.size())
        }

        // read a next directory
        cmd= "ls -la " ALIB_BASE_DIR "/src/alib/threadmodel";
        UT_PRINT("Cmd: {!Q'}", cmd)
        cmdResult= system::TShellCommand<MonoAllocator>::Run(cmd, as, &sv);
        UT_EQ(   0, cmdResult)
        UT_EQ(28UL, sv.size())
        if ( cmdResult == 0 && as.IsNotEmpty()) {
            Log_Info("Cmd executed. Result={:>03}, lines: {}, cmd: {!Q'}", cmdResult, sv.size(), cmd )
            Log_Prune(
                int lineNo= 0;
                for ( auto& line : sv )
                    Log_Info("{:>2}: {}", ++lineNo, line )
            )
            UT_EQ(28UL, sv.size())
        }

        // repeat without providing the vector (test nullptr checks)
        UT_PRINT("Cmd: {!Q'}", cmd)
        integer oldBuffLen= as.Length();
        cmdResult= system::TShellCommand<MonoAllocator>::Run(cmd, as);
        UT_EQ(   0, cmdResult)
        UT_EQ(28UL, sv.size())
        UT_TRUE(oldBuffLen + 20 < as.Length() )
        if ( cmdResult == 0 && as.IsNotEmpty()) {
            Log_Info("Cmd executed. Result={:>03}, lines: {}, cmd: {!Q'}", cmdResult, sv.size(), cmd )
            Log_Prune(
                int lineNo= 0;
                for ( auto& line : sv )
                    Log_Info("{:>2}: {}", ++lineNo, line )
            )
            UT_EQ(28UL, sv.size())
        }
    }
    #endif
}

//--------------------------------------------------------------------------------------------------
//--- ByteSize
//--------------------------------------------------------------------------------------------------
UT_METHOD(ClassByteSize)
{
    UT_INIT()

    UT_PRINT("") UT_PRINT( "### Class ByteSizeXY ###" )
    AString     buf;
    buf << ByteSizeUnits::B   << ','  << ByteSizeUnits::B_SI   << ','
        << ByteSizeUnits::KiB << ','  << ByteSizeUnits::kB     << ','
        << ByteSizeUnits::MiB << ','  << ByteSizeUnits::MB     << ','
        << ByteSizeUnits::GiB << ','  << ByteSizeUnits::GB     << ','
        << ByteSizeUnits::TiB << ','  << ByteSizeUnits::TB     << ','
        << ByteSizeUnits::PiB << ','  << ByteSizeUnits::PB     << ','
        << ByteSizeUnits::EiB << ','  << ByteSizeUnits::EB     << ','
        << ByteSizeUnits::ZiB << ','  << ByteSizeUnits::ZB     << ','
        << ByteSizeUnits::YiB << ','  << ByteSizeUnits::YB     << ','
        << ByteSizeUnits::RiB << ','  << ByteSizeUnits::RB     << ','
        << ByteSizeUnits::QiB << ','  << ByteSizeUnits::QB    ;
    UT_EQ(String(A_CHAR("B,"  ) A_CHAR("B,") 
                A_CHAR("KiB,") A_CHAR("kB,")
                A_CHAR("MiB,") A_CHAR("MB,")
                A_CHAR("GiB,") A_CHAR("GB,")
                A_CHAR("TiB,") A_CHAR("TB,")
                A_CHAR("PiB,") A_CHAR("PB,")
                A_CHAR("EiB,") A_CHAR("EB,")
                A_CHAR("ZiB,") A_CHAR("ZB,")
                A_CHAR("YiB,") A_CHAR("YB,")
                A_CHAR("RiB,") A_CHAR("RB,")
                A_CHAR("QiB,") A_CHAR("QB" )), buf )
    UT_PRINT(buf)

    //--------------------- AString::Append --------------
    buf.Reset(ByteSizeIEC(    1                )); UT_PRINT(buf) UT_EQ( String(A_CHAR(      "1B")), String(buf) )
    buf.Reset(ByteSizeIEC( 500                 )); UT_PRINT(buf) UT_EQ( String(A_CHAR(    "500B")), String(buf) )
    buf.Reset(ByteSizeIEC( 950                 )); UT_PRINT(buf) UT_EQ( String(A_CHAR(  "0.9KiB")), String(buf) )
    buf.Reset(ByteSizeIEC( 1000                )); UT_PRINT(buf) UT_EQ( String(A_CHAR(  "1.0KiB")), String(buf) )
    buf.Reset(ByteSizeIEC( 1023                )); UT_PRINT(buf) UT_EQ( String(A_CHAR(  "1.0KiB")), String(buf) )
    buf.Reset(ByteSizeIEC(   1 * 1024LL        )); UT_PRINT(buf) UT_EQ( String(A_CHAR(  "1.0KiB")), String(buf) )
    buf.Reset(ByteSizeIEC(   1 * 1538LL        )); UT_PRINT(buf) UT_EQ( String(A_CHAR(  "1.5KiB")), String(buf) )
    buf.Reset(ByteSizeIEC(  10 * 1024LL        )); UT_PRINT(buf) UT_EQ( String(A_CHAR( "10.0KiB")), String(buf) )
    buf.Reset(ByteSizeIEC( 155 * 1024LL        )); UT_PRINT(buf) UT_EQ( String(A_CHAR("155.0KiB")), String(buf) )
    buf.Reset(ByteSizeIEC( 999 * 1024LL        )); UT_PRINT(buf) UT_EQ( String(A_CHAR(  "1.0MiB")), String(buf) )
    buf.Reset(ByteSizeIEC(1000ULL << 10        )); UT_PRINT(buf) UT_EQ( String(A_CHAR(  "1.0MiB")), String(buf) )
    buf.Reset(ByteSizeIEC(  10ULL << 20        )); UT_PRINT(buf) UT_EQ( String(A_CHAR( "10.0MiB")), String(buf) )
    buf.Reset(ByteSizeIEC( 155ULL << 20        )); UT_PRINT(buf) UT_EQ( String(A_CHAR("155.0MiB")), String(buf) )
    buf.Reset(ByteSizeIEC( 999ULL << 20        )); UT_PRINT(buf) UT_EQ( String(A_CHAR(  "1.0GiB")), String(buf) )
    buf.Reset(ByteSizeIEC(1000ULL << 20        )); UT_PRINT(buf) UT_EQ( String(A_CHAR(  "1.0GiB")), String(buf) )
    #if ALIB_SIZEOF_INTEGER==8				  
    buf.Reset(ByteSizeIEC(  10ULL << 30        )); UT_PRINT(buf) UT_EQ( String(A_CHAR( "10.0GiB")), String(buf) )
    buf.Reset(ByteSizeIEC( 155ULL << 30        )); UT_PRINT(buf) UT_EQ( String(A_CHAR("155.0GiB")), String(buf) )
    buf.Reset(ByteSizeIEC( 999ULL << 30        )); UT_PRINT(buf) UT_EQ( String(A_CHAR(  "1.0TiB")), String(buf) )
    buf.Reset(ByteSizeIEC(1000ULL << 30        )); UT_PRINT(buf) UT_EQ( String(A_CHAR(  "1.0TiB")), String(buf) )
    buf.Reset(ByteSizeIEC(  10ULL << 40        )); UT_PRINT(buf) UT_EQ( String(A_CHAR( "10.0TiB")), String(buf) )
    buf.Reset(ByteSizeIEC( 155ULL << 40        )); UT_PRINT(buf) UT_EQ( String(A_CHAR("155.0TiB")), String(buf) )
    buf.Reset(ByteSizeIEC( 999ULL << 40        )); UT_PRINT(buf) UT_EQ( String(A_CHAR(  "1.0PiB")), String(buf) )
    buf.Reset(ByteSizeIEC(1000ULL << 40        )); UT_PRINT(buf) UT_EQ( String(A_CHAR(  "1.0PiB")), String(buf) )
    buf.Reset(ByteSizeIEC(  10ULL << 50        )); UT_PRINT(buf) UT_EQ( String(A_CHAR( "10.0PiB")), String(buf) )
    buf.Reset(ByteSizeIEC( 155ULL << 50        )); UT_PRINT(buf) UT_EQ( String(A_CHAR("155.0PiB")), String(buf) )
    buf.Reset(ByteSizeIEC( 999ULL << 50        )); UT_PRINT(buf) UT_EQ( String(A_CHAR(  "1.0EiB")), String(buf) )
    buf.Reset(ByteSizeIEC(1000ULL << 50        )); UT_PRINT(buf) UT_EQ( String(A_CHAR(  "1.0EiB")), String(buf) )
    buf.Reset(ByteSizeIEC(  10ULL << 60        )); UT_PRINT(buf) UT_EQ( String(A_CHAR( "10.0EiB")), String(buf) )
    buf.Reset(ByteSizeIEC(0xFFFFFFFFFFFFFFFFULL)); UT_PRINT(buf) UT_EQ( String(A_CHAR( "16.0EiB")), String(buf) )
    #endif

    buf.Reset(ByteSizeSI (    1                          )); UT_PRINT(buf) UT_EQ( String(A_CHAR(     "1B")), String(buf) )
    buf.Reset(ByteSizeSI ( 500                           )); UT_PRINT(buf) UT_EQ( String(A_CHAR(   "500B")), String(buf) )
    buf.Reset(ByteSizeSI ( 901                           )); UT_PRINT(buf) UT_EQ( String(A_CHAR(  "0.9kB")), String(buf) )
    buf.Reset(ByteSizeSI ( 945                           )); UT_PRINT(buf) UT_EQ( String(A_CHAR(  "0.9kB")), String(buf) )
    buf.Reset(ByteSizeSI ( 955                           )); UT_PRINT(buf) UT_EQ( String(A_CHAR(  "1.0kB")), String(buf) )
    buf.Reset(ByteSizeSI ( 999                           )); UT_PRINT(buf) UT_EQ( String(A_CHAR(  "1.0kB")), String(buf) )
    buf.Reset(ByteSizeSI ( 1000                          )); UT_PRINT(buf) UT_EQ( String(A_CHAR(  "1.0kB")), String(buf) )
    buf.Reset(ByteSizeSI ( 1024                          )); UT_PRINT(buf) UT_EQ( String(A_CHAR(  "1.0kB")), String(buf) )
    buf.Reset(ByteSizeSI ( 500   * 1000ULL               )); UT_PRINT(buf) UT_EQ( String(A_CHAR("500.0kB")), String(buf) )
    buf.Reset(ByteSizeSI ( 901   * 1000ULL               )); UT_PRINT(buf) UT_EQ( String(A_CHAR(  "0.9MB")), String(buf) )
    buf.Reset(ByteSizeSI ( 945   * 1000ULL               )); UT_PRINT(buf) UT_EQ( String(A_CHAR(  "0.9MB")), String(buf) )
    buf.Reset(ByteSizeSI ( 955   * 1000ULL               )); UT_PRINT(buf) UT_EQ( String(A_CHAR(  "1.0MB")), String(buf) )
    buf.Reset(ByteSizeSI ( 999   * 1000ULL               )); UT_PRINT(buf) UT_EQ( String(A_CHAR(  "1.0MB")), String(buf) )
    buf.Reset(ByteSizeSI ( 1000  * 1000ULL               )); UT_PRINT(buf) UT_EQ( String(A_CHAR(  "1.0MB")), String(buf) )
    buf.Reset(ByteSizeSI ( 1024  * 1000ULL               )); UT_PRINT(buf) UT_EQ( String(A_CHAR(  "1.0MB")), String(buf) )
    buf.Reset(ByteSizeSI ( 500   * 1000000ULL            )); UT_PRINT(buf) UT_EQ( String(A_CHAR("500.0MB")), String(buf) )
    buf.Reset(ByteSizeSI ( 901   * 1000000ULL            )); UT_PRINT(buf) UT_EQ( String(A_CHAR(  "0.9GB")), String(buf) )
    buf.Reset(ByteSizeSI ( 945   * 1000000ULL            )); UT_PRINT(buf) UT_EQ( String(A_CHAR(  "0.9GB")), String(buf) )
    buf.Reset(ByteSizeSI ( 955   * 1000000ULL            )); UT_PRINT(buf) UT_EQ( String(A_CHAR(  "1.0GB")), String(buf) )
    buf.Reset(ByteSizeSI ( 999   * 1000000ULL            )); UT_PRINT(buf) UT_EQ( String(A_CHAR(  "1.0GB")), String(buf) )
    buf.Reset(ByteSizeSI ( 1000  * 1000000ULL            )); UT_PRINT(buf) UT_EQ( String(A_CHAR(  "1.0GB")), String(buf) )
    buf.Reset(ByteSizeSI ( 1024  * 1000000ULL            )); UT_PRINT(buf) UT_EQ( String(A_CHAR(  "1.0GB")), String(buf) )
#if ALIB_SIZEOF_INTEGER==8
    buf.Reset(ByteSizeSI ( 500   * 1000000000ULL         )); UT_PRINT(buf) UT_EQ( String(A_CHAR("500.0GB")), String(buf) )
    buf.Reset(ByteSizeSI ( 901   * 1000000000ULL         )); UT_PRINT(buf) UT_EQ( String(A_CHAR(  "0.9TB")), String(buf) )
    buf.Reset(ByteSizeSI ( 945   * 1000000000ULL         )); UT_PRINT(buf) UT_EQ( String(A_CHAR(  "0.9TB")), String(buf) )
    buf.Reset(ByteSizeSI ( 955   * 1000000000ULL         )); UT_PRINT(buf) UT_EQ( String(A_CHAR(  "1.0TB")), String(buf) )
    buf.Reset(ByteSizeSI ( 999   * 1000000000ULL         )); UT_PRINT(buf) UT_EQ( String(A_CHAR(  "1.0TB")), String(buf) )
    buf.Reset(ByteSizeSI ( 1000  * 1000000000ULL         )); UT_PRINT(buf) UT_EQ( String(A_CHAR(  "1.0TB")), String(buf) )
    buf.Reset(ByteSizeSI ( 1024  * 1000000000ULL         )); UT_PRINT(buf) UT_EQ( String(A_CHAR(  "1.0TB")), String(buf) )
    buf.Reset(ByteSizeSI ( 500   * 1000000000000ULL      )); UT_PRINT(buf) UT_EQ( String(A_CHAR("500.0TB")), String(buf) )
    buf.Reset(ByteSizeSI ( 901   * 1000000000000ULL      )); UT_PRINT(buf) UT_EQ( String(A_CHAR(  "0.9PB")), String(buf) )
    buf.Reset(ByteSizeSI ( 945   * 1000000000000ULL      )); UT_PRINT(buf) UT_EQ( String(A_CHAR(  "0.9PB")), String(buf) )
    buf.Reset(ByteSizeSI ( 955   * 1000000000000ULL      )); UT_PRINT(buf) UT_EQ( String(A_CHAR(  "1.0PB")), String(buf) )
    buf.Reset(ByteSizeSI ( 999   * 1000000000000ULL      )); UT_PRINT(buf) UT_EQ( String(A_CHAR(  "1.0PB")), String(buf) )
    buf.Reset(ByteSizeSI ( 1000  * 1000000000000ULL      )); UT_PRINT(buf) UT_EQ( String(A_CHAR(  "1.0PB")), String(buf) )
    buf.Reset(ByteSizeSI ( 1024  * 1000000000000ULL      )); UT_PRINT(buf) UT_EQ( String(A_CHAR(  "1.0PB")), String(buf) )
    buf.Reset(ByteSizeSI ( 500   * 1000000000000000ULL   )); UT_PRINT(buf) UT_EQ( String(A_CHAR("500.0PB")), String(buf) )
    buf.Reset(ByteSizeSI ( 901   * 1000000000000000ULL   )); UT_PRINT(buf) UT_EQ( String(A_CHAR(  "0.9EB")), String(buf) )
    buf.Reset(ByteSizeSI ( 945   * 1000000000000000ULL   )); UT_PRINT(buf) UT_EQ( String(A_CHAR(  "0.9EB")), String(buf) )
    buf.Reset(ByteSizeSI ( 955   * 1000000000000000ULL   )); UT_PRINT(buf) UT_EQ( String(A_CHAR(  "1.0EB")), String(buf) )
    buf.Reset(ByteSizeSI ( 999   * 1000000000000000ULL   )); UT_PRINT(buf) UT_EQ( String(A_CHAR(  "1.0EB")), String(buf) )
    buf.Reset(ByteSizeSI ( 1000  * 1000000000000000ULL   )); UT_PRINT(buf) UT_EQ( String(A_CHAR(  "1.0EB")), String(buf) )
    buf.Reset(ByteSizeSI ( 1024  * 1000000000000000ULL   )); UT_PRINT(buf) UT_EQ( String(A_CHAR(  "1.0EB")), String(buf) )
    buf.Reset(ByteSizeSI (  10   * 1000000000000000000ULL)); UT_PRINT(buf) UT_EQ( String(A_CHAR( "10.0EB")), String(buf) )
    buf.Reset(ByteSizeSI (0xFFFFFFFFFFFFFFFFULL          )); UT_PRINT(buf) UT_EQ( String(A_CHAR( "18.4EB")), String(buf) )
#endif

    //--------------------- AString::Append(Box) --------------
    {ALIB_LOCK_RECURSIVE_WITH(Formatter::DefaultLock)
        auto fmt= Formatter::Default;
        buf.Reset(); fmt->Format(buf, "Test in fmt-field: <{:>10}>", ByteSize(  100000, ' ') ); UT_PRINT(buf)
        buf.Reset(); fmt->Format(buf, "Test in fmt-field: <{:>10}>", ByteSizeSI(100000, '-') ); UT_PRINT(buf)
    }
#if ALIB_SIZEOF_INTEGER==8
    auto pair= ByteSizeIEC(155ULL << 30 ).GetMagnitude();
    buf.Reset(pair.first) << pair.second; UT_PRINT(buf) UT_EQ(A_CHAR("155.0GiB"), buf)
    UT_EQ( 155.0, ByteSizeIEC(155ULL << 30 ).ConvertTo(ByteSizeUnits::GiB) )

         pair= ByteSizeSI(500   * 1000000000000ULL ).GetMagnitude();
    buf.Reset(pair.first) << pair.second; UT_PRINT(buf) UT_EQ(A_CHAR("500.0TB"), buf)
    UT_EQ( 500.0, ByteSizeSI(500   * 1000000000000ULL ).ConvertTo(ByteSizeUnits::TB) )
#endif

}


#include "aworx_unittests_end.hpp"

} //namespace


#endif // ALIB_UT_CAMP
