// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//
// Notes:
// This tool replaces simple, pure 'integer' template instantiation nodes in doxygen
// dot files by their parameterized template class counterparts.
// Furthermore inheritance graphs that contain only one template type node are deleted (the node
// is deleted from the file) when the template parameter is not the default one.
// Finally, all namespace names and newline tokens are removed from the node names
// (occurs only in template parameters).
//
// This is a quick and dirty written piece of code.
// Maybe doxygen will support an option to remove instantiated classes some day.
// #################################################################################################
/** @file */ // Hello Doxygen

// to preserve the right order, we are not includable directly from outside.
#include <alib/alox.hpp>
#include "alib/compatibility/std_iostream.hpp"
#include "alib/config/inifile.hpp"

#include <fstream>
#include <iostream>
#include <list>
#include <algorithm>

using namespace aworx;
using namespace std;


// #################################################################################################
// Globals
// #################################################################################################
extern bool     DebugMode;
       bool     DebugMode;
extern IniFile* Inifile;
       IniFile* Inifile;
extern AString  FileName;
       AString  FileName;
extern String   NewFileNameSuffix;
       String   NewFileNameSuffix= ".corrected.dot";
extern bool     IsSingleTNodeInheritFile;
       bool     IsSingleTNodeInheritFile= false;

// #################################################################################################
// Simple data structures
// #################################################################################################
struct Content
{
    enum class Type{ Other, Node, Link };

    Type        type;
    Substring   LineRest;

    Content ( Type pType) : type(pType) {}
};

struct Node : public Content
{
    int       Num;
    Substring TClassName;
    String128 TParamName;
    bool      IsIntInstance   = false;
    int       TInstantiationNum  = -1;
    int       ReplacementNodeNum = -1;

    Node()  : Content( Content::Type::Node )  {}
};

struct Link : public Content
{
    int       From;
    int       To;
    bool      Skip               = false;
    Link()  : Content( Content::Type::Link ) {}
};

struct Line
{
    AString     original;
    Content*    content = nullptr;
};

struct DotFile
{
    list<Line>           Lines;
    list<Node*>          TClasses;
    list<Line*>          TClassesUntouched;
    vector<Node*>        ReplacedNodes;
    vector<Link*>        SortedLinks;
};

extern DotFile dotFile;
       DotFile dotFile;


// #################################################################################################
// Parse a single line and create content entry
// #################################################################################################
bool ParseLine( Line& fileLine );
bool ParseLine( Line& fileLine )
{
    Substring line(fileLine.original);

    // Ignore lines not starting with "Node"
    if ( !line.ConsumeString<Case::Sensitive, Whitespaces::Trim >( "Node") )
        return true;

    // From now on, unknown means error (stop processing file)
    int nodeNum;
    if( !line.ConsumeInt( nodeNum ) )
    {
        Log_Error( "Error reading line: ", fileLine.original );
        return false;
    }

    //------------- read node definition lines -------------
    if( line.ConsumeString<Case::Sensitive, Whitespaces::Trim>( "[label=\""  ) )
    {
        integer idx= line.IndexOf( '\"' );
        if (idx < 0 )
        {
            Log_Error( "Error reading line: ", fileLine.original );
            return false;
        }

        Substring  lineRest;
        line.Split( idx, lineRest, 1 );
        Log_Verbose( "Found Node {} = {!Tab14}", nodeNum, line );

        // is template class node?
        idx= line.IndexOf( '<' );
        if ( idx < 0 )
            return true;

        // save information
        Node* node= new Node();
        fileLine.content=   node;
        node->Num=          nodeNum;
        node->LineRest=     lineRest;
        line.ConsumeChars<false>( idx - 1, &node->TClassName );

        // read contents of brackets
        line.ConsumeChars<false>( 2 );
        idx= line.LastIndexOf( '>' );
        Log_Assert( idx > 0, "terminating '>' not found" );
        line.ConsumeCharsFromEnd( line.Length() - idx );
        line.ConsumeCharFromEnd('\\');

        node->IsIntInstance=     line.ConsumeInt( node->TInstantiationNum )
                             &&  line.Trim().IsEmpty();


        // save if original name
        if ( !node->IsIntInstance )
        {
            if ( line.Trim().IndexOfAny<Inclusion::Include>(", &") < 0  && isupper( node->TClassName.CharAtStart() ) )
            {
                node->TParamName= line;
                dotFile.TClasses.emplace_back( node );

                Log_Info( "  Saveing param name: class {!Q}= {!Q}", node->TClassName, node->TParamName );
                Variable var( nullptr, String64() << "TCLASS_" << node->TClassName, '\0', "auto generated" );
                var.Add( node->TParamName );
                Inifile->Store( var );
            }
            else
            {
                Log_Info( "  Template param <{}> found, not usable as parameter name", line );
                dotFile.TClassesUntouched.emplace_back( &fileLine );
            }

        }
        else
        {
            Log_Info( "  Template<NN> found" );
        }

        // OK
        return true;
    }

    //------------- read link lines -------------
    if( line.ConsumeString<Case::Sensitive, Whitespaces::Trim>( "-> Node" ) )
    {
        Link* link= new Link();
        fileLine.content=   link;
        link->From=         nodeNum;
        line.ConsumeInt( link->To );
        link->LineRest=     line;

        return true;
    }

    // error: format not recognized
    return false;
}

// #################################################################################################
// Read/Write File
// #################################################################################################
int ReadFile();
int ReadFile()
{
    Log_Info( "Reading file: ", FileName );

    dotFile.Lines.clear();
    ifstream file( FileName.ToCString() );
    if ( !file.is_open() )
    {
        cerr << "DoxygenGraphTemplateInstanceRemover:  Error opening file: "<< FileName << endl;
        return -1;
    }

    aworx::ISReadLineN readOp= aworx::ISReadLineN( &file );
    while( !readOp.IsEOF )
    {
        dotFile.Lines.emplace_back( Line() );
        dotFile.Lines.back().original << readOp;

        // Replacing namespaces in untouched template node lines
        Line* line= &dotFile.Lines.back();
        {
            line->original.SearchAndReplace("\\l", "" ); // new line characters
            line->original.SearchAndReplace("aworx::lib::boxing::"                 , "" );
            line->original.SearchAndReplace("aworx::lib::config::"                 , "" );
            line->original.SearchAndReplace("aworx::lib::debug::"                  , "" );
            line->original.SearchAndReplace("aworx::lib::lang::"                   , "" );
            line->original.SearchAndReplace("aworx::lib::strings::"                , "" );
            line->original.SearchAndReplace("aworx::lib::system::"                 , "" );
            line->original.SearchAndReplace("aworx::lib::threads::"                , "" );
            line->original.SearchAndReplace("aworx::lib::time"                     , "" );
            line->original.SearchAndReplace("aworx::lib::util::"                   , "" );
            line->original.SearchAndReplace("aworx::lib::"                         , "" );
            line->original.SearchAndReplace("aworx::lib::lox::core::textlogger::"  , "" );
            line->original.SearchAndReplace("aworx::lib::lox::core::"              , "" );
            line->original.SearchAndReplace("aworx::lib::lox::"                    , "" );

            //line->original.SearchAndReplace("aworx::lib::lox::lang::textlogger::",       "" ); //very strange, but this occurs!?
            //line->original.SearchAndReplace("lang::aworx::lib::lox::core::",             "" );
            //line->original.SearchAndReplace("aworx::lib::lox::lang::",                   "" );
            //line->original.SearchAndReplace("lox::lang::",                          "" );
            //line->original.SearchAndReplace("lang::",                               "" );

            line->original.SearchAndReplace("std::",                                "" );
            line->original.SearchAndReplace("< ",        "<" );
            line->original.SearchAndReplace("< ",        "<" );
            line->original.SearchAndReplace("\\< ",     "\\<" );
            line->original.SearchAndReplace("\\< ",     "\\<" );
            line->original.SearchAndReplace("&lt; ",     "&lt;" );
            line->original.SearchAndReplace("&lt; ",     "&lt;" );
            line->original.SearchAndReplace(" >",        ">" );
            line->original.SearchAndReplace(" >",        ">" );
            line->original.SearchAndReplace(" &gt;",     "&gt;" );
            line->original.SearchAndReplace(" &gt;",     "&gt;" );
            line->original.SearchAndReplace(" \\>",     "\\>" );
            line->original.SearchAndReplace(" \\>",     "\\>" );
        }

        // for testing if all is fetched
        // if( line->original.IndexOf( "::" ) > 0 )
        //     Log_Error( line->original );

        if( !ParseLine( dotFile.Lines.back() ) )
            return 0;
    }

    return static_cast<int>( dotFile.Lines.size() );
}



// #################################################################################################
// Build all information needed to write the new file
// #################################################################################################
bool Build();
bool Build()
{
    Log_Info( String512() << "Building..." );

    // Special treatment for inheritance graphs with one single node which is a template node
    // and not the 'default': Will be emptied.
    if ( FileName.IndexOf("inherit_") >= 0 && dotFile.Lines.size() == 8 )
    {
        Line* theTNode= nullptr;
        int cntNodes= 0;
        for ( Line& line : dotFile.Lines )
            if (    line.content != nullptr
                 && line.content->type == Content::Type::Node )
            {
                cntNodes++;
                theTNode= &line;
            }

        if( cntNodes == 1 )
        {
            InMemoryPlugin::Section* ifSection= Inifile->Sections[0];

            for( InMemoryPlugin::Entry* entry : ifSection->Entries )
            {
                if( entry->Name.StartsWith( "TCLASS_" ) )
                {
                    String32 className( entry->Name, 7 );
                    if( theTNode->original.IndexOf( className ) > 0 )
                    {
                        Variable var( nullptr, entry->Name );
                        Inifile->Load( var );
                        if( theTNode->original.IndexOf( var.GetString() ) < 0 )
                        {
                            //Log_Warning( String512() << "Deleting inherit file (single node): " << theTNode->original );
                            theTNode->original= "";
                            theTNode->content= nullptr;
                            IsSingleTNodeInheritFile= true;
                            return true;
                        }
                        break;
                    }
                }
            }
        }
    }


    // ------------ Nodes: find nodes that are to be replaced ------------
    Log_Info( String512() << "Searching replacement nodes..." );
    int qtyReplacements= 0;
    for ( Line& line : dotFile.Lines )
    {
        // not a node line
        if (    line.content == nullptr
             || line.content->type != Content::Type::Node )
            continue;

        // not a template int instantiation line
        Node* actNode= static_cast<Node*>(line.content);
        if ( !actNode->IsIntInstance )
            continue;

        // template instantiations: search main node
        bool found= false;
        for ( auto& replacementNode : dotFile.TClasses )
            if( actNode->TClassName.Equals( replacementNode ->TClassName ) )
            {
                qtyReplacements++;
                actNode->ReplacementNodeNum= replacementNode->Num;
                dotFile.ReplacedNodes.emplace_back( actNode );
                found= true;
                break;
            }

        // if no original TClass found, this instantiation Node will become the main!
        if ( !found )
        {
            // make this one the main node!
            actNode->IsIntInstance= false;

            Variable var( nullptr, String64() << "TCLASS_" << actNode->TClassName );
            if ( Inifile->Load( var ) )
                actNode->TParamName._()._( var.GetString() );
            else
                actNode->TParamName= "T";


            Log_Info( String512() <<  "  No general TClass found for this node. Using this node as target for all: "
                                  <<  actNode->TClassName << "<" << actNode->TInstantiationNum << ">"
                                  <<  ", with '" << actNode->TParamName << "' as parameter name."
                                  );
            dotFile.TClasses.emplace_back(actNode);
        }
    }

//    if ( qtyReplacements == 0 )
//    {
//        Log_Info( String512() << "Nothing to replace. Using original file." );
//        return false;
//    }

    Log_Info( String512() <<  dotFile.TClasses.size() << " TClass(es) found:" );
    Log_Prune ( for( auto tClass : dotFile.TClasses )               )
        Log_Verbose( String32() << "  " <<tClass->TClassName )


    // sort nodes
    std::sort( begin(dotFile.ReplacedNodes), end( dotFile.ReplacedNodes ),
              [](Node* n1, Node* n2) { return n1->Num < n2->Num; } );

    Log_Info( String512() << "Replacement nodes sorted" );
    Log_Prune( for (auto n : dotFile.ReplacedNodes) { )
        Log_Verbose( String512() << "  " << n->Num << " \"" << n->TClassName <<'\"' << Format::Tab(20)
                    << "   ->  " << n->ReplacementNodeNum  );
    Log_Prune( } )


    // ------------ Links: set new node numbers ------------
    Log_Info( String512() << "Processing links: Replace nodes and sort..." );
    for ( Line& line : dotFile.Lines )
    {
        if ( line.content == nullptr )
            continue;

        if ( line.content->type == Content::Type::Link )
        {
            Link* link= static_cast<Link*>(line.content);
            dotFile.SortedLinks.emplace_back(link);

            // search and set replace node for 'from' node
            auto it=
            std::lower_bound( begin(dotFile.ReplacedNodes), end( dotFile.ReplacedNodes ),
                              nullptr,
                              [link](Node* n, Node* ){ return n->Num < link->From; } );

            if( it != dotFile.ReplacedNodes.end() && (*it)->Num == link->From )
                link->From= (*it)->ReplacementNodeNum;


            // search and set replace node for 'to' node
            it=
            std::lower_bound( begin(dotFile.ReplacedNodes), end( dotFile.ReplacedNodes ),
                              nullptr,
                              [link](Node* n, Node* ){ return n->Num < link->To; } );

            if( it != dotFile.ReplacedNodes.end() && (*it)->Num == link->To )
                link->To= (*it)->ReplacementNodeNum;
        }
    }

    // sort links
    std::sort( begin(dotFile.SortedLinks), end( dotFile.SortedLinks ),
              [](Link* l1, Link* l2)
              {
                return l1->From != l2->From ? l1->From < l2->From
                                            : l1->To   < l2->To;
              } );

    Log_Prune( for (auto n : dotFile.SortedLinks) { )
        Log_Verbose( String512() << "  " << n->From << " -> " << n->To  );
    Log_Prune( } )

    // ------------ remove double links ------------
    Log_Info( String512() << "Removing double links..." );
    Link* lastLink= nullptr;
    for( Link* link : dotFile.SortedLinks )
    {
        if (    lastLink != nullptr
             && lastLink->From == link->From
             && lastLink->To   == link->To
             && link->LineRest.IndexOf("label=") < 0 )
        {
            Log_Verbose( String512() << "  Removing link: "
                                     << "  Node"   << link->From << " -> Node" << link->To
                                     << link->LineRest );
            link->Skip= true;
            continue;
        }
        lastLink= link;
    }

    return true;
}

bool WriteFile();
bool WriteFile()
{
    Log_Info( "Writing file " );

    // create file or redirect to cout (in debug mode)
    ofstream* file= nullptr;
    if (!DebugMode)
    {
        FileName._(NewFileNameSuffix);
        file= new ofstream( FileName.ToCString() );
        if ( !file->is_open() )
        {
            cerr << "DoxygenGraphTemplateInstanceRemover: Error writing file" << endl;
            return false;
        }
    }
    ostream& out= DebugMode ? cout : *file;

    Log_Info( "Untouched stuff" );

    // write file
    bool nodesAndLinksWritten= IsSingleTNodeInheritFile;
    for ( Line& line : dotFile.Lines )
    {
        // write untouched lines
        if( line.content == nullptr )
        {
            out << line.original << endl;
            continue;
        }

        // write our stuff once the first node/link line comes
        if ( !nodesAndLinksWritten )
        {
            nodesAndLinksWritten= true;

            Log_Info( "Writing Nodes" );

            // write nodes
            Log_Info( "Writing untouched template nodes" );
            for( Line* nline : dotFile.TClassesUntouched )
            {
                out << nline->original << endl;
            }

            // write nodes
            Log_Info( "Writing template replacement nodes" );
            for( Node* node : dotFile.TClasses )
            {
                out  << "  Node" << node->Num << " [label=\"" << node->TClassName
                     << "\\<" << node->TParamName << "\\>\""
                     << node->LineRest << endl;
            }

            // write links
            Log_Info( "Writing links" );
            for( Link* link : dotFile.SortedLinks )
                if ( !link->Skip )
                    out << "  Node"   << link->From  << " -> Node" << link->To << link->LineRest << endl;
            Log_Info( "The rest is untouched stuff again" );
        }
    }

    Log_Info( "Replaced File: ", FileName );

    if (!DebugMode)
        delete file;

    return true;
}

// #################################################################################################
// InvokeDotAndExit
// #################################################################################################
void     InvokeDotAndExit( int argc, char *argv[] );
void     InvokeDotAndExit( int argc, char *argv[] )
{
    if (DebugMode )
        return;

    //----- invoking dot ---------
    AString dotCommand("dot");
    dotCommand._(' ')._(FileName);
    for( int i= 2; i< argc ; i++ )
        dotCommand << ' ' << argv[i];

    FILE* pipe = popen( dotCommand.ToCString(), "r");

    string result;
    if (!pipe)
    {
        cerr << "Error invoking command: `" << dotCommand << '`'  << endl;
        cerr << "              returned: "  << result             << endl;

        exit(2);
    }

    pclose(pipe);

    exit(0);
}


int main(int argc, char *argv[])
{
    DebugMode= argc==1;

    // init ALib
    lib::ALIB.Init( argc, argv);
    Log_AddDebugLogger();
    Log_SetDomain( "DOXGRAPH", Scope::Filename  );
    Log_SetVerbosity( "DEBUG_LOGGER", DebugMode || lib::ALIB.IsDebuggerPresent()
                                      ?  Verbosity::Verbose :  Verbosity::Warning,
                                      "/DOXGRAPH" );


    if (!DebugMode )
    {
      //Log_SetVerbosity( "DEBUG_LOGGER",  Verbosity::Info , "/DOXGRAPH" );
        lib::lang::Report::GetDefault().PushHaltFlags( false, false );
        FileName= argv[1];
    }
    else
    {
        //String debugFile( "/classaworx_1_1lox_1_1core_1_1textlogger_1_1ObjectConverter__coll__graph.dot" );
        //String debugFile( "/classaworx_1_1lib_1_1strings_1_1ASTerminatable__inherit__graph.dot");
        //String debugFile( "/classaworx_1_1lox_1_1core_1_1textlogger_1_1TextLogger__inherit__graph.dot");
        //String debugFile( "/classaworx_1_1lox_1_1core_1_1textlogger_1_1MetaInfo__coll__graph.dot" );
        //String debugFile( "/classaworx_1_1lox_1_1loggers_1_1AnsiConsoleLogger__coll__graph.dot" );
        String debugFile( "classaworx_1_1lib_1_1lang_1_1Singleton__inherit__graph.dot" ); // "inherit_graph_49.dot" );

        FileName._("../html/cpp_ref");
        for( int depth= 0; depth < 10 ; depth++ )
        {
            if ( Directory::Exists( FileName ) )
            {
                FileName << '/' << debugFile;
                break;
            }
            FileName.InsertAt( "../", 0 );
        }
    }

    Log_Warning( "dotFixer: Processing File: ", FileName );

    // read INI file
    Inifile= new IniFile("doxygenDotFixer.cfg");
    //Log_Assert( Inifile->LastStatus == IniFile::Status::Ok , "IniFile not found" );
    Inifile->AutoSave=      true;
    Inifile->FileComments=
     "======================================================================================" "\n"
     "ALib - A-Worx Utility Library"                                                          "\n"
     "Copyright 2013-2018 A-Worx GmbH, Germany"                                               "\n"
     "Published under 'Boost Software License' (a free software license, see LICENSE.txt)"    "\n"
     "======================================================================================" "\n"
     "This tool replaces simple, pure 'integer' template instantiation nodes in doxygen"      "\n"
     "dot files by their parameterized template class counterparts."                          "\n"
     "Furthermore inheritance graphs that contain only one template type node are deleted"    "\n"
     "(the node is deleted from the file) when the template parameter is not the default"     "\n"
     "one.) Finally, all namespace names and newline tokens are removed from the node names"  "\n"
     "(occurs only in template parameters)."                                                  "\n"
     ""                                                                                       "\n"
     "--------------------------------------------------------------------------------------" "\n"
     "This cfg-file is auto generated."                                                       "\n"
     "Template parameter names detected across invocations are stored here."                  "\n"
     "--------------------------------------------------------------------------------------"
     ;

    // do it
    if( ReadFile() )
    {
        Build();
        WriteFile();
    }

    InvokeDotAndExit( argc, argv );

    // cleanup
    for( Line& line : dotFile.Lines )
        if ( line.content != nullptr )
        {
            if ( line.content->type == Content::Type::Link )
                delete static_cast<Link*>( line.content );
            else
                delete static_cast<Node*>( line.content );
        }


    delete Inifile;
    FileName.SetNull();
    lib::ALIB.TerminationCleanUp();

    return 0;
}

