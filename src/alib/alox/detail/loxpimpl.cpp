// #################################################################################################
//  aworx::lib::lox - ALox Logging Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)
#   if !defined (HPP_ALIB_FS_MODULES_DISTRIBUTION)
#      include "alib/lib/fs_modules/distribution.hpp"
#   endif

#   if !defined (HPP_ALOX_CONSOLE_LOGGER)
#      include "alib/alox/loggers/consolelogger.hpp"
#   endif
#   if !defined (HPP_ALOX_ANSI_LOGGER)
#      include "alib/alox/loggers/ansilogger.hpp"
#   endif
#   if !defined (HPP_ALOX_WINDOWS_CONSOLE_LOGGER)
#      include "alib/alox/loggers/windowsconsolelogger.hpp"
#   endif

#   if !defined(HPP_ALIB_ENUMS_SERIALIZATION)
#      include "alib/enums/serialization.hpp"
#   endif
#   if !defined (HPP_ALIB_FS_COMMONENUMS)
#      include "alib/lib/fs_commonenums/commonenums.hpp"
#   endif
#   if !defined (HPP_ALIB_STRINGS_UTIL_TOKENIZER)
#      include "alib/strings/util/tokenizer.hpp"
#   endif
#   if !defined (HPP_ALIB_STRINGS_FORMAT)
#      include "alib/strings/format.hpp"
#   endif

#define HPP_ALIB_LOX_PROPPERINCLUDE
#   if !defined (HPP_ALOX_DETAIL_SCOPE)
#       include "alib/alox/detail/scopestore.inl"
#   endif
#   if !defined (HPP_ALOX_DETAIL_SCOPEINFO)
#       include "alib/alox/detail/scopeinfo.inl"
#   endif
#   include "alib/alox/detail/scopedump.inl"
#undef HPP_ALIB_LOX_PROPPERINCLUDE

#if !defined (HPP_ALIB_ALOXMODULE)
#   include "alib/alox/aloxmodule.hpp"
#endif
#if !defined(HPP_ALIB_RESULTS_REPORT)
#   include "alib/results/report.hpp"
#endif
#endif // !defined(ALIB_DOX)


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

#if ALIB_THREADS
    #define UNDEFINED_THREAD threads::UNDEFINED
#else
    #define UNDEFINED_THREAD 0
#endif

namespace aworx { namespace lib { namespace lox { namespace detail {

/** Domain substitution rules. */
struct DomainSubstitutionRule
{
    /** Rule types. */
    public: enum class Type
    {
        Exact,       ///< Exact match.
        StartsWith,  ///< Starts with match.
        EndsWith,    ///< Ends with match.
        Substring    ///< Any sub-string.
    };

    Type            type;          ///< Denotes the type of the rule, depending of what
                                   ///< was set in originally as search path
    NString32       Search;        ///< The path to search.
    NString32       Replacement;   ///< The replacement.

    /**  Constructor.
     * @param s The path to search.
     * @param r The replacement.
    */
    DomainSubstitutionRule( const NString& s, const NString& r )
    {
        Search     .DbgDisableBufferReplacementWarning();
        Replacement.DbgDisableBufferReplacementWarning();

        // get type and adjust given search parameter
        integer startPos=   0;
        integer length=     s.Length();
        if ( s.CharAtStart() == '*' )
        {
            ++startPos;
            --length;
            if ( s.CharAtEnd() == '*' )
            {
                type= Type::Substring;
                --length;
            }
            else
                type= Type::EndsWith;
        }
        else
        {
            if ( s.CharAtEnd() == '*' )
            {
                type= Type::StartsWith;
                --length;
            }
            else
                type= Type::Exact;
        }
        Search._( s, startPos, length );

        // minimum rule check
        if (   (     (    type == Type::Exact
                       || type == Type::StartsWith )
                 && Search.CharAtStart() != '/'
                )
            || (    type == Type::EndsWith
                &&  Search.CharAtEnd() == '/'
               )
           )
            Search.Reset(); // illegal rule


        Replacement= r;
    }
}; // struct DomainSubstitutionRule


/** ************************************************************************************************
 * Implementation struct for class \alib{lox,Lox} following the
 * \https{Pimpl Idiom,en.cppreference.com/w/cpp/language/pimpl}.
 **************************************************************************************************/
struct LoxImpl
{
    /** The self contained monotonic allocator, that also contains this struct itself. */
    MonoAllocator*                             monoAllocator;

    /** The self contained monotonic allocator, that also contains this struct itself. */
    MonoAllocator::Snapshot                    initialSnapshot;

#if ALIB_THREADS
    /** A mutex to control parallel access. */
    threads::ThreadLock                         Lock;
#else
    int                                         AcquirementsCount;
#endif

    /**
     * A counter for the quantity of calls. The count includes logs that were suppressed by
     * disabled <em>Log Domain</em> and those suppressed by the optional log condition parameter.
     */
    integer                                     CntLogCalls                                      =0;

    /** A temporary variable to be reused (allocate once pattern). */
    Variable                                    tempVar;

    /** A list of a list of logables used for (recursive) logging. */
    std::vector<Boxes*, StdContMA<Boxes*>>      logableContainers;

    /** A list of a list of logables used for (recursive) internal logging. */
    std::vector<Boxes*, StdContMA<Boxes*>>      internalLogables;

    /** The recursion counter for internal logging. */
    integer                                     internalLogRecursionCounter                     = 0;

    /** Information about the source code, method, thread, etc. invoking a log call */
    ScopeInfo                                   scopeInfo;

    /**
     * The root domain \"/\". All registered domains become a sub domain of this root.
     * If a <em>Sub-Log Domain's Verbosity</em> is not explicitly set, such sub domain inherits
     * the verbosity of its parent.
     */
    Domain*                                     domains;

    /** The root domain for internal <em>Log Domains</em>. */
    Domain*                                     internalDomains;

    /** Scope Domains */
    ScopeStore<NString                 , true > scopeDomains;

    /** Prefix logables store */
    ScopeStore<PrefixLogable*          , true > scopePrefixes;

    /** Log once counters */
    ScopeStore<std::map<NString, int>* , false> scopeLogOnce;

    /** Log data store */
    ScopeStore<std::map<NString, Box>* , false> scopeLogData;

    /** Used for tabular output of logger lists */
    integer                                     maxLoggerNameLength                              =0;

    /** Used for tabular output of logger lists */
    integer                                     maxDomainPathLength;

    /** A key value used in stores if no key is given (global object). */
    const aworx::NString                        noKeyHashKey                                  = "$";


    /** The list of domain substitution rules. */
    List<DomainSubstitutionRule>                domainSubstitutions;

    /** Flag if a warning on circular rule detection was logged. */
    bool                                        oneTimeWarningCircularDS                     =false;

    /** Flag used with configuration variable LOXNAME_DUMP_STATE_ON_EXIT. */
    bool                                        loggerAddedSinceLastDebugState               =false;

    /**
     * Constructor.
     * @param ma    The externally created, self-contained monotonic allocator, that also contains
     *              this field.
     * @param name  The lox's name.
     */
    LoxImpl( MonoAllocator* ma, const NString& name )
    : monoAllocator      ( ma )
#if ALIB_THREADS
    , Lock               ( Safeness::Safe )
#else
    , AcquirementsCount  ( 0  )
#endif
    , logableContainers  (*ma )
    , internalLogables   (*ma )
    , scopeInfo          ( name     , ma, tempVar )
    , scopeDomains       ( scopeInfo, ma )
    , scopePrefixes      ( scopeInfo, ma )
    , scopeLogOnce       ( scopeInfo, ma )
    , scopeLogData       ( scopeInfo, ma )
    , domainSubstitutions( ma )
    {
        initialSnapshot= ma->TakeSnapshot();
        LI::init(this);
    }

    /** Destructor.  */
    ~LoxImpl()
    {
        LI::Reset( this, false );
    }

    /** Returns the number of (recursive) acquirements of this \b Lox.
     *  If greater than \c 1, this is either recursive logging or a user has explicitly
     *  acquired this lox repeatedly (which is not recommended to do).
     *
     *  @return The number of acquirements. */
    int CountAcquirements()
    {
        #if ALIB_THREADS
            return Lock.CountAcquirements();
        #else
            return AcquirementsCount;
        #endif
    }

}; // struct LoxImpl


#define ASSERT_ACQUIRED  ALIB_ASSERT_ERROR (  impl->CountAcquirements() > 0, "Lox not acquired" )

// #################################################################################################
// Constructors/destructor
// #################################################################################################

void LI::Construct( Lox* lox, const NString& name, bool doRegister )
{
    MonoAllocator* selfContainedBA= MonoAllocator::Create( 8* 1024 );
    #if ALIB_DEBUG_MONOMEM
        selfContainedBA->LogDomain= A_CHAR("MA/ALOX/LOX");
    #endif

    lox->impl= selfContainedBA->Emplace<LoxImpl>( selfContainedBA, name );

    if( doRegister )
        ALOX.Register( lox, ContainerOp::Insert );
}

void LI::Destruct( Lox* lox )
{
    if( ALOX.Get( lox->GetName(), CreateIfNotExists::No  ) == lox )
        ALOX.Register( lox, ContainerOp::Remove );

    lox->impl->~LoxImpl();
    lox->impl->monoAllocator->~MonoAllocator(); // just destruct, as this is self-contained
}

const NString&  LI::GetName(LoxImpl* impl)
{
    return impl->scopeInfo.loxName;
}

integer&        LI::GetLogCounter(LoxImpl* impl)
{
    return impl->CntLogCalls;
}

#if ALIB_THREADS
ThreadLock&     LI::getLock(LoxImpl* impl)
{
    ALIB_DBG( DbgCheckSingleThreaded(); )
    return impl->Lock;
}
#endif

void            LI::Acquire(LoxImpl* impl, const NCString& file, int line, const NCString& func )
{
#if ALIB_THREADS
    ALIB_REL_DBG(    impl->Lock.Acquire();
                   , impl->Lock.Acquire(file,line,func);    )
#else
    ++impl->AcquirementsCount;
    ALIB_DBG(DbgCheckSingleThreaded();)
#endif

    impl->scopeInfo.Set( file, line, func ALIB_IF_THREADS(, impl->Lock.GetOwner()) );
}

void            LI::Release(LoxImpl* impl)
{
    impl->scopeInfo.Release();
#if ALIB_THREADS
    impl->Lock.Release();
#else
    --impl->AcquirementsCount;
#endif
}

void LI::init(LoxImpl* impl)
{
    impl->logableContainers.reserve(5); // 5 equals the recursive logging warning threshold

    // create domain trees
    impl->domains        = impl->monoAllocator->Emplace<Domain>( impl->monoAllocator, ""    );
    impl->internalDomains= impl->monoAllocator->Emplace<Domain>( impl->monoAllocator, "$"   );

    // create internal sub-domains
    const NString internalDomainList[]= {"LGR","DMN", "PFX", "THR", "LGD", "VAR" };
    for ( auto& it : internalDomainList )
        impl->internalDomains->Find( it, 1, nullptr );
    impl->maxDomainPathLength=  Lox::InternalDomains.Length() + 3;

    // read domain substitution rules from configuration
    Variable variable( Variables::DOMAIN_SUBSTITUTION,
                       #if !ALIB_CHARACTERS_WIDE
                            LI::GetName(impl)
                       #else
                            String128( LI::GetName( impl ) )
                       #endif
                     );
    if ( ALOX.GetConfig().Load( variable ) != Priorities::NONE )
    {
        for( int ruleNo= 0; ruleNo< variable.Size(); ++ruleNo )
        {
            const String& rule= variable.GetString( ruleNo );
            if( rule.IsEmpty() )
                continue;

            integer idx= rule.IndexOf( A_CHAR("->") );
            if ( idx > 0 )
            {
                NString64 domainPath ( rule.Substring<false>( 0, idx  )                          ); domainPath .Trim();
                NString64 replacement( rule.Substring<false>( idx + 2, rule.Length() - idx - 2 ) ); replacement.Trim();
                LI::SetDomainSubstitutionRule( impl, domainPath, replacement );
            }
            else
            {
                // using alib warning here as we can't do internal logging in the constructor
                ALIB_WARNING( "Syntax error in variable {!Q}.", variable.Fullname() )
            }
        }
    }

}

void  LI::Reset(LoxImpl* impl, bool reInitialze)
{
    #if ALOX_DBG_LOG
        if( impl == Log::Get()->impl && Log::DebugLogger != nullptr )
        {
            LOG_ACQUIRE
            Log::RemoveDebugLogger( Log::Get() );
            LOG_RELEASE
        }
    #endif

    auto& domains           = impl->domains;
    auto& internalDomains   = impl->internalDomains;
    auto& scopeDomains      = impl->scopeDomains;
    auto& scopePrefixes     = impl->scopePrefixes;
    auto& scopeLogOnce      = impl->scopeLogOnce;
    auto& scopeLogData      = impl->scopeLogData;

    // unregister each logger in std domains and remove it in internals
    for ( int i= static_cast<int>(domains->CountLoggers()) - 1  ; i >= 0  ; --i )
    {
        Logger* logger= domains->GetLogger( i );
        int ii= internalDomains->GetLoggerNo( logger );
        if ( ii >= 0 )
            internalDomains->RemoveLogger( ii );
        logger->AcknowledgeLox( impl, ContainerOp::Remove );
    }

    // unregister remaining loggers in internal domains
    for ( int i= static_cast<int>(internalDomains->CountLoggers()) - 1  ; i >= 0  ; --i )
    {
        Logger* logger= internalDomains->GetLogger( i );
        logger->AcknowledgeLox( impl, ContainerOp::Remove );
    }

    // clear domain trees
    monomem::Destruct(domains);
    monomem::Destruct(internalDomains);

    // clear scope domains
    if ( scopeDomains.globalStore.IsNotNull() )
        delete[] scopeDomains.globalStore.Buffer();

    {
        ScopeStore<NString, true>::LanguageStoreT::RecursiveIterator it;
        for ( it.Initialize( scopeDomains.languageStore.Root() ); it.IsValid(); it.Next() )
            if( it.Node().Value().IsNotNull() )
                delete[] it.Node().Value().Buffer();
    }

    #if ALIB_THREADS
        for ( auto& thread : scopeDomains.threadStore )
            for ( auto& it : thread.second )
                strings::DeleteString( it );
    #endif

    if(reInitialze)
        scopeDomains.Reset();

    // clear scopePrefixes
    if ( scopePrefixes.globalStore )
        delete static_cast<PrefixLogable*>( scopePrefixes.globalStore );

    {
        ScopeStore<PrefixLogable*, true>::LanguageStoreT::RecursiveIterator it;
        for ( it.Initialize(scopePrefixes.languageStore.Root()); it.IsValid(); it.Next())
            if( it.Node().Value() )
                delete static_cast<PrefixLogable*>( it.Node().Value() );
    }


    #if ALIB_THREADS
        for ( auto& thread : scopePrefixes.threadStore )
            for ( auto& it : thread.second )
                delete static_cast<PrefixLogable*>(it);
    #endif

    if(reInitialze)
        scopePrefixes.Reset();

    // clear log once information
    if ( scopeLogOnce.globalStore )
    {
        for( auto mapEntry=   scopeLogOnce.globalStore->begin();
                  mapEntry != scopeLogOnce.globalStore->end();
                  ++mapEntry  )
            strings::DeleteString( mapEntry->first );
        delete scopeLogOnce.globalStore;
    }

    {
        ScopeStore<std::map<NString, int>*, false>::LanguageStoreT::RecursiveIterator it;
        for ( it.Initialize(scopeLogOnce.languageStore.Root()); it.IsValid(); it.Next() )
            if( it.Node().Value() )
            {
                for( auto mapEntry=   it.Node().Value()->begin();
                          mapEntry != it.Node().Value()->end();
                          ++mapEntry  )
                    delete[] mapEntry->first.Buffer();
                delete it.Node().Value();
            }
    }


    #if ALIB_THREADS
        for ( auto& it : scopeLogOnce.threadStore )
        {
            for( auto mapEntry=   it.second->begin();
                      mapEntry != it.second->end();
                      ++mapEntry  )
                delete[] mapEntry->first.Buffer();
            delete it.second;
        }
    #endif

    if(reInitialze)
        scopeLogOnce.Reset();

    // delete LogData objects
    if ( scopeLogData.globalStore )
    {
        for( auto mapEntry=   scopeLogData.globalStore->begin();
                  mapEntry != scopeLogData.globalStore->end();
                  ++mapEntry  )
            delete[] mapEntry->first.Buffer();
        delete scopeLogData.globalStore;
    }

    {
        ScopeStore<std::map<NString, Box>*, false>::LanguageStoreT::RecursiveIterator it;
        for ( it.Initialize(scopeLogData.languageStore.Root()); it.IsValid(); it.Next() )
            if( it.Node().Value() != nullptr )
            {
                for( auto mapEntry=   it.Node().Value()->begin();
                          mapEntry != it.Node().Value()->end();
                          ++mapEntry  )
                    delete[] mapEntry->first.Buffer();
                delete it.Node().Value();
            }
    }


    #if ALIB_THREADS
        for ( auto it : scopeLogData.threadStore )
        {
            for( auto mapEntry=   it.second->begin();
                      mapEntry != it.second->end();
                      ++mapEntry  )
                delete[] mapEntry->first.Buffer();
            delete it.second;
        }
    #endif

    // other things
    if(reInitialze)
    {
              scopeLogData              .Reset();
        impl->domainSubstitutions       .Reset();
ALIB_IF_THREADS(
        impl->scopeInfo.threadDictionary.Reset(); )
        impl->CntLogCalls=              0;

        // finally clear the monotonic allocator and rebuild the necessary objects
        impl->monoAllocator->Reset( impl->initialSnapshot );
        new (&impl->scopeInfo.scopes)  std::vector<Scope , StdContMA<Scope >>( StdContMA<Scope >(*impl->monoAllocator) );
        new( &impl->logableContainers) std::vector<Boxes*, StdContMA<Boxes*>>( StdContMA<Boxes*>(*impl->monoAllocator) );
        new( &impl->internalLogables ) std::vector<Boxes*, StdContMA<Boxes*>>( StdContMA<Boxes*>(*impl->monoAllocator) );

        LI::SetSourcePathTrimRule( impl, nullptr, Inclusion::Include,
                                  999999, // code for clearing
                                  Case::Ignore, NullNString(), Reach::Global, Priorities::NONE  );

        LI::init(impl);
    }
}

void  LI::SetSourcePathTrimRule( LoxImpl* impl,
                                 const NCString& path,
                                 Inclusion       includeString  ,
                                 int             trimOffset     ,
                                 Case            sensitivity    ,
                                 const NString&  trimReplacement,
                                 Reach           reach          ,
                                 Priorities      priority         )

{
    impl->scopeInfo.SetSourcePathTrimRule( path, includeString, trimOffset, sensitivity,
                                           trimReplacement, reach, priority );
}

void        LI::SetDomain( LoxImpl* impl, const NString& scopeDomain, Scope scope, threads::Thread* thread )
{
    if ( !LI::isThreadRelatedScope( impl, scope ) )
        return;
    LI::setDomain( impl, scopeDomain, scope, false, thread );
}

// #################################################################################################
// Methods
// #################################################################################################
Logger* LI::GetLogger(LoxImpl* impl,  const NString& loggerName )
{
    ASSERT_ACQUIRED

    // search logger
    Logger* logger;
    if ( (logger= impl->domains        ->GetLogger( loggerName ) ) != nullptr )    return logger;
    if ( (logger= impl->internalDomains->GetLogger( loggerName ) ) != nullptr )    return logger;

    // not found
    Boxes& logables= LI::acquireInternalLogables(impl);
    logables.Add( "No logger named {!Q} found.", loggerName );
    LI::logInternal( impl, Verbosity::Warning, "LGR", logables );
    return nullptr;
}

//! @cond NO_DOX
void verbositySettingToVariable( Domain& domain, int loggerNo, Variable& var, String512& tempBuf );
void verbositySettingToVariable( Domain& domain, int loggerNo, Variable& var, String512& tempBuf )
{
    tempBuf.Reset() <<  domain.FullPath << '=' << domain.GetVerbosity( loggerNo );
    var.Add( tempBuf );

    // loop over all sub domains (recursion)
    for ( Domain& subDomain : domain.SubDomains )
        verbositySettingToVariable( subDomain, loggerNo, var, tempBuf );
}
//! @endcond

void LI::writeVerbositiesOnLoggerRemoval( LoxImpl* impl, Logger* logger )
{
    // When writing back we will use this priority as the maximum to write. This way, if this was
    // an automatic default value, we will not write back into the user's variable store.
    // As always, only if the app fetches new variables on termination, this is entry is copied.
    Box replacements[2]= { LI::GetName( impl ), logger->GetName()  };
    VariableDecl verbositiesDecl( Variables::VERBOSITY );
    Variable variable( verbositiesDecl, replacements );

    // first token is "writeback" ?
    ALOX.GetConfig().Load( variable );
    if ( variable.Size() == 0 )
        return;
    Substring firstArg( variable.GetString() );
    if ( !firstArg.ConsumeString<Case::Ignore, Whitespaces::Trim>( A_CHAR("writeback") ) )
        return;

    // optionally read a destination variable name
    Substring destVarCategory= nullptr;
    Substring destVarName    = nullptr;

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
            Boxes& logables= LI::acquireInternalLogables(impl);
            logables.Add( "Argument 'writeback' in variable {!Q}.\\n"
                          "Error: Wrong destination variable name format: {!Q}",
                          variable.Fullname(), firstArg );
            LI::logInternal( impl, Verbosity::Error, "VAR", logables );
            return;
        }
    }

    // either write directly into LOX_LOGGER_VERBOSITY variable...
    String256 oldFullName; oldFullName.DbgDisableBufferReplacementWarning();
    oldFullName << variable.Fullname();
    if( destVarName.IsEmpty() )
    {
        variable.ClearValues( 1 );
    }

    // ...or into a new given variable
    else
    {
        variable.Declare( destVarCategory, destVarName, verbositiesDecl.Delim );
        variable.SetFmtHints                 ( variable.FmtHints()                        );
        variable.ReplaceFormatAttrAlignment  ( variable.FormatAttrAlignment()             );
        variable.ReplaceComments             ( String256("Created at runtime through config option 'writeback' in variable \")")
                                                         << oldFullName <<  "\"." );
    }

    // collect verbosities
    {
        int loggerNoMainDom= impl->domains        ->GetLoggerNo( logger );
        int loggerNoIntDom=  impl->internalDomains->GetLoggerNo( logger );

        String512 tempBuf; tempBuf.DbgDisableBufferReplacementWarning();
        if ( loggerNoMainDom >= 0 ) verbositySettingToVariable( *impl->domains        , loggerNoMainDom, variable, tempBuf );
        if ( loggerNoIntDom  >= 0 ) verbositySettingToVariable( *impl->internalDomains, loggerNoIntDom , variable, tempBuf );
    }

    // now store using the same plug-in as original variable has
    variable.SetPriority( variable.Priority() );
    ALOX.GetConfig().Store( variable );

    // internal logging
    {
        Boxes& logables= LI::acquireInternalLogables(impl);
        logables.Add( "Argument 'writeback' in variable {!Q}:\\n  Verbosities for logger {!Q} written ",
                      oldFullName, logger->GetName() );

        if( destVarName.IsEmpty() )
            logables.Add( "(to source variable)." );
        else
            logables.Add( "to variable {!Q}.", variable.Fullname() );
        LI::logInternal( impl, Verbosity::Info, "VAR", logables );
    }

    // verbose logging of the value written
    {
        Boxes& logables= LI::acquireInternalLogables(impl);
        logables.Add("  Value:");
        for( int i= 0; i< variable.Size() ; ++i )
            logables.Add( "\n    ", variable.GetString(i) );
        LI::logInternal( impl, Verbosity::Verbose, "VAR", logables );
    }
}

void LI::dumpStateOnLoggerRemoval(LoxImpl* impl)
{
    if( !impl->loggerAddedSinceLastDebugState )
        return;
    impl->loggerAddedSinceLastDebugState= false;

    Variable variable( Variables::DUMP_STATE_ON_EXIT,
                       #if !ALIB_CHARACTERS_WIDE
                            LI::GetName( impl )
                       #else
                            String128( LI::GetName( impl ) )
                       #endif
                     );
    ALOX.GetConfig().Load( variable );

    NString64  domain;
    Verbosity  verbosity= Verbosity::Info;
    Substring  tok;
    bool error= false;
    StateInfo flags= StateInfo::NONE;
    for( int tokNo= 0; tokNo< variable.Size(); ++tokNo )
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
                    enums::Parse<Verbosity>( tok, verbosity );
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
        if( !enums::Parse<StateInfo>( tok, stateInfo ) )
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
        Boxes& logables= LI::acquireInternalLogables(impl);
        logables.Add( "Unknown argument {!Q} in variable {} = {!Q}.",
                      tok, variable.Fullname(), variable.GetString() );
        LI::logInternal( impl, Verbosity::Error, "VAR", logables);
    }

    if ( flags != StateInfo::NONE )
    {
        LI::State( impl, domain, verbosity, A_CHAR("Auto dump state on exit requested: "), flags );
    }
}


bool LI::RemoveLogger( LoxImpl* impl, Logger* logger )
{
    ASSERT_ACQUIRED

    int noMainDom=  impl->domains        ->GetLoggerNo( logger );
    int noIntDom=   impl->internalDomains->GetLoggerNo( logger );

    if( noMainDom >= 0 || noIntDom >= 0 )
    {
        LI::dumpStateOnLoggerRemoval(impl);
        LI::writeVerbositiesOnLoggerRemoval( impl, logger );

        if( noMainDom >= 0 )
            impl->domains->RemoveLogger( noMainDom );

        if( noIntDom >= 0 )
            impl->internalDomains->RemoveLogger( noIntDom );

        logger->AcknowledgeLox( impl, ContainerOp::Remove );

        return true;
    }

    // not found
    Boxes& logables= LI::acquireInternalLogables(impl);
    logables.Add( "Logger {!Q} not found. Nothing removed.", logger );
    LI::logInternal( impl, Verbosity::Warning, "LGR", logables );
    return false;
}

Logger* LI::RemoveLogger(LoxImpl* impl,  const NString& loggerName )
{
    ASSERT_ACQUIRED

    int noMainDom=  impl->domains        ->GetLoggerNo( loggerName );
    int noIntDom=   impl->internalDomains->GetLoggerNo( loggerName );

    if( noMainDom >= 0 || noIntDom >= 0 )
    {
        Logger*                 logger=         impl->domains->GetLogger( noMainDom );
        if( logger == nullptr ) logger= impl->internalDomains->GetLogger( noIntDom );

        LI::dumpStateOnLoggerRemoval(impl);
        LI::writeVerbositiesOnLoggerRemoval( impl, logger );

        if( noMainDom >= 0 )
            impl->domains->RemoveLogger( noMainDom );

        if( noIntDom >= 0 )
            impl->internalDomains->RemoveLogger( noIntDom );

        logger->AcknowledgeLox( impl, ContainerOp::Remove );

        Boxes& logables= LI::acquireInternalLogables(impl);
        logables.Add( "Logger {!Q} removed.", logger );
        LI::logInternal( impl, Verbosity::Info, "LGR", logables );
        return logger;
    }

    // not found
    Boxes& logables= LI::acquireInternalLogables(impl);
    logables.Add( "Logger {!Q} not found. Nothing removed.", loggerName );
    LI::logInternal( impl, Verbosity::Warning, "LGR", logables );

    return nullptr;
}

void LI::SetVerbosity(LoxImpl* impl,  Logger* logger, Verbosity verbosity, const NString& domain, Priorities priority )
{
    ASSERT_ACQUIRED

    // check
    if ( logger == nullptr )
    {
        Boxes& logables= LI::acquireInternalLogables(impl);
        logables.Add( "Given Logger is \"null\". Verbosity not set." );
        LI::logInternal( impl, Verbosity::Error, "LGR", logables );
        return;
    }

    // this might create the (path of) domain(s) and set the \e Logger's verbosities like their
    // first parent's or as given in configuration
    Domain* dom= LI::evaluateResultDomain( impl, domain );

    // search logger, insert if not found
    bool isNewLogger= false;
    int no= dom->GetLoggerNo( logger );
    if( no < 0 )
    {
        no= dom->AddLogger( logger );

        // error, logger with same name already exists
        if( no < 0 )
        {
            LI::logInternal( impl, Verbosity::Error, "LGR", LI::acquireInternalLogables(impl)
                         .Add( "Unable to add logger {!Q}. Logger with same name exists.", logger ) );


            LI::logInternal( impl, Verbosity::Verbose, "LGR",
                         LI::acquireInternalLogables(impl).Add(
                           "  Request was: SetVerbosity({!Q}, {!Q}, Verbosity::{}, {}). ",
                           logger, dom->FullPath, verbosity, priority   ) );

            Logger* existingLogger= dom->GetLogger( logger->GetName() );
            LI::logInternal( impl, Verbosity::Verbose, "LGR",  LI::acquireInternalLogables(impl)
                         .Add( "  Existing Logger: {!Q}.", existingLogger ) );

            return;
        }

        // We have to register with the SmartLock facility of the \e Logger.
        // But only if we have not done this yet, via the 'other' root domain tree
        if ( ( dom->GetRoot() == impl->domains ?  impl->internalDomains->GetLoggerNo( logger )
                                         :          impl->domains->GetLoggerNo( logger ) ) < 0 )
        {
            logger->AcknowledgeLox( impl, ContainerOp::Insert );
        }

        // store size of name to support tabular internal log output
        if ( impl->maxLoggerNameLength < logger->GetName().Length() )
             impl->maxLoggerNameLength=  logger->GetName().Length();

        // for internal log
        isNewLogger= true;

        // remember that a logger was set after the last removal
        // (for variable LOXNAME_DUMP_STATE_ON_EXIT)
        impl->loggerAddedSinceLastDebugState= true;
    }

    // get verbosities from configuration
    if( isNewLogger )
    {
        Boxes& logables= LI::acquireInternalLogables(impl);
        logables.Add( "Logger {!Q}.", logger );
        if( domain.StartsWith(Lox::InternalDomains) )
            logables.Add(" added for internal log messages.");
        else
            logables.Add(" added.");
        LI::logInternal( impl, Verbosity::Info, "LGR",  logables );

        // we have to get all verbosities of already existing domains
        Box replacements[2]= { LI::GetName( impl ), logger->GetName()  };
        impl->tempVar.Declare( Variables::VERBOSITY, replacements );
        if( ALOX.GetConfig().Load( impl->tempVar ) != Priorities::NONE )
        {
            LI::getAllVerbosities( impl, logger, *impl->domains         );
            LI::getAllVerbosities( impl, logger, *impl->internalDomains );
        }
    }

    // do
    dom->SetVerbosity( no, verbosity, priority );

    Boxes& logables= LI::acquireInternalLogables(impl);

    logables.Add( "Logger {!Q}: {!Fill}{!Q'}{!Fill}= Verbosity::{}.",
                  logger->GetName(),
                  impl->maxLoggerNameLength - logger->GetName().Length(),
                  dom->FullPath,
                  impl->maxDomainPathLength - dom->FullPath.Length() + 1,
                  std::make_pair(verbosity, priority)                           );

    Verbosity actVerbosity= dom->GetVerbosity( no );
    if( actVerbosity != verbosity )
        logables.Add( " Lower priority ({} < {}). Remains {}.",
                      priority, dom->GetPriority(no), actVerbosity );

    LI::logInternal( impl, Verbosity::Info, "LGR", logables );
}

void LI::SetVerbosity(LoxImpl* impl,  const NString& loggerName, Verbosity verbosity, const NString& domain, Priorities priority )
{
    ASSERT_ACQUIRED

    // get domain
    Domain* dom= LI::evaluateResultDomain( impl, domain );

    // get logger
    Logger* logger;
    int no= dom->GetLoggerNo( loggerName );
    if( no >= 0 )
        logger= dom->GetLogger( no );
    else
    {
        // we have to check if the logger was added in the 'other' tree
        Domain* otherTree=  dom->GetRoot() == impl->domains ?  impl->internalDomains
                                                      :  impl->domains;
        no= otherTree->GetLoggerNo( loggerName );
        if ( no < 0 )
        {
            // error
            Boxes& logables= LI::acquireInternalLogables(impl);
            logables.Add( "Logger not found. Request was: SetVerbosity({!Q}, {!Q}, Verbosity::{}, {}).",
                          loggerName, dom->FullPath, verbosity, priority );
            LI::logInternal( impl, Verbosity::Warning, "LGR",  logables );
            return;
        }

        logger= otherTree->GetLogger( no );
    }

    // use the overloaded method
    LI::SetVerbosity( impl, logger, verbosity, domain, priority );
}

void LI::setDomain( LoxImpl* impl,
                         const NString& scopeDomain, Scope   scope,
                         bool           removeNTRSD, threads::Thread* thread )
{
    //note: the public class interface assures that \p{removeNTRSD} (named thread related scope domain)
    // only evaluates true for thread related scopes

    ASSERT_ACQUIRED

    // check
    int pathLevel= LI::checkScopeInformation( impl, scope, "DMN" );
    if( pathLevel < 0 )
        return;

    #if ALIB_THREADS
        ThreadID threadID= thread != nullptr ? thread->GetId() : UNDEFINED_THREAD;
    #else
        threads::ThreadID threadID= UNDEFINED_THREAD;
        (void) thread;
    #endif

    NString previousScopeDomain;

    impl->scopeDomains.InitAccess( scope, pathLevel, threadID );
    if ( removeNTRSD )
    {
        previousScopeDomain= impl->scopeDomains.Remove( scopeDomain );
    }
    else
    {
        if ( scopeDomain.IsNotEmpty() )
        {
            NString128 trimmable( scopeDomain );
            previousScopeDomain= impl->scopeDomains.Store( strings::AllocateCopy( trimmable.Trim() ) );
        }
        else
            previousScopeDomain= impl->scopeDomains.Remove( nullptr );
    }

    // log info on this
    Boxes& logables= LI::acquireInternalLogables(impl);
    if ( !removeNTRSD && scopeDomain.IsNotEmpty() )
    {
        logables.Add("{!Q'} set as default for {}.",  scopeDomain, (scope + pathLevel) );

        if ( previousScopeDomain  == nullptr )
            LI::logInternal( impl, Verbosity::Info,    "DMN", logables );
        else
        {
            if ( previousScopeDomain.Equals( scopeDomain ) )
            {
                logables.Add( " (Was already set.)" );
                LI::logInternal( impl, Verbosity::Verbose, "DMN", logables );
            }
            else
            {
                logables.Add( " Replacing previous default {!Q'}.", previousScopeDomain );
                LI::logInternal( impl, Verbosity::Warning, "DMN", logables );
            }
        }

    }
    else
    {
        if ( previousScopeDomain  != nullptr )
        {
            logables.Add("{!Q'} removed from {}.",  previousScopeDomain, (scope + pathLevel) );
            LI::logInternal( impl, Verbosity::Info, "DMN", logables );
        }
        else
        {
            if ( removeNTRSD )
                logables.Add("{!Q'} not found. Nothing removed for {}.",  scopeDomain );
            else
                logables.Add("Empty Scope Domain given, nothing registered for {}.",  scopeDomain);

            logables.Add( scope + pathLevel);
            LI::logInternal( impl, Verbosity::Warning, "DMN", logables );
        }
    }

    // it is on us to delete the previous one
    if ( previousScopeDomain.IsNotNull() )
        strings::DeleteString( previousScopeDomain );
}

void LI::RemoveThreadDomain(LoxImpl* impl,  const NString& scopeDomain, Scope scope, threads::Thread* thread )
{
    if ( !LI::isThreadRelatedScope( impl, scope ) )
        return;

    // check
    if (  scopeDomain.IsEmpty() )
    {
        Boxes& logables= LI::acquireInternalLogables(impl);
        logables.Add( "Illegal parameter. No scope domain path given. Nothing removed for {}.",
                      scope );
        LI::logInternal( impl, Verbosity::Warning, "DMN", logables );

        // do nothing
        return;
    }

    // invoke internal master
    LI::setDomain( impl, scopeDomain, scope, true, thread);
}

void LI::SetDomainSubstitutionRule(LoxImpl* impl,  const NString& domainPath, const NString& replacement )
{
    // check null param: clears all rules
    if ( domainPath.IsEmpty() )
    {
        impl->oneTimeWarningCircularDS= false;
        impl->domainSubstitutions.Clear();
        LI::logInternal( impl, Verbosity::Info, "DMN", "Domain substitution rules removed.");
        return;
    }


    // create rule
    DomainSubstitutionRule newRule( domainPath, replacement );
    if ( newRule.Search.IsEmpty() )
    {
        LI::logInternal( impl, Verbosity::Warning, "DMN", "Illegal domain substitution rule. Nothing stored." );
        return;
    }

    // search existing rule
    List<DomainSubstitutionRule>::Iterator  it;
    for( it= impl->domainSubstitutions.begin(); it != impl->domainSubstitutions.end() ; ++it )
    {
        if (     (*it).type == newRule.type
              && (*it).Search.Equals( newRule.Search ) )
            break;
    }

    // no replacement given?
    if ( replacement.IsEmpty() )
    {
        Boxes& logables= LI::acquireInternalLogables(impl);
        if ( it == impl->domainSubstitutions.end() )
        {
            logables.Add("Domain substitution rule {!Q} not found. Nothing to remove.",  domainPath );
            LI::logInternal( impl, Verbosity::Warning, "DMN", logables );
            return;
        }

        logables.Add("Domain substitution rule {!Q} -> {!Q} removed.", domainPath, (*it).Replacement );
        LI::logInternal( impl, Verbosity::Info, "DMN", logables );
        impl->domainSubstitutions.Erase( it );
        return;
    }

    Boxes& logables= LI::acquireInternalLogables(impl);
    logables.Add("Domain substitution rule {!Q} -> {!Q} set.", domainPath, newRule.Replacement );

    // change of rule
    NString256 msg;
    if ( it != impl->domainSubstitutions.end() )
    {
        msg << " Replacing previous -> \"" << (*it).Replacement  << "\".";
        logables.Add( msg );
        (*it).Replacement.Reset( newRule.Replacement );
    }
    else
        impl->domainSubstitutions.EmplaceBack( newRule );

    LI::logInternal( impl, Verbosity::Info, "DMN", logables );
}

void LI::setPrefix(LoxImpl* impl,  const Box& prefix, Scope scope, threads::Thread* thread  )
{
    ASSERT_ACQUIRED

    // check
    int pathLevel= LI::checkScopeInformation( impl, scope, "PFX" );
    if( pathLevel < 0 )
        return;

    #if ALIB_THREADS
        ThreadID threadID= thread != nullptr ? thread->GetId() : UNDEFINED_THREAD;
    #else
        threads::ThreadID threadID= UNDEFINED_THREAD;
        (void) thread;
    #endif

    impl->scopePrefixes.InitAccess( scope, pathLevel, threadID );
    bool isVoidOrEmpty=    prefix.IsType<void>()
                        || prefix.IsNull()
                        || ( prefix.IsArray() && !prefix.UnboxLength() );

    Box* previousLogable=  !isVoidOrEmpty ? impl->scopePrefixes.Store( new PrefixLogable( prefix ) )
                                          : impl->scopePrefixes.Remove( nullptr );


    Boxes& logables= LI::acquireInternalLogables(impl);
    logables.Add( "Object ");
    Verbosity intMsgVerbosity= Verbosity::Info;
    if ( !isVoidOrEmpty )
    {
        logables.Add( prefix, " added as prefix logable for {}.", (scope + pathLevel) );

        if ( previousLogable  != nullptr )
        {
            if ( previousLogable->Call<FEquals>( prefix )  )
            {
                logables.Add(" (Same as before.)");
                intMsgVerbosity= Verbosity::Verbose;
            }
            else
                logables.Add(" Replacing previous {}.", *previousLogable );
        }
    }
    else
    {
        if ( previousLogable  != nullptr )
            logables.Add( "{!Q} removed from list of prefix logables for {}.", *previousLogable);
        else
        {
            logables.Add( "<nullptr> given but no prefix logable to remove for {}.");
            intMsgVerbosity= Verbosity::Warning;
        }
        logables.Add( scope + pathLevel );
    }
    LI::logInternal( impl, intMsgVerbosity, "PFX", logables );

    // it is on us to delete the previous one
    if ( previousLogable != nullptr )
        delete static_cast<PrefixLogable*>( previousLogable );
}


void LI::SetPrefix(LoxImpl* impl,  const Box& prefix, const NString& domain, Inclusion otherPLs )
{
    ASSERT_ACQUIRED

    Domain* dom= LI::evaluateResultDomain( impl, domain );

    bool isVoidOrEmpty=    prefix.IsType<void>()
                        || prefix.IsNull()
                        || ( prefix.IsArray() && !prefix.UnboxLength() );

    Boxes& logables= LI::acquireInternalLogables(impl);
    Verbosity intLogVerbosity= Verbosity::Info;
    PrefixLogable* removedLogable= nullptr;

    if ( !isVoidOrEmpty )
    {
        // create logable: if String* type, then copy the string. We are responsible, then.
        logables.Add( "Object {} added as prefix logable for ", prefix );

        dom->PrefixLogables.EmplaceBack( new PrefixLogable( prefix ), otherPLs );
    }
    else
    {
        auto qtyPLs=  dom->PrefixLogables.Size();
        if ( qtyPLs > 0 )
        {
            removedLogable= dom->PrefixLogables.Back().first;
            dom->PrefixLogables.PopBack();
            logables.Add( "Object {} removed from list of prefix logables for",
                          *static_cast<Box*>(removedLogable)  );
        }
        else
        {
            logables.Add( "No prefix logables to remove for" );
            intLogVerbosity= Verbosity::Warning;
        }
    }

    logables.Add(" domain {!Q'}.", dom->FullPath);
    LI::logInternal( impl, intLogVerbosity, "PFX", logables );

    if( removedLogable )
        delete removedLogable;
}


#if defined (__GLIBCXX__) || defined(__APPLE__)
    void LI::SetStartTime(LoxImpl* impl,  time_t startTime, const NString& loggerName )
    {
        TickConverter converter;
        LI::SetStartTime( impl, converter.ToTicks( DateTime::FromEpochSeconds( startTime ) ), loggerName );
    }

#elif defined( _WIN32 )
    void LI::SetStartTime(LoxImpl* impl,  const FILETIME& startTime, const NString& loggerName )
    {
        TickConverter converter;
        LI::SetStartTime( impl, converter.ToTicks( DateTime::FromFileTime( startTime ) ), loggerName );
    }
#else
    #pragma message "Unknown Platform in file: " __FILE__ )
#endif

void LI::SetStartTime(LoxImpl* impl,  Ticks startTime, const NString& loggerName )
{
    ASSERT_ACQUIRED

    bool foundOne= false;
    for( int loggerNo= 0; loggerNo < impl->domains->CountLoggers(); ++loggerNo )
    {
        // request logger only from main domain tree
        Logger* logger= impl->domains->GetLogger( loggerNo );
        if( loggerName.IsNotEmpty() && !loggerName.Equals<Case::Ignore>( logger->GetName()) )
            continue;
        foundOne= true;

        // log info on this
        Boxes& logables= LI::acquireInternalLogables(impl);
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

        LI::logInternal( impl, Verbosity::Info, "LGR", logables );
    }

    if ( loggerName.IsNotEmpty() && !foundOne )
    {
        Boxes& logables= LI::acquireInternalLogables(impl);
        logables.Add( "Logger {!Q}: not found. Start time not set.", loggerName );
        LI::logInternal( impl, Verbosity::Error, "LGR", logables );
        return;
    }
}


void LI::MapThreadName(LoxImpl* impl,  const String& threadName, threads::ThreadID id )
{
    #if ALIB_THREADS
        ASSERT_ACQUIRED

        // get current thread id
        String origThreadName;
        if ( id == 0 )
        {
            Thread*         t= Thread::GetCurrent();
            id=             t->GetId();
            origThreadName= t->GetName();
        }
        else
            origThreadName= nullptr;

        // add entry
        impl->scopeInfo.threadDictionary.EmplaceOrAssign(id, threadName);

        // log info on this
        Boxes& logables= LI::acquireInternalLogables(impl);
        logables.Add( "Mapped thread ID {} to {!Q}.", id, threadName);
        if ( origThreadName.IsNotEmpty() )
            logables.Add(" Original thread name: {!Q}.", origThreadName );
        LI::logInternal( impl, Verbosity::Info, "THR", logables );
    #else
    (void) impl;
    (void) threadName;
    (void) id;
    #endif
}

void LI::once(  LoxImpl* impl,
                 const NString& domain,   Verbosity verbosity,
                 const Box&     logable,
                 const String&  pGroup,
                 Scope          scope,
                 int            quantity                                                            )
{
    int pathLevel= LI::checkScopeInformation( impl, scope, "DMN" );
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
            group._('#')._( impl->scopeInfo.GetLineNumber() );
        }

        // not GLOBAL scope: Unique group per Scope
        else
        {
            group._( impl->noKeyHashKey );
        }
    }

    // get the store
    impl->scopeLogOnce.InitAccess( scope, pathLevel, UNDEFINED_THREAD );

    std::map<NString, int>* map= impl->scopeLogOnce.Get();
    if( map == nullptr )
    {
        map= new std::map<NString, int>();
        impl->scopeLogOnce.Store( map );
    }

    // create map entry (if not created yet)
    auto it=  map->find( group );
    if (it == map->end() )
        it=   map->insert( std::make_pair( strings::AllocateCopy(group), 0) ).first;

    // log Once
    if ( quantity >= 0 )
    {
        if ( it->second < quantity )
        {
            ++it->second;

            // do the log
            LI::GetLogableContainer(impl) .Add( std::forward<const Box&>( logable ) );
            LI::Entry( impl, domain, verbosity );

            // log info if this was the last time
            if( it->second == quantity )
            {
                Boxes& logables= LI::acquireInternalLogables(impl);
                logables.Add( "Once() reached limit of {} logs. No further logs for ", quantity );

                if ( groupWasEmtpy )
                    logables.Add( scope == Scope::Global ? Box( "this line" )
                                                         : Box(scope + pathLevel) );
                else
                {
                    logables.Add( "group {!Q}", group );
                    if ( scope != Scope::Global )
                        logables.Add(" in ", (scope + pathLevel) );
                }
                logables.Add('.');

                LI::logInternal( impl, Verbosity::Info, "", logables );
            }
        }
    }

    // log Nth
    else
    {
        if ( it->second++ % -quantity == 0 )
        {
            LI::GetLogableContainer(impl) .Add( std::forward<const Box&>( logable ) );
            LI::Entry( impl, domain, verbosity );
        }
    }
}

void LI::store( LoxImpl* impl, const Box& data,  const NString& pKey,  Scope scope )
{
    // We need a key. If none is given, we use a constant one indicating that storage is
    // associated exclusively with scope
    NString256 key(pKey);
    bool keyWasEmtpy= key.IsEmpty();
    if ( keyWasEmtpy )
        key= impl->noKeyHashKey;

    // get path level
    int pathLevel= 0;
    if ( scope > Scope::Path )
    {
        pathLevel= UnderlyingIntegral( scope - Scope::Path );
        scope= Scope::Path;
    }

    // get the store
    impl->scopeLogData.InitAccess( scope, pathLevel, UNDEFINED_THREAD );
    std::map<NString, Box>* map= impl->scopeLogData.Get();
    if( map == nullptr )
    {
        map= new std::map<NString, Box>;
        impl->scopeLogData.Store( map );
    }

    Boxes& logables= LI::acquireInternalLogables(impl);

    // create map entry (if not created yet)
    auto it=  map->find( key );
    if ( !data.IsType<void>() )
    {
        bool replacedPrevious= false;
        if ( it == map->end() )
            it=   map->insert( std::make_pair( strings::AllocateCopy(key), data ) ).first;
        else
        {
            replacedPrevious= true;
            it->second= data;
        }

        // log info if this was the last time
        logables.Add( "Stored data " );

        if ( !keyWasEmtpy )
            logables.Add( " with key {!Q} ", key );
        logables.Add( "in {}.", (scope + pathLevel) );
        if ( replacedPrevious )
            logables.Add( " (Replaced and deleted previous.)" );
    }

    // delete
    else
    {
        if ( it != map->end() )
        {
            const nchar* keyBuffer= it->first.Buffer();
            map->erase( it );
            if ( map->size() == 0 )
            {
                delete map;
                impl->scopeLogData.Remove( nullptr );
            }
            delete [] keyBuffer;
            logables.Add( "Deleted map data " );
        }
        else
            logables.Add( "No map data found to delete " );

        if ( !keyWasEmtpy )
            logables.Add( " with key {!Q} ", key );
        logables.Add( "in {}.", (scope + pathLevel) );
    }

    LI::logInternal( impl, Verbosity::Info, "LGD", logables );
}


Box LI::retrieve( LoxImpl* impl, const NString& pKey, Scope scope )
{
    // We need a key. If none is given, we use a constant one indicating that storage is
    // associated exclusively with scope
    NString256 key= pKey;
    bool keyWasEmtpy= key.IsEmpty();
    if ( keyWasEmtpy )
        key= impl->noKeyHashKey;

    int pathLevel= 0;
    if ( scope > Scope::Path )
    {
        pathLevel= UnderlyingIntegral( scope - Scope::Path );
        scope= Scope::Path;
    }
    // get the data (create if not found)
    impl->scopeLogData.InitAccess( scope, pathLevel, UNDEFINED_THREAD );
    Box returnValue;
    for( int i= 0; i < 2 ; ++i )
    {
        std::map<NString, Box>* map= impl->scopeLogData.Get();
        if( map != nullptr )
        {
            auto it=  map->find( key );
            if ( it != map->end() )
                returnValue= it->second;
        }

        if ( returnValue.IsType<void>() )
            LI::store( impl, Box(), pKey, scope + pathLevel );
        else
            break;
    }

    // log info if this was the last time
    Boxes& logables= LI::acquireInternalLogables(impl);
    logables.Add( "Data " );

    if ( !keyWasEmtpy )
        logables.Add( " with key {!Q} ", key );
    logables.Add( "in ", (scope + pathLevel), ( !returnValue.IsType<void>() ? " received."
                                                                            : " not found." ) );

    LI::logInternal( impl, Verbosity::Info, "LGD", logables );
    return returnValue;
}


void LI::State( LoxImpl* impl,
                 const NString&   domain,
                 Verbosity        verbosity,
                 const  String&   headLine,
                 StateInfo        flags      )
{
    ASSERT_ACQUIRED

    NAString buf;
    buf.SetBuffer( 2048 );
    if ( headLine.IsNotEmpty() )
        buf._NC( headLine ).NewLine();

    LI::GetState( impl, buf, flags );

    LI::GetLogableContainer(impl) .Add( buf );
    LI::Entry( impl, domain, verbosity );
}

Boxes&  LI::GetLogableContainer(LoxImpl* impl)
{
    auto cntAcquirements= impl->CountAcquirements();
    ALIB_ASSERT_ERROR(   cntAcquirements >= 1, "Lox not acquired." )
    ALIB_ASSERT_WARNING( cntAcquirements <  5, "Logging recursion depth >= 5" )
    while( static_cast<int>(impl->logableContainers.size()) < cntAcquirements )
        impl->logableContainers.emplace_back( impl->monoAllocator->Emplace<Boxes>(impl->monoAllocator) );
    Boxes& logables= *impl->logableContainers[static_cast<size_t>(cntAcquirements - 1)];
    logables.clear();
    return logables;
}

void LI::Entry(LoxImpl* impl, const NString& domain, Verbosity verbosity )
{
    ASSERT_ACQUIRED

    // auto-initialization of debug loggers
    #if ALOX_DBG_LOG
        if(   impl == Log::Get()->impl
           && impl->domains->CountLoggers() == 0
           && Log::DebugLogger == nullptr )
            Log::AddDebugLogger( Log::Get() );
    #endif

    ALIB_ASSERT_ERROR(ALOX.IsBootstrapped(), "ALox (ALib) was not properly bootstrapped." )

    ++impl->CntLogCalls;

    if ( impl->domains->CountLoggers() == 0 )
        return;

    LI::log( impl,
             LI::evaluateResultDomain( impl, domain ),
             verbosity,
             *impl->logableContainers[static_cast<size_t>(impl->CountAcquirements() - 1)],
             Inclusion::Include );
}

void LI::IncreaseLogCounter( LoxImpl* impl)
{
    ++impl->CntLogCalls;
}

void LI::entryDetectDomainImpl(LoxImpl* impl,  Verbosity verbosity )
{
    Boxes& logables= *impl->logableContainers[static_cast<size_t>(impl->CountAcquirements() - 1)];
    if ( logables.Size() > 1 && logables[0].IsArrayOf<nchar>() )
    {
        NString firstArg= logables[0].Unbox<NString>();

        // accept internal domain at the start
        integer idx= 0;
        if( firstArg.StartsWith( Lox::InternalDomains ) )
            idx+= Lox::InternalDomains.Length();

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
            LI::Entry( impl, nullptr, verbosity );
            return;
        }

        logables.erase( logables.begin() );
        LI::Entry( impl, firstArg, verbosity );
        return;
    }

    LI::Entry( impl, nullptr, verbosity );
}


// #################################################################################################
// internals
// #################################################################################################
Domain* LI::evaluateResultDomain(LoxImpl* impl,  const NString& domainPath )
{
    NString128 resDomain;

    // 0. internal domain tree?
    if ( domainPath.StartsWith( Lox::InternalDomains ) )
    {
        // cut "$/" from the path
        resDomain._( domainPath, Lox::InternalDomains.Length() );
        return LI::findDomain( impl, *impl->internalDomains, resDomain );
    }

    // loop over scopes
    NString64 localPath; localPath.DbgDisableBufferReplacementWarning();
    impl->scopeDomains.InitWalk( Scope::ThreadInner,
                           // we have to provide NullString if parameter is empty
                           domainPath.IsNotEmpty() ? localPath._(domainPath)
                                                   : NullNString()
                           );
    NString nextDefault;
    while( (nextDefault= impl->scopeDomains.Walk() ).IsNotNull() )
    {
        ALIB_ASSERT( nextDefault.IsNotEmpty() )

        if ( resDomain.IsNotEmpty() )
            resDomain.InsertAt( "/", 0);
        resDomain.InsertAt( nextDefault, 0 );

        // absolute path? That's it
        if ( resDomain.CharAtStart() == Domain::Separator() )
            break;
    }

    return LI::findDomain( impl, *impl->domains, resDomain );
}

void LI::getVerbosityFromConfig(LoxImpl* impl,  Logger*  logger, Domain&  dom )
{
    // get logger number. It may happen that the logger is not existent in this domain tree.
    int loggerNo= dom.GetLoggerNo( logger ) ;
    if ( loggerNo < 0 )
        return;

    for( int varNo= 0; varNo< impl->tempVar.Size(); ++varNo )
    {
        Tokenizer verbosityTknzr( impl->tempVar.GetString( varNo ), '=' );

        NString256 domainStrBuf;
        Substring domainStrParser= verbosityTknzr.Next();
        if ( domainStrParser.ConsumeString<Case::Ignore>( A_CHAR("INTERNAL_DOMAINS")) )
        {
            while ( domainStrParser.ConsumeChar('/') )
                ;
            domainStrBuf << Lox::InternalDomains << domainStrParser;
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
        if(     ( searchMode == 0 && dom.FullPath.Equals         <Case::Ignore>( domainStr )     )
            ||  ( searchMode == 1 && dom.FullPath.StartsWith<true,Case::Ignore>( domainStr )     )
            ||  ( searchMode == 2 && dom.FullPath.EndsWith  <true,Case::Ignore>( domainStr )     )
            ||  ( searchMode == 3 && dom.FullPath.IndexOf   <true,Case::Ignore>( domainStr ) >=0 )
            )
        {
            Verbosity verbosity(Verbosity::Info);
            enums::Parse<Verbosity>(verbosityStr, verbosity );
            dom.SetVerbosity( loggerNo, verbosity, impl->tempVar.Priority() );

            // log info on this
            NString512 msg;
            msg._NC( "Logger \"" )._NC( logger->GetName() ) ._NC( "\":" )._(NFormat::Tab(11 + impl->maxLoggerNameLength))
                ._NC( '\'' )._NC( dom.FullPath )
                ._( '\'' ).InsertChars(' ', impl->maxDomainPathLength - dom.FullPath.Length() + 1 )
                ._( "= Verbosity::" )
                ._( std::make_pair(verbosity, dom.GetPriority( loggerNo )) ).TrimEnd()
                ._NC( '.' );

            LI::logInternal( impl, Verbosity::Info, "LGR", msg );
        }
    }
}

void LI::getDomainPrefixFromConfig(LoxImpl* impl,  Domain&  dom )
{
    Variable variable( Variables::PREFIXES,
                       #if !ALIB_CHARACTERS_WIDE
                            LI::GetName( impl )
                       #else
                            String128( LI::GetName( impl ) )
                       #endif
                     );
    if( ALOX.GetConfig().Load( variable ) == Priorities::NONE )
        return;

    for( int varNo= 0; varNo< variable.Size(); ++varNo )
    {
        Tokenizer prefixTok( variable.GetString( varNo ), '=' );

        NString128 domainStrBuf;
        Substring domainStrParser= prefixTok.Next();
        if ( domainStrParser.ConsumeString<Case::Ignore>( A_CHAR("INTERNAL_DOMAINS")) )
        {
            while ( domainStrParser.ConsumeChar('/') )
                ;
            domainStrBuf << Lox::InternalDomains << domainStrParser;
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
            enums::ParseEnumOrTypeBool( prefixTokInner.Actual, otherPLs, Inclusion::Exclude, Inclusion::Include );

        int searchMode= 0;
        if ( domainStr.ConsumeChar       ( '*' ) )    searchMode+= 2;
        if ( domainStr.ConsumeCharFromEnd( '*' ) )    searchMode+= 1;
        if(     ( searchMode == 0 && dom.FullPath.Equals         <Case::Ignore>( domainStr )     )
            ||  ( searchMode == 1 && dom.FullPath.StartsWith<true,Case::Ignore>( domainStr )     )
            ||  ( searchMode == 2 && dom.FullPath.EndsWith  <true,Case::Ignore>( domainStr )     )
            ||  ( searchMode == 3 && dom.FullPath.IndexOf   <true,Case::Ignore>( domainStr ) >=0 )
            )
        {
            dom.PrefixLogables.EmplaceBack( new PrefixLogable( prefixStr ), otherPLs );

            // log info on this
            NString128 msg; msg._NC( "String \"" )._NC( prefixStr )._NC ( "\" added as prefix logable for domain \'" )
                               ._NC( dom.FullPath )
                               ._NC( "\'. (Retrieved from configuration variable" )
                               ._NC( variable.Fullname() )._( ".)" );

            LI::logInternal( impl, Verbosity::Info, "PFX", msg );
        }
    }
}

void LI::getAllVerbosities(LoxImpl* impl,  Logger*  logger, Domain&  dom )
{
    // get verbosity for us
    LI::getVerbosityFromConfig( impl, logger, dom );

    // loop over all sub domains (recursion)
    for ( Domain& subDomain : dom.SubDomains )
        LI::getAllVerbosities( impl, logger, subDomain );
}


Domain* LI::findDomain(LoxImpl* impl,  Domain& rootDomain, NString domainPath )
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
                if ( impl->maxDomainPathLength < dom->FullPath.Length() )
                     impl->maxDomainPathLength=  dom->FullPath.Length();

                // log info on new domain
                Boxes& logables= LI::acquireInternalLogables(impl);
                logables.Add( "{!Q} registered.", dom->FullPath );
                LI::logInternal( impl, Verbosity::Info, "DMN", logables );
            }

            // read domain from config
            if ( !dom->ConfigurationAlreadyRead )
            {
                dom->ConfigurationAlreadyRead= true;

                Box replacements[2];
                for ( int i= 0; i < dom->CountLoggers(); ++i )
                {
                    Logger* logger= dom->GetLogger(i);
                    replacements[0]= LI::GetName( impl );
                    replacements[1]= logger->GetName();
                    if ( Priorities::NONE != ALOX.GetConfig().Load( impl->tempVar.Declare(Variables::VERBOSITY,
                                                                                 replacements ) ) )
                        LI::getVerbosityFromConfig( impl, logger, *dom );
                }

                LI::getDomainPrefixFromConfig( impl,  *dom );
            }

            if ( wasCreated )
            {
                if ( dom->CountLoggers() == 0 )
                    LI::logInternal( impl, Verbosity::Verbose, "DMN", "   No loggers set, yet." );
                else
                {
                    for ( int i= 0; i < dom->CountLoggers(); ++i )
                    {
                        NString256 msg; msg._("  \"")._( dom->GetLogger(i)->GetName() )._("\": ");
                                        msg.InsertChars( ' ', impl->maxLoggerNameLength  + 6 - msg.Length() );
                                        msg._( dom->FullPath )._(" = " )
                                        ._(std::make_pair(dom->GetVerbosity(i), dom->GetPriority(i)));
                        LI::logInternal( impl, Verbosity::Verbose, "DMN", msg );
                    }
                }
            }
            else
                break;
        }

        // apply domain substitutions
        if( !impl->domainSubstitutions.IsEmpty() )
        {
            substPath.Reset();
            while( maxSubstitutions-- > 0  )
            {
                // loop over rules
                bool substituted= false;
                for( auto& rule : impl->domainSubstitutions )
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
            if ( maxSubstitutions <= 0 && !impl->oneTimeWarningCircularDS )
            {
                impl->oneTimeWarningCircularDS= true;
                LI::logInternal( impl, Verbosity::Error, "DMN",
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

int LI::checkScopeInformation(LoxImpl* impl,  Scope& scope, const NString& internalDomain )
{
    int pathLevel= 0;
    if ( scope > Scope::Path )
    {
        pathLevel= UnderlyingIntegral( scope - Scope::Path );
        scope= Scope::Path;
    }

    if (     ( scope == Scope::Path     &&  impl->scopeInfo.GetFullPath().IsEmpty() )
         ||  ( scope == Scope::Filename &&  impl->scopeInfo.GetFileName().IsEmpty() )
         ||  ( scope == Scope::Method   &&  impl->scopeInfo.GetMethod()  .IsEmpty() ) )
    {
        Boxes& logables= LI::acquireInternalLogables(impl);
        logables.Add( "Missing scope information. Cant use {}.", (scope + pathLevel) );
        LI::logInternal( impl, Verbosity::Error, internalDomain, logables );
        return -1;
    }
    return pathLevel;
}

bool LI::isThreadRelatedScope(LoxImpl* impl,  Scope scope )
{
    // check
    if (    scope == Scope::ThreadOuter
         || scope == Scope::ThreadInner )
        return true;

    Boxes& logables= LI::acquireInternalLogables(impl);
    logables.Add( "Illegal parameter, only Scope::ThreadOuter and Scope::ThreadInner allowed."
                  " Given: {}.", scope );
    LI::logInternal( impl, Verbosity::Error, "DMN", logables );

    ALIB_DBG( results::Report::GetDefault()
                .DoReport(  impl->scopeInfo.GetOrigFile(), impl->scopeInfo.GetLineNumber(), impl->scopeInfo.GetMethod(),
                            results::Report::Types::Error,
                            "Illegal parameter, only Scope::ThreadOuter and Scope::ThreadInner allowed." );
            )

    return false;
}

void LI::log(LoxImpl* impl,  Domain* dom, Verbosity verbosity, Boxes& logables, Inclusion includePrefixes )
{
    ++dom->CntLogCalls;
    bool logablesCollected= false;
    PrefixLogable marker(nullptr);
    for ( int i= 0; i < dom->CountLoggers() ; ++i )
        if( dom->IsActive( i, verbosity ) )
        {
            // lazily collect objects once an active logger is found
            if ( !logablesCollected )
            {
                logablesCollected= true;
                impl->scopePrefixes.InitWalk( Scope::ThreadInner, &marker );
                const Box* next;
                int qtyUserLogables= static_cast<int>( logables.Size() );
                int qtyThreadInners= -1;

                while( (next= impl->scopePrefixes.Walk() ) != nullptr )
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
                                for (int pfxI= static_cast<int>(boxes->Size()) - 1 ; pfxI >= 0 ; --pfxI )
                                    logables.emplace( logables.begin() + ( qtyThreadInners < 0 ? qtyUserLogables : 0 ),
                                                    (*boxes)[static_cast<size_t>(pfxI)] );
                            }
                            else
                                logables.emplace( logables.begin() + ( qtyThreadInners < 0 ? qtyUserLogables : 0 ), *next );
                        }
                    }

                    // was this the actual? then insert domain-associated logables now
                    else
                    {
                        bool excludeOthers= false;
                        qtyThreadInners= static_cast<int>( logables.Size() ) - qtyUserLogables;
                        Domain* pflDom= dom;
                        while ( pflDom != nullptr )
                        {
                            for( auto it= pflDom->PrefixLogables.rbegin() ; it != pflDom->PrefixLogables.rend() ; ++it )
                            {
                                // a list of logables? Copy them
                                PrefixLogable& prefix= *it->first;
                                if ( prefix.IsType<Boxes*>() )
                                {
                                    auto* boxes= prefix.Unbox<Boxes*>();
                                    for (int pfxI= static_cast<int>(boxes->Size()) - 1 ; pfxI >= 0 ; --pfxI )
                                        logables.emplace( logables.begin(),
                                                          (*boxes)[static_cast<size_t>(pfxI)] );
                                }
                                else
                                    logables.emplace( logables.begin(), prefix );


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
                            for ( int ii= 0; ii < qtyThreadInners ; ++ii )
                                logables.pop_back();
                            break;
                        }
                    }
                }
            } // end of collection

            Logger* logger= dom->GetLogger(i);
            ALIB_LOCK_WITH(*logger)
                ++logger->CntLogs;
                logger->Log( *dom, verbosity, logables, impl->scopeInfo );
                logger->TimeOfLastLog= Ticks::Now();
        }
}

Boxes&  LI::acquireInternalLogables(LoxImpl* impl)
{
    if( static_cast<integer>(impl->internalLogables.size()) == impl->internalLogRecursionCounter )
    {
        Boxes* newLogables= impl->monoAllocator->Emplace<Boxes>(impl->monoAllocator);
        impl->internalLogables.emplace_back( newLogables );
    }

    return *impl->internalLogables[static_cast<size_t>(impl->internalLogRecursionCounter++)];
}

void LI::logInternal(LoxImpl* impl,  Verbosity verbosity, const NString& subDomain, Boxes& msg )
{
    ALIB_ASSERT_ERROR(ALOX.IsBootstrapped(), "ALox (ALib) was not properly bootstrapped." )
    LI::log( impl, LI::findDomain( impl, *impl->internalDomains, subDomain ), verbosity, msg, Inclusion::Exclude );

    impl->internalLogables[static_cast<size_t>(--impl->internalLogRecursionCounter)]->clear();
}

void LI::logInternal(LoxImpl* impl,  Verbosity verbosity, const NString& subDomain, const NString& msg )
{
    Boxes& logables= LI::acquireInternalLogables(impl);
    logables.Add( msg );
    LI::logInternal( impl, verbosity, subDomain, logables );
}

#if ALIB_DEBUG_MONOMEM
MonoAllocator& LI::DbgGetMonoAllocator(LoxImpl* impl)
{
    return  *impl->monoAllocator;
}
#endif


#if !defined(ALIB_DOX)

namespace {
void getStateDomainRecursive( Domain& domain, integer maxDomainPathLength, NAString& buf );
void getStateDomainRecursive( Domain& domain, integer maxDomainPathLength, NAString& buf )
{
    integer reference= buf.Length();
    buf._("  "); domain.ToString( buf );
    integer idx= buf.IndexOf( '[', reference );
    buf.InsertChars( ' ', maxDomainPathLength + 5 - idx + reference, idx);
    buf.NewLine();

    // loop over all sub domains (recursion)
    for ( Domain& subDomain : domain.SubDomains )
        getStateDomainRecursive( subDomain, maxDomainPathLength, buf );
}

void getStateDomainsWithDiffVerb( Domain& dom, int loggerNo, std::vector<Domain*>& results );
void getStateDomainsWithDiffVerb( Domain& dom, int loggerNo, std::vector<Domain*>& results )
{
    if (    dom.Parent == nullptr
        ||  dom.Parent->GetVerbosity(loggerNo) != dom.GetVerbosity(loggerNo) )
        results.emplace_back( &dom );

    for( auto& it : dom.SubDomains )
        getStateDomainsWithDiffVerb( it, loggerNo, results );
}

void getStateCollectPrefixes( Domain& dom, integer indentSpaces, NAString& target );
void getStateCollectPrefixes( Domain& dom, integer indentSpaces, NAString& target )
{
    AString buffer;
    for ( auto& pfl : dom.PrefixLogables )
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
        buffer._NC( "<domain>           [" )._NC( dom.FullPath )._NC(']').NewLine();
    }
    target << buffer;

    for( auto& subDom : dom.SubDomains )
        getStateCollectPrefixes( subDom, indentSpaces, target );
}

} // anonymous namespace

#endif // !defined(ALIB_DOX)


void LI::GetState( LoxImpl* impl, NAString& buf, StateInfo flags )
{
    ASSERT_ACQUIRED

    ScopeDump scopeDump( ALIB_IF_THREADS( impl->scopeInfo.threadDictionary, )
                         impl->noKeyHashKey, buf );

    if ( HasBits( flags, StateInfo::CompilationFlags ) )
    {
        buf._NC( "ALib Version:      " )._NC( ALIB.Version)
           ._NC(" (Rev. ")                ._( ALIB.Revision)._(')').NewLine();
        buf._NC( "ALib Comiler Symbols:" ).NewLine();
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
    if( aworx::HasBits( flags,  StateInfo::Basic ) )
        buf._NC( "Name:            \"" )._( impl->scopeInfo.GetLoxName() )._('\"').NewLine();

    if( HasBits( flags,  StateInfo::Version ) )
    {
        buf._NC( "Version:         " )._NC( ALOX.Version)
           ._NC(" (Rev. "            )._(   ALOX.Revision)._(')').NewLine();
        ALIB_IF_THREADS(
            buf._NC( "Thread Safeness: " )._NC( impl->Lock.GetSafeness() ).NewLine(); )
    }

    if( HasBits( flags,  StateInfo::Basic ) )
        buf._NC( "#Log Calls:      " )._NC( impl->CntLogCalls     ).NewLine();

    if(    HasBits( flags,  StateInfo::Basic )
        || HasBits( flags,  StateInfo::Version )  )
        buf.NewLine();

    //  source path trim info
    if( HasBits( flags,  StateInfo::SPTR ) )
    {
        buf._NC( "Source Path Trimming Rules: " ).NewLine();

        int cnt= 0;
        // do 2 times, 0== global list, 1 == local list
        for( int trimInfoNo= 0; trimInfoNo < 2 ; ++trimInfoNo )
        {
            // choose local or global list
            std::vector<ScopeInfo::SourcePathTrimRule>* trimInfoList=
                       trimInfoNo == 0   ? &ScopeInfo::GlobalSPTRs
                                         : &impl->scopeInfo.LocalSPTRs;


            // loop over trimInfo
            for ( auto& ti : *trimInfoList )
            {
                ++cnt;
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
    if( HasBits( flags,  StateInfo::DSR ) )
    {
        buf._NC( "Domain Substitution Rules: " ).NewLine();
        if( !impl->domainSubstitutions.IsEmpty() )
        {
            // get size
            integer maxWidth= 0;
            for ( auto& it : impl->domainSubstitutions )
                if ( maxWidth < it.Search.Length() )
                     maxWidth = it.Search.Length();
            maxWidth+= 2;

            // write
            for ( auto& it : impl->domainSubstitutions )
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
    if( HasBits( flags,  StateInfo::Once ) )
    {
        buf._NC( "Once() Counters: " ).NewLine();
        if ( scopeDump.writeStoreMap( &impl->scopeLogOnce ) == 0 )
            buf._NC("  <no Once() counters set>" ).NewLine();
        buf.NewLine();
    }

    // Log Data
    if( HasBits( flags,  StateInfo::LogData ) )
    {
        buf._NC( "Log Data: " ).NewLine();
        if ( scopeDump.writeStoreMap( &impl->scopeLogData ) == 0 )
            buf._NC("  <no data objects stored>" ).NewLine();
        buf.NewLine();
    }

    // Prefix Logables
    if( HasBits( flags,  StateInfo::PrefixLogables ) )
    {
        buf._NC( "Prefix Logables: " ).NewLine();
        integer oldLength= buf.Length();
        scopeDump.writeStore( &impl->scopePrefixes, 2 );
        getStateCollectPrefixes( *impl->domains, 2, buf );
        if ( oldLength == buf.Length() )
            buf._NC("  <no prefix logables set>" ).NewLine();
        buf.NewLine();
    }

    // thread mappings
    if( HasBits( flags,  StateInfo::ThreadMappings ) )
    {
        #if ALIB_THREADS
            buf._NC( "Named Threads:   " ).NewLine();
            if ( impl->scopeInfo.threadDictionary.Size() == 0 )
                buf._NC("  <no thread name mappings set>" ).NewLine();
            else
                for ( auto& pair : impl->scopeInfo.threadDictionary )
                {
                    buf._NC( "  " ) << NFormat::Field( String32() << '(' << pair.first << "):", 7, Alignment::Left )
                                    << '\"' << pair.second << '\"';
                    buf.NewLine();
                }
            buf.NewLine();
        #endif
    }

    // Scope Domains
    if( HasBits( flags,  StateInfo::ScopeDomains ) )
    {
        buf._NC( "Scope Domains: " ).NewLine();
        if ( scopeDump.writeStore( &impl->scopeDomains, 2 ) == 0 )
            buf._NC("  <no scope domains set>" ).NewLine();
        buf.NewLine();
    }

    // Loggers
    if( HasBits( flags,  StateInfo::Loggers ) )
    {
        TickConverter dateTimeConverter;
        std::vector<Domain*> domainsWithDiffVerb;
        for (int treeNo= 0; treeNo < 2; ++treeNo )
        {
            int cnt= 0;
            Domain* domTree;
            if( treeNo==0 )
            {
                 domTree= impl->domains;
                 buf._NC( "Loggers:" ).NewLine();
            }
            else
            {
                 domTree= impl->internalDomains;
                 buf._NC( "Loggers on Internal Domains:" ).NewLine();
            }

            for ( int loggerNo= 0; loggerNo< domTree->CountLoggers(); ++loggerNo )
            {
                ++cnt;
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
                getStateDomainsWithDiffVerb( *domTree, loggerNo, domainsWithDiffVerb);
                for ( Domain* dom : domainsWithDiffVerb )
                {
                    buf._NC("    ")
                       ._(  dom == *domainsWithDiffVerb.begin() ? "Verbosities:   "
                                                                : "               " );

                    integer tabRef= buf.Length();
                    buf << dom->FullPath << NFormat::Tab( impl->maxDomainPathLength +1, tabRef);

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
    if( HasBits( flags,  StateInfo::InternalDomains ) )
    {
        buf._NC( "Internal Domains:" ).NewLine();
        getStateDomainRecursive( *impl->internalDomains, impl->maxDomainPathLength, buf );
        buf.NewLine();
    }

    // Domains
    if( HasBits( flags,  StateInfo::Domains ) )
    {
        buf._NC( "Domains:" ).NewLine();
        getStateDomainRecursive( *impl->domains        , impl->maxDomainPathLength, buf );
        buf.NewLine();
    }
}

}// namespace aworx::lib::lox[::detail]


// #################################################################################################
// Static methods of Lox
// #################################################################################################
TextLogger* Lox::CreateConsoleLogger(const NString& name)
{
    //--- check configuration setting "CONSOLE_TYPE"  ---

    Variable variable( Variables::CONSOLE_TYPE );
    ALOX.GetConfig().Load( variable );
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
                  variable.Fullname(), variable.GetString() )

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

}}}// namespace [aworx::lib::lox]

#undef UNDEFINED_THREAD
#undef ASSERT_ACQUIRED
