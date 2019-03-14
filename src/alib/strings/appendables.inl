// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#if !defined(HPP_ALIB_ASTRING_PROPPERINCLUDE)
#   error "ALib sources with ending '.inl' must not be included from outside."
#endif

namespace aworx { namespace lib { namespace strings {

// #################################################################################################
// struct T_Append: fundamental types
// #################################################################################################

// We are faking all template specializations of namespace strings for doxygen into namespace
// strings::APPENDABLES to keep the documentation of namespace string clean!
#if ALIB_DOCUMENTATION_PARSER
namespace APPENDABLES {
#endif

/** Specialization of functor \alib{strings,T_Append} for type \c bool. */
template<typename TChar> struct       T_Append<bool ,TChar>
{
    /** ********************************************************************************************
     * Writes the words "true" or "false" to the given AString.
     * @param target The \b AString that \b Append was invoked on.
     * @param b      The boolean to write to \p{target}.
     **********************************************************************************************/
    void operator()( TAString<TChar>& target, bool b );
};
extern template ALIB_API void T_Append<bool , nchar>::operator()( TAString<nchar>&, bool  );
extern template ALIB_API void T_Append<bool , wchar>::operator()( TAString<wchar>&, bool  );
extern template ALIB_API void T_Append<bool , xchar>::operator()( TAString<xchar>&, bool  );



#if ALIB_DEBUG
/** Specialization of functor \alib{strings,T_Append} for type \c std::type_info. */
template<typename TChar> struct       T_Append<std::type_info ,TChar>
{
    /** ********************************************************************************************
     * Writes the demangled type name.
     * @param target The \b AString that \b Append was invoked on.
     * @param type   The type to write to \p{target}.
     **********************************************************************************************/
    void operator()( TAString<TChar>& target, const std::type_info& type );
};
extern template ALIB_API void T_Append<std::type_info, nchar>::operator()( TAString<nchar>&, const std::type_info&  );
extern template ALIB_API void T_Append<std::type_info, wchar>::operator()( TAString<wchar>&, const std::type_info&  );
extern template ALIB_API void T_Append<std::type_info, xchar>::operator()( TAString<xchar>&, const std::type_info&  );

#endif


// We are faking all template specializations of namespace strings for doxygen into namespace
// strings::APPENDABLES to keep the documentation of namespace string clean!
#if ALIB_DOCUMENTATION_PARSER
}
#endif


// #################################################################################################
// class Format
// #################################################################################################

/** ************************************************************************************************
 * This is a type purely made to be \ref alib_strings_assembly_ttostring "appended" to objects of
 * type \alib{strings,TAString,AString}.
 * Various constructors accept integer and floating point values, along with formatting options.
 * The specialization of functor \alib{strings,T_Append} will use a given (or defaulted) instance
 * of class \alib{strings,TNumberFormat,NumberFormat} to format the encapsulated value and
 * append the result to the \b %AString in question.
 *
 * \note
 *   Within the same header file that this class in declared in, there are several
 *   specializations of functor \alib{strings,T_Append} defined for plain integer and
 *   floating point types. These specializations create an object of this type, providing the
 *   value only, hence, using this classes constructor's default values. The number format
 *   used as default by the constructors of this class is
 *   \alib{strings,TNumberFormat::Computational,NumberFormat::Computational}.
 *   As a result, the application of such core types, as in:
 *   \snippet "DOX_ALIB_ASTRING_APPEND.cpp"     DOX_ALIB_APPEND_FORMAT1
 *   which produces:
 *   \snippet "DOX_ALIB_APPEND_FORMAT1.txt"     OUTPUT
 *
 * \note
 *   does \b not use a locale specific number format. Instead it uses one that is exchangeable
 *   between applications independent from the locale setting of the executing machine.<br>
 *   Consequently, for locale specific output, an object of this class needs to be appended
 *   along with a locale enabled instance of \b %NumberFormat. For example:
 *   \snippet "DOX_ALIB_ASTRING_APPEND.cpp"     DOX_ALIB_APPEND_FORMAT2
 *   which - dependent on the current local setting - might produce:
 *   \snippet "DOX_ALIB_APPEND_FORMAT2.txt"     OUTPUT
 *
 *
 * <b>Inner Types:</b><br>
 * Besides accepting plain number types, this class in addition aggregates several public inner
 * types, namely
 * - \alib{strings,TFormat::Tab,Format::Tab},
 * - \alib{strings,TFormat::Field,Format::Field},
 * - \alib{strings,TFormat::Escape,Format::Escape},
 * - \alib{strings,TFormat::Bin,Format::Bin},
 * - \alib{strings,TFormat::Hex,Format::Hex} and
 * - \alib{strings,TFormat::Oct,Format::Oct}.
 *
 * Each of these classes provide certain formatting options, which are implemented by a
 * corresponding specialization of functor \alib{strings,T_Append}.
 *
 * \note The types are not 'physically' related to this class. Instead, they have been
 *       aggregated here as a design decision.
 *
 * <b>Details on Formats:</b><br>
 * Details on the options of formatting integer and floating point numbers are documented
 * with class
 * \alib{strings,TNumberFormat,NumberFormat}.
 *
 * @tparam TChar The character type.<br>
 *   Alias names for specializations of this class using character types
 *   \alib{characters,character}, \alib{characters,nchar}, \alib{characters,wchar},
 *   \alib{characters,xchar}, \alib{characters,complementChar} and \alib{characters,strangeChar}
 *   are provided in namespace #aworx with type definitions \aworx{Format}, \aworx{NFormat},
 *   \aworx{WFormat}, \aworx{XFormat}, \aworx{ComplementFormat} and \aworx{StrangeFormat}.
 **************************************************************************************************/
template<typename TChar>
class TFormat
{
    public:

    // #############################################################################################
    // Inner types
    // #############################################################################################

    /** ********************************************************************************************
     * Implements a temporary object which is \ref alib_strings_assembly_ttostring "appended"
     * to instances of type \alib{strings,TAString,AString}.
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
        /** The tab positions are multiples of this value. */
        integer     tabSize;

        /** The reference length of the AString which is taken as relative tab position
         *  (instead of the beginning of the string). If -1, the target %AString is
         *  searched backwards for the last newline and this position is used as the
         *  reference.
         */
        integer     reference;

        /** The minimum pad characters to add. Defaults to 1. */
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
     * Used to create temporary objects which are \ref alib_strings_assembly_ttostring "appended"
     * to \alib{strings,TAString,AString} instances.<br>
     *
     * Appends the given object to the AString using a defined 'field'-width.
     * If the contents of the field is shorter than parameter \p{width} specifies, the field is
     * filled with a corresponding amount of \p{padChar} characters.<br>
     * Parameter \p{alignment} of type
     * \ref aworx::lib::Alignment "Alignment" allows to left-, right- or center-align
     * the contents of the field.
     *
     * \note
     *   In case, module \alibmod_boxing is not available, the field content
     *   parameter will be of type <c>const String&</c>.<br>
     *   Otherwise, box-function \alib{boxing,FAppend} will be invoked on
     *   the given box internally to receive the string representation.
     *
     * \note
     *   Therefore, it is mandatory, that for any type that is used with this class
     *   to be formatted in a field, this box-function has to be implemented. As
     *   documented with that interface, for types that are
     *   \ref alib_strings_assembly_ttostring "appendable" to \b %AString
     *   objects already, all that is needed is to use macro
     *   \ref ALIB_BOXING_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE with the type in the bootstrap
     *   section of an application.
     **********************************************************************************************/
    struct Field
    {
        public:
        #if ALIB_MODULE_BOXING  || ALIB_DOCUMENTATION_PARSER
        Box                     theContent;   ///< The content of the field. If module
                                              ///< \alibmod_nolink_boxing  is not available, this field
                                              ///< is of type <c>const TString<TChar>&</c>
        #else
        const TString<TChar>&   theContent;
        #endif
        integer                 theWidth;     ///< The width of the field.
        Alignment               theAlignment; ///< The alignment of the contents within the field.
        TChar                   padChar;      ///< The characters used for padding the contents within the field.

        /**
         * Constructor. Copies the parameters.
         *
         * @param content   The contents of the field. If module \alibmod_boxing  is not
         *                  available, this field is of type <c>const TString<TChar>&</c>,
         *                  otherwise of type \alib{boxing,Box}.
         * @param width     The width of the field
         * @param alignment The alignment of the contents within the field.
         *                  Defaults to
         *                  \ref aworx::lib::Alignment "Alignment::Right"
         *                  Other options are
         *                  \ref aworx::lib::Alignment "Alignment::Left" and
         *                  \ref aworx::lib::Alignment "Alignment::Center".
         * @param fillChar  The character used to fill the field up to its size.
         *                  Defaults to ' ' (space).
         */
        Field(
                #if ALIB_MODULE_BOXING
                Box              content,
                #else
                const TString<TChar>&  content,
                #endif

                integer           width,
                Alignment   alignment  =Alignment::Right,
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
     * Implements a temporary object which is \ref alib_strings_assembly_ttostring "appended"
     * to instances of type \alib{strings,TAString,AString}.
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

        /** The start of the region to convert. */
        integer      startIdx;

        /** The length of the region to convert. */
        integer      length;


        /**
         * Constructor. Copies the parameters.
         *
         * @param escape        \b Switch::On escapes ascii characters (the default),
         *                      \b Switch::Off converts  escaped strings to ascii codes.
         * @param regionStart   The start of the region to convert.
         * @param regionLength  The length of the region to convert.
         */
        Escape( Switch escape= Switch::On, integer regionStart = 0, integer regionLength =MAX_LEN )
        : pSwitch(escape), startIdx(regionStart), length(regionLength)
        {}
    };


    /** ********************************************************************************************
     * Implements a temporary object which can be  \ref alib_strings_assembly_ttostring "appended"
     * to instances of type \alib{strings,TAString,AString}.
     *
     * Appends an integer value in binary format.
     *
     * \see
     *   Class \alib{strings,TNumberFormat} for more information on formatting options for binary
     *   number output.
     **********************************************************************************************/
    struct Bin
    {
        public:
        uint64_t        theValue;   ///< The value to write.
        int             theWidth;   ///< The minimum width of the number to write.
                                    ///< Defaults to \c 0
                                    ///< which denotes to choose the value of field
                                    ///< \alib{strings,TNumberFormat::BinFieldWidth,NumberFormat::BinFieldWidth}.
        TNumberFormat<TChar>* nf;   ///< The number format to use. Defaults to \c nullptr which chooses
                                    ///< \alib{strings,TNumberFormat::Computational,NumberFormat::Computational}.

        /**
         * Constructor, taking the value and formatting parameters.
         *
         * @param value         The value to write.
         * @param overrideWidth Defaults to \c 0 which
         *                      denotes to choose the value of field
         *                      \alib{strings,TNumberFormat::BinFieldWidth,NumberFormat::BinFieldWidth}.
         * @param numberFormat  The number format to use. Defaults to \c nullptr which chooses
         *                      the static singleton found in
         *                      \alib{strings,TNumberFormat::Computational,NumberFormat::Computational}.
         * @tparam TIntegral    Value type which has to be statically castable to \c std::uint64_t.
         */
        template<typename TIntegral>
        Bin( TIntegral                 value,
             int                       overrideWidth= 0,
             TNumberFormat<TChar>*  numberFormat = nullptr        )
        : theValue  (static_cast<uint64_t>(value))
        , theWidth  (overrideWidth)
        , nf        (numberFormat)                  {}

        /**
         * Constructor, taking the value and a just an object of type \b %NumberFormat.
         *
         * @param value         The value to write.
         * @param numberFormat  The number format to use. Defaults to \c nullptr which chooses
         *                      the static singleton found in
         *                      \alib{strings,TNumberFormat::Computational,NumberFormat::Computational}.
         * @tparam TIntegral    Value type which has to be statically castable to \c std::uint64_t.
         */
        template<typename TIntegral>
        Bin( TIntegral                 value,
             TNumberFormat<TChar>*  numberFormat                  )
        : theValue  (static_cast<uint64_t>(value))
        , theWidth  (0)
        , nf        (numberFormat)                  {}

    };

    /** ********************************************************************************************
     * Implements a temporary object which is \ref alib_strings_assembly_ttostring "appended"
     * to instances of type \alib{strings,TAString,AString}.
     *
     * Appends an integer value in hexadecimal format.
     *
     * \see
     *   Class \alib{strings,TNumberFormat} for more information on formatting options for
     *   hexadecimal number output.
     **********************************************************************************************/
    struct Hex
    {
        public:
        uint64_t    theValue;       ///< The value to write.
        int         theWidth;       ///< The minimum width of the number to write.
                                    ///< Defaults to \c 0
                                    ///< which denotes to choose the value of field
                                    ///< \alib{strings,TNumberFormat::HexFieldWidth,NumberFormat::HexFieldWidth}.
        TNumberFormat<TChar>* nf;///< The number format to use. Defaults to \c nullptr which chooses
                                    ///< \alib{strings,TNumberFormat::Computational,NumberFormat::Computational}.

        /**
         * Constructor, taking the value and formatting parameters.
         *
         * @param value         The value to write.
         * @param overrideWidth Defaults to \c 0 which
         *                      denotes to choose the value of field
         *                      \alib{strings,TNumberFormat::HexFieldWidth,NumberFormat::HexFieldWidth}.
         * @param numberFormat  The number format to use. Defaults to \c nullptr which chooses
         *                      the static singleton found in
         *                      \alib{strings,TNumberFormat::Computational,NumberFormat::Computational}.
         * @tparam TIntegral    Value type which has to be statically castable to \c std::uint64_t.
         */
        template<typename TIntegral>
        Hex( TIntegral                  value,
             int                        overrideWidth= 0,
             TNumberFormat<TChar>*   numberFormat = nullptr        )
        : theValue  (static_cast<uint64_t>(value))
        , theWidth  (overrideWidth)
        , nf        (numberFormat)                  {}

        /**
         * Constructor, taking the value and a just an object of type \b %NumberFormat.
         *
         * @param value         The value to write.
         * @param numberFormat  The number format to use. Defaults to \c nullptr which chooses
         *                      the static singleton found in
         *                      \alib{strings,TNumberFormat::Computational,NumberFormat::Computational}.
         */
        template<typename TIntegral>
        Hex( TIntegral                  value,
             TNumberFormat<TChar>*   numberFormat                  )
        : theValue  (static_cast<uint64_t>(value))
        , theWidth  (0)
        , nf        (numberFormat)                  {}
    };

    /** ********************************************************************************************
     * Implements a temporary object which is \ref alib_strings_assembly_ttostring "appended"
     * to instances of type \alib{strings,TAString,AString}.
     *
     * Appends an integer value in octal format.
     *
     * \see
     *   Class \alib{strings,TNumberFormat} for more information on formatting options for octal
     *   number output.
     **********************************************************************************************/
    struct Oct
    {
        public:
        uint64_t        theValue; ///< The value to write.
        int             theWidth; ///< The minimum width of the number to write.
                                  ///< Defaults to \c 0
                                  ///< which denotes to choose the value of field
                                  ///< \alib{strings,TNumberFormat::OctFieldWidth,NumberFormat::OctFieldWidth}.
        TNumberFormat<TChar>* nf;   ///< The number format to use. Defaults to \c nullptr which chooses
                                  ///< \alib{strings,TNumberFormat::Computational,NumberFormat::Computational}.

        /**
         * Constructor, taking the value and formatting parameters.
         *
         * @param value         The value to write.
         * @param overrideWidth Defaults to \c 0 which
         *                      denotes to choose the value of field
         *                      \alib{strings,TNumberFormat::OctFieldWidth,NumberFormat::OctFieldWidth}.
         * @param numberFormat  The number format to use. Defaults to \c nullptr which chooses
         *                      the static singleton found in
         *                      \alib{strings,TNumberFormat::Computational,NumberFormat::Computational}.
         * @tparam TIntegral    Value type which has to be statically castable to \c std::uint64_t.
         */
        template<typename TIntegral>
        Oct( TIntegral                 value,
             int                        overrideWidth= 0,
             TNumberFormat<TChar>*   numberFormat = nullptr        )
        : theValue  (static_cast<uint64_t>(value))
        , theWidth  (overrideWidth)
        , nf        (numberFormat)                  {}

        /**
         * Constructor, taking the value and a just an object of type \b %NumberFormat.
         *
         * @param value         The value to write.
         * @param numberFormat  The number format to use. Defaults to \c nullptr which chooses
         *                      the static singleton found in
         *                      \alib{strings,TNumberFormat::Computational,NumberFormat::Computational}.
         * @tparam TIntegral    Value type which has to be statically castable to \c std::uint64_t.
         */
        template<typename TIntegral>
        Oct( TIntegral                  value,
             TNumberFormat<TChar>*   numberFormat                  )
        : theValue  (static_cast<uint64_t>(value))
        , theWidth  (0)
        , nf        (numberFormat)                  {}
    };

    // #############################################################################################
    // Fields (class Format)
    // #############################################################################################

    int             valueType;      ///< Flag witch value to use (1= sInt, 2=uInt, 3=fp )
    /** The union to hold the data provided with the different constructors. */
    union
    {
         int64_t    sInt;       ///< The value when using constructor with signed integer types.
        uint64_t    uInt;       ///< The value when using constructor with unsigned integer types.
        double      fp;         ///< The value when using constructor with type double.
    } v; ///< The data

    int             theWidth;       ///< The minimum width of the number to write.
                                    ///< Defaults to \c 0
                                    ///< which denotes to choose the value of field
                                    ///< \alib{strings,TNumberFormat::DecMinimumFieldWidth,NumberFormat::DecMinimumFieldWidth}.
    TNumberFormat<TChar>* nf;    ///< The number format to use. Defaults to \c nullptr which chooses
                                    ///< the static singleton found in
                                    ///< \alib{strings,TNumberFormat::Computational,NumberFormat::Computational}.

    /**
     * Constructor. Stores parameters.
     *
     * @param value         The value to write.
     * @param overrideWidth Defaults to \c 0 which
     *                      denotes to choose the value of field
     *                      \alib{strings,TNumberFormat::DecMinimumFieldWidth,NumberFormat::DecMinimumFieldWidth}.
     * @param numberFormat  The number format to use. Defaults to \c nullptr which chooses
     *                      the static singleton found in
     *                      \alib{strings,TNumberFormat::Computational,NumberFormat::Computational}.
     */
    TFormat( int64_t                     value,
                int                         overrideWidth= 0,
                TNumberFormat<TChar>*    numberFormat = nullptr        )
    : valueType(1)
    , theWidth (overrideWidth)
    , nf       (numberFormat)                  { v.sInt= static_cast<int64_t>(value); }

    /**
     * Constructor. Stores parameters.
     *
     * @param value         The value to write.
     * @param numberFormat  The number format to use. Defaults to \c nullptr which chooses
     *                      the static singleton found in
     *                      \alib{strings,TNumberFormat::Computational,NumberFormat::Computational}.
     */
    TFormat( int64_t                     value,
                TNumberFormat<TChar>*    numberFormat = nullptr        )
    : valueType(1)
    , theWidth (0)
    , nf       (numberFormat)                  { v.sInt= static_cast<int64_t>(value); }

    /**
     * Constructor. Stores parameters.
     *
     * @param value         The value to write.
     * @param overrideWidth Defaults to \c 0 which
     *                      denotes to choose the value of field
     *                      \alib{strings,TNumberFormat::DecMinimumFieldWidth,NumberFormat::DecMinimumFieldWidth}
     * @param numberFormat  The number format to use. Defaults to \c nullptr which chooses
     *                      the static singleton found in
     *                      \alib{strings,TNumberFormat::Computational,NumberFormat::Computational}.
     */
    TFormat( uint64_t                    value,
                int                         overrideWidth= 0,
                TNumberFormat<TChar>*    numberFormat = nullptr        )
    : valueType(2)
    , theWidth (overrideWidth)
    , nf       (numberFormat)                  { v.uInt= static_cast<uint64_t>(value); }

    /**
     * Constructor. Stores parameters.
     *
     * @param value         The value to write.
     * @param numberFormat  The number format to use. Defaults to \c nullptr which chooses
     *                      the static singleton found in
     *                      \alib{strings,TNumberFormat::Computational,NumberFormat::Computational}.
     */
    TFormat( uint64_t                    value,
                TNumberFormat<TChar>*    numberFormat = nullptr        )
    : valueType(2)
    , theWidth (0)
    , nf       (numberFormat)                  { v.uInt= static_cast<uint64_t>(value); }

    /**
     * Constructor. Stores parameters.
     *
     * @param value         The value to write.
     * @param overrideWidth Defaults to \c 0 which
     *                      denotes to choose the value of field
     *                      \alib{strings,TNumberFormat::DecMinimumFieldWidth,NumberFormat::DecMinimumFieldWidth}.
     * @param numberFormat  The number format to use. Defaults to \c nullptr which chooses
     *                      the static singleton found in
     *                      \alib{strings,TNumberFormat::Computational,NumberFormat::Computational}.
     */
    TFormat( int32_t                     value,
                int                         overrideWidth= 0,
                TNumberFormat<TChar>*    numberFormat = nullptr        )
    : valueType(1)
    , theWidth (overrideWidth)
    , nf       (numberFormat)                  { v.sInt= static_cast<int64_t>(value); }

    /**
     * Constructor. Stores parameters.
     *
     * @param value         The value to write.
     * @param numberFormat  The number format to use. Defaults to \c nullptr which chooses
     *                      the static singleton found in
     *                      \alib{strings,TNumberFormat::Computational,NumberFormat::Computational}.
     */
    TFormat( int32_t                     value,
                TNumberFormat<TChar>*    numberFormat = nullptr        )
    : valueType(1)
    , theWidth (0)
    , nf       (numberFormat)                  { v.sInt= static_cast<int64_t>(value); }

    /**
     * Constructor. Stores parameters.
     *
     * @param value         The value to write.
     * @param overrideWidth Defaults to \c 0 which
     *                      denotes to choose the value of field
     *                      \alib{strings,TNumberFormat::DecMinimumFieldWidth,NumberFormat::DecMinimumFieldWidth}.
     * @param numberFormat  The number format to use. Defaults to \c nullptr which chooses
     *                      the static singleton found in
     *                      \alib{strings,TNumberFormat::Computational,NumberFormat::Computational}.
     */
    TFormat( uint32_t                    value,
                int                         overrideWidth= 0,
                TNumberFormat<TChar>*    numberFormat = nullptr        )
    : valueType(2)
    , theWidth (overrideWidth)
    , nf       (numberFormat)                  { v.uInt= static_cast<uint64_t>(value); }

    /**
     * Constructor. Stores parameters.
     *
     * @param value         The value to write.
     * @param numberFormat  The number format to use. Defaults to \c nullptr which chooses
     *                      the static singleton found in
     *                      \alib{strings,TNumberFormat::Computational,NumberFormat::Computational}.
     */
    TFormat( uint32_t                    value,
                TNumberFormat<TChar>*    numberFormat = nullptr        )
    : valueType(2)
    , theWidth (0)
    , nf       (numberFormat)                  { v.uInt= static_cast<uint64_t>(value); }


    /**
     * Constructor. Stores parameters.
     *
     * @param value         The value to write.
     * @param overrideWidth Defaults to \c 0 which
     *                      denotes to choose the value of field
     *                      \alib{strings,TNumberFormat::DecMinimumFieldWidth,NumberFormat::DecMinimumFieldWidth}.
     * @param numberFormat  The number format to use. Defaults to \c nullptr which chooses
     *                      the static singleton found in
     *                      \alib{strings,TNumberFormat::Computational,NumberFormat::Computational}.
     */
    TFormat( intGap_t                    value,
                int                         overrideWidth= 0,
                TNumberFormat<TChar>*    numberFormat = nullptr        )
    : valueType(1)
    , theWidth (overrideWidth)
    , nf       (numberFormat)                  { v.sInt= static_cast<int64_t>(value); }

    /**
     * Constructor. Stores parameters.
     *
     * @param value         The value to write.
     * @param numberFormat  The number format to use. Defaults to \c nullptr which chooses
     *                      the static singleton found in
     *                      \alib{strings,TNumberFormat::Computational,NumberFormat::Computational}.
     */
    TFormat( intGap_t                    value,
                TNumberFormat<TChar>*    numberFormat = nullptr        )
    : valueType(1)
    , theWidth (0)
    , nf       (numberFormat)                  { v.sInt= static_cast<int64_t>(value); }

    /**
     * Constructor. Stores parameters.
     *
     * @param value         The value to write.
     * @param overrideWidth Defaults to \c 0 which
     *                      denotes to choose the value of field
     *                      \alib{strings,TNumberFormat::DecMinimumFieldWidth,NumberFormat::DecMinimumFieldWidth}.
     * @param numberFormat  The number format to use. Defaults to \c nullptr which chooses
     *                      the static singleton found in
     *                      \alib{strings,TNumberFormat::Computational,NumberFormat::Computational}.
     */
    TFormat( uintGap_t                   value,
                int                         overrideWidth= 0,
                TNumberFormat<TChar>*    numberFormat = nullptr        )
    : valueType(2)
    , theWidth (overrideWidth)
    , nf       (numberFormat)                  { v.uInt= static_cast<uint64_t>(value); }

    /**
     * Constructor. Stores parameters.
     *
     * @param value         The value to write.
     * @param numberFormat  The number format to use. Defaults to \c nullptr which chooses
     *                      the static singleton found in
     *                      \alib{strings,TNumberFormat::Computational,NumberFormat::Computational}.
     */
    TFormat( uintGap_t                   value,
                TNumberFormat<TChar>*    numberFormat = nullptr        )
    : valueType(2)
    , theWidth (0)
    , nf       (numberFormat)                  { v.uInt= static_cast<uint64_t>(value); }


    /**
     * Constructor. Stores parameters.
     *
     * @param value         The value to write.
     * @param overrideWidth Defaults to \c 0 which
     *                      denotes to choose the value of field
     *                      \alib{strings,TNumberFormat::DecMinimumFieldWidth,NumberFormat::DecMinimumFieldWidth}.
     * @param numberFormat  The number format to use. Defaults to \c nullptr which chooses
     *                      the static singleton found in
     *                      \alib{strings,TNumberFormat::Computational,NumberFormat::Computational}.
     */
    TFormat( int16_t                     value,
                int                         overrideWidth= 0,
                TNumberFormat<TChar>*    numberFormat = nullptr        )
    : valueType(1)
    , theWidth (overrideWidth)
    , nf       (numberFormat)                  { v.sInt= static_cast<int64_t>(value); }

    /**
     * Constructor. Stores parameters.
     *
     * @param value         The value to write.
     * @param numberFormat  The number format to use. Defaults to \c nullptr which chooses
     *                      the static singleton found in
     *                      \alib{strings,TNumberFormat::Computational,NumberFormat::Computational}.
     */
    TFormat( int16_t                     value,
                TNumberFormat<TChar>*    numberFormat = nullptr        )
    : valueType(1)
    , theWidth (0)
    , nf       (numberFormat)                  { v.sInt= static_cast<int64_t>(value); }

    /**
     * Constructor. Stores parameters.
     *
     * @param value         The value to write.
     * @param overrideWidth Defaults to \c 0 which
     *                      denotes to choose the value of field
     *                      \alib{strings,TNumberFormat::DecMinimumFieldWidth,NumberFormat::DecMinimumFieldWidth}.
     * @param numberFormat  The number format to use. Defaults to \c nullptr which chooses
     *                      the static singleton found in
     *                      \alib{strings,TNumberFormat::Computational,NumberFormat::Computational}.
     */
    TFormat( uint16_t                    value,
                int                         overrideWidth= 0,
                TNumberFormat<TChar>*    numberFormat = nullptr        )
    : valueType(2)
    , theWidth (overrideWidth)
    , nf       (numberFormat)                  { v.uInt= static_cast<uint64_t>(value); }

    /**
     * Constructor. Stores parameters.
     *
     * @param value         The value to write.
     * @param numberFormat  The number format to use. Defaults to \c nullptr which chooses
     *                      the static singleton found in
     *                      \alib{strings,TNumberFormat::Computational,NumberFormat::Computational}.
     */
    TFormat( uint16_t                    value,
                TNumberFormat<TChar>*    numberFormat                       )
    : valueType(2)
    , theWidth (0)
    , nf       (numberFormat)                  { v.uInt= static_cast<uint64_t>(value); }


    /**
     * Constructor. Stores parameters.
     *
     * @param value         The value to write.
     * @param overrideWidth Defaults to \c 0 which
     *                      denotes to choose the value of field
     *                      \alib{strings,TNumberFormat::DecMinimumFieldWidth,NumberFormat::DecMinimumFieldWidth}.
     * @param numberFormat  The number format to use. Defaults to \c nullptr which chooses
     *                      the static singleton found in
     *                      \alib{strings,TNumberFormat::Computational,NumberFormat::Computational}.
     */
    TFormat( double                      value,
                int                         overrideWidth= 0,
                TNumberFormat<TChar>*    numberFormat = nullptr        )
    : valueType(3)
    , theWidth (overrideWidth)
    , nf       (numberFormat)                  { v.fp= value; }

    /**
     * Constructor, taking a double value and a just an object of type \b %NumberFormat.
     *
     * @param value         The value to write.
     * @param numberFormat  The number format to use. Defaults to \c nullptr which chooses
     *                      the static singleton found in
     *                      \alib{strings,TNumberFormat::Computational,NumberFormat::Computational}
     */
    TFormat( double                      value,
                TNumberFormat<TChar>*    numberFormat = nullptr        )
    : valueType(3)
    , theWidth (0)
    , nf       (numberFormat)                  { v.fp= value; }


}; // class format



// #################################################################################################
// Corresponding specializations of struct  T_Append
// #################################################################################################

// We are faking all template specializations of namespace strings for doxygen into namespace
// strings::APPENDABLES to keep the documentation of namespace string clean!
#if ALIB_DOCUMENTATION_PARSER
    namespace APPENDABLES {
#endif

/** Specialization of functor \alib{strings,T_Append} for type \c Format. */
template<typename TChar> struct T_Append<TFormat<TChar> ,TChar>
{
    /** ********************************************************************************************
     * Appends a string representation of the value encapsulated in the given \b Format value.
     *
     * @param target The \b AString that \b Append was invoked on.
     * @param fmt    The format object.
     **********************************************************************************************/
    void operator()( TAString<TChar>& target,  const TFormat<TChar>& fmt );
};
extern template ALIB_API void T_Append<TFormat<nchar> , nchar>::operator()( TAString<nchar>&, const TFormat<nchar>&  );
extern template ALIB_API void T_Append<TFormat<wchar> , wchar>::operator()( TAString<wchar>&, const TFormat<wchar>&  );
extern template ALIB_API void T_Append<TFormat<xchar> , xchar>::operator()( TAString<xchar>&, const TFormat<xchar>&  );

/** Specialization of functor \alib{strings,T_Append} for type \c Format::Tab. */
template<typename TChar> struct T_Append<typename TFormat<TChar>::Tab ,TChar>
{
    /** ********************************************************************************************
     * Appends tabulator characters to the given string.
     *
     * @param  target   The \b AString that \b Append was invoked on.
     * @param  tab      The object to append.
     **********************************************************************************************/
    void operator()( TAString<TChar>& target, const typename TFormat<TChar>::Tab& tab);
};
extern template ALIB_API void T_Append<TFormat<nchar>::Tab , nchar>::operator()( TAString<nchar>&, const TFormat<nchar>::Tab&  );
extern template ALIB_API void T_Append<TFormat<wchar>::Tab , wchar>::operator()( TAString<wchar>&, const TFormat<wchar>::Tab&  );
extern template ALIB_API void T_Append<TFormat<xchar>::Tab , xchar>::operator()( TAString<xchar>&, const TFormat<xchar>::Tab&  );

/** Specialization of functor \alib{strings,T_Append} for type \c Format::Field. */
template<typename TChar> struct T_Append<typename TFormat<TChar>::Field ,TChar>
{
    /** ********************************************************************************************
     * Appends a field with the adjusted boxed content to the given string.
     *
     * @param target The \b AString that \b Append was invoked on.
     * @param field  The object to append.
     **********************************************************************************************/
    void operator()( TAString<TChar>& target, const typename TFormat<TChar>::Field& field);
};

extern template ALIB_API void T_Append<TFormat<nchar>::Field , nchar>::operator()( TAString<nchar>&, const TFormat<nchar>::Field&  );
extern template ALIB_API void T_Append<TFormat<wchar>::Field , wchar>::operator()( TAString<wchar>&, const TFormat<wchar>::Field&  );
extern template ALIB_API void T_Append<TFormat<xchar>::Field , xchar>::operator()( TAString<xchar>&, const TFormat<xchar>::Field&  );

/** Specialization of functor \alib{strings,T_Append} for type \c Format::Escape. */
template<typename TChar> struct T_Append<typename TFormat<TChar>::Escape ,TChar>
{
    /** ********************************************************************************************
     * Escapes or un-escapes the characters in the given string.
     *
     * @param target The \b AString that \b Append was invoked on.
     * @param esc    The object to append.
     **********************************************************************************************/
    void operator()( TAString<TChar>& target, const typename TFormat<TChar>::Escape& esc);
};

extern template ALIB_API void T_Append<TFormat<nchar>::Escape , nchar>::operator()( TAString<nchar>&, const TFormat<nchar>::Escape&  );
extern template ALIB_API void T_Append<TFormat<wchar>::Escape , wchar>::operator()( TAString<wchar>&, const TFormat<wchar>::Escape&  );
extern template ALIB_API void T_Append<TFormat<xchar>::Escape , xchar>::operator()( TAString<xchar>&, const TFormat<xchar>::Escape&  );


/** Specialization of functor \alib{strings,T_Append} for type \c int8_t. */
template<typename TChar> struct T_Append<int8_t ,TChar>
{
    /**
     * Creates a defaulted object of type \alib{strings,TFormat,Format} and defers the number
     * conversion to the corresponding static specialization for that type.
     *
     * @param target The \b AString that \b Append was invoked on.
     * @param value  The value to write.
     */
    inline void operator()( TAString<TChar>& target,  const int8_t& value )
    {
        return T_Append<TFormat<TChar>,TChar>()( target,     TFormat<TChar>( static_cast<int64_t >(value), nullptr ) );
    }
};

/** Specialization of functor \alib{strings,T_Append} for type \c uint8_t. */
template<typename TChar> struct T_Append<uint8_t ,TChar>
{
    /**
     * Creates a defaulted object of type \alib{strings,TFormat,Format} and defers the number
     * conversion to the corresponding static specialization for that type.
     *
     * @param target The \b AString that \b Append was invoked on.
     * @param value  The value to write.
     */
    inline void operator()( TAString<TChar>& target,  const uint8_t& value )
    {
        return T_Append<TFormat<TChar>,TChar>()( target,     TFormat<TChar>( static_cast<uint64_t >(value), nullptr ) );
    }
};

/** Specialization of functor \alib{strings,T_Append} for type \c int16_t. */
template<typename TChar> struct T_Append<int16_t ,TChar>
{
    /**
     * Creates a defaulted object of type \alib{strings,TFormat,Format} and defers the number
     * conversion to the corresponding static specialization for that type.
     *
     * @param target The \b AString that \b Append was invoked on.

     * @param value  The value to write.
     */
    inline void operator()( TAString<TChar>& target,  const int16_t& value )
    {
        return T_Append<TFormat<TChar>,TChar>()( target,     TFormat<TChar>( value, nullptr ) );
    }
};

/** Specialization of functor \alib{strings,T_Append} for type \c uint16_t. */
template<typename TChar> struct T_Append<uint16_t ,TChar>
{
    /**
     * Creates a defaulted object of type \alib{strings,TFormat,Format} and defers the number
     * conversion to the corresponding static specialization for that type.
     *
     * @param target The \b AString that \b Append was invoked on.
     * @param value  The value to write.
     */
    inline void operator()( TAString<TChar>& target,  const uint16_t& value )
    {
        return T_Append<TFormat<TChar>,TChar>()( target,     TFormat<TChar>( value, nullptr ) );
    }
};

/** Specialization of functor \alib{strings,T_Append} for type \c int32_t. */
template<typename TChar> struct T_Append<int32_t ,TChar>
{
    /**
     * Creates a defaulted object of type \alib{strings,TFormat,Format} and defers the number
     * conversion to the corresponding static specialization for that type.
     *
     * @param target The \b AString that \b Append was invoked on.
     * @param value  The value to write.
     */
    inline void operator()( TAString<TChar>& target,  const int32_t& value )
    {
        return T_Append<TFormat<TChar>,TChar>()( target,     TFormat<TChar>( value, nullptr ) );
    }
};

/** Specialization of functor \alib{strings,T_Append} for type \c uint32_t. */
template<typename TChar> struct T_Append<uint32_t ,TChar>
{
    /**
     * Creates a defaulted object of type \alib{strings,TFormat,Format} and defers the number
     * conversion to the corresponding static specialization for that type.
     *
     * @param target The \b AString that \b Append was invoked on.
     * @param value  The value to write.
     */
    inline void operator()( TAString<TChar>& target,  const uint32_t& value )
    {
        return T_Append<TFormat<TChar>,TChar>()( target,     TFormat<TChar>( value, nullptr ) );
    }
};


/** Specialization of functor \alib{strings,T_Append} for type \c int64_t. */
template<typename TChar> struct T_Append<int64_t ,TChar>
{
    /**
     * Creates a defaulted object of type \alib{strings,TFormat,Format} and defers the number
     * conversion to the corresponding static specialization for that type.
     *
     * @param target The \b AString that \b Append was invoked on.
     * @param value  The value to write.
     */
    inline void operator()( TAString<TChar>& target,  const int64_t& value )
    {
        return T_Append<TFormat<TChar>,TChar>()( target,     TFormat<TChar>( value, nullptr ) );
    }
};

/** Specialization of functor \alib{strings,T_Append} for type \c uint64_t. */
template<typename TChar> struct T_Append<uint64_t ,TChar>
{
    /**
     * Creates a defaulted object of type \alib{strings,TFormat,Format} and defers the number
     * conversion to the corresponding static specialization for that type.
     *
     * @param target The \b AString that \b Append was invoked on.
     * @param value  The value to write.
     */
    inline void operator()( TAString<TChar>& target,  const uint64_t& value )
    {
        return T_Append<TFormat<TChar>,TChar>()( target,     TFormat<TChar>( value, nullptr ) );
    }
};

/** Specialization of functor \alib{strings,T_Append} for type \c intGap_t. */
template<typename TChar> struct T_Append<intGap_t ,TChar>
{
    /**
     * Creates a defaulted object of type \alib{strings,TFormat,Format} and defers the number
     * conversion to the corresponding static specialization for that type.
     *
     * @param target The \b AString that \b Append was invoked on.
     * @param value  The value to write.
     */
    inline void operator()( TAString<TChar>& target,  const intGap_t& value )
    {
        return T_Append<TFormat<TChar>,TChar>()( target,     TFormat<TChar>( value, nullptr ) );
    }
};


/** Specialization of functor \alib{strings,T_Append} for type \c uintGap_t. */
template<typename TChar> struct T_Append<uintGap_t ,TChar>
{
    /**
     * Creates a defaulted object of type \alib{strings,TFormat,Format} and defers the number
     * conversion to the corresponding static specialization for that type.
     *
     * @param target The \b AString that \b Append was invoked on.
     * @param value  The value to write.
     */
    inline void operator()( TAString<TChar>& target,  const uintGap_t& value )
    {
        return T_Append<TFormat<TChar>,TChar>()( target,     TFormat<TChar>( value, nullptr ) );
    }
};

/** Specialization of functor \alib{strings,T_Append} for type \c float. */
template<typename TChar> struct T_Append<float ,TChar>
{
    /**
     * Creates a defaulted object of type \alib{strings,TFormat,Format} and defers the number
     * conversion to the corresponding static specialization for that type.
     *
     * @param target The \b AString that \b Append was invoked on.
     * @param value  The value to write.
     */
    inline void operator()( TAString<TChar>& target,  const float& value)
    {
        return T_Append<TFormat<TChar>,TChar>()( target, TFormat<TChar>( static_cast<double>(value), nullptr ) );
    }
};

/** Specialization of functor \alib{strings,T_Append} for type \c double. */
template<typename TChar> struct T_Append<double ,TChar>
{
    /**
     * Creates a defaulted object of type \alib{strings,TFormat,Format} and defers the number
     * conversion to the corresponding static specialization for that type.
     *
     * @param target The \b AString that \b Append was invoked on.
     * @param value  The value to write.
     */
    inline void operator()( TAString<TChar>& target,  const double& value)
    {
        return T_Append<TFormat<TChar>,TChar>()( target,  TFormat<TChar>( value, nullptr ) );
    }
};

/** Specialization of functor \alib{strings,T_Append} for type \c Format::Bin. */
template<typename TChar> struct T_Append<typename TFormat<TChar>::Bin ,TChar>
{
    /** ********************************************************************************************
     * Appends a string representation of objects of type \alib{strings,TFormat::Bin,Format::Bin}.
     *
     * @param target The \b AString that \b Append was invoked on.
     * @param fmt    The format object.
     **********************************************************************************************/
    void operator()( TAString<TChar>& target,  const typename TFormat<TChar>::Bin& fmt );
};
extern template ALIB_API void T_Append<TFormat<nchar>::Bin , nchar>::operator()( TAString<nchar>&, const TFormat<nchar>::Bin&  );
extern template ALIB_API void T_Append<TFormat<wchar>::Bin , wchar>::operator()( TAString<wchar>&, const TFormat<wchar>::Bin&  );
extern template ALIB_API void T_Append<TFormat<xchar>::Bin , xchar>::operator()( TAString<xchar>&, const TFormat<xchar>::Bin&  );

/** Specialization of functor \alib{strings,T_Append} for type \c Format::Hex. */
template<typename TChar> struct T_Append<typename TFormat<TChar>::Hex ,TChar>
{
    /** ********************************************************************************************
     * Appends a string representation of objects of type \alib{strings,TFormat::Hex,Format::Hex}.
     *
     * @param target The \b AString that \b Append was invoked on.
     * @param fmt    The format object.
     **********************************************************************************************/
    void operator()( TAString<TChar>& target,  const typename TFormat<TChar>::Hex& fmt );
};
extern template ALIB_API void T_Append<TFormat<nchar>::Hex , nchar>::operator()( TAString<nchar>&, const TFormat<nchar>::Hex&  );
extern template ALIB_API void T_Append<TFormat<wchar>::Hex , wchar>::operator()( TAString<wchar>&, const TFormat<wchar>::Hex&  );
extern template ALIB_API void T_Append<TFormat<xchar>::Hex , xchar>::operator()( TAString<xchar>&, const TFormat<xchar>::Hex&  );

/** Specialization of functor \alib{strings,T_Append} for type \c Format::Oct. */
template<typename TChar> struct T_Append<typename TFormat<TChar>::Oct ,TChar>
{
    /** ********************************************************************************************
     * Appends a string representation of objects of type \alib{strings,TFormat::Oct,Format::Oct}.
     *
     * @param target The \b AString that \b Append was invoked on.
     * @param fmt    The format object.
     **********************************************************************************************/
    void operator()( TAString<TChar>& target,  const typename TFormat<TChar>::Oct& fmt );
};
extern template ALIB_API void T_Append<TFormat<nchar>::Oct , nchar>::operator()( TAString<nchar>&, const TFormat<nchar>::Oct&  );
extern template ALIB_API void T_Append<TFormat<wchar>::Oct , wchar>::operator()( TAString<wchar>&, const TFormat<wchar>::Oct&  );
extern template ALIB_API void T_Append<TFormat<xchar>::Oct , xchar>::operator()( TAString<xchar>&, const TFormat<xchar>::Oct&  );


// We are faking all template specializations of namespace strings for doxygen into namespace
// strings::APPENDABLES to keep the documentation of namespace string clean!
#if ALIB_DOCUMENTATION_PARSER
}
#endif

}}} // namespace [aworx::lib::strings]

