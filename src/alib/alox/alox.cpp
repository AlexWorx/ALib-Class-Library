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
#include "ALib.Compatibility.StdStrings.H"

//============================================== Module ============================================
#if ALIB_C20_MODULES
    module ALib.ALox;
    import   ALib.Lang;
    import   ALib.Containers.List;
    import   ALib.Monomem;
    import   ALib.Boxing;
    import   ALib.EnumRecords;
    import   ALib.Strings;
    import   ALib.EnumRecords.Bootstrap;
    import   ALib.Variables;
    import   ALib.Camp;
    import   ALib.Camp.Base;
    import   ALib.ALox.Impl;
#else
#   include "ALib.Lang.H"
#   include "ALib.Containers.List.H"
#   include "ALib.Monomem.H"
#   include "ALib.Boxing.H"
#   include "ALib.EnumRecords.Bootstrap.H"
#   include "ALib.Variables.H"
#   include "ALib.Camp.H"
#   include "ALib.Camp.Base.H"
#   include "ALib.ALox.H"
#   include "ALib.ALox.Impl.H"
#endif
//========================================== Implementation ========================================
#if !DOXYGEN
namespace alib::lox::detail {
namespace { ListMA<Lox*>      loxes(monomem::GLOBAL_ALLOCATOR); }
integer dbgCountLoxes()                                                     { return loxes.size(); }
void    shutdownLoxes() {
    while ( loxes.IsNotEmpty() )
        loxes.back()->~Lox();
    loxes.Reset();
}

}
#endif

namespace alib::lox {

#   include "ALib.Lang.CIFunctions.H"

//##################################################################################################
// Lox management
//##################################################################################################
#if !DOXYGEN

#if ALOX_DBG_LOG
    Lox*                DEBUG_LOX = nullptr;  // will be created in ALoxCamp::Bootstrap
#endif

#endif


// The lox singletons for debug and release logging
Lox*     Lox::Get( const NString& name, lang::CreateIfNotExists create ) {
    ALIB_LOCK_RECURSIVE_WITH(monomem::GLOBAL_ALLOCATOR_LOCK)

    // search
    for( auto* it : detail::loxes )
        if( it->GetName().Equals<CHK, lang::Case::Ignore>( name ) )
            return it;


    // create?
    if ( create == lang::CreateIfNotExists::Yes ) {
        Lox* newLox= new Lox ( name, false );
        detail::loxes.emplace_back( newLox );
        return newLox;
    }

    // not found
    return nullptr;
}

void     Lox::Register( Lox* lox, lang::ContainerOp operation ) {
    ALIB_LOCK_RECURSIVE_WITH(monomem::GLOBAL_ALLOCATOR_LOCK)

    // check
    if ( lox == nullptr ) {
        ALIB_ERROR( "ALOX", "Nullptr given" )
        return;
    }

    // remove
    if( operation == lang::ContainerOp::Remove ) {
        for( auto search= detail::loxes.begin() ; search != detail::loxes.end() ; ++search )
            if ( *search == lox ) {
                (void) detail::loxes.erase( search );
                return;
            }
        ALIB_WARNING( "ALOX", "Given lox named \"{}\" could not be found for removal.",
                      lox != nullptr ? lox->GetName() : "<null>"          )
    }

    // insert
    else {
        for( auto* it : detail::loxes )
            if( it->GetName().Equals<NC>( lox->GetName() ) ) {
                ALIB_ERROR( "ALOX", "Given lox named \"{}\" was already registered. "
                                    "Registration ignored.", lox->GetName() )
                return;
            }
        detail::loxes.emplace_back( lox );
}}


Lox::Lox(const NString& name, bool doRegister ) {
    impl= detail::LI::Construct( name );

    if( doRegister )
        Register( this, lang::ContainerOp::Insert );
}

Lox::~Lox() {
    if( Get( GetName(), lang::CreateIfNotExists::No  ) == this )
        Register( this, lang::ContainerOp::Remove );

    detail::LI::Destruct( impl );
}


//##################################################################################################
// Static methods of Lox
//##################################################################################################
TextLogger* Lox::CreateConsoleLogger(const NString& name) {
    //--- check configuration setting "CONSOLE_TYPE"  ---
    Variable variable= variables::CampVariable(ALOX, Variables::CONSOLE_TYPE );
    if(variable.Define())
        variable= String( A_CHAR("Default") );

    Substring val   = variable;
    val.Trim();
    if( val.IsEmpty() ||
        val.Equals<NC, lang::Case::Ignore>( A_CHAR("Default") ) ) goto DEFAULT;

    if( val.Equals<NC, lang::Case::Ignore>( A_CHAR("Plain")   ) ) return new ConsoleLogger    ( name );
    if( val.Equals<NC, lang::Case::Ignore>( A_CHAR("Ansi")    ) ) return new AnsiConsoleLogger( name );

    if( val.Equals<NC, lang::Case::Ignore>( A_CHAR("Windows") ) )
                                                                 #if defined( _WIN32 )
                                                                     return new WindowsConsoleLogger( name );
                                                                 #else
                                                                     goto DEFAULT;
                                                                 #endif
    ALIB_WARNING( "ALOX", "Unrecognized value in config variable \"{}\" = \"{}\".",
                     variable, variable.GetString() )

    DEFAULT:
    if( variable.Define(variables::Priority::Standard) )
        variable.GetString().Reset(A_CHAR("Default"));

    #if defined( _WIN32 )
        // if there is no console window we do not do colors
        if ( !BASECAMP.HasConsoleWindow )
            return new ConsoleLogger( name );
        else
            return new WindowsConsoleLogger( name );
    #else
        return new AnsiConsoleLogger( name );
    #endif
}


#if ALOX_DBG_LOG
//##################################################################################################
// Auto detection of DEBUG environment
//##################################################################################################
TextLogger*   Log::DebugLogger= nullptr;
TextLogger*   Log::IDELogger  = nullptr;

void Log::AddDebugLogger( Lox* lox ) {
    static bool recursion= false;
    if( recursion )
        return;
    recursion= true;

    // block recursion caused by log operations in this code
    if ( DebugLogger != nullptr ) {
        ALIB_WARNING( "ALOX", "Log::AddDebugLogger(): called twice." )
        recursion= false;
        return;
    }
    DebugLogger= reinterpret_cast<decltype(DebugLogger)>(-1);

    // add a VStudio logger if this is a VStudio debug session
    #if defined(_MSC_VER) && ALIB_DEBUG
        if( BASECAMP.IsDebuggerPresent() ) {
            Variable variable= variables::CampVariable( ALOX, Variables::NO_IDE_LOGGER );
            bool createIDELogger= variable.IsNotDefined() || (variable.GetBool() == false);

            if(createIDELogger) {
                IDELogger= new VStudioLogger("IDE_LOGGER");

                // add logger
                lox->SetVerbosity( IDELogger, Verbosity::Verbose, "/"  );
                lox->SetVerbosity( IDELogger, Verbosity::Warning, Lox::InternalDomains );
    }       }
    #endif

    // add a default console logger
    DebugLogger= Lox::CreateConsoleLogger("DEBUG_LOGGER");

    // add logger by setting verbosities
    lox->SetVerbosity( DebugLogger, Verbosity::Verbose );
    lox->SetVerbosity( DebugLogger, Verbosity::Warning, Lox::InternalDomains );

    // check various variables, if existed already externally. If not, create them empty or
    // with debug defaults (only done here, namely for debug logger)
    {ALIB_LOCK_WITH(ALOX.GetConfig())
        // Verbosity: If not, set 'ExportAll' flag
        Variable variable= variables::CampVariable( ALOX );
        Box replacements[2]= { "LOG",  "DEBUG_LOGGER"  };
        variable.Declare( Variables::VERBOSITY, replacements );
        if( variable.IsNotDefined() ) {
            (void) variable.Define();
            variable.Get<CVVerbosities>().ExportAll= true;
        }

        variable.Declare( Variables::SPTR_LOX           , "LOG" );   (void) variable.Define();
        variable.Declare( Variables::DOMAIN_SUBSTITUTION, "LOG" );   (void) variable.Define();
        variable.Declare( Variables::PREFIXES           , "LOG" );   (void) variable.Define();
        variable.Declare( Variables::DUMP_STATE_ON_EXIT , "LOG" );   (void) variable.Define();
        if( dynamic_cast<AnsiConsoleLogger*>(DebugLogger) != nullptr )
            variable.Declare( Variables::CONSOLE_LIGHT_COLORS );   (void) variable.Define();
    }

    // set ALib's assertion plugin with one that uses ALox
    SetALibAssertionPlugin( lox );

    recursion= false;
}

void Log::RemoveDebugLogger( Lox* lox ) {
    // remove ALox specific assertion plugin of ALib
    SetALibAssertionPlugin( nullptr );

    // remove debug logger(s)
    ALIB_ASSERT_WARNING( DebugLogger != nullptr, "ALOX",
                         "Log::RemoveDebugLogger(): no debug logger to remove." )

    if ( DebugLogger != nullptr ) {
        lox->RemoveLogger( DebugLogger );

        delete DebugLogger;
        DebugLogger= nullptr;
    }

    #if defined(_WIN32) && ALIB_DEBUG
        if ( IDELogger != nullptr ) {
            lox->RemoveLogger( IDELogger );

            delete IDELogger;
            IDELogger= nullptr;
        }
    #endif
}
#endif // ALOX_DBG_LOG


//##################################################################################################
// ALoxAssertionPlugin
//##################################################################################################
#if ALIB_DEBUG

//==================================================================================================
/// This function will be set to global pointer \alib{assert::PLUGIN} when calling
/// method \alib{lox;Log::AddDebugLogger}.<br>
/// If no debug-logging is used, or method \b AddDebugLogger is not used, then
/// this function can also be used with a different \b Lox and explicitly activated using
/// the static method \alib{lox;Log::SetALibAssertionPlugin}.
/// Uses internal domain <b>"/ALIB"</b> for logging, respectively to what the global variable
/// \ref ALOX_ASSERTION_PLUGIN_DOMAIN_PREFIX is set.
/// @param ci      Information about the scope of invocation.
/// @param type    The type of the message. As a convention, \c 0 is an assertion, \c 1 is a
///                warning, \c 2 is an info message, \c 3 or above are a verbose messages.
/// @param domain  The domain of the assertion, warning, or message.
///                Will be appended to the \alox domain.
/// @param msg     The assembled message to print.
//==================================================================================================
void  ALoxAssertionPlugin( const lang::CallerInfo& ci,
                           int                     type,
                           std::string_view        domain,
                           std::string_view        msg      );

std::string_view const ALOX_ASSERTION_PLUGIN_DOMAIN_PREFIX = "/ALIB";

#if !DOXYGEN
namespace { Lox* assertionLox= nullptr; }
#endif

void Log::SetALibAssertionPlugin ( Lox* pLox ) {

    // remove plugin
    if ( pLox == nullptr ) {
        if ( assertionLox == nullptr )
            return;

        assertionLox->Acquire( ALIB_CALLER );
            assertionLox->GetLogableContainer().Add( "ALoxAssertionPlugin removed "
                                                     "from Lox {!Q}", assertionLox->GetName()  );
            assertionLox->Entry( ALOX_ASSERTION_PLUGIN_DOMAIN_PREFIX, Verbosity::Verbose );
        assertionLox->Release ();
        assertionLox      = nullptr;
        assert::PLUGIN= nullptr;
        return;
    }

    // add plugin
    assertionLox      = pLox;
    assert::PLUGIN= ALoxAssertionPlugin;
    assertionLox->Acquire( ALIB_CALLER );
        assertionLox->GetLogableContainer().Add( "ALoxAssertionPlugin set to Lox {!Q}.", pLox->GetName() );
        assertionLox->Entry( ALOX_ASSERTION_PLUGIN_DOMAIN_PREFIX, Verbosity::Verbose );

        // we set the verbosity only now. This should
        // - allow to have the above verbose message seen once
        // - in case the values become externalized, this setting is written to such external
        //   configuration file and thus is not displayed a second time.
        assertionLox->SetVerbosity( DebugLogger, Verbosity::Warning, ALOX_ASSERTION_PLUGIN_DOMAIN_PREFIX );
    assertionLox->Release ();
}

void ALoxAssertionPlugin( const lang::CallerInfo&  ci,
                           int                     type,
                           std::string_view        domain,
                           std::string_view        msg      ) {
    assertionLox->Acquire( ci );

        auto& logables= assertionLox->GetLogableContainer();
        logables.Add( msg );

        auto verbosity= type == 0 ? Verbosity::Error   :
                        type == 1 ? Verbosity::Warning :
                        type == 2 ? Verbosity::Info    :
                                    Verbosity::Verbose ;

        NString256 dom(ALOX_ASSERTION_PLUGIN_DOMAIN_PREFIX); dom << '/' << domain;

        assertionLox->Entry( dom, verbosity );
    assertionLox->Release ();
}


#endif //ALIB_DEBUG

#   include "ALib.Lang.CIMethods.H"
}// namespace [alib::lox]
