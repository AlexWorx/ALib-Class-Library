//==================================================================================================
/// \file
/// This header-file is part of module \alib_strings of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
ALIB_EXPORT namespace alib {  namespace strings { namespace detail {

//==================================================================================================
/// Reads digits \c '0' to \c '9' into a positive integral value.
///
/// \note
///   The function is very basic, i.e., it does not use an object of type
///   #"str TNumberFormat", does not tolerate group characters and so forth.
///   Instead, it simply reads decimal digits, until a non-digit character
///   is found or the string ends.
///
/// @tparam        TChar The character type of the string to parse from.
/// @param         src   The string to read the value from.
/// @param[in,out] idx   The start point for parsing within \p{src}. Will be set to point
///                      behind the last character consumed. If unchanged, this indicates
///                      that no parsable number was found. If out of bounds, \c 0 is returned.
/// @return The parsed value. In addition, on success, parameter \p{idx} is moved to point to the
///         first character behind the parsed number.
//==================================================================================================
template<typename TChar>
uint64_t  ParseDecDigits( const TString<TChar>& src, integer& idx );


//==================================================================================================
/// Parses signed integer numbers, optionally in binary, hexadecimal or octal format.
///
/// Leading characters defined in field #"TNumberFormat::Whitespaces" of the
/// #"%^NumberFormat" object given with \p{nf} are ignored.
/// An optional sign character \c '+' or \c '-' is parsed. If found, again whitespace characters
/// may follow behind such sign and are ignored.
///
/// Then, the function detects any literal prefixes as defined in fields
/// #"TNumberFormat::BinLiteralPrefix",
/// #"TNumberFormat::HexLiteralPrefix" and
/// #"TNumberFormat::OctLiteralPrefix" (usually \c 0b, \c 0x and \c 0o) and
/// invokes one of the functions #"ParseDec", #"ParseBin", #"ParseHex" or #"ParseOct".
///
/// @tparam        TChar The character type of the string to parse from.
/// @param         src   The string to read the value from.
/// @param[in,out] idx   The start point for parsing within \p{src}. Will be set to point
///                      behind the last character consumed. If unchanged, this indicates
///                      that no parsable number was found.
/// @param         nf    The number format to use.
/// @return The parsed value. In addition, on success, parameter \p{idx} is moved to point to the
///         first character behind the parsed number.
//==================================================================================================
template<typename TChar>
ALIB_DLL
int64_t   ParseInt( const TString<TChar>& src, integer& idx, const TNumberFormat<TChar>& nf );

//==================================================================================================
/// Reads an unsigned integral value in \b decimal format from the given character
/// array at the given position.<br>
/// Sign literals \c '-' or \c '+' are \b not accepted and parsing will fail if found.
/// Whitespace and grouping characters, as defined in fields
/// #"TNumberFormat::Whitespaces" and
/// #"TNumberFormat::ThousandsGroupChar" of the #"%^NumberFormat" object given with \p{nf}
/// will be ignored (tolerated) regardless on their position between digits. To suppress the parsing
/// of group characters, set field #"TNumberFormat::ThousandsGroupChar" to <c>'\\0'</c>.
/// To suppress whitespace consumption before reading the value, set field
/// #"TNumberFormat::Whitespaces" to \e nulled or empty string.
///
///
/// @tparam        TChar The character type of the string to parse from.
/// @param         src   The string to read the value from.
/// @param[in,out] idx   The start point for parsing within \p{src}. Will be set to point
///                      behind the last character consumed. If unchanged, this indicates
///                      that no parsable number was found.
/// @param         nf    The number format to use.
/// @return The parsed value. In addition, on success, parameter \p{idx} is moved to point to the
///         first character behind the parsed number.
//==================================================================================================
template<typename TChar>
ALIB_DLL
uint64_t  ParseDec( const TString<TChar>& src, integer& idx, const TNumberFormat<TChar>& nf );

//==================================================================================================
/// Reads an unsigned integral value in \b binary format from the given character
/// array at the given position.<br>
/// Sign literals \c '-' or \c '+' are \b not accepted and parsing will fail if found.
/// Whitespace and grouping characters, as defined in fields
/// #"TNumberFormat::Whitespaces",
/// #"TNumberFormat::BinNibbleGroupChar",
/// #"TNumberFormat::BinByteGroupChar"
/// #"TNumberFormat::BinWordGroupChar" and
/// #"TNumberFormat::BinWord32GroupChar" of the #"%^NumberFormat" object given with \p{nf},
/// will be ignored (tolerated), regardless on their position between digits. To suppress the parsing
/// of group characters, set the fields to <c>'\\0'</c>. To suppress whitespace consumption,
/// set field #"TNumberFormat::Whitespaces" to \e nulled or empty string.<br>
///
/// @tparam        TChar The character type of the string to parse from.
/// @param         src   The string to read the value from.
/// @param[in,out] idx   The start point for parsing within \p{src}. Will be set to point
///                      behind the last character consumed. If unchanged, this indicates
///                      that no parsable number was found.
/// @param         nf    The number format to use.
/// @return The parsed value. In addition, on success, parameter \p{idx} is moved to point to the
///         first character behind the parsed number.
//==================================================================================================
template<typename TChar>
ALIB_DLL
uint64_t  ParseBin( const TString<TChar>& src, integer& idx, const TNumberFormat<TChar>& nf );

//==================================================================================================
/// Reads an unsigned integral value in \b hexadecimal format from the given character array at the
/// given position.<br>
/// Sign literals \c '-' or \c '+' are \b not accepted and parsing will fail if found.
/// Whitespace and grouping characters, as defined in fields
/// #"TNumberFormat::Whitespaces",
/// #"TNumberFormat::HexByteGroupChar",
/// #"TNumberFormat::HexWordGroupChar" and
/// #"TNumberFormat::HexWord32GroupChar" of the #"%^NumberFormat" object given with \p{nf},
/// will be ignored (tolerated) regardless of their position between digits. To suppress the parsing
/// of group characters, set the fields to <c>'\\0'</c>. To suppress whitespace consumption,
/// set field #"TNumberFormat::Whitespaces" to \e nulled or empty string.<br>
///
/// Letters 'a' to 'f' are parsed ignoring their case. This is independent of the setting of the
/// flag #"NumberFormatFlags::HexLowerCase" in the field #"TNumberFormat::Flags" of the given
/// \p{nf}.
///
/// @tparam        TChar The character type of the string to parse from.
/// @param         src   The string to read the value from.
/// @param[in,out] idx   The start point for parsing within \p{src}. Will be set to point
///                      behind the last character consumed. If unchanged, this indicates
///                      that no parsable number was found.
/// @param         nf    The number format to use.
/// @return The parsed value. In addition, on success, parameter \p{idx} is moved to point to the
///         first character behind the parsed number.
//==================================================================================================
template<typename TChar>
ALIB_DLL
uint64_t  ParseHex( const TString<TChar>& src, integer& idx, const TNumberFormat<TChar>& nf );

//==================================================================================================
/// Reads an unsigned integral value in \b binary format from the given character array at the
/// given position.<br>
/// Sign literals \c '-' or \c '+' are \b not accepted and parsing will fail if found.
/// Whitespace and grouping characters, as defined in fields
/// #"TNumberFormat::Whitespaces" and
/// #"TNumberFormat::OctGroupChar" of the #"%^NumberFormat" object given with \p{nf},
/// will be ignored (tolerated) regardless of their position between digits. To suppress the parsing
/// of group characters, set the field to <c>'\\0'</c>. To suppress whitespace consumption,
/// set field #"TNumberFormat::Whitespaces" to \e nulled or empty string.<br>
///
/// @tparam        TChar The character type of the string to parse from.
/// @param         src   The string to read the value from.
/// @param[in,out] idx   The start point for parsing within \p{src}. Will be set to point
///                      behind the last character consumed. If unchanged, this indicates
///                      that no parsable number was found.
/// @param         nf    The number format to use.
/// @return The parsed value. In addition, on success, parameter \p{idx} is moved to point to the
///         first character behind the parsed number.
//==================================================================================================
template<typename TChar>
ALIB_DLL
uint64_t  ParseOct( const TString<TChar>& src, integer& idx, const TNumberFormat<TChar>& nf );

//==================================================================================================
/// Reads a floating point value from the given character array at the given position.<br>
/// Sign literals \c '-' or \c '+' are \b not accepted and parsing will fail if found.
///
/// If the strings defined in fields
/// #"TNumberFormat::NANLiteral" and
/// #"TNumberFormat::INFLiteral" are found in the #"%^NumberFormat" object given with
/// \p{nf}, the corresponding double constant (not a number, positive/negative infinity) will be
/// returned.
///
/// @tparam        TChar The character type of the string to parse from.
/// @param         src   The string to read the value from.
/// @param[in,out] idx   The start point for parsing within \p{src}. Will be set to point
///                      behind the last character consumed. If unchanged, this indicates
///                      that no parsable number was found.
/// @param         nf    The number format to use.
/// @return The parsed value. In addition, on success, parameter \p{idx} is moved to point to the
///         first character behind the parsed number.
//==================================================================================================
template<typename TChar>
ALIB_DLL
double ParseFloat( const TString<TChar>& src, integer& idx, const TNumberFormat<TChar>& nf );

//==================================================================================================
/// Converts the given long value to a string representation in decimal format.<br>
///
/// The maximum number of digits written is 20. In addition, grouping characters may be written
/// according the flag #"NumberFormatFlags::WriteGroupChars" and the fields
/// #"ThousandsGroupChar" and #"LeadingGroupCharReplacement"
/// of the given \p{nf}.
///
/// The minimum width of the output is taken from field
/// #"TNumberFormat::DecMinimumFieldWidth" unless overwritten by parameter \p{minWidth}.
/// If the minimum width is greater than the sum of digits and* grouping characters needed to write
/// \p{value}, then \c '0' digits are prepended between the sign
/// and the number.
///
/// \attention
///   The function does not (and cannot) check an overflow of the given character buffer
///   when writing.
///
/// \see
///   Function #".WriteDecSigned" to write signed decimals.
///
/// @tparam TChar    The character type of the string to write into.
/// @param value     The value to write.
/// @param buffer    The character array to write the value to. Needs to be long enough
///                  (after \p{idx}) to carry the string written.
/// @param idx       The index within \p{buffer} to start writing.
/// @param minWidth  The minimum width of the output.
///                  If \c 0, the value of #"%DecMinimumFieldWidth" of argument \p{nf} is used.
/// @param nf        Number format definitions.
/// @return The index pointing to behind the last character written in \p{buffer}.
//==================================================================================================
template<typename TChar>
ALIB_DLL
integer  WriteDecUnsigned( uint64_t value, TChar* buffer, integer idx, int minWidth,
                           const TNumberFormat<TChar>& nf );

//==================================================================================================
/// Converts the given long value to a string representation into a signed decimal format.<br>
/// For negative numbers, \c '-' is written, the sign of positive numbers (if any) depends
/// on field #"TNumberFormat::PlusSign" of the #"%^NumberFormat" object given with \p{nf}.
/// After that, the value is negated (made positive) and #".WriteDecUnsigned" is invoked.
///
/// @tparam TChar    The character type of the string to write into.
/// @param value     The value to write.
/// @param buffer    The character array to write the value to. Needs to be long enough
///                  (after \p{idx}) to carry the string written.
/// @param idx       The index within \p{buffer} to start writing.
/// @param minWidth  The minimum width of the output.
///                  If \c 0, the value of #"%DecMinimumFieldWidth" in \p{nf} is used.
/// @param nf        Number format definitions.
/// @return The index pointing to behind the last character written in \p{buffer}.
//==================================================================================================
template<typename TChar>
ALIB_DLL
integer  WriteDecSigned( int64_t value, TChar* buffer, integer idx, int minWidth,
                         const TNumberFormat<TChar>& nf );

//==================================================================================================
/// Converts the given long value to a string representation in binary format.<br>
///
/// The maximum number of digits written is 64. In addition, grouping characters may be written
/// according the settings of the flags and fields
/// #"NumberFormatFlags::WriteGroupChars",
/// #"TNumberFormat::BinNibbleGroupChar",
/// #"TNumberFormat::BinByteGroupChar",
/// #"TNumberFormat::BinWordGroupChar",
/// #"TNumberFormat::BinWord32GroupChar" and
/// #"TNumberFormat::LeadingGroupCharReplacement"
///
/// of the #"%^NumberFormat" object given with \p{nf}.
///
/// The minimum width of the output is taken from field #"TNumberFormat::BinFieldWidth"
/// unless overwritten by parameter \p{minWidth}. If the width is greater than digits found in
/// \p{value}, \c '0' digits are prepended. The width is taking group characters into account.
///
/// \attention
///   The function does not (and cannot) check an overflow of the given character buffer
///   when writing.
///
/// \attention
///   If the value is greater than can be represented by the output width, these greater
///   digits are cut. This is true for this function as well as for #".WriteHex" and #".WriteOct". The
///   rationale behind this is that this way, larger numbers do not need to be masked before
///   writing.
///   (In other words: it is assumed that there is a reason for providing the width).
///
/// \note
///   The literal prefix found in field #"TNumberFormat::BinLiteralPrefix" of \p{nf}
///   is \b not written. The field is only used for detecting formats with function #"ParseInt".
///
/// @tparam TChar    The character type of the string to write into.
/// @param value     The value to write.
/// @param buffer    The character array to write the value to. Needs to be long enough
///                  (after \p{idx}) to carry the string written.
/// @param idx       The index within \p{buffer} to start writing.
/// @param minWidth  The minimum width of the output.
///                  If \c 0, the value of #"%BinFieldWidth" of argument \p{nf} is used.
/// @param nf        Number format definitions.
/// @return The index pointing to behind the last character written in \p{buffer}.
//==================================================================================================
template<typename TChar>
ALIB_DLL
integer  WriteBin( uint64_t value, TChar* buffer, integer idx, int minWidth,
                   const TNumberFormat<TChar>& nf );


//==================================================================================================
/// Converts the given long value to a string representation in hexadecimal format.<br>
///
/// The maximum number of digits written is \b 16. In addition, grouping characters may be written
/// according the settings of the flags and fields
/// #"NumberFormatFlags::WriteGroupChars",
/// #"TNumberFormat::HexByteGroupChar",
/// #"TNumberFormat::HexWordGroupChar",
/// #"TNumberFormat::HexWord32GroupChar" and
/// #"TNumberFormat::LeadingGroupCharReplacement"
/// of the given \p{nf}.
///
/// The minimum width of the output is taken from field #"TNumberFormat::HexFieldWidth"
/// unless overwritten by the parameter \p{minWidth}. If the width is greater than digits found in
/// \p{value}, \c '0' digits are prepended. The width is taking group characters into account.
///
/// \attention
///   The function does not (and cannot) check an overflow of the given character buffer
///   when writing.
///
/// \attention
///   If the value is greater than can be represented by the output width, these greater
///   digits are cut. This is true for this function as well as #".WriteBin" and writeOct. The
///   rationale behind this is that this way, larger numbers do not need to be masked before
///   writing.
///   (In other words: it is assumed that there is a reason for providing the width).
///
/// \note
///   The literal prefix found in field #"TNumberFormat::HexLiteralPrefix" of \p{nf}
///   is \b not written. The field is only used for detecting formats with function #"ParseInt".
///
/// @tparam TChar    The character type of the string to write into.
/// @param value     The value to write.
/// @param buffer    The character array to write the value to. Needs to be long enough
///                  (after \p{idx}) to carry the string written.
/// @param idx       The index within \p{buffer} to start writing.
/// @param minWidth  The minimum width of the output.
///                  If \c 0, the value of #"%HexFieldWidth" of argument \p{nf} is used.
/// @param nf        Number format definitions.
/// @return The index pointing to behind the last character written in \p{buffer}.
//==================================================================================================
template<typename TChar>
ALIB_DLL
integer  WriteHex( uint64_t value, TChar* buffer, integer idx, int minWidth,
                   const TNumberFormat<TChar>& nf );

//==================================================================================================
/// Converts the given long value to a string representation in octal format.<br>
///
/// The maximum number of digits written is 64. In addition, grouping characters may be written
/// according the settings of the flags and fields
/// #"NumberFormatFlags::WriteGroupChars",
/// #"TNumberFormat::OctGroupChar" and
/// #"TNumberFormat::LeadingGroupCharReplacement"
///
/// of the given \p{nf}.
///
/// The minimum width of the output is taken from field #"TNumberFormat::OctFieldWidth"
/// unless overwritten by parameter \p{minWidth}. If the width is greater than digits found in
/// \p{value}, \c '0' digits are prepended. The width is taking group characters into account.
///
/// \attention
///   The function does not (and cannot) check an overflow of the given character buffer
///   when writing.
///
/// \attention
///   If the value is greater than can be represented by the output width, these greater
///   digits are cut. This is true for this function as well as #".WriteBin" and writeHex. The
///   rationale behind this is that this way, larger numbers do not need to be masked before
///   writing.
///   (In other words: it is assumed that there is a reason for providing the width).
///
/// \note
///   The literal prefix found in field #"TNumberFormat::OctLiteralPrefix" of \p{nf}
///   is \b not written. The field is only used for detecting formats with function #"ParseInt".
///
/// @tparam TChar    The character type of the string to write into.
/// @param value     The value to write.
/// @param buffer    The character array to write the value to. Needs to be long enough
///                  (after \p{idx}) to carry the string written.
/// @param idx       The index within \p{buffer} to start writing.
/// @param minWidth  The minimum width of the output.
///                  If \c 0, the value of #"%OctFieldWidth" of argument \p{nf} is used.
/// @param nf        Number format definitions.
/// @return The index pointing to behind the last character written in \p{buffer}.
//==================================================================================================
template<typename TChar>
ALIB_DLL
integer  WriteOct( uint64_t value, TChar* buffer, integer idx, int minWidth,
                   const TNumberFormat<TChar>& nf );

//==================================================================================================
/// Writes a string representation of the given \c double value.
///
/// Grouping characters are written according to the flags and members
/// #"NumberFormatFlags::WriteGroupChars",
/// #"TNumberFormat::ThousandsGroupChar" and
/// #"TNumberFormat::LeadingGroupCharReplacement":
///
/// The minimum width of the integral part of the output is taken from field
/// #"TNumberFormat::IntegralPartMinimumWidth" unless overwritten by parameter
/// \p{minWidth}.
/// If the width is greater than integral digits found in \p{value}, \c '0' digits are prepended.
/// The width is taking group characters into account.
///
/// If field #"TNumberFormat::FractionalPartWidth" as well as the width of the integral
/// part (provided or set) equals \c -1 the function may choose scientific notation.
/// This is done for numbers smaller than <c>10E-4</c> or larger than <c>10E+6</c>.<br>
///
/// If the given value is not a number, #"TNumberFormat::NANLiteral" is written.
/// If infinite, #"TNumberFormat::INFLiteral".
///
/// The output format is dependent on various further settings provided in the flas and fields
/// of the given \p{nf}. Those are
/// #"NumberFormatFlags::ForceScientific",
/// #"NumberFormatFlags::ForceDecimalPoint",
/// #"NumberFormatFlags::WriteExponentPlusSign",
/// #"TNumberFormat::DecimalPointChar", and
/// #"TNumberFormat::ExponentSeparator".
///
/// \attention
///   The function does not (and cannot) check an overflow of the given character buffer
///   when writing.
///
/// @tparam TChar    The character type of the string to write into.
/// @param value     The value to write.
/// @param buffer    The character array to write the value to. Needs to be long enough
///                  (after \p{idx}) to carry the string written.
/// @param idx       The index within \p{buffer} to start writing.
/// @param minWidth  The minimum width of the integral part of the output.
///                  If \c 0, the value of #"%IntegralPartMinimumWidth" of argument \p{nf} is used.
/// @param nf        Number format definitions.
/// @return The index pointing to behind the last character written in \p{buffer}.
//==================================================================================================
template<typename TChar>
ALIB_DLL
integer  WriteFloat( double value, TChar*  buffer, integer idx, int minWidth,
                     const TNumberFormat<TChar>& nf );


//##################################################################################################
// Template instantiation declarations
//##################################################################################################
#if !DOXYGEN

extern template ALIB_DLL uint64_t ParseDecDigits  <nchar>( const TString<nchar>&, integer& );
extern template ALIB_DLL  int64_t ParseInt        <nchar>( const TString<nchar>&, integer&, const TNumberFormat<nchar>& );
extern template ALIB_DLL uint64_t ParseDec        <nchar>( const TString<nchar>&, integer&, const TNumberFormat<nchar>& );
extern template ALIB_DLL uint64_t ParseBin        <nchar>( const TString<nchar>&, integer&, const TNumberFormat<nchar>& );
extern template ALIB_DLL uint64_t ParseHex        <nchar>( const TString<nchar>&, integer&, const TNumberFormat<nchar>& );
extern template ALIB_DLL uint64_t ParseOct        <nchar>( const TString<nchar>&, integer&, const TNumberFormat<nchar>& );
extern template ALIB_DLL double   ParseFloat      <nchar>( const TString<nchar>&, integer&, const TNumberFormat<nchar>& );
extern template ALIB_DLL integer  WriteDecUnsigned<nchar>( uint64_t, nchar*, integer, int,  const TNumberFormat<nchar>& );
extern template ALIB_DLL integer  WriteDecSigned  <nchar>( int64_t , nchar*, integer, int,  const TNumberFormat<nchar>& );
extern template ALIB_DLL integer  WriteBin        <nchar>( uint64_t, nchar*, integer, int,  const TNumberFormat<nchar>& );
extern template ALIB_DLL integer  WriteHex        <nchar>( uint64_t, nchar*, integer, int,  const TNumberFormat<nchar>& );
extern template ALIB_DLL integer  WriteOct        <nchar>( uint64_t, nchar*, integer, int,  const TNumberFormat<nchar>& );
extern template ALIB_DLL integer  WriteFloat      <nchar>( double  , nchar*, integer, int,  const TNumberFormat<nchar>& );

extern template ALIB_DLL uint64_t ParseDecDigits  <wchar>( const TString<wchar>&, integer& );
extern template ALIB_DLL  int64_t ParseInt        <wchar>( const TString<wchar>&, integer&, const TNumberFormat<wchar>& );
extern template ALIB_DLL uint64_t ParseDec        <wchar>( const TString<wchar>&, integer&, const TNumberFormat<wchar>& );
extern template ALIB_DLL uint64_t ParseBin        <wchar>( const TString<wchar>&, integer&, const TNumberFormat<wchar>& );
extern template ALIB_DLL uint64_t ParseHex        <wchar>( const TString<wchar>&, integer&, const TNumberFormat<wchar>& );
extern template ALIB_DLL uint64_t ParseOct        <wchar>( const TString<wchar>&, integer&, const TNumberFormat<wchar>& );
extern template ALIB_DLL double   ParseFloat      <wchar>( const TString<wchar>&, integer&, const TNumberFormat<wchar>& );
extern template ALIB_DLL integer  WriteDecUnsigned<wchar>( uint64_t, wchar*, integer, int,  const TNumberFormat<wchar>& );
extern template ALIB_DLL integer  WriteDecSigned  <wchar>( int64_t , wchar*, integer, int,  const TNumberFormat<wchar>& );
extern template ALIB_DLL integer  WriteBin        <wchar>( uint64_t, wchar*, integer, int,  const TNumberFormat<wchar>& );
extern template ALIB_DLL integer  WriteHex        <wchar>( uint64_t, wchar*, integer, int,  const TNumberFormat<wchar>& );
extern template ALIB_DLL integer  WriteOct        <wchar>( uint64_t, wchar*, integer, int,  const TNumberFormat<wchar>& );
extern template ALIB_DLL integer  WriteFloat      <wchar>( double  , wchar*, integer, int,  const TNumberFormat<wchar>& );

extern template ALIB_DLL uint64_t ParseDecDigits  <xchar>( const TString<xchar>&, integer& );
extern template ALIB_DLL  int64_t ParseInt        <xchar>( const TString<xchar>&, integer&, const TNumberFormat<xchar>& );
extern template ALIB_DLL uint64_t ParseDec        <xchar>( const TString<xchar>&, integer&, const TNumberFormat<xchar>& );
extern template ALIB_DLL uint64_t ParseBin        <xchar>( const TString<xchar>&, integer&, const TNumberFormat<xchar>& );
extern template ALIB_DLL uint64_t ParseHex        <xchar>( const TString<xchar>&, integer&, const TNumberFormat<xchar>& );
extern template ALIB_DLL uint64_t ParseOct        <xchar>( const TString<xchar>&, integer&, const TNumberFormat<xchar>& );
extern template ALIB_DLL double   ParseFloat      <xchar>( const TString<xchar>&, integer&, const TNumberFormat<xchar>& );
extern template ALIB_DLL integer  WriteDecUnsigned<xchar>( uint64_t, xchar*, integer, int,  const TNumberFormat<xchar>& );
extern template ALIB_DLL integer  WriteDecSigned  <xchar>( int64_t , xchar*, integer, int,  const TNumberFormat<xchar>& );
extern template ALIB_DLL integer  WriteBin        <xchar>( uint64_t, xchar*, integer, int,  const TNumberFormat<xchar>& );
extern template ALIB_DLL integer  WriteHex        <xchar>( uint64_t, xchar*, integer, int,  const TNumberFormat<xchar>& );
extern template ALIB_DLL integer  WriteOct        <xchar>( uint64_t, xchar*, integer, int,  const TNumberFormat<xchar>& );
extern template ALIB_DLL integer  WriteFloat      <xchar>( double  , xchar*, integer, int,  const TNumberFormat<xchar>& );
#endif //!DOXYGEN


}}} // namespace [alib::strings::detail]
