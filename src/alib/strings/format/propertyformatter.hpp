// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################

#ifndef HPP_ALIB_STRINGS_FORMAT_PROPERTY_FORMATTER
#define HPP_ALIB_STRINGS_FORMAT_PROPERTY_FORMATTER

#if !defined (HPP_ALIB_LANG_EXCEPTION)
#include "alib/lang/exception.hpp"
#endif
#if !defined (HPP_ALIB_STRINGS_FORMAT_FORMATTER_STD)
#include "alib/strings/format/formatterstdimpl.hpp"
#endif


namespace aworx { namespace lib { namespace strings { namespace format {


/** ************************************************************************************************
 * # Introduction # {#alib_namespace_strings_propertyformatter_overview}
 *
 * This class can be used to offer customized format strings to end users.
 * For this, the format string definition of \alib
 * \alib{strings::format,FormatterBase,Formatter} types is extended to support identifiers,
 * which act as placeholders for object values. The  identifiers placed in the string have to match
 * to the normal formatting placeholders in respect to their total number and value type.
 *
 * In the constructor of this class, a custom format string which is based on standard \alib
 * format strings is passed. This string is processed as follows:
 * - all custom identifiers are removed
 * - for each identifier found, a reference to an associated callback function is stored.
 *
 * Hence, the translation of the custom format string into \alib formatter strings and the
 * selection of the callback functions is done only once.
 *
 * The only interface method #Format then accepts a target string object and a
 * \ref aworx::lib::boxing "boxed object" of custom type.
 *
 * A vector of tuples is defined and needs to be passed to the constructor of this class.
 * The first element of each tuple contains the \e 'identifier' string which is
 * removed from the format string if found. The second element contains a callback function used
 * to retrieve the data when the identifier is found in the format string.
 *
 * The whole approach is useful in cases where an application wants to allow a customizable output
 * format of data objects.
 *
 * # Sample # {#alib_namespace_strings_propertyformatter_sample}
 * Let us assume, an application uses an enumeration and a simple struct:
 * \snippet "DOX_ALIB_STRINGS.cpp"     DOX_ALIB_STRINGS_PROPERTY_FORMATTER_1
 *
 * In the application's configuration file, a custom output format for objects of type \b Person
 * should be user defined. We document to the user that he can use Python-style or
 * Java-style output formats - extended by the ability to place the following three identifiers in
 * the string:
 * - \@name
 * - \@age
 * - \@hobby.
 *
 * A possible format string then would be:
 *
 *          "{@name} is aged {@age} and his/her hobby is {@hobby}"
 *
 * To tell this class how to retrieve the replacement values, we need to define three callback
 * functions:
 * \snippet "DOX_ALIB_STRINGS.cpp"     DOX_ALIB_STRINGS_PROPERTY_FORMATTER_2
 * It is a good idea to place the callback functions in an anonymous (sub-) namespace as they
 * are only referred to once (in the next step).<br>
 * As this sample is very trivial, the second parameter <c>AString&</c> is not used and named.
 * Details on the purpose and use of this parameter is addressed
 * \ref alib_namespace_strings_propertyformatter_callbacks "below".
 *
 * As a next step, these functions need to be collected together in a "translation table".
 * The table primarily holds a string denoting the replacement identifier and a pointer to the
 * corresponding callback function. For convenience, the table type is provided with <c>using</c>
 * defintion #TCallbackTable.
 *
 * In our sample, the definition of the table looks like this:
 * \snippet "DOX_ALIB_STRINGS.cpp"     DOX_ALIB_STRINGS_PROPERTY_FORMATTER_3
 *
 * This is all we need! Of-course, an external declaration of our table \b PersonCallbacks
 * should be placed somewhere in the projects' header file. With this, a code like this may now use
 * the custom formatter strings:
 * \snippet "DOX_ALIB_STRINGS.cpp"     DOX_ALIB_STRINGS_PROPERTY_FORMATTER_4
 *
 * Running the code above produces the following output:
 * \snippet "DOX_ALIB_STRINGS_PROPERTY_FORMATTER.txt"     OUTPUT
 *
 * Serializing objects in a custom format (e.g. a user could define its own \b JSon object output)
 * should be the most obvious and frequent use case. And this is what the classes name suggests.
 * Nevertheless, the callback methods might be more complex than just returning "properties" of
 * objects. The next sections gives more details on the custom callbacks.
 *
 * \~Comment  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * \~
 * # Callback Functions # {#alib_namespace_strings_propertyformatter_callbacks}
 * The call back function's signature is defined with <c>using</c> definition #TCallback which
 * evaluates to
 *
 *                Box (*)(const Box&, AString&)
 *
 * The return type is \ref aworx::lib::boxing::Box "Box", which allows the callback function to
 * return objects of arbitrary type, respectively all types that received support
 * with \alib formatters.
 *
 * The first input parameter provides the data object passed to method #Format.
 * If the callback functions are dedicated to a property formatter that receives a certain
 * object type (what they usually are), the custom type can be straightly unboxed.
 *
 * The second parameter is an \b AString object which optionally can be used to assemble
 * string objects in more complex callback methods. Note, that if this string is not
 * empty after the callback invocation, the result is copied to a string buffer allocated
 * in the heap memory. In this case, the box object returned by the callback is ignored
 * and instead the copy of the string is passed to the formatter. If C++ string constants
 * (e.g <c>"true"</c> are to be returned by the callbacks, those constants do not need
 * to be copied to the buffer, as their memory is statically allocated. In other words,
 * the \b %AString buffer provided in the second parameter is needed to be used
 * only in cases that a string gets assembled in the callback function!
 *
 * \~Comment  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * \~
 * # Identifiers And The Escape Character '@' # {#alib_namespace_strings_propertyformatter_escape}
 * The escape character used to find custom identifiers in the format string is defined
 * with member #ESCCharacter and defaults to \b '\@'.
 * This escape character is searched in the format string. If found, the identifier is
 * read by consuming alphabetic characters. If a 'closing' second occurrence of \b '@' is found
 * while the identifier is parsed, then this character
 * gets eliminated from the custom format string as well. Hence, the two format strings
 *
 *          "{@name}"
 *          "{@name@}"
 * are both valid and equal.<br>
 *
 * In case of
 * \alib{strings::format,FormatterPythonStyleBase,FormatterPythonStyle} format strings,
 * it is a good idea to place the identifier right inside the brackets. It just looks very intuitive.
 * However, these versions:
 *
 *          "@name{}"
 *          "{}@name"
 * are also valid custom format strings.
 *
 * In case of
 * \alib{strings::format,FormatterJavaStyleBase,FormatterJavaStyle}, which uses \b '\%'
 * as its escape character, we consider the best option to put the custom identifier in front
 * of each \b '\%' symbol. The string of the example given above would then look like this:
 *
 *          "@name%s is aged @age%d and his/her hobby is @hobby%s"
 * Doing it this way, the \b '\%' symbol acts as a very natural delimiter for the custom identifier.
 *
 * As a final note, with the second element of tuple \ref TCallbackTableEntry, it is possible
 * to allow abbreviations of identifier names. This column denotes the minimum
 * number of characters to be matched. As in the above sample a value of \c 1 is provided, each
 * identifier of the custom format string can be abbreviated down to one character. Consequently
 * the following format string samples are all equal and allowed:
 *
 *          "{@name}"
 *          "{@nam}"
 *          "{@na}"
 *          "{@n}"
 * <p>
\~Comment ####################################################################################### \~
 * # Reference Documentation #
 * @throws aworx::lib::strings::format::Exceptions::UnknownPropertyInFormatString
 **************************************************************************************************/
class PropertyFormatter
{
    public:
        /**
         * The signature of the callback functions.
         * See \ref alib_namespace_strings_propertyformatter_callbacks for more information.
         */
        using TCallback=   Box (*)(const Box&, AString&);

        /**
         * The entry type expected/used in the translation table. The entries are tuples with
         * the following elements:
         *
         * <center>Idx</center> | <center>type</center>  | <center>Description</center>
         * - - - - - - - -| - - - - - - - - - - - - - - -| - - - - - - - - - - - - - - - - - - - -
         * 0  | \alib{strings,StringBase,String}| The identifier string
         * 1  | <c>int</c>  | The minimum characters of the identifier to read in the format string. If less or equal to zero, abbreviations are not allowed)
         * 2  | #TCallback  | The callback function for this identifier. See \ref alib_namespace_strings_propertyformatter_callbacks
         */
        using TCallbackTableEntry=      std::tuple< String, int, TCallback >;

        /// Type definition of the callback table. A simple vector of #TCallbackTableEntry objects.
        using TCallbackTable=           std::vector<TCallbackTableEntry>;

        /// The resulting format string passed to \alib formatters.
        SPFormatter                     stdFormatter;

        /** The prefix used to search identifiers in the user defined format string. Defaults
         *  Defaults to <c>'@'</c>.    */
        character                       ESCCharacter                                          = '@';


    protected:
        /**
         * Internal type definition for the list of callback table entries collected in the
         * constructor by parsing the extended format string.
         */
        using TCallbackResultTable= std::vector<const TCallbackTableEntry*>;

        /// The original format string. Used only for exception information.
        AString                 propertyFormatString;

        /// The resulting format string passed to \alib formatters.
        AString                 formatString;


        /// The callback functions to receive the format data.
        TCallbackResultTable    callBacks;

        /// The boxed results of the callbacks (reused container)
        Boxes                   results;

    public:

        /**
         * Constructor. Processes the given format string and builds internal structures
         * to be able to quickly process invocations of method #Format.
         *
         * @param customFormatString  The format string as described in the class documentation.
         * @param propertyTable       Table with property identifier names and callback functions to
         *                            retrieve the property values.
         * @param formatter           The formatter to use. Defaults to \c nullptr which selects
         *                            \alib \alib{strings,Strings::GetDefaultFormatter,default formatter}.
         *
         * @throws aworx::lib::strings::format::Exceptions::UnknownPropertyInFormatString
         */
        ALIB_API
        PropertyFormatter( const String             customFormatString,
                           const TCallbackTable&    propertyTable,
                           SPFormatter              formatter= nullptr );

        /**
         * Writes the formatted output of the properties of the given \p{TFormattable} object
         * to a given target string.
         *
         * @param target    The target string to write into.
         * @param src       The custom object which is passed to the callback methods to collect
         *                  the formatter arguments.
         */
        ALIB_API
        void    Format( AString& target, const Box&  src );


}; // class PropertyFormatter

}}} // namespace aworx[::lib::strings::format]

/// Type alias in namespace #aworx.
using PropertyFormatter  =    aworx::lib::strings::format::PropertyFormatter;

}  // namespace aworx


#endif // HPP_ALIB_STRINGS_FORMAT_PROPERTY_FORMATTER
