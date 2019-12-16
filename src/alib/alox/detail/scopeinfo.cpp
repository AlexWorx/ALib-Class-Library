// #################################################################################################
//  aworx::lib::lox::detail - ALox Logging Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)
#   if !defined(HPP_ALIB_ALOX)
#      include "alib/alox/alox.hpp"
#   endif

#   if !defined(HPP_ALIB_ALOXMODULE)
#      include "alib/alox/aloxmodule.hpp"
#   endif

#   if !defined(HPP_ALOX_DETAIL_SCOPEINFO)
#      define HPP_ALIB_LOX_PROPPERINCLUDE
#          include "alib/alox/detail/scopeinfo.inl"
#      undef HPP_ALIB_LOX_PROPPERINCLUDE
#   endif

#   if !defined(HPP_ALIB_FS_COMMONENUMS)
#       include "alib/lib/fs_commonenums/commonenums.hpp"
#   endif

#   if !defined(HPP_ALIB_ENUMS_SERIALIZATION)
#      include "alib/enums/serialization.hpp"
#   endif
#   if !defined(HPP_ALIB_STRINGS_UTIL_TOKENIZER)
#      include "alib/strings/util/tokenizer.hpp"
#   endif
#endif // !defined(ALIB_DOX)


// conditional expression is constant (which is a great thing for the compiler by the way ;-)
#if defined(_MSC_VER)
    #pragma warning( push )
    #pragma warning( disable : 4127 )
#endif

namespace aworx { namespace lib { namespace lox { namespace detail {

// #################################################################################################
// Static
// #################################################################################################

std::vector<ScopeInfo::SourcePathTrimRule>  ScopeInfo::GlobalSPTRs;
int                                         ScopeInfo::DefaultCacheSize                         = 5;

bool                                        ScopeInfo::GlobalSPTRsReadFromConfig       = false;

ScopeInfo::ScopeInfo( const NString& pName, MonoAllocator* allocator, Variable& tempVar )
#if ALIB_THREADS
: threadDictionary( allocator )
,
#else
:
#endif
 scopes          ( *allocator )
{
    loxName= allocator->EmplaceString( pName );
    characters::CharArray<nchar>::ToUpper( const_cast<char*>( loxName.Buffer() ), loxName.Length() );
    ALIB_ASSERT_ERROR( !loxName.Equals( "GLOBAL" ), "Name \"GLOBAL\" not allowed for Lox instances" )
    cache= allocator->EmplaceArray<SourceFile>( cacheSize= DefaultCacheSize );

    lastSourceFile= &cache[0];

    ALIB_IF_THREADS( threadDictionary.EmplaceUnique(Thread::GetMain()->GetId(), "PROCESS" ); )

    // read trim rules from config
    // do 2 times, 0== local list, 1== global list
    std::vector<SourcePathTrimRule>* trimInfoList;
    for( int trimInfoNo= 0; trimInfoNo < 2 ; ++trimInfoNo )
    {
        // check if done... or set list
        if ( trimInfoNo == 0 )
        {
            trimInfoList= &LocalSPTRs;
            ALIB_STRINGS_FROM_NARROW(loxName,sLoxName, 1024)
            tempVar.Declare( Variables::SPTR_LOX, sLoxName );
        }
        else
        {
            if ( GlobalSPTRsReadFromConfig )
                continue;
            GlobalSPTRsReadFromConfig= true;

            trimInfoList= &GlobalSPTRs;
            tempVar.Declare( Variables::SPTR_GLOBAL );
        }
        ALOX.GetConfig().Load( tempVar );

        if( tempVar.Priority() != Priorities::NONE  )
        {
            for( int ruleNo= 0; ruleNo < tempVar.Size(); ++ruleNo )
            {
                Tokenizer ruleTknzr( tempVar.GetString( ruleNo ), ',' );
                trimInfoList->emplace_back();
                SourcePathTrimRule& rule=trimInfoList->back();
                rule.Priority=  tempVar.Priority();

                ruleTknzr.Next();
                if( ( rule.IsPrefix= !ruleTknzr.Actual.StartsWith( A_CHAR("*") ) ) == false )
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

                enums::ParseEnumOrTypeBool( ruleTknzr.Next(), rule.IncludeString, Inclusion::Exclude, Inclusion::Include );
                ruleTknzr.Next().ConsumeInt( rule.TrimOffset );
                enums::ParseEnumOrTypeBool( ruleTknzr.Next(), rule.Sensitivity, Case::Ignore, Case::Sensitive );

                rule.TrimReplacement.Reset( ruleTknzr.Next() );
            }
        }
    }
}

void ScopeInfo::Set ( const NCString& sourceFileName, int lineNumber, const NCString& methodName
                      ALIB_IF_THREADS(, Thread* pThread ) )
{
    ++actScopeDepth;
    ALIB_ASSERT( actScopeDepth < 8)
    if( scopes.size() == static_cast<size_t>(actScopeDepth) )
        scopes.emplace_back();
    Scope& s= scopes[static_cast<size_t>(actScopeDepth)];


    s.timeStamp=    Ticks::Now();
    s.origLine=     lineNumber;
    s.origMethod=   methodName;
    s.sourceFile=   lastSourceFile;

    ALIB_IF_THREADS(
    this->thread=   pThread;
    threadName=     nullptr; )

    // if different file than before, search file in cache
    if ( s.sourceFile->origFile.Buffer() != sourceFileName.Buffer() )
    {
        int           oldestIdx= -1;
        uint64_t  oldestTime= ++cacheRun;

        s.sourceFile= nullptr;
        for( int i= 0; i< cacheSize; ++i )
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

void  ScopeInfo::SetSourcePathTrimRule( const NCString&     path,
                                        Inclusion           includeString,
                                        int                 trimOffset,
                                        Case                sensitivity,
                                        const NString&      trimReplacement,
                                        Reach               reach,
                                        Priorities          priority )
{
    // clear cache to have lazy variables reset with the next invocation
    for ( int i= 0; i< cacheSize; ++i )
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
        ++it;

    it= trimInfoList->emplace(it);
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
    rule.TrimReplacement.Reset( trimReplacement );
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
    for( int trimInfoNo= 0; trimInfoNo < 2 ; ++trimInfoNo )
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
                actual->trimmedPath=        actual->trimmedPath.Substring( startIdx, actual->trimmedPath.Length() - startIdx );
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
        currentDir.DbgDisableBufferReplacementWarning();

        Directory::CurrentDirectory( currentDir );

        // Get the prefix that is the same in both paths
        integer i= 0;
        integer maxIdx= currentDir.Length();
        if ( maxIdx > actual->trimmedPath.Length() )
             maxIdx=  actual->trimmedPath.Length();

        while (  i < maxIdx &&    characters::CharArray<character>::ToUpper( currentDir[i] )
                               == characters::CharArray<character>::ToUpper( static_cast<character>(actual->trimmedPath[i]) )  )
            ++i;

        if ( i > 1 )
        {
            currentDir.ShortenTo( i );
            NCString origFile= actual->origFile;
            ALIB_STRINGS_TO_NARROW(currentDir,nCurrentDir,1024)
            SetSourcePathTrimRule( nCurrentDir, Inclusion::Include, 0, Case::Ignore, NullNString(),
                                   Reach::Local, Priorities::AutoDetected );
            actual->origFile= origFile;
            trimPath(); // one recursive call
        }
    }
}


}}}}// namespace [aworx::lib::lox::detail]

#if defined(_MSC_VER)
    #pragma warning( pop )
#endif
