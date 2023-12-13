/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_system of the \aliblong.
 *
 * \emoji :copyright: 2013-2023 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_SYSTEM_ENVIRONMENT
#define HPP_ALIB_SYSTEM_ENVIRONMENT 1

#if !defined (HPP_ALIB_STRINGS_ASTRING)
#   include "alib/strings/astring.hpp"
#endif

ALIB_ASSERT_MODULE(SYSTEM)

#if !defined (HPP_ALIB_STRINGS_CSTRING)
#   include "alib/strings/cstring.hpp"
#endif

namespace aworx { namespace lib { namespace system {

/** ************************************************************************************************
 * Simple struct providing one static method that reads an environment variable into an
 * \alib string type.
 *
 * While under posix compatible operating systems, environment variable names are
 * case sensitive, under Microsoft windows they are not.
 **************************************************************************************************/
struct EnvironmentVariables
{
    /** ************************************************************************************************
     * Reads an environment variable into an \alib string.
     *
     * \attention
     *    1. While under posix compatible operating systems, environment variable names are
     *       case sensitive, under Microsoft windows they are not.
     * \attention
     *    2. As documented with
     *    \https{std::getenv,en.cppreference.com/w/cpp/utility/program/getenv}, the operation is
     *    not thread safe against changes of the environment in parallel threads.
     *
     *  @param varName      The name of the variable to be retrieved.
     *  @param target       An AString to write the result to.
     *  @param targetData   If \c CurrentData::Keep, the parameter \p{target} is not cleared
     *                      before the result is written. Defaults to \c CurrentData::Clear.
     *  @return \c true if variable was found.
     **************************************************************************************************/
    ALIB_API static
    bool  Get(  const CString&    varName,
                AString&          target,
                CurrentData       targetData  =CurrentData::Clear  );
};

}} // namespace lib::system

/// Type alias in namespace #aworx.
using     EnvironmentVariables=       lib::system::EnvironmentVariables;

}  // namespace [aworx]

#endif // HPP_ALIB_SYSTEM_ENVIRONMENT
