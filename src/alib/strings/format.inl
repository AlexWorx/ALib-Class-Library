//==================================================================================================
/// \file
/// This header file is part of module \alib_strings of the \aliblong.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_STRINGS_FORMAT
#define HPP_ALIB_STRINGS_FORMAT 1
#pragma once

#if !defined(HPP_ALIB_STRINGS_TASTRING_INLINING)
#   error "ALib sources with ending '.inl' must not be included from outside."
#endif

#if ALIB_BOXING
#   include "alib/boxing/boxing.hpp"
#endif

#include <limits>

namespace alib {  namespace strings {

//==================================================================================================
/// This is a type purely made to be \ref alib_strings_assembly_ttostring "appended" to objects of
/// type \alib{strings;TAString;AString}.
/// Various constructors accept integer and floating point values, along with formatting options.
/// The specialization of functor \alib{strings;T_Append} will use a given (or defaulted) instance
/// of class \alib{strings;TNumberFormat;NumberFormat} to format the encapsulated value and
/// append the result to the \b %AString in question.
///
/// \note
///   Within the same header file that this class in declared in, there are several
///   specializations of functor \alib{strings;T_Append} defined for plain integer and
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
///   does \b not use a locale specific number format. Instead, it uses one that is exchangeable
///   between applications independent of the locale setting of the executing machine.<br>
///   Consequently, for locale specific output, an object of this class needs to be appended
///   along with a locale enabled instance of \b %NumberFormat. For example:
///   \snippet "DOX_ASTRING_APPEND.cpp"     DOX_APPEND_FORMAT2
///   which - dependent on the current local setting - might produce:
///   \verbinclude "DOX_APPEND_FORMAT2.txt"
///
///
/// <b>Inner Types:</b><br>
/// Besides accepting plain number types, this class in addition aggregates several public inner
/// types, namely
/// - \alib{strings::TFormat;Tab},
/// - \alib{strings::TFormat;Field},
/// - \alib{strings::TFormat;Escape},
/// - \alib{strings::TFormat;Bin},
/// - \alib{strings::TFormat;Hex},
/// - \alib{strings::TFormat;Oct}, and 
/// - \alib{strings::TFormat;Fill}.
///
///
/// Each of these classes provide certain formatting options, which are implemented by a
/// corresponding specialization of functor \alib{strings;T_Append}.
///
/// \note The types are not 'physically' related to this class.
///       Instead, placing them here as public inner types has been purely a design decision.
///       Rationales were to make them more easily findable being aggregated here, to
///       depopulate the namespace and finally to make the using code more readable.
///
/// <b>Details on Formats:</b><br>
/// Details on the options of formatting integer and floating point numbers are documented
/// with class
/// \alib{strings;TNumberFormat;NumberFormat}.
///
/// @tparam TChar The \ref alib_characters_chars "character type" of the \b AString that
///               instances can be "applied" to.
//==================================================================================================
template<typename TChar>
class TFormat
{
    public:

    // #############################################################################################
    // Inner types
    // #############################################################################################

    //==============================================================================================
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
    //==============================================================================================
    struct Tab
    {
        public:
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
        Tab(  integer size, integer referenceIdx = 0, integer minPadChars= 1, TChar fillChar= ' ' )
        : tabSize(size), reference(referenceIdx), minPad(minPadChars), tabChar(fillChar)
        {}
    };

    //==============================================================================================
    /// Used to create temporary objects which are \ref alib_strings_assembly_ttostring "appended"
    /// to \alib{strings;TAString;AString} instances.<br>
    ///
    /// Appends the given object to the AString using a defined 'field'-width.
    /// If the contents of the field is shorter than parameter \p{width} specifies, the field is
    /// filled with a corresponding amount of \p{padChar} characters.<br>
    /// Parameter \p{alignment} of type
    /// \alib{lang;Alignment} allows left-, right- or center-aligning.
    /// the contents of the field.
    ///
    /// \note
    ///   In case, module \alib_boxing is not available, the field content
    ///   parameter will be of type <c>const String&</c>.<br>
    ///   Otherwise, box-function \alib{boxing;FAppend} will be invoked on
    ///   the given box internally to receive the string representation.
    ///
    /// \note
    ///   Therefore, it is mandatory, that for any type that is used with this class
    ///   to be formatted in a field, this box-function has to be implemented. As
    ///   documented with that interface, for types that are
    ///   \ref alib_strings_assembly_ttostring "appendable" to \b %AString
    ///   objects already, all that is needed is to use macro
    ///   \ref ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE with the type in the bootstrap
    ///   section of an application.
    //==============================================================================================
    struct Field
    {
        public:
        #if ALIB_BOXING  || DOXYGEN
        Box                     theContent;   ///< The content of the field. If module
                                              ///< \alib_boxing_nl  is not available, this field
                                              ///< is of type <c>const TString<TChar>&</c>
        #else
        const TString<TChar>&   theContent;
        #endif
        integer                 fieldWidth;   ///< The width of the field.
        lang::Alignment         alignment;    ///< The alignment of the contents within the field.
        TChar                   padChar;      ///< The characters used for padding the contents within the field.

        /// Constructor. Copies the parameters.
        ///
        /// @param content    The contents of the field. If module \alib_boxing  is not
        ///                   available, this field is of type <c>const TString<TChar>&</c>,
        ///                   otherwise of type \alib{boxing;Box}.
        /// @param pWidth     The width of the field
        /// @param pAlignment The alignment of the contents within the field.
        ///                   Defaults to
        ///                   \alib{lang;Alignment;Alignment::Right}
        ///                   Other options are
        ///                   \alib{lang;Alignment;Alignment::Left} and
        ///                   \alib{lang;Alignment;Alignment::Center}.
        /// @param fillChar   The character used to fill the field up to its size.
        ///                   Defaults to ' ' (space).
        Field(
                #if ALIB_BOXING
                Box              content,
                #else
                const TString<TChar>&  content,
                #endif

                integer           pWidth,
                lang::Alignment   pAlignment = lang::Alignment::Right,
                TChar             fillChar   = ' '                       )
        #if ALIB_BOXING
        : theContent(content),
        #else
        : theContent(content.IsNotNull() ? content : EMPTY_STRING ),
        #endif
          fieldWidth(pWidth),
          alignment(pAlignment),
          padChar(fillChar)
        {}

    };

    //==============================================================================================
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
    //==============================================================================================
    struct Escape
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
        Escape( lang::Switch escape= lang::Switch::On, integer regionStart = 0, integer regionLength =MAX_LEN )
        : pSwitch(escape), startIdx(regionStart), length(regionLength)
        {}
    };


    //==============================================================================================
    /// Implements a temporary object which can be  \ref alib_strings_assembly_ttostring "appended"
    /// to instances of type \alib{strings;TAString;AString}.
    ///
    /// Appends an integral value in binary format.
    ///
    /// \see
    ///   Class \alib{strings;TNumberFormat} for more information on formatting options for binary
    ///   number output.
    //==============================================================================================
    struct Bin
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
        Bin( TIntegral                 value,
             int                       overrideWidth= 0,
             TNumberFormat<TChar>*  numberFormat = nullptr        )
        : theValue  (static_cast<uint64_t>(value))
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
        Bin( TIntegral                 value,
             TNumberFormat<TChar>*  numberFormat                  )
        : theValue  (static_cast<uint64_t>(value))
        , theWidth  (0)
        , nf        (numberFormat)                  {}

    };

    //==============================================================================================
    /// Implements a temporary object which is \ref alib_strings_assembly_ttostring "appended"
    /// to instances of type \alib{strings;TAString;AString}.
    ///
    /// Appends an integral value in hexadecimal format.
    ///
    /// \see
    ///   Class \alib{strings;TNumberFormat} for more information on formatting options for
    ///   hexadecimal number output.
    //==============================================================================================
    struct Hex
    {
        public:
        uint64_t    theValue;       ///< The value to write.
        int         theWidth;       ///< The minimum width of the number to write.
                                    ///< Defaults to \c 0
                                    ///< which denotes to choose the value of field
                                    ///< \alib{strings;TNumberFormat::HexFieldWidth;NumberFormat::HexFieldWidth}.
        TNumberFormat<TChar>* nf;///< The number format to use. Defaults to \c nullptr which chooses
                                    ///< \alib{strings;TNumberFormat::Computational;NumberFormat::Computational}.

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
        Hex( TIntegral                  value,
             int                        overrideWidth= 0,
             TNumberFormat<TChar>*   numberFormat = nullptr        )
        : theValue  (static_cast<uint64_t>(value))
        , theWidth  (overrideWidth)
        , nf        (numberFormat)                  {}

        /// Constructor, taking the value and a just an object of type \b %NumberFormat.
        ///
        /// @param value         The value to write.
        /// @param numberFormat  The number format to use. Defaults to \c nullptr which chooses
        ///                      the static singleton found in
        ///                      \alib{strings;TNumberFormat::Computational;NumberFormat::Computational}.
        template<typename TIntegral>
        Hex( TIntegral                  value,
             TNumberFormat<TChar>*   numberFormat                  )
        : theValue  (static_cast<uint64_t>(value))
        , theWidth  (0)
        , nf        (numberFormat)                  {}
    };

    //==============================================================================================
    /// Implements a temporary object which is \ref alib_strings_assembly_ttostring "appended"
    /// to instances of type \alib{strings;TAString;AString}.
    ///
    /// Appends an integral value in octal format.
    ///
    /// \see
    ///   Class \alib{strings;TNumberFormat} for more information on formatting options for octal
    ///   number output.
    //==============================================================================================
    struct Oct
    {
        public:
        uint64_t        theValue; ///< The value to write.
        int             theWidth; ///< The minimum width of the number to write.
                                  ///< Defaults to \c 0
                                  ///< which denotes to choose the value of field
                                  ///< \alib{strings;TNumberFormat::OctFieldWidth;NumberFormat::OctFieldWidth}.
        TNumberFormat<TChar>* nf;   ///< The number format to use. Defaults to \c nullptr which chooses
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
        Oct( TIntegral                 value,
             int                        overrideWidth= 0,
             TNumberFormat<TChar>*   numberFormat = nullptr        )
        : theValue  (static_cast<uint64_t>(value))
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
        Oct( TIntegral                  value,
             TNumberFormat<TChar>*   numberFormat                  )
        : theValue  (static_cast<uint64_t>(value))
        , theWidth  (0)
        , nf        (numberFormat)                  {}
    };

    //==============================================================================================
    /// Implements a temporary object which is \ref alib_strings_assembly_ttostring "appended"
    /// to instances of type \alib{strings;TAString;AString}.<br>
    /// Appends a given number of characters.
    //==============================================================================================
    struct Fill
    {
        TChar        fillChar;  ///< The character to write.
        int          count;     ///< The number of characters to write.

        /// Constructor.
        /// @param pFillChar  The character to write.
        /// @param pCount     The number of characters to write.
        Fill( TChar   pFillChar, int     pCount )
        : fillChar(pFillChar)
        , count   (pCount)                                                                        {}
    };

    // #############################################################################################
    // Fields (class Format)
    // #############################################################################################


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


#if DOXYGEN
    /// Constructor. Stores parameters.
    ///
    /// @tparam T            The type of argument \p{value}. Deduced by the compiler. Integer and
    ///                      floating-point types are accepted
    /// @param value         The value to write.
    /// @param overrideWidth Defaults to \c 0 which
    ///                      denotes to choose the value of field
    ///                      \alib{strings;TNumberFormat::DecMinimumFieldWidth;NumberFormat::DecMinimumFieldWidth}.
    /// @param numberFormat  The number format to use. Defaults to \c nullptr which chooses
    ///                      the static singleton found in
    ///                      \alib{strings;TNumberFormat::Computational;NumberFormat::Computational}.
    template<typename T>
    inline
    TFormat( T                        value,
             int                      overrideWidth= 0,
             TNumberFormat<TChar>*    numberFormat = nullptr        );

    /// Alternative constructor that omits parameter \p{width} and set it to \c 0.
    ///
    /// @tparam T            The type of argument \p{value}. Deduced by the compiler. Integer and
    ///                      floating-point types are accepted
    /// @param value         The value to write.
    /// @param numberFormat  The number format to use. Defaults to \c nullptr which chooses
    ///                      the static singleton found in
    ///                      \alib{strings;TNumberFormat::Computational;NumberFormat::Computational}.
    template<typename T>
    inline
    TFormat( T                        value,
             TNumberFormat<TChar>*    numberFormat = nullptr        );

#else
    template<typename TInteger, ATMP_T_IF( int, std::numeric_limits<TInteger>::is_integer) = 0 >
    TFormat( TInteger                 value,
             int                      overrideWidth= 0,
             TNumberFormat<TChar>*    numberFormat = nullptr        )
    : nf       (numberFormat)
    , width    (overrideWidth)
    , valueType( std::numeric_limits<TInteger>::is_signed ? 1 : 2 )
    { v.value= static_cast<int64_t>(value); }

    template<typename TInteger, ATMP_T_IF( int, std::numeric_limits<TInteger>::is_integer) = 0 >
    TFormat( TInteger                 value,
             TNumberFormat<TChar>*    numberFormat = nullptr        )
    : nf       (numberFormat)
    , width    (0)
    , valueType( std::numeric_limits<TInteger>::is_signed ? 1 : 2 )
    { v.value= static_cast<int64_t>(value); }

    template<typename TFloat, ATMP_T_IF( int,     ATMP_EQ(TFloat, double)
                                              ||  ATMP_EQ(TFloat, float )  )= 0 >
    TFormat( TFloat                   value,
             int                      overrideWidth= 0,
             TNumberFormat<TChar>*    numberFormat = nullptr        )
    : nf       (numberFormat)
    , width    (overrideWidth)
    , valueType( 3 )
    { v.fpValue= static_cast<double>(value); }

    template<typename TFloat, ATMP_T_IF( int,     ATMP_EQ(TFloat, double)
                                                ||  ATMP_EQ(TFloat, float )  )= 0 >
    TFormat( TFloat                   value,
             TNumberFormat<TChar>*    numberFormat = nullptr        )
    : nf       (numberFormat)
    , width    ( 0 )
    , valueType( 3 )
    { v.fpValue= static_cast<double>(value); }
#endif // DOXYGEN


}; // class format



// #################################################################################################
// Corresponding specializations of struct  T_Append
// #################################################################################################

// Faking all template specializations of namespace strings for doxygen into namespace
// strings::APPENDABLES to keep the documentation of namespace string clean!
#if DOXYGEN
    namespace APPENDABLES {
#endif

/// Specialization of functor \alib{strings;T_Append} for type \c Format.
template<typename TChar, typename TAllocator> struct T_Append<TFormat<TChar> ,TChar,TAllocator>
{
    //==============================================================================================
    /// Appends a string representation of the value encapsulated in the given \b Format value.
    ///
    /// @param target The \b AString that \b Append was invoked on.
    /// @param fmt    The format object.
    //==============================================================================================
    void operator()( TAString<TChar,TAllocator>& target,  const TFormat<TChar>& fmt );
};

/// Specialization of functor \alib{strings;T_Append} for type \c Format::Tab.
template<typename TChar, typename TAllocator> struct T_Append<typename TFormat<TChar>::Tab ,TChar,TAllocator>
{
    //==============================================================================================
    /// Appends tabulator characters to the given string.
    ///
    /// @param  target   The \b AString that \b Append was invoked on.
    /// @param  tab      The object to append.
    //==============================================================================================
    void operator()( TAString<TChar,TAllocator>& target, const typename TFormat<TChar>::Tab& tab);
};

/// Specialization of functor \alib{strings;T_Append} for type \c Format::Field.
template<typename TChar, typename TAllocator> struct T_Append<typename TFormat<TChar>::Field ,TChar,TAllocator>
{
    //==============================================================================================
    /// Appends a field with the adjusted boxed content to the given string.
    ///
    /// @param target The \b AString that \b Append was invoked on.
    /// @param field  The object to append.
    //==============================================================================================
    void operator()( TAString<TChar,TAllocator>& target, const typename TFormat<TChar>::Field& field);
};


/// Specialization of functor \alib{strings;T_Append} for type \c Format::Escape.
template<typename TChar, typename TAllocator> struct T_Append<typename TFormat<TChar>::Escape ,TChar,TAllocator>
{
    //==============================================================================================
    /// Escapes or un-escapes the characters in the given string.
    ///
    /// @param target The \b AString that \b Append was invoked on.
    /// @param esc    The object to append.
    //==============================================================================================
    void operator()( TAString<TChar,TAllocator>& target, const typename TFormat<TChar>::Escape& esc);
};




/// Specialization of functor \alib{strings;T_Append} for type \c Format::Bin.
template<typename TChar, typename TAllocator> struct T_Append<typename TFormat<TChar>::Bin ,TChar,TAllocator>
{
    //==============================================================================================
    /// Appends a string representation of objects of type \alib{strings;TFormat::Bin;Format::Bin}.
    ///
    /// @param target The \b AString that \b Append was invoked on.
    /// @param fmt    The format object.
    //==============================================================================================
    void operator()( TAString<TChar,TAllocator>& target,  const typename TFormat<TChar>::Bin& fmt );
};

/// Specialization of functor \alib{strings;T_Append} for type \c Format::Hex.
template<typename TChar, typename TAllocator> struct T_Append<typename TFormat<TChar>::Hex ,TChar,TAllocator>
{
    //==============================================================================================
    /// Appends a string representation of objects of type \alib{strings;TFormat::Hex;Format::Hex}.
    ///
    /// @param target The \b AString that \b Append was invoked on.
    /// @param fmt    The format object.
    //==============================================================================================
    void operator()( TAString<TChar,TAllocator>& target,  const typename TFormat<TChar>::Hex& fmt );
};

/// Specialization of functor \alib{strings;T_Append} for type \c Format::Oct.
template<typename TChar, typename TAllocator> struct T_Append<typename TFormat<TChar>::Oct ,TChar,TAllocator>
{
    //==============================================================================================
    /// Appends a string representation of objects of type \alib{strings;TFormat::Oct;Format::Oct}.
    ///
    /// @param target The \b AString that \b Append was invoked on.
    /// @param fmt    The format object.
    //==============================================================================================
    void operator()( TAString<TChar,TAllocator>& target,  const typename TFormat<TChar>::Oct& fmt );
};

/// Specialization of functor \alib{strings;T_Append} for type \c Format::Fill.
template<typename TChar, typename TAllocator> struct T_Append<typename TFormat<TChar>::Fill ,TChar,TAllocator>
{
    //==============================================================================================
    /// Appends objects of type \alib{strings;TFormat::Fill;Format::Fill}.
    /// @param target The \b AString that \b Append was invoked on.
    /// @param fmt    The format object.
    //==============================================================================================
    void operator()( TAString<TChar,TAllocator>& target,  const typename TFormat<TChar>::Fill& fmt );
};
// Faking all template specializations of namespace strings for doxygen into namespace
// strings::APPENDABLES to keep the documentation of namespace string clean!
#if DOXYGEN
}
#endif

}} // namespace [alib::strings]

#endif // HPP_ALIB_STRINGS_FORMAT

