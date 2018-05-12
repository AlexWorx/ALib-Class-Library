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
#if defined(HPP_COM_ALIB_TEST_INCLUDES) && defined(HPP_ALIB_CONFIG_INI_FILE)
    #error "Header already included"
#endif

// then, set include guard
#ifndef HPP_ALIB_CONFIG_INI_FILE
//! @cond NO_DOX
#define HPP_ALIB_CONFIG_INI_FILE 1
//! @endcond


// #################################################################################################
// includes
// #################################################################################################
#if !defined (HPP_ALIB_CONFIG_INMEMORY_PLUGIN)
    #include "alib/config/inmemoryplugin.hpp"
#endif

namespace aworx { namespace lib { namespace strings { namespace util {
class StringWriter;
}}}}


namespace aworx { namespace lib { namespace config {
/** ************************************************************************************************
 * Specialization of class #InMemoryPlugin, which reads and writes a simple configuration file
 * consisting of sections containing key/value pairs.
 *
 * This class is provided for the case that no other configuration mechanism is available.
 * In general, application specific configuration mechanisms already exist in other libraries
 * used. Those should be adopted by creating a lean interface plug-in for \alib.
 *
 * Some remarks on the functionality and supported format:
 * - Comments
 *   - Comment lines at the beginning of the file are associated with the file and are written
 *     back. Such comment block is stopped with a single blank line.
 *   - Lines that start (apart from whitespace) with either a double
 *     slash \c "//", a sharp sign \c '#' or a semicolon <c>';'</c> are comment lines.
 *   - Comment lines that are added to variables in the software are using the symbol defined in
 *     filed #DefaultCommentPrefix, which defaults to \c '#'. If this is changed in the file,
 *     such changes are preserved.
 *   - Comment lines before sections and variables are associated with the respective objects
 *     and are not overwritten by comments set in the code. However, variables without
 *     comments which are overwritten in code including comments, get such comment appended.
 *   - Comments can not reside in the same line together with section names or variables.
 *   - Commented variables receive a blank line before the comment.
 *   - Commented Sections receive two blank lines before the comment. One if they are not commented.
 *
 * - Sections:
 *   - Sections names are enclosed by brackets \c '[' and \c ']'.
 *   - Section names can be repeated. In this case the corresponding section is continued.
 *     When the file is written, the sections are merged. Otherwise the order of sections and
 *     the variables within the section is kept intact on writing.
 *
 * - Variables
 *   - Variable names and their values are separated by an equal sign \c '='.
 *   - New variables inserted are formatted according to other variables found. E.g. the
 *     equal sign of all variables within a section are aligned on the same column.
 *   - Formats of variables added or changed by the user are kept intact, as long as the
 *     software does not store a value.
 *
 * - Continued Lines:
 *   - Variables definition are being continued (values are concatenated) if the line ends
 *     with a backslash \c '\\'.
 *   - Comment lines in-between continued lines are recognized as such. To continue a variable
 *     after a 'continued' comment line, the comment line needs to end with a backslash \c '\\'.
 *   - Variables with multiple values created (or modified) in the software, are written
 *     in continued lines, with each line showing one value, ending with the variables'
 *     delimiter character and a trailing \c '\\'.
 *   - Comment lines within Variables defined in multiple lines are removed when a variable
 *     is written back.
 *
 * - Escaping values
 *   - Spaces <c>' '</c> and tabulators \c '\\t' are ignored at the start and end of each line and before
 *     and after the equal sign \c '='.
 *   - Consequently, whitespaces at the start or end of a value either need to be escaped
 *     using <c>'\\ '</c> or the whole value has to be surrounded by double quotes \c ".
 *   - Values added or modified by the software that contain spaces at the start or end
 *     are surrounded by double quotes (instead of escaping them)
 *   - Double quotes in values are always escaped when writing values and have to be escaped
 *     when editing the file.
 *   - Values may consist of a list of double quoted values. Whitespaces between such
 *     values are ignored. Consequently, long strings may be enclosed in double quotes
 *     and continued in the next line when the line ends with a backslash \c '\\'.
 *   - Almost any character can be escaped. E.g \c "\a" is read as \c 'a'.
 *   - On writing only non-printable characters and double quotation marks are escaped.
 *
 * - Other remarks
 *   - Sequences of blank lines are reduced to one blank line, when writing the file.
 *   - Erroneous lines are ignored and not written back. Line numbers with erroneous lines
 *     are collected in field #LinesWithReadErrors.
 *
\~Comment ####################################################################################### \~
 * # Reference Documentation #
 * @throws aworx::lib::config::Exceptions
 *   - \alib{config::Exceptions::ErrorOpeningFile}
 *   - \alib{config::Exceptions::ErrorWritingFile}
 **************************************************************************************************/
 class IniFile : public InMemoryPlugin
{

    // #############################################################################################
    // Public fields
    // #############################################################################################
    public:
        /** ****************************************************************************************
         * A configuration section entry (a variable)
         ******************************************************************************************/
        class Entry : public InMemoryPlugin::Entry
        {
            public:
                /** The raw string as read from the INI file. Ready to be written back when
                 * variable is untouched. */
                AString             RawValue;

                /**
                 * Constructs an Entry
                 * @param name    The name of the section.
                 */
                Entry( const String& name ) : InMemoryPlugin::Entry( name ) {}

                /// Destructor
                virtual ~Entry()    override
                {}

                /**
                 * Overrides default method. If we have not parsed the INI file text value, yet,
                 * we do this now.
                 *
                 * @param parent    The plug-in we belong to.
                 * @param variable  The variable to fill with our values.
                 */
                ALIB_API
                virtual void ToVariable( const InMemoryPlugin& parent, Variable& variable ) override;

                /**
                 * Overrides default method. Clears the raw value, and calls base method.
                 *
                 * @param parent    The plug-in we belong to.
                 * @param variable  The variable to fill with our values.
                 */
                ALIB_API
                virtual void FromVariable( const InMemoryPlugin& parent, Variable& variable ) override;
        };

        /** ****************************************************************************************
         * A configuration section entry (a variable)
         ******************************************************************************************/
        class Section: public InMemoryPlugin::Section
        {
            /**
             * Constructs a Section
             * @param name    The name of the section.
             */
            public :
            Section( const String& name ) : InMemoryPlugin::Section( name ) {}
            /**
             * Overrides base classes method to create an entry of our type.
             * @param name    The name of the entry.
             * @return An object of type \ref Entry "IniFile::Entry".
             */
            protected:
            inline
            virtual Entry*  createEntry(const String& name )
            {
                return new IniFile::Entry( name );
            }

        };


        /** If this is set to \c true, any variable change will lead to writing the file immediately
            by invoking #WriteFile. Defaults to false */
        bool                             AutoSave                                           = false;

        /// The standard file extension used for \alib configuration files. Defaults to ".ini"
        static String                    DefaultFileExtension;

        /** The file name. This might include a path or not. Should be set properly before
            the file is read. */
        AString                          FileName;

        /// The file header which will be written out as a comment lines with "# " prefixes
        AString                          FileComments;

        /** Is cleared and filled with faulty line numbers when reading the file. (E.g. when a
            line is no section and no comment but still has no equal sign ('=').  */
        std::vector<int>                 LinesWithReadErrors;

        /**
         * The prefix that is used for comment lines of sections or variables that have been
         * added 'in code' (variables that have not been read from the file).
         * Comments that were read from the file preserve their prefix.
         * If comments including one of the valid prefixes are added to a variable or section
         * 'in code', such prefix is preserved. */
        String                           DefaultCommentPrefix                           =ASTR("# ");

        /// Denotes if a space should be written before a delimiter.
        bool                             FormatSpaceBeforeDelim                             = false;

        /** Denotes if a space should be written after a delimiter. (Applies only to single
            line mode of writing attributes.)  */
        bool                             FormatSpaceAfterDelim                              =  true;

        /** Denotes whether the spaces that are inserted when aligning attributes are
         *  located before or behind the delimiter. */
        bool                             FormatIncludeDelimInAttrAlignment                  = false;

    // #############################################################################################
    // Constructor/destructor
    // #############################################################################################
    public:
        /** ****************************************************************************************
         * Constructs an instance of this class and reads the file.
         * If no file name is given, the file name is set to the process name with extension
         * found in public static field #DefaultFileExtension.
         *
         * If the given file name equals <c>'*'</c>, no file is read and field #AutoSave is set
         * to \c false.
         *
         * If the given name does not start with a path separation character,
         * \alib{system,Directory::SpecialFolder::HomeConfig} is prepended.
         *
         * @param filePathAndName  The name (and path) of the file to read and write.
         *                         Provide "*" to suppress reading a file.
         *                         Defaults to nullptr.
         ******************************************************************************************/
        ALIB_API               IniFile( const String& filePathAndName= nullptr );

        /** ****************************************************************************************
         * Virtual Destructor.
         ******************************************************************************************/
        virtual               ~IniFile()   {}

    // #############################################################################################
    // Interface
    // #############################################################################################
        /** ****************************************************************************************
         * Clears all configuration data.
         ******************************************************************************************/
        ALIB_API
        virtual void           Reset();

        /** ****************************************************************************************
         * Clears all configuration data and reads the file. It might happen that lines are
         * ignored or otherwise marked as faulty. All numbers of such lines get collected in
         * field LinesWithReadErrors.
         * @throws Exception( \alib{config,Exceptions,config::Exceptions::ErrorOpeningFile} ).
         ******************************************************************************************/
        ALIB_API
        void     ReadFile();

        /** ****************************************************************************************
         * Write all configuration data into the file.
         * @throws Exception( \alib{config,Exceptions,config::Exceptions::ErrorOpeningFile} ).
         ******************************************************************************************/
        ALIB_API
        void     WriteFile();

    // #############################################################################################
    // ConfigurationPlugin interface implementation
    // #############################################################################################
    public:
         /** ****************************************************************************************
          * Return the plug-in name, in this case, the file name.
          * @return The name of the plug-in.
          ******************************************************************************************/
         virtual String  Name()   const
         {
            return FileName;
         }

        using InMemoryPlugin::Load;
        using InMemoryPlugin::Store;

        /** ****************************************************************************************
         * Creates or replaces existing variable in our storage. If #AutoSave is set, the file
         * is written
         *
         * @param variable  The variable to retrieve.
         * @return \c true if the variable was written, \c false if not. The latter might only
         *         happen if the variable given was illegal, e.g. empty name.
         ******************************************************************************************/
        inline
        virtual bool  Store( Variable& variable )
        {
            InMemoryPlugin::Store( variable );
            if ( AutoSave )
                WriteFile();
            return true;
        }

    // #############################################################################################
    // Protected methods
    // #############################################################################################
    protected:

        /** ****************************************************************************************
         * Overrides base classes method to create a section of our type.
         *
         * Checks for resource string <c>"INI_CMT_sectionName"</c> in config library singleton
         * \ref aworx::lib::CONFIG. If found, the comment is added. Therefore, to add a default
         * comment, in the bootstrap section of the software, such resources may be added to the
         * resources of library <b>aworx::lib::CONFIG</b>.<br>
         * The comment string read from the resources is formatted using
         * \alib{strings::format,SimpleTextBase::AddMarked,SimpleText::AddMarked}.
         *
         *
         * @param sectionName    The name of the section.
         * @return An object of type \ref Section "IniFile::Section".
         ******************************************************************************************/
        ALIB_API
        virtual Section*    createSection( const String& sectionName );

        /** ****************************************************************************************
         * Writes a list of comments to the file. Comment lines are started with '#'.
         * @param os       The encapsulated output stream to write to.
         * @param comments The comment lines for the section.
         ******************************************************************************************/
        ALIB_API
        void                writeComments( strings::util::StringWriter&   os,
                                           const AString&                       comments );

};


}} // namespace lib::config

/// Type alias in namespace #aworx.
using     IniFile=       aworx::lib::config::IniFile;

}  // namespace aworx

#endif // HPP_ALIB_CONFIG_INI_FILE
