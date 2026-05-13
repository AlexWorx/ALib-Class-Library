//==================================================================================================
/// \file
/// This header-file is part of module \alib_resources of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
ALIB_EXPORT namespace alib::camp {

#if ALIB_CAMP_RESOURCE_COMPILATION || DOXYGEN

/// Error values that the function #"LoadResourceFile" may return.
enum class ResourceFileErrors {
    OK,                           ///< No error.
    RCFileNotFoundOrAccessible,   ///< Error opening the resource file.
    ErrIllegalResourceName,       ///< Resource name contains illegal characters. Allowed are ASCII
                                  ///< characters \c 33 and \c 35-\c 127.
    ErrNameTooLong,               ///< Resource name exceeds the maximum allowed length of 511
                                  ///< characters.
    ErrLineEndWhileInQuotes,      ///< Line ends while inside quotes.
    DuplicateResourceName,        ///< A duplicate resource name was found.

    CPPFileNotFoundOrAccessible,  ///< Error opening the cpp file to patch.
    CPPFileMissingStartMarker,    ///< Missing start marker in the cpp file to patch.
    CPPFileMissingEndMarker,      ///< Missing end marker in the cpp file to patch.
    CPPFileNotWritable,           ///< Error writing the cpp file to patch.
};

/// Represents an error that occurred with the function #"LoadResourceFile".
struct ResourceFileError {
    /// The resource key (identifier).
    ResourceFileErrors ErrorCode;

    /// The line number in the resource file where this error occurred.
    int                LineNo                                                                    =0;

    /// The column number in the resource file where this error occurred.
    int                ColNo                                                                     =0;

    /// A line number in the resource file where this error also relates to.
    int                LineNo2                                                                   =0;

    /// Additional error information. Set as follows:
    /// - With #"ResourceFileErrors::RCFileNotFoundOrAccessible", contains the file name.
    /// - With #"ResourceFileErrors::DuplicateResourceName", contains the resource name.
    /// - With #"ResourceFileErrors::ErrIllegalResourceName", contains the ascii code of the
    ///   character.
    Box                AdditonalInfo                                                       =nullptr;
};


/// Represents a single resource entry loaded from an <c>.alibrc</c> resource file.
/// This structure stores the key-value pair along with its source location for diagnostic purposes.
struct ResourceListEntry {
    NString Key;       ///< The resource key (identifier).
    String  Value;     ///< The resource value (content).
    int     LineNo;    ///< The line number in the resource file where this entry was found.
};

/// A collection of resource entries, typically loaded from an <c>.alibrc</c> file.
using ResourceList=         StdVectorMA<ResourceListEntry>;

/// A list of errors that occurred with #"LoadResourceFile".
using RCErrorList=          StdVectorMA<ResourceFileError>;

/// Patches a C++ source file with generated resource loading code.
///
/// This function updates a designated section in a C++ source file by replacing the content
/// between two marker comments with generated code that loads resources via
/// #"ResourcePool::BootstrapBulk;2".
///
/// The file given with \p cppFilePath must contain two commented marker lines:
/// - <c>ALIB-RESOURCE-COMPILER-REPLACEMENT-START</c>
/// - <c>ALIB-RESOURCE-COMPILER-REPLACEMENT-END</c>
///
/// All lines between these markers are replaced with generated key/value pairs formatted
/// as C++ string literals suitable for #"ResourcePool::BootstrapBulk;2".
///
/// The generated code includes:
/// - Properly escaped string literals for both keys and values
/// - Multi-line values broken into readable segments
/// - Aligned formatting for better readability
///
/// For an example of the generated output, see #"F;App/sample.cpp;2".
///
/// \see This method is typically called by #"DevtimeResourceCompiler::Do;2".
///
/// @param cppFilePath The path to the C++ source file to patch.
/// @param resources   The list of resources to generate code for.
/// @param errors      A list receiving errors that might occur.
/// @param dryRun      If given as \c true, the file is not written.
ALIB_DLL void PatchCPPFile( const Path&          cppFilePath,
                            const ResourceList&  resources,
                            RCErrorList&         errors,
                            bool                 dryRun= false    );


/// Reads an external resource file (`.alibrc`) and appends parsed resources to \p destination.
///
/// ## Syntax Overview
/// Each resource entry starts at a non-empty, non-comment line:
///
/// ```text
/// <Key> [=] <Value>
/// ```
///
/// - **Key**
///   - A sequence of printable ASCII characters (code 33..127), excluding `"` (double quote).
///   - Ends at whitespace or at `=` (if present).
///
/// - Optional **`=`**
///   - Whitespace may surround it:
///     - `Key Value`
///     - `Key=Value`
///     - `Key = Value`
///
/// - **Comments**
///   - A line whose first non-whitespace characters are `#` or `//` is ignored.
///
/// ## Value Types
///
/// 1. **Quoted**: starts with <c>'"'</c>.
///    - Supports backslash escapes (e.g. <c>'\\n'</c>, <c>'\\t'</c>, <c>'\\\\'</c>, <c>'\\"'</c>,
///      ...).
///    - Preserves spaces and tabs inside the quotes.
///
/// 2. **Plain**: anything else on the same line.
///    - Everything until end-of-line becomes the value (trailing whitespace is trimmed).
///    - Backslash escapes are processed.
///
/// 3. **Block scalars**: start with <c>'|'</c> or <c>'>'</c> (must appear as the first
///    non-whitespace of the value)
///    - `Key |`  <c>Literal block: preserves newlines.
///    - `Key >`  <c>Folded block</c>: converts line breaks to single spaces, except that empty lines
///      create paragraph breaks (<c>'\n'</c>).
///    - Block content starts on the next line and is terminated by a non-empty line whose indentation
///      is **not greater** than the indentation of the key line. (Empty lines are always part of the block.)
///    - Backslash escapes are processed in the block content.
///    - Trailing empty lines are ignored; i.e., trailing <c>'\\n'</c> characters are stripped from
///      the parsed value. To enforce trailing newlines, use explicit escape sequences at the end
///      of the last content line.
///
/// 4. **Compact scalars**: start with <c>'~'</c>
///    - Intended for machine-parsed token streams (e.g. `a,1,b,2,...`) that should be
///      human-formattable in the resource file.
///    - Outside quoted portions, <b>all whitespace is ignored</b> (including newlines in the
///      block form).
///    - Quoted portions are allowed and preserve whitespace:
///      <c>DateC  ~1,now,"1.1.2026",1,,2,file,1,filename,"my file"</c>
///    - Two forms:
///      - <c>Key ~</c>  : compact until end-of-line.
///      - <c>Key ~|</c> : compact block (indented), lines may be wrapped/aligned freely.
///    - Backslash escapes are processed (inside and outside quotes).
///
/// @param rcFileName    The path to the <c>.alibrc</c> input file.
/// @param destination   The destination vector to receive parsed resource entries.
/// @param errors        A list receiving read errors.
ALIB_DLL void LoadResourceFile( Path& rcFileName, ResourceList& destination,
                                RCErrorList& errors );

/// Writes a given list of errors (usually generated with function #"LoadResourceFile") to the
/// given \p{output} in a human-readable format.
/// @param errors    The error list.
/// @param output    The output formatter.
/// @param fileName  The name of the resource file.
ALIB_DLL void ListErrors( RCErrorList& errors, Paragraphs& output, const PathString& fileName);

#endif // ALIB_CAMP_RESOURCE_COMPILATION || DOXYGEN

/// Manages the compilation of externalized resources from <c>.alibrc</c> files into C++ source code.
///
/// This class provides a development-time mechanism to:
/// 1. Parse human-readable <c>.alibrc</c> resource files
/// 2. Register the resources in a \alibcamp's resource pool
/// 3. Generate C++ source code that hard-codes the resources for production builds
///
/// This class operates only when the configuration macro #"ALIB_CAMP_RESOURCE_COMPILATION" is set.
/// Otherwise, the class is an almost empty stub.
/// This approach allows a two-phase workflow:
///
/// <b>Development Phase:</b>
/// - Resources are defined in editable <c>.alibrc</c> text files
/// - The compiler parses these files at program startup
/// - Generated C++ code is written to designated source files
/// - Changes to resources are immediately reflected
///
/// <b>Production Phase:</b>
/// - The symbol #"ALIB_CAMP_RESOURCE_COMPILATION" is unset
/// - Resources are loaded from the generated C++ code
/// - No file I/O or parsing occurs at runtime
/// - The <c>.alibrc</c> files are not needed for distribution
///
/// The compiler uses file modification timestamps to determine when re-compilation is needed.
/// If the target C++ file is newer than the <c>.alibrc</c> file (with 10ms tolerance), no action
/// is taken.
///
/// \see
///   - Method #"DevtimeResourceCompiler::Do" for the main entry point.
///   - File #"F;App/sample.cpp;2" for a complete usage example.
///   - Chapter #"alib_camp_rc" of the Programmer's Manual of module \alibcamp_nl.
class DevtimeResourceCompiler {
  protected:
    #if ALIB_CAMP_RESOURCE_COMPILATION || DOXYGEN
    /// The allocator to use. Created only in case resources are to be compiled (and a C++ file
    /// is to be patched).
    MonoAllocator* ma                                                                      =nullptr;
    #endif

  public:
    #if ALIB_CAMP_RESOURCE_COMPILATION || DOXYGEN
    /// Destructor. This is empty if the symbol #"ALIB_CAMP_RESOURCE_COMPILATION" is not set.
    ~DevtimeResourceCompiler()                                                 { if(ma) delete ma; }

    /// Main entry point for resource compilation.
    ///
    /// This method performs the complete resource compilation workflow:
    /// 1. Resolves file paths (relative paths are resolved against \p CallingFile's directory)
    /// 2. Checks file modification timestamps to avoid unnecessary recompilation
    /// 3. Parses the <c>.alibrc</c> resource file
    /// 4. Registers all resources in the given camp's resource pool
    /// 5. Generates C++ code and patches the target source file
    ///
    /// **File Path Resolution:**
    /// - Both \p AlibrcFileName and \p CppFileName may be relative or absolute paths
    /// - Relative paths are resolved against the directory containing \p CallingFile
    /// - Typically, pass <c>__FILE__</c> as \p CallingFile to resolve relative to the calling source
    ///
    /// **Timestamp-Based Compilation:**
    /// - Compares modification times of <c>.alibrc</c> and <c>.cpp</c> files
    /// - If the C++ file is newer (with 10ms tolerance), no action is taken
    /// - Returns <c>false</c> to indicate no compilation occurred
    ///
    /// **Target File Requirements:**
    /// The C++ file specified by \p CppFileName must contain two marker comments:
    /// \code
    ///   // ALIB-RESOURCE-COMPILER-REPLACEMENT-START
    ///   ... generated code will replace this section ...
    ///   // ALIB-RESOURCE-COMPILER-REPLACEMENT-END
    /// \endcode
    ///
    /// All lines between these markers are replaced with generated resource loading code.
    /// The markers themselves are preserved.
    ///
    /// **Conditional Compilation:**
    /// When #"ALIB_CAMP_RESOURCE_COMPILATION" is not set, this method becomes a no-op
    /// and always returns <c>false</c>.
    ///
    /// **Example Usage:**
    /// \snippet{lineno} "App/sample.cpp" DOX_ALIB_SAMPLE_RESOURCE_COMPILER
    ///
    /// For the corresponding <c>.alibrc</c> file format, see manual section
    /// #"alib_camp_rc_rc_fileformat"..<br>
    /// For the generated output, see the replacement section in #"F;App/sample.cpp;2".
    ///
    /// @param alibrcFileName    Path to the input <c>.alibrc</c> resource file.
    /// @param callingFile       Reference file for resolving relative paths
    ///                          (typically <c>__FILE__</c>).
    /// @param campInstance      The \alibcamp instance whose resource pool receives the
    ///                          resources.
    /// @param resourceCategory  The resource category name for registering resources.
    /// @param allowReplacements Determines whether overriding existing resource-keys should
    ///                          raise an \alib_assertion.
    /// @param cppFileName       Path to the target C++ source file to be patched.
    ///                          If an empty string is given, then the resource file will be
    ///                          loaded in any case and resources will be added.
    /// @return
    ///   <c>true</c> if resources were compiled and the C++ file was updated successfully,
    ///   <c>false</c> if compilation was skipped (file already up-to-date), an error occurred,
    ///   or #"ALIB_CAMP_RESOURCE_COMPILATION" is not set.
    ALIB_DLL bool Do( const NString& alibrcFileName,
                      const NString& callingFile,
                      Camp&          campInstance,
                      const NString& resourceCategory,
                      bool           allowReplacements,
                      const NString& cppFileName        );
    #else
        bool Do(const NString&, const NString&, Camp&, const NString&, bool, const NString&)
        { return false; }
    #endif
};

} // namespace [alib::camp]
