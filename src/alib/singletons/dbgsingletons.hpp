//==================================================================================================
/// \file
/// This header file is part of module \alib_singletons of the \aliblong.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_SINGLETONS_DBGSINGLETONS
#define HPP_ALIB_SINGLETONS_DBGSINGLETONS 1
#pragma once
#include "alib/singletons/singleton.hpp"
#if ALIB_FEAT_SINGLETON_MAPPED && ALIB_DEBUG

#include "alib/compatibility/std_typeinfo.hpp"
#if ALIB_MONOMEM  && ALIB_CONTAINERS
#   include "alib/containers/hashtable.hpp"
#   include "alib/monomem/monoallocator.hpp"
#else
#   include <unordered_map>
#   include <mutex>
#   include <cstring>
#endif
namespace alib::singletons {

#if ALIB_MONOMEM && ALIB_CONTAINERS
    //==============================================================================================
    /// This debug helper function returns a type map with void pointers to all singletons.
    ///
    /// The function may be used to investigate which singleton objects are created within a
    /// process and the point in (run-) time of creation.
    ///
    /// \note
    ///   This method is available only with debug-builds of %ALib and if
    ///   code selection symbol \ref ALIB_FEAT_SINGLETON_MAPPED evaluates to true.<br>
    ///   The method included with \alibheader{singletons/dbgsingletons.hpp}.
    ///
    /// \note
    ///   If the \alibdist includes \alib_strings then a simple dumping method is available with
    ///   #DbgGetSingletons(NAString&).
    ///
    /// \attention
    ///   If modules \alib_monomem and \alib_containers are not included in the \alibdist, then the
    ///   result type of this method changes to <c>std::unordered_map<TypeKey, void*>&</c>.
    ///
    /// @return A map of singleton types with void pointers to the singletons.
    //==============================================================================================
    ALIB_API  HashMap           <MonoAllocator,
                                 TypeFunctors::Key, void*,
                                 TypeFunctors::Hash,
                                 TypeFunctors::EqualTo,
                                 lang::Caching::Auto,
                                 Recycling::None           >&   DbgGetSingletons();
#else
    ALIB_API  std::unordered_map<TypeFunctors::Key, void*,
                                 TypeFunctors::Hash,
                                 TypeFunctors::EqualTo     >&   DbgGetSingletons();
#endif

    #if ALIB_STRINGS
         //=========================================================================================
        /// This debug function writes all type names and addresses of each currently defined
        /// instance of class
        /// \alib{singletons;Singleton}
        /// into the given AString.<br>
        ///
        /// \note
        ///   This method is available only
        ///   - with \ref ALIB_DEBUG "debug-builds" of the software,
        ///   - if module \ref alib::strings "ALib Strings" is included in the \alibdist, and
        ///   - if code selection symbol \ref ALIB_FEAT_SINGLETON_MAPPED is \c true.
        ///
        ///   The method included with \alibheader{singletons/dbgsingletons.hpp}.
        ///
        /// \note
        ///   In case module <b>%ALib Strings</b> is not available, then alternative method
        ///  \alib{singletons;DbgGetSingletons} can be used. This returns a list of
        ///  \c std::type_info structs together with (void-) pointers to the singletons.
        ///
        /// @param target The target string to write the list of singletons to.
        ///
        /// @return The number of singletons written.
         //=========================================================================================
        ALIB_API  int           DbgGetSingletons( NAString& target );
    #endif // ALIB_STRINGS

} // namespace [alib::singletons]


#endif // ALIB_FEAT_SINGLETON_MAPPED && ALIB_DEBUG
#endif // HPP_ALIB_SINGLETONS_DBGSINGLETONS

