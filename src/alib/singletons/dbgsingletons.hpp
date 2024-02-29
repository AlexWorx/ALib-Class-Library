/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_singletons of the \aliblong.
 *
 * \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_SINGLETONS_DBGSINGLETONS
#define HPP_ALIB_SINGLETONS_DBGSINGLETONS 1

#if  !defined(HPP_ALIB_SINGLETONS_SINGLETON)
#   include "alib/singletons/singleton.hpp"
#endif

#if ALIB_FEAT_SINGLETON_MAPPED && ALIB_DEBUG

#if !defined(HPP_ALIB_COMPATIBILITY_STD_TYPEINFO)
#   include "alib/compatibility/std_typeinfo.hpp"
#endif

#if ALIB_MONOMEM
#   if !defined(HPP_ALIB_MONOMEM_HASHMAP)
#      include "alib/monomem/hashmap.hpp"
#   endif
#   if !defined(HPP_ALIB_MONOMEM_HASHSET)
#      include "alib/monomem/hashset.hpp"
#   endif
#else
#   if !defined(_GLIBCXX_UNORDERED_MAP) && !defined(_UNORDERED_MAP_)
#      include <unordered_map>
#   endif
#   if !defined (_GLIBCXX_MUTEX) && !defined(_MUTEX_)
#      include <mutex>
#   endif
#   if !defined (_GLIBCXX_CSTRING) && !defined(_CSTRING_)
#      include <cstring>
#   endif
#endif
namespace alib {  namespace singletons {

#if ALIB_MONOMEM
    /** ********************************************************************************************
     * This debug helper function returns a type map with void pointers to all singletons.
     *
     * The function may be used to investigate which singleton objects are created within a
     * process and the point in (run-) time of creation.
     *
     * \note
     *   This method is available only with debug builds of %ALib and if
     *   code selection symbol \ref ALIB_FEAT_SINGLETON_MAPPED evaluates to true.<br>
     *   The method included with \alibheader{singletons/dbgsingletons.hpp}.
     *
     * \note
     *   If the \alibdist includes \alib_strings then a simple dumping method is available with
     *   \alib{singletons,DbgGetSingletons(NAString&)}.<br>
     *
     * \attention
     *   If module \alib_monomem is not included in the \alibdist, then the result type of this
     *   method changes to <c>std::unordered_map<TypeKey, void*>&</c>.
     *
     *
     * @return A map of singleton types with void pointers to the singletons.
     **********************************************************************************************/
    ALIB_API  HashMap           <TypeFunctors::Key, void*,
                                 TypeFunctors::Hash,
                                 TypeFunctors::EqualTo     >&   DbgGetSingletons();
#else
    ALIB_API  std::unordered_map<TypeFunctors::Key, void*,
                                 TypeFunctors::Hash,
                                 TypeFunctors::EqualTo     >&   DbgGetSingletons();
#endif

    #if ALIB_STRINGS
         /** ***************************************************************************************
         * This debug function writes all type names and addresses of each currently defined
         * instance of class
         * \alib{singletons,Singleton}
         * into the given AString.<br>
         *
         * \note
         *   This method is available only
         *   - with \ref ALIB_DEBUG "debug builds" of the software,
         *   - if module \ref alib::strings "ALib Strings" is included in the \alibdist, and
         *   - if code selection symbol \ref ALIB_FEAT_SINGLETON_MAPPED is \c true.
         *
         *   The method included with \alibheader{singletons/dbgsingletons.hpp}.
         *
         * \note
         *   In case module <b>%ALib Strings</b> is not available, then alternative method
         *  \alib{singletons,DbgGetSingletons} can be used. This returns a list of
         *  \c std::type_info structs together with (void-) pointers to the singletons.
         *
         * @param target The target string to write the list of singletons to.
         *
         * @return The number of singletons written.
         ******************************************************************************************/
        ALIB_API  int           DbgGetSingletons( NAString& target );
    #endif // ALIB_STRINGS

}} // namespace [alib::singletons]


#endif // ALIB_FEAT_SINGLETON_MAPPED && ALIB_DEBUG
#endif // HPP_ALIB_SINGLETONS_DBGSINGLETONS
