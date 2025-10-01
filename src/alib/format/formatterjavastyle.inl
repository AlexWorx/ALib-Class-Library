//==================================================================================================
/// \file
/// This header-file is part of module \alib_format of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
namespace alib::format {

//==================================================================================================
/// Implements a \alib{format;Formatter} according to the
/// \https{formatting standards of the Java language,docs.oracle.com/javase/8/docs/api/java/util/Formatter.html}.
///
/// \note
///   Inherited, public fields of parent class \b FormatterStdImpl provide important possibilities
///   for changing the formatting behavior of instances of this class. Therefore, do not forget
///   to consult the \ref alib::format::FormatterStdImpl "parent classes documentation".
///
/// In general, the original specification is covered quite well. The differences and specialties
/// are:
/// - In deviation of the Java specification, after creation, a formatter in this implementation
///   does not produce locale aware output. Instead, number formatting is set to "computational",
///   hence the decimal point separator is <c>'.'</c> and the grouping character <c>','</c>.
///   As the syntax specification does not provide a feature to switch between standard and locale
///   setting, the corresponding fields of #AlternativeNumberFormat are not used with this formatter.
///   Instead, to enable localized output, method
///   \alib{strings;TNumberFormat::SetFromLocale;NumberFormat::SetFromLocale}
///   has to be invoked on field
///   \alib{format;FormatterStdImpl::DefaultNumberFormat;FormatterStdImpl::DefaultNumberFormat}.
///   Alternatively, attributes of this object may be changed manually or by other means to reflect
///   a desired locale.
/// - Hexadecimal floating point output (conversion type \c 'a' and \c 'A') is not supported.
/// - Flag <c>'('</c>, used to write negative numbers in round brackets, is not supported.
/// - Addressing the previous argument index using \c '%<' is already allowed with the first
///   placeholder.
///   This Chooses the first argument. (In Java a \b MissingFormatArgumentException would be thrown.)
/// - Flag \c '^' is an extension to the standard and denotes
///   center-alignment - just like \c '-' in the standard denotes left-alignment.
///   Right-alignment is the default.
///
///<p>
/// - <b>Floating point values:</b>
///   - If standard field type \c 's' is given together with a precision, the field is cut, even if
///     it cuts the number somewhere. (This is just a warning and same behavior as in original
///     specification.)
///   - For lower case floating point format types (\c 'f', \c 'g' and \c 'e'), the values specified
///     in attributes \b %ExponentSeparator, \b %NANLiteral and \b %INFLiteral of object
///     \alib{format;FormatterStdImpl::AlternativeNumberFormat;FormatterStdImpl::AlternativeNumberFormat}
///     are used. For upper case types (\c 'G' and \c 'E'), the corresponding attributes in
///     \alib{format;FormatterStdImpl::DefaultNumberFormat;FormatterStdImpl::DefaultNumberFormat} apply.
///   - Fixed point format (\c 'f' ) is not supported to use arbitrary length.
///     See class \alib{strings;TNumberFormat;NumberFormat} for the limits.
///     Due to this limitation, the default number of fractional digits is not set with type \c 'f',
///     while in Java it is set to \c 6. This is to allow higher numbers up to \c 1.e13 to be printed
///     in non-scientific format
///   - When both, a \p{width} and a \p{precision} is given, then the \p{precision} determines the
///     fractional part, even if the type is \b 'g' or \b 'G'. This is different from the
///     corresponding specification of Java formatter, which uses \p{precision} as the overall
///     width in case of types \b 'g' or \b 'G'.
///
///<p>
/// - <b>Hexadecimal and Octal Numbers:</b>
///   - Hexadecimal and octal output is <b>cut in size</b> (!) when a field width is given that
///     is smaller than the resulting amount of digits of the number arguments provided.
///       \note  This implies that a value written might not be equal to the value given.
///              This is not a bug but a design decision. The rationale behind this is that with this
///              behavior, there is no need to mask lower digits when passing the arguments to the
///              format invocation. In other words, the formatter "assumes" that the given field width
///              indicates that only a corresponding number of lower digits are of interest.
///
///   - If no width is given and the argument contains a boxed pointer, then the platform-dependent
///     full output width of pointer types is used.
///   - The number <b>grouping option</b> (<c>','</c>) can also be used with binary, hexadecimal
///     and octal output.
///     The types support different grouping separators for nibbles, bytes, 16-bit and 32-bit words.
///     Changing the separator symbols, is not possible with the format fields of the format strings
///     (if it was, this would become very incompatible to Java standards). Changes have to be made
///     before the format operation by modifying field
///     \alib{format;FormatterStdImpl::AlternativeNumberFormat;FormatterStdImpl::AlternativeNumberFormat}
///     which is provided through parent class \b %FormatterStdImpl.
///
///   - Alternative form (\c '#') adds prefixes as specified in members
///     - \alib{strings;TNumberFormat::HexLiteralPrefix;HexLiteralPrefix} and
///     - \alib{strings;TNumberFormat::OctLiteralPrefix;OctLiteralPrefix}.
///
///     For upper case formats,  those are taken from field
///     \alib{format;FormatterStdImpl::DefaultNumberFormat;FormatterStdImpl::DefaultNumberFormat},
///     for lower case formats from
///     \alib{format;FormatterStdImpl::AlternativeNumberFormat;FormatterStdImpl::AlternativeNumberFormat}.
///     All defaults may be changed by the user.
///
///<p>
/// - <b>Time and Date:</b>
///   - In this C++ version of the class, boxed values of type \alib{time;DateTime} are applicable
///     to conversion type \c 't'.
///   - The following time conversion suffix characters are supported:
///     \c 'H', \c 'k', \c 'I', \c 'l', \c 'M', \c 'S', \c 'B', \c 'b', \c 'h', \c 'A', \c 'a',
///     \c 'Y', \c 'y', \c 'm', \c 'd', \c 'e', \c 'R', \c 'T', \c 'D' and \c 'F'
///   - The following time conversion suffix characters are \b not supported:
///     \c 'L', \c 'N', \c 'p', \c 'z', \c 'Z', \c 's', \c 'Q', \c 'C', \c 'j', \c 'r' and \c 'c'.
///
///\I{##############################################################################################}
/// # Reference Documentation #
/// @throws <b>alib::format::FMTExceptions</b>
///   - \alib{format::FMTExceptions;ArgumentIndexIs0}
///   - \alib{format::FMTExceptions;ArgumentIndexOutOfBounds}
///   - \alib{format::FMTExceptions;IncompatibleTypeCode}
///   - \alib{format::FMTExceptions;NegativeValuesInBracketsNotSupported}
///   - \alib{format::FMTExceptions;MissingPrecisionValueJS}
///   - \alib{format::FMTExceptions;HexadecimalFloatFormatNotSupported}
///   - \alib{format::FMTExceptions;NoAlternateFormOfConversion}
///   - \alib{format::FMTExceptions;NoPrecisionWithConversion}
///   - \alib{format::FMTExceptions;UnknownDateTimeConversionSuffix}
///   - \alib{format::FMTExceptions;UnknownConversionJS}
///
//==================================================================================================
class FormatterJavaStyle : public FormatterStdImpl
{
    // #############################################################################################
    // Protected fields
    // #############################################################################################
    protected:
        /// Set of extended placeholder attributes, needed for this type of formatter in
        /// addition to parent's \alib{format::FormatterStdImpl;PlaceholderAttributes}.
        struct PlaceholderAttributesJS
        {
            /// The character after conversion type 't'/'T'.
            character           DateTime;

            /// The value read from the precision field.
            /// This is set to \c -1 in #resetPlaceholder.
            int8_t              Precision;

            /// The default precision if not given.
            /// This is set to \c 6 in #resetPlaceholder, but is changed when specified.
            int8_t              DefaultPrecision;

            /// Convert to upper case.
            bool                ConversionUpper;

            /// Alternate form given ('#').
            bool                AlternateForm;
        };

        /// The extended placeholder attributes.
        PlaceholderAttributesJS placeholderJS;


    // #############################################################################################
    //  Constructor/Destructor
    // #############################################################################################
    public:
        //==========================================================================================
        /// Constructs this formatter.
        /// Inherited field #DefaultNumberFormat is initialized to meet the formatting defaults of
        /// Java.
        //==========================================================================================
        ALIB_DLL
        FormatterJavaStyle();

        //==========================================================================================
        /// Clones and returns a copy of this formatter.
        ///
        /// If the formatter attached to field
        /// \alib{format;Formatter::Next} is of type \b %FormatterStdImpl, then that
        /// formatter is copied as well.
        ///
        /// @returns An object of type \b %FormatterPythonStyle and with the same custom settings
        ///          than this.
        //==========================================================================================
        ALIB_DLL virtual
        SPFormatter         Clone()                                                        override;

    // #############################################################################################
    //  Implementation of FormatterStdImpl interface
    // #############################################################################################
    protected:

        //==========================================================================================
        /// Invokes parent implementation and then applies some changes to reflect what is defined as
        /// default in the Java string format specification.
        //==========================================================================================
        ALIB_DLL
        virtual void        resetPlaceholder()                                             override;

        //==========================================================================================
        /// Searches for \c '\%' which is not '%%' or '%n'.
        ///
        /// @return The index found, -1 if not found.
        //==========================================================================================
        ALIB_DLL
        virtual integer     findPlaceholder()                                              override;

        //==========================================================================================
        /// Parses placeholder field in Java syntax. The portion \p{format_spec} is not set as this
        /// is not supported by the syntax.
        ///
        /// @return \c true on success, \c false on errors.
        //==========================================================================================
        ALIB_DLL
        virtual bool        parsePlaceholder()                                             override;

        //==========================================================================================
        /// Does nothing. Java does not support custom format specifications.
        ///
        /// @return \c true to indicate success.
        //==========================================================================================
        virtual bool        parseStdFormatSpec()                                           override
        {
            return true;
        }

        //==========================================================================================
        /// Implementation of abstract method \alib{format;FormatterStdImpl::writeStringPortion}.<br>
        /// Replaces \c "%%" with \c '\%' and \c "%n" with ascii \c 0x0a. In addition applies
        /// \alib{strings;TEscape;Escape} on \p{target} which replaces
        /// standard codes like \c "\\n", \c "\\r" or \c "\\t" with corresponding ascii codes.
        /// (The latter is an extension to the standard behavior of Java formatter.)
        ///
        /// @param length The number of characters to write.
        //==========================================================================================
        ALIB_DLL
        virtual void        writeStringPortion( integer length )                           override;

        //==========================================================================================
        /// All that this formatter does with this overridden method is to convert strings to
        /// upper case.
        ///
        /// @param startIdx  The index of the start of the field written in #targetString.
        ///                  \c -1 indicates pre-phase.
        /// @param target    The target string, only if different from field #targetString, which
        ///                  indicates intermediate phase.
        /// @return \c false, if the placeholder should be skipped (nothing is written for it).
        ///         \c true otherwise.
        //==========================================================================================
        ALIB_DLL
        virtual bool        preAndPostProcess( integer    startIdx,
                                               AString*   target     )                     override;

        //==========================================================================================
        /// Makes some attribute adjustments and invokes standard implementation
        /// @return \c true if OK, \c false if replacement should be aborted.
        //==========================================================================================
        ALIB_DLL
        virtual bool        checkStdFieldAgainstArgument()                                 override;
};

} // namespace [alib::format]

ALIB_EXPORT namespace alib {
/// Type alias in namespace \b alib.
using  FormatterJavaStyle  =   format::FormatterJavaStyle;
}

