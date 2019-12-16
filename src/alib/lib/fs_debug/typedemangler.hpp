/** ************************************************************************************************
 * \file
 * This header file  is part of file set \alibfs_debug of the \aliblong.
 *
 * \emoji :copyright: 2013-2019 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_FS_DEBUG_TYPEDEMANGLER
#define HPP_ALIB_FS_DEBUG_TYPEDEMANGLER 1

#if !defined(HPP_ALIB_COMPILERS) && !defined(ALIB_DOX)
#   include "alib/lib/compilers.hpp"
#endif

ALIB_ASSERT_FILESET(DEBUG)


#if !defined (_TYPEINFO) && !defined(_TYPEINFO_)
    #include <typeinfo>
#endif


#if ALIB_DEBUG

namespace aworx { namespace lib {

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


}} // namespace [aworx::lib}

#endif // ALIB_DEBUG

#endif // HPP_ALIB_FS_DEBUG_TYPEDEMANGLER
