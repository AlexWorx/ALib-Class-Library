//==================================================================================================
/// \file
/// This header-file is part of module \alib_alox of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
ALIB_EXPORT namespace alib { namespace lox {

//==================================================================================================
/// Simple class with static tool functions.
/// Currently the only functionality is logging objects of type #"LogTools::Exception".
//==================================================================================================
class LogTools {
  public:
    /// Logs an #"%alib"::Exception. If available, the correct source information is used for
    /// each entry.
    ///
    /// Parameter \p{domainPrefix} and \p{logPrefix} are both set on outer thread scope (using
    /// #"Lox::SetDomain(const NString&, Scope)" and #"Lox::SetPrefix(const Box&, Scope)".
    /// The reason for this approach is that it may occur that other prefixes or scope domains
    /// are set on source scope for the source files and methods found in the exception
    /// entry's source code information. This is especially important to understand in respect
    /// to the log domain: With the default value of parameter \p{domainPrefix} being \e "/ERR",
    /// all source related domains will appear as a subdomain of \e "/ERR" when the exception
    /// is logged using this tool function.
    ///
    /// @param lox           The lox to log into.
    /// @param e             The exception to log.
    /// @param verbosity     The verbosity of the log output.<br>
    ///                      Defaults to #"%Verbosity::Error".
    /// @param domainPrefix  The domain to log into. If nulled , no domain is set and
    ///                      the default domain is used instead.<br>
    ///                      Defaults to \e "/ERR".
    /// @param logPrefix     A log prefix string. If nulled, no prefix is set.<br>
    ///                      Defaults to <b>"  "</b> (two spaces).
    ALIB_DLL
    static void Exception( Lox&                          lox,
                           const exceptions::Exception&  e,
                           Verbosity                     verbosity      = Verbosity::Error,
                           const NString&                domainPrefix   = "/ERR",
                           const String&                 logPrefix      = A_CHAR("  ")    );
}; // class LogTools

} // namespace alib[::lox]

/// Type alias in namespace #"%alib".
using     LogTools=           lox::LogTools;

}  // namespace [alib]
