/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_text of the \aliblong.
 *
 * \emoji :copyright: 2013-2023 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_TEXT_PROPERTY_FORMATTER
#define HPP_ALIB_TEXT_PROPERTY_FORMATTER

#if !defined (HPP_ALIB_TEXT_FORMATTER_STD)
#   include "alib/text/formatterstdimpl.hpp"
#endif

#if !defined (_GLIBCXX_VECTOR) && !defined(_VECTOR_)
    #include <vector>
#endif

namespace aworx { namespace lib { namespace text {


/** ************************************************************************************************
 * # Introduction # {#alib_ns_strings_propertyformatter_overview}
 *
 * This class can be used to offer customized format strings to end users. For example, when
 * it should be allowed to end-users to store output format strings of complex type.
 *
 * For this, the format string definition of \alib \alib{text,Formatter}
 * types is extended to support identifiers,
 * which act as placeholders for object values. The  identifiers placed in the string have to match
 * to the normal formatting placeholders in respect to their total number and value type.
 *
 * \note
 *   In other words: This class allows not only to have custom format strings which consume a
 *   list of data objects that is hard-coded, but also to have the replacement data objects defined
 *   by the end user.
 *
 * In the constructor of this class, a custom format string which whose syntax is based on
 * \ref alib_text_intro "standard ALib format strings" is passed.
 * However, before using the format string, it is processed as follows:
 * - It is searched for <em>"custom identifiers"</em> within the format string.
 * - For each identifier found, a reference to an associated callback function is stored.
 * - The <em>"custom identifier"</em> found is removed from the format string.
 *
 * It could be said, that the format string given "is compiled" with the constructor of this class.
 *
 * The only interface method #Format then accepts a
 * \ref aworx::lib::boxing "boxed object" of custom type that is used to collect the format string's
 * placeholder data, by using the stored callback functions.
 *
 * The identifiers are denoted by prefixing them with (customizable) character <c>'@'</c>.
 *
 * A vector of objects of inner type \b IdentifierEntry needs to be defined and passed to the
 * constructor of this class.
 * Field \alib{strings::util::PropertyFormatter::IdentifierEntry,Name} contains the \e 'identifier'
 * string which is removed from the format string if found.
 * Field \alib{strings::util::PropertyFormatter::IdentifierEntry,Callback} has to point to a
 * callback function used to retrieve the data when the identifier is found in the format string.
 *
 * The whole approach is useful in cases where an application wants to allow a customizable output
 * format of data objects.
 *
 * # Sample # {#alib_ns_strings_propertyformatter_sample}
 * Let us assume, an application uses an enumeration and a simple struct:
 * \snippet "DOX_ALIB_STRINGS.cpp"     DOX_ALIB_STRINGS_PROPERTY_FORMATTER_1
 *
 * In the application's configuration file, a custom output format for objects of type \b Person
 * should be user defined. We document to the user that she can use Python-style or
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
 * \note
 *   The custom identifiers might appear anywhere in the format string. But as sampled above,
 *   placing them in directly in the formatter replacement fields, makes the string very well
 *   readable. Also, it is important to have exactly one identifier for each replacement field,
 *   which this notation of-course supports well.
 *
 * To tell this class how to retrieve the replacement values, we need to define three callback
 * functions:
 * \snippet "DOX_ALIB_STRINGS.cpp"     DOX_ALIB_STRINGS_PROPERTY_FORMATTER_2
 * It is a good idea to place the callback functions in an anonymous (sub-) namespace as they
 * are only referred to once (in the next step).<br>
 * As this sample is very trivial, the second parameter <c>AString&</c> is not used and named.
 * Details on the purpose and use of this parameter is addressed
 * \ref alib_ns_strings_propertyformatter_callbacks "below".
 *
 * As a next step, these functions need to be collected together in a "translation table".
 * The table primarily holds a string denoting the replacement identifier and a pointer to the
 * corresponding callback function. For convenience, the table type is provided with <c>using</c>
 * definition #TCallbackTable.
 *
 * In our sample, the definition of the table looks like this:
 * \snippet "DOX_ALIB_STRINGS.cpp"     DOX_ALIB_STRINGS_PROPERTY_FORMATTER_3
 *
 * This is all we need! Of-course, an external declaration of our table \b PersonCallbacks
 * should be placed somewhere in the project's header file. With this, a code like this may now use
 * the custom formatter strings:
 * \snippet "DOX_ALIB_STRINGS.cpp"     DOX_ALIB_STRINGS_PROPERTY_FORMATTER_4
 *
 * Running the code above produces the following output:
 * \verbinclude "DOX_ALIB_STRINGS_PROPERTY_FORMATTER.txt"
 *
 * Serializing objects in a custom format (e.g. a user could define its own \b JSon object output)
 * should be the most obvious and frequent use case. And this is what the classes name suggests.
 * Nevertheless, the callback methods might be more complex than just returning "properties" of
 * objects. The next sections gives more details on the custom callbacks.
 *
 * \I{ * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * }
 * # Callback Functions # {#alib_ns_strings_propertyformatter_callbacks}
 * The call back function's signature is defined with <c>using</c>-statement #TCallback which
 * evaluates to
 *
 *                Box (*)(const Box&, AString&)
 *
 * The return type is \ref aworx::lib::boxing::Box "Box", which allows the callback function to
 * return objects of arbitrary type, respectively all types that are are equipped to be used
 * with \alib formatters.
 *
 * The first input parameter provides the data object passed to method #Format.
 * If the callback functions are dedicated to a property formatter that receives a certain
 * object type (what they usually are), the custom type can be straightly unboxed.
 *
 * \note
 *   The name of the class, as well as the sample above, indicate that the data objects are
 *   simple objects with rather simple "properties" to read. While this is the usual use case,
 *   a user of this class might pass more complex objects, or even the root object of the
 *   application to the formatter. Then, the callback function may retrieve (or calculate)
 *   any data that the application provides.
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
 * \I{ * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * }
 * # Identifiers And The Escape Character '@' # {#alib_ns_strings_propertyformatter_escape}
 * The escape character used to find custom identifiers in the format string is defined
 * with parameter \p{ESCCharacter} of the constructor and defaults to \b '\@'.
 * This escape character is searched in the format string. If found, the identifier is
 * read by consuming alphabetic characters. Optionally, the end of a custom identifier can be
 * marked with an additional escape character.
 * Hence, the two format strings
 *
 *          "{@name}"
 *          "{@name@}"
 * are both valid and equal.<br>
 *
 * In case of
 * \alib{text,FormatterPythonStyle,FormatterPythonStyle} format strings,
 * it is a good idea to place the identifier right inside the brackets. It just looks very intuitive.
 * However, these versions:
 *
 *          "@name{}"
 *          "{}@name"
 * are also valid custom format strings.
 *
 * In case of
 * \alib{text,FormatterJavaStyle,FormatterJavaStyle}, which uses \b '\%'
 * as its escape character, we consider the best option to put the custom identifier in front
 * of each \b '\%' symbol. The string of the example given above would then look like this:
 *
 *          "@name%s is aged @age%d and his/her hobby is @hobby%s"
 * Doing it this way, the \b '\%' symbol acts as a very natural delimiter for the custom identifier.
 *
 * Furthermore, field \alib{strings::util::PropertyFormatter,IdentifierEntry::MinimumRecognitionLength}
 * allows abbreviations of identifier names. It denotes the minimum number of characters to be
 * matched. As in the above sample a value of \c 1 is provided, each
 * identifier of the custom format string can be abbreviated down to one character. Consequently
 * the following format string samples are all equal and allowed:
 *
 *          "{@name}"
 *          "{@nam}"
 *          "{@na}"
 *          "{@n}"
 *
 * Finally, to insert the escape character itself into the format string, it has to be doubly
 * inserted (<c>"@@"</c>).
 *
 * \I{ * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * }
 * # Alternative # {#alib_ns_strings_propertyformatter_alternative}
 * If \alib_expressions is compiled with the library, a utility class similar to this one
 * is available with \alib{expressions::util,ExpressionFormatter}.
 * With this, complex expressions and calculations might be used instead of only having simple
 * identifiers as replacements.
 *
 * \I{#############################################################################################}
 * # Reference Documentation #
 * @throws #aworx::lib::text::Exceptions::UnknownPropertyInFormatString
 **************************************************************************************************/
class PropertyFormatter
{
    public:
        /**
         * The signature of the callback functions.
         * See \ref alib_ns_strings_propertyformatter_callbacks for more information.
         */
        using TCallback=   Box (*)(const Box&, AString&);

        /**
         * The entry type of translation table.
         */
        struct IdentifierEntry
        {
            /**  The identifier string to search in the original format string. */
            String      Name;

            /**  The minimum characters of the identifier to read in the format string.
             *   If less or equal to zero, abbreviations are not allowed. */
            int         MinimumRecognitionLength;

            /**  The callback function for this identifier.
             *   See \ref alib_ns_strings_propertyformatter_callbacks.  */
            TCallback   Callback;
        };

        /** Type definition of the callback table.  */
        using TCallbackTable=       std::vector<IdentifierEntry>;


    protected:
        /**
         * Internal type definition for the list of callback table entries collected in the
         * constructor by parsing the extended format string.
         */
        using TCallbackResultTable= std::vector<const IdentifierEntry*>;

        /** The formatter (as given in the constructor). */
        SPFormatter             stdFormatter;

        /** The original format string. Used only for exception information. */
        AString                 propertyFormatString;

        /** The resulting format string passed to \alib formatters. */
        AString                 formatString;

        /** The callback functions to receive the format data. */
        TCallbackResultTable    callBacks;

    public:

        /** ****************************************************************************************
         * Constructor. Processes the given format string and builds internal structures
         * which are then used with invocations of method #Format.
         *
         * @param customFormatString  The format string as described in the class documentation.
         * @param propertyTable       Table with property identifier names and callback functions to
         *                            retrieve the property values.
         * @param formatter           The formatter to use. Defaults to \c nullptr which selects
         *                            \alib{text,Formatter::GetDefault,default formatter}.
         * @param ESCCharacter        The prefix used to search identifiers in \p{customFormatString}.
         *                            Defaults to <c>'@'</c>.
         *
         * @throws #aworx::lib::text::Exceptions::UnknownPropertyInFormatString
         ******************************************************************************************/
        ALIB_API
        PropertyFormatter( const String             customFormatString,
                           const TCallbackTable&    propertyTable,
                           SPFormatter              formatter            = nullptr,
                           character                ESCCharacter         = '@'       );

        /** ****************************************************************************************
         * Writes the formatted output of the properties of the given \p{TFormattable} object
         * to a given \p{target} string.
         *
         * @param target    The target string to write into.
         * @param src       The custom object which is passed to the callback methods to collect
         *                  the formatter arguments.
         ******************************************************************************************/
        ALIB_API
        void    Format( AString& target, const Box&  src );
}; // class PropertyFormatter

}} // namespace aworx[::lib::text]

/// Type alias in namespace #aworx.
using PropertyFormatter  =    lib::text::PropertyFormatter;

} // namespace [aworx]


#endif // HPP_ALIB_TEXT_PROPERTY_FORMATTER
