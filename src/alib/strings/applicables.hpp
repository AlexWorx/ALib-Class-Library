// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################

// needed for Doxygen include of the typedefs at the end of the file
///@file

// include guard
#ifndef HPP_ALIB_STRINGS_BUILTIN_APPLICABLES
#define HPP_ALIB_STRINGS_BUILTIN_APPLICABLES 1

// to preserve the right order, we are not includable directly from outside.
#if !defined(ALIB_PROPER_INCLUSION)
    #error "include 'alib/alib.hpp' instead of this header"
#endif


// This is for testing internal include order, see stdafx_alib.h
#if defined(HPP_ALIB_STRINGS_NUMBERFORMAT)
    #error "ALib Include order error"
#endif
#if defined(HPP_ALIB_STRINGS_UTIL_TOKENIZER)
    #error "ALib Include order error"
#endif

namespace aworx { namespace lib { namespace strings
{


// #################################################################################################
// struct T_Apply: fundamental types
// #################################################################################################

// We are faking all template specializations of namespace strings for doxygen into namespace
// strings::applyto to keep the documentation of namespace string clean!
#if defined(DOX_PARSER)
namespace applyto {
#endif

/// Specialization of template struct \alib{strings,T_Apply} for type \ref aworx::character.*.
template<> struct       T_Apply<character* ,character> : public std::true_type
{
    /** ********************************************************************************************
     * Writes a cstring to the given AString.
     * @param target The object that \b Apply was invoked on.
     * @param src    The character string to append to \p{target}.
     * @return The number of characters appended to target.
     **********************************************************************************************/
    static inline integer Apply( AStringBase<character>& target, const character* src )
    {
        integer length= CString<character>::Length( src );
        target.template Append<false>( src, length );
        return length;
    }
};

/// Specialization of template struct \alib{strings,T_Apply} for type \ref aworx::complementChar.
template<> struct       T_Apply<complementChar* ,character> : public std::true_type
{
    /** ********************************************************************************************
     * Writes a cstring of complementary character type to the given AString.
     * @param target The object that \b Apply was invoked on.
     * @param src    The wide character string to append to \p{target}.
     * @return The number of characters appended to target.
     **********************************************************************************************/
    static inline integer Apply( AStringBase<character>& target, const complementChar* src)
    {
        integer length= CString<complementChar>::Length( src );
        if( length )
            target.template Append<false>( src, length );
        return length;
    }
};

/// Specialization of template struct \alib{strings,T_Apply} for type \c strangeChar*.
template<typename TChar> struct       T_Apply<strangeChar* ,TChar> : public std::true_type
{
    /** ********************************************************************************************
     * This method converts the provided \b %strangeChar string (zero-terminated array of
     * \ref aworx::strangeChar) and appends it to the \p{target}.
     * Appropriate, platform dependent conversion is performed according to the system's locale
     * configuration.
     *
     * @param target The object that \b Apply was invoked on.
     * @param src    The wide character string to append to \p{target}.
     * @return The number of characters appended to target.
     **********************************************************************************************/
    static inline integer Apply( AStringBase<TChar>& target, const strangeChar* src)
    {
        integer length= CString<strangeChar>::Length( src );
        if( length )
            target.template Append<false>( src, length );
        return length;
    }
};

/// Specialization of template struct \alib{strings,T_Apply} for type \c bool.
template<typename TChar> struct       T_Apply<bool ,TChar> : public std::true_type
{
    /** ********************************************************************************************
     * By default, writes the words "true" or "false" to the given AString.
     * The names are resourced in library \alib{strings,Strings} with keys
     * \c "APPLY_T" and \c "APPLY_F".
     * @param target The object that \b Apply was invoked on.
     * @param b      The boolean to write to \p{target}.
     * @return The number of characters appended to target.
     **********************************************************************************************/
    ALIB_API
    static integer Apply( AStringBase<TChar>& target, bool& b );
};


#if ALIB_DEBUG
/// Specialization of template struct \alib{strings,T_Apply} for type \c std::type_info.
template<typename TChar> struct       T_Apply<std::type_info ,TChar> : public std::true_type
{
    /** ********************************************************************************************
     * Writes the demangled type name.
     * @param target The object that \b Apply was invoked on.
     * @param type   The type to write to \p{target}.
     * @return The number of characters appended to target.
     **********************************************************************************************/
    ALIB_API
    static integer Apply( AStringBase<TChar>& target, const std::type_info& type );
};
#endif


// We are faking all template specializations of namespace strings for doxygen into namespace
// strings::apply to keep the documentation of namespace string clean!
#if defined(DOX_PARSER)
}
#endif


// #################################################################################################
// class Format
// #################################################################################################

/** ************************************************************************************************
 * This is a type applicable to objects of type
 * \alib{strings,AStringBase,AString}. Various constructors consume integer and
 * floating point types, along with formatting options. The specialization of struct
 * \alib{strings,T_Apply}
 *  will use a given (or defaulted) instance of class
 * \alib{strings,NumberFormatBase,NumberFormat} to append the value to the
 * \b %AString in question.
 *
 * \note
 *   Within the same header file that this class in declared in, there are several
 *   specializations of \alib{strings,T_Apply} defined for plain integer and
 *   floating point types. These specializations create an object of this type, providing the
 *   value only, hence, using this classes constructors' default values. The number format
 *   used as default by the constructors of this class is
 *   \alib{strings,NumberFormatBase::Computational,NumberFormat::Computational}.
 *   As a result, the application of such core types, as in:
 *   \snippet "DOX_ALIB_APPLYTO.cpp"     DOX_ALIB_APPLYTO_FORMAT1
 *   which produces:
 *   \snippet "DOX_ALIB_APPLYTO_FORMAT1.txt"     OUTPUT
 *
 * \note
 *   does \b not use a locale specific number format. Instead it uses one that is exchangeable
 *   between applications independent from the locale setting of the executing machine.<br>
 *   Consequently, for locale specific output, an object of this class needs to be applied
 *   along with a locale enabled instance of \b %NumberFormat. For example:
 *   \snippet "DOX_ALIB_APPLYTO.cpp"     DOX_ALIB_APPLYTO_FORMAT2
 *   which - dependent on the current local setting - might produce:
 *   \snippet "DOX_ALIB_APPLYTO_FORMAT2.txt"     OUTPUT
 *
 *
 * <b>Inner Types:</b><br>
 * Besides accepting plain number types, this class in addition aggregates several public inner
 * types, namely
 * - \alib{strings,FormatBase::Tab,Format::Tab},
 * - \alib{strings,FormatBase::Field,Format::Field},
 * - \alib{strings,FormatBase::Escape,Format::Escape},
 * - \alib{strings,FormatBase::Bin,Format::Bin},
 * - \alib{strings,FormatBase::Hex,Format::Hex} and
 * - \alib{strings,FormatBase::Oct,Format::Oct}.
 *
 * Each of these classes has partial implementations of template function
 * \alib{strings,T_Apply} and provide certain formatting options.
 *
 * \note The types are not 'physically' related to this class. Instead, they have been
 *       aggregated here for 'esthetical' reasons.
 *
 * <b>Details on Formats:</b><br>
 * Details on the options of formatting integer and floating point numbers are documented
 * with class
 * \alib{strings,NumberFormatBase,NumberFormat}.
 *
 * @tparam TChar    The character type. Implementations for \c char and \c wchar_t are provided
 *                  with type definitions \ref aworx::NFormat and \ref aworx::WFormat.
 **************************************************************************************************/
template<typename TChar>
class FormatBase
{
    public:

    // #############################################################################################
    // Inner types
    // #############################################################################################

    /** ********************************************************************************************
     * Used to create temporary objects which are \alib{strings,T_Apply,applied} to
     * \alib{strings,AStringBase,AString} instances.<br>
     *
     * Appends \e tab characters (as provided) to reach a certain length (aka tabulator position)
     * of the \p{target}. The tab position is referenced to an optionally given \p{reference} value
     * which might be the start of the string or the position of the last newline. If this
     * parameter is negative, the last newline characters are searched from the end of the
     * string backwards.<br>
     * Referring to that as position 0, the tab position is then located at the next multiple
     * of parameter \p{tabSize}, after having added  \p{minPad} tab characters.
     **********************************************************************************************/
    struct Tab
    {
        public:
        /// The tab positions are multiples of this value.
        integer     tabSize;

        /** The reference length of the AString which is taken as relative tab position
         *  (instead of the beginning of the string). If -1, the target %AString is
         *  searched backwards for the last newline and this position is used as the
         *  reference.
         */
        integer     reference;

        /// The minimum pad characters to add. Defaults to 1.
        integer     minPad;

        /** The character to insert to reach the tab position. Normally this is the space
         *  character ' '.*/
        TChar       tabChar;

        /**
         * Constructor. Copies the parameters.
         *
         * @param size          The tab positions are multiples of this parameter.
         * @param referenceIdx  The reference index marking the start of the actual line.
         *                      If -1, the last new-line character is searched from the end of
         *                      the string backwards, and used. Defaults to 0.
         * @param minPadChars   The minimum pad characters to add. Defaults to 1.
         * @param fillChar      The character to insert to reach the tab position.
         *                      Defaults to ' ' (space).
         */
        Tab(  integer size, integer referenceIdx = 0, integer minPadChars= 1, TChar fillChar= ' ' )
        : tabSize(size), reference(referenceIdx), minPad(minPadChars), tabChar(fillChar)
        {}
    };

    /** ********************************************************************************************
     * Used to create temporary objects which are \alib{strings,T_Apply,applied} to
     * \alib{strings,AStringBase,AString} instances.<br>
     *
     * Appends the given object to the AString using a defined 'field'-width.
     * If the contents of the field is shorter than parameter \p{width} specifies, the field is
     * filled with a corresponding amount of \p{padChar} characters.<br>
     * Parameter \p{alignment} of type
     * \ref aworx::lib::lang::Alignment "Alignment" allows to left-, right- or center-align
     * the contents of the field.
     *
     * \note In case, module <b>%ALib %Boxing</b> is not available, the field content
     *       parameter will be of type <c>const String&</c>.<br>
     *       Otherwise, boxing interface \alib{strings::boxing,IApply} will be invoked on
     *       the given box internally to receive the string representation.
     *
     * \note Therefore, it is mandatory, that for any type that is used with this class
     *       to be formatted in a field, this boxing interface has to be implemented. As
     *       documented with that interface, for types that are applicable to \b %AString
     *       objects already, all that is needed is to use macro
     *       \ref ALIB_BOXING_DEFINE_IAPPLY_FOR_APPLICABLE_TYPE with the type in the bootstrap
     *       section of an application.
     **********************************************************************************************/
    struct Field
    {
        public:
        #if ALIB_MODULE_BOXING
        Box              theContent;   ///< The content of the field. If module
                                       ///< <b>%ALib %Boxing</b> is not available, this field
                                       ///< is of type <c>const StringBase<TChar>&</c>
        #else
        const StringBase<TChar>&  theContent;
        #endif
        integer          theWidth;     ///< The width of the field.
        lang::Alignment  theAlignment; ///< The alignment of the contents within the field.
        TChar            padChar;      ///< The characters used for padding the contents within the field.

        /**
         * Constructor. Copies the parameters.
         *
         * @param content   The contents of the field. If module <b>%ALib %Boxing</b> is not
         *                  available, this field is of type <c>const StringBase<TChar>&</c>,
         *                  otherwise of type \alib{boxing,Box}.
         * @param width     The width of the field
         * @param alignment The alignment of the contents within the field.
         *                  Defaults to
         *                  \ref aworx::lib::lang::Alignment "Alignment::Right"
         *                  Other options are
         *                  \ref aworx::lib::lang::Alignment "Alignment::Left" and
         *                  \ref aworx::lib::lang::Alignment "Alignment::Center".
         * @param fillChar  The character used to fill the field up to its size.
         *                  Defaults to ' ' (space).
         */
        Field(
                #if ALIB_MODULE_BOXING
                Box              content,
                #else
                const StringBase<TChar>&  content,
                #endif

                integer           width,
                lang::Alignment   alignment  =lang::Alignment::Right,
                TChar             fillChar   = ' '                       )
        #if ALIB_MODULE_BOXING
        : theContent(content),
        #else
        : theContent(content.IsNotNull() ? content : EmptyString ),
        #endif
          theWidth(width),
          theAlignment(alignment),
          padChar(fillChar)
        {}

    };

    /** ********************************************************************************************
     * Used to create temporary objects which are \alib{strings,T_Apply,applied} to
     * \alib{strings,AStringBase,AString} instances.<br>
     *
     * Escapes non-printable characters in the given region, or reversely converts such escaped
     * characters to their ASCII values.
     *
     * The characters converted are
     * <c>'\\\\'</c>, <c>'\\r'</c>, <c>'\\n'</c>, <c>'\\t'</c>, <c>'\\a'</c>,
     * <c>'\\b'</c>,  <c>'\\v'</c>, <c>'\\f'</c>, <c>'\\e'</c> and  <c>'"'</c>.
     *
     * If the new region length is needed to be known, it can be calculated as the sum of
     * the old region length and the difference of the string's length before and after the
     * operation.
     **********************************************************************************************/
    struct Escape
    {
        public:
        /** The direction of conversion: \b Switch::On escapes ascii characters, while
         *  \b Switch::Off converts escaped strings to ascii codes.*/
        Switch       pSwitch;

        /// The start of the region to convert.
        integer      startIdx;

        /// The length of the region to convert.
        integer      length;


        /**
         * Constructor. Copies the parameters.
         *
         * @param escape        \b Switch::On escapes ascii characters (the default),
         *                      \b Switch::Off converts  escaped strings to ascii codes.
         * @param regionStart   The start of the region to convert.
         * @param regionLength  The length of the region to convert.
         */
        Escape( Switch escape= Switch::On, integer regionStart = 0, integer regionLength= MaxLen )
        : pSwitch(escape), startIdx(regionStart), length(regionLength)
        {}
    };


    /** ********************************************************************************************
     * Used to create temporary objects which are \alib{strings,T_Apply,applied} to
     * \alib{strings,AStringBase,AString} instances.<br>
     * Appends an integer value in binary format.<br>
     * See \alib{strings,NumberFormatBase::WriteBin,NumberFormat::WriteBin}
     * for more information on formatting options.
     **********************************************************************************************/
    struct Bin
    {
        public:
        uint64_t        theValue;   ///< The value to write.
        int             theWidth;   ///< The minimum width of the number to write.
                                    ///< Defaults to \c 0
                                    ///< which denotes to choose the value of field
                                    ///< \alib{strings,NumberFormatBase::BinFieldWidth,NumberFormat::BinFieldWidth}.
        NumberFormatBase<TChar>* nf;///< The number format to use. Defaults to \c nullptr which chooses
                                    ///< \alib{strings,NumberFormatBase::Computational,NumberFormat::Computational}.

        /**
         * Constructor, taking the value and formatting parameters.
         *
         * @param value         The value to write.
         * @param overrideWidth Defaults to \c 0 which
         *                      denotes to choose the value of field
         *                      \alib{strings,NumberFormatBase::BinFieldWidth,NumberFormat::BinFieldWidth}.
         * @param numberFormat  The number format to use. Defaults to \c nullptr which chooses
         *                      the static singleton found in
         *                      \alib{strings,NumberFormatBase::Computational,NumberFormat::Computational}.
         * @tparam TIntegral    Value type which has to be statically castable to \c std::uint64_t.
         */
        template<typename TIntegral>
        Bin( TIntegral                 value,
             int                       overrideWidth= 0,
             NumberFormatBase<TChar>*  numberFormat = nullptr        )
        : theValue  (static_cast<uint64_t>(value))
        , theWidth  (overrideWidth)
        , nf        (numberFormat)                  {}

        /**
         * Constructor, taking the value and a just an object of type \b %NumberFormat.
         *
         * @param value         The value to write.
         * @param numberFormat  The number format to use. Defaults to \c nullptr which chooses
         *                      the static singleton found in
         *                      \alib{strings,NumberFormatBase::Computational,NumberFormat::Computational}.
         * @tparam TIntegral    Value type which has to be statically castable to \c std::uint64_t.
         */
        template<typename TIntegral>
        Bin( TIntegral                 value,
             NumberFormatBase<TChar>*  numberFormat                  )
        : theValue  (static_cast<uint64_t>(value))
        , theWidth  (0)
        , nf        (numberFormat)                  {}

    };

    /** ********************************************************************************************
     * Used to create temporary objects which are \alib{strings,T_Apply,applied} to
     * \alib{strings,AStringBase,AString} instances.<br>
     * Appends an integer value in hexadecimal format.<br>
     * See \alib{strings,NumberFormatBase::WriteHex,NumberFormat::WriteHex}
     * for more information on formatting options.
     **********************************************************************************************/
    struct Hex
    {
        public:
        uint64_t    theValue;       ///< The value to write.
        int         theWidth;       ///< The minimum width of the number to write.
                                    ///< Defaults to \c 0
                                    ///< which denotes to choose the value of field
                                    ///< \alib{strings,NumberFormatBase::HexFieldWidth,NumberFormat::HexFieldWidth}.
        NumberFormatBase<TChar>* nf;///< The number format to use. Defaults to \c nullptr which chooses
                                    ///< \alib{strings,NumberFormatBase::Computational,NumberFormat::Computational}.

        /**
         * Constructor, taking the value and formatting parameters.
         *
         * @param value         The value to write.
         * @param overrideWidth Defaults to \c 0 which
         *                      denotes to choose the value of field
         *                      \alib{strings,NumberFormatBase::HexFieldWidth,NumberFormat::HexFieldWidth}.
         * @param numberFormat  The number format to use. Defaults to \c nullptr which chooses
         *                      the static singleton found in
         *                      \alib{strings,NumberFormatBase::Computational,NumberFormat::Computational}.
         * @tparam TIntegral    Value type which has to be statically castable to \c std::uint64_t.
         */
        template<typename TIntegral>
        Hex( TIntegral                  value,
             int                        overrideWidth= 0,
             NumberFormatBase<TChar>*   numberFormat = nullptr        )
        : theValue  (static_cast<uint64_t>(value))
        , theWidth  (overrideWidth)
        , nf        (numberFormat)                  {}

        /**
         * Constructor, taking the value and a just an object of type \b %NumberFormat.
         *
         * @param value         The value to write.
         * @param numberFormat  The number format to use. Defaults to \c nullptr which chooses
         *                      the static singleton found in
         *                      \alib{strings,NumberFormatBase::Computational,NumberFormat::Computational}.
         */
        template<typename TIntegral>
        Hex( TIntegral                  value,
             NumberFormatBase<TChar>*   numberFormat                  )
        : theValue  (static_cast<uint64_t>(value))
        , theWidth  (0)
        , nf        (numberFormat)                  {}
    };

    /** ********************************************************************************************
     * Used to create temporary objects which are \alib{strings,T_Apply,applied} to
     * \alib{strings,AStringBase,AString} instances.<br>
     * Appends an integer value in octal format.<br>
     * See \alib{strings,NumberFormatBase::WriteHex,NumberFormat::WriteHex}
     * for more information on formatting options.
     **********************************************************************************************/
    struct Oct
    {
        public:
        uint64_t        theValue; ///< The value to write.
        int             theWidth; ///< The minimum width of the number to write.
                                  ///< Defaults to \c 0
                                  ///< which denotes to choose the value of field
                                  ///< \alib{strings,NumberFormatBase::OctFieldWidth,NumberFormat::OctFieldWidth}.
        NumberFormatBase<TChar>* nf;   ///< The number format to use. Defaults to \c nullptr which chooses
                                  ///< \alib{strings,NumberFormatBase::Computational,NumberFormat::Computational}.

        /**
         * Constructor, taking the value and formatting parameters.
         *
         * @param value         The value to write.
         * @param overrideWidth Defaults to \c 0 which
         *                      denotes to choose the value of field
         *                      \alib{strings,NumberFormatBase::OctFieldWidth,NumberFormat::OctFieldWidth}.
         * @param numberFormat  The number format to use. Defaults to \c nullptr which chooses
         *                      the static singleton found in
         *                      \alib{strings,NumberFormatBase::Computational,NumberFormat::Computational}.
         * @tparam TIntegral    Value type which has to be statically castable to \c std::uint64_t.
         */
        template<typename TIntegral>
        Oct( TIntegral                 value,
             int                        overrideWidth= 0,
             NumberFormatBase<TChar>*   numberFormat = nullptr        )
        : theValue  (static_cast<uint64_t>(value))
        , theWidth  (overrideWidth)
        , nf        (numberFormat)                  {}

        /**
         * Constructor, taking the value and a just an object of type \b %NumberFormat.
         *
         * @param value         The value to write.
         * @param numberFormat  The number format to use. Defaults to \c nullptr which chooses
         *                      the static singleton found in
         *                      \alib{strings,NumberFormatBase::Computational,NumberFormat::Computational}.
         * @tparam TIntegral    Value type which has to be statically castable to \c std::uint64_t.
         */
        template<typename TIntegral>
        Oct( TIntegral                  value,
             NumberFormatBase<TChar>*   numberFormat                  )
        : theValue  (static_cast<uint64_t>(value))
        , theWidth  (0)
        , nf        (numberFormat)                  {}
    };

    // #############################################################################################
    // Fields (class Format)
    // #############################################################################################

    int             valueType;      ///< Flag witch value to use (1= sInt, 2=uInt, 3=fp )
    /// The union to hold the data provided with the different constructors.
    union
    {
         int64_t    sInt;       ///< The value when using constructor with signed integer types.
        uint64_t    uInt;       ///< The value when using constructor with unsigned integer types.
        double      fp;         ///< The value when using constructor with type double.
    } v; ///< The data

    int             theWidth;       ///< The minimum width of the number to write.
                                    ///< Defaults to \c 0
                                    ///< which denotes to choose the value of field
                                    ///< \alib{strings,NumberFormatBase::DecMinimumFieldWidth,NumberFormat::DecMinimumFieldWidth}.
    NumberFormatBase<TChar>* nf;    ///< The number format to use. Defaults to \c nullptr which chooses
                                    ///< the static singleton found in
                                    ///< \alib{strings,NumberFormatBase::Computational,NumberFormat::Computational}.

    /**
     * Constructor. Stores parameters.
     *
     * @param value         The value to write.
     * @param overrideWidth Defaults to \c 0 which
     *                      denotes to choose the value of field
     *                      \alib{strings,NumberFormatBase::DecMinimumFieldWidth,NumberFormat::DecMinimumFieldWidth}.
     * @param numberFormat  The number format to use. Defaults to \c nullptr which chooses
     *                      the static singleton found in
     *                      \alib{strings,NumberFormatBase::Computational,NumberFormat::Computational}.
     */
    FormatBase( int64_t                    value,
                int                        overrideWidth= 0,
                NumberFormatBase<TChar>*   numberFormat = nullptr        )
    : valueType(1)
    , theWidth (overrideWidth)
    , nf       (numberFormat)                  { v.sInt= static_cast<int64_t>(value); }

    /**
     * Constructor. Stores parameters.
     *
     * @param value         The value to write.
     * @param numberFormat  The number format to use. Defaults to \c nullptr which chooses
     *                      the static singleton found in
     *                      \alib{strings,NumberFormatBase::Computational,NumberFormat::Computational}.
     */
    FormatBase( int64_t                    value,
            NumberFormatBase<TChar>*   numberFormat = nullptr        )
    : valueType(1)
    , theWidth (0)
    , nf       (numberFormat)                  { v.sInt= static_cast<int64_t>(value); }

    /**
     * Constructor. Stores parameters.
     *
     * @param value         The value to write.
     * @param overrideWidth Defaults to \c 0 which
     *                      denotes to choose the value of field
     *                      \alib{strings,NumberFormatBase::DecMinimumFieldWidth,NumberFormat::DecMinimumFieldWidth}
     * @param numberFormat  The number format to use. Defaults to \c nullptr which chooses
     *                      the static singleton found in
     *                      \alib{strings,NumberFormatBase::Computational,NumberFormat::Computational}.
     */
    FormatBase( uint64_t                   value,
            int                        overrideWidth= 0,
            NumberFormatBase<TChar>*   numberFormat = nullptr        )
    : valueType(2)
    , theWidth (overrideWidth)
    , nf       (numberFormat)                  { v.uInt= static_cast<uint64_t>(value); }

    /**
     * Constructor. Stores parameters.
     *
     * @param value         The value to write.
     * @param numberFormat  The number format to use. Defaults to \c nullptr which chooses
     *                      the static singleton found in
     *                      \alib{strings,NumberFormatBase::Computational,NumberFormat::Computational}.
     */
    FormatBase( uint64_t                   value,
            NumberFormatBase<TChar>*   numberFormat = nullptr        )
    : valueType(2)
    , theWidth (0)
    , nf       (numberFormat)                  { v.uInt= static_cast<uint64_t>(value); }

    /**
     * Constructor. Stores parameters.
     *
     * @param value         The value to write.
     * @param overrideWidth Defaults to \c 0 which
     *                      denotes to choose the value of field
     *                      \alib{strings,NumberFormatBase::DecMinimumFieldWidth,NumberFormat::DecMinimumFieldWidth}.
     * @param numberFormat  The number format to use. Defaults to \c nullptr which chooses
     *                      the static singleton found in
     *                      \alib{strings,NumberFormatBase::Computational,NumberFormat::Computational}.
     */
    FormatBase( int32_t                    value,
            int                        overrideWidth= 0,
            NumberFormatBase<TChar>*   numberFormat = nullptr        )
    : valueType(1)
    , theWidth (overrideWidth)
    , nf       (numberFormat)                  { v.sInt= static_cast<int64_t>(value); }

    /**
     * Constructor. Stores parameters.
     *
     * @param value         The value to write.
     * @param numberFormat  The number format to use. Defaults to \c nullptr which chooses
     *                      the static singleton found in
     *                      \alib{strings,NumberFormatBase::Computational,NumberFormat::Computational}.
     */
    FormatBase( int32_t                    value,
            NumberFormatBase<TChar>*   numberFormat = nullptr        )
    : valueType(1)
    , theWidth(0)
    , nf       (numberFormat)                  { v.sInt= static_cast<int64_t>(value); }

    /**
     * Constructor. Stores parameters.
     *
     * @param value         The value to write.
     * @param overrideWidth Defaults to \c 0 which
     *                      denotes to choose the value of field
     *                      \alib{strings,NumberFormatBase::DecMinimumFieldWidth,NumberFormat::DecMinimumFieldWidth}.
     * @param numberFormat  The number format to use. Defaults to \c nullptr which chooses
     *                      the static singleton found in
     *                      \alib{strings,NumberFormatBase::Computational,NumberFormat::Computational}.
     */
    FormatBase( uint32_t                   value,
            int                        overrideWidth= 0,
            NumberFormatBase<TChar>*   numberFormat = nullptr        )
    : valueType(2)
    , theWidth (overrideWidth)
    , nf       (numberFormat)                  { v.uInt= static_cast<uint64_t>(value); }

    /**
     * Constructor. Stores parameters.
     *
     * @param value         The value to write.
     * @param numberFormat  The number format to use. Defaults to \c nullptr which chooses
     *                      the static singleton found in
     *                      \alib{strings,NumberFormatBase::Computational,NumberFormat::Computational}.
     */
    FormatBase( uint32_t                   value,
            NumberFormatBase<TChar>*   numberFormat = nullptr        )
    : valueType(2)
    , theWidth (0)
    , nf       (numberFormat)                  { v.uInt= static_cast<uint64_t>(value); }


    /**
     * Constructor. Stores parameters.
     *
     * @param value         The value to write.
     * @param overrideWidth Defaults to \c 0 which
     *                      denotes to choose the value of field
     *                      \alib{strings,NumberFormatBase::DecMinimumFieldWidth,NumberFormat::DecMinimumFieldWidth}.
     * @param numberFormat  The number format to use. Defaults to \c nullptr which chooses
     *                      the static singleton found in
     *                      \alib{strings,NumberFormatBase::Computational,NumberFormat::Computational}.
     */
    FormatBase( intGap_t                   value,
            int                        overrideWidth= 0,
            NumberFormatBase<TChar>*   numberFormat = nullptr        )
    : valueType(1)
    , theWidth (overrideWidth)
    , nf       (numberFormat)                  { v.sInt= static_cast<int64_t>(value); }

    /**
     * Constructor. Stores parameters.
     *
     * @param value         The value to write.
     * @param numberFormat  The number format to use. Defaults to \c nullptr which chooses
     *                      the static singleton found in
     *                      \alib{strings,NumberFormatBase::Computational,NumberFormat::Computational}.
     */
    FormatBase( intGap_t                   value,
            NumberFormatBase<TChar>*   numberFormat = nullptr        )
    : valueType(1)
    , theWidth(0)
    , nf       (numberFormat)                  { v.sInt= static_cast<int64_t>(value); }

    /**
     * Constructor. Stores parameters.
     *
     * @param value         The value to write.
     * @param overrideWidth Defaults to \c 0 which
     *                      denotes to choose the value of field
     *                      \alib{strings,NumberFormatBase::DecMinimumFieldWidth,NumberFormat::DecMinimumFieldWidth}.
     * @param numberFormat  The number format to use. Defaults to \c nullptr which chooses
     *                      the static singleton found in
     *                      \alib{strings,NumberFormatBase::Computational,NumberFormat::Computational}.
     */
    FormatBase( uintGap_t                  value,
            int                        overrideWidth= 0,
            NumberFormatBase<TChar>*   numberFormat = nullptr        )
    : valueType(2)
    , theWidth (overrideWidth)
    , nf       (numberFormat)                  { v.uInt= static_cast<uint64_t>(value); }

    /**
     * Constructor. Stores parameters.
     *
     * @param value         The value to write.
     * @param numberFormat  The number format to use. Defaults to \c nullptr which chooses
     *                      the static singleton found in
     *                      \alib{strings,NumberFormatBase::Computational,NumberFormat::Computational}.
     */
    FormatBase( uintGap_t                  value,
            NumberFormatBase<TChar>*   numberFormat = nullptr        )
    : valueType(2)
    , theWidth(0)
    , nf       (numberFormat)                  { v.uInt= static_cast<uint64_t>(value); }


    /**
     * Constructor. Stores parameters.
     *
     * @param value         The value to write.
     * @param overrideWidth Defaults to \c 0 which
     *                      denotes to choose the value of field
     *                      \alib{strings,NumberFormatBase::DecMinimumFieldWidth,NumberFormat::DecMinimumFieldWidth}.
     * @param numberFormat  The number format to use. Defaults to \c nullptr which chooses
     *                      the static singleton found in
     *                      \alib{strings,NumberFormatBase::Computational,NumberFormat::Computational}.
     */
    FormatBase( int16_t                    value,
            int                        overrideWidth= 0,
            NumberFormatBase<TChar>*   numberFormat = nullptr        )
    : valueType(1)
    , theWidth (overrideWidth)
    , nf       (numberFormat)                  { v.sInt= static_cast<int64_t>(value); }

    /**
     * Constructor. Stores parameters.
     *
     * @param value         The value to write.
     * @param numberFormat  The number format to use. Defaults to \c nullptr which chooses
     *                      the static singleton found in
     *                      \alib{strings,NumberFormatBase::Computational,NumberFormat::Computational}.
     */
    FormatBase( int16_t                    value,
            NumberFormatBase<TChar>*   numberFormat = nullptr        )
    : valueType(1)
    , theWidth(0)
    , nf       (numberFormat)                  { v.sInt= static_cast<int64_t>(value); }

    /**
     * Constructor. Stores parameters.
     *
     * @param value         The value to write.
     * @param overrideWidth Defaults to \c 0 which
     *                      denotes to choose the value of field
     *                      \alib{strings,NumberFormatBase::DecMinimumFieldWidth,NumberFormat::DecMinimumFieldWidth}.
     * @param numberFormat  The number format to use. Defaults to \c nullptr which chooses
     *                      the static singleton found in
     *                      \alib{strings,NumberFormatBase::Computational,NumberFormat::Computational}.
     */
    FormatBase( uint16_t                   value,
            int                        overrideWidth= 0,
            NumberFormatBase<TChar>*   numberFormat = nullptr        )
    : valueType(2)
    , theWidth (overrideWidth)
    , nf       (numberFormat)                  { v.uInt= static_cast<uint64_t>(value); }

    /**
     * Constructor. Stores parameters.
     *
     * @param value         The value to write.
     * @param numberFormat  The number format to use. Defaults to \c nullptr which chooses
     *                      the static singleton found in
     *                      \alib{strings,NumberFormatBase::Computational,NumberFormat::Computational}.
     */
    FormatBase( uint16_t       value,
            NumberFormat*  numberFormat                       )
    : valueType(2)
    , theWidth(0)
    , nf       (numberFormat)                  { v.uInt= static_cast<uint64_t>(value); }


    /**
     * Constructor. Stores parameters.
     *
     * @param value         The value to write.
     * @param overrideWidth Defaults to \c 0 which
     *                      denotes to choose the value of field
     *                      \alib{strings,NumberFormatBase::DecMinimumFieldWidth,NumberFormat::DecMinimumFieldWidth}.
     * @param numberFormat  The number format to use. Defaults to \c nullptr which chooses
     *                      the static singleton found in
     *                      \alib{strings,NumberFormatBase::Computational,NumberFormat::Computational}.
     */
    FormatBase( double                      value,
            int                         overrideWidth= 0,
            NumberFormatBase<TChar>*    numberFormat = nullptr        )
    : valueType(3)
    , theWidth (overrideWidth)
    , nf       (numberFormat)                  { v.fp= value; }

    /**
     * Constructor, taking a double value and a just an object of type \b %NumberFormat.
     *
     * @param value         The value to write.
     * @param numberFormat  The number format to use. Defaults to \c nullptr which chooses
     *                      the static singleton found in
     *                      \alib{strings,NumberFormatBase::Computational,NumberFormat::Computational}
     */
    FormatBase( double                      value,
            NumberFormatBase<TChar>*    numberFormat = nullptr        )
    : valueType(3)
    , theWidth (0)
    , nf       (numberFormat)                  { v.fp= value; }


}; // class format



// #################################################################################################
// Corresponding specializations of struct  T_Apply
// #################################################################################################

// We are faking all template specializations of namespace strings for doxygen into namespace
// strings::applyto to keep the documentation of namespace string clean!
#if defined(DOX_PARSER)
    namespace applyto {
#else
    ALIB_WARNINGS_ALLOW_TEMPLATE_META_PROGRAMMING
#endif


/// Specialization of template struct \alib{strings,T_Apply} for type \c Format.
template<typename TChar> struct T_Apply<FormatBase<TChar> ,TChar> : public std::true_type
{
    /** ********************************************************************************************
     * Applies type
     * \alib{strings,FormatBase,Format}.
     *
     * @param target The object that \b Apply was invoked on.
     * @param fmt    The format object.
     * @return The number of characters appended to target.
     **********************************************************************************************/
    ALIB_API static integer Apply( AStringBase<TChar>& target,  const FormatBase<TChar>& fmt );
};

/// Specialization of template struct \alib{strings,T_Apply} for type \c Format::Tab.
template<typename TChar> struct T_Apply<typename FormatBase<TChar>::Tab ,TChar> : public std::true_type
{
    /** ********************************************************************************************
     * Applies type
     * \alib{strings,FormatBase::Tab,Format::Tab}.
     *
     * @param  target   The object that \b Apply was invoked on.
     * @param  tab      The object to \e apply.
     * @return The number of characters appended to target.
     **********************************************************************************************/
    ALIB_API static   integer Apply( AStringBase<TChar>& target, const typename FormatBase<TChar>::Tab& tab);
};

/// Specialization of template struct \alib{strings,T_Apply} for type \c Format::Field.
template<typename TChar> struct T_Apply<typename FormatBase<TChar>::Field ,TChar> : public std::true_type
{
    /** ********************************************************************************************
     * Applies type
     * \alib{strings,FormatBase::Field,Format::Field}.
     *
     * @param target The object that \b Apply was invoked on.
     * @param field  The object to \e apply.
     * @return The number of characters appended to target.
     **********************************************************************************************/
    ALIB_API static   integer Apply( AStringBase<TChar>& target, const typename FormatBase<TChar>::Field& field);
};

/// Specialization of template struct \alib{strings,T_Apply} for type \c Format::Escape.
template<typename TChar> struct T_Apply<typename FormatBase<TChar>::Escape ,TChar> : public std::true_type
{
    /** ********************************************************************************************
     * Applies type
     * \alib{strings,FormatBase::Escape,Format::Escape}.
     *
     * @param target The object that \b Apply was invoked on.
     * @param field  The object to \e apply.
     * @return The number of characters appended to target.
     **********************************************************************************************/
    ALIB_API static   integer Apply( AStringBase<TChar>& target, const typename FormatBase<TChar>::Escape& field);
};

/// Specialization of template struct \alib{strings,T_Apply} for type \c int8_t.
template<typename TChar> struct T_Apply<int8_t ,TChar> : public std::true_type
{
    /**
     * Applies type \c int8_t. Creates an object of type
     * \alib{strings,FormatBase,Format} with default values
     * and passes this to the corresponding \alib{strings,T_Apply}\<Format\> specialization.
     *
     * @param target The object that \b Apply was invoked on.
     * @param value  The value to write.
     * @return The number of characters appended to target.
     */
    static inline   integer Apply( AStringBase<TChar>& target,  int8_t& value )
    {
        return T_Apply<FormatBase<TChar>,TChar>::Apply( target,     FormatBase<TChar>( static_cast<int64_t >(value), nullptr ) );
    }
};

/// Specialization of template struct \alib{strings,T_Apply} for type \c uint8_t.
template<typename TChar> struct T_Apply<uint8_t ,TChar> : public std::true_type
{
    /**
     * Applies type \c uint8_t. Creates an object of type
     * \alib{strings,FormatBase,Format} with default values
     * and passes this to the corresponding \alib{strings,T_Apply}\<Format\> specialization.
     *
     * @param target The object that \b Apply was invoked on.
     * @param value  The value to write.
     * @return The number of characters appended to target.
     */
    static inline   integer Apply( AStringBase<TChar>& target,  uint8_t& value )
    {
        return T_Apply<FormatBase<TChar>,TChar>::Apply( target,     FormatBase<TChar>( static_cast<uint64_t >(value), nullptr ) );
    }
};

/// Specialization of template struct \alib{strings,T_Apply} for type \c int16_t.
template<typename TChar> struct T_Apply<int16_t ,TChar> : public std::true_type
{
    /**
     * Applies type \c int16_t. Creates an object of type
     * \alib{strings,FormatBase,Format} with default values
     * and passes this to the corresponding \alib{strings,T_Apply}\<Format\> specialization.
     *
     * @param target The object that \b Apply was invoked on.

     * @param value  The value to write.
     * @return The number of characters appended to target.
     */
    static inline   integer Apply( AStringBase<TChar>& target,  int16_t& value )
    {
        return T_Apply<FormatBase<TChar>,TChar>::Apply( target,     FormatBase<TChar>( value, nullptr ) );
    }
};

/// Specialization of template struct \alib{strings,T_Apply} for type \c uint16_t.
template<typename TChar> struct T_Apply<uint16_t ,TChar> : public std::true_type
{
    /**
     * Applies type \c uint16_t. Creates an object of type
     * \alib{strings,FormatBase,Format} with default values
     * and passes this to the corresponding \alib{strings,T_Apply}\<Format\> specialization.
     *
     * @param target The object that \b Apply was invoked on.
     * @param value  The value to write.
     * @return The number of characters appended to target.
     */
    static inline   integer Apply( AStringBase<TChar>& target,  uint16_t& value )
    {
        return T_Apply<FormatBase<TChar>,TChar>::Apply( target,     FormatBase<TChar>( value, nullptr ) );
    }
};

/// Specialization of template struct \alib{strings,T_Apply} for type \c int32_t.
template<typename TChar> struct T_Apply<int32_t ,TChar> : public std::true_type
{
    /**
     * Applies type \c int32_t. Creates an object of type
     * \alib{strings,FormatBase,Format} with default values
     * and passes this to the corresponding \alib{strings,T_Apply}\<Format\> specialization.
     *
     * @param target The object that \b Apply was invoked on.
     * @param value  The value to write.
     * @return The number of characters appended to target.
     */
    static inline   integer Apply( AStringBase<TChar>& target,  int32_t& value )
    {
        return T_Apply<FormatBase<TChar>,TChar>::Apply( target,     FormatBase<TChar>( value, nullptr ) );
    }
};

/// Specialization of template struct \alib{strings,T_Apply} for type \c uint32_t.
template<typename TChar> struct T_Apply<uint32_t ,TChar> : public std::true_type
{
    /**
     * Applies type \c uint32_t. Creates an object of type
     * \alib{strings,FormatBase,Format} with default values
     * and passes this to the corresponding \alib{strings,T_Apply}\<Format\> specialization.
     *
     * @param target The object that \b Apply was invoked on.
     * @param value  The value to write.
     * @return The number of characters appended to target.
     */
    static inline   integer Apply( AStringBase<TChar>& target,  uint32_t& value )
    {
        return T_Apply<FormatBase<TChar>,TChar>::Apply( target,     FormatBase<TChar>( value, nullptr ) );
    }
};


/// Specialization of template struct \alib{strings,T_Apply} for type \c int64_t.
template<typename TChar> struct T_Apply<int64_t ,TChar> : public std::true_type
{
    /**
     * Applies type \c int64_t. Creates an object of type
     * \alib{strings,FormatBase,Format} with default values
     * and passes this to the corresponding \alib{strings,T_Apply}\<Format\> specialization.
     *
     * @param target The object that \b Apply was invoked on.
     * @param value  The value to write.
     * @return The number of characters appended to target.
     */
    static inline   integer Apply( AStringBase<TChar>& target,  int64_t& value )
    {
        return T_Apply<FormatBase<TChar>,TChar>::Apply( target,     FormatBase<TChar>( value, nullptr ) );
    }
};

/// Specialization of template struct \alib{strings,T_Apply} for type \c uint64_t.
template<typename TChar> struct T_Apply<uint64_t ,TChar> : public std::true_type
{
    /**
     * Applies type \c uint64_t. Creates an object of type
     * \alib{strings,FormatBase,Format} with default values
     * and passes this to the corresponding \alib{strings,T_Apply}\<Format\> specialization.
     *
     * @param target The object that \b Apply was invoked on.
     * @param value  The value to write.
     * @return The number of characters appended to target.
     */
    static inline   integer Apply( AStringBase<TChar>& target,  uint64_t& value )
    {
        return T_Apply<FormatBase<TChar>,TChar>::Apply( target,     FormatBase<TChar>( value, nullptr ) );
    }
};

/// Specialization of template struct \alib{strings,T_Apply} for type \c intGap_t.
template<typename TChar> struct T_Apply<intGap_t ,TChar> : public std::true_type
{
    /**
     * Applies type  #aworx::intGap_t. Creates an object of type
     * \alib{strings,FormatBase,Format} with default values
     * and passes this to the corresponding \alib{strings,T_Apply}\<Format\> specialization.
     *
     * @param target The object that \b Apply was invoked on.
     * @param value  The value to write.
     * @return The number of characters appended to target.
     */
    static inline   integer Apply( AStringBase<TChar>& target,  intGap_t& value )
    {
        return T_Apply<FormatBase<TChar>,TChar>::Apply( target,     FormatBase<TChar>( value, nullptr ) );
    }
};


/// Specialization of template struct \alib{strings,T_Apply} for type \c uintGap_t.
template<typename TChar> struct T_Apply<uintGap_t ,TChar> : public std::true_type
{
    /**
     * Applies type #aworx::uintGap_t. Creates an object of type
     * \alib{strings,FormatBase,Format} with default values
     * and passes this to the corresponding \alib{strings,T_Apply}\<Format\> specialization.
     *
     * @param target The object that \b Apply was invoked on.
     * @param value  The value to write.
     * @return The number of characters appended to target.
     */
    static inline   integer Apply( AStringBase<TChar>& target,  uintGap_t& value )
    {
        return T_Apply<FormatBase<TChar>,TChar>::Apply( target,     FormatBase<TChar>( value, nullptr ) );
    }
};

/// Specialization of template struct \alib{strings,T_Apply} for type \c float.
template<typename TChar> struct T_Apply<float ,TChar> : public std::true_type
{
    /**
     * Applies type \c float. Creates an object of type
     * \alib{strings,FormatBase,Format} with default values
     * and passes this to the corresponding \alib{strings,T_Apply}\<Format\> specialization.
     *
     * @param target The object that \b Apply was invoked on.
     * @param value  The value to write.
     * @return The number of characters appended to target.
     */
    static inline integer Apply( AStringBase<TChar>& target, float& value)
    {
        return T_Apply<FormatBase<TChar>,TChar>::Apply( target, FormatBase<TChar>( static_cast<double>(value), nullptr ) );
    }
};

/// Specialization of template struct \alib{strings,T_Apply} for type \c double.
template<typename TChar> struct T_Apply<double ,TChar> : public std::true_type
{
    /**
     * Applies type \c double. Creates an object of type
     * \alib{strings,FormatBase,Format} with default values
     * and passes this to the corresponding \alib{strings,T_Apply}\<Format\> specialization.
     *
     * @param target The object that \b Apply was invoked on.
     * @param value  The value to write.
     * @return The number of characters appended to target.
     */
    static inline integer Apply( AStringBase<TChar>& target, double& value)
    {
        return T_Apply<FormatBase<TChar>,TChar>::Apply( target,  FormatBase<TChar>( value, nullptr ) );
    }
};

/// Specialization of template struct \alib{strings,T_Apply} for type \c Format::Bin.
template<typename TChar> struct T_Apply<typename FormatBase<TChar>::Bin ,TChar> : public std::true_type
{
    /** ********************************************************************************************
     * Applies objects of type
     * \alib{strings,FormatBase::Bin,Format::Bin}.
     *
     * @param target The object that \b Apply was invoked on.
     * @param fmt    The format object.
     * @return The number of characters appended to target.
     **********************************************************************************************/
    ALIB_API static integer Apply( AStringBase<TChar>& target,  const typename FormatBase<TChar>::Bin& fmt );
};

/// Specialization of template struct \alib{strings,T_Apply} for type \c Format::Hex.
template<typename TChar> struct T_Apply<typename FormatBase<TChar>::Hex ,TChar> : public std::true_type
{
    /** ********************************************************************************************
     * Applies objects of type
     * \alib{strings,FormatBase::Hex,Format::Hex}.
     *
     * @param target The object that \b Apply was invoked on.
     * @param fmt    The format object.
     * @return The number of characters appended to target.
     **********************************************************************************************/
    ALIB_API static integer Apply( AStringBase<TChar>& target,  const typename FormatBase<TChar>::Hex& fmt );
};

/// Specialization of template struct \alib{strings,T_Apply} for type \c Format::Oct.
template<typename TChar> struct T_Apply<typename FormatBase<TChar>::Oct ,TChar> : public std::true_type
{
    /** ********************************************************************************************
     * Applies objects of type
     * \alib{strings,FormatBase::Oct,Format::Oct}.
     *
     * @param target The object that \b Apply was invoked on.
     * @param fmt    The format object.
     * @return The number of characters appended to target.
     **********************************************************************************************/
    ALIB_API static integer Apply( AStringBase<TChar>& target,  const typename FormatBase<TChar>::Oct& fmt );
};

#if !defined(DOX_PARSER)
    ALIB_WARNINGS_RESTORE
#endif

// We are faking all template specializations of namespace strings for doxygen into namespace
// strings::apply to keep the documentation of namespace string clean!
#if defined(DOX_PARSER)
}
#endif

}}} // namespace [aworx::lib::strings]

#endif // HPP_ALIB_STRINGS_BUILTIN_APPLICABLES
