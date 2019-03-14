// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#ifndef HPP_ALIB_CONFIG_INMEMORY_PLUGIN
#define HPP_ALIB_CONFIG_INMEMORY_PLUGIN 1

#if !defined (HPP_ALIB_CONFIG_PLUGINS)
#   include "alib/config/plugins.hpp"
#endif

namespace aworx { namespace lib { namespace config {

/** ************************************************************************************************
 * Specialization of abstract interface class #ConfigurationPlugin, which holds
 * configuration variables in memory. Instances of this class are used to create two plug-ins
 * within standard configurations:
 * - One with priority
 *   \alib{config,Priorities,Priorities::DefaultValues} (low priority)
 *   to store default values not found in other configuration plug-ins and
 * - a second with priority
 *   \alib{config,Priorities,Priorities::ProtectedValues} (maximum priority),
 *   which can be used protect configuration values from external changes.
 *
 * This class in addition acts as the parent of class
 * \ref aworx::lib::config::IniFile "IniFile" and potentially other (custom) classes.
 * For this reason, it divides the set of variables into sections (according to the category),
 * allows comment strings for variables and sections, and virtualizes some key methods to
 * allow descendants to take specific actions.
 *
 * This class offers important internal fields and types for public access (Bauhaus code style).
 * However, in standard cases, only the interface methods of this class should be used.
 **************************************************************************************************/
class InMemoryPlugin : public ConfigurationPlugin
{

    private:
    /** Internal class implementing per section iteration feature. */
    class InMemoryPluginIteratorImpl;

    // #############################################################################################
    // protected fields
    // #############################################################################################
    protected:
        /** The name of the plug-in. Provided in constructor, returned with #Name. */
        String                         name;

    // #############################################################################################
    // inner classes
    // #############################################################################################
    public:
        /** ****************************************************************************************
         * A configuration section's entry
         ******************************************************************************************/
        class Entry
        {

            public:
                String32               Name;                        ///< The name of the section.
                AString                Comments;                    ///< The comments of the section.
                character              Delim = '\0';                ///< The delimiter (for splitting output into to multiple lines).
                FormatHints            FmtHints= FormatHints::None; ///< The format hints of the variable
                String16               FormatAttrAlignment;         ///< The format attribute alignment hint of the variable
                std::vector<AString>   Values;                      ///< The list of values of the entry of the section.

                /**
                 * Constructs an Entry
                 * @param varName    The name of the section.
                 */
                Entry( const String& varName )
                {
                    Name.DbgDisableBufferReplacementWarning();
                    Name._( varName );
                }
                /** Destructor. */
                virtual ~Entry()   {}

                /**
                 * Virtual method that copies the values of an entry to the given \p{variable}.
                 *
                 * @param parent    The plug-in we belong to.
                 * @param variable  The variable to fill with our values.
                 */
                ALIB_API
                virtual void ToVariable( const InMemoryPlugin& parent, Variable& variable );

                /**
                 * Virtual method that copies the values of the given \p{variable} to us.
                 *
                 * @param parent    The plug-in we belong to.
                 * @param variable  The variable to fill with our values.
                 */
                ALIB_API
                virtual void FromVariable( const InMemoryPlugin& parent, Variable& variable );
        };

        /** ****************************************************************************************
         * A configuration section
         ******************************************************************************************/
        class Section
        {
            public:
                String32             Name;         ///< The name of the section.
                AString              Comments;     ///< The comments of the section.

                std::vector<Entry*>  Entries;      ///< The list of variables of the section.

            /**
             * Constructs a Section
             * @param sectionName    The name of the section.
             */
            Section( const String& sectionName )
            : Name( sectionName )
            {
                Name.DbgDisableBufferReplacementWarning();
            }

            /** Destructor. */
            virtual ~Section()
            {
                for ( Entry* entry : Entries )
                    delete entry;
            }

            /**
             * Searches an entry with the given name. The search is performed case insensitive
             * @param entryName      The name of the variable to be searched.
             * @param create    Denotes if an entry should be created if not found.
             * @return The variable if found, else \c nullptr.
             */
            ALIB_API
            Entry*     GetEntry ( const String& entryName, bool create );

            /**
             * Deletes an entry (if exists)
             * @param entryName      The name of the variable to be searched.
             * @return \c true if the entry was found and deleted, \c false otherwise.
             */
            ALIB_API
            bool       DeleteEntry ( const String& entryName );

            protected:
            /** ************************************************************************************
             * Virtual method to create an Entry.
             * (Provided to allow descendant classes to created extended versions of an entry. See
             * also \ref aworx::lib::config::InMemoryPlugin::createSection "InMemoryPlugin::createSection".)
             *
             * @param  entryName The name of the entry.
             * @return An object of type \ref Entry "InMemoryPlugin::Entry".
             **************************************************************************************/
            inline
            virtual Entry*  createEntry( const String& entryName )
            {
                return new Entry( entryName );
            }
        };

    // #############################################################################################
    // Public fields
    // #############################################################################################
    public:
        /** The list of sections. */
        std::vector<Section*>            Sections;

    // #############################################################################################
    // Constructor/destructor
    // #############################################################################################
    public:
        /** ****************************************************************************************
         * Constructor.
         * @param pName The name of the plug-in as returned with #Name.
         ******************************************************************************************/
        inline
        InMemoryPlugin( String pName )
        : ConfigurationPlugin()
        , name( pName )
        {
            Reset();
        }

        /** ****************************************************************************************
         * Virtual Destructor.
         ******************************************************************************************/
        ALIB_API
        virtual      ~InMemoryPlugin();

    // #############################################################################################
    // Interface
    // #############################################################################################
        using ConfigurationPlugin::Load;
        using ConfigurationPlugin::Store;

        /** ****************************************************************************************
         * Clears all configuration data.
         ******************************************************************************************/
        ALIB_API
        virtual void    Reset();

        /** ****************************************************************************************
         * Searches the \ref aworx::lib::config::InMemoryPlugin::Section "Section" with the given name.
         *
         * @param sectionName      The name of the section to be retrieved.
         * @return Returns the section if it was found, nullptr otherwise.
         ******************************************************************************************/
        ALIB_API
        Section*        SearchSection( const String& sectionName)  const;

        /** ****************************************************************************************
         * Searches the \ref aworx::lib::config::InMemoryPlugin::Section "Section" with the given
         * name. If the section was not found, it is created.
         * If the section is found and has no comments, then the provided comments are appended.
         * @param sectionName The name of the section to be retrieved.
         * @param comments    The comment lines for the section, in the case the section is not
         *                    found. If this is \e nulled, no section comments are created.
         * @return Returns the section if it was found or created. nullptr otherwise.
         ******************************************************************************************/
        ALIB_API
        Section*        SearchOrCreateSection( const String& sectionName, const String& comments );

    // #############################################################################################
    // ConfigurationPlugin interface implementation
    // #############################################################################################
    public:

        /** ****************************************************************************************
         * Abstract method to return a plug-in name. The name may be used in human readable
         * output, e.g. log-files or exception messages to tell a user for example, which plug-in
         * loaded a variable containing a syntax error.
         * @return The name of the plug-in.
         ******************************************************************************************/
        virtual String  Name()  const
        {
            return name;
        }

        /** ****************************************************************************************
         * Searches the variable in our storage.
         *
         * @param variable  The variable to retrieve.
         * @param searchOnly   If \c true, the variable is not read. Defaults to \c false.
         * @return \c true if variable was found, \c false if not.
         ******************************************************************************************/
        ALIB_API
        virtual bool    Load( Variable& variable, bool searchOnly= false )  const;

        /** ****************************************************************************************
         * Creates or replaces existing variable in our storage.
         *
         * @param variable  The variable to retrieve.
         * @return \c true if the variable was written, \c false if not. The latter might only
         *         happen if the variable given was illegal, e.g. empty name.
         ******************************************************************************************/
        ALIB_API
        virtual bool    Store( Variable& variable );

        /** ****************************************************************************************
         * Creates an iterator object to return all variables within a section.
         *
         * The iterator object returned, needs to be deleted by the caller.
         *
         * \note Method \alib{config,Configuration::GetIterator} returns an iterator that allows
         *       to iterate across all variables of a section found in all plug-ins
         *
         * @param sectionName  The name of the section to iterate.
         *
         * @returns The iterator requested.
         ******************************************************************************************/
        ALIB_API
        virtual Iterator*       GetIterator( const String& sectionName );

    // #############################################################################################
    // Internals
    // #############################################################################################
    protected:
        /** ****************************************************************************************
         * Virtual method to create a section.
         * (Provided to allow descendant classes to created extended versions of an entry.)
         * @param  sectionName The name of the section
         * @return An object of type \ref Section "InMemoryPlugIn::Section".
         ******************************************************************************************/
        inline
        virtual Section*  createSection( const String& sectionName )
        {
            return new InMemoryPlugin::Section( sectionName );
        }

};


}} // namespace lib::config

/// Type alias in namespace #aworx.
using     InMemoryPlugin=       aworx::lib::config::InMemoryPlugin;

}  // namespace [aworx]

#endif // HPP_ALIB_CONFIG_INMEMORY_PLUGIN
