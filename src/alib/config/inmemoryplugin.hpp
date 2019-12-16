/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_config of the \aliblong.
 *
 * \emoji :copyright: 2013-2019 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_CONFIG_INMEMORY_PLUGIN
#define HPP_ALIB_CONFIG_INMEMORY_PLUGIN 1

#if !defined (HPP_ALIB_CONFIG_PLUGINS)
#   include "alib/config/plugins.hpp"
#endif
#if !defined(HPP_ALIB_MONOMEM_HASHMAP)
#   include "alib/monomem/hashmap.hpp"
#endif
#if !defined(HPP_ALIB_MONOMEM_LIST)
#   include "alib/monomem/list.hpp"
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
 * \ref aworx::lib::config::IniFile "IniFile" and may be used likewise as a base of custom plug-ins.
 *
 * To serve the \b IniFile requirements, this plug-ing divides the set of variables into sections
 * (according to the category of a variable), allows comment strings for variables and sections,
 * and virtualizes some key methods to allow descendants to take specific actions.
 *
 * ## Monotonic Allocation ##
 * This type uses monotonic allocation for all internal data, except long name and comment strings.
 * Nevertheless, due to the recycling facilities of the container types of module
 * \alib_monomem, it is 100% safe against memory exhaustion, independent from the number
 * of repeated insert and delete operations.
 **************************************************************************************************/
class InMemoryPlugin : public ConfigurationPlugin
{
    #if !defined(ALIB_DOX)
        friend class InMemoryPluginIteratorImpl;
        friend class Configuration;
    #endif

    // #############################################################################################
    // Types
    // #############################################################################################
    public:
        class Section;
        class Entry;

    protected:
        /** The list type used by struct \b Section to store entries.    */
        using TEntryList    = List<Entry   , Recycling::Shared>;

        /** The list type used by struct \b Entry to store additional values of multi-value
         *  entries.                                                                         */
        using TValueList    = List<String32, Recycling::Shared>;

    public:
        /** ****************************************************************************************
         * A section entry. Corresponds to a configuration \alib{config,Variable}.
         ******************************************************************************************/
        class Entry
        {
            #if !defined(ALIB_DOX)
                friend class InMemoryPlugin;
            #endif

            protected:
                /** The entry's name  */
                String32        name;

            public:
                /** The comments of the section. */
                AString         Comments;

                /** The delimiter (for splitting output into to multiple lines). */
                character       Delim                                                        = '\0';

                /** The format hints of the variable */
                FormatHints     FmtHints                                        = FormatHints::None;

                /** Used to vertically align attributes in multi-line attributes.
                 * Use cases are "=", ":" or "->".<br>
                 * Used by \alib{config,IniFile} and potentially by custom derived plug-in types. */
                String8         FormatAttrAlignment;

                /** The first values of the entry. */
                String32        Value;

                /** The rest of the values. */
                TValueList      AdditionalValues;

                /** The value as received from an external source. Derived types may use this
                 *  "original" value in the case that it was not changed by software.
                 *  For example in case of INI-files, this approach preserves a user's manual
                 *  format, when writing back the file. */
                String32        RawValue;

            /**
             * Constructs an Entry
             * @param pName        The name of the entry.
             * @param pAllocator   The allocator used for the list of additional values.
             * @param recycler     The recycler for value entries.
             */
            Entry( const String&                pName,
                   MonoAllocator*               pAllocator,
                   TValueList::TSharedRecycler& recycler     )
            : AdditionalValues( pAllocator, recycler )
            {
                name.DbgDisableBufferReplacementWarning();
                Value              .DbgDisableBufferReplacementWarning();
                FormatAttrAlignment.DbgDisableBufferReplacementWarning();
                RawValue           .DbgDisableBufferReplacementWarning();

                name.Reset(pName);
            }

            /**
             * Returns the (unique) name of this entry.
             * @return The name of this entry
             */
            const String&   Name()                                                             const
            {
                return name;
            }


            /**
             * Increases or shrinks the number of elements in #AdditionalValues to match
             * the given \p{requestedSize} <b>- 1</b>.
             * @param requestedSize The requested number of values of this entry.
             * @return An iterator pointing to the first additional value, hence the second
             *         value of this entry.
             */
            TValueList::Iterator SetValueCount( integer requestedSize )
            {
                integer valSize= static_cast<integer>( AdditionalValues.Size() + 1  );
                while (valSize < requestedSize )
                {
                    AdditionalValues.EmplaceBack();
                    AdditionalValues.Back().DbgDisableBufferReplacementWarning();
                    ++valSize;
                }
                while (valSize > requestedSize )
                {
                    AdditionalValues.PopBack();
                    --valSize;
                }

                return AdditionalValues.begin();
            }
        }; // class Entry



        /** ****************************************************************************************
         * A configuration section. Corresponds to a configuration category.
         ******************************************************************************************/
        class Section
        {
            #if !defined(ALIB_DOX)
                friend class InMemoryPlugin;
                friend class InMemoryPluginIteratorImpl;
                friend class Configuration;
            #endif

            protected:
                String32        name;         ///< The name of the section.
                TEntryList      entries;      ///< The list of variables of the section.

            public:
                mutable AString Comments;     ///< The comments of the section. This object is
                                              ///< mutable with constant instances.

            /**
             * Constructs a section.
             * @param sectionName  The name of the section.
             * @param pAllocator   The allocator used for the list of entries.
             * @param recycler     The recycler for the list of entries.
             */
            Section(const String&                sectionName,
                    MonoAllocator*               pAllocator,
                    TEntryList::TSharedRecycler& recycler        )
            : entries( pAllocator, recycler )
            {
                name.DbgDisableBufferReplacementWarning();
                name.Reset(sectionName);
            }

            /**
             * Returns the (unique) name of this section.
             * @return The name of this entry.
             */
            const String&       Name()                                                         const
            {
                return name;
            }

            /**
             * Returns the (unique) name of this section.
             * @return The name of this entry.
             */
            const TEntryList&   Entries()                                                      const
            {
                return entries;
            }

        };

    // #############################################################################################
    // Hashtable for entries
    // #############################################################################################
    protected:
        /** Hash functor for nodes hashed in field #entryTable. */
        struct EntryKey
        {
            const String&     SectionName;  ///< The name of the section.
            const String&     EntryName;    ///< The name of the entry.

            /** Constructor.
             *  @param sectionName The section's name of an entry.
             *  @param entryName   The name of an entry.
             */
            EntryKey( const String& sectionName, const String& entryName )
            : SectionName( sectionName)
            , EntryName  ( entryName   )
            {}

            /** Hash functor for nodes hashed in field #entryTable. */
            struct Hash
            {
                /**
                 * Calculates a hash code for \b NodeKey objects.
                 * @param key The key to hash.
                 * @return The hash code.
                 */
                std::size_t operator()(const EntryKey& key)                                    const
                {
                    return    key.SectionName.HashcodeIgnoreCase()
                            + key.EntryName  .HashcodeIgnoreCase();
                }
            };

            /** Equality functor for nodes in field #entryTable. */
            struct EqualTo
            {
                /**
                 * Invokes \alib{strings,TString::Equals,String::Equals} on \p{lhs}, passing \p{rhs}
                 * and returns the result.
                 * @param lhs The first string object.
                 * @param rhs The second string object.
                 * @return The result of the string comparison.
                 */
                bool operator()(const EntryKey& lhs,  const EntryKey& rhs )                    const
                {
                    return     (     (lhs.SectionName.IsEmpty()          && rhs.SectionName.IsEmpty() )
                                  ||  lhs.SectionName.Equals<Case::Ignore>( rhs.SectionName ) )
                            && (     (lhs.EntryName  .IsEmpty()          && rhs.EntryName  .IsEmpty() )
                                  ||  lhs.EntryName  .Equals<Case::Ignore>( rhs.EntryName   ) );
                }
            };

        };

    // #############################################################################################
    // protected fields
    // #############################################################################################
    protected:
        /** A monotonic allocator used for allocating sections and entries.   */
        MonoAllocator*                      allocator;

        /** The name of the plug-in. Provided in constructor, returned with #Name. */
        String                              plugInName;

        /** The shared recycler for \b Entry list nodes.
         *  This object is passed to the entry list of each section.      */
        TEntryList::TSharedRecycler         entryRecycler;

        /** The shared recycler for value list nodes of struct \b Entry.
         *  This object is passed to the value list of each entry.        */
        TValueList::TSharedRecycler         valueRecycler;

        /**
         * The entry hash set.
         * This is used to find entries by section name and entry name.
         * The set contains all entries of all sections.
         */
        monomem::HashMap<EntryKey,  std::pair<Section*, TEntryList::Iterator>,
                        EntryKey::Hash,
                        EntryKey::EqualTo>  entryTable;


        /** The list of sections. */
        List<Section>                       sections;

        /**
         * If a monotonic allocator instance was provided from outside, this flag is \c false.
         * otherwise \c true.
         */
        bool                                isMAOwner;

    // #############################################################################################
    // Constructor/destructor
    // #############################################################################################
    public:
        /** ****************************************************************************************
         * Constructor.
         * @param pName      The name of the plug-in as returned with #Name.
         * @param externalMA External monotonic allocator. If not given, an internal one
         *                   is created.
         *                   Optional and defaults to \c nullptr.
         ******************************************************************************************/
        ALIB_API
        InMemoryPlugin( String pName, MonoAllocator* externalMA= nullptr );

        /** ****************************************************************************************
         * Virtual Destructor.
         ******************************************************************************************/
        ALIB_API
        virtual
       ~InMemoryPlugin()                                                                   override;

    // #############################################################################################
    // Conversion of variables to entries and vice versa.
    // #############################################################################################
        /**
         * Virtual method that copies the values of an entry to the given \p{variable}.
         *
         * @param entry     The entry to convert.
         * @param variable  The variable to fill with our values.
         */
        ALIB_API
        virtual void        ToVariable(   Entry& entry, Variable& variable )                  const;

        /**
         * Virtual method that copies the values of the given \p{variable} to an entry.
         *
         * @param entry     The entry to convert.
         * @param variable  The variable to fill with our values.
         */
        ALIB_API
        virtual void        FromVariable( Entry& entry, Variable& variable )                  const;

    // #############################################################################################
    // Interface
    // #############################################################################################
        using ConfigurationPlugin::Load;
        using ConfigurationPlugin::Store;

        /** ****************************************************************************************
         * Clears all configuration data.
         ******************************************************************************************/
        ALIB_API
        virtual void        Clear();

        /** ****************************************************************************************
         * Searches the \ref aworx::lib::config::InMemoryPlugin::Section "Section" with the given
         * name.
         *
         * @param sectionName      The name of the section to be retrieved.
         * @return Returns the section if it was found, nullptr otherwise.
         ******************************************************************************************/
        ALIB_API
        const Section*      SearchSection( const String& sectionName );

        /** ****************************************************************************************
         * Searches the \ref aworx::lib::config::InMemoryPlugin::Section "Section" with the given
         * name. If the section was not found, it is created.
         *
         * @param sectionName The name of the section to be retrieved.
         * @return Returns a <c>std::pair</c> with the first element being a pointer to the section
         *         and the second a boolean that is \c true if the section was created and \c false
         *         otherwise.
         ******************************************************************************************/
        ALIB_API
        std::pair<Section*, bool>
                            SearchOrCreateSection( const String& sectionName );

        /** ****************************************************************************************
         * Searches an entry with the given name.
         * @param  section  The name of the section.
         * @param  name     The name of the entry to be searched.
         * @return The entry if found, else \c nullptr.
         ******************************************************************************************/
        Entry*              SearchEntry ( const String& section, const String& name )
        {
            return &*searchEntry( section, name );
        }

        /** ****************************************************************************************
         * Searches an entry with the given name.
         * @param  section  The section.
         * @param  name     The name of the entry to be searched.
         * @return The entry if found, else \c nullptr.
         ******************************************************************************************/
        Entry*              SearchEntry ( Section* section, const String& name )
        {
            return &*searchEntry( section->name, name );
        }

        /** ****************************************************************************************
         * Returns a constant  reference to the sections of this plug-in.
         * @return The list of sections.
         ******************************************************************************************/
        const List<Section>& Sections()                                                        const
        {
            return sections;
        }

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
        virtual String      Name()                                                    const override
        {
            return plugInName;
        }

        /** ****************************************************************************************
         * Searches the variable in our storage.
         *
         * @param variable      The variable to retrieve.
         * @param searchOnly    If \c true, the variable is not read. Defaults to \c false.
         * @return \c true if variable was found, \c false if not.
         ******************************************************************************************/
        ALIB_API
        virtual bool        Load( Variable& variable, bool searchOnly= false )             override;

        /** ****************************************************************************************
         * Creates or replaces existing variable in our storage.
         * If the variable has no value, an existing entry is deleted.
         *
         * @param variable  The variable to retrieve.
         * @return \c true if the variable was written, \c false if not. The latter might only
         *         happen if the variable given was illegal, e.g. empty name.
         ******************************************************************************************/
        ALIB_API
        virtual bool        Store( Variable& variable )                                    override;

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
        virtual Iterator*   GetIterator( const String& sectionName )                       override;

    // #############################################################################################
    // Internals
    // #############################################################################################
    protected:
        /** ****************************************************************************************
         * Appends a new section to the end of the list.
         * Must be invoked only if the section does not exist, yet.
         * @param  sectionName The name of the section.
         * @return The entry if found, else \c nullptr.
         ******************************************************************************************/
        virtual Section*        createSection( const String& sectionName )
        {
            return &sections.EmplaceBack( sectionName, allocator, entryRecycler );
        }

        /** ****************************************************************************************
         * Searches an entry with the given name. The search is performed case insensitive.
         * @param  section  The name of the section
         * @param  name     The name of the entry to be searched.
         * @return The entry if found, else \c nullptr.
         ******************************************************************************************/
        Entry*                  searchEntry (const String& section, const String& name )
        {
            auto it=  entryTable.Find( EntryKey(section, name ) );
            if( it != entryTable.end() )
                return &*it.Mapped().second;
            return nullptr;
        }

        /** ****************************************************************************************
         * Creates a new entry.
         * Must be invoked only if the entry does not exist, yet.
         * @param  section  The section.
         * @param  name     The name of the entry to be created.
         * @return The entry found or created.
         ******************************************************************************************/
        ALIB_API
        Entry*                  createEntry ( Section* section, const String& name )
        {
            auto entryIt= section->entries.Emplace      ( section->entries.end(),
                                                          name, allocator, valueRecycler );
            entryTable                    .EmplaceUnique( EntryKey(section->Name(), entryIt->Name()),
                                                          std::make_pair(section, entryIt) );
            return &*entryIt;
        }

}; // InMemoryPlugin


}} // namespace lib::config

/// Type alias in namespace #aworx.
using     InMemoryPlugin=       lib::config::InMemoryPlugin;

}  // namespace [aworx]

#endif // HPP_ALIB_CONFIG_INMEMORY_PLUGIN
