// #################################################################################################
//  AWorx ALib Unit Tests
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"
#include "unittests/alib_test_selection.hpp"

#if ALIB_UT_CONFIGURATION

#include "alib/lang/basecamp/basecamp.hpp"
#include "alib/alox.hpp"

#if ALIB_ALOX
#   include "alib/alox/aloxcamp.hpp"
#endif
#   include "alib/config/inifilefeeder.hpp"
#   include "alib/config/configuration.hpp"
#   include "alib/config/plugins.hpp"
#   include "alib/lang/system/path.hpp"
#   include "alib/compatibility/std_strings_iostream.hpp"

#   include "alib/monomem/monoallocator.hpp"

#define TESTCLASSNAME       UT_Config
#include "unittests/aworx_unittests.hpp"


#include <iostream>
#include <fstream>

using namespace std;
using namespace alib;

namespace {
int systemCall(const NCString& cmd, AString& result)
{
#if !defined( _WIN32 )

    // Open pipe to file
    FILE* pipe = popen(cmd, "r");
    if (!pipe)
        return -1;

    // read till end of process
    int qtyResults= 0;
    NString4K readBuf;
    while (!feof(pipe))
    {
        // use buffer to read and add to result
        if (fgets(readBuf.VBuffer(), 1024 * 4 - 1, pipe) != nullptr)
        {
            readBuf.DetectLength();
            readBuf.DeleteEnd(NNEW_LINE);
            result << readBuf << NEW_LINE;
            ++qtyResults;
        }
    }

    pclose(pipe);
    return qtyResults;
#else
	(void) cmd; 
	(void) result; 
    return 0;
#endif

}


struct MyPlugin : public config::ConfigurationPlugin
{
    MyPlugin()
    : config::ConfigurationPlugin(alib::config::Priority::Environment - 1 )       {}

    virtual String   Name()  const override { return A_CHAR("MYPLUGIN"); }
    virtual bool     Get( const String& name, AString& target ) override
    {
        if( name.Equals( A_CHAR("MY_VAR") ) )
        {
            target <<  A_CHAR("frommyplugin");
            return true;
        }
        return false;
    }

};

struct Listener : public ConfigurationListener
{
    int qtyDecl= 0;
    int qtyDef = 0;
    int qtyDel = 0;
    ut_aworx::AWorxUnitTesting& ut;

    Listener(ut_aworx::AWorxUnitTesting& pUT)  : ut(pUT)                                          {}


    void    Notify(const Variable &variable, Event event, Priority previousPriority )       override
    {
        UT_PRINT( "{}-Event received for variable {!Q}. Priority={} (previously {}).",
                    event == Event::Creation   ? "Creation"
                  : event == Event::Definition ? "Definition"
                  :                              "Deletion",
                  variable,
                  variable.GetPriority(),
                  previousPriority                                      )

             if( event == Event::Creation   ) ++qtyDecl;
        else if( event == Event::Definition ) ++qtyDef;
        else                                  ++qtyDel;
    }
};

} // anonymous namespace


namespace ut_aworx {


UT_CLASS

//--------------------------------------------------------------------------------------------------
//--- Read and write a configuration file
//--------------------------------------------------------------------------------------------------
UT_METHOD(Basics)
{
    UT_INIT()

    //--------------------- test preset --------------------
    MonoAllocator ma(ALIB_DBG("UTCfgBasics",) 8);
    Configuration cfg(ma);
    cfg.PresetImportString(A_CHAR("MY_VAR"), A_CHAR("value"));
    Variable vc(cfg);

    vc.Declare(A_CHAR("MY_VAR"), A_CHAR("S"));  UT_TRUE (vc.IsDefined())
                                                UT_TRUE (vc.GetString() == A_CHAR("value"))
                                                UT_TRUE (vc.Try(A_CHAR("MY_VAR")))
    vc.Delete();                                UT_FALSE(vc.Try(A_CHAR("MY_VAR")))

    // delete variable and check if preset still works
    vc.Declare(A_CHAR("MY_VAR"), A_CHAR("S"));  UT_TRUE(vc.IsDefined())
                                                UT_TRUE(vc.GetString() == A_CHAR("value"))
    vc.Delete();                                UT_FALSE(vc.Try(A_CHAR("MY_VAR")))

    // delete preset and check that preset is gone
    cfg.PresetImportString(A_CHAR("MY_VAR"), NULL_STRING);
    vc.Declare(A_CHAR("MY_VAR"), A_CHAR("S"));  UT_FALSE(vc.IsDefined())
    vc.Delete();

    // do the same as above with nested preset values (this can't and must not delete the outer preset node)
    cfg.PresetImportString(A_CHAR("MY_VAR")    , A_CHAR("value"));
    cfg.PresetImportString(A_CHAR("MY_VAR/SUB"), A_CHAR("sub"));

    vc.Declare(A_CHAR("MY_VAR"), A_CHAR("S"));      UT_TRUE(vc.IsDefined())
                                                    UT_TRUE(vc.GetString() == A_CHAR("value"))
    vc.Declare(A_CHAR("MY_VAR/SUB"), A_CHAR("S"));  UT_TRUE(vc.IsDefined())
                                                    UT_TRUE(vc.GetString() == A_CHAR("sub"))

    vc.Delete();
    vc.Declare(A_CHAR("MY_VAR"), A_CHAR("S"));      UT_TRUE(vc.IsDefined())
                                                    UT_TRUE(vc.AsCursor().Name() == A_CHAR("MY_VAR"))
                                                    UT_TRUE(vc.GetString() == A_CHAR("value"))
    vc.Delete();
    cfg.PresetImportString(A_CHAR("MY_VAR"), NULL_STRING);
    vc.Declare(A_CHAR("MY_VAR/SUB"), A_CHAR("S"));  UT_TRUE(vc.IsDefined())
                                                    UT_TRUE(vc.GetString() == A_CHAR("sub"))
    vc.Declare(A_CHAR("MY_VAR"), A_CHAR("S"));      UT_FALSE(vc.IsDefined())


    //--------------------- test substitution --------------------
    {
        vc.Declare(A_CHAR("SUBS"), A_CHAR("S"), A_CHAR("Substitute"));
        Variable vc2( cfg, A_CHAR("TARGET"), A_CHAR("S"), A_CHAR("X ${SUBS} Y") );
        UT_EQ( A_CHAR("X Substitute Y"), vc2.GetString())

        vc .Declare(A_CHAR("SUBS2"  ), A_CHAR("I"), A_CHAR("42"));
        vc2.Declare(A_CHAR("TARGET2"), A_CHAR("S"), A_CHAR("X ${SUBS2} Y") );
        UT_EQ( A_CHAR("X 42 Y"), vc2.GetString())
        vc2.Declare(A_CHAR("TARGET3"), A_CHAR("S"), A_CHAR("X ${NOTAVAIL} Y") );
        UT_EQ( A_CHAR("X  Y"), vc2.GetString())

        cfg.SubstitutionVariableStart= A_CHAR("%");
        cfg.SubstitutionVariableEnd  = A_CHAR( "");
        vc2.Declare(A_CHAR("TARGET4"), A_CHAR("S"), A_CHAR("X %Subs2 Y") );
        UT_EQ( A_CHAR("X  Y"), vc2.GetString())
    }

    //--------------------- test some types --------------------
    {
        vc.Declare(A_CHAR("BOX_INT"), A_CHAR("BOX"), A_CHAR("12345"));
        UT_TRUE( vc.GetBox().IsType<integer>() ) UT_EQ( 12345, vc.GetBox().Unbox<integer>() )

        vc.Declare(A_CHAR("BOX_FLT"), A_CHAR("BOX"), A_CHAR("123.45"));
        UT_TRUE( vc.GetBox().IsType<double>() ) UT_EQ( 123.45, vc.GetBox().Unbox<double>() )

        vc.Declare(A_CHAR("BOX_OTH"), A_CHAR("BOX"), A_CHAR("all_other_is_string"));
        UT_TRUE( vc.GetBox().IsType<String>() ) UT_EQ( A_CHAR("all_other_is_string"), vc.GetBox().Unbox<String>() )
    }
}

//--------------------------------------------------------------------------------------------------
//--- Read and write a configuration file
//--------------------------------------------------------------------------------------------------
UT_METHOD(ConfigCustomPlugIn)
{
    UT_INIT()

    MonoAllocator ma(ALIB_DBG("UTCfgCP",) 8);
    Configuration cfg(ma);
    MyPlugin plugin;
    cfg.InsertPlugin( &plugin );

    Variable vc(cfg);
    vc.Declare(A_CHAR("MY_VAR"), A_CHAR("S"));
    UT_TRUE(vc.IsDefined())
    UT_TRUE(vc.GetString() == A_CHAR("frommyplugin"))

    vc.Declare(A_CHAR("MY_VAR_X"), A_CHAR("S"));
    UT_TRUE(!vc.IsDefined())
}

//--------------------------------------------------------------------------------------------------
//--- Read and write a configuration file
//--------------------------------------------------------------------------------------------------
UT_METHOD(ConfigCommandLineArgs)
{
    UT_INIT()

    const char* const args[]=
    {
        "COMMANDLINE"                      ,
        "-SingleHyphen=12"                 ,
        "--DoubleHyphen=yes"               ,
        "--Empty"                          ,
        "--Whitespaces   =  Hello Test  "  ,
        "--integer =  42"                  ,
        "--double =  3.14"                 ,
        "--ALIB_ITER=x"                    ,
        "--ALIB_ITER2=y"                   ,
        "--ALIB_NOITER"                    ,
        "-Home=overwritten"                ,
        "--ALIB_TEST=passed"               ,
    };

    auto alibArgCBackup= alib::ARG_C;
    auto alibArgVBackup= alib::ARG_VN;
    alib::ARG_C = sizeof(args)/sizeof(const char*);
    alib::ARG_VN= const_cast<const char**>( args );

    MonoAllocator ma(ALIB_DBG("UTCfg",) 8);
    Configuration cfg(ma);

    Variable vc(cfg);
    String256 buf;

    UT_EQ( Priority::CLI,   (vc.Declare(A_CHAR("SingleHyphen" ), A_CHAR("I"  ))).GetPriority()) UT_EQ( 12, vc.GetInt() )
                             vc.Declare(A_CHAR("DoubleHyphen" ), A_CHAR("B"  ));                UT_EQ( true , vc.GetBool())
                                                                                                UT_EQ( 2    , vc.Get<config::Bool>().TokenIndex)
                                                                                                UT_EQ( A_CHAR("Yes"), vc.Export(buf.Reset()) )
    UT_EQ( Priority::CLI,    vc.GetPriority())

    UT_EQ( Priority::CLI , vc.Declare(A_CHAR("Empty"        ), A_CHAR("S")).GetPriority() )   UT_EQ( EMPTY_STRING         , vc.GetString() )
    UT_EQ( Priority::CLI , vc.Declare(A_CHAR("Whitespaces"  ), A_CHAR("S")).GetPriority() )   UT_EQ( A_CHAR("Hello Test" ), vc.GetString() )
    UT_EQ( Priority::CLI , vc.Declare(A_CHAR("HOME"         ), A_CHAR("S")).GetPriority() )   UT_EQ( A_CHAR("overwritten"), vc.GetString() )
    UT_EQ( Priority::CLI , vc.Declare(A_CHAR("integer"      ), A_CHAR("I")).GetPriority() )   UT_EQ( 42                   , vc.GetInt() )
    UT_EQ( Priority::NONE, vc.Declare(A_CHAR("notexistent"  ), A_CHAR("S")).GetPriority() )
    UT_EQ( Priority::CLI , vc.Declare(A_CHAR("integer"      ), A_CHAR("I")).GetPriority() )   UT_EQ( 42                   , vc.GetInt() )
    UT_EQ( Priority::CLI , vc.Declare(A_CHAR("double"       ), A_CHAR("F")).GetPriority() )   UT_EQ( 3.14                 , vc.GetDouble() )
    UT_EQ( Priority::CLI , vc.Declare(A_CHAR("double"       ), A_CHAR("F")).GetPriority() )   UT_EQ( 3.14                 , vc.GetDouble() )
    UT_EQ( Priority::CLI , vc.Declare(A_CHAR("ALIB/test"    ), A_CHAR("S")).GetPriority() )   UT_EQ( A_CHAR("passed" )    , vc.GetString() )
    UT_EQ( Priority::NONE, vc.Declare(A_CHAR("notexistent"  ), A_CHAR("S")).GetPriority() )

    alib::ARG_C=  alibArgCBackup;
    alib::ARG_VN= alibArgVBackup;
}

UT_METHOD(ConfigCommandLineArgsWChar)
{
    // note: this is just WChar input if ALib is compiled with narrow strings.
    //       When compiling ALib with wide characters as default, the test above will convert
    UT_INIT()

    const wchar_t* const args[]=
    {
        L"COMMANDLINE"                      ,
        L"-SingleHyphen=12"                 ,
        L"--DoubleHyphen=yes"               ,
        L"--Empty"                          ,
        L"--Whitespaces   =  Hello Test  "  ,
        L"--integer =  42"                  ,
        L"--double =  3.14"                 ,
        L"--ALIB_ITER=x"                    ,
        L"--ALIB_ITER2=y"                   ,
        L"--ALIB_NOITER"                    ,
        L"-Home=overwritten"                ,
        L"--ALIB_TEST=passed"               ,
    };

    auto alibArgCBackup= alib::ARG_C;
    auto alibArgVBackup= alib::ARG_VN;
    alib::ARG_C = sizeof(args)/sizeof(const char*);
    alib::ARG_VN= nullptr;
    alib::ARG_VW= const_cast<const wchar_t**>( args );

    MonoAllocator ma(ALIB_DBG("UTCfgCLAW",) 8);
    Configuration cfg(ma);
    Variable vc(cfg);
    String256 buf;

    UT_EQ( Priority::CLI,   (vc.Declare(A_CHAR("SingleHyphen" ), A_CHAR("I"  ))).GetPriority())  UT_EQ( 12   , vc.GetInt() )
                             vc.Declare(A_CHAR("DoubleHyphen" ), A_CHAR("B"  ));                 UT_EQ( true , vc.GetBool())
                                                                                                 UT_EQ( 2    , vc.Get<config::Bool>().TokenIndex)
                                                                                                 UT_EQ( A_CHAR("Yes"), vc.Export(buf.Reset()) )
    UT_EQ( Priority::CLI,    vc.GetPriority())

    UT_EQ( Priority::CLI , vc.Declare(A_CHAR("Empty"        ), A_CHAR("S")).GetPriority() )   UT_EQ( EMPTY_STRING         , vc.GetString() )
    UT_EQ( Priority::CLI , vc.Declare(A_CHAR("Whitespaces"  ), A_CHAR("S")).GetPriority() )   UT_EQ( A_CHAR("Hello Test" ), vc.GetString() )
    UT_EQ( Priority::CLI , vc.Declare(A_CHAR("HOME"         ), A_CHAR("S")).GetPriority() )   UT_EQ( A_CHAR("overwritten"), vc.GetString() )
    UT_EQ( Priority::CLI , vc.Declare(A_CHAR("integer"      ), A_CHAR("I")).GetPriority() )   UT_EQ( 42                   , vc.GetInt() )
    UT_EQ( Priority::NONE, vc.Declare(A_CHAR("notexistent"  ), A_CHAR("S")).GetPriority() )
    UT_EQ( Priority::CLI , vc.Declare(A_CHAR("integer"      ), A_CHAR("I")).GetPriority() )   UT_EQ( 42                   , vc.GetInt() )
    UT_EQ( Priority::CLI , vc.Declare(A_CHAR("double"       ), A_CHAR("F")).GetPriority() )   UT_EQ( 3.14                 , vc.GetDouble() )
    UT_EQ( Priority::CLI , vc.Declare(A_CHAR("double"       ), A_CHAR("F")).GetPriority() )   UT_EQ( 3.14                 , vc.GetDouble() )
    UT_EQ( Priority::CLI , vc.Declare( A_CHAR("ALIB/test"   ), A_CHAR("S")).GetPriority() )   UT_EQ( A_CHAR("passed" )    , vc.GetString() )
    UT_EQ( Priority::NONE, vc.Declare(A_CHAR("notexistent"  ), A_CHAR("S")).GetPriority() )

    alib::ARG_C=  alibArgCBackup;
    alib::ARG_VN= alibArgVBackup;
}

/** ********************************************************************************************
 * ConfigIniFiles
 **********************************************************************************************/
// tests the basic INI-File class, which loads and stores "raw" data, without any connection
// to a Configuration.
UT_METHOD(ConfigIniFilePlain)
{
    UT_INIT()
    Path sampleIniPath (alib::SystemFolders::Temp, A_PATH("test1.ini"));
    Path sampleIniPath2(alib::SystemFolders::Temp, A_PATH("test2.ini"));

    #if defined(_WIN32)
    #   define LF "\r\n"
    #else
    #   define LF "\n"
    #endif
    // --------- basics on first test ini-file --------------
    {
      NString contents=
         "/// --------------------------------------------------------------------------" LF
         "/// ALib Unit Tests Data" LF
         "/// (c) 2024 AWorx GmbH. Published under MIT License (Open Source)." LF
         "/// --------------------------------------------------------------------------" LF
         "/// Last file comment" LF
         "" LF
         "/// S0V11C1" LF
         "V1= ValueS0V1" LF
         "" LF
         "" LF
         "/// Section1 Comment1" LF
         "# Section1 Comment2" LF
         "[Section1]" LF
         "" LF
         "# S1V1C1" LF
         "# S1V1C2" LF
         "V1= ValueS1V1" LF
         "V2= ValueS1V2" LF
         "" LF
         "" LF
         "# S1V3C1" LF
         "V3= ValueS1V3" LF
         "" LF
         "" LF
         "/// Section2 Comment1" LF
         "[Section2]" LF
         "" LF
         "V1= ValueS2V1" LF
         "" LF
         "# S2V2C1" LF
         "# S2V2C2" LF
         "V2= ValueS2V2L1\\" LF
         "       ValueS2V2L2" LF
         "" LF
         "# S2V3C1" LF
         "V3= ValueS2V3" LF ;

        std::ofstream outputFile;
        ALIB_STRINGS_TO_NARROW(sampleIniPath, nSampleIniPath, 256)
        outputFile.open(nSampleIniPath);
        outputFile << contents;
        outputFile.close();

      contents=
         "/// SampleFile2" LF
         "" LF
         "/// SectionFile2 Comment1" LF
         "[SectionFile2]" LF
         "V1= ValueS2F2V1" LF
         "" LF                ;
        ALIB_STRINGS_TO_NARROW(sampleIniPath2, nSampleIniPath2, 256)
        outputFile.open(nSampleIniPath2);
        outputFile << contents;
        outputFile.close();
    }

    config::IniFile inif(sampleIniPath);
    UT_EQ(  5, inif.FileComments.Count(NEW_LINE))
    UT_TRUE(inif.FileComments.EndsWith(String32("Last file comment") << NEW_LINE))

    UT_EQ(  7, inif.Count() )
    UT_EQ(  3, inif.Sections.Count() )
    auto* section= inif.SearchSection(A_CHAR(""));       UT_TRUE( section != nullptr)
                                                         UT_TRUE( section == &inif.Sections.Front() )
    auto* var = &section->Entries.Front();               UT_TRUE( var != nullptr)
                                                         UT_TRUE( var == inif.SearchEntry(section->Name, A_CHAR("V1")).second )
                                                         UT_EQ  ( A_CHAR("= ValueS0V1"), var->RawValue)
                                                         UT_EQ  ( A_CHAR("ValueS0V1"  ), var->Value)
    section   = inif.SearchSection(A_CHAR("Section1"));  UT_TRUE( section != nullptr)
    auto secIt= inif.Sections.begin(); ++secIt;          UT_TRUE( section == &*secIt )
    section   = inif.SearchSection(A_CHAR("Section2"));  UT_TRUE( section != nullptr)
    secIt     = inif.Sections.begin(); ++secIt; ++secIt; UT_TRUE( section == &*secIt )

    Path writePath(alib::lang::system::SystemFolders::Temp); writePath.Add(A_PATH("test1.cpy.ini"));
    inif.Write(writePath);


    AString sysCallBuf;
    systemCall(NString1K("diff --ignore-blank-lines  ") << sampleIniPath << " " << writePath, sysCallBuf );
    UT_PRINT( "DIFF: ", sysCallBuf )
    UT_TRUE(sysCallBuf.IsEmpty())

    // --------- Add a section --------------
    section          = inif.CreateSection(A_CHAR("Programmed"));
    section->Comments.Allocate(inif.Allocator, A_CHAR("\n# SProgrammed Comment\n"));
    UT_EQ(  4, inif.Sections.Count() )
    UT_EQ(  0, section->Entries.Count() )
    UT_EQ(  7, inif.Count() ) // remains 7

    var          = inif.CreateEntry(section, A_CHAR("V1"));
    var->Comments.Allocate(inif.Allocator, A_CHAR("PRV1"));
    var->NewValue.Allocate(inif.Allocator, A_CHAR("=ProgV1Val"));
    UT_EQ(  1, section->Entries.Count() )
    UT_EQ(  8, inif.Count() )

    // search new section
    auto* section2= inif.SearchSection(A_CHAR("Programmed"));                      UT_TRUE( section == section2)
    auto* var2    = inif.SearchEntry  (A_CHAR("Programmed"), A_CHAR("V1")).second; UT_TRUE( var     == var2    )

    // write and make a diff (but its output is not programmatically tested)
    writePath.Change(alib::lang::system::SystemFolders::Temp, A_PATH("test1.added.ini"));
    inif.Write(writePath);
    systemCall(NString1K("diff ") << sampleIniPath << " " << writePath, sysCallBuf );
    UT_PRINT( "DIFF: ", sysCallBuf )


    // --------- Delete an entry --------------
    var = inif.SearchEntry(A_CHAR("Section1"), A_CHAR("V2")).second;     UT_EQ( A_CHAR("V2"), var->Name)
    var2= inif.DeleteEntry(A_CHAR("Section1"), A_CHAR("V2"));            UT_EQ( var , var2)       UT_EQ(  7, inif.Count() )
    var = inif.SearchEntry(A_CHAR("Section1"), A_CHAR("V2")).second;     UT_EQ( nullptr, var )

    // --------- Reset --------------
    inif.Reset();     UT_EQ(  0, inif.Count() )
                      UT_EQ(  0, inif.Sections.Count() )

    inif.Read(writePath);       UT_EQ(  8, inif.Count() )
                                UT_EQ(  4, inif.Sections.Count() )

    // read a second INI-file
    inif.Read(sampleIniPath2);  UT_EQ(  9, inif.Count() )
                                UT_EQ(  5, inif.Sections.Count() )
    UT_EQ(String128("/// SampleFile2") << LF, inif.FileComments)
    writePath.Change(alib::lang::system::SystemFolders::Temp, A_PATH("test1.merged.ini"));
    inif.Write(writePath);

    // thats it for now
    #undef LF

}

UT_METHOD(ConfigIniFiles)
{
    UT_INIT()

    // write sample config file
    UT_PRINT("") UT_PRINT( "### Configuration with IniFile ###" )
    const char* iniFileContents=
    "##########################################################################"  "\n"
    "## unit test config file"                                                    "\n"
    "##########################################################################"  "\n"
    "// this is also a comment"                                                   "\n"
    "; and this is as well"                                                       "\n"
    ""                                                                            "\n"
    "HOME=     overwritten_by_environment"                                        "\n"
    "HOMEPATH= overwritten_by_environment"                                        "\n"
    ""                                                                            "\n"
    "concat=    start =5,          \\"                                            "\n"
    "           end   =32,       \\"                                              "\n"
    "           \\#no comment,   \\"                                              "\n"
    "           \\;nocomment,   \\"                                               "\n"
    "           ;a comment,   \\"                                                 "\n"
    "           getsLonger,    \\"                                                "\n"
    "           getsLongerxxx,   \\"                                              "\n"
    "           getsshorter,    \\"                                               "\n"
    "           getsLongerxxxxx,  \\"                                             "\n"
    "           getsLongerxxxxxxxxx,  \\"                                         "\n"
    "           getsshorterxx,    \\"                                             "\n"
    "           last"                                                             "\n"
    ""                                                                            "\n"
    ""                                                                            "\n"
    "CUBA=a country"                                                              "\n"
    "# The size "                                                                 "\n"
    " SIZE=  25 "                                                                 "\n"
    ""                                                                            "\n"
    "# doble comment line"                                                        "\n"
    "# double, I meant"                                                           "\n"
    "2Comments= much talk"                                                        "\n"
    ""                                                                            "\n"
    "# A great section"                                                           "\n"
    "[Great Section] "                                                            "\n"
    "SectionVar=5"                                                                "\n"
    "Double=12.3"                                                                 "\n"
    "Tricky=  backslash\\\\"                                                      "\n"
    "OverWritten=No"                                                              "\n"
    ""                                                                            "\n"
    "# A 2nd section"                                                             "\n"
    "[2nd Section] "                                                              "\n"
    "SectionVar=6"                                                                "\n"
    ""                                                                            "\n"
    ""                                                                            "\n"
    "[Great Section] "                                                            "\n"
    "SECTION_CONTINUED   = yEs"                                                   "\n"
    ""                                                                            "\n"
    "OverWritten=Yes"                                                             "\n"
    ""                                                                            "\n"
    "[ESC] "                                                                      "\n"
    "Blanks=  \" x \""                                                            "\n"
    "Tabs=\t\t\\tx\\t"                                                            "\n"
    "nrslash= \"\\n\\r//\\\\\""                                                   "\n"
   ;

    Path sampleIniPath (alib::SystemFolders::Temp, A_PATH("unittest_testiniFile.ini") );
    ALIB_STRINGS_TO_NARROW(sampleIniPath, nFileName, 1024)

    // write sample config file
    {
        ofstream iniFile;
        iniFile.open ( nFileName.Terminate() );
        iniFile << iniFileContents;
        iniFile.close();
    }

    MonoAllocator ma(ALIB_DBG("UTCfgIniF",) 8);
    Configuration cfg(ma);
    Variable vc(cfg);
    IniFileFeeder iniFile(cfg);
    iniFile.ImportStart(nFileName);
    iniFile.ImportAll();
    iniFile.ImportEnd();

    // check some values
    vc.Declare(  A_CHAR("CUBA")   , A_CHAR("S")   );     UT_EQ( A_CHAR("a country"),    vc.GetString()  )
    vc.Declare(  A_CHAR("SIZE")   , A_CHAR("S")   );     UT_EQ( A_CHAR("25"),           vc.GetString()  )
    vc.Declare(  A_CHAR("concat") , A_CHAR("SV,") );     UT_EQ( 11 , vc.Size())

                                                 UT_EQ( A_CHAR("start =5")   ,  vc.GetString(0) )
                                                 UT_EQ( A_CHAR("end   =32")  ,  vc.GetString(1) )
                                                 UT_EQ( A_CHAR("#no comment"),  vc.GetString(2) )
                                                 UT_EQ( A_CHAR(";nocomment") ,  vc.GetString(3) )

    vc.Declare( A_CHAR("ESC/Blanks"  ), A_CHAR("S")  );  UT_EQ( A_CHAR(" x "),          vc.GetString()  )
    vc.Declare( A_CHAR("ESC/Tabs"    ), A_CHAR("S")  );  UT_EQ( A_CHAR("\tx\t"),        vc.GetString()  )
    vc.Declare( A_CHAR("ESC/nrslash" ), A_CHAR("S")  );  UT_EQ( A_CHAR("\n\r//\\"),     vc.GetString()  )

    vc.Declare( A_CHAR("Great Section/SectionVar")       , A_CHAR("S"));   UT_EQ( A_CHAR("5"),            vc.GetString()  )
    vc.Declare( A_CHAR("2nd Section/SectionVar")         , A_CHAR("S"));   UT_EQ( A_CHAR("6"),            vc.GetString()  )
    vc.Declare( A_CHAR("Great Section/SECTION_CONTINUED"), A_CHAR("S"));   UT_EQ( A_CHAR("yEs"),          vc.GetString()  )
    vc.Declare( A_CHAR("Great Section/Tricky")           , A_CHAR("S"));   UT_EQ( A_CHAR("backslash\\"),  vc.GetString()  )
    vc.Declare( A_CHAR("Great Section/OverWritten")      , A_CHAR("S"));   UT_EQ( A_CHAR("Yes"),          vc.GetString()  )



    // check if environment variable "home" overwrites INI file
    const character* HOME_ENV_NAME;
    #if defined(_WIN32)
        HOME_ENV_NAME= A_CHAR("HOmepATH");
    #else
        HOME_ENV_NAME= A_CHAR("HOME");
    #endif

    Variable vIniFile(BASECAMP);
    {ALIB_LOCK_WITH(ALOX.GetConfigLock())
        vIniFile.Declare(HOME_ENV_NAME, A_CHAR("S") ); }
    UT_EQ( Priority::Environment, vIniFile.GetPriority() )
    UT_TRUE( vIniFile.GetString().IsNotEmpty() )
    UT_FALSE( vIniFile.GetString().Equals( A_CHAR("overwritten_by_environment") ) )
    {ALIB_LOCK_SHARED_WITH(ALOX.GetConfigLock())
        UT_PRINT( "Variable {}={!Q}", vIniFile, String(vIniFile) )
    }

    // change a value and write a new one
    {ALIB_LOCK_WITH(ALOX.GetConfigLock())
        vc.Declare( A_CHAR("New Section/newvar"), A_CHAR("S") );
        if( vc.Define() )
            vc= String(A_CHAR("new"));
        vc.Declare( A_CHAR("New Section/newvar"), A_CHAR("S"));
    }
    UT_EQ( A_CHAR("new")     ,  vc.GetString() )
    UT_EQ( Priority::Standard,  vc.GetPriority() )

    // no "category" variable (in old ALib versions, variables had a category/name)
    {ALIB_LOCK_WITH(ALOX.GetConfigLock())
        vc.Declare( A_CHAR("newvar"), A_CHAR("S"));
        if(vc.Define())
            vc= String(A_CHAR("alib"));
        vc.Declare( A_CHAR("newvar"), A_CHAR("S"));
    }
    UT_EQ( A_CHAR("alib")    ,  vc.GetString() )
    UT_EQ( Priority::Standard,  vc.GetPriority() )

    {ALIB_LOCK_WITH(ALOX.GetConfigLock())
        vc.Declare( A_CHAR("newvarList"), A_CHAR("SV,"));
        if(vc.Define())
        {
            vc.Get<StringVectorPA>().Add( A_CHAR("val1=5")     );
            vc.Get<StringVectorPA>().Add( A_CHAR("val2=10")    );
            vc.Get<StringVectorPA>().Add( A_CHAR("val3=hello") );
        }

        vc.Declare( A_CHAR("commented"), A_CHAR("S"), A_CHAR("2lines") );
        if(vc.Define())
            vc= String(A_CHAR("this is c-line 1\nand this line 2") );
    }
    // write the file
    Path sampleIniPathWriteback (alib::SystemFolders::Temp, A_PATH("unittest_testiniFile.writeback.ini") );
    iniFile.ExportStart(sampleIniPathWriteback);
    iniFile.ExportSubTree(A_CHAR("/"));
    iniFile.ExportEnd(sampleIniPathWriteback);

    // Reset config and load the written file into another config
    cfg.Reset();
    iniFile.ImportStart(sampleIniPathWriteback);
    iniFile.ImportAll();
    iniFile.ImportEnd();


    // compare all
    vc.Declare(  A_CHAR("CUBA")   , A_CHAR("S")   );     UT_EQ( A_CHAR("a country"),    vc.GetString()  )
    vc.Declare(  A_CHAR("SIZE")   , A_CHAR("S")   );     UT_EQ( A_CHAR("25"),           vc.GetString()  )
    vc.Declare(  A_CHAR("concat") , A_CHAR("SV,") );     UT_EQ( 11 , vc.Size())

                                                 UT_EQ( A_CHAR("start =5")   ,  vc.GetString(0) )
                                                 UT_EQ( A_CHAR("end   =32")  ,  vc.GetString(1) )
                                                 UT_EQ( A_CHAR("#no comment"),  vc.GetString(2) )
                                                 UT_EQ( A_CHAR(";nocomment") ,  vc.GetString(3) )

    vc.Declare( A_CHAR("ESC/Blanks"  ), A_CHAR("S")  );  UT_EQ( A_CHAR(" x "),          vc.GetString()  )
    vc.Declare( A_CHAR("ESC/Tabs"    ), A_CHAR("S")  );  UT_EQ( A_CHAR("\tx\t"),        vc.GetString()  )
    vc.Declare( A_CHAR("ESC/nrslash" ), A_CHAR("S")  );  UT_EQ( A_CHAR("\n\r//\\"),     vc.GetString()  )

    vc.Declare( A_CHAR("Great Section/SectionVar")       , A_CHAR("S"));   UT_EQ( A_CHAR("5"),            vc.GetString()  )
    vc.Declare( A_CHAR("2nd Section/SectionVar")         , A_CHAR("S"));   UT_EQ( A_CHAR("6"),            vc.GetString()  )
    vc.Declare( A_CHAR("Great Section/SECTION_CONTINUED"), A_CHAR("S"));   UT_EQ( A_CHAR("yEs"),          vc.GetString()  )
    vc.Declare( A_CHAR("Great Section/Tricky")           , A_CHAR("S"));   UT_EQ( A_CHAR("backslash\\"),  vc.GetString()  )
    vc.Declare( A_CHAR("Great Section/OverWritten")      , A_CHAR("S"));   UT_EQ( A_CHAR("Yes"),          vc.GetString()  )


    vc.Declare( A_CHAR("New Section/newvar"), A_CHAR("S") );   UT_EQ( A_CHAR("new")     , vc.GetString() )
    vc.Declare( A_CHAR("newvar")            , A_CHAR("S") );   UT_EQ( A_CHAR("alib")    , vc.GetString() )

    vc.Declare( A_CHAR("newvarList"), A_CHAR("SV,") );
    UT_TRUE(vc.IsDefined())
    {
        UT_EQ( A_CHAR("val1=5")     , vc.GetString(0) )
        UT_EQ( A_CHAR("val2=10")    , vc.GetString(1) )
        UT_EQ( A_CHAR("val3=hello") , vc.GetString(2) )
    }
}

/** ********************************************************************************************
 * ConfigDefaultAndProtected
 **********************************************************************************************/

UT_METHOD(ConfigDefaultAndProtected)
{
    UT_INIT()
    const wchar_t* args[]=
    {
        L"COMMANDLINE",
        L"--TEST_VARIABLE=fromCommandLine",
    };
    auto origArgC = alib::ARG_C;
    auto origArgVN= alib::ARG_VN;
    auto origArgVW= alib::ARG_VW;
    alib::ARG_C  = 2;
    alib::ARG_VN = nullptr;
    alib::ARG_VW = args;

    MonoAllocator ma(ALIB_DBG("UTCfgDAP",) 8);
    Configuration cfg(ma);
    Variable var(cfg);

    // command line
    var.Declare( A_CHAR("TEST_VARIABLE"), A_CHAR("S") );
    UT_EQ( Priority::CLI,  var.GetPriority() )   UT_EQ( A_CHAR("fromCommandLine")    ,var.GetString() )

    // set default, does not overwrite
    if(var.Define(Priority::DefaultValues) )           var= String(A_CHAR("not overwriting"));
    UT_EQ( Priority::CLI,  var.GetPriority() )   UT_EQ( A_CHAR("fromCommandLine")    ,var.GetString() )

    // set standard, does not overwrite
    if(var.Define() ) var= String(A_CHAR("not overwriting"));
    UT_EQ( Priority::CLI,  var.GetPriority() )   UT_EQ( A_CHAR("fromCommandLine")    ,var.GetString() )

    // set protected, overwrites command line
    if(var.Define(alib::config::Priority::Protected) ) var= String(A_CHAR("does overwrite"));
    UT_EQ( Priority::Protected,  var.GetPriority() )   UT_EQ( A_CHAR("does overwrite")    ,var.GetString() )

    // set standard, does not overwrite
    if(var.Define() )                                  var= String(A_CHAR("not overwriting"));
    UT_EQ( Priority::Protected,  var.GetPriority() )   UT_EQ( A_CHAR("does overwrite")    ,var.GetString() )

    alib::ARG_C = origArgC ;
    alib::ARG_VN= origArgVN;
    alib::ARG_VW= origArgVW;
}

/** ********************************************************************************************
 * ConfigSubstitution
 **********************************************************************************************/
UT_METHOD(ConfigSubstitution)
{
    UT_INIT()
    const wchar_t* args[]=
    {
        L"COMMANDLINE",
        L"--REPL_CMDLINE=ReplCommandLine",
        L"--NOCATCMDLINE=NoCatCommandLine",
    };
    auto origArgC = alib::ARG_C;
    auto origArgVN= alib::ARG_VN;
    auto origArgVW= alib::ARG_VW;
    alib::ARG_C  = 3;
    alib::ARG_VN = nullptr;
    alib::ARG_VW = args;


    MonoAllocator ma(ALIB_DBG("UTCfgSubs",) 8);
    Configuration cfg(ma);
    Variable var(cfg);

    // replacements from command line plugin
    var.Declare(A_CHAR("TEST_1"), A_CHAR("S"),  A_CHAR("no replacment")                   ); UT_EQ( A_CHAR("no replacment")                   ,var.GetString() )
    var.Declare(A_CHAR("TEST_2"), A_CHAR("S"),  A_CHAR("${UKN}")                          ); UT_EQ( A_CHAR("")                                ,var.GetString() )
    var.Declare(A_CHAR("TEST_3"), A_CHAR("S"),  A_CHAR("-${UKN} * ${UKN2}-")              ); UT_EQ( A_CHAR("- * -")                           ,var.GetString() )
    var.Declare(A_CHAR("TEST_4"), A_CHAR("S"),  A_CHAR("${NOCATCMDLINE}")                 ); UT_EQ( A_CHAR("NoCatCommandLine")                ,var.GetString() )
    var.Declare(A_CHAR("TEST_5"), A_CHAR("S"),  A_CHAR("${${NOCATCMDLINE}$")              ); UT_EQ( A_CHAR("$")                               ,var.GetString() )

    var.Declare(A_CHAR("TEST_6"), A_CHAR("S"),  A_CHAR("${REPL_CMDLINE}")                 ); UT_EQ( A_CHAR("ReplCommandLine")                 ,var.GetString() )
    var.Declare(A_CHAR("TEST_8"), A_CHAR("S"),  A_CHAR("${repL_CmdLine}")                 ); UT_EQ( A_CHAR("ReplCommandLine")                 ,var.GetString() )
    var.Declare(A_CHAR("TEST_9"), A_CHAR("S"),  A_CHAR("${repL_CmdLine}${repL_CmdLine}")  ); UT_EQ( A_CHAR("ReplCommandLineReplCommandLine")  ,var.GetString() )
    var.Declare(A_CHAR("TEST_0"), A_CHAR("S"),  A_CHAR("${repL_CmdLine} ${repL_CmdLine}" )); UT_EQ( A_CHAR("ReplCommandLine ReplCommandLine") ,var.GetString() )
                                                                      
    // nested variables                                               
    var.Declare(A_CHAR("Rep_Var2") ,A_CHAR("S"), A_CHAR("nested")        );
    var.Declare(A_CHAR("Rep_Var1") ,A_CHAR("S"), A_CHAR("${Rep_Var2}")   );
    var.Declare(A_CHAR("TEST_11" ) ,A_CHAR("S"), A_CHAR("${Rep_Var2}")   ); UT_EQ( A_CHAR("nested")  ,var.GetString() )
    var.Declare(A_CHAR("TEST_12" ) ,A_CHAR("S"), A_CHAR("${Rep_Var1}")   ); UT_EQ( A_CHAR("nested")  ,var.GetString() )

    // substitution with presets
    cfg.PresetImportString(A_CHAR("PRESET1")        , A_CHAR("result")    );
    cfg.PresetImportString(A_CHAR("PRESET2")        , A_CHAR("${PRESET1}")   );
    var.Declare(A_CHAR("TEST_14")  , A_CHAR("S")    , A_CHAR("${PRESET2}")   );
    UT_EQ(A_CHAR("result") , var.GetString() )

    // illegal recursion: All variables are empty
    var.Declare(A_CHAR("Rep_VarN1"), A_CHAR("S"), A_CHAR("${Rep_VarN2}")   ); UT_PRINT( "Rep_VarN1={!Q}", var.GetString() )
    var.Declare(A_CHAR("Rep_VarN2"), A_CHAR("S"), A_CHAR("${Rep_VarN1}")   ); UT_PRINT( "Rep_VarN2={!Q}", var.GetString() )
    var.Declare(A_CHAR("TEST_13")  , A_CHAR("S"), A_CHAR("${Rep_VarN1}")   ); UT_PRINT( "TEST_13  ={!Q}", var.GetString() )

    // Recursion with presets
    cfg.PresetImportString(A_CHAR("RecursivePreset"), A_CHAR("${RecursivePreset}")   );
    var.Declare(A_CHAR("RECPRE")  , A_CHAR("S")    , A_CHAR("${RecursivePreset}")   );
    UT_EQ(A_CHAR("") , var.GetString() )

    // custom variables
    var.Declare(A_CHAR("REP_CUST") ,A_CHAR("S"),  A_CHAR("cf")              );
    var.Declare(A_CHAR("TEST_15")  ,A_CHAR("S"),  A_CHAR(">>${REP_CUST}<<") );     UT_EQ( A_CHAR(">>cf<<") ,var.GetString() )

    cfg.SubstitutionVariableStart= A_CHAR("$");
    cfg.SubstitutionVariableEnd=   nullptr;
    var.Declare(A_CHAR("TEST_VARIABLE"), A_CHAR("S"), A_CHAR(">>$REP_CUST<<"));    UT_EQ( A_CHAR(">>cf<<") ,var.GetString() )

    cfg.SubstitutionVariableStart= A_CHAR("€€€-");
    cfg.SubstitutionVariableEnd=   A_CHAR("--");
    
    var.Import(A_CHAR(">>€€€-REP_CUST--<<")          , Priority::Protected ); UT_EQ( A_CHAR(">>cf<<")       ,var.GetString() )
    var.Import(A_CHAR(">>€€€-REP_CUST--")            , Priority::Protected ); UT_EQ( A_CHAR(">>cf")         ,var.GetString() )
    var.Import(A_CHAR("€€€-REP_CUST--")              , Priority::Protected ); UT_EQ( A_CHAR("cf")           ,var.GetString() )
    var.Import(A_CHAR("€€€-REP_CUST--€€€-REP_CUST--"), Priority::Protected ); UT_EQ( A_CHAR("cfcf")         ,var.GetString() )
    var.Import(A_CHAR("€€-REP_CUST--")               , Priority::Protected ); UT_EQ( A_CHAR("€€-REP_CUST--"),var.GetString() )

    UT_PRINT( A_CHAR("One warning should follow") )
    if(var.Define(Priority::Protected)) var= String(A_CHAR("€€€-REP_CUST-")                ); UT_EQ( A_CHAR("€€€-REP_CUST-"),var.GetString() )

    cfg.SubstitutionVariableStart= A_CHAR("${");
    cfg.SubstitutionVariableEnd  = A_CHAR("}");

    // multi line replacements
    var.Declare(A_CHAR("ML_REPL1") , A_CHAR("SV;"), A_CHAR("repl1-v1;repl1-v2")    ); UT_EQ( 2, var.Size() )
    var.Declare(A_CHAR("ML_REPL2") , A_CHAR("SV;"), A_CHAR("repl2-v1;repl2-v2")    ); UT_EQ( 2, var.Size() )
    var.Declare(A_CHAR("ML_VAR"  ) , A_CHAR("SV;"), A_CHAR("${ML_REPL1};${ML_REPL2}")  );
    UT_EQ( 4, var.Size() )
    UT_EQ( A_CHAR("repl1-v1"), var.GetString(0) )
    UT_EQ( A_CHAR("repl1-v2"), var.GetString(1) )
    UT_EQ( A_CHAR("repl2-v1"), var.GetString(2) )
    UT_EQ( A_CHAR("repl2-v2"), var.GetString(3) )

    alib::ARG_C = origArgC ;
    alib::ARG_VN= origArgVN;
    alib::ARG_VW= origArgVW;
}

/** ********************************************************************************************
 * ConfigListener
 **********************************************************************************************/
UT_METHOD(ConfigListener)
{
    UT_INIT()

    UT_PRINT("------------ listen to specific variables ---------------")
    {
        Listener l(ut);
        SharedConfiguration cfg(10);
        cfg.DbgCriticalSections(lang::Switch::Off);

        Variable listenTo(*cfg);
        listenTo.Declare(A_CHAR("X/LISTEN") , A_CHAR("I")    );
        cfg->MonitorDistinctVariable( lang::ContainerOp::Insert, &l, ConfigurationListener::Event::Definition, listenTo );
        cfg->MonitorDistinctVariable( lang::ContainerOp::Insert, &l, ConfigurationListener::Event::Deletion  , listenTo );

        Variable var(*cfg);                                          UT_EQ( 0, l.qtyDecl)  UT_EQ( 0, l.qtyDef)  UT_EQ( 0, l.qtyDel)
        var.Declare(A_CHAR("Y/LISTEN"), A_CHAR("I")  );             UT_EQ( 0, l.qtyDecl)  UT_EQ( 0, l.qtyDef)  UT_EQ( 0, l.qtyDel)
        (void) var.Define();                                        UT_EQ( 0, l.qtyDecl)  UT_EQ( 0, l.qtyDef)  UT_EQ( 0, l.qtyDel)
        var.Declare(A_CHAR("Z/LISTEN"), A_CHAR("I"), A_CHAR("1") ); UT_EQ( 0, l.qtyDecl)  UT_EQ( 0, l.qtyDef)  UT_EQ( 0, l.qtyDel)
        (void) var.Define();                                        UT_EQ( 0, l.qtyDecl)  UT_EQ( 0, l.qtyDef)  UT_EQ( 0, l.qtyDel)
        var.Declare(A_CHAR("X/LISTEN"), A_CHAR("I")  );             UT_EQ( 0, l.qtyDecl)  UT_EQ( 0, l.qtyDef)  UT_EQ( 0, l.qtyDel)
        (void) var.Define();                                        UT_EQ( 0, l.qtyDecl)  UT_EQ( 1, l.qtyDef)  UT_EQ( 0, l.qtyDel)
        (void) var.Define();                                        UT_EQ( 0, l.qtyDecl)  UT_EQ( 2, l.qtyDef)  UT_EQ( 0, l.qtyDel)
        (void) var.Delete();                                        UT_EQ( 0, l.qtyDecl)  UT_EQ( 2, l.qtyDef)  UT_EQ( 1, l.qtyDel)
        cfg->MonitorDistinctVariable( lang::ContainerOp::Remove, &l, ConfigurationListener::Event::Definition,  listenTo );
        cfg->MonitorDistinctVariable( lang::ContainerOp::Remove, &l, ConfigurationListener::Event::Deletion  ,  listenTo );
    }

    UT_PRINT("------------ listen to named variables ---------------")
    {
        Listener l(ut);
        SharedConfiguration cfg(10);
        cfg.DbgCriticalSections(lang::Switch::Off);
        cfg->MonitorVariablesByName( lang::ContainerOp::Insert, &l , ConfigurationListener::Event::Creation   , A_CHAR("LISTEN"));
        cfg->MonitorVariablesByName( lang::ContainerOp::Insert, &l , ConfigurationListener::Event::Definition , A_CHAR("LISTEN"));
        cfg->MonitorVariablesByName( lang::ContainerOp::Insert, &l , ConfigurationListener::Event::Deletion   , A_CHAR("LISTEN"));
        UT_EQ(3, cfg->MonitorStop(&l) )


        cfg->MonitorVariablesByName( lang::ContainerOp::Insert, &l , ConfigurationListener::Event::Creation   , A_CHAR("LISTEN"));
        cfg->MonitorVariablesByName( lang::ContainerOp::Insert, &l , ConfigurationListener::Event::Definition , A_CHAR("LISTEN"));
        cfg->MonitorVariablesByName( lang::ContainerOp::Insert, &l , ConfigurationListener::Event::Deletion   , A_CHAR("LISTEN"));

        Variable var(*cfg);                                          UT_EQ( 0, l.qtyDecl)  UT_EQ( 0, l.qtyDef)  UT_EQ( 0, l.qtyDel)
        var.Declare(A_CHAR("X"         ), A_CHAR("I")            ); UT_EQ( 0, l.qtyDecl)  UT_EQ( 0, l.qtyDef)  UT_EQ( 0, l.qtyDel)
        var.Declare(A_CHAR("X/LISTEN"  ), A_CHAR("I")            ); UT_EQ( 1, l.qtyDecl)  UT_EQ( 0, l.qtyDef)  UT_EQ( 0, l.qtyDel)
        var.Declare(A_CHAR("X/LISTEN/Z"), A_CHAR("I")            ); UT_EQ( 1, l.qtyDecl)  UT_EQ( 0, l.qtyDef)  UT_EQ( 0, l.qtyDel)
        var.Declare(A_CHAR("Y/LISTEN"  ), A_CHAR("I"),A_CHAR("1")); UT_EQ( 2, l.qtyDecl)  UT_EQ( 1, l.qtyDef)  UT_EQ( 0, l.qtyDel)
        (void) var.Define();                                        UT_EQ( 2, l.qtyDecl)  UT_EQ( 2, l.qtyDef)  UT_EQ( 0, l.qtyDel)
        (void) var.Define();                                        UT_EQ( 2, l.qtyDecl)  UT_EQ( 3, l.qtyDef)  UT_EQ( 0, l.qtyDel)
        (void) var.Delete();                                        UT_EQ( 2, l.qtyDecl)  UT_EQ( 3, l.qtyDef)  UT_EQ( 1, l.qtyDel)
        cfg->MonitorVariablesByName( lang::ContainerOp::Remove, &l , ConfigurationListener::Event::Definition , A_CHAR("LISTEN"));
        var.Declare(A_CHAR("Z/LISTEN"), A_CHAR("I"),A_CHAR("1") );  UT_EQ( 3, l.qtyDecl)  UT_EQ( 3, l.qtyDef)  UT_EQ( 1, l.qtyDel)
        (void) var.Delete();                                        UT_EQ( 3, l.qtyDecl)  UT_EQ( 3, l.qtyDef)  UT_EQ( 2, l.qtyDel)
        cfg->MonitorVariablesByName( lang::ContainerOp::Remove, &l , ConfigurationListener::Event::Creation   , A_CHAR("LISTEN"));
        cfg->MonitorVariablesByName( lang::ContainerOp::Remove, &l , ConfigurationListener::Event::Deletion   , A_CHAR("LISTEN"));
        var.Declare(A_CHAR("W/LISTEN"), A_CHAR("I"), A_CHAR("1") ); UT_EQ( 3, l.qtyDecl)  UT_EQ( 3, l.qtyDef)  UT_EQ( 2, l.qtyDel)
        (void) var.Delete();                                        UT_EQ( 3, l.qtyDecl)  UT_EQ( 3, l.qtyDef)  UT_EQ( 2, l.qtyDel)

    }

    UT_PRINT("------------ listen to a branch of the configuration variable tree (by cursor) ---------------")
    {
        Listener l(ut);
        SharedConfiguration cfg(10);
        cfg.DbgCriticalSections(lang::Switch::Off);
        Variable var(*cfg);
        var.Declare(A_CHAR("X/V1") , A_CHAR("I")    );
        auto cursor= var.AsCursor().Parent();
        cfg->MonitorPath( lang::ContainerOp::Insert, &l, ConfigurationListener::Event::Creation  , cursor );
        cfg->MonitorPath( lang::ContainerOp::Insert, &l, ConfigurationListener::Event::Definition, cursor );
        cfg->MonitorPath( lang::ContainerOp::Insert, &l, ConfigurationListener::Event::Deletion  , cursor );

        (void) var.Define();                                      UT_EQ( 0, l.qtyDecl)  UT_EQ( 1, l.qtyDef)  UT_EQ( 0, l.qtyDel)
        var.Declare(A_CHAR("Y/V1") ,A_CHAR("I")               );  UT_EQ( 0, l.qtyDecl)  UT_EQ( 1, l.qtyDef)  UT_EQ( 0, l.qtyDel)
        var.Declare(A_CHAR("Y/V2") ,A_CHAR("I"), A_CHAR("0")  );  UT_EQ( 0, l.qtyDecl)  UT_EQ( 1, l.qtyDef)  UT_EQ( 0, l.qtyDel)
        var.Declare(A_CHAR("X/V2") ,A_CHAR("I")               );  UT_EQ( 1, l.qtyDecl)  UT_EQ( 1, l.qtyDef)  UT_EQ( 0, l.qtyDel)
        var.Declare(A_CHAR("X/V3") ,A_CHAR("I"), A_CHAR("0")  );  UT_EQ( 2, l.qtyDecl)  UT_EQ( 2, l.qtyDef)  UT_EQ( 0, l.qtyDel)
        (void) var.Define();                                      UT_EQ( 2, l.qtyDecl)  UT_EQ( 3, l.qtyDef)  UT_EQ( 0, l.qtyDel)
        var.Declare(A_CHAR("Z/V1"), A_CHAR("I")               );  UT_EQ( 2, l.qtyDecl)  UT_EQ( 3, l.qtyDef)  UT_EQ( 0, l.qtyDel)
        var.Declare(A_CHAR("X/V1"), A_CHAR("I") , A_CHAR("0") );  UT_EQ( 2, l.qtyDecl)  UT_EQ( 3, l.qtyDef)  UT_EQ( 0, l.qtyDel)
        var.Delete ();                                            UT_EQ( 2, l.qtyDecl)  UT_EQ( 3, l.qtyDef)  UT_EQ( 1, l.qtyDel)
        cfg->MonitorPath( lang::ContainerOp::Remove, &l, ConfigurationListener::Event::Creation  , cursor );
        cfg->MonitorPath( lang::ContainerOp::Remove, &l, ConfigurationListener::Event::Definition, cursor );
        cfg->MonitorPath( lang::ContainerOp::Remove, &l, ConfigurationListener::Event::Deletion  , cursor );
        var.Declare(A_CHAR("X/V4") ,A_CHAR("I") ,A_CHAR("0"));    UT_EQ( 2, l.qtyDecl)  UT_EQ( 3, l.qtyDef)  UT_EQ( 1, l.qtyDel)
        var.Delete ();                                            UT_EQ( 2, l.qtyDecl)  UT_EQ( 3, l.qtyDef)  UT_EQ( 1, l.qtyDel)
    }

    UT_PRINT("------------ listen to a branch of the configuration variable tree (by path string) ---------------")
    {
        Listener l(ut);
        SharedConfiguration cfg(10);
        cfg.DbgCriticalSections(lang::Switch::Off);
        Variable var(*cfg);
        var.Declare(A_CHAR("X/V1") , A_CHAR("I")    );
        cfg->MonitorPathPrefix( lang::ContainerOp::Insert, &l, ConfigurationListener::Event::Creation   , A_CHAR("/X") );
        cfg->MonitorPathPrefix( lang::ContainerOp::Insert, &l, ConfigurationListener::Event::Definition , A_CHAR( "X") );
        cfg->MonitorPathPrefix( lang::ContainerOp::Insert, &l, ConfigurationListener::Event::Deletion   , A_CHAR( "X") );
                                                                      // ^^^ Missing slash is ignored
        (void) var.Define();                                       UT_EQ ( 0, l.qtyDecl)UT_EQ ( 1, l.qtyDef)UT_EQ ( 0, l.qtyDel)
        var.Declare(A_CHAR("Y/V1") , A_CHAR("I")        );         UT_EQ ( 0, l.qtyDecl)UT_EQ ( 1, l.qtyDef)UT_EQ ( 0, l.qtyDel)
        var.Declare(A_CHAR("Y/V2") , A_CHAR("I") , A_CHAR("0")  ); UT_EQ ( 0, l.qtyDecl)UT_EQ ( 1, l.qtyDef)UT_EQ ( 0, l.qtyDel)
        var.Declare(A_CHAR("X/V2") , A_CHAR("I")        );         UT_EQ ( 1, l.qtyDecl)UT_EQ ( 1, l.qtyDef)UT_EQ ( 0, l.qtyDel)
        var.Declare(A_CHAR("X/V3") , A_CHAR("I") , A_CHAR("0")  ); UT_EQ ( 2, l.qtyDecl)UT_EQ ( 2, l.qtyDef)UT_EQ ( 0, l.qtyDel)
        (void) var.Define();                                       UT_EQ ( 2, l.qtyDecl)UT_EQ ( 3, l.qtyDef)UT_EQ ( 0, l.qtyDel)
        var.Declare(A_CHAR("Z/V1") , A_CHAR("I")        );         UT_EQ ( 2, l.qtyDecl)UT_EQ ( 3, l.qtyDef)UT_EQ ( 0, l.qtyDel)
        var.Declare(A_CHAR("X/V1") , A_CHAR("I") , A_CHAR("0")  ); UT_EQ ( 2, l.qtyDecl)UT_EQ ( 3, l.qtyDef)UT_EQ ( 0, l.qtyDel)
        var.Delete ();                                             UT_EQ ( 2, l.qtyDecl)UT_EQ ( 3, l.qtyDef)UT_EQ ( 1, l.qtyDel)
        cfg->MonitorPathPrefix( lang::ContainerOp::Remove, &l, ConfigurationListener::Event::Creation   , A_CHAR("/X") );
        cfg->MonitorPathPrefix( lang::ContainerOp::Remove, &l, ConfigurationListener::Event::Definition , A_CHAR( "X") );
        cfg->MonitorPathPrefix( lang::ContainerOp::Remove, &l, ConfigurationListener::Event::Deletion   , A_CHAR( "X") );
        var.Declare(A_CHAR("X/V4") , A_CHAR("I") , A_CHAR("0")  ); UT_EQ ( 2, l.qtyDecl)UT_EQ ( 3, l.qtyDef)UT_EQ ( 1, l.qtyDel)
        var.Delete();                                              UT_EQ ( 2, l.qtyDecl)UT_EQ ( 3, l.qtyDef)UT_EQ ( 1, l.qtyDel)
    }

    UT_PRINT("------------ listen to variables that contain a substring in their full path ---------------")
    {
        Listener l(ut);
        SharedConfiguration cfg(10);
        cfg.DbgCriticalSections(lang::Switch::Off);
        Variable var(*cfg);
        var.Declare(A_CHAR("X/V1") , A_CHAR("I") );
        cfg->MonitorPathSubstring( lang::ContainerOp::Insert, &l, ConfigurationListener::Event::Creation  , A_CHAR("X/") );
        cfg->MonitorPathSubstring( lang::ContainerOp::Insert, &l, ConfigurationListener::Event::Definition, A_CHAR("X/") );
        cfg->MonitorPathSubstring( lang::ContainerOp::Insert, &l, ConfigurationListener::Event::Deletion  , A_CHAR("X/") );

        (void) var.Define();                                        UT_EQ ( 0, l.qtyDecl)UT_EQ ( 1, l.qtyDef)UT_EQ ( 0, l.qtyDel)
        var.Declare(A_CHAR("Y/V1") , A_CHAR("I")        );          UT_EQ ( 0, l.qtyDecl)UT_EQ ( 1, l.qtyDef)UT_EQ ( 0, l.qtyDel)
        var.Declare(A_CHAR("Y/V2") , A_CHAR("I") , A_CHAR("0"));    UT_EQ ( 0, l.qtyDecl)UT_EQ ( 1, l.qtyDef)UT_EQ ( 0, l.qtyDel)
        var.Declare(A_CHAR("X/V2") , A_CHAR("I")        );          UT_EQ ( 1, l.qtyDecl)UT_EQ ( 1, l.qtyDef)UT_EQ ( 0, l.qtyDel)
        var.Declare(A_CHAR("X/V3") , A_CHAR("I") , A_CHAR("0"));    UT_EQ ( 2, l.qtyDecl)UT_EQ ( 2, l.qtyDef)UT_EQ ( 0, l.qtyDel)
        (void) var.Define();                                        UT_EQ ( 2, l.qtyDecl)UT_EQ ( 3, l.qtyDef)UT_EQ ( 0, l.qtyDel)
        var.Declare(A_CHAR("Z/V1"), A_CHAR("I")        );           UT_EQ ( 2, l.qtyDecl)UT_EQ ( 3, l.qtyDef)UT_EQ ( 0, l.qtyDel)
        var.Declare(A_CHAR("X/V1"), A_CHAR("I") , A_CHAR("0")  );   UT_EQ ( 2, l.qtyDecl)UT_EQ ( 3, l.qtyDef)UT_EQ ( 0, l.qtyDel)
        var.Delete();                                               UT_EQ ( 2, l.qtyDecl)UT_EQ ( 3, l.qtyDef)UT_EQ ( 1, l.qtyDel)
        cfg->MonitorPathSubstring( lang::ContainerOp::Remove, &l, ConfigurationListener::Event::Creation  , A_CHAR("X/") );
        cfg->MonitorPathSubstring( lang::ContainerOp::Remove, &l, ConfigurationListener::Event::Definition, A_CHAR("X/") );
        cfg->MonitorPathSubstring( lang::ContainerOp::Remove, &l, ConfigurationListener::Event::Deletion  , A_CHAR("X/") );
        var.Declare(A_CHAR("X/V4"), A_CHAR("I") ,A_CHAR("0")  );    UT_EQ ( 2, l.qtyDecl)UT_EQ ( 3, l.qtyDef)UT_EQ ( 1, l.qtyDel)
        var.Delete();                                               UT_EQ ( 2, l.qtyDecl)UT_EQ ( 3, l.qtyDef)UT_EQ ( 1, l.qtyDel)

        cfg->MonitorPathSubstring( lang::ContainerOp::Insert, &l, ConfigurationListener::Event::Creation  , A_CHAR("/V") );
        cfg->MonitorPathSubstring( lang::ContainerOp::Insert, &l, ConfigurationListener::Event::Definition, A_CHAR("/V") );
        cfg->MonitorPathSubstring( lang::ContainerOp::Insert, &l, ConfigurationListener::Event::Deletion  , A_CHAR("/V") );
        var.Declare(A_CHAR("A/V1") , A_CHAR("I")        );          UT_EQ ( 3, l.qtyDecl)UT_EQ ( 3, l.qtyDef)UT_EQ ( 1, l.qtyDel)
        var.Declare(A_CHAR("B/V2") , A_CHAR("I") , A_CHAR("0")  );  UT_EQ ( 4, l.qtyDecl)UT_EQ ( 4, l.qtyDef)UT_EQ ( 1, l.qtyDel)
        (void) var.Define();                                        UT_EQ ( 4, l.qtyDecl)UT_EQ ( 5, l.qtyDef)UT_EQ ( 1, l.qtyDel)
        (void) var.Delete();                                        UT_EQ ( 4, l.qtyDecl)UT_EQ ( 5, l.qtyDef)UT_EQ ( 2, l.qtyDel)
        cfg->MonitorPathSubstring( lang::ContainerOp::Remove, &l, ConfigurationListener::Event::Creation  , A_CHAR("/V") );
        cfg->MonitorPathSubstring( lang::ContainerOp::Remove, &l, ConfigurationListener::Event::Definition, A_CHAR("/V") );
    }
}


#include "unittests/aworx_unittests_end.hpp"

} //namespace

#endif // ALIB_UT_CONFIGURATION
