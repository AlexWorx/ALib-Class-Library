/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_monomem of the \aliblong.
 *
 * \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_MONOMEM_MASTRING
#define HPP_ALIB_MONOMEM_MASTRING

#if !defined (HPP_ALIB_MONOMEM_MONOALLOCATOR)
#   include "alib/monomem/monoallocator.hpp"
#endif

#if ALIB_STRINGS

#if !defined (HPP_ALIB_STRINGS_LOCALSTRING)
#   include "alib/strings/localstring.hpp"
#endif


namespace alib {  namespace monomem {

/** ************************************************************************************************
 * The name of this class stands for "monotonically allocated string".
 * Consequently, it publicly derives from \alib{strings,TAString,AString} and provides some
 * special constructors that perform an initial allocation using a given
 * \alib{monomem,MonoAllocator}. The constructors allow to specify an (additional)
 * capacity to allocate, besides what is needed when copying the construction parameter.
 *
 * If the string exceeds this initial capacity, dynamic memory is allocated, just likewise
 * class \alib{strings,TLocalString,LocalString} does, in the case its stack-memory is exceeded.
 *
 * If it is assured that no dynamic allocation was performed during the life-cycle of an instance,
 * the instance does not need to be destructed. In this case, the instance itself might be allocated
 * in the same \alib{monomem,MonoAllocator} (without performing additional precautions for proper
 * destruction).
 *
 * @tparam TChar The character type.<br>
 *   Alias names for specializations of this class using character types
 *   \alib{characters,character}, \alib{characters,nchar}, \alib{characters,wchar},
 *   \alib{characters,xchar}, \alib{characters,complementChar} and \alib{characters,strangeChar}
 *   are provided in namespace #alib with type definitions \alib{MAString}, \alib{NMAString},
 *   \alib{WMAString}, \alib{XMAString}, \alib{ComplementMAString} and \alib{StrangeMAString}.
 **************************************************************************************************/
template<typename TChar>
class TMAString : public strings::TAString<TChar>
{
    public:

    /**
     * Allocates a scope string of given capacity.
     * @param allocator  The monotonic allocator to take the initial memory from.
     * @param pCapacity  The capacity of the of the string.
     */
    TMAString( MonoAllocator& allocator, integer pCapacity )
    : strings::TAString<TChar>( allocator.AllocArray<TChar>( static_cast<size_t>(pCapacity + 1) ) ,
                                                                                 pCapacity + 1 )
    {}

    /**
     * Allocates a scope string of given capacity plus the length of the string \p{src} and copies
     * \p{source} to it.
     * @param allocator          The monotonic allocator to take the initial memory from.
     * @param src                The source string to copy.
     * @param additionalCapacity The extra capacity to allocate.
     */
    TMAString( MonoAllocator& allocator, const strings::TString<TChar>& src, integer additionalCapacity )
    : strings::TAString<TChar>( allocator.AllocArray<TChar>( src.Length() + additionalCapacity + 1),
                                                             src.Length() + additionalCapacity + 1 )
    {
        strings::TString<TChar>::length= src.CopyTo( strings::TString<TChar>::vbuffer );
    }

    /**
     * Returns a string with contents of the given \p{initialValue} written at the start.
     * and the given additional capacity.<br>
     * Internally, a \alib{strings,TLocalString,LocalString<1024>} is used to first convert
     * \p{initialValue} to a string, which is then copied to this object.
     * @tparam TInitialValue     The type of the initial object to write into the string.
     * @param allocator          The monotonic allocator to take the initial memory from.
     * @param initialValue       A boxed object to write to the start of the string.
     * @param additionalCapacity Additional capacity to allocate in \p{scope}.
     */
    template<typename TInitialValue>
    TMAString( MonoAllocator& allocator, const TInitialValue& initialValue, integer additionalCapacity)
    : strings::TAString<TChar>()
    {
        strings::TLocalString<TChar, 1024> tmp;
        tmp.DbgDisableBufferReplacementWarning();
        tmp << initialValue;

        strings::TAString<TChar>::SetBuffer( allocator.AllocArray<TChar>( tmp.Length() + additionalCapacity + 1 ),
                                             tmp.Length() + additionalCapacity + 1  ,
                                             tmp.Length(), lang::Responsibility::KeepWithSender );

        tmp.CopyTo( strings::TString<TChar>::vbuffer );
    }
};


} // namespace alib[::monomem]

/// Type alias in namespace \b alib.
using  MAString          =     monomem::TMAString<character>;

/// Type alias in namespace \b alib.
using  ComplementMAString=     monomem::TMAString<complementChar>;

/// Type alias in namespace \b alib.
using  StrangeMAString   =     monomem::TMAString<strangeChar>;

/// Type alias in namespace \b alib.
using  NMAString         =     monomem::TMAString<nchar>;

/// Type alias in namespace \b alib.
using  WMAString         =     monomem::TMAString<wchar>;

/// Type alias in namespace \b alib.
using  XMAString         =     monomem::TMAString<xchar>;

} // namespace [alib]


// set character array type traits for TMAString
#if !defined(ALIB_DOX)

namespace alib::characters {
template<typename TChar>
struct  T_CharArray<monomem::TMAString<TChar>, TChar>
{
    static constexpr AccessType       Access       = AccessType::Implicit;
    static constexpr const TChar*     Buffer(const monomem::TMAString<TChar>& src)  { return src.Buffer(); }
    static constexpr integer          Length(const monomem::TMAString<TChar>& src)  { return src.Length(); }

    static constexpr ConstructionType Construction = ConstructionType::NONE;
};
} // namespace [alib::characters]

#endif // !defined(ALIB_DOX)


#endif // ALIB_STRINGS
#endif // HPP_ALIB_MONOMEM_MASTRING
