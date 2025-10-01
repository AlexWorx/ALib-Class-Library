//==================================================================================================
/// \file
/// This header-file is part of module \alib_strings of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib {  namespace strings {

//==================================================================================================
/// This class specializes parent class \alib{strings;TString;String} to allow reduction of
/// the length of the represented string by cutting characters from the front or the end.
/// Such reduction does not affect the character array represented, but only its representation
/// by instances of this type.
///
/// In other words, the difference to base class \alib{strings;TString;String} is, that this type
/// allows increasing the pointer to the character array's start and to decrease its stored length.
/// In all other respects, this class has the same lightweight nature and performance as its base.
/// Furthermore, the flexible template programming mechanics for seamless construction are exposed
/// from the base class and likewise available.
///
/// Like base class \alib{strings;TString;String}, the class cannot, and therefore does not,
/// verify that the underlying buffer is (still) properly allocated and contains valid data.
/// It is up to the user of this class to make sure the buffer stays intact until any referencing
/// object of this type is disposed.
///
/// @tparam TChar The \ref alib_characters_chars "character type" of this string-type.
///         Alias names for specializations along the different character types
///         are provided in namespace #alib with type definitions
///         \alib{Substring},
///         \alib{NSubstring},
///         \alib{WSubstring},
///         \alib{XSubstring},
///         \alib{ComplementSubstring}, and
///         \alib{StrangeSubstring}.
//==================================================================================================
template<typename TChar>
class TSubstring : public TString<TChar>
{
  protected:
    /// The base string-type.
    using base=   TString<TChar>;

  public:
    #if !DOXYGEN
        // Import parent constructors.
        // Due to a doxygen bug in 1.8.14, we must not tell doxygen that we import overloaded
        // methods.
        using base::TString;
    #endif

    /// Default constructor creating a \ref alib_strings_details_nulled \e "nulled" substring.
    TSubstring()
    : base()
    {}

    /// Constructor using a string reference.
    /// @param src   The source string.
    TSubstring( const TString<TChar>& src )
    : base(src)
    {}

    /// Sets this object to zero length.
    /// @return \c *this to allow concatenated calls.
    TSubstring&  Clear()
    {
        base::length=  0;
        return *this;
    }

    /// Moves the start to the first character not found in given character set \p{whiteSpaces}.
    ///
    /// @param whiteSpaces  The characters used for trimming.
    ///                     Defaults to  \ref alib::DEFAULT_WHITESPACES
    /// @return \c *this to allow concatenated calls.
    TSubstring&  TrimStart( const TCString<TChar>& whiteSpaces
                                                = CStringConstantsTraits<TChar>::DefaultWhitespaces() )
    {
        if ( base::length > 0 )
        {
            integer idx= characters::IndexOfAnyExcluded( base::buffer  ,
                                                                           base::length,
                                                                           whiteSpaces.Buffer(),
                                                                           whiteSpaces.Length() );
            if(  idx < 0 )
                idx= base::length;
            base::buffer+= idx;
            base::length-= idx;
        }
        return *this;
    }

    /// Moves the start to the first character not found in given character set \p{whiteSpaces}.
    ///
    /// @param whiteSpaces  The characters used for trimming.
    ///                     Defaults to  \ref alib::DEFAULT_WHITESPACES
    /// @return \c *this to allow concatenated calls.
    TSubstring&  TrimEnd( const TCString<TChar>& whiteSpaces
                                                = CStringConstantsTraits<TChar>::DefaultWhitespaces() )
    {
        if ( base::length > 0 )
        {
            base::length= characters::LastIndexOfAnyExclude( base::buffer,
                                                                               base::length - 1,
                                                                               whiteSpaces.Buffer(),
                                                                               whiteSpaces.Length() ) + 1;
        }
        return *this;
    }

    /// Invokes #TrimStart and #TrimEnd .
    ///
    /// @param whiteSpaces  The characters used for trimming.
    ///                     Defaults to  \ref alib::DEFAULT_WHITESPACES
    /// @return \c *this to allow concatenated calls.
    TSubstring&  Trim( const TCString<TChar>& whiteSpaces
                                                = CStringConstantsTraits<TChar>::DefaultWhitespaces() )
    {
        return   TrimEnd  ( whiteSpaces )
                .TrimStart( whiteSpaces );
    }

    /// Retrieve and remove the first character from the substring.
    ///
    /// @tparam TTrimBeforeConsume Determines if the string should be (left-) trimmed before the
    ///                            consume operation. Defaults to \b Whitespaces::Keep.
    /// @tparam TCheck  Defaults to \alib{CHK}, which is the normal invocation mode.
    ///                 If \c \<false\> is added to the method name, no parameter check is
    ///                 performed.
    ///
    /// @return The character at the start of the represented region.
    ///         If this \b %Substring is empty or \e nulled, '\0' is returned.
    template < typename          TCheck            = CHK,
               lang::Whitespaces TTrimBeforeConsume= lang::Whitespaces::Keep  >
    TChar       ConsumeChar()
    {
        if constexpr ( TCheck::value )
        {
            if constexpr ( TTrimBeforeConsume == lang::Whitespaces::Trim )
                TrimStart();
            if( base::IsEmpty() )
                return '\0';
        }
        else
        {
            ALIB_ASSERT_ERROR(!base::IsEmpty(),"STRINGS","Non checking but called on empty string" )
            if constexpr ( TTrimBeforeConsume == lang::Whitespaces::Trim )
                TrimStart();
        }

        --base::length;
        return *base::buffer++;
    }

    /// Checks if this object starts with the given character \p{consumable}. If it does, this
    /// character is cut from this object.
    ///
    /// @tparam TSensitivity       The sensitivity of the comparison.
    ///                            Defaults to \b Case::Sensitive.
    /// @tparam TTrimBeforeConsume Determines if the string should be (left-) trimmed before the
    ///                            consume operation. Defaults to \b Whitespaces::Keep.
    /// @param  consumable         The consumable character.
    /// @return \c true, if this object was starting with \p{consumable} and consequently the
    ///         string was cut by one, \c false otherwise.
    template< lang::Case        TSensitivity=       lang::Case::Sensitive,
              lang::Whitespaces TTrimBeforeConsume= lang::Whitespaces::Keep>
    bool        ConsumeChar( TChar   consumable )

    {
        if ( TTrimBeforeConsume == lang::Whitespaces::Trim )
            TrimStart();

        if (    ( TSensitivity == lang::Case::Sensitive &&    base::CharAtStart()  !=   consumable  )
             || ( TSensitivity == lang::Case::Ignore    &&    characters::ToUpper(base::CharAtStart())
                                                           != characters::ToUpper(consumable) ) )
            return false;

        ++base::buffer;
        --base::length;
        return true;
    }

    /// Checks if this object ends with the given character \p{consumable}. If it does, this
    /// character is cut from the end of object.
    ///
    /// @tparam TSensitivity       The sensitivity of the comparison.
    ///                            Defaults to \b Case::Sensitive.
    /// @tparam TTrimBeforeConsume Determines if the string should be (left-) trimmed before the
    ///                            consume operation. Defaults to \b Whitespaces::Keep.
    /// @param  consumable         The consumable character.
    /// @return \c true, if this object was starting with \p{consumable} and consequently the
    ///         string was cut by one.
    template< lang::Case        TSensitivity=       lang::Case::Sensitive,
              lang::Whitespaces TTrimBeforeConsume= lang::Whitespaces::Keep>
    bool        ConsumeCharFromEnd( TChar  consumable )
    {
        if ( TTrimBeforeConsume == lang::Whitespaces::Trim )
            TrimEnd();

        if (    ( TSensitivity == lang::Case::Sensitive &&    base::CharAtEnd()  !=    consumable  )
             || ( TSensitivity == lang::Case::Ignore    &&    characters::ToUpper(base::CharAtEnd())
                                                           != characters::ToUpper(consumable) ) )
            return false;
        --base::length;
        return true;
    }

    /// Retrieve and remove the last character in the substring.
    ///
    /// @tparam TCheck  Defaults to \alib{CHK}, which is the normal invocation mode.
    ///                 If \c \<false\> is added to the method name, no check whether this
    ///                 string is empty is performed.
    ///
    /// @tparam TTrimBeforeConsume Determines if the string should be (right-) trimmed before the
    ///                            consume operation. Defaults to \b Whitespaces::Keep.
    /// @return The character at the start of the represented region.
    ///         If this \b %Substring is empty or \e nulled, '\0' is returned.
    template <typename TCheck= CHK,
              lang::Whitespaces TTrimBeforeConsume= lang::Whitespaces::Keep >
    TChar     ConsumeCharFromEnd()
    {
        if constexpr ( TTrimBeforeConsume == lang::Whitespaces::Trim )
            TrimEnd();

        if constexpr ( TCheck::value )
        {
            if( base::IsEmpty() )
                return '\0';
        }
        else
        {
            ALIB_ASSERT_ERROR(!base::IsEmpty(),"STRINGS","Non checking but called on empty string" )
        }
        return *(base::buffer + --base::length );
    }

    /// Cuts the given number of characters from the beginning of the Substring and optionally
    /// places the portion that was cut in parameter \p{target} (if provided).<br>
    ///
    /// If parameter \p{regionLength} is negative, nothing is cut and optional argument
    /// \p{target} is set empty. If \p{regionLength} is equal or greater than this
    /// object's length, all contents is 'moved' to \p{target}.
    ///
    /// @tparam TCheck  Defaults to \alib{CHK}, which is the normal invocation mode.
    ///                 If \c \<false\>, parameter \p{regionLength} has to be in the range of
    ///                 this object's size.
    ///
    /// @param regionLength  The length of the region at the start to delete.
    /// @param target        An optional target \b %Substring that receives the portion that
    ///                      is cut from this object. Defaults to nullptr.
    ///
    /// @return The new length of the substring.
    template <typename TCheck= CHK>
    integer  ConsumeChars( integer regionLength, TSubstring* target= nullptr )
    {
        if constexpr ( TCheck::value )
        {
            if ( regionLength < 0 )
            {
                if ( target != nullptr )
                    target->Clear();
                return  base::length;
            }
            if ( regionLength > base::length )
                regionLength= base::length;
        }
        else
        {
            ALIB_ASSERT_ERROR( regionLength >=0 && regionLength <= base::length,
                 "Non checking and index out of range: 0 <= {} <= {}.", regionLength, base::length )

        }

        if ( target != nullptr )
            *target= this->base::template Substring<NC>( 0, regionLength );

        base::buffer+= regionLength;
        base::length-= regionLength;
        return base::length;
    }

    /// Cuts the given number of characters from the end of the \b %Substring and optionally
    /// places the portion that was cut in parameter \p{target} (if provided).<br>
    /// Parameter \p{regionLength} is checked to be between 0 and length. If negative, nothing
    /// is cut and \p{target} is set empty. If \p{regionLength} is greater than this
    /// object's length, all contents is 'moved' to \p{target}.
    ///
    /// @tparam TCheck  Defaults to \alib{CHK}, which is the normal invocation mode.
    ///                 If \c \<false\> is added to the method name, no parameter check is
    ///                 performed.
    ///
    /// @param regionLength  The length of the region at the start to delete.
    /// @param target        An optional target \b %Substring that receives the portion that
    ///                      is cut from this object. Defaults to nullptr.
    ///
    /// @return The new length of the substring.
    template <typename TCheck= CHK>
    integer   ConsumeCharsFromEnd( integer regionLength, TSubstring* target= nullptr )
    {
        if constexpr ( TCheck::value )
        {
            if ( regionLength < 0 )
            {
                if ( target != nullptr )
                    target->Clear();
                return  base::length;
            }
            if ( regionLength > base::length )
                regionLength= base::length;
        }
        else
        {
            ALIB_ASSERT_ERROR( regionLength >=0 && regionLength <= base::length,
                 "Non checking and index out of range: 0 <= {} <= {}.", regionLength, base::length )
        }

        if ( target != nullptr )
            *target= this->base::template Substring<NC>( base::length - regionLength, regionLength );

        base::length-= regionLength;
        return base::length;
    }

    /// Cuts the given number of characters from the beginning of the Substring and optionally
    /// places the portion that was cut in parameter \p{target}.<br>
    /// Parameter \p{regionLength} is checked to be between 0 and length. If negative, nothing
    /// is cut and \p{target} is set empty, respectively left untouched depending on
    /// \p{TTargetData}.
    ///
    /// If \p{regionLength} is greater than this  object's length, all contents is 'moved'
    /// to \p{target}.
    ///
    /// @tparam TCheck        Defaults to \alib{CHK}, which is the normal invocation mode.
    ///                       If \c \<false\> is added to the method name, no parameter check
    ///                       is performed.
    /// @tparam TTargetData   If \c CurrentData::Keep, \b %AString \p{target} is not cleared
    ///                       before the result is written. Defaults to \c CurrentData::Clear.
    /// @tparam TAllocator    The allocation type of the \p{target} string, as prototyped with
    ///                       \alib{lang;Allocator}. Deduced by the compiler.
    /// @param regionLength   The length of the region at the start to delete.
    /// @param target         A target \b %AString that receives the portion that
    ///                       is cut from this object.
    /// @param separatorWidth This width is added to what is cut from this string, while
    ///                       \p{target} still receives the portion defined by \p{regionLength}.
    ///                       Defaults to 0.
    ///
    /// @return The new length of the substring.
    template <typename           TCheck           = CHK,
              lang::CurrentData  TTargetData      = lang::CurrentData::Clear,
              typename           TAllocator                                       >
    integer ConsumeChars( integer                      regionLength,
                          TAString<TChar, TAllocator>& target,
                          integer                      separatorWidth   =0         )
    {
        if constexpr ( TTargetData == lang::CurrentData::Clear  )
            target.Reset();

        if constexpr ( TCheck::value )
        {
            if ( separatorWidth < 0 )
                separatorWidth= 0;

            if ( regionLength   < 0 )
                return  base::length;

            if ( regionLength   > base::length - separatorWidth )
            {
                regionLength= base::length - separatorWidth;
                if ( regionLength   < 0 )
                    return  base::length;
            }

        }
        else
        {
            ALIB_ASSERT_ERROR( separatorWidth  >= 0, "STRINGS",
                               "Non checking but separator width negative: ", separatorWidth )
            ALIB_ASSERT_ERROR(    regionLength >= 0
                               && regionLength + separatorWidth <= base::length, "STRINGS",
                               "Non checking but regionLength out of bounds: 0 <= {}, {} <= {}.",
                               regionLength, regionLength + separatorWidth, base::length )
        }

        target.template _<NC>( *this, 0, regionLength );

        regionLength+= separatorWidth;
        base::buffer+= regionLength ;
        base::length-= regionLength;
        return base::length;
    }

    /// Cuts the given number of characters from the beginning of the Substring and optionally
    /// places the portion that was cut in parameter \p{target}.<br>
    /// Parameter \p{regionLength} is checked to be between 0 and length. If negative, nothing
    /// is cut and \p{target} is set empty, respectively left untouched depending on
    /// \p{TTargetData}.
    ///
    /// If \p{regionLength} is greater than this  object's length, all contents is 'moved'
    /// to \p{target}.
    ///
    /// @tparam TCheck  Defaults to \alib{CHK}, which is the normal invocation mode.
    ///                 If \c \<false\> is added to the method name, no parameter check is
    ///                 performed.
    ///
    /// @param regionLength    The length of the region at the start to delete.
    /// @param target          A target \b %String that receives the portion that
    ///                        is cut from this object.
    /// @param separatorWidth  This width is added to what is cut from this string, while
    ///                        \p{target} still receives the portion defined by \p{regionLength}.
    ///                        Defaults to 0.
    ///
    /// @return The new length of the substring.
    template <typename TCheck = CHK>
    integer ConsumeChars( integer          regionLength,
                          TString<TChar>&  target,
                          integer          separatorWidth   =0         )
    {
        if constexpr ( TCheck::value )
        {
            if ( separatorWidth < 0 )
                separatorWidth= 0;

            if ( regionLength   < 0 )
                return  base::length;

            if ( regionLength   > base::length - separatorWidth )
            {
                regionLength= base::length - separatorWidth;
                if ( regionLength   < 0 )
                    return  base::length;
            }
        }
        else
        {
            ALIB_ASSERT_ERROR( separatorWidth  >= 0,
                               "STRINGS", "Non checking but separator width negative"  )
            ALIB_ASSERT_ERROR(    regionLength >= 0
                               && regionLength + separatorWidth <= base::length, "STRINGS",
                               "Non checking but regionLength out of bounds: 0 <= {}, {} <= {}.",
                               regionLength, regionLength + separatorWidth, base::length )
        }

        target= String( base::buffer, regionLength );

        regionLength+= separatorWidth;
        base::buffer+= regionLength ;
        base::length-= regionLength;
        return base::length;
    }

    /// Cuts the given number of characters from the end of the substring and
    /// places the portion that was cut in parameter \p{target}.<br>
    /// Parameter \p{regionLength} is checked to be between 0 and length. If negative, nothing
    /// is cut and \p{target} is set empty, respectively left untouched depending on \p
    /// If \p{regionLength} is greater than this object's length, all contents is 'moved'
    /// to \p{target}.
    ///
    /// @tparam TCheck         Defaults to \alib{CHK}, which is the normal invocation mode.
    ///                        If \c \<false\> is added to the method name, no parameter check is
    ///                        performed.
    /// @tparam TTargetData    If \c CurrentData::Keep, the parameter \p{target} is not cleared
    ///                        before the result is written. Defaults to \c CurrentData::Clear.
    /// @tparam TAllocator     The allocator-type of the \p{target}.
    ///                        Deduced by the compiler.
    ///
    /// @param regionLength    The length of the region at the start to delete.
    /// @param target          A target \b %AString that receives the portion that
    ///                        is cut from this object.
    /// @param separatorWidth  This width is added to what is cut from this string, while
    ///                        \p{target} still receives the portion defined by \p{regionLength}.
    ///                        Defaults to 0.
    ///
    /// @return The new length of the substring.
    template <typename           TCheck           = CHK,
              lang::CurrentData  TTargetData      = lang::CurrentData::Clear,
              typename           TAllocator >
    integer ConsumeCharsFromEnd( integer                        regionLength,
                                 TAString<TChar,TAllocator>&    target,
                                 integer                        separatorWidth   =0      )
    {
        if constexpr ( TTargetData == lang::CurrentData::Clear  )
            target.Reset();

        if constexpr ( TCheck::value )
        {
            if ( separatorWidth < 0 )                              separatorWidth= 0;
            if ( regionLength   < 0 )                              return  base::length;
            if ( regionLength   > base::length - separatorWidth )  regionLength= base::length - separatorWidth;
            if ( regionLength   < 0 )                              return  base::length;
        }
        else
        {
            ALIB_ASSERT_ERROR( separatorWidth >=0 ,
                               "STRINGS", "Non checking but separator width negative"  )
            ALIB_ASSERT_ERROR( regionLength >=0 && regionLength + separatorWidth <= base::length,
                    "STRINGS", "Non checking but regionLength out of bounds: 0 <= {}, {} <= {}.",
                               regionLength, regionLength + separatorWidth, base::length )
        }

        target.template _<NC>( *this, base::length - regionLength, regionLength );

        base::length-= regionLength + separatorWidth;
        return base::length;
    }

    /// Searches \p{separator} and cuts the beginning of this string.
    /// What was consumed is returned.
    ///
    /// If the separator is \b not found, all of this string is consumed and returned.
    ///
    /// @param separator         The separator to search.  Defaults to <c>','</c>.
    /// @param includeSeparator  Determines if the separator should be consumed as well.
    ///                          defaults to \alib{lang;Inclusion;Include}
    ///
    /// @return The token consumed.
    TString<TChar>  ConsumeToken( TChar separator= ',',
                                  lang::Inclusion includeSeparator = lang::Inclusion::Include  )
    {
        ALIB_ASSERT_ERROR( base::IsNotNull() , "STRINGS", "ConsumeToken on nulled Substring" )

        integer        separatorPos= base::IndexOfOrLength( separator );
        base result      = base( base::buffer, separatorPos );

        base::buffer+= separatorPos;
        base::length-= separatorPos;
        if(    includeSeparator == lang::Inclusion::Include
            && base::length > 0 )
        {
            ++base::buffer;
            --base::length;
        }
        return result;
    }

    /// Checks if this object starts with the given string \p{consumable}. If it does, this
    /// string is cut from this object.
    ///
    /// @tparam TSensitivity       The sensitivity of the comparison.
    ///                            Defaults to \b Case::Sensitive.
    /// @tparam TTrimBeforeConsume Determines if the string should be (left-) trimmed before the
    ///                            consume operation. Defaults to \b Whitespaces::Keep.
    /// @param  consumable         The consumable string.
    /// @return \c true, if this object was starting with \p{consumable} and consequently the
    ///         string was cut.
    template< lang::Case        TSensitivity=       lang::Case::Sensitive,
              lang::Whitespaces TTrimBeforeConsume= lang::Whitespaces::Keep >
    bool        ConsumeString( const TString<TChar>&     consumable  )
    {
        if constexpr ( TTrimBeforeConsume == lang::Whitespaces::Trim )
            TrimStart();

        if ( !base::template StartsWith<CHK,TSensitivity>( consumable ) )
            return false;

        base::buffer+= consumable.Length();
        base::length-= consumable.Length();
        return true;
    }

    /// Checks if this object ends with the given string \p{consumable}. If it does, this
    /// string is cut from the end of object.
    ///
    /// @tparam TSensitivity       The sensitivity of the comparison.
    ///                            Defaults to \b Case::Sensitive.
    /// @tparam TTrimBeforeConsume Determines if the string should be (left-) trimmed before the
    ///                           consume operation. Defaults to \b Whitespaces::Keep.
    /// @param  consumable         The consumable string
    /// @return \c true, if this object was starting with \p{consumable} and consequently the
    ///         string was cut.
    template< lang::Case        TSensitivity=       lang::Case::Sensitive,
              lang::Whitespaces TTrimBeforeConsume= lang::Whitespaces::Keep >
    bool        ConsumeStringFromEnd( const TString<TChar>&  consumable )
    {
        if constexpr ( TTrimBeforeConsume == lang::Whitespaces::Trim )
            TrimEnd();

        if ( !base::template EndsWith<CHK,TSensitivity>( consumable ) )
            return false;
        base::length-= consumable.Length();
        return true;
    }

    /// Consumes a minimum of \p{minChars} of string \p{consumable} from the start of this
    /// substring. If the minimum characters could not be found, nothing is consumed, otherwise,
    /// the method consumes as much as possible.<br>
    ///
    /// This method is useful read "tokens" from a string that may be abbreviated.
    /// Within \alib this method is for example used with
    /// \ref alib_enums_records_details_serialization "deserialization of enumeration elements".
    ///
    /// @tparam TSensitivity       The sensitivity of the comparison.
    ///                            Defaults to \b Case::Ignore.
    /// @tparam TTrimBeforeConsume Determines if the string should be (left-) trimmed before the
    ///                            first character consume operation.
    ///                            Defaults to \b Whitespaces::Keep.
    /// @param  consumable         The consumable string.
    /// @param  minChars           The minimum amount of characters to consume. If \c 0 or
    ///                            negative, the length of \p{consumable} is chosen.
    ///                            Optional and defaults to \c 1.
    /// @return The amount of characters consumed.
    template< lang::Case        TSensitivity=       lang::Case::Ignore,
              lang::Whitespaces TTrimBeforeConsume= lang::Whitespaces::Keep >
    integer    ConsumePartOf(  const TString<TChar>&     consumable,
                               int                       minChars           = 1 )
    {
        if constexpr ( TTrimBeforeConsume == lang::Whitespaces::Trim )
            TrimStart();
        if ( minChars <= 0 )
            minChars= int( consumable.Length() );

        if ( minChars == 0 || minChars > consumable.Length() )
            return 0;

        integer diff= base::IndexOfFirstDifference( consumable, TSensitivity );
        if( diff < integer( minChars ) )
            return 0;
        ConsumeChars( diff );
        return diff;
    }

    /// Consumes a field from the beginning of this substring, which is surrounded by
    /// given start end end character identifiers. If both are the same, e.g., \c '"', then
    /// the first occurrence of the end character is used. If they are not the same, e.g.
    /// \c '<' and \c '>', then repeated start characters are counted and consumption only ends
    /// when a corresponding amount of end characters has been found.
    ///
    /// @tparam TTrimBeforeConsume Determines if the string should be (left-) trimmed before the
    ///                            consume operation. Defaults to \b Whitespaces::Keep.
    /// @param  startChar  The start character of the field to consume.
    /// @param  endChar    The end character of the field to consume.
    /// @return The string consumed. \b NULL_STRING on error (start/end character not found)
    template< lang::Whitespaces TTrimBeforeConsume= lang::Whitespaces::Keep >
    TString<TChar>  ConsumeField( TChar startChar, TChar endChar  )
    {
        if constexpr ( TTrimBeforeConsume == lang::Whitespaces::Trim )
            TrimStart();

        integer endIdx;
        if (    base::CharAtStart() != startChar
             || (endIdx= base::IndexOfSegmentEnd( startChar, endChar, 1)) < 0  )
            return nullptr;


        base result= base( base::buffer + 1, endIdx - 1 );
        base::buffer+= (endIdx + 1);
        base::length-= (endIdx + 1);
        return result;
    }

    /// Consumes all characters \c '0' to \c '9' at the start of this object and stores the
    /// value they represent in \p{result}.<br>
    /// Unlike methods #ConsumeInt or #ConsumeDec, this method does not consume (respectively
    /// accept) sign-, whitespace- or group-characters.
    ///
    /// @param [out] result    A reference to a variable of an integral type which receives the
    ///                        result value.
    ///
    /// @return  \c true if a number was found and consumed, \c false otherwise.
    inline bool   ConsumeDecDigits( std::integral auto& result )
    {
        uint64_t resultImpl;
        bool    returnValue= consumeDecDigitsImpl( resultImpl );
        result= static_cast<std::remove_reference_t<decltype(result)>>(resultImpl);

        return  returnValue;
    }

    /// Consumes an integral value in decimal, binary, hexadecimal or octal format from the
    /// string.
    ///
    /// Parameter \p{numberFormat} defaults to \c nullptr. This denotes static singleton
    /// \alib{strings;TNumberFormat::Computational;NumberFormat::Computational}
    /// which is configured to not using (not allowing) grouping characters.
    ///
    /// For more information on number conversion, see class
    /// \alib{strings;TNumberFormat;NumberFormat}.
    ///
    /// @param [out] result    A reference to a variable of an integral type which receives the
    ///                        result value.
    /// @param  numberFormat   The number format to use. Defaults to \c nullptr.
    /// @return  \c true if a number was found and consumed, \c false otherwise.
    bool ConsumeInt( std::integral auto& result, TNumberFormat<TChar>* numberFormat= nullptr )
    {
        int64_t resultImpl;
        bool    returnValue= consumeIntImpl( resultImpl, numberFormat );
        result= static_cast<std::remove_reference_t<decltype(result)>>( resultImpl );
        return  returnValue;
    }

    /// Consumes an unsigned integer in standard decimal format from the start of this %AString.
    ///
    /// Parameter \p{numberFormat} defaults to \c nullptr. This denotes static singleton
    /// \alib{strings;TNumberFormat::Computational;NumberFormat::Computational}
    /// which is configured to not using (not allowing) grouping characters.
    ///
    /// Sign literals \c '-' or \c '+' are \b not accepted and parsing will fail.
    /// For reading signed integral values, see methods #ConsumeInt, for floating point numbers
    /// #ConsumeFloat.
    ///
    /// For more information on number conversion, see class
    /// \alib{strings;TNumberFormat;NumberFormat}.
    ///
    /// @param [out] result    A reference to a variable of an integral type which receives the
    ///                        result value.
    /// @param numberFormat    The number format to use. Defaults to \c nullptr.
    /// @return  \c true if a number was found and consumed, \c false otherwise.
    bool   ConsumeDec( std::integral auto& result, TNumberFormat<TChar>* numberFormat= nullptr )
    {
        uint64_t resultImpl;
        bool     returnValue= consumeDecImpl( resultImpl, numberFormat );
        result=  static_cast<std::remove_reference_t<decltype(result)>>( resultImpl );
        return   returnValue;
    }

    /// Consumes an unsigned integer in binary format from the start of this string.
    ///
    /// Parameter \p{numberFormat} defaults to \c nullptr. This denotes static singleton
    /// \alib{strings;TNumberFormat::Computational;NumberFormat::Computational}
    /// which is configured to not using (not allowing) grouping characters.
    ///
    /// For more information on number conversion, see class
    /// \alib{strings;TNumberFormat;NumberFormat}.
    ///
    /// @param [out] result    A reference to the result value.
    /// @param numberFormat    The number format to use. Defaults to \c nullptr.
    /// @return  \c true if a number was found and consumed, \c false otherwise.
    bool   ConsumeBin( std::integral auto& result, TNumberFormat<TChar>* numberFormat= nullptr )
    {
        uint64_t resultImpl;
        bool     returnValue= consumeBinImpl( resultImpl, numberFormat );
        result=  static_cast<std::remove_reference_t<decltype(result)>>( resultImpl );
        return   returnValue;
    }

    /// Consumes an unsigned integer in hexadecimal format from the start of this string.
    ///
    /// Parameter \p{numberFormat} defaults to \c nullptr. This denotes static singleton
    /// \alib{strings;TNumberFormat::Computational;NumberFormat::Computational}
    /// which is configured to not using (not allowing) grouping characters.
    ///
    /// For more information on number conversion, see class
    /// \alib{strings;TNumberFormat;NumberFormat}.
    ///
    /// @param [out] result    A reference to a variable of an integral type which receives the
    ///                        result value.
    /// @param numberFormat    The number format to use. Defaults to \c nullptr.
    ///
    /// @return  \c true if a number was found and consumed, \c false otherwise.
    bool   ConsumeHex( std::integral auto& result, TNumberFormat<TChar>* numberFormat= nullptr )
    {
        uint64_t resultImpl;
        bool     returnValue= consumeHexImpl( resultImpl, numberFormat );
        result=  static_cast<std::remove_reference_t<decltype(result)>>( resultImpl );
        return   returnValue;
    }

    /// Consumes an unsigned integer in octal format from the start of this string.
    ///
    /// Parameter \p{numberFormat} defaults to \c nullptr. This denotes static singleton
    /// \alib{strings;TNumberFormat::Computational;NumberFormat::Computational}
    /// which is configured to not use (not allow) grouping characters.
    ///
    /// For more information on number conversion, see class
    /// \alib{strings;TNumberFormat;NumberFormat}.
    ///
    /// @param [out] result    A reference to a variable of an integral type which receives the
    ///                        result value.
    /// @param numberFormat    The number format to use. Defaults to \c nullptr.
    ///
    /// @return  \c true if a number was found and consumed, \c false otherwise.
    bool   ConsumeOct( std::integral auto& result, TNumberFormat<TChar>* numberFormat= nullptr )
    {
        uint64_t resultImpl;
        bool     returnValue= consumeOctImpl( resultImpl, numberFormat );
        result=  static_cast<std::remove_reference_t<decltype(result)>>( resultImpl );
        return   returnValue;
    }

    /// Consumes a floating point number from the start of this string.
    ///
    /// on the given \p{numberFormat} instance.<br>
    /// Parameter \p{numberFormat} defaults to \c nullptr. This denotes static singleton
    /// \alib{strings;TNumberFormat::Computational;NumberFormat::Computational}
    /// which is configured to 'international' settings (not using the locale) and therefore
    /// also not parsing grouping characters.
    ///
    /// For more information on parsing options for floating point numbers and number
    /// conversion in general, see class
    /// \alib{strings;TNumberFormat;NumberFormat}.
    ///
    /// @param [out] result    A reference to the result value.
    /// @param numberFormat    The number format to use. Defaults to \c nullptr.
    ///
    /// @return  \c true if a number was found and consumed, \c false otherwise.
    ALIB_DLL
    bool  ConsumeFloat( double&                 result,
                        TNumberFormat<TChar>*   numberFormat     =nullptr   );


    /// Splits this substring into two parts. What remains in this object is the region
    /// from 0 to \p{position}.
    /// \p{target} receives the rest. If \p{separatorWidth} is given, this is subtracted from
    /// the front of \p{target}.
    ///
    /// @tparam TCheck  Defaults to \alib{CHK}, which is the normal invocation mode.
    ///                 If \c \<false\> is added to the method name, no parameter check is
    ///                 performed.
    ///
    /// @param position        The index where this object is split.
    /// @param target          The target substring to receive the right part of the string.
    /// @param separatorWidth  This does not change what remains in this object, but defines
    ///                        the number of characters that are cut from the front of the
    ///                        \p{target}. Defaults to 0.
    /// @param trim            If \c true, both substrings will be trimmed.
    ///
    /// @return \c *this to allow concatenated calls.
    template <typename TCheck= CHK>
    TSubstring& Split( integer position, TSubstring& target, integer separatorWidth =0,
                       bool trim= false )

    {
        if constexpr ( TCheck::value )
            base::AdjustRegion( position, separatorWidth );
        else
        {
            ALIB_ASSERT_ERROR( position >=0 && position <= base::length, "STRINGS",
                  "Non checking but position out of bounds: 0 <= {} < {}.", position, base::length )
            ALIB_ASSERT_ERROR( position + separatorWidth <= base::length, "STRINGS",
                  "Non checking but position + separator width out of bounds: {} <= {}.",
                  position + separatorWidth, base::length )
        }

        target= this->base::template Substring<NC>( position + separatorWidth,
                                                       base::length - position - separatorWidth );
        base::length= position;
        if( trim )
        {
            target.Trim();
            this ->Trim();
        }
        return *this;
    }

  //################################################################################################
  // Protected Methods
  //################################################################################################
  protected:
    /// Implementation of #ConsumeDecDigits (the non-inline part).
    ///
    /// @param [out] result    A reference to the result value.
    /// @return  \c true if a number was found and consumed, \c false otherwise.
    ALIB_DLL  bool   consumeDecDigitsImpl( uint64_t& result );

    /// Implementation of #ConsumeInt (the non-inline part).
    ///
    /// @param [out] result    A reference to the result value.
    /// @param numberFormat    The number format to use.
    /// @return  \c true if a number was found and consumed, \c false otherwise.
    ALIB_DLL  bool   consumeIntImpl( int64_t& result, TNumberFormat<TChar>* numberFormat );

    /// Implementation of #ConsumeDec (the non-inline part).
    ///
    /// @param [out] result    A reference to the result value.
    /// @param numberFormat    The number format to use.
    /// @return  \c true if a number was found and consumed, \c false otherwise.
    ALIB_DLL  bool   consumeDecImpl( uint64_t& result, TNumberFormat<TChar>* numberFormat );

    /// Implementation of #ConsumeBin (the non-inline part).
    ///
    /// @param [out] result    A reference to the result value.
    /// @param numberFormat    The number format to use.
    /// @return  \c true if a number was found and consumed, \c false otherwise.
    ALIB_DLL  bool   consumeBinImpl( uint64_t& result, TNumberFormat<TChar>* numberFormat );

    /// Implementation of #ConsumeHex (the non-inline part).
    ///
    /// @param [out] result    A reference to the result value.
    /// @param numberFormat    The number format to use.
    /// @return  \c true if a number was found and consumed, \c false otherwise.
    ALIB_DLL  bool   consumeHexImpl( uint64_t& result, TNumberFormat<TChar>* numberFormat );

    /// Implementation of #ConsumeOct (the non-inline part).
    ///
    /// @param [out] result    A reference to the result value.
    /// @param numberFormat    The number format to use.
    /// @return  \c true if a number was found and consumed, \c false otherwise.
    ALIB_DLL  bool   consumeOctImpl( uint64_t& result, TNumberFormat<TChar>* numberFormat );
}; // class TSubstring

// #################################################################################################
// TSubstring type aliases
// #################################################################################################
}
/// Type alias in namespace \b alib.
using  Substring        =     strings::TSubstring <character>;

/// Type alias in namespace \b alib.
using  ComplementSubstring =  strings::TSubstring <complementChar>;

/// Type alias in namespace \b alib.
using  StrangeSubstring =     strings::TSubstring <strangeChar>;

/// Type alias in namespace \b alib.
using  NSubstring       =     strings::TSubstring <nchar>;

/// Type alias in namespace \b alib.
using  WSubstring       =     strings::TSubstring <wchar>;

/// Type alias in namespace \b alib.
using  XSubstring       =     strings::TSubstring <xchar>;

// #################################################################################################
// Specializations of ArrayTraits for this class Substring
// #################################################################################################
namespace characters {
#if !DOXYGEN
template<typename TChar>    struct  ArrayTraits<strings::TSubstring<TChar>, TChar>
{
    using T= strings::TSubstring<TChar>;
    static constexpr Policy       Access                               = Policy::Implicit;
    static constexpr Policy       Construction                         = Policy::Implicit;
    static constexpr const TChar* Buffer   (const T& src)              { return src.Buffer(); }
    static constexpr integer      Length   (const T& src)              { return src.Length(); }
    static constexpr T            Construct(const TChar* b, integer l) { return T(b, l); }
};
#endif // !DOXYGEN

} namespace strings {


extern template ALIB_DLL bool TSubstring<nchar>::ConsumeFloat        (   double& , TNumberFormat<nchar>* );
extern template ALIB_DLL bool TSubstring<nchar>::consumeDecDigitsImpl( uint64_t&                         );
extern template ALIB_DLL bool TSubstring<nchar>::consumeIntImpl      (  int64_t& , TNumberFormat<nchar>* );
extern template ALIB_DLL bool TSubstring<nchar>::consumeDecImpl      ( uint64_t& , TNumberFormat<nchar>* );
extern template ALIB_DLL bool TSubstring<nchar>::consumeBinImpl      ( uint64_t& , TNumberFormat<nchar>* );
extern template ALIB_DLL bool TSubstring<nchar>::consumeHexImpl      ( uint64_t& , TNumberFormat<nchar>* );
extern template ALIB_DLL bool TSubstring<nchar>::consumeOctImpl      ( uint64_t& , TNumberFormat<nchar>* );

extern template ALIB_DLL bool TSubstring<wchar>::ConsumeFloat        (   double& , TNumberFormat<wchar>* );
extern template ALIB_DLL bool TSubstring<wchar>::consumeDecDigitsImpl( uint64_t&                         );
extern template ALIB_DLL bool TSubstring<wchar>::consumeIntImpl      (  int64_t& , TNumberFormat<wchar>* );
extern template ALIB_DLL bool TSubstring<wchar>::consumeDecImpl      ( uint64_t& , TNumberFormat<wchar>* );
extern template ALIB_DLL bool TSubstring<wchar>::consumeBinImpl      ( uint64_t& , TNumberFormat<wchar>* );
extern template ALIB_DLL bool TSubstring<wchar>::consumeHexImpl      ( uint64_t& , TNumberFormat<wchar>* );
extern template ALIB_DLL bool TSubstring<wchar>::consumeOctImpl      ( uint64_t& , TNumberFormat<wchar>* );

extern template ALIB_DLL bool TSubstring<xchar>::ConsumeFloat        (   double& , TNumberFormat<xchar>* );
extern template ALIB_DLL bool TSubstring<xchar>::consumeDecDigitsImpl( uint64_t&                         );
extern template ALIB_DLL bool TSubstring<xchar>::consumeIntImpl      (  int64_t& , TNumberFormat<xchar>* );
extern template ALIB_DLL bool TSubstring<xchar>::consumeDecImpl      ( uint64_t& , TNumberFormat<xchar>* );
extern template ALIB_DLL bool TSubstring<xchar>::consumeBinImpl      ( uint64_t& , TNumberFormat<xchar>* );
extern template ALIB_DLL bool TSubstring<xchar>::consumeHexImpl      ( uint64_t& , TNumberFormat<xchar>* );
extern template ALIB_DLL bool TSubstring<xchar>::consumeOctImpl      ( uint64_t& , TNumberFormat<xchar>* );

} // namespace alib::[strings]
} // namespace [alib]

// Note(25/01/17):
//   Clang strangely did not find the following templated operators when they resided in an
//   exported namespace.
//   The workaround was to not export the namespace but export each operator instead.
//   We think this is wrong behavior and not aligned with the language specification.
#if !DOXYGEN
namespace alib::strings {

ALIB_EXPORT
template<typename TChar>
bool operator== (const TSubstring<TChar>& lhs, const TSubstring<TChar>& rhs)
{ return  lhs. template Equals   <CHK, lang::Case::Sensitive>(rhs); }

ALIB_EXPORT
template<typename TChar, typename T>
requires (!std::is_same_v<T, TSubstring<TChar>>)
bool operator== (const TSubstring<TChar>& lhs, const T&     rhs)
{ return  lhs. template Equals   <CHK, lang::Case::Sensitive>(rhs); }

ALIB_EXPORT
template<typename TChar>
auto operator<=> (const TSubstring<TChar>& lhs, const TSubstring<TChar>& rhs)
{ return  lhs. template CompareTo<CHK, lang::Case::Sensitive>(rhs); }

ALIB_EXPORT
template<typename TChar, typename T>
requires (!std::same_as<TSubstring<TChar>, T>)
auto operator<=> (const TSubstring<TChar>& lhs, const T&     rhs)
{ return  lhs. template CompareTo<CHK, lang::Case::Sensitive>(rhs); }

} // namespace [alib::strings]
#endif // !DOXYGEN
