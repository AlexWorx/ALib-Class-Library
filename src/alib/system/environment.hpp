// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
/** @file */ // Hello Doxygen


// check for alib.hpp already there but not us
#if !defined (HPP_ALIB)
    #error "include \"alib/alib.hpp\" before including this header"
#endif
#if defined(HPP_COM_ALIB_TEST_INCLUDES) && defined(HPP_ALIB_SYSTEM_ENVIRONMENT)
    #error "Header already included"
#endif

// then, set include guard
#ifndef HPP_ALIB_SYSTEM_ENVIRONMENT
//! @cond NO_DOX
#define HPP_ALIB_SYSTEM_ENVIRONMENT 1
//! @endcond

namespace aworx { namespace lib { namespace system
{

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
 *   Defaults to \b Cases::Sensitive. An compile time assertion is given when \b Case::Ignore
 *   is passed, because this is not supported. In release compilations, the parameter is ignored.
 *
 *  @param varName      The name of the variable to be retrieved.
 *  @param target       An AString to write the result to.
 *  @param targetData   If \c CurrentData::Keep, the parameter \p{target} is not cleared
 *                      before the result is written. Defaults to \c CurrentData::Clear.
 *  @param sensitivity  Use case sensitive mode for variable names. Not supported under
 *                      Microsoft Windows OS. Defaults to
 *                      \ref aworx::lib::lang::Case::Ignore "Case::Ignore".
 *  @return \c true if variable was found.
 **************************************************************************************************/
ALIB_API extern
bool  GetEnvironmentVariable(  const TString&    varName,
                               AString&          target,
                               lang::CurrentData targetData  =lang::CurrentData::Clear,
                            #if defined (__unix__)
                               lang::Case        sensitivity =lang::Case::Sensitive
                            #elif defined(__APPLE__)
                               lang::Case        sensitivity =lang::Case::Sensitive
                            #elif defined(_WIN32)
                               lang::Case        sensitivity =lang::Case::Ignore
                            #else
                                #pragma message ("Unknown Platform in file: " __FILE__ )
                            #endif
                               );
}}} // namespace lib::system

#endif // HPP_ALIB_SYSTEM_ENVIRONMENT
