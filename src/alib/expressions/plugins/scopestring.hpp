// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#ifndef HPP_ALIB_EXPRESSIONS_PLUGINS_SCOPESTRING
#define HPP_ALIB_EXPRESSIONS_PLUGINS_SCOPESTRING

#ifndef HPP_ALIB_EXPRESSIONS_SCOPE
#   include "alib/expressions/scope.hpp"
#endif

namespace aworx { namespace lib { namespace expressions { namespace plugins {

/** ************************************************************************************************
 * Simple string class used by native C++ \alib{expressions,CallbackDecl,expression callback functions}
 * to allocate string data returned as (intermediate) expression result.
 *
 * Optional constructors allow to copy a source string or alternatively (any) boxed value.
 * (Boxes provided need to have implemented box-function \alib{boxing,FAppend}.).
 * The additional size needed besides optional copy data has to be known at the creation of the
 * string. A \b %ScopeString must not exceed its capacity after creation. Doing so, results in a
 * memory leak, as the string will not be properly destructed.
 * In debug compilations an \alib warning is raised when capacity gets exceeded.
 *
 * \note
 *  If dynamic strings are needed, a custom \b %Scope class may be created that allows allocation
 *  of \e 'true' \b %AString objects, e.g. stored in a vector of pointers which are deleted
 *  upon clearing of the scope.
 **************************************************************************************************/
class ScopeString : public strings::TAString<character>
{
    public:

    /**
     * Allocates a scope string of given capacity.
     * @param scope     The scope to allocated data with.
     * @param capacity  The capacity of the of the string.
     */
    ScopeString( Scope& scope, integer capacity )
    : strings::TAString<character>( scope.Memory.AllocArray<character>( static_cast<size_t>(capacity + 1) ) ,
                                                                                               capacity + 1 )
    {}

    /**
     * Allocates a scope string of given capacity plus the length of the string \p{src} and copies
     * \p{source} to it.
     * @param scope     The scope to allocated data with.
     * @param src       The source string to copy.
     * @param additionalCapacity The extra capacity to allocate
     */
    ScopeString( Scope& scope, const strings::TString<character>& src, integer additionalCapacity )
    : strings::TAString<character>( scope.Memory.AllocArray<character>( src.Length() + additionalCapacity + 1),
                                                                        src.Length() + additionalCapacity + 1 )
    {
        strings::TString<character>::length= src.CopyTo( strings::TString<character>::vbuffer );
    }

    /**
     * Returns a string with contents of the given \p{box} written at the start (using boxing
     * interface \alib{boxing,FAppend}) and the given additional capacity.
     * @param scope              The scope to allocated data with.
     * @param additionalCapacity Additional capacity to allocate in \p{scope}.
     * @param value              A boxed object to write to the start of the string.
     */
    ScopeString( Scope& scope, integer additionalCapacity, const Box& value)
    : strings::TAString<character>()
    {
        LocalString<256> tmp;
        tmp.DbgDisableBufferReplacementWarning();
        tmp << value;

        SetBuffer( scope.Memory.AllocArray<character>( tmp.Length() + additionalCapacity + 1 ),
                                                       tmp.Length() + additionalCapacity + 1  ,
                                                       tmp.Length(), Responsibility::KeepWithSender );

        tmp.CopyTo( strings::TString<character>::vbuffer );
    }
};


}}} // namespace aworx[::lib::expressions::plugins]

/// Type alias in namespace #aworx.
using      ScopeString=    aworx::lib::expressions::plugins::ScopeString;

} // namespace [aworx]


// allow boxing for Scope string
#if !ALIB_DOCUMENTATION_PARSER

namespace aworx { namespace lib { namespace characters {
template<>
struct  T_CharArray<expressions::plugins::ScopeString, character>
{
    static        constexpr AccessType       Access       = AccessType::Implicit;
    static inline constexpr const character* Buffer(const expressions::plugins::ScopeString& src)  { return src.Buffer(); }
    static inline constexpr integer          Length(const expressions::plugins::ScopeString& src)  { return src.Length(); }

    static        constexpr ConstructionType Construction = ConstructionType::NONE;
};
}}}


#endif // !ALIB_DOCUMENTATION_PARSER

#endif // HPP_ALIB_EXPRESSIONS_PLUGINS_SCOPESTRING

