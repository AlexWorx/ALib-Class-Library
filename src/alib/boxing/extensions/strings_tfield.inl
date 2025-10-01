//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
/// With supporting legacy or module builds, .mpp-files are either recognized by the build-system
/// as C++20 Module interface files, or are included by the
/// \ref alib_manual_modules_impludes "import/include headers".
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#if ALIB_STRINGS

ALIB_EXPORT namespace alib::strings  {

// Note:
// If boxing is not included in the ALib Build, then an alternative version of this class
// is declared and implemented directly in module Strings.

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
///   In case, the module \alib_boxing is not available, the field content parameter will be of
///   type <c>const String&</c> and this class is available after the inclusion of the header
///   \implude{Strings}.
///
/// \note
///   Otherwise, a different implementation is used, which becomes available only with the
///   inclusion of the header \implude{Boxing}. That version stores a \alib{boxing;Box} instead
///   of a string type, and this way is able to place any type which disposes about an
///   implementation of box-function \alib{boxing;FAppend}.<br>
///
/// \note
///   Therefore, it is mandatory that for any type that is used with this class to be formatted
///   in a field, this box-function has to be implemented.
///   As documented with that interface, for types that are
///   \ref alib_strings_assembly_ttostring "appendable" to \b %AString objects already, all that
///   is needed is to use macro \ref ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE
///   with the type in the bootstrap section of an application.
/// @tparam TChar The \ref alib_characters_chars "character type" of the \b AString that
///               instances can be "applied" to.
template<typename TChar>
struct TField
{
    public:
    Box             theContent;   ///< The content of the field. If module
                                  ///< \alib_boxing_nl is not available, this field
                                  ///< is of type <c>const TString<TChar>&</c>
    integer         fieldWidth;   ///< The width of the field.
    lang::Alignment alignment;    ///< The alignment of the contents within the field.
    TChar           padChar;      ///< The characters used for padding the contents within the field.

    /// Constructor. Copies the parameters.
    ///
    /// @param content    The contents of the field. If the module \alib_boxing  is not
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
    TField( Box              content,
            integer          pWidth,
            lang::Alignment  pAlignment = lang::Alignment::Right,
            TChar            fillChar   = ' '                       )
    : theContent(content)
    , fieldWidth(pWidth)
    , alignment(pAlignment)
    , padChar(fillChar)                                                                           {}
};

/// Specialization of functor \alib{strings;AppendableTraits} for type \b %TField.
///
/// @tparam TChar      Character type of the target \b AString.
/// @tparam TAllocator Allocator type of the target \b AString.
template<typename TChar, typename TAllocator> struct AppendableTraits<TField<TChar> ,TChar,TAllocator>
{
    /// Writes the contents of \p{field} according to its specification.
    /// @param  target The AString to append \p{src} to.
    /// @param  field  The field instance (usually a temporary).
    void operator()( TAString<TChar,TAllocator>& target, const TField<TChar>& field);
};

extern template ALIB_DLL void AppendableTraits<TField <nchar>, nchar, lang::HeapAllocator>::operator()( TAString<nchar, lang::HeapAllocator>&, const TField <nchar>& );
extern template ALIB_DLL void AppendableTraits<TField <wchar>, wchar, lang::HeapAllocator>::operator()( TAString<wchar, lang::HeapAllocator>&, const TField <wchar>& );

}  // namespace [alib::strings]

ALIB_EXPORT namespace alib {
/// Type alias in namespace \b alib.
using  Field           =     strings::TField<character>;

/// Type alias in namespace \b alib.
using  NField          =     strings::TField<nchar>;

/// Type alias in namespace \b alib.
using  WField          =     strings::TField<wchar>;
}
#endif

