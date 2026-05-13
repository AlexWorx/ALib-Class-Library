//==================================================================================================
/// \file
/// This header-file is part of module \alib_lang of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
ALIB_EXPORT namespace alib { namespace lang {

/// A simple struct that holds source code location information.
/// Usually, this is information about a caller of a function, used with debug-compilations.
/// Module \alib_alox might be explicitly compiled to include such caller information also in
/// release-versions of software.
///
///
/// @see
/// - Chapter #"alib_manual_appendix_callerinfo" of the General Programmer's Manual.
/// - Macros #"ALIB_CALLER", #"ALIB_CALLER_PRUNED", #"ALIB_COMMA_CALLER_PRUNED" and
///   #"ALIB_CALLER_NULLED".
/// - Instances of this type are appendable to class #"%AString" in a default way.
///   This is implemented with functor
///   #"AppendableTraits<lang::CallerInfo, TChar, TAllocator>".
///
/// - Class #"FMTCallerInfo" defines a format specification to customize
///   the output.
///   As always, #"FMTCallerInfo;that syntax" is directly available in
///   placeholder fields of class #"FormatterPythonStyle".
struct CallerInfo {
    const char*     File{nullptr};  ///< The name of the source file as given by compiler.
    int             Line{0};        ///< The line number within #".File".
    const char*     Func{nullptr};  ///< The function name of the source location. Nulled if
                                    ///< the location is not inside a function or method.
                                    ///
    #if ALIB_EXT_LIB_THREADS_AVAILABLE
    std::thread::id ThreadID;       ///< The ID of the calling thread.
    #endif
    const std::type_info* TypeInfo{nullptr}; ///< The calling type.
};


} // namespace alib[::lang]

/// Type alias in namespace #"%alib".
using CallerInfo      = lang::CallerInfo;

} // namespace [alib]
