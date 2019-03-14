// #################################################################################################
//  aworx::lib::lox - ALox Logging Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined (HPP_ALIB_ALOX)
    #include "alib/alox/alox.hpp"
#endif

#if !defined (HPP_ALIB_TIME_TICKSCONVERTER)
    #include "alib/time/tickconverter.hpp"
#endif

#if !defined (HPP_ALIB_SYSTEM_CALENDAR)
    #include "alib/system/calendar.hpp"
#endif

#if !defined (HPP_ALOX_CORE_SCOPEDUMP)
    #define HPP_ALIB_LOX_PROPPERINCLUDE
    #   include "alib/alox/detail/scopedump.inl"
    #undef HPP_ALIB_LOX_PROPPERINCLUDE
#endif

#if !defined (HPP_ALIB_LIB_COMMONENUMS_RESOURCED)
    #include "alib/lib/commonenumsresourced.hpp"
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



namespace aworx { namespace lib { namespace lox {

using namespace detail;


//! @cond NO_DOX
void getStateDomainRecursive( Domain& domain, integer maxDomainPathLength, NAString& buf );
void getStateDomainRecursive( Domain& domain, integer maxDomainPathLength, NAString& buf )
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

void getStateDomainsWithDiffVerb( Domain* dom, int loggerNo, std::vector<Domain*>& results );
void getStateDomainsWithDiffVerb( Domain* dom, int loggerNo, std::vector<Domain*>& results )
{
    if (    dom->Parent == nullptr
        ||  dom->Parent->GetVerbosity(loggerNo) != dom->GetVerbosity(loggerNo) )
        results.emplace_back( dom );

    for( auto* it : dom->SubDomains )
        getStateDomainsWithDiffVerb( it, loggerNo, results );
}

void getStateCollectPrefixes( Domain* dom, integer indentSpaces, NAString& target );
void getStateCollectPrefixes( Domain* dom, integer indentSpaces, NAString& target )
{
    AString buffer;
    for ( auto& pfl : dom->PrefixLogables )
    {
        buffer.InsertChars( ' ', indentSpaces );
        buffer << '"';
        integer actLen= buffer.Length();
        buffer._( *static_cast<Box*>(pfl.first) );
        ESC::ReplaceToReadable( buffer, actLen );
        buffer << Format::Escape( Switch::On, actLen );
        buffer << '"';
        if ( pfl.second == Inclusion::Exclude )
            buffer._NC( " (Excl.)" );
        buffer._NC( Format::Tab( 25, -1 ) );
        buffer._NC( "<domain>           [" )._NC( dom->FullPath )._NC(']').NewLine();
    }
    target << buffer;

    for( auto* it : dom->SubDomains )
        getStateCollectPrefixes( it, indentSpaces, target );
}
//! @endcond

void Lox::GetState( NAString& buf, StateInfo flags )
{
    ALIB_ASSERT_ERROR ( Lock.GetSafeness() == Safeness::Unsafe || Lock.CountAcquirements() > 0,
                        "Lox not acquired" )


    ScopeDump scopeDump( scopeInfo.threadDictionary, noKeyHashKey, buf );

    if ( EnumContains( flags, StateInfo::CompilationFlags ) )
    {
        buf._NC( "ALib Version:      " )._NC( ALIB.Version)
           ._NC(" (Rev. ")                ._( ALIB.Revision)._(')').NewLine();
        buf._NC( "ALib compilation symbols:" ).NewLine();
        {
            for( auto& p : ALIB.CompilationFlagMeanings )
            {
                buf << "  " << NFormat::Field( p.first, 41, Alignment::Left ) << ':'
                    << (ALIB.CompilationFlags & p.second  ? " On" : " Off")
                    << NewLine();
            }
        }

        buf.NewLine();
    }

    // basic lox info
    if( EnumContains( flags,  StateInfo::Basic ) )
        buf._NC( "Name:            \"" )._( scopeInfo.GetLoxName() )._('\"').NewLine();

    if( EnumContains( flags,  StateInfo::Version ) )
    {
        buf._NC( "Version:         " )._NC( ALOX.Version)
           ._NC(" (Rev. "            )._(   ALOX.Revision)._(')').NewLine();
        buf._NC( "Thread Safeness: " )._NC( Lock.GetSafeness() ).NewLine();
    }

    if( EnumContains( flags,  StateInfo::Basic ) )
        buf._NC( "#Log Calls:      " )._NC( CntLogCalls     ).NewLine();

    if(    EnumContains( flags,  StateInfo::Basic )
        || EnumContains( flags,  StateInfo::Version )  )
        buf.NewLine();

    //  source path trim info
    if( EnumContains( flags,  StateInfo::SPTR ) )
    {
        buf._NC( "Source Path Trimming Rules: " ).NewLine();

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
                buf._NC( trimInfoNo == 0 ? "  Global: "
                                         : "  Local:  " );
                buf._NC( ti.IsPrefix ?  "\"" : "\"*");
                buf._NC( ti.Path )._NC( "\", " );
                buf._NC( ti.IncludeString );
                if ( ti.TrimOffset != 0 )
                    buf._NC( ti.Path )._NC( "\", Offset: " )._NC( ti.TrimOffset );
                buf._NC( ", Priority: " )._( ti.Priority );
                buf.NewLine();
            }
        }


        if ( cnt == 0 )
            buf._NC("  <no rules set>" ).NewLine();
        buf.NewLine();
    }

    //  domain substitutions
    if( EnumContains( flags,  StateInfo::DSR ) )
    {
        buf._NC( "Domain Substitution Rules: " ).NewLine();
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
                buf._NC( "  " );
                if (    it.type == DomainSubstitutionRule::Type::EndsWith
                     || it.type == DomainSubstitutionRule::Type::Substring )
                    buf._NC( '*' );

                buf._NC( it.Search );
                if (    it.type == DomainSubstitutionRule::Type::StartsWith
                     || it.type == DomainSubstitutionRule::Type::Substring )
                    buf._NC( '*' );

                buf._NC( NFormat::Tab( maxWidth, -1, 0 ) )
                   ._NC( " -> " )
                   ._NC( it.Replacement );
                buf.NewLine();
            }
        }
        else
            buf._NC("  <no rules set>" ).NewLine();
        buf.NewLine();
    }

    // Log Once Counters
    if( EnumContains( flags,  StateInfo::Once ) )
    {
        buf._NC( "Once() Counters: " ).NewLine();
        if ( scopeDump.writeStoreMap( &scopeLogOnce ) == 0 )
            buf._NC("  <no Once() counters set>" ).NewLine();
        buf.NewLine();
    }

    // Log Data
    if( EnumContains( flags,  StateInfo::LogData ) )
    {
        buf._NC( "Log Data: " ).NewLine();
        if ( scopeDump.writeStoreMap( &scopeLogData ) == 0 )
            buf._NC("  <no data objects stored>" ).NewLine();
        buf.NewLine();
    }

    // Prefix Logables
    if( EnumContains( flags,  StateInfo::PrefixLogables ) )
    {
        buf._NC( "Prefix Logables: " ).NewLine();
        integer oldLength= buf.Length();
        scopeDump.writeStore( &scopePrefixes, 2 );
        getStateCollectPrefixes( &domains, 2, buf );
        if ( oldLength == buf.Length() )
            buf._NC("  <no prefix logables set>" ).NewLine();
        buf.NewLine();
    }

    // thread mappings
    if( EnumContains( flags,  StateInfo::ThreadMappings ) )
    {
        buf._NC( "Named Threads:   " ).NewLine();
        if ( scopeInfo.threadDictionary.size() == 0 )
            buf._NC("  <no thread name mappings set>" ).NewLine();
        else
            for ( auto& pair : scopeInfo.threadDictionary )
            {
                buf._NC( "  " ) << NFormat::Field( String32() << '(' << pair.first << "):", 7, Alignment::Left )
                                << '\"' << pair.second << '\"';
                buf.NewLine();
            }
        buf.NewLine();
    }

    // Scope Domains
    if( EnumContains( flags,  StateInfo::ScopeDomains ) )
    {
        buf._NC( "Scope Domains: " ).NewLine();
        if ( scopeDump.writeStore( &scopeDomains, 2 ) == 0 )
            buf._NC("  <no scope domains set>" ).NewLine();
        buf.NewLine();
    }

    // Loggers
    if( EnumContains( flags,  StateInfo::Loggers ) )
    {
        TickConverter dateTimeConverter;
        std::vector<Domain*> domainsWithDiffVerb;
        for (int treeNo= 0; treeNo < 2; ++treeNo )
        {
            int cnt= 0;
            Domain* domTree;
            if( treeNo==0 )
            {
                 domTree= &domains;
                 buf._NC( "Loggers:" ).NewLine();
            }
            else
            {
                 domTree= &internalDomains;
                 buf._NC( "Loggers on Internal Domains:" ).NewLine();
            }

            for ( int loggerNo= 0; loggerNo< domTree->CountLoggers(); loggerNo++ )
            {
                cnt++;
                String64 as64;
                CalendarDateTime ct(Initialization::Suppress);

                Logger* logger= domTree->GetLogger(loggerNo);
                buf._NC( "  "  )._NC( *logger  ).NewLine();
                buf._NC( "    Lines logged:  "     )._NC( logger->CntLogs                                   ).NewLine();

                ct.Set( dateTimeConverter.ToDateTime(logger->TimeOfCreation) );
                buf._NC( "    Creation time: "     )._NC( ct.Format( A_CHAR("yyyy-MM-dd HH:mm:ss"), as64.Reset()) ).NewLine();

                ct.Set( dateTimeConverter.ToDateTime(logger->TimeOfLastLog) );
                buf._NC( "    Last log time: "     )._NC( ct.Format( A_CHAR("yyyy-MM-dd HH:mm:ss"), as64.Reset()) ).NewLine();

                domainsWithDiffVerb.clear();
                getStateDomainsWithDiffVerb( domTree, loggerNo, domainsWithDiffVerb);
                for ( Domain* dom : domainsWithDiffVerb )
                {
                    buf._NC("    ")
                       ._(  dom == *domainsWithDiffVerb.begin() ? "Verbosities:   "
                                                                : "               " );

                    integer tabRef= buf.Length();
                    buf << dom->FullPath << NFormat::Tab( maxDomainPathLength +1, tabRef);

                    buf << "= " << std::make_pair(dom->GetVerbosity( loggerNo ), dom->GetPriority(loggerNo) )
                        << NewLine();
                }
            }
            if ( cnt == 0 )
                buf._NC("  <no loggers attached>" ).NewLine();
            buf.NewLine();
        }
    }

    // Internal Domains
    if( EnumContains( flags,  StateInfo::InternalDomains ) )
    {
        buf._NC( "Internal Domains:" ).NewLine();
        getStateDomainRecursive( internalDomains, maxDomainPathLength, buf );
        buf.NewLine();
    }

    // Domains
    if( EnumContains( flags,  StateInfo::Domains ) )
    {
        buf._NC( "Domains:" ).NewLine();
        getStateDomainRecursive( domains,  maxDomainPathLength,  buf );
        buf.NewLine();
    }
}


}}}// namespace [aworx::lib::lox]
