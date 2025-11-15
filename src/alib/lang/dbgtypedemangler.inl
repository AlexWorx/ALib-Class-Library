//==================================================================================================
/// \file
/// This header-file is part of module \alib_lang of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
///
/// \note
///   To reduce complexity, this header is not shown in inclusion graphs of this documentation.
//==================================================================================================
#if ALIB_DEBUG

ALIB_EXPORT namespace alib::lang {

//==================================================================================================
/// Retrieves human-readable names from C++ run-time type information.<br>
/// This class is available only with debug-builds of \alib.
///
/// Module \alib_strings functor
/// \alib{strings::APPENDABLES;AppendableTraits<std::type_info,TChar,TAllocator>} in debug-compilations,
/// which internally makes use of this type. This allows appending the result of keyword \c typeid
/// directly to \b AString instances.
//==================================================================================================
class DbgTypeDemangler
{
  protected:
    const char*     name;       ///< The translated name.
    bool            failed;     ///< If \c true, demangling failed.

  public:
    /// Constructor
    /// @param typeInfo The information struct on the C++ type.
    ALIB_DLL
    DbgTypeDemangler( const std::type_info& typeInfo );

    #if defined(__GNUC__) || defined(__clang__)
    /// Destructor
    ALIB_DLL
    ~DbgTypeDemangler();
    #endif

    /// Returns the demangled, human-readable name of the type which was provided in the
    /// constructor.
    /// @return The demangled type name.
    ALIB_DLL
    const char*  Get();

}; // class DbgTypeDemangler

} // namespace [alib::lang]

#endif // ALIB_DEBUG
