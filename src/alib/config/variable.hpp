/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_config of the \aliblong.
 *
 * \emoji :copyright: 2013-2019 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_CONFIG_VARIABLE
#define HPP_ALIB_CONFIG_VARIABLE 1

#if !defined (HPP_ALIB_CONFIG_PRIORITIES)
    #include "alib/config/priorities.hpp"
#endif

#if !defined (HPP_ALIB_STRINGS_STRING)
    #include "alib/strings/string.hpp"
#endif

#if !defined (HPP_ALIB_C)
#   include "alib/config/variabledecl.hpp"
#endif

#if !defined (HPP_ALIB_MONOMEM_SELF_CONTAINED)
#   include "alib/monomem/selfcontained.hpp"
#endif
#if !defined (HPP_ALIB_MONOMEM_STDCONTAINERMA)
#   include "alib/monomem/stdcontainerma.hpp"
#endif

#if !defined (_GLIBCXX_VECTOR) && !defined(_VECTOR_)
#   include <vector>
#endif

namespace aworx { namespace lib { namespace config {

// forward declarations
class  Configuration;

namespace detail {

/**
 * Fields of class \alib{config,Variable}, which is monotonically allocated and derived from
 * \alib{monomem,SelfContained} and as such needs this special struct for field definition.
 */
struct VariableFields
{
    /** The \b %Configuration that was recently used to request or store the value. */
    Configuration*                  Config;

    /** The configuration variable category. */
    String                          Category;

    /** The configuration variable name. */
    String                          Name;

    /**
     * The delimiter used for parsing and storing values by simple textual plug-ins which
     * use the default version of \alib{config,XTernalizer} for in- and externalizing variables.
     */
    character                       Delim;

    /**
     * Hints for formatting textual configuration files. (Used by class
     * \alib{config,IniFile} and potentially by custom plug-ins.
     */
    FormatHints                     FmtHints;

    /**
     * If set, attributes written in multi-lines are vertically aligned by this character or
     * string. Use cases are "=", ":" or "->".<br> Used by \alib{config,IniFile} and
     * potentially by custom plug-ins.
     */
    String                          FormatAttrAlignment                               = nullptr;

    /** The configuration variable comments with placeholders replaced. */
    String                          Comments                                          = nullptr;

    /** The configuration variable category and name concatenated with an underscore
     *  character \c '_'. This is useful e.g. for naming variables in log messages. */
    String                          Fullname;

    /**
     * The default value provided as an externalized string.
     *
     * The only occasion that this value is used is with method
     * \alib{config,Configuration::Load}.
     * If no plug-in has this variable defined and this field is not \e nulled, then the value
     * is written into plug-in of priority \alib{config,Priorities::DefaultValues},
     * respectively - if this default plug-in was replaced by the user - into a plug-in found at
     * or below this priority.
     *
     * In this case, the value is parsed using method
     * \alib{config,XTernalizer::LoadFromString} of field
     * \alib{config,ConfigurationPlugin::StringConverter} of the plug-in writing the value.
     *
     * \note
     *   The field is ignored when using the plug-in interface
     *   \alib{config,ConfigurationPlugin::Load} directly.
     *   To store this value 'manually' directly to a plug-in, invoke
     *   \alib{config,ConfigurationPlugin::Store} with providing value explicitly.
     */
    String                          DefaultValue                                      = nullptr;

    /**
     * A value related to the priority of a configuration plug-in.
     * The following values apply:
     * - \alib{config,Priorities::NONE} after creation or declaration (reuse).
     * - The priority of the plug-in that loaded the value (after calling
     *   \alib{config,Configuration::Load}).
     * - The priority of the plug-in that stored the value (after calling
     *   \alib{config,Configuration::Store}).
     * - \c 0, if the last load or store operation failed.
     * In addition prior to storing a variable, the value might be manually set. See
     * documentation of \alib{config,Configuration::Store}) for details.
     */
    Priorities                      Priority;

    /** The values. */
    std::vector<String, StdContMA<String>>  values;

    /**
     * Constructor.
     * @param fields Pointer to the fields pointer of the self-contained object.
     *               \note This is a pointer to the pointer, as at the point of invocation,
     *                     the self-contained fields are not fully initialized, yet. This
     *                     object itself is missing. However the allocator within the struct
     *                     is initialized already If the pointer was given directly, the compiler
     *                     would complain.
     */
    VariableFields( lib::monomem::SelfContained<VariableFields>::Fields** fields)
    : values( (*fields)->allocator )
    {}

}; // struct VariableFields

} // namespace detail

/** ************************************************************************************************
 * This class is used to load and store external configuration data with objects of class
 * \alib{config,Configuration} and its plug-ins \alib{config,ConfigurationPlugin}.
 *
 * \note
 *   For general information about external configuration variables, see namespace documentation
 *   \ref aworx::lib::config "aworx::lib::config".
 *
 * <b>Construction/Redeclaration:</b><br>
 * While constructors accepting attributes of a variable exist, it is recommended to
 * declare all external configuration variables in a central place, using statically or dynamically
 * allocated objects of type \alib{config,VariableDecl} and pass such record to the constructor of
 * a variable.
 *
 * The class is \alib{monomem,SelfContained,self-contained} in a \alib{monomem,MonoAllocator}
 * and supports is designed to be 'reused'. This is implementation is chosen to avoid repeated
 * allocation/de-allocation of dynamic memory when a larger amount of variables are read or written.
 * After invoking one of the overloaded methods #Declare, which share the same signatures as
 * the overloaded constructors, a variable is freshly initialized and its monotonic allocator is reseet.
 *  *
 * <b>Values:</b><br>
 * A variable can contain zero, one or several values. If zero, then the variable was either not
 * loaded, yet or the load operation failed (no configuration plug-in defined the variable).
 * Method #Size reports the currently available values and methods #GetString(int), #GetInteger(int)
 * and #GetFloat(int) return a value. Internally all values are stored as strings. If
 * field #Config is set, its field \alib{config,Configuration::NumberFormat} is used for floating
 * point conversion.
 *
 * When storing a variable that contains more than one value, field #Delim has to be set.
 * Conversely, when reading a variable that contains multiple values, the delimiter has to be set
 * prior to the load operation.
 * \note
 *   This is not true when loading/storing a variable directly in a plug-in of type
 *   \alib{config,InMemoryPlugin} or might also not be true with custom configuration plug-in types
 *   which for example might store the values in a database.<br>
 *   However, with the default plug-ins \alib{config,CLIArgs}, \alib{config,Environment} and
 *   \alib{config,IniFile}, the delimiter is needed!
 *   Therefore, it is best practice to always define a proper delimiter if a variable contains
 *   multiple values.
 *
 * To set the contents of various fields, methods whose name is prefixed with <b>Replace</b> are
 * given. Each of these methods allocates new storage in the internal monotonic allocator, which is
 * freed only with method #Reset, which is internally called with each overloaded #Declare method.
 * The using software has to ensure that #Reset is invoked on a regular basis, to not generate
 * a "memory leak" by increasing the allocated space in the monotonic allocator.
 * In common use-cases of the class, this is not an issue.
 *
 * <b>Loading and Storing:</b><br>
 * There are two ways of loading and storing a variable:
 * - Using the interface of class \alib{config,Configuration} which allows to load and store
 *   variables from different sources (plug-ins) in a prioritized way.
 * - Using the interface of class \alib{config,ConfigurationPlugin} which may be used if the
 *   decision about the source or drain of a load/store operation is explicitly made by a code
 *   unit - instead of by the configuration.
 *
 * Storing empty variables (method #Size returns \c 0) deletes a variable from the those
 * configuration plug-ins that are write enabled.
 **************************************************************************************************/
class Variable : protected monomem::SelfContained<detail::VariableFields>
{
    // #############################################################################################
    // Constructors and Declaration
    // #############################################################################################
    public:
        /** ****************************************************************************************
         * Move constructor moves the entries and deletes them in source object.
         * @param src The object to move.
         ******************************************************************************************/
        Variable(Variable&& src) noexcept
        : SelfContained( std::move(src) )
        {}

        /** ****************************************************************************************
         * Constructs an undefined Variable. Prior to using this, #Declare has to be invoked.
         ******************************************************************************************/
        Variable()
        : SelfContained( 4 * 1024, &fields )
        {
            #if ALIB_DEBUG_MONOMEM
                Allocator().LogDomain= A_CHAR("MA/CFG/VAR");
            #endif

            Reset();
        }

        /** ****************************************************************************************
         * Constructs a variable from a declaration.
         * Strings named \c "%1", \c "%2" ... \c "%N" found in the fields #Category, #Name,
         * #Comments and #DefaultValue are replaced with given replacement values found
         * in boxed argument (list) \p{replacements}.
         *
         * @param declaration    The declaration data of the variable.
         * @param replacements   List of replacement values. Must be of boxed types that are
         *                       \alib{strings,T_Append,appendable} to class \b AString.
         *                       To provide more than one object, pass an object of class
         *                       \alib{boxing,Boxes} or a boxed array.
         ******************************************************************************************/
        Variable( const VariableDecl& declaration,  const Box& replacements )
        : SelfContained( 2048, &fields )
        {
            #if ALIB_DEBUG_MONOMEM
                Allocator().LogDomain= A_CHAR("MA/CFG/VAR");
            #endif
            Declare( declaration, replacements );
        }

        /** ****************************************************************************************
         * Constructs a variable using the declaration of another variable. The values are not
         * copied.
         *
         * @param variable  A variable to copy the declaration (which is comprised with fields
         *                  #Category, #Name, #Fullname, #Delim, #Comments and #DefaultValue) from.
         ******************************************************************************************/
        Variable( const Variable& variable )
        : SelfContained( 2048, &fields )
        {
            #if ALIB_DEBUG_MONOMEM
                Allocator().LogDomain= A_CHAR("MA/CFG/VAR");
            #endif
            Declare( variable );
        }

        /** ****************************************************************************************
         * Constructs a variable from the given values
         *
         * @param category  The category of the variable.
         * @param name      The name of the variable
         * @param delim     A proposal for a delimiter that might be used by some plug-ins
         *                  to delimit different values from each other (e.g. INI files).
         * @param comments  Comment lines that might be added in the configuration storage
         *                  (plug-in implementation dependent).
         ******************************************************************************************/
        Variable( const String& category,  const String& name,  character delim= '\0',
                  const String& comments  = nullptr      )
        : SelfContained( 2048, &fields )
        {
            #if ALIB_DEBUG_MONOMEM
                Allocator().LogDomain= A_CHAR("MA/CFG/VAR");
            #endif
            Declare( category, name, delim, comments );
        }

        #if defined(ALIB_DOX)
        /** ****************************************************************************************
         * Constructs a variable from an enum element equipped with
         * \ref alib_enums_records "ALib Enum Records" of type \alib{config,VariableDecl}.
         *
         * @tparam TEnum         The type of parameter \p{declaration}
         * @tparam TEnableIf     Not to be specified. Used by the compiler to select this
         *                       constructor only for associated custom C++ enum types.
         * @param declaration    Element of an enum class that is representing configuration
         *                       variables.
         ******************************************************************************************/
        template<typename TEnum, typename TEnableIf=void>
        inline
        Variable( TEnum declaration );
        #else
        template<typename TEnum, typename TEnableIf=
        ATMP_VOID_IF( EnumRecords<TEnum>::template AreOfType<VariableDecl>() ) >
        Variable( TEnum declaration )
        : SelfContained( 2048, &fields )
        {
            #if ALIB_DEBUG_MONOMEM
                Allocator().LogDomain= A_CHAR("MA/CFG/VAR");
            #endif
            Declare( declaration );
        }
        #endif

#if defined(ALIB_DOX)
        /** ****************************************************************************************
         * Constructs a variable from an enum element equipped with
         * \ref alib_enums_records "ALib Enum Records" of type \alib{config,VariableDecl}.
         *
         * Strings named \c "%1", \c "%2" ... \c "%N" found in the fields #Category, #Name,
         * #Comments and #DefaultValue are replaced with given replacement values found
         * in boxed argument (list) \p{replacements}.
         *
         * @tparam TEnum         The type of parameter \p{declaration}
         * @tparam TEnableIf     Not to be specified. Used by the compiler to select this
         *                       constructor only for associated custom C++ enum types.
         * @param declaration    Element of an enum class that is representing configuration
         *                       variables.
         * @param replacements   List of replacement values. Must be of boxed types that are
         *                       \alib{strings,T_Append,appendable} to class \b AString.
         *                       To provide more than one object, pass an object of class
         *                       \alib{boxing,Boxes} or a boxed array of boxes.
         ******************************************************************************************/
        template<typename TEnum, typename TEnableIf= void>
        Variable( TEnum declaration,  const Box& replacements );
#else
        template<typename TEnum, typename TEnableIf=
        ATMP_VOID_IF( EnumRecords<TEnum>::template AreOfType<VariableDecl>() ) >
        Variable( TEnum declaration,  const Box& replacements )
        : SelfContained( 2048, &fields )
        {
            #if ALIB_DEBUG_MONOMEM
                Allocator().LogDomain= A_CHAR("MA/CFG/VAR");
            #endif
            Declare( declaration, replacements );
        }
#endif

        /** ****************************************************************************************
         * Re-initializes this variable from a given declaration.
         * Placeholders \c "%1", \c "%2" ... \c "%N" found in fields #Category, #Name, #Comments
         * and #DefaultValue are replaced with the replacement values given with boxed argument
         * (list) \p{replacements}.
         *
         * @param declaration    The declaration data of the variable.
         * @param replacements   List of replacement values. Must be of boxed types that are
         *                       \alib{strings,T_Append,appendable} to class \b AString.
         *                       To provide more than one object, pass an object of class
         *                       \alib{boxing,Boxes} or a boxed array.
         * @return \c *this to allow concatenated operations.
         ******************************************************************************************/
        ALIB_API
        Variable&   Declare( const  VariableDecl& declaration,  const Box& replacements );


        #if defined(ALIB_DOX)
        /** ****************************************************************************************
         * Re-initializes this variable and resets its declaration.
         * Internally uses an instance of class \alib{config,VariableDecl} constructed with
         * enum element \p{declaration}.
         *
         * @param declaration    Element of an enum class that is representing configuration
         *                       variables.
         * @tparam TEnum         The type of parameter \p{declaration}
         * @return \c *this to allow concatenated operations.
         ******************************************************************************************/
        template<typename TEnum>
        Variable&   Declare( TEnum declaration );
        #else
        template<typename TEnum>
        ATMP_T_IF( Variable&, EnumRecords<TEnum>::template AreOfType<VariableDecl>() )
        Declare( TEnum declaration )
        {
            VariableDecl decl( declaration );
            return Declare( decl, nullptr );
        }
        #endif


        /** ****************************************************************************************
         * Declares a variable using the declaration of another variable.
         * The values are not copied.
         *
         * @param variable  A variable to copy the declaration (which is comprised with fields
         *                  #Category, #Name, #Fullname, #Delim, #Comments and #DefaultValue) from.
         * @return \c *this to allow concatenated operations.
         ******************************************************************************************/
        ALIB_API
        Variable& Declare ( const Variable& variable );

        /** ****************************************************************************************
         * Re-initializes the variable using the specific values given.
         *
         * @param category  The category of the variable.
         * @param name      The name of the variable
         * @param delim     A proposal for a delimiter that might be used by some plug-ins
         *                  to delimit different values from each other (e.g. INI files).
         *                  Defaults to \c '\0'.
         * @param comments  Comment lines that might be added in the configuration storage
         *                  (plug-in implementation dependent).
         *                  Defaults to \c nullptr.
         *
         * @return \c *this to allow concatenated operations.
         ******************************************************************************************/
        ALIB_API
        Variable&   Declare( const String& category,
                             const String& name,
                             character     delim    = '\0',
                             const String& comments = nullptr                  );


        /** ****************************************************************************************
         * Re-initializes the variable using the specific values given.
         * This overload accepts complementary string character types for each argument.
         *
         * @param category  The category of the variable.
         * @param name      The name of the variable
         * @param delim     A proposal for a delimiter that might be used by some plug-ins
         *                  to delimit different values from each other (e.g. INI files).
         *                  Defaults to \c '\0'.
         * @param comments  Comment lines that might be added in the configuration storage
         *                  (plug-in implementation dependent).
         *                  Defaults to \c nullptr.
         *
         * @return \c *this to allow concatenated operations.
         ******************************************************************************************/
        ALIB_API
        Variable&   Declare( const ComplementString&  category,
                             const ComplementString&  name,
                             complementChar           delim    = '\0',
                             const ComplementString&  comments = nullptr                  );

        /** ****************************************************************************************
         * Clears all fields and resets internal monotonic allocator.
         *
         * @param nameAndCategory If <b>CurrentData::CurrentData::Keep</b> is given, the
         *                        name and category of the variable will be restored after
         *                        the reset.<br>
         *                        Defaults to <b>CurrentData::Clear</b>.
         *
         * @return \c *this to allow concatenated operations.
         ******************************************************************************************/
        ALIB_API
        Variable&   Reset( CurrentData nameAndCategory= CurrentData::Clear );


    // #############################################################################################
    // Interface
    // #############################################################################################
    public:
        /**
         * The \b %Configuration that was recently used to request or store the value.
         * @return The associated configuration object or \c nullptr if no configuration is
         *         assigned.
         */
        const Configuration*    Config()                                                       const
        {
            return  Self().Config;
        }

        /**
         * The \b %Configuration that was recently used to request or store the value.
         * @return The associated configuration object or \c nullptr if no configuration is
         *         assigned.
         */
        Configuration*          Config()
        {
            return  Self().Config;
        }

        /**
         * Sets the \b %Configuration to load or store the value.
         * @param config The configuration to use.
         */
        void                    SetConfig( Configuration* config )
        {
            Self().Config= config;
        }


        /**
         * Returns this configuration variable's category.
         * @return The category of this variable.
         */
        const String&           Category()                                                     const
        {
            return  Self().Category;
        }

        /**
         * Returns this configuration variable's name.
         * @return The name of this variable.
         */
        const String&           Name()                                                         const
        {
            return  Self().Name;
        }

        /**
         * The delimiter used for parsing and storing values by simple textual plug-ins which
         * use the default version of \alib{config,XTernalizer} for in- and externalizing variables.
         * @return The delimiter character of this variable.
         */
        character               Delim()                                                        const
        {
            return  Self().Delim;
        }

        /**
         * Sets the delimiter used for parsing and storing values.
         * \see
         *   Method #Delim for more informaiton.
         * @param delim The configuration to use.
         */
        void                    SetDelim( character delim )
        {
            Self().Delim= delim;
        }

        /**
         * Hints for formatting textual configuration files. (Used by class \alib{config,IniFile}
         * and potentially by custom plug-ins.
         * @return The format hints.
         */
        FormatHints             FmtHints()                                                     const
        {
            return  Self().FmtHints;
        }

        /**
         * Sets the hints for formatting textual configuration files.
         * (Used by class \alib{config,IniFile} and potentially by custom plug-ins.
         *
         * @param hints The hints to be used.
         */
        void                    SetFmtHints( FormatHints hints)
        {
            Self().FmtHints= hints;
        }

        /**
         * If set, attributes written in multi-lines are vertically aligned by this character or
         * string. Use cases are "=", ":" or "->".<br>
         * Used by \alib{config,IniFile} and potentially by custom plug-ins.
         * @return The alignment of attributes.
         */
        const String&           FormatAttrAlignment()                                          const
        {
            return  Self().FormatAttrAlignment;
        }

        /**
         * Returns this configuration variable's comments.
         * @return The comments of this variable.
         */
        const String&           Comments()                                                     const
        {
            return  Self().Comments;
        }

        /**
         * The configuration variable category and name concatenated with an underscore
         * character \c '_'. This is useful e.g. for naming variables in log messages.
         *
         * \note
         *   This method is not declared <c>const</c>, as the full name might be assembled
         *   with the invocation of this method.
         *
         * @return The name and category  of this variable separated by \c '_'.
         */
        ALIB_API const String&  Fullname();

        /**
         * The default value provided as an externalized string.
         *
         * The only occasion that this value is used is with method
         * \alib{config,Configuration::Load}.
         * If no plug-in has this variable defined and this field is not \e nulled, then the value
         * is written into plug-in of priority \alib{config,Priorities::DefaultValues},
         * respectively - if this default plug-in was replaced by the user - into a plug-in found at
         * or below this priority.
         *
         * In this case, the value is parsed using method
         * \alib{config,XTernalizer::LoadFromString} of field
         * \alib{config,ConfigurationPlugin::StringConverter} of the plug-in writing the value.
         *
         * \note
         *   The field is ignored when using the plug-in interface
         *   \alib{config,ConfigurationPlugin::Load} directly.
         *   To store this value 'manually' directly to a plug-in, invoke
         *   \alib{config,ConfigurationPlugin::Store} with providing value explicitly.
         *
         * @return The default value of this variagble.
         */
        const String&           DefaultValue()                                                 const
        {
            return  Self().DefaultValue;
        }

        /**
         * A value related to the priority of a configuration plug-in.
         * The following values apply:
         * - \alib{config,Priorities::NONE} after creation or declaration (reuse).
         * - The priority of the plug-in that loaded the value (after calling
         *   \alib{config,Configuration::Load}).
         * - The priority of the plug-in that stored the value (after calling
         *   \alib{config,Configuration::Store}).
         * - \c 0, if the last load or store operation failed.
         * In addition prior to storing a variable, the value might be manually set. See
         * documentation of
         * \alib{config,Configuration::Store}) for details.
         *
         * @return The priority setting for this variable.
         */
        Priorities              Priority()                                                     const
        {
            return  Self().Priority;
        }

        /**
         * Sets the priority.
         * \see
         *   Method #Priority for more information.
         * @param priority The priority to use.
         */
        void                    SetPriority( Priorities priority )
        {
            Self().Priority= priority;
        }


        /**
         * Sets the value received by #Category.
         * \note
         *   This method allocates a new string object in the internal \alib{monomem,MonoAllocator}
         *   which will only be released with resetting this variable instance.
         *
         * @param newValue The new category to set.
         */
        void                    ReplaceCategory( const String& newValue )
        {
            Self().Category           = Allocator().EmplaceString( newValue );
        }

        /**
         * Sets the value received by #Name.
         * \note
         *   This method allocates a new string object in the internal \alib{monomem,MonoAllocator}
         *   which will only be released with resetting this variable instance.
         *
         * @param newValue The new name to set.
         */
        void                    ReplaceName( const String& newValue )
        {
            Self().Name               = Allocator().EmplaceString( newValue );
        }

        /**
         * Sets the value received by #Comments.
         * \note
         *   This method allocates a new string object in the internal \alib{monomem,MonoAllocator}
         *   which will only be released with resetting this variable instance.
         *
         * @param newValue The new comments to set.
         */
        void                    ReplaceComments( const String& newValue )
        {
            Self().Comments           = Allocator().EmplaceString( newValue );
        }

        /**
         * Sets the value received by #DefaultValue.
         * \note
         *   This method allocates a new string object in the internal \alib{monomem,MonoAllocator}
         *   which will only be released with resetting this variable instance.
         *
         * @param newValue The new default value to set.
         */
        void                    ReplaceDefaultValue( const String& newValue )
        {
            Self().DefaultValue       = Allocator().EmplaceString( newValue );
        }

        /**
         * Sets the value received by #FormatAttrAlignment.
         * \note
         *   This method allocates a new string object in the internal \alib{monomem,MonoAllocator}
         *   which will only be released with resetting this variable instance.
         *
         * @param newValue The new alignment to set.
         */
        void                    ReplaceFormatAttrAlignment( const String& newValue )
        {
            Self().FormatAttrAlignment= Allocator().EmplaceString( newValue );
        }


    // #############################################################################################
    // Value access
    // #############################################################################################
        /** ****************************************************************************************
         * Returns the number of values set in this object.
         *
         * @return The number of values set.
         ******************************************************************************************/
        integer                 Size() const
        {
            return static_cast<integer>(Self().values.size());
        }

        /** ****************************************************************************************
         * Clears the value, respectively the list of values set.
         * After invoking this method, #Size will return \p{startIdx} and #GetString will
         * return \c nullptr for any index greater or equal to \p{startIdx}.
         *
         * @param  startIdx  The index of the first value to be cleared. Defaults to \c 0.
         * @return \c *this to allow concatenated operations.
         ******************************************************************************************/
        Variable&               ClearValues(int startIdx= 0)
        {
            auto& vector= Self().values;

            if( startIdx == 0 )
                vector.clear();
            else
                vector.erase( vector.begin() + startIdx, vector.end() );
            return *this;
        }

        /** ****************************************************************************************
         * Adds a value to the end of the list of values.
         * @param  value  The string value to add.
         ******************************************************************************************/
        ALIB_API void           Add(const String& value);

        /** ****************************************************************************************
         * Adds an integral value to the end of the list of values.
         * @param  value  The value to add.
         ******************************************************************************************/
        ALIB_API void           Add(int64_t value);

        /** ****************************************************************************************
         * Adds an integral value to the end of the list of values.
         * @param  value  The value to add.
         ******************************************************************************************/
          void                  Add(int32_t value) { Add( static_cast<int64_t>(value) );  }

        /** ****************************************************************************************
         * Adds a floating point value to the end of the list of values.
         * @param  value  The value to add.
         ******************************************************************************************/
        ALIB_API void           Add(double value);

        /** ****************************************************************************************
         * Replaces the value at \p{idx} with the given string.
         *
         * @param  idx          The index of the value to replace.
         * @param  replacement  The string value to set.
         ******************************************************************************************/
        ALIB_API void           ReplaceValue( int idx, const String& replacement );

        /** ****************************************************************************************
         * Replaces the value at \p{idx} with the values of the given other variable.
         *
         * @param  idx          The index of the value to replace.
         * @param  replVariable The variable providing the replacement values.
         ******************************************************************************************/
        ALIB_API void           ReplaceValue( int idx, Variable& replVariable );

        /** ****************************************************************************************
         * Returns the stored value with the given index.
         * Valid values for parameter \p{idx} are between \b 0 and #Size <b> - 1</b>.<br>
         * In debug-compilations an error is raised if the given \p{idx} is out of range.
         *
         * @param  idx  The index of the value to be retrieved.  Defaults to \c 0.
         * @return The value at \p{idx}.
         ******************************************************************************************/
        const String&           GetString( int idx= 0 )
        {
            ALIB_ASSERT_ERROR( idx >= 0  &&  idx < Size(),
                               "Index {} is out of range.", idx )
            return Self().values[static_cast<size_t>(idx)];
        }

        /** ****************************************************************************************
         * Returns the value at \p{idx} interpreted as an integer.
         * If the index is invalid, \c 0 is returned.
         * @param  idx  The index of the value to be retrieved.  Defaults to \c 0.
         * @return The value at \p{idx} interpreted as an integral value.
         ******************************************************************************************/
        ALIB_API integer        GetInteger(int idx= 0);

        /** ****************************************************************************************
         * Returns the value at \p{idx} interpreted as a double value.
         * If the index is invalid, \c 0.0 is returned.
         * Parsing is done using field \c NumberFormat of field #Config, respectively, if this is
         * not set, the static singleton \alib{strings,TNumberFormat::Global,NumberFormat::Global}.
         *
         * @param  idx  The index of the value to be retrieved.  Defaults to \c 0.
         * @return The value at \p{idx} interpreted as a double value.
         ******************************************************************************************/
        ALIB_API double         GetFloat(int idx= 0);

        /** ****************************************************************************************
         * Returns \c true if the first value represents a boolean 'true'.
         * Evaluation is done using field #Config, respectively if this is not set, the static
         * singleton \alib{strings,TNumberFormat::Global,NumberFormat::Global}.
         *
         * @param  idx  The index of the value to be retrieved.  Defaults to \c 0.
         * @return The value at \p{idx} interpreted as a boolean value.
         ******************************************************************************************/
        ALIB_API bool           IsTrue(int idx= 0);

        /** ****************************************************************************************
         * Searches in the values of this variable for the pattern <c>attrName = result</c> and
         * sets parameter \p{result} to the string following this pattern.
         *
         * @param      attrName     The name of the attribute searched.
         * @param[out] result       A sub-string with the result.
         * @param      attrDelim    The delimiter to search for. Defaults to '='.
         * @return \c true if the attribute was found, \c false otherwise.
         *         In the latter case, output argument \p{result} will be a \e nulled string.
         ******************************************************************************************/
        ALIB_API bool           GetAttribute( const String& attrName  , Substring& result,
                                              character     attrDelim= A_CHAR('=') );

    // #############################################################################################
    // protected methods
    // #############################################################################################
    protected:

};  // class Variable

}} // namespace aworx[::lib::config]

/// Type alias in namespace #aworx.
using     Variable=           lib::config::Variable;

}  // namespace [aworx]

#endif // HPP_ALIB_CONFIG_VARIABLE
