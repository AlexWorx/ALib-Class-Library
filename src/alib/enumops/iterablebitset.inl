//==================================================================================================
/// \file
/// This header-file is part of the module \alib_enumops of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib { namespace enumops{

//==================================================================================================
/// This is a type alias (using statement) that is available (enabled) for enumerations for
///  which the type trait \alib{enumops;IterableTraits} is specialized.
/// With this, the interface methods of templated class \alib{lang;TBitSet} accept and return
/// enumeration elements.
///
/// @see For a quick tutorial on the use of this type, see chapter
/// \ref alib_enums_iter_bitset "3.5 Using Class TBitSet with Iterable Enums" of the Programmer's
/// Manual of the module \alib_enumops_nl.
///
/// @tparam TEnum      The enum type to use with class \alib{lang;TBitSet}.
//==================================================================================================
template<typename TEnum>
requires alib::enumops::IsIterable<TEnum>
using EnumBitSet =  lang::TBitSet<TEnum, IterableTraits<TEnum>::End,
                                         IterableTraits<TEnum>::Begin>;
} // namespace alib[::enumops::]

/// Type alias in namespace \b alib.
/// @see For more information, consult type definition \alib{enumops;EnumBitSet}.
template<typename TEnum>
requires alib::enumops::IsIterable<TEnum>
using EnumBitSet =  lang::TBitSet<TEnum, enumops::IterableTraits<TEnum>::End,
                                         enumops::IterableTraits<TEnum>::Begin>;

} // namespace [alib]
