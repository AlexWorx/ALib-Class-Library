// #################################################################################################
//  alib::lox::detail - ALox Logging Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !DOXYGEN
#   include "alib/alox/alox.hpp"
#   include "alib/alox/aloxcamp.hpp"
#   define HPP_ALIB_LOX_PROPPERINCLUDE
#       include "alib/alox/detail/scopeinfo.inl"
#   undef HPP_ALIB_LOX_PROPPERINCLUDE
#   include "alib/compatibility/std_strings_functional.hpp"
#   include "alib/enums/serialization.hpp"
#   include "alib/strings/util/tokenizer.hpp"
#endif // !DOXYGEN


// conditional expression is constant (which is a great thing for the compiler by the way ;-)
#if defined(_MSC_VER)
    #pragma warning( push )
    #pragma warning( disable : 4127 )
#endif

using namespace alib::lang::system;

namespace alib {  namespace lox { namespace detail {

// #################################################################################################
// Static
// #################################################################################################

std::vector<ScopeInfo::SourcePathTrimRule>  ScopeInfo::GlobalSPTRs;
bool                                        ScopeInfo::GlobalSPTRsReadFromConfig       = false;

ScopeInfo::ScopeInfo( const NString& pName, MonoAllocator& allocator )
#if ALIB_THREADS
: threadDictionary   (allocator)
,
#else
:
#endif
  parsedFileNameCache(allocator, 4, 6)
, callStack         (allocator)
, loxName            (allocator, pName)
{
    callStack.reserve(2);
    characters::ToUpper( const_cast<char*>( loxName.Buffer() ), loxName.Length() );
    ALIB_ASSERT_ERROR( !loxName.Equals<NC>( "GLOBAL" ), "ALOX", "Name \"GLOBAL\" not allowed for Lox instances" )

    IF_ALIB_THREADS( threadDictionary.EmplaceUnique(Thread::GetMain()->GetID(), "PROCESS" ); )

    // read trim rules from config
    // do 2 times, 0== local list, 1== global list
    std::vector<SourcePathTrimRule>* trimInfoList;
    for( int trimInfoNo= 0; trimInfoNo < 2 ; ++trimInfoNo )
    { ALIB_LOCK_WITH(ALOX.GetConfigLock())
        Variable variable(ALOX);
        
        // check if done... or set list
        if ( trimInfoNo == 0 )
        {
            trimInfoList= &LocalSPTRs;
            ALIB_STRINGS_FROM_NARROW(loxName,sLoxName, 1024)
            variable.Declare( Variables::SPTR_LOX,  sLoxName );
        }
        else
        {
            if ( GlobalSPTRsReadFromConfig )
                continue;
            GlobalSPTRsReadFromConfig= true;

            trimInfoList= &GlobalSPTRs;
            variable.Declare( Variables::SPTR_GLOBAL );
        }

        if( variable.IsDefined()  )
        {
            Tokenizer tokOuter;
            tokOuter.Set(variable, ';', true);
            while(tokOuter.HasNext())
            {
                Tokenizer ruleTknzr( tokOuter.Next(), ',' );
                trimInfoList->emplace_back();
                SourcePathTrimRule& rule=trimInfoList->back();
                rule.Priority=  variable.GetPriority();

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

                if( DIRECTORY_SEPARATOR == '/' )
                    rule.Path.SearchAndReplace( '\\', '/'  );
                else
                    rule.Path.SearchAndReplace( '/' , '\\' );

                enums::ParseEnumOrTypeBool( ruleTknzr.Next(), rule.IncludeString, lang::Inclusion::Exclude, lang::Inclusion::Include );
                ruleTknzr.Next().ConsumeInt( rule.TrimOffset );
                enums::ParseEnumOrTypeBool( ruleTknzr.Next(), rule.Sensitivity, lang::Case::Ignore, lang::Case::Sensitive );

                rule.TrimReplacement.Reset( ruleTknzr.Next() );
            }
        }
    }
}

void ScopeInfo::Set ( const lang::CallerInfo& ci )
{
    ++callStackSize;
    ALIB_ASSERT( callStackSize < 8)
    if( callStack.size() == size_t(callStackSize) )
        callStack.emplace_back();

    FrameRecord& scope= callStack[size_t(callStackSize)];

    scope.timeStamp =  Ticks::Now();
    scope.origLine  =  ci.Line;
    scope.origMethod=  ci.Func;
    auto resultPair =   parsedFileNameCache.Try( ci.File ); // search file in cache
    if( resultPair.first == false )
        resultPair.second.Construct(ci.File);
    scope.Parsed    =  &*resultPair.second;

   // we must not use ci.ThreadID, because this might be nulled with release logging
    IF_ALIB_THREADS(  threadNativeIDx= std::this_thread::get_id();
                      thread        = nullptr;
                      threadName    = nullptr;
                      )
}

void  ScopeInfo::SetSourcePathTrimRule( const NCString&     path,
                                        lang::Inclusion     includeString,
                                        int                 trimOffset,
                                        lang::Case          sensitivity,
                                        const NString&      trimReplacement,
                                        lang::Reach         reach,
                                        Priority            priority )
{
    // clear cache to have lazy variables reset with the next invocation
    parsedFileNameCache.Clear();

    // clear command
    if ( trimOffset == 999999 )
    {
        LocalSPTRs.clear();
        if ( reach == lang::Reach::Global )
            GlobalSPTRs.clear();
        AutoDetectTrimableSourcePath=  (includeString == lang::Inclusion::Include);

        // reset config read flag. This is done for unit testing. Not really useful/needed in real life.
        GlobalSPTRsReadFromConfig=  false;
        return;
    }

    // choose local or global list
    std::vector<SourcePathTrimRule>* trimInfoList=
                   reach == lang::Reach::Global  ? &GlobalSPTRs
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

    if( DIRECTORY_SEPARATOR == '/' )
        rule.Path.SearchAndReplace( '\\', '/'  );
    else
        rule.Path.SearchAndReplace( '/' , '\\' );

    rule.IncludeString=   includeString;
    rule.TrimOffset=      trimOffset;
    rule.Sensitivity=     sensitivity;
    rule.TrimReplacement.Reset( trimReplacement );
    if( DIRECTORY_SEPARATOR == '/' )
        rule.TrimReplacement.SearchAndReplace( '\\', '/'  );
    else
        rule.TrimReplacement.SearchAndReplace( '/' , '\\' );
}


void ScopeInfo::trimPath()
{
    bool trimmed= false;

    ParsedFileName* actual= callStack[size_t(callStackSize)].Parsed;
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
                idx= ( ti.Sensitivity == lang::Case::Sensitive  ? actual->trimmedPath.StartsWith<CHK,lang::Case::Sensitive>( ti.Path )
                                                                : actual->trimmedPath.StartsWith<CHK,lang::Case::Ignore   >( ti.Path )
                     )

                     ? 0 : -1;
            else
                idx= ti.Sensitivity == lang::Case::Sensitive  ? actual->trimmedPath.IndexOf<CHK, lang::Case::Sensitive>( ti.Path )
                                                              : actual->trimmedPath.IndexOf<CHK, lang::Case::Ignore   >( ti.Path );
            if ( idx >= 0 )
            {
                integer startIdx= idx + ( ti.IncludeString == lang::Inclusion::Include ? ti.Path.Length() : 0 ) + ti.TrimOffset;
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
        Path currentDir(SystemFolders::Current);

        // Get the prefix that is the same in both paths
        integer i= 0;
        integer maxIdx= currentDir.Length();
        if ( maxIdx > actual->trimmedPath.Length() )
             maxIdx=  actual->trimmedPath.Length();

        while (  i < maxIdx &&    characters::ToUpper( currentDir[i] )
                               == characters::ToUpper( static_cast<character>(actual->trimmedPath[i]) )  )
            ++i;

        if ( i > 1 )
        {
            currentDir.ShortenTo( i );
            NCString origFile= actual->origFile;
            ALIB_PATH_TO_NARROW(currentDir,nCurrentDir,1024)
            SetSourcePathTrimRule( nCurrentDir, lang::Inclusion::Include, 0, lang::Case::Ignore, NULL_NSTRING,
                                   lang::Reach::Local, Priority::AutoDetected );
            actual->origFile= origFile;
            trimPath(); // one recursive call
        }
    }
}


}}} // namespace [alib::lox::detail]

#if defined(_MSC_VER)
    #pragma warning( pop )
#endif

