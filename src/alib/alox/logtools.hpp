/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_alox of the \aliblong.
 *
 * \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/

#ifndef HPP_ALOX_LOGTOOLS
#define HPP_ALOX_LOGTOOLS 1

#if !defined (HPP_ALIB_ALOX)
    #include "alib/alox/alox.hpp"
#endif

// #################################################################################################
// includes
// #################################################################################################

namespace alib {

namespace lang { class Exception; }

namespace lox {

/** ************************************************************************************************
 * Simple class with static tool functions.
 * Currently the only functionality is logging objects of type \alib{lang,Exception}.
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
         * Logs an \b %alib::Exception. If available, the correct source information is used for
         * each entry.
         *
         * Parameter \p{domainPrefix} and \p{logPrefix} are both set on outer thread scope (using
         * \ref alib::lox::Lox::SetDomain "SetDomain( domainPrefix, Scope::ThreadOuter )" and
         * \ref alib::lox::Lox::SetPrefix "SetPrefix( logPrefix, Scope::ThreadOuter )".
         * The reason for this approach is that it may occur that other prefixes or scope domains
         * are set set on source scope for the source files and methods found in the exception
         * entry's source code information. This is especially important to understand in respect
         * to the log domain: With the default value of parameter \p{domainPrefix} being \e "/ERR",
         * all source related domains will appear as a sub-domain of \e "/ERR" when the exception
         * is logged using this tool function.
         *
         * @param lox           The lox to log into.
         * @param e             The exception to log.
         * @param verbosity     The verbosity of the log output.<br>
         *                      Defaults to \b Verbosity::Error.
         * @param domainPrefix  The domain to log into. If \b NullString() , no domain is set and
         *                      the default domain is used instead.<br>
         *                      Defaults to \e "/ERR".
         * @param logPrefix     A log prefix string. If \b NullString() , no prefix is set<br>
         *                      Defaults to <b>"  "</b> (two spaces).
         ******************************************************************************************/
        ALIB_API
        static void Exception( Lox&                    lox,
                               const lang::Exception&  e,
                               Verbosity               verbosity      = Verbosity::Error,
                               const NString&          domainPrefix   = "/ERR",
                               const String&           logPrefix      = A_CHAR("  ")    );
}; // class LogTools

} // namespace alib[::lox]

/// Type alias in namespace \b alib.
using     LogTools=           lox::LogTools;

}  // namespace [alib]


#endif // HPP_ALOX_LOGTOOLS
