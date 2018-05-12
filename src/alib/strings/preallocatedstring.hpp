// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################

// needed for Doxygen include of the using statements at the end of the file
///@file

// include guard
#ifndef HPP_ALIB_STRINGS_PREALLOCATEDSTRING
#define HPP_ALIB_STRINGS_PREALLOCATEDSTRING 1

// to preserve the right order, we are not includable directly from outside.
#if !defined(ALIB_PROPER_INCLUSION)
    #error "include 'alib/alib.hpp' instead of this header"
#endif

// #################################################################################################
// forward declarations
// #################################################################################################
namespace aworx { namespace lib { namespace strings {

/** ************************************************************************************************
 * A simple class template specializing \alib{strings,AStringBase,AString}. It provides an internal
 * character array member of size \p{TCapacity} as the default "external" buffer to the \b %AString.
 *
 * If an instance of this class* is allocated on the stack, allocation and destruction is performed
 * in \e O(1).
 * If an instance of this class is allocated on the heap (using new), then still some performance
 * improvements apply, because besides the class object itself, no second memory allocation for
 * the string buffer is performed.
 *
 * If during the use of an instance of this class a larger buffer is needed, such buffer will get
 * allocated from the heap. This means, the use of this class is safe and no restrictions apply.
 * Of-course, for performance critical code sections, the predefined size should be chosen large
 * enough to allow the internal buffer to survive the use.<br>
 *
 * In debug compilations of \alib, parent class #AString optionally raises an one-time warning
 * (using
 * \ref aworx::lib::lang::Report::DoReport "Report::DoReport") if an external buffer is replaced by a
 * new (heap) allocation. (From an \b %AString perspective, this classes' internal buffer is an
 * external one). For more information,  see
 * \alib{strings,AStringBase::SetBuffer,AString::SetBuffer}.<p>
 *
 * \note For commonly used sizes, some convenient typedefs exists in namespace aworx.
 *       Type definitions are:
 *       - #String16, #String32 ... #String1K, #String2K, #String4K,
 *       - #NString16, #NString32 ... #NString1K, #NString2K, #NString4K, and
 *       - #WString16, #WString32 ... #WString1K, #WString2K, #WString4K.
 *
 * @tparam TCapacity The capacity of the buffer allocated inside the class with member #preAllocBuf.
 **************************************************************************************************/
template <typename TChar, integer TCapacity>
class PreallocatedStringBase : public AStringBase<TChar>
{
    // #############################################################################################
    //  protected fields
    // #############################################################################################
    protected:
        /// The internal buffer with size specified by the template parameter \p{TCapacity}.
        TChar         preAllocBuf[TCapacity];

    // #############################################################################################
    //  Constructors/Destructor
    // #############################################################################################
    public:
        /** ****************************************************************************************
         * Constructs an empty \b %PreallocatedString with an internal buffer of size \p{TCapacity}.
         ******************************************************************************************/
        constexpr
        PreallocatedStringBase()
        : AStringBase<TChar>( preAllocBuf, TCapacity )
        #if defined(_MSC_VER)
            , preAllocBuf {}  // MSC 2015 wants that, but generates no code with "" as init value
        #else
            , preAllocBuf {}
        #endif
        {}

        /** ****************************************************************************************
         * Copy constructor.
         * Constructs an \b PreallocatedString<TCapacity> and invokes
         * \alib{strings,AStringBase::Apply,AString::Apply}
         * to copy the contents of parameter \p{copy}.
         * @param copy The object to copy the contents from.
         ******************************************************************************************/
        PreallocatedStringBase(const PreallocatedStringBase& copy)
        : AStringBase<TChar>( preAllocBuf, TCapacity )
        {
            AStringBase<TChar>::Apply( copy );
        }

        /** ****************************************************************************************
         * Constructs the object and uses parents' method \e %Apply
         * to append objects of arbitrary
         * type.
         * See \alib{strings,AStringBase::Apply,AString::Apply} for more information
         * about which types are supported and how external, user defined types can be made
         * compatible to this implicit constructor.
         *
         * @param src  The source to take the buffer and length from of template type T.
         ******************************************************************************************/
        template <class T>
        PreallocatedStringBase (const  T& src )
        : AStringBase<TChar>( preAllocBuf, TCapacity )
        {
            AStringBase<TChar>::Apply( src );
        }

        /** ****************************************************************************************
         * Constructor copying a region of a \alib{strings,StringBase,String}.
         * @param src          The reference to the \b %String to copy from.
         * @param regionStart  The start of the region in \p{src} to append.
         * @param regionLength The length of the region in \p{src} to append.
         *                     Defaults to \b MaxLen.
         ******************************************************************************************/
        PreallocatedStringBase( const StringBase<TChar>& src, integer regionStart,
                                integer regionLength= MaxLen )
        : AStringBase<TChar>( preAllocBuf, TCapacity )
        {
            if ( src.IsNotNull() )
                AStringBase<TChar>::_( src, regionStart, regionLength );
        }


    // #################################################################################################
    //  Inherit operator=
    // #################################################################################################

        /** ****************************************************************************************
         * The <em>default C++ assign operator</em> (<code>ClassT & operator=(const ClassT &)</code>).<br>
         * Copies the contents of the given object \p{copy}.
         * Other assignment operators are inherited (with <em>using</em> statement) from
         * parent \b %AString.
         *
         * @param  copy  The object to copy the contents from.
         * @return \c *this to allow concatenated calls.
         ******************************************************************************************/
        PreallocatedStringBase& operator= (const PreallocatedStringBase&  copy)
        {
            if ( copy.IsNull())
            {
                AStringBase<TChar>::SetNull();
                return *this;
            }
            AStringBase<TChar>::Clear();
            return static_cast<PreallocatedStringBase&>( AStringBase<TChar>::template Append<false>( copy.Buffer(), copy.Length() ) );
        }

        // Due to a doxygen bug in 1.8.14, we must not tell doxygen that we import overloaded methods.
        //! @cond NO_DOX
        using AStringBase<TChar>::operator=;
        //! @endcond


}; // class PreallocatedStringBase

// #################################################################################################
// Namespace methods
// #################################################################################################
    /** ********************************************************************************************
     * Partial specialization of struct <em>T_IsTerminatable</em> which is designed to test objects
     * of arbitrary type if it is allowed in the case that they are not terminated,
     * to write termination character '\0' into their character buffer at the first position after
     * their last character.
     *
     * This Specialization inherits from \c std::true_type for all sizes of type
     * PreallocatedStringBase<TCapacity> as they always reserve one character in their buffer's
     * capacity.
     **********************************************************************************************/
    template<typename TChar, const integer TCapacity>
    struct  T_IsTerminatable<PreallocatedStringBase<TChar, TCapacity>>   : public std::true_type {};

}} // namespace aworx::[lib::strings]



} // namespace aworx

#endif // HPP_ALIB_STRINGS_PREALLOCATEDSTRING
