//==================================================================================================
/// \file
/// This header-file is part of module \alib_variables of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib {  namespace variables {
//==================================================================================================
/// Adapter type between class \alib{variables;IniFile} and class \alib{variables;Configuration}.
/// INI-Files contents usually are fed in with bootstrapping an application and optionally be
/// written out when an application ends (to populate empty INI-files or add new variables to
/// existing ones). For this, an instance of this type is to be locally created each time. In other
/// words, no instance of this type needs to be kept in memory during the life-span of an
/// application.
/// @see Chapter \ref alib_variables_external_ini of the Programmer's Manual of camp \alib_variables.
/// @throws alib::variables::Exceptions::ErrorWritingFile \I{CLANGDUMMY}
//==================================================================================================
class IniFileFeeder
{
  //=========================================== Internals ==========================================
  protected:
    /// The configuration to work with. Set with construction.
    Configuration&          configuration;

    /// The priority to use to define variables. Set with construction.
    Priority                priority;

    /// The INI-file. Created with methods #ImportStart and #ExportStart.
    IniFile*                iniFile                                                       = nullptr;

    /// Implements #ImportSection.
    ///
    /// @param section The section to import
    /// @return The number of variables imported.
    ALIB_DLL
    int         importSection( IniFile::Section& section );


  //========================================= Public Fields ========================================
  public:
    /// The desired maximum width of the INI-file. Defaults to <c>100</c>.
    /// This value is used with utility method #AddResourcedSectionComments.
    int                     LineWidth                                                         = 100;

    /// The prefix that is used for comment lines of sections or variables that have been
    /// added 'in code' (variables that have not been read from the file).
    /// Comments that were read from the file preserve their prefix.
    /// If comments including one of the valid prefixes are added to a variable or section
    /// 'in code', such prefix is preserved.
    String                  DefaultCommentPrefix                                      =A_CHAR("# ");

  //==================================== Constructors/destructor ===================================
  public:
    /// Constructor.
    /// @param pConfiguration Stored in the field #configuration.
    /// @param pPriority      Stored in the field #priority.
    ///                       \alib{variables;Priority;ConfigFile}.
    IniFileFeeder( Configuration& pConfiguration, Priority pPriority= Priority::ConfigFile )
    : configuration(pConfiguration)
    , priority     (pPriority)                                                                    {}

    /// Virtual destructor. \ref alib_mod_assert "Raises an ALib warning" in debug-compilations,
    /// if the internal INI-file was not disposed before deletion. Disposal is made with methods
    /// #ImportEnd and ExportEnd.
    virtual ~IniFileFeeder() {
        #if ALIB_DEBUG && !ALIB_DEBUG_ASSERTION_PRINTABLES
            ALIB_ASSERT_WARNING(iniFile == nullptr, "VARIABLES",
              "Export or import sequence not completed before deleting the INI-file.\n"
              "Filename of previous sequence: {}", iniFile->FileName )
            delete iniFile;
        #endif

    }

  //=========================================== Interface ==========================================
    /// Imports all entries in the section named \p{sectionName} in the underlying INI-file into
    /// the configuration.
    /// @param sectionName The name of the section to import.
    /// @return The number of variables imported.
    ALIB_DLL
    int         ImportSection( const String& sectionName );

    /// Imports all entries in the section named \p{sectionName} in the underlying INI-file into
    /// the configuration. This special version of the method accepts parameter \p{typeName}
    /// and hence declares the variables in case they have not been declared, yet.
    ///
    /// \note
    ///   This method is useful to load a section of variables of the same type with a) avoiding to
    ///   set preset string and b) with the advantage that a using code may use
    ///   \alib{variables;Variable::Try} to find the variable already.<br>
    ///   This method may be used, for example, with camp \alib_expressions to declare nested
    ///   expressions to an \alib{expressions;StandardRepository}.
    /// @param sectionName The name of the section to import.
    /// @param typeName    The type of the variable to declare.
    /// @return The number of variables imported.
    ALIB_DLL
    int         ImportSection( const String& sectionName, const String& typeName );

    /// Imports all entries found INI-file into the configuration. Variables which are already
    /// declared and have a lower priority definition, will be set with the value found.
    /// Otherwise, a \alib{variables;Configuration::PresetImportString;preset string} is set for the
    /// variable.
    /// @return The number of variables imported.
    ALIB_DLL
    int         ImportAll();

    /// Creates the internal \p{iniFile} instance.
    /// @param path The filepath to the INI-file.
    /// @return A pointer to the (otherwise internal) \alib{variables;IniFile} object that was opened.
    ///         Use with caution. Could be used, for example, to detect the fact that the INI-file
    ///         was empty or not found.
    IniFile* ImportStart( const system::Path&  path ) {
        if( iniFile != nullptr ) {
            ALIB_ASSERT_WARNING(iniFile == nullptr, "VARIABLES",
              "Export or import sequence not completed before starting a new one.\n"
              "Previous filename: ", iniFile->FileName )
            delete iniFile;
        }
        iniFile= new IniFile(path);
        return iniFile;
    }

    /// Creates the internal \p{iniFile} instance.
    /// @param path The filepath to the INI-file.
    /// @return A pointer to the (otherwise internal) \alib{variables;IniFile} object that was
    ///         opened. Use with caution. Could be used, for example, to detect the fact that
    ///         the INI-file was empty or not found.
    IniFile* ExportStart( const system::Path& path )                   { return ImportStart(path); }

    /// Closes and deletes the internal \p{iniFile} instance.
    void ImportEnd() {
        if(iniFile == nullptr) {
            ALIB_ERROR( "VARIABLES", "No INI-file loaded when trying to end import/export." )
            return;
        }
        delete iniFile;
        iniFile= nullptr;
    }

    /// Closes and deletes the internal \p{iniFile} instance without writing an opened INI-file.
    void ExportEnd()                                                                { ImportEnd(); }

    /// Writes the contents of the internal \p{iniFile} instance created with #ExportStart into
    /// an (optionally) different file and then deletes the instance.
    /// @param path The file path for writing.
    void ExportEnd(const system::Path&  path ) {
        if(iniFile == nullptr) {
            ALIB_ERROR( "VARIABLES", "No INI-file loaded when trying to import data." )
            return;
        }
        iniFile->Write(path);
        delete iniFile;
        iniFile= nullptr;
    }


    /// Writes the given variable to the internal INI-file.
    /// If an INI-file was previously loaded, only those variables which are not existing in the
    /// INI-file are written. This method can thus be used to "populate" an empty INI-file with
    /// variables and their resourced or hard-coded default values and this way present all
    /// configurable options to an end-user.
    ///
    /// Furthermore, a user can specify the word "writeback" (respectively what is resourced
    /// with to token <c>"CFGIniWB"</c>) to force overwriting the INI-file contents with the current
    /// value of the variable.
    ///
    /// @param var The variable to export.
    /// @return \c true if the variable either did not exist yet in the INI-file or if it existed
    ///            and the write-back mechanism was in place. \c false otherwise.
    ALIB_DLL
    bool        Export( const Variable& var );

    /// Creates a variable cursor with the given \p{cfg} and \p{path} and passes it to overloaded
    /// method #Export(const Variable&).
    /// @param path  The path of the variable to export.
    /// @return \c   true if the variable either did not exist yet in the INI-file or if it existed
    ///              and the write-back mechanism was in place. \c false otherwise.
    bool         Export(const String& path) {
        auto cursor= configuration.Root();
        auto remainder= cursor.GoTo(path);
        if( remainder.IsNotEmpty() ) {
            ALIB_WARNING("VARIABLES", "Configuration variable path \"{}\" to export not found.",
                                      path)
            return false;
        }
        return Export( Variable(cursor) );
    }


    /// Writes all variables below the given \alib{containers;StringTree::Cursor}
    /// of the \alib{variables;Configuration} into the #iniFile, previously created with #ExportStart.
    /// If an INI-file was previously loaded, only those variables which are not existing in the
    /// INI-file are written.
    /// This method can thus be used to "populate" an empty INI-file with default values and
    /// this way present all configurable options to an end-user.
    ///
    /// Furthermore, a user can specify the word "writeback" (respectively what is resourced
    /// with to token <c>"CFGIniWB"</c>) to force overwriting the INI-file contents with the current
    /// value of the variable.
    ///
    /// @param cursor              A cursor into the configuration's \alib{containers;StringTree}.
    /// @param directChildrenOnly  If \c true, only the direct children of this cursor are written
    ///                            as variables. Defaults to \c false.
    /// @return The number of variables exported. A negative value on error.
    ALIB_DLL
    int        ExportSubTree( Configuration::Cursor cursor, bool directChildrenOnly= false );

    /// Invokes #ExportSubTree(Configuration::Cursor, bool) by creating a cursor from the given
    /// \p{path}
    /// @param path                The path to the subtree (or single variable) to use.
    /// @param directChildrenOnly  If \c true, only the direct children of this \p{path} are written
    ///                            as variables. Defaults to \c false.
    /// @return The number of variables exported. A negative value on error.
    int        ExportSubTree( const String& path, bool directChildrenOnly= false ) {
        auto cursor= configuration.Root();
        auto remainder= cursor.GoTo(path);
        if( remainder.IsNotEmpty() ) {
            ALIB_WARNING("VARIABLES", "Configuration variable path \"{}\" to export not found.")
            return -1;
        }

        return ExportSubTree( cursor, directChildrenOnly );
    }

    /// Searches the entry of the internal INI-file that represents the given \p{variable}
    /// @param variable   The variable to search in the INI-file.
    /// @return The section and entry if found. If not found, the second member of the pair is
    ///         emtpy.
    ALIB_DLL
    std::pair<IniFile::Section*, IniFile::Entry*>   SearchEntry ( const Variable& variable );

    /// Searches the entry of the internal INI-file that represents the given variable determined
    /// by parameter \p{path}
    /// @param path   The path of the variable to search in the INI-file.
    /// @return The section and entry if found. If not found, the second member of the pair is
    ///         emtpy.
    ALIB_DLL
    std::pair<IniFile::Section*, IniFile::Entry*>   SearchEntry ( const String& path );

    /// This method may be used to set the \ref config_IniFile_writebackflag "writeback flag"
    /// of an entry in the associated INI-file. The flag is only set if the entry was
    /// programmatically created (usually with one of the <b>Export</b>-methods of this class) and
    /// if it was not present in the INI-file before.
    /// @see Chapter \ref alib_variables_external_session of the Programmer's Manual of module
    /// \alib_variables_nl.
    /// @param path  The path of the variable to export.
    /// @return \c true if the variable did exist and was newly added to the INI-file, \c false
    ///            otherwise.
    ALIB_DLL
    bool         SetWriteBackFlag( const String& path );

    /// Invokes overloaded method #SetWriteBackFlag(const String& path).
    /// @param var The variable to modifiy.
    /// @return \c true if the variable did exist and was newly added to the INI-file, \c false
    ///            otherwise.
    ALIB_DLL
    bool        SetWriteBackFlag( const Variable& var );

    #if ALIB_RESOURCES
    /// This is a utility function that reads section comments from
    /// \ref alib_mod_resources "externalized string resources".
    ///
    /// All sections of the underlying INI-file are processed, but resourced comments
    /// are only added in the case that a section's comment string is \c nulled. This is not the
    /// case if a section was read from an INI-file, as even if no comments are given,
    /// the field is empty, but not \e nulled. In other words, only sections that have been
    /// programmatically added during the run of software are changed<br>
    /// This approach allows a user to remove the comments, without the software restoring them.
    ///
    /// The resource names are assembled from given \p{resourceNamePrefix} and the section name
    /// The resource strings found are processed using method
    /// \alib{format;Paragraphs.AddMarked}. This allows using text macros like <b>'\@HL'</b>
    /// to format the text.
    ///
    /// \see
    ///    Field #LineWidth, which is respected when formatting comment lines.
    ///
    /// @param resourcePool          The resource pool to use.
    /// @param resourceCategory      The category of the resourced comments.
    /// @param resourceNamePrefix    A prefix of the resource name.
    /// @return The number of section comments written. This is useful to detect if the INI-file
    ///         needs to be written.
    ALIB_DLL  int       AddResourcedSectionComments( ResourcePool&    resourcePool,
                                                     const NString&   resourceCategory,
                                                     const NString&   resourceNamePrefix  );
    #endif

    /// Allows access to the internal INI-file object created with either method #ImportStart or
    /// #ExportStart. In debug-compilations, this method asserts that an INI-file was opened.
    /// In release-compilations, a non-existing INI-file leads to undefined behavior.
    /// @return The currently opened INI-File.
    IniFile& GetIniFile() {
        ALIB_ASSERT_ERROR( iniFile != nullptr, "VARIABLES",
            "No INI-file loaded when trying to access it." )
        return *iniFile;
    }

}; // class IniFileFeeder


} // namespace alib[::config]

/// Type alias in namespace \b alib.
using     IniFileFeeder=       variables::IniFileFeeder;

} // namespace [alib]
