// #################################################################################################
//  aworx::lib::lox::core - ALox Logging Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib.hpp"
#include "alib/alox.hpp"


// conditional expression is constant (which is a great thing for the compiler by the way ;-)
#if defined(_MSC_VER)
    #pragma warning( push )
    #pragma warning( disable : 4127 )
#endif

using namespace std;
namespace aworx { namespace lib { namespace lox { namespace core   {

// #################################################################################################
// Static
// #################################################################################################

std::vector<ScopeInfo::SourcePathTrimRule>  ScopeInfo::GlobalSPTRs;
int                                         ScopeInfo::DefaultCacheSize                         = 5;

bool                                        ScopeInfo::GlobalSPTRsReadFromConfig       = false;

ScopeInfo::ScopeInfo( const NString& pName, const std::map<int, String32>&  pThreadDictionary )
: loxName( pName )
, threadDictionary(pThreadDictionary)
{
    loxName.ToUpper();
    ALIB_ASSERT_ERROR( !loxName.Equals( "GLOBAL" ), ASTR("Name \"GLOBAL\" not allowed for Lox instances") );
    cache= new SourceFile[static_cast<size_t>(cacheSize= DefaultCacheSize)];
    lastSourceFile= &cache[0];

    // read trim rules from config
    // do 2 times, 0== local list, 1== global list
    std::vector<SourcePathTrimRule>* trimInfoList;
    for( int trimInfoNo= 0; trimInfoNo < 2 ; trimInfoNo++ )
    {
        // check if done... or set list
        Variable variable;
        if ( trimInfoNo == 0 )
        {
            trimInfoList= &LocalSPTRs;
            ALIB_NARROW_TO_STANDARD_TSTRING(loxName,sLoxName)
            variable.Declare( Variables::SPTR_LOX, sLoxName );
        }
        else
        {
            if ( GlobalSPTRsReadFromConfig )
                continue;
            GlobalSPTRsReadFromConfig= true;

            trimInfoList= &GlobalSPTRs;
            variable.Declare( Variables::SPTR_GLOBAL );
        }
        ALOX.Config->Load( variable );

        if( variable.Priority != Priorities::NONE  )
        {
            for( int ruleNo= 0; ruleNo< variable.Size(); ruleNo++ )
            {
                Tokenizer ruleTknzr( variable.GetString( ruleNo ), ',' );
                trimInfoList->emplace_back();
                SourcePathTrimRule& rule=trimInfoList->back();
                rule.Priority=  variable.Priority;

                ruleTknzr.Next();
                if( ( rule.IsPrefix= !ruleTknzr.Actual.StartsWith( ASTR("*") ) ) == false )
                    ruleTknzr.Actual.ConsumeChars(1);
                rule.Path._( ruleTknzr.Actual );
                if ( rule.Path.CharAtEnd() == '*' )
                    rule.Path.DeleteEnd( 1 );

                if ( rule.Path.IsEmpty() )
                {
                    trimInfoList->pop_back();
                    continue;
                }

                if( DirectorySeparator == '/' )
                    rule.Path.SearchAndReplace( '\\', '/'  );
                else
                    rule.Path.SearchAndReplace( '/' , '\\' );

                ruleTknzr.Next().ConsumeEnumOrBool( rule.IncludeString, Inclusion::Exclude, Inclusion::Include );
                ruleTknzr.Next().ConsumeInt( rule.TrimOffset );
                ruleTknzr.Next().ConsumeEnumOrBool( rule.Sensitivity, Case::Ignore, Case::Sensitive );

                rule.TrimReplacement=   ruleTknzr.Next();
            }
        }
    }
}

ScopeInfo::~ScopeInfo()
{
    delete[] cache;
}


void ScopeInfo::Set ( const NTString& sourceFileName, int lineNumber, const NTString& methodName,
                      Thread* pThread )
{
    //scopes.push( std::forward<Scope>(Scope()) );
    actScopeDepth++;
    ALIB_ASSERT( actScopeDepth < 8)
    if( scopes.size() == static_cast<size_t>( actScopeDepth ) )
        scopes.emplace_back();
    Scope& s= scopes[static_cast<size_t>(actScopeDepth)];


    s.timeStamp=    Ticks::Now();
    s.origLine=     lineNumber;
    s.origMethod=   methodName;
    s.sourceFile=   lastSourceFile;

    this->thread=   pThread;
    threadName=     nullptr;

    // if different file than before, search file in cache
    if ( s.sourceFile->origFile.Buffer() != sourceFileName.Buffer() )
    {
        int           oldestIdx= -1;
        uint64_t  oldestTime= ++cacheRun;

        s.sourceFile= nullptr;
        for( int i= 0; i< cacheSize; i++ )
        {
            if ( cache[i].origFile.Buffer() == sourceFileName.Buffer() )
            {
                s.sourceFile= &cache[i];
                break;
            }

            if( oldestTime > cache[i].timeStamp )
            {
                oldestTime= cache[i].timeStamp;
                oldestIdx= i;
            }
        }

        // not found? Use the oldest
        if ( s.sourceFile == nullptr )
        {
            s.sourceFile= &cache[oldestIdx];
            s.sourceFile->Clear();
            s.sourceFile->origFile= sourceFileName;
        }

        // mark as used
        s.sourceFile->timeStamp= cacheRun;
    }

}

void  ScopeInfo::SetSourcePathTrimRule( const NTString&     path,
                                        Inclusion           includeString,
                                        int                 trimOffset,
                                        Case                sensitivity,
                                        const NString&      trimReplacement,
                                        Reach               reach,
                                        Priorities          priority )
{
    // clear cache to have lazy variables reset with the next invocation
    for ( int i= 0; i< cacheSize; i++ )
        cache[i].Clear();

    // clear command
    if ( trimOffset == 999999 )
    {
        LocalSPTRs.clear();
        if ( reach == Reach::Global )
            GlobalSPTRs.clear();
        AutoDetectTrimableSourcePath=  (includeString == Inclusion::Include);

        // reset config read flag. This is done for unit testing. Not really useful/needed in real life.
        GlobalSPTRsReadFromConfig=  false;
        return;
    }

    // choose local or global list
    std::vector<SourcePathTrimRule>* trimInfoList=
                   reach == Reach::Global  ? &GlobalSPTRs
                                           : &LocalSPTRs;

    // insert sorted by priority
    auto it= trimInfoList->begin();
    while( it != trimInfoList->end() && priority < it->Priority  )
        it++;

    it= trimInfoList->insert(it, SourcePathTrimRule() );
    SourcePathTrimRule& rule= *it;
    rule.Priority= priority;

    // if path starts with '*', it is not a prefix. We store without *
    rule.Path._(path, (rule.IsPrefix= (path.CharAtStart() != '*') ) == true  ? 0 : 1 );
    if ( rule.Path.CharAtEnd() == '*' )
        rule.Path.DeleteEnd( 1 );
    if ( rule.Path.IsEmpty() )
    {
        trimInfoList->erase( it );
        return;
    }

    if( DirectorySeparator == '/' )
        rule.Path.SearchAndReplace( '\\', '/'  );
    else
        rule.Path.SearchAndReplace( '/' , '\\' );

    rule.IncludeString=   includeString;
    rule.TrimOffset=      trimOffset;
    rule.Sensitivity=     sensitivity;
    rule.TrimReplacement= trimReplacement;
    if( DirectorySeparator == '/' )
        rule.TrimReplacement.SearchAndReplace( '\\', '/'  );
    else
        rule.TrimReplacement.SearchAndReplace( '/' , '\\' );
}


void ScopeInfo::trimPath()
{
    bool trimmed= false;

    SourceFile* actual= scopes[static_cast<size_t>(actScopeDepth)].sourceFile;
    integer idx= getPathLength();
    if( idx < 0 )
    {
        actual->trimmedPath= "";
        return;
    }
    actual->trimmedPath= NString( actual->origFile.Buffer(), idx );


    // do 2 times, 0== local list, 1== global list
    for( int trimInfoNo= 0; trimInfoNo < 2 ; trimInfoNo++ )
    {
        // choose local or global list
        std::vector<SourcePathTrimRule>* trimInfoList=
                   trimInfoNo == 0   ? &LocalSPTRs
                                     : &GlobalSPTRs;

        // loop over trimInfo
        for ( auto& ti : *trimInfoList )
        {
            if( ti.IsPrefix )
                idx= ( ti.Sensitivity == Case::Sensitive  ? actual->trimmedPath.StartsWith<true,Case::Sensitive>( ti.Path )
                                                          : actual->trimmedPath.StartsWith<true,Case::Ignore   >( ti.Path )
                     )

                     ? 0 : -1;
            else
                idx= ti.Sensitivity == Case::Sensitive  ? actual->trimmedPath.IndexOf<true, Case::Sensitive>( ti.Path )
                                                        : actual->trimmedPath.IndexOf<true, Case::Ignore   >( ti.Path );
            if ( idx >= 0 )
            {
                integer startIdx= idx + ( ti.IncludeString == Inclusion::Include ? ti.Path.Length() : 0 ) + ti.TrimOffset;
                actual->trimmedPath=        actual->trimmedPath.Substring<false>( startIdx, actual->trimmedPath.Length() - startIdx );
                actual->trimmedPathPrefix=  ti.TrimReplacement;

                trimmed= true;
                break;
            }
        }

        if (trimmed)
            break;
    }

    // if nothing was found and the flag is still set, try once to auto-detect rule
    // from the 'difference' of source path and current working directory
    if( !trimmed && AutoDetectTrimableSourcePath )
    {
        AutoDetectTrimableSourcePath= false; // do this only once

        // get system execution path and compare to file path
        String256 currentDir;
        ALIB_WARN_ONCE_PER_INSTANCE_DISABLE( currentDir,  ReplaceExternalBuffer );

        Directory::CurrentDirectory( currentDir );

        // Get the prefix that is the same in both paths
        integer i= 0;
        integer maxIdx= currentDir.Length();
        if ( maxIdx > actual->trimmedPath.Length() )
            maxIdx= actual->trimmedPath.Length();

        while (  i < maxIdx &&  tolower( currentDir[i] ) == tolower( actual->trimmedPath[i] )  )
            i++;

        if ( i > 1 )
        {
            currentDir.SetLength<false>( i );
            NTString origFile= actual->origFile;
                ALIB_STD_TO_NARROW_TSTRING(currentDir,nCurrentDir)
                SetSourcePathTrimRule( nCurrentDir, Inclusion::Include, 0, Case::Ignore, NNullString,
                                       Reach::Local, Priorities::AutoDetected );
            actual->origFile= origFile;
            trimPath(); // one recursive call
        }
    }
}


}}}}// namespace [aworx::lib::lox::core]

#if defined(_MSC_VER)
    #pragma warning( pop )
#endif
