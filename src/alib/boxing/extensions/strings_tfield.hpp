//==================================================================================================
/// \file
/// This header-file is part of module \alib_boxing of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
#if ALIB_STRINGS

ALIB_EXPORT namespace alib::strings  {

// Note:
// If boxing is not included in the ALib Build, then an alternative version of this class
// is declared and implemented directly in module Strings.

/// Used to create temporary objects which are #"alib_strings_assembly_ttostring;appended"
/// to #"^AString" instances.<br>
///
/// Appends the given object to the AString using a defined 'field'-width.
/// If the contents of the field is shorter than parameter \p{width} specifies, the field is
/// filled with a corresponding amount of \p{padChar} characters.<br>
/// Parameter \p{alignment} of type #"lang::Alignment" allows left-, right- or center-aligning.
/// the contents of the field.
///
/// \note
///   In case, the module \alib_boxing is not available, the field content parameter will be of
///   type <c>const String&</c> and this class is available after the inclusion of the header
///   #"F;ALib.Strings.H".
///
/// \note
///   Otherwise, a different implementation is used, which becomes available only with the
///   inclusion of the header #"F;ALib.Boxing.H". That version stores a #"Box" instead
///   of a string type, and this way is able to place any type which disposes about an
///   implementation of box-function #"FAppend".<br>
///
/// \note
///   Therefore, it is mandatory that for any type that is used with this class to be formatted
///   in a field, this box-function has to be implemented.
///   As documented with that interface, for types that are
///   #"alib_strings_assembly_ttostring;appendable" to #"%AString" objects already, all that
///   is needed is to use macro #"ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE"
///   with the type in the bootstrap section of an application.
/// @tparam TChar The #"alib_characters_chars;character type" of the #"%AString" that
///               instances can be "applied" to.
template<typename TChar>
struct TField {
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
    ///                   otherwise of type #"Box".
    /// @param pWidth     The width of the field
    /// @param pAlignment The alignment of the contents within the field.
    ///                   Defaults to #"Alignment::Right"
    ///                   Other options are #"Alignment::Left" and #"Alignment::Center".
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


#if DOXYGEN
namespace APPENDABLES {
#endif

/// Specialization of functor #"AppendableTraits" for type #"%TField".
/// @tparam TChar      Character type of the target #"%AString".
/// @tparam TAllocator Allocator type of the target #"%AString".
template<typename TChar, typename TAllocator>
struct AppendableTraits<TField<TChar> ,TChar,TAllocator> {
    /// Writes the contents of \p{field} according to its specification.
    /// @param  target The AString to append \p{src} to.
    /// @param  field  The field instance (usually a temporary).
    void operator()( TAString<TChar,TAllocator>& target, const TField<TChar>& field);
};
#if DOXYGEN
}  // namespace alib::strings[::APPENDABLES]
#endif

extern template ALIB_DLL void AppendableTraits<TField <nchar>, nchar, lang::HeapAllocator>::operator()( TAString<nchar, lang::HeapAllocator>&, const TField <nchar>& );
extern template ALIB_DLL void AppendableTraits<TField <wchar>, wchar, lang::HeapAllocator>::operator()( TAString<wchar, lang::HeapAllocator>&, const TField <wchar>& );

}  // namespace [alib::strings]

ALIB_EXPORT namespace alib {
/// Type alias in namespace #"%alib".
using  Field           =     strings::TField<character>;

/// Type alias in namespace #"%alib".
using  NField          =     strings::TField<nchar>;

/// Type alias in namespace #"%alib".
using  WField          =     strings::TField<wchar>;
}
#endif
