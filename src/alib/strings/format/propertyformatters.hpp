// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################

#ifndef HPP_ALIB_STRINGS_FORMAT_PROPERTY_FORMATTERS
#define HPP_ALIB_STRINGS_FORMAT_PROPERTY_FORMATTERS


#if !defined(HPP_ALIB_STRINGS_FORMAT_PROPERTY_FORMATTER)
#   include "alib/strings/format/propertyformatter.hpp"
#endif


namespace aworx { namespace lib { namespace strings { namespace format {


/** ************************************************************************************************
 * # Introduction # {#alib_namespace_strings_propertyformatter_map_overview}
 *
 * This template class provides a map of \alib{strings::format,PropertyFormatter} objects that use
 * format strings which are defined by variables of a \alib{config,Configuration}.
 *
 * The use case is about having different versions of how an object is formatted and
 * to have these versions configurable to end users through the \alib configuration system.
 *
\~Comment ####################################################################################### \~
 * # Sample # {#alib_namespace_strings_propertyformatter_map_sample}
 * The concept is best explained by a code sample. From the documentation of class
 * \alib{strings::format,PropertyFormatter}, we are using the same setup as introduced in
 * documentation of class \alib{strings::format,PropertyFormatter}:
 *
 * \snippet "DOX_ALIB_STRINGS.cpp"     DOX_ALIB_STRINGS_PROPERTY_FORMATTER_1
 * \snippet "DOX_ALIB_STRINGS.cpp"     DOX_ALIB_STRINGS_PROPERTY_FORMATTER_2
 * \snippet "DOX_ALIB_STRINGS.cpp"     DOX_ALIB_STRINGS_PROPERTY_FORMATTER_3
 *
 * We want to be able to have three different output formats: \e Short, \e Default and \e All.
 * We define an enumeration for this that we declare to be representing configuration variables
 * using macro \ref ALIB_CONFIG_VARIABLES "ALIB_CONFIG_VARIABLES":
 *
 * \snippet "DOX_ALIB_STRINGS.cpp"     DOX_ALIB_STRINGS_PROPERTY_FORMATTER_MAP_1
 *
 * The variables need to be resourced. For this sample, we do this right in our sample source.
 * Of-course, this code usually is placed in method \alib{lang,Library::init}. Also, note that this
 * sample simply uses the \alib library object. Usually a custom application would provide its own
 * library:
 * \snippet "DOX_ALIB_STRINGS.cpp"     DOX_ALIB_STRINGS_PROPERTY_FORMATTER_MAP_2
 *
 * Note that the default value and variable comment string would be saved when we write
 * back the variables to a configuration file. This hints users to the fact that the output format
 * is changeable, i.e. when they open an INI-file.
 * See \alib{config,Configuration::FetchFromDefault} for information about how to write built-in
 * default variables like this to an external configuration file.
 *
 * With this in place, the \b %PropertyFormatters can be created and its method
 * #Format can be used:
 *
 * \snippet "DOX_ALIB_STRINGS.cpp"     DOX_ALIB_STRINGS_PROPERTY_FORMATTER_MAP_31
 *
 * But we're not done yet: The next goal is to have a lightweight object
 * that is \"\alib{strings,T_Apply,applicable}\" to objects of class \alib{strings,AStringBase,AString}.
 * Such object is given with template type \alib{strings::format,PropertyFormatterMapApplicable}.
 * It is a simple helper struct that holds a reference to the the formatter map, the format
 * enumeration value and the object to format.<br>
 * We define  a shortcut to the type with the right template arguments in place:
 * \snippet "DOX_ALIB_STRINGS.cpp"     DOX_ALIB_STRINGS_PROPERTY_FORMATTER_MAP_32
 *
 * Now, we are set and can start using the formatter:
 * \snippet "DOX_ALIB_STRINGS.cpp"     DOX_ALIB_STRINGS_PROPERTY_FORMATTER_MAP_4
 *
 * Running the code above produces the following output:
 * \snippet "DOX_ALIB_STRINGS_PROPERTY_FORMATTER_MAP.txt"     OUTPUT
 *
 * To make  "application calls" to \b %AString even shorter, macros may be used:
 * \snippet "DOX_ALIB_STRINGS.cpp"     DOX_ALIB_STRINGS_PROPERTY_FORMATTER_MAP_5
 *
 * The output is:
 * \snippet "DOX_ALIB_STRINGS_PROPERTY_FORMATTER_MAP_2.txt"   OUTPUT
 *
\~Comment ####################################################################################### \~
 * # Using The Applicable With Formatters # {#alib_namespace_strings_propertyformatter_map_usingwithfmts}
 * The sample above showcased how to \"\alib{strings,T_Apply,apply}\" a lightwight object of helper
 * class \alib{strings::format,PropertyFormatterMapApplicable} to objects of class \alib{strings,AStringBase,AString}.
 * A final step is to define the \ref aworx::lib::boxing "boxing" interface
 * \alib{strings::boxing,IApply} to make boxed objects of our helper type applicable. As the type
 * itself is already applicable, we can use templated helper
 * \alib{strings::boxing,IApply_TApplicable} for that:
 * \snippet "DOX_ALIB_STRINGS.cpp"     DOX_ALIB_STRINGS_PROPERTY_FORMATTER_MAP_6
 * As described in the namespace documentation of \alib boxing, such interface definition should
 * be done only once and be placed in the bootstrap section of a software.
 *
 * With this, using formatters in combination with our helper struct works fine:
 * \snippet "DOX_ALIB_STRINGS.cpp"     DOX_ALIB_STRINGS_PROPERTY_FORMATTER_MAP_7
 *
 * This code produces the following output.
 * \snippet "DOX_ALIB_STRINGS_PROPERTY_FORMATTER_MAP_3.txt"   OUTPUT
 *
\~Comment ####################################################################################### \~
 * # Reference Documentation #
 * @throws aworx::lib::strings::format::Exceptions::MissingConfigurationVariable
 *
 * @tparam  TFormattable   The type that is formatted
 * @tparam  TVariable      The enumeration of variables which define the format strings.
 **************************************************************************************************/
template<typename TFormattable, typename TVariable >
class PropertyFormatters
{
    protected:
        /// The map of formatters
        std::map< TVariable, PropertyFormatter* >       formatters;

        /// The callback table for the property formatters. (As given in the constructor.)
        typename PropertyFormatter::TCallbackTable&     callbacks;

    public:
        /// The formatter to use. (As given in the constructor.)
        SPFormatter                                     Formatter;


    public:
        /**
         * Constructor storing arguments in fields.
         * All default values of the variables are stored in the \ref aworx::lib::config "configuration"
         * and thus - if the configuration is write enabled - can be noted by the end-user to be
         * configurable.
         *
         * Parameter \p{formatter} defaults to \c nullptr, which causes this constructor to create
         * a clone of the formatter returned by \alib{strings,Strings::GetDefaultFormatter}.
         * Note, that this is in contrast to the similar parameter of class
         * \alib{strings::format,PropertyFormatter}, which uses the default formatter itself
         * in the case that \c nullptr is provided. The rationale behind creating a copy is that
         * it is quite likely that the \b %PropertyFormatter objects created here are used
         * nested within other format operations. This is especially true with the use of helper
         * struct \alib{strings::format,PropertyFormatterMapApplicable}. Now, nested formatting must
         * not be done with the same formatter object.
         *
         *
         * @param callbackTable  The callback table as specified with
         *                       \alib{strings::format,PropertyFormatter::TCallbackTable}.
         * @param formatter      The underlying formatter. Used for construction of
         *                       \alib{strings::format,PropertyFormatter}s.<br>
         *                       Defaults to \c nullptr which denotes to create a copy of the
         *                       formatter returned by
         *                       \alib{strings,Strings::GetDefaultFormatter}.
         */
        PropertyFormatters( typename PropertyFormatter::TCallbackTable&  callbackTable,
                            SPFormatter                               formatter= nullptr )
        : callbacks ( callbackTable )
        , Formatter ( formatter )
        {
            if ( Formatter == nullptr )
                Formatter.reset( lib::STRINGS.GetDefaultFormatter()->Clone() );

            auto varMetaData= EnumMetaData<TVariable>::GetSingleton();
            varMetaData->CheckLoad();
            Variable var;
            for( auto& optionRecord : varMetaData->Table )
                lib::lang::T_Resourced<TVariable>::Lib().Config->StoreDefault( var.Declare( TVariable( std::get<0>( optionRecord ) ) ) );
        }

        /**
         * Destructor. Deletes the \alib{strings::format,PropertyFormatter}s which got created.
         */
        ~PropertyFormatters()
        {
            for( auto& formatter : formatters )
                delete formatter.second;
        }

        /**
         * Chooses or - if not yet available - creates the right \alib{strings::format,PropertyFormatter}
         * and invokes its method \alib{strings::format,PropertyFormatter::Format,Format}.
         *
         * @param target       The target string to write into.
         * @param option       The user defined formatting option.
         * @param formattable  The custom object which is passed to the callback methods to collect
         *                     the formatter arguments.
         */
        void Format( AString& target, TVariable  option, TFormattable& formattable )
        {
            auto formatter= formatters.find( option );
            if( formatter == formatters.end() )
            {
                // find option in the table
                Variable var;
                auto varMetaData= EnumMetaData<TVariable>::GetSingleton();
                for( auto& varEntry : varMetaData->Table )
                {
                    TVariable varEnum= TVariable( std::get<0>(varEntry) );
                    if( varEnum == option )
                    {
                        if(    lib::lang::T_Resourced<TVariable>::Lib().Config->Load( var.Declare( varEnum ) ) == Priorities::NONE
                            || var.GetString()->IsEmpty() )
                            throw Exception(ALIB_CALLER_NULLED, Exceptions::MissingConfigurationVariable,
                                            var.Fullname );

                        formatter= formatters.insert( std::make_pair(option, new PropertyFormatter( var.GetString(), callbacks, Formatter) ) ).first;
                        break;
                    }
                }

                ALIB_ASSERT_ERROR( formatter != formatters.end(),
                   "No entry for option <{}> found in option table of PropertyFormatters.",
                    option  );
            }

            formatter->second->Format( target, formattable );
        }

}; // class PropertyFormatters

/**
 * Helper struct for users of class \alib{strings::format,PropertyFormatters}.
 * Objects of this type are \alib{strings,T_Apply,applicable} to \b %AStrings.
 *
 * A custom using statement should be used to access this type in a very short fashion,
 * to make "applications" to \b %AStrings a short call. Alternatively one ore more preprocessor
 * macros might even lead to shorter invocations. (See sample in class description).
 */

template<typename TFormattable, typename TOptionEnum >
struct PropertyFormatterMapApplicable
{
    PropertyFormatters<TFormattable, TOptionEnum>&   FormatterMap; ///< The formatter map to use.
    TOptionEnum        Option;      ///< The format option defining the
                                    ///< \alib{strings::format,PropertyFormatter} used.
    TFormattable&  Formattable;     ///< The object to be formatted.


    /**
     * Constructor. Just saves the given parameters.
     * @param pFormatterMap The \alib{strings::format,PropertyFormatters} to use.
     * @param pOption       The format option defining the
     *                      \alib{strings::format,PropertyFormatter} used.
     * @param pFormattable  The object to be formatted.
     */
    PropertyFormatterMapApplicable( PropertyFormatters<TFormattable, TOptionEnum>&  pFormatterMap,
                                    TOptionEnum                                        pOption,
                                    TFormattable&                                  pFormattable )
    : FormatterMap(pFormatterMap)
    , Option(pOption)
    , Formattable(pFormattable)
    {}
};


} // namespace aworx::lib::strings[::format]

// #################################################################################################
// ##### T_Apply specialization
// #################################################################################################


/**
 * Specialization of \alib{strings,T_Apply} for helper struct \b %PropertyFormatterMapApplicable
 * used to format objects with \alib{strings::format,PropertyFormatters}.
 * @tparam TFormattable The formattable object type.
 * @tparam TOptionEnum      The format option enumeration type.
 */
template<typename TFormattable, typename TOptionEnum >
struct       T_Apply<typename format::PropertyFormatterMapApplicable<TFormattable,TOptionEnum>, character > : public std::true_type
{
    /**
     * Invokes the \alib{strings::format,PropertyFormatters::Format} of the formatter map
     * encapsulated in \p{src}.
     * @param target  The target string.
     * @param src     The helper struct object.
     * @return Constant \p{1}.
     */
    static integer Apply( AStringBase<character>& target, typename format::PropertyFormatterMapApplicable<TFormattable,TOptionEnum>& src )
    {
        src.FormatterMap.Format( target, src.Option, src.Formattable );
        return 1;
    }
};

}} // namespace aworx::[lib::strings]




/// Type alias in namespace #aworx.
template<typename TFormattable, typename TOptionEnum >
using PropertyFormatters= aworx::lib::strings::format::PropertyFormatters<TFormattable, TOptionEnum>;

}  // namespace [aworx]


#endif // HPP_ALIB_STRINGS_FORMAT_PROPERTY_FORMATTERS
