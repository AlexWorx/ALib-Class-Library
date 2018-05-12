// #################################################################################################
//  aworx::lib::lox - ALox Logging Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
/** @file */ // Hello Doxygen

// include ALox main header first...
#if !defined (HPP_ALIB_ALOX)
    #error "lox.hpp must not be included manually. Include alox.hpp instead."
#endif

// Due to our blocker above, this include will never be executed. But having it, allows IDEs
// (e.g. QTCreator) to read the symbols when opening this file
#if !defined (HPP_ALIB_ALOX)
    #include "alib/alox/alox.hpp"
#endif

// then, set include guard
#ifndef HPP_ALOX_LOGTOOLS
#define HPP_ALOX_LOGTOOLS 1

// #################################################################################################
// includes
// #################################################################################################

namespace aworx { namespace lib { namespace lox {

/** ************************************************************************************************
 * Simple class with static tool functions.
 * Currently the only functionality is logging objects of type
 * \ref aworx::lib::lang::Exception "Exception".
 **************************************************************************************************/
class LogTools
{
    // #############################################################################################
    // Public fields
    // #############################################################################################
    public:



    // #############################################################################################
    // Interface
    // #############################################################################################
    public:
        /** ****************************************************************************************
         * Logs an \b %aworx::Exception. If available, the correct source information is used for
         * each entry.
         *
         * Parameter \p{domainPrefix} and \p{logPrefix} are both set on outer thread scope (using
         * \ref aworx::lib::lox::Lox::SetDomain "SetDomain( domainPrefix, Scope::ThreadOuter )" and
         * \ref aworx::lib::lox::Lox::SetPrefix "SetPrefix( logPrefix, Scope::ThreadOuter )".
         * The reason for this approach is that it may occur that other prefixes or scope domains
         * are set set on source scope for the source files and methods found in the exception
         * entries' source code information. This is especially important to understand in respect
         * to the log domain: With the default value of parameter \p{domainPrefix} being \e "/ERR",
         * all source related domains will appear as a subdomain of \e "/ERR" when the exception
         * is logged using this tool function.
         *
         * @param lox           The lox to log into.
         * @param e             The exception to log.
         * @param verbosity     The verbosity of the log output.<br>
         *                      Defaults to \b Verbosity::Error.
         * @param domainPrefix  The domain to log into. If \c NullString, no domain is set and
         *                      the default domain is used instead.<br>
         *                      Defaults to \e "/ERR".
         * @param logPrefix     A log prefix string. If \c NullString, no prefix is set<br>
         *                      Defaults to <b>"  "</b> (two spaces).
         ******************************************************************************************/
        ALIB_API
        static void Exception( Lox&               lox,
                               aworx::Exception&  e,
                               Verbosity          verbosity           = Verbosity::Error,
                               const NString&     domainPrefix        = "/ERR",
                               const String&      logPrefix           = ASTR("  ")    );


}; // class LogTools

}} // namespace aworx[::lib::lox]

/// Type alias in namespace #aworx.
using     LogTools=           aworx::lib::lox::LogTools;

}  // namespace aworx


#endif // HPP_ALOX_LOGTOOLS
