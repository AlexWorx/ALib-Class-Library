//==================================================================================================
/// \file
/// This header-file is part of module \alib_variables of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================

// forward declarations
ALIB_EXPORT namespace alib::variables
{
class Configuration;
class ConfigurationPlugin;
class Declaration;
class Variable;
}

ALIB_EXPORT namespace alib::variables::detail {

/// This struct is used as the reinterpretation type of generic pointers to ones reflecting
/// the effective custom type of a configuration variable. While reinterpretation casts do not
/// need such templated model, its use increases readability of the code.
/// @tparam T The variable's data type.
template<typename T>    struct VData
{
    T               custom;   ///< The custom data that this object stores.

    /// Reinterprets the <c>this</c>-pointer to a <c>VData<TReinterpret>*</c> and returns
    /// member #custom.
    /// @tparam TReinterpret The type to reinterpret to.
    /// @return A reference to the custom data.
    template<typename TReinterpret> inline        TReinterpret&  As()
    { return reinterpret_cast<      VData<TReinterpret>*>(this)->custom;  }

    /// Reinterprets the <c>this</c>-pointer to a <c>const VData<TReinterpret>*</c> and returns
    /// member #custom.
    /// @tparam TReinterpret The type to reinterpret to.
    /// @return A const reference to the custom data.
    template<typename TReinterpret> inline  const TReinterpret&  As()                          const
    { return reinterpret_cast<const VData<TReinterpret>*>(this)->custom;  }
};

/// Convenience definition for "invalid" data element which is used everywhere as a generic
/// pointer type, which is reinterpreted when to the target type, when needed.
using VDATA     =       VData<void*>;

} //namespace [alib::variables::detail]

ALIB_EXPORT namespace alib::variables {

/// Abstract, virtual struct which provides meta-information about types storable in the
/// \b StringTree nodes of class \alib{variables;Configuration}. A pointer to a singleton of this
/// type is stored together with a reinterpreted pointer to the custom data record.
///
/// To register a custom data type with the configuration system, this type has to inherited and
/// all methods implemented. It is recommended to define custom derived types using macro
/// \ref ALIB_VARIABLES_DEFINE_TYPE.
/// Derived types are to be registered with the configuration instance by invoking
/// \alib{variables;Configuration::RegisterType}.
///
/// @see Chapter \ref alib_variables_types_custom of the Programmer's Manual of camp \alib_variables.
struct VMeta
{
    /// Virtual empty destructor.
    virtual         ~VMeta()                                                                      {}

    /// Abstract virtual method. Descendants need to return the type name they care of.<br>
    /// Types declared with macro \ref ALIB_VARIABLES_DEFINE_TYPE implement this method
    /// rightfully.
    /// @return The type's name.
    virtual String   typeName()                                                            const =0;

    #if ALIB_DEBUG
    /// Abstract virtual method. Descendants need to return the <c>std::type_info&</c> received
    /// with <c>typeid()</c>. This method is available only in debug-compilations and is used
    /// to assert that the correct types are read from declared variables.<br>
    /// Types declared with macro \ref ALIB_VARIABLES_DEFINE_TYPE implement this method
    /// rightfully.
    /// @return The C++ type ID.
    virtual const std::type_info&   dbgTypeID()                                                  =0;
    #endif

    /// Abstract virtual method. Descendants need to return '<c>sizeof(T)</c>', with \p{T} being
    /// the custom type. Types declared with macro \ref ALIB_VARIABLES_DEFINE_TYPE implement
    /// this method rightfully. With that, it is also asserted that the alignment of the custom
    /// type is not greater than '<c>alignof(uint64_t)</c>', respectively not greater than
    /// what is specified with the compiler-symbol \ref ALIB_MONOMEM_POOLALLOCATOR_DEFAULT_ALIGNMENT.
    /// @return The size of the variable's content type.
    virtual size_t  size()                                                                       =0;

    /// Abstract virtual method. Descendants need to construct a custom instance at the given
    /// \p{memory}. This is done using a "placement-new" as follows:
    ///
    ///             new (memory) MyType();
    ///
    /// The pool allocator is \b not provided to allocate the custom type itself (this was already
    /// done before calling this method). Instead, it may be used to allocate members
    /// in the custom type. It may also be passed to the instance for further use during it's
    /// lifecycle. However, in this case chapter \ref alib_variables_multithreading of the
    /// Programmer's Manual has to be considered.
    ///  @param memory  The pointer to the object of custom type.
    ///  @param pool    The object pool of the configuration. May be used to dispose pool objects.
    virtual void    construct(void* memory, PoolAllocator& pool)                                 =0;

    /// Abstract virtual method. Descendants need to destruct a custom instance at the given
    /// \p{memory}. This is done by calling the destructor as follows:
    ///
    ///             reinterpret_cast<MyTypeName*>(memory)->~MyTypeName();
    ///
    /// The pool allocator is \b not provided to free the custom type itself (this will be done
    /// automatically right after the call to this method). Instead, it may be used to free
    /// members of the type, which had been allocated during construction or during the use.
    ///
    /// \note
    ///   However, for most types included with \alib, for example, \alib{AStringPA}, if used as a
    ///   member of the custom type, no special action need to be taken and it is sufficient to call
    ///   the destructor of the custom type. This is because these types take a copy of the pool on
    ///   construction and thus their destructor disposes allocated pool objects autonomously.
    ///   In other words, if only such members are used, then method \c construct has to be
    ///   implemented to pass the pool object to the members, but this method simply invokes the
    ///   destructor of the custom type as shown above.
    ///
    ///  @param memory  The place to construct the custom type at.
    ///  @param pool    The object pool of the configuration. May be used to allocate pool objects.
    virtual void    destruct(void* memory, PoolAllocator& pool)                                  =0;


    /// Abstract virtual method. Descendants need to implement this method and de-serialize
    /// (aka parse) the custom type from the given string value.
    /// \attention
    ///   Types declared with macro \ref ALIB_VARIABLES_DEFINE_TYPE only declare this method.
    ///   An implementation needs to be provided in a compilation unit.
    ///
    /// @param data    A pointer to the user type which is to be initialized.
    /// @param cfg     The configuration that holds the variable.
    /// @param escaper An escaper to be used to convert external strings to C++ native strings.
    /// @param src     The source string to parse. This may be assigned to a value of type
    ///                \alib{strings;TSubstring} which already provides simple parser mechanics.
    ///                Likewise, \alib{strings::util;TTokenizer} might be an easy helper to be
    ///                used for parsing.
    virtual void    imPort( detail::VDATA*  data, Configuration& cfg, const StringEscaper& escaper,
                            const String&   src    )                                             =0;

    /// Abstract virtual method. Descendants need to implement this method. It is invoked when a
    /// variable is written into an external configuration source (in this case 'drain') or
    /// otherwise needs to be serialized.
    ///
    /// Note that export functions are allowed to add \alib{NEW_LINE} codes into the export string.
    /// This allows external configuration systems to nicely format their entries, in case those
    /// are human-readable. See chapter \ref alib_variables_external_custom of the Programmer's
    /// Manual for more information.
    ///
    /// \attention
    ///   Types declared with macro \ref ALIB_VARIABLES_DEFINE_TYPE only declare this method.
    ///   An implementation needs to be provided in a compilation unit. Otherwise linking software
    ///   fails.
    ///
    ///  @param data    A pointer to the user type which is to be serialized.
    ///  @param cfg     The configuration that holds the variable.
    ///  @param escaper An escaper to be used to escape strings.
    ///  @param dest    The destination string. Must not be reset prior writing, but appended.
    ///
    virtual void    exPort( detail::VDATA*   data, Configuration& cfg, const StringEscaper& escaper,
                            AString& dest )                                                      =0;
};

} // namespace [alib::variables]

//==================================================================================================
// ==== Built-in Types
//==================================================================================================
ALIB_EXPORT namespace alib::variables
{
/// Variable content type used with boolean type <c>"B"</c>. When this type is imported,
/// the value is tried to be parsed with the tokens in
/// \alib{variables;Configuration::BooleanTokens}. If successful, the index of the pair of
/// <c>true/false</c>-tokens is stored in field #TokenIndex. When exported back to
/// a configuration file or otherwise serialized or printed, then the right human-readable
/// term, corresponding to the potentially now different #Value is used.
struct Bool
{
    /// The boolean value. Defaults to \c false.
    bool        Value                                                                        =false;

    /// The index in the \alib{variables;Configuration::BooleanTokens;list of tokens} found when
    /// imported from a string. Can also be set programmatically to force a certain output
    /// "format". Defaults to \c -1 which indicates that the value was not parsed.
    /// On exporting, when \c -1, index \c 0 is used.
    int8_t      TokenIndex                                                                      =-1;

    /// Assignment operator.
    /// @param newValue The value to set.
    /// @return The new value.
    bool operator=(bool newValue)                                        { return Value= newValue; }

    /// Implicit cast operator to \c bool.
    /// @return Returns the stored value.
    operator bool()                                                          const { return Value; }
};


    /// Type definition used with configuration variables of type <c>"SV,"</c>, which stores a
    /// string array, imported by parsing a comma-separated string list.
    ///
    /// \attention
    ///   When exported, \alib{NEW_LINE} codes are added after each comma. This allows external
    ///   configuration systems to smoothly format longer lists of values. However, the
    ///   new line codes usually have to be detected with writing and eliminated on import.
    ///   Built-in type \alib{variables;IniFile} processes such codes correctly.
    using StringVectorComma      = alib::StringVectorPA;

    /// Type definition used with configuration variables of type <c>"SV;"</c>, which stores a
    /// string array, imported by parsing a string list separated by character <c>';'</c>.
    ///
    /// \attention
    ///   When exported, \alib{NEW_LINE} codes are added after each semicolon. This allows external
    ///   configuration systems to smoothly format longer lists of values. However, the
    ///   new line codes usually have to be detected with writing and eliminated on import.
    ///   Built-in type \alib{variables;IniFile} processes such codes correctly.
    using StringVectorSemicolon  = alib::StringVectorPA;

}  // namespace [alib::variables]




#if !DOXYGEN
namespace alib::variables::detail {


struct VMeta_integer : public VMeta
{
    ALIB_DLL String                 typeName()                const override { return A_CHAR("I"); }
ALIB_DBG(
    ALIB_DLL const std::type_info&  dbgTypeID()            override      { return typeid(integer); }
)
    ALIB_DLL void   construct(void* dest, PoolAllocator&)         override { new (dest) integer(); }
    ALIB_DLL void   destruct (void*     , PoolAllocator&)                                override {}
    ALIB_DLL size_t size()                                      override { return sizeof(integer); }
    ALIB_DLL void   imPort( VDATA*, Configuration&, const StringEscaper&, const String& )  override;
    ALIB_DLL void   exPort( VDATA*, Configuration&, const StringEscaper&,      AString& )  override;
};

struct VMeta_float : public VMeta
{
    ALIB_DLL String                 typeName()                const override { return A_CHAR("F"); }
ALIB_DBG(
    ALIB_DLL const std::type_info&  dbgTypeID()            override       { return typeid(double); }
)
    ALIB_DLL void   construct(void* dest, PoolAllocator&)          override { new (dest) double(); }
    ALIB_DLL void   destruct (void*     , PoolAllocator&)                                override {}
    ALIB_DLL size_t size()                                       override { return sizeof(double); }
    ALIB_DLL void   imPort( VDATA*, Configuration&, const StringEscaper&, const String& )  override;
    ALIB_DLL void   exPort( VDATA*, Configuration&, const StringEscaper&,      AString& )  override;
};

struct VMeta_String : public VMeta
{
    ALIB_DLL String                 typeName()                const override { return A_CHAR("S"); }
ALIB_DBG(
    ALIB_DLL const std::type_info&  dbgTypeID()            override    { return typeid(AStringPA); }
)
    ALIB_DLL void   construct(void* dest, PoolAllocator& pool)                              override
    { new (dest) AStringPA(pool); }
    ALIB_DLL void   destruct (void* dest, PoolAllocator&)                                   override
    { reinterpret_cast<AStringPA*>(dest)->~AStringPA();  }
    ALIB_DLL size_t size()                                    override { return sizeof(AStringPA); }
    ALIB_DLL void   imPort( VDATA*, Configuration&, const StringEscaper&, const String& )  override;
    ALIB_DLL void   exPort( VDATA*, Configuration&, const StringEscaper&,      AString& )  override;
};

} //namespace [alib::variables::detail]


ALIB_VARIABLES_DEFINE_TYPE(                       alib::variables::, Bool                 , A_CHAR("B"  ) )
ALIB_VARIABLES_DEFINE_TYPE(                       alib::boxing::, Box                  , A_CHAR("BOX") )
ALIB_VARIABLES_DEFINE_TYPE_WITH_POOL_CONSTRUCTOR( alib::variables::, StringVectorComma    , A_CHAR("SV,") )
ALIB_VARIABLES_DEFINE_TYPE_WITH_POOL_CONSTRUCTOR( alib::variables::, StringVectorSemicolon, A_CHAR("SV;") )

#endif // DOXYGEN
