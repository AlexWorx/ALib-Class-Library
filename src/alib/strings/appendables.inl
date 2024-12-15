
//==================================================================================================
/// \file
/// This header file is part of module \alib_strings of the \aliblong.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_STRINGS_APPENDABLES
#define HPP_ALIB_STRINGS_APPENDABLES 1
#pragma once
#if !defined(HPP_ALIB_STRINGS_TASTRING_INLINING)
#   error "ALib sources with ending '.inl' must not be included from outside."
#endif

namespace alib {  namespace strings {

// #################################################################################################
// struct T_Append: fundamental types
// #################################################################################################

// Faking all template specializations of namespace strings for doxygen into namespace
// strings::APPENDABLES to keep the documentation of namespace string clean!
#if DOXYGEN
namespace APPENDABLES {
#endif

/// Specialization of functor \alib{strings;T_Append} for type \c bool.
template<typename TChar, typename TAllocator> struct       T_Append<bool ,TChar,TAllocator>
{
    //==============================================================================================
    /// Writes the words "true" or "false" to the given AString.
    /// @param target The \b AString that \b Append was invoked on.
    /// @param b      The boolean to write to \p{target}.
    //==============================================================================================
    void operator()( TAString<TChar,TAllocator>& target, bool b )
    {
        target.template _<NC>( b ? "true" : "false" );
    }
};

/// Specialization of functor \alib{strings;T_Append} for type \c int8_t.
template<typename TChar, typename TAllocator> struct T_Append<int8_t ,TChar,TAllocator>
{
    /// Creates a defaulted object of type \alib{strings;TFormat;Format} and defers the number
    /// conversion to the corresponding static specialization for that type.
    ///
    /// @param target The \b AString that \b Append was invoked on.
    /// @param value  The value to write.
    void operator()( TAString<TChar,TAllocator>& target,  int8_t value )
    {
        T_Append<int64_t, TChar,TAllocator>()( target, value );
    }
};

/// Specialization of functor \alib{strings;T_Append} for type \c uint8_t.
template<typename TChar, typename TAllocator> struct T_Append<uint8_t ,TChar,TAllocator>
{
    /// Creates a defaulted object of type \alib{strings;TFormat;Format} and defers the number
    /// conversion to the corresponding static specialization for that type.
    ///
    /// @param target The \b AString that \b Append was invoked on.
    /// @param value  The value to write.
    void operator()( TAString<TChar,TAllocator>& target,  uint8_t value )
    {
        T_Append<uint64_t, TChar,TAllocator>()( target, value );
    }
};

/// Specialization of functor \alib{strings;T_Append} for type \c int16_t.
template<typename TChar, typename TAllocator> struct T_Append<int16_t ,TChar,TAllocator>
{
    /// Creates a defaulted object of type \alib{strings;TFormat;Format} and defers the number
    /// conversion to the corresponding static specialization for that type.
    ///
    /// @param target The \b AString that \b Append was invoked on.
    /// @param value  The value to write.
    void operator()( TAString<TChar,TAllocator>& target,  int16_t value )
    {
        T_Append<int64_t, TChar,TAllocator>()( target, value );
    }

};

/// Specialization of functor \alib{strings;T_Append} for type \c uint16_t.
template<typename TChar, typename TAllocator> struct T_Append<uint16_t ,TChar,TAllocator>
{
    /// Creates a defaulted object of type \alib{strings;TFormat;Format} and defers the number
    /// conversion to the corresponding static specialization for that type.
    ///
    /// @param target The \b AString that \b Append was invoked on.
    /// @param value  The value to write.
    void operator()( TAString<TChar,TAllocator>& target,  uint16_t value )
    {
        T_Append<uint64_t, TChar,TAllocator>()( target, value );
    }

};

/// Specialization of functor \alib{strings;T_Append} for type \c int32_t.
template<typename TChar, typename TAllocator> struct T_Append<int32_t ,TChar,TAllocator>
{
    /// Creates a defaulted object of type \alib{strings;TFormat;Format} and defers the number
    /// conversion to the corresponding static specialization for that type.
    ///
    /// @param target The \b AString that \b Append was invoked on.
    /// @param value  The value to write.
    void operator()( TAString<TChar,TAllocator>& target,  int32_t value )
    {
        T_Append<int64_t, TChar,TAllocator>()( target, value );
    }

};

/// Specialization of functor \alib{strings;T_Append} for type \c uint32_t.
template<typename TChar, typename TAllocator> struct T_Append<uint32_t ,TChar,TAllocator>
{
    /// Creates a defaulted object of type \alib{strings;TFormat;Format} and defers the number
    /// conversion to the corresponding static specialization for that type.
    ///
    /// @param target The \b AString that \b Append was invoked on.
    /// @param value  The value to write.
    void operator()( TAString<TChar,TAllocator>& target,  uint32_t value )
    {
        T_Append<uint64_t, TChar,TAllocator>()( target, value );
    }

};


/// Specialization of functor \alib{strings;T_Append} for type \c int64_t.
template<typename TChar, typename TAllocator> struct T_Append<int64_t ,TChar,TAllocator>
{
    /// Creates a defaulted object of type \alib{strings;TFormat;Format} and defers the number
    /// conversion to the corresponding static specialization for that type.
    ///
    /// @param target The \b AString that \b Append was invoked on.
    /// @param value  The value to write.
    void operator()( TAString<TChar,TAllocator>& target,  int64_t value );
};

/// Specialization of functor \alib{strings;T_Append} for type \c uint64_t.
template<typename TChar,  typename TAllocator> struct T_Append<uint64_t ,TChar,TAllocator>
{
    /// Creates a defaulted object of type \alib{strings;TFormat;Format} and defers the number
    /// conversion to the corresponding static specialization for that type.
    ///
    /// @param target The \b AString that \b Append was invoked on.
    /// @param value  The value to write.
    void operator()( TAString<TChar,TAllocator>& target,  uint64_t value );
};

/// Specialization of functor \alib{strings;T_Append} for type \c intGap_t.
template<typename TChar,  typename TAllocator> struct T_Append<intGap_t ,TChar,TAllocator>
{
    /// Creates a defaulted object of type \alib{strings;TFormat;Format} and defers the number
    /// conversion to the corresponding static specialization for that type.
    ///
    /// @param target The \b AString that \b Append was invoked on.
    /// @param value  The value to write.
    void operator()( TAString<TChar,TAllocator>& target,  intGap_t value )
    {
        T_Append<int64_t, TChar,TAllocator>()( target, value );
    }

};

/// Specialization of functor \alib{strings;T_Append} for type \c uintGap_t.
template<typename TChar,  typename TAllocator> struct T_Append<uintGap_t ,TChar,TAllocator>
{
    /// Creates a defaulted object of type \alib{strings;TFormat;Format} and defers the number
    /// conversion to the corresponding static specialization for that type.
    ///
    /// @param target The \b AString that \b Append was invoked on.
    /// @param value  The value to write.
    void operator()( TAString<TChar,TAllocator>& target,  uintGap_t value )
    {
        T_Append<uint64_t, TChar,TAllocator>()( target, value );
    }
};

/// Specialization of functor \alib{strings;T_Append} for type \c float.
template<typename TChar,  typename TAllocator> struct T_Append<float ,TChar,TAllocator>
{
    /// Creates a defaulted object of type \alib{strings;TFormat;Format} and defers the number
    /// conversion to the corresponding static specialization for that type.
    ///
    /// @param target The \b AString that \b Append was invoked on.
    /// @param value  The value to write.
    void operator()( TAString<TChar,TAllocator>& target,  float value)
    {
        T_Append<double, TChar,TAllocator>()( target, static_cast<double>(value) );
    }
};

/// Specialization of functor \alib{strings;T_Append} for type \c double.
template<typename TChar,  typename TAllocator> struct T_Append<double ,TChar,TAllocator>
{
    /// Creates a defaulted object of type \alib{strings;TFormat;Format} and defers the number
    /// conversion to the corresponding static specialization for that type.
    ///
    /// @param target The \b AString that \b Append was invoked on.
    /// @param value  The value to write.
    void operator()( TAString<TChar,TAllocator>& target,  double value);
};

/// Specialization of functor \alib{strings;T_Append} for type <c>long double</c>.
template<typename TChar,  typename TAllocator> struct T_Append<long double ,TChar,TAllocator>
{
    /// Casts the value "down" to just \c double and creates a defaulted object of
    /// type \alib{strings;TFormat;Format} and defers the number
    /// conversion to the corresponding static specialization for type \c double.
    ///
    /// @param target The \b AString that \b Append was invoked on.
    /// @param value  The value to write.
    void operator()( TAString<TChar,TAllocator>& target,  long double value)
    {
        T_Append<double, TChar,TAllocator>()( target, static_cast<double>(value) );
    }
};


#if ALIB_DEBUG
/// Specialization of functor \alib{strings;T_Append} for type \c std::type_info.<br>
/// Available only in debug-compilations.
template<typename TChar,  typename TAllocator> struct     T_Append<std::type_info ,TChar,TAllocator>
{
    //==============================================================================================
    /// Writes the demangled type name.
    /// @param target The \b AString that \b Append was invoked on.
    /// @param type   The type to write to \p{target}.
    //==============================================================================================
    void operator()( TAString<TChar,TAllocator>& target, const std::type_info& type );
};
#endif

#if ALIB_EXT_LIB_THREADS_AVAILABLE
/// Specialization of functor \alib{strings;T_Append} for type \c std::thread::id.<br>
template<typename TChar,  typename TAllocator> struct    T_Append<std::thread::id ,TChar,TAllocator>
{
    //==============================================================================================
    /// Writes the thread information.
    /// @param target The \b AString that \b Append was invoked on.
    /// @param type   The thread ID to write to \p{target}.
    //==============================================================================================
    void operator()( TAString<TChar,TAllocator>& target, const std::thread::id& type );
};
#endif

/// Specialization of functor \alib{strings;T_Append} for type \alib{lang;CallerInfo}.<br>
/// This functor writes all information available in a standard format.
/// A more flexible way is provided with formatter type \alib{lang::format;FMTCallerInfo} that
/// defines a format string to pick certain fields of the \b CallerInfo.
/// This format specification is also availble in placeholders of type
/// \alib{lang::format;FormatterPythonStyle}.
template<typename TChar,  typename TAllocator> struct   T_Append<lang::CallerInfo ,TChar,TAllocator>
{
    //==============================================================================================
    /// Writes thread and source location.
    /// @param target The \b AString that \b Append was invoked on.
    /// @param ci     The caller information to write to \p{target}.
    //==============================================================================================
    void operator()( TAString<TChar,TAllocator>& target, const lang::CallerInfo& ci );
};



// Faking all template specializations of namespace strings for doxygen into namespace
// strings::APPENDABLES to keep the documentation of namespace string clean!
#if DOXYGEN
}
#endif


}} // namespace [alib::strings]

#endif // HPP_ALIB_STRINGS_APPENDABLES

