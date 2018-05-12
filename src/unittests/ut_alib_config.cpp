// #################################################################################################
//  aworx - Unit Tests
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alox.hpp"

#if !defined (HPP_ALIB_CONFIG_INI_FILE)
    #include "alib/config/inifile.hpp"
#endif
#if !defined (HPP_ALIB_CONFIG_CONFIGURATION)
    #include "alib/config/configuration.hpp"
#endif
#if !defined (HPP_ALIB_SYSTEM_DIRECTORY)
    #include "alib/system/directory.hpp"
#endif


#define TESTCLASSNAME       CPP_ALib_Config
#include "aworx_unittests.hpp"


#include <iostream>
#include <fstream>

using namespace std;
using namespace aworx;

namespace ut_aworx {

UT_CLASS()

//--------------------------------------------------------------------------------------------------
//--- Read and write a configuration file
//--------------------------------------------------------------------------------------------------

UT_METHOD(ConfigCommandLineArgs)
{
    UT_INIT();

    const char* args[]=
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

    Configuration cfg;
    cfg.SetCommandLineArgs( sizeof(args)/sizeof(char*), const_cast<char**>( args ) );

    Variable var;
    UT_EQ( Priorities::CLI,    cfg.Load   ( var.Declare( EmptyString,      ASTR("SingleHyphen") )) );   UT_EQ( ASTR("12"),            *var.GetString()     );
                               cfg.Load   ( var.Declare( EmptyString,      ASTR("DoubleHyphen") ));     UT_EQ( true,             var.IsTrue()        );
    UT_EQ( Priorities::CLI,    var.Priority);

    UT_EQ( Priorities::CLI,    cfg.Load   ( var.Declare( EmptyString,      ASTR("Empty")        )) );   UT_EQ( EmptyString,              *var.GetString()     );
    UT_EQ( Priorities::CLI,    cfg.Load   ( var.Declare( EmptyString,      ASTR("Whitespaces")  )) );   UT_EQ( ASTR("Hello Test"),    *var.GetString()     );
    UT_EQ( Priorities::CLI,    cfg.Load   ( var.Declare( EmptyString,      ASTR("HOME")         )) );   UT_EQ( ASTR("overwritten"),   *var.GetString()     );
    UT_EQ( Priorities::CLI,    cfg.Load   ( var.Declare( EmptyString,      ASTR("integer")      )) );   UT_EQ( 42,               var.GetInteger()    );
    UT_EQ( Priorities::NONE,   cfg.Load   ( var.Declare( EmptyString,      ASTR("notexistent")  )) );   UT_EQ( 0,                var.GetInteger()    );
    UT_EQ( Priorities::CLI,    cfg.Load   ( var.Declare( EmptyString,      ASTR("integer")      )) );   UT_EQ( 42,               var.GetInteger()    );
    UT_EQ( Priorities::CLI,    cfg.Load   ( var.Declare( EmptyString,      ASTR("double")       )) );
    UT_EQ( 3.14,            var.GetFloat() );
    UT_EQ( Priorities::CLI,    cfg.Load   ( var.Declare( EmptyString,      ASTR("double")       )) );   UT_EQ( 3.14,             var.GetFloat()      );
    UT_EQ( Priorities::CLI,    cfg.Load   ( var.Declare( ASTR("ALIB"),  ASTR("test")         )) );   UT_EQ( ASTR("passed"),        *var.GetString()     );
    UT_EQ( Priorities::NONE,   cfg.Load   ( var.Declare( EmptyString,      ASTR("notexistent")  )) );   UT_EQ( 0.0,              var.GetFloat() );

    auto* it= cfg.GetPluginTypeSafe<lib::config::CLIArgs>()->GetIterator(ASTR("ALIB"));
    UT_TRUE ( it->Next( var ) ); UT_EQ(String(ASTR("ITER")) , var.Name );  UT_EQ( ASTR("x"),        *var.GetString()     );
    UT_TRUE ( it->Next( var ) ); UT_EQ(String(ASTR("ITER2")), var.Name );  UT_EQ( ASTR("y"),        *var.GetString()     );
    UT_TRUE ( it->Next( var ) ); UT_EQ(String(ASTR("TEST")) , var.Name );  UT_EQ( ASTR("passed"),        *var.GetString()     );
    UT_FALSE( it->Next( var ) );
    delete it;

}

UT_METHOD(ConfigCommandLineArgsWChar)
{
    UT_INIT();


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

    Configuration cfg;
    cfg.SetCommandLineArgs( sizeof(args)/sizeof(wchar_t*), const_cast<wchar_t**>( args ) );

    Variable var;
    UT_EQ( Priorities::CLI,    cfg.Load   ( var.Declare( EmptyString,      ASTR("SingleHyphen") )) );   UT_EQ( ASTR("12"),           *var.GetString()     );
                               cfg.Load   ( var.Declare( EmptyString,      ASTR("DoubleHyphen") ));     UT_EQ( true,            var.IsTrue()        );
    UT_EQ( Priorities::CLI,    var.Priority);

    UT_EQ( Priorities::CLI,    cfg.Load   ( var.Declare( EmptyString,      ASTR("Empty")        )) );   UT_EQ( EmptyString,             *var.GetString()     );
    UT_EQ( Priorities::CLI,    cfg.Load   ( var.Declare( EmptyString,      ASTR("Whitespaces")  )) );   UT_EQ( ASTR("Hello Test"),   *var.GetString()     );
    UT_EQ( Priorities::CLI,    cfg.Load   ( var.Declare( EmptyString,      ASTR("HOME")         )) );   UT_EQ( ASTR("overwritten"),  *var.GetString()     );
    UT_EQ( Priorities::CLI,    cfg.Load   ( var.Declare( EmptyString,      ASTR("integer")      )) );   UT_EQ( 42,              var.GetInteger()    );
    UT_EQ( Priorities::NONE,   cfg.Load   ( var.Declare( EmptyString,      ASTR("notexistent")  )) );   UT_EQ( 0,               var.GetInteger()    );
    UT_EQ( Priorities::CLI,    cfg.Load   ( var.Declare( EmptyString,      ASTR("integer")      )) );   UT_EQ( 42,              var.GetInteger()    );
    UT_EQ( Priorities::CLI,    cfg.Load   ( var.Declare( EmptyString,      ASTR("double")       )) );   UT_EQ( 3.14,            var.GetFloat()      );
    UT_EQ( Priorities::NONE,   cfg.Load   ( var.Declare( EmptyString,      ASTR("notexistent")  )) );   UT_EQ( 0.0,             var.GetFloat()      );
    UT_EQ( Priorities::CLI,    cfg.Load   ( var.Declare( EmptyString,      ASTR("double")       )) );   UT_EQ( 3.14,            var.GetFloat()      );
    UT_EQ( Priorities::CLI,    cfg.Load   ( var.Declare( ASTR("ALIB"),  ASTR("test")         )) );   UT_EQ( ASTR("passed"),       *var.GetString()     );

    UT_EQ( Priorities::NONE,   cfg.Load   ( var.Declare( ASTR("IGNORE"),ASTR("Home")         )) );
    cfg.GetPluginTypeSafe<aworx::lib::config::CLIArgs>()->DefaultCategories.emplace_back(ASTR("IGNORE"));
    UT_EQ( Priorities::CLI,    cfg.Load   ( var.Declare( ASTR("IGNORE"),ASTR("Home")         )) );   UT_EQ( ASTR("overwritten"),  *var.GetString()     );
    UT_EQ( Priorities::NONE,   cfg.Load   ( var.Declare( ASTR("IGNORE"),ASTR("Homexyz")      )) );
    cfg.GetPluginTypeSafe<aworx::lib::config::CLIArgs>()->AllowedMinimumShortCut=5;
    UT_EQ( Priorities::NONE,   cfg.Load   ( var.Declare( ASTR("IGNORE"),ASTR("Homexyz")      )) );
    cfg.GetPluginTypeSafe<aworx::lib::config::CLIArgs>()->AllowedMinimumShortCut=4;
    UT_EQ( Priorities::CLI,    cfg.Load   ( var.Declare( ASTR("IGNORE"),ASTR("Homexyz")      )) );   UT_EQ( ASTR("overwritten"),   *var.GetString()    );


    auto* it= cfg.GetPluginTypeSafe<lib::config::CLIArgs>()->GetIterator(ASTR("ALIB"));
    UT_TRUE ( it->Next( var ) ); UT_EQ(String(ASTR("ITER")) , var.Name );  UT_EQ( ASTR("x"),        *var.GetString()     );
    UT_TRUE ( it->Next( var ) ); UT_EQ(String(ASTR("ITER2")), var.Name );  UT_EQ( ASTR("y"),        *var.GetString()     );
    UT_TRUE ( it->Next( var ) ); UT_EQ(String(ASTR("TEST")) , var.Name );  UT_EQ( ASTR("passed"),        *var.GetString()     );
    UT_FALSE( it->Next( var ) );
    delete it;

}

/** ********************************************************************************************
 * ConfigIniFiles
 **********************************************************************************************/

UT_METHOD(ConfigIniFiles)
{
    UT_INIT();

    // write sample config file
    UT_PRINT(""); UT_PRINT( "### Configuration with IniFile ###" );
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
        ALIB_STD_TO_NARROW_TSTRING(fileName, nFileName)
        iniFile.open ( nFileName.ToCString() );
        iniFile << iniFileContents;
        iniFile.close();
    }

    Variable var;

    IniFile iniFile( fileName );

    // check some values
    iniFile.Load( var.Declare( EmptyString,               ASTR("CUBA")        ) );   UT_EQ( ASTR("a country"),      *var.GetString() );
    iniFile.Load( var.Declare( EmptyString,               ASTR("cUbA")        ) );   UT_EQ( ASTR("a country"),      *var.GetString() );
    iniFile.Load( var.Declare( EmptyString,               ASTR("SIZE")        ) );   UT_EQ( ASTR("25"),             *var.GetString() );
    iniFile.Load( var.Declare( EmptyString,               ASTR("concat"), ',' ) );   UT_EQ( 11 , var.Size());
                                                                     UT_EQ( ASTR("start =5")        ,*var.GetString(0) );
                                                                     UT_EQ( ASTR("end   =32")       ,*var.GetString(1) );
                                                                     UT_EQ( ASTR("#no comment")     ,*var.GetString(2) );
                                                                     UT_EQ( ASTR(";nocomment")      ,*var.GetString(3) );

    iniFile.Load( var.Declare( ASTR("ESC"),            ASTR("Blanks")      ) );   UT_EQ( ASTR(" x "),            *var.GetString() );
    iniFile.Load( var.Declare( ASTR("ESC"),            ASTR("Blanks2")     ) );   UT_EQ( ASTR(" x  y "),         *var.GetString() );
    iniFile.Load( var.Declare( ASTR("ESC"),            ASTR("Tabs")        ) );   UT_EQ( ASTR("\tx\t"),          *var.GetString() );
    iniFile.Load( var.Declare( ASTR("ESC"),            ASTR("nrslash")     ) );   UT_EQ( ASTR("\n\r//\\"),       *var.GetString() );

    iniFile.Load( var.Declare( ASTR("Great Section"),  ASTR("SectionVar")         ) );   UT_EQ( ASTR("5"),           *var.GetString() );
    iniFile.Load( var.Declare( ASTR("2nd Section"),    ASTR("SectionVar")         ) );   UT_EQ( ASTR("6"),           *var.GetString() );
    iniFile.Load( var.Declare( ASTR("Great Section"),  ASTR("SECTION_CONTINUED")  ) );   UT_EQ( ASTR("yEs"),         *var.GetString() );
    iniFile.Load( var.Declare( ASTR("Great Section"),  ASTR("Tricky")             ) );   UT_EQ( ASTR("backslash\\"), *var.GetString() );
    iniFile.Load( var.Declare( ASTR("Great Section"),  ASTR("Overwritten")        ) );   UT_EQ( ASTR("Yes"),         *var.GetString() );


    // add it to ALIB config
    lib::ALIB.Config->InsertPlugin( &iniFile, Priorities::Standard );
    lib::ALIB.Config->Load( var.Declare( EmptyString,               ASTR("CUBA")              ) );   UT_EQ( ASTR("a country")  , *var.GetString() );
    lib::ALIB.Config->Load( var.Declare( EmptyString,               ASTR("cUbA")              ) );   UT_EQ( ASTR("a country")  , *var.GetString() );
    lib::ALIB.Config->Load( var.Declare( EmptyString,               ASTR("SIZE")              ) );   UT_EQ( ASTR("25")         , *var.GetString() );
    lib::ALIB.Config->Load( var.Declare( EmptyString,               ASTR("concat"), ','       ) );   UT_EQ( 11 , var.Size());
                                                                                      UT_EQ( ASTR("start =5")   , *var.GetString(0) );
                                                                                      UT_EQ( ASTR("end   =32")  , *var.GetString(1) );
    lib::ALIB.Config->Load( var.Declare( ASTR("Great Section"),  ASTR("SectionVar")        ) );   UT_EQ( ASTR("5")          , *var.GetString() );
    lib::ALIB.Config->Load( var.Declare( ASTR("2nd Section"),    ASTR("SectionVar")        ) );   UT_EQ( ASTR("6")          , *var.GetString() );
    lib::ALIB.Config->Load( var.Declare( ASTR("Great Section"),  ASTR("SECTION_CONTINUED") ) );   UT_EQ( ASTR("yEs")        , *var.GetString() );
    lib::ALIB.Config->Load( var.Declare( ASTR("Great Section"),  ASTR("Tricky")            ) );   UT_EQ( ASTR("backslash\\"), *var.GetString() );
    lib::ALIB.Config->Load( var.Declare( ASTR("Great Section"),  ASTR("SECTION_CONTINUED") ) );   UT_TRUE( var.IsTrue() );


    // check if environment variable "home" overwrites INI file
    const character* HOME_ENV_NAME;
    #if defined(_WIN32)
        HOME_ENV_NAME= ASTR("HOmepATH");
    #else
        HOME_ENV_NAME= ASTR("HOME");
    #endif

    Variable vIniFile;   iniFile.Load( vIniFile.Declare( EmptyString, HOME_ENV_NAME) );                 UT_EQ( ASTR("overwritten_by_environment"), *vIniFile.GetString() );
    lib::ALIB.Config->Load  ( var.Declare(EmptyString, HOME_ENV_NAME) ); UT_EQ( Priorities::Environment, var.Priority );
    UT_TRUE( var.GetString()->IsNotEmpty() );
    UT_TRUE( !vIniFile.GetString()->Equals( var.GetString() ) );


    // change a value and write a new one
    var.Declare( ASTR("New Section"),  ASTR("newvar"));
    var.Priority= Priorities::Standard;

    UT_EQ( Priorities::Standard, lib::ALIB.Config->Store( var, ASTR("new") ) );
    lib::ALIB.Config->Load  ( var.Declare(ASTR("New Section"),  ASTR("newvar")) );  UT_EQ( ASTR("new"),   *var.GetString() );

    var.Declare( EmptyString,             ASTR("newvar"));
    var.Priority= Priorities::Standard;
    UT_EQ( Priorities::Standard, lib::ALIB.Config->Store( var, ASTR("aworx")) );
    lib::ALIB.Config->Load  ( var.Declare(EmptyString,             ASTR("newvar")) );  UT_EQ( ASTR("aworx"), *var.GetString() );


    var.Declare( EmptyString,   ASTR("newvarList"), ',');
    var.Add(ASTR("val1=5"));
    var.Add(ASTR("val2=10"));
    var.Add(ASTR("val3=hello"));
    var.Priority= Priorities::Standard;
    UT_EQ( Priorities::Standard, lib::ALIB.Config->Store(var) );
    lib::ALIB.Config->Load (  var.Declare( EmptyString,  ASTR("newvarList"))   );


    var.Declare( EmptyString,   ASTR("commented"), ',', ASTR("2lines"));
    var.Priority= Priorities::Standard;
    UT_EQ( Priorities::Standard, lib::ALIB.Config->Store(  var, ASTR("this is c-line 1 \nand this line 2") ) );

    // iterate
    {
        auto it= iniFile.GetIterator(ASTR("Great Section"));
        int cntVars= 0;
        bool varFound[5] = {false,false,false,false,false};
        while( it->Next( var ) )
        {
            cntVars++;
            if( String(ASTR("SectionVar")       ).Equals( var.Name ) ) {  varFound[0]|= true; UT_EQ( 5            , var.GetInteger() );  }
            if( String(ASTR("Double")           ).Equals( var.Name ) ) {  varFound[1]|= true; UT_EQ(ASTR("12.3")        ,*var.GetString()  );  }
            if( String(ASTR("Tricky")           ).Equals( var.Name ) ) {  varFound[2]|= true; UT_EQ( ASTR("backslash\\"),*var.GetString()  );  }
            if( String(ASTR("OverWritten")      ).Equals( var.Name ) ) {  varFound[3]|= true; UT_EQ( ASTR("Yes")        ,*var.GetString()  );  }
            if( String(ASTR("SECTION_CONTINUED")).Equals( var.Name ) ) {  varFound[4]|= true; UT_EQ( ASTR("yEs")        ,*var.GetString()  );  }
        }
        UT_EQ( 5, cntVars );
        for( int i=0; i<5; i++ )
            UT_TRUE( varFound[i] )

        delete it;
    }

    // write the file
    iniFile.FileName._(".writeback.txt");
    iniFile.WriteFile();

    // load the written file into another config
    IniFile readBack( iniFile.FileName );
    Variable varBack;

    // compare all
    {
        String512 msg;
        for ( InMemoryPlugin::Section* section : iniFile.Sections )
        {
            for ( InMemoryPlugin::Entry* entry : section->Entries )
            {
                msg.Clear()._( "Reading variable " )
                ._(Format::Field( String256() << section->Name << '/' << entry->Name, 40, Alignment::Left) );
                UT_PRINT( msg );

                char delim= '\0';
                if(     entry->Name.Equals(ASTR("concat"))
                    ||  entry->Name.Equals(ASTR("newvarList"))       )
                    delim= ',';

                iniFile .Load( var    .Declare( section->Name, entry->Name, delim) );
                readBack.Load( varBack.Declare( section->Name, entry->Name, delim) );

                UT_EQ( var.Size(), varBack.Size() );
                for ( int i= 0; i< var.Size(); i++ )
                {
                    integer idx= var.GetString(i)->IndexOf('=');
                    if( idx < 0 )
                    {
                        UT_EQ( *var.GetString(i), *varBack.GetString(i) );
                    }
                    else
                    {
                        integer idxBack= varBack.GetString(i)->IndexOf('=');
                        Substring orig= var    .GetString(i)->Substring( 0, idx     );
                        Substring back= varBack.GetString(i)->Substring( 0, idxBack );
                        UT_EQ( orig.Trim(), back.Trim() );
                        orig= var    .GetString(i)->Substring( idx     +1 );
                        back= varBack.GetString(i)->Substring( idxBack +1 );
                        UT_EQ( orig.Trim(), back.Trim() );
                    }
                }
            }
        }
    }

    readBack.Load ( var.Declare( ASTR("New Section"),  ASTR("newvar") ) );   UT_EQ( ASTR("new")    , *var.GetString() );
    readBack.Load ( var.Declare( EmptyString,          ASTR("newvar") ) );   UT_EQ( ASTR("aworx")  , *var.GetString() );


    lib::ALIB.Config->RemovePlugin( &iniFile );


    lib::ALIB.Config->InsertPlugin( &readBack, Priorities::Standard );
    lib::ALIB.Config->Load( var.Declare( ASTR("New Section"),  ASTR("newvar")  ) );   UT_EQ( ASTR("new")        ,*var.GetString() );
    lib::ALIB.Config->Load( var.Declare( EmptyString,          ASTR("newvar")  ) );   UT_EQ( ASTR("aworx")      ,*var.GetString() );

    lib::ALIB.Config->RemovePlugin( &readBack );
}

/** ********************************************************************************************
 * ConfigDefaultAndProtected
 **********************************************************************************************/

UT_METHOD(ConfigDefaultAndProtected)
{
    UT_INIT();

    const wchar_t* args[]=
    {
        L"COMMANDLINE",
        L"--TEST_VARIABLE=fromCommandLine",
    };

    Configuration cfg;
    cfg.SetCommandLineArgs( 2, const_cast<wchar_t**>( args ) );
    Variable var;

    InMemoryPlugin& defaultValues  = *cfg.GetPluginTypeSafe<aworx::InMemoryPlugin>( Priorities::DefaultValues   );
    InMemoryPlugin& protectedValues= *cfg.GetPluginTypeSafe<aworx::InMemoryPlugin>( Priorities::ProtectedValues );

    // command line
    UT_EQ( Priorities::CLI,    cfg.Load  ( var.Declare( ASTR("TEST"),      ASTR("VARIABLE") ) ) );   UT_EQ( ASTR("fromCommandLine")    ,*var.GetString() );

    // set default, does not overwrite
    var.Declare( ASTR("TEST"), ASTR("VARIABLE"));  var.Add(ASTR("not overwriting"));   defaultValues.Store( var );
    UT_EQ( Priorities::CLI,    cfg.Load  ( var.Declare( ASTR("TEST"),      ASTR("VARIABLE") ) ) );   UT_EQ( ASTR("fromCommandLine")    ,*var.GetString() );

    // set protected, overwrites command line
    var.Declare( ASTR("TEST"), ASTR("VARIABLE"));  var.Add(ASTR("does overwrite"));   protectedValues.Store( var );
    UT_EQ( Priorities::ProtectedValues,  cfg.Load  ( var.Declare( ASTR("TEST"),      ASTR("VARIABLE") ) ) );   UT_EQ( ASTR("does overwrite")     ,*var.GetString() );

    // set default, something else
    var.Declare( ASTR("TEST"), ASTR("VAR2"));      var.Add(ASTR("this is var 2"));   defaultValues.Store( var );
    UT_EQ( Priorities::DefaultValues,    cfg.Load  ( var.Declare( ASTR("TEST"),      ASTR("VAR2")     ) ) );   UT_EQ( ASTR("this is var 2")      ,*var.GetString() );

    // set and remove an entry using plugin interface
    var.Declare( ASTR("TEST"), ASTR("Remove") );    UT_EQ( 0, var.Size() );     UT_EQ( Priorities::NONE            ,var.Priority );
    defaultValues.Load( var );      UT_EQ( 0, var.Size() );     UT_EQ( Priorities::NONE            ,var.Priority );
    var.Add(ASTR("To be deleted"));           UT_EQ( 1, var.Size() );     UT_EQ( Priorities::NONE            ,var.Priority );
    defaultValues.Store( var );     UT_EQ( 1, var.Size() );     UT_EQ( Priorities::NONE            ,var.Priority );
    var.Declare( ASTR("TEST"), ASTR("Remove") );    UT_EQ( 0, var.Size() );     UT_EQ( Priorities::NONE            ,var.Priority );
    defaultValues.Load( var );      UT_EQ( 1, var.Size() );     UT_EQ( Priorities::NONE            ,var.Priority );
    var.ClearValues();                  UT_EQ( 0, var.Size() );     UT_EQ( Priorities::NONE            ,var.Priority );
    defaultValues.Store( var );     UT_EQ( 0, var.Size() );     UT_EQ( Priorities::NONE            ,var.Priority );
    var.Declare( ASTR("TEST"), ASTR("Remove") );    UT_EQ( 0, var.Size() );     UT_EQ( Priorities::NONE            ,var.Priority );
    defaultValues.Load( var );      UT_EQ( 0, var.Size() );     UT_EQ( Priorities::NONE            ,var.Priority );

    // set and remove an entry using configuration interface
    cfg              .Load ( var );     UT_EQ( 0, var.Size() );     UT_EQ( Priorities::NONE            ,var.Priority );
    cfg              .Store( var );     UT_EQ( 0, var.Size() );     UT_EQ( Priorities::NONE            ,var.Priority );
    var.Add(ASTR("To be deleted"));           UT_EQ( 1, var.Size() );     UT_EQ( Priorities::NONE            ,var.Priority );
    cfg              .Store( var );     UT_EQ( 1, var.Size() );     UT_EQ( Priorities::DefaultValues   ,var.Priority );
    var.Declare( ASTR("TEST"), ASTR("Remove") );    UT_EQ( 0, var.Size() );     UT_EQ( Priorities::NONE            ,var.Priority );
    cfg              .Load ( var );     UT_EQ( 1, var.Size() );     UT_EQ( Priorities::DefaultValues   ,var.Priority );
    var.Declare( ASTR("TEST"), ASTR("Remove") );    UT_EQ( 0, var.Size() );     UT_EQ( Priorities::NONE            ,var.Priority );
    cfg              .Store( var );     UT_EQ( 0, var.Size() );     UT_EQ( Priorities::DefaultValues   ,var.Priority );
    cfg              .Load ( var );     UT_EQ( 0, var.Size() );     UT_EQ( Priorities::NONE            ,var.Priority );
    var.Declare( ASTR("TEST"), ASTR("Remove") );    UT_EQ( 0, var.Size() );     UT_EQ( Priorities::NONE            ,var.Priority );
    cfg              .Load ( var );     UT_EQ( 0, var.Size() );     UT_EQ( Priorities::NONE            ,var.Priority );

    // protected
    var.Declare( ASTR("TEST"), ASTR("Protected"));  UT_EQ( 0, var.Size() );     UT_EQ( Priorities::NONE            ,var.Priority );
    var.DefaultValue._( ASTR("Default")  );
    lib::ALIB.Config->StoreDefault(var, ASTR("def par"));        UT_EQ( ASTR("def par"),   *var.GetString() );  UT_EQ( Priorities::DefaultValues   ,var.Priority );

    var.ClearValues();
    var.Add( ASTR("def var") );
    lib::ALIB.Config->StoreDefault( var );                 UT_EQ( ASTR("def var"),   *var.GetString() );  UT_EQ( Priorities::DefaultValues   ,var.Priority );

    var.ClearValues();
    lib::ALIB.Config->StoreDefault( var );                 UT_EQ( ASTR("Default"),   *var.GetString() );  UT_EQ( Priorities::DefaultValues   ,var.Priority );

    var.ClearValues();
    var.Add( ASTR("def var") );
    lib::ALIB.Config->Protect( var );                      UT_EQ( ASTR("def var"),   *var.GetString() );  UT_EQ( Priorities::ProtectedValues ,var.Priority );
    lib::ALIB.Config->Protect( var,ASTR("prot par"));            UT_EQ( ASTR("prot par"),  *var.GetString() );  UT_EQ( Priorities::ProtectedValues ,var.Priority );
    var.ClearValues();
    lib::ALIB.Config->Protect( var );                      UT_EQ( ASTR("Default"),   *var.GetString() );  UT_EQ( Priorities::ProtectedValues ,var.Priority );
    var.DefaultValue.SetNull();
    var.ClearValues();
    lib::ALIB.Config->Protect( var );                      UT_EQ( 0, var.Size()                );  UT_EQ( Priorities::ProtectedValues ,var.Priority );
    lib::ALIB.Config->Load( var );                         UT_EQ( ASTR("Default"),   *var.GetString() );  UT_EQ( Priorities::DefaultValues   ,var.Priority );

}

/** ********************************************************************************************
 * ConfigReplacementVariables
 **********************************************************************************************/
UT_METHOD(ConfigReplacementVariables)
{
    UT_INIT();
    const wchar_t* args[]=
    {
        L"COMMANDLINE",
        L"--REPL_CMDLINE=ReplCommandLine",
        L"--NOCATCMDLINE=NoCatCommandLine",
    };

    Configuration cfg;
    cfg.SetCommandLineArgs( 3, const_cast<wchar_t**>( args ) );
    Variable var;

    InMemoryPlugin& defaultValues  = *cfg.GetPluginTypeSafe<aworx::InMemoryPlugin>( Priorities::DefaultValues   );
    InMemoryPlugin& protectedValues= *cfg.GetPluginTypeSafe<aworx::InMemoryPlugin>( Priorities::ProtectedValues );

    // replacements from command line plugin
    protectedValues.Store( var.Declare(ASTR("TEST"), ASTR("VARIABLE")), ASTR("no replacment")               ); cfg.Load( var );   UT_EQ( ASTR("no replacment")                   ,*var.GetString() );
    protectedValues.Store( var.Declare(ASTR("TEST"), ASTR("VARIABLE")), ASTR("$UKN")                        ); cfg.Load( var );   UT_EQ( ASTR("")                                ,*var.GetString() );
    protectedValues.Store( var.Declare(ASTR("TEST"), ASTR("VARIABLE")), ASTR("-$UKN * $UKN2-")              ); cfg.Load( var );   UT_EQ( ASTR("- * -")                           ,*var.GetString() );
    protectedValues.Store( var.Declare(ASTR("TEST"), ASTR("VARIABLE")), ASTR("$NOCATCMDLINE")               ); cfg.Load( var );   UT_EQ( ASTR("NoCatCommandLine")                ,*var.GetString() );
    protectedValues.Store( var.Declare(ASTR("TEST"), ASTR("VARIABLE")), ASTR("$$NOCATCMDLINE$")             ); cfg.Load( var );   UT_EQ( ASTR("$NoCatCommandLine$")              ,*var.GetString() );
    protectedValues.Store( var.Declare(ASTR("TEST"), ASTR("VARIABLE")), ASTR("$REPL_CMDLINE")               ); cfg.Load( var );   UT_EQ( ASTR("ReplCommandLine")                 ,*var.GetString() );
    protectedValues.Store( var.Declare(ASTR("TEST"), ASTR("VARIABLE")), ASTR("$REPL_ CMDLINE")              ); cfg.Load( var );   UT_EQ( ASTR(" CMDLINE" )                       ,*var.GetString() );
    protectedValues.Store( var.Declare(ASTR("TEST"), ASTR("VARIABLE")), ASTR("$repL_CmdLine")               ); cfg.Load( var );   UT_EQ( ASTR("ReplCommandLine")                 ,*var.GetString() );
    protectedValues.Store( var.Declare(ASTR("TEST"), ASTR("VARIABLE")), ASTR("$repL_CmdLine$repL_CmdLine")  ); cfg.Load( var );   UT_EQ( ASTR("ReplCommandLineReplCommandLine")  ,*var.GetString() );
    protectedValues.Store( var.Declare(ASTR("TEST"), ASTR("VARIABLE")), ASTR("$repL_CmdLine $repL_CmdLine" )); cfg.Load( var );   UT_EQ( ASTR("ReplCommandLine ReplCommandLine") ,*var.GetString() );

    // replacements without category name
    protectedValues.Store( var.Declare(nullptr, ASTR("NOCAT" ) ), ASTR("NoCat" )   );
    protectedValues.Store( var.Declare(nullptr, ASTR("NO_CAT") ), ASTR("No_cat")   );
    protectedValues.Store( var.Declare(ASTR("TEST"), ASTR("VARIABLE")), ASTR("$nocat" )                      ); cfg.Load( var );   UT_EQ( ASTR("NoCat")                           ,*var.GetString() );
    protectedValues.Store( var.Declare(ASTR("TEST"), ASTR("VARIABLE")), ASTR("$_nocat")                      ); cfg.Load( var );   UT_EQ( ASTR("NoCat")                           ,*var.GetString() );

    // need to add an underscore, if no category but name contains underscore!
    protectedValues.Store( var.Declare(ASTR("TEST"), ASTR("VARIABLE")), ASTR("$no_cat")                      ); cfg.Load( var );   UT_EQ( ASTR("")                                ,*var.GetString() );
    protectedValues.Store( var.Declare(ASTR("TEST"), ASTR("VARIABLE")), ASTR("$_no_cat")                     ); cfg.Load( var );   UT_EQ( ASTR("No_cat")                          ,*var.GetString() );


    // nested variables
    protectedValues.Store( var.Declare(ASTR("Rep"), ASTR("Var1")     ), ASTR("$Rep_Var2")                    );
    protectedValues.Store( var.Declare(ASTR("Rep"), ASTR("Var2")     ), ASTR("nested")                       );
    protectedValues.Store( var.Declare(ASTR("TEST"), ASTR("VARIABLE")), ASTR("$rep_var2")                    ); cfg.Load( var );   UT_EQ( ASTR("nested")                          ,*var.GetString() );
    protectedValues.Store( var.Declare(ASTR("TEST"), ASTR("VARIABLE")), ASTR("$rep_var1")                    ); cfg.Load( var );   UT_EQ( ASTR("nested")                          ,*var.GetString() );

    // illegal recursion
    UT_PRINT( ASTR("One warning should follow") );
    protectedValues.Store( var.Declare(ASTR("Rep"), ASTR("Var1")     ), ASTR("$Rep_Var2")                    );
    protectedValues.Store( var.Declare(ASTR("Rep"), ASTR("Var2")     ), ASTR("$Rep_Var1")                    );
    protectedValues.Store( var.Declare(ASTR("TEST"), ASTR("VARIABLE")), ASTR("$rep_var1")                    ); cfg.Load( var );

    // custom variables
    protectedValues.Store( var.Declare(ASTR("Rep"), ASTR("CUST"))    ,  ASTR("cf")                           );

    protectedValues.Store( var.Declare(ASTR("TEST"), ASTR("VARIABLE")),ASTR(">>$REP_CUST<<")                 ); cfg.Load( var );   UT_EQ( ASTR(">>cf<<")                          ,*var.GetString() );

    cfg.SubstitutionVariableStart= ASTR("${");
    cfg.SubstitutionVariableEnd=   ASTR("}");
    protectedValues.Store( var.Declare(ASTR("TEST"), ASTR("VARIABLE")), ASTR(">>${REP_CUST}<<")              ); cfg.Load( var );   UT_EQ( ASTR(">>cf<<")                          ,*var.GetString() );

    cfg.SubstitutionVariableStart= ASTR("€€€-");
    cfg.SubstitutionVariableEnd=   ASTR("--");
    var.Declare( ASTR("TEST"), ASTR("VARIABLE") );
    protectedValues.Store( var,  ASTR(">>€€€-REP_CUST--<<")           ); cfg.Load( var );   UT_EQ( ASTR(">>cf<<")                          ,*var.GetString() );
    protectedValues.Store( var,  ASTR(">>€€€-REP_CUST--")             ); cfg.Load( var );   UT_EQ( ASTR(">>cf")                            ,*var.GetString() );
    protectedValues.Store( var,  ASTR("€€€-REP_CUST--")               ); cfg.Load( var );   UT_EQ( ASTR("cf")                              ,*var.GetString() );
    protectedValues.Store( var,  ASTR("€€€-REP_CUST--€€€-REP_CUST--") ); cfg.Load( var );   UT_EQ( ASTR("cfcf")                            ,*var.GetString() );
    protectedValues.Store( var,  ASTR("€€-REP_CUST--")                ); cfg.Load( var );   UT_EQ( ASTR("€€-REP_CUST--")                   ,*var.GetString() );

    UT_PRINT( ASTR("One warning should follow") );
    protectedValues.Store( var,  ASTR("€€€-REP_CUST-")                ); cfg.Load( var );   UT_EQ( ASTR("€€€-REP_CUST-")                   ,*var.GetString() );

    cfg.SubstitutionVariableStart= ASTR("$");
    cfg.SubstitutionVariableEnd=   nullptr;

    // multi line replacements
    defaultValues.Store(var.Declare(ASTR("ML"), ASTR("REPL1"), ';'), ASTR("repl1-v1;repl1-v2")    );
    defaultValues.Store(var.Declare(ASTR("ML"), ASTR("REPL2"), ';'), ASTR("repl2-v1;repl2-v2")    );
    defaultValues.Store(var.Declare(ASTR("ML"), ASTR("VAR")  , ';'), ASTR("$ML_REPL1;$ML_REPL2")  );
    var.Declare(ASTR("ML"), ASTR("VAR"), ';' );
    cfg.Load( var );
    UT_EQ( 4, var.Size() );
    UT_EQ( ASTR("repl1-v1"), *var.GetString(0) );
    UT_EQ( ASTR("repl1-v2"), *var.GetString(1) );
    UT_EQ( ASTR("repl2-v1"), *var.GetString(2) );
    UT_EQ( ASTR("repl2-v2"), *var.GetString(3) );
}


/** ********************************************************************************************
 * ConfigIteration
 **********************************************************************************************/
UT_METHOD(ConfigIteration)
{
    UT_INIT();

    // we fake a second command line plug-in to test that each variable is delivered only once
    aworx::lib::config::CLIArgs cliArgs;
    std::vector<String> args= {ASTR("--ALOX_CONSOLE_TYPE=overwritten")};
    cliArgs.SetArgs( &args );
    lib::ALOX.Config->InsertPlugin( &cliArgs, Priorities::CLI + 1 );

    auto* it= lib::ALOX.Config->GetIterator(ASTR("ALOX"));
    int cntVars= 0;
    int cntALOX_CONSOLE_TYPE= 0;

    while( it->Next() )
    {
        cntVars++;
        if( it->Actual.Fullname.Equals(ASTR("ALOX_CONSOLE_TYPE")) )
        {
            cntALOX_CONSOLE_TYPE++;
            UT_EQ( ASTR("overwritten"), *it->Actual.GetString() );
        }
        UT_PRINT( "Iteration: Variable found {}={!Q}", it->Actual.Fullname, it->Actual.GetString() );
    }
    UT_TRUE( cntVars >= 3 );
    UT_EQ  ( 1, cntALOX_CONSOLE_TYPE );

    lib::ALOX.Config->RemovePlugin( &cliArgs );
    delete it;
}

UT_CLASS_END

}; //namespace



