//##################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
#include "alib_precompile.hpp"
#if !defined(ALIB_C20_MODULES) || ((ALIB_C20_MODULES != 0) && (ALIB_C20_MODULES != 1))
#   error "Symbol ALIB_C20_MODULES has to be given to the compiler as either 0 or 1"
#endif
#if ALIB_C20_MODULES
    module;
#endif
//========================================= Global Fragment ========================================
#include "alib/boxing/boxing.prepro.hpp"
#include "alib/variables/variables.prepro.hpp"
#include "alib/alox/alox.prepro.hpp"
#include "ALib.Monomem.StdContainers.H"
//============================================== Module ============================================
#if ALIB_C20_MODULES
    module ALib.ALox.Impl;
    import   ALib.ALox;
    import   ALib.Lang;
    import   ALib.EnumOps;
    import   ALib.Strings;
    import   ALib.Strings.Tokenizer;
    import   ALib.Boxing;
    import   ALib.EnumRecords;
    import   ALib.EnumRecords.Bootstrap;
    import   ALib.Variables;
    import   ALib.Camp;
    import   ALib.Camp.Base;
    import   ALib.Bootstrap;
#else
#   include "ALib.Bootstrap.H"
#   include "ALib.Lang.H"
#   include "ALib.Strings.H"
#   include "ALib.Strings.Tokenizer.H"
#   include "ALib.Boxing.H"
#   include "ALib.EnumRecords.Bootstrap.H"
#   include "ALib.Variables.H"
#   include "ALib.Camp.H"
#   include "ALib.Camp.Base.H"
#   include "ALib.ALox.H"
#   include "ALib.ALox.Impl.H"
#endif
//========================================== Implementation ========================================
#if !ALIB_SINGLE_THREADED
#   define UNDEFINED_THREAD threads::UNDEFINED
#else
#   define UNDEFINED_THREAD 0
#endif

ALIB_BOXING_VTABLE_DEFINE( std::pair<alib::lox::Verbosity
                           ALIB_COMMA alib::variables::Priority>, vt_lox_pair_verby_prio )

ALIB_BOXING_VTABLE_DEFINE( alib::lox::detail::Logger*         , vt_lox_logger    )

#   include "ALib.Lang.CIFunctions.H"

namespace alib {  namespace lox { namespace detail {

/// Domain substitution rules.
struct DomainSubstitutionRule
{
  /// Rule types.
  public: enum class Type
    {
        Exact,       ///< Exact match.
        StartsWith,  ///< Starts with match.
        EndsWith,    ///< Ends with match.
        Substring    ///< Any substring.
    };

    Type            type;          ///< Denotes the type of the rule, depending of what
                                   ///< was set in originally as search path
    NString32       Search;        ///< The path to search.
    NString32       Replacement;   ///< The replacement.

    /// Constructor.
    /// @param s The path to search.
    /// @param r The replacement.
    DomainSubstitutionRule( const NString& s, const NString& r ) {
        Search     .DbgDisableBufferReplacementWarning();
        Replacement.DbgDisableBufferReplacementWarning();

        // get type and adjust given search parameter
        integer startPos=   0;
        integer length=     s.Length();
        if ( s.CharAtStart() == '*' ) {
            ++startPos;
            --length;
            if ( s.CharAtEnd() == '*' ) {
                type= Type::Substring;
                --length;
            }
            else
                type= Type::EndsWith;
        } else {
            if ( s.CharAtEnd() == '*' ) {
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


//==================================================================================================
/// Implementation struct for class \alib{lox;Lox} following the
/// \https{Pimpl Idiom,en.cppreference.com/w/cpp/language/pimpl}.
//==================================================================================================
struct LoxImpl
{
    /// The self contained monotonic allocator, that also contains this struct itself.
    MonoAllocator&                      monoAllocator;

    /// A pool allocator that uses #monoAllocator as its source.
    PoolAllocator                       poolAllocator;

    /// Snapshot taken before embedding the lox in the #monoAllocator.
    monomem::Snapshot                   beforeLox;

    #if !ALIB_SINGLE_THREADED
    /// A mutex to control parallel access.
    threads::RecursiveLock          Lock;
    #endif

    /// Counts the number of nested (recursive) acquirements.
    int                                 AcquirementsCount;

    /// A counter for the quantity of calls. The count includes logs suppressed by
    /// disabled <em>Log Domain</em> and those suppressed by the optional log condition parameter.
    integer                             CntLogCalls                                              =0;

    /// A list of a list of logables used for (recursive) logging.
    StdVectorMA<BoxesMA*>               logableContainers;

    /// A list of a list of logables used for (recursive) internal logging.
    StdVectorMA<BoxesMA*>               internalLogables;

    /// The recursion counter for internal logging.
    integer                             internalLogRecursionCounter                             = 0;

    /// Information about the source code, method, thread, etc. invoking a log call
    ScopeInfo                           scopeInfo;

    /// The root domain \"/\". All registered domains become a sub domain of this root.
    /// If a <em>Sub-Log Domain's Verbosity</em> is not explicitly set, such sub domain inherits
    /// the verbosity of its parent.
    Domain*                             domains;

    /// The root domain for internal <em>Log Domains</em>.
    Domain*                             internalDomains;

    /// Scope Domains
    ScopeStore<NString       , true >   scopeDomains;

    /// Prefix logables store
    ScopeStore<PrefixLogable*, true >   scopePrefixes;

    /// Log once counters
    ScopeStore<SSMap<int>*   , false>   scopeLogOnce;

    /// Log data store
    ScopeStore<SSMap<Box>*   , false>   scopeLogData;

    /// Used for tabular output of logger lists
    integer                             maxLoggerNameLength                                      =0;

    /// Used for tabular output of logger lists
    integer                             maxDomainPathLength;

    /// A key value used in stores if no key is given (global object).
    const NString                       noKeyHashKey                                          = "$";


    /// The list of domain substitution rules.
    ListMA<DomainSubstitutionRule>      domainSubstitutions;

    /// Flag if a warning on circular rule detection was logged.
    bool                                oneTimeWarningCircularDS                             =false;

    /// Flag used with configuration variable LOXNAME_DUMP_STATE_ON_EXIT.
    bool                                loggerAddedSinceLastDebugState                       =false;

    /// Constructor.
    /// @param ma    The externally created, self-contained monotonic allocator, that also contains
    ///              this instance.
    /// @param name  The lox's name.
    LoxImpl( MonoAllocator* ma, const NString& name )
    : monoAllocator      ( *ma )
    , poolAllocator      ( monoAllocator )
    , AcquirementsCount  ( 0  )
    , logableContainers  ( monoAllocator )
    , internalLogables   ( monoAllocator )
    , scopeInfo          ( name     , monoAllocator )
    , scopeDomains       ( scopeInfo, monoAllocator )
    , scopePrefixes      ( scopeInfo, monoAllocator )
    , scopeLogOnce       ( scopeInfo, monoAllocator )
    , scopeLogData       ( scopeInfo, monoAllocator )
    , domainSubstitutions( monoAllocator )
    {
        IF_ALIB_THREADS(  ALIB_DBG(Lock.Dbg.Name= "Lox";) )
        LI::init(this);
    }

    /// Destructor.
    ~LoxImpl() {
        // unregister each logger in std domains and remove it in internals
        for ( int i= domains->CountLoggers() - 1  ; i >= 0  ; --i ) {
            Logger* logger= domains->GetLogger( i );
            int ii= internalDomains->GetLoggerNo( logger );
            if ( ii >= 0 )
                internalDomains->RemoveLogger( ii );
            logger->AcknowledgeLox( this, lang::ContainerOp::Remove );
        }

        // unregister remaining loggers in internal domains
        for ( int i= internalDomains->CountLoggers() - 1  ; i >= 0  ; --i ) {
            Logger* logger= internalDomains->GetLogger( i );
            logger->AcknowledgeLox( this, lang::ContainerOp::Remove );
    }   }

    /// Returns the number of (recursive) acquirements of this \b Lox.
    /// If greater than \c 1, this is either recursive logging or a user has explicitly
    /// acquired this lox repeatedly (which is not recommended to do).
    ///
    /// @return The number of acquirements.
    int CountAcquirements()                             const noexcept { return AcquirementsCount; }

    /// Shortcut to allocate arbitrary objects in #poolAllocator.
    /// @tparam T     The type to allocate.
    /// @tparam TArgs Types of variadic parameters given with parameter \p{args}.
    ///               Deduced by the compiler.
    /// @param  args  Variadic parameters to be forwarded to the constructor of type \p{T}.
    /// @return The allocated object.
    template<typename T, typename... TArgs>
    T*      newPO(TArgs&&... args)  { return poolAllocator().New<T>(std::forward<TArgs>(args)...); }

    /// Shortcut to delete arbitrary objects in #poolAllocator.
    /// @tparam T     The type to allocate. Deduced by the compiler.
    /// @param  o     The allocated object.
    template<typename T>
    void    deletePO(T* o)                                         { poolAllocator().Delete<T>(o); }

}; // struct LoxImpl


#define ASSERT_ACQUIRED  ALIB_ASSERT_ERROR( impl->CountAcquirements() >0,"ALOX","Lox not acquired" )

//##################################################################################################
// Constructors/destructor
//##################################################################################################
LoxImpl* LI::Construct(const NString& name) {
    lang::HeapAllocator ha;
    MonoAllocator* selfContainedMA= MonoAllocator::Create( ALIB_DBG(nullptr,) ha, 8* 1024 );
    ALIB_DBG( selfContainedMA->DbgName= NCString(*selfContainedMA, NString128("Lox") << name).Buffer(); )
    auto snapShot= selfContainedMA->TakeSnapshot();
    LoxImpl* result= (*selfContainedMA)().New<LoxImpl>( selfContainedMA, name );
    result->beforeLox= snapShot;
    return result;
}

void LI::Destruct( LoxImpl* impl ) {
    auto& ma= impl->monoAllocator;
    impl->~LoxImpl();
    lang::Destruct(ma); // just destruct, as this is self-contained
}

const NString&  LI::GetName(LoxImpl* impl)                       { return impl->scopeInfo.loxName; }

integer&        LI::GetLogCounter(LoxImpl* impl)                       { return impl->CntLogCalls; }

#if !ALIB_SINGLE_THREADED
RecursiveLock&     LI::getLock(LoxImpl* impl)
{
    ALIB_DBG( assert::SingleThreaded(); )
    return impl->Lock;
}
#endif

void            LI::Acquire(LoxImpl* impl, const lang::CallerInfo& ci  ) {
    #if !ALIB_SINGLE_THREADED
        ALIB_REL_DBG(    impl->Lock.AcquireRecursive();
                       , impl->Lock.AcquireRecursive(ci);    )
    #else
        ALIB_DBG( assert::SingleThreaded());
    #endif
    ++impl->AcquirementsCount;
    impl->scopeInfo.Set( ci );
}

void            LI::Release(LoxImpl* impl) {
    impl->scopeInfo.PopNestedScope();
    --impl->AcquirementsCount;
    #if !ALIB_SINGLE_THREADED
        impl->Lock.ReleaseRecursive(ALIB_CALLER_PRUNED);
    #endif
}

void LI::init(LoxImpl* impl) {
    impl->logableContainers.reserve(5); // 5 equals the recursive logging warning threshold

    // create domain trees
    impl->domains        = impl->monoAllocator().New<Domain>(impl->monoAllocator, impl->poolAllocator, ""    );
    impl->internalDomains= impl->monoAllocator().New<Domain>(impl->monoAllocator, impl->poolAllocator, "$"   );

    // create internal Subdomains
    const NString internalDomainList[]= {"LGR","DMN", "PFX", "THR", "LGD", "VAR" };
    for ( auto& it : internalDomainList )
        impl->internalDomains->Find( it, 1, nullptr );
    impl->maxDomainPathLength=  Lox::InternalDomains.Length() + 3;

    // read domain substitution rules from configuration
    Variable var= variables::CampVariable(ALOX);
    {
        ALIB_LOCK_WITH(ALOX.GetConfig())
        var.Declare( Variables::DOMAIN_SUBSTITUTION,
                                         #if !ALIB_CHARACTERS_WIDE
                                                      GetName(impl)
                                         #else
                                                      String128( GetName( impl ) )
                                         #endif
                                                      );
    }
    if ( var.IsDefined() ) {
        for( int ruleNo= 0 ; ruleNo < var.Size() ; ++ruleNo ) {
            Substring rule= var.GetString(ruleNo);
            if( rule.Trim().IsEmpty() )
                continue;

            integer idx= rule.IndexOf( A_CHAR("->") );
            if ( idx > 0 ) {
                NString256 domainPath ( rule.Substring<NC>( 0, idx  )                          ); domainPath .Trim();
                NString256 replacement( rule.Substring<NC>( idx + 2, rule.Length() - idx - 2 ) ); replacement.Trim();
                SetDomainSubstitutionRule( impl, domainPath, replacement );
            } else {
                // using alib warning here as we can't do internal logging in the constructor
                ALIB_WARNING( "ALOX", "Syntax error in variable \"{}\".", var )
    }   }   }

}

void  LI::Reset(LoxImpl* impl) {
    #if ALOX_DBG_LOG
        if( impl == Log::Get()->impl && Log::DebugLogger != nullptr ) {
            LOG_ACQUIRE
            Log::RemoveDebugLogger( Log::Get() );
            LOG_RELEASE
        }
    #endif

    SetSourcePathTrimRule( impl, nullptr, lang::Inclusion::Include,
                           999999, // code for clearing
                           lang::Case::Ignore, NULL_NSTRING, lang::Reach::Global, Priority::NONE  );

    //  clear the monotonic allocator and rebuild the impl
    MonoAllocator* loxMA    = &impl->monoAllocator;
    auto           snapshot = impl->beforeLox;
    NString128 name( impl->scopeInfo.loxName);
    (*loxMA)().Delete(impl);
    loxMA->Reset(snapshot);
    ALIB_DBG( auto* dbgNewAddress= )
    (*loxMA)().New<LoxImpl>(loxMA, name ); // creates the impl at the same position, therefore, it does not need
                                           // to be stored (and passed back)
    ALIB_ASSERT_ERROR( impl == dbgNewAddress, "ALOX", "Internal error. This must never happen." )
    impl->beforeLox= snapshot;
}

void LI::SetFileNameCacheCapacity( LoxImpl* impl, integer numberOfLists, integer entriesPerList )
{ impl->scopeInfo.SetFileNameCacheCapacity( numberOfLists, entriesPerList ); }

#if ALIB_DEBUG_MEMORY
MonoAllocator& LI::DbgGetMonoAllocator( LoxImpl* impl ) { return impl->monoAllocator; }
#endif

void  LI::SetSourcePathTrimRule( LoxImpl* impl,
                                 const NCString& path,
                                 lang::Inclusion includeString  ,
                                 int             trimOffset     ,
                                 lang::Case      sensitivity    ,
                                 const NString&  trimReplacement,
                                 lang::Reach     reach          ,
                                 Priority        priority         )
{
    impl->scopeInfo.SetSourcePathTrimRule( path, includeString, trimOffset, sensitivity,
                                           trimReplacement, reach, priority );
}

void        LI::SetDomain( LoxImpl*         impl , const NString&   scopeDomain,
                           Scope            scope, threads::Thread* thread       ) {
    if ( !isThreadRelatedScope( impl, scope ) )
        return;
    setDomain( impl, scopeDomain, scope, false, thread );
}

//##################################################################################################
// Methods
//##################################################################################################
Logger* LI::GetLogger(LoxImpl* impl,  const NString& loggerName ) {
    ASSERT_ACQUIRED

    // search logger
    Logger* logger;
    if ( (logger= impl->domains        ->GetLogger( loggerName ) ) != nullptr )    return logger;
    if ( (logger= impl->internalDomains->GetLogger( loggerName ) ) != nullptr )    return logger;

    // not found
    BoxesMA& logables= acquireInternalLogables(impl);
    logables.Add( "No logger named {!Q} found.", loggerName );
    logInternal( impl, Verbosity::Warning, "LGR", logables );
    return nullptr;
}

//! @cond NO_DOX
namespace {
void writeVerbVarRecursive( Domain& domain, int loggerNo, CVVerbosities& verbosities,
                            Verbosity parentVerbosity ) {
    auto verbosity= domain.GetVerbosity( loggerNo );
    if( parentVerbosity != verbosity || verbosities.ExportAll )
        verbosities.Add(String256(domain.FullPath) << '=' << verbosity );

    // loop over all subdomains (recursion)
    for ( Domain& subDomain : domain.SubDomains )
        writeVerbVarRecursive( subDomain, loggerNo, verbosities, verbosity );
}
} // anonymous namespace
//! @endcond

void LI::writeVerbositiesOnLoggerRemoval( LoxImpl* impl, Logger* logger ) {
DOX_MARKER([DOX_VARIABLES_REPLACEMENTS2])
Variable var= variables::CampVariable(ALOX);
{ALIB_LOCK_WITH(ALOX.GetConfig())
    // a local array of boxes of size two, to fill variable placeholders
    Box replacements[2]=
    {
        GetName( impl ),    // name of this Lox
        logger->GetName()   // name of the Logger
    };

    // declare the individually named variable
    var.Declare( Variables::VERBOSITY, replacements );
}
DOX_MARKER( [DOX_VARIABLES_REPLACEMENTS2])
    // we do not care about the writing rights.
    (void) var.Define();
    auto& cvVerb= var.Get<CVVerbosities>();
    cvVerb.Clear();

    // collect verbosities
    {
        int loggerNoMainDom= impl->domains        ->GetLoggerNo( logger );
        int loggerNoIntDom=  impl->internalDomains->GetLoggerNo( logger );

        if ( loggerNoMainDom >= 0 ) writeVerbVarRecursive( *impl->domains        , loggerNoMainDom, cvVerb, Verbosity(-1)  );
        if ( loggerNoIntDom  >= 0 ) writeVerbVarRecursive( *impl->internalDomains, loggerNoIntDom , cvVerb, Verbosity(-1)  );
    }

    // internal logging
    {
        // get variable name. Needs shared acquisition
        String256 varName;
        { ALIB_LOCK_SHARED_WITH(ALOX.GetConfig())
            varName << var; // this is needed because we are logging the name of a variable!
        }
        BoxesMA& logables= acquireInternalLogables(impl);
        logables.Add( "Verbosities for logger {!Q} written to variable {!Q}",
                      logger->GetName(), varName );
        logInternal( impl, Verbosity::Info, "VAR", logables );
    }

    // verbose logging of the value written
    {
        BoxesMA& logables= acquireInternalLogables(impl);
        logables.Add("  Value:");
        for( auto& it : cvVerb )
            logables.Add( "\n    ", it );
        logInternal( impl, Verbosity::Verbose, "VAR", logables );
}   }

void LI::dumpStateOnLoggerRemoval(LoxImpl* impl) {
    if( !impl->loggerAddedSinceLastDebugState )
        return;
    impl->loggerAddedSinceLastDebugState= false;

    Variable variable= variables::CampVariable(ALOX, Variables::DUMP_STATE_ON_EXIT,
                                                   #if !ALIB_CHARACTERS_WIDE
                                                       GetName( impl )
                                                   #else
                                                       String128( GetName( impl ) )
                                                   #endif
                                                                                      );
    if( !variable.IsDefined() )
        return;
    NString64  domain;
    Verbosity  verbosity= Verbosity::Info;
    Substring  tok;
    bool error= false;
    StateInfo flags= StateInfo::NONE;
    Tokenizer tknzr;
    tknzr.Set(variable, ',', true);
    while( tknzr.HasNext() ) {
        tok=  tknzr.Next();

        // read log domain and verbosity
        if( tok.IndexOf( '=' ) > 0 ) {
            if( tok.ConsumePartOf<lang::Case::Ignore, lang::Whitespaces::Trim>( A_CHAR("verbosity"), 1) ) {
                if( tok.ConsumeChar<lang::Case::Sensitive, lang::Whitespaces::Trim>( '=' ) )
                    enumrecords::Parse<Verbosity>( tok, verbosity );
                continue;
            }
            if( tok.ConsumePartOf<lang::Case::Ignore, lang::Whitespaces::Trim>( A_CHAR("domain"), 1) ) {
                if( tok.ConsumeChar<lang::Case::Sensitive, lang::Whitespaces::Trim>( '=' ) )
                    domain= tok.Trim();
                continue;
            }
            error= true;
            break;
        }

        // read and add state
        StateInfo stateInfo;
        if( !enumrecords::Parse<StateInfo>( tok, stateInfo ) ) {
            error= true;
            break;
        }

        // None clears all, others are added
        if( stateInfo == StateInfo::NONE )
            flags= StateInfo::NONE;
        else
            flags|= stateInfo;
    }
    if( error ) {
        BoxesMA& logables= acquireInternalLogables(impl);
        logables.Add( "Unknown argument {!Q} in variable {} = {!Q}.",
                      tok, variable, variable.GetString() );
        logInternal( impl, Verbosity::Error, "VAR", logables);
    }

    if ( flags != StateInfo::NONE ) {
        State( impl, domain, verbosity, A_CHAR("Auto dump state on exit requested: "), flags );
}   }


bool LI::RemoveLogger( LoxImpl* impl, Logger* logger ) {
    ASSERT_ACQUIRED

    int noMainDom=  impl->domains        ->GetLoggerNo( logger );
    int noIntDom=   impl->internalDomains->GetLoggerNo( logger );

    if( noMainDom >= 0 || noIntDom >= 0 ) {
        dumpStateOnLoggerRemoval(impl);
        writeVerbositiesOnLoggerRemoval( impl, logger );

        if( noMainDom >= 0 )
            impl->domains->RemoveLogger( noMainDom );

        if( noIntDom >= 0 )
            impl->internalDomains->RemoveLogger( noIntDom );

        logger->AcknowledgeLox( impl, lang::ContainerOp::Remove );

        return true;
    }

    // not found
    BoxesMA& logables= acquireInternalLogables(impl);
    logables.Add( "Logger {!Q} not found. Nothing removed.", logger );
    logInternal( impl, Verbosity::Warning, "LGR", logables );
    return false;
}

Logger* LI::RemoveLogger(LoxImpl* impl,  const NString& loggerName ) {
    ASSERT_ACQUIRED

    int noMainDom=  impl->domains        ->GetLoggerNo( loggerName );
    int noIntDom=   impl->internalDomains->GetLoggerNo( loggerName );

    if( noMainDom >= 0 || noIntDom >= 0 ) {
        Logger*                 logger=         impl->domains->GetLogger( noMainDom );
        if( logger == nullptr ) logger= impl->internalDomains->GetLogger( noIntDom );

        dumpStateOnLoggerRemoval(impl);
        writeVerbositiesOnLoggerRemoval( impl, logger );

        if( noMainDom >= 0 )
            impl->domains->RemoveLogger( noMainDom );

        if( noIntDom >= 0 )
            impl->internalDomains->RemoveLogger( noIntDom );

        logger->AcknowledgeLox( impl, lang::ContainerOp::Remove );

        BoxesMA& logables= acquireInternalLogables(impl);
        logables.Add( "Logger {!Q} removed.", logger );
        logInternal( impl, Verbosity::Info, "LGR", logables );
        return logger;
    }

    // not found
    BoxesMA& logables= acquireInternalLogables(impl);
    logables.Add( "Logger {!Q} not found. Nothing removed.", loggerName );
    logInternal( impl, Verbosity::Warning, "LGR", logables );

    return nullptr;
}

void LI::SetVerbosity(LoxImpl* impl,  Logger* logger, Verbosity verbosity, const NString& domain, Priority priority )
{   ASSERT_ACQUIRED

    // check
    if ( logger == nullptr ) {
        BoxesMA& logables= acquireInternalLogables(impl);
        logables.Add( "Given Logger is \"null\". Verbosity not set." );
        logInternal( impl, Verbosity::Error, "LGR", logables );
        return;
    }

    // this might create the (path of) domain(s) and set the \e Logger's verbosities like their
    // first parent's or as given in configuration
    Domain* dom= evaluateResultDomain( impl, domain );

    // search logger, insert if not found
    bool isNewLogger= false;
    int no= dom->GetLoggerNo( logger );
    if( no < 0 ) {
        no= dom->AddLogger( logger );

        // error, logger with same name already exists
        if( no < 0 ) {
            logInternal( impl, Verbosity::Error, "LGR", acquireInternalLogables(impl)
                       .Add( "Unable to add logger {!Q}. Logger with same name exists.", logger ) );


            logInternal( impl, Verbosity::Verbose, "LGR",
                         acquireInternalLogables(impl).Add(
                           "  Request was: SetVerbosity({!Q}, {!Q}, Verbosity::{}, {}). ",
                           logger, dom->FullPath, verbosity, priority   ) );

            Logger* existingLogger= dom->GetLogger( logger->GetName() );
            logInternal( impl, Verbosity::Verbose, "LGR",  acquireInternalLogables(impl)
                         .Add( "  Existing Logger: {!Q}.", existingLogger ) );

            return;
        }

        // We have to tell the logger that it got inserted, but only if we have not done this yet,
        // via the 'other' root domain tree.
        if ( ( dom->GetRoot() == impl->domains ?  impl->internalDomains->GetLoggerNo( logger )
                                               :  impl->domains->GetLoggerNo( logger )
             ) < 0 )
        {
            logger->AcknowledgeLox( impl, lang::ContainerOp::Insert );
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
    if( isNewLogger ) {
        BoxesMA& logables= acquireInternalLogables(impl);
        logables.Add( "Logger {!Q}.", logger );
        if( domain.StartsWith(Lox::InternalDomains) )
            logables.Add(" added for internal log messages.");
        else
            logables.Add(" added.");
        logInternal( impl, Verbosity::Info, "LGR",  logables );

        // we have to get all verbosities of already existing domains
        Box replacements[2]= { GetName( impl ), logger->GetName()  };
        Variable varVerbosities= variables::CampVariable(ALOX, Variables::VERBOSITY, replacements );
        if( varVerbosities.IsDefined() ) {
            getAllVerbosities( impl, varVerbosities, logger, *impl->domains         );
            getAllVerbosities( impl, varVerbosities, logger, *impl->internalDomains );
    }   }

    // do
    dom->SetVerbosity( no, verbosity, priority );

    BoxesMA& logables= acquireInternalLogables(impl);

    logables.Add( "Logger {!Q}: {!Fill}{!Q'}{!Fill}= Verbosity::{}.",
                  logger->GetName(),
                  impl->maxLoggerNameLength - logger->GetName().Length(),
                  dom->FullPath,
                  impl->maxDomainPathLength - dom->FullPath.Length() + 1,
                  boxing::MakePair(verbosity, priority)                           );

    Verbosity actVerbosity= dom->GetVerbosity( no );
    if( actVerbosity != verbosity )
        logables.Add( " Lower priority ({} < {}). Remains {}.",
                      priority, dom->GetPriority(no), actVerbosity );

    logInternal( impl, Verbosity::Info, "LGR", logables );
}

void LI::SetVerbosity(LoxImpl*       impl     , const NString& loggerName,
                      Verbosity      verbosity, const NString& domain    , Priority priority ) {
    // get logger
    Logger* logger;
    {
        ASSERT_ACQUIRED

       Domain* dom= evaluateResultDomain( impl, domain );

        int no= dom->GetLoggerNo( loggerName );
        if( no >= 0 )
            logger= dom->GetLogger( no );
        else {
            // we have to check if the logger was added in the 'other' tree
            Domain* otherTree=  dom->GetRoot() == impl->domains ?  impl->internalDomains
                                                          :  impl->domains;
            no= otherTree->GetLoggerNo( loggerName );
            if ( no < 0 ) {
                // error
                BoxesMA& logables= acquireInternalLogables(impl);
                logables.Add( "Logger not found. Request was: SetVerbosity({!Q}, {!Q}, Verbosity::{}, {}).",
                              loggerName, dom->FullPath, verbosity, priority );
                logInternal( impl, Verbosity::Warning, "LGR",  logables );
                return;
            }

            logger= otherTree->GetLogger( no );
    }   }
    // use the overloaded method
    SetVerbosity( impl, logger, verbosity, domain, priority );
}

void LI::setDomain( LoxImpl* impl,
                    const NString& scopeDomain, Scope   scope,
                    bool           removeNTRSD, threads::Thread* thread ) {
    //note: the public class interface ensures that \p{removeNTRSD} (named thread related scope domain)
    // only evaluates true for thread related scopes

    ASSERT_ACQUIRED

    // check
    int pathLevel= checkScopeInformation( impl, scope, "DMN" );
    if( pathLevel < 0 )
        return;

    #if !ALIB_SINGLE_THREADED
        ThreadID threadID= thread != nullptr ? thread->GetID() : UNDEFINED_THREAD;
    #else
        threads::ThreadID threadID= UNDEFINED_THREAD;
        (void) thread;
    #endif

    NString previousScopeDomain;

    impl->scopeDomains.InitAccess( scope, pathLevel, threadID );
    if ( removeNTRSD ) {
        previousScopeDomain= impl->scopeDomains.Remove( scopeDomain );
    } else {
        if ( scopeDomain.IsNotEmpty() ) {
            NString128 trimmable( scopeDomain );
            previousScopeDomain= impl->scopeDomains.Store( NString(impl->poolAllocator, trimmable.Trim() ) );
        }
        else
            previousScopeDomain= impl->scopeDomains.Remove( nullptr );
    }

    // log info on this
    BoxesMA& logables= acquireInternalLogables(impl);
    if ( !removeNTRSD && scopeDomain.IsNotEmpty() ) {
        logables.Add("{!Q'} set as default for {}.",  scopeDomain, (scope + pathLevel) );

        if ( previousScopeDomain.IsNull() )
            logInternal( impl, Verbosity::Info,    "DMN", logables );
        else {
            if ( previousScopeDomain.Equals<NC>( scopeDomain ) ) {
                logables.Add( "(Wasalreadyset.)");
                logInternal(  impl,Verbosity::Verbose,"DMN",logables);
            } else {
                logables.Add( " Replacing previous default {!Q'}.", previousScopeDomain );
                logInternal( impl, Verbosity::Warning, "DMN", logables );
        }   }

    } else {
        if ( previousScopeDomain.IsNotNull() ) {
            logables.Add("{!Q'} removed from {}.",  previousScopeDomain, (scope + pathLevel) );
            logInternal( impl, Verbosity::Info, "DMN", logables );
        } else {
            if ( removeNTRSD )
                logables.Add("{!Q'} not found. Nothing removed for {}.",  scopeDomain );
            else
                logables.Add("Empty Scope Domain given, nothing registered for {}.",  scopeDomain);

            logables.Add( scope + pathLevel);
            logInternal( impl, Verbosity::Warning, "DMN", logables );
    }   }

    // it is on us to delete the previous one
    if ( previousScopeDomain.IsNotNull() )
       previousScopeDomain.Free(impl->poolAllocator);
}

void LI::RemoveThreadDomain( LoxImpl*   impl,  const NString&   scopeDomain,
                             Scope      scope, threads::Thread* thread )        {
    if ( !isThreadRelatedScope( impl, scope ) )
        return;

    // check
    if (  scopeDomain.IsEmpty() ) {
        BoxesMA& logables= acquireInternalLogables(impl);
        logables.Add( "Illegal parameter. No scope domain path given. Nothing removed for {}.",
                      scope );
        logInternal( impl, Verbosity::Warning, "DMN", logables );

        // do nothing
        return;
    }

    // invoke internal master
    setDomain( impl, scopeDomain, scope, true, thread);
}

void LI::SetDomainSubstitutionRule(LoxImpl* impl,  const NString& domainPath,
    const NString& replacement ) {
    // check null param: clears all rules
    if ( domainPath.IsEmpty() ) {
        impl->oneTimeWarningCircularDS= false;
        impl->domainSubstitutions.Clear();
        logInternal( impl, Verbosity::Info, "DMN", "Domain substitution rules removed.");
        return;
    }


    // create rule
    DomainSubstitutionRule newRule( domainPath, replacement );
    if ( newRule.Search.IsEmpty() ) {
        logInternal( impl, Verbosity::Warning, "DMN", "Illegal domain substitution rule. Nothing stored." );
        return;
    }

    // search existing rule
    ListMA<DomainSubstitutionRule>::iterator  it;
    for( it= impl->domainSubstitutions.begin(); it != impl->domainSubstitutions.end() ; ++it ) {
        if (     (*it).type == newRule.type
              && (*it).Search.Equals<NC>( newRule.Search ) )
            break;
    }

    // no replacement given?
    if ( replacement.IsEmpty() ) {
        BoxesMA& logables= acquireInternalLogables(impl);
        if ( it == impl->domainSubstitutions.end() ) {
            logables.Add("Domain substitution rule {!Q} not found. Nothing to remove.",  domainPath );
            logInternal( impl, Verbosity::Warning, "DMN", logables );
            return;
        }

        logables.Add("Domain substitution rule {!Q} -> {!Q} removed.", domainPath, (*it).Replacement );
        logInternal( impl, Verbosity::Info, "DMN", logables );
        (void) impl->domainSubstitutions.erase( it );
        return;
    }

    BoxesMA& logables= acquireInternalLogables(impl);
    logables.Add("Domain substitution rule {!Q} -> {!Q} set.", domainPath, newRule.Replacement );

    // change of rule
    NString256 msg;
    if ( it != impl->domainSubstitutions.end() ) {
        msg << " Replacing previous -> \"" << (*it).Replacement  << "\".";
        logables.Add( msg );
        (*it).Replacement.Reset( newRule.Replacement );
    }
    else
        impl->domainSubstitutions.emplace_back( newRule );

    if( ALOX.IsBootstrapped() ) // this function might be called very early.
        logInternal( impl, Verbosity::Info, "DMN", logables );
}

void LI::setPrefix(LoxImpl* impl, const Box& prefix, Scope scope, threads::Thread* thread ) {
    ASSERT_ACQUIRED

    // check
    int pathLevel= checkScopeInformation( impl, scope, "PFX" );
    if( pathLevel < 0 )
        return;

    #if !ALIB_SINGLE_THREADED
        ThreadID threadID= thread != nullptr ? thread->GetID() : UNDEFINED_THREAD;
    #else
        threads::ThreadID threadID= UNDEFINED_THREAD;
        (void) thread;
    #endif

    impl->scopePrefixes.InitAccess( scope, pathLevel, threadID );
    bool isVoidOrEmpty=    prefix.IsType<void>()
                        || prefix.IsNull()
                        || ( prefix.IsArray() && !prefix.UnboxLength() );

    Box* previousLogable=  !isVoidOrEmpty ? impl->scopePrefixes.Store( impl->newPO<PrefixLogable>( impl->poolAllocator, prefix ) )
                                          : impl->scopePrefixes.Remove( nullptr );


    BoxesMA& logables= acquireInternalLogables(impl);
    logables.Add( "Object ");
    Verbosity intMsgVerbosity= Verbosity::Info;
    if ( !isVoidOrEmpty ) {
        logables.Add( prefix, " added as prefix logable for {}.", (scope + pathLevel) );

        if ( previousLogable  != nullptr ) {
            if ( previousLogable->Call<FEquals>( prefix )  ) {
                logables.Add(" (Same as before.)");
                intMsgVerbosity= Verbosity::Verbose;
            }
            else
                logables.Add(" Replacing previous {}.", *previousLogable );
        }
    } else {
        if ( previousLogable  != nullptr )
            logables.Add( "{!Q} removed from list of prefix logables for {}.", *previousLogable);
        else {
            logables.Add( "<nullptr> given but no prefix logable to remove for {}.");
            intMsgVerbosity= Verbosity::Warning;
        }
        logables.Add( scope + pathLevel );
    }

    logInternal( impl, intMsgVerbosity, "PFX", logables );

    // it is on us to delete the previous one
    if ( previousLogable != nullptr )
        impl->deletePO(static_cast<PrefixLogable*>( previousLogable ));
}


void LI::SetPrefix( LoxImpl*        impl  , const Box&      prefix,
                    const NString&  domain, lang::Inclusion otherPLs ) {

    ASSERT_ACQUIRED

    Domain* dom= evaluateResultDomain( impl, domain );

    bool isVoidOrEmpty=    prefix.IsType<void>()
                        || prefix.IsNull()
                        || ( prefix.IsArray() && !prefix.UnboxLength() );

    BoxesMA& logables= acquireInternalLogables(impl);
    Verbosity intLogVerbosity= Verbosity::Info;
    PrefixLogable* removedLogable= nullptr;

    if ( !isVoidOrEmpty ) {
        // create logable: if String* type, then copy the string. We are responsible, then.
        logables.Add( "Object {} added as prefix logable for ", prefix );

        dom->PrefixLogables.emplace_back( impl->newPO<PrefixLogable>( impl->poolAllocator, prefix ), otherPLs );
    } else {
        auto cntPLs=  dom->PrefixLogables.size();
        if ( cntPLs > 0 ) {
            removedLogable= dom->PrefixLogables.back().first;
            dom->PrefixLogables.pop_back();
            logables.Add( "Object {} removed from list of prefix logables for",
                          *static_cast<Box*>(removedLogable)  );
        } else {
            logables.Add( "No prefix logables to remove for" );
            intLogVerbosity= Verbosity::Warning;
    }   }

    logables.Add(" domain {!Q'}.", dom->FullPath);
    logInternal( impl, intLogVerbosity, "PFX", logables );

    if( removedLogable )
        impl->deletePO(removedLogable);
}


#if defined (__GLIBCXX__) || defined(_LIBCPP_VERSION) || defined(__APPLE__)  || defined(__ANDROID_NDK__)
void LI::SetStartTime(LoxImpl* impl,  time_t startTime, const NString& loggerName )
{
    TickConverter converter;
    SetStartTime( impl, converter.ToTicks( DateTime::FromEpochSeconds( startTime ) ), loggerName );
}

#elif defined( _WIN32 )
    void LI::SetStartTime(LoxImpl* impl,  const FILETIME& startTime, const NString& loggerName )
    {
        TickConverter converter;
        SetStartTime( impl, converter.ToTicks( DateTime::FromFileTime( startTime ) ), loggerName );
    }
#else
    #pragma message "Unknown Platform in file: " __FILE__ )
#endif

void LI::SetStartTime(LoxImpl* impl,  Ticks startTime, const NString& loggerName ) {
    ASSERT_ACQUIRED

    bool foundOne= false;
    for( int loggerNo= 0; loggerNo < impl->domains->CountLoggers(); ++loggerNo ) {
        // request logger only from main domain tree
        Logger* logger= impl->domains->GetLogger( loggerNo );
        if( loggerName.IsNotEmpty() && !loggerName.Equals<NC, lang::Case::Ignore>( logger->GetName()) )
            continue;
        foundOne= true;

        // log info on this
        BoxesMA& logables= acquireInternalLogables(impl);
        logables.Add( "Logger {!Q}: Start time set to ", logger->GetName() );
        if ( !startTime.IsSet() ) {
            startTime= Ticks::Now();
            logables.Add( "'now'" );
        } else {
            DateTime asDateTime;
            TextLogger* asTextLogger= dynamic_cast<TextLogger*>(logger);
            if( asTextLogger != nullptr )
                asDateTime= asTextLogger->DateConverter.ToDateTime( startTime );
            else
                asDateTime= TickConverter().ToDateTime( startTime );
            logables.Add( "{:yyyy-MM-dd HH:mm:ss}", asDateTime );
        }
        // do
        logger->TimeOfCreation.SetAs( startTime );
        logger->TimeOfLastLog .SetAs( startTime );

        logInternal( impl, Verbosity::Info, "LGR", logables );
    }

    if ( loggerName.IsNotEmpty() && !foundOne ) {
        BoxesMA& logables= acquireInternalLogables(impl);
        logables.Add( "Logger {!Q}: not found. Start time not set.", loggerName );
        logInternal( impl, Verbosity::Error, "LGR", logables );
        return;
}   }


void LI::MapThreadName(LoxImpl* impl,  const String& threadName, threads::ThreadID id ) {
    #if !ALIB_SINGLE_THREADED

        ASSERT_ACQUIRED

        // get current thread id
        String origThreadName;
        if ( id == 0 ) {
            Thread*         t= Thread::GetCurrent();
            id=             t->GetID();
            origThreadName= t->GetName();
        }
        else
            origThreadName= nullptr;

        // add entry
        impl->scopeInfo.threadDictionary.EmplaceOrAssign(id, threadName);

        // log info on this
        BoxesMA& logables= acquireInternalLogables(impl);
        logables.Add( "Mapped thread ID {} to {!Q}.", id, threadName);
        if ( origThreadName.IsNotEmpty() )
            logables.Add(" Original thread name: {!Q}.", origThreadName );
        logInternal( impl, Verbosity::Info, "THR", logables );
    #else
    (void) impl;
    (void) threadName;
    (void) id;
    #endif
}

void LI::once(   LoxImpl* impl,
                 const NString& domain,   Verbosity verbosity,
                 const Box&     logable,
                 const String&  pGroup,
                 Scope          scope,
                 int            quantity                        ) {
    int pathLevel= checkScopeInformation( impl, scope, "DMN" );
    if( pathLevel < 0 )
        return;

    // We need a group. If none is given, there are two options:
    NString512 group(pGroup);
    bool groupWasEmtpy= group.IsEmpty();
    if ( groupWasEmtpy ) {
        // GLOBAL scope: exact code line match
        if ( scope == Scope::Global ) {
            scope= Scope::Filename;
            group._('#')._( impl->scopeInfo.GetLineNumber() );
        }

        // not GLOBAL scope: Unique group per Scope
        else
            group._( impl->noKeyHashKey );
    }

    // get the store
    impl->scopeLogOnce.InitAccess( scope, pathLevel, UNDEFINED_THREAD );

    SSMap<int>* map= impl->scopeLogOnce.Get();
    if( map == nullptr ) {
        map= impl->newPO<SSMap<int>>(impl->poolAllocator);
        impl->scopeLogOnce.Store( map );
    }

    // create map entry (if not created yet)
    auto it=  map->Find( group );
    if (it == map->end() )
        it=   map->InsertUnique( std::make_pair( NString(impl->poolAllocator, group), 0) );

    // log Once
    if ( quantity >= 0 ) {
        if ( it->second < quantity ) {
            ++it->second;

            // do the log
            GetLogableContainer(impl) .Add( std::forward<const Box&>( logable ) );
            Entry( impl, domain, verbosity );

            // log info if this was the last time
            if( it->second == quantity ) {
                BoxesMA& logables= acquireInternalLogables(impl);
                logables.Add( "Once() reached limit of {} logs. No further logs for ", quantity );

                if ( groupWasEmtpy )
                    logables.Add( scope == Scope::Global ? Box( "this line" )
                                                         : Box(scope + pathLevel) );
                else {
                    logables.Add( "group {!Q}", group );
                    if ( scope != Scope::Global )
                        logables.Add(" in ", (scope + pathLevel) );
                }
                logables.Add('.');

                logInternal( impl, Verbosity::Info, "", logables );
    }   }   }

    // log Nth
    else {
        if ( it->second++ % -quantity == 0 ) {
            GetLogableContainer(impl) .Add( std::forward<const Box&>( logable ) );
            Entry( impl, domain, verbosity );
}   }   }

void LI::store( LoxImpl* impl, const Box& data,  const NString& pKey,  Scope scope ) {
    // We need a key. If none is given, we use a constant one indicating that storage is
    // associated exclusively with scope
    NString256 key(pKey);
    bool keyWasEmtpy= key.IsEmpty();
    if ( keyWasEmtpy )
        key= impl->noKeyHashKey;

    // get path level
    int pathLevel= 0;
    if ( scope > Scope::Path ) {
        pathLevel= int( scope - Scope::Path );
        scope= Scope::Path;
    }

    // get the store
    impl->scopeLogData.InitAccess( scope, pathLevel, UNDEFINED_THREAD );
    SSMap<Box>* map= impl->scopeLogData.Get();
    if( map == nullptr ) {
        map= impl->newPO<SSMap<Box>>(impl->poolAllocator);
        impl->scopeLogData.Store( map );
    }

    BoxesMA& logables= acquireInternalLogables(impl);

    // create map entry (if not created yet)
    auto it=  map->Find( key );
    if ( !data.IsType<void>() ) {
        bool replacedPrevious= false;
        if ( it == map->end() )
            map->InsertUnique( std::make_pair( NString(impl->poolAllocator, key), data ) );
        else {
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
    else {
        if ( it != map->end() ) {
            auto keyString= it->first;
            map->erase( it );
            if ( map->Size() == 0 ) {
                impl->deletePO(map);
                impl->scopeLogData.Remove( nullptr );
            }
            keyString.Free(impl->poolAllocator);
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


Box LI::retrieve( LoxImpl* impl, const NString& pKey, Scope scope ) {
    // We need a key. If none is given, we use a constant one indicating that storage is
    // associated exclusively with scope
    NString256 key= pKey;
    bool keyWasEmtpy= key.IsEmpty();
    if ( keyWasEmtpy )
        key= impl->noKeyHashKey;

    int pathLevel= 0;
    if ( scope > Scope::Path ) {
        pathLevel= int( scope - Scope::Path );
        scope= Scope::Path;
    }
    
    // get the data (create if not found)
    impl->scopeLogData.InitAccess( scope, pathLevel, UNDEFINED_THREAD );
    Box returnValue;
    SSMap<Box>* map= impl->scopeLogData.Get();
    if( map != nullptr ) {
        auto it=  map->Find( key );
        if ( it != map->end() )
            returnValue= it->second;
    }

    if ( returnValue.IsType<void>() )
        store( impl, Box(), pKey, scope + pathLevel );

    // log info if this was the last time
    BoxesMA& logables= acquireInternalLogables(impl);
    logables.Add( "Data " );

    if ( !keyWasEmtpy )
        logables.Add( " with key {!Q} ", key );
    logables.Add( "in ", (scope + pathLevel), ( !returnValue.IsType<void>() ? " received."
                                                                            : " not found." ) );

    logInternal( impl, Verbosity::Info, "LGD", logables );
    return returnValue;
}


void LI::State( LoxImpl* impl,
                 const NString&   domain,
                 Verbosity        verbosity,
                 const  String&   headLine,
                 StateInfo        flags      ) {
    ASSERT_ACQUIRED

    NAString buf;
    buf.SetBuffer( 2048 );
    if ( headLine.IsNotEmpty() )
        buf._<NC>( headLine ).NewLine();

    GetState( impl, buf, flags );

    GetLogableContainer(impl) .Add( buf );
    Entry( impl, domain, verbosity );
}

BoxesMA&  LI::GetLogableContainer(LoxImpl* impl) {
    auto cntAcquirements= impl->CountAcquirements();
    ALIB_ASSERT_ERROR(   cntAcquirements >= 1, "ALOX", "Lox not acquired." )
    ALIB_ASSERT_WARNING( cntAcquirements <  5, "ALOX", "Logging recursion depth >= 5" )
    while( int(impl->logableContainers.size()) < cntAcquirements )
        impl->logableContainers.emplace_back( impl->monoAllocator().New<BoxesMA>(impl->monoAllocator) );
    BoxesMA& logables= *impl->logableContainers[size_t(cntAcquirements - 1)];
    logables.clear();
    return logables;
}

void LI::Entry(LoxImpl* impl, const NString& domain, Verbosity verbosity ) {
    ASSERT_ACQUIRED

    // auto-initialization of debug loggers
    #if ALOX_DBG_LOG
        if(   impl == Log::Get()->impl
           && impl->domains->CountLoggers() == 0
           && Log::DebugLogger == nullptr )
            Log::AddDebugLogger( Log::Get() );
    #endif

    ALIB_ASSERT_ERROR(ALOX.IsBootstrapped(), "ALOX", "ALox (ALib) was not properly bootstrapped." )

    ++impl->CntLogCalls;

    if ( impl->domains->CountLoggers() == 0 )
        return;

    log( impl,
         evaluateResultDomain( impl, domain ),
         verbosity,
         *impl->logableContainers[size_t(impl->CountAcquirements() - 1)],
         lang::Inclusion::Include );
}

int LI::IsActive(LoxImpl* impl, Verbosity verbosity, const NString& domain, NAString* resultDomain){
    ASSERT_ACQUIRED

    // auto-initialization of debug loggers
    #if ALOX_DBG_LOG
        if(   impl == Log::Get()->impl
           && impl->domains->CountLoggers() == 0
           && Log::DebugLogger == nullptr )
            Log::AddDebugLogger( Log::Get() );
    #endif

    ALIB_ASSERT_ERROR(ALOX.IsBootstrapped(), "ALOX", "ALox (ALib) was not properly bootstrapped." )

    if ( impl->domains->CountLoggers() == 0 )
        return 0;

    Domain* dom= evaluateResultDomain( impl, domain );
    if ( resultDomain != nullptr )
        resultDomain->_( dom->FullPath );

    int result= 0;
    for ( int i= 0; i < dom->CountLoggers() ; ++i )
        if( dom->IsActive( i, verbosity ) )
            ++result;
    return result;
}

void LI::IncreaseLogCounter( LoxImpl* impl)                                 { ++impl->CntLogCalls; }

void LI::entryDetectDomainImpl(LoxImpl* impl,  Verbosity verbosity ) {
    BoxesMA& logables= *impl->logableContainers[size_t(impl->CountAcquirements() - 1)];
    if ( logables.Size() > 1 && logables[0].IsArrayOf<nchar>() ) {
        NString firstArg= logables[0].Unbox<NString>();

        // accept internal domain at the start
        integer idx= 0;
        if( firstArg.StartsWith( Lox::InternalDomains ) )
            idx+= Lox::InternalDomains.Length();

        // loop over domain and check for illegal characters
        bool illegalCharacterFound= false;
        for( ;  idx< firstArg.Length() ; ++idx ) {
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
        }   }

        if ( illegalCharacterFound ) {
            Entry( impl, nullptr, verbosity );
            return;
        }

        logables.erase( logables.begin() );
        Entry( impl, firstArg, verbosity );
        return;
    }

    Entry( impl, nullptr, verbosity );
}


//##################################################################################################
// internals
//##################################################################################################
Domain* LI::evaluateResultDomain(LoxImpl* impl,  const NString& domainPath ) {
    NString128 resDomain;

    // 0. internal domain tree?
    if ( domainPath.StartsWith( Lox::InternalDomains ) ) {
        // cut "$/" from the path
        resDomain._( domainPath, Lox::InternalDomains.Length() );
        return findDomain( impl, *impl->internalDomains, resDomain );
    }

    // loop over scopes
    NString64 localPath; localPath.DbgDisableBufferReplacementWarning();
    impl->scopeDomains.InitWalk( Scope::ThreadInner,
                           // we have to provide NULL_STRING if parameter is empty
                           domainPath.IsNotEmpty() ? localPath._(domainPath)
                                                   : NULL_NSTRING
                           );
    NString nextDefault;
    while( (nextDefault= impl->scopeDomains.Walk() ).IsNotNull() ) {
        ALIB_ASSERT( nextDefault.IsNotEmpty(), "ALOX" )

        if ( resDomain.IsNotEmpty() )
            resDomain.InsertAt( "/", 0);
        resDomain.InsertAt( nextDefault, 0 );

        // absolute path? That's it
        if ( resDomain.CharAtStart() == Domain::Separator() )
            break;
    }
    return findDomain( impl, *impl->domains, resDomain );
}

void LI::getVerbosityFromConfig(LoxImpl* impl,  Variable& v, Logger*  logger, Domain&  dom ) {
    // get logger number. It may happen that the logger is not existent in this domain tree.
    int loggerNo= dom.GetLoggerNo( logger ) ;
    if ( loggerNo < 0 )
        return;

    auto& cvVerb  = v.Get<alib::lox::CVVerbosities>();

    for (auto it : cvVerb) {
        Tokenizer verbosityTknzr( it, '=' );

        NString256 domainStrBuf;
        Substring domainStrParser= verbosityTknzr.Next();
        if ( domainStrParser.ConsumeString<lang::Case::Ignore>( A_CHAR("INTERNAL_DOMAINS")) ) {
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
        if(     ( searchMode == 0 && dom.FullPath.Equals    <NC ,lang::Case::Ignore>( domainStr )     )
            ||  ( searchMode == 1 && dom.FullPath.StartsWith<CHK,lang::Case::Ignore>( domainStr )     )
            ||  ( searchMode == 2 && dom.FullPath.EndsWith  <CHK,lang::Case::Ignore>( domainStr )     )
            ||  ( searchMode == 3 && dom.FullPath.IndexOf   <CHK,lang::Case::Ignore>( domainStr ) >=0 )
            )
        {
            Verbosity verbosity(Verbosity::Info);
            enumrecords::Parse<Verbosity>(verbosityStr, verbosity );
            dom.SetVerbosity( loggerNo, verbosity, v.GetPriority() );

            // log info on this
            NString512 msg;
            msg._<NC>( "Logger \"" )._<NC>( logger->GetName() ) ._<NC>( "\":" )._(NTab(11 + impl->maxLoggerNameLength))
                ._<NC>( '\'' )._<NC>( dom.FullPath )
                ._( '\'' ).InsertChars(' ', impl->maxDomainPathLength - dom.FullPath.Length() + 1 )
                ._( "= Verbosity::" )
                ._( boxing::MakePair(verbosity, dom.GetPriority( loggerNo )) ).TrimEnd()
                ._<NC>( '.' );

            logInternal( impl, Verbosity::Info, "LGR", msg );
}   }   }

void LI::getDomainPrefixFromConfig(LoxImpl* impl, Domain& dom ) {
    Variable variable= variables::CampVariable(ALOX);
    {ALIB_LOCK_WITH(ALOX.GetConfig())
        const Declaration* decl= Declaration::Get( Variables::PREFIXES);
        decl= ALOX.GetConfig()->StoreDeclaration( decl,
                                                   #if !ALIB_CHARACTERS_WIDE
                                                        GetName( impl )
                                                   #else
                                                        String128( GetName( impl ) )
                                                   #endif
                                                                        );
        if( !variable.Try(decl) )
            return;
    }

    Tokenizer prefixTokOuter;
    prefixTokOuter.Set(variable, ';', true);
    while(prefixTokOuter.HasNext()) {
        Tokenizer prefixTok( prefixTokOuter.Next(), '=' );

        NString128 domainStrBuf;
        Substring domainStrParser= prefixTok.Next();
        if ( domainStrParser.ConsumeString<lang::Case::Ignore>( A_CHAR("INTERNAL_DOMAINS")) ) {
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

        lang::Inclusion otherPLs= lang::Inclusion::Include;
        prefixTokInner.Next();
        if ( prefixTokInner.Actual.IsNotEmpty() )
            enumrecords::ParseEnumOrTypeBool( prefixTokInner.Actual, otherPLs, lang::Inclusion::Exclude, lang::Inclusion::Include );

        int searchMode= 0;
        if ( domainStr.ConsumeChar       ( '*' ) )    searchMode+= 2;
        if ( domainStr.ConsumeCharFromEnd( '*' ) )    searchMode+= 1;
        if(     ( searchMode == 0 && dom.FullPath.Equals    <NC ,lang::Case::Ignore>( domainStr )     )
            ||  ( searchMode == 1 && dom.FullPath.StartsWith<CHK,lang::Case::Ignore>( domainStr )     )
            ||  ( searchMode == 2 && dom.FullPath.EndsWith  <CHK,lang::Case::Ignore>( domainStr )     )
            ||  ( searchMode == 3 && dom.FullPath.IndexOf   <CHK,lang::Case::Ignore>( domainStr ) >=0 )
            )
        {
            dom.PrefixLogables.emplace_back( impl->newPO<PrefixLogable>( impl->poolAllocator, prefixStr ), otherPLs );

            // log info on this
            NString128 msg; msg._<NC>( "String \"" )._<NC>( prefixStr )._<NC>( "\" added as prefix logable for domain \'" )
                               ._<NC>( dom.FullPath )
                               ._<NC>( "\'. (Retrieved from configuration variable \'" )._<NC>(variable)._( "\'.)" );

            logInternal( impl, Verbosity::Info, "PFX", msg );
}   }   }

void LI::getAllVerbosities(LoxImpl* impl, Variable& varVerbosities, Logger*  logger, Domain&  dom) {
    // get verbosity for us
    getVerbosityFromConfig( impl, varVerbosities, logger, dom );

    // loop over all subdomains (recursion)
    for ( Domain& subDomain : dom.SubDomains )
        getAllVerbosities( impl, varVerbosities, logger, subDomain );
}


Domain* LI::findDomain(LoxImpl* impl,  Domain& rootDomain, NString domainPath ) {
    int maxSubstitutions= 10;
    NString128 substPath;
    for(;;) {
        // loop for creating domains, one by one
        Domain* dom= nullptr;
        for(;;) {
            bool wasCreated;
            dom= rootDomain.Find( domainPath, 1, &wasCreated );
            if ( wasCreated ) {
                // get maximum domain path length (for nicer State output only...)
                if ( impl->maxDomainPathLength < dom->FullPath.Length() )
                     impl->maxDomainPathLength=  dom->FullPath.Length();

                // log info on new domain
                BoxesMA& logables= acquireInternalLogables(impl);
                logables.Add( "{!Q} registered.", dom->FullPath );
                logInternal( impl, Verbosity::Info, "DMN", logables );
            }

            // read domain from config
            if ( !dom->ConfigurationAlreadyRead )
            { dom->ConfigurationAlreadyRead= true;

                Box replacements[2];
                for ( int i= 0; i < dom->CountLoggers(); ++i ) {
                    Logger* logger= dom->GetLogger(i);
                    replacements[0]= GetName( impl );
                    replacements[1]= logger->GetName();
                    Variable varVerbosities= variables::CampVariable(ALOX, Variables::VERBOSITY, replacements );
                    if ( varVerbosities.IsDefined() )
                        getVerbosityFromConfig( impl, varVerbosities, logger, *dom );
                }

                getDomainPrefixFromConfig( impl,  *dom );
            }

            if ( wasCreated ) {
                if ( dom->CountLoggers() == 0 )
                    logInternal( impl, Verbosity::Verbose, "DMN", "   No loggers set, yet." );
                else
                    for ( int i= 0; i < dom->CountLoggers(); ++i ) {
                        NString256 msg; msg._("  \"")._( dom->GetLogger(i)->GetName() )._("\": ");
                                        msg.InsertChars( ' ', impl->maxLoggerNameLength  + 6 - msg.Length() );
                                        msg._( dom->FullPath )._(" = " )
                                        ._(boxing::MakePair(dom->GetVerbosity(i), dom->GetPriority(i)));
                        logInternal( impl, Verbosity::Verbose, "DMN", msg );
            }       }
            else
                break;
        }

        // apply domain substitutions
        if( !impl->domainSubstitutions.empty() ) {
            substPath.Reset();
            NSubstring domFullPath= dom->FullPath;
            if ( domFullPath.CharAtStart<NC>() == '$' )
                domFullPath.ConsumeChar();

            while( maxSubstitutions-- > 0  ) {
                // loop over rules
                bool substituted= false;
                for( auto& rule : impl->domainSubstitutions ) {
                    switch( rule.type ) {
                        case DomainSubstitutionRule::Type::StartsWith:
                            if( substPath.IsEmpty() ) {
                                if ( domFullPath.StartsWith( rule.Search ) ) {
                                    substPath._( rule.Replacement )._( domFullPath, rule.Search.Length() );
                                    substituted= true;
                                    continue;
                                }
                            } else {
                                if ( substPath.StartsWith( rule.Search ) ) {
                                    substPath.ReplaceSubstring<NC>( rule.Replacement, 0, rule.Search.Length()  );
                                    substituted= true;
                                    continue;
                            }   }
                        break;

                        case DomainSubstitutionRule::Type::EndsWith:
                            if( substPath.IsEmpty() ) {
                                if ( domFullPath.EndsWith( rule.Search ) ) {
                                    substPath._( domFullPath, 0, domFullPath.Length() - rule.Search.Length() )._( rule.Replacement );
                                    substituted= true;
                                    continue;
                                }
                            } else {
                                if ( substPath.EndsWith( rule.Search ) ) {
                                    substPath.DeleteEnd( rule.Search.Length() )._( rule.Replacement );
                                    substituted= true;
                                    continue;
                            }   }
                        break;


                        case DomainSubstitutionRule::Type::Substring:
                        {
                            if( substPath.IsEmpty() ) {
                                integer idx= domFullPath.IndexOf( rule.Search );
                                if ( idx >= 0 ) {
                                    substPath._( domFullPath, 0, idx )._( rule.Replacement)._( domFullPath, idx + rule.Search.Length() );
                                    substituted= true;
                                    continue; //next rule
                                }
                            } else {
                                integer idx= substPath.IndexOf( rule.Search, 0 );
                                if ( idx >= 0 ) {
                                    substPath.ReplaceSubstring<NC>( rule.Replacement, idx, rule.Search.Length()  );
                                    substituted= true;
                                    continue; //next rule
                        }   }   }
                        break;


                        case DomainSubstitutionRule::Type::Exact:
                        {
                            if( substPath.IsEmpty() ) {
                                if ( domFullPath.Equals<NC>( rule.Search ) ) {
                                    substPath._( rule.Replacement);
                                    substituted= true;
                                    continue; //next rule
                                }
                                if ( domFullPath.CharAtStart<NC>() == '$' ) {
                                    substPath._( rule.Replacement);
                                    substituted= true;
                                    continue; //next rule
                                }
                            } else {
                                if ( substPath.Equals<NC>( rule.Search) ) {
                                    substPath.Reset( rule.Replacement );
                                    substituted= true;
                                    continue; //next rule
                        }   }   }
                        break;

                        default: ALIB_ERROR("ALOX", "Illegal switch state." ) break;
                    } // switch rule type

                }//rules loop

                // stop if non was found
                if( !substituted )
                    break;
            }

            // too many substitutions?
            if ( maxSubstitutions <= 0 && !impl->oneTimeWarningCircularDS ) {
                impl->oneTimeWarningCircularDS= true;
                logInternal( impl, Verbosity::Error, "DMN",
                             "The Limit of 10 domain substitutions was reached. Circular substitution assumed!"
                             " (This error is only reported once!)"   );
            }

            // anything substituted?
            if( substPath.Length() > 0 ) {
                domainPath= substPath;
                continue;
        }   }

        return dom;
}   }

int LI::checkScopeInformation(LoxImpl* impl,  Scope& scope, const NString& internalDomain ) {
    int pathLevel= 0;
    if ( scope > Scope::Path ) {
        pathLevel= int( scope - Scope::Path );
        scope= Scope::Path;
    }

    if (     ( scope == Scope::Path     &&  impl->scopeInfo.GetFullPath().IsEmpty() )
         ||  ( scope == Scope::Filename &&  impl->scopeInfo.GetFileName().IsEmpty() )
         ||  ( scope == Scope::Method   &&  impl->scopeInfo.GetMethod()  .IsEmpty() ) )
    {
        BoxesMA& logables= acquireInternalLogables(impl);
        logables.Add( "Missing scope information. Cant use {}.", (scope + pathLevel) );
        logInternal( impl, Verbosity::Error, internalDomain, logables );
        return -1;
    }
    return pathLevel;
}

bool LI::isThreadRelatedScope(LoxImpl* impl, Scope scope ) {
    // check
    if (    scope == Scope::ThreadOuter
         || scope == Scope::ThreadInner )
        return true;

    BoxesMA& logables= acquireInternalLogables(impl);
    logables.Add( "Illegal parameter, only Scope::ThreadOuter and Scope::ThreadInner allowed."
                  " Given: {}.", scope );
    logInternal( impl, Verbosity::Error, "DMN", logables );

    #if ALIB_DEBUG
        alib::assert::Raise( {  impl->scopeInfo.GetOrigFile(),
                                impl->scopeInfo.GetLineNumber(),
                                impl->scopeInfo.GetMethod(),
        #if !ALIB_SINGLE_THREADED
                                impl->scopeInfo.GetThreadNativeID(),
        #elif ALIB_EXT_LIB_THREADS_AVAILABLE
                                std::thread::id(),
        #endif
                                impl->scopeInfo.GetTypeInfo()
                             },
                             0, "Illegal scope type \"{}\" given. Only Scope::ThreadOuter and "
                                "Scope::ThreadInner allowed.", scope );
    #endif

    return false;
}

void LI::log( LoxImpl*  impl     , Domain*  dom,
              Verbosity verbosity, BoxesMA& logables, lang::Inclusion includePrefixes ) {
    ++dom->CntLogCalls;
    bool logablesCollected= false;
    PrefixLogable marker(impl->poolAllocator, nullptr);
    for ( int i= 0; i < dom->CountLoggers() ; ++i )
        if( dom->IsActive( i, verbosity ) ) {
            // lazily collect objects once an active logger is found
            if ( !logablesCollected ) {
                logablesCollected= true;
                impl->scopePrefixes.InitWalk( Scope::ThreadInner, &marker );
                const Box* next;
                int userLogablesSize= int( logables.Size() );
                int threadInnersSize= -1;

                while( (next= impl->scopePrefixes.Walk() ) != nullptr ) {
                    if( next != &marker ) {
                        // this is false for internal domains (only domain specific logables are added there)
                        if ( includePrefixes == lang::Inclusion::Include ) {
                            // after marker is read, logables need to be prepended. This is checked below
                            // using "qtyThreadInners < 0"
                            if ( next->IsType<BoxesMA*>() ) {
                                auto* boxes= next->Unbox<BoxesMA*>();
                                for (auto pfxI= boxes->Size() - 1 ; pfxI >= 0 ; --pfxI )
                                    logables.emplace( logables.begin() + ( threadInnersSize < 0 ? userLogablesSize : 0 ),
                                                    (*boxes)[size_t(pfxI)] );
                            }
                            else if ( next->IsType<Boxes*>() ) {
                                auto* boxes= next->Unbox<Boxes*>();
                                for (auto pfxI= boxes->Size() - 1 ; pfxI >= 0 ; --pfxI )
                                    logables.emplace( logables.begin() + ( threadInnersSize < 0 ? userLogablesSize : 0 ),
                                                    (*boxes)[size_t(pfxI)] );
                            }
                            else if ( next->IsType<BoxesPA*>() ) {
                                auto* boxes= next->Unbox<BoxesPA*>();
                                for (auto pfxI= boxes->Size() - 1 ; pfxI >= 0 ; --pfxI )
                                    logables.emplace( logables.begin() + ( threadInnersSize < 0 ? userLogablesSize : 0 ),
                                                    (*boxes)[size_t(pfxI)] );
                            }
                            else
                                logables.emplace( logables.begin() + ( threadInnersSize < 0 ? userLogablesSize : 0 ), *next );
                    }   }

                    // was this the actual? then insert domain-associated logables now
                    else {
                        bool excludeOthers= false;
                        threadInnersSize= int( logables.Size() ) - userLogablesSize;
                        Domain* pflDom= dom;
                        while ( pflDom != nullptr ) {
                            for( auto it= pflDom->PrefixLogables.rbegin() ; it != pflDom->PrefixLogables.rend() ; ++it ) {
                                // a list of logables? Copy them
                                PrefixLogable& prefix= *it->first;
                                if ( prefix.IsType<Boxes*>() ) {
                                    auto* boxes= prefix.Unbox<Boxes*>();
                                    for (auto pfxI= boxes->Size() - 1 ; pfxI >= 0 ; --pfxI )
                                        logables.emplace( logables.begin(),
                                                          (*boxes)[size_t(pfxI)] );
                                }
                                else if ( prefix.IsType<BoxesMA*>() ) {
                                    auto* boxes= prefix.Unbox<BoxesMA*>();
                                    for (auto pfxI= boxes->Size() - 1 ; pfxI >= 0 ; --pfxI )
                                        logables.emplace( logables.begin(),
                                                          (*boxes)[size_t(pfxI)] );
                                }
                                else if ( prefix.IsType<BoxesPA*>() ) {
                                    auto* boxes= prefix.Unbox<BoxesPA*>();
                                    for (auto pfxI= boxes->Size() - 1 ; pfxI >= 0 ; --pfxI )
                                        logables.emplace( logables.begin(),
                                                          (*boxes)[size_t(pfxI)] );
                                }
                                else
                                    logables.emplace( logables.begin(), prefix );


                                if ( it->second == lang::Inclusion::Exclude ) {
                                    excludeOthers= true;
                                    break;
                            }   }

                            pflDom= excludeOthers ? nullptr :  pflDom->Parent;
                        }

                        // found a stoppable one? remove those from thread inner and break
                        if (excludeOthers) {
                            for ( int ii= 0; ii < threadInnersSize ; ++ii )
                                logables.pop_back();
                            break;
                }   }   }
            } // end of collection

            Logger* logger= dom->GetLogger(i);
            { ALIB_LOCK_RECURSIVE_WITH(*logger)
                ++logger->CntLogs;
                logger->Log( *dom, verbosity, logables, impl->scopeInfo );
                logger->TimeOfLastLog= Ticks::Now();
}       }   }

BoxesMA&  LI::acquireInternalLogables(LoxImpl* impl) {
    if( integer(impl->internalLogables.size()) == impl->internalLogRecursionCounter ) {
        BoxesMA* newLogables= impl->monoAllocator().New<BoxesMA>(impl->monoAllocator);
        impl->internalLogables.emplace_back( newLogables );
    }

    return *impl->internalLogables[size_t(impl->internalLogRecursionCounter++)];
}

void LI::logInternal(LoxImpl* impl,  Verbosity verbosity, const NString& subDomain, BoxesMA& msg ) {
    ALIB_ASSERT_ERROR(ALOX.IsBootstrapped(), "ALOX", "ALox (ALib) was not properly bootstrapped." )
    log( impl, findDomain( impl, *impl->internalDomains, subDomain ), verbosity, msg, lang::Inclusion::Exclude );

    impl->internalLogables[size_t(--impl->internalLogRecursionCounter)]->clear();
}

void LI::logInternal( LoxImpl*       impl     , Verbosity      verbosity,
                      const NString& subDomain, const NString& msg        ) {
    BoxesMA& logables= acquireInternalLogables(impl);
    logables.Add( msg );
    logInternal( impl, verbosity, subDomain, logables );
}

#if !DOXYGEN

namespace {
void getStateDomainRecursive( Domain& domain, integer maxDomainPathLength, NAString& buf );
void getStateDomainRecursive( Domain& domain, integer maxDomainPathLength, NAString& buf ) {
    integer reference= buf.Length();
    buf._("  "); domain.ToString( buf );
    integer idx= buf.IndexOf( '[', reference );
    buf.InsertChars( ' ', maxDomainPathLength + 5 - idx + reference, idx);
    buf.NewLine();

    // loop over all subdomains (recursion)
    for ( Domain& subDomain : domain.SubDomains )
        getStateDomainRecursive( subDomain, maxDomainPathLength, buf );
}

void getStateDomainsWithDiffVerb( Domain& dom, int loggerNo, std::vector<Domain*>& results );
void getStateDomainsWithDiffVerb( Domain& dom, int loggerNo, std::vector<Domain*>& results ) {
    if (    dom.Parent == nullptr
        ||  dom.Parent->GetVerbosity(loggerNo) != dom.GetVerbosity(loggerNo) )
        results.emplace_back( &dom );

    for( auto& it : dom.SubDomains )
        getStateDomainsWithDiffVerb( it, loggerNo, results );
}

void getStateCollectPrefixes( Domain& dom, integer indentSpaces, NAString& target );
void getStateCollectPrefixes( Domain& dom, integer indentSpaces, NAString& target ) {
    AString buffer;
    for ( auto& pfl : dom.PrefixLogables ) {
        buffer.InsertChars( ' ', indentSpaces );
        buffer << '"';
        integer actLen= buffer.Length();
        buffer._( *static_cast<Box*>(pfl.first) );
        ESC::ReplaceToReadable( buffer, actLen );
        buffer << Escape( lang::Switch::On, actLen );
        buffer << '"';
        if ( pfl.second == lang::Inclusion::Exclude )
            buffer._<NC>( " (Excl.)" );
        buffer._<NC>( Tab( 25, -1 ) );
        buffer._<NC>( "<domain>           [" )._<NC>( dom.FullPath )._<NC>(']').NewLine();
    }
    target << buffer;

    for( auto& subDom : dom.SubDomains )
        getStateCollectPrefixes( subDom, indentSpaces, target );
}

} // anonymous namespace

#endif // !DOXYGEN


void LI::GetState( LoxImpl* impl, NAString& buf, StateInfo flags ) {
    ASSERT_ACQUIRED

    ScopeDump scopeDump( IF_ALIB_THREADS( impl->scopeInfo.threadDictionary, )
                         impl->noKeyHashKey, buf );

    if ( HasBits( flags, StateInfo::CompilationFlags ) ) {
        buf._<NC>( "ALib Version:      "    )._<NC>( alib::VERSION)
           ._<NC>(" (Rev. ")                 ._       ( alib::REVISION)._(')').NewLine();
        buf._<NC>( "ALib Compiler Symbols:" ).NewLine();
        {
            for( auto& p : alib::COMPILATION_FLAG_MEANINGS ) {
                buf << "  " << NField( p.Name, 41, lang::Alignment::Left ) << ':'
                    << (alib::COMPILATION_FLAGS.bits[p.Flag/8] & (1 << p.Flag % 8)  ? " On" : " Off")
                    << NEW_LINE;
            }

        }

        buf.NewLine();
    }

    // basic lox info
    if( alib::HasBits( flags,  StateInfo::Basic ) )
        buf._<NC>( "Name:            \"" )._( impl->scopeInfo.GetLoxName() )._('\"').NewLine();

    if( HasBits( flags,  StateInfo::Version ) ) {
        buf._<NC>( "Version:         " )._<NC>( alib::VERSION)
           ._<NC>(" (Rev. "            )._(        alib::REVISION)._(')').NewLine();
    }

    if( HasBits( flags,  StateInfo::Basic ) )
        buf._<NC>( "#Log Calls:      " )._<NC>( impl->CntLogCalls     ).NewLine();

    if(    HasBits( flags,  StateInfo::Basic )
        || HasBits( flags,  StateInfo::Version )  )
        buf.NewLine();

    //  source path trim info
    if( HasBits( flags,  StateInfo::SPTR ) ) {
        buf._<NC>( "Source Path Trimming Rules: " ).NewLine();

        int cnt= 0;
        // do 2 times, 0== global list, 1 == local list
        for( int trimInfoNo= 0; trimInfoNo < 2 ; ++trimInfoNo ) {
            // choose local or global list
            std::vector<ScopeInfo::SourcePathTrimRule>* trimInfoList=
                       trimInfoNo == 0   ? &ScopeInfo::GlobalSPTRs
                                         : &impl->scopeInfo.LocalSPTRs;


            // loop over trimInfo
            for ( auto& ti : *trimInfoList ) {
                ++cnt;
                buf._<NC>( trimInfoNo == 0 ? "  Global: "
                                         : "  Local:  " );
                buf._<NC>( ti.IsPrefix ?  "\"" : "\"*");
                buf._<NC>( ti.Path )._<NC>( "\", " );
                buf._<NC>( ti.IncludeString );
                if ( ti.TrimOffset != 0 )
                    buf._<NC>( ti.Path )._<NC>( "\", Offset: " )._<NC>( ti.TrimOffset );
                buf._<NC>( ", Priority: " )._( ti.Priority );
                buf.NewLine();
        }   }


        if ( cnt == 0 )
            buf._<NC>("  <no rules set>" ).NewLine();
        buf.NewLine();
    }

    //  domain substitutions
    if( HasBits( flags,  StateInfo::DSR ) ) {
        buf._<NC>( "Domain Substitution Rules: " ).NewLine();
        if( !impl->domainSubstitutions.empty() ) {
            // get size
            integer maxWidth= 0;
            for ( auto& it : impl->domainSubstitutions )
                if ( maxWidth < it.Search.Length() )
                     maxWidth = it.Search.Length();
            maxWidth+= 2;

            // write
            for ( auto& it : impl->domainSubstitutions ) {
                buf._<NC>( "  " );
                if (    it.type == DomainSubstitutionRule::Type::EndsWith
                     || it.type == DomainSubstitutionRule::Type::Substring )
                    buf._<NC>( '*' );

                buf._<NC>( it.Search );
                if (    it.type == DomainSubstitutionRule::Type::StartsWith
                     || it.type == DomainSubstitutionRule::Type::Substring )
                    buf._<NC>( '*' );

                buf._<NC>( NTab( maxWidth, -1, 0 ) )
                   ._<NC>( " -> " )
                   ._<NC>( it.Replacement );
                buf.NewLine();
        }   }
        else
            buf._<NC>("  <no rules set>" ).NewLine();
        buf.NewLine();
    }

    // Log Once Counters
    if( HasBits( flags,  StateInfo::Once ) ) {
        buf._<NC>( "Once() Counters: " ).NewLine();
        if ( scopeDump.writeStoreMap( &impl->scopeLogOnce ) == 0 )
            buf._<NC>("  <no Once() counters set>" ).NewLine();
        buf.NewLine();
    }

    // Log Data
    if( HasBits( flags,  StateInfo::LogData ) ) {
        buf._<NC>( "Log Data: " ).NewLine();
        if ( scopeDump.writeStoreMap( &impl->scopeLogData ) == 0 )
            buf._<NC>("  <no data objects stored>" ).NewLine();
        buf.NewLine();
    }

    // Prefix Logables
    if( HasBits( flags,  StateInfo::PrefixLogables ) ) {
        buf._<NC>( "Prefix Logables: " ).NewLine();
        integer oldLength= buf.Length();
        scopeDump.writeStore( &impl->scopePrefixes, 2 );
        getStateCollectPrefixes( *impl->domains, 2, buf );
        if ( oldLength == buf.Length() )
            buf._<NC>("  <no prefix logables set>" ).NewLine();
        buf.NewLine();
    }

    // thread mappings
    if( HasBits( flags,  StateInfo::ThreadMappings ) ) {
        #if !ALIB_SINGLE_THREADED
            buf._<NC>( "Named Threads:   " ).NewLine();
            if ( impl->scopeInfo.threadDictionary.Size() == 0 )
                buf._<NC>("  <no thread name mappings set>" ).NewLine();
            else
                for ( auto& pair : impl->scopeInfo.threadDictionary ) {
                    buf._<NC>( "  " ) << NField( String32() << '(' << pair.first << "):", 7, lang::Alignment::Left )
                                    << '\"' << pair.second << '\"';
                    buf.NewLine();
                }
            buf.NewLine();
        #endif
    }

    // Scope Domains
    if( HasBits( flags,  StateInfo::ScopeDomains ) ) {
        buf._<NC>( "Scope Domains: " ).NewLine();
        if ( scopeDump.writeStore( &impl->scopeDomains, 2 ) == 0 )
            buf._<NC>("  <no scope domains set>" ).NewLine();
        buf.NewLine();
    }

    // Loggers
    if( HasBits( flags,  StateInfo::Loggers ) ) {
        TickConverter dateTimeConverter;
        std::vector<Domain*> domainsWithDiffVerb;
        for (int treeNo= 0; treeNo < 2; ++treeNo ) {
            int cnt= 0;
            Domain* domTree;
            if( treeNo==0 ) {
                 domTree= impl->domains;
                 buf._<NC>( "Loggers:" ).NewLine();
            } else {
                 domTree= impl->internalDomains;
                 buf._<NC>( "Loggers on Internal Domains:" ).NewLine();
            }

            for ( int loggerNo= 0; loggerNo< domTree->CountLoggers(); ++loggerNo ) {
                ++cnt;
                String64 as64;
                CalendarDateTime ct(lang::Initialization::Suppress);

                Logger* logger= domTree->GetLogger(loggerNo);
                buf._<NC>( "  "  )._<NC>( *logger  ).NewLine();
                buf._<NC>( "    Lines logged:  "     )._<NC>( logger->CntLogs                                   ).NewLine();

                ct.Set( dateTimeConverter.ToDateTime(logger->TimeOfCreation) );
                buf._<NC>( "    Creation time: "     )._<NC>( ct.Format( A_CHAR("yyyy-MM-dd HH:mm:ss"), as64.Reset()) ).NewLine();

                ct.Set( dateTimeConverter.ToDateTime(logger->TimeOfLastLog) );
                buf._<NC>( "    Last log time: "     )._<NC>( ct.Format( A_CHAR("yyyy-MM-dd HH:mm:ss"), as64.Reset()) ).NewLine();

                domainsWithDiffVerb.clear();
                getStateDomainsWithDiffVerb( *domTree, loggerNo, domainsWithDiffVerb);
                for ( Domain* dom : domainsWithDiffVerb ) {
                    buf._<NC>("    ")
                       ._(  dom == *domainsWithDiffVerb.begin() ? "Verbosities:   "
                                                                : "               " );

                    integer tabRef= buf.Length();
                    buf << dom->FullPath << NTab( impl->maxDomainPathLength +1, tabRef);

                    buf << "= " << boxing::MakePair(dom->GetVerbosity( loggerNo ), dom->GetPriority(loggerNo) )
                        << NEW_LINE;
            }   }
            if ( cnt == 0 )
                buf._<NC>("  <no loggers attached>" ).NewLine();
            buf.NewLine();
    }   }

    // Internal Domains
    if( HasBits( flags,  StateInfo::InternalDomains ) ) {
        buf._<NC>( "Internal Domains:" ).NewLine();
        getStateDomainRecursive( *impl->internalDomains, impl->maxDomainPathLength, buf );
        buf.NewLine();
    }

    // Domains
    if( HasBits( flags,  StateInfo::Domains ) ) {
        buf._<NC>( "Domains:" ).NewLine();
        getStateDomainRecursive( *impl->domains        , impl->maxDomainPathLength, buf );
        buf.NewLine();
}   }

} // namespace alib::lox[::detail]



}} // namespace [alib::lox]
#   include "ALib.Lang.CIMethods.H"

#undef UNDEFINED_THREAD
#undef ASSERT_ACQUIRED
