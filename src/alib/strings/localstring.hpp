// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#ifndef HPP_ALIB_STRINGS_LOCALSTRING
#define HPP_ALIB_STRINGS_LOCALSTRING 1

#if !defined (HPP_ALIB_STRINGS_ASTRING)
#   include "alib/strings/astring.hpp"
#endif

namespace aworx { namespace lib { namespace strings {

/** ************************************************************************************************
 * This type specializes class \b AString by adding a character array member of templated size
 * which is used as the "external" string buffer. The single motivation for the use of this
 * class is performance optimization.
 *
 * Assembled character strings often have a rather short lifetime and are disposed quickly
 * after their usage. If the maximum length of the assembled string is known, such strings
 * may be created as local values of this type, with that maximum length given as template
 * parameter \p{TCapacity}. With that, the allocation and de-allocation of heap memory for the
 * string is omitted and allocation and destruction of the local string is performed in constant
 * time O(1).
 *
 * But even if an instance of this class is allocated on the heap (using keyword \c new), still some
 * performance improvements apply, because besides the class object itself, no second memory
 * allocation for the string buffer is performed.
 *
 * If during the use of an instance of this class the buffer's fixed \p{TCapacity} is exceeded,
 * a new buffer is allocated from the heap, just as it is always done with growing strings managed
 * in \b AString objects. This means, the use of this class is safe and no restrictions apply.
 * Of-course, if possible, for performance critical code sections, the predefined size \p{TCapacity}
 * should be chosen large enough to allow the internal buffer to survive the use.<br>
 * In debug compilations of \alib, parent class #AString optionally and by default raises a
 * \ref aworx::lib::results::Report::DoReport "warning" if an external buffer is replaced by a
 * new (heap) allocation. (Note that from an \b %AString perspective, this class's internal
 * field #buffer is an external character array). With use cases that do not allow to forsee a
 * maximum buffer size, the warning has to be disabled by invoking
 * \alib{strings::TAString,DbgDisableBufferReplacementWarning}.
 *
 * For more information on warnings, see \alib{strings,TAString::SetBuffer,AString::SetBuffer}.
 *
 * \note
 *   For commonly used sizes and character types, some convenient typedefs exists in namespace aworx.
 *   Type definitions are:
 *   - #String8, #String16, #String32 ... #String1K, #String2K, #String4K,
 *   - #NString8, #NString16, #NString32 ... #NString1K, #NString2K, #NString4K, and
 *   - #WString8, #WString16, #WString32 ... #WString1K, #WString2K, #WString4K.
 *
 * @tparam TChar The character type.<br>
 *   Alias names for specializations of this class using character types
 *   \alib{characters,character}, \alib{characters,nchar}, \alib{characters,wchar},
 *   \alib{characters,xchar}, \alib{characters,complementChar} and \alib{characters,strangeChar}
 *   are provided in namespace #aworx with type definitions \b LocalString<TCapacity>,
 *   \b NLocalString<TCapacity>, \b WLocalString<TCapacity>,
 *   \b XLocalString<TCapacity>, \b ComplementLocalString<TCapacity> and
 *   \b StrangeLocalString<TCapacity>.
 * @tparam TCapacity The capacity of the buffer allocated inside the class with member #buffer.
 **************************************************************************************************/
template <typename TChar, integer TCapacity>
class TLocalString : public TAString<TChar>
{
    // #############################################################################################
    //  protected fields
    // #############################################################################################
    protected:
        /** The internal buffer with size specified by the template parameter \p{TCapacity}.
         *  Passed as an external buffer to parent class \b AString.*/
        TChar         buffer[TCapacity];

    // #############################################################################################
    //  Constructors/Destructor
    // #############################################################################################
    public:
        /** ****************************************************************************************
         * Constructs an empty \b %LocalString. Field #buffer will be set as an the external
         * buffer of parent \b AString.
         * Unlike all other \alib string classes, objects of this type are not \e nulled
         * after default construction.
         ******************************************************************************************/
        constexpr
        TLocalString()
        : TAString<TChar>( buffer, TCapacity )
        , buffer {}
        {}

        /** ****************************************************************************************
         * Copy constructor. Copies the string data of parameter \p{copy} to this instance
         * @param copy The object to copy the contents from.
         ******************************************************************************************/
        TLocalString(const TLocalString& copy)
        : TAString<TChar>( buffer, TCapacity )
        {
            TAString<TChar>::Append( copy.Buffer(), copy.Length() );
        }

        /** ****************************************************************************************
         * Constructs the object and uses parent's method \b %Append to create a string
         * representation of given "appendable" object \p{src}.
         *
         * \see
         *   Manual chapter \ref alib_strings_assembly_ttostring for more information
         *   about which types are supported and how external, user defined types can be made
         *   compatible to this implicit constructor.
         *
         * @tparam TAppendable  The type of parameter \p{src} that has a specialization of
         *                      functor \alib{strings,T_Append}.
         * @param src  The source to take the buffer and length from of template type T.
         ******************************************************************************************/
        template <class TAppendable>
        TLocalString (const  TAppendable& src )
        : TAString<TChar>( buffer, TCapacity )
        {
            TAString<TChar>::Append( src );
        }

#if ALIB_MODULE_STRINGFORMAT
        /** ****************************************************************************************
         * Constructor that accepts a format string and variadic template arguments collected
         * in an object of type \alib{boxing,Boxes}.<br>
         * The parent's constructor invokes \alib{strings,TAString::FormatArgs,FormatArgs} which
         * performs a format operation according to the given parameters.
         *
         * \note
         *   There is \b no variant of this constructor available that takes a format string and
         *   a variadic template parameter list which creates the \b %Boxes object expected by
         *   this method implicitly. Such constructor would lead to ambiguities with other
         *   overloaded constructors. Therefore, the Boxes object expected here has to be explicitly
         *   created with the call (or before).<br>
         *   A sample invocation of this constructor looks like this:
         *
         *          String256 myString( Boxes("Hello {}: The answer is {}", "world", 6 * 7) );
         *
         * @param  args   The format string and a corresponding list of boxed arguments.
         *
         * \par Module Dependencies
         *    This constructor is only available if module \alibmod_stringformat is included in the
         *    \alibdist.
         ******************************************************************************************/
        inline
        TLocalString( const Boxes& args )
        : TAString<TChar>( args )
        {}

#endif
        /** ****************************************************************************************
         * The <em>default C++ assign operator</em> (<code>ClassT & operator=(const ClassT &)</code>).<br>
         * Copies the contents of the given object \p{copy}.
         *
         * @param  copy  The object to copy the contents from.
         * @return \c *this to allow concatenated calls.
         ******************************************************************************************/
        TLocalString& operator= (const TLocalString&  copy)
        {
            if ( copy.IsNull())
            {
                TAString<TChar>::SetNull();
                return *this;
            }
            TAString<TChar>::Reset();
            return static_cast<TLocalString&>( TAString<TChar>::template Append<false>( copy.Buffer(), copy.Length() ) );
        }

}; // class TLocalString


}} // namespace aworx::[lib::strings]

} // namespace aworx


#endif // HPP_ALIB_STRINGS_LOCALSTRING
