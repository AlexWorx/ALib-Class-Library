//==================================================================================================
/// \file
/// This header file is part of module \alib_config of the \aliblong.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_CONFIG_INIFILE
#define HPP_ALIB_CONFIG_INIFILE 1
#pragma once
#include "alib/containers/list.hpp"
#include "alib/containers/hashtable.hpp"
#include "alib/lang/system/path.hpp"
#include "alib/lang/message/report.hpp"

namespace alib::strings::compatibility::std { class StringWriter; }

namespace alib {  namespace config {
//==================================================================================================
///  This class implements a rather simple foundation for reading and writing INI-Files.
///
///  \note This class is not considered to be used directly to interface into the \alib
///        configuration system implemented with camp \alib_config, which introduces this class.
///        Instead, class \alib{config;IniFileFeeder} provides all mechanics to easily use
///        INI-files. For this reason, there is no \ref alib_nsalib_type_aliases "type alias"
///        defined for this class in namespace #alib.
///
///  The design goal was to preserve any user formatting of the INI-file as much as possible.
///  Thus, if the INI-file is written without any modification since it was read from a file,
///  the resulting file should quite exactly in respect comments, values and even whitespaces.
///  Exceptions are:
///  - Whitespaces at the end of lines, are trimmed.
///  - Sections that occur more than one time in the original file, are merged into one (the first)
///    occurrence.
///  - Some empty lines are removed.
///
///  When read, a linked list of INI-file sections is created and within each section a list of
///  variables is created. The linked lists allow to write sections and their variables in the same
///  order they have been read. In addition to the lists, a hashtable is created which allows
///  finding variables quickly. To find a section, the section list is iterated.
///
///  After a file has been read (or also on a blank instance of this class), sections and
///  variables can be manipulated (insert, delete, modify).
///  It is also possible to read one or more files in a sequence and write a merged INI-file back.
///
///  This class does not perform any interpretation of the variable values. Neither
///  escape sequences are converted, nor array values parsed or anything. Instead, the "raw"
///  value of each variable, including the equal sign <c>'='</c> after the variable name is stored.
///  It is up to other types to parse and interpret variable values and to convert a programm's
///  variable values to strings which can be stored in INI-files (ASCII or UTF8 files).
///
///  \attention
///    This class is also \b not designed to be used as a variable store with continued modifications
///    during a run of a software. The class performs \ref alib_mods_contmono "monotonic allocation"
///    but does not implement recycling of any objects (sections, variables, comments, values, etc.).
///    Instead, the allocated memory is continued to grow with any operation.<br>
///    This is a design decision and the class is meant to be used rather along the following
///    approach:
///    - Load an INI-file at the start of an application, transfer the values to a different place
///      and then delete the instance of this class.
///    - Before terminating an application, load the file again, perform appropriate changes,
///      write the file back and delete the instance of this class.
///
///  \attention
///    Such approach is implemented with type \alib{config;IniFileFeeder}.
///    Hence, the direct use of this class is not recommended, instead use the techniques described
///    in chapter \ref alib_config_external_ini  of the Programmer's Manual of this \alibcamp.
///
///  Some remarks on the functionality and supported format:
///  - Comments<br>
///    - Lines that start (apart from whitespace) with either a double
///      slash \c "//", a sharp sign \c '#' or a semicolon <c>';'</c> are comment lines.
///    - Comment lines at the beginning of the file are associated with the file and are written
///      back. Such comment block is stopped with the first single blank line.
///    - Comment lines and empty lines before sections and variables are associated with the
///      respective objects.
///    - Comments cannot reside in the same line together with section names or variables.
///
///  - Sections<br>
///    - Sections names are enclosed by brackets \c '[' and \c ']'.
///    - Section names can be repeated. In this case the corresponding section is continued.
///      As mentioned above, when the file is written, the sections are merged.
///
///  - Variables<br>
///    - Variable names and their values are separated by an equal sign \c '='.
///    - Variables definition are being continued (values are concatenated) if the line ends
///      with a backslash \c '\\'.
///    - Comment lines in-between continued lines are recognized as such. To continue a variable
///      after a 'continued' comment line, the comment line needs to end with a backslash \c '\\'.
///
///  \anchor config_IniFile_writebackflag
///  - Writeback Attribution<br>
///    This is a non-standard feature of this class. Anywhere in the file, a line with the term
///    <c>writeback</c> may appear. This flags the next section or variable to be written back
///    by a software. This way, changes can easily be taken over into the INI-file in the right
///    syntax that a software expects. In the case of using class \alib{config;Configuration}
///    and associated type \alib{config;IniFileFeeder} (the intended case!), the following use-cases
///    can be achieved:
///    - Command line argument might be used overwrite INI-file values. Using this flag, such
///      overwrites become persistant with a next invocation of the software without again
///      specifying this argument.
///    - In case of interactive software, changes may come from a user manipulating configuration
///      dialogs. If the user had specified "writeback", this configuration
///      change would automatically end up in the INI-file, even if the software does not provide own
///      mechanics for this.
///    - Session-related variables can be stored and updated in the INI-file, hence without creating
///      a distinct temporary session file. This is useful for rather volatile variables and such
///      that are implementing convenience features, rather than basic functionality.
///
///    The term <em>"writeback"</em> is a resourced using token <c>"INIWB"</c> and thus can be
///    localized (translated to a different language) as appropriate.
///
///  - Erroneous lines<br>
///    Lines with errors are ignored and not written back. Line numbers with erroneous lines
///    are collected in field #LinesWithReadErrors.
///
///  @see Chapter \ref alib_config_external_ini of the Programmer's Manual of \alibcamp \alib_config
///       for how to use INI-files with \alib.
///\I{=============================================================================================}
///  # Reference Documentation #
///  @throws alib::config::Exceptions::ErrorOpeningFile
///  @throws alib::config::Exceptions::ErrorWritingFile
//==================================================================================================
class IniFile
{
  //====================================    Public Allocator    ====================================
  public:
    /// A monotonic allocator used for allocating sections and entries.
    MonoAllocator           Allocator;

  //====================================   Entry and Section   =====================================
  public:
    /// An entry in a \alib{config::IniFile;Section}.
    struct Entry
    {
         String Name     = NULL_STRING; ///< The entry's name.
         String Comments = NULL_STRING; ///< The entry's comments.
         String RawValue = NULL_STRING; ///< The 'raw' value, which is everything after the
                                        ///< variable name, including the equal sign <c>'='</c>.
         String Value    = NULL_STRING; ///< The trimmed value. Multiline values are likewise trimmed
                                        ///< and backslashes and line feeds are removed <c>'\\</c>.
                                        ///< This value is to be used used for reading a variable's content.
         String NewValue = NULL_STRING; ///< If this value is set, #RawValue will ignored on writing.
         bool   WriteBack= false;       ///< If given, a write back indicator was found for this entry.
         void*  Custom   = nullptr;     ///< May be used by freely by customers of this class.
                                        ///< Initialized with \c nullptr, but otherwise not touched.

    };

    /// A section of the INI-file.
    struct Section
    {
      friend class containers::List<MonoAllocator, Section>;
      protected:
        /// Constructor. Protected and thus to be used only by friend class
        /// \alib{containers;List}.
        /// @param monoAllocator The allocator of the \b IniFile.
        Section( MonoAllocator& monoAllocator )
        : Entries(monoAllocator)                                                             {}

      public:
        String                                        Name    = NULL_STRING; ///< The name of the section.
        String                                        Comments= NULL_STRING; ///< The comment lines of the section.
        List<MonoAllocator, Entry, Recycling::None>   Entries;               ///< The list of variables of the section.
        bool                                          WriteBack= false;      ///< If given, a write back indicator was found for this entry.
    };

  //=====================   Subtypes for Sections, Entries and the Hashtable   =====================
  protected:
    /// Hash functor for nodes hashed in field #entryTable. Ignores letter case.
    struct EntryKey
    {
        const String&     SectionName;  ///< The name of the section.
        const String&     EntryName;    ///< The name of the entry.

        /// Constructor.
        /// @param sectionName The section's name of an entry.
        /// @param entryName   The name of an entry.
        EntryKey( const String& sectionName, const String& entryName )
        : SectionName( sectionName)
        , EntryName  ( entryName   )
        {}

        /// Hash functor for nodes hashed in field #entryTable.
        struct Hash
        {
            /// Calculates a hash code for \b NodeKey objects.
            /// @param key The key to hash.
            /// @return The hash code.
            std::size_t operator()(const EntryKey& key)                                    const
            {
                return    key.SectionName.HashcodeIgnoreCase()
                        ^ key.EntryName  .HashcodeIgnoreCase();
            }
        };

        /// Equality functor for nodes hashed in field #entryTable.
        struct EqualTo
        {
            /// Invokes \alib{strings;TString::Equals;String::Equals} on \p{lhs}, passing \p{rhs}
            /// and returns the result.
            /// @param lhs The first string object.
            /// @param rhs The second string object.
            /// @return The result of the string comparison.
            bool operator()(const EntryKey& lhs,  const EntryKey& rhs )                    const
            {
                return     (     (lhs.SectionName.IsEmpty() && rhs.SectionName.IsEmpty() )
                              ||  lhs.SectionName.Equals<NC, lang::Case::Ignore>( rhs.SectionName ) )
                        && (     (lhs.EntryName  .IsEmpty() && rhs.EntryName  .IsEmpty() )
                              ||  lhs.EntryName  .Equals<NC, lang::Case::Ignore>( rhs.EntryName   ) );
            }
        };
    };

  //=====================================   Other internals    =====================================
  protected:
    /// The entry hash set.
    /// This is used to find entries by section name and entry name.
    /// The set contains all entries of all sections.
    HashMap< MonoAllocator,
             EntryKey,
             std::pair<Section*, Entry*>,
             EntryKey::Hash,
             EntryKey::EqualTo >            entryTable;


    /// Tests if the given string starts with <em>'#'</em>, <em>';'</em> or <em>'//'</em>.
    /// @param subs The string to test.
    /// @return \c true if this is a comment line, \c false otherwise.
    bool    startsWithCommentSymbol( String& subs );


  //====================================  Other Public fields    ===================================
  public:
    /// The list of sections.
    List<MonoAllocator, Section>    Sections;

    /// The file name.
    lang::system::PathString        FileName                                              = nullptr;

    /// The file header which will be written out as a comment lines with "# " prefixes.
    String                          FileComments                                          = nullptr;

    /// Filled with faulty line numbers when reading the file. (E.g., when a line is no section,
    /// no comment and not the attribute "writeback", but still has no equal sign ('=').
    List<MonoAllocator, integer>    LinesWithReadErrors;


  //=================================   Constructor/Destructor    ==================================
  public:
    /// Default constructor.
    ALIB_API            IniFile();

    /// Constructs an instance of this class and reads the file specified with \p{path}.
    /// @param path The filepath to the INI-file.
    inline IniFile( const lang::system::Path&  path )
    : IniFile()                                                                    { Read( path ); }

    /// Destructor.
                       ~IniFile()
    {
        #if ALIB_DEBUG
        for( auto& section : Sections )
            if( section.Name.IsNotEmpty() && section.Comments.IsNull() )
                ALIB_WARNING( "CONFIG",
                              "Hint: New section {!Q} , which was programmatically added to\n"
                              "      INI-file {!Q}, has no comments.",
                              section.Name, FileName )
        #endif
    }


  //========================================   Interface    ========================================

    /// Clears all data, resets the internal mono allocator.
    ALIB_API void       Reset();

    /// Counts the number of entries over all sections.
    /// @return The number of "variables" in this INI-file.
    inline   integer    Count()                                        { return entryTable.Size(); }

    /// Appends a new section to the end of the list of sections.
    /// Must be invoked only if a section with the same name does not exist, yet.
    /// @see Method SearchOrCreateSection.
    /// @param  name  The name of the section.
    /// @return The created section.
    ALIB_API Section*   CreateSection( const String& name);

    /// Deletes a section.
    /// @param  name  The name of the section to be deleted.
    /// @return The deleted section. If not found, \c nullptr is returned. The section will
    ///         still be a valid and accessible object within the mono allocator.
    ALIB_API Section*   DeleteSection( const String& name);

    /// Searches the section with the given name.
    /// @param sectionName  The name of the section to be retrieved.
    /// @return Returns the section if it was found, nullptr otherwise.
    ALIB_API Section*   SearchSection( const String& sectionName );

    /// Searches the section with the given name.
    /// @param sectionName  The name of the section to be retrieved.
    /// @return Returns the section if it was found, nullptr otherwise.
    ALIB_API std::pair<Section*, bool>  SearchOrCreateSection( const String& sectionName );

    /// Creates a new entry.
    /// Must be invoked only if the entry does not exist, yet. The given entry name is copied to
    /// the internal allocator.
    /// @param  section  The section.
    /// @param  name     The name of the entry to be created.
    /// @return The entry found or created.
    ALIB_API Entry*     CreateEntry ( Section* section, const String& name );

    /// Deletes an entry.
    /// @param  section  The section of the entry.
    /// @param  name     The name of the entry to be deleted.
    /// @return The entry deleted. If not found, \c nullptr is returned. The entry will
    ///         still be a valid and accessible object within the mono allocator.
    ALIB_API Entry*     DeleteEntry ( Section* section, const String& name );

    /// Deletes an entry. This overloaded searches the section by its name first.
    /// @param  sectionName  The section.
    /// @param  name         The name of the entry to be deleted.
    /// @return The entry deleted. If not found, \c nullptr is returned. The entry will
    ///         still be a valid and accessible object within the monotonic allocator.
    inline   Entry*     DeleteEntry ( const String& sectionName, const String& name )
    {
        auto* section= SearchSection( sectionName );
        if( section == nullptr )
            return nullptr;

        return DeleteEntry( section, name );
    }

    /// Searches an entry with the given name. The search is performed case insensitive.
    /// @param  section  The name of the section
    /// @param  name     The name of the entry to be searched.
    /// @return The section and entry if found, a \e nulled pair if not found.
    ALIB_API std::pair<Section*, Entry*>  SearchEntry (const String& section, const String& name );

    /// Parses the comments for newline tokens and adds given comment \p{prefix} to each line
    /// (in case no known comment prefix is present, yet). Then storage is allocated and
    /// output reference \p{dest} is set accordingly.
    /// @param  dest      The destination string to set.
    /// @param  comments  The comments to add.
    /// @param  prefix    The prefix to use if no other prefixes were found.
    ///                   Defaults to <c>"# "</c>.
    ALIB_API void       AddComments ( String& dest, const String& comments,
                                      const String& prefix= A_CHAR("# ") );

    //======================================   Read/Write    =======================================
  public:
    //==============================================================================================
    /// Reads an INI-File and adds its contents to the existing data.
    /// In case only the new entries should be contained, use method #Reset to delete existing
    /// data before invoking this function.
    ///
    /// It might happen that lines are ignored or otherwise marked as faulty. All numbers of such
    /// lines get collected in field #LinesWithReadErrors.
    ///
    /// Any other i/o errors throws, with the exception of "file not found", which simply
    /// returns \c -1.
    ///
    /// @param path   The file to read and write.
    /// @return \c -1 if the file does not exist. Otherwise the number of entries read.
    ///
    /// @throws Exception( \alib{config;Exceptions;config::Exceptions::ErrorOpeningFile} ).
    //==============================================================================================
    ALIB_API
    integer     Read( const lang::system::CPathString&  path );

    //==============================================================================================
    /// Writes the data into the file.
    /// @param path The file to to write. If this is nulled, the default, then the
    ///             same file name as with the last #Read is used.
    /// @throws Exception( \alib{config;Exceptions;config::Exceptions::ErrorOpeningFile} ).
    //==============================================================================================
    ALIB_API
    void        Write(const lang::system::PathString&  path= lang::system::NULL_PATH );
};

} // namespace alib[::config]


} // namespace [alib]

#endif // HPP_ALIB_CONFIG_INIFILE

