// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#ifndef HPP_ALIB_BOXING_BOXING
#define HPP_ALIB_BOXING_BOXING 1

#if !defined(HPP_ALIB_BOXING_PREDEF)
#   include "alib/boxing/boxing_predef.hpp"
#endif

ALIB_ASSERT_MODULE(BOXING)


#if !defined(HPP_ALIB_LIB_PREDEF_TMP)
#   include "alib/lib/predef_tmp.hpp"
#endif

#if !defined(HPP_ALIB_LIB_ASSERT)
#   include "alib/lib/assert.hpp"
#endif

#if !defined(HPP_ALIB_LIB_INTEGERS)
#   include "alib/lib/integers.hpp"
#endif

#if !defined(HPP_ALIB_LIB_COMMONENUMS)
#   include "alib/lib/commonenums.hpp"
#endif

#if !defined(HPP_ALIB_SINGLETONS_SINGLETON)
#   include "alib/singletons/singleton.hpp"
#endif

#if !defined(HPP_ALIB_CHARACTERS_CHARACTERS)
#   include "alib/characters/characters.hpp"
#endif

#if ALIB_MODULE_STRINGS && !defined(HPP_ALIB_STRINGS_FWDS)
#   include "alib/strings/fwds.hpp"
#endif

#if !defined(HPP_ALIB_BOXING_FWDS)
#   include "alib/boxing/fwds.hpp"
#endif


#if ALIB_DEBUG
#   if !defined(HPP_ALIB_LIB_TYPEMAP)
#       include "alib/lib/typemap.hpp"
#   endif

#   if !defined(HPP_ALIB_LIB_TYPEDEMANGLER)
#       include "alib/lib/typedemangler.hpp"
#   endif
#endif

#if !defined (_GLIBCXX_TYPEINDEX) && !defined(_TYPEINDEX_)
#   include <typeindex>
#endif


//##################################################################################################
//###################################        INL Headers         ###################################
//##################################################################################################

#define HPP_ALIB_BOXING_PROPPERINCLUDE
#   include "alib/boxing/placeholder.inl"
#   include "alib/boxing/typetraits.inl"
#   include "alib/boxing/functions.inl"
#   include "alib/boxing/detail/vtable.inl"
#   include "alib/boxing/customizations.inl"
#   include "alib/boxing/box.inl"
#   include "alib/boxing/boxes.inl"
#   include "alib/boxing/functiondefs.inl"
#undef HPP_ALIB_BOXING_PROPPERINCLUDE


namespace aworx { namespace lib { namespace boxing {

//##################################################################################################
//#############      Namespace functions  (Init, Register, RegisterDefault)      ###################
//##################################################################################################

/** ************************************************************************************************
 * This method needs to be called with bootstrapping a software.
 * It mainly registers default and type-specific implementations of the built-in
 * \ref alib_boxing_functions "box-functions".
 * In addition, in debug-compilations, \ref alib_boxing_more_opt_staticvt "static vtables" are
 * registered.
 *
 * The \ref alib_manual_bootstrapping "standard bootstrap" code of \alib will perform this.
 * Only if \alibmod_nolink_boxing is used extracted from the overall set of \alibmods, this
 * function has to be invoked "manually".
 *
 * Multiple invocations of this method are ignored.
 *
 * \see
 *   For information about using this method, consult chapter
 *   \ref alib_manual_bootstrapping_smallmods of the \ref alib_manual.
 * ************************************************************************************************/
void        Init();

/** ************************************************************************************************
 * Namespace function used for the registration of type-specific implementations of
 * \ref alib_boxing_functions "box-functions".
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
void Register( typename TFDecl::Signature function )
{
    detail::T_VTableFactory<TMapping>::Get()->Functions.template Set<TFDecl>( function );
}

/** ************************************************************************************************
 * Namespace function used for the registration of a default implementation of a
 * \ref alib_boxing_functions "box-function".
 *
 * @tparam TFDecl    The \ref alib_boxing_functions_concepts_decl "type of function" to register.
 * @param  function  Pointer to the function's default implementation.
 * ************************************************************************************************/
template<typename TFDecl>
inline
void RegisterDefault( typename TFDecl::Signature function )
{
    detail::DEFAULT_FUNCTIONS.Set<TFDecl>( function );
}


}}} // namespace [aworx::lib::boxing]


// #################################################################################################
// T_Append<Box>
// #################################################################################################
#if ALIB_MODULE_STRINGS
#include "alib/strings/astring.hpp"

namespace aworx { namespace lib { namespace strings {

// We are faking all template specializations of namespace strings for doxygen into namespace
// strings::APPENDABLES to keep the documentation of namespace string clean!
#if ALIB_DOCUMENTATION_PARSER
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

#if ALIB_DOCUMENTATION_PARSER
}    // namespace aworx::lib::strings[::appendables]
#endif

}}}  // namespace [aworx::lib::strings]

#endif // ALIB_MODULE_STRINGS

#endif // HPP_ALIB_BOXING_BOXING

