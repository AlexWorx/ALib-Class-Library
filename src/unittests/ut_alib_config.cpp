// #################################################################################################
//  AWorx ALib Unit Tests
//
//  Copyright 2013-2023 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"
#include "unittests/alib_test_selection.hpp"
#if ALIB_UT_CONFIGURATION

#include "alib/lib/fs_modules/distribution.hpp"
#include "alib/alox.hpp"

#if ALIB_ALOX && !defined (HPP_ALIB_ALOXMODULE)
#   include "alib/alox/aloxmodule.hpp"
#endif
#if !defined (HPP_ALIB_CONFIG_INI_FILE)
#   include "alib/config/inifile.hpp"
#endif
#if !defined (HPP_ALIB_CONFIG_CONFIGURATION)
#   include "alib/config/configuration.hpp"
#endif
#if !defined (HPP_ALIB_SYSTEM_DIRECTORY)
#   include "alib/system/directory.hpp"
#endif
#if !defined (HPP_ALIB_STRINGS_FORMAT)
#    include "alib/strings/format.hpp"
#endif

#if !defined (HPP_ALIB_MONOMEM_MONOALLOCATOR)
#   include "alib/monomem/monoallocator.hpp"
#endif

#include "alib/lib/fs_commonenums/commonenumdefs_aliased.hpp"


#define TESTCLASSNAME       CPP_ALib_Config
#include "unittests/aworx_unittests.hpp"


#include <iostream>
#include <fstream>

using namespace std;
using namespace aworx;

namespace ut_aworx {


UT_CLASS

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

    Configuration cfg(CreateDefaults::Yes);
    cfg.SetCommandLineArgs( sizeof(args)/sizeof(const char*), const_cast<const char**>( args ) );

    Variable var;
    UT_EQ( Priorities::CLI,    cfg.Load   ( var.Declare( EmptyString(),      A_CHAR("SingleHyphen") )) )   UT_EQ( A_CHAR("12"),         var.GetString()   )
                               cfg.Load   ( var.Declare( EmptyString(),      A_CHAR("DoubleHyphen") ));    UT_EQ( true,                 var.IsTrue()      )
    UT_EQ( Priorities::CLI,    var.Priority())

    UT_EQ( Priorities::CLI,    cfg.Load   ( var.Declare( EmptyString(),      A_CHAR("Empty")        )) )   UT_EQ( EmptyString(),        var.GetString()   )
    UT_EQ( Priorities::CLI,    cfg.Load   ( var.Declare( EmptyString(),      A_CHAR("Whitespaces")  )) )   UT_EQ( A_CHAR("Hello Test"), var.GetString()   )
    UT_EQ( Priorities::CLI,    cfg.Load   ( var.Declare( EmptyString(),      A_CHAR("HOME")         )) )   UT_EQ( A_CHAR("overwritten"),var.GetString()   )
    UT_EQ( Priorities::CLI,    cfg.Load   ( var.Declare( EmptyString(),      A_CHAR("integer")      )) )   UT_EQ( 42,                   var.GetInteger()  )
    UT_EQ( Priorities::NONE,   cfg.Load   ( var.Declare( EmptyString(),      A_CHAR("notexistent")  )) )   UT_EQ( 0,                    var.GetInteger()  )
    UT_EQ( Priorities::CLI,    cfg.Load   ( var.Declare( EmptyString(),      A_CHAR("integer")      )) )   UT_EQ( 42,                   var.GetInteger()  )
    UT_EQ( Priorities::CLI,    cfg.Load   ( var.Declare( EmptyString(),      A_CHAR("double")       )) )
    UT_EQ( 3.14,            var.GetFloat() )
    UT_EQ( Priorities::CLI,    cfg.Load   ( var.Declare( EmptyString(),      A_CHAR("double")       )) )   UT_EQ( 3.14,                 var.GetFloat()    )
    UT_EQ( Priorities::CLI,    cfg.Load   ( var.Declare( A_CHAR("ALIB"),     A_CHAR("test")         )) )   UT_EQ( A_CHAR("passed"),     var.GetString()   )
    UT_EQ( Priorities::NONE,   cfg.Load   ( var.Declare( EmptyString(),      A_CHAR("notexistent")  )) )   UT_EQ( 0.0,                  var.GetFloat()    )

    auto* it= cfg.GetPluginTypeSafe<lib::config::CLIArgs>()->GetIterator(A_CHAR("ALIB"));
    UT_TRUE ( it->Next( var ) ) UT_EQ(String(A_CHAR("ITER")) , var.Name() )  UT_EQ( A_CHAR("x"),        var.GetString()     )
    UT_TRUE ( it->Next( var ) ) UT_EQ(String(A_CHAR("ITER2")), var.Name() )  UT_EQ( A_CHAR("y"),        var.GetString()     )
    UT_TRUE ( it->Next( var ) ) UT_EQ(String(A_CHAR("TEST")) , var.Name() )  UT_EQ( A_CHAR("passed"),   var.GetString()     )
    UT_FALSE( it->Next( var ) )
    delete it;

}

UT_METHOD(ConfigCommandLineArgsWChar)
{
    UT_INIT()


    const wchar_t* args[]=
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

    Configuration cfg(CreateDefaults::Yes);
    cfg.SetCommandLineArgs( sizeof(args)/sizeof(wchar_t*),  args  );

    Variable var;
    UT_EQ( Priorities::CLI,    cfg.Load   ( var.Declare( EmptyString(),      A_CHAR("SingleHyphen") )) )   UT_EQ( A_CHAR("12"),          var.GetString()   )
                               cfg.Load   ( var.Declare( EmptyString(),      A_CHAR("DoubleHyphen") ));    UT_EQ( true,                  var.IsTrue()      )
    UT_EQ( Priorities::CLI,    var.Priority())

    UT_EQ( Priorities::CLI,    cfg.Load   ( var.Declare( EmptyString(),      A_CHAR("Empty")        )) )   UT_EQ( EmptyString(),         var.GetString()   )
    UT_EQ( Priorities::CLI,    cfg.Load   ( var.Declare( EmptyString(),      A_CHAR("Whitespaces")  )) )   UT_EQ( A_CHAR("Hello Test"),  var.GetString()   )
    UT_EQ( Priorities::CLI,    cfg.Load   ( var.Declare( EmptyString(),      A_CHAR("HOME")         )) )   UT_EQ( A_CHAR("overwritten"), var.GetString()   )
    UT_EQ( Priorities::CLI,    cfg.Load   ( var.Declare( EmptyString(),      A_CHAR("integer")      )) )   UT_EQ( 42,                    var.GetInteger()  )
    UT_EQ( Priorities::NONE,   cfg.Load   ( var.Declare( EmptyString(),      A_CHAR("notexistent")  )) )   UT_EQ( 0,                     var.GetInteger()  )
    UT_EQ( Priorities::CLI,    cfg.Load   ( var.Declare( EmptyString(),      A_CHAR("integer")      )) )   UT_EQ( 42,                    var.GetInteger()  )
    UT_EQ( Priorities::CLI,    cfg.Load   ( var.Declare( EmptyString(),      A_CHAR("double")       )) )   UT_EQ( 3.14,                  var.GetFloat()    )
    UT_EQ( Priorities::NONE,   cfg.Load   ( var.Declare( EmptyString(),      A_CHAR("notexistent")  )) )   UT_EQ( 0.0,                   var.GetFloat()    )
    UT_EQ( Priorities::CLI,    cfg.Load   ( var.Declare( EmptyString(),      A_CHAR("double")       )) )   UT_EQ( 3.14,                  var.GetFloat()    )
    UT_EQ( Priorities::CLI,    cfg.Load   ( var.Declare( A_CHAR("ALIB"),     A_CHAR("test")         )) )   UT_EQ( A_CHAR("passed"),      var.GetString()   )

    UT_EQ( Priorities::NONE,   cfg.Load   ( var.Declare( A_CHAR("IGNORE"),   A_CHAR("Home")         )) )
    lib::monomem::AcquireGlobalAllocator(ALIB_CALLER_PRUNED);
        cfg.GetPluginTypeSafe<aworx::lib::config::CLIArgs>()->DefaultCategories.emplace_back(A_CHAR("IGNORE"));
    lib::monomem::ReleaseGlobalAllocator();

    UT_EQ( Priorities::CLI,    cfg.Load   ( var.Declare( A_CHAR("IGNORE"),   A_CHAR("Home")         )) )   UT_EQ( A_CHAR("overwritten"), var.GetString()   )
    UT_EQ( Priorities::NONE,   cfg.Load   ( var.Declare( A_CHAR("IGNORE"),   A_CHAR("Homexyz")      )) )
    cfg.GetPluginTypeSafe<aworx::lib::config::CLIArgs>()->AllowedMinimumShortCut=5;
    UT_EQ( Priorities::NONE,   cfg.Load   ( var.Declare( A_CHAR("IGNORE"),   A_CHAR("Homexyz")      )) )
    cfg.GetPluginTypeSafe<aworx::lib::config::CLIArgs>()->AllowedMinimumShortCut=4;
    UT_EQ( Priorities::CLI,    cfg.Load   ( var.Declare( A_CHAR("IGNORE"),   A_CHAR("Homexyz")      )) )   UT_EQ( A_CHAR("overwritten"), var.GetString()   )


    auto* it= cfg.GetPluginTypeSafe<lib::config::CLIArgs>()->GetIterator(A_CHAR("ALIB"));
    UT_TRUE ( it->Next( var ) ) UT_EQ(String(A_CHAR("ITER")) , var.Name() )   UT_EQ( A_CHAR("x"),        var.GetString()     )
    UT_TRUE ( it->Next( var ) ) UT_EQ(String(A_CHAR("ITER2")), var.Name() )   UT_EQ( A_CHAR("y"),        var.GetString()     )
    UT_TRUE ( it->Next( var ) ) UT_EQ(String(A_CHAR("TEST")) , var.Name() )   UT_EQ( A_CHAR("passed"),   var.GetString()     )
    UT_FALSE( it->Next( var ) )
    delete it;

}

/** ********************************************************************************************
 * ConfigIniFiles
 **********************************************************************************************/

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
    "Blanks2= \" x \" \\"                                                         "\n"
    "         \" y \" "                                                           "\n"
    "Tabs=\t\t\\tx\\t"                                                            "\n"
    "nrslash= \"\\n\\r//\\\\\""                                                   "\n"
   ;

    AString fileName;
    Directory::CurrentDirectory( fileName );
    fileName._("/unittest_testiniFile.cfg");

    // write sample config file
    {
        ofstream iniFile;
        ALIB_STRINGS_TO_NARROW(fileName, nFileName,1024)
        iniFile.open ( nFileName );
        iniFile << iniFileContents;
        iniFile.close();
    }

    Variable var;

    IniFile iniFile(fileName );

    // check some values
    iniFile.Load( var.Declare( EmptyString(),            A_CHAR("CUBA")        ) );   UT_EQ( A_CHAR("a country"),          var.GetString()  )
    iniFile.Load( var.Declare( EmptyString(),            A_CHAR("cUbA")        ) );   UT_EQ( A_CHAR("a country"),          var.GetString()  )
    iniFile.Load( var.Declare( EmptyString(),            A_CHAR("SIZE")        ) );   UT_EQ( A_CHAR("25"),                 var.GetString()  )
    iniFile.Load( var.Declare( EmptyString(),            A_CHAR("concat"), ',' ) );   UT_EQ( 11 , var.Size())
                                                                                      UT_EQ( A_CHAR("start =5")         ,  var.GetString(0) )
                                                                                      UT_EQ( A_CHAR("end   =32")        ,  var.GetString(1) )
                                                                                      UT_EQ( A_CHAR("#no comment")      ,  var.GetString(2) )
                                                                                      UT_EQ( A_CHAR(";nocomment")       ,  var.GetString(3) )

    iniFile.Load( var.Declare( A_CHAR("ESC"),            A_CHAR("Blanks")      ) );   UT_EQ( A_CHAR(" x "),                var.GetString()  )
    iniFile.Load( var.Declare( A_CHAR("ESC"),            A_CHAR("Blanks2")     ) );   UT_EQ( A_CHAR(" x  y "),             var.GetString()  )
    iniFile.Load( var.Declare( A_CHAR("ESC"),            A_CHAR("Tabs")        ) );   UT_EQ( A_CHAR("\tx\t"),              var.GetString()  )
    iniFile.Load( var.Declare( A_CHAR("ESC"),            A_CHAR("nrslash")     ) );   UT_EQ( A_CHAR("\n\r//\\"),           var.GetString()  )

    iniFile.Load( var.Declare( A_CHAR("Great Section"),  A_CHAR("SectionVar")         ) );   UT_EQ( A_CHAR("5"),           var.GetString()  )
    iniFile.Load( var.Declare( A_CHAR("2nd Section"),    A_CHAR("SectionVar")         ) );   UT_EQ( A_CHAR("6"),           var.GetString()  )
    iniFile.Load( var.Declare( A_CHAR("Great Section"),  A_CHAR("SECTION_CONTINUED")  ) );   UT_EQ( A_CHAR("yEs"),         var.GetString()  )
    iniFile.Load( var.Declare( A_CHAR("Great Section"),  A_CHAR("Tricky")             ) );   UT_EQ( A_CHAR("backslash\\"), var.GetString()  )
    iniFile.Load( var.Declare( A_CHAR("Great Section"),  A_CHAR("Overwritten")        ) );   UT_EQ( A_CHAR("Yes"),         var.GetString()  )


    // add it to ALIB config
    ALIB.GetConfig().InsertPlugin( &iniFile, Priorities::Standard );
    ALIB.GetConfig().Load( var.Declare( EmptyString(),            A_CHAR("CUBA")              ) );   UT_EQ( A_CHAR("a country")  , var.GetString() )
    ALIB.GetConfig().Load( var.Declare( EmptyString(),            A_CHAR("cUbA")              ) );   UT_EQ( A_CHAR("a country")  , var.GetString() )
    ALIB.GetConfig().Load( var.Declare( EmptyString(),            A_CHAR("SIZE")              ) );   UT_EQ( A_CHAR("25")         , var.GetString() )
    ALIB.GetConfig().Load( var.Declare( EmptyString(),            A_CHAR("concat"), ','       ) );   UT_EQ( 11 , var.Size())
                                                                                                 UT_EQ( A_CHAR("start =5")   , var.GetString(0))
                                                                                                 UT_EQ( A_CHAR("end   =32")  , var.GetString(1))
    ALIB.GetConfig().Load( var.Declare( A_CHAR("Great Section"),  A_CHAR("SectionVar")        ) );   UT_EQ( A_CHAR("5")          , var.GetString() )
    ALIB.GetConfig().Load( var.Declare( A_CHAR("2nd Section"),    A_CHAR("SectionVar")        ) );   UT_EQ( A_CHAR("6")          , var.GetString() )
    ALIB.GetConfig().Load( var.Declare( A_CHAR("Great Section"),  A_CHAR("SECTION_CONTINUED") ) );   UT_EQ( A_CHAR("yEs")        , var.GetString() )
    ALIB.GetConfig().Load( var.Declare( A_CHAR("Great Section"),  A_CHAR("Tricky")            ) );   UT_EQ( A_CHAR("backslash\\"), var.GetString() )
    ALIB.GetConfig().Load( var.Declare( A_CHAR("Great Section"),  A_CHAR("SECTION_CONTINUED") ) );   UT_TRUE( var.IsTrue() )


    // check if environment variable "home" overwrites INI file
    const character* HOME_ENV_NAME;
    #if defined(_WIN32)
        HOME_ENV_NAME= A_CHAR("HOmepATH");
    #else
        HOME_ENV_NAME= A_CHAR("HOME");
    #endif

    Variable vIniFile;   iniFile.Load( vIniFile.Declare( EmptyString(), HOME_ENV_NAME) );                 UT_EQ( A_CHAR("overwritten_by_environment"), vIniFile.GetString() )
    ALIB.GetConfig().Load  ( var.Declare(EmptyString(), HOME_ENV_NAME) ); UT_EQ( Priorities::Environment, var.Priority() )
    UT_TRUE( var.GetString().IsNotEmpty() )
    UT_TRUE( !vIniFile.GetString().Equals( var.GetString() ) )


    // change a value and write a new one
    var.Declare( A_CHAR("New Section"),  A_CHAR("newvar"));
    var.SetPriority( Priorities::Standard );

    UT_EQ( Priorities::Standard, ALIB.GetConfig().Store( var, A_CHAR("new") ) )
    ALIB.GetConfig().Load  ( var.Declare(A_CHAR("New Section"),  A_CHAR("newvar")) );  UT_EQ( A_CHAR("new"),   var.GetString() )

    var.Declare( EmptyString(),             A_CHAR("newvar"));
    var.SetPriority( Priorities::Standard );
    UT_EQ( Priorities::Standard, ALIB.GetConfig().Store( var, A_CHAR("aworx")) )
    ALIB.GetConfig().Load  ( var.Declare(EmptyString(),             A_CHAR("newvar")) );  UT_EQ( A_CHAR("aworx"), var.GetString() )


    var.Declare( EmptyString(),   A_CHAR("newvarList"), ',');
    var.Add(A_CHAR("val1=5"));
    var.Add(A_CHAR("val2=10"));
    var.Add(A_CHAR("val3=hello"));
    var.SetPriority( Priorities::Standard );
    UT_EQ( Priorities::Standard, ALIB.GetConfig().Store(var) )
    ALIB.GetConfig().Load (  var.Declare( EmptyString(),  A_CHAR("newvarList"))   );


    var.Declare( EmptyString(),   A_CHAR("commented"), ',', A_CHAR("2lines"));
    var.SetPriority( Priorities::Standard );
    UT_EQ( Priorities::Standard, ALIB.GetConfig().Store(  var, A_CHAR("this is c-line 1\nand this line 2") ) )

    // iterate
    ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
    {
        auto it= iniFile.GetIterator(A_CHAR("Great Section"));
        int cntVars= 0;
        bool varFound[5] = {false,false,false,false,false};
        while( it->Next( var ) )
        {
            ++cntVars;
            if( String(A_CHAR("SectionVar")       ).Equals( var.Name() ) ) {  varFound[0]|= true; UT_EQ( 5                    , var.GetInteger() )  }
            if( String(A_CHAR("Double")           ).Equals( var.Name() ) ) {  varFound[1]|= true; UT_EQ(A_CHAR("12.3")        , var.GetString()  )  }
            if( String(A_CHAR("Tricky")           ).Equals( var.Name() ) ) {  varFound[2]|= true; UT_EQ( A_CHAR("backslash\\"), var.GetString()  )  }
            if( String(A_CHAR("OverWritten")      ).Equals( var.Name() ) ) {  varFound[3]|= true; UT_EQ( A_CHAR("Yes")        , var.GetString()  )  }
            if( String(A_CHAR("SECTION_CONTINUED")).Equals( var.Name() ) ) {  varFound[4]|= true; UT_EQ( A_CHAR("yEs")        , var.GetString()  )  }
        }
        UT_EQ( 5, cntVars )
        for( int i= 0; i != 5; ++i )
            UT_TRUE( varFound[i] )

        delete it;
    }
    ALIB_WARNINGS_RESTORE

    // write the file
    iniFile.FileName._(".writeback.txt");
    iniFile.WriteFile();

    // load the written file into another config
    IniFile readBack( iniFile.FileName );
    Variable varBack;

    // compare all
    {
        String512 msg;
        for ( auto& section : iniFile.Sections() )
        {
            for ( auto& entry : section.Entries() )
            {
                msg.Reset( "Reading variable " )
                ._(Format::Field( String256() << section.Name() << '/' << entry.Name(), 40, Alignment::Left) );
                UT_PRINT( msg )

                character delim= '\0';
                if(     entry.Name().Equals(A_CHAR("concat"))
                    ||  entry.Name().Equals(A_CHAR("newvarList"))       )
                    delim= ',';

                iniFile .Load( var    .Declare( section.Name(), entry.Name(), delim) );
                readBack.Load( varBack.Declare( section.Name(), entry.Name(), delim) );

                UT_EQ( var.Size(), varBack.Size() )
                for ( int i= 0; i< var.Size(); ++i )
                {
                    integer idx= var.GetString(i).IndexOf('=');
                    if( idx < 0 )
                    {
                        UT_EQ( var.GetString(i), varBack.GetString(i) )
                    }
                    else
                    {
                        integer idxBack= varBack.GetString(i).IndexOf('=');
                        Substring orig = var    .GetString(i).Substring( 0, idx     );
                        Substring back = varBack.GetString(i).Substring( 0, idxBack );
                        UT_EQ( orig.Trim(), back.Trim() )
                        orig= var    .GetString(i).Substring( idx     +1 );
                        back= varBack.GetString(i).Substring( idxBack +1 );
                        UT_EQ( orig.Trim(), back.Trim() )
                    }
                }
            }
        }
    }

    readBack.Load ( var.Declare( A_CHAR("New Section"),  A_CHAR("newvar") ) );   UT_EQ( A_CHAR("new")      , var.GetString() )
    readBack.Load ( var.Declare( EmptyString(),          A_CHAR("newvar") ) );   UT_EQ( A_CHAR("aworx")    , var.GetString() )


    ALIB.GetConfig().RemovePlugin( &iniFile );


    ALIB.GetConfig().InsertPlugin( &readBack, Priorities::Standard );
    ALIB.GetConfig().Load( var.Declare( A_CHAR("New Section"),  A_CHAR("newvar")  ) );   UT_EQ( A_CHAR("new")  , var.GetString() )
    ALIB.GetConfig().Load( var.Declare( EmptyString(),          A_CHAR("newvar")  ) );   UT_EQ( A_CHAR("aworx"), var.GetString() )

    ALIB.GetConfig().RemovePlugin( &readBack );
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

    Configuration cfg(CreateDefaults::Yes);
    cfg.SetCommandLineArgs( 2, args  );
    Variable var;

    InMemoryPlugin& defaultValues  = *cfg.GetPluginTypeSafe<aworx::InMemoryPlugin>( Priorities::DefaultValues   );
    InMemoryPlugin& protectedValues= *cfg.GetPluginTypeSafe<aworx::InMemoryPlugin>( Priorities::ProtectedValues );

    // command line
    UT_EQ( Priorities::CLI,    cfg.Load  ( var.Declare( A_CHAR("TEST"),      A_CHAR("VARIABLE") ) ) )   UT_EQ( A_CHAR("fromCommandLine")    ,var.GetString() )

    // set default, does not overwrite
    var.Declare( A_CHAR("TEST"), A_CHAR("VARIABLE"));  var.Add(A_CHAR("not overwriting"));   defaultValues.Store( var );
    UT_EQ( Priorities::CLI,    cfg.Load  ( var.Declare( A_CHAR("TEST"),      A_CHAR("VARIABLE") ) ) )   UT_EQ( A_CHAR("fromCommandLine")    ,var.GetString() )

    // set protected, overwrites command line
    var.Declare( A_CHAR("TEST"), A_CHAR("VARIABLE"));  var.Add(A_CHAR("does overwrite"));   protectedValues.Store( var );
    UT_EQ( Priorities::ProtectedValues,  cfg.Load  ( var.Declare( A_CHAR("TEST"),      A_CHAR("VARIABLE") ) ) )   UT_EQ( A_CHAR("does overwrite")     ,var.GetString() )

    // set default, something else
    var.Declare( A_CHAR("TEST"), A_CHAR("VAR2"));      var.Add(A_CHAR("this is var 2"));   defaultValues.Store( var );
    UT_EQ( Priorities::DefaultValues,    cfg.Load  ( var.Declare( A_CHAR("TEST"),      A_CHAR("VAR2")     ) ) )   UT_EQ( A_CHAR("this is var 2")      ,var.GetString() )

    // set and remove an entry using plugin interface
    var.Declare( A_CHAR("TEST"), A_CHAR("Remove") );    UT_EQ( 0, var.Size() )     UT_EQ( Priorities::NONE ,var.Priority() )
    defaultValues.Load( var );                          UT_EQ( 0, var.Size() )     UT_EQ( Priorities::NONE ,var.Priority() )
    var.Add(A_CHAR("To be deleted"));                   UT_EQ( 1, var.Size() )     UT_EQ( Priorities::NONE ,var.Priority() )
    defaultValues.Store( var );                         UT_EQ( 1, var.Size() )     UT_EQ( Priorities::NONE ,var.Priority() )
    var.Declare( A_CHAR("TEST"), A_CHAR("Remove") );    UT_EQ( 0, var.Size() )     UT_EQ( Priorities::NONE ,var.Priority() )
    defaultValues.Load( var );                          UT_EQ( 1, var.Size() )     UT_EQ( Priorities::NONE ,var.Priority() )
    var.ClearValues();                                  UT_EQ( 0, var.Size() )     UT_EQ( Priorities::NONE ,var.Priority() )
    defaultValues.Store( var );                         UT_EQ( 0, var.Size() )     UT_EQ( Priorities::NONE ,var.Priority() )
    var.Declare( A_CHAR("TEST"), A_CHAR("Remove") );    UT_EQ( 0, var.Size() )     UT_EQ( Priorities::NONE ,var.Priority() )
    defaultValues.Load( var );                          UT_EQ( 0, var.Size() )     UT_EQ( Priorities::NONE ,var.Priority() )

    // set and remove an entry using configuration interface
    cfg              .Load ( var );                     UT_EQ( 0, var.Size() )     UT_EQ( Priorities::NONE          ,var.Priority() )
    cfg              .Store( var );                     UT_EQ( 0, var.Size() )     UT_EQ( Priorities::NONE          ,var.Priority() )
    var.Add(A_CHAR("To be deleted"));                   UT_EQ( 1, var.Size() )     UT_EQ( Priorities::NONE          ,var.Priority() )
    cfg              .Store( var );                     UT_EQ( 1, var.Size() )     UT_EQ( Priorities::DefaultValues ,var.Priority() )
    var.Declare( A_CHAR("TEST"), A_CHAR("Remove") );    UT_EQ( 0, var.Size() )     UT_EQ( Priorities::NONE          ,var.Priority() )
    cfg              .Load ( var );                     UT_EQ( 1, var.Size() )     UT_EQ( Priorities::DefaultValues ,var.Priority() )
    var.Declare( A_CHAR("TEST"), A_CHAR("Remove") );    UT_EQ( 0, var.Size() )     UT_EQ( Priorities::NONE          ,var.Priority() )
    cfg              .Store( var );                     UT_EQ( 0, var.Size() )     UT_EQ( Priorities::DefaultValues ,var.Priority() )
    cfg              .Load ( var );                     UT_EQ( 0, var.Size() )     UT_EQ( Priorities::NONE          ,var.Priority() )
    var.Declare( A_CHAR("TEST"), A_CHAR("Remove") );    UT_EQ( 0, var.Size() )     UT_EQ( Priorities::NONE          ,var.Priority() )
    cfg              .Load ( var );                     UT_EQ( 0, var.Size() )     UT_EQ( Priorities::NONE          ,var.Priority() )

    // protected
    var.Declare( A_CHAR("TEST"), A_CHAR("Protected"));  UT_EQ( 0, var.Size() )     UT_EQ( Priorities::NONE          ,var.Priority() )
    var.ReplaceDefaultValue( A_CHAR("Default")  );
    ALIB.GetConfig().StoreDefault(var, A_CHAR("def par"));  UT_EQ( A_CHAR("def par"),   var.GetString() ) UT_EQ( Priorities::DefaultValues   ,var.Priority() )

    var.ClearValues();
    var.Add( A_CHAR("def var") );
    ALIB.GetConfig().StoreDefault( var );                 UT_EQ( A_CHAR("def var"),   var.GetString() )   UT_EQ( Priorities::DefaultValues   ,var.Priority() )

    var.ClearValues();
    ALIB.GetConfig().StoreDefault( var );                 UT_EQ( A_CHAR("Default"),   var.GetString() )   UT_EQ( Priorities::DefaultValues   ,var.Priority() )

    var.ClearValues();
    var.Add( A_CHAR("def var") );
    ALIB.GetConfig().Protect( var );                      UT_EQ( A_CHAR("def var"),   var.GetString() )   UT_EQ( Priorities::ProtectedValues ,var.Priority() )
    ALIB.GetConfig().Protect( var,A_CHAR("prot par"));    UT_EQ( A_CHAR("prot par"),  var.GetString() )   UT_EQ( Priorities::ProtectedValues ,var.Priority() )
    var.ClearValues();
    ALIB.GetConfig().Protect( var );                      UT_EQ( A_CHAR("Default"),   var.GetString() )   UT_EQ( Priorities::ProtectedValues ,var.Priority() )
    var.ReplaceDefaultValue( NullString() );
    var.ClearValues();
    ALIB.GetConfig().Protect( var );                      UT_EQ( 0, var.Size() )                          UT_EQ( Priorities::ProtectedValues ,var.Priority() )
    ALIB.GetConfig().Load( var );                         UT_EQ( A_CHAR("Default"),   var.GetString() )   UT_EQ( Priorities::DefaultValues   ,var.Priority() )

}

/** ********************************************************************************************
 * ConfigReplacementVariables
 **********************************************************************************************/
UT_METHOD(ConfigReplacementVariables)
{
    UT_INIT()
    const wchar_t* args[]=
    {
        L"COMMANDLINE",
        L"--REPL_CMDLINE=ReplCommandLine",
        L"--NOCATCMDLINE=NoCatCommandLine",
    };

    Configuration cfg(CreateDefaults::Yes);
    cfg.SetCommandLineArgs( 3, args );
    Variable var;

    InMemoryPlugin& defaultValues  = *cfg.GetPluginTypeSafe<aworx::InMemoryPlugin>( Priorities::DefaultValues   );
    InMemoryPlugin& protectedValues= *cfg.GetPluginTypeSafe<aworx::InMemoryPlugin>( Priorities::ProtectedValues );

    // replacements from command line plugin
    protectedValues.Store( var.Declare(A_CHAR("TEST"), A_CHAR("VARIABLE")), A_CHAR("no replacment")               ); cfg.Load( var );   UT_EQ( A_CHAR("no replacment")                   ,var.GetString() )
    protectedValues.Store( var.Declare(A_CHAR("TEST"), A_CHAR("VARIABLE")), A_CHAR("$UKN")                        ); cfg.Load( var );   UT_EQ( A_CHAR("")                                ,var.GetString() )
    protectedValues.Store( var.Declare(A_CHAR("TEST"), A_CHAR("VARIABLE")), A_CHAR("-$UKN * $UKN2-")              ); cfg.Load( var );   UT_EQ( A_CHAR("- * -")                           ,var.GetString() )
    protectedValues.Store( var.Declare(A_CHAR("TEST"), A_CHAR("VARIABLE")), A_CHAR("$NOCATCMDLINE")               ); cfg.Load( var );   UT_EQ( A_CHAR("NoCatCommandLine")                ,var.GetString() )
    protectedValues.Store( var.Declare(A_CHAR("TEST"), A_CHAR("VARIABLE")), A_CHAR("$$NOCATCMDLINE$")             ); cfg.Load( var );   UT_EQ( A_CHAR("$NoCatCommandLine$")              ,var.GetString() )

    protectedValues.Store( var.Declare(A_CHAR("TEST"), A_CHAR("VARIABLE")), A_CHAR("$REPL_CMDLINE")               ); cfg.Load( var );   UT_EQ( A_CHAR("ReplCommandLine")                 ,var.GetString() )
    protectedValues.Store( var.Declare(A_CHAR("TEST"), A_CHAR("VARIABLE")), A_CHAR("$REPL_ CMDLINE")              ); cfg.Load( var );   UT_EQ( A_CHAR(" CMDLINE" )                       ,var.GetString() )
    protectedValues.Store( var.Declare(A_CHAR("TEST"), A_CHAR("VARIABLE")), A_CHAR("$repL_CmdLine")               ); cfg.Load( var );   UT_EQ( A_CHAR("ReplCommandLine")                 ,var.GetString() )
    protectedValues.Store( var.Declare(A_CHAR("TEST"), A_CHAR("VARIABLE")), A_CHAR("$repL_CmdLine$repL_CmdLine")  ); cfg.Load( var );   UT_EQ( A_CHAR("ReplCommandLineReplCommandLine")  ,var.GetString() )
    protectedValues.Store( var.Declare(A_CHAR("TEST"), A_CHAR("VARIABLE")), A_CHAR("$repL_CmdLine $repL_CmdLine" )); cfg.Load( var );   UT_EQ( A_CHAR("ReplCommandLine ReplCommandLine") ,var.GetString() )

    // replacements without category name
    protectedValues.Store( var.Declare(nullptr, A_CHAR("NOCAT" ) ), A_CHAR("NoCat" )   );
    protectedValues.Store( var.Declare(nullptr, A_CHAR("NO_CAT") ), A_CHAR("No_cat")   );
    protectedValues.Store( var.Declare(A_CHAR("TEST"), A_CHAR("VARIABLE")), A_CHAR("$nocat" )                      ); cfg.Load( var );   UT_EQ( A_CHAR("NoCat")                           ,var.GetString() )
    protectedValues.Store( var.Declare(A_CHAR("TEST"), A_CHAR("VARIABLE")), A_CHAR("$_nocat")                      ); cfg.Load( var );   UT_EQ( A_CHAR("NoCat")                           ,var.GetString() )

    // need to add an underscore, if no category but name contains underscore!
    protectedValues.Store( var.Declare(A_CHAR("TEST"), A_CHAR("VARIABLE")), A_CHAR("$no_cat")                      ); cfg.Load( var );   UT_EQ( A_CHAR("")                                ,var.GetString() )
    protectedValues.Store( var.Declare(A_CHAR("TEST"), A_CHAR("VARIABLE")), A_CHAR("$_no_cat")                     ); cfg.Load( var );   UT_EQ( A_CHAR("No_cat")                          ,var.GetString() )


    // nested variables
    protectedValues.Store( var.Declare(A_CHAR("Rep"), A_CHAR("Var1")     ), A_CHAR("$Rep_Var2")                    );
    protectedValues.Store( var.Declare(A_CHAR("Rep"), A_CHAR("Var2")     ), A_CHAR("nested")                       );
    protectedValues.Store( var.Declare(A_CHAR("TEST"), A_CHAR("VARIABLE")), A_CHAR("$rep_var2")                    ); cfg.Load( var );   UT_EQ( A_CHAR("nested")                          ,var.GetString() )
    protectedValues.Store( var.Declare(A_CHAR("TEST"), A_CHAR("VARIABLE")), A_CHAR("$rep_var1")                    ); cfg.Load( var );   UT_EQ( A_CHAR("nested")                          ,var.GetString() )

    // illegal recursion
    UT_PRINT( A_CHAR("One warning should follow") )
    protectedValues.Store( var.Declare(A_CHAR("Rep"), A_CHAR("Var1")     ), A_CHAR("$Rep_Var2")                    );
    protectedValues.Store( var.Declare(A_CHAR("Rep"), A_CHAR("Var2")     ), A_CHAR("$Rep_Var1")                    );
    protectedValues.Store( var.Declare(A_CHAR("TEST"), A_CHAR("VARIABLE")), A_CHAR("$rep_var1")                    ); cfg.Load( var );

    // custom variables
    protectedValues.Store( var.Declare(A_CHAR("Rep"), A_CHAR("CUST"))    ,  A_CHAR("cf")                           );

    protectedValues.Store( var.Declare(A_CHAR("TEST"), A_CHAR("VARIABLE")),A_CHAR(">>$REP_CUST<<")                 ); cfg.Load( var );   UT_EQ( A_CHAR(">>cf<<")                          ,var.GetString() )

    cfg.SubstitutionVariableStart= A_CHAR("${");
    cfg.SubstitutionVariableEnd=   A_CHAR("}");
    protectedValues.Store( var.Declare(A_CHAR("TEST"), A_CHAR("VARIABLE")), A_CHAR(">>${REP_CUST}<<")              ); cfg.Load( var );   UT_EQ( A_CHAR(">>cf<<")                          ,var.GetString() )

    cfg.SubstitutionVariableStart= A_CHAR("€€€-");
    cfg.SubstitutionVariableEnd=   A_CHAR("--");
    var.Declare( A_CHAR("TEST"), A_CHAR("VARIABLE") );
    protectedValues.Store( var,  A_CHAR(">>€€€-REP_CUST--<<")           ); cfg.Load( var );   UT_EQ( A_CHAR(">>cf<<")                          ,var.GetString() )
    protectedValues.Store( var,  A_CHAR(">>€€€-REP_CUST--")             ); cfg.Load( var );   UT_EQ( A_CHAR(">>cf")                            ,var.GetString() )
    protectedValues.Store( var,  A_CHAR("€€€-REP_CUST--")               ); cfg.Load( var );   UT_EQ( A_CHAR("cf")                              ,var.GetString() )
    protectedValues.Store( var,  A_CHAR("€€€-REP_CUST--€€€-REP_CUST--") ); cfg.Load( var );   UT_EQ( A_CHAR("cfcf")                            ,var.GetString() )
    protectedValues.Store( var,  A_CHAR("€€-REP_CUST--")                ); cfg.Load( var );   UT_EQ( A_CHAR("€€-REP_CUST--")                   ,var.GetString() )

    UT_PRINT( A_CHAR("One warning should follow") )
    protectedValues.Store( var,  A_CHAR("€€€-REP_CUST-")                ); cfg.Load( var );   UT_EQ( A_CHAR("€€€-REP_CUST-")                   ,var.GetString() )

    cfg.SubstitutionVariableStart= A_CHAR("$");
    cfg.SubstitutionVariableEnd=   nullptr;

    // multi line replacements
    defaultValues.Store(var.Declare(A_CHAR("ML"), A_CHAR("REPL1"), ';'), A_CHAR("repl1-v1;repl1-v2")    );
    defaultValues.Store(var.Declare(A_CHAR("ML"), A_CHAR("REPL2"), ';'), A_CHAR("repl2-v1;repl2-v2")    );
    defaultValues.Store(var.Declare(A_CHAR("ML"), A_CHAR("VAR")  , ';'), A_CHAR("$ML_REPL1;$ML_REPL2")  );
    var.Declare(A_CHAR("ML"), A_CHAR("VAR"), ';' );
    cfg.Load( var );
    UT_EQ( 4, var.Size() )
    UT_EQ( A_CHAR("repl1-v1"), var.GetString(0) )
    UT_EQ( A_CHAR("repl1-v2"), var.GetString(1) )
    UT_EQ( A_CHAR("repl2-v1"), var.GetString(2) )
    UT_EQ( A_CHAR("repl2-v2"), var.GetString(3) )
}


/** ********************************************************************************************
 * ConfigIteration
 **********************************************************************************************/
#if ALIB_ALOX
UT_METHOD(ConfigIteration)
{
    UT_INIT()

    // we fake a second command line plug-in to test that each variable is delivered only once
    aworx::lib::config::CLIArgs cliArgs;
    cliArgs.AlternativeArgs.emplace_back( A_CHAR("--ALOX_CONSOLE_TYPE=overwritten")  );
    lib::ALOX.GetConfig().InsertPlugin( &cliArgs, Priorities::CLI + 1 );

    auto* it= lib::ALOX.GetConfig().GetIterator(A_CHAR("ALOX"));
    int cntVars= 0;
    int cntALOX_CONSOLE_TYPE= 0;

    while( it->Next() )
    {
        ++cntVars;
        if( it->Actual.Fullname().Equals(A_CHAR("ALOX_CONSOLE_TYPE")) )
        {
            ++cntALOX_CONSOLE_TYPE;
            UT_EQ( A_CHAR("overwritten"), it->Actual.GetString() )
        }
        UT_PRINT( "Iteration: Variable found {}={!Q}", it->Actual.Fullname(), it->Actual.GetString() )
    }
    UT_TRUE( cntVars >= 3 )
    UT_EQ  ( 1, cntALOX_CONSOLE_TYPE )

    // repeat that (to test Reset )
    it->ResetToSection( A_CHAR("ALOX") );
    cntVars= 0;
    cntALOX_CONSOLE_TYPE= 0;

    while( it->Next() )
    {
        ++cntVars;
        if( it->Actual.Fullname().Equals(A_CHAR("ALOX_CONSOLE_TYPE")) )
        {
            ++cntALOX_CONSOLE_TYPE;
            UT_EQ( A_CHAR("overwritten"), it->Actual.GetString() )
        }
        UT_PRINT( "Iteration: Variable found {}={!Q}", it->Actual.Fullname(), it->Actual.GetString() )
    }
    UT_TRUE( cntVars >= 3 )
    UT_EQ  ( 1, cntALOX_CONSOLE_TYPE )


    delete it;


    lib::ALOX.GetConfig().RemovePlugin( &cliArgs );
}
#endif


#include "unittests/aworx_unittests_end.hpp"

} //namespace

#endif // ALIB_UT_CONFIGURATION
