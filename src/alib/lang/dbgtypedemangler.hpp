/** ************************************************************************************************
 * \file
 * This header file is part of the \aliblong. It does not belong to an \alibmod and is
 * included in any \alibdist.
 *
 * \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 *
 * \note
 *   To reduce complexity, this header is not shown in inclusion graphs of this documentation.
 **************************************************************************************************/
#ifndef HPP_ALIB_LANG_DBGTYPEDEMANGLER
#define HPP_ALIB_LANG_DBGTYPEDEMANGLER 1

#if !defined(HPP_ALIB) && !defined(ALIB_DOX)
#   include "alib/alib.hpp"
#endif

#if ALIB_DEBUG

// #################################################################################################
// Type De-mangling
// #################################################################################################
#if !defined (_TYPEINFO) && !defined(_TYPEINFO_)
    #include <typeinfo>
#endif

namespace alib::lang {

/** ********************************************************************************************
 * Retrieves human readable names from C++ run-time type information.<br>
 * This class is available only with debug builds of \alib.
 **********************************************************************************************/
class DbgTypeDemangler
{
    protected:
        /** The translated name name. */
        const char*  name;

    public:
        /** ************************************************************************************
         * Constructor
         * @param typeInfo The information struct on the C++ type.
         **************************************************************************************/
        ALIB_API
        DbgTypeDemangler( const std::type_info& typeInfo );

        /** ************************************************************************************
         * Destructor
         **************************************************************************************/
        ALIB_API
        ~DbgTypeDemangler();

        /** ************************************************************************************
         * Returns the demangled, human readable name of the type which was provided in the
         * constructor.
         * @return The demangled type name.
         **************************************************************************************/
        ALIB_API
        const char*  Get();
}; // class DbgTypeDemangler

} // namespace [alib::lang]

#endif // ALIB_DEBUG

#endif // HPP_ALIB_LANG_DBGTYPEDEMANGLER
