/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_strings of the \aliblong.
 *
 * \emoji :copyright: 2013-2019 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_STRINGS_APPENDABLES
#define HPP_ALIB_STRINGS_APPENDABLES 1

#if !defined(HPP_ALIB_STRINGS_ASTRING)
#   error "ALib sources with ending '.inl' must not be included from outside."
#endif

namespace aworx { namespace lib { namespace strings {

// #################################################################################################
// struct T_Append: fundamental types
// #################################################################################################

// Faking all template specializations of namespace strings for doxygen into namespace
// strings::APPENDABLES to keep the documentation of namespace string clean!
#if defined(ALIB_DOX)
namespace APPENDABLES {
#endif

/** Specialization of functor \alib{strings,T_Append} for type \c bool. */
template<typename TChar> struct       T_Append<bool ,TChar>
{
    /** ********************************************************************************************
     * Writes the words "true" or "false" to the given AString.
     * @param target The \b AString that \b Append was invoked on.
     * @param b      The boolean to write to \p{target}.
     **********************************************************************************************/
    void operator()( TAString<TChar>& target, bool b );
};
extern template ALIB_API void T_Append<bool , nchar>::operator()( TAString<nchar>&, bool  );
extern template ALIB_API void T_Append<bool , wchar>::operator()( TAString<wchar>&, bool  );
extern template ALIB_API void T_Append<bool , xchar>::operator()( TAString<xchar>&, bool  );

/** Specialization of functor \alib{strings,T_Append} for type \c int8_t. */
template<typename TChar> struct T_Append<int8_t ,TChar>
{
    /**
     * Creates a defaulted object of type \alib{strings,TFormat,Format} and defers the number
     * conversion to the corresponding static specialization for that type.
     *
     * @param target The \b AString that \b Append was invoked on.
     * @param value  The value to write.
     */
    void operator()( TAString<TChar>& target,  int8_t value )
    {
        T_Append<int64_t, TChar>()( target, value );
    }
};

/** Specialization of functor \alib{strings,T_Append} for type \c uint8_t. */
template<typename TChar> struct T_Append<uint8_t ,TChar>
{
    /**
     * Creates a defaulted object of type \alib{strings,TFormat,Format} and defers the number
     * conversion to the corresponding static specialization for that type.
     *
     * @param target The \b AString that \b Append was invoked on.
     * @param value  The value to write.
     */
    void operator()( TAString<TChar>& target,  uint8_t value )
    {
        T_Append<uint64_t, TChar>()( target, value );
    }
};

/** Specialization of functor \alib{strings,T_Append} for type \c int16_t. */
template<typename TChar> struct T_Append<int16_t ,TChar>
{
    /**
     * Creates a defaulted object of type \alib{strings,TFormat,Format} and defers the number
     * conversion to the corresponding static specialization for that type.
     *
     * @param target The \b AString that \b Append was invoked on.

     * @param value  The value to write.
     */
    void operator()( TAString<TChar>& target,  int16_t value )
    {
        T_Append<int64_t, TChar>()( target, value );
    }

};

/** Specialization of functor \alib{strings,T_Append} for type \c uint16_t. */
template<typename TChar> struct T_Append<uint16_t ,TChar>
{
    /**
     * Creates a defaulted object of type \alib{strings,TFormat,Format} and defers the number
     * conversion to the corresponding static specialization for that type.
     *
     * @param target The \b AString that \b Append was invoked on.
     * @param value  The value to write.
     */
    void operator()( TAString<TChar>& target,  uint16_t value )
    {
        T_Append<uint64_t, TChar>()( target, value );
    }

};

/** Specialization of functor \alib{strings,T_Append} for type \c int32_t. */
template<typename TChar> struct T_Append<int32_t ,TChar>
{
    /**
     * Creates a defaulted object of type \alib{strings,TFormat,Format} and defers the number
     * conversion to the corresponding static specialization for that type.
     *
     * @param target The \b AString that \b Append was invoked on.
     * @param value  The value to write.
     */
    void operator()( TAString<TChar>& target,  int32_t value )
    {
        T_Append<int64_t, TChar>()( target, value );
    }

};

/** Specialization of functor \alib{strings,T_Append} for type \c uint32_t. */
template<typename TChar> struct T_Append<uint32_t ,TChar>
{
    /**
     * Creates a defaulted object of type \alib{strings,TFormat,Format} and defers the number
     * conversion to the corresponding static specialization for that type.
     *
     * @param target The \b AString that \b Append was invoked on.
     * @param value  The value to write.
     */
    void operator()( TAString<TChar>& target,  uint32_t value )
    {
        T_Append<uint64_t, TChar>()( target, value );
    }

};


/** Specialization of functor \alib{strings,T_Append} for type \c int64_t. */
template<typename TChar> struct T_Append<int64_t ,TChar>
{
    /**
     * Creates a defaulted object of type \alib{strings,TFormat,Format} and defers the number
     * conversion to the corresponding static specialization for that type.
     *
     * @param target The \b AString that \b Append was invoked on.
     * @param value  The value to write.
     */
    void operator()( TAString<TChar>& target,  int64_t value );
};

/** Specialization of functor \alib{strings,T_Append} for type \c uint64_t. */
template<typename TChar> struct T_Append<uint64_t ,TChar>
{
    /**
     * Creates a defaulted object of type \alib{strings,TFormat,Format} and defers the number
     * conversion to the corresponding static specialization for that type.
     *
     * @param target The \b AString that \b Append was invoked on.
     * @param value  The value to write.
     */
    void operator()( TAString<TChar>& target,  uint64_t value );
};

/** Specialization of functor \alib{strings,T_Append} for type \c intGap_t. */
template<typename TChar> struct T_Append<intGap_t ,TChar>
{
    /**
     * Creates a defaulted object of type \alib{strings,TFormat,Format} and defers the number
     * conversion to the corresponding static specialization for that type.
     *
     * @param target The \b AString that \b Append was invoked on.
     * @param value  The value to write.
     */
    void operator()( TAString<TChar>& target,  intGap_t value )
    {
        T_Append<int64_t, TChar>()( target, value );
    }

};

/** Specialization of functor \alib{strings,T_Append} for type \c uintGap_t. */
template<typename TChar> struct T_Append<uintGap_t ,TChar>
{
    /**
     * Creates a defaulted object of type \alib{strings,TFormat,Format} and defers the number
     * conversion to the corresponding static specialization for that type.
     *
     * @param target The \b AString that \b Append was invoked on.
     * @param value  The value to write.
     */
    void operator()( TAString<TChar>& target,  uintGap_t value )
    {
        T_Append<uint64_t, TChar>()( target, value );
    }
};

/** Specialization of functor \alib{strings,T_Append} for type \c float. */
template<typename TChar> struct T_Append<float ,TChar>
{
    /**
     * Creates a defaulted object of type \alib{strings,TFormat,Format} and defers the number
     * conversion to the corresponding static specialization for that type.
     *
     * @param target The \b AString that \b Append was invoked on.
     * @param value  The value to write.
     */
    void operator()( TAString<TChar>& target,  float value)
    {
        T_Append<double, TChar>()( target, static_cast<double>(value) );
    }
};

/** Specialization of functor \alib{strings,T_Append} for type \c double. */
template<typename TChar> struct T_Append<double ,TChar>
{
    /**
     * Creates a defaulted object of type \alib{strings,TFormat,Format} and defers the number
     * conversion to the corresponding static specialization for that type.
     *
     * @param target The \b AString that \b Append was invoked on.
     * @param value  The value to write.
     */
    void operator()( TAString<TChar>& target,  double value);
};

extern template ALIB_API void T_Append<  int64_t, nchar>::operator()( TAString<nchar>&,   int64_t );
extern template ALIB_API void T_Append<  int64_t, wchar>::operator()( TAString<wchar>&,   int64_t );
extern template ALIB_API void T_Append<  int64_t, xchar>::operator()( TAString<xchar>&,   int64_t );
extern template ALIB_API void T_Append< uint64_t, nchar>::operator()( TAString<nchar>&,  uint64_t );
extern template ALIB_API void T_Append< uint64_t, wchar>::operator()( TAString<wchar>&,  uint64_t );
extern template ALIB_API void T_Append< uint64_t, xchar>::operator()( TAString<xchar>&,  uint64_t );
extern template ALIB_API void T_Append<double   , nchar>::operator()( TAString<nchar>&, double    );
extern template ALIB_API void T_Append<double   , wchar>::operator()( TAString<wchar>&, double    );
extern template ALIB_API void T_Append<double   , xchar>::operator()( TAString<xchar>&, double    );




#if ALIB_DEBUG
/** Specialization of functor \alib{strings,T_Append} for type \c std::type_info. */
template<typename TChar> struct       T_Append<std::type_info ,TChar>
{
    /** ********************************************************************************************
     * Writes the demangled type name.
     * @param target The \b AString that \b Append was invoked on.
     * @param type   The type to write to \p{target}.
     **********************************************************************************************/
    void operator()( TAString<TChar>& target, const std::type_info& type );
};
extern template ALIB_API void T_Append<std::type_info, nchar>::operator()( TAString<nchar>&, const std::type_info&  );
extern template ALIB_API void T_Append<std::type_info, wchar>::operator()( TAString<wchar>&, const std::type_info&  );
extern template ALIB_API void T_Append<std::type_info, xchar>::operator()( TAString<xchar>&, const std::type_info&  );

#endif


// Faking all template specializations of namespace strings for doxygen into namespace
// strings::APPENDABLES to keep the documentation of namespace string clean!
#if defined(ALIB_DOX)
}
#endif


}}} // namespace [aworx::lib::strings]

#endif // HPP_ALIB_STRINGS_APPENDABLES
