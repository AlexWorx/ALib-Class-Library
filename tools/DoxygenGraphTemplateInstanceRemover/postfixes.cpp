// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//
// Notes:
//   This is a postprocessor for the doxygen website created. The following is performed
//   - Anchor fixes:
//     The current doxygen version 1.10 (as of March 24) obviously replaces '<' and '>' when
//     used in aliases to "&lt;" and "&gt;". This is reversed with this tool.
//
// This is a quick and dirty written piece of code.
// #################################################################################################

// to preserve the right order, we are not includable directly from outside.
#include "ALib.ALox.H"
#include "ALib.Strings.StdIOStream.H"
#include "ALib.System.H"
#include "ALib.Files.H"
#include "ALib.Files.Expressions.H"
#include "ALib.Files.TextFile.H"

using namespace alib;
using namespace std;


// #################################################################################################
// Globals
// #################################################################################################
namespace {
    MonoAllocator   ma(ALIB_DBG("DGTIR",) 64);
}


#include "ALib.Lang.CIFunctions.H"

/**
 * Note:
 * This method does not work. Well, it works as expected, but the approach does not work
 * unfortunately. It replaces occurrences of "&lt;" and "&gt;" that are valid.
 * Therefore , it is not used currently, instead, fixExternalLinkImage() is used.
 * Let's hope there are not too many other errors?
 *
 *
 * Doxygen, since Version 1.10 (or so) replaces characters "<" and ">" by HTML codes "&lt;" and
 * "&gt;", if these anchors are created using custom commands (doxygen macros).
 * This is, of course, wrong because it destroys the HTML syntax.
 *
 * This function searches for the start and end of anchors in an html file and replaces
 * "&lt;" and "&gt;" back to the ASCII characters in between.
 *
 * @param file The opened and read file.
 * @param fileNameForLoggingOnly As it says.
 * @return The number of anchors fixed. If \c 0 is returned, the file was not changed.
 */
int fixHTMLAnchors(TextFile& file, const String& fileNameForLoggingOnly);
int fixHTMLAnchors(TextFile& file, const String& fileNameForLoggingOnly)
{
    Log_SetDomain( "ANCHORS", Scope::Method)

    ALIB_DBG( int cntLine= 0; )
    ALIB_REL( (void) fileNameForLoggingOnly; )
    int cntAnchorsFixed= 0;
    bool inAnchor= false;  // Anchors often spread over several lines.
    // loop over all lines
    for(auto& lineOrig: file )
    {
        String2K newLine;

        Log_Info("LINES", "Line {}: {}", cntLine, lineOrig )
        integer actStart= 0;
        integer actEnd;
        // loop over anchors in line (could be more than one)
        for(;;)
        {
            // find start and end of anchor and switch "inAnchor"-mode
            if( !inAnchor )
            {
                actStart= lineOrig.IndexOf("<a href=", actStart);
                if(actStart < 0 )
                    break;
                inAnchor= true;
                continue;
            }
            else
            {
                actEnd=  lineOrig.IndexOf("</a>", actStart);
                if(actEnd < 0)
                    actEnd= lineOrig.Length();
                else
                    inAnchor= false;
            }

            // replace all occurrences of
            if ( newLine.IsEmpty() )
                newLine << lineOrig;
            int cntReplacements= 0;
            String replacements[]= { "&lt;", "<",
                                     "&gt;", ">"  };
            for (size_t replIdx = 0; replIdx < sizeof(replacements)/sizeof(String); replIdx+=2 )
            {
                auto count= newLine.SearchAndReplace( replacements[replIdx],
                                                      replacements[replIdx+1],
                                                      actStart,
                                                      100,
                                                      lang::Case::Sensitive,
                                                      actEnd );
                actEnd-= 3 * count;
                cntReplacements+= count;
            }

            if( cntReplacements > 0 )
            {
                ++cntAnchorsFixed;
                Log_Verbose( "{} replacements in line {}. Line now:", cntReplacements, cntLine, newLine)
                Log_If( cntReplacements > 2, Verbosity::Warning,
                        "{} replacements in line {}. Line now:", cntReplacements, cntLine, newLine )
            }


            // if we are not (anymore) in an anchor, we stay in this line and search for a next anchor
            if(!inAnchor)
                actStart= actEnd + 4;
            else
                break; // next line!
        }
        ALIB_DBG(++cntLine;)
        if (newLine.Length() > 0)
            lineOrig.Allocate( file.GetAllocator(), newLine);

    } // line-loop

    Log_If( cntAnchorsFixed > 0, Verbosity::Info, "{} anchors fixed", cntAnchorsFixed, fileNameForLoggingOnly )
    return cntAnchorsFixed;
}

/**
 * Doxygen, since Version 1.10 (or so) replaces characters "<" and ">" by HTML codes "&lt;" and
 * "&gt;", with our \http{} macro. We are currently not sure, if other macros/replacements
 * are affected. Lets see.
 *
 * This function searches for
 *      &lt;img src="external_link.svg" height="12" width="10"&gt;
 * and replaces with
 *        <img src="external_link.svg" height="12" width="10">
 *
 *
 * @param file The opened and read file.
 * @param fileNameForLoggingOnly As it says.
 * @return The number of anchors fixed. If \c 0 is returned, the file was not changed.
 */
int fixExternalLinkImage(TextFile& file, const String& fileNameForLoggingOnly);
int fixExternalLinkImage(TextFile& file, const String& fileNameForLoggingOnly)
{
    Log_SetDomain( "ANCHORS", Scope::Method)

    ALIB_DBG( int cntLine= 0; )
    ALIB_REL( (void) fileNameForLoggingOnly; )
    int cntFixes= 0;

    // loop over all lines
    for(auto& line: file )
    {
        Log_Info("LINES", "Line {}: {}", cntLine, line )
        if ( line.IndexOf("&lt;img src=\"external_link.svg\" height=\"12\" width=\"10\"&gt;") )
        {
String8K oldLine(line);
auto     oldCntFixes= cntFixes;
            String8K newLine(line);
            cntFixes+=
              newLine.SearchAndReplace("&lt;img src=\"external_link.svg\" height=\"12\" width=\"10\"&gt;",
                                       "<img src=\"external_link.svg\" height=\"12\" width=\"10\">"    );
            line.Allocate( file.GetAllocator(), newLine );
if ( cntFixes > 0 && oldCntFixes < cntFixes )
 Log_Error("LINES", "Fixing line: {}\n         to: {}\n    in file: ", oldLine, line, fileNameForLoggingOnly )
        }
        ALIB_DBG(++cntLine;)
    }

    Log_If( cntFixes > 0, Verbosity::Info, "{} anchors fixed", cntFixes, fileNameForLoggingOnly )
    return cntFixes;
}

/**
 * This function searches all HTML files and calls #fixHTMLAnchors for each file.
 * If the latter changes the file, the file is written here.
 * @param srcDir The source directory to scan for HTML files.
 * @return \c 0 if all is well. Other values indicate some error.
 */
int postProcessHTMLFiles(const String& srcDir);
int postProcessHTMLFiles(const String& srcDir)
{
    Log_AddDebugLogger()
    Log_SetDomain( "DOXFX", Scope::Filename  )
    Log_SetVerbosity( "DEBUG_LOGGER", Verbosity::Warning  , "/ALIB/FILES/TXTF" )
    Log_SetVerbosity( "DEBUG_LOGGER", Verbosity::Verbose  , "/DOXFX" )
    Log_SetVerbosity( "DEBUG_LOGGER", Verbosity::Warning  , "/DOXFX/ANCHORS" )
    Log_SetVerbosity( "DEBUG_LOGGER", Verbosity::Warning  , "/DOXFX/ANCHORS/LINES" )

    //----------------------- scan html directory ---------------------------
    SharedFTree                 fileTree(10);
    ScanParameters              scanParameters(srcDir);
    std::vector<ResultsPaths>   resultPaths;
    FileExpressions             fex;

    scanParameters.IncludeArtificialFS= true;
    scanParameters.FileFilter= fex.CreateFilter(A_CHAR("EndsWith(name, \".html\")"));
    files::ScanFiles( fileTree, scanParameters, resultPaths);
    if( resultPaths.size() == 0 )
    {
        Log_Error( "No files found with given directory {} ", srcDir )
        return 1;
    }
    Log_Info( "Scan result paths: ", resultPaths.size() )

    //----------------- get recursive iterator from fileTree and iterate -----------------
    Path                      filePath;
    FTree::RecursiveIterator  rit;

    int sumFiles       = 0;
    int sumFixedFiles  = 0;
    int sumFixedAnchors= 0;
    rit.SetPathGeneration(lang::Switch::On);
    auto snapshot= ma.TakeSnapshot();
    for( rit.Initialize(*fileTree.Get()) ; rit.IsValid() ; rit.Next() )
    {
        if( rit.Node()->Type() != alib::files::FInfo::Types::REGULAR )
            continue;

        TextFile file( ma );
        ++sumFiles;
        rit.FullPath(filePath);
        file.Read( filePath );

        //int cntReplacements= fixHTMLAnchors(file, filePath);
        int cntReplacements= fixExternalLinkImage(file, filePath);
        if(cntReplacements > 0)
        {
            file.Write( filePath );
            ++sumFixedFiles;
            sumFixedAnchors+= cntReplacements;
        }
        ma.Reset(snapshot);
    }

    std::cout << sumFixedFiles << "/" << sumFiles << " files fixed. (" <<  sumFixedAnchors << " anchors)" << endl;

    return 0;
}
#include "ALib.Lang.CIMethods.H"
