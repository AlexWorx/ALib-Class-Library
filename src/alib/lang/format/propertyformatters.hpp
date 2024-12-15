//==================================================================================================
/// \file
/// This header file is part of sub-namespace #alib::lang::format of module \alib_basecamp of
/// the \aliblong.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_LANG_FORMAT_PROPERTY_FORMATTERS
#define HPP_ALIB_LANG_FORMAT_PROPERTY_FORMATTERS
#pragma once
#if !defined(DOXYGEN)
#   include "alib/alib.hpp"
#endif

ALIB_ASSERT_MODULE(STRINGS)
ALIB_ASSERT_MODULE(BOXING)

#if ALIB_CONFIGURATION

#include "alib/lang/format/propertyformatter.hpp"
#include "alib/lang/format/fmtexceptions.hpp"
#include "alib/config/configuration.hpp"
#include "alib/lang/message/report.hpp"
#include <unordered_map>

namespace alib { namespace  lang::format {

//==================================================================================================
/// # Introduction # {#alib_ns_strings_propertyformatter_map_overview}
///
/// This template class provides a map of \alib{lang::format;PropertyFormatter} objects that use
/// format strings which are defined by variables of a \alib{config;Configuration}.
///
/// The use case is about having different versions of how an object is formatted and
/// to have these versions configurable to end users through the \alib configuration system.
///
///\I{##############################################################################################}
/// # Sample # {#alib_ns_strings_propertyformatter_map_sample}
/// The concept is best explained by a code sample. From the documentation of class
/// \alib{lang::format;PropertyFormatter}, we are using the sample setup:
///
/// \snippet "DOX_STRINGS.cpp"     DOX_STRINGS_PROPERTY_FORMATTER_1
/// \snippet "DOX_STRINGS.cpp"     DOX_STRINGS_PROPERTY_FORMATTER_2
/// \snippet "DOX_STRINGS.cpp"     DOX_STRINGS_PROPERTY_FORMATTER_3
///
/// Three different output formats, \e Short, \e Default and \e All should be supported
/// We define an enumeration type for this purpose and use macro \ref ALIB_ENUMS_ASSIGN_RECORD
/// to equip the enumeration with \ref alib_enums_records "ALib Enum Records" of type
/// \alib{config;Declaration}.
/// In addition, macro \ref ALIB_RESOURCED is used to announce that the record data is defined using
/// resourced string data.
///
/// \snippet "DOX_STRINGS.cpp"     DOX_STRINGS_PROPERTY_FORMATTER_MAP_1
///
/// The following piece of code is to be performed during
/// \ref alib_manual_bootstrapping "bootstrapping".
/// We take the freedom to use the resource pool of module \alib{lang::basecamp;BaseCamp}:
/// \snippet "DOX_STRINGS.cpp"     DOX_STRINGS_PROPERTY_FORMATTER_MAP_2
///
/// \note
///   The default value and variable comment string can be saved when we write back the variables
///   to a configuration file. This hints users to the fact that the output format is changeable,
///   i.e., when they open an INI-file. See chapter \ref alib_config_external_ini of the Programmer's
///   Manual of camp \alib_config for information about how to write built-in default variables like
///   this to an external configuration file.
///
/// In the next phase of bootstrapping, our enumeration records have to be initialized from the
/// resource pool:
///
/// \snippet "DOX_STRINGS.cpp"     DOX_STRINGS_PROPERTY_FORMATTER_MAP_22
///
/// With this in place, the \b %PropertyFormatters can be created and method
/// #Format can be used:
///
/// \snippet "DOX_STRINGS.cpp"     DOX_STRINGS_PROPERTY_FORMATTER_MAP_31
///
/// But we're still not fully done yet: The next goal is to have a lightweight object that is
/// \"\alib{strings;T_Append;appendable}\" to objects of class \alib{strings;TAString;AString}.
/// Such object is given with template type \alib{lang::format;PropertyFormatterMapAppendable}.
/// It is a simple helper-struct that holds a reference to the formatter map, the format enumeration
/// value and the object to format.<br>
/// We define  a shortcut to the type with the right template arguments in place:
/// \snippet "DOX_STRINGS.cpp"     DOX_STRINGS_PROPERTY_FORMATTER_MAP_32
///
/// Now, we are set and can start using the formatter:
/// \snippet "DOX_STRINGS.cpp"     DOX_STRINGS_PROPERTY_FORMATTER_MAP_4
///
/// Running the code above produces the following output:
/// \verbinclude "DOX_STRINGS_PROPERTY_FORMATTER_MAP.txt"
///
/// To make the code even shorter, macros may be used:
/// \snippet "DOX_STRINGS.cpp"     DOX_STRINGS_PROPERTY_FORMATTER_MAP_5
///
/// The output is:
/// \verbinclude "DOX_STRINGS_PROPERTY_FORMATTER_MAP_2.txt"
///
///\I{##############################################################################################}
/// # Using The Appendable With Formatters # {#alib_ns_strings_propertyformatter_map_usingwithfmts}
/// The sample above showcased how to \ref alib_strings_assembly_ttostring "append" a lightweight
/// object of helper-class \alib{lang::format;PropertyFormatterMapAppendable} to objects of class
/// \alib{strings;TAString;AString}.
/// A final step is to define the \ref alib::boxing "boxing" interface
/// \alib{boxing;FAppend} to make boxed objects of our helper-type applicable. As the type
/// itself is already appendable, we can use a built-in simple macro for that:
///
/// \snippet "DOX_STRINGS.cpp"     DOX_STRINGS_PROPERTY_FORMATTER_MAP_6
///
/// As described in the Programmer's Manual of module \alib_boxing, such interface definition
/// should be done only once and be placed in the bootstrap section of a process.
///
/// With this, using formatters in combination with our helper-struct works fine:
/// \snippet "DOX_STRINGS.cpp"     DOX_STRINGS_PROPERTY_FORMATTER_MAP_7
///
/// This code produces the following output.
/// \verbinclude "DOX_STRINGS_PROPERTY_FORMATTER_MAP_3.txt"
///
///\I{##############################################################################################}
/// # Module Dependencies #
/// This class is only available if module \alib_config is included in the \alibdist.
///
///\I{##############################################################################################}
/// # Reference Documentation #
/// @throws alib::lang::format::FMTExceptions::MissingConfigurationVariable
///
/// @tparam  TFormattable   The type that is formatted.
/// @tparam  TVariables      The enumeration of variables which defines the format strings.
//==================================================================================================
template<typename TFormattable, typename TVariables >
class PropertyFormatters
{
    protected:
        /// The callback table for the property formatters (as given in the constructor).
        typename PropertyFormatter::TCallbackTable&         callbacks;

        /// The configuration used to load (and store the default) format strings from.
        config::Configuration&                              config;

        /// The map of formatters
        std::unordered_map<TVariables, PropertyFormatter*>   formatters;

    public:
        /// The formatter (as given in the constructor).
        SPFormatter                                         Formatter;

    public:
        /// Constructor storing arguments in fields.
        /// All variables are declared within the given \p{configuration} and defined with their
        /// default values given with their declaration records (associated with \p{TVariables})
        /// and thus - if exported to a write-enabled external configuration source like
        /// for example \alib{config;IniFileFeeder} - can be noted by the end-user to be
        /// configurable.
        ///
        /// Parameter \p{formatter} defaults to \c nullptr, which causes this constructor to create
        /// a clone of the static formatter given with \alib{lang::format;Formatter::Default}.
        /// Note, that this is in contrast to the similar parameter of class
        /// \alib{lang::format;PropertyFormatter}, which uses the default formatter itself
        /// in the case that \c nullptr is provided. The rationale behind creating a copy is that
        /// it is quite likely that the \b %PropertyFormatter objects created here are used
        /// nested within other format operations. This is especially true with the use of helper
        /// struct \alib{lang::format;PropertyFormatterMapAppendable}. Now, nested formatting must
        /// not be done with the same formatter object.
        ///
        ///
        /// @param callbackTable  The callback table as specified with
        ///                       \alib{lang::format;PropertyFormatter::TCallbackTable}.
        /// @param configuration  The configuration to load the format strings from (and to store
        ///                       the default values defined by the enum record of \p{TVariables}).
        /// @param formatter      The underlying formatter. Used for construction of
        ///                       \alib{lang::format;PropertyFormatter}s.<br>
        ///                       Defaults to \c nullptr which denotes to create a copy of the
        ///                       static formatter given with \alib{lang::format;Formatter::Default}.
        PropertyFormatters( typename PropertyFormatter::TCallbackTable&  callbackTable,
                            config::Configuration&                       configuration,
                            SPFormatter                                  formatter      = nullptr )
        : callbacks( callbackTable )
        , config   ( configuration )
        , Formatter( formatter )
        {
            if ( Formatter == nullptr )
                Formatter= Formatter::Default->Clone();

            for( auto recordIt=   EnumRecords<TVariables>().begin() ;
                      recordIt != EnumRecords<TVariables>().end()   ; ++recordIt  )
            {
                Variable var(config, recordIt.Enum() );
            }
        }

        /// Destructor. Deletes the \alib{lang::format;PropertyFormatter}s which got created.
        ~PropertyFormatters()
        {
            for( auto& formatter : formatters )
                delete formatter.second;
        }

        /// Chooses or - if not yet available - creates the right \alib{lang::format;PropertyFormatter}
        /// and invokes its method \alib{lang::format;PropertyFormatter::Format;Format}.
        ///
        /// @param target       The target string to write into.
        /// @param option       The user-defined formatting option.
        /// @param formattable  The custom object which is passed to the callback methods to collect
        ///                     the formatter arguments.
        void Format( AString& target, TVariables option, TFormattable& formattable )
        {
            auto formatter=  formatters.find( option );
            if( formatter == formatters.end() )
            {
                // find option in the table
                for( auto varEntryIt=  EnumRecords<TVariables>().begin() ;
                     varEntryIt     != EnumRecords<TVariables>().end()   ; ++varEntryIt  )
                    if( varEntryIt.Enum() == option )
                    {
                        Variable  vc(config, varEntryIt.Enum() );
                        if(    vc.GetPriority() == Priority::NONE
                            || vc.GetString().IsEmpty() )
                        {
                            throw Exception(ALIB_CALLER_NULLED, FMTExceptions::MissingConfigurationVariable,
                                            String256(vc) );
                        }
                        formatter= formatters.emplace(
                               std::make_pair( option,
                                               new PropertyFormatter( vc, callbacks, Formatter  ) )
                                                     ).first;
                        break;
                    }

                ALIB_ASSERT_ERROR( formatter != formatters.end(), "FMT",
                                   "No entry for option {!Q<>} found in option table of PropertyFormatters.",
                    option  )
            }

            formatter->second->Format( target, formattable );
        }

}; // class PropertyFormatters

/// Helper-struct for users of class \alib{lang::format;PropertyFormatters}.
/// Objects of this type are \alib{strings;T_Append;appendable} to \b %AStrings.
///
/// A custom using statement should be used to access this type in a very short fashion,
/// to make "applications" to \b %AStrings a short call. Alternatively one ore more preprocessor
/// macros might even lead to shorter invocations. (See sample in class description).

template<typename TFormattable, typename TOptionEnum >
struct PropertyFormatterMapAppendable
{
    PropertyFormatters<TFormattable, TOptionEnum>&   FormatterMap; ///< The formatter map to use.
    TOptionEnum        Option;      ///< The format option defining the
                                    ///< \alib{lang::format;PropertyFormatter} used.
    TFormattable&  Formattable;     ///< The object to be formatted.


    /// Constructor. Just saves the given parameters.
    /// @param pFormatterMap The \alib{lang::format;PropertyFormatters} to use.
    /// @param pOption       The format option defining the
    ///                      \alib{lang::format;PropertyFormatter} used.
    /// @param pFormattable  The object to be formatted.
    PropertyFormatterMapAppendable( PropertyFormatters<TFormattable, TOptionEnum>&  pFormatterMap,
                                    TOptionEnum                                        pOption,
                                    TFormattable&                                  pFormattable )
    : FormatterMap(pFormatterMap)
    , Option(pOption)
    , Formattable(pFormattable)
    {}
}; // struct PropertyFormatterMapAppendable


// #################################################################################################
// ##### T_Append specialization
// #################################################################################################

} namespace strings {

#if DOXYGEN
    namespace APPENDABLES { // Documentation fake namespace
#endif

/// Specialization of functor \alib{strings;T_Append} for helper-struct
/// \b %PropertyFormatterMapAppendable used to format objects with
/// \alib{lang::format;PropertyFormatters}.
/// @tparam TFormattable The formattable object type.
/// @tparam TOptionEnum      The format option enumeration type.
template<typename TFormattable, typename TOptionEnum >
struct       T_Append<typename  lang::format::PropertyFormatterMapAppendable<TFormattable,TOptionEnum>, character, lang::HeapAllocator>
{
    /// Invokes the \alib{lang::format;PropertyFormatters::Format} of the formatter map
    /// encapsulated in \p{src}.
    /// @param target  The target string.
    /// @param src     The helper-struct object.
    void operator()( TAString<character, lang::HeapAllocator>& target, const typename  lang::format::PropertyFormatterMapAppendable<TFormattable,TOptionEnum>& src )
    {
        src.FormatterMap.Format( target, src.Option, src.Formattable );
    }
};

/// Specialization of functor \alib{strings;T_Append} for pointers to helper-struct
/// \b %PropertyFormatterMapAppendable used to format objects with
/// \alib{lang::format;PropertyFormatters}.
/// @tparam TFormattable The formattable object type.
/// @tparam TOptionEnum      The format option enumeration type.
template<typename TFormattable, typename TOptionEnum >
struct       T_Append<typename  lang::format::PropertyFormatterMapAppendable<TFormattable,TOptionEnum>*, character, lang::HeapAllocator>
{
    /// Invokes the \alib{lang::format;PropertyFormatters::Format} of the formatter map
    /// encapsulated in \p{src}.
    /// @param target  The target string.
    /// @param src     Pointer to the helper-struct object.
    void operator()( TAString<character, lang::HeapAllocator>& target, const typename  lang::format::PropertyFormatterMapAppendable<TFormattable,TOptionEnum>* src )
    {
        src->FormatterMap.Format( target, src->Option, src->Formattable );
    }
};

#if DOXYGEN
} // APPENDABLES documentation fake namespace
#endif

} // namespace alib[::strings]


/// Type alias in namespace \b alib.
template<typename TFormattable, typename TOptionEnum >
using PropertyFormatters=  lang::format::PropertyFormatters<TFormattable, TOptionEnum>;

}  // namespace [alib]

#endif // ALIB_CONFIGURATION

#endif // HPP_ALIB_LANG_FORMAT_PROPERTY_FORMATTERS

