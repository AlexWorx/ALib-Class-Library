/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_files of the \aliblong.
 *
 * \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_FILES_FTOOLS
#define HPP_ALIB_FILES_FTOOLS 1

#if !defined (HPP_ALIB_FILES_FINFO)
#   include "alib/files/finfo.hpp"
#endif

namespace alib { namespace files {

/** ************************************************************************************************
 * Simple helper class to resolve owner and group ids to strings names. The simple approach
 * here is to store the last recently requested ID/name pairs and return this if a subsequent
 * request asks for the same id. Otherwise, a system call is performed.
 **************************************************************************************************/
class OwnerAndGroupResolver
{
  protected:
    FInfo::TOwnerAndGroupID    lastUID; ///< The last requested user ID..
    FInfo::TOwnerAndGroupID    lastGID; ///< The last requested group ID.
    AString             lastUIDResult;  ///< The result associated with #lastUID.
    AString             lastGIDResult;  ///< The result associated with #lastGID.

  public:
    /** Constructor */
    OwnerAndGroupResolver()
    : lastUID(FInfo::UnknownID), lastGID(FInfo::UnknownID)
    , lastUIDResult("?"), lastGIDResult("?")
    {}


    ALIB_API
    /**
     * Writes owner and group names of \p{finfo} to the given strings.
     * @param finfo     The file to examine.
     * @return A pair of strings, denoting the owner and group name.
     */
    std::pair<String,String> Get( const FInfo& finfo );

}; //class OwnerAndGroupResolver

} // namespace alib[::files]


/// Type alias in namespace \b alib.
using     OwnerAndGroupResolver =   files::OwnerAndGroupResolver;

} // namespace [alib]

#endif // HPP_ALIB_FILES_FTOOLS
