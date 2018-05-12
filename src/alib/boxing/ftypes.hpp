// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
/** @file */ // Hello Doxygen

//  set include guard
#ifndef HPP_ALIB_BOXING_FTYPES
#define HPP_ALIB_BOXING_FTYPES 1

#if !defined (HPP_ALIB_BOXING_LIB)
    #include "alib/boxing/boxing.hpp"
#endif


#if ALIB_FEAT_BOXING_FTYPES


namespace aworx { namespace lib { namespace boxing { namespace ftypes
{
    /** This is a type alias to \c int64_t. With default implementation of boxing C++ fundamental
     *  types in place, all signed integer types get boxed to this type.  */
    using boxed_int=                   int64_t;

    /// Unsigned version of #boxed_int
    using boxed_uint=                  uint64_t;

// We are faking all template specializations of namespace boxing for doxygen into namespace
// ftypes to keep documentation of namespace boxing clean!
#if !defined(DOX_PARSER)
}
#endif



// #################################################################################################
// Specializations of struct T_Boxing
// #################################################################################################

//! @cond NO_DOX

ALIB_BOXING_SPECIALIZE_CB_CUB(bool          , bool               , false , true  );
ALIB_BOXING_SPECIALIZE_CB_CUB(character     , character          , false , true  );
ALIB_BOXING_SPECIALIZE_CB_CUB(complementChar, complementChar     , false , true  );
ALIB_BOXING_SPECIALIZE_CB_CUB(strangeChar   , strangeChar        , false , true  );

ALIB_BOXING_SPECIALIZE_CB_CUB( int8_t       , ftypes::boxed_int  , false , false );
ALIB_BOXING_SPECIALIZE_CB_CUB(uint8_t       , ftypes::boxed_uint , false , false );
ALIB_BOXING_SPECIALIZE_CB_CUB( int16_t      , ftypes::boxed_int  , false , false );
ALIB_BOXING_SPECIALIZE_CB_CUB(uint16_t      , ftypes::boxed_uint , false , false );
ALIB_BOXING_SPECIALIZE_CB_CUB( int32_t      , ftypes::boxed_int  , false , false );
ALIB_BOXING_SPECIALIZE_CB_CUB(uint32_t      , ftypes::boxed_uint , false , false );
ALIB_BOXING_SPECIALIZE_CB_CUB( int64_t      , ftypes::boxed_int  , false , true  );
ALIB_BOXING_SPECIALIZE_CB_CUB(uint64_t      , ftypes::boxed_uint , false , true  );
ALIB_BOXING_SPECIALIZE_CB_CUB( intGap_t     , ftypes::boxed_int  , false , false );
ALIB_BOXING_SPECIALIZE_CB_CUB(uintGap_t     , ftypes::boxed_uint , false , false );


ALIB_BOXING_SPECIALIZE_CB(    float         , double             , false , false );
ALIB_BOXING_SPECIALIZE(       double        , double             , false , true  );

ALIB_BOXING_SPECIALIZE_CB(   character     *, character          , true  , false );
ALIB_BOXING_SPECIALIZE_CB(   complementChar*, complementChar     , true  , false );
ALIB_BOXING_SPECIALIZE_CB(   strangeChar   *, strangeChar        , true  , false );


// #################################################################################################
// Definitions of T_Boxing::Boxing
// #################################################################################################
inline void  T_Boxing<bool          >::Boxing( Box& box, const bool          & value )  { box.data.Value= static_cast<boxvalue>(value);  box.data.Length= 0;  }
inline void  T_Boxing<character     >::Boxing( Box& box, const character     & value )  { box.data.Value= static_cast<boxvalue>(value);  box.data.Length= 0;  }
inline void  T_Boxing<complementChar>::Boxing( Box& box, const complementChar& value )  { box.data.Value= static_cast<boxvalue>(value);  box.data.Length= 0;  }
inline void  T_Boxing<strangeChar   >::Boxing( Box& box, const strangeChar   & value )  { box.data.Value= static_cast<boxvalue>(value);  box.data.Length= 0;  }

inline void  T_Boxing< int8_t       >::Boxing( Box& box, const  int8_t       & value )  { box.data.Value= static_cast<boxvalue>(value);  box.data.Length= 0;  }
inline void  T_Boxing<uint8_t       >::Boxing( Box& box, const uint8_t       & value )  { box.data.Value= static_cast<boxvalue>(value);  box.data.Length= 0;  }
inline void  T_Boxing< int16_t      >::Boxing( Box& box, const int16_t       & value )  { box.data.Value= static_cast<boxvalue>(value);  box.data.Length= 0;  }
inline void  T_Boxing<uint16_t      >::Boxing( Box& box, const uint16_t      & value )  { box.data.Value= static_cast<boxvalue>(value);  box.data.Length= 0;  }
inline void  T_Boxing< int32_t      >::Boxing( Box& box, const  int32_t      & value )  { box.data.Value= static_cast<boxvalue>(value);  box.data.Length= 0;  }
inline void  T_Boxing<uint32_t      >::Boxing( Box& box, const uint32_t      & value )  { box.data.Value= static_cast<boxvalue>(value);  box.data.Length= 0;  }
inline void  T_Boxing< int64_t      >::Boxing( Box& box, const  int64_t      & value )  { box.data.Value= static_cast<boxvalue>(value);  box.data.Length= 0;  }
inline void  T_Boxing<uint64_t      >::Boxing( Box& box, const uint64_t      & value )  { box.data.Value= static_cast<boxvalue>(value);  box.data.Length= 0;  }
inline void  T_Boxing< intGap_t     >::Boxing( Box& box, const  intGap_t     & value )  { box.data.Value= static_cast<boxvalue>(value);  box.data.Length= 0;  }
inline void  T_Boxing<uintGap_t     >::Boxing( Box& box, const uintGap_t     & value )  { box.data.Value= static_cast<boxvalue>(value);  box.data.Length= 0;  }

ALIB_WARNINGS_ALLOW_TEMPLATE_META_PROGRAMMING

inline void  T_Boxing<float>::Boxing( Box& box, const float& value )
{
    DefaultBoxing( box, static_cast<double>(value) );
}

inline void  T_Boxing<character*>::Boxing( Box& box, character* const& value )
{
    if( sizeof(value) < sizeof(boxvalue) )
        box.data.Value= 0;
    memcpy( &box.data.Value, &value, sizeof(value) );
    box.data.Length= value == nullptr || *value == '\0' ? 0 : -1;
}

inline void  T_Boxing<complementChar*>::Boxing( Box& box,  complementChar* const& value )
{
    if( sizeof(value) < sizeof(boxvalue) )
        box.data.Value= 0;
    memcpy( &box.data.Value, &value, sizeof(value) );
    box.data.Length= value == nullptr || *value == '\0' ? 0 : -1;
}

inline void  T_Boxing<strangeChar   *>::Boxing( Box& box, strangeChar* const& value )
{
    if( sizeof(value) < sizeof(boxvalue) )
        box.data.Value= 0;
    memcpy( &box.data.Value, &value, sizeof(value) );
    box.data.Length= value == nullptr || *value == '\0' ? 0 : -1;
}

ALIB_WARNINGS_RESTORE


// #################################################################################################
// Definitions of T_Boxing::Unboxing
// #################################################################################################
inline bool           T_Boxing<bool          >::Unboxing( const Box& box )  { return box.data.Value != 0;                           }
inline character      T_Boxing<character     >::Unboxing( const Box& box )  { return static_cast<character     >( box.data.Value ); }
inline complementChar T_Boxing<complementChar>::Unboxing( const Box& box )  { return static_cast<complementChar>( box.data.Value ); }
inline strangeChar    T_Boxing<strangeChar   >::Unboxing( const Box& box )  { return static_cast<strangeChar   >( box.data.Value ); }

inline  int64_t       T_Boxing< int64_t      >::Unboxing( const Box& box )  { return static_cast< int64_t      >( box.data.Value ); }
inline uint64_t       T_Boxing<uint64_t      >::Unboxing( const Box& box )  { return static_cast<uint64_t      >( box.data.Value ); }

//! @endcond

// #################################################################################################
// Specializations of T_SGetArraySize<> for character types
// #################################################################################################
/**
 * Specialization of template function for type \c character, that subtracts \c 1 from the provided
 * array extend to denote the length of a character string.
 *
 * @param arrayExtent  The array extent. \c 1 will be deducted.
 * @returns The length as given by \p{arrayExtent} but reduced by \c 1.
 */
template<>  inline constexpr  integer    T_SetArraySize<character>   ( integer arrayExtent )
{
    return arrayExtent -1;
}

/**
 * Specialization of template function for type \c complementChar, that subtracts \c 1 from the provided
 * array extend to denote the length of a character string.
 *
 * @param arrayExtent  The array extent. \c 1 will be deducted.
 * @returns The length as given by \p{arrayExtent} but reduced by \c 1.
 */
template<>  inline constexpr  integer    T_SetArraySize<complementChar>( integer arrayExtent )
{
    return arrayExtent -1;
}

/**
 * Specialization of template function for type \c strangeChar, that subtracts \c 1 from the provided
 * array extend to denote the length of a character string.
 *
  * @param arrayExtent  The array extent. \c 1 will be deducted.
* @returns The length as given by \p{arrayExtent} but reduced by \c 1.
 */
template<>  inline constexpr  integer    T_SetArraySize<strangeChar>( integer arrayExtent )
{
    return arrayExtent -1;
}

// #################################################################################################
// Implementation of standard Boxer interfaces, IEquals, IIsNull(), IIsEmpty
// #################################################################################################

// We are faking all template specializations of namespace boxing for doxygen into namespace
// ftypes to keep documentation of namespace boxing clean!
#if !defined(DOX_PARSER)
namespace ftypes
{
#endif


/// Implements 'Equals' - interface for boxed \c double values.
class IEquals_Tdouble : public IEquals, public Singleton<IEquals_Tdouble>
{
    /**
     * Checks if \p{lhs} with boxed type \c double equals \p{rhs}. This is done by first comparing
     * the exact values. If they differ, the difference is calculated and compared to
     * <c> 2.0f * std::numeric_limits<float>::epsilon()</c>. If smaller, \c true is returned.
     *
     * @param lhs   The box that the interface was invoked on.
     * @param rhs   The boxed value to compare us to.
     * @return \c true if \p{comp} equals \p{lhs}, \c false otherwise.
     */
    ALIB_API
    virtual bool Invoke( const Box& lhs, const Box& rhs );
};


/// Implements 'Equals' - interface for boxed \c nchar[] values.
class IEquals_TncharArr : public IEquals, public Singleton<IEquals_TncharArr>
{
    /**
     * Checks if \p{lhs} with boxed type \c nchar[] equals \p{rhs}.
     *
     * @param lhs   The box that the interface was invoked on.
     * @param rhs   The boxed value to compare us to.
     * @return \c true if \p{comp} equals \p{lhs}, \c false otherwise.
     */
    ALIB_API
    virtual bool Invoke( const Box& lhs, const Box& rhs );
};

/// Implements 'Equals' - interface for boxed \c wchar[] values.
class IEquals_TwcharArr : public IEquals, public Singleton<IEquals_TwcharArr>
{
    /**
     * Checks if \p{lhs} with boxed type \c wchar[] equals \p{rhs}.
     *
     * @param lhs   The box that the interface was invoked on.
     * @param rhs   The boxed value to compare us to.
     * @return \c true if \p{comp} equals \p{lhs}, \c false otherwise.
     */
    ALIB_API
    virtual bool Invoke( const Box& lhs, const Box& rhs );
};

#if ALIB_MODULE_STRINGS
/// Implements 'Equals' - interface for boxed \c strangeChar[] values.
class IEquals_TstrangeCharArr : public IEquals, public Singleton<IEquals_TstrangeCharArr>
{
    /**
     * Checks if \p{lhs} with boxed type \c strangeChar[] equals \p{rhs}.
     *
     * @param lhs   The box that the interface was invoked on.
     * @param rhs   The boxed value to compare us to.
     * @return \c true if \p{comp} equals \p{lhs}, \c false otherwise.
     */
    ALIB_API
    virtual bool Invoke( const Box& lhs, const Box& rhs );
};

#endif


/// Implements 'IsLess' - interface for boxed \c boxed_uint values.
class IIsLess_boxed_uint : public IIsLess, public Singleton<IIsLess_boxed_uint>
{
    /**
     * Returns the result of the comparison for unsigned integer values.
     *
     * @param lhs   The box that the interface was invoked on.
     * @param rhs   The boxed value to compare us to.
     * @return \c true if \p{lhs} is smaller than \p{rhs}, \c false otherwise.
     */
    ALIB_API
    virtual bool Invoke( const Box& lhs, const Box& rhs );
};

/// Implements 'IsLess' - interface for boxed \c boxed_int values.
class IIsLess_boxed_int : public IIsLess, public Singleton<IIsLess_boxed_int>
{
    /**
     * Returns the result of the comparison for signed integer values.
     *
     * @param lhs   The box that the interface was invoked on.
     * @param rhs   The boxed value to compare us to.
     * @return \c true if \p{lhs} is smaller than \p{rhs}, \c false otherwise.
     */
    ALIB_API
    virtual bool Invoke( const Box& lhs, const Box& rhs );
};

/// Implements 'IsLess' - interface for boxed \c double values.
class IIsLess_double : public IIsLess, public Singleton<IIsLess_double>
{
    /**
     * Returns the result of the comparison for double values.
     *
     * @param lhs   The box that the interface was invoked on.
     * @param rhs   The boxed value to compare us to.
     * @return \c true if \p{lhs} is smaller than \p{rhs}, \c false otherwise.
     */
    ALIB_API
    virtual bool Invoke( const Box& lhs, const Box& rhs );
};



}}} // namespace aworx::[lib::boxing::ftypes]

/// Type alias in namespace #aworx.
using     boxed_int   =   aworx::lib::boxing::ftypes::boxed_int;

/// Type alias in namespace #aworx.
using     boxed_uint  =   aworx::lib::boxing::ftypes::boxed_uint;
} // namespace [aworx]

#endif // ALIB_FEAT_BOXING_FTYPES
#endif // HPP_ALIB_BOXING_FTYPES
