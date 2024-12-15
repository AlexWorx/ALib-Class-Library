//==================================================================================================
/// \file
/// This header file is part of module \alib_basecamp of the \aliblong.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_CAMP_MESSAGE_MESSAGE
#define HPP_ALIB_CAMP_MESSAGE_MESSAGE 1
#pragma once
#include "alib/strings/cstring.hpp"
ALIB_ASSERT_MODULE(CAMP)

#include "alib/boxing/enum.hpp"
#include "alib/monomem/sharedmonoval.hpp"

namespace alib::lang {

//==================================================================================================
/// This struct stores a list of information objects of arbitrary types, by publicly inheriting type
/// \alib{BoxesMA}.
/// In addition, the \alib{lang;CallerInfo} is attached, which usually refers to the place
/// (and thread) tat constructed an instance of this type.<br>
/// Finally, a type identifier is available with field #Type.
///
/// Note that while message data might be passed with construction, informational data may be added,
/// changed or removed during the life-cycle of an instance using the inherited interface of
/// class \alib{boxing;TBoxes}.
///
/// Inside \alib, the struct is used with types \alib{lang;Exception} and \alib{lang;Report}.
//==================================================================================================
struct Message : BoxesMA
{
    CallerInfo  CI;       ///< The source code location that this message relates to.
    Enum        Type;     ///< A type identifier, defined with construction by providing an element
                              ///< of an arbitrary enumeration type.
    /// Constructor.
    /// @param ci                 Information about the scope of invocation.
    /// @param monoAllocator      The allocator to store the arguments in.
    /// @param type               The message type.
    /// @param args               Variadic, templated list of arguments.
    template <typename... TBoxables>
    Message( const CallerInfo& ci,
             MonoAllocator& monoAllocator,
             const Enum& type, TBoxables&&... args )
    : TBoxes( monoAllocator )
    , CI  (ci)
    , Type(type)
    { Add( std::forward<TBoxables>(args)... ); }
};

} // namespace [alib::lang]


// Customize boxing of type Message to its parent type Boxes. This allows "flattening" all
// arguments stored in a message when added to an instance of Boxes.
ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING( alib::lang::Message*, boxing::TBoxes<MonoAllocator>* )


#endif // HPP_ALIB_CAMP_MESSAGE_MESSAGE

