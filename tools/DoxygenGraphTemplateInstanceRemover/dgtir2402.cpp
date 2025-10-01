// #################################################################################################
//  ALib C++ Library Tools - DoxygenGraphTemplateInstanceRemover
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//
// Notes:
// This is a replacement for the previous version of this tool. First of all, the previous
// version did not work with the actual version of doxygen anymore. Secondly, it was not very
// well documented and could not easily be understood. Thirdly, we decided to not include
// the "classlist" anymore, as we noticed that it does not bring any value to the user.
// Thus, the template singletons do not need to be removed anymore.
// This leads to a very simple tool, that just deletes ALib namespace names to make the Types
// better readable.
//
// This is a quick and dirty written piece of code.
// Maybe doxygen will support an option to remove instantiated classes some day.
// #################################################################################################

// to preserve the right order, we are not includable directly from outside.
#include "ALib.ALox.H"
#include "ALib.Strings.StdIOStream.H"
#include "ALib.Strings.H"
#include "ALib.Variables.IniFile.H"
#include "ALib.System.H"
#include "ALib.Camp.Base.H"

#include <fstream>
#include <iostream>
#include <list>

using namespace alib;
using namespace std;

#include "ALib.Lang.CIFunctions.H"
int main2402();
int main2402()
{
    bool DebugMode= alib::ARG_C==1;

    Log_AddDebugLogger()
    Log_SetDomain( "DGTIR", Scope::Filename  )
    Log_SetVerbosity( "DEBUG_LOGGER", DebugMode || BASECAMP.IsDebuggerPresent()
                                      ?  Verbosity::Verbose :  Verbosity::Verbose,
                                      "/DGTIR" )


    AString     FileName;
    if (!DebugMode )
    {
        Log_SetVerbosity( "DEBUG_LOGGER",  Verbosity::Info , "/DOXGRAPH" )
        FileName.Reset( NCString(alib::ARG_VN[1]) );
    }
    else
    {

        //String debugFile( "classalib_1_1core_1_1textlogger_1_1ObjectConverter__coll__graph.dot" );
        //String debugFile( "classalib_1_1strings_1_1ASTerminatable__inherit__graph.dot");
        //String debugFile( "classalib_1_1core_1_1textlogger_1_1TextLogger__inherit__graph.dot");
        //String debugFile( "classalib_1_1core_1_1textlogger_1_1MetaInfo__coll__graph.dot" );
        //String debugFile( "classalib_1_1lox_1_1loggers_1_1AnsiConsoleLogger__coll__graph.dot" );
        String debugFile( "classalib_1_1expressions_1_1detail_1_1ParserImpl_a68c8954b9b5e3de7d582dc1129a4139f_cgraph.dot.corrected.dot" );

        FileName.Reset("/home/dev/A-Worx/ALib/html/") << '/' << debugFile;
    }

    errno= 0;
    ifstream iFile( FileName.Terminate() );
    if ( !iFile.is_open() )
    {
        cerr << "DoxygenGraphTemplateInstanceRemover:  Error "<< errno <<" opening file: "<< FileName << endl;
        return -1;
    }

    ofstream* oFile= nullptr;
    if (!DebugMode)
    {
        FileName._(".patched.dot");
        oFile= new ofstream( FileName.Terminate() );
        if ( !oFile->is_open() )
        {
            cerr << "DoxygenGraphTemplateInstanceRemover: Error writing file" << endl;
            return false;
        }
    }

    Log_Info( "dotFixer: invoked as: dot {} {} {} {} {} -> output file: {}",
                                                          alib::ARG_C > 1 ? alib::ARG_VN[1] : "",
                                                          alib::ARG_C > 2 ? alib::ARG_VN[2] : "",
                                                          alib::ARG_C > 3 ? alib::ARG_VN[3] : "",
                                                          alib::ARG_C > 4 ? alib::ARG_VN[4] : "",
                                                          alib::ARG_C > 5 ? alib::ARG_VN[5] : "",
                                                          FileName
                                                         )


    ostream& out= DebugMode ? cout : *oFile;


    alib::ISReadLineN readOp= alib::ISReadLineN( &iFile );
    AString line;
    AString label;
    while( !readOp.IsEOF )
    {
        line.Reset( readOp );

        constexpr String LABEL("label=\"");
        // Replacing namespaces in untouched template node lines
        integer labelStart= line.IndexOf(LABEL);
        if(labelStart>0)
        {
            labelStart+= LABEL.Length();
            integer labelEnd= line.IndexOf('"', labelStart + 1 );
            label.Reset(line.Substring(labelStart, labelEnd- labelStart));
            //Log_Info("Label before replacement: ", label )

            label.SearchAndReplace("\\l"                      , "" ); // new label characters
            label.SearchAndReplace("alib::"                   , "" );
            label.SearchAndReplace("assert::"                 , "" );
            label.SearchAndReplace("bitbuffer::"              , "" );
            label.SearchAndReplace("boxing::"                 , "" );
            label.SearchAndReplace("camp::"                   , "" );
            label.SearchAndReplace("characters::"             , "" );
            label.SearchAndReplace("containers::"             , "" );
            label.SearchAndReplace("cli::"                    , "" );
            label.SearchAndReplace("compatibility::"          , "" );
            label.SearchAndReplace("enumops::"                , "" );
            label.SearchAndReplace("enumrecords::"            , "" );
            label.SearchAndReplace("exceptions::"             , "" );
            label.SearchAndReplace("expressions::"            , "" );
            label.SearchAndReplace("files::"                  , "" );
            label.SearchAndReplace("format::"                 , "" );
            label.SearchAndReplace("lang::"                   , "" );
            label.SearchAndReplace("lox::textlogger::"        , "" );
            label.SearchAndReplace("lox::detail::"            , "" );
            label.SearchAndReplace("lox::"                    , "" );
            label.SearchAndReplace("monomem::"                , "" );
            label.SearchAndReplace("resources::"              , "" );
            label.SearchAndReplace("singletons::"             , "" );
            label.SearchAndReplace("strings::"                , "" );
            label.SearchAndReplace("system::"                 , "" );
            label.SearchAndReplace("threadmodel"              , "" );
            label.SearchAndReplace("threads"                  , "" );
            label.SearchAndReplace("time"                     , "" );
            label.SearchAndReplace("variables::"              , "" );

            label.SearchAndReplace("std::",      "" );
            label.SearchAndReplace("< ",        "<" );
            label.SearchAndReplace("< ",        "<" );
            label.SearchAndReplace("\\< ",     "\\<" );
            label.SearchAndReplace("\\< ",     "\\<" );
            label.SearchAndReplace("&lt; ",     "&lt;" );
            label.SearchAndReplace("&lt; ",     "&lt;" );
            label.SearchAndReplace(" >",        ">" );
            label.SearchAndReplace(" >",        ">" );
            label.SearchAndReplace(" &gt;",     "&gt;" );
            label.SearchAndReplace(" &gt;",     "&gt;" );
            label.SearchAndReplace(" \\>",     "\\>" );
            label.SearchAndReplace(" \\>",     "\\>" );

            // for testing if all is fetched
            //Log_Assert( label.IndexOf( "::" ) < 0, label )
            //Log_Info("Label after  replacement: ", label )
            label.Trim();
            out  << line.Substring(0, labelStart )
                 << label
                 << line.Substring( labelEnd)
                 << endl;
        }
        else
            out << line << endl;
    }

    //----- invoking dot ---------
    NAString dotCommand("dot");
    dotCommand._(' ')._(FileName);
    for( int i= 2; i< alib::ARG_C ; ++i )
        dotCommand << ' ' << NString(alib::ARG_VN[i]);

    FILE* pipe = popen( dotCommand.Terminate(), "r");

    string result;
    if (!pipe)
    {
        cerr << "Error invoking command: `" << dotCommand << '`'  << endl;
        cerr << "              returned: "  << result             << endl;

        return 2;
    }

    pclose(pipe);
    return 0;
}

#include "ALib.Lang.CIMethods.H"
