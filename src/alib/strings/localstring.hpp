/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_strings of the \aliblong.
 *
 * \emoji :copyright: 2013-2023 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
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
 * With debug builds of \alib, parent class #AString optionally and by default raises a
 * \ref aworx::lib::results::Report::DoReport "warning" if an external buffer is replaced by a
 * new (heap) allocation. (Note that from an \b %AString perspective, this class's internal
 * field \b buffer is an external character array). With use cases that do not allow to foresee a
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
 * @tparam TCapacity The capacity of the string buffer allocated locally "inside" the class.
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
        TChar         localBuffer[TCapacity];

    // #############################################################################################
    //  Constructors/Destructor
    // #############################################################################################
    public:
        /** ****************************************************************************************
         * Constructs an empty \b %LocalString. Inherited field \alib{strings::TString,buffer}
         * will be set as an the external buffer of parent \b AString.
         * Unlike all other \alib string classes, objects of this type are not \e nulled
         * after default construction.
         ******************************************************************************************/
        constexpr
        TLocalString()
        : TAString<TChar>( localBuffer, TCapacity )
        , localBuffer {}
        {}

        /** ****************************************************************************************
         * Copy constructor. Copies the string data of parameter \p{copy} to this instance
         * @param copy The object to copy the contents from.
         ******************************************************************************************/
        TLocalString(const TLocalString& copy)
        : TAString<TChar>( localBuffer, TCapacity )
        {
            ALIB_DBG( TAString<TChar>::dbgWarnWhenExternalBufferIsReplaced= copy.dbgWarnWhenExternalBufferIsReplaced; )
            TAString<TChar>::Append( copy.Buffer(), copy.Length() );
        }

        /** ****************************************************************************************
         * Move constructor.
         * See \ref alib_ns_strings_astring_copymove "Copy/Move Constructor and Assignment"
         * for details.
         * @param move The object to move.
         ******************************************************************************************/
        TLocalString(TLocalString&& move)                                                   noexcept
        : TAString<TChar>( localBuffer, TCapacity )
        {
            // given move object has external buffer: we have to copy
            if ( !move.HasInternalBuffer() )
            {
                ALIB_DBG( TAString<TChar>::dbgWarnWhenExternalBufferIsReplaced= move.dbgWarnWhenExternalBufferIsReplaced; )
                TAString<TChar>::Append( move );
                return;
            }

            // copy values
             TString<TChar>::buffer=         move.buffer;
             TString<TChar>::length=         move.length;
            TAString<TChar>::capacity=       move.capacity;

            // clean moved object (buffer does not need to be nulled as AString destructor
            // checks capacity > 0  )
            move.capacity= 0;

            // in debug mode, more copying and more destructor prevention is needed
            #if ALIB_DEBUG
                TAString<TChar>::dbgWarnWhenExternalBufferIsReplaced= move.dbgWarnWhenExternalBufferIsReplaced;
                #if ALIB_DEBUG_STRINGS
                TAString<TChar>::debugLastAllocRequest  = move.debugLastAllocRequest;
                            move.buffer                 = nullptr;
                            move.length                 = 0;
                #endif
            #endif
        }

        /** ****************************************************************************************
         * Copy assign operator.
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

        /** ****************************************************************************************
         * Move assign operator.
         * Copies the contents of the given object \p{copy}.
         *
         * @param  move  The object to move the contents from.
         * @return \c *this to allow concatenated calls.
         ******************************************************************************************/
        TLocalString& operator= (TLocalString&&  move)
        {
            if( move.IsNull() )
            {
                ALIB_DBG( TAString<TChar>::dbgWarnWhenExternalBufferIsReplaced= move.dbgWarnWhenExternalBufferIsReplaced; )
                TAString<TChar>::SetNull();
                return *this;
            }

            // copy if move has its local buffer or this has lost its local buffer
            if ( !move.HasInternalBuffer() || TAString<TChar>::HasInternalBuffer() )
            {
                TAString<TChar>::Reset( move );
                return *this;
            }

            // copy other buffer from moved object
             TString<TChar>::buffer=         move.buffer;
             TString<TChar>::length=         move.length;
            TAString<TChar>::capacity=       move.capacity;

            // clean moved object (buffer does not need to be nulled)
            move.capacity= 0;

            // in debug mode, more copying and more destructor prevention is needed
            #if ALIB_DEBUG
                TAString<TChar>::dbgWarnWhenExternalBufferIsReplaced= move.dbgWarnWhenExternalBufferIsReplaced;
                #if ALIB_DEBUG_STRINGS
                TAString<TChar>::debugLastAllocRequest  = move.debugLastAllocRequest;
                            move.buffer                 = nullptr;
                            move.length                 = 0;
                #endif
            #endif
            return *this;
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
        : TAString<TChar>( localBuffer, TCapacity )
        {
            TAString<TChar>::Append( src );
        }
}; // class TLocalString


#if ALIB_DEBUG
/** ************************************************************************************************
 * This simple specialization of \alib{strings,TLocalString} disables the warning about
 * replacements of the internal buffer in debug-compilations. This may be used in situations,
 * where it is not possible to disable this warning after construction, for example if a local
 * string is <em>emplaced</em> in a container and extensions of it's local capacity are well
 * accepted (for a minority of the emplaced strings).
 *
 * In release compilations, this type does not exist, but is replaced by a simple using
 * statement.
 *
 * @tparam TChar     The character type.
 * @tparam TCapacity The capacity of local, embedded string buffer.
 **************************************************************************************************/
template <typename TChar, integer TCapacity>
struct  TLocalStringNoWarning : public TLocalString<TChar,TCapacity>
{
    /**
     * Default constructor.
     */
    ALIB_CPP14_CONSTEXPR
    TLocalStringNoWarning()
    : TLocalString<TChar,TCapacity>()
    {
        TAString<TChar>::dbgWarnWhenExternalBufferIsReplaced= false;
    }

    /**
     * Constructor taking a string to copy.
     * @param src  The string to copy into this object.
     */
    TLocalStringNoWarning( const String& src )
    : TLocalString<TChar,TCapacity>()
    {
        TAString<TChar>::dbgWarnWhenExternalBufferIsReplaced= false;
        TAString<TChar>::Append( src.Buffer(), src.Length() );
    }
};
#else
    template <typename TChar, integer TCapacity>
    using   TLocalStringNoWarning = TLocalString<TChar, TCapacity>;
#endif



}} // namespace aworx::[lib::strings]

} // namespace aworx


#endif // HPP_ALIB_STRINGS_LOCALSTRING
