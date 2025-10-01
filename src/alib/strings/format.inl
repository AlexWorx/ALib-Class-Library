//==================================================================================================
/// \file
/// This header-file is part of module \alib_strings of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================


ALIB_EXPORT namespace alib {  namespace strings {

/// This is a type purely made to be \ref alib_strings_assembly_ttostring "appended" to objects of
/// type \alib{strings;TAString;AString}.
/// Various constructors accept integer and floating point values, along with formatting options.
/// The specialization of functor \alib{strings;AppendableTraits} will use a given (or defaulted)
/// instance of class \alib{strings;TNumberFormat;NumberFormat} to format the encapsulated value and
/// append the result to the \b %AString in question.
///
/// \note
///   Within the same header-file that this class in declared in, there are several
///   specializations of functor \alib{strings;AppendableTraits} defined for plain integer and
///   floating point types. These specializations create an object of this type, providing the
///   value only, hence, using this classes constructor's default values. The number format
///   used as default by the constructors of this class is
///   \alib{strings;TNumberFormat::Computational;NumberFormat::Computational}.
///   As a result, the application of such core types, as in:
///   \snippet "DOX_ASTRING_APPEND.cpp"     DOX_APPEND_FORMAT1
///   which produces:
///   \verbinclude "DOX_APPEND_FORMAT1.txt"
///
/// \note
///   does \b not use a locale-specific number format. Instead, it uses one that is exchangeable
///   between applications independent of the locale setting of the executing machine.<br>
///   Consequently, for locale-specific output, an object of this class needs to be appended
///   along with a locale enabled instance of \b %NumberFormat. For example:
///   \snippet "DOX_ASTRING_APPEND.cpp"     DOX_APPEND_FORMAT2
///   which - dependent on the current local setting - might produce:
///   \verbinclude "DOX_APPEND_FORMAT2.txt"
///
///
/// <b>Details on Formats:</b><br>
/// Details on the options of formatting integer and floating point numbers are documented
/// with class
/// \alib{strings;TNumberFormat;NumberFormat}.
///
/// @tparam TChar The \ref alib_characters_chars "character type" of the \b AString that
///               instances can be "applied" to.
template<typename TChar>
class TDec
{
  public:
    /// The union to hold an integral or floating point value provided with the different
    /// constructors.
    union
    {
        int64_t     value;          ///< The value when using constructor with signed integer types.
        double      fpValue;        ///< The value when using constructor with type double.
    } v; ///< The data

    TNumberFormat<TChar>* nf;       ///< The number format to use. Defaults to \c nullptr which chooses
                                    ///< the static singleton found in
                                    ///< \alib{strings;TNumberFormat::Computational;NumberFormat::Computational}.

    int             width;          ///< The minimum width of the number to write.
                                    ///< Defaults to \c 0 which denotes to choose the value of field
                                    ///< \alib{strings;TNumberFormat::DecMinimumFieldWidth;NumberFormat::DecMinimumFieldWidth}.
    int             valueType;      ///< Flag witch value to use (1= sInt, 2=uInt, 3=fp )

    /// Constructor. Stores parameters.
    /// @tparam TIntegral    The type of argument \p{value}. Deduced by the compiler.
    ///                      Only integral values are accepted.
    /// @param value         The value to write.
    /// @param overrideWidth Defaults to \c 0 which denotes to choose the value of field
    ///                      \alib{strings;TNumberFormat::DecMinimumFieldWidth;NumberFormat::DecMinimumFieldWidth}.
    /// @param numberFormat  The number format to use.
    ///                      Defaults to \c nullptr which chooses the static singleton found in
    ///                      \alib{strings;TNumberFormat::Computational;NumberFormat::Computational}.
    template<typename TIntegral>
    requires std::integral<TIntegral>
    TDec( TIntegral                value,
          int                      overrideWidth= 0,
          TNumberFormat<TChar>*    numberFormat = nullptr        )
    : nf       (numberFormat)
    , width    (overrideWidth)
    , valueType( std::numeric_limits<TIntegral>::is_signed ? 1 : 2 )
    { v.value= int64_t(value); }


    /// Alternative constructor that omits parameter \p{overrideWidth} and set it to \c 0.
    /// @tparam TIntegral    The type of argument \p{value}. Deduced by the compiler.
    ///                      Only integral values are accepted.
    /// @param value         The value to write.
    /// @param numberFormat  The number format to use.
    ///                      Defaults to \c nullptr which chooses the static singleton found in
    ///                      \alib{strings;TNumberFormat::Computational;NumberFormat::Computational}.
    template<typename TIntegral>
    requires std::integral<TIntegral>
    TDec( TIntegral                value,
             TNumberFormat<TChar>*    numberFormat = nullptr        )
    : nf       (numberFormat)
    , width    (0)
    , valueType( std::numeric_limits<TIntegral>::is_signed ? 1 : 2 )
    { v.value= int64_t(value); }


    /// Constructor. Stores parameters.
    /// @tparam TFloat       The type of argument \p{value}. Deduced by the compiler.
    ///                      Only floating-point values are accepted.
    /// @param value         The value to write.
    /// @param overrideWidth Defaults to \c 0 which denotes to choose the value of field
    ///                      \alib{strings;TNumberFormat::DecMinimumFieldWidth;NumberFormat::DecMinimumFieldWidth}.
    /// @param numberFormat  The number format to use.
    ///                      Defaults to \c nullptr which chooses the static singleton found in
    ///                      \alib{strings;TNumberFormat::Computational;NumberFormat::Computational}.
    template<typename TFloat>
    requires std::floating_point<TFloat>
    TDec( TFloat                   value,
             int                      overrideWidth= 0,
             TNumberFormat<TChar>*    numberFormat = nullptr        )
    : nf       (numberFormat)
    , width    (overrideWidth)
    , valueType( 3 )                                      { v.fpValue= double(value); }

    /// Alternative constructor that omits parameter \p{overrideWidth} and set it to \c 0.
    /// @tparam TFloat       The type of argument \p{value}. Deduced by the compiler.
    ///                      Only floating-point values are accepted.
    /// @param value         The value to write.
    /// @param numberFormat  The number format to use.
    ///                      Defaults to \c nullptr which chooses the static singleton found in
    ///                      \alib{strings;TNumberFormat::Computational;NumberFormat::Computational}.
    template<typename TFloat>
    requires std::floating_point<TFloat>
    TDec( TFloat                   value,
             TNumberFormat<TChar>*    numberFormat = nullptr        )
    : nf       (numberFormat)
    , width    ( 0 )
    , valueType( 3 )                                      { v.fpValue= double(value); }

}; // class format


/// Implements a temporary object which is \ref alib_strings_assembly_ttostring "appended"
/// to instances of type \alib{strings;TAString;AString}.
///
/// Appends \e tab characters (as provided) to reach a certain length (aka tabulator position)
/// of the \p{target}. The tab position is referenced to an optionally given \p{reference} value
/// which might be the start of the string or the position of the last newline. If this
/// parameter is negative, the last newline characters are searched from the end of the
/// string backwards.<br>
/// Referring to that as position 0, the tab position is then located at the next multiple
/// of parameter \p{tabSize}, after having added  \p{minPad} tab characters.
/// @tparam TChar The \ref alib_characters_chars "character type" of the \b AString that
///               instances can be "applied" to.
template<typename TChar>
struct TTab
{
    /// The tab positions are multiples of this value.
    integer     tabSize;

    /// The reference length of the AString which is taken as relative tab position
    /// (instead of the beginning of the string). If -1, the target %AString is
    /// searched backwards for the last newline and this position is used as the
    /// reference.
    integer     reference;

    /// The minimum pad characters to add. Defaults to 1.
    integer     minPad;

    /// The character to insert to reach the tab position. Normally this is the space
    /// character ' '.
    TChar       tabChar;

    /// Constructor. Copies the parameters.
    ///
    /// @param size          The tab positions are multiples of this parameter.
    /// @param referenceIdx  The reference index marking the start of the actual line.
    ///                      If -1, the last new-line character is searched from the end of
    ///                      the string backwards, and used. Defaults to 0.
    /// @param minPadChars   The minimum pad characters to add. Defaults to 1.
    /// @param fillChar      The character to insert to reach the tab position.
    ///                      Defaults to ' ' (space).
    TTab(  integer size, integer referenceIdx = 0, integer minPadChars= 1, TChar fillChar= ' ' )
    : tabSize(size), reference(referenceIdx), minPad(minPadChars), tabChar(fillChar)
    {}
};

#if !ALIB_BOXING
// This version of the class is only used if module Boxing is not available.
template<typename TChar>
struct TField
{
    public:
    const TString<TChar>&   theContent;
    integer                 fieldWidth;   ///< The width of the field.
    lang::Alignment         alignment;    ///< The alignment of the contents within the field.
    TChar                   padChar;      ///< The characters used for padding the contents within the field.

    TField( const TString<TChar>&  content,
            integer                pWidth,
            lang::Alignment        pAlignment = lang::Alignment::Right,
            TChar                  fillChar   = ' '                       )
    : theContent(content.IsNotNull() ? content : EMPTY_STRING )
    , fieldWidth(pWidth)
    , alignment (pAlignment)
    , padChar   (fillChar)                                                                        {}
};
#endif// !ALIB_BOXING

/// Implements a temporary object which is \ref alib_strings_assembly_ttostring "appended"
/// to instances of type \alib{strings;TAString;AString}.
///
/// Escapes non-printable characters in the given region, or reversely converts such escaped
/// characters to their ASCII values.
///
/// The characters converted are
/// <c>'\\\\'</c>, <c>'\\r'</c>, <c>'\\n'</c>, <c>'\\t'</c>, <c>'\\a'</c>,
/// <c>'\\b'</c>,  <c>'\\v'</c>, <c>'\\f'</c>, <c>'\\e'</c> and  <c>'"'</c>.
///
/// If the new region length is needed to be known, it can be calculated as the sum of
/// the old region length and the difference of the string's length before and after the
/// operation.
/// @tparam TChar The \ref alib_characters_chars "character type" of the \b AString that
///               instances can be "applied" to.
template<typename TChar>
struct TEscape
{
    public:
    /// The direction of conversion: \b Switch::On escapes ascii characters, while
    /// \b Switch::Off converts escaped strings to ascii codes.
    lang::Switch pSwitch;

    /// The start of the region to convert.
    integer      startIdx;

    /// The length of the region to convert.
    integer      length;


    /// Constructor. Copies the parameters.
    ///
    /// @param escape        \b Switch::On escapes ascii characters (the default),
    ///                      \b Switch::Off converts  escaped strings to ascii codes.
    /// @param regionStart   The start of the region to convert.
    /// @param regionLength  The length of the region to convert.
    TEscape( lang::Switch escape= lang::Switch::On, integer regionStart = 0, integer regionLength =MAX_LEN )
    : pSwitch(escape), startIdx(regionStart), length(regionLength)
    {}
};


/// Implements a temporary object which can be  \ref alib_strings_assembly_ttostring "appended"
/// to instances of type \alib{strings;TAString;AString}.
///
/// Appends an integral value in binary format.
///
/// \see
///   Class \alib{strings;TNumberFormat} for more information on formatting options for binary
///   number output.
/// @tparam TChar The \ref alib_characters_chars "character type" of the \b AString that
///               instances can be "applied" to.
template<typename TChar>
struct TBin
{
    public:
    uint64_t        theValue;   ///< The value to write.
    int             theWidth;   ///< The minimum width of the number to write.
                                ///< Defaults to \c 0
                                ///< which denotes to choose the value of field
                                ///< \alib{strings;TNumberFormat::BinFieldWidth;NumberFormat::BinFieldWidth}.
    TNumberFormat<TChar>* nf;   ///< The number format to use. Defaults to \c nullptr which chooses
                                ///< \alib{strings;TNumberFormat::Computational;NumberFormat::Computational}.

    /// Constructor, taking the value and formatting parameters.
    ///
    /// @tparam TIntegral    Value type which has to be statically castable to \c std::uint64_t.
    /// @param value         The value to write.
    /// @param overrideWidth Defaults to \c 0 which
    ///                      denotes to choose the value of field
    ///                      \alib{strings;TNumberFormat::BinFieldWidth;NumberFormat::BinFieldWidth}.
    /// @param numberFormat  The number format to use. Defaults to \c nullptr which chooses
    ///                      the static singleton found in
    ///                      \alib{strings;TNumberFormat::Computational;NumberFormat::Computational}.
    template<typename TIntegral>
    TBin( TIntegral                 value,
          int                       overrideWidth= 0,
          TNumberFormat<TChar>*  numberFormat = nullptr        )
    : theValue  (uint64_t(value))
    , theWidth  (overrideWidth)
    , nf        (numberFormat)                  {}

    /// Constructor, taking the value and a just an object of type \b %NumberFormat.
    ///
    /// @tparam TIntegral    Value type which has to be statically castable to \c std::uint64_t.
    /// @param value         The value to write.
    /// @param numberFormat  The number format to use. Defaults to \c nullptr which chooses
    ///                      the static singleton found in
    ///                      \alib{strings;TNumberFormat::Computational}.
    template<typename TIntegral>
    TBin( TIntegral value, TNumberFormat<TChar>* numberFormat )
    : theValue  (uint64_t(value))
    , theWidth  (0)
    , nf        (numberFormat)                                                                    {}

};

/// Implements a temporary object which is \ref alib_strings_assembly_ttostring "appended"
/// to instances of type \alib{strings;TAString;AString}.
///
/// Appends an integral value in hexadecimal format.
///
/// \see
///   Class \alib{strings;TNumberFormat} for more information on formatting options for
///   hexadecimal number output.
/// @tparam TChar The \ref alib_characters_chars "character type" of the \b AString that
///               instances can be "applied" to.
template<typename TChar>
struct THex
{
    uint64_t    theValue;     ///< The value to write.
    int         theWidth;     ///< The minimum width of the number to write.
                              ///< Defaults to \c 0
                              ///< which denotes choosing the value of field
                              ///< \alib{strings;TNumberFormat::HexFieldWidth}.
    TNumberFormat<TChar>* nf; ///< The number format to use. Defaults to \c nullptr which chooses
                              ///< \alib{strings;TNumberFormat::Computational}.

    /// Constructor, taking the value and formatting parameters.
    ///
    /// @tparam TIntegral    Value type which has to be statically castable to \c std::uint64_t.
    /// @param value         The value to write.
    /// @param overrideWidth Defaults to \c 0 which
    ///                      denotes to choose the value of field
    ///                      \alib{strings;TNumberFormat::HexFieldWidth;NumberFormat::HexFieldWidth}.
    /// @param numberFormat  The number format to use. Defaults to \c nullptr which chooses
    ///                      the static singleton found in
    ///                      \alib{strings;TNumberFormat::Computational;NumberFormat::Computational}.
    template<typename TIntegral>
    THex( TIntegral value, int overrideWidth= 0, TNumberFormat<TChar>* numberFormat = nullptr )
    : theValue  (uint64_t(value))
    , theWidth  (overrideWidth)
    , nf        (numberFormat)                  {}

    /// Constructor, taking the value and a just an object of type \b %NumberFormat.
    ///
    /// @param value         The value to write.
    /// @param numberFormat  The number format to use. Defaults to \c nullptr which chooses
    ///                      the static singleton found in
    ///                      \alib{strings;TNumberFormat::Computational;NumberFormat::Computational}.
    template<typename TIntegral>
    THex( TIntegral value, TNumberFormat<TChar>* numberFormat )
    : theValue  (uint64_t(value))
    , theWidth  (0)
    , nf        (numberFormat)                  {}
};

/// Implements a temporary object which is \ref alib_strings_assembly_ttostring "appended"
/// to instances of type \alib{strings;TAString;AString}.
///
/// Appends an integral value in octal format.
///
/// \see
///   Class \alib{strings;TNumberFormat} for more information on formatting options for octal
///   number output.
/// @tparam TChar The \ref alib_characters_chars "character type" of the \b AString that
///               instances can be "applied" to.
template<typename TChar>
struct TOct
{
    uint64_t        theValue; ///< The value to write.
    int             theWidth; ///< The minimum width of the number to write.
                              ///< Defaults to \c 0
                              ///< which denotes to choose the value of field
                              ///< \alib{strings;TNumberFormat::OctFieldWidth;NumberFormat::OctFieldWidth}.
    TNumberFormat<TChar>* nf; ///< The number format to use. Defaults to \c nullptr which chooses
                              ///< \alib{strings;TNumberFormat::Computational;NumberFormat::Computational}.

    /// Constructor, taking the value and formatting parameters.
    ///
    /// @tparam TIntegral    Value type which has to be statically castable to \c std::uint64_t.
    /// @param value         The value to write.
    /// @param overrideWidth Defaults to \c 0 which
    ///                      denotes to choose the value of field
    ///                      \alib{strings;TNumberFormat::OctFieldWidth;NumberFormat::OctFieldWidth}.
    /// @param numberFormat  The number format to use. Defaults to \c nullptr which chooses
    ///                      the static singleton found in
    ///                      \alib{strings;TNumberFormat::Computational;NumberFormat::Computational}.
    template<typename TIntegral>
    TOct( TIntegral value, int overrideWidth= 0, TNumberFormat<TChar>* numberFormat = nullptr )
    : theValue  (uint64_t(value))
    , theWidth  (overrideWidth)
    , nf        (numberFormat)                  {}

    /// Constructor, taking the value and a just an object of type \b %NumberFormat.
    ///
    /// @tparam TIntegral    Value type which has to be statically castable to \c std::uint64_t.
    /// @param value         The value to write.
    /// @param numberFormat  The number format to use. Defaults to \c nullptr which chooses
    ///                      the static singleton found in
    ///                      \alib{strings;TNumberFormat::Computational;NumberFormat::Computational}.
    template<typename TIntegral>
    TOct( TIntegral value, TNumberFormat<TChar>* numberFormat )
    : theValue  (uint64_t(value))
    , theWidth  (0)
    , nf        (numberFormat)                  {}
};

/// Implements a temporary object which is \ref alib_strings_assembly_ttostring "appended"
/// to instances of type \alib{strings;TAString;AString}.<br>
/// Appends a given number of characters.
/// @tparam TChar The \ref alib_characters_chars "character type" of the \b AString that
///               instances can be "applied" to.
template<typename TChar>
struct TFill
{
    TChar        fillChar;  ///< The character to write.
    int          count;     ///< The number of characters to write.

    /// Constructor.
    /// @param pFillChar  The character to write.
    /// @param pCount     The number of characters to write.
    TFill(TChar pFillChar, int pCount )
    : fillChar(pFillChar)
    , count   (pCount)                                                                        {}
};

// #################################################################################################
// Corresponding specializations of struct  AppendableTraits
// #################################################################################################

// Faking all template specializations of namespace strings for doxygen into namespace
// strings::APPENDABLES to keep the documentation of namespace string clean!
#if DOXYGEN
    namespace APPENDABLES {
#endif

/// Specialization of functor \alib{strings;AppendableTraits} for type \c Format.
template<typename TChar, typename TAllocator> struct AppendableTraits<TDec<TChar> ,TChar,TAllocator>
{
    /// Appends a string representation of the value encapsulated in the given \b Format value.
    ///
    /// @param target The \b AString that \b Append was invoked on.
    /// @param src    The format object.
    void operator()( TAString<TChar,TAllocator>& target,  const TDec<TChar>& src );
};

/// Specialization of functor \alib{strings;AppendableTraits} for type \c TTab.
template<typename TChar, typename TAllocator> struct AppendableTraits<TTab<TChar> ,TChar,TAllocator>
{
    /// Appends tabulator characters to the given string.
    ///
    /// @param  target   The \b AString that \b Append was invoked on.
    /// @param  tab      The object to append.
    void operator()( TAString<TChar,TAllocator>& target, const TTab<TChar>& tab);
};

#if !ALIB_BOXING
template<typename TChar, typename TAllocator> struct AppendableTraits<TField<TChar> ,TChar,TAllocator>
{
    void operator()( TAString<TChar,TAllocator>& target, const TField<TChar>& field);
};
#endif
    
/// Specialization of functor \alib{strings;AppendableTraits} for type \c Escape.
template<typename TChar, typename TAllocator> struct AppendableTraits<TEscape<TChar> ,TChar,TAllocator>
{
    /// Escapes or un-escapes the characters in the given string.
    ///
    /// @param target The \b AString that \b Append was invoked on.
    /// @param esc    The object to append.
    void operator()( TAString<TChar,TAllocator>& target, const TEscape<TChar>& esc);
};

/// Specialization of functor \alib{strings;AppendableTraits} for type \c Bin.
template<typename TChar, typename TAllocator> struct AppendableTraits<TBin<TChar> ,TChar,TAllocator>
{
    /// Appends a string representation of the given \p{src}.
    /// @param target The \b AString that \b Append was invoked on.
    /// @param src    The format object.
    void operator()( TAString<TChar,TAllocator>& target,  const TBin<TChar>& src );
};

/// Specialization of functor \alib{strings;AppendableTraits} for type \c Hex.
template<typename TChar, typename TAllocator> struct AppendableTraits<THex<TChar>,TChar,TAllocator>
{
    /// Appends a string representation of the given \p{src}.
    /// @param target The \b AString that \b Append was invoked on.
    /// @param src    The format object.
    void operator()( TAString<TChar,TAllocator>& target,  const THex<TChar>& src );
};

/// Specialization of functor \alib{strings;AppendableTraits} for type \c Oct.
template<typename TChar, typename TAllocator> struct AppendableTraits<TOct<TChar> ,TChar,TAllocator>
{
    /// Appends a string representation of the given \p{src}.
    /// @param target The \b AString that \b Append was invoked on.
    /// @param src    The format object.
    void operator()( TAString<TChar,TAllocator>& target,  const TOct<TChar>& src );
};

/// Specialization of functor \alib{strings;AppendableTraits} for type \c Fill.
template<typename TChar, typename TAllocator> struct AppendableTraits<TFill<TChar> ,TChar,TAllocator>
{
    /// Appends a string representation of the given \p{src}.
    /// @param target The \b AString that \b Append was invoked on.
    /// @param src    The format object.
    void operator()( TAString<TChar,TAllocator>& target,  const TFill<TChar>& src );
};
// Faking all template specializations of namespace strings for doxygen into namespace
// strings::APPENDABLES to keep the documentation of namespace string clean!
#if DOXYGEN
}
#endif
}

/// Type alias in namespace \b alib.
using  Tab           =     strings::TTab<character>;

/// Type alias in namespace \b alib.
using  NTab          =     strings::TTab<nchar>;

/// Type alias in namespace \b alib.
using  WTab          =     strings::TTab<wchar>;

#if !ALIB_BOXING
/// Type alias in namespace \b alib.
using  Field         =     strings::TField<character>;

/// Type alias in namespace \b alib.
using  NField        =     strings::TField<nchar>;

/// Type alias in namespace \b alib.
using  WField        =     strings::TField<wchar>;
#endif

/// Type alias in namespace \b alib.
using  Escape        =     strings::TEscape<character>;

/// Type alias in namespace \b alib.
using  NEscape       =     strings::TEscape<nchar>;

/// Type alias in namespace \b alib.
using  WEscape       =     strings::TEscape<wchar>;

/// Type alias in namespace \b alib.
using  Dec           =     strings::TDec<character>;

/// Type alias in namespace \b alib.
using  NDec          =     strings::TDec<nchar>;

/// Type alias in namespace \b alib.
using  WDec          =     strings::TDec<wchar>;

/// Type alias in namespace \b alib.
using  Hex           =     strings::THex<character>;

/// Type alias in namespace \b alib.
using  NHex          =     strings::THex<nchar>;

/// Type alias in namespace \b alib.
using  WHex          =     strings::THex<wchar>;

/// Type alias in namespace \b alib.
using  Oct           =     strings::TOct<character>;

/// Type alias in namespace \b alib.
using  NOct          =     strings::TOct<nchar>;

/// Type alias in namespace \b alib.
using  WOct          =     strings::TOct<wchar>;

/// Type alias in namespace \b alib.
using  Bin           =     strings::TBin<character>;

/// Type alias in namespace \b alib.
using  NBin          =     strings::TBin<nchar>;

/// Type alias in namespace \b alib.
using  WBin          =     strings::TBin<wchar>;

/// Type alias in namespace \b alib.
using  Fill          =     strings::TFill<character>;

/// Type alias in namespace \b alib.
using  NFill         =     strings::TFill<nchar>;

/// Type alias in namespace \b alib.
using  WFill         =     strings::TFill<wchar>;

} // namespace [alib::strings]


