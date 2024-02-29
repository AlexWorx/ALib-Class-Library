/** ************************************************************************************************
 * \file
 * This header file is part of sub-namespace #alib::lang::format of module \alib_basecamp of
 * the \aliblong.
 *
 * \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_LANG_FORMAT_BYTESIZE
#define HPP_ALIB_LANG_FORMAT_BYTESIZE 1

#if !defined(HPP_ALIB) && !defined(ALIB_DOX)
#   include "alib/alib.hpp"
#endif

ALIB_ASSERT_MODULE(CAMP)

#if !defined (HPP_ALIB_LANG_FORMAT_EXCEPTIONS)
    #include "alib/lang/format/fmtexceptions.hpp"
#endif

#if !defined (HPP_ALIB_ENUMS_ARITHMETICAL)
    #include "alib/enums/arithmetical.hpp"
#endif
#if !defined (HPP_ALIB_ENUMS_BITWISE)
    #include "alib/enums/bitwise.hpp"
#endif
#if !defined (HPP_ALIB_ENUMS_ITERABLE)
#   include "alib/enums/iterable.hpp"
#endif
#if !defined (HPP_ALIB_ENUMS_RECORDS)
#   include "alib/enums/records.hpp"
#endif
#if !defined(HPP_ALIB_ENUMS_SERIALIZATION)
#   include "alib/enums/serialization.hpp"
#endif

namespace alib { namespace  lang::format {

/**
 * Enumeration of byte-size units to output with types \alib{lang::format,ByteSizeIEC} and
 * \alib{lang::format,ByteSizeSI}.<br>
 * The unit symbols are read from the \ref alib_basecamp_resources "resources" of module class
 * \alib{lang::basecamp,BaseCamp}. The list is given with resource name <b>BS</b>.
*/
enum ByteSizeUnits : uint8_t
{
       IEC=  0,  ///< Begin of SI units.
         B=  0,  ///<  2^  0, Bbyte     , factor 1.
       KiB=  1,  ///<  2^ 10, Kibibyte  , factor 1024.
       MiB=  2,  ///<  2^ 20, Mebibyte  , factor 1048576.
       GiB=  3,  ///<  2^ 30, Gibibyte  , factor 1073741824.
       TiB=  4,  ///<  2^ 40, Tebibyte  , factor 1099511627776.
       PiB=  5,  ///<  2^ 50, Pebibyte  , factor 1125899906842624.
       EiB=  6,  ///<  2^ 60, Exbibyte  , factor 1152921504606846976.
       ZiB=  7,  ///<  2^ 70, Zebibyte  , factor 1180591620717411303424.
       YiB=  8,  ///<  2^ 80, Yobibyte  , factor 1208925819614629174706176.
       RiB=  9,  ///<  2^ 90, Robibyte  , factor 1237940039285380274899124224.
       QiB= 10,  ///<  2^100, Quebibyte , factor 1267650600228229401496703205376.
   IEC_END= 11,  ///< End of SI units

        SI= 11,  ///< Begin of IEC units.
      B_SI= 11,  ///<  2^  0, Byte      , factor 1.
        kB= 12,  ///< 10^  3, Kilobyte  , factor 100.
        MB= 13,  ///< 10^  6, Megabyte  , factor 100000.
        GB= 14,  ///< 10^  9, Gigabyte  , factor 100000000.
        TB= 15,  ///< 10^ 12, Terabyte  , factor 100000000000.
        PB= 16,  ///< 10^ 15, Petabyte  , factor 100000000000000.
        EB= 17,  ///< 10^ 18, Exabyte   , factor 100000000000000000.
        ZB= 18,  ///< 10^ 21, Zettabyte , factor 100000000000000000000.
        YB= 19,  ///< 10^ 24, Yottabyte , factor 100000000000000000000000.
        RB= 20,  ///< 10^ 27, Ronnabyte , factor 100000000000000000000000000.
        QB= 21,  ///< 10^ 30, Quettabyte, factor 100000000000000000000000000000.
   SI_END= 22,  ///< END of IEC units.
};

/** The \b NumberFormat singleton used with  \ref alib_strings_assembly_ttostring "appending"
 *  instances of structs \alib{lang::format,ByteSizeIEC} and \alib{lang::format,ByteSizeSI} to
 *  \b %AString objects. The object is allocated in the \alib{monomem,GlobalAllocator} during
 *  bootstrapping of module \alib_basecamp_nl.
 *
 *  Fields of interest here, are
 *  \alib{strings::TNumberFormat,DecimalPointChar}, which is copied during bootstrap from
 *  \alib{strings::TNumberFormat,Global,NumberFormat::Global}, and
 *  \alib{strings::TNumberFormat,FractionalPartWidth} which is set to \c 1 during bootstrap.
 */
extern NumberFormat*    ByteSizeNumberFormat;

/**
 * This namespace function searches the next "fitting" magnitude of a given \p{byteSize}, so that
 * it can be expressed as a floating point between \c 0 and \c 999, hence with three digits.
 * The function is used for \ref alib_strings_assembly_ttostring "appending" byte sizes to
 * to class \b %AString. Helper types \alib{lang::format,ByteSizeIEC} and \alib{lang::format,ByteSizeSI}
 * allow the convient use of this method in combination with \b AString objects and with parameter
 * lists of \alib formatter functions. When using these helpers, singleton
 * \alib{lang::format,ByteSizeNumberFormat} is used.
 *
 * \note In the very, very seldom case, that a software has different threads, and more than one of
 *       those needs to format byte sizes in different (!) number formats, the singleton approach is
 *       not feasable. This is the reason, why this method was exposed publicly instead of using
 *       an implementation in an anonymous namesapce.
 *
 * @param target             The target string.
 * @param byteSize           The value to print.
 * @param magnitudeThreshold The lowest value to use with the next lower possible
 *                           magnitude. If for example to \c 900, then <em>0.9 GiB</em>
 *                           is preferred over <em>900.0 MiB</em>.
 * @param unitSeparator      If not <c>'\0'</c>, this character is printed after the number and
 *                           before the unit.
 * @param unit               The unit system to convert to. This parameter must be set to either
 *                           \alib{lang::format,ByteSizeUnits,IEC} or \alib{lang::format,ByteSizeUnits,SI}.
 *                           Other values are undefined behaviour.
 * @param nf                 The number format object to use.
 */
ALIB_API
void FormatByteSize(AString& target, uinteger byteSize, uint16_t magnitudeThreshold,
                    char unitSeparator, ByteSizeUnits unit, NumberFormat& nf);

/** ************************************************************************************************
 * This struct is used to format sizes of byte arrays (streams, files, etc), in accordance
 * with IEC units (KiB, MiB, GiB, etc). Integral values embedded in this struct are
 * \ref alib_strings_assembly_ttostring "appendable" to class \b %AString.
 *
 * @see Sibling struct \alib{lang::format,ByteSizeSI}
 **************************************************************************************************/
struct ByteSizeIEC
{
    uinteger    Value;              ///< The encapsulated value to print.
    uint16_t    MagnitudeThreshold; ///< The lowest value to use with the next lower possible
                                    ///< magnitude. If for example to \c 900, then <em>0.9 GiB</em>
                                    ///< is preferred over <em>900.0 MiB</em>.
    char        UnitSeparator;      ///< An optional character to separate the number from the unit.

    /** Constructor.
     *  @param value               The value to append/format.
     *  @param magnitudeThreshold  Stored in #MagnitudeThreshold. Defaults to <c>8*1024/10</c>.
     *  @param unitSeparator       Separation character between printed number and unit.
     *                             Defaults to <c>0</c>.   */
    constexpr ByteSizeIEC( uinteger  value, uint16_t magnitudeThreshold = 8*1024/10,
                           char      unitSeparator= '\0' )                                  noexcept
    : Value             { value }
    , MagnitudeThreshold{ magnitudeThreshold }
    , UnitSeparator     { unitSeparator }                                                         {}

    /**
     * Evaluates the magnitude of the value and returns the converted \c double value between
     * \c 0.0 and \p{threshold}.
     * @return The #Value converted to \c double together with the magnitude flag.
     */
    ALIB_API
    std::pair<double,ByteSizeUnits> GetMagnitude();

    /**
     * Returns a double value converted to the given unit.
     * Note that even this class otherwise is responsible for IEC-units, conversions
     * to SI-units may be requested.
     * @param unit The unit to convert to.
     * @return Field #Value as a \c double converted to \p{unit}.
     */
    ALIB_API
    double                          ConvertTo(ByteSizeUnits unit);

}; //struct ByteSizeIEC

/** ************************************************************************************************
 * This struct is used to format sizes of byte arrays (streams, files, etc), in accordance
 * with SI units (kB, MB, GB, etc). Integral values embedded in this struct are
 * \ref alib_strings_assembly_ttostring "appendable" to class \b %AString.
 *
 * @see Sibling struct \alib{lang::format,ByteSizeIEC}
 **************************************************************************************************/
struct ByteSizeSI
{
    uinteger    Value;              ///< The encapsulated value to print.
    uint16_t    MagnitudeThreshold; ///< The lowest value to use with the next lower possible
                                    ///< magnitude. If for example to \c 900, then <em>0.9 GiB</em>
                                    ///< is preferred over <em>900.0 MiB</em>.
    char        UnitSeparator;      ///< An optional character to separate the number from the unit.

    /** Constructor.
     *  @param value                The value to append/format.
     *  @param magnitudeThreshold   Stored in #MagnitudeThreshold. Defaults to <c>800</c>.
     *  @param unitSeparator        Separation character between printed number and unit.
     *                              Defaults to <c>0</c>.   */
    constexpr ByteSizeSI( uinteger  value, uint16_t magnitudeThreshold = 800,
                          char      unitSeparator= '\0' )                                   noexcept
    : Value             { value }
    , MagnitudeThreshold{ magnitudeThreshold }
    , UnitSeparator     { unitSeparator }                                                         {}

    /**
     * Evaluates the magnitude of the value and returns the converted \c double value between
     * \c 0.0 and \p{threshold}.
     * @return The #Value converted to \c double together with the magnitude flag.
     */
    ALIB_API
    std::pair<double,ByteSizeUnits> GetMagnitude();

    /**
     * Returns a double value converted to the given unit.
     * Note that even this class otherwise is responsible for SI-units, conversions
     * to IEC-units may be requested.
     * @param unit The unit to convert to.
     * @return Field #Value as a \c double converted to \p{unit}.
     */
    ALIB_API
    double                          ConvertTo(ByteSizeUnits unit);
}; //struct ByteSizeSI

} namespace strings {

#if defined(ALIB_DOX)
namespace APPENDABLES { // Documentation fake namespace
#endif

/** Specialization of functor \alib{strings,T_Append} for type \alib{lang::format,ByteSizeIEC}.
 *  \note This specialization is available only if module \alib_basecamp is included in the
 *        \alibdist and a using source file includes header \alibheader{lang/format/bytesize.hpp}.    */
template<> struct T_Append< lang::format::ByteSizeIEC ,character>
{
    /** Appends the formatted \b ByteSizeIEC to the \p{target}.
     * @param target  The \b AString that \b Append was invoked on.
     * @param size    The size to append.                                 */
    ALIB_API
    void operator()( AString& target,  const  lang::format::ByteSizeIEC size );
};

/** Specialization of functor \alib{strings,T_Append} for type \alib{lang::format,ByteSizeSI}.
 *  \note This specialization is available only if module \alib_basecamp is included in the
 *        \alibdist and a using source file includes header \alibheader{lang/format/bytesize.hpp}.    */
template<> struct T_Append< lang::format::ByteSizeSI ,character>
{
    /** Appends the formatted \b ByteSizeSI to the \p{target}.
     * @param target  The \b AString that \b Append was invoked on.
     * @param size    The size to append.                                 */
    ALIB_API
    void operator()( AString& target,  const  lang::format::ByteSizeSI size );
};


#if defined(ALIB_DOX)
} // APPENDABLES documentation fake namespace
#endif


} // namespace alib[::strings]

/// Type alias in namespace \b alib.
using     ByteSizeUnits =    lang::format::ByteSizeUnits;

/// Type alias in namespace \b alib.
using     ByteSize      =    lang::format::ByteSizeIEC;

/// Type alias in namespace \b alib.
using     ByteSizeIEC   =    lang::format::ByteSizeIEC;

/// Type alias in namespace \b alib.
using     ByteSizeSI    =    lang::format::ByteSizeSI;


}  // namespace [alib]

ALIB_ENUMS_MAKE_ARITHMETICAL( alib::lang::format::ByteSizeUnits)
ALIB_ENUMS_MAKE_ITERABLE(     alib::lang::format::ByteSizeUnits, alib::lang::format::ByteSizeUnits::SI_END)
ALIB_ENUMS_ASSIGN_RECORD(     alib::lang::format::ByteSizeUnits, ERSerializable )
ALIB_RESOURCED_IN_MODULE(     alib::lang::format::ByteSizeUnits, alib::BASECAMP, "BS" )

#endif // HPP_ALIB_LANG_FORMAT_BYTESIZE
