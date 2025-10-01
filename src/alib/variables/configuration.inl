//==================================================================================================
/// \file
/// This header-file is part of module \alib_variables of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib {  namespace variables {

class  Configuration;
struct ConfigurationListener;
class  Variable;
class  Configuration;

namespace detail {
struct ConfigNodeHandler;

//==================================================================================================
/// This is the entry element type of the \alib{containers;StringTree} of class
/// \alib{variables;Configuration}.
//==================================================================================================
class Entry
{
  friend class  alib::variables::Variable;
  friend class  alib::variables::Configuration;
  friend struct alib::variables::detail::ConfigNodeHandler;

  protected:
    VDATA*             data       = nullptr; ///< The list hook of values.
    VMeta*             meta       ;          ///< The virtual handler instance for this variable.
    const Declaration* declaration;          ///< Declaration information. Can be \c nullptr in case
                                             ///< a variable was not declared using a record.
    Priority           priority   ;          ///< The priority level of this data element.

  public:
    /// Deleted copy-constructor.
    Entry(const Entry&)                                                                    = delete;

    /// Deleted move-constructor.
    Entry(Entry&&)                                                                         = delete;

    /// Defaulted default constructor.
    Entry() : data(nullptr), meta{nullptr}, declaration{nullptr}, priority{Priority::NONE}        {}
};  // class Entry


/// A shortcut to the base class of the base class of class \alib{variables;Configuration}.
using TTree= containers::detail::StringTreeBase< MonoAllocator, Entry, ConfigNodeHandler,
                                                 Recycling::Private  >;

/// Specialized \ref alib_ns_containers_stringtree_referencedoc "TNodeHandler" for class
/// \alib{variables;Configuration} which recycles variables and their extended information objects.
///
/// In respect to the node name allocation, this type behaves like
/// default handler \alib{containers;StringTreeNamesDynamic}. In debug compilations,
/// statistics variables \alib{containers;DBG_STATS_STRINGTREE_NAMES} and
/// \alib{containers;DBG_STATS_STRINGTREE_NAME_OVERFLOWS} are increased, just like the original
/// does.
struct ConfigNodeHandler
{
    /// The character type that the \b StringTree uses for child name and path strings.
    using CharacterType  = character;

    /// The string-type of a node's name. This is a simple static string, allocated with the
    /// pool allocator.
    using NameStringType =  strings::TString<character>;

    /// Copies the node's name to the local string.
    ///
    /// @param  tree  The instance of struct \alib{containers;detail::StringTreeBase} that invokes
    ///               this method. Any member may be accessed, including
    ///               \alib{containers::detail::StringTreeBase;nodeTable} which contains the
    ///               \alib{MonoAllocator} that the tree uses for the allocation of nodes.
    /// @param  node  The node that was just created. Allows access to the key and
    ///               custom value data. While the parent and sibling nodes are likewise accessible,
    ///               it is strictly forbidden to modify those.
    inline static
    void InitializeNode( TTree& tree, TTree::Node&  node );


    /// This implementation frees any dynamically allocated memory of the node's name and in
    /// addition recycles any extended information object attached to the \alib{variables;Variable}
    /// object.
    /// @param  tree  The instance of struct \alib{containers;detail::StringTreeBase} that invokes
    ///               this method. Any member may be accessed, including
    ///               \alib{containers::detail::StringTreeBase;nodeTable} which contains the
    ///               \alib{MonoAllocator} that the tree uses for the allocation of nodes.
    /// @param  node  The node that is to be removed. Allows access to the key and
    ///               custom value data. While the parent and sibling nodes are likewise accessible,
    ///               it is strictly forbidden to modify those.
    static ALIB_DLL
    void FreeNode( TTree& tree, TTree::Node& node );  // inline implementation is below
};  // struct ConfigNodeHandler

} // namespace detail



// =================================================================================================
///  Abstract virtual interface type to implement types observing configuration changes.
///  @see Chapter \ref alib_variables_monitoring of the Programmer's Manual of camp \alib_variables_nl.
// =================================================================================================
struct ConfigurationListener
{
    /// The type of change that imposes the notification of a listener.
    enum class Event
    {
        Creation,     ///< A variable was declared for the first time.
        Definition,   ///< A variable was defined or re-defined with the same or a higher priority.
        Deletion,     ///< A variable is deleted. Note that the variable is still valid
                      ///< when method #Notify is invoked and will be deleted right after
                      ///< the invocation of all listeners in question.
    };

    /// Virtual destructor.
    virtual ~ConfigurationListener()                                                              {}

    /// The virtual notification method.
    /// @param variable          The variable that was modified.
    /// @param event             The type of modification.
    /// @param previousPriority  The priority of the variable before event type
    ///                          \alib{variables;ConfigurationListener::Event;Event::Definition}.<p>
    ///                          With other events, this parameter is undefined.
    virtual void    Notify( const Variable& variable, Event event, Priority previousPriority )  = 0;

}; // struct ConfigurationListener

//==================================================================================================
/// Abstract class that defines the plug-in interface for class \alib{variables;Configuration}.
/// Plug-ins provide configuration data (variables) from external configuration data sources
/// at the moment those are requested.
///
/// The rationale for this concept is that some external configuration sources may provide a lot of
/// data that is not related to an application. This is for example true for environment
/// variables, the windows registry or the Gnome variable systems <em>gconf/dconf</em>.
/// In these cases, variables have to be read into the \alib variable system only at the moment
/// they are declared.
///
/// With other sources, like dedicated INI-files, a different technique is usually preferred:
/// Here, all data is read and imported into the configuration system with an application's
/// bootstrap and variables (respectively whole subtrees of variables) may be exported when an
/// application exits, just to populate empty INI-files with default values or to add new variables
/// which occur with a new software version.
///
/// Category and Variable names are character case-insensitive for the plug-ins predefined
/// with \alib. It is up to a custom implementation to decide to ignore character case in custom
/// specializations of this class as well.
//==================================================================================================
class ConfigurationPlugin  : public lang::Plugin<Configuration, Priority>
{
  protected:
    /// A default string escaper instance. This is used with all plugins provided with \alib.
    /// (Namely \alib{variables;CLIVariablesPlugin} and \alib{variables;EnvironmentVariablesPlugin}.)
    StringEscaperStandard   stringEscaper;

    /// Constructor which is protected, as this is an abstract class.
    /// @param pPriority      The priority that this plug-in uses.
    ConfigurationPlugin( Priority pPriority )
    : Plugin(pPriority)                                                                           {}

  public:
    /// Virtual Destructor.
    virtual ~ConfigurationPlugin() {}

    /// Derived types may return a different, customized implementation specific to their needs.
    /// This default implementation returns field #stringEscaper.
    /// @return An escaper used to convert string values from and to escaped sequences as provided
    ///         by a configuration source that is based on serialization of variable values to and
    ///         from ASCII/unicode strings which are human-readable and placeable in text files,
    ///         command line parameters, etc.
    virtual const StringEscaper&   GetEscaper()                     const  { return stringEscaper; }

    /// Abstract method. Descendents need to return a plug-in name. The name may be used in human
    /// readable output, e.g., log-files or exception messages to tell a user for example, which
    /// plug-in loaded a variable containing a syntax error.
    /// @return The name of the plug-in.
    virtual String                  Name()                                               const   =0;

    //==============================================================================================
    /// Abstract method that has to be overwritten by descendants.
    /// Searches and retrieves the value of a configuration variable.
    /// @param      name    The name of the variable to retrieve.
    /// @param[out] target  A reference to the buffer to write the variable's exported value to.
    /// @return \c true if variable was found within this configuration source, \c false if not.
    //==============================================================================================
    ALIB_DLL
    virtual bool  Get( const String& name, AString& target )                                     =0;
};


// =================================================================================================
/// This class is the container for \alib variables.<br>
/// The use of this type is documented with the \ref alib_mod_variables "Programmer's Manual" of
/// camp \alib_variables_nl.
///
/// Beside the interface that this class provides, publicly inherited interfaces of base types
/// \alib{containers;StringTree} and \alib{lang;PluginContainer} are available for use.
///
/// @see
///  Note that a general configuration object is provided with \alibcamps.
///  Usually one instance is shared between all \alibcamps_nl.
///  By \ref alib_mod_bs_customize "customizing the bootstrap process",
///  dedicated configuration instances for built-in or custom \alibcamps_nl can be created.
// =================================================================================================
class Configuration : public StringTree<MonoAllocator,detail::Entry, detail::ConfigNodeHandler>
                    , public lang::PluginContainer<ConfigurationPlugin, Priority>
{
    /// Alias to the StringTree-parent.
    using base = StringTree<MonoAllocator,detail::Entry, detail::ConfigNodeHandler>;

    // friends
    friend class Variable;
    friend class detail::Entry;
    friend struct detail::ConfigNodeHandler;

  public:
    /// An object pool for recycling variable values as well as the hashtable entries.
    /// It is \ref alib_contmono_chaining "chained" to the allocator provided with construction.
    ///
    /// The pool may be used in accordance with the general rules imposed by camp \alib_monomem.
    /// If so, in multithreaded environments, this configuration has to be locked (in addition
    /// to all other custom locks when interfacing this type), when using this pool from custom
    /// code.
    PoolAllocator               Pool;

protected:
    /// Record used to manage registered listeners.
    struct ListenerRecord
    {
        ConfigurationListener*  listener;      ///< The listener to register or dispose.
        int                     event;         ///< The event to listen to.
        ConstCursorHandle       variable;      ///< If given, the variable to listen to.
        ConstCursorHandle       subTree;       ///< If given, the subtree of variables to listen to.
        AStringPA               variableName;  ///< If given, the variable's name to listen to.
        AStringPA               pathPrefix;    ///< If given, the start string of the file path to monitor.
        AStringPA               pathSubstring; ///< If given, the substring to match in the path
                                               ///< (including the variable name) of files to monitor.
    };

    /// A default plugin created and inserted with construction.
    ConfigurationPlugin*        cliPlugin;

    /// A default plugin created and inserted with construction.
    ConfigurationPlugin*        environmentPlugin;

    /// Helper-struct used for implementing field #types.
    struct ValueDescriptorTypes : containers::TSubsetKeyDescriptor<VMeta*, const String&>
    {
        /// Returns the name of the type.
        /// @param src The value of the element to hash.
        /// @return The key-portion of the stored value.
        const String Key(const VMeta* src)                         const { return src->typeName(); }
    };

    
    /// A hashtable for the registered types. Key is the type name, value is the VMeta singleton.
    using TypesHashTable= HashTable<MonoAllocator            ,
                                    ValueDescriptorTypes     ,
                                    std::hash<String>        ,
                                    std::equal_to<String>    ,
                                    lang::Caching::Enabled   ,
                                    Recycling::None          >;
    
    /// A hashtable for the registered types. Key is the type name, value is the VMeta singleton.
    TypesHashTable                      types;


    /// Helper-struct used for implementing field #replacementDeclarations.
    struct ValueDescriptorRD : containers::TSubsetKeyDescriptor<Declaration*, const String&>
    {
        /// Returns the name of the declaration.
        /// @param src The value of the element to hash.
        /// @return The key-portion of the stored value.
        const String&  Key(const Declaration* src)                     const { return src->Name(); }
    };

    /// Type definition of the container that stores declarations allocated with method
    /// \alib{variables;Configuration::StoreDeclaration}.
    using RDHashTable= HashTable< MonoAllocator  ,   // TAllocator
                       ValueDescriptorRD         ,   // TValueDescriptor
                       std::hash    <String>     ,   // THash
                       std::equal_to<String>     ,   // TEqual
                       lang::Caching::Disabled   ,   // THashCaching
                       Recycling::None           >;  // TRecycling


    /// A hashtable for declarations allocated with method
    /// \alib{variables;Configuration::StoreDeclaration}.
    RDHashTable                                 replacementDeclarations;

    /// The list of registered listeners.
    List<MonoAllocator, ListenerRecord>         listeners;

  //==================================     Protected Methods     ===================================
    /// Implementation of \alib{variables;Configuration::RegisterType}.
    /// @tparam TVMeta    The meta-information type of the type to register.
    template<typename TVMeta>
    void registerType()
    {
        auto* meta= GetAllocator()().New<TVMeta>();
        types.EmplaceUnique( meta );
    }

    /// Stores the given information as a default value, which is imported to a variable
    /// as soon as it becomes declared.
    /// @see Overloaded public methods \alib{variables;Configuration::PresetImportString}.
    /// @param name     The name of the variable.
    /// @param value    The value to import.
    /// @param escaper  The escaper to use for importing.
    /// @param priority The priority to set for the value.
    ALIB_DLL  void presetImportString(const String& name, const String& value,
                                      const StringEscaper* escaper,   Priority priority);

    /// Implements the various overloaded listener registration methods.
    /// @param listener         The listener to register or dispose.
    /// @param insertOrRemove   Denotes registration or disposal of a listener.
    /// @param event            The event to listen to.
    /// @param variable         If given, the exported value of the variable to listen to.
    /// @param subTree          If given, the exported value of the subtree of variables to listen to.
    /// @param variableName     If given, the variable's name to listen to.
    /// @param pathPrefix       If given, the start string of the path of the variables to monitor.
    /// @param pathSubstring    If given, the substring to match in the path (including the variable name)
    ///                         of the variables to monitor.
    ALIB_DLL  void registerListener( ConfigurationListener*         listener,
                                     lang::ContainerOp              insertOrRemove,
                                     int                            event,
                                     const Variable*                variable,
                                     const StringTree::Cursor*      subTree,
                                     const String&                  variableName,
                                     const String&                  pathPrefix,
                                     const String&                  pathSubstring  );

    /// Notifies registered listeners on events.
    /// @param event            The event that occurred.
    /// @param variable         The variable.
    /// @param variablePath     The full path of the variable. Might be nulled if not available, yet.
    /// @param previousPriority The priority of the variable before the event.
    ALIB_DLL  void notifyListeners(  int                event,
                                     const Variable&    variable,
                                     const String&      variablePath,
                                     Priority           previousPriority  );

  //=====================================     Public Members     ===================================
  public:
    /// Number format definition used to read and write int and float values.
    /// Can be tweaked to change the way external information is parsed.
    alib::NumberFormat                  NumberFormat;

    /// A standard string escaper that can be used to set preset values received from external
    /// escaped strings.
    StringEscaperStandard               Escaper;

    /// The start string to identify substitution variables.
    /// Defaults to single character \c '$'. If a string is set, i.e., \c "${", then field
    /// #SubstitutionVariableEnd may be set accordingly, i.e., \c "}"
    String                              SubstitutionVariableStart                    = A_CHAR("${");

    /// The end of a substitution variables.
    /// If this field is set, then field #SubstitutionVariableDelimiters is ignored. If this
    /// field is nullptr (the default) or empty, it is ignored and characters in field
    /// #SubstitutionVariableDelimiters are used to identify the end of the variable.
    String                              SubstitutionVariableEnd                      = A_CHAR("}");

    /// The delimiters used to identify the end of a substitutable variable.
    /// If field #SubstitutionVariableEnd is not empty, this field is ignored. Otherwise, all
    /// characters defined in this string are used to identify the end of a substitution
    /// variable.
    CString            SubstitutionVariableDelimiters=  A_CHAR(" $@,.;:\"\'+-*\\§%&()[]{}<>=?'`~#");

    /// This is a list that holds pairs of type \alib{strings::util::Token} which is used by the
    /// built-in boolean variable type (type name "B").
    /// The list defines how extern string data (for example, from command line args, environment
    /// variables, or INI-files) is converted to boolean values.
    ///
    /// When parsed from outside, the index of the pair that had the match is stored and
    /// in the case of writing back (e.g., to the INI-file), the corresponding representation of
    /// this pair will be written out. (For example, if a user used "On" in the configuration file,
    /// and the software switched it to \c false, the word "Off" is to be written.
    ///
    /// During bootstrap, this list might be modified, but before any variable was read. Otherwise
    /// the indices would become invalid. However, the preferred way of changing the accepted
    /// tokens is to modify the resources. This allows having localization of terms, in case
    /// this should be needed some day.
    ///
    /// If even with modification of the predefined list,
    /// some other needs or flexibility is wanted, then the definition of an alternative
    /// boolean configuration variable type is the way to go.
    ///
    /// The list of resourced predefined tokens after construction of a configuration is:
    ///
    ///  Index  | True Token          | False Token
    /// --------|---------------------|--------------------
    ///    0    |   <c>false I 1</c>  | <c>true  I 1</c>
    ///    1    |   <c>0     I 1</c>  | <c>1     I 1</c>
    ///    2    |   <c>no    I 1</c>  | <c>yes   I 1</c>
    ///    3    |   <c>off   I 3</c>  | <c>on    I 2</c>
    ///    4    |   <c>---   I 1</c>  | <c>OK    I 2</c>
    ///
    /// @see Methods #ParseBooleanToken and #WriteBooleanToken.
    List<MonoAllocator, std::pair<Token,Token>, Recycling::None> BooleanTokens;


  //=============================       Constructor/destructor     =================================
    /// Constructs a Configuration.
    ///
    /// If \p{addDefaultPlugins} is \c true, registers the initial plug-ins as follows:
    ///
    ///  Priority                                      | Plug-in Type
    /// -----------------------------------------------|------------------------------
    ///  \alib{variables;Priority;Priority::Environment}  | \alib{variables;EnvironmentVariablesPlugin}
    ///  \alib{variables;Priority;Priority::CLI}          | \alib{variables;CLIVariablesPlugin}
    ///
    /// Furthermore all \ref alib_variables_types_builtin "built-in variable types" are registered.
    ///
    /// Modifications (in respect to plug-ins and registered types) should
    /// be performed during bootstrap, right after construction of this type (when no parallel
    /// threads are active) or with prior  locking the \alib{variables;Configuration} instance.
    ///
    /// @param allocator      The allocator to use.
    /// @param createDefaults Determines if default plug-ins are to be created.
    ///                       Defaults to \c Yes.
    ALIB_DLL
    Configuration( MonoAllocator&       allocator,
                   lang::CreateDefaults createDefaults= lang::CreateDefaults::Yes );

    /// Destructor.
    ALIB_DLL ~Configuration();

//========================================    Interface    =======================================
    /// Registers a variable type with this configuration. The only parameter is the template
    /// parameter \p{TVMeta}. The function will create the singleton of this type and store
    /// it in a hash table of types. When a variable associated with this configuration object
    /// is declared, this variable type is used as the key to this hash table.
    /// Then this singleton object of the given type is responsible for construction, destruction
    /// and import/export of variables of this type.
    /// @tparam TVMeta    The meta-information type of the type to register.
    template<typename TVMeta>
    void            RegisterType()                                       { registerType<TVMeta>(); }

    /// Creates and stores a declaration. Placeholders \c "%1", \c "%2" ... \c "%N" found in
    /// the fields (except \alib{variables::Declaration;typeName}) are replaced with the
    /// replacement values given with boxed argument (list) \p{replacements}.
    ///
    /// \attention
    ///   This method creates a copy a new declaration instance within the monotonic allocator
    ///   of this \b Configuration object. This imposes a potential memory drain (leak).
    ///   To mitigate the problem and widen the use cases, this method uses a hashtable
    ///   to cache the resulting declarations. This means, that only for combinations of parameters
    ///   that result to a different variable name, a declaration is created.
    ///
    /// @see Chapter \ref alib_variables_names_placeholders of the Programmer's Manual of
    ///      camp \alib_variables_nl.
    ///
    /// @param orig         The original, typically resourced declaration with placeholders.
    /// @param replacements A list of replacement values. Must be of boxed types that are
    ///                     \alib{strings,AppendableTraits,appendable} to class \b AString.
    ///                     To provide more than one object, pass an object of class
    ///                     \alib{boxing,Boxes} or a boxed array.
    /// @return The allocated (or cached) copy of \p{orig}.
    ALIB_DLL
    const Declaration*  StoreDeclaration( const Declaration* orig, const Box& replacements );

    /// Declares and defines all variables of the given enum type which do not contain
    /// placeholders are contained in the variable's name.
    ///
    /// The purpose of this method is to fill the configuration system with variables
    /// (usually during or right after bootstrap) regardless of their later use, which depends
    /// on the concrete execution path of a run of software. The rationale to do this is is
    /// twofold:
    /// 1. To allow external configuaration plug-ins to set all variable's values, which is not
    ///    possible if a variable is not declared, yet, and thus its type is not known. In
    ///    that case, an external source can only use method
    ///    \alib{variables;Configuration::PresetImportString}, which in the end leads to correctly
    ///    setting a variable's external value, but which creates a little overhead in respect
    ///    to execution time and memory consumption.
    /// 2. To give external configuaration plug-ins the possibiltiy to "populate" their files
    ///    with all known variables, including their default values and comments, again
    ///    independently from a concrete excution path of a run of the software, which might
    ///    not declare certain variables and thus those would not be "known" to the external
    ///    source. Populating external configuration files (with the first run of software),
    ///    tremendously supports an end-user in learning about configuration options.<br>
    ///    For example, built-in class \alib{variables;IniFileFeeder} supports such "population"
    ///    of for INI-fles through its \b Export methods.
    ///
    /// Altogether, the use of this method does not not impose much of an overhead, because:
    /// - as explained above, the need of method \alib{variables;Configuration::PresetImportString}
    ///   is avoided for all variables that do not contain placeholders, and because
    /// - most variables would anyhow be declared later during the run of software
    ///   and thus, this declaration is just performed earlier and the variable are already in
    ///   place when software later uses them.
    ///
    /// @see Chapter \ref alib_variables_external_ini of the Programmer's Manual of camp
    ///      \alib_variables.
    ///
    /// @tparam TEnum Enumeration type equipped with \ref alib_enums_records "ALib Enum Records"
    ///               of type \alib{variables;Declaration}.
    template<typename TEnum>
    requires ( EnumRecords<TEnum>::template AreOfType<Declaration>() )
    void PreloadVariables();

    /// Stores the given information as a default value, which will be assigned to a variable as
    /// soon as it is declared. If a variable is deleted and later again declared, the given
    /// \p{value} is again imported. In other words, a preset will not be removed with a first
    /// declaration and use.<br>
    /// To delete a previously set import string, a \e nulled string is to be passed with
    /// for parameter \p{value}.
    ///
    /// This method is to be used in cases when a declaration of a variable is not possible,
    /// i.e., when a variable's type is not known, which often is the case when external
    /// configuration values are to be set.
    ///
    /// @see
    ///   - Overloaded public methods #PresetImportString.
    ///   - Chapter \ref  alib_variables_external_ini of the Programmer's Manual of camp
    ///     \alib_variables.
    /// @param name     The name of the variable.
    /// @param value    The value to import. Will be 'unescaped' using \p{escaper}.
    /// @param escaper  The escaper to use for importing. Has to be still valid at the time(s)
    ///                 the value read. A pointer to field #Escaper might be passed here.
    /// @param priority The priority to set for the value.
    ///                 Defaults to \alib{variables;Priority;Default}.
    void PresetImportString(const String& name, const String& value, StringEscaper* escaper,
                            Priority priority= Priority::DefaultValues)
    { presetImportString( name, value, escaper, priority);  }

    /// Same as overloaded version
    /// #PresetImportString(const String&, const String&,StringEscaper*, Priority),
    /// but does not accept an \alib{strings::util;StringEscaper} and thus expects the value is in
    /// C++ internal string format (aka 'not escaped').
    /// @param name     The name of the variable.
    /// @param value    The value to import.
    /// @param priority The priority to set for the value.
    ///                 Defaults to \alib{variables;Priority;Default}.
    void PresetImportString( const String& name, const String& value,
                             Priority priority= Priority::DefaultValues )
    { presetImportString( name, value, nullptr, priority);  }

    /// Deletes the given variable \p{path}, respectively all variables under the given \p{path} as
    /// well as the path node itself.
    ///
    /// This is a convenience method. Full access to the underlying \alib{containers;StringTree}
    /// is provided by public inheritance, which offers many other ways of manipulating
    /// variable data. As an example, consider the implementation of this method:
    /// \snippet "variables/configuration.cpp"  DOX_VARIABLES_DELETE_SAMPLE
    ///
    ///
    /// @param path     The path in the variable tree.
    /// @return \c true on success, \c false, if the given \p{path} was not found.
    ALIB_DLL
    bool                      DeletePath( const String& path );

    //==============================================================================================
    /// Utility method that checks if a given value represents boolean \b true.
    ///
    /// @see Field #BooleanTokens, which is used by this function and sibling method
    ///      #WriteBooleanToken
    ///
    /// @param src  The input string to check.
    /// @return   Returns the value found and the index of the pair of tokens that matched.
    ///           If no token matched <c>(false,-1)</c> is returned.
    //==============================================================================================
    ALIB_DLL
    std::pair<bool,int8_t>    ParseBooleanToken( const String&  src );

    /// Utility method that writes the given boolean value as specified with the token
    /// found at \p{index} in #BooleanTokens.<br>
    /// This method is used internally by the meta-information handler of variable type
    /// <c>"B"</c>/<c>bool</c>.
    /// @see Sibling method #ParseBooleanToken.
    ///
    /// @param value  The boolean value to write.
    /// @param index  The token to use. If negative, this is corrected to \c 0.
    /// @param dest   The destination string.
    /// @return \p{dest} to allow concatenated operations.
    ALIB_DLL
    AString&                  WriteBooleanToken( bool value, int8_t index, AString& dest );


    // ===============================   Listener Registration   ===================================

    /// Inserts or removes a listener to a specific variable.
    /// Note that this version of the method cannot be used to fetch declaration events,
    /// because the given \p{variable} obviously is already declared and created.
    /// @param insertOrRemove Denotes whether the listener should be inserted or removed.
    ///                       (Only enum elements \b ContainerOp::Insert or \b ContainerOp::Remove
    ///                       must be passed.)
    /// @param listener       The listener to register.
    /// @param event          The event to listen to.
    /// @param variable       The variable to listen to.
    /// @see Chapter \ref alib_variables_monitoring of the Programmer's Manual of camp \alib_variables_nl.
    void MonitorDistinctVariable( lang::ContainerOp               insertOrRemove,
                                  ConfigurationListener*          listener,
                                  ConfigurationListener::Event    event,
                                  const Variable&                 variable       )
    {
        ALIB_ASSERT_WARNING( event != ConfigurationListener::Event::Creation, "VARIABLES",
                    "Event::Creation will never be invoked with this listener-registration-type.")
        registerListener( listener,
                          insertOrRemove,
                          int(event),
                          &variable, nullptr,
                          NULL_STRING, NULL_STRING, NULL_STRING );
    }

    /// Inserts or removes a listener for all variables that share the given \p{variableName}.
    /// @param insertOrRemove Denotes whether the listener should be inserted or removed.
    ///                       (Only enum elements \b ContainerOp::Insert or \b ContainerOp::Remove
    ///                       must be passed.)
    /// @param listener       The listener to register.
    /// @param event          The event to listen to.
    /// @param variableName   The name of one or more variables to listen to.
    /// @see Chapter \ref alib_variables_monitoring of the Programmer's Manual of camp \alib_variables_nl.
    void MonitorVariablesByName( lang::ContainerOp              insertOrRemove,
                                 ConfigurationListener*         listener,
                                 ConfigurationListener::Event   event,
                                 const String&                  variableName )
    {
        ALIB_ASSERT_ERROR( variableName.IsNotEmpty(), "VARIABLES", "Empty variable name given.")
        registerListener( listener, insertOrRemove, int(event),
                          nullptr, nullptr, variableName, NULL_STRING, NULL_STRING );
    }


    /// Inserts or removes a listener for all variables below subtree specified by the
    /// given \p{cursor}.
    /// @param insertOrRemove Denotes whether the listener should be inserted or removed.
    ///                       (Only enum elements \b ContainerOp::Insert or \b ContainerOp::Remove
    ///                       must be passed.)
    /// @param listener       The listener to register.
    /// @param event          The event to listen to.
    /// @param cursor         The parent node in the configuration variable tree of the variables
    ///                       to monitor.
    /// @see Chapter \ref alib_variables_monitoring of the Programmer's Manual of camp \alib_variables_nl.
    void MonitorPath(  lang::ContainerOp               insertOrRemove,
                       ConfigurationListener*          listener,
                       ConfigurationListener::Event    event,
                       const Configuration::Cursor&    cursor            )
    {
        registerListener( listener,
                          insertOrRemove,
                          int(event),
                          nullptr, &cursor,
                          NULL_STRING, NULL_STRING, NULL_STRING );
    }



    /// Inserts or removes a listener for all variables below the subtree specified by the
    /// given \p{startPath}.
    /// @param insertOrRemove Denotes whether the listener should be inserted or removed.
    ///                       (Only enum elements \b ContainerOp::Insert or \b ContainerOp::Remove
    ///                       must be passed.)
    /// @param listener       The listener to register.
    /// @param event          The event to listen to.
    /// @param pathPrefix     The path prefix of the subtree of variables to monitor. Note that a
    ///                       missing leading separator character <c>'/'</c> will be added.
    /// @see Chapter \ref alib_variables_monitoring of the Programmer's Manual of camp \alib_variables_nl.
    void MonitorPathPrefix( lang::ContainerOp               insertOrRemove,
                            ConfigurationListener*          listener,
                            ConfigurationListener::Event    event,
                            const String&                   pathPrefix      )
    {
        ALIB_ASSERT_ERROR( pathPrefix.IsNotEmpty(), "VARIABLES", "Empty path prefix given.")
        registerListener( listener,
                          insertOrRemove,
                          int(event),
                          nullptr, nullptr,
                          NULL_STRING, pathPrefix, NULL_STRING );
    }

    /// Inserts or removes a listener for all variables below the subtree specified by the
    /// given \p{startPath}.
    /// @param insertOrRemove Denotes whether the listener should be inserted or removed.
    ///                       (Only enum elements \b ContainerOp::Insert or \b ContainerOp::Remove
    ///                       must be passed.)
    /// @param listener       The listener to register.
    /// @param event          The event to listen to.
    /// @param pathSubstring  The substring to match in the path (including the variable name)
    ///                       of the variables to monitor.
    /// @see Chapter \ref alib_variables_monitoring of the Programmer's Manual of camp \alib_variables_nl.
    void MonitorPathSubstring( lang::ContainerOp               insertOrRemove,
                               ConfigurationListener*          listener,
                               ConfigurationListener::Event    event,
                               const String&                   pathSubstring  )
    {
        ALIB_ASSERT_ERROR( pathSubstring.IsNotEmpty(), "VARIABLES", "Empty path substring given.")
        registerListener( listener,
                          insertOrRemove,
                          int(event),
                          nullptr, nullptr,
                          NULL_STRING, NULL_STRING, pathSubstring );
    }

    /// Removes all registrations of the given listener.
    /// @param listener The listener to remove.
    /// @return The number of registrations that have been removed.
    /// @see Chapter \ref alib_variables_monitoring of the Programmer's Manual of camp \alib_variables_nl.
    ALIB_DLL
    int MonitorStop( ConfigurationListener*  listener );

}; // struct Configuration

void detail::ConfigNodeHandler::InitializeNode( TTree& tree, typename TTree::Node&  node )
{
    node.name.storage.Allocate(static_cast<Configuration&>(tree).Pool, node.name.key );
}


//==================================================================================================
// Class Variable
//==================================================================================================

//==================================================================================================
/// This is the central interface type into the module \alib_variables.
/// The class is a very lightweight (size of two times <c>sizeof(void*)</c>) object, because
/// it rather is a pointer to a variable instead of an implementation of its data structures.
/// The types' true nature is inherited from class \alib{containers;StringTree::Cursor}
/// which is the base of this class. The inheritance is defined <c>protected</c> and this class
/// received its own dedicated interface reflecting the domain of <em>configuration variables</em>.
/// Nevertheless, the protected base type is accessible with the explicit cast method #AsCursor.
/// While a reader might think this is a contradiction, this was made by design: Casting to
/// a <b>StringTree::Cursor</b> is only necessary for implementing \alib{variables;ConfigurationPlugin}
/// or similar extraordinary things. For the common use, the dedicated interface is all that is
/// needed.
///
/// @see
///   - For a quick tutorial about using \alib variables, consult the tutorial-style
///     \ref alib_mod_variables "Programmer's Manual" of camp \alib_variables_nl.
///   - For this class, a \ref alibtools_debug_helpers_gdb "pretty printer" for the
///     GNU debugger is provided.
///   - Besides the constructors found with this class, with the inclusion of the module
///     \alib_camp in the \alibbuild, and the inclusion of the header file \implude{Camp},
///     a set of overloaded namespace functions \alib{variables;CampVariable} allow the construction
///     of variables in the \b Configuration instance included in \alibcamps.
//==================================================================================================
class Variable : protected Configuration::Cursor
{
    friend class alib::variables::Configuration;

    /// The base cursor type of the internal \b StringTree. This type is used to perform
    /// cursor operations on \b Configuration instances.
    using Cursor= Configuration::Cursor;

  protected:
    /// Called by declaration methods. Creates necessary internal data structs, searches
    /// configuration plug-ins for data, and, in case not found, optionally applies the given
    /// \p{defaultValue}
    /// @param typeName      The type name.
    /// @param defaultValue  A default value, if available.
    void    create(const String& typeName, const String& defaultValue );

    /// Performs variable substitution. This method is called before any call to
    /// \alib{variables;VMeta::imPort}.
    /// @param importString The import string.
    /// @param buf          The substitution buffer.
    /// @param escaper      The escaper used to externalize substitution values.
    /// @return Either the original import string, or a reference to the given buffer containing
    /// the substituted copy.
    const String&    substitute(const String& importString, AString& buf,
                                const StringEscaper* escaper );

    /// Returns the meta-information object.
    /// @return The virtual meta handler.
    inline VMeta* getMeta()                                   const { return Cursor::Value().meta; }


    // #############################################################################################
    // Constructors
    // #############################################################################################
  public:
    /// Default constructor.
    /// \note
    ///   A variable that was default-constructed cannot be used later, without assigning
    ///   a configuration with declaration. If for example a field member of type variable
    ///   cannot be constructed with the provision of a corresponding \b Configuration, one of
    ///   the following options has to be taken:
    ///   - The variable value gets a newly constructed instance assigned at a later stage
    ///     when the  configuration is known, or
    ///   - the configuration is provided with one of the overloaded #Declare methods that
    ///     accept a configuration.
    Variable()                                                                            = default;

    /// Constructs an instance of this type from its base type.
    /// This constructor is for advanced use when direct operations with class \b StringTree and
    /// its cursor and iterator types are performed.<br>
    /// Note that the variable has to be declared already, when this constructor is used.
    /// @param cursor The \b StringTree cursor.
    Variable(const Cursor& cursor  )
    : Cursor(cursor)                                                                              {}

    /// Constructs an instance of this type from its base type.
    /// This constructor is for advanced use when direct operations with class \b StringTree and
    /// its cursor and iterator types are performed.<br>
    /// Note that the previous variable declaratin will be destroyed, in case a variable had been
    /// declared at this cursor already.
    /// @param cursor The \b StringTree cursor.
    /// @param typeName      The type name.
    /// @param defaultValue  A default value, if available. Defaults to \e nulled string.
    Variable( const Cursor& cursor, const String& typeName, const String& defaultValue= NULL_STRING )
    : Cursor(cursor)      { Cursor::Value().declaration= nullptr; create( typeName, defaultValue); }

    /// Constructs an undeclared variable "cursor" associated with the given configuration object.
    /// \note The internal \alib{containers,StringTree::Cursor} equals
    ///       the root cursor received with \alib{containers,StringTree::Root} after this
    ///       construction.
    /// @see Overloaded namespace functions \alib{variables;CampVariable} which become accessible
    ///      with the header file \implude{Camp}.
    /// @param cfg The configuration to use.
    explicit Variable(Configuration& cfg)
    : Cursor(cfg.Root())                                                                          {}

    /// Constructs and declares this variable, without using or allocating a declaration struct.
    /// @see Overloaded namespace functions \alib{variables;CampVariable} which become accessible
    ///      with the header file \implude{Camp}.
    /// @param cfg          The configuration to use.
    /// @param name         The name of the variable.
    /// @param typeName     The type of the variable.
    /// @param defaultValue An optional default value. Defaults to \e nulled string.
    Variable( Configuration& cfg,
              const String&  name, const String& typeName,
              const String&  defaultValue= NULL_STRING )
    : Cursor(cfg.Root())                                { Declare( name, typeName, defaultValue ); }


    /// Constructs and declares this variable.
    /// @see Overloaded namespace functions \alib{variables;CampVariable} which become accessible
    ///      with the header file \implude{Camp}.
    /// @param cfg The configuration to use.
    /// @param decl The declaration to use.
    Variable(Configuration& cfg, const Declaration* decl)
    : Cursor(cfg.Root())                                                         {Declare( decl ); }

    /// Constructs and declares this variable.
    /// @see Overloaded namespace functions \alib{variables;CampVariable} which become accessible
    ///      with the header file \implude{Camp}.
    /// @tparam TEnum  The type of parameter \p{Enum}
    /// @param  cfg    The configuration to use.
    /// @param  Enum   Element of an enum type that represents resourced configuration variables.
    template<typename TEnum>
    inline
    Variable(Configuration& cfg, TEnum Enum )
    : Cursor(cfg.Root())                                      { Declare( Declaration::Get(Enum) ); }

    /// Constructs and declares this variable.
    /// \attention This constructor might allocate a new record in the mono allocator of \p{cfg}.
    ///            To avoid memory drain, consult the documentation of the method
    ///            \alib{variables;Configuration::StoreDeclaration}.
    /// @see Overloaded namespace functions \alib{variables;CampVariable} which become accessible
    ///      with the header file \implude{Camp}.
    /// @tparam TEnum         The type of parameter \p{Enum}
    /// @param  cfg           The configuration to use.
    /// @param  Enum          Element of an enum type that represents resourced configuration variables.
    /// @param  replacements  Replacement values.
    template<typename TEnum>
    requires std::is_enum_v<TEnum>
    Variable(Configuration& cfg, TEnum Enum, const Box replacements )
    : Cursor(cfg.Root())  { Declare( StoreDeclaration( Declaration::Get( Enum ), replacements ) ); }

    /// This is an explicit <c>cast operator</c> to the protected base class.
    /// \note C++, for good reasons, does not allow implicit cast operators to protected base
    ///       classes, as this would undermine the protected nature of the inheritance.
    ///       Now, we undermine this explicitly. Why is this still good software design? Well,
    ///       class \b Variable restricts and changes the interface of class \b Cursor in a way that
    ///       it seems to be a very different entity. And this is right in the vast majority of use
    ///       cases. In the remaining cases such access to the cursor features is needed. For
    ///       example, when implementing a custom \alib{variables;ConfigurationPlugin}.<br>
    ///       By the same token, this class has explicit constructors that accept a \c Cursor.
    /// @return This instance cast 'down' to its protected base class.
    Cursor& AsCursor()                                       { return static_cast<Cursor&>(*this); }

    /// \c const version of the <c>cast operator</c> to the protected base class.
    /// @see For details, see the mutable version of this operator.
    /// @return This instance cast 'down' to its protected base class.
    const Cursor& AsCursor()                     const { return static_cast<const Cursor&>(*this); }

  // #############################################################################################
  // Declaration
  // #############################################################################################
  public:
    /// Declares this variable, without using or allocating a declaration struct.
    ///
    /// @param name         The name of the variable.
    /// @param typeName     The type of the variable.
    /// @param defaultValue An optional default value. Defaults to \e nulled string.
    /// @return <c>*this</c> to allow concatenated operations.
    ALIB_DLL
    Variable&      Declare( const String& name, const String& typeName,
                            const String& defaultValue= NULL_STRING );

    /// Moves this variable cursor (aka \alib{containers;StringTree::Cursor})
    /// to represent the variable of the given declaration. If the variable does not exist, yet
    /// it is created. If the variable exists, the given \p{decl} is ignored.
    /// In debug compilations, an \ref alib_mod_assert "error is raised" in case the variable
    /// existed but has a different declaration attached.
    ///
    /// If the variable is created and the \p{declaration} contains a default value, such value is
    /// imported from the string. In this case, \alib{variables;Priority;DefaultValues} is returned.
    ///
    /// @param declaration The declaration singleton.
    /// @return <c>*this</c> to allow concatenated operations.
    ALIB_DLL
    Variable&      Declare( const Declaration* declaration );

    /// Declares this variable.
    /// @tparam TEnum  The type of parameter \p{Enum}
    /// @param  Enum   Element of an enum type that represents resourced configuration variables.
    /// @return The priority of the variable's data.
    template<typename TEnum>
    requires ( EnumRecords<TEnum>::template AreOfType<Declaration>() )
    Variable&      Declare( TEnum Enum )              {  return Declare( Declaration::Get(Enum) ); }


    /// Declares this variable.
    /// \attention
    ///   This method might allocate a new record in the mono allocator of \p{cfg}.
    ///   To avoid memory drain, consult the documentation of the method
    ///   \alib{variables;Configuration::StoreDeclaration}.
    ///
    /// @tparam TEnum         The type of parameter \p{Enum}
    /// @param  Enum          Element of an enum type that represents resourced configuration variables.
    /// @param  replacements  Replacement values.
    /// @return The priority of the variable's data.
    template<typename TEnum>
    requires ( EnumRecords<TEnum>::template AreOfType<Declaration>() )
    Variable&   Declare( TEnum Enum, const Box& replacements )
    {
        return Declare( GetConfiguration().StoreDeclaration( Declaration::Get( Enum ),
                                                             replacements               ) );
    }

    /// Returns \c true if this variable is declared. If \c false is returned,
    /// this the is just a path node inside the \b StringTree of the \b Configuration.<br>
    ///
    /// \c false will be returned only if:
    /// - An instance of a variable was created with the constructor that only accepts a
    ///   \b Configuration instance (an undeclared variable).
    /// - An instance of a variable was created with the constructor that accepts a cursor
    ///   object of a \b Configuration, and this cursor does not point to a declared variable.
    /// - Method #Try was called on an instance of this type, and the given variable name
    ///   did not represent a declared variable.
    ///
    /// @return \c true if the underlying \b StringTree::Cursor of the \b Configuration that this
    ///         variable is associated with, points to a declared variable.
    ///         Otherwise \c false is returned.
    bool                IsDeclared()               const { return Cursor::Value().data != nullptr; }

    /// Tries to move the underlying \b StringTree::Cursor of the \b Configuration that this
    /// variable is associated with, to the given \p{path} (aka variable name).
    /// If this succeeds, the result of #IsDeclared() is returned. Otherwise \c false.
    ///
    /// Note that this method still returns \c false if a
    /// \alib{variables;Configuration::PresetImportString;preset value} exists for the variable,
    /// because preset values can only be imported if a variable's type is known, hence when
    /// a variable is declared. But this method explicitly avoids declarations.
    ///
    /// @see Overloaded versions, which declare and define a variable if a preset exists.
    ///
    /// @param name The name of the variable to try.
    /// @return \c true if the variable name specified with the given \p{declaration} existed and
    ///         was declared. Otherwise \c false is returned and the variable's value is not allowed
    ///         to be accessed.
    bool                Try(const String& name)
    { return Cursor::GoToRoot().GoTo( name ).IsEmpty() && IsDeclared(); }

    /// Tries to move the underlying \b StringTree::Cursor of the \b Configuration that this
    /// variable is associated with, to the given \p{path} (aka variable name).
    ///
    /// \note
    ///    This method automatically declares and defines variables, for which an
    ///    \alib{variables;Configuration::PresetImportString;preset value} is set.
    ///
    /// @param name         The name of the variable.
    /// @param typeName     The type of the variable.
    /// @return \c true if the variable name specified with the given \p{declaration} existed and
    ///         was declared. Otherwise \c false is returned and the variable's value is not allowed
    ///         to be accessed.
    ALIB_DLL
    bool                Try(const String& name, const String& typeName);

    /// Tries to move the underlying \b StringTree::Cursor of the \b Configuration that this
    /// variable is associated with, to the variable name found with given \p{declaration}.
    /// If this succeeds, it is checked if the variable has no declaration record set, and if not,
    /// the one given is stored with the variable.
    ///
    /// \note
    ///    This method automatically declares and defines variables, for which an
    ///    \alib{variables;Configuration::PresetImportString;preset value} is set.
    ///
    /// @param decl The declaration of the variable to use.
    /// @return \c true if the variable name specified with the given \p{declaration} existed and
    ///         was declared, or if a preset value existed. Otherwise \c false is returned and
    ///         the variable's value is not allowed to be accessed.
    ALIB_DLL
    bool                Try(const Declaration* decl);

    /// Deletes a variable. After this method is executed, the internal cursor will be
    /// invalid and further operations on this variable instance can only be made after a next
    /// declaration.
    ALIB_DLL
    void                Delete();

    /// Assembles the variable's name from its path within the internal
    /// \alib{containers;StringTree}.
    /// @param target The string buffer to append this variable's name to.
    /// @return The given \b AString to allow concatenated operations.
    AString&            Name(AString& target)                                                  const
    {
        return AssemblePath(target, Tree().Root(), lang::CurrentData::Keep);
    }


    // #############################################################################################
    // Get/Delete
    // #############################################################################################
    public:

    /// Increases the reported priority of this variable to the value given. If the current value
    /// is higher than the given one, it is not increased and \c false is returned.
    /// In this case, a program should not write the variable's value.
    ///
    /// \note
    ///   A programmer can easily ignore the result of this method. However, the suggested
    ///   usage is a contract to make the \alib variable system secure.
    ///   For this reason, this method is attributed <c>[[nodiscard]]</c>.<br>
    ///
    /// \note
    ///   If the return value is to be ignored for good reason (for example if
    ///   \alib{variables;Priority;Priority::Protected} is passed, add a simple <c>(void)</c> before
    ///   the call.
    ///
    /// @see
    ///  - Chapter \ref alib_variables_definition of the Programmer's Manual for more information.
    ///  - Chapter \ref alib_variables_monitoring to learn about how to monitor changes of variables.
    ///
    /// @param requestedPriority The new, higher priority to set.
    /// @return \c true, if the current priority was lower or equal to the given one, \c false
    /// otherwise.
    ALIB_DLL
    [[nodiscard]]
    bool Define( Priority requestedPriority= Priority::Standard );

    /// Returns the declaration struct of this variable, in case it was constructed or declared
    /// with an overloaded constructor or method #Declare, which accepts such struct.
    /// Otherwise, a nullptr is returned.
    ///
    /// \note If a variable is doubly declared by two or more code entities (which is the usual
    ///       case), a first declaration given is stored and returned from now on.
    ///       If a second, different declaration struct is passed for the same variable, an
    ///       assertion is raised in debug compilation. Hence, once a declaration is given,
    ///       it is always available for inspection.
    ///
    /// @return \c nullptr, if no declaration struct was given. Otherwise, the singleton declaration
    ///         object provided.
    const  Declaration* GetDeclaration()             const  { return Cursor::Value().declaration; }

    /// Returns a \c reference to the configuration this variable resides in.
    /// @return The associated configuration instance.
    Configuration&      GetConfiguration()                const   { return  Tree<Configuration>(); }

    /// The priority of a variable indicates "who" or "what" defined the variable and provided
    /// the current value. It may be defaulted values, values coming from external configuration
    /// files, from command line arguments, environment variables or from common code entities
    /// ('hard-coded' values). Besides the elements in enumeration \alib{variables;Priority},
    /// also any value in between may be given. Doing so, allows fine-tuning which entity exacly
    /// is allowed to write values. For example, if a variable's priority is set in code
    /// to \alib{variables;Priority;Environment}, then environment variables and CLI arguments may
    /// still overwrite a value. If set to \alib{variables;Priority;Environment + 1}, then
    /// environment variables would not overwrite, but still CLI arguments could.
    ///
    /// A priority of \alib{variables;Priority;Protected} disallows any code entity to make
    /// further changes, but those that again use this priority.
    ///
    /// @return The priority setting for this variable, given with the last successful call to
    ///         #Define.
    Priority              GetPriority()                  const { return  Cursor::Value().priority; }

    /// This is a shortcut to <c>Priority() != Priority::NONE</c> and thus, this method
    /// checks whether any data is defined.<br>
    /// Variables which are declared using a \alib{variables;Declaration} that
    /// contains a default value, will have this default value parsed and contain a data set of
    /// priority \alib{variables;Priority;DefaultValues}.
    ///
    /// @return \c true if data of any priority is available in this variable, \c false if not.
    inline
    bool                    IsDefined()                                                        const
    {
        ALIB_ASSERT_ERROR( IsDeclared(), "VARIABLES", "Variable not declared: ", this )
        return  Cursor::Value().priority != Priority::NONE;
    }

    /// This is the negation of method #IsDefined.
    ///
    /// @return \c false if data of any priority is available in this variable, \c true if not.
    inline
    bool                    IsNotDefined()                            const { return !IsDefined(); }

    /// This is a shortcut to <c>Priority() > Priority::Standard</c> and thus, this method
    /// checks whether an explicit definition with a priority higher than \e Standard was performed,
    /// which - by contract - is done only when external data sources are read, for example
    /// configuration files, environment variables, or command line parameters.
    ///
    /// @return \c true if data contained has a higher priority than \alib{variables;Priority;Standard},
    ///         \c false if not.
    inline
    bool                    IsDefinedExternally()  const {return Priority() > Priority::Standard;}



    /// Imports a variable value from a serialized string representation. Internally, this is
    /// performed by calling \alib{variables;VMeta::imPort}  on the virtual meta object
    /// associated with this variable's type.
    /// @param src      The source string.
    /// @param priority The priority of the import. If the variable is already defined in a
    ///                 higher priority, nothing is done.
    /// @param escaper  The \b StringEscaper to use for importing an escaped string. Defaults to
    ///                 \c nullptr which indicates that a programmatically defined C++ string is to
    ///                 be parsed. Usually, if a \alib{variables;ConfigurationPlugin} invokes this method
    ///                 it would pass its own suitable implementation here.
    ALIB_DLL
    void Import( const String&        src,   Priority   priority,
                 const StringEscaper* escaper= nullptr);


    /// Exports the contents of this variable to the given buffer.
    /// This is done by calling \alib{variables;VMeta::exPort} on the virtual meta object
    /// associated with this variable's type.
    ///
    /// @param dest    The destination string. Must not be reset prior writing, but appended.
    /// @param escaper The \b StringEscaper to use for exporting a C++ string to a (probably)
    ///               escaped string format. Defaults to \c nullptr which indicates that no
    ///               conversion (aka 'escaping') has to be performed. Usually, if
    ///               a \alib{variables;ConfigurationPlugin} invokes this method to write-back the
    ///               contents of a variable, it would pass its own suitable implementation here.
    /// @return \p{dest} to allow concatenated operations.
    AString&    Export(AString& dest, const StringEscaper* escaper= nullptr)                    const
    {
        StringEscaper nonEscaper;
        if( !escaper )
            escaper= &nonEscaper;
        getMeta()->exPort( Cursor::Value().data, GetConfiguration(), *escaper, dest );
        return dest;
    }

    /// Returns a reference to the variable's value. To receive a value the variable has to be
    /// defined, which is either done, by
    /// - A provision of a default value with declaration,
    /// - A configuration plug-in like \alib{variables;CLIVariablesPlugin} or
    ///   \alib{variables;EnvironmentVariablesPlugin},
    /// - Some bootstrap or other code entities like reading external configuration data from
    ///   3rd party sources,
    /// - or finally "hard coded" with method #Define.
    ///
    /// If this is not done, in debug-compilations an \ref alib_mod_assert "error is raised".
    /// In release-builds, it is defined (!) behavior: A default constructed value of \p{T} is
    /// returned.
    /// Hence, the whole concept of defining a variable and methods #IsDefined, etc. is
    /// rather a contract than a necessity. This is explained in section
    /// \ref alib_variables_definition of the Programmer's Manual of this \alibcamp.
    ///
    /// Template parameter \p{T} has to be specified
    /// in alignment with the declaration of the variable. It is the users responsibility to
    /// specify the right type, as internally a <c>reinterpret_cast</c> has to be performed.
    /// In other words, providing a wrong type is undefined behavior.
    ///
    /// In debug-compilations, an \ref alib_mod_assert "error is raised" if the type does not
    /// match the declaration.
    ///
    /// @tparam T The type to receive.
    /// @return A reference to the value of the variable.
    template<typename T>        T& Get()
    {
        ALIB_ASSERT_ERROR( !Cursor::IsRoot(), "VARIABLES",
                  "Requesting value from undeclared variable \"{}\"", this )
        ALIB_ASSERT_ERROR( IsDefined(), "VARIABLES",
                  "Requesting value from undefined variable \"{}\".\n"
                  "Use 'var.Define();' to mitigate this error.", this )
        ALIB_ASSERT_ERROR( getMeta()->dbgTypeID() == typeid(T), "VARIABLES",
                  "Wrong variable type requested for variable: \"{}\".\n"
                  "  Stored   : {}\n"
                  "  Requested: {}\n",
                  this, &getMeta()->dbgTypeID(), &typeid(T)   )

        return Cursor::Value().data->As<T>();
    }

    /// <c>const</c>-version of overloaded method.
    ///
    /// @tparam T The type to receive.
    /// @return A reference to the value of the variable.
    template<typename T>  const T& Get()                                                    const
    {
        ALIB_ASSERT_ERROR( Cursor::Value().data != nullptr, "VARIABLES",
                  "Requested value from unset variable: ", *this )
        ALIB_ASSERT_ERROR( IsDefined(), "VARIABLES",
                  "Requesting value from undefined variable. Use 'var.Define();' to mitigate\n"
                  "this error. Variable: ", *this )
        ALIB_ASSERT_ERROR( getMeta()->dbgTypeID() == typeid(T), "VARIABLES",
                  "Wrong variable type requested for variable: \"{}\".\n"
                  "  Stored   : {}\n"
                  "  Requested: {}\n",
                  *this, getMeta()->dbgTypeID(), typeid(T)               )

        return Cursor::Value().data->As<T>();
    }

    operator bool            ()                   { return Get<Bool     >(); }                     ///< @return Calls and returns \ref Get "Get\<Bool\>()".
    operator integer         ()                   { return Get<integer  >(); }                     ///< @return Calls and returns \ref Get "Get\<integer\>()".
    operator float           ()                   { return float(Get<double>()); }                 ///< @return Calls and returns \ref Get "Get\<double\>()" cast to \c float.
    operator double          ()                   { return Get<double   >(); }                     ///< @return Calls and returns \ref Get "Get\<double\>()".
    operator const String&   ()                   { return Get<AStringPA>(); }                     ///< @return Calls and returns \ref Get "Get\<AStringPA\>()".
    operator const Substring ()                   { return Get<AStringPA>(); }                     ///< @return Calls and returns \ref Get "Get\<AStringPA\>()".

                  bool         GetBool()          { return Get<Bool     >(); }                     ///< @return Calls and returns \ref Get "Get\<Bool\>()".
                  integer      GetInt()           { return Get<integer  >(); }                     ///< @return Calls and returns \ref Get "Get\<integer\>()".
                  float        GetFloat()         { return float(Get<double>()); }                 ///< @return Calls and returns \ref Get "Get\<double\>()" cast to \c float.
                  double       GetDouble()        { return Get<double   >(); }                     ///< @return Calls and returns \ref Get "Get\<double\>()".
                  Box&         GetBox()           { return Get<Box      >(); }                     ///< @return Calls and returns \ref Get "Get\<AStringPA\>()".
                  AStringPA&   GetString()        { return Get<AStringPA>(); }                     ///< @return Calls and returns \ref Get "Get\<AStringPA\>()".
               StringVectorPA& GetStrings()       { return Get<StringVectorPA>(); }                ///< @return Calls and returns \ref Get "Get\<StringVectorPA\>()".
                  String&      GetString(int idx) { return Get<StringVectorPA>().at(size_t(idx)); }///< @param idx The index of the requested string. @return Calls and returns \ref Get "Get\<StringVectorPA\>().Lines.at(idx)".
                  int          Size()             { return int(Get<StringVectorPA>().size()); }    ///< @return Calls and returns \ref Get "Get\<StringVectorPA\>().Lines.size()".

    bool          operator= (bool          val) { return Get<Bool      >()= val; }                 ///< Calls \ref Get "Get\<Bool\>()" and assigns \p{val}.                    @param val The value to assign. @return The \p{val} given.
    integer       operator= (integer       val) { return Get<integer   >()= val; }                 ///< Calls \ref Get "Get\<integer\>()" and assigns \p{val}.                 @param val The value to assign. @return The \p{val} given.
    float         operator= (float         val) { Get<double>()= double(val); return val; }        ///< Calls \ref Get "Get\<double\>()" and assigns \p{val}.        @param val The value to assign. @return The \p{val} given.
    double        operator= (double        val) { return Get<double    >()= val; }                 ///< Calls \ref Get "Get\<double\>()" and assigns \p{val}.                  @param val The value to assign. @return The \p{val} given.
    const String& operator= (const String& val) { return Get<AStringPA>().Reset(val); }            ///< Calls \ref Get "Get\<AStringPA\>()" and resets the string to \p{val}.  @param val The value to assign. @return The \p{val} given.



    /// Convenience method. If #IsDefined returns \c true, the value of this variable is returned.
    /// Otherwise the variable gets defined with the given \p{priority} and \p{value} is imported.
    /// @param value     The value to import in case the variable is not defined.
    /// @param priority  The priority to set on import in case the variable is not defined.
    ///                  Defaults to \alib{variables;Priority;DefaultValues}.
    /// @return \c false if data of any priority is available in this variable, \c true if not.
    inline
    String&   GetOrSetDefault( const String& value, Priority priority= Priority::DefaultValues )
    {
        if(IsDefined())
            return Get<AStringPA>();

        (void) Define(priority);
        return (Get<AStringPA>().Reset(value));
    }

    /// Convenience method. If #IsDefined returns \c true, the value of this variable is returned.
    /// Otherwise the variable gets defined with the given \p{priority} and \p{value} is imported.
    /// @param value     The value to import in case the variable is not defined.
    /// @param priority  The priority to set on import in case the variable is not defined.
    ///                  Defaults to \alib{variables;Priority;DefaultValues}.
    /// @return \c false if data of any priority is available in this variable, \c true if not.
    inline
    bool    GetOrSetDefault( bool value, Priority priority= Priority::DefaultValues )
    {
        if(IsDefined())
            return Get<Bool>();

        (void) Define(priority);
        return (Get<Bool>()= value);
    }

    /// Convenience method. If #IsDefined returns \c true, the value of this variable is returned.
    /// Otherwise the variable gets defined with the given \p{priority} and \p{value} is imported.
    /// @param value     The value to import in case the variable is not defined.
    /// @param priority  The priority to set on import in case the variable is not defined.
    ///                  Defaults to \alib{variables;Priority;DefaultValues}.
    /// @return \c false if data of any priority is available in this variable, \c true if not.
    inline
    bool    GetOrSetDefault( integer value, Priority priority= Priority::DefaultValues )
    {
        if(IsDefined())
            return GetInt();

        (void) Define(priority);
        return (Get<integer  >()= value);
    }


}; // class Variable

// =================================================================================================
// Definition of inline Configuration methods that deal with class Variable
// =================================================================================================
#if !DOXYGEN
template<typename TEnum>
requires ( EnumRecords<TEnum>::template AreOfType<Declaration>() )
void Configuration::PreloadVariables()
{
    for( auto recordIt=  EnumRecords<TEnum>::begin()
         ;    recordIt!= EnumRecords<TEnum>::end  ()
         ; ++ recordIt                                 )
    {
        const Declaration* decl= Declaration::Get( recordIt.Enum() );
        if(decl->EnumElementName.IndexOf('%') < 0)
            Variable(*this, decl);
    }
}
#endif

/// Utility type which implements \alib{monomem;TSharedMonoVal} with class
/// \alib{variables;Configuration}.
/// The result of combining both is an automatic pointer to a \b %Configuration that is
/// "self-contained" in the first buffer of a \alib{MonoAllocator} together with the allocator
/// itself.
/// All configuration variables are deleted and all associated memory is freed when the last copy
/// of the pointer goes out of scope.
///
/// Along with the \b Configuration, this shared object includes a \alib{threads;SharedLock}.
/// See chapter \ref alib_contmono_smv_locking of the Programmer's Manual of module \alib_monomem
/// for further information on how to protect the contents of this type against
/// thread-racing-conditions.
///
/// @tparam TLock The lock type passed to the template parameter of parent type
///               \alib{monomem;TSharedMonoVal} with the same name.<br>
///               With the inclusion of module \alib_threads in the \alibbuild, the type-alias
///               #alib::SharedConfiguration chooses type \alib{threads;SharedLock}.<br>
///               Otherwise, in case \alib is compiled without threading support, the alias chooses
///               <c>void</c>.<br>
///               If it is assured that no racing-conditions occur with shared instances in
///               multithreaded software, the using code may pass <c>void</c> here as well.
template<typename TLock>
class TSharedConfiguration : public  monomem::TSharedMonoVal<Configuration, HeapAllocator, TLock>
{
    /// Exposed shortcut to the base type.
    using Base= monomem::TSharedMonoVal<Configuration, HeapAllocator, TLock>;

  public:
    /// Constructs an empty instance, hence a cleared automatic pointer.
    TSharedConfiguration()                                                                = default;

    /// Constructs an empty instance from \c std::nullptr.
    /// This constructor is necessary to allow assignment of \c nullptr to values of this type,
    /// which clears the automatic pointer.
    TSharedConfiguration(std::nullptr_t)                                                 noexcept {}

    /// Constructor.
    /// Calls the constructor of parent \b TSharedMonoVal and then invokes
    /// \alib{monomem;TSharedMonoVal::ConstructT} passing the mono allocator that the
    /// parent creates this instance in.<br>
    /// Furthermore calls DbgCriticalSections to enable assertions to locked usage.
    /// @param initialBufferSizeInKB The initial size of memory buffers.
    ///                              Passed to the allocator given with parent class
    ///                              \alib{monomem;TSharedMonoVal}.
    /// @param bufferGrowthInPercent Optional growth factor in percent, applied to the buffer size
    ///                              with each next buffer allocation.
    ///                              Should be set to \c 200, to double the size with each
    ///                              allocation.
    ///                              Defaults to \c 200.
    ///                              Passed to the allocator given with parent class
    ///                              \alib{monomem;TSharedMonoVal}.
    /// @param createDefaults        Determines if default plug-ins are to be created.
    ///                              Defaults to \c Yes.
    TSharedConfiguration( size_t              initialBufferSizeInKB,
                         unsigned int         bufferGrowthInPercent = 200,
                         lang::CreateDefaults createDefaults= lang::CreateDefaults::Yes  )
    : Base(initialBufferSizeInKB, bufferGrowthInPercent)
    {
        Base::ConstructT( Base::GetAllocator(), createDefaults );
        DbgCriticalSections(lang::Switch::On);
        ALIB_DBG(Base::GetAllocator().DbgName= "Configuration";)

    }

    /// Defaulted copy-constructor.
    TSharedConfiguration( const TSharedConfiguration& ) = default;

    /// Defaulted copy-assignment operator.
    /// @return A reference to <c>this</c>.
    TSharedConfiguration& operator=(const TSharedConfiguration&)                          = default;


    /// Destructor.
    /// Calls #DbgCriticalSections to stop checking the integrated \p{TLock}.
    ~TSharedConfiguration()                              { DbgCriticalSections(lang::Switch::Off); }

    #if DOXYGEN
    /// Enables or disables critical section checks between the contained \p{T} and the likewise
    /// contained \p{TLock}.<br>
    /// In case \p{TLock} equals <c>void</c> or if symbol \ref ALIB_DEBUG_CRITICAL_SECTIONS is not
    /// set, this method is empty (and its use is optimized out).
    /// @param onOff The switch.
    void DbgCriticalSections(lang::Switch onOff);
    #else
        template<typename TRequires= typename Base::LockType>
        requires (!std::same_as<TRequires, void>)
        void DbgCriticalSections(lang::Switch onOff)
        {
            #if ALIB_DEBUG_CRITICAL_SECTIONS
            if ( !Base::IsNulled() )
            {
                if( onOff == lang::Switch::On )    Base::Self().NodeTable().dcs.DCSLock= &Base::GetLock();
                else                               Base::Self().NodeTable().dcs.DCSLock= nullptr;
            }
            #else
                (void) onOff;
            #endif
        }

        template<typename TRequires= typename Base::LockType>
        requires std::same_as<TRequires, void>
        void DbgCriticalSections(lang::Switch)                                                    {}
    #endif

    /// Clears all data, plugins, type-registrations, etc., from this configuration.
    /// Then this object is reinitialized to its constructor defaults.<br>
    /// The \b MonoAllocator of the parent class is likewise reset.
    ///
    /// All shared instances remain valid (while, of course, their content is likewise reset).
    /// @param createDefaults Determines if default plug-ins are to be created.
    ///                       Defaults to \c Yes.
    void            Reset(lang::CreateDefaults createDefaults= lang::CreateDefaults::Yes)
    {
        DbgCriticalSections(lang::Switch::Off);
        Base::Reset( Base::GetAllocator(), createDefaults );
        DbgCriticalSections(lang::Switch::On);
    }



}; //class Configuration


} // namespace alib[::config]


using Configuration        = variables::Configuration;                     ///< Type alias in namespace \b alib.
#if !ALIB_SINGLE_THREADED || DOXYGEN
using SharedConfiguration  = variables::TSharedConfiguration<SharedLock>;   ///< Type alias in namespace \b alib.
#else
using SharedConfiguration  = variables::TSharedConfiguration<void>;
#endif
using ConfigurationListener= variables::ConfigurationListener;             ///< Type alias in namespace \b alib.
using Variable             = variables::Variable;                          ///< Type alias in namespace \b alib.

}  // namespace [alib]



// #################################################################################################
// struct AppendableTraits<Variable>
// #################################################################################################

// Faking all template specializations of namespace strings for doxygen into namespace
// strings::APPENDABLES to keep the documentation of namespace string clean!
namespace alib::strings {
#if DOXYGEN
namespace APPENDABLES {
#endif

/// Specialization of functor \alib{strings;AppendableTraits} for type <c>Variable</c>.
template<> struct       AppendableTraits<variables::Variable, nchar, lang::HeapAllocator>
{
    /// Writes the variable name represented by the given configuration cursor to the given AString.
    /// @param target   The \b NAString that \b Append was invoked on.
    /// @param variable The variable (or section).
    ALIB_DLL void operator()( TAString<nchar, lang::HeapAllocator>& target, const variables::Variable& variable );
};

/// Specialization of functor \alib{strings;AppendableTraits} for type <c>Variable</c>.
template<> struct       AppendableTraits<variables::Variable, wchar, lang::HeapAllocator>
{
    /// Writes the variable name represented by the given configuration cursor to the given AString.
    /// @param target   The \b WAString that \b Append was invoked on.
    /// @param variable The variable (or section).
    ALIB_DLL void operator()( TAString<wchar, lang::HeapAllocator>& target, const variables::Variable& variable );
};

#if DOXYGEN
} // namespace alib::strings[APPENDABLES]
#endif
} // namespace [alib::strings]

ALIB_BOXING_VTABLE_DECLARE(   alib::variables::Configuration::ConstCursor, vt_config_constcursor )
ALIB_BOXING_VTABLE_DECLARE(   alib::variables::Variable     , vt_config_variable      )



