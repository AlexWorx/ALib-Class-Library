/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_system of the \aliblong.
 *
 * \emoji :copyright: 2013-2019 A-Worx GmbH, Germany.
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
 * Returns the value of an environment variable.
 *
 * While under posix compatible operating systems, environment variable names are
 * case sensitive, under Microsoft windows they are not. This behavior is reflected by the
 * default value of parameter \p{sensitivity}. Its options are as follows:
 * - <b>GNU/Linux</b><br>
 *   Defaults to \b Cases::Sensitive. Can be changed by passing \b Case::Ignore.
 * - <b>Windows OS</b><br>
 *   Defaults to \b Cases::Ignore. A run-time assertion is thrown when \b Case::Sensitive
 *   is passed, because this is not supported. In release compilations, the parameter is ignored.
 * - <b>Mac OS</b><br>
 *   Defaults to \b Cases::Sensitive. A compile-time assertion is given when \b Case::Ignore
 *   is passed, because this is not supported. In release compilations, the parameter is ignored.
 *
 *  @param varName      The name of the variable to be retrieved.
 *  @param target       An AString to write the result to.
 *  @param targetData   If \c CurrentData::Keep, the parameter \p{target} is not cleared
 *                      before the result is written. Defaults to \c CurrentData::Clear.
 *  @param sensitivity  Use case sensitive mode for variable names. Not supported under
 *                      Microsoft Windows OS. Defaults to
 *                      \ref aworx::lib::Case::Ignore "Case::Ignore".
 *  @return \c true if variable was found.
 **************************************************************************************************/
ALIB_API extern
bool  GetEnvironmentVariable(  const CString&    varName,
                               AString&          target,
                               CurrentData targetData  =CurrentData::Clear,
                            #if defined (__unix__)
                               Case        sensitivity =Case::Sensitive
                            #elif defined(__APPLE__)
                               Case        sensitivity =Case::Sensitive
                            #elif defined(_WIN32)
                               Case        sensitivity =Case::Ignore
                            #else
                                #pragma message ("Unknown Platform in file: " __FILE__ )
                            #endif
                               );
}}} // namespace lib::system

#endif // HPP_ALIB_SYSTEM_ENVIRONMENT
