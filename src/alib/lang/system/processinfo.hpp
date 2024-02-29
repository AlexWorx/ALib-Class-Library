/** ************************************************************************************************
 * \file
 * This header file is part of sub-namespace #  alib::lang::system of module \alib_basecamp of
 * the \aliblong.
 *
 * \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_CAMP_PROCESSINFO
#define HPP_ALIB_CAMP_PROCESSINFO 1

#if !defined (HPP_ALIB_STRINGS_ASTRING)
#   include "alib/strings/astring.hpp"
#endif

ALIB_ASSERT_MODULE(CAMP)

namespace alib { namespace  lang::system {

/** ************************************************************************************************
 *  This class represents process information
 **************************************************************************************************/
class ProcessInfo
{
    // #############################################################################################
    // Fields
    // #############################################################################################
    protected:
        /** Information on the current process. This is initialized once, with the first invocation
         *  of #Current.   */
        ALIB_API static ProcessInfo    current;

    public:
        /** The process id as AString. */
        uinteger    PID;

        /** The command line which invoked this process. */
        AString     CmdLine;

        /** The name of the process.<br> Under GNU/Linux this is read from /proc/nnn/stat and may
         *  differ from #ExecFileName. Under Windows OS, it is he same as field #ExecFileName.  */
        AString     Name;

        /** The path of the executable (if available to us). */
        AString     ExecFilePath;

        /** The file name of the executable (excluding #ExecFilePath). Under GNU/Linux,
         *  if we have no access to read that value, field #ExecFilePath will be empty
         *  while this field is filled with #Name.
         */
        AString     ExecFileName;

    // #############################################################################################
    // Fields under unix like OS
    // #############################################################################################
    #if defined( __unix__ )  || defined(ALIB_DOX)

            /** The parent's process id as AString. (Unix like OS / Mac OS only.) */
            uinteger    PPID;

            /** The contents of /proc/PID/stat file. (Unix like OS only.) */
            AString     Stat;

            /** The state field (2) within \ref Stat. (Unix like OS only.) */
            AString     StatState;

            /** The process group field (4) within \ref Stat. (Unix like OS only.) */
            AString     StatPGRP;
    #endif

    // #############################################################################################
    // Fields under Mac OS
    // #############################################################################################
    #if defined( __APPLE__ )
            uinteger    PPID;
    #endif

    // #############################################################################################
    // Fields under Windows OS
    // #############################################################################################
    #if defined( _WIN32 ) || defined(ALIB_DOX)
        public:

             /** For console processes, this is the title displayed in the title bar. (Windows OS only.) */
            AString     ConsoleTitle;
    #endif


    // #############################################################################################
    // Protected methods
    // #############################################################################################
    protected:
        /** ****************************************************************************************
         * Default constructor to create an empty instance
         ******************************************************************************************/
                        ProcessInfo()                   {}


        #if defined( __unix__ )
            /** ************************************************************************************
             * Reads a field from the data found in /proc/PID/stat.
             *  @param fieldNo  The field number to read
             *  @param target   The variable to store the result in
             *  @returns \c true if successful.
             **************************************************************************************/
            bool        getStatField( int fieldNo, AString& target );
        #endif

        /** ************************************************************************************
         * Internal get() method with different platform implementations.
         *
         * @param PID  The ID of the process information is requested on.
         *             If a \e nulled string is provided, the current process is addressed.
         *
         * @return \c true on success, \c false otherwise.
         **************************************************************************************/
        ALIB_API bool  get( uinteger PID  );

    // #############################################################################################
    // Interface
    // #############################################################################################
    public:
        #if defined( __unix__ ) || defined(__APPLE__)
            /** ************************************************************************************
             * Constructor that initializes this instance according to the process information
             * received from the system.
             *
             * \note
             *   This method is implemented for Unix like OS (incl. Mac OS) only.
             *   On Windows OS, only information about the current process can be received
             *   using static method #Current.
             *
             * @param processID    The ID of the process information is requested on.
             *                     If a \e nulled string is provided, the current process is
             *                     addressed.
             ***************************************************************************************/
                                   ProcessInfo( uinteger processID  ) { get( processID ); }

            /** ************************************************************************************
             * Fills the fields of this instance according to the process information received from
             * the system.
             *
             * \note
             *   This method is implemented for Unix like OS (incl. Mac OS) only.
             *   On Windows OS, only information about the current process can be received
             *   using static method #Current.
             *
             * @param processID  The ID of the process information is requested on.
             *                   If a \e nulled string is provided, the current process is
             *                   addressed.
             *
             * @return \c true on success, \c false otherwise.
             **************************************************************************************/
            ALIB_API bool          Get( uinteger processID  ) { return get( processID ); }
        #endif

        /** ****************************************************************************************
         * Static method that receives information on the current process.
         *
         * @return A constant reference to a \c %ProcessInfo object representing the current process.
         ******************************************************************************************/
        ALIB_API static
        const ProcessInfo&         Current();
};

} // namespace [ lang::system]

/// Type alias in namespace \b alib.
using     ProcessInfo=        lang::system::ProcessInfo;

} // namespace [alib]

#endif // HPP_ALIB_CAMP_PROCESSINFO
