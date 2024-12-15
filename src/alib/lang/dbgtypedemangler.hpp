//==================================================================================================
/// \file
/// This header file is part of the \aliblong. It does not belong to an \alibmod and is
/// included in any \alibdist.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
///
/// \note
///   To reduce complexity, this header is not shown in inclusion graphs of this documentation.
//==================================================================================================
#ifndef HPP_ALIB_LANG_DBGTYPEDEMANGLER
#define HPP_ALIB_LANG_DBGTYPEDEMANGLER 1
#pragma once
#if !defined(DOXYGEN)
#   include "alib/alib.hpp"
#endif

#if ALIB_DEBUG

#if ALIB_STRINGS
#   include "alib/strings/fwds.hpp"
#endif

namespace alib::lang {

//==================================================================================================
/// Retrieves human-readable names from C++ run-time type information.<br>
/// This class is available only with debug-builds of \alib.
///
/// Module \alib_strings functor
/// \alib{strings::APPENDABLES;T_Append<std::type_info,TChar,TAllocator>} in debug-compilations,
/// which internally makes use of this type. This allows appending the result of keyword \c typeid
/// directly to \b AString instances.
//==================================================================================================
class DbgTypeDemangler
{
    protected:
        const char*     name;       ///< The translated name.
        bool            failed;     ///< If \c true, demangling failed.

    public:
        //==========================================================================================
        /// Constructor
        /// @param typeInfo The information struct on the C++ type.
        //==========================================================================================
        ALIB_API
        DbgTypeDemangler( const std::type_info& typeInfo );

    #if defined(__GNUC__) || defined(__clang__)
        //==========================================================================================
        /// Destructor
        //==========================================================================================
        ALIB_API
        ~DbgTypeDemangler();
    #endif

        //==========================================================================================
        /// Returns the demangled, human-readable name of the type which was provided in the
        /// constructor.
        /// @return The demangled type name.
        //==========================================================================================
        ALIB_API
        const char*  Get();

        #if ALIB_STRINGS
            //======================================================================================
            /// Removes all namespaces from the type receivable with #Get, including those of its
            /// nested templates, and writes the result to \p{target}.
            /// \par Availability
            ///   This method is available only if the module \alib_strings is included in the
            ///   \alibdist. Note that while method #Get always returns a narrow character
            ///   array, this method accepts the string-type chosen with the \alibdist_nl.
            /// @param target The target string to write to.
            /// @return The given \p{target} to allow concatenated operations.
            //======================================================================================
            ALIB_API
            NAString&    GetShort(NAString& target);
        #endif
}; // class DbgTypeDemangler

} // namespace [alib::lang]

#endif // ALIB_DEBUG

#endif // HPP_ALIB_LANG_DBGTYPEDEMANGLER

