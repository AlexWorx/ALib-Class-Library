// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#ifndef HPP_ALIB_STRINGS_DETAIL_NUMBERCONVERSION
#define HPP_ALIB_STRINGS_DETAIL_NUMBERCONVERSION 1

#if !defined (HPP_ALIB_STRINGS_NUMBERFORMAT)
#   include "alib/strings/numberformat.hpp"
#endif

namespace aworx { namespace lib { namespace strings { namespace detail {


// clang 7.0.1 falsely reports:
// "warning: '@tparam' command used in a comment that is not attached to a template declaration
#if !ALIB_DOCUMENTATION_PARSER
    ALIB_WARNINGS_IGNORE_DOCS
#endif

/** ************************************************************************************************
* Reads digits \c '0' to \c '9' into a positive integer value.
 *
 * \note
 *   The function is very basic, i.e. it does not use an object of type
 *   \alib{strings,TNumberFormat}, does not tolerate group characters and so forth.
 *   Instead, it simply reads decimal digits, until a non-digit character
 *   is found or the string ends.
 *
 * @param         src   The string to read the value from.
 * @param[in,out] idx   The start point for parsing within \p{src}. Will be set to point
 *                      behind the last character consumed. If unchanged, this indicates
 *                      that no parsable number was found. If out of bounds, \c 0 is returned.
 * @tparam        TChar The character type of the string to parse from.
 * @return The parsed value. In addition, on success, parameter \p{idx} is moved to point to the
 *         first character behind the parsed number.
 **************************************************************************************************/
template<typename TChar>
uint64_t  ParseDecDigits( const TString<TChar>& src, integer& idx );


/** ************************************************************************************************
* Parses signed integer numbers, optionally in binary, hexadecimal or octal format.
 *
 * Leading characters defined in field \alib{strings::TNumberFormat,Whitespaces} of the
 * \b NumberFormat object given with \p{nf} are ignored.
 * An optional sign character \c '+' or \c '-' is parsed. If found, again whitespace characters
 * may follow behind such sign and are ignored.
 *
 * Then, the function detects any literal prefixes as defined in fields
 * \alib{strings::TNumberFormat,BinLiteralPrefix},
 * \alib{strings::TNumberFormat,HexLiteralPrefix} and
 * \alib{strings::TNumberFormat,OctLiteralPrefix} (usually \c 0b, \c 0x and \c 0o) and
 * invokes one of the functions #ParseDec, #ParseBin, #ParseHex or #ParseOct.
 *
 * @param         src   The string to read the value from.
 * @param[in,out] idx   The start point for parsing within \p{src}. Will be set to point
 *                      behind the last character consumed. If unchanged, this indicates
 *                      that no parsable number was found.
 * @param         nf    The number format to use.
 * @tparam        TChar The character type of the string to parse from.
 * @return The parsed value. In addition, on success, parameter \p{idx} is moved to point to the
 *         first character behind the parsed number.
 **************************************************************************************************/
template<typename TChar>
ALIB_API
int64_t   ParseInt( const TString<TChar>& src, integer& idx, const TNumberFormat<TChar>& nf );

/** ************************************************************************************************
* Reads an unsigned integer value in \b decimal format from the given character
 * array at the given position.<br>
 * Sign literals \c '-' or \c '+' are \b not accepted and parsing will fail if found.
 * Whitespace and grouping characters, as defined in fields
 * \alib{strings::TNumberFormat,Whitespaces} and
 * \alib{strings::TNumberFormat,ThousandsGroupChar} of the \b NumberFormat object given with \p{nf}
 * will be ignored (tolerated) regardless on their position between digits. To suppress the parsing
 * of group characters, set field \alib{strings::TNumberFormat,ThousandsGroupChar} to <c>'\\0'</c>.
 * To suppress whitespace consumption before reading the value, set field
 * \alib{strings::TNumberFormat,Whitespaces} to \e nulled or empty string.
 *
 *
 * @param         src   The string to read the value from.
 * @param[in,out] idx   The start point for parsing within \p{src}. Will be set to point
 *                      behind the last character consumed. If unchanged, this indicates
 *                      that no parsable number was found.
 * @param         nf    The number format to use.
 * @tparam        TChar The character type of the string to parse from.
 * @return The parsed value. In addition, on success, parameter \p{idx} is moved to point to the
 *         first character behind the parsed number.
 **************************************************************************************************/
template<typename TChar>
ALIB_API
uint64_t  ParseDec( const TString<TChar>& src, integer& idx, const TNumberFormat<TChar>& nf );

/** ************************************************************************************************
* Reads an unsigned integer value in \b binary format from the given character
 * array at the given position.<br>
 * Sign literals \c '-' or \c '+' are \b not accepted and parsing will fail if found.
 * Whitespace and grouping characters, as defined in fields
 * \alib{strings::TNumberFormat,Whitespaces},
 * \alib{strings::TNumberFormat,BinNibbleGroupChar},
 * \alib{strings::TNumberFormat,BinByteGroupChar}
 * \alib{strings::TNumberFormat,BinWordGroupChar} and
 * \alib{strings::TNumberFormat,BinWord32GroupChar} of the \b NumberFormat object given with \p{nf},
 * will be ignored (tolerated), regardless on their position between digits. To suppress the parsing
 * of group characters, set the fields to <c>'\\0'</c>. To suppress whitespace consumption,
 * set field \alib{strings::TNumberFormat,Whitespaces} to \e nulled or empty string.<br>
 *
 * @param         src   The string to read the value from.
 * @param[in,out] idx   The start point for parsing within \p{src}. Will be set to point
 *                      behind the last character consumed. If unchanged, this indicates
 *                      that no parsable number was found.
 * @param         nf    The number format to use.
 * @tparam        TChar The character type of the string to parse from.
 * @return The parsed value. In addition, on success, parameter \p{idx} is moved to point to the
 *         first character behind the parsed number.
 **************************************************************************************************/
template<typename TChar>
ALIB_API
uint64_t  ParseBin( const TString<TChar>& src, integer& idx, const TNumberFormat<TChar>& nf );

/** ************************************************************************************************
* Reads an unsigned integer value in \b hexadecimal format from the given character
 * array at the given position.<br>
 * Sign literals \c '-' or \c '+' are \b not accepted and parsing will fail if found.
 * Whitespace and grouping characters, as defined in fields
 * \alib{strings::TNumberFormat,Whitespaces},
 * \alib{strings::TNumberFormat,HexByteGroupChar},
 * \alib{strings::TNumberFormat,HexWordGroupChar} and
 * \alib{strings::TNumberFormat,HexWord32GroupChar} of the \b NumberFormat object given with \p{nf},
 * will be ignored (tolerated) regardless of their position between digits. To suppress the parsing
 * of group characters, set the fields to <c>'\\0'</c>. To suppress whitespace consumption,
 * set field \alib{strings::TNumberFormat,Whitespaces} to \e nulled or empty string.<br>
 *
 * Letters 'a' to 'f' are parsed ignoring their case. This is independent from the setting of field
 * \alib{strings::TNumberFormat,HexLowerCase}.
 *
 * @param         src   The string to read the value from.
 * @param[in,out] idx   The start point for parsing within \p{src}. Will be set to point
 *                      behind the last character consumed. If unchanged, this indicates
 *                      that no parsable number was found.
 * @param         nf    The number format to use.
 * @tparam        TChar The character type of the string to parse from.
 * @return The parsed value. In addition, on success, parameter \p{idx} is moved to point to the
 *         first character behind the parsed number.
 **************************************************************************************************/
template<typename TChar>
ALIB_API
uint64_t  ParseHex( const TString<TChar>& src, integer& idx, const TNumberFormat<TChar>& nf );

/** ************************************************************************************************
* Reads an unsigned integer value in \b binary format from the given character
 * array at the given position.<br>
 * Sign literals \c '-' or \c '+' are \b not accepted and parsing will fail if found.
 * Whitespace and grouping characters, as defined in fields
 * \alib{strings::TNumberFormat,Whitespaces} and
 * \alib{strings::TNumberFormat,OctGroupChar} of the \b NumberFormat object given with \p{nf},
 * will be ignored (tolerated) regardless of their position between digits. To suppress the parsing
 * of group characters, set the field to <c>'\\0'</c>. To suppress whitespace consumption,
 * set field \alib{strings::TNumberFormat,Whitespaces} to \e nulled or empty string.<br>
 *
 * @param         src   The string to read the value from.
 * @param[in,out] idx   The start point for parsing within \p{src}. Will be set to point
 *                      behind the last character consumed. If unchanged, this indicates
 *                      that no parsable number was found.
 * @param         nf    The number format to use.
 * @tparam        TChar The character type of the string to parse from.
 * @return The parsed value. In addition, on success, parameter \p{idx} is moved to point to the
 *         first character behind the parsed number.
 **************************************************************************************************/
template<typename TChar>
ALIB_API
uint64_t  ParseOct( const TString<TChar>& src, integer& idx, const TNumberFormat<TChar>& nf );

/** ************************************************************************************************
* Reads a floating point value from the given character array at the given position.<br>
 * Sign literals \c '-' or \c '+' are \b not accepted and parsing will fail if found.
 *
 * If the strings defined in fields
 * \alib{strings::TNumberFormat,NANLiteral} and
 * \alib{strings::TNumberFormat,INFLiteral} are found in the \b NumberFormat object given with
 * \p{nf}, the corresponding double constant (not a number, positive/negative infinity) will be
 * returned.
 *
 * @param         src   The string to read the value from.
 * @param[in,out] idx   The start point for parsing within \p{src}. Will be set to point
 *                      behind the last character consumed. If unchanged, this indicates
 *                      that no parsable number was found.
 * @param         nf    The number format to use.
 * @tparam        TChar The character type of the string to parse from.
 * @return The parsed value. In addition, on success, parameter \p{idx} is moved to point to the
 *         first character behind the parsed number.
 **************************************************************************************************/
template<typename TChar>
ALIB_API
double ParseFloat( const TString<TChar>& src, integer& idx, const TNumberFormat<TChar>& nf );

/** ************************************************************************************************
* Converts the given long value to a string representation in decimal format.<br>
 *
 * The maximum number of digits written is 20. In addition, grouping characters may be written
 * according the settings of fields
 * \alib{strings::TNumberFormat,WriteGroupChars},
 * \alib{strings::TNumberFormat,ThousandsGroupChar} and
 * \alib{strings::TNumberFormat,LeadingGroupCharReplacement} of the \b NumberFormat object given
 * with \p{nf}.
 *
 * The minimum width of the output is taken from field
 * \alib{strings::TNumberFormat,DecMinimumFieldWidth} unless overwritten by parameter \p{minWidth}.
 * If the minimum width is greater than the sum of digits and* grouping characters needed to write
 * \p{value}, then \c '0' digits are prepended between the sign
 * and the number.
 *
 * \attention
 *   The function does not (and can not) check an overflow of the given character buffer
 *   when writing.
 *
 * \see
 *   Function #WriteDecSigned to write signed decimals.
 *
 * @param value     The value to write.
 * @param buffer    The character array to write the value to. Needs to be long enough
 *                  (after \p{idx}) to carry the string written.
 * @param idx       The index within \p{buffer} to start writing.
 * @param minWidth  The minimum width of the output.
 *                  If \c 0, the value of \b DecMinimumFieldWidth of argument \p{nf} is used.
 * @param nf        Number format definitions.
 * @tparam TChar    The character type of the string to write into.
 * @return  The index pointing to behind the last character written in \b buffer.
 **************************************************************************************************/
template<typename TChar>
ALIB_API
integer  WriteDecUnsigned( uint64_t value, TChar* buffer, integer idx, int minWidth,
                           const TNumberFormat<TChar>& nf );

/** ************************************************************************************************
* Converts the given long value to a string representation into a signed decimal format.<br>
 * For negative numbers, \c '-' is written, the sign of positive numbers (if any) depends
 * on field \alib{strings::TNumberFormat,PlusSign} of the \b NumberFormat object given with \p{nf}.
 * After that, the value is negated (made positive) and #WriteDecUnsigned is invoked.
 *
 * @param value     The value to write.
 * @param buffer    The character array to write the value to. Needs to be long enough
 *                  (after \p{idx}) to carry the string written.
 * @param idx       The index within \p{buffer} to start writing.
 * @param minWidth  The minimum width of the output.
 *                  If \c 0, the value of \b DecMinimumFieldWidth in \p{nf} is used.
 * @param nf        Number format definitions.
 * @tparam TChar    The character type of the string to write into.
 * @return  The index pointing to behind the last character written in \b buffer.
 **************************************************************************************************/
template<typename TChar>
ALIB_API
integer  WriteDecSigned( int64_t value, TChar* buffer, integer idx, int minWidth,
                         const TNumberFormat<TChar>& nf );

/** ************************************************************************************************
* Converts the given long value to a string representation in binary format.<br>
 *
 * The maximum number of digits written is 64. In addition, grouping characters may be written
 * according the settings of fields
 * \alib{strings::TNumberFormat,WriteGroupChars},
 * \alib{strings::TNumberFormat,BinNibbleGroupChar},
 * \alib{strings::TNumberFormat,BinByteGroupChar},
 * \alib{strings::TNumberFormat,BinWordGroupChar},
 * \alib{strings::TNumberFormat,BinWord32GroupChar} and
 * \alib{strings::TNumberFormat,LeadingGroupCharReplacement} of the \b NumberFormat object given
 * with \p{nf}.
 *
 * The minimum width of the output is taken from field \alib{strings::TNumberFormat,BinFieldWidth}
 *  unless overwritten by parameter \p{minWidth}. If the width is greater than digits found in
 *  \p{value}, \c '0' digits are prepended. The width is taking group characters into account.
 *
 * \attention
 *   The function does not (and can not) check an overflow of the given character buffer
 *   when writing.
 *
 * \attention
 *  If the value is greater than can be represented by the output width, these greater
 *  digits are cut. This is true for this function as well as for #WriteHex and #WriteOct. The
 *  rationale behind this is that this way, larger numbers do not need to be masked before
 *  writing.
 *  (In other words: it is assumed that there is a reason for providing the width).
 *
 * \note
 *   The literal prefix found in field \alib{strings::TNumberFormat,BinLiteralPrefix} of \p{nf}
 *   is \b not written. The field is only used for detecting formats with function #ParseInt.
 *
 * @param value     The value to write.
 * @param buffer    The character array to write the value to. Needs to be long enough
 *                  (after \p{idx}) to carry the string written.
 * @param idx       The index within \p{buffer} to start writing.
 * @param minWidth  The minimum width of the output.
 *                  If \c 0, the value of \b BinFieldWidth of argument \p{nf} is used.
 * @param nf        Number format definitions.
 * @tparam TChar    The character type of the string to write into.
 * @return  The index pointing to behind the last character written in \b buffer.
 **************************************************************************************************/
template<typename TChar>
ALIB_API
integer  WriteBin( uint64_t value, TChar* buffer, integer idx, int minWidth,
                   const TNumberFormat<TChar>& nf );


/** ************************************************************************************************
* Converts the given long value to a string representation in hexadecimal format.<br>
 *
 * The maximum number of digits written is \b 16. In addition, grouping characters may be written
 * according the settings of fields
 * \alib{strings::TNumberFormat,WriteGroupChars},
 * \alib{strings::TNumberFormat,HexByteGroupChar},
 * \alib{strings::TNumberFormat,HexWordGroupChar},
 * \alib{strings::TNumberFormat,HexWord32GroupChar} and
 * \alib{strings::TNumberFormat,LeadingGroupCharReplacement} of the \b NumberFormat object given
 * with \p{nf}.
 *
 * The minimum width of the output is taken from field \alib{strings::TNumberFormat,HexFieldWidth}
 * unless overwritten by parameter \p{minWidth}. If the width is greater than digits found in
 * \p{value}, \c '0' digits are prepended. The width is taking group characters into account.
 *
 * \attention
 *   The function does not (and can not) check an overflow of the given character buffer
 *   when writing.
 *
 * \attention
 *  If the value is greater than can be represented by the output width, these greater
 *  digits are cut. This is true for this function as well as #WriteBin and writeOct. The
 *  rationale behind this is that this way, larger numbers do not need to be masked before
 *  writing.
 *  (In other words: it is assumed that there is a reason for providing the width).
 *
 * \note
 *   The literal prefix found in field \alib{strings::TNumberFormat,HexLiteralPrefix} of \p{nf}
 *   is \b not written. The field is only used for detecting formats with function #ParseInt.
 *
 * @param value     The value to write.
 * @param buffer    The character array to write the value to. Needs to be long enough
 *                  (after \p{idx}) to carry the string written.
 * @param idx       The index within \p{buffer} to start writing.
 * @param minWidth  The minimum width of the output.
 *                  If \c 0, the value of \b HexFieldWidth of argument \p{nf} is used.
 * @param nf        Number format definitions.
 * @tparam TChar    The character type of the string to write into.
 * @return  The index pointing to behind the last character written in \b buffer.
 **************************************************************************************************/
template<typename TChar>
ALIB_API
integer  WriteHex( uint64_t value, TChar* buffer, integer idx, int minWidth,
                   const TNumberFormat<TChar>& nf );

/** ************************************************************************************************
* Converts the given long value to a string representation in octal format.<br>
 *
 * The maximum number of digits written is 64. In addition, grouping characters may be written
 * according the settings of fields
 * \alib{strings::TNumberFormat,WriteGroupChars},
 * \alib{strings::TNumberFormat,OctGroupChar} and
 * \alib{strings::TNumberFormat,LeadingGroupCharReplacement}.
 *
 * The minimum width of the output is taken from field
 * \alib{strings::TNumberFormat,OctFieldWidth} unless overwritten by parameter \p{minWidth}.
 * If the width is greater than digits found in \p{value}, \c '0' digits are prepended.
 * The width is taking group characters into account.
 *
 * \attention
 *   The function does not (and can not) check an overflow of the given character buffer
 *   when writing.
 *
 * \attention
 *  If the value is greater than can be represented by the output width, these greater
 *  digits are cut. This is true for this function as well as #WriteBin and writeHex. The
 *  rationale behind this is that this way, larger numbers do not need to be masked before
 *  writing.
 *  (In other words: it is assumed that there is a reason for providing the width).
 *
 * \note
 *   The literal prefix found in field \alib{strings::TNumberFormat,OctLiteralPrefix} of \p{nf}
 *   is \b not written. The field is only used for detecting formats with function #ParseInt.
 *
 * @param value     The value to write.
 * @param buffer    The character array to write the value to. Needs to be long enough
 *                  (after \p{idx}) to carry the string written.
 * @param idx       The index within \p{buffer} to start writing.
 * @param minWidth  The minimum width of the output.
 *                  If \c 0, the value of \b OctFieldWidth of argument \p{nf} is used.
 * @param nf        Number format definitions.
 * @tparam TChar    The character type of the string to write into.
 * @return  The index pointing to behind the last character written in \b buffer.
 **************************************************************************************************/
template<typename TChar>
ALIB_API
integer  WriteOct( uint64_t value, TChar* buffer, integer idx, int minWidth,
                   const TNumberFormat<TChar>& nf );

/** ************************************************************************************************
* Writes a string representation of the given \c double value.
 *
 * Grouping characters are written according the settings of fields
 * \alib{strings::TNumberFormat,WriteGroupChars},
 * \alib{strings::TNumberFormat,ThousandsGroupChar} and
 * \alib{strings::TNumberFormat,LeadingGroupCharReplacement} of the \b NumberFormat object given
 * with \p{nf}.
 *
 * The minimum width of the integral part of the output is taken from field
 * \alib{strings::TNumberFormat,IntegralPartMinimumWidth} unless overwritten by parameter
 * \p{minWidth}.
 * If the width is greater than integral digits found in \p{value}, \c '0' digits are prepended.
 * The width is taking group characters into account.
 *
 * If field \alib{strings::TNumberFormat,FractionalPartWidth} as well as the width of the integral
 * part (provided or set) equals \c -1 the function may choose scientific notation.
 * This is done for numbers smaller than <c>10E-4</c> or larger than <c>10E+6</c>.<br>
 *
 * If the given value is not a number, \alib{strings::TNumberFormat,NANLiteral} is written.
 * If infinite, \alib{strings::TNumberFormat,INFLiteral}.
 *
 * The output format is dependent on various further settings provided in the fields of parameter
 * \p{nf}. Those are
 * \alib{strings::TNumberFormat,DecimalPointChar},
 * \alib{strings::TNumberFormat,ExponentSeparator},
 * \alib{strings::TNumberFormat,ForceDecimalPoint},
 * \alib{strings::TNumberFormat,WriteExponentPlusSign} and
 * \alib{strings::TNumberFormat,ForceScientific}.
 *
 * \attention
 *   The function does not (and can not) check an overflow of the given character buffer
 *   when writing.
 *
 * @param value     The value to write.
 * @param buffer    The character array to write the value to. Needs to be long enough
 *                  (after \p{idx}) to carry the string written.
 * @param idx       The index within \p{buffer} to start writing.
 * @param minWidth  The minimum width of the integral part of the output.
 *                  If \c 0, the value of \b IntegralPartMinimumWidth of argument \p{nf} is used.
 * @param nf        Number format definitions.
 * @tparam TChar    The character type of the string to write into.
 * @return  The index pointing to behind the last character written in \b buffer.
 **************************************************************************************************/
template<typename TChar>
ALIB_API
integer  WriteFloat( double value, TChar*  buffer, integer idx, int minWidth,
                     const TNumberFormat<TChar>& nf );

ALIB_WARNINGS_RESTORE



// #################################################################################################
// Template instantiation declarations
// #################################################################################################
#if !ALIB_DOCUMENTATION_PARSER

extern template ALIB_API uint64_t ParseDecDigits  <nchar>( const TString<nchar>&, integer& );
extern template ALIB_API  int64_t ParseInt        <nchar>( const TString<nchar>&, integer&, const TNumberFormat<nchar>& );
extern template ALIB_API uint64_t ParseDec        <nchar>( const TString<nchar>&, integer&, const TNumberFormat<nchar>& );
extern template ALIB_API uint64_t ParseBin        <nchar>( const TString<nchar>&, integer&, const TNumberFormat<nchar>& );
extern template ALIB_API uint64_t ParseHex        <nchar>( const TString<nchar>&, integer&, const TNumberFormat<nchar>& );
extern template ALIB_API uint64_t ParseOct        <nchar>( const TString<nchar>&, integer&, const TNumberFormat<nchar>& );
extern template ALIB_API double   ParseFloat      <nchar>( const TString<nchar>&, integer&, const TNumberFormat<nchar>& );
extern template ALIB_API integer  WriteDecUnsigned<nchar>( uint64_t, nchar*, integer, int,  const TNumberFormat<nchar>& );
extern template ALIB_API integer  WriteDecSigned  <nchar>( int64_t , nchar*, integer, int,  const TNumberFormat<nchar>& );
extern template ALIB_API integer  WriteBin        <nchar>( uint64_t, nchar*, integer, int,  const TNumberFormat<nchar>& );
extern template ALIB_API integer  WriteHex        <nchar>( uint64_t, nchar*, integer, int,  const TNumberFormat<nchar>& );
extern template ALIB_API integer  WriteOct        <nchar>( uint64_t, nchar*, integer, int,  const TNumberFormat<nchar>& );
extern template ALIB_API integer  WriteFloat      <nchar>( double  , nchar*, integer, int,  const TNumberFormat<nchar>& );

extern template ALIB_API uint64_t ParseDecDigits  <wchar>( const TString<wchar>&, integer& );
extern template ALIB_API  int64_t ParseInt        <wchar>( const TString<wchar>&, integer&, const TNumberFormat<wchar>& );
extern template ALIB_API uint64_t ParseDec        <wchar>( const TString<wchar>&, integer&, const TNumberFormat<wchar>& );
extern template ALIB_API uint64_t ParseBin        <wchar>( const TString<wchar>&, integer&, const TNumberFormat<wchar>& );
extern template ALIB_API uint64_t ParseHex        <wchar>( const TString<wchar>&, integer&, const TNumberFormat<wchar>& );
extern template ALIB_API uint64_t ParseOct        <wchar>( const TString<wchar>&, integer&, const TNumberFormat<wchar>& );
extern template ALIB_API double   ParseFloat      <wchar>( const TString<wchar>&, integer&, const TNumberFormat<wchar>& );
extern template ALIB_API integer  WriteDecUnsigned<wchar>( uint64_t, wchar*, integer, int,  const TNumberFormat<wchar>& );
extern template ALIB_API integer  WriteDecSigned  <wchar>( int64_t , wchar*, integer, int,  const TNumberFormat<wchar>& );
extern template ALIB_API integer  WriteBin        <wchar>( uint64_t, wchar*, integer, int,  const TNumberFormat<wchar>& );
extern template ALIB_API integer  WriteHex        <wchar>( uint64_t, wchar*, integer, int,  const TNumberFormat<wchar>& );
extern template ALIB_API integer  WriteOct        <wchar>( uint64_t, wchar*, integer, int,  const TNumberFormat<wchar>& );
extern template ALIB_API integer  WriteFloat      <wchar>( double  , wchar*, integer, int,  const TNumberFormat<wchar>& );

extern template ALIB_API uint64_t ParseDecDigits  <xchar>( const TString<xchar>&, integer& );
extern template ALIB_API  int64_t ParseInt        <xchar>( const TString<xchar>&, integer&, const TNumberFormat<xchar>& );
extern template ALIB_API uint64_t ParseDec        <xchar>( const TString<xchar>&, integer&, const TNumberFormat<xchar>& );
extern template ALIB_API uint64_t ParseBin        <xchar>( const TString<xchar>&, integer&, const TNumberFormat<xchar>& );
extern template ALIB_API uint64_t ParseHex        <xchar>( const TString<xchar>&, integer&, const TNumberFormat<xchar>& );
extern template ALIB_API uint64_t ParseOct        <xchar>( const TString<xchar>&, integer&, const TNumberFormat<xchar>& );
extern template ALIB_API double   ParseFloat      <xchar>( const TString<xchar>&, integer&, const TNumberFormat<xchar>& );
extern template ALIB_API integer  WriteDecUnsigned<xchar>( uint64_t, xchar*, integer, int,  const TNumberFormat<xchar>& );
extern template ALIB_API integer  WriteDecSigned  <xchar>( int64_t , xchar*, integer, int,  const TNumberFormat<xchar>& );
extern template ALIB_API integer  WriteBin        <xchar>( uint64_t, xchar*, integer, int,  const TNumberFormat<xchar>& );
extern template ALIB_API integer  WriteHex        <xchar>( uint64_t, xchar*, integer, int,  const TNumberFormat<xchar>& );
extern template ALIB_API integer  WriteOct        <xchar>( uint64_t, xchar*, integer, int,  const TNumberFormat<xchar>& );
extern template ALIB_API integer  WriteFloat      <xchar>( double  , xchar*, integer, int,  const TNumberFormat<xchar>& );
#endif //!ALIB_DOCUMENTATION_PARSER


}}}} // namespace [aworx::lib::strings::detail]


#endif // HPP_ALIB_STRINGS_DETAIL_NUMBERCONVERSION
