//==================================================================================================
/// \file
/// This header file is part of module \alib_basecamp of the \aliblong.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_LANG_CAMP_BOOTSTRAP
#define HPP_ALIB_LANG_CAMP_BOOTSTRAP 1
#pragma once
#include "alib/lang/basecamp/camp.hpp"
#include "alib/monomem/monoallocator.hpp"
#include "alib/containers/list.hpp"

namespace alib {

/// Lists all \alibcamps in the order of bootstrapping. Shutdown is performed in reverse order.
/// This list is used by overloaded functions
/// - \doxlinkproblem{namespacealib.html;ac33999bb9be31380c6ae139210d23083;alib::Bootstrap(BootstrapPhases targetPhase; lang::Camp* targetCamp, int,int,TCompilationFlags)}.
///
///   and
/// - \doxlinkproblem{namespacealib.html;acd654b95c6e1833f4f04d0fc19e4ce36;alib::Shutdown(ShutdownPhases targetPhase; lang::Camp* targetCamp)}
///
/// For details on when and how to manipulate this list explicitly, see the detailed explanations in
/// chapter \ref alib_manual_bootstrapping "4. Bootstrapping And Shutting Down ALib" of
/// the \ref alib_manual.
///
/// \note Should for any reason it is needed to \b not use method \alib{BootstrapAddDefaultCamps}
///       to default-fill this list before adding custom camps, then the allocator of this
///       list has to be set before filling it. This can be done with:
///
///              CAMPS.GetAllocator()=  monomem::GLOBAL_ALLOCATOR;
ALIB_API
extern List<MonoAllocator, lang::Camp*>  CAMPS;

///  Fills list \ref alib::CAMPS, taking the permutation of modules contained in the \alibdist into
///  account.<br>
///  This function is usually invoked automatically.
///
///  For details on when and how to explicitly invoke this function, see the detailed explanations in
///  chapter \ref alib_manual_bootstrapping "4. Bootstrapping And Shutting Down ALib" of
///  the \ref alib_manual.
ALIB_API
void            BootstrapAddDefaultCamps();

//==================================================================================================
/// This function is usually invoked automatically and rightfully with the standard, parameterless
/// version \ref alib::Bootstrap(int,int,TCompilationFlags) "alib::Bootstrap().
///
/// For details on when and how to invoke this method explicitly, see the detailed explanations in
/// chapter \ref alib_manual_bootstrapping of the \ref alib_manual.
///
/// @param targetPhase      The initialization level to perform.
/// @param targetCamp       The "highest" \alibcamp in list \alib{CAMPS} to bootstrap.
///                         Defaults to <c>nullptr</c>, which chooses the end of the list.
/// @param alibVersion      The \alib version required.
///                         Defaults to \ref ALIB_VERSION and \b must not be passed when invoking
///                         this function.
/// @param alibRevision     The \alib sub-version required.
///                         Defaults to \ref ALIB_REVISION and \b must not be passed when invoking
///                         this function.
/// @param compilationFlags The flags as defined in invoking compilation unit.
///                         Defaults to
///                         \doxlinkproblem{group__GrpALibPreproMacros.html;gaac71b6511690f5af0abf5213b0434111;ALIB_COMPILATION_FLAGS;ALIB_COMPILATION_FLAGS}
///                         and must not be passed when
///                         invoking this function.
//==================================================================================================
ALIB_API
void     Bootstrap( BootstrapPhases     targetPhase,
                    lang::Camp*         targetCamp,
                    int                 alibVersion,
                    int                 alibRevision,
                    TCompilationFlags   compilationFlags);

//==================================================================================================
/// This function is usually invoked automatically and rightfully with the standard, parameterless
/// version \ref alib::Shutdown().
///
/// For details on when and how to invoke this method explicitly, see the detailed explanations in
/// chapter \ref alib_manual_bootstrapping "4. Bootstrapping And Shutting Down ALib" of
/// the \ref alib_manual.
///
/// @param targetPhase The termination level to perform.
/// @param targetCamp  The "lowest" \alibcamp in list \alib{CAMPS} to shut down.
///                    Defaults to <c>nullptr</c>, which chooses the start of the list.
//==================================================================================================
ALIB_API
void     Shutdown( ShutdownPhases targetPhase,
                   lang::Camp*    targetCamp);

} // namespace [alib]
#endif // HPP_ALIB_LANG_CAMP_BOOTSTRAP

