// #################################################################################################
//  aworx::lib::lox - ALox Logging Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################

// needed for Doxygen include
/// @file

#ifndef HPP_ALOX_MACROS
#define HPP_ALOX_MACROS 1


#if !defined(HPP_ALIB_ALOX)
    #error "include alox.hpp instead of alox_macros.hpp"
#endif


// #################################################################################################
// Macros not related to logging
// #################################################################################################

// #################################################################################################
// ALOX_SYMBOL_DEBUG
// #################################################################################################

#if defined(DOX_PARSER)
    #define  ALOX_COMPILATION_FLAGS
#else

    #define ALOX_DBG_LOG_VFYBIT                (1 << 0)
    #define ALOX_DBG_LOG_CI_VFYBIT             (1 << 1)
    #define ALOX_REL_LOG_VFYBIT                (1 << 2)
    #define ALOX_REL_LOG_CI_VFYBIT             (1 << 3)

    #define    ALOX_COMPILATION_FLAGS  (        ( ALOX_DBG_LOG     * ALOX_DBG_LOG_VFYBIT    ) \
                                              + ( ALOX_DBG_LOG_CI  * ALOX_DBG_LOG_CI_VFYBIT ) \
                                              + ( ALOX_REL_LOG     * ALOX_REL_LOG_VFYBIT    ) \
                                              + ( ALOX_REL_LOG_CI  * ALOX_REL_LOG_CI_VFYBIT ) \
                                       )
#endif

//! @endcond

/**
 * @defgroup GrpALoxMacrosLowLevel    ALox Low level Macros
 *
 * The preprocessor macros listed here are the foundation for
 * \ref GrpMacrosDebugLog and \ref GrpMacrosReleaseLog.
 *
 * The definition of the macros is dependent on \ref GrpALibCodeSelectorSymbols, which in turn
 * mostly depend on \ref GrpALibCompilerSymbols.<p>
 *
 * Developers that use ALox in standard use cases do not need to know or even change the macros
 * listed here.
 * @{
 *
 * \def  ALOX_COMPILATION_FLAGS
 *  Macro to provide the a compilation flag verifier value to be used with method
 *  \alib{lang,Library::VerifyCompilationFlags} of singleton \alib{ALOX}.
 */



// #################################################################################################
// Macros to access the Lox instance
// #################################################################################################


/**
 * \def  LOG_LOX
 * The Lox instance used by all debug logging macros. This can be overwritten (prior or after
 * including alox.hpp) to allow different instances of class Lox for debug logging within
 * different source entities. However, other ways to structure log output and to separate
 * log information into different streams exists in \alox and overwriting this macro is not
 * recommended for standard use cases.<p>
 *
 * Note: The definition must provide a reference (not a pointer) to the \b %Lox object.
 *
 * \def  LOX_LOX
 * The Lox instance used by all release logging macros. This has to be set (prior or after
 * including alox.hpp) to provide access to a, dedicated instance of class Lox created for
 * release logging within a software.<br>
 * It is of-course allowed to use different instances within different source entities.
 * However, other ways to structure log output and separate log streams exists in \alox and should
 * be evaluated prior to introducing different instances of class Lox.<p>
 *
 * Note: The definition must provide a reference (not a pointer) to the \b %Lox object.
 */

#if !defined(LOG_LOX)
    #define LOG_LOX       (*aworx::lib::ALOX.Log())
#endif

#if defined(DOX_PARSER)
    #define LOX_LOX
#endif


// #################################################################################################
// Macros for setting the source code info prior to executing a log
// #################################################################################################


/**
 * \def  LOG_CI
 * Dependent on #ALOX_DBG_LOG_CI, this macro provides comma delimited source information
 * (sourcefile, line number, function name) or corresponding \c null values.
 *
 * \def  LOX_CI
 * Dependent on #ALOX_REL_LOG_CI, this macro provides comma delimited source information
 * (sourcefile, line number, function name) or corresponding \c null values.
 */

#ifdef  ALOX_DBG_LOG_CI
    #define LOG_CI      ALIB_CALLER
#else
    #define LOG_CI      nullptr, 0, nullptr
#endif

#ifdef  ALOX_REL_LOG_CI
    #define LOX_CI      ALIB_CALLER
#else
    #define LOX_CI      nullptr, 0, nullptr
#endif


/**
 * \def  LOG_ACQUIRE
 * Macro that is placed at the beginning of almost all macros of type \ref GrpMacrosDebugLog.
 * Provides scope information (provided that \ref ALOX_DBG_LOG_CI is set) to the lox and places
 * the debug lox instance access code using \ref LOG_LOX.
 */
#define LOG_ACQUIRE     { aworx::Lox& _log= LOG_LOX; _log.Acquire( LOG_CI );

/**
 * \def  LOG_RELEASE
 * Macro that is placed at the end of almost all macros of type \ref GrpMacrosDebugLog.
 * Releases the lox which was acquired when passing scope information (in macro \ref LOG_ACQUIRE.).
 */
#define LOG_RELEASE         _log.Release(); }

/**
 * \def  LOX_ACQUIRE
 * Macro that is placed at the beginning of almost all macros of type \ref GrpMacrosReleaseLog.
 * Provides scope information (provided that \ref ALOX_REL_LOG_CI is set) to the lox and places
 * the release lox instance access code using \ref LOX_LOX.
 */
#define LOX_ACQUIRE     { aworx::Lox& _lox= LOX_LOX; _lox.Acquire( LOX_CI );

/**
 * \def  LOX_RELEASE
 * Macro that is placed at the end of almost all macros of type \ref GrpMacrosDebugLog.
 * Releases the lox which was acquired when passing scope information (in macro \ref LOG_ACQUIRE.).
 */
#define     LOX_RELEASE     _lox.Release(); }

/** @} */ // End of GrpALoxMacrosLowLevel


// #################################################################################################
// Debug logging macros
// #################################################################################################

/**
 * @defgroup GrpMacrosDebugLog ALox Macros For Debug Logging
 *
 * The macros listed here, are provided to place debug <em>Log Statements</em> within source code
 * using \alox.
 * Besides that, macros controlling and setting preferences for \alox exists.<p>
 *
 * The exclusive use of these macros should be sufficient to support most of common debug logging
 * statements with \alox. Should some functionality be not available easily with using the macros,
 * of-course, the normal C++ API of \alox can be used in parallel to using the macros.
 * For proper pruning of code that is using the C++ API, such code has to be enclosed by<br>
 \verbatim
 #if ALOX_DBG_LOG
    ...
 #endif
 \endverbatim
 * lines, or embedded in macro \ref Log_Prune.
 * \note To be clear: When using the macros, the <em>\#if / \#endif</em> statements are \e not needed!
 *
 * All macro names are prefixed "Log_". This implies that they are macros to implement *debug*
 * logging. In contrast to this, a set of similar macros exists for release logging
 * (see \ref GrpMacrosReleaseLog). Those are
 * prefixed "Lox_". (The choice of the prefixes *Log_* and *Lox* provide  maximum source code
 * compatibility of <b>%ALox for C++</b> log lines in comparison to ALox for C# and ALox for Java.<p>
 *
 * Most macros make use of the macro \ref LOG_LOX, which references a singleton
 * object of class \ref aworx::lib::lox::Lox "Lox" that is used for all debug logging.
 * This singleton concept covers most
 * uses cases for debug logging. If more flexibility is wanted, then either macro
 * \ref LOG_LOX might be changed for different compilation units or the <b>%ALox for C++ API</b>
 * might be used instead of the macros listed here.
 *
 * \note The <em>Scope Domain</em> mechanism of \alox, as well as other \alox functionality
 * which relies on \e Scopes use the information provided by \alib macro \ref ALIB_CALLER.
 * Changing this macro, might cause these mechanism to fail.
 * @{
 */


/**
 * \def  Log_Prune
 * This very simple macro is used for pruning debug <em>Log Statements</em>.
 * While it is used as a building block of all other macros for debug logging, for code entities
 * using \alox, it provides an easy way to prune code lines that get inserted purely to support
 * logging, e.g. to create \alox loggers or to prepare more complex log output. (The alternative
 * way is to enclose such code within
 * <code> \#ifdef ALOX_DBG_LOG </code> / <code> \#endif </code> preprocessor lines.
 * <p>
 *
 * Depends on \ref ALOX_DBG_LOG. If this is set, the macro just copies the code provided, else it
 * does not copy it, hence removes the code.
 */
#if ALOX_DBG_LOG
    #define Log_Prune( ... ) __VA_ARGS__
#else
    #define Log_Prune( ... )
#endif


/// Invokes \alox{Lox::SetSourcePathTrimRule} on the debug singleton of class Lox defined in macro \ref LOG_LOX.
#define Log_SetSourcePathTrimRule(...)          Log_Prune( LOG_ACQUIRE _log.SetSourcePathTrimRule( __VA_ARGS__ );    LOG_RELEASE )

/// Invokes \alox{Lox::ClearSourcePathTrimRules} on the debug singleton of class Lox defined in macro \ref LOG_LOX.
#define Log_ClearSourcePathTrimRules(...)       Log_Prune( LOG_ACQUIRE _log.ClearSourcePathTrimRules( __VA_ARGS__ ); LOG_RELEASE )

/// Invokes \alox{Log::AddDebugLogger} to create, add and configure a default debug logger suitable for the platform and toolset.
#define Log_AddDebugLogger()                    Log_Prune( LOG_ACQUIRE    Log::AddDebugLogger   ( &_log );           LOG_RELEASE )

/// Invokes \alox{Log::RemoveDebugLogger} to remove and delete a debug logger created by \ref aworx::lib::lox::Log::AddDebugLogger "Log::AddDebugLogger".
#define Log_RemoveDebugLogger()                 Log_Prune( LOG_ACQUIRE    Log::RemoveDebugLogger( &_log );           LOG_RELEASE )

/// Invokes \alox{Lox::GetLogger} on the debug singleton of class Lox defined in macro \ref LOG_LOX.
#define Log_GetLogger( identifier, name )       Log_Prune( aworx::lib::lox::core::Logger* identifier;                                      \
                                                           LOG_ACQUIRE  identifier= _log.GetLogger( name );          LOG_RELEASE )

/// Invokes \alox{Lox::RemoveLogger} on the debug singleton of class Lox defined in macro \ref LOG_LOX.
#define Log_RemoveLogger( logger)               Log_Prune( LOG_ACQUIRE _log.RemoveLogger ( logger );                 LOG_RELEASE )

/// Invokes \alox{Lox::SetDomain} on the debug singleton of class Lox defined in macro \ref LOG_LOX.
#define Log_SetDomain(...)                      Log_Prune( LOG_ACQUIRE _log.SetDomain    ( __VA_ARGS__ );            LOG_RELEASE )

/// Invokes \alox{Lox::RemoveThreadDomain} on the debug singleton of class Lox defined in macro \ref LOG_LOX.
#define Log_RemoveThreadDomain(...)             Log_Prune( LOG_ACQUIRE _log.RemoveThreadDomain( __VA_ARGS__ );       LOG_RELEASE )

/// Invokes \alox{Lox::SetDomainSubstitutionRule} on the debug singleton of class Lox defined in macro \ref LOG_LOX.
#define Log_SetDomainSubstitutionRule(...)      Log_Prune( LOG_ACQUIRE _log.SetDomainSubstitutionRule( __VA_ARGS__); LOG_RELEASE )

/// Invokes one of the overloaded methods \alox{Lox::SetVerbosity} on the debug singleton of class Lox defined in macro \ref LOG_LOX.
#define Log_SetVerbosity(...)                   Log_Prune( LOG_ACQUIRE _log.SetVerbosity ( __VA_ARGS__ );            LOG_RELEASE )

/// Invokes \alox{Lox::SetStartTime} on the debug singleton of class Lox defined in macro \ref LOG_LOX.
#define Log_SetStartTime( ... )                 Log_Prune( LOG_ACQUIRE _log.SetStartTime ( __VA_ARGS__ );            LOG_RELEASE )

/// Invokes \alox{Lox::MapThreadName} on the debug singleton of class Lox defined in macro \ref LOG_LOX.
#define Log_MapThreadName( threadName )         Log_Prune( LOG_ACQUIRE _log.MapThreadName( threadName );             LOG_RELEASE )

/// Invokes \alox{Lox::State} on the debug singleton of class Lox defined in macro \ref LOG_LOX.
#define Log_LogState(...)                       Log_Prune( LOG_ACQUIRE _log.State        ( __VA_ARGS__ );            LOG_RELEASE )

/// Invokes \alox{Lox::Verbose} on the debug singleton of class Lox defined in macro \ref LOG_LOX.
#define Log_Verbose(...)                        Log_Prune( LOG_ACQUIRE _log.Verbose      ( __VA_ARGS__ );            LOG_RELEASE )

/// Invokes \alox{Lox::Info} on the debug singleton of class Lox defined in macro \ref LOG_LOX.
#define Log_Info(...)                           Log_Prune( LOG_ACQUIRE _log.Info         ( __VA_ARGS__ );            LOG_RELEASE )

/// Invokes \alox{Lox::Warning} on the debug singleton of class Lox defined in macro \ref LOG_LOX.
#define Log_Warning(...)                        Log_Prune( LOG_ACQUIRE _log.Warning      ( __VA_ARGS__ );            LOG_RELEASE )

/// Invokes \alox{Lox::Error} on the debug singleton of class Lox defined in macro \ref LOG_LOX.
#define Log_Error(...)                          Log_Prune( LOG_ACQUIRE _log.Error        ( __VA_ARGS__ );            LOG_RELEASE )

/// Invokes \alox{Lox::Assert} on the debug singleton of class Lox defined in macro \ref LOG_LOX.
#define Log_Assert(...)                         Log_Prune( LOG_ACQUIRE _log.Assert       ( __VA_ARGS__ );            LOG_RELEASE )

/// Invokes \alox{Lox::If} on the debug singleton of class Lox defined in macro \ref LOG_LOX.
#define Log_If(...)                             Log_Prune( LOG_ACQUIRE _log.If           ( __VA_ARGS__ );            LOG_RELEASE )

/// Invokes \alox{Lox::Once} on the debug singleton of class Lox defined in macro \ref LOG_LOX.
#define Log_Once(...)                           Log_Prune( LOG_ACQUIRE _log.Once         ( __VA_ARGS__ );            LOG_RELEASE )

/// Invokes \alox{Lox::Entry} on the debug singleton of class Lox defined in macro \ref LOG_LOX.
#define Log_Entry(...)                          Log_Prune( LOG_ACQUIRE _log.Entry        ( __VA_ARGS__ );            LOG_RELEASE )

/// Invokes \alox{Lox::SetPrefix} on the debug singleton of class Lox defined in macro \ref LOG_LOX.
#define Log_SetPrefix(...)                      Log_Prune( LOG_ACQUIRE _log.SetPrefix    ( __VA_ARGS__ );            LOG_RELEASE )

/// Invokes \alox{Lox::Store} on the debug singleton of class Lox defined in macro \ref LOG_LOX.
#define Log_Store(...)                          Log_Prune( LOG_ACQUIRE _log.Store        ( __VA_ARGS__ );            LOG_RELEASE )

/// Invokes \alox{Lox::Retrieve} on the debug singleton of class Lox defined in macro \ref LOG_LOX.
#define Log_Retrieve(data,...)                  Log_Prune( Box data; LOG_ACQUIRE data= _log.Retrieve( __VA_ARGS__ ); LOG_RELEASE )

/// Invokes \alox{LogTools::Exception} providing the debug singleton of class Lox defined in macro \ref LOG_LOX.
#define Log_Exception(...)                      Log_Prune( aworx::lib::lox::LogTools::Exception( LOG_LOX, __VA_ARGS__ );   )

/// @}


// #################################################################################################
// Release logging macros
// #################################################################################################
/**
 * @defgroup GrpMacrosReleaseLog        ALox Macros For Release Logging
 *
 * The macros listed here, are provided to place release <em>Log Statements</em> within source code using
 * \alox. Besides that, macros controlling and setting preferences for ALox exists.<p>
 *
 * The exclusive use of these macros should be sufficient to support most of common release logging
 * statements with \alox. Should some functionality be not available easily using the macros,
 * of-course, the normal C++ API of \alox can be used in parallel to using the macros.
 * Code that is using the C++ API might be enclosed by<br>
 \verbatim
 #if ALOX_REL_LOG
    ...
 #endif
 \endverbatim
 * lines, to remove them when compiling a release version of the software unit with pruned
 * release log macros. Alternatively, such code might be embedded in macro \ref Lox_Prune.
 * (Such compilation would be made by passing \ref ALOX_REL_LOG_OFF to the
 * compiler and could be useful in certain situations.) <p>
 *
 * Prior to using the macros, each code entity has to set the preprocessor macro \ref LOX_LOX
 * This can be done in a general header file of the software,
 * (e.g. the same that exposes the release-Lox object to that source), or, in more complex
 * scenarios with more than one release-Lox object, at any other appropriate source location.
 *
 * All macro names are prefixed with the term <b>Lox_</b>. This implies that they are macros to
 * implement <em>release logging</em>. In contrast to this, a set of similar macros exists for
 * <em>debug logging</em> (see \ref GrpMacrosDebugLog). Those are
 * prefixed <b>Log_</b>.
 * \note The choice of the prefixes <b>Log_</b> and <b>Lox_</b> was made to provide  maximum source
 * code compatibility of <b>%ALox for C++</b> log lines in comparison to other ALox implementations
 * in other programming languages, which provide a class Log as a kind of 'mirror' class of
 * class Lox. Invocations to class Log get pruned in these languages. <p>
 *
 * Most macros make use of the macro \ref LOX_LOX, which references a singleton
 * object of class \ref aworx::lib::lox::Lox "Lox" that is used for all release logging.
 * This singleton concept covers most
 * uses cases for release logging. If more flexibility is wanted, then either the macro
 * \ref LOX_LOX might be changed for different compilation units or the <b>%ALox C++ API</b>
 * might be used instead of the macros listed here.
 * @{
 */


/**
 * \def  Lox_Prune
 * This very simple macro is used for pruning release <em>Log Statements</em> in the moment release logging
 * gets disabled (what in standard release scenarios is not done).<p>
 *
 * While it is used as a building block of all other macros for release logging, for code entities
 * using \alox, it provides an easy way to prune code lines that get inserted purely to support
 * logging, e.g. to create \alox loggers or to prepare more complex log output. (The alternative
 * way is to enclose such code within
 * <code> \#ifdef ALOX_REL_LOG </code> / <code> \#endif </code> preprocessor lines.
 * <p>
 *
 * Depends on \ref ALOX_REL_LOG. If this is set, the macro just copies the code provided, else it
 * does not copy it, hence removes the code.
 */

#if ALOX_REL_LOG
    #define Lox_Prune( releaseLogCode ) releaseLogCode
#else
    #define Lox_Prune( releaseLogCode )
#endif




/// Invokes \alox{Lox::SetSourcePathTrimRule} on the object of type Lox defined in macro \ref LOX_LOX.
#define Lox_SetSourcePathTrimRule(...)      Lox_Prune( LOX_ACQUIRE _lox.SetSourcePathTrimRule( __VA_ARGS__ ); LOX_RELEASE )

/// Invokes \alox{Lox::ClearSourcePathTrimRules} on the object of type Lox defined in macro \ref LOX_LOX.
#define Lox_ClearSourcePathTrimRules(...)   Lox_Prune( LOX_ACQUIRE _lox.ClearSourcePathTrimRules( __VA_ARGS__ ); LOX_RELEASE )

/// Invokes \alox{Lox::GetLogger} on the object of type Lox defined in macro \ref LOX_LOX.
#define Lox_GetLogger( identifier, name )   Lox_Prune( aworx::lib::lox::core::Logger* identifier; LOX_ACQUIRE identifier= _lox.GetLogger ( name ); LOX_RELEASE )

/// Invokes \alox{Lox::RemoveLogger} on the object of type Lox defined in macro \ref LOX_LOX.
#define Lox_RemoveLogger( logger )          Lox_Prune( LOX_ACQUIRE _lox.RemoveLogger ( logger );        LOX_RELEASE )


/** Invokes \alox{Lox::SetDomain} on the object of type Lox defined in macro \ref LOX_LOX.
 * \attention
 *  If \ref ALOX_REL_LOG_CI is not set, which is the default for release logging, and when used with language-related
 *  \e Scopes, this method will log an internal warning and will not be effective in respect to
 *  \e %Scope.Path, \e %Scope.Filename, and \e %Scope.Method. See [ALox User Manual](../manual.html) for detailed information.
 *  If Scope Domains based on source-related scopes should be supported in release logging, the
 *  software entity has to be compiled with precompiler symbol \ref ALOX_REL_LOG_CI_ON.
 *  Note that one effect of setting this symbol is, that information on source code paths and file names, as well as
 *  method names make their way into the release executable. This may not be wanted.
 */
#define Lox_SetDomain(...)                  Lox_Prune( LOX_ACQUIRE _lox.SetDomain    (__VA_ARGS__);        LOX_RELEASE )

/// Invokes \alox{Lox::RemoveThreadDomain} on the object of type Lox defined in macro \ref LOX_LOX.
#define Lox_RemoveThreadDomain(...)         Lox_Prune( LOX_ACQUIRE _lox.RemoveThreadDomain( __VA_ARGS__ ); LOX_RELEASE )

/// Invokes \alox{Lox::SetDomainSubstitutionRule} on the object of type Lox defined in macro \ref LOX_LOX.
#define Lox_SetDomainSubstitutionRule(...)  Lox_Prune( LOX_ACQUIRE _lox.SetDomainSubstitutionRule( __VA_ARGS__ );   LOX_RELEASE )

/// Invokes one of the overloaded methods \alox{Lox::SetVerbosity} on the object of type Lox defined in macro \ref LOX_LOX.
#define Lox_SetVerbosity(...)               Lox_Prune( LOX_ACQUIRE _lox.SetVerbosity ( __VA_ARGS__ );      LOX_RELEASE )

/// Invokes \alox{Lox::SetStartTime} on the object of type Lox defined in macro \ref LOX_LOX.
#define Lox_SetStartTime( ... )             Lox_Prune( LOX_ACQUIRE _lox.SetStartTime ( __VA_ARGS__ );      LOX_RELEASE )

/// Invokes \alox{Lox::MapThreadName} on the object of type Lox defined in macro \ref LOX_LOX.
#define Lox_MapThreadName( threadName )     Lox_Prune( LOX_ACQUIRE _lox.MapThreadName( threadName );       LOX_RELEASE )

/// Invokes \alox{Lox::State} on the object of type Lox defined in macro \ref LOX_LOX.
#define Lox_LogState(...)                   Lox_Prune( LOX_ACQUIRE _lox.State        ( __VA_ARGS__ );      LOX_RELEASE )

/// Invokes \alox{Lox::Verbose} on the object of type Lox defined in macro \ref LOX_LOX.
#define Lox_Verbose(...)                    Lox_Prune( LOX_ACQUIRE _lox.Verbose      ( __VA_ARGS__ );      LOX_RELEASE )

/// Invokes \alox{Lox::Info} on the object of type Lox defined in macro \ref LOX_LOX.
#define Lox_Info(...)                       Lox_Prune( LOX_ACQUIRE _lox.Info         ( __VA_ARGS__ );      LOX_RELEASE )

/// Invokes \alox{Lox::Warning} on the object of type Lox defined in macro \ref LOX_LOX.
#define Lox_Warning(...)                    Lox_Prune( LOX_ACQUIRE _lox.Warning      ( __VA_ARGS__ );      LOX_RELEASE )

/// Invokes \alox{Lox::Error} on the object of type Lox defined in macro \ref LOX_LOX.
#define Lox_Error(...)                      Lox_Prune( LOX_ACQUIRE _lox.Error        ( __VA_ARGS__ );      LOX_RELEASE )

/// Invokes \alox{Lox::Assert} on the object of type Lox defined in macro \ref LOX_LOX.
#define Lox_Assert(...)                     Lox_Prune( LOX_ACQUIRE _lox.Assert       ( __VA_ARGS__);       LOX_RELEASE )

/// Invokes \alox{Lox::If} on the object of type Lox defined in macro \ref LOX_LOX.
#define Lox_If(...)                         Lox_Prune( LOX_ACQUIRE _lox.If           ( __VA_ARGS__);       LOX_RELEASE )

/// Invokes \alox{Lox::Once} on the object of type Lox defined in macro \ref LOX_LOX.
#define Lox_Once(...)                       Lox_Prune( LOX_ACQUIRE _lox.Once         ( __VA_ARGS__ );      LOX_RELEASE )

/// Invokes \alox{Lox::Entry} on the object of type Lox defined in macro \ref LOX_LOX.
#define Lox_Entry(...)                      Lox_Prune( LOX_ACQUIRE _lox.Entry        ( __VA_ARGS__ );      LOX_RELEASE )


/** Invokes \alox{Lox::Store} on the object of type Lox defined in macro \ref LOX_LOX.
 *  \attention  If \ref ALOX_REL_LOG_CI is not set, which is the default for release logging
 *             statements, Log Data can not be used in conjunction with language-related \e Scopes.<br>
 *             If <em>Log Data</em> should be supported in release logging, the software entity has to be compiled
 *             with precompiler symbol \ref ALOX_REL_LOG_CI_ON. Note that one effect of setting
 *             this symbol is, that information on source code paths and file names, as well as
 *             method names make their way into the release executable. This may not be wanted.<br>
 *             In general, methods \b Lox::Store and \b Lox::Retrieve should exclusively be used for
 *             debug logging. See user manual for more information.
 */
#define Lox_Store(...)                      Lox_Prune( LOX_ACQUIRE _lox.Store        ( __VA_ARGS__ );      LOX_RELEASE )

/// Invokes \alox{Lox::Retrieve} on the object of type Lox defined in macro \ref LOX_LOX.
#define Lox_Retrieve(data,...)              Lox_Prune( LOX_ACQUIRE LogData* data= _lox.Retrieve( __VA_ARGS__ );  LOX_RELEASE )

/// Invokes \alox{Lox::SetPrefix} on the object of type Lox defined in macro \ref LOX_LOX.
#define Lox_SetPrefix(...)                  Lox_Prune( LOX_ACQUIRE _lox.SetPrefix     ( __VA_ARGS__ );     LOX_RELEASE )

/// Invokes \alox{LogTools::Exception} providing the object of type Lox defined in macro \ref LOX_LOX.
#define Lox_Exception(...)                  Log_Prune( aworx::lib::lox::LogTools::Exception( LOX_LOX, __VA_ARGS__ );   )

/** @} */ // GrpMacrosReleaseLog


#endif // HPP_ALOX_MACROS

