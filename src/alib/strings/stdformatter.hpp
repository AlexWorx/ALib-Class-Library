//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================


//##################################################################################################
//################################ std::formatter<alib::strings::xyz> ##############################
//##################################################################################################

#if DOXYGEN
namespace alib::strings::compatibility::std {
#else
#   if __has_include(<format>)
namespace std {
#   else
namespace fmt {
#   endif
#endif

/// Standard formatter specialization for #"TString" for use with \c std::format.
///
/// By deriving from <c>std::formatter<std::basic_string_view<TChar>, TChar></c>,
/// we leverage the library’s optimized handling for string views, thereby avoiding issues
/// with non-constexpr functions in the custom formatter path.
/// @see Chapter #"alib_strings_stdformat" of the Programmer's Manual of the module
//       \alib_strings_nl.
/// @tparam TChar The character type.
template<typename TChar>
struct formatter<alib::strings::TString<TChar>, TChar>
     : formatter<std::basic_string_view<TChar>, TChar> {

    // No need to override parse() since the base class does the work

    /// This function converts the given \p{str} <c>std::basic_string_view<TChar></c> and
    /// then calls the base class's formatter, which is fully constexpr-capable.
    /// @tparam TContext The type of the format context.
    /// @param str The \alib string to format.
    /// @param ctx The formatting context provided by \c std::format.
    /// @return An iterator to the end of the output range.
    template<typename TContext>
    auto format(const alib::strings::TString<TChar>& str, TContext& ctx)                     const {
        // Delegate formatting to the std::basic_string_view<TChar> formatter
        return formatter<std::basic_string_view<TChar>, TChar>::format(
            static_cast<std::basic_string_view<TChar>>(str), ctx);
    }
};

/// Standard formatter specialization for #"TCString" for use with \c std::format.
///
/// By deriving from <c>std::formatter<std::basic_string_view<TChar>, TChar></c>,
/// we leverage the library’s optimized handling for string views, thereby avoiding issues
/// with non-constexpr functions in the custom formatter path.
/// @see Chapter #"alib_strings_stdformat" of the Programmer's Manual of the module
//       \alib_strings_nl.
/// @tparam TChar The character type.
template<typename TChar>
struct formatter<alib::strings::TCString<TChar>, TChar>
     : formatter<std::basic_string_view<TChar>, TChar>  {

    // No need to override parse() since the base class does the work

    /// This function converts the given \p{str} <c>std::basic_string_view<TChar></c> and
    /// then calls the base class's formatter, which is fully constexpr-capable.
    /// @tparam TContext The type of the format context.
    /// @param str The \alib string to format.
    /// @param ctx The formatting context provided by \c std::format.
    /// @return An iterator to the end of the output range.
    template<typename TContext>
    auto format(const alib::strings::TCString<TChar>& str, TContext& ctx)                    const {
        // Delegate formatting to the std::basic_string_view<TChar> formatter
        return formatter<std::basic_string_view<TChar>, TChar>::format(
            static_cast<std::basic_string_view<TChar>>(str), ctx);
    }
};

/// Standard formatter specialization for #"TAString" for use with \c std::format.
///
/// By deriving from <c>std::formatter<std::basic_string_view<TChar>, TChar></c>,
/// we leverage the library’s optimized handling for string views, thereby avoiding issues
/// with non-constexpr functions in the custom formatter path.
/// @see Chapter #"alib_strings_stdformat" of the Programmer's Manual of the module
//       \alib_strings_nl.
/// @tparam TChar The character type.
template<typename TChar, typename TAllocator>
struct formatter<alib::strings::TAString<TChar, TAllocator>, TChar>
     : formatter<std::basic_string_view<TChar>, TChar> {

    /// This function converts the given \p{str} <c>std::basic_string_view<TChar></c> and
    /// then calls the base class's formatter, which is fully constexpr-capable.
    /// @tparam TContext The type of the format context.
    /// @param str The \alib string to format.
    /// @param ctx The formatting context provided by \c std::format.
    /// @return An iterator to the end of the output range.
    template<typename TContext>
    auto format(const alib::strings::TAString<TChar, TAllocator>& str, TContext& ctx)        const {
        return formatter<std::basic_string_view<TChar>, TChar>::format(
            static_cast<std::basic_string_view<TChar>>(str), ctx);
    }
};

/// Standard formatter specialization for #"TSubstring" for use with \c std::format.
///
/// By deriving from <c>std::formatter<std::basic_string_view<TChar>, TChar></c>,
/// we leverage the library’s optimized handling for string views, thereby avoiding issues
/// with non-constexpr functions in the custom formatter path.
/// @see Chapter #"alib_strings_stdformat" of the Programmer's Manual of the module
//       \alib_strings_nl.
/// @tparam TChar The character type.
template<typename TChar>
struct formatter<alib::strings::TSubstring<TChar>, TChar>
  : formatter<std::basic_string_view<TChar>, TChar> {

    // No need to override parse() since the base class does the work

    /// This function converts the given \p{str} <c>std::basic_string_view<TChar></c> and
    /// then calls the base class's formatter, which is fully constexpr-capable.
    /// @tparam TContext The type of the format context.
    /// @param str The \alib string to format.
    /// @param ctx The formatting context provided by \c std::format.
    /// @return An iterator to the end of the output range.
    template<typename TContext>
    auto format(const alib::strings::TSubstring<TChar>& str, TContext& ctx)                  const {
        // Delegate formatting to the std::basic_string_view<TChar> formatter
        return formatter<std::basic_string_view<TChar>, TChar>::format(
            static_cast<std::basic_string_view<TChar>>(str), ctx);
    }
};

/// Standard formatter specialization for #"TLocalString" for use with \c std::format.
///
/// By deriving from <c>std::formatter<std::basic_string_view<TChar>, TChar></c>,
/// we leverage the library’s optimized handling for string views, thereby avoiding issues
/// with non-constexpr functions in the custom formatter path.
/// @see Chapter #"alib_strings_stdformat" of the Programmer's Manual of the module
//       \alib_strings_nl.
/// @tparam TChar The character type.
template<typename TChar, alib::integer TCapacity, typename TAllocator>
struct formatter<alib::strings::TLocalString<TChar,TCapacity,TAllocator>, TChar>
     : formatter<std::basic_string_view<TChar>, TChar> {
// No need to override parse() since the base class does the work

    /// This function converts the given \p{str} <c>std::basic_string_view<TChar></c> and
    /// then calls the base class's formatter, which is fully constexpr-capable.
    /// @tparam TContext The type of the format context.
    /// @param str The \alib string to format.
    /// @param ctx The formatting context provided by \c std::format.
    /// @return An iterator to the end of the output range.
    template<typename TContext>
    auto format(const alib::strings::TLocalString<TChar,TCapacity,TAllocator>& str,
                TContext&                                                      ctx )         const {
        // Delegate formatting to the std::basic_string_view<TChar> formatter
        return formatter<std::basic_string_view<TChar>, TChar>::format(
            static_cast<std::basic_string_view<TChar>>(str), ctx);
    }
};

} // namespace [std]

//##################################################################################################
//################################# std::formatter<alib::Appendable> ###############################
//##################################################################################################

#if DOXYGEN
namespace alib::strings::APPENDABLES {
#else
namespace alib::strings {
#endif

/// For this simple templated wrapper struct, a generic specialization of type traits
/// \c std::formatter exists if the template type \p{TAppendable} has a specialization of
/// #"AppendableTraits".
/// In other words, all types which have been #"alib_strings_assembly_ttostring;made appendable"
/// to class #"TAString", can be wrapped in this type and then used as
/// arguments of function \c std::format.
///
/// \note
///   Unfortunately, it is not possible with all compilers to generically specialize
///   \c std::formatter for all appendable types. With some compilers, this leads to an
///   unresolvable ambiguity for types which have both, a specialization for \c std::formatter
///   and for #"%AppendableTraits". Therefore, this wrapper has to be used.
///
/// @see Chapter #"alib_strings_stdformat" of the Programmer's Manual of the module
///      \alib_strings_nl.
///
/// @tparam TAppendable  The type of the wrapped appendable.
template<typename TAppendable>
struct Appendable {
    const TAppendable&    appendable; ///< A reference to the wrapped appendable.

    /// Constructor.
    /// @param pAppendable The wrapped appendable.
    Appendable(const TAppendable& pAppendable)                         : appendable{pAppendable}  {}
};

/// C++17 Deduction Guide to construct the type #"APPENDABLES::Appendable".
/// @tparam TAppendable  The type of the wrapped appendable.
template<typename TAppendable>
Appendable(const TAppendable& ) -> Appendable<TAppendable>                                        ;


} // namespace [alib::strings[::APPENDABLES]]

#if DOXYGEN
namespace alib::strings::compatibility::std {
#else
# if __has_include(<format>)
namespace std {
# else
namespace fmt {
# endif
#endif


/// This specialization of type traits \c std::formatter enables to format the wrapper type
/// #"APPENDABLES::Appendable", for all types that have a specialization of #"AppendableTraits".
///
/// By deriving from <c>std::formatter<std::basic_string_view<TChar>, TChar></c>,
/// we leverage the library’s optimized handling for string views, thereby avoiding issues
/// with non-constexpr functions in the custom formatter path.
///
/// @see
///   - Wrapper type #"APPENDABLES::Appendable"
///   - Chapter #"alib_strings_stdformat" of the Programmer's Manual of the module
//      \alib_strings_nl.
/// @tparam TAppendable The type of the value wrapped in type #"%APPENDABLES::Appendable".
/// @tparam TChar       The character type used with the formatting operation.
template <typename TAppendable, typename TChar>
requires alib::strings::IsAppendable<TAppendable, TChar, alib::lang::HeapAllocator>
struct formatter<alib::strings::Appendable<TAppendable>, TChar>
     : formatter<std::basic_string_view<TChar>, TChar> {
    /// Creates an intermediate local string (avoiding dynamic allocation in most cases),
    /// appends the value wrapped in \p{wrapper} to it, and then delegates formatting to the
    /// \c std::basic_string_view<TChar> formatter.
    /// @param wrapper The wrapper containing the appendable to append.
    /// @param ctx     The formatting context.
    /// @return An end iterator of the output range.
    template <typename FormatContext>
    auto format(const alib::strings::Appendable<TAppendable>&  wrapper,
                FormatContext&                                 ctx)                          const {
        alib::strings::TLocalStringNoWarning<TChar, 256, alib::lang::HeapAllocator> buf;
        buf.Append(wrapper.appendable);

        return formatter<std::basic_string_view<TChar>, TChar>::format(
                         static_cast<std::basic_string_view<TChar>>(buf), ctx);
    }
};

} // namespace [std], for doxygen it is [alib::strings::compatibility::std]

// Faking all template specializations of namespace strings for doxygen into namespace
// strings::APPENDABLES to keep the documentation of namespace string clean!
#if DOXYGEN
namespace alib::strings::APPENDABLES {
#else
namespace alib::strings {
#endif

/// This struct enables the use of C++20 function \c std::format - and with that any adaptation
/// of custom types - to be directly appendable to class #"TAString", without the
/// need of creating an intermediate temporary \c std::string.
///
/// For that, this struct stores the format string and a tuple of decayed formatting arguments.
/// With a corresponding specialization of struct #"AppendableTraits", the formatted
/// output is produced using C++20's \c std::vformat_to and a <c>std::back_insert_iterator</c>
/// that appends characters directly to an #"%AString".
///
/// \note
///   C++20 supports formatting only on character types \c char and \c wchar_t.
///   With that, the specializations of #"AppendableTraits" are likewise
///   only defined for these concrete character types. If you compile \alib to switch
///   the default character type, for example, by using configuration macro
///   #"ALIB_CHARACTERS_SIZEOF_WCHAR" to change the default size
///   (which is defined by the compiler !), then appending this type \c TStdFormat might
///   not be available.
///
/// \note
///   For the same reason, the alias definitions of this struct, namely
///   - alib::StdFormat,
///   - alib::NStdFormat, and
///   - alib::WStdFormat
///
/// \note
///   use these explicit character types, instead of the "logical" types #"alib::nchar;2" and
///   #"alib::wchar;2".
///
/// @see Chapter #"alib_strings_stdformat" of the Programmer's Manual of the module
///      \alib_strings_nl.
///
/// @tparam TChar  The character type of the #"%TAString" and the format string.
/// @tparam TArgs  Variadic template parameters representing the types of the formatting arguments.
template<typename TChar, typename... TArgs>
struct TStdFormat {
    /// The format string given construction. Will be passed to <c>std::format</c>.
    TString<TChar> format;

    /// The variadic arguments given construction. Will be passed to <c>std::format</c>.
    std::tuple<std::decay_t<TArgs>...> arguments;

    /// Constructor. Uses perfect forwarding and stores decayed copies of the arguments.
    /// @param formatString Stored in #".format".
    /// @param args         Stored in #".arguments".
    TStdFormat(const TString<TChar>& formatString, TArgs&&... args)
    : format(formatString)
    , arguments(std::make_tuple(std::forward<TArgs>(args)...))                                    {}
};


/// C++17 Deduction Guide to construct the type #"APPENDABLES::TStdFormat".
/// @tparam TChar The character type of the target \c std::string.
/// @tparam TArgs The types of the variadic formatter arguments.
template<typename TChar, typename... TArgs>
TStdFormat(TString<TChar>, TArgs&&...) -> TStdFormat<TChar, TArgs...>;

/// Specialization of the functor #"AppendableTraits" for the type
/// #"TStdFormat;TStdFormat<char>".
template<typename TAllocator, typename... Args>
struct  AppendableTraits<TStdFormat<char, Args...> ,char,TAllocator> {
    /// The functor operator.
    /// @param target     The target string-buffer.
    /// @param fmtpackage The packaged argumentds to \c std::format.
    void operator()( TAString<char,TAllocator>&         target,
                     const TStdFormat<char, Args...>&   fmtpackage ) {
        std::back_insert_iterator<TAString<char, TAllocator>> it(target);

        #if __has_include(<format>)
            namespace f_temp=std;
        #else
            namespace f_temp=fmt;
        #endif

        // Use std::apply to expand the tuple and pass each argument to make_format_args.
        std::apply( [&](auto&&... a) {
                        f_temp::vformat_to(it, fmtpackage.format,
                        f_temp::make_format_args(static_cast<const decltype(a)&>(a)...));
                        },
                    fmtpackage.arguments  );
    }
};

/// Specialization of the functor #"AppendableTraits" for the type
/// #"TStdFormat;TStdFormat<wchar_t>".
template<typename TAllocator, typename... Args>
struct  AppendableTraits<TStdFormat<wchar_t, Args...> ,wchar_t,TAllocator>  {
    /// The functor operator.
    /// @param target     The target string-buffer.
    /// @param fmtpackage The packaged argumentds to \c std::format.
    void operator()( TAString<wchar_t,TAllocator>&       target,
                     const TStdFormat<wchar_t, Args...>& fmtpackage ) {
        std::back_insert_iterator<TAString<wchar_t, TAllocator>> it(target);

        // Use std::apply to expand the tuple and pass each argument to make_format_args.
        std::apply( [&](auto&&... a) {
                     // Cast each argument to a const lvalue reference.
                     vformat_to(it, fmtpackage.format,
                                make_wformat_args(static_cast<const decltype(a)&>(a)...));
                    },
                    fmtpackage.arguments
        );
    }
};

} // namespace [alib::strings]

namespace alib {

    /// Type alias in namespace #"%alib".
    template<typename... Args>
    using StdFormat= strings::TStdFormat<character, Args...>;

    /// Type alias in namespace #"%alib".
    template<typename... Args>
    using NStdFormat= strings::TStdFormat<char, Args...>;

    /// Type alias in namespace #"%alib".
    template<typename... Args>
    using WStdFormat= strings::TStdFormat<wchar_t, Args...>;

    /// Type alias in namespace #"%alib".
    template<typename TAppendable>
    using Appendable=  strings::Appendable<TAppendable>;
}
