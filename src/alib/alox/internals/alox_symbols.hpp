// #################################################################################################
//  aworx::lib::lox - ALox Logging Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################

// needed for Doxygen include
/// @file

#ifndef HPP_ALOX_SYMBOLS
#define HPP_ALOX_SYMBOLS 1


/**
 * @addtogroup GrpALibCompilerSymbols
 * @{
 * \def  ALOX_DBG_LOG_ON
 *  Explicitly enables debug logging by defining the code selection symbol \ref ALOX_DBG_LOG.
 *
 *  Must not be used in conjunction with \ref ALOX_DBG_LOG_OFF.
 *
 *  If neither <b>ALOX_DBG_LOG_ON</b> nor <b>ALOX_DBG_LOG_OFF</b> are specified, \alox will determine the
 *  inclusion of debug <em>Log Statements</em> by evaluating macro #ALIB_DEBUG.
 *  If ALIB_DEBUG is set, ALOX_DBG_LOG will also be set.
 *
 *
 * \def  ALOX_DBG_LOG_OFF
 *  Explicitly disables debug logging by leaving the code selection symbol \ref ALOX_DBG_LOG undefined.
 *  Any debug <em>Log Statements</em> that are embedded in code by using the \ref GrpMacrosDebugLog, will
 *  get pruned.
 *
 *  Must not be used in conjunction with \ref ALOX_DBG_LOG_ON.
 *
 *  If neither <b>ALOX_DBG_LOG_ON</b> nor <b>ALOX_DBG_LOG_OFF</b> are specified, \alox will determine the
 *  inclusion of debug <em>Log Statements</em> by evaluating macro #ALIB_DEBUG.
 *  If ALIB_DEBUG is set, ALOX_DBG_LOG will also be set.
 *
 * \def  ALOX_REL_LOG_ON
 *  Explicitly enables release logging by defining the code selection symbol \ref ALOX_REL_LOG.
 *
 *  Must not be used in conjunction with \ref ALOX_REL_LOG_OFF.
 *
 *  If neither <b>ALOX_REL_LOG_ON</b> nor <b>ALOX_REL_LOG_OFF</b> are specified, release logging
 *  implemented by \alox release log macros is enabled. Therefore, this symbol is provided only for
 *  completeness. There is no real need to pass it to the compiler.
 *
 *  Remember, that any release logging that is not implemented using the \alox release logging macros,
 *  but instead using the \alox classes in the source code directly, is not affected by this
 *  macro.
 *
 * \def  ALOX_REL_LOG_OFF
 *  Explicitly disables release logging by leaving the code selection symbol \ref ALOX_REL_LOG
 *  undefined.
 *  Any release <em>Log Statements</em> that are embedded in code by using the
 *  \ref GrpMacrosReleaseLog, will get pruned.
 *
 *  Must not be used in conjunction with \ref ALOX_REL_LOG_ON.
 *
 *  If neither <b>ALOX_REL_LOG_ON</b> nor <b>ALOX_REL_LOG_OFF</b> are specified, release logging
 *  implemented by \alox release log macros is enabled. Therefore, this symbol is provided only for
 *  completeness. There is no real need to pass it to the compiler.
 *
 *  Remember, that any release logging that is not implemented using the \alox release logging macros,
 *  but instead using the \alox classes in the source code directly, is not affected by this
 *  macro.
 *
 * \def  ALOX_DBG_LOG_CI_ON
 *  Explicitly enables the generation of scope information using the built-in preprocessor
 *  macros like "__FILE__" and "__LINE__" for \alox debug logging statements
 *
 *  Passing this Symbol to the compiler, makes \alox define the code selection symbol
 *  \ref ALOX_DBG_LOG_CI.
 *  Must not be used in conjunction with \ref ALOX_DBG_LOG_CI_OFF.
 *
 *  If neither <b>ALOX_DBG_LOG_CI_ON</b> nor <b>ALOX_DBG_LOG_CIO_OFF</b> are specified, \alox will enable
 *  caller source information for debug logging. In other words, <b>ALOX_DBG_LOG_CI_ON</b> is the
 *  default and passing the symbol to the compiler is not mandatory.
 *
 *  \note The \alox <em>Scope Domain</em> mechanism as well as the ALox features \e Lox.Once and
 *  <em>Log Data</em> rely on caller information.
 *  These mechanisms are therefore not available to debug logging unless this symbol is set.
 *
 * \def  ALOX_DBG_LOG_CI_OFF
 *  Disables the generation of scope information using the built-in preprocessor
 *  macros like "__FILE__" and "__LINE__" for \alox debug logging statements
 *
 *  Passing this Symbol to the compiler, makes \alox to leave the code selection symbol
 *  \ref ALOX_DBG_LOG_CI undefined.<br>
 *  Must not be used in conjunction with \ref ALOX_DBG_LOG_CI_ON.
 *
 *  If neither <b>ALOX_DBG_LOG_CI_ON</b> nor <b>ALOX_DBG_LOG_CIO_OFF</b> are specified, then
 *  \ref ALOX_DBG_LOG_CI_ON is the default.
 *
 *  \note The \alox Scope Domain mechanism as well as the ALox features \e Lox.Once and
 *  <em>Log Data</em> rely on caller information.
 *  These mechanisms are therefore not available to debug logging when this symbol is set.
 *
 *
 * \def  ALOX_REL_LOG_CI_ON
 *  Explicitly enables the generation of scope information using the built-in preprocessor
 *  macros like "__FILE__" and "__LINE__" for \alox release logging statements
 *
 *  Passing this Symbol to the compiler, makes \alox define the code selection symbol
 *  \ref ALOX_REL_LOG_CI.<br>
 *  Must not be used in conjunction with \ref ALOX_REL_LOG_CI_OFF.
 *
 *  If neither <b>ALOX_REL_LOG_CI_ON</b> nor <b>ALOX_REL_LOG_CIO_OFF</b> are specified, \alox will disable
 *  caller source information for release logging.
 *
 *  Enabling source info for for release logging is seldom wanted. Release executables should
 *  not generate log output that an end user is not able to understand.
 *  It can make sense however, if release log information from the field goes back to the
 *  software development team.
 *  Furthermore, it can be very helpful to enable scope information for release <em>Log Statements</em>
 *  in the debug version of a compilation unit.
 *
 *  \note The \alox <em>Scope Domain</em> mechanism as well as the ALox features \e Lox.Once and
 *  <em>Log Data</em> rely on caller information.
 *  These mechanisms are therefore not available to debug logging unless this symbol is set.
 *
 *
 * \def  ALOX_REL_LOG_CI_OFF
 *  Disables the generation of scope information using the built-in preprocessor
 *  macros like "__FILE__" and "__LINE__" for \alox release logging statements
 *
 *  Passing this Symbol to the compiler, makes \alox to leave the code selection symbol
 *  \ref ALOX_REL_LOG_CI undefined.<br>
 *  Must not be used in conjunction with \ref ALOX_REL_LOG_CI_ON.
 *  If neither <b>ALOX_REL_LOG_CI_ON</b> nor <b>ALOX_REL_LOG_CIO_OFF</b> are specified, then
 *  \ref ALOX_REL_LOG_CI_OFF is the default.  In other words: passing the symbol to the
 *  compiler is not mandatory.
 *
 *  \note The \alox <em>Scope Domain</em> mechanism as well as the ALox features \e Lox.Once and
 *  <em>Log Data</em> rely on caller information.
 * These mechanisms are therefore not available to release logging unless
 *  \ref ALOX_REL_LOG_CI_ON is set.
 *
 *
 * @}
 *
 * @addtogroup GrpALibCodeSelectorSymbols
 * @{
 *
 * @name ALox Log-Code Selection
 * This group of code selection symbols is defined with the use of module \alibmod_alox and
 * used to select the inclusion of log code. The symbols are seldom to be used by end-user code.
 * Instead, the macros used for the log statements themselves (see \ref GrpMacrosDebugLog and
 * \ref GrpMacrosReleaseLog) are variably defined based on these symbols.
 *
 * \def  ALOX_DBG_LOG
 *  If defined as \c true, debug <em>Log Statements</em> are enabled. Hence, users of \alox can
 *  conditionally compile special debug logging code that normally belongs to corresponding
 *  debug logging statements.
 *  This is useful for example, to conditionally compile code that calculates and gathers
 *  information to do some more complex log output.
 *
 *  See \ref ALOX_DBG_LOG_ON and \ref ALOX_DBG_LOG_OFF for information about how to manipulate
 *  this symbol.
 *
 * \def  ALOX_REL_LOG
 *  If defined, release <em>Log Statements</em> are enabled. Hence, users of \alox can conditionally
 *  compile special debug logging code that normally belongs to corresponding
 *  debug logging statements.
 *  This is useful for example, to conditionally compile code that calculates and gathers
 *  information to do some more complex log output.
 *
 *  See \ref ALOX_REL_LOG_ON and \ref ALOX_REL_LOG_OFF for information about how to manipulate
 *  this symbol.
 *
 * \def  ALOX_DBG_LOG_CI
 *  If defined, scope information is passed to \alox on invocation of debug
 *  <em>Log Statements</em>.
 *  Hence, users of \alox can conditionally compile special debug logging code that normally
 *  belongs to corresponding debug logging statements based on this symbol. A sample would be
 *  the definition of different log line meta information formats, depending on the availability
 *  of scope information.
 *
 *  \note The \alox <em>Scope Domain</em> mechanism as well as the ALox features \e Lox.Once and
 *  <em>Log Data</em> rely on caller information.
 *  These mechanisms are therefore not available to debug logging unless this symbol is set.
 *
 *
 *  See \ref ALOX_DBG_LOG_CI_ON and \ref ALOX_DBG_LOG_CI_OFF for information about how to manipulate
 *  this symbol.
 *
 * \def  ALOX_REL_LOG_CI
 *  If defined, scope information is passed to \alox on invocation of release log
 *  statements when using the \ref GrpMacrosReleaseLog.<br>
 *  Hence, users of \alox can conditionally compile special debug logging code that normally
 *  belongs to corresponding debug logging statements based on this symbol. A sample would be
 *  the definition of different log line meta-information formats, depending on the availability of
 *  scope information.
 *
 *  \note The \alox <em>Scope Domain</em> mechanism as well as the ALox features \e Lox.Once and
 *  <em>Log Data</em> rely on caller information.
 *  These mechanisms are therefore not available to debug logging unless this symbol is set.
 *
 *  See \ref ALOX_REL_LOG_CI_ON and \ref ALOX_REL_LOG_CI_OFF for information about how to manipulate
 *  this symbol.
 *
 * @}
 * @}
 *
*/
#if defined(DOX_PARSER)
    #define     ALOX_DBG_LOG
    #define     ALOX_DBG_LOG_ON
    #define     ALOX_DBG_LOG_OFF
#else
    #if defined(ALOX_DBG_LOG)
        #error "ALOX_DBG_LOG must not be set from outside"
    #endif

    #if defined(ALOX_DBG_LOG_OFF) && defined(ALOX_DBG_LOG_ON)
        #error "ALOX_DBG_LOG_OFF / ALOX_DBG_LOG_ON are both set"
    #endif

    #if defined(ALOX_DBG_LOG_ON) || ( ALIB_DEBUG && !defined(ALOX_DBG_LOG_OFF) )
        #define ALOX_DBG_LOG 1
    #else
        #define ALOX_DBG_LOG 0
    #endif
#endif //DOX_PARSER


#if defined(DOX_PARSER)
    #define     ALOX_REL_LOG
    #define     ALOX_REL_LOG_ON
    #define     ALOX_REL_LOG_OFF
#else
    #if defined(ALOX_REL_LOG)
        #error "ALOX_REL_LOG must not be set from outside"
    #endif

    #if defined(ALOX_REL_LOG_OFF) && defined(ALOX_REL_LOG_ON)
        #error "ALOX_REL_LOG_OFF / ALOX_REL_LOG_ON are both set"
    #endif

    #if !defined(ALOX_REL_LOG_OFF)
        #define ALOX_REL_LOG 1
    #else
        #define ALOX_REL_LOG 0
    #endif
#endif //DOX_PARSER


// #################################################################################################
// DBG_LOG_CI / REL_LOG_CI
// #################################################################################################

#if defined(DOX_PARSER)
    #define     ALOX_DBG_LOG_CI
    #define     ALOX_DBG_LOG_CI_ON
    #define     ALOX_DBG_LOG_CI_OFF
#else
    #if defined(ALOX_DBG_LOG_CI)
        #error "ALOX_DBG_LOG_CI must not be set from outside"
    #endif

    #if defined(ALOX_DBG_LOG_CI_OFF) && defined(ALOX_DBG_LOG_CI_ON)
        #error "ALOX_DBG_LOG_CI_OFF / ALOX_DBG_LOG_CI_ON are both set"
    #endif

    #if ALOX_DBG_LOG && !defined( ALOX_DBG_LOG_CI_OFF )
        #define ALOX_DBG_LOG_CI 1
    #else
        #define ALOX_DBG_LOG_CI 0
    #endif
#endif //DOX_PARSER



#if defined(DOX_PARSER)
    #define     ALOX_REL_LOG_CI
    #define     ALOX_REL_LOG_CI_ON
    #define     ALOX_REL_LOG_CI_OFF
#else
    #if defined(ALOX_REL_LOG_CI)
        #error "ALOX_REL_LOG_CI must not be set from outside"
    #endif

    #if defined(ALOX_REL_LOG_CI_OFF) && defined(ALOX_REL_LOG_CI_ON)
        #error "ALOX_REL_LOG_CI_OFF / ALOX_REL_LOG_CI_ON are both set"
    #endif

    #if ALOX_REL_LOG && defined( ALOX_REL_LOG_CI_ON )
        #define ALOX_REL_LOG_CI 1
    #else
        #define ALOX_REL_LOG_CI 0
    #endif
#endif //DOX_PARSER



#endif // HPP_ALOX_SYMBOLS
