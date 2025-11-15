
//==================================================================================================
/// \file
/// This header-file is part of module \alib_strings of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib {  namespace strings {

//##################################################################################################
// struct AppendableTraits: fundamental types
//##################################################################################################

// Faking all template specializations of namespace strings for doxygen into namespace
// strings::APPENDABLES to keep the documentation of namespace string clean!
#if DOXYGEN
namespace APPENDABLES {
#endif

/// Specialization of functor \alib{strings;AppendableTraits} for type \c bool.
template<typename TChar, typename TAllocator> struct       AppendableTraits<bool ,TChar,TAllocator>
{
    /// Writes the words "true" or "false" to the given AString.
    /// @param target The \b AString that \b Append was invoked on.
    /// @param b      The boolean to write to \p{target}.
    void operator()( TAString<TChar,TAllocator>& target, bool b )
    { target.template _<NC>( b ? "true" : "false" ); }
};

/// Specialization of functor \alib{strings;AppendableTraits} for type \c int8_t.
template<typename TChar, typename TAllocator> struct AppendableTraits<int8_t ,TChar,TAllocator>
{
    /// Uses function \alib{strings;detail::WriteDecSigned} with
    /// \alib{strings;TNumberFormat<TChar>::Computational} to write the number.
    /// @param target The \b AString that \b Append was invoked on.
    /// @param value  The value to write.
    void operator()( TAString<TChar,TAllocator>& target,  int8_t value )
    { AppendableTraits<int64_t, TChar,TAllocator>()( target, value ); }
};

/// Specialization of functor \alib{strings;AppendableTraits} for type \c uint8_t.
template<typename TChar, typename TAllocator> struct AppendableTraits<uint8_t ,TChar,TAllocator>
{
    /// Uses function \alib{strings;detail::WriteDecUnsigned} with
    /// \alib{strings;TNumberFormat<TChar>::Computational} to write the number.
    /// @param target The \b AString that \b Append was invoked on.
    /// @param value  The value to write.
    void operator()( TAString<TChar,TAllocator>& target,  uint8_t value )
    { AppendableTraits<uint64_t, TChar,TAllocator>()( target, value ); }
};

/// Specialization of functor \alib{strings;AppendableTraits} for type \c int16_t.
template<typename TChar, typename TAllocator> struct AppendableTraits<int16_t ,TChar,TAllocator>
{
    /// Uses function \alib{strings;detail::WriteDecSigned} with
    /// \alib{strings;TNumberFormat<TChar>::Computational} to write the number.
    /// @param target The \b AString that \b Append was invoked on.
    /// @param value  The value to write.
    void operator()( TAString<TChar,TAllocator>& target,  int16_t value )
    { AppendableTraits<int64_t, TChar,TAllocator>()( target, value ); }

};

/// Specialization of functor \alib{strings;AppendableTraits} for type \c uint16_t.
template<typename TChar, typename TAllocator> struct AppendableTraits<uint16_t ,TChar,TAllocator>
{
    /// Uses function \alib{strings;detail::WriteDecUnsigned} with
    /// \alib{strings;TNumberFormat<TChar>::Computational} to write the number.
    /// @param target The \b AString that \b Append was invoked on.
    /// @param value  The value to write.
    void operator()( TAString<TChar,TAllocator>& target,  uint16_t value )
    { AppendableTraits<uint64_t, TChar,TAllocator>()( target, value ); }

};

/// Specialization of functor \alib{strings;AppendableTraits} for type \c int32_t.
template<typename TChar, typename TAllocator> struct AppendableTraits<int32_t ,TChar,TAllocator>
{
    /// Uses function \alib{strings;detail::WriteDecSigned} with
    /// \alib{strings;TNumberFormat<TChar>::Computational} to write the number.
    /// @param target The \b AString that \b Append was invoked on.
    /// @param value  The value to write.
    void operator()( TAString<TChar,TAllocator>& target,  int32_t value )
    { AppendableTraits<int64_t, TChar,TAllocator>()( target, value ); }

};

/// Specialization of functor \alib{strings;AppendableTraits} for type \c uint32_t.
template<typename TChar, typename TAllocator> struct AppendableTraits<uint32_t ,TChar,TAllocator>
{
    /// Uses function \alib{strings;detail::WriteDecUnsigned} with
    /// \alib{strings;TNumberFormat<TChar>::Computational} to write the number.
    /// @param target The \b AString that \b Append was invoked on.
    /// @param value  The value to write.
    void operator()( TAString<TChar,TAllocator>& target,  uint32_t value )
    { AppendableTraits<uint64_t, TChar,TAllocator>()( target, value ); }

};


/// Specialization of functor \alib{strings;AppendableTraits} for type \c int64_t.
template<typename TChar, typename TAllocator> struct AppendableTraits<int64_t ,TChar,TAllocator>
{
    /// Uses function \alib{strings;detail::WriteDecSigned} with
    /// \alib{strings;TNumberFormat<TChar>::Computational} to write the number.
    /// @param target The \b AString that \b Append was invoked on.
    /// @param value  The value to write.
    void operator()( TAString<TChar,TAllocator>& target,  int64_t value );
};

/// Specialization of functor \alib{strings;AppendableTraits} for type \c uint64_t.
template<typename TChar,  typename TAllocator> struct AppendableTraits<uint64_t ,TChar,TAllocator>
{
    /// Uses function \alib{strings;detail::WriteDecUnsigned} with
    /// \alib{strings;TNumberFormat<TChar>::Computational} to write the number.
    /// @param target The \b AString that \b Append was invoked on.
    /// @param value  The value to write.
    void operator()( TAString<TChar,TAllocator>& target,  uint64_t value );
};

/// Specialization of functor \alib{strings;AppendableTraits} for type \c intGap_t.
template<typename TChar,  typename TAllocator> struct AppendableTraits<intGap_t ,TChar,TAllocator>
{
    /// Uses function \alib{strings;detail::WriteDecSigned} with
    /// \alib{strings;TNumberFormat<TChar>::Computational} to write the number.
    /// @param target The \b AString that \b Append was invoked on.
    /// @param value  The value to write.
    void operator()( TAString<TChar,TAllocator>& target,  intGap_t value )
    { AppendableTraits<int64_t, TChar,TAllocator>()( target, value ); }
};

/// Specialization of functor \alib{strings;AppendableTraits} for type \c uintGap_t.
template<typename TChar,  typename TAllocator> struct AppendableTraits<uintGap_t ,TChar,TAllocator>
{
    /// Uses function \alib{strings;detail::WriteDecUnsigned} with
    /// \alib{strings;TNumberFormat<TChar>::Computational} to write the number.
    /// @param target The \b AString that \b Append was invoked on.
    /// @param value  The value to write.
    void operator()( TAString<TChar,TAllocator>& target,  uintGap_t value )
    { AppendableTraits<uint64_t, TChar,TAllocator>()( target, value ); }
};

/// Specialization of functor \alib{strings;AppendableTraits} for type \c float.
template<typename TChar,  typename TAllocator> struct AppendableTraits<float ,TChar,TAllocator>
{
    /// Uses function \alib{strings;detail::WriteFloat} with
    /// \alib{strings;TNumberFormat<TChar>::Computational} to write the number.
    /// @param target The \b AString that \b Append was invoked on.
    /// @param value  The value to write.
    void operator()( TAString<TChar,TAllocator>& target,  float value)
    { AppendableTraits<double, TChar,TAllocator>()( target, double(value) ); }
};

/// Specialization of functor \alib{strings;AppendableTraits} for type \c double.
template<typename TChar,  typename TAllocator> struct AppendableTraits<double ,TChar,TAllocator>
{
    /// Uses function \alib{strings;detail::WriteFloat} with
    /// \alib{strings;TNumberFormat<TChar>::Computational} to write the number.
    /// @param target The \b AString that \b Append was invoked on.
    /// @param value  The value to write.
    void operator()( TAString<TChar,TAllocator>& target,  double value);
};

/// Specialization of functor \alib{strings;AppendableTraits} for type <c>long double</c>.
template<typename TChar,  typename TAllocator> struct AppendableTraits<long double ,TChar,TAllocator>
{
    /// Casts the value "down" to just \c double and uses function \alib{strings;detail::WriteFloat}
    /// with \alib{strings;TNumberFormat<TChar>::Computational} to write the number.
    ///
    /// @param target The \b AString that \b Append was invoked on.
    /// @param value  The value to write.
    void operator()( TAString<TChar,TAllocator>& target,  long double value)
    { AppendableTraits<double, TChar,TAllocator>()( target, double(value) ); }
};


#if ALIB_DEBUG
/// Specialization of functor \alib{strings;AppendableTraits} for type \c std::type_info.<br>
/// Available only in debug-compilations.
template<typename TChar,  typename TAllocator> struct     AppendableTraits<std::type_info ,TChar,TAllocator>
{
    /// Writes the demangled type name.
    /// @param target The \b AString that \b Append was invoked on.
    /// @param type   The type to write to \p{target}.
    void operator()( TAString<TChar,TAllocator>& target, const std::type_info& type );
};
#endif

#if ALIB_EXT_LIB_THREADS_AVAILABLE
/// Specialization of functor \alib{strings;AppendableTraits} for type \c std::thread::id.<br>
template<typename TChar,  typename TAllocator> struct    AppendableTraits<std::thread::id ,TChar,TAllocator>
{
    /// Writes the thread information.
    /// @param target The \b AString that \b Append was invoked on.
    /// @param type   The thread ID to write to \p{target}.
    void operator()( TAString<TChar,TAllocator>& target, const std::thread::id& type );
};
#endif

/// Specialization of functor \alib{strings;AppendableTraits} for type \alib{lang;CallerInfo}.<br>
/// This functor writes all information available in a standard format.
/// A more flexible way is provided with formatter type \alib{format;FMTCallerInfo} that
/// defines a format string to pick certain fields of the \b CallerInfo.
/// This format specification is also availble in placeholders of type
/// \alib{format;FormatterPythonStyle}.
template<typename TChar,  typename TAllocator> struct   AppendableTraits<lang::CallerInfo ,TChar,TAllocator>
{
    /// Writes thread and source location.
    /// @param target The \b AString that \b Append was invoked on.
    /// @param ci     The caller information to write to \p{target}.
    void operator()( TAString<TChar,TAllocator>& target, const lang::CallerInfo& ci );
};

/// An enumeration of the list of "units" needed when describing time-spans, for example,
/// in the term <c>"3 days 2,4 hours"</c>.
enum class DayTimeUnits
{
    TS_ZERO       , ///< In \b DT_UNITS, this defaults to <b></b>.
    DayPlural     , ///< In \b DT_UNITS, this defaults to <b></b>.
    DaySingular   , ///< In \b DT_UNITS, this defaults to <b></b>.
    HourPlural    , ///< In \b DT_UNITS, this defaults to <b></b>.
    HourSingular  , ///< In \b DT_UNITS, this defaults to <b></b>.
    MinPlural     , ///< In \b DT_UNITS, this defaults to <b></b>.
    MinSingular   , ///< In \b DT_UNITS, this defaults to <b></b>.
    SecPlural     , ///< In \b DT_UNITS, this defaults to <b></b>.
    SecSingular   , ///< In \b DT_UNITS, this defaults to <b></b>.
    MlSecPlural   , ///< In \b DT_UNITS, this defaults to <b></b>.
    MlSecSingular , ///< In \b DT_UNITS, this defaults to <b></b>.
    McSecPlural   , ///< In \b DT_UNITS, this defaults to <b></b>.
    McSecSingular , ///< In \b DT_UNITS, this defaults to <b></b>.
    NSecPlural    , ///< In \b DT_UNITS, this defaults to <b></b>.
    NSecSingular  , ///< In \b DT_UNITS, this defaults to <b></b>.
    SIZE_OF_UNITS , ///< Stop marker
};

/// A list of "units" needed when describing time-spans, for example, in the term
/// <b>"3 days, 2 hours and 1 second"</b>.
/// This is used with the specialization of the traits-functor \alib{strings;AppendableTraits}
/// for type \alib{time;TimePointBase::Duration;DateTime::Duration}.
///
/// If module \alib_camp is not included in the \alibbuild, the values default to:
///  \snippet "strings/astring.cpp"        DT_UNITS_DEFAULTS
/// In case the module is included, the same values are resourced in #alib::BASECAMP under
/// key <c>"DurUnts"</c>. In both cases a user might change this during bootstrap.
extern String DT_UNITS[size_t(DayTimeUnits::SIZE_OF_UNITS)];

/// Specialization of functor \alib{strings;AppendableTraits} for type
/// \alib{time;TimePointBase::Duration;DateTime::Duration}.
template<typename TChar,  typename TAllocator> struct   AppendableTraits<time::DateTime::Duration ,TChar,TAllocator>
{
    /// Appends a human-readable string representation of objects of templated inner type
    /// \alib{time;TimePointBase::Duration} of type \alib{time;DateTime}.
    ///
    /// \see
    ///   For details of the conversion, see
    ///   \ref anchor_T_Append_TimePointBase_Duration "documentation of sibling method"
    ///   for type \b %Ticks, which shares this method's implementation.
    ///
    /// @param target    The \b AString that \b Append was invoked on.
    /// @param duration  The duration to append.
    void operator()( TAString<TChar,TAllocator>& target, const time::DateTime::Duration duration );
};

/// Specialization of functor \alib{strings;AppendableTraits} for type
/// \alib{time;TimePointBase::Duration;Ticks::Duration}.
template<typename TChar, typename TAllocator> struct AppendableTraits<time::Ticks::Duration ,TChar,TAllocator>
{
    /// \anchor anchor_T_Append_TimePointBase_Duration
    /// Appends a human-readable string representation of objects of templated inner type
    /// \alib{time;TimePointBase::Duration} of type \alib{time;Ticks}.
    ///
    /// Depending on the length of the duration, a different human-readable time unit or combination
    /// of it is used. The following rules are checked from top to bottom:
    /// - If zero, resource string \b "TS_ZERO" is written.
    /// - If negative, a minus sign <c>'-'</c> is written and the value is negated.
    /// - If greater than 10 days, writes the number of days as floating point number.
    /// - If between 1 and 10 days, writes the integral number of days and the additional hours as
    ///   floating point number.
    /// - If greater than an hour, writes the integral number of hours and integral minutes.
    /// - If greater than a minute, writes the integral number of minutes and integral seconds.
    /// - If greater than a second, writes the number of seconds as floating point number.
    /// - If greater than a millisecond, writes the number of milliseconds as floating point number.
    /// - If greater than a microsecond, writes the number of microseconds as floating point number.
    /// - If greater than a nanosecond, writes the number of nanoseconds as floating point number.
    ///
    /// All floating point numbers are written with two digits fractional precision.<br>
    /// The unit symbols are read from the \ref alib_mod_resources "resources" of module class
    /// \alib{camp;Basecamp}. The list is given with resource name <b>UNITS</b>.
    ///
    /// @param target    The \b AString that \b Append was invoked on.
    /// @param duration  The duration to append.
    void operator()( TAString<TChar,TAllocator>& target,  const time::Ticks::Duration duration );
};

#if !ALIB_SINGLE_THREADED

/// Specialization of functor \alib{strings;AppendableTraits} for type \alib{threads;Thread}.
template<> struct AppendableTraits<threads::Thread, nchar, lang::HeapAllocator> {
    /// Appends the name and ID of the thread.
    /// @param target The \b AString that \b Append was invoked on.
    /// @param thread The thread to append.
    ALIB_DLL void operator()( TAString<nchar, lang::HeapAllocator>& target, const threads::Thread& thread);
};

/// Specialization of functor \alib{strings;AppendableTraits} for type \alib{threads;Thread}.
template<> struct AppendableTraits<threads::Thread, wchar, lang::HeapAllocator> {
    /// Appends the name and ID of the thread.
    /// @param target The \b AString that \b Append was invoked on.
    /// @param thread The thread to append.
    ALIB_DLL void operator()( TAString<wchar, lang::HeapAllocator>& target, const threads::Thread& thread);
};
#endif


// Faking all template specializations of namespace strings for doxygen into namespace
// strings::APPENDABLES to keep the documentation of namespace string clean!
#if DOXYGEN
}
#endif

}} // namespace [alib::strings]
