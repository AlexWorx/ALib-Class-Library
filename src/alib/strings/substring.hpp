// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#ifndef HPP_ALIB_STRINGS_SUBSTRING
#define HPP_ALIB_STRINGS_SUBSTRING 1

#if !defined (HPP_ALIB_STRINGS_CSTRING)
#   include "alib/strings/cstring.hpp"
#endif

#if !defined (HPP_ALIB_STRINGS_ATRING)
#   include "alib/strings/astring.hpp"
#endif

#if ALIB_MODULE_RESOURCES
#   if !defined (HPP_ALIB_RESOURCES_ENUM_META_DATA)
#      include "alib/resources/enummetadata.hpp"
#   endif
#   if !defined (HPP_ALIB_RESOURCES_ENUM_META_DATA_SPECIFICATION)
#      include "alib/resources/enummetadataspec.hpp"
#   endif
#endif

#if ALIB_MODULE_ENUMS
#   if !defined(HPP_ALIB_ENUMS_ENUM_BITWISE)
#      include "alib/enums/enumbitwise.hpp"
#   endif
#endif


namespace aworx { namespace lib { namespace strings {

/** ************************************************************************************************
 * This class specializes parent class \alib{strings,TString,String} to allow reduction of
 * the length of the represented string by cutting characters from the front or the end.
 * Such reduction does not affect the character array represented, but only its representation
 * by instances of this type.
 *
 * In other words, the difference to base class \alib{strings,TString,String} is, that this type
 * allows to increase the pointer to the character array's start and to decrease its stored length.
 * In all other respects, this class has the same lightweight nature and performance as its base.
 * Furthermore, the flexible TMP mechanics for seamless construction is exposed from the base
 * class and likewise available.
 *
 * Like base class \alib{strings,TString,String}, the class can not, and therefore does not,
 * verify that the underlying buffer is (still) properly allocated and contains valid data.
 * It is up to the user of this class to make sure the buffer stays intact until any referencing
 * object of this type is disposed.
 *
 * @tparam TChar The character type.<br>
 *   Alias names for specializations of this class using character types
 *   \alib{characters,character}, \alib{characters,nchar}, \alib{characters,wchar},
 *   \alib{characters,xchar}, \alib{characters,complementChar} and \alib{characters,strangeChar}
 *   are provided in namespace #aworx with type definitions \aworx{Substring}, \aworx{NSubstring},
 *   \aworx{WSubstring}, \aworx{XSubstring}, \aworx{ComplementSubstring} and
 *   \aworx{StrangeSubstring}.
 **************************************************************************************************/
template<typename TChar>
class TSubstring : public TString<TChar>
{
    public:
    #if !ALIB_DOCUMENTATION_PARSER
        // Import parent constructors
        // Due to a doxygen bug in 1.8.14, we must not tell doxygen that we import overloaded methods.
        using TString<TChar>::TString;
    #endif

        /** ****************************************************************************************
         * Default constructor creating a \ref alib_strings_details_nulled \e "nulled" sub-string.
         ******************************************************************************************/
        inline
        TSubstring()
        : TString<TChar>()
        {}

        /** ****************************************************************************************
         * Constructor using a string reference.
         * @param src   The source string.
         ******************************************************************************************/
        inline
        TSubstring( const TString<TChar>& src )
        : TString<TChar>(src)
        {}

        /** ****************************************************************************************
         * Sets this object to zero length.
         * @return \c *this to allow concatenated calls.
         ******************************************************************************************/
        inline
        TSubstring&  Clear()
        {
            TString<TChar>::length=  0;
            return *this;
        }

        /** ****************************************************************************************
         * Moves the start to the first character not found in given character set \p{whiteSpaces}.
         *
         * @param whiteSpaces  The characters used for trimming.
         *                     Defaults to  \ref aworx::DefaultWhitespaces
         * @return \c *this to allow concatenated calls.
         ******************************************************************************************/
        inline
        TSubstring&  TrimStart( const TCString<TChar>& whiteSpaces
                                                    = TT_StringConstants<TChar>::DefaultWhitespaces() )
        {
            if ( TString<TChar>::length > 0 )
            {
                integer idx= characters::CharArray<TChar>::IndexOfAnyExcluded( TString<TChar>::buffer  , TString<TChar>::length,
                                                                                      whiteSpaces.Buffer(),        whiteSpaces.Length() );
                if(  idx < 0 )
                    idx= TString<TChar>::length;
                TString<TChar>::buffer+= idx;
                TString<TChar>::length-= idx;
            }
            return *this;
        }

        /** ****************************************************************************************
         * Moves the start to the first character not found in given character set \p{whiteSpaces}.
         *
         * @param whiteSpaces  The characters used for trimming.
         *                     Defaults to  \ref aworx::DefaultWhitespaces
         * @return \c *this to allow concatenated calls.
         ******************************************************************************************/
        inline
        TSubstring&  TrimEnd( const TCString<TChar>& whiteSpaces
                                                    = TT_StringConstants<TChar>::DefaultWhitespaces() )
        {
            if ( TString<TChar>::length > 0 )
            {
                TString<TChar>::length= characters::CharArray<TChar>::LastIndexOfAnyExclude( TString<TChar>::buffer,
                                                                                             TString<TChar>::length - 1,
                                                                                                 whiteSpaces.Buffer(),
                                                                                                 whiteSpaces.Length() ) + 1;
            }
            return *this;
        }

        /** ****************************************************************************************
         * Invokes #TrimStart and #TrimEnd .
         *
         * @param whiteSpaces  The characters used for trimming.
         *                     Defaults to  \ref aworx::DefaultWhitespaces
         * @return \c *this to allow concatenated calls.
         ******************************************************************************************/
        inline
        TSubstring&  Trim( const TCString<TChar>& whiteSpaces
                                                    = TT_StringConstants<TChar>::DefaultWhitespaces() )
        {
            return   TrimEnd  ( whiteSpaces )
                    .TrimStart( whiteSpaces );
        }

ALIB_WARNINGS_IGNORE_IF_CONSTEXPR

        /** ****************************************************************************************
         * Retrieve and remove the first character from the sub-string.
         *
         * @tparam TTrimBeforeConsume Determines if the string should be (left-) trimmed before the
         *                            consume operation. Defaults to \b Whitespaces::Keep.
         * @tparam TCheck  Defaults to \c true which is the normal invocation mode.
         *                 If \c \<false\> is added to the method name, no parameter check is
         *                 performed.
         *
         * @return The character at the start of the represented region.
         *         If this \b %Substring is empty or \e nulled, '\0' is returned.
         ******************************************************************************************/
        template < bool        TCheck            = true,
                   Whitespaces TTrimBeforeConsume= Whitespaces::Keep  >
        inline
        TChar       ConsumeChar()
        {
            if ALIB_CPP17_CONSTEXPR ( TCheck )
            {
                if ALIB_CPP17_CONSTEXPR ( TTrimBeforeConsume == Whitespaces::Trim )
                    TrimStart();
                if( TString<TChar>::IsEmpty() )
                    return '\0';
            }
            else
            {
                ALIB_ASSERT_ERROR( !TString<TChar>::IsEmpty(), "NC: empty string" )
                if ALIB_CPP17_CONSTEXPR ( TTrimBeforeConsume == Whitespaces::Trim )
                    TrimStart();
            }

            TString<TChar>::length--;
            return *TString<TChar>::buffer++;
        }

        /** ****************************************************************************************
         * Checks if this object starts with the given character \p{consumable}. If it does, this
         * character is cut from this object.
         *
         * @param  consumable         The consumable character.
         *
         * @tparam TSensitivity       The sensitivity of the comparison.
         *                            Defaults to \b Case::Sensitive.
         * @tparam TTrimBeforeConsume Determines if the string should be (left-) trimmed before the
         *                           consume operation. Defaults to \b Whitespaces::Keep.
         * @return \c true, if this object was starting with \p{consumable} and consequently the
         *         string was cut by one.
         ******************************************************************************************/
        template< Case        TSensitivity=       Case::Sensitive,
                  Whitespaces TTrimBeforeConsume= Whitespaces::Keep>
        inline
        bool        ConsumeChar( TChar   consumable )

        {
            if ( TTrimBeforeConsume == Whitespaces::Trim )
                TrimStart();

            if (    ( TSensitivity == Case::Sensitive &&         TString<TChar>::CharAtStart()  !=         consumable  )
                 || ( TSensitivity == Case::Ignore    && toupper(TString<TChar>::CharAtStart()) != toupper(consumable) ) )
                return false;
            TString<TChar>::buffer++;
            TString<TChar>::length--;
            return true;
        }

        /** ****************************************************************************************
         * Checks if this object ends with the given character \p{consumable}. If it does, this
         * character is cut from the end of object.
         *
         * @param consumable          The consumable character.
         * @tparam TSensitivity       The sensitivity of the comparison.
         *                            Defaults to \b Case::Sensitive.
         * @tparam TTrimBeforeConsume Determines if the string should be (left-) trimmed before the
         *                            consume operation. Defaults to \b Whitespaces::Keep.
         * @return \c true, if this object was starting with \p{consumable} and consequently the
         *         string was cut by one.
         ******************************************************************************************/
        template< Case        TSensitivity=       Case::Sensitive,
                  Whitespaces TTrimBeforeConsume= Whitespaces::Keep>
        inline
        bool        ConsumeCharFromEnd( nchar  consumable )
        {
            if ( TTrimBeforeConsume == Whitespaces::Trim )
                TrimEnd();

            if (    ( TSensitivity == Case::Sensitive &&         TString<TChar>::CharAtEnd()  !=         consumable  )
                 || ( TSensitivity == Case::Ignore    && toupper(TString<TChar>::CharAtEnd()) != toupper(consumable) ) )
                return false;
            TString<TChar>::length--;
            return true;
        }

        /** ****************************************************************************************
         * Retrieve and remove the last character in the sub-string.
         *
         * @tparam TCheck  Defaults to \c true which is the normal invocation mode.
         *                 If \c \<false\> is added to the method name, no check whether this
         *                 string is empty is performed.
         *
         * @tparam TTrimBeforeConsume Determines if the string should be (right-) trimmed before the
         *                            consume operation. Defaults to \b Whitespaces::Keep.
         * @return The character at the start of the represented region.
         *         If this \b %Substring is empty or \e nulled, '\0' is returned.
         ******************************************************************************************/
        template <bool TCheck= true,
                  Whitespaces TTrimBeforeConsume= Whitespaces::Keep >
        inline
        TChar     ConsumeCharFromEnd()
        {
            if ALIB_CPP17_CONSTEXPR ( TTrimBeforeConsume == Whitespaces::Trim )
                TrimEnd();

            if ALIB_CPP17_CONSTEXPR ( TCheck )
            {
                if( TString<TChar>::IsEmpty() )
                    return '\0';
            }
            else
            {
                ALIB_ASSERT_ERROR( !TString<TChar>::IsEmpty(), "NC: empty string" )
            }
            return *(TString<TChar>::buffer + --TString<TChar>::length );
        }

        /** ****************************************************************************************
         * Cuts the given number of characters from the beginning of the Substring and optionally
         * places the portion that was cut in parameter \p{target} (if provided).<br>
         *
         * If parameter \p{regionLength} is negative, nothing is cut and optional argument
         * \p{target} is set empty. If \p{regionLength} is equal or greater than this
         * object's length, all contents is 'moved' to \p{target}.
         *
         * @tparam TCheck  Defaults to \c true which is the normal invocation mode.
         *                 If \c \<false\>, parameter \p{regionLength} has to be in the range of
         *                 this object's size.
         *
         * @param regionLength  The length of the region at the start to delete.
         * @param target        An optional target \b %Substring that receives the portion that
         *                      is cut from this object. Defaults to nullptr.
         *
         * @return The new length of the sub-string.
         ******************************************************************************************/
        template <bool TCheck= true>
        inline
        integer  ConsumeChars( integer regionLength, TSubstring* target= nullptr )
        {
            if ALIB_CPP17_CONSTEXPR ( TCheck )
            {
                if ( regionLength < 0 )
                {
                    if ( target != nullptr )
                        target->Clear();
                    return  TString<TChar>::length;
                }
                if ( regionLength > TString<TChar>::length )
                    regionLength= TString<TChar>::length;
            }
            else
            {
                ALIB_ASSERT_ERROR( regionLength >=0 && regionLength <= TString<TChar>::length,
                                   "NC: regionLength out of bounds"            )
            }

            if ( target != nullptr )
                *target= this->TString<TChar>::template Substring<false>( 0, regionLength );

            TString<TChar>::buffer+= regionLength;
            TString<TChar>::length-= regionLength;
            return TString<TChar>::length;
        }

        /** ****************************************************************************************
         * Cuts the given number of characters from the end of the \b %Substring and optionally
         * places the portion that was cut in parameter \p{target} (if provided).<br>
         * Parameter \p{regionLength} is checked to be between 0 and length. If negative, nothing
         * is cut and \p{target} is set empty. If \p{regionLength} is greater than this
         * object's length, all contents is 'moved' to \p{target}.
         *
         * @tparam TCheck  Defaults to \c true which is the normal invocation mode.
         *                 If \c \<false\> is added to the method name, no parameter check is
         *                 performed.
         *
         * @param regionLength  The length of the region at the start to delete.
         * @param target        An optional target \b %Substring that receives the portion that
         *                      is cut from this object. Defaults to nullptr.
         *
         * @return The new length of the sub-string.
         ******************************************************************************************/
        template <bool TCheck= true>
        inline
        integer   ConsumeCharsFromEnd( integer regionLength, TSubstring* target= nullptr )
        {
            if ALIB_CPP17_CONSTEXPR ( TCheck )
            {
                if ( regionLength < 0 )
                {
                    if ( target != nullptr )
                        target->Clear();
                    return  TString<TChar>::length;
                }
                if ( regionLength > TString<TChar>::length )
                    regionLength= TString<TChar>::length;
            }
            else
            {
                ALIB_ASSERT_ERROR( regionLength >=0 && regionLength <= TString<TChar>::length,
                                   "NC: regionLength out of bounds"             )
            }

            if ( target != nullptr )
                *target= this->TString<TChar>::template Substring<false>( TString<TChar>::length - regionLength, regionLength );

            TString<TChar>::length-= regionLength;
            return TString<TChar>::length;
        }

        /** ****************************************************************************************
         * Cuts the given number of characters from the beginning of the Substring and optionally
         * places the portion that was cut in parameter \p{target}.<br>
         * Parameter \p{regionLength} is checked to be between 0 and length. If negative, nothing
         * is cut and \p{target} is set empty, respectively left untouched depending on
         * \p{TTargetData}.
         *
         * If \p{regionLength} is greater than this  object's length, all contents is 'moved'
         * to \p{target}.
         *
         * @tparam TCheck  Defaults to \c true which is the normal invocation mode.
         *                 If \c \<false\> is added to the method name, no parameter check is
         *                 performed.
         *
         * @param regionLength    The length of the region at the start to delete.
         * @param target          A target \b %AString that receives the portion that
         *                        is cut from this object.
         * @param separatorWidth  This width is added to what is cut from this string, while
         *                        \p{target} still receives the portion defined by \p{regionLength}.
         *                        Defaults to 0.
         * @tparam TTargetData    If \c CurrentData::Keep, \b %AString \p{target} is not cleared
         *                        before the result is written. Defaults to \c CurrentData::Clear.
         *
         * @return The new length of the sub-string.
         ******************************************************************************************/
        template <bool         TCheck           = true,
                  CurrentData  TTargetData      = CurrentData::Clear>
        inline
        integer ConsumeChars( integer              regionLength,
                              TAString<TChar>&  target,
                              integer              separatorWidth   =0         )
        {
            if ALIB_CPP17_CONSTEXPR ( TTargetData == CurrentData::Clear  )
                target.Reset();

            if ALIB_CPP17_CONSTEXPR ( TCheck )
            {
                if ( separatorWidth < 0 )                        separatorWidth= 0;
                if ( regionLength   < 0 )                        return  TString<TChar>::length;
                if ( regionLength   > TString<TChar>::length - separatorWidth )  regionLength= TString<TChar>::length - separatorWidth;
                if ( regionLength   < 0 )                        return  TString<TChar>::length;
            }
            else
            {
                ALIB_ASSERT_ERROR( separatorWidth >=0 , "NC: separator width negative"  )
                ALIB_ASSERT_ERROR( regionLength >=0 && regionLength + separatorWidth <= TString<TChar>::length,
                                   "NC: regionLength out of bounds"             )
            }

            target.template _<false>( *this, 0, regionLength );

            regionLength+= separatorWidth;
            TString<TChar>::buffer+= regionLength ;
            TString<TChar>::length-= regionLength;
            return TString<TChar>::length;
        }

        /** ****************************************************************************************
         * Cuts the given number of characters from the end of the sub-string and
         * places the portion that was cut in parameter \p{target}.<br>
         * Parameter \p{regionLength} is checked to be between 0 and length. If negative, nothing
         * is cut and \p{target} is set empty, respectively left untouched depending on \p
         * If \p{regionLength} is greater than this object's length, all contents is 'moved'
         * to \p{target}.
         *
         * @tparam TCheck  Defaults to \c true which is the normal invocation mode.
         *                 If \c \<false\> is added to the method name, no parameter check is
         *                 performed.
         *
         * @param regionLength    The length of the region at the start to delete.
         * @param target          A target \b %AString that receives the portion that
         *                        is cut from this object.
         * @param separatorWidth  This width is added to what is cut from this string, while
         *                        \p{target} still receives the portion defined by \p{regionLength}.
         *                        Defaults to 0.
         * @tparam TTargetData    If \c CurrentData::Keep, the parameter \p{target} is not cleared
         *                        before the result is written. Defaults to \c CurrentData::Clear.
         *
         * @return The new length of the sub-string.
         ******************************************************************************************/
        template <bool            TCheck           = true,
                  CurrentData     TTargetData      = CurrentData::Clear>
        inline
        integer ConsumeCharsFromEnd( integer             regionLength,
                                     AString&            target,
                                     integer             separatorWidth   =0      )
        {
            if ALIB_CPP17_CONSTEXPR ( TTargetData == CurrentData::Clear  )
                target.Reset();

            if ALIB_CPP17_CONSTEXPR ( TCheck )
            {
                if ( separatorWidth < 0 )                        separatorWidth= 0;
                if ( regionLength   < 0 )                        return  TString<TChar>::length;
                if ( regionLength   > TString<TChar>::length - separatorWidth )  regionLength= TString<TChar>::length - separatorWidth;
                if ( regionLength   < 0 )                        return  TString<TChar>::length;
            }
            else
            {
                ALIB_ASSERT_ERROR( separatorWidth >=0 , "NC: separator width negative"  )
                ALIB_ASSERT_ERROR( regionLength >=0 && regionLength + separatorWidth <= TString<TChar>::length,
                                   "NC: regionLength out of bounds"             )
            }

            target._<false>( *this, TString<TChar>::length - regionLength, regionLength );

            TString<TChar>::length-= regionLength + separatorWidth;
            return TString<TChar>::length;
        }

        /** ****************************************************************************************
         * Searches \p{separator} and cuts the beginning of this string, including the separator.
         * What was consumed is returned, excluding the separator.
         *
         * If the separator is not found, all of this string is consumed and returned.
         *
         * @param separator  The separator to search.
         *                   Defaults to <c>','</c>.
         *
         * @return The token consumed.
         ******************************************************************************************/
        inline
        TString<TChar>  ConsumeToken( TChar separator= ',' )
        {
            ALIB_ASSERT_ERROR( TString<TChar>::IsNotNull() , "ConsumeToken on nulled Substring" )
            integer separatorPos= TString<TChar>::IndexOfOrLength( separator );
            TString<TChar> result= TString<TChar>( TString<TChar>::buffer, separatorPos );

            TString<TChar>::buffer+= separatorPos;
            TString<TChar>::length-= separatorPos;
            if( TString<TChar>::length > 0 )
            {
                TString<TChar>::buffer++;
                TString<TChar>::length--;
            }
            return result;
        }

        /** ****************************************************************************************
         * Checks if this object starts with the given string \p{consumable}. If it does, this
         * string is cut from this object.
         *
         * @param  consumable         The consumable string.
         * @tparam TSensitivity       The sensitivity of the comparison.
         *                            Defaults to \b Case::Sensitive.
         * @tparam TTrimBeforeConsume Determines if the string should be (left-) trimmed before the
         *                            consume operation. Defaults to \b Whitespaces::Keep.
         * @return \c true, if this object was starting with \p{consumable} and consequently the
         *         string was cut.
         ******************************************************************************************/
        template< Case        TSensitivity=       Case::Sensitive,
                  Whitespaces TTrimBeforeConsume= Whitespaces::Keep >
        inline
        bool        ConsumeString( const TString<TChar>&     consumable  )
        {
            if ALIB_CPP17_CONSTEXPR ( TTrimBeforeConsume == Whitespaces::Trim )
                TrimStart();

            if ( !TString<TChar>::template StartsWith<true,TSensitivity>( consumable ) )
                return false;

            TString<TChar>::buffer+= consumable.Length();
            TString<TChar>::length-= consumable.Length();
            return true;
        }

        /** ****************************************************************************************
         * Checks if this object ends with the given string \p{consumable}. If it does, this
         * string is cut from the end of object.
         *
         * @param  consumable         The consumable string
         * @tparam TSensitivity       The sensitivity of the comparison.
         *                            Defaults to \b Case::Sensitive.
         * @tparam TTrimBeforeConsume Determines if the string should be (left-) trimmed before the
         *                           consume operation. Defaults to \b Whitespaces::Keep.
         * @return \c true, if this object was starting with \p{consumable} and consequently the
         *         string was cut.
         ******************************************************************************************/
        template< Case        TSensitivity=       Case::Sensitive,
                  Whitespaces TTrimBeforeConsume= Whitespaces::Keep >
        inline
        bool        ConsumeStringFromEnd( const TString<TChar>&  consumable )
        {
            if ALIB_CPP17_CONSTEXPR ( TTrimBeforeConsume == Whitespaces::Trim )
                TrimEnd();

            if ( !TString<TChar>::template EndsWith<true,TSensitivity>( consumable ) )
                return false;
            TString<TChar>::length-= consumable.Length();
            return true;
        }

        /** ****************************************************************************************
         * Consumes a minimum of \p{minChars} of string \p{consumable} from the start of this
         * sub-string. If the minimum characters could not be found, nothing is consumed, otherwise,
         * the method consumes as much as possible.<br>
         *
         * This method is useful read "tokens" from a string that may be abbreviated. Internally,
         * this method is used by method #ConsumeEnum.
         *
         *
         * @param  consumable         The consumable string.
         * @param  minChars           The minimum amount of characters to consume. If \c 0 or
         *                            negative, the length of \p{consumable} is chosen.
         *                            Optional and defaults to \c 1.
         * @tparam TSensitivity       The sensitivity of the comparison.
         *                            Defaults to \b Case::Ignore.
         * @tparam TTrimBeforeConsume Determines if the string should be (left-) trimmed before the
         *                            first character consume operation.
         *                            Defaults to \b Whitespaces::Keep.
         * @return The amount of characters consumed.
         ******************************************************************************************/
        template< Case        TSensitivity=       Case::Ignore,
                  Whitespaces TTrimBeforeConsume= Whitespaces::Keep >
        integer    ConsumePartOf(  const TString<TChar>&     consumable,
                                   int                          minChars           = 1 )
        {
            if ALIB_CPP17_CONSTEXPR ( TTrimBeforeConsume == Whitespaces::Trim )
                TrimStart();
            if ( minChars <= 0 )
                minChars= static_cast<int>( consumable.Length() );

            if ( minChars == 0 || minChars > consumable.Length() )
                return 0;

            integer diff= TString<TChar>::IndexOfFirstDifference( consumable, TSensitivity );
            if( diff < static_cast<integer>( minChars ) )
                return 0;
            ConsumeChars( diff );
            return diff;
        }

        /** ****************************************************************************************
         * Consumes a field from the beginning of this sub-string, which is surrounded by
         * given start end end character identifiers. If both are the same, e.g. \c '"', then
         * the first occurrence of the end character is used. If they are not the same, e.g.
         * \c '<' and \c '>', then repeated start characters are counted and consumption only ends
         * when a corresponding amount of end characters has been found.
         *
         * @param  startChar  The start character of the field to consume.
         * @param  endChar    The end character of the field to consume.
         * @tparam TTrimBeforeConsume Determines if the string should be (left-) trimmed before the
         *                            consume operation. Defaults to \b Whitespaces::Keep.
         * @return The string consumed. \b NullString on error (start/end character not found)
         ******************************************************************************************/
        template< Whitespaces TTrimBeforeConsume= Whitespaces::Keep >
        inline
        TString<TChar>  ConsumeField( TChar startChar, TChar endChar  )
        {
            if ALIB_CPP17_CONSTEXPR ( TTrimBeforeConsume == Whitespaces::Trim )
                TrimStart();

            integer endIdx;
            if (    TString<TChar>::CharAtStart() != startChar
                 || (endIdx= TString<TChar>::IndexOfSegmentEnd( startChar, endChar, 1)) < 0  )
                return nullptr;


            TString<TChar> result= TString<TChar>( TString<TChar>::buffer + 1, endIdx - 1 );
            TString<TChar>::buffer+= (endIdx + 1);
            TString<TChar>::length-= (endIdx + 1);
            return result;
        }

    #if ALIB_MODULE_RESOURCES
        /** ****************************************************************************************
         * Consumes an element of a C++ enum which is equipped with "\alib enum meta data".
         * For more information consult
         * \ref anchor_T_EnumMetaDataDecl_read_enum "T_EnumMetaDataDecl documentation".
         *
         * \note
         *   This method is applicable to \alib{enums,T_EnumIsBitwise,bitwise enums} as well.
         *   However, only one element name is parsed.
         *   To parse multiple elements (ored to one enum value), use method #ConsumeEnumBitwise.
         *
         * @param[out] result         The result enum element given as reference.
         * @tparam TEnum              The enumeration type.
         * @tparam TSensitivity       The sensitivity of the comparison.
         *                            Defaults to \b Case::Sensitive.
         * @tparam TTrimBeforeConsume Determines if the string should be (left-) trimmed before the
         *                            consume operation.<br>
         *                            Defaults to \b Whitespaces::Trim.
         * @tparam TEnableIf          Internal. Do \b not specify!<br>
         *                            (Defaults to \c std::enable_if type, to enable the compiler to
         *                            select this method only for types that have specialized member
         *                            \alib{resources,T_EnumMetaDataDeclReadWrite::MinParseLengthIndex}
         *                            evaluating to a value different to \c 0.
         * @return \c true if an enum element was read, \c false otherwise.
         *
         * \par Module Dependencies
         *   This method is only available if \alibmod_resources is included in the \alibdist.
         ******************************************************************************************/
        template<typename    TEnum,
                 Case        TSensitivity        = Case::Ignore,
                 Whitespaces TTrimBeforeConsume  = Whitespaces::Trim,
        typename TEnableIf= ATMP_VOID_IF( resources::T_EnumMetaDataDeclReadWrite<TEnum>::MinParseLengthIndex!= 0)>
        bool    ConsumeEnum(  TEnum&  result )
        {
            auto& enumMetaData= EnumMetaData<TEnum>::GetSingleton();
            enumMetaData.CheckLoad();

            if ALIB_CPP17_CONSTEXPR ( TTrimBeforeConsume == Whitespaces::Trim )
                TrimStart();

            for( auto& entry : enumMetaData.Table )
            {
                if ( ConsumePartOf<TSensitivity>( EnumReadWriteInfo<TEnum>::Name( entry ),
                                                  std::get<T_EnumMetaDataDeclReadWrite<TEnum>::MinParseLengthIndex>( entry )
                                                 )  > 0 )
                {
                    result= enumMetaData.Enum( entry );
                    return true;
                }
            }
            return false;
        }

#if ALIB_MODULE_ENUMS
        /** ****************************************************************************************
         * Repeatedly invokes #ConsumeEnum until \p{delim} is not found. The enum element values
         * are or'ed in \p{result}.
         *
         * \note
         *   This method is applicable to \alib{enums,T_EnumIsBitwise,bitwise enums} only.
         *
         *
         * @param[out] result         The result enum element given as reference.
         * @tparam TSensitivity       The sensitivity of the comparison.
         *                            Defaults to \b Case::Ignore.
         * @tparam TTrimBeforeConsume Determines if the string should be (left-) trimmed before and
         *                            after each consume operation.<br>
         *                            Defaults to \b Whitespaces::Trim.
         * @tparam delimiter          The delimiter character of the enum elements.<br>
         *                            Defaults to <c>','</c>.
         * @tparam keepLastDelim      If \c true , the delimiter will be kept in this sub-string, if
         *                            after the delimiter no further enum element was found.
         *                            If \c false, the delimiter will be kept.<br>
         *                            Defaults to \c true.
         * @tparam TEnum              The enumeration type.
         * @tparam TEnableIf          Internal. Do \b not specify!<br>
         *                            (Defaults to \c std::enable_if type, to enable the compiler to
         *                            select this operator only for types that have specialized
         *                            \alib{enums,T_EnumIsBitwise} and also specialized member
         *                            \alib{resources,T_EnumMetaDataDeclReadWrite::MinParseLengthIndex}
         *                            evaluating to a value different to \c 0.)
         * @return \c true if an enum element was read, \c false otherwise.
         *
         * \par Module Dependencies
         *   This method is only available if module \alibmod_enums is included in the \alibdist.
         ******************************************************************************************/
        template<typename    TEnum,
                 Case        TSensitivity       = Case::Ignore,
                 Whitespaces TTrimBeforeConsume = Whitespaces::Trim,
                 TChar       delimiter          = ',',
                 bool        keepLastDelim      = true,
                 typename TEnableIf= ATMP_VOID_IF(   T_EnumMetaDataDeclReadWrite<TEnum>::MinParseLengthIndex
                                                     !=  0
                                                  && T_EnumIsBitwise<TEnum>::value)>
        bool    ConsumeEnumBitwise( TEnum&  result )
        {
            bool mResult= false;
            result= TEnum(0);
            TSubstring restoreBeforeDelim;
            if ALIB_CPP17_CONSTEXPR ( keepLastDelim )
                restoreBeforeDelim= *this;
            for(;;)
            {
                if ALIB_CPP17_CONSTEXPR ( TTrimBeforeConsume == Whitespaces::Trim )
                    TrimStart();
                TEnum actEnum;
                if ( !ConsumeEnum<TEnum, TSensitivity, TTrimBeforeConsume>( actEnum ) )
                {
                    if ALIB_CPP17_CONSTEXPR ( keepLastDelim )
                        *this= restoreBeforeDelim;
                    return mResult;
                }
                result|=  actEnum;
                mResult=  true;
                if ALIB_CPP17_CONSTEXPR ( TTrimBeforeConsume == Whitespaces::Trim )
                    TrimStart();
                if ALIB_CPP17_CONSTEXPR ( keepLastDelim )
                    restoreBeforeDelim=  *this;
                if( !ConsumeChar<TSensitivity, TTrimBeforeConsume>( delimiter ) )
                    return mResult;

            }
        }
#endif

        /** ****************************************************************************************
         * Convenience method that first uses #ConsumeEnum to try and read an element of a C++
         * enum. If this is not successful, an enum of type \alib{Bool} is tried to be read.
         * If this is successful, depending on the value read, the \p{TEnum} values given
         * as parameters \p{falseValue} and \p{trueValue} are assigned.
         * Otherwise false is returned.
         *
         * For more information consult
         * \ref anchor_T_EnumMetaDataDecl_read_enum "T_EnumMetaDataDecl documentation".
         *
         * \note
         *   This method is applicable to bitwise enums as well. However, only one element name is
         *   parsed. To parse multiple elements (ored to one enum value), use method
         *   #ConsumeEnumBitwise.
         *
         *
         * @param[out] result         The result enum element given as reference.
         * @tparam TEnum              The enumeration type.
         * @param  trueValue          The \p{TEnum} value to use in case of \c Bool::True was read.
         * @param  falseValue         The \p{TEnum} value to use in case of \c Bool::False was read.
         * @tparam TTrimBeforeConsume Determines if the string should be (left-) trimmed before the
         *                            consume operation. Passed to #ConsumeEnum.<br>
         *                            Defaults to \b Whitespaces::Trim.
         * @tparam TSensitivity       The sensitivity of the comparison.
         *                            Defaults to \b Case::Ignore.
         * @tparam TEnableIf          Internal. Do \b not specify!<br>
         *                            (Defaults to \c std::enable_if type, to enable the compiler to
         *                            select this method only for types that have specialized member
         *                            \alib{resources,T_EnumMetaDataDeclReadWrite::MinParseLengthIndex}
         *                            evaluating to a value different to \c 0.
         * @return \c true if an element of \p{TEnum} or \alib{Bool} could be read,
         *         \c false otherwise.
         ******************************************************************************************/
        template<typename    TEnum,
                 Case        TSensitivity       = Case::Ignore,
                 Whitespaces TTrimBeforeConsume = Whitespaces::Trim,
        typename TEnableIf= ATMP_VOID_IF(T_EnumMetaDataDeclReadWrite<TEnum>::MinParseLengthIndex != 0) >
        bool    ConsumeEnumOrBool(  TEnum&            result,
                                    TEnum             falseValue,
                                    TEnum             trueValue         )
        {
            // first try to read a TEnum
            if( ConsumeEnum<TEnum, TSensitivity, TTrimBeforeConsume>( result ) )
                return true;

            // if failed, read boolean
            Bool boolEnum;
            if( ConsumeEnum<Bool,  TSensitivity, TTrimBeforeConsume>( boolEnum ) )
            {
                result= boolEnum == Bool::True ? trueValue : falseValue;
                return true;
            }

            // failed
            return false;
        }
    #endif

ALIB_WARNINGS_RESTORE // ALIB_WARNINGS_IGNORE_IF_CONSTEXPR


                /** ****************************************************************************************
         * Consumes all characters \c '0' to \c '9' at the start of this object and stores the
         * value they represent in \p{result}.<br>
         * Unlike methods #ConsumeInt or #ConsumeDec, this method does not consume (accept)
         * sign-, whitespace- or group-characters.
         *
         * @param [out] result    A reference to the result value.
         * @tparam    TInteger    The output type.
         *                        Must be statically castable from \b uint64_t.
         * @tparam TEnableIf      Internal. Do \b not specify!<br>
         *                        (Defaults to \c std::enable_if type, to enable the compiler to
         *                        select this method only for integer types.
         *
         * @return  \c true if a number was found and consumed, \c false otherwise.
         ******************************************************************************************/
        template<typename TInteger,
                 typename TEnableIf= typename std::enable_if<    std::is_integral<TInteger>::value
                                                            >::type>
        inline
        bool     ConsumeDecDigits( TInteger& result )
        {
            uint64_t resultImpl;
            bool    returnValue= consumeDecDigitsImpl( resultImpl );
            result= static_cast<TInteger>( resultImpl );
            return  returnValue;
        }

        /** ****************************************************************************************
         * Consumes an integer value in decimal, binary, hexadecimal or octal format from the
         * string.
         *
         * Parameter \p{numberFormat} defaults to \c nullptr. This denotes static singleton
         * \alib{strings,TNumberFormat::Computational,NumberFormat::Computational}
         * which is configured to not using (not allowing) grouping characters.
         *
         * For more information on number conversion, see class
         * \alib{strings,TNumberFormat,NumberFormat}.
         *
         * @param [out] result    A reference to the result value.
         * @param  numberFormat   The number format to use. Defaults to \c nullptr.
         * @tparam TInteger       The output type.
         *                        Must be statically castable from \b uint64_t.
         * @tparam TEnableIf      Internal. Do \b not specify!<br>
         *                        (Defaults to \c std::enable_if type, to enable the compiler to
         *                        select this method only for integer types.
         * @return  \c true if a number was found and consumed, \c false otherwise.
         ******************************************************************************************/
        template<typename TInteger,
                 typename TEnableIf= typename std::enable_if<    std::is_integral<TInteger>::value
                                                            >::type
                >
        inline
        bool   ConsumeInt( TInteger& result, TNumberFormat<TChar>* numberFormat= nullptr )
        {
            int64_t resultImpl;
            bool    returnValue= consumeIntImpl( resultImpl, numberFormat );
            result= static_cast<TInteger>( resultImpl );
            return  returnValue;
        }

        /** ****************************************************************************************
         * Consumes an unsigned integer in standard decimal format from the start of this %AString.
         *
         * Parameter \p{numberFormat} defaults to \c nullptr. This denotes static singleton
         * \alib{strings,TNumberFormat::Computational,NumberFormat::Computational}
         * which is configured to not using (not allowing) grouping characters.
         *
         * Sign literals \c '-' or \c '+' are \b not accepted and parsing will fail.
         * For reading signed integer values, see methods #ConsumeInt, for floating point numbers
         * #ConsumeFloat.
         *
         * For more information on number conversion, see class
         * \alib{strings,TNumberFormat,NumberFormat}.
         *
         * @param [out] result    A reference to the result value.
         * @param numberFormat    The number format to use. Defaults to \c nullptr.
         * @tparam TInteger       The output type.
         *                        Must be statically castable from \b uint64_t.
         * @tparam TEnableIf      Internal. Do \b not specify!<br>
         *                        (Defaults to \c std::enable_if type, to enable the compiler to
         *                        select this method only for integer types.
         * @return  \c true if a number was found and consumed, \c false otherwise.
         ******************************************************************************************/
        template<typename TInteger,
                 typename TEnableIf= typename std::enable_if<    std::is_integral<TInteger>::value
                                                            >::type
                >
        inline
        bool   ConsumeDec( TInteger& result, TNumberFormat<TChar>* numberFormat= nullptr )
        {
            uint64_t resultImpl;
            bool     returnValue= consumeDecImpl( resultImpl, numberFormat );
            result=  static_cast<TInteger>( resultImpl );
            return   returnValue;
        }

        /** ****************************************************************************************
         * Consumes an unsigned integer in binary format from the start of this string.
         *
         * Parameter \p{numberFormat} defaults to \c nullptr. This denotes static singleton
         * \alib{strings,TNumberFormat::Computational,NumberFormat::Computational}
         * which is configured to not using (not allowing) grouping characters.
         *
         * For more information on number conversion, see class
         * \alib{strings,TNumberFormat,NumberFormat}.
         *
         * @param [out] result    A reference to the result value.
         * @param numberFormat    The number format to use. Defaults to \c nullptr.
         * @tparam    TInteger    The output type.
         *                        Must be statically castable from \b uint64_t.
         * @tparam TEnableIf      Internal. Do \b not specify!<br>
         *                        (Defaults to \c std::enable_if type, to enable the compiler to
         *                        select this method only for integer types.
         *
         * @return  \c true if a number was found and consumed, \c false otherwise.
         ******************************************************************************************/
        template<typename TInteger,
                 typename TEnableIf= typename std::enable_if<    std::is_integral<TInteger>::value
                                                            >::type
                >
        inline
        bool   ConsumeBin( TInteger& result, TNumberFormat<TChar>* numberFormat= nullptr )
        {
            uint64_t resultImpl;
            bool     returnValue= consumeBinImpl( resultImpl, numberFormat );
            result=  static_cast<TInteger>( resultImpl );
            return   returnValue;
        }

        /** ****************************************************************************************
         * Consumes an unsigned integer in hexadecimal format from the start of this string.
         *
         * Parameter \p{numberFormat} defaults to \c nullptr. This denotes static singleton
         * \alib{strings,TNumberFormat::Computational,NumberFormat::Computational}
         * which is configured to not using (not allowing) grouping characters.
         *
         * For more information on number conversion, see class
         * \alib{strings,TNumberFormat,NumberFormat}.
         *
         * @param [out] result    A reference to the result value.
         * @param numberFormat    The number format to use. Defaults to \c nullptr.
         * @tparam    TInteger    The output type.
         *                        Must be statically castable from \b uint64_t.
         * @tparam TEnableIf      Internal. Do \b not specify!<br>
         *                        (Defaults to \c std::enable_if type, to enable the compiler to
         *                        select this method only for integer types.
         *
         * @return  \c true if a number was found and consumed, \c false otherwise.
         ******************************************************************************************/
        template<typename TInteger,
                 typename TEnableIf= typename std::enable_if<    std::is_integral<TInteger>::value
                                                            >::type
                >
        inline
        bool   ConsumeHex( TInteger& result, TNumberFormat<TChar>* numberFormat= nullptr )
        {
            uint64_t resultImpl;
            bool     returnValue= consumeHexImpl( resultImpl, numberFormat );
            result=  static_cast<TInteger>( resultImpl );
            return   returnValue;
        }

        /** ****************************************************************************************
         * Consumes an unsigned integer in octal format from the start of this string.
         *
         * Parameter \p{numberFormat} defaults to \c nullptr. This denotes static singleton
         * \alib{strings,TNumberFormat::Computational,NumberFormat::Computational}
         * which is configured to not using (not allowing) grouping characters.
         *
         * For more information on number conversion, see class
         * \alib{strings,TNumberFormat,NumberFormat}.
         *
         * @param [out] result    A reference to the result value.
         * @param numberFormat    The number format to use. Defaults to \c nullptr.
         * @tparam    TInteger    The output type.
         *                        Must be statically castable from \b uint64_t.
         * @tparam TEnableIf      Internal. Do \b not specify!<br>
         *                        (Defaults to \c std::enable_if type, to enable the compiler to
         *                        select this method only for integer types.
         *
         * @return  \c true if a number was found and consumed, \c false otherwise.
         ******************************************************************************************/
        template<typename TInteger,
                 typename TEnableIf= typename std::enable_if<    std::is_integral<TInteger>::value
                                                            >::type
                >
        inline
        bool   ConsumeOct( TInteger& result, TNumberFormat<TChar>* numberFormat= nullptr )
        {
            uint64_t resultImpl;
            bool     returnValue= consumeOctImpl( resultImpl, numberFormat );
            result=  static_cast<TInteger>( resultImpl );
            return   returnValue;
        }

        /** ****************************************************************************************
         * Consumes a floating point number from the start of this string.
         *
         * on the given \p{numberFormat} instance.<br>
         * Parameter \p{numberFormat} defaults to \c nullptr. This denotes static singleton
         * \alib{strings,TNumberFormat::Computational,NumberFormat::Computational}
         * which is configured to 'international' settings (not using the locale) and therefore
         * also not parsing grouping characters.
         *
         * For more information on parsing options for floating point numbers and number
         * conversion in general, see class
         * \alib{strings,TNumberFormat,NumberFormat}.
         *
         * @param [out] result    A reference to the result value.
         * @param numberFormat    The number format to use. Defaults to \c nullptr.
         *
         * @return  \c true if a number was found and consumed, \c false otherwise.
         ******************************************************************************************/
        ALIB_API
        bool  ConsumeFloat( double&                 result,
                            TNumberFormat<TChar>*   numberFormat     =nullptr   );


        /** ****************************************************************************************
         * Splits this sub-string into two parts. What remains in this object is the region
         * from 0 to \p{position}.
         * \p{target} receives the rest. If \p{separatorWidth} is given, this is subtracted from
         * the front of \p{target}.
         *
         * @tparam TCheck  Defaults to \c true which is the normal invocation mode.
         *                 If \c \<false\> is added to the method name, no parameter check is
         *                 performed.
         *
         * @param position        The index where this object is split.
         * @param target          The target sub-string to receive the right part of the string.
         * @param separatorWidth  This does not change what remains in this object, but defines
         *                        the number of characters that are cut from the front of the
         *                        \p{target}. Defaults to 0.
         * @param trim            If \c true, both sub-strings will be trimmed.
         *
         * @return \c *this to allow concatenated calls.
         ******************************************************************************************/
        template <bool TCheck= true>
        inline
        TSubstring& Split( integer position, TSubstring& target, integer separatorWidth =0,
                           bool trim= false )

        {
            if ALIB_CPP17_CONSTEXPR ( TCheck )
            {
                TString<TChar>::AdjustRegion( position, separatorWidth );
            }
            else
            {
                ALIB_ASSERT_ERROR( position >=0 && position <= TString<TChar>::length,
                                   "NC: position out of bounds"             )
                ALIB_ASSERT_ERROR( position + separatorWidth <= TString<TChar>::length,
                                   "NC: position + separator width out of bounds" )
            }

            target= this->TString<TChar>::template Substring<false>( position + separatorWidth,
                                                                     TString<TChar>::length - position - separatorWidth );
            TString<TChar>::length= position;
            if( trim )
            {
                target.Trim();
                this ->Trim();
            }
            return *this;
        }

    //##############################################################################################
    // Protected Methods
    //##############################################################################################
    protected:
        /** ****************************************************************************************
         * Implementation of #ConsumeDecDigits (the non-inline part).
         *
         * @param [out] result    A reference to the result value.
         * @return  \c true if a number was found and consumed, \c false otherwise.
         ******************************************************************************************/
        ALIB_API  bool   consumeDecDigitsImpl( uint64_t& result );

        /** ****************************************************************************************
         * Implementation of #ConsumeInt (the non-inline part).
         *
         * @param [out] result    A reference to the result value.
         * @param numberFormat    The number format to use.
         * @return  \c true if a number was found and consumed, \c false otherwise.
         ******************************************************************************************/
        ALIB_API  bool   consumeIntImpl( int64_t& result, TNumberFormat<TChar>* numberFormat );

        /** ****************************************************************************************
         * Implementation of #ConsumeDec (the non-inline part).
         *
         * @param [out] result    A reference to the result value.
         * @param numberFormat    The number format to use.
         * @return  \c true if a number was found and consumed, \c false otherwise.
         ******************************************************************************************/
        ALIB_API  bool   consumeDecImpl( uint64_t& result, TNumberFormat<TChar>* numberFormat );

        /** ****************************************************************************************
         * Implementation of #ConsumeBin (the non-inline part).
         *
         * @param [out] result    A reference to the result value.
         * @param numberFormat    The number format to use.
         * @return  \c true if a number was found and consumed, \c false otherwise.
         ******************************************************************************************/
        ALIB_API  bool   consumeBinImpl( uint64_t& result, TNumberFormat<TChar>* numberFormat );

        /** ****************************************************************************************
         * Implementation of #ConsumeHex (the non-inline part).
         *
         * @param [out] result    A reference to the result value.
         * @param numberFormat    The number format to use.
         * @return  \c true if a number was found and consumed, \c false otherwise.
         ******************************************************************************************/
        ALIB_API  bool   consumeHexImpl( uint64_t& result, TNumberFormat<TChar>* numberFormat );

        /** ****************************************************************************************
         * Implementation of #ConsumeOct (the non-inline part).
         *
         * @param [out] result    A reference to the result value.
         * @param numberFormat    The number format to use.
         * @return  \c true if a number was found and consumed, \c false otherwise.
         ******************************************************************************************/
        ALIB_API  bool   consumeOctImpl( uint64_t& result, TNumberFormat<TChar>* numberFormat );


}; // class TSubstring

extern template ALIB_API bool TSubstring<nchar>::ConsumeFloat        (   double& , TNumberFormat<nchar>* );
extern template ALIB_API bool TSubstring<nchar>::consumeDecDigitsImpl( uint64_t&                         );
extern template ALIB_API bool TSubstring<nchar>::consumeIntImpl      (  int64_t& , TNumberFormat<nchar>* );
extern template ALIB_API bool TSubstring<nchar>::consumeDecImpl      ( uint64_t& , TNumberFormat<nchar>* );
extern template ALIB_API bool TSubstring<nchar>::consumeBinImpl      ( uint64_t& , TNumberFormat<nchar>* );
extern template ALIB_API bool TSubstring<nchar>::consumeHexImpl      ( uint64_t& , TNumberFormat<nchar>* );
extern template ALIB_API bool TSubstring<nchar>::consumeOctImpl      ( uint64_t& , TNumberFormat<nchar>* );

extern template ALIB_API bool TSubstring<wchar>::ConsumeFloat        (   double& , TNumberFormat<wchar>* );
extern template ALIB_API bool TSubstring<wchar>::consumeDecDigitsImpl( uint64_t&                         );
extern template ALIB_API bool TSubstring<wchar>::consumeIntImpl      (  int64_t& , TNumberFormat<wchar>* );
extern template ALIB_API bool TSubstring<wchar>::consumeDecImpl      ( uint64_t& , TNumberFormat<wchar>* );
extern template ALIB_API bool TSubstring<wchar>::consumeBinImpl      ( uint64_t& , TNumberFormat<wchar>* );
extern template ALIB_API bool TSubstring<wchar>::consumeHexImpl      ( uint64_t& , TNumberFormat<wchar>* );
extern template ALIB_API bool TSubstring<wchar>::consumeOctImpl      ( uint64_t& , TNumberFormat<wchar>* );

extern template ALIB_API bool TSubstring<xchar>::ConsumeFloat        (   double& , TNumberFormat<xchar>* );
extern template ALIB_API bool TSubstring<xchar>::consumeDecDigitsImpl( uint64_t&                         );
extern template ALIB_API bool TSubstring<xchar>::consumeIntImpl      (  int64_t& , TNumberFormat<xchar>* );
extern template ALIB_API bool TSubstring<xchar>::consumeDecImpl      ( uint64_t& , TNumberFormat<xchar>* );
extern template ALIB_API bool TSubstring<xchar>::consumeBinImpl      ( uint64_t& , TNumberFormat<xchar>* );
extern template ALIB_API bool TSubstring<xchar>::consumeHexImpl      ( uint64_t& , TNumberFormat<xchar>* );
extern template ALIB_API bool TSubstring<xchar>::consumeOctImpl      ( uint64_t& , TNumberFormat<xchar>* );

}} // namespace aworx::[lib::strings]

/// Type alias in namespace #aworx.
using     Substring  =    lib::strings::TSubstring<character>;

/// Type alias in namespace #aworx.
using     NSubstring =    lib::strings::TSubstring<nchar>;

/// Type alias in namespace #aworx.
using     WSubstring =    lib::strings::TSubstring<wchar>;

/// Type alias in namespace #aworx.
using     XSubstring =    lib::strings::TSubstring<xchar>;

} // namespace aworx

#endif // HPP_ALIB_STRINGS_SUBSTRING



