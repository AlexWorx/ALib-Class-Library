/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_basecamp of the \aliblong.
 *
 * \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_LANG_CAMP_BOOTSTRAP
#define HPP_ALIB_LANG_CAMP_BOOTSTRAP 1

#if !defined(HPP_ALIB_LANG_CAMP)
#   include "alib/lang/basecamp/camp.hpp"
#endif

#if !defined(HPP_ALIB_MONOMEM_LIST)
#   include "alib/monomem/list.hpp"
#endif

namespace alib {

/**
 * Lists all \alibcamps in the order of bootstrapping. Shutdown is performed in reverse order.
 * This list is used by overloaded functions
 * - \ref alib::Bootstrap(BootstrapPhases targetPhase, lang::Camp* targetCamp,int,int,TCompilationFlags)
 *   and
 * - \ref alib::Shutdown(ShutdownPhases targetPhase, lang::Camp* targetCamp).
 *
 * For details on when and how to manipulate this list explicitly, see the detailed explanations in
 * chapter \ref alib_manual_bootstrapping "4. Bootstrapping And Shutting Down ALib" of
 * the \ref alib_manual.
 */
ALIB_API
extern List<lang::Camp*>           Camps;

/**
 *  Fills list \ref alib::Camps, taking the permutation of modules contained in the \alibdist into
 *  account.<br>
 *  This function is usually invoked automatically.
 *
 *  For details on when and how to explicitly invoke this function, see the detailed explanations in
 *  chapter \ref alib_manual_bootstrapping "4. Bootstrapping And Shutting Down ALib" of
 *  the \ref alib_manual.
 */
ALIB_API
void            BootstrapAddDefaultCamps();

/** ************************************************************************************************
 * This function is usually invoked automatically and rightfully with the standard, parameterless
 * version \ref alib::Bootstrap(int,int,TCompilationFlags) "alib::Bootstrap().
 *
 * For details on when and how to invoke this method explicitly, see the detailed explanations in
 * chapter \ref alib_manual_bootstrapping of the \ref alib_manual.
 *
 * @param targetPhase      The initialization level to perform.
 * @param targetCamp       The "highest" \alibcamp in list \alib{Camps} to bootstrap.
 *                         Defaults to <c>nullptr</c>, which chooses the end of the list.
 * @param alibVersion      The \alib version required.
 *                         Defaults to \ref ALIB_VERSION and \b must not be passed when invoking
 *                         this function.
 * @param alibRevision     The \alib sub-version required.
 *                         Defaults to \ref ALIB_REVISION and \b must not be passed when invoking
 *                         this function.
 * @param compilationFlags The flags as defined in invoking compilation unit.
 *                         Defaults to \ref ALIB_COMPILATION_FLAGS and must not be passed when
 *                         invoking this function.
 **************************************************************************************************/
ALIB_API
void     Bootstrap( BootstrapPhases     targetPhase,
                    lang::Camp*         targetCamp         = nullptr,
                    int                 alibVersion        = ALIB_VERSION,
                    int                 alibRevision       = ALIB_REVISION,
                    TCompilationFlags   compilationFlags   = TCompilationFlags{ALIB_COMPILATION_FLAGS} );

/** ************************************************************************************************
 * This function is usually invoked automatically and rightfully with the standard, parameterless
 * version \ref alib::Shutdown().
 *
 * For details on when and how to invoke this method explicitly, see the detailed explanations in
 * chapter \ref alib_manual_bootstrapping "4. Bootstrapping And Shutting Down ALib" of
 * the \ref alib_manual.
 *
 * @param targetPhase The termination level to perform.
 * @param targetCamp  The "lowest" \alibcamp in list \alib{Camps} to shut down.
 *                    Defaults to <c>nullptr</c>, which chooses the start of the list.
 **************************************************************************************************/
ALIB_API
void     Shutdown( ShutdownPhases targetPhase,
                   lang::Camp*    targetCamp  = nullptr  );

} // namespace [alib]
#endif // HPP_ALIB_LANG_CAMP_BOOTSTRAP
