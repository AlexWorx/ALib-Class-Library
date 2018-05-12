// #################################################################################################
//  aworx::lib::lox - ALox Logging Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib.hpp"

#if !defined (HPP_ALIB_ALOX)
    #include "alib/alox/alox.hpp"
#endif

#if !defined (HPP_ALIB_CONFIG_CONFIGURATION)
    #include "alib/config/configuration.hpp"
#endif
#if !defined (HPP_ALIB_TIME_TICKSCONVERTER)
    #include "alib/time/ticksconverter.hpp"
#endif
#if !defined (HPP_ALIB_TIME_CALENDAR)
    #include "alib/time/calendar.hpp"
#endif
#if !defined (HPP_ALOX_CORE_SCOPEDUMP)
    #include "core/scopedump.hpp"
#endif
#if !defined (HPP_ALIB_CONTAINERS_STRINGTREE)
    #include "alib/containers/stringtree.hpp"
#endif

#if !defined (_GLIBCXX_ALGORITHM) && !defined(_ALGORITHM_)
    #include <algorithm>
#endif

// For code compatibility with ALox Java/C++
// We have to use underscore as the start of the name and for this have to disable a compiler
// warning. But this is a local code (cpp file) anyhow.
#if defined(__clang__)
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif

    #define _NC _<false>

#if defined(__clang__)
    #pragma clang diagnostic pop
#endif


using namespace std;

namespace aworx { namespace lib { namespace lox {

using namespace core;


//! @cond NO_DOX
void getStateDomainRecursive( Domain& domain, integer maxDomainPathLength, AString& buf );
void getStateDomainRecursive( Domain& domain, integer maxDomainPathLength, AString& buf )
{
    integer reference= buf.Length();
    buf._("  "); domain.ToString( buf );
    integer idx= buf.IndexOf( '[', reference );
    buf.InsertChars( ' ', maxDomainPathLength + 5 - idx + reference, idx);
    buf.NewLine();

    // loop over all sub domains (recursion)
    for ( Domain* subDomain : domain.SubDomains )
        getStateDomainRecursive( *subDomain, maxDomainPathLength, buf );
}

void getStateDomainsWithDiffVerb( Domain* dom, int loggerNo, vector<Domain*>& results );
void getStateDomainsWithDiffVerb( Domain* dom, int loggerNo, vector<Domain*>& results )
{
    if (    dom->Parent == nullptr
        ||  dom->Parent->GetVerbosity(loggerNo) != dom->GetVerbosity(loggerNo) )
        results.emplace_back( dom );

    for( auto* it : dom->SubDomains )
        getStateDomainsWithDiffVerb( it, loggerNo, results );
}

void getStateCollectPrefixes( Domain* dom, integer indentSpaces, AString& target );
void getStateCollectPrefixes( Domain* dom, integer indentSpaces, AString& target )
{
    for ( auto& pfl : dom->PrefixLogables )
    {
        target.InsertChars( ' ', indentSpaces );
        target << '"';
        integer actLen= target.Length();
        target._( static_cast<Box*>(pfl.first) );
        ESC::ReplaceToReadable( target, actLen );
        target << Format::Escape( Switch::On, actLen );
        target << '"';
        if ( pfl.second == Inclusion::Exclude )
            target._NC( ASTR(" (Excl.)") );
        target._NC( Format::Tab( 25, -1 ) );
        target._NC( ASTR("<domain>           [") )._NC( dom->FullPath )._NC(']').NewLine();
    }

    for( auto* it : dom->SubDomains )
        getStateCollectPrefixes( it, indentSpaces, target );
}
//! @endcond

void Lox::GetState( AString& buf, StateInfo flags )
{
    ALIB_ASSERT_ERROR ( Lock.GetSafeness() == Safeness::Unsafe || Lock.CountAcquirements() > 0,
                        ASTR("Lox not acquired") );


    ScopeDump scopeDump( threadDictionary, noKeyHashKey, buf );

    if ( EnumContains( flags, StateInfo::CompilationFlags ) )
    {
        buf._NC( ASTR("ALib Version:      ") )._NC( lib::ALIB.Version)._NC(ASTR(" (Rev. "))._( lib::ALIB.Revision)._(')').NewLine();
        buf._NC( ASTR("ALib compilation symbols:") ).NewLine();
        {
            for( auto& p : lib::ALIB.CompilationFlagMeanings )
            {
                buf << ASTR("  ") << Format::Field( p.first, 28, Alignment::Left ) << ':'
                    << (lib::ALIB.CompilationFlags & p.second  ? ASTR(" On") : ASTR(" Off"))
                    << NewLine;
            }
        }

        buf._NC( ASTR("ALox compilation symbols:") ).NewLine();
        {
            for( auto& p : lib::ALOX.CompilationFlagMeanings )
            {
                buf << ASTR("  ") << Format::Field( p.first, 28, Alignment::Left ) << ':'
                    << (lib::ALOX.CompilationFlags & p.second  ? ASTR(" On") : ASTR(" Off"))
                    << NewLine;
            }
        }
        buf.NewLine();
    }

    // basic lox info
    if( EnumContains( flags,  StateInfo::Basic ) )
        buf._NC( ASTR("Name:            \"" ))._( scopeInfo.GetLoxName() )._('\"').NewLine();

    if( EnumContains( flags,  StateInfo::Version ) )
    {
        buf._NC( ASTR("Version:         ") )._NC( ALOX.Version)._NC(ASTR(" (Rev. "))._( ALOX.Revision)._(')').NewLine();
        buf._NC( ASTR("Thread Safeness: ") )._NC( Lock.GetSafeness() ).NewLine();
    }

    if( EnumContains( flags,  StateInfo::Basic ) )
        buf._NC( ASTR("#Log Calls:      ") )._NC( CntLogCalls     ).NewLine();

    if(    EnumContains( flags,  StateInfo::Basic )
        || EnumContains( flags,  StateInfo::Version )  )
        buf.NewLine();

    //  source path trim info
    if( EnumContains( flags,  StateInfo::SPTR ) )
    {
        buf._NC( ASTR("Source Path Trimming Rules: ") ).NewLine();

        int cnt= 0;
        // do 2 times, 0== global list, 1 == local list
        for( int trimInfoNo= 0; trimInfoNo < 2 ; trimInfoNo++ )
        {
            // choose local or global list
            std::vector<ScopeInfo::SourcePathTrimRule>* trimInfoList=
                       trimInfoNo == 0   ? &ScopeInfo::GlobalSPTRs
                                         : &scopeInfo.LocalSPTRs;


            // loop over trimInfo
            for ( auto& ti : *trimInfoList )
            {
                cnt++;
                buf._NC( trimInfoNo == 0 ? ASTR("  Global: ")
                                         : ASTR("  Local:  ") );
                buf._NC( ti.IsPrefix ?  ASTR("\"") : ASTR("\"*"));
                buf._NC( ti.Path )._NC( ASTR("\", ") );
                buf._NC( ti.IncludeString );
                if ( ti.TrimOffset != 0 )
                    buf._NC( ti.Path )._NC( ASTR("\", Offset: ") )._NC( ti.TrimOffset );
                buf._NC( ASTR(", Priority: ") )._( ti.Priority );
                buf.NewLine();
            }
        }


        if ( cnt == 0 )
            buf._NC(ASTR("  <no rules set>") ).NewLine();
        buf.NewLine();
    }

    //  domain substitutions
    if( EnumContains( flags,  StateInfo::DSR ) )
    {
        buf._NC( ASTR("Domain Substitution Rules: ") ).NewLine();
        if( domainSubstitutions.size() > 0 )
        {
            // get size
            integer maxWidth= 0;
            for ( auto& it : domainSubstitutions )
                if ( maxWidth < it.Search.Length() )
                     maxWidth = it.Search.Length();
            maxWidth+= 2;

            // write
            for ( auto& it : domainSubstitutions )
            {
                buf._NC( ASTR("  ") );
                if (    it.type == DomainSubstitutionRule::Type::EndsWith
                     || it.type == DomainSubstitutionRule::Type::Substring )
                    buf._NC( '*' );

                buf._NC( it.Search );
                if (    it.type == DomainSubstitutionRule::Type::StartsWith
                     || it.type == DomainSubstitutionRule::Type::Substring )
                    buf._NC( '*' );

                buf._NC( Format::Tab( maxWidth, -1, 0 ) )
                   ._NC( ASTR(" -> ") )
                   ._NC( it.Replacement );
                buf.NewLine();
            }
        }
        else
            buf._NC(ASTR("  <no rules set>") ).NewLine();
        buf.NewLine();
    }

    // Log Once Counters
    if( EnumContains( flags,  StateInfo::Once ) )
    {
        buf._NC( ASTR("Once() Counters: ") ).NewLine();
        if ( scopeDump.writeStoreMap( &scopeLogOnce ) == 0 )
            buf._NC(ASTR("  <no Once() counters set>") ).NewLine();
        buf.NewLine();
    }

    // Log Data
    if( EnumContains( flags,  StateInfo::LogData ) )
    {
        buf._NC( ASTR("Log Data: ") ).NewLine();
        if ( scopeDump.writeStoreMap( &scopeLogData ) == 0 )
            buf._NC(ASTR("  <no data objects stored>") ).NewLine();
        buf.NewLine();
    }

    // Prefix Logables
    if( EnumContains( flags,  StateInfo::PrefixLogables ) )
    {
        buf._NC( ASTR("Prefix Logables: ") ).NewLine();
        integer oldLength= buf.Length();
        scopeDump.writeStore( &scopePrefixes, 2 );
        getStateCollectPrefixes( &domains, 2, buf );
        if ( oldLength == buf.Length() )
            buf._NC(ASTR("  <no prefix logables set>") ).NewLine();
        buf.NewLine();
    }

    // thread mappings
    if( EnumContains( flags,  StateInfo::ThreadMappings ) )
    {
        buf._NC( ASTR("Named Threads:   ") ).NewLine();
        if ( threadDictionary.size() == 0 )
            buf._NC(ASTR("  <no thread name mappings set>") ).NewLine();
        else
            for ( auto& pair : threadDictionary )
            {
                buf._NC( ASTR("  ") ) << Format::Field( String32() << '(' << pair.first << ASTR("):"), 7, Alignment::Left )
                                << '\"' << pair.second << '\"';
                buf.NewLine();
            }
        buf.NewLine();
    }

    // Scope Domains
    if( EnumContains( flags,  StateInfo::ScopeDomains ) )
    {
        buf._NC( ASTR("Scope Domains: ") ).NewLine();
        if ( scopeDump.writeStore( &scopeDomains, 2 ) == 0 )
            buf._NC(ASTR("  <no scope domains set>") ).NewLine();
        buf.NewLine();
    }

    // Loggers
    if( EnumContains( flags,  StateInfo::Loggers ) )
    {
        TicksConverter dateTimeConverter;
        vector<Domain*> domainsWithDiffVerb;
        for (int treeNo= 0; treeNo < 2; ++treeNo )
        {
            int cnt= 0;
            Domain* domTree;
            if( treeNo==0 )
            {
                 domTree= &domains;
                 buf._NC( ASTR("Loggers:") ).NewLine();
            }
            else
            {
                 domTree= &internalDomains;
                 buf._NC( ASTR("Loggers on Internal Domains:") ).NewLine();
            }

            for ( int loggerNo= 0; loggerNo< domTree->CountLoggers(); loggerNo++ )
            {
                cnt++;
                String64 as64;
                CalendarDateTime ct(Initialization::Suppress);

                Logger* logger= domTree->GetLogger(loggerNo);
                buf._NC( ASTR("  ")  )._NC( logger  ).NewLine();
                buf._NC( ASTR("    Lines logged:  ")     )._NC( logger->CntLogs                                   ).NewLine();

                ct.Set( dateTimeConverter.ToDateTime(logger->TimeOfCreation) );
                buf._NC( ASTR("    Creation time: ")     )._NC( ct.Format( ASTR("yyyy-MM-dd HH:mm:ss"), as64._()) ).NewLine();

                ct.Set( dateTimeConverter.ToDateTime(logger->TimeOfLastLog) );
                buf._NC( ASTR("    Last log time: ")     )._NC( ct.Format( ASTR("yyyy-MM-dd HH:mm:ss"), as64._()) ).NewLine();

                domainsWithDiffVerb.clear();
                getStateDomainsWithDiffVerb( domTree, loggerNo, domainsWithDiffVerb);
                for ( Domain* dom : domainsWithDiffVerb )
                {
                    buf._NC( ASTR("    ") )
                       ._(  dom == *domainsWithDiffVerb.begin() ? ASTR("Verbosities:   ")
                                                                : ASTR("               ") );

                    integer tabRef= buf.Length();
                    buf << dom->FullPath << Format::Tab( maxDomainPathLength +1, tabRef);

                    buf << ASTR("= ") << std::make_pair(dom->GetVerbosity( loggerNo ), dom->GetPriority(loggerNo) )
                        << NewLine;
                }
            }
            if ( cnt == 0 )
                buf._NC(ASTR("  <no loggers attached>") ).NewLine();
            buf.NewLine();
        }
    }

    // Internal Domains
    if( EnumContains( flags,  StateInfo::InternalDomains ) )
    {
        buf._NC( ASTR("Internal Domains:") ).NewLine();
        getStateDomainRecursive( internalDomains, maxDomainPathLength, buf );
        buf.NewLine();
    }

    // Domains
    if( EnumContains( flags,  StateInfo::Domains ) )
    {
        buf._NC( ASTR("Domains:") ).NewLine();
        getStateDomainRecursive( domains,  maxDomainPathLength,  buf );
        buf.NewLine();
    }
}


}}}// namespace [aworx::lib::lox]
