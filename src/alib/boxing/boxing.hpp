//==================================================================================================
/// \file
/// This header file is part of module \alib_boxing of the \aliblong.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_BOXING_BOXING
#define HPP_ALIB_BOXING_BOXING 1
#pragma once
#if !defined(DOXYGEN)
#   include "alib/alib.hpp"
#endif

ALIB_ASSERT_MODULE(BOXING)


#include "alib/lang/tmp.hpp"
#include "alib/lang/integers.hpp"
#include "alib/singletons/singleton.hpp"
#include "alib/characters/chartraits.hpp"

#if ALIB_STRINGS
#   include "alib/strings/fwds.hpp"
#endif

#include "alib/boxing/fwds.hpp"
#include "alib/lang/commonenumdefs.hpp"
#include <typeindex>

//##################################################################################################
//###################################        INL Headers         ###################################
//##################################################################################################

#include "alib/boxing/placeholder.inl"
#include "alib/boxing/typetraits.inl"
#include "alib/boxing/functions.inl"
#include "alib/boxing/detail/vtable.inl"
#include "alib/boxing/customizations.inl"
#include "alib/boxing/box.inl"
#include "alib/boxing/boxes.inl"
#include "alib/boxing/functiondefs.inl"

//##################################################################################################
//########      Namespace functions  (Bootstrap, BootstrapRegister, RegisterDefault)      ##########
//##################################################################################################
#include "alib/lang/callerinfo_functions.hpp"
namespace alib {  namespace boxing {

//==================================================================================================
/// This method needs to be called with bootstrapping a process.
/// It mainly registers default and type-specific implementations of the built-in
/// \ref alib_boxing_functions "box-functions".
/// In addition, in debug-compilations, \ref alib_boxing_more_opt_staticvt "static vtables" are
/// registered.
///
/// The \ref alib_manual_bootstrapping "standard bootstrap" code of \alib, hence the (overloaded)
/// functions \ref alib::Bootstrap will call this function.
///
/// Multiple invocations of this method are forbidden.
///
/// \see
///   For information about using this method, consult chapter
///   \ref alib_manual_bootstrapping_nocamps of the \ref alib_manual.
//==================================================================================================
ALIB_API
void        Bootstrap();

//==================================================================================================
/// Frees resources and shuts down module \alib_boxing.
/// Multiple invocations of this method are forbidden.
/// The \ref alib_manual_bootstrapping "standard bootstrap" code of \alib, hence the (overloaded)
/// functions \ref alib::Shutdown will call this function.
///
/// \see
///   Sibling function \alib{boxing::Bootstrap}.
//==================================================================================================
ALIB_API
void       Shutdown();

//==================================================================================================
/// Registers  \ref alib_boxing_functions "box-function" \p{function} of type \p{TFDecl} for
/// boxes of mapped type \p{TMapping}.
///
/// \attention
///   Function registration and function invocation are not protected against racing conditions
///   of multithreaded access. For this reason, it is advised to invoke this function exclusively
///   while \ref alib_manual_bootstrapping "bootstrapping" a software, when no threads are started,
///   yet. Registrations can be made before bootstrapping \alib, respectively during or after
///   phase \alib{BootstrapPhases::PrepareResources}.
///
/// \attention
///   If for any reason registration is performed \b after bootstrapping \alib and module
///   \alib_monomem is included in the \alibdist, and this function is invoked after
///   \alib was bootstrapped, then before an invocation of this method, mutex
///   \alib{monomem;GLOBAL_ALLOCATOR_LOCK} has to be acquired. This can be done with:
///           \snippet "ut_monomem.cpp"     DOX_MONOMEM_LOCK_GLOBALALLOCATOR
///
/// \attention
///   Note that even when this lock is set, still multithreaded access to registration and/or
///   box-function invocations is <b>not allowed</b>.
///
/// @tparam TFDecl    The \ref alib_boxing_functions_concepts_decl "type of function" to register.
/// @tparam TMapping  The mapped type that boxes store, which are to be equipped with a specialized
///                   function implementation.<br>
///                   Must be either one of \alib{boxing;TMappedTo;TMappedTo<T>} or
///                   \alib{boxing;TMappedToArrayOf;TMappedToArrayOf<T>}.
/// @param  function  Pointer to the function implementation.
//==================================================================================================
template<typename TFDecl, typename TMapping>
inline
void BootstrapRegister( typename TFDecl::Signature function )
{
    ALIB_ASSERT_ERROR(
        nullptr == detail::T_VTableFactory<TMapping>::Get()->Functions.template Get<TFDecl>(false),
        "BOXING", "Doubly defined function" )

    detail::T_VTableFactory<TMapping>::Get()->Functions.template Set<TFDecl>( function );
}

//==================================================================================================
/// Registers a default implementation of a \ref alib_boxing_functions "box-function", which
/// is invoked if no type-specific implementation is registered for a mapped type.
///
/// \attention
///   Function registration and function invocation are not protected against racing conditions
///   of multithreaded access. For this reason, it is advised to invoke this function exclusively
///   while \ref alib_manual_bootstrapping "bootstrapping" a software, when no threads are started,
///   yet. Registrations can be made before bootstrapping \alib, respectively during or after
///   phase \alib{BootstrapPhases::PrepareResources}.
///
/// \attention
///   If for any reason registration is performed \b after bootstrapping \alib and module
///   \alib_monomem is included in the \alibdist, and this function is invoked after
///   \alib was bootstrapped, then, before an invocation of this method, mutex
///   \alib{monomem;GLOBAL_ALLOCATOR_LOCK} has to be acquired. This can be done with:
///           \snippet "ut_monomem.cpp"     DOX_MONOMEM_LOCK_GLOBALALLOCATOR
///
/// \attention
///   Note that even when this lock is set, still multithreaded access to registration and/or
///   box-function invocations is <b>not allowed</b>.
///
/// @tparam TFDecl    The \ref alib_boxing_functions_concepts_decl "type of function" to register.
/// @param  function  Pointer to the function's default implementation.
//==================================================================================================
template<typename TFDecl>
inline
void BootstrapRegisterDefault( typename TFDecl::Signature function )
{
    detail::DEFAULT_FUNCTIONS.Set<TFDecl>( function );
}

}} // namespace [alib::boxing]
#include "alib/lang/callerinfo_methods.hpp"


// #################################################################################################
// T_Append<Box>
// #################################################################################################
#if ALIB_STRINGS
#define HPP_ALIB_STRINGS_TASTRING_INLINING
#include "alib/strings/detail/tastring.inl"
#undef HPP_ALIB_STRINGS_TASTRING_INLINING

namespace alib {  namespace strings {

// Faking all template specializations of namespace strings for doxygen into namespace
// strings::APPENDABLES to keep the documentation of namespace string clean!
#if DOXYGEN
namespace APPENDABLES {
#endif

    /// Specialization of template functor \alib{strings;T_Append} for type \alib{boxing;Box}.
    /// @tparam TChar      The character type of the target \b %Astring.
    /// @tparam TAllocator The allocator that the target \b %AString uses, as prototyped with
    ///                    \alib{lang;Allocator}.
    template<typename TChar, typename TAllocator>  struct T_Append<boxing::Box, TChar, TAllocator>
    {
        //==========================================================================================
        /// Writes the given boxed object. This is done by invoking box-function
        /// \alib{boxing;FAppend} on box \p{value}.
        ///
        /// @param target  The \b AString that \b Append was invoked on.
        /// @param box     The box to its contents to \p{target}.
        //==========================================================================================
        void operator()( TAString<TChar,TAllocator>& target, const boxing::Box& box )
        {
            box.Call<boxing::FAppend<TChar,TAllocator>>( target );
        }
    };

#if DOXYGEN
}    // namespace alib::strings[::appendables]
#endif

}}  // namespace [alib::strings]

#endif // ALIB_STRINGS

#endif // HPP_ALIB_BOXING_BOXING



