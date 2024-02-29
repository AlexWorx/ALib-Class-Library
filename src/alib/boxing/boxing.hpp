/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_boxing of the \aliblong.
 *
 * \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_BOXING_BOXING
#define HPP_ALIB_BOXING_BOXING 1

#if !defined(HPP_ALIB) && !defined(ALIB_DOX)
#   include "alib/alib.hpp"
#endif

ALIB_ASSERT_MODULE(BOXING)


#if !defined(HPP_ALIB_LANG_TMP) && !defined(ALIB_DOX)
#   include "alib/lang/tmp.hpp"
#endif

#if !defined(HPP_ALIB_LANG_INTEGERS)
#   include "alib/lang/integers.hpp"
#endif

#if !defined(HPP_ALIB_SINGLETONS_SINGLETON)
#   include "alib/singletons/singleton.hpp"
#endif

#if !defined(HPP_ALIB_CHARACTERS_CHARACTERS)
#   include "alib/characters/characters.hpp"
#endif

#if ALIB_STRINGS && !defined(HPP_ALIB_STRINGS_FWDS)
#   include "alib/strings/fwds.hpp"
#endif

#if !defined(HPP_ALIB_BOXING_FWDS)
#   include "alib/boxing/fwds.hpp"
#endif

#if !defined(HPP_ALIB_LANG_COMMONENUMS_DEFS)
#   include "alib/lang/commonenumdefs.hpp"
#endif

#if !defined (_GLIBCXX_TYPEINDEX) && !defined(_TYPEINDEX_)
#   include <typeindex>
#endif


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
namespace alib {  namespace boxing {

/** ************************************************************************************************
 * This method needs to be called with bootstrapping a software.
 * It mainly registers default and type-specific implementations of the built-in
 * \ref alib_boxing_functions "box-functions".
 * In addition, in debug-compilations, \ref alib_boxing_more_opt_staticvt "static vtables" are
 * registered.
 *
 * The \ref alib_manual_bootstrapping "standard bootstrap" code of \alib, hence the (overloaded)
 * functions \ref alib::Bootstrap will call this function.
 *
 * Multiple invocations of this method are ignored.
 *
 * \see
 *   For information about using this method, consult chapter
 *   \ref alib_manual_bootstrapping_nocamps of the \ref alib_manual.
 * ************************************************************************************************/
ALIB_API
void        Bootstrap();

/** ************************************************************************************************
 * Registers  \ref alib_boxing_functions "box-function" \p{function} of type \p{TFDecl} for
 * boxes of mapped type \p{TMapping}.
 *
 * \attention
 *   Function registration and function invocation are not protected against racing conditions
 *   of multi-threaded access. For this reason, it is advised to invoke this function exclusively
 *   while \ref alib_manual_bootstrapping "bootstrapping" a software, when no threads are started,
 *   yet. Registrations can be made prior to bootstrapping \alib, respectively during or after
 *   phase \alib{BootstrapPhases::PrepareResources}.
 *
 * \attention
 *   If for any reason registration is performed \b after bootstrapping \alib and module
 *   \alib_monomem is included in the \alibdist, and this function is invoked after
 *   \alib was bootstrapped, then prior to an invocation of this method, mutex
 *   \alib{monomem,GlobalAllocatorLock} has to be acquired. This can be done with:
 *
 *           ALIB_LOCK_WITH( alib::monomem::GlobalAllocatorLock )
 *
 * \attention
 *   Note that even when this lock is set, still multi-threaded access to registration and/or
 *   box-function invocations is <b>not allowed</b>.
 *
 * @tparam TFDecl    The \ref alib_boxing_functions_concepts_decl "type of function" to register.
 * @tparam TMapping  The mapped type that boxes store, which are to be equipped with a specialized
 *                   function implementation.<br>
 *                   Must be either one of \alib{boxing,TMappedTo,TMappedTo<T>} or
 *                   \alib{boxing,TMappedToArrayOf,TMappedToArrayOf<T>}.
 * @param  function  Pointer to the function implementation.
 * ************************************************************************************************/
template<typename TFDecl, typename TMapping>
inline
void BootstrapRegister( typename TFDecl::Signature function )
{
    ALIB_ASSERT_ERROR( nullptr == detail::T_VTableFactory<TMapping>::Get()->Functions.template Get<TFDecl>(false),
                       "BOXING", "Doubly defined function" )

    detail::T_VTableFactory<TMapping>::Get()->Functions.template Set<TFDecl>( function );
}

/** ************************************************************************************************
 * Registers a default implementation of a \ref alib_boxing_functions "box-function", which
 * is invoked if no type-specific implementation is registered for a mapped type.
 *
 * \attention
 *   Function registration and function invocation are not protected against racing conditions
 *   of multi-threaded access. For this reason, it is advised to invoke this function exclusively
 *   while \ref alib_manual_bootstrapping "bootstrapping" a software, when no threads are started,
 *   yet. Registrations can be made prior to bootstrapping \alib, respectively during or after
 *   phase \alib{BootstrapPhases::PrepareResources}.
 *
 * \attention
 *   If for any reason registration is performed \b after bootstrapping \alib and module
 *   \alib_monomem is included in the \alibdist, and this function is invoked after
 *   \alib was bootstrapped, then prior to an invocation of this method, mutex
 *   \alib{monomem,GlobalAllocatorLock} has to be acquired. This can be done with:
 *
 *           ALIB_LOCK_WITH( alib::monomem::GlobalAllocatorLock )
 *
 * \attention
 *   Note that even when this lock is set, still multi-threaded access to registration and/or
 *   box-function invocations is <b>not allowed</b>.
 *
 * @tparam TFDecl    The \ref alib_boxing_functions_concepts_decl "type of function" to register.
 * @param  function  Pointer to the function's default implementation.
 * ************************************************************************************************/
template<typename TFDecl>
inline
void BootstrapRegisterDefault( typename TFDecl::Signature function )
{
    detail::DEFAULT_FUNCTIONS.Set<TFDecl>( function );
}


}} // namespace [alib::boxing]


// #################################################################################################
// T_Append<Box>
// #################################################################################################
#if ALIB_STRINGS
#include "alib/strings/astring.hpp"

namespace alib {  namespace strings {

// Faking all template specializations of namespace strings for doxygen into namespace
// strings::APPENDABLES to keep the documentation of namespace string clean!
#if defined(ALIB_DOX)
namespace APPENDABLES {
#endif

    /**
     * Specialization of template functor \alib{strings,T_Append} for type \alib{boxing,Box}.
     */
    template<typename TChar>  struct T_Append<boxing::Box, TChar>
    {
        /** ****************************************************************************************
         * Writes the given boxed object. This is done by invoking box-function
         * \alib{boxing,FAppend} on box \p{value}.
         *
         * @param target  The \b AString that \b Append was invoked on.
         * @param box     The box to its contents to \p{target}.
         ******************************************************************************************/
        void operator()( TAString<TChar>& target, const boxing::Box& box )
        {
            box.Call<boxing::FAppend<TChar>>( target );
        }
    };

#if defined(ALIB_DOX)
}    // namespace alib::strings[::appendables]
#endif

}}  // namespace [alib::strings]

#endif // ALIB_STRINGS

#endif // HPP_ALIB_BOXING_BOXING
