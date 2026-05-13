//==================================================================================================
/// \file
/// This header-file is part of module \alib_format of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
ALIB_EXPORT namespace alib { namespace  format {

/// Enumeration of byte-size units to output with types #"ByteSizeIEC" and
/// #"ByteSizeSI".<br>
/// The unit symbols are read from the #"alib_mod_resources;resources" of module class
/// #"BASECAMP;2". The list is given with resource name <b>BS</b>.
enum class ByteSizeUnits : uint8_t {
       IEC=  0,  ///< Begin of SI units.
         B=  0,  ///< 2^ 0, Bbyte, factor 1.
       KiB=  1,  ///< 2^ 10, Kibibyte, factor 1024.
       MiB=  2,  ///< 2^ 20, Mebibyte, factor 1048576.
       GiB=  3,  ///< 2^ 30, Gibibyte, factor 1073741824.
       TiB=  4,  ///< 2^ 40, Tebibyte, factor 1099511627776.
       PiB=  5,  ///< 2^ 50, Pebibyte, factor 1125899906842624.
       EiB=  6,  ///< 2^ 60, Exbibyte, factor 1152921504606846976.
       ZiB=  7,  ///< 2^ 70, Zebibyte, factor 1180591620717411303424.
       YiB=  8,  ///< 2^ 80, Yobibyte, factor 1208925819614629174706176.
       RiB=  9,  ///< 2^ 90, Robibyte, factor 1237940039285380274899124224.
       QiB= 10,  ///< 2^100, Quebibyte, factor 1267650600228229401496703205376.
   IEC_END= 11,  ///< End of SI units

        SI= 11,  ///< Begin of IEC units.
      B_SI= 11,  ///< 2^ 0, Byte, factor 1.
        kB= 12,  ///< 10^ 3, Kilobyte, factor 100.
        MB= 13,  ///< 10^ 6, Megabyte, factor 100000.
        GB= 14,  ///< 10^ 9, Gigabyte, factor 100000000.
        TB= 15,  ///< 10^ 12, Terabyte, factor 100000000000.
        PB= 16,  ///< 10^ 15, Petabyte, factor 100000000000000.
        EB= 17,  ///< 10^ 18, Exabyte, factor 100000000000000000.
        ZB= 18,  ///< 10^ 21, Zettabyte, factor 100000000000000000000.
        YB= 19,  ///< 10^ 24, Yottabyte, factor 100000000000000000000000.
        RB= 20,  ///< 10^ 27, Ronnabyte, factor 100000000000000000000000000.
        QB= 21,  ///< 10^ 30, Quettabyte, factor 100000000000000000000000000000.
    SI_END= 22,  ///< END of IEC units.
};

/// The #"%^NumberFormat" singleton used with  #"alib_strings_assembly_ttostring;appending"
/// instances of structs #"ByteSizeIEC" and #"ByteSizeSI" to
/// #"%AString" objects. The object is allocated in the #"GLOBAL_ALLOCATOR" during
/// bootstrapping.
///
/// Fields of interest here, are
/// #"TNumberFormat::DecimalPointChar", which is copied during bootstrap from
/// #"TNumberFormat::Global", and
/// #"TNumberFormat::FractionalPartWidth" which is set to \c 1 during bootstrap.
extern NumberFormat*    BYTESIZE_NUMBER_FORMAT;

/// This namespace function searches the next "fitting" magnitude of a given \p{byteSize}, so that
/// it can be expressed as a floating point between \c 0 and \c 999, hence with three digits.
/// The function is used for #"alib_strings_assembly_ttostring;appending" byte sizes to
/// class #"%AString". Helper-types #"ByteSizeIEC" and #"ByteSizeSI"
/// allow the convenient use of this method in combination with #"%AString" objects and with parameter
/// lists of \alib formatter functions. When using these helpers, singleton
/// #"BYTESIZE_NUMBER_FORMAT" is used.
///
/// \note In the seldom case, that software has different threads, and more than one of
///       those needs to format byte sizes in different number formats, the singleton approach is
///       not feasible. This is the reason, why this method was exposed publicly instead of using
///       an implementation in an anonymous namespace.
///
/// @param target             The target string.
/// @param byteSize           The value to print.
/// @param magnitudeThreshold The lowest value to use with the next lower possible
///                           magnitude. If, for example, to \c 900, then <em>0.9 GiB</em>
///                           is preferred over <em>900.0 MiB</em>.
/// @param unitSeparator      If not <c>'\0'</c>, this character is printed after the number and
///                           before the unit.
/// @param unit               The unit system to convert to. This parameter must be set to either
///                           #"ByteSizeUnits;IEC" or
///                           #"ByteSizeUnits;SI".
///                           Other values are undefined behavior.
/// @param nf                 The number format object to use.
ALIB_DLL
void FormatByteSize(AString& target, uinteger byteSize, uint16_t magnitudeThreshold,
                    char unitSeparator, ByteSizeUnits unit, NumberFormat& nf);

//==================================================================================================
/// This struct is used to format sizes of byte arrays (streams, files, etc), in accordance
/// with IEC units which are based on 1024 bytes (\e KiB, \e MiB, \e GiB, etc).
/// Integral values embedded in this struct are #"alib_strings_assembly_ttostring;appendable"
/// to class #"%AString".
///
/// @see Sibling struct #"ByteSizeSI"
//==================================================================================================
struct ByteSizeIEC {
    uinteger   Value;              ///< The encapsulated value to print.
    uint16_t   MagnitudeThreshold; ///< The lowest value to use with the next lower possible
                                   ///< magnitude. If, for example, to \c 900, then <em>0.9 GiB</em>
                                   ///< is preferred over <em>900.0 MiB</em>.
    char       UnitSeparator;      ///< An optional character to separate the number from the unit.

    /// Constructor.
    /// @param value               The value to append/format.
    /// @param magnitudeThreshold  Stored in #".MagnitudeThreshold". Defaults to <c>8*1024/10</c>.
    /// @param unitSeparator       Separation character between printed number and unit.
    ///                            Defaults to <c>0</c>.
    constexpr ByteSizeIEC( uinteger  value, uint16_t magnitudeThreshold = 8*1024/10,
                           char      unitSeparator= '\0' )                                  noexcept
    : Value             { value }
    , MagnitudeThreshold{ magnitudeThreshold }
    , UnitSeparator     { unitSeparator }                                                         {}

    /// Evaluates the magnitude of the value and returns the converted \c double value between
    /// \c 0.0 and \p{threshold}.
    /// @return The #"Value" converted to \c double together with the magnitude flag.
    ALIB_DLL
    std::pair<double,ByteSizeUnits> GetMagnitude();

    /// Returns a double value converted to the given unit.
    /// Note that while this class otherwise is responsible for IEC units, conversions
    /// to SI-units may be requested.
    /// @param unit The unit to convert to.
    /// @return Field #"Value" as a \c double converted to \p{unit}.
    ALIB_DLL
    double                          ConvertTo(ByteSizeUnits unit);

}; //struct ByteSizeIEC

//==================================================================================================
/// This struct is used to format sizes of byte arrays (streams, files, etc), in accordance
/// with SI units which are based on 1000 bytes (\e kB, \e MB, \e GB, etc).
/// Integral values embedded in this struct are #"alib_strings_assembly_ttostring;appendable" to class #"%AString".
///
/// @see Sibling struct #"ByteSizeIEC"
//==================================================================================================
struct ByteSizeSI {
    uinteger   Value;              ///< The encapsulated value to print.
    uint16_t   MagnitudeThreshold; ///< The lowest value to use with the next lower possible
                                   ///< magnitude. If, for example, to \c 900, then <em>0.9 GiB</em>
                                   ///< is preferred over <em>900.0 MiB</em>.
    char       UnitSeparator;      ///< An optional character to separate the number from the unit.

    /// Constructor.
    /// @param value                The value to append/format.
    /// @param magnitudeThreshold   Stored in #".MagnitudeThreshold". Defaults to <c>800</c>.
    /// @param unitSeparator        Separation character between printed number and unit.
    ///                             Defaults to <c>0</c>.
    constexpr ByteSizeSI( uinteger  value, uint16_t magnitudeThreshold = 800,
                          char      unitSeparator= '\0' )                                   noexcept
    : Value             { value }
    , MagnitudeThreshold{ magnitudeThreshold }
    , UnitSeparator     { unitSeparator }                                                         {}

    /// Evaluates the magnitude of the value and returns the converted \c double value between
    /// \c 0.0 and \p{threshold}.
    /// @return The #"Value" converted to \c double together with the magnitude flag.
    ALIB_DLL
    std::pair<double,ByteSizeUnits> GetMagnitude();

    /// Returns a double value converted to the given unit.
    /// Note that while this class otherwise is responsible for SI units, conversions
    /// to IEC-units may be requested.
    /// @param unit The unit to convert to.
    /// @return Field #"Value" as a \c double converted to \p{unit}.
    ALIB_DLL
    double                          ConvertTo(ByteSizeUnits unit);
}; //struct ByteSizeSI

} namespace strings {

#if DOXYGEN
namespace APPENDABLES { // Documentation fake namespace
#endif

/// Specialization of functor #"AppendableTraits" for type #"ByteSizeIEC".
/// \note This specialization is available only if the module \alib_format is included in the
///       \alibbuild.
template<>
struct AppendableTraits< format::ByteSizeIEC ,character, lang::HeapAllocator> {
    /// Appends the formatted #"%ByteSizeIEC" to the \p{target}.
    /// @param target  The #"%AString" that #"%Append(const TAppendable&)" was invoked on.
    /// @param size    The size to append.
    ALIB_DLL
    void operator()( AString& target,  const  format::ByteSizeIEC size );
};

/// Specialization of functor #"AppendableTraits" for type #"ByteSizeSI".
/// \note This specialization is available only if the module \alib_format is included in the
///       \alibbuild.
template<>
struct AppendableTraits< format::ByteSizeSI ,character, lang::HeapAllocator> {
    /// Appends the formatted #"%ByteSizeSI" to the \p{target}.
    /// @param target  The #"%AString" that #"%Append(const TAppendable&)" was invoked on.
    /// @param size    The size to append.
    ALIB_DLL
    void operator()( AString& target,  const  format::ByteSizeSI size );
};


#if DOXYGEN
} // APPENDABLES documentation fake namespace
#endif


} // namespace alib[::strings]

/// Type alias in namespace #"%alib".
using     ByteSizeUnits =    format::ByteSizeUnits;

/// Type alias in namespace #"%alib".
using     ByteSize      =    format::ByteSizeIEC;

/// Type alias in namespace #"%alib".
using     ByteSizeIEC   =    format::ByteSizeIEC;

/// Type alias in namespace #"%alib".
using     ByteSizeSI    =    format::ByteSizeSI;


}  // namespace [alib]

ALIB_ENUMS_MAKE_ARITHMETICAL( alib::format::ByteSizeUnits)
ALIB_ENUMS_MAKE_ITERABLE(     alib::format::ByteSizeUnits, alib::format::ByteSizeUnits::SI_END)
ALIB_ENUMS_ASSIGN_RECORD(     alib::format::ByteSizeUnits, ERSerializable )

ALIB_BOXING_VTABLE_DECLARE( alib::format::ByteSizeIEC      , vt_lang_format_bytesize_iec )
ALIB_BOXING_VTABLE_DECLARE( alib::format::ByteSizeSI       , vt_lang_format_bytesize_si  )
ALIB_BOXING_VTABLE_DECLARE( alib::format::ByteSizeUnits    , vt_lang_format_bytesize_units )
