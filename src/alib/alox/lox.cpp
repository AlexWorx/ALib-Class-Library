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

#if !defined (HPP_ALOX_CONSOLE_LOGGER)
    #include "alib/alox/loggers/consolelogger.hpp"
#endif
#if !defined (HPP_ALOX_ANSI_LOGGER)
    #include "alib/alox/loggers/ansilogger.hpp"
#endif
#if !defined (HPP_ALOX_WINDOWS_CONSOLE_LOGGER)
    #include "alib/alox/loggers/windowsconsolelogger.hpp"
#endif

#if !defined (HPP_ALIB_LIB_COMMONENUMS_RESOURCED)
#   include "alib/lib/commonenumsresourced.hpp"
#endif

#if !defined (HPP_ALIB_ALOX_VARIABLES)
    #include "alib/alox/variables.hpp"
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

// #################################################################################################
// Constructors/destructor
// #################################################################################################

Lox::Lox(const NString& name, bool doRegister )
: Lock( Safeness::Safe)
, scopeInfo( name )
, domains        ( nullptr, ""    )
, internalDomains( nullptr, "$"   )
, scopeDomains ( scopeInfo, false )
, scopeLogOnce ( scopeInfo, true  )
, scopePrefixes( scopeInfo, false )
, scopeLogData ( scopeInfo, true  )
{
    // create internal sub-domains
    const NString internalDomainList[]= {"LGR","DMN", "PFX", "THR", "LGD", "VAR" };
    for ( auto& it : internalDomainList )
        internalDomains.Find( it, 1, nullptr );
    maxDomainPathLength=  ALox::InternalDomains.Length() + 3;

    // register with ALox
    if( doRegister )
        ALOX.Register( this, ContainerOp::Insert );

    // read domain substitution rules from configuration
    Variable variable( Variables::DOMAIN_SUBSTITUTION,
                       #if ALIB_CHARACTERS_ARE_NARROW
                            GetName()
                       #else
                            String128( GetName() )
                       #endif
                     );
    if ( ALOX.Config->Load( variable ) != Priorities::NONE )
    {
        for( int ruleNo= 0; ruleNo< variable.Size(); ruleNo++ )
        {
            AString& rule= variable.GetString( ruleNo );
            if( rule.IsEmpty() )
                continue;

            integer idx= rule.IndexOf( A_CHAR("->") );
            if ( idx > 0 )
            {
                NString64 domainPath ( rule.Substring<false>( 0, idx  )                          ); domainPath .Trim();
                NString64 replacement( rule.Substring<false>( idx + 2, rule.Length() - idx - 2 ) ); replacement.Trim();
                SetDomainSubstitutionRule( domainPath, replacement );
            }
            else
            {
                // using alib warning here as we can't do internal logging in the constructor
                ALIB_WARNING( "Syntax error in variable {!Q}.", variable.Fullname );
            }
        }
    }
}

Lox::~Lox()
{
    if( IsRegistered() )
        ALOX.Register( this, ContainerOp::Remove );
    clear();
}

void  Lox::clear()
{
    // unregister each logger in std domains and remove it in internals
    for ( int i= static_cast<int>(domains.CountLoggers()) - 1  ; i >= 0  ; i-- )
    {
        Logger* logger= domains.GetLogger( i );
        int ii= internalDomains.GetLoggerNo( logger );
        if ( ii >= 0 )
            internalDomains.RemoveLogger( ii );
        logger->AcknowledgeLox( *this, ContainerOp::Remove );
    }

    // unregister remaining loggers in internal domains
    for ( int i= static_cast<int>(internalDomains.CountLoggers()) - 1  ; i >= 0  ; i-- )
    {
        Logger* logger= internalDomains.GetLogger( i );
        logger->AcknowledgeLox( *this, ContainerOp::Remove );
    }

    // clear domain trees
    domains.Data.clear();
    while( domains.SubDomains.size() > 0 )
    {
        delete domains.SubDomains.back();
        domains.SubDomains.pop_back();
    }
    domains.CntLogCalls= 0;

    internalDomains.Data.clear();
    while( internalDomains.SubDomains.size() > 0 )
    {
        delete internalDomains.SubDomains.back();
        internalDomains.SubDomains.pop_back();
    }
    internalDomains.CntLogCalls= 0;


    // the following would be a good exercise for TMP (to move this into ~ScopeStore)
    // The problem is, we are storing
    // a) AString*
    // b) std::map<AString, int>*
    // c) std::map<AString, Box>
    // This means, a) and b) are 'the same', c) needs an inner iteration everywhere.
    // If we had a map<int, XYZ*>, then a next, separated loop needed to be added.

    // clear scope domains
    if ( scopeDomains.globalStore )
        delete scopeDomains.globalStore;

    for ( auto* it : *scopeDomains.languageStore )
        if( it )
            delete it;

    scopeDomains.languageStore->Clear();

    for ( auto& thread : scopeDomains.threadOuterStore )
        for ( auto& it : thread.second )
            delete it;

    for ( auto& thread : scopeDomains.threadInnerStore )
        for ( auto& it : thread.second )
            delete it;
    scopeDomains.Clear();

    // clear scopePrefixes
    if ( scopePrefixes.globalStore )
        delete static_cast<PrefixLogable*>( scopePrefixes.globalStore );

    for ( auto* it : *scopePrefixes.languageStore )
        if( it )
            delete static_cast<PrefixLogable*>( it );

    scopePrefixes.languageStore->Clear();


    for ( auto& thread : scopePrefixes.threadOuterStore )
        for ( auto& it : thread.second )
            delete static_cast<PrefixLogable*>(it);

    for ( auto& thread : scopePrefixes.threadInnerStore )
        for ( auto& it : thread.second )
            delete static_cast<PrefixLogable*>(it);

    scopePrefixes.Clear();

    // clear log once information
    if ( scopeLogOnce.globalStore )
        delete scopeLogOnce.globalStore;

    for ( auto* it : *scopeLogOnce.languageStore )
        if( it )
            delete it;
    scopeLogOnce.languageStore->Clear();


    for ( auto& thread : scopeLogOnce.threadOuterStore )
        for ( auto& it : thread.second )
            delete it;

    for ( auto& thread : scopeLogOnce.threadInnerStore )
        for ( auto& it : thread.second )
            delete it;
    scopeLogOnce.Clear();

    // delete LogData objects
    if ( scopeLogData.globalStore )
        delete scopeLogData.globalStore;

    for ( auto* map : *scopeLogData.languageStore )
        if( map )
            delete map;
    scopeLogData.languageStore->Clear();


    for ( auto& thread : scopeLogData.threadOuterStore )
        for ( auto& vec : (thread.second) )
            delete vec;

    for ( auto& thread : scopeLogData.threadInnerStore )
        for ( auto& vec : (thread.second) )
            delete vec;

    scopeLogData.Clear();

    // other things
    domainSubstitutions.clear();
    scopeInfo.threadDictionary.clear();
    for( Boxes* boxes: logableContainers )
        delete boxes;
    logableContainers.clear();
    for( Boxes* boxes: internalLogables )
        delete boxes;
    internalLogables.clear();
    CntLogCalls=            0;

}

void  Lox::Reset()
{
    clear();

    ClearSourcePathTrimRules( Reach::Global, true );
}

// #################################################################################################
// Methods
// #################################################################################################

TextLogger* Lox::CreateConsoleLogger(const NString& name)
{
    //--- check configuration setting "CONSOLE_TYPE"  ---

    Variable variable( Variables::CONSOLE_TYPE );
    ALOX.Config->Load( variable );
    Substring val= variable.GetString();
    val.Trim();
    if( val.IsEmpty() ||
        val.Equals<Case::Ignore>( A_CHAR("default") ) ) goto DEFAULT;

    if( val.Equals<Case::Ignore>( A_CHAR("plain")   ) ) return new ConsoleLogger    ( name );
    if( val.Equals<Case::Ignore>( A_CHAR("Ansi")    ) ) return new AnsiConsoleLogger( name );

    if( val.Equals<Case::Ignore>( A_CHAR("WINDOWS") ) )
                                                    #if defined( _WIN32 )
                                                        return new WindowsConsoleLogger( name );
                                                    #else
                                                        goto DEFAULT;
                                                    #endif


    ALIB_WARNING( "Unrecognized value in config variable {!Q} = {!Q}.",
                  variable.Fullname, variable.GetString() );

    DEFAULT:

    #if defined( _WIN32 )
        // if there is no console window we do not do colors
        if ( !ALIB.HasConsoleWindow )
            return new ConsoleLogger( name );
        else
            return new WindowsConsoleLogger( name );
    #else
        return new AnsiConsoleLogger( name );
    #endif

}

Logger* Lox::GetLogger( const NString& loggerName )
{
    ALIB_ASSERT_ERROR ( Lock.GetSafeness() == Safeness::Unsafe || Lock.CountAcquirements() > 0,
                        "Lox not acquired" )

    // search logger
    Logger* logger;
    if ( (logger= domains        .GetLogger( loggerName ) ) != nullptr )    return logger;
    if ( (logger= internalDomains.GetLogger( loggerName ) ) != nullptr )    return logger;

    // not found
    Boxes& logables= acquireInternalLogables();
    logables.Add( "No logger named {!Q} found.", loggerName );
    logInternal( Verbosity::Warning, "LGR", logables );
    return nullptr;
}

//! @cond NO_DOX
void verbositySettingToVariable( Domain& domain, int loggerNo, Variable& var );
void verbositySettingToVariable( Domain& domain, int loggerNo, Variable& var )
{
    var.Add()._( domain.FullPath )
             ._('=')
             ._( domain.GetVerbosity( loggerNo ) );

    // loop over all sub domains (recursion)
    for ( Domain* subDomain : domain.SubDomains )
        verbositySettingToVariable( *subDomain, loggerNo, var );
}
//! @endcond

void Lox::writeVerbositiesOnLoggerRemoval( Logger* logger )
{
    // When writing back we will use this priority as the maximum to write. This way, if this was
    // an automatic default value, we will not write back into the user's variable store.
    // As always, only if the app fetches new variables on termination, this is entry is copied.
    Variable variable( Variables::VERBOSITY, GetName(), logger->GetName() );

    // first token is "writeback" ?
    ALOX.Config->Load( variable );
    if ( variable.Size() == 0 )
        return;
    Substring firstArg( variable.GetString() );
    if ( !firstArg.ConsumeString<Case::Ignore, Whitespaces::Trim>( A_CHAR("writeback") ) )
        return;

    // optionally read a destination variable name
    Substring destVarCategory;
    Substring destVarName;

    if( firstArg.Trim().IsNotEmpty() )
    {
        // separate category from variable name
        integer catSeparatorIdx= firstArg.IndexOf( '_' );
        if (catSeparatorIdx >= 0 )
        {
            destVarCategory= firstArg.Substring<false>( 0                   , catSeparatorIdx );
            destVarName    = firstArg.Substring       ( catSeparatorIdx + 1);
        }
        else
            destVarName= firstArg;

        if ( destVarName.IsEmpty() )
        {
            Boxes& logables= acquireInternalLogables();
            logables.Add( "Argument 'writeback' in variable {!Q}.\\n"
                          "Error: Wrong destination variable name format: {!Q}",
                          variable.Fullname, firstArg );
            logInternal( Verbosity::Error, "VAR", logables );
            return;
        }
    }

    // either write directly into LOX_LOGGER_VERBOSITY variable...
    Variable  destVarLocalObject;
    Variable* destVar;
    if( destVarName.IsEmpty() )
    {
        variable.ClearValues( 1 );
        destVar= &variable;
    }
    // ...or into a new given variable
    else
    {
        destVar= &destVarLocalObject;
        destVar->Declare( destVarCategory, destVarName, VariableDecl(Variables::VERBOSITY).Delim() );
        destVar->FmtHints=            variable.FmtHints;
        destVar->FormatAttrAlignment= variable.FormatAttrAlignment;
        destVar->Comments._("Created at runtime through config option 'writeback' in variable \")")
                         ._(variable.Fullname)._("\".");
    }

    // collect verbosities
    {
        int loggerNoMainDom= domains        .GetLoggerNo( logger );
        int loggerNoIntDom=  internalDomains.GetLoggerNo( logger );

        if ( loggerNoMainDom >= 0 ) verbositySettingToVariable( domains        , loggerNoMainDom, *destVar );
        if ( loggerNoIntDom  >= 0 ) verbositySettingToVariable( internalDomains, loggerNoIntDom , *destVar );
    }

    // now store using the same plug-in as original variable has
    destVar->Priority= variable.Priority;
    ALOX.Config->Store( *destVar );

    // internal logging
    Boxes& logables= acquireInternalLogables();
    logables.Add( "Argument 'writeback' in variable {!Q}:\\n  Verbosities for logger {!Q} written ",
                  variable.Fullname, logger->GetName() );

    if( destVarName.IsEmpty() )
        logables.Add( "(to source variable)." );
    else
        logables.Add( "to variable {!Q}.", destVar->Fullname );
    logInternal( Verbosity::Info, "VAR", logables );

    // verbose logging of the value written
    NString512 intMsg;
    intMsg.DbgDisableBufferReplacementWarning();
    intMsg._("  Value:");
    for( int i= 0; i< destVar->Size() ; i++ )
        intMsg._( "\n    " )._( destVar->GetString(i) );
    logables= acquireInternalLogables();
    logables.Add( intMsg );
    logInternal( Verbosity::Verbose, "VAR", logables );
}

void Lox::dumpStateOnLoggerRemoval()
{
    if( !loggerAddedSinceLastDebugState )
        return;
    loggerAddedSinceLastDebugState= false;

    Variable variable( Variables::DUMP_STATE_ON_EXIT,
                       #if ALIB_CHARACTERS_ARE_NARROW
                            GetName()
                       #else
                            String128( GetName() )
                       #endif
                     );
    ALOX.Config->Load( variable );

    NString64  domain;
    Verbosity  verbosity= Verbosity::Info;
    Substring  tok;
    bool error= false;
    StateInfo flags= StateInfo::NONE;
    for( int tokNo= 0; tokNo< variable.Size(); tokNo++ )
    {
        tok=  variable.GetString( tokNo );
        if( tok.IsEmpty() )
            continue;


        // read log domain and verbosity
        if( tok.IndexOf( '=' ) > 0 )
        {
            if( tok.ConsumePartOf<Case::Ignore, Whitespaces::Trim>( A_CHAR("verbosity"), 1) )
            {
                if( tok.ConsumeChar<Case::Sensitive, Whitespaces::Trim>( '=' ) )
                    tok.ConsumeEnum<Verbosity>( verbosity );
                continue;
            }
            if( tok.ConsumePartOf<Case::Ignore, Whitespaces::Trim>( A_CHAR("domain"), 1) )
            {
                if( tok.ConsumeChar<Case::Sensitive, Whitespaces::Trim>( '=' ) )
                    domain= tok.Trim();
                continue;
            }
            error= true;
            break;
        }

        // read and add state
        StateInfo stateInfo;
        if( !tok.ConsumeEnum<StateInfo>( stateInfo ) )
        {
            error= true;
            break;
        }

        // as soon as this flag is found, we quit
        if( stateInfo == StateInfo::NONE )
            return;

        flags|= stateInfo;
    }
    if( error )
    {
        Boxes& logables= acquireInternalLogables();
        logables.Add( "Unknown argument {!Q} in variable {} = {!Q}.",
                      tok, variable.Fullname, variable.GetString() );
        logInternal( Verbosity::Error, "VAR", logables);
    }

    if ( flags != StateInfo::NONE )
    {
        State( domain, verbosity, A_CHAR("Auto dump state on exit requested: "), flags );
    }
}


bool Lox::RemoveLogger( Logger* logger )
{
    ALIB_ASSERT_ERROR ( Lock.GetSafeness() == Safeness::Unsafe || Lock.CountAcquirements() > 0,
                        "Lox not acquired" )

    int noMainDom=  domains        .GetLoggerNo( logger );
    int noIntDom=   internalDomains.GetLoggerNo( logger );

    if( noMainDom >= 0 || noIntDom >= 0 )
    {
        dumpStateOnLoggerRemoval();
        writeVerbositiesOnLoggerRemoval( logger );

        if( noMainDom >= 0 )
            domains.RemoveLogger( noMainDom );

        if( noIntDom >= 0 )
            internalDomains.RemoveLogger( noIntDom );

        logger->AcknowledgeLox( *this, ContainerOp::Remove );

        return true;
    }

    // not found
    Boxes& logables= acquireInternalLogables();
    logables.Add( "Logger {!Q} not found. Nothing removed.", logger );
    logInternal( Verbosity::Warning, "LGR", logables );
    return false;
}

Logger* Lox::RemoveLogger( const NString& loggerName )
{
    ALIB_ASSERT_ERROR ( Lock.GetSafeness() == Safeness::Unsafe || Lock.CountAcquirements() > 0,
                        "Lox not acquired" )

    int noMainDom=  domains        .GetLoggerNo( loggerName );
    int noIntDom=   internalDomains.GetLoggerNo( loggerName );

    if( noMainDom >= 0 || noIntDom >= 0 )
    {
        Logger*                 logger=         domains.GetLogger( noMainDom );
        if( logger == nullptr ) logger= internalDomains.GetLogger( noIntDom );

        dumpStateOnLoggerRemoval();
        writeVerbositiesOnLoggerRemoval( logger );

        if( noMainDom >= 0 )
            domains.RemoveLogger( noMainDom );

        if( noIntDom >= 0 )
            internalDomains.RemoveLogger( noIntDom );

        logger->AcknowledgeLox( *this, ContainerOp::Remove );

        Boxes& logables= acquireInternalLogables();
        logables.Add( "Logger {!Q} removed.", logger );
        logInternal( Verbosity::Info, "LGR", logables );
        return logger;
    }

    // not found
    Boxes& logables= acquireInternalLogables();
    logables.Add( "Logger {!Q} not found. Nothing removed.", loggerName );
    logInternal( Verbosity::Warning, "LGR", logables );

    return nullptr;
}

void Lox::SetVerbosity( Logger* logger, Verbosity verbosity, const NString& domain, Priorities priority )
{
    ALIB_ASSERT_ERROR ( Lock.GetSafeness() == Safeness::Unsafe || Lock.CountAcquirements() > 0,
                        "Lox not acquired" )

    // check
    if ( logger == nullptr )
    {
        Boxes& logables= acquireInternalLogables();
        logables.Add( "Given Logger is \"null\". Verbosity not set." );
        logInternal( Verbosity::Error, "LGR", logables );
        return;
    }

    // this might create the (path of) domain(s) and set the \e Logger's verbosities like their
    // first parent's or as given in configuration
    Domain* dom= evaluateResultDomain( domain );

    // search logger, insert if not found
    bool isNewLogger= false;
    int no= dom->GetLoggerNo( logger );
    if( no < 0 )
    {
        no= dom->AddLogger( logger );

        // error, logger with same name already exists
        if( no < 0 )
        {
            Boxes& logables= acquireInternalLogables();
            logables.Add( "Unable to add logger {!Q}. Logger with same name exists.", logger );
            logInternal( Verbosity::Error, "LGR", logables );


            logables= acquireInternalLogables();
            logables.Add( "  Request was: SetVerbosity({!Q}, {!Q}, Verbosity::{}, {}). ",
                          logger, dom->FullPath, verbosity, priority   );
            logInternal( Verbosity::Verbose, "LGR", logables );

            Logger* existingLogger= dom->GetLogger( logger->GetName() );
            logables= acquireInternalLogables();
            logables.Add( "  Existing Logger: {!Q}.", existingLogger );
            logInternal( Verbosity::Verbose, "LGR",  logables );

            return;
        }

        // We have to register with the SmartLock facility of the \e Logger.
        // But only if we have not done this yet, via the 'other' root domain tree
        if ( ( dom->GetRoot() == &domains ? internalDomains.GetLoggerNo( logger )
                                          :         domains.GetLoggerNo( logger ) ) < 0 )
        {
            logger->AcknowledgeLox( *this, ContainerOp::Insert );
        }

        // store size of name to support tabular internal log output
        if ( maxLoggerNameLength < logger->GetName().Length() )
             maxLoggerNameLength=  logger->GetName().Length();

        // for internal log
        isNewLogger= true;

        // remember that a logger was set after the last removal
        // (for variable LOXNAME_DUMP_STATE_ON_EXIT)
        loggerAddedSinceLastDebugState= true;
    }

    // get verbosities from configuration
    if( isNewLogger )
    {
        Boxes& logables= acquireInternalLogables();
        logables.Add( "Logger {!Q}.", logger );
        if( domain.StartsWith(ALox::InternalDomains) )
            logables.Add(" added for internal log messages.");
        else
            logables.Add(" added.");
        logInternal( Verbosity::Info, "LGR",  logables );

        // we have to get all verbosities of already existing domains
        Variable variable( Variables::VERBOSITY, GetName(), logger->GetName() );
        if( ALOX.Config->Load( variable ) != Priorities::NONE )
        {
            getAllVerbosities( logger, &domains         , variable );
            getAllVerbosities( logger, &internalDomains , variable );
        }
    }

    // do
    dom->SetVerbosity( no, verbosity, priority );

    Boxes& logables= acquireInternalLogables();

    logables.Add( "Logger {!Q}: {!Fill}'{}'{!Fill}= Verbosity::{}.",
                  logger->GetName(),
                  maxLoggerNameLength - logger->GetName().Length(),
                  dom->FullPath,
                  maxDomainPathLength - dom->FullPath.Length() + 1,
                  std::make_pair(verbosity, priority)                           );

    Verbosity actVerbosity= dom->GetVerbosity( no );
    if( actVerbosity != verbosity )
        logables.Add( " Lower priority ({} < {}). Remains {}.",
                      priority, dom->GetPriority(no), actVerbosity );

    logInternal( Verbosity::Info, "LGR", logables );
}

void Lox::SetVerbosity( const NString& loggerName, Verbosity verbosity, const NString& domain, Priorities priority )
{
    ALIB_ASSERT_ERROR ( Lock.GetSafeness() == Safeness::Unsafe || Lock.CountAcquirements() > 0,
                        "Lox not acquired" )

    // get domain
    Domain* dom= evaluateResultDomain( domain );

    // get logger
    Logger* logger;
    int no= dom->GetLoggerNo( loggerName );
    if( no >= 0 )
        logger= dom->GetLogger( no );
    else
    {
        // we have to check if the logger was added in the 'other' tree
        Domain* otherTree=  dom->GetRoot() == &domains ? &internalDomains
                                                       : &domains;
        no= otherTree->GetLoggerNo( loggerName );
        if ( no < 0 )
        {
            // error
            Boxes& logables= acquireInternalLogables();
            logables.Add( "Logger not found. Request was: SetVerbosity({!Q}, {!Q}, Verbosity::{}, {}).",
                          loggerName, dom->FullPath, verbosity, priority );
            logInternal( Verbosity::Warning, "LGR",  logables );
            return;
        }

        logger= otherTree->GetLogger( no );
    }

    // use the overloaded method
    SetVerbosity( logger, verbosity, domain, priority );
}

void Lox::setDomainImpl( const NString& scopeDomain, Scope   scope,
                         bool           removeNTRSD, Thread* thread )
{
    //note: the public class interface assures that \p{removeNTRSD} (named thread related scope domain)
    // only evaluates true for thread related scopes

    ALIB_ASSERT_ERROR ( Lock.GetSafeness() == Safeness::Unsafe || Lock.CountAcquirements() > 0,
                        "Lox not acquired" )

    // check
    int pathLevel= checkScopeInformation( scope, "DMN" );
    if( pathLevel < 0 )
        return;

    ThreadID threadID= thread != nullptr ? thread->GetId() : threads::UNDEFINED;

    NAString* previousScopeDomain;

    scopeDomains.InitAccess( scope, pathLevel, threadID );
    if ( removeNTRSD )
    {
        NString64 searchedValue( scopeDomain );
        previousScopeDomain= scopeDomains.Remove( &searchedValue );
    }
    else
    {
        if ( scopeDomain.IsNotEmpty() )
        {
            NAString* newValue= new NAString( scopeDomain );
            newValue->Trim();
            previousScopeDomain= scopeDomains.Store ( newValue );
        }
        else
            previousScopeDomain= scopeDomains.Remove( nullptr );
    }

    // log info on this
    NString256 msg;
    if ( !removeNTRSD && scopeDomain.IsNotEmpty() )
    {
        msg << '\'' << scopeDomain
            << "\' set as default for " << (scope + pathLevel)  << '.' ;

        if ( previousScopeDomain  == nullptr )
            logInternal( Verbosity::Info,    "DMN", msg );
        else
        {
            if ( previousScopeDomain->Equals( scopeDomain ) )
            {
                msg << " (Was already set.)";
                logInternal( Verbosity::Verbose, "DMN", msg );
            }
            else
            {
                msg << " Replacing previous default \'" << *previousScopeDomain << "\'.";
                logInternal( Verbosity::Warning, "DMN", msg );
            }
        }

    }
    else
    {
        if ( previousScopeDomain  != nullptr )
        {
            msg << '\'' << *previousScopeDomain
                << "\' removed from " << (scope + pathLevel)  << '.';
            logInternal( Verbosity::Info, "DMN", msg );
        }
        else
        {
            if ( removeNTRSD )
                msg << '\'' << scopeDomain << "\' not found. Nothing removed for ";
            else
                msg  << "Empty Scope Domain given, nothing registered for ";
            msg << (scope + pathLevel) << '.';

            logInternal( Verbosity::Warning, "DMN", msg );
        }
    }

    // it is on us to delete the previous one
    if ( previousScopeDomain != nullptr )
        delete previousScopeDomain;
}

void Lox::RemoveThreadDomain( const NString& scopeDomain, Scope scope, Thread* thread )
{
    if ( !isThreadRelatedScope( scope ) )
        return;

    // check
    if (  scopeDomain.IsEmpty() )
    {
        NString256 msg;
        msg  << "Illegal parameter. No scope domain path given. Nothing removed for "
             << scope <<  '.';
        logInternal( Verbosity::Warning, "DMN", msg );

        // do nothing
        return;
    }

    // invoke internal master
    setDomainImpl( scopeDomain, scope, true, thread);
}

void Lox::SetDomainSubstitutionRule( const NString& domainPath, const NString& replacement )
{
    // check null param: clears all rules
    if ( domainPath.IsEmpty() )
    {
        oneTimeWarningCircularDS= false;
        domainSubstitutions.clear();
        logInternal( Verbosity::Info, "DMN", "Domain substitution rules removed.");
        return;
    }


    // create rule
    DomainSubstitutionRule newRule( domainPath, replacement );
    if ( newRule.Search.IsEmpty() )
    {
        logInternal( Verbosity::Warning, "DMN", "Illegal domain substitution rule. Nothing stored." );
        return;
    }

    // search existing rule
    std::vector<DomainSubstitutionRule>::iterator  it;
    for( it= domainSubstitutions.begin(); it != domainSubstitutions.end() ; ++it )
    {
        if (     (*it).type == newRule.type
              && (*it).Search.Equals( newRule.Search ) )
            break;
    }

    // no replacement given?
    if ( replacement.IsEmpty() )
    {
        Boxes& logables= acquireInternalLogables();
        if ( it == domainSubstitutions.end() )
        {
            logables.Add("Domain substitution rule {!Q} not found. Nothing to remove.",  domainPath );
            logInternal( Verbosity::Warning, "DMN", logables );
            return;
        }

        logables.Add("Domain substitution rule {!Q} -> {!Q} removed.", domainPath, (*it).Replacement );
        logInternal( Verbosity::Info, "DMN", logables );
        domainSubstitutions.erase( it );
        return;
    }

    Boxes& logables= acquireInternalLogables();
    logables.Add("Domain substitution rule {!Q} -> {!Q} set.", domainPath, newRule.Replacement );

    // change of rule
    NString256 msg;
    if ( it != domainSubstitutions.end() )
    {
        msg << " Replacing previous -> \"" << (*it).Replacement  << "\".";
        logables.Add( msg );
        (*it).Replacement.Reset( newRule.Replacement );
    }
    else
        domainSubstitutions.emplace_back( newRule );

    logInternal( Verbosity::Info, "DMN", logables );
}

void Lox::setPrefixImpl( const Box& prefix, Scope scope, Thread* thread  )
{
    ALIB_ASSERT_ERROR ( Lock.GetSafeness() == Safeness::Unsafe || Lock.CountAcquirements() > 0,
                        "Lox not acquired" )

    // check
    int pathLevel= checkScopeInformation( scope, "PFX" );
    if( pathLevel < 0 )
        return;

    ThreadID threadID= thread != nullptr ? thread->GetId() : threads::UNDEFINED;

    scopePrefixes.InitAccess( scope, pathLevel, threadID );
    bool isVoidOrEmpty=    prefix.IsType<void>()
                        || prefix.IsNull()
                        || ( prefix.IsArray() && !prefix.UnboxLength() );

    Box* previousLogable=  !isVoidOrEmpty ? scopePrefixes.Store( new PrefixLogable( prefix ) )
                                          : scopePrefixes.Remove( nullptr );


    NString256 intMsg( "Object ");
    Verbosity intMsgVerbosity= Verbosity::Info;
    if ( !isVoidOrEmpty )
    {
        intMsg << prefix
               << " added as prefix logable for " << (scope + pathLevel)  << '.' ;

        if ( previousLogable  != nullptr )
        {
            if ( previousLogable->Call<FEquals>( prefix )  )
            {
                intMsg << " (Same as before.)";
                intMsgVerbosity= Verbosity::Verbose;
            }
            else
            {
                intMsg << " Replacing previous "
                       << *previousLogable
                       << '.';
            }
        }
    }
    else
    {
        if ( previousLogable  != nullptr )
        {
            intMsg << *previousLogable
                   << " removed from list of prefix logables for ";
        }
        else
        {
            intMsg  << "<nullptr> given but no prefix logable to remove for ";

            intMsgVerbosity= Verbosity::Warning;
        }
        intMsg  << (scope + pathLevel) << '.';
    }
    logInternal( intMsgVerbosity, "PFX", intMsg );

    // it is on us to delete the previous one
    if ( previousLogable != nullptr )
    {
        delete static_cast<PrefixLogable*>( previousLogable );
    }
}


void Lox::SetPrefix( const Box& prefix, const NString& domain, Inclusion otherPLs )
{
    ALIB_ASSERT_ERROR ( Lock.GetSafeness() == Safeness::Unsafe || Lock.CountAcquirements() > 0,
                        "Lox not acquired" )

    Domain* dom= evaluateResultDomain( domain );

    bool isVoidOrEmpty=    prefix.IsType<void>()
                        || prefix.IsNull()
                        || ( prefix.IsArray() && !prefix.UnboxLength() );

    NString256 msg;
    Verbosity intLogVerbosity= Verbosity::Info;

    if ( !isVoidOrEmpty )
    {
        // create logable: if String* type, then copy the string. We are responsible, then.
        msg._("Object ");

        //dom->PrefixLogables.emplace_back( PrefixLogable( prefix ), otherPLs );
        dom->PrefixLogables.emplace_back( new PrefixLogable( prefix ), otherPLs );

        msg << prefix  << " added as prefix logable for";
    }
    else
    {
        size_t qtyPLs=  dom->PrefixLogables.size();
        if ( qtyPLs > 0 )
        {
            PrefixLogable* removedLogable= dom->PrefixLogables[ qtyPLs - 1 ].first;
            dom->PrefixLogables.pop_back();
            msg << "Object " <<  *static_cast<Box*>(removedLogable) << " removed from list of prefix logables for";
            delete removedLogable;
        }
        else
        {
            msg << "No prefix logables to remove for";
            intLogVerbosity= Verbosity::Warning;
        }
    }

    msg << " domain \'" << dom->FullPath << "\'.";
    logInternal( intLogVerbosity, "PFX", msg );

}


#if defined (__GLIBCXX__) || defined(__APPLE__)
    void Lox::SetStartTime( time_t startTime, const NString& loggerName )
    {
        TickConverter converter;
        SetStartTime( converter.ToTicks( DateTime::FromEpochSeconds( startTime ) ), loggerName );
    }

#elif defined( _WIN32 )
    void Lox::SetStartTime( const FILETIME& startTime, const NString& loggerName )
    {
        TickConverter converter;
        SetStartTime( converter.ToTicks( DateTime::FromFileTime( startTime ) ), loggerName );
    }
#else
    #pragma message "Unknown Platform in file: " __FILE__ )
#endif

void Lox::SetStartTime( Ticks startTime, const NString& loggerName )
{
    ALIB_ASSERT_ERROR ( Lock.GetSafeness() == Safeness::Unsafe || Lock.CountAcquirements() > 0,
                        "Lox not acquired" )

    bool foundOne= false;
    for( int loggerNo= 0; loggerNo < domains.CountLoggers(); ++loggerNo )
    {
        // request logger only from main domain tree
        Logger* logger= domains.GetLogger( loggerNo );
        if( loggerName.IsNotEmpty() && !loggerName.Equals<Case::Ignore>( logger->GetName()) )
            continue;
        foundOne= true;

        // log info on this
        Boxes& logables= acquireInternalLogables();
        logables.Add( "Logger {!Q}: Start time set to ", logger->GetName() );
        if ( !startTime.IsSet() )
        {
            startTime= Ticks::Now();
            logables.Add( "'now'" );
        }
        else
        {
            DateTime asDateTime;
            TextLogger* asTextLogger= dynamic_cast<TextLogger*>(logger);
            if( asTextLogger != nullptr )
                asDateTime= asTextLogger->MetaInfo->DateConverter.ToDateTime( startTime );
            else
                asDateTime= TickConverter().ToDateTime( startTime );
            logables.Add( "{:yyyy-MM-dd HH:mm:ss}", asDateTime );
        }
        // do
        logger->TimeOfCreation.SetAs( startTime );
        logger->TimeOfLastLog .SetAs( startTime );

        logInternal( Verbosity::Info, "LGR", logables );
    }

    if ( loggerName.IsNotEmpty() && !foundOne )
    {
        Boxes& logables= acquireInternalLogables();
        logables.Add( "Logger {!Q}: not found. Start time not set.", loggerName );
        logInternal( Verbosity::Error, "LGR", logables );
        return;
    }
}


void Lox::MapThreadName( const String& threadName, ThreadID id )
{
    ALIB_ASSERT_ERROR ( Lock.GetSafeness() == Safeness::Unsafe || Lock.CountAcquirements() > 0,
                        "Lox not acquired" )

    // get current thread id
    String origThreadName;
    if ( id == 0 )
    {
        Thread*         t= Thread::GetCurrent();
        id=             t->GetId();
        origThreadName= t->GetName();
    }

    // add entry
    scopeInfo.threadDictionary[id]= threadName;

    // log info on this
    Boxes& logables= acquireInternalLogables();
    logables.Add( "Mapped thread ID {} to {!Q}.", id, threadName);
    if ( origThreadName.IsNotEmpty() )
        logables.Add(" Original thread name: {!Q}.", origThreadName );
    logInternal( Verbosity::Info, "THR", logables );
}

void Lox::once(  const NString& domain,   Verbosity verbosity,
                 const Box&     logable,
                 const String&  pGroup,
                 Scope          scope,
                 int            quantity                                                            )
{
    int pathLevel= checkScopeInformation( scope, "DMN" );
    if( pathLevel < 0 )
        return;

    // We need a group. If none is given, there are two options:
    NString512 group(pGroup);
    bool groupWasEmtpy= group.IsEmpty();
    if ( groupWasEmtpy )
    {
        // GLOBAL scope: exact code line match
        if ( scope == Scope::Global )
        {
            scope= Scope::Filename;
            group._('#')._( scopeInfo.GetLineNumber() );
        }

        // not GLOBAL scope: Unique group per Scope
        else
        {
            group._( noKeyHashKey );
        }
    }

    // get the store
    scopeLogOnce.InitAccess( scope, pathLevel, threads::UNDEFINED );

    std::map<NAString, int>* map= scopeLogOnce.Get();
    if( map == nullptr )
    {
        map= new std::map<NAString, int>();
        scopeLogOnce.Store( map );
    }

    // create map entry (if not created yet)
    auto it=  map->find( group );
    if (it == map->end() )
        it=   map->insert( std::make_pair( group, 0) ).first;

    // log Once
    if ( quantity >= 0 )
    {
        if ( it->second < quantity )
        {
            it->second++;

            // do the log
            GetLogableContainer().Add( std::forward<const Box&>( logable ) );
            Entry( domain, verbosity );

            // log info if this was the last time
            if( it->second == quantity )
            {
                NString256 msg;
                msg <<  "Once() reached limit of "     << quantity
                    <<  " logs. No further logs for ";

                if ( groupWasEmtpy )
                {
                    if ( scope == Scope::Global )
                        msg << "this line";
                    else
                        msg  << (scope + pathLevel);
                }
                else
                {
                    msg << "group \"" << group << '"';
                    if ( scope != Scope::Global )
                        msg << " in "  << (scope + pathLevel);
                }
                msg << '.';

                logInternal( Verbosity::Info, "", msg );
            }
        }
    }

    // log Nth
    else
    {
        if ( it->second++ % -quantity == 0 )
        {
            GetLogableContainer().Add( std::forward<const Box&>( logable ) );
            Entry( domain, verbosity );
        }
    }
}

void Lox::storeImpl( const Box& data,  const NString& pKey,  Scope scope )
{
    // We need a key. If none is given, we use a constant one indicating that storage is
    // associated exclusively with scope
    NString256 key(pKey);
    bool keyWasEmtpy= key.IsEmpty();
    if ( keyWasEmtpy )
        key= noKeyHashKey;

    // get path level
    int pathLevel= 0;
    if ( scope > Scope::Path )
    {
        pathLevel= EnumValue( scope - Scope::Path );
        scope= Scope::Path;
    }

    // get the store
    scopeLogData.InitAccess( scope, pathLevel, threads::UNDEFINED );
    std::map<NAString, Box>* map= scopeLogData.Get();
    if( map == nullptr )
    {
        map= new std::map<NAString, Box>;
        scopeLogData.Store( map );
    }

    NString128 msg;

    // create map entry (if not created yet)
    auto it=  map->find( key );
    if ( !data.IsType<void>() )
    {
        bool replacedPrevious= false;
        if ( it == map->end() )
            it=   map->insert( std::make_pair( key, data ) ).first;
        else
        {
            replacedPrevious= true;
            it->second= data;
        }

        // log info if this was the last time
        msg <<  "Stored data ";

        if ( !keyWasEmtpy )
            msg << " with key \"" << key << "\" ";
        msg << "in "  << (scope + pathLevel) << '.';
        if ( replacedPrevious )
            msg << " (Replaced and deleted previous.)";
    }

    // delete
    else
    {
        if ( it != map->end() )
        {
            map->erase( it );
            if ( map->size() == 0 )
            {
                delete map;
                scopeLogData.Remove( nullptr );
            }
            msg <<  "Deleted map data ";
        }
        else
            msg <<  "No map data found to delete ";

        if ( !keyWasEmtpy )
            msg << " with key \"" << key << "\" ";
        msg << "in "  << (scope + pathLevel) << '.';
    }

    logInternal( Verbosity::Info, "LGD", msg );
}


Box Lox::retrieveImpl( const NString& pKey, Scope scope )
{
    // We need a key. If none is given, we use a constant one indicating that storage is
    // associated exclusively with scope
    NString256 key= pKey;
    bool keyWasEmtpy= key.IsEmpty();
    if ( keyWasEmtpy )
        key= noKeyHashKey;

    int pathLevel= 0;
    if ( scope > Scope::Path )
    {
        pathLevel= EnumValue( scope - Scope::Path );
        scope= Scope::Path;
    }
    // get the data (create if not found)
    scopeLogData.InitAccess( scope, pathLevel, threads::UNDEFINED );
    Box returnValue;
    for( int i= 0; i < 2 ; i++ )
    {
        std::map<NAString, Box>* map= scopeLogData.Get();
        if( map != nullptr )
        {
            auto it=  map->find( key );
            if ( it != map->end() )
                returnValue= it->second;
        }

        if ( returnValue.IsType<void>() )
            storeImpl( Box(), pKey, scope + pathLevel );
        else
            break;
    }

    // log info if this was the last time
    NString128 msg;
    msg <<  "Data ";

    if ( !keyWasEmtpy )
        msg << " with key \"" << key << "\" ";
    msg << "in "  << (scope + pathLevel)
        << ( !returnValue.IsType<void>() ? " received." : " not found." );

    logInternal( Verbosity::Info, "LGD", msg );
    return returnValue;
}


void Lox::State( const NString&   domain,
                 Verbosity        verbosity,
                 const  String&   headLine,
                 StateInfo        flags      )
{
    ALIB_ASSERT_ERROR ( Lock.GetSafeness() == Safeness::Unsafe || Lock.CountAcquirements() > 0,
                        "Lox not acquired" )

    NAString buf;
    buf.SetBuffer( 2048 );
    if ( headLine.IsNotEmpty() )
        buf._NC( headLine ).NewLine();

    GetState( buf, flags );

    GetLogableContainer().Add( buf );
    Entry( domain, verbosity );
}

Boxes&  Lox::GetLogableContainer()
{
    auto cntAcquirements= Lock.CountAcquirements();
    ALIB_ASSERT_ERROR(  cntAcquirements >= 1, "Lox not acquired." )
    ALIB_ASSERT_WARNING( cntAcquirements < 5, "Logging recursion depth >= 5" )
    while( logableContainers.size() < static_cast<size_t>(cntAcquirements) )
        logableContainers.emplace_back( new Boxes() );

    Boxes& logables= *logableContainers[static_cast<size_t>(cntAcquirements - 1)];
    logables.clear();
    return logables;
}

void Lox::Entry( const NString& domain, Verbosity verbosity )
{
    ALIB_ASSERT_ERROR( Lock.GetSafeness() == Safeness::Unsafe || Lock.CountAcquirements() > 0,
                       "Lox not acquired" )

    // auto-initialization of debug loggers
    #if ALOX_DBG_LOG
        if( domains.CountLoggers() == 0 && this == ALox::Log() )
            Log::AddDebugLogger( this );
    #endif

    ALIB_ASSERT_ERROR(ALOX.IsInitialized(), "ALox not initialized" )

    CntLogCalls++;

    if ( domains.CountLoggers() == 0 )
        return;

    log( evaluateResultDomain( domain ), verbosity, *logableContainers[static_cast<size_t>(Lock.CountAcquirements() - 1)], Inclusion::Include );
}

void Lox::entryDetectDomainImpl( Verbosity verbosity )
{
    Boxes& logables= *logableContainers[static_cast<size_t>(Lock.CountAcquirements() - 1)];
    if ( logables.size() > 1 && logables[0].IsArrayOf<nchar>() )
    {
        NString firstArg= logables[0].Unbox<NString>();

        // accept internal domain at the start
        integer idx= 0;
        if( firstArg.StartsWith( ALox::InternalDomains ) )
            idx+= ALox::InternalDomains.Length();

        // loop over domain and check for illegal characters
        bool illegalCharacterFound= false;
        for( ;  idx< firstArg.Length() ; ++idx )
        {
            char c= firstArg[idx];
            if (!    (    isdigit( c )
                       || ( c >= 'A' && c <= 'Z' )
                       || c == '-'
                       || c == '_'
                       || c == '/'
                       || c == '.'
              )      )
            {
                illegalCharacterFound= true;
                break;
            }
        }

        if ( illegalCharacterFound )
        {
            Entry( nullptr, verbosity );
            return;
        }

        logables.erase( logables.begin() );
        Entry( firstArg, verbosity );
        return;
    }

    Entry( nullptr, verbosity );
}


// #################################################################################################
// internals
// #################################################################################################
Domain* Lox::evaluateResultDomain( const NString& domainPath )
{
    NString128 resDomain;

    // 0. internal domain tree?
    if ( domainPath.StartsWith( ALox::InternalDomains ) )
    {
        // cut "$/" from the path
        resDomain._( domainPath, ALox::InternalDomains.Length() );
        return findDomain( internalDomains, resDomain );
    }

    // loop over scopes
    NString64 localPath;
    scopeDomains.InitWalk( Scope::ThreadInner,
                           // we have to provide nullptr if parameter is empty
                           domainPath.IsNotEmpty() ? &localPath._(domainPath)
                                                   : nullptr
                           );
    NAString* nextDefault;
    while( (nextDefault= scopeDomains.Walk() ) != nullptr )
    {
        ALIB_ASSERT( nextDefault->IsNotEmpty() );

        if ( resDomain.IsNotEmpty() )
            resDomain.InsertAt( "/", 0);
        resDomain.InsertAt( *nextDefault, 0 );

        // absolute path? That's it
        if ( resDomain.CharAtStart() == Domain::Separator() )
            break;
    }

    return findDomain( domains, resDomain );
}

void Lox::getVerbosityFromConfig( detail::Logger*  logger, detail::Domain*  dom,
                                  Variable& variable )
{
    // get logger number. It may happen that the logger is not existent in this domain tree.
    int loggerNo= dom->GetLoggerNo( logger ) ;
    if ( loggerNo < 0 )
        return;

    for( int varNo= 0; varNo< variable.Size(); varNo++ )
    {
        Tokenizer verbosityTknzr( variable.GetString( varNo ), '=' );

        NString256 domainStrBuf;
        Substring domainStrParser= verbosityTknzr.Next();
        if ( domainStrParser.ConsumeString<Case::Ignore>( A_CHAR("INTERNAL_DOMAINS")) )
        {
            while ( domainStrParser.ConsumeChar('/') )
                ;
            domainStrBuf << ALox::InternalDomains << domainStrParser;
        }
        else
            domainStrBuf._( domainStrParser );

        NSubstring domainStr= domainStrBuf ;

        Substring    verbosityStr= verbosityTknzr.Next();
        if ( verbosityStr.IsEmpty() )
            continue;

        int searchMode= 0;
        if ( domainStr.ConsumeChar       ( '*' ) )    searchMode+= 2;
        if ( domainStr.ConsumeCharFromEnd( '*' ) )    searchMode+= 1;
        if(     ( searchMode == 0 && dom->FullPath.Equals         <Case::Ignore>( domainStr )     )
            ||  ( searchMode == 1 && dom->FullPath.StartsWith<true,Case::Ignore>( domainStr )     )
            ||  ( searchMode == 2 && dom->FullPath.EndsWith  <true,Case::Ignore>( domainStr )     )
            ||  ( searchMode == 3 && dom->FullPath.IndexOf   <true,Case::Ignore>( domainStr ) >=0 )
            )
        {
            Verbosity verbosity(Verbosity::Info);
            verbosityStr.ConsumeEnum<Verbosity>( verbosity );
            dom->SetVerbosity( loggerNo, verbosity, variable.Priority );

            // log info on this
            NString512 msg; msg._NC( "Logger \"" )._NC( logger->GetName() ) ._NC( "\":" )._(NFormat::Tab(11 + maxLoggerNameLength))
                               ._NC( '\'' )._NC( dom->FullPath )
                               ._( '\'' ).InsertChars(' ', maxDomainPathLength - dom->FullPath.Length() + 1 )
                               ._( "= Verbosity::" )
                               ._( std::make_pair(verbosity, dom->GetPriority( loggerNo )) ).TrimEnd()
                               ._NC( '.' );

            logInternal( Verbosity::Info, "LGR", msg );
        }
    }
}

void Lox::getDomainPrefixFromConfig( detail::Domain*  dom )
{
    Variable variable( Variables::PREFIXES,
                       #if ALIB_CHARACTERS_ARE_NARROW
                            GetName()
                       #else
                            String128( GetName() )
                       #endif
                     );
    if( ALOX.Config->Load( variable ) == Priorities::NONE )
        return;

    for( int varNo= 0; varNo< variable.Size(); varNo++ )
    {
        Tokenizer prefixTok( variable.GetString( varNo ), '=' );

        NString128 domainStrBuf;
        Substring domainStrParser= prefixTok.Next();
        if ( domainStrParser.ConsumeString<Case::Ignore>( A_CHAR("INTERNAL_DOMAINS")) )
        {
            while ( domainStrParser.ConsumeChar('/') )
                ;
            domainStrBuf << ALox::InternalDomains << domainStrParser;
        }
        else
            domainStrBuf._( domainStrParser );

        NSubstring domainStr= domainStrBuf ;

        Tokenizer prefixTokInner( prefixTok.Next(), ',' );
        Substring prefixStr= prefixTokInner.Next();
        if ( prefixStr.IsEmpty() )
            continue;
        if ( prefixStr.ConsumeChar( '\"' ) )
            prefixStr.ConsumeCharFromEnd( '\"' );

        Inclusion otherPLs= Inclusion::Include;
        prefixTokInner.Next();
        if ( prefixTokInner.Actual.IsNotEmpty() )
            prefixTokInner.Actual.ConsumeEnumOrBool( otherPLs, Inclusion::Exclude, Inclusion::Include );

        int searchMode= 0;
        if ( domainStr.ConsumeChar       ( '*' ) )    searchMode+= 2;
        if ( domainStr.ConsumeCharFromEnd( '*' ) )    searchMode+= 1;
        if(     ( searchMode == 0 && dom->FullPath.Equals         <Case::Ignore>( domainStr )     )
            ||  ( searchMode == 1 && dom->FullPath.StartsWith<true,Case::Ignore>( domainStr )     )
            ||  ( searchMode == 2 && dom->FullPath.EndsWith  <true,Case::Ignore>( domainStr )     )
            ||  ( searchMode == 3 && dom->FullPath.IndexOf   <true,Case::Ignore>( domainStr ) >=0 )
            )
        {
            dom->PrefixLogables.emplace_back( new PrefixLogable( prefixStr ), otherPLs );

            // log info on this
            NString128 msg; msg._NC( "String \"" )._NC( prefixStr )._NC ( "\" added as prefix logable for domain \'" )
                               ._NC( dom->FullPath )
                               ._NC( "\'. (Retrieved from configuration variable" )
                               ._NC( variable.Fullname )._( ".)" );

            logInternal( Verbosity::Info, "PFX", msg );
        }
    }
}

void Lox::getAllVerbosities( detail::Logger*  logger, detail::Domain*  dom,
                             Variable& variable  )
{
    // get verbosity for us
    getVerbosityFromConfig( logger, dom, variable );

    // loop over all sub domains (recursion)
    for ( Domain* subDomain : dom->SubDomains )
        getAllVerbosities( logger, subDomain, variable );
}


Domain* Lox::findDomain( Domain& rootDomain, NString domainPath )
{
    int maxSubstitutions= 10;
    NString128 substPath;
    for(;;)
    {
        // loop for creating domains, one by one
        Domain* dom= nullptr;
        for(;;)
        {
            bool wasCreated;
            dom= rootDomain.Find( domainPath, 1, &wasCreated );
            if ( wasCreated )
            {
                // get maximum domain path length (for nicer State output only...)
                if ( maxDomainPathLength < dom->FullPath.Length() )
                     maxDomainPathLength=  dom->FullPath.Length();

                // log info on new domain
                Boxes& logables= acquireInternalLogables();
                logables.Add( "{!Q} registered.", dom->FullPath );
                logInternal( Verbosity::Info, "DMN", logables );
            }

            // read domain from config
            if ( !dom->ConfigurationRead )
            {
                dom->ConfigurationRead= true;

                Variable variable;
                for ( int i= 0; i < dom->CountLoggers(); ++i )
                {
                    Logger* logger= dom->GetLogger(i);
                    if ( Priorities::NONE != ALOX.Config->Load( variable.Declare(Variables::VERBOSITY, GetName(), logger->GetName()) ) )
                        getVerbosityFromConfig( logger, dom, variable );
                }

                getDomainPrefixFromConfig( dom );
            }

            if ( wasCreated )
            {
                if ( dom->CountLoggers() == 0 )
                    logInternal( Verbosity::Verbose, "DMN", "   No loggers set, yet." );
                else
                {
                    for ( int i= 0; i < dom->CountLoggers(); i++ )
                    {
                        NString256 msg; msg._("  \"")._( dom->GetLogger(i)->GetName() )._("\": ");
                                        msg.InsertChars( ' ', maxLoggerNameLength  + 6 - msg.Length() );
                                        msg._( dom->FullPath )._(" = " )
                                        ._(std::make_pair(dom->GetVerbosity(i), dom->GetPriority(i)));
                        logInternal( Verbosity::Verbose, "DMN", msg );
                    }
                }
            }
            else
                break;
        }

        // apply domain substitutions
        if( domainSubstitutions.size() > 0 )
        {
            substPath.Reset();
            while( maxSubstitutions-- > 0  )
            {
                // loop over rules
                bool substituted= false;
                for( auto& rule : domainSubstitutions )
                {
                    switch( rule.type )
                    {
                        case DomainSubstitutionRule::Type::StartsWith:
                            if( substPath.IsEmpty() )
                            {
                                if ( dom->FullPath.StartsWith( rule.Search ) )
                                {
                                    substPath._( rule.Replacement )._( dom->FullPath, rule.Search.Length() );
                                    substituted= true;
                                    continue;
                                }
                            }
                            else
                            {
                                if ( substPath.StartsWith( rule.Search ) )
                                {
                                    substPath.ReplaceSubstring<false>( rule.Replacement, 0, rule.Search.Length()  );
                                    substituted= true;
                                    continue;
                                }
                            }
                        break;

                        case DomainSubstitutionRule::Type::EndsWith:
                            if( substPath.IsEmpty() )
                            {
                                if ( dom->FullPath.EndsWith( rule.Search ) )
                                {
                                    substPath._( dom->FullPath, 0, dom->FullPath.Length() - rule.Search.Length() )._( rule.Replacement );
                                    substituted= true;
                                    continue;
                                }
                            }
                            else
                            {
                                if ( substPath.EndsWith( rule.Search ) )
                                {
                                    substPath.DeleteEnd( rule.Search.Length() )._( rule.Replacement );
                                    substituted= true;
                                    continue;
                                }
                            }
                        break;


                        case DomainSubstitutionRule::Type::Substring:
                        {
                            if( substPath.IsEmpty() )
                            {
                                integer idx= dom->FullPath.IndexOf( rule.Search );
                                if ( idx >= 0 )
                                {
                                    substPath._( dom->FullPath, 0, idx )._( rule.Replacement)._( dom->FullPath, idx + rule.Search.Length() );
                                    substituted= true;
                                    continue; //next rule
                                }
                            }
                            else
                            {
                                integer idx= substPath.IndexOf( rule.Search, 0 );
                                if ( idx >= 0 )
                                {
                                    substPath.ReplaceSubstring<false>( rule.Replacement, idx, rule.Search.Length()  );
                                    substituted= true;
                                    continue; //next rule
                                }
                            }
                        }
                        break;


                        case DomainSubstitutionRule::Type::Exact:
                        {
                            if( substPath.IsEmpty() )
                            {
                                if ( dom->FullPath.Equals( rule.Search ) )
                                {
                                    substPath._( rule.Replacement);
                                    substituted= true;
                                    continue; //next rule
                                }
                            }
                            else
                            {
                                if ( substPath.Equals( rule.Search) )
                                {
                                    substPath.Reset( rule.Replacement );
                                    substituted= true;
                                    continue; //next rule
                                }
                            }
                        }
                        break;

                    } // switch rule type

                }//rules loop

                // stop if non was found
                if( !substituted )
                    break;
            }

            // too many substitutions?
            if ( maxSubstitutions <= 0 && !oneTimeWarningCircularDS )
            {
                oneTimeWarningCircularDS= true;
                logInternal( Verbosity::Error, "DMN",
                             "The Limit of 10 domain substitutions was reached. Circular substitution assumed!"
                             " (This error is only reported once!)"   );
            }

            // anything substituted?
            if( substPath.Length() > 0 )
            {
                domainPath= substPath;
                continue;
            }
        }

        return dom;
    }
}

int Lox::checkScopeInformation( Scope& scope, const NString& internalDomain )
{
    int pathLevel= 0;
    if ( scope > Scope::Path )
    {
        pathLevel= EnumValue( scope - Scope::Path );
        scope= Scope::Path;
    }

    if (     ( scope == Scope::Path     &&  scopeInfo.GetFullPath().IsEmpty() )
         ||  ( scope == Scope::Filename &&  scopeInfo.GetFileName().IsEmpty() )
         ||  ( scope == Scope::Method   &&  scopeInfo.GetMethod()  .IsEmpty() ) )
    {
        NString256 msg;
            msg << "Missing scope information. Cant use "  << (scope + pathLevel) << '.';
        logInternal( Verbosity::Error, internalDomain, msg );
        return -1;
    }
    return pathLevel;
}

bool Lox::isThreadRelatedScope( Scope scope )
{
    // check
    if (    scope == Scope::ThreadOuter
         || scope == Scope::ThreadInner )
        return true;

    NString128 msg;
    msg  << "Illegal parameter, only Scope::ThreadOuter and Scope::ThreadInner allowed."
            " Given: " << scope  << '.';
    logInternal( Verbosity::Error, "DMN", msg );

    ALIB_DBG( results::Report::GetDefault()
                .DoReport(  scopeInfo.GetOrigFile(), scopeInfo.GetLineNumber(), scopeInfo.GetMethod(),
                            results::Report::Types::Error,
                            "Illegal parameter, only Scope::ThreadOuter and Scope::ThreadInner allowed." );
            )

    return false;
}

void Lox::log( detail::Domain* dom, Verbosity verbosity, Boxes& logables, Inclusion includePrefixes )
{
    dom->CntLogCalls++;
    bool logablesCollected= false;
    Box marker;
    for ( int i= 0; i < dom->CountLoggers() ; i++ )
        if( dom->IsActive( i, verbosity ) )
        {
            // lazily collect objects once an active logger is found
            if ( !logablesCollected )
            {
                logablesCollected= true;
                scopePrefixes.InitWalk( Scope::ThreadInner, &marker );
                const Box* next;
                int qtyUserLogables= static_cast<int>( logables.size() );
                int qtyThreadInners= -1;

                while( (next= scopePrefixes.Walk() ) != nullptr )
                {
                    if( next != &marker )
                    {
                        // this is false for internal domains (only domain specific logables are added there)
                        if ( includePrefixes == Inclusion::Include )
                        {
                            // after marker is read, logables need to be prepended. This is checked below
                            // using "qtyThreadInners < 0"
                            if ( next->IsType<Boxes*>() )
                            {
                                auto* boxes= next->Unbox<Boxes*>();
                                for (int pfxI= static_cast<int>(boxes->size()) - 1 ; pfxI >= 0 ; --pfxI )
                                    logables.insert( logables.begin() + ( qtyThreadInners < 0 ? qtyUserLogables : 0 ),
                                                    (*boxes)[static_cast<size_t>(pfxI)] );
                            }
                            else
                                logables.insert( logables.begin() + ( qtyThreadInners < 0 ? qtyUserLogables : 0 ), *next );
                        }
                    }

                    // was this the actual? then insert domain-associated logables now
                    else
                    {
                        bool excludeOthers= false;
                        qtyThreadInners= static_cast<int>( logables.size() ) - qtyUserLogables;
                        Domain* pflDom= dom;
                        while ( pflDom != nullptr )
                        {
                            for( auto it= pflDom->PrefixLogables.rbegin() ; it != pflDom->PrefixLogables.rend() ; it++ )
                            {
                                // a list of logables? Copy them
                                PrefixLogable& prefix= *it->first;
                                if ( prefix.IsType<Boxes*>() )
                                {
                                    auto* boxes= prefix.Unbox<Boxes*>();
                                    for (int pfxI= static_cast<int>(boxes->size()) - 1 ; pfxI >= 0 ; --pfxI )
                                        logables.insert( logables.begin(), (*boxes)[static_cast<size_t>(pfxI)] );
                                }
                                else
                                    logables.insert( logables.begin(), prefix );


                                if ( it->second == Inclusion::Exclude )
                                {
                                    excludeOthers= true;
                                    break;
                                }
                            }

                            pflDom= excludeOthers ? nullptr :  pflDom->Parent;
                        }

                        // found a stoppable one? remove those from thread inner and break
                        if (excludeOthers)
                        {
                            for ( int ii= 0; ii < qtyThreadInners ; ii++ )
                                logables.pop_back();
                            break;
                        }
                    }
                }
            } // end of collection

            Logger* logger= dom->GetLogger(i);
            ALIB_LOCK_WITH(*logger)
                logger->CntLogs++;
                logger->Log( *dom, verbosity, logables, scopeInfo );
                logger->TimeOfLastLog= Ticks::Now();
        }
}

void Lox::logInternal( Verbosity verbosity, const NString& subDomain, Boxes& msg )
{
    ALIB_ASSERT_ERROR(ALOX.IsInitialized(), "ALox not initialized" )
    log( findDomain( internalDomains, subDomain ), verbosity, msg, Inclusion::Exclude );

    internalLogables[--internalLogRecursionCounter]->clear();
}

void Lox::logInternal( Verbosity verbosity, const NString& subDomain, const NString& msg )
{
    Boxes& logables= acquireInternalLogables();
    logables.Add( msg );
    logInternal( verbosity, subDomain, logables );
}

}}}// namespace [aworx::lib::lox]
