//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_VARIABLES_PP
#define HPP_ALIB_VARIABLES_PP
#pragma once
#ifndef INL_ALIB
#   include "alib/alib.inl"
#endif
#if ALIB_VARIABLES

//##################################################################################################
// Enum Priority.
// Note: This is excluded from the module because it is used by ALox and if it was included, ALox
//       needed to include Configuration in is base module already.
//##################################################################################################
namespace alib {  namespace variables {


//==================================================================================================
/// \alib{enumops;ArithmeticalTraits;Arithmetical enumeration} used to control write access to
/// configuration variables, depending on the source of assignable values.
/// @see Chapter \ref alib_variables_definition_prios of the Programmer's Manual of camp
///      \alib_variables_nl.
//==================================================================================================
enum class Priority : uint16_t
{
    /// This priority value denotes that a variable is undefined and has no value set.
    /// The underlying integral value is \c 0.
    NONE                    = 0,

    /// Constant providing a priority which is even lower than default. A use-case for this
    /// priority are for third party libraries that may preset variables in cases where values are
    /// estimated or detected instead of defaulted.<br>
    /// A using code of such library may then overwrite the auto-detection estimates, by setting a
    /// default value in the configuration.<br>
    /// This priority is not used internally (by any \alib camp) today.<br>
    /// The underlying integral value is \c 1,000.
    AutoDetected            = 1000,

    /// Used to store default values, either from (resourced) declarations, hard-coded values,
    /// or values provided with method \alib{variables;Configuration::PresetImportString}.
    /// The underlying integral value is \c 2,000.
    DefaultValues           = 2000,

    /// This is the default priority when a variable is defined for setting a 'hard-coded' value.
    /// The underlying integral value is \c 4,000. Hard-coded values have a higher priority
    /// than default values, but are deemed to get overwritten by any other configuration source.
    Standard                = 4000,

    /// External application configuration sources use this element to define variables
    /// found. This element is also used with built-in class \alib{variables;IniFileFeeder}.
    /// The underlying integral value is \c 6,000.
    ConfigFile              = 6000,

    /// Used with plug-in \alib{variables;EnvironmentVariablesPlugin}.
    /// The underlying integral value is \c 8,000.
    Environment             = 8000,

    /// Used to store temporary session information. Those are higher than \b Environment but lower
    /// than \b CLI. This session priority is only a proposal. Implementations might use a
    /// different value, even for different variables, for example, <b>Environment - 1 </b> or
    /// <b>CLI + 1 </b>. It depends on the use case.<br>
    /// The underlying integral value is \c 10,000.
    SessionFile             = 10000,

    /// Used with plug-in \alib{variables;CLIVariablesPlugin}.
    /// The underlying integral value is \c 12,000.
    CLI                     = 12000,

    /// Used to store generate temporary session information. While usually changes made in
    /// source code has a low priority, session information is overwritten by running software, no
    /// matter from which external source an existing values came.
    /// If software wants to disallow the change of session information imposed by a library
    /// or a different software part, still a value can be set to protected.
    /// The underlying integral value is \c 14,000.
    Session                 = 14000,

    /// Used to define variables with protected values. If all code entities apply to the
    /// \ref alib_variables_definition "contract that this camp imposes" in respect to variable
    /// definitions and priorities, a value set with this priority cannot be manipulated from
    /// "outside", hence by config files, command line arguments or any custom configuration source
    /// or plug-in.
    ///
    /// The underlying integral value is <c>std::numeric_limits<int>::max()</c>.
    Protected               = (std::numeric_limits<uint16_t>::max)(),

};

} // namespace alib::[config]

/// Type alias in namespace \b alib.
using     Priority=       alib::variables::Priority;

} // namespace [alib]

//##################################################################################################
// Symbols introduced by module ALib.Variables
//##################################################################################################
#define ALIB_VARIABLES_DEFINE_TYPE( Namespace, CPPName,CfgTypeString)                              \
ALIB_EXPORT namespace alib::variables::detail {                                                    \
struct VMeta_ ## CPPName : public VMeta                                                            \
{                                                                                                  \
         ALIB_DLL String                typeName ()                                                     const  override { return CfgTypeString; }      \
ALIB_DBG(ALIB_DLL const std::type_info& dbgTypeID()                                                            override { return typeid(Namespace  CPPName); }  )  \
         ALIB_DLL void                  construct(void* obj, PoolAllocator&)                                   override { new (obj) Namespace  CPPName(); }        \
         ALIB_DLL void                  destruct (void* obj, PoolAllocator&)                                   override { reinterpret_cast<Namespace  CPPName*>(obj)->~CPPName(); } \
         ALIB_DLL size_t                size     ()                                                            override { static_assert(alignof(Namespace CPPName) <= alib::PoolAllocator::MAX_ALIGNMENT); return (std::max)( sizeof(Namespace CPPName), sizeof(void*) ); } \
         ALIB_DLL void                  imPort   (VDATA*, Configuration&, const StringEscaper&, const String&) override;\
         ALIB_DLL void                  exPort   (VDATA*, Configuration&, const StringEscaper&,      AString&) override;\
};}

#define ALIB_VARIABLES_DEFINE_TYPE_WITH_POOL_CONSTRUCTOR( Namespace, CPPName,CfgTypeString)        \
ALIB_EXPORT namespace alib::variables::detail {                                                    \
struct VMeta_ ## CPPName : public VMeta                                                            \
{                                                                                                  \
         ALIB_DLL String                typeName ()                                                     const  override { return CfgTypeString; }      \
ALIB_DBG(ALIB_DLL const std::type_info& dbgTypeID()                                                            override { return typeid(Namespace  CPPName); }  )  \
         ALIB_DLL void                  construct(void* obj, PoolAllocator& pool)                              override { new (obj) Namespace  CPPName(pool); }        \
         ALIB_DLL void                  destruct (void* obj, PoolAllocator&)                                   override { reinterpret_cast<Namespace  CPPName*>(obj)->~CPPName(); } \
         ALIB_DLL size_t                size     ()                                                            override { static_assert(alignof(Namespace CPPName) <= alib::PoolAllocator::MAX_ALIGNMENT); return (std::max)( sizeof(Namespace CPPName), sizeof(void*) ); } \
         ALIB_DLL void                  imPort   (VDATA*, Configuration&, const StringEscaper&, const String&) override;\
         ALIB_DLL void                  exPort   (VDATA*, Configuration&, const StringEscaper&,      AString&) override;\
};}


#define ALIB_VARIABLES_REGISTER_TYPE(CPPName)                                                      \
     GetConfig()->RegisterType<alib::variables::detail::VMeta_ ## CPPName>();



#endif
#endif // HPP_ALIB_VARIABLES_PP
