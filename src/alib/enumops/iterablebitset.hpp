//==================================================================================================
/// \file
/// This header-file is part of the module \alib_enumops of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
ALIB_EXPORT namespace alib { namespace enumops{

//==================================================================================================
/// This is a type alias (using statement) that is available (enabled) for enumerations for
///  which the type trait #"IterableTraits" is specialized.
/// With this, the interface methods of templated class #"TBitSet" accept and return
/// enumeration elements.
///
/// @see For a quick tutorial on the use of this type, see chapter
/// #"alib_enums_iter_bitset" of the Programmer's
/// Manual of the module \alib_enumops_nl.
///
/// @tparam TEnum      The enum type to use with class #"TBitSet".
//==================================================================================================
template<typename TEnum>
requires alib::enumops::IsIterable<TEnum>
using EnumBitSet =  lang::TBitSet<TEnum, IterableTraits<TEnum>::End,
                                         IterableTraits<TEnum>::Begin>;
} // namespace alib[::enumops::]

/// Type alias in namespace #"%alib".
/// @see For more information, consult type definition #"enumops::EnumBitSet".
template<typename TEnum>
requires alib::enumops::IsIterable<TEnum>
using EnumBitSet =  lang::TBitSet<TEnum, enumops::IterableTraits<TEnum>::End,
                                         enumops::IterableTraits<TEnum>::Begin>;

} // namespace [alib]
