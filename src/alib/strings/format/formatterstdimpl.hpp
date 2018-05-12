// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
/** @file */ // Hello Doxygen

// include guard
#ifndef HPP_ALIB_STRINGS_FORMAT_FORMATTER_STD
#define HPP_ALIB_STRINGS_FORMAT_FORMATTER_STD 1

// #################################################################################################
// includes
// #################################################################################################
#if !defined (HPP_ALIB_STRINGS_FORMAT_FORMATTER)
    #include "alib/strings/format/formatter.hpp"
#endif


namespace aworx { namespace lib { namespace strings { namespace format {

/** ************************************************************************************************
 * Base class for \alib built-in formatters.
 * This class implements abstract method
 * \alib{strings::format,FormatterBase::format,Formatter::format} and introduces a set of
 * new abstract methods that have to be implemented by descendants.
 *
 * The documentation of the class is split into two sections: The first addresses users of
 * descendant classes, the second provides information on how to extend this class to implement
 * a custom formatter.
 *
 * ## Documentation For Users Of Derived Classes ##
 *
 * Derived formatter types will set attributes in fields #DefaultNumberFormat and
 * #AlternativeNumberFormat to reflect the default behavior of their formatting syntax.<br>
 * Users of derived formatters may modify these attributes prior to performing a format operation
 * to change those defaults. While this leads to a deviation of the formatting standard, it may be
 * used instead of providing corresponding syntactic information within each and every replacement
 * field of the format strings. Some modifications may not even be possible with just using
 * the format specification.
 *
 *
 * ## Documentation For Implementing Derived, Custom Formatters ##
 *
 * Derived types need to set default values for attributes in fields
 * #DefaultNumberFormat and #AlternativeNumberFormat within their constructor once - according
 * to defaults specified by the formatting syntax.
 * This should not be repeated per format operation. This way users of the type are allowed
 * to change such default setting (even if they this may the formatter deviates from the standard
 * it implements).
 *
 * All fields of this class named with prefix \c pha (for example, #phaAlignment) together comprise
 * the set of formatting attributes which can be modified by placeholder semantics of the format string.
 * If further attributes are needed, derived types are free to define custom members to store such
 * attributes. Implementations provided with \alib give such extended attributes the prefix
 * \c phaExt.
 *
 * When parsing a placeholder of a format string, abstract method #parsePlaceholder
 * may set field #phaFormatSpec to reflect a format-specific portion the placeholder string.
 * If it does, it will be checked if the argument supports
 * <b>%ALib %Boxing</b> interface \ref aworx::lib::strings::boxing::IFormat "IFormat"
 * and if so, the boxing interface is invoked with this string as parameter.
 * If the argument does not support the interface, method #parseStdFormatSpec is invoked to
 * now parse this portion of the placeholder string in a default way.<br>
 * This concept allows customized format specifications for custom argument types! As an example,
 * a format specification for date/time argument types may support a custom format string
 * like \c "yyyy-MM-dd HH:mm:ss".
 * \note
 *   This concept is implemented with class
 *   \alib{strings::format,FormatterPythonStyleBase,FormatterPythonStyle} as the
 *   "Python format mini language" supports such custom format specifications. Class
 *   \alib{strings::format,FormatterJavaStyleBase,FormatterJavaStyle} does \b not support
 *   this mechanism.
 *
 * The following describes the formatting process in detail (the implementation of method #format)
 * and this way helps to understand what is required from the implementation of the abstract methods:
 *
 * 1.  A newline character check is made and formatting is aborted if one was found
 *     (as proposed by contract with parent class).
 *
 * 2.  Method parameters are stored in fields #targetString, #formatString, #arguments and
 *     #argOffset. This way, the parameters can be accessed from any implemented method without
 *     the need of passing them as parameters once more.<br>
 *     In addition, field #parser is initialized. This \b Substring is used to parse
 *     the format string. Parsed portions will be consumed from the front of the string.
 *     Finally fields #argsConsumed and #nextAutoIdx are initialized to value \c 0.
 *
 * 3.  <b>Start of the loop</b> to find and process placeholders in the format string.
 *
 * 4.  Abstract method #findPlaceholder is invoked. If this fails (no further placeholder was found)
 *     parsing stops. If, and only if, a placeholder was found before, the rest of the string
 *     in #parser is written and abstract method #replaceEscapeSequences is invoked for this rest.
 *
 * 5.  The portion of the format string before the placeholder position is written and abstract
 *     method #replaceEscapeSequences is invoked on this portion (if not empty).
 *
 * 6.  Method #resetPHAs is invoked, to reset the attributes that will be parsed in the next
 *     step. The values that are set are implementation specific and need to reflect the
 *     default formatting options if no specific options are given in the format string.
 *
 * 7.  Abstract Method #parsePlaceholder is invoked to parse and consume tokens from string #parser
 *     and while doing this, to store the parsed format attributes in the fields with
 *     name prefix \c pha (or extended attributes of a derived formatter type).<br>
 *     If an argument (positional) index is found during parsing, then method
 *     #setArgument is to be invoked by abstract method \b %parsePlaceholder providing that index.<br>
 *     If the format syntax of the formatter contains a separated format specification string
 *     (a substring of the placeholder string),  then the method may store such format
 *     substring in field
 *     \ref aworx::lib::strings::format::FormatterStdImpl::phaFormatSpec "phaFormatSpec".
 *
 * 8.  Next, it is checked if an argument was set by \b %parsePlaceholder. If not, #setArgument
 *     is invoked providing \c -1 for the index to indicate auto-indexing.
 *     \note
 *       If auto-indexing should be implemented differently than done with default method
 *       #setArgument, then a custom formatter might either override the method or,
 *       in the case no index is given in the format string, just
 *       set fields
 *       \ref aworx::lib::strings::format::FormatterStdImpl::phaArgument    "phaArgument" and
 *       \ref aworx::lib::strings::format::FormatterStdImpl::phaArgumentIdx "phaArgumentIdx"
 *       already in \b %parsePlaceholder according to its own strategy
 *
 * 9.  Method #preAndPostProcess is invoked with parameter \p{startIdx} equalling \c -1
 *     (indicating pre-processing). This allows for example to insert tab fill-characters
 *     (tab stops) prior to writing the contents of the field.
 *
 * 10. Method  #writeCustomFormat is invoked. This allows derived formatters to write arguments in a
 *     custom way. If the method returns \c true, the loop is continued ( &rarr; Step 4.). <br>
 *     The default implementation checks if a format specification was stored in #phaFormatSpec
 *     and if yes, if boxing interface
 *     \ref aworx::lib::strings::boxing::IFormat "IFormat" is defined for #phaArgument.
 *     Un this case, the interface is invoked and \c true returned.
 *
 * 11. Again, if a format specification was stored in
 *     \ref aworx::lib::strings::format::FormatterStdImpl::phaFormatSpec "phaFormatSpec"
 *     method #parseStdFormatSpec is invoked which needs to set further attributes
 *     in the \b %Placeholder object according to to the standard format specification of the formatter.
 *
 * 12. Now, as all fields that represent formatting attributes (fields with prefix \c pha) are well
 *     set (or kept with their defaulted value), method #checkStdFieldAgainstArgument is invoked.
 *     This method is virtual but not abstract. Its default implementation checks the
 *     placeholder attributes against the provided argument type and raises an error if the
 *     argument does not fit to the placeholder format specification.
 *
 * 13. Method #writeStdArgument is invoked. This method is virtual but not abstract.
 *     Its default implementation writes the argument value formatted according to the attribute
 *     fields.
 *
 * 14. Finally #preAndPostProcess is invoked with parameter \p{startIdx} pointing to the first
 *     character in #targetString of the argument written.
 *     Here, actions like case conversion might be done on the field written.
 *
 * 14. End of loop ( &rarr; Step 4.)
 *
 *
 * @tparam TChar    The character type. Implementations for \c char and \c wchar_t are provided.
 **************************************************************************************************/
template<typename TChar>
class FormatterStdImpl : public FormatterBase<TChar>
{
    // #############################################################################################
    //  protected fields
    // #############################################################################################
    protected:
        /** The name of the formatter as provided in the constructor. Used for generating
         *  error messages. */
        const StringBase<TChar> formatterName;

        /// The target string as provided with method #Format.
        AStringBase<TChar>*     targetString;

        /// The length of the target string before adding the formatted contents.
        integer                 targetStringStartLength;

        /// The format string as provided with method #Format.
        StringBase<TChar>       formatString;

        /// The list of arguments provided with method #Format.
        const Boxes*            arguments;

        /// The offset of the first argument to use. Provided with method #Format.
        int                     argOffset;

        /// The current (remaining) format string.
        SubstringBase<TChar>    parser;

        /// The number of arguments consumed by the current format string.
        int                     argsConsumed;

        /// Counter for auto-indexed arguments.
        int                     nextAutoIdx;

        /// A buffer to be used when writing aligned fields.
        AStringBase<TChar>      fieldBuffer;

        /** If \c false the formatters specification expects argument to be numbered from
         * <c>0..N</c>. If \c true from <c>1..N</c>.                                              */
        bool                    argumentCountStartsWith1                                    = false;

    // #############################################################################################
    //  Placeholder attribute members
    // #############################################################################################
    protected:
        /// Denotes the requested value type of placeholders.
        enum class PHType
        {
            NotGiven        , ///< The default
            String          , ///< %String type requested

            Character       , ///< Converts a given character or integer number to the corresponding
                              ///< unicode character before printing.

            // Integer types
            IntBase10       , ///< Outputs a given number in base 10. The default.
            IntBinary       , ///< Outputs a given number in base 2.
            IntOctal        , ///< Outputs a given number in base 8.
            IntHex          , ///< Outputs a given number in base 16.

            // Floating types
            Float           , ///< Outputs a number in floating point format.

            // Bool
            Bool            , ///< Writes "true" or "false".
            HashCode        , ///< Writes raw box data as hex.

            // special
            Fill            , ///< Writes #phaFillChar x-times. Used with python-style conversion
                              ///< <b>{!Fill[C]}</b>
        };

        /** The type of the attribute as specified in the placeholder.
         *  This is set to
         *  \ref aworx::lib::strings::format::FormatterStdImpl::PHType::NotGiven "PHType::NotGiven"
         *  in default implementation of #resetPHAs. */
        PHType                      phaType;

        /** The current Box.
         *  This is set by #parsePlaceholder if explicit indexing is used. Otherwise by #format which
         *  invokes #setArgument if #parsePlaceholder did not set it yet.
         *  Set to \c nullptr in default implementation of #resetPHAs. */
        const Box*                  phaArgument;

        /** The positional index of the current #phaArgument.
         *  This is set by #parsePlaceholder if explicit indexing is used. Otherwise by #format which
         *  invokes #setArgument if #parsePlaceholder did not set it yet.
         *  Set to \c -1 in default implementation of #resetPHAs. */
        int                         phaArgumentIdx;

        /** The index of the previous argument. Used when addressing previous argument
         *  number (eg. in Java formatting style this could be "%<$...").
         *  This is set to #phaArgumentIdx in default implementation of #resetPHAs. */
        int                         phaPreviousArgumentIdx;

        /** The portion of the replacement field that represents the format specification.
         *  This field might be set in method #parsePlaceholder and consumed in methods
         *  #writeCustomFormat and #parseStdFormatSpec.<br>
         *  This field is nulled in default implementation of #resetPHAs. */
        SubstringBase<TChar>        phaFormatSpec;

        /** The number format object for the actual attribute. With method #resetPHAs values found
         *  in object #DefaultNumberFormat will be copied into this. */
        NumberFormatBase<TChar>     phaNF;

        /** The (minimum) width of the output.
         *  This is set to \c 0 in default implementation of #resetPHAs. */
        int                         phaWidth;


        /** If not negative, the string representation of the argument is cut prior to
         *  applying any field operation. It could be also named "precision", hence
         *  the number of characters to show - even if the field will be wider.
         *  This is set to \c -1 in default implementation of #resetPHAs. */
        int                         phaCutContent;

        /** If true, an alignment was explicitly specified.
         *  This is set to \c false in default implementation of #resetPHAs. */
        bool                        phaAlignmentSpecified;

        /** The alignment of the contents within a field.
         *  This is set to \ref aworx::lib::lang::Alignment::Left "Alignment::Left" in default
         *  implementation of #resetPHAs. */
        Alignment                   phaAlignment;

        /** The filling character for fields that are larger than their content.
         *  Method #resetPHAs will set this to <c>' '</c>. */
        TChar                       phaFillChar;

        /** Forces the padding to be placed after the sign (if any) but before the digits.
         *  This is used for printing fields in the form ‘+000000120'.
         *  This alignment option is only valid for numeric types.
         *  Set to \c false in default implementation of #resetPHAs. */
        bool                        phaSignPaddingMode;

        /** Used with binary, octal, or hexadecimal output. Specifies that the output will be
         *  prefixed by strings found in fields
         *  \alib{strings,NumberFormatBase::BinLiteralPrefix,BinLiteralPrefix},
         *  \alib{strings,NumberFormatBase::HexLiteralPrefix,HexLiteralPrefix} or
         *  \alib{strings,NumberFormatBase::OctLiteralPrefix,OctLiteralPrefix} which
         *  default to  \c "0b", \c "0o" and \c "0x".
         *  Set to \c false in default implementation of #resetPHAs. */
        bool                        phaWriteBinOctHexPrefix;

        /** Can be \c true for float-types. If \c true, the value is multiplied with 100 and
         *  a percentage symbol \c '\%' is printed after the value.
         *  Set to \c false in default implementation of #resetPHAs. */
        bool                        phaIsPercentage;

        /** This is the (format specific) type code of the current format operation.
         *  Used only to display error messages. May be used differently in derived classes.
         *  Is nulled in default implementation of #resetPHAs. */
        TChar                       phaTypeCode;

        /** This is the position in the format string where the actual type code was read from.
         *  Used for exception argument generation (Exceptions::IncompatibleTypeCode).
         *  If -1, the actual parse position is used.  */
        integer                     phaTypeCodePosition;


    // #############################################################################################
    //  Constructor/destructor
    // #############################################################################################
    public:
        /** ****************************************************************************************
         * Constructor.
         * @param formatterClassName The name of the derived class. Used to generate error messages
         *              including a link into the online documentation. (Therefor
         *              has to be the exact name.
         ******************************************************************************************/
                                FormatterStdImpl( const StringBase<TChar>& formatterClassName );


    // #############################################################################################
    //  Implementation of abstract interface of parent class Formatter
    // #############################################################################################
    protected:

        /** ****************************************************************************************
         * Implemented abstract format method which invokes a set of new abstract methods
         * as described in the main documentation of this class.
         *
         * @param targetString  An AString that takes the result.
         * @param formatString  The format string.
         * @param arguments     The objects to convert.
         * @param argOffset     The first object in \p{arguments} to use.
         *
         * @return The number of args consumed.
         ******************************************************************************************/
        ALIB_API
        virtual int             format( AStringBase<TChar>&        targetString,
                                        const StringBase<TChar>&   formatString,
                                        const Boxes&               arguments,
                                        int                        argOffset )             override;


    // #############################################################################################
    //  Introduction of new, partly abstract methods to be implemented (or optionally overwritten)
    //  by descendents.
    // #############################################################################################
    protected:

        /** ****************************************************************************************
         * Abstract method to search the next index of an argument placeholder in the remaining
         * substring (field #parser) of the format string.
         *
         * @return The index found, \c -1 if not found.
         ******************************************************************************************/
        virtual integer        findPlaceholder()                                               = 0;

        /** ****************************************************************************************
         * Overridable method to clean and reset the fields representing the current placeholder
         * attributes (those with name prefix \c pha) prior to parsing them.
         *
         * The default implementation sets all pha-fields as documented per field.
         * \note
         *   Derived classes (aka the specific formatter classes) are to invoke this (parent)
         *   implementation first and then to make some own adjustments to meet the defaults that
         *   apply to the formatting specification implemented by the derived class and - if this
         *   applies - also to reset extended attributes of the derived formatter type.
         ******************************************************************************************/
        virtual void            resetPHAs();


        /** ****************************************************************************************
         * Abstract method to parse the format definition at the start of string
         * #parser and set the placeholder attributes accordingly.<br>
         * Field #phaFormatSpec might be set by this method to portion of the placeholder format
         * string. If so, methods #writeCustomFormat and #parseStdFormatSpec are used to then parse
         * this portion of the placeholder string.
         *
         * @return \c true on success, \c false on errors.
         ******************************************************************************************/
        virtual bool            parsePlaceholder()                                              = 0;

        /** ****************************************************************************************
         * Virtual method that may write an argument using a custom method/format.
         * The default implementation checks if #phaFormatSpec is set and object
         * \ref aworx::lib::strings::format::FormatterStdImpl::phaArgument "phaArgument"
         * supports an own format specifier by providing <b>%ALib %Boxing</b> interface
         * \ref aworx::lib::strings::boxing::IFormat "IFormat".
         * If so, the result of the formatting is written directly into the #targetString
         * and \c true is returned which causes method #format (which invokes this method) to
         * continue with the next replacement field.
         * If \c false is returned, method #format continues the field processing by invoking
         * #parseStdFormatSpec, #checkStdFieldAgainstArgument and #writeStdArgument.
         *
         * @return \c true if #phaArgument was written, \c false otherwise.
         ******************************************************************************************/
        virtual bool            writeCustomFormat();

        /** ****************************************************************************************
         * Abstract method to parse the format specification for standard types (those that
         * are not processed by #writeCustomFormat). This method may be left empty
         * (just return constant \c true) if method #parsePlaceholder will never sets
         * field #phaFormatSpec.
         *
         * @return \c true on success, \c false on errors.
         ******************************************************************************************/
        virtual bool            parseStdFormatSpec()                                            = 0;

        /** ****************************************************************************************
         * Virtual method invoked after #parseStdFormatSpec and before #writeStdArgument().
         * The default implementation checks the settings of placeholder attribute values
         * (fields with prefix \c pha), which were set by #parsePlaceholder and optionally by
         * #parseStdFormatSpec, against the type of the argument given.
         *
         * If type and format information is missing in the format string, reasonable default
         * values are set depending on the type of the argument.
         *
         * @throws
         *   If the argument type contradicts the replacement field type, exception
         *   \alib{strings::format::Exceptions,IncompatibleTypeCode} is thrown.
         *
         * @return \c true if OK, \c false if replacement should be aborted.
         ******************************************************************************************/
        virtual bool            checkStdFieldAgainstArgument();

        /** ****************************************************************************************
         * Virtual method to write the argument. The default implementation should be sufficient
         * for most derived formatter implementations, but of-course can be overridden and extended.
         ******************************************************************************************/
        virtual void            writeStdArgument();

        /** ****************************************************************************************
         * Virtual method to do pre- and post- processing of the field written.
         * Pre-processing could for example be adding tabulator spaces, letter case conversions,
         *
         * A negative given index \p{startIdx} indicates the pre-processing phase.
         * If \p{target} is given, this indicates an "intermediate phase": The argument has been
         * written, but no alignment or cutting has been done, yet. This phase should usually
         * be ignored, but is for example important for search and replacement actions.
         * If a field has a custom format implementation (e.g. time and date values), then
         * the intermediate phase is never called.
         *
         * \note
         *   The reason why this method is \b not implemented as two different ones is that
         *   derived classes might do some more complicated parsing of parts of the placeholder
         *   string in this method. In this case, the parsing is needed to be implemented only
         *   once, while the finally parsed commands are only conditionally executed depending
         *   if executed as pre or post phase.
         *
         * @param startIdx  If \c -1 pre-processing is indicated, otherwise post-processing and
         *                  the index of the start of the field written in #targetString is given.
         * @param target    The target string, only if different from field #targetString, which
         *                  indicates intermediate phase.
         * @return \c false, if the placeholder should be skipped (nothing is written for it).
         *         \c true otherwise.
         ******************************************************************************************/
        virtual bool           preAndPostProcess(integer startIdx, AStringBase<TChar>* target= nullptr )
        {
            (void) startIdx;
            (void) target;
            return true;
        }

        /** ****************************************************************************************
         * Helper method (overridable) that usually is invoked by implementation of
         * #parsePlaceholder when an argument index is read from the format string,
         *
         * If this does not happen, method #format will invoke this method providing \c -1 for
         * value of parameter \p{pos} to automatically choose the next argument.
         *
         * Consequently, this method sets the fields
         * \ref aworx::lib::strings::format::FormatterStdImpl::phaArgument    "Argument" and
         * \ref aworx::lib::strings::format::FormatterStdImpl::phaArgumentIdx "ArgumentIdx"
         * in given \p{field}.
         * For auto-values, it increments #nextAutoIdx.
         * Finally, this method is responsible for the correct book-keeping of #argsConsumed.
         *
         * @param pos   The index of the argument.
         *              If \c -1 is given, the index is auto-incremented using field #nextAutoIdx.
         * @return \c true on success, \c false on errors.
         ******************************************************************************************/
        virtual bool            setArgument( int pos );

        /** ****************************************************************************************
         * Replace "escaped" placeholder field characters. For example these are \c "{{" in
         * python style or \c "%%" in JAVA style.<br>
         * In addition other escape sequences defined with the format are to be replaced
         * with this method.
         *
         * @param startIdx The start of the region to replace
         ******************************************************************************************/
        virtual void            replaceEscapeSequences( integer startIdx )  = 0;
};


extern template                FormatterStdImpl<char   >::FormatterStdImpl( const NString&);
extern template ALIB_API int   FormatterStdImpl<char   >::format( NAString&, const NString&, const Boxes&, int);
extern template          void  FormatterStdImpl<char   >::resetPHAs();
extern template          bool  FormatterStdImpl<char   >::setArgument(int);
extern template          bool  FormatterStdImpl<char   >::checkStdFieldAgainstArgument();
extern template          void  FormatterStdImpl<char   >::writeStdArgument();
extern template          bool  FormatterStdImpl<char   >::writeCustomFormat();

extern template                FormatterStdImpl<wchar_t>::FormatterStdImpl( const WString&);
extern template ALIB_API int   FormatterStdImpl<wchar_t>::format( WAString&, const WString&, const Boxes&, int);
extern template          void  FormatterStdImpl<wchar_t>::resetPHAs();
extern template          bool  FormatterStdImpl<wchar_t>::setArgument(int);
extern template          bool  FormatterStdImpl<wchar_t>::checkStdFieldAgainstArgument();
extern template          void  FormatterStdImpl<wchar_t>::writeStdArgument();
extern template          bool  FormatterStdImpl<wchar_t>::writeCustomFormat();



}}} // namespace [aworx::lib::strings::format]

}  // namespace aworx




#endif // HPP_ALIB_STRINGS_FORMAT_FORMATTER_STD
