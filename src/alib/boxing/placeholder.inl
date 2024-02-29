/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_boxing of the \aliblong.
 *
 * \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_BOXING_PLACEHOLDER
#define HPP_ALIB_BOXING_PLACEHOLDER 1

#if !defined(HPP_ALIB_BOXING_BOXING)
#   error "ALib sources with ending '.inl' must not be included from outside."
#endif

#if !defined (_GLIBCXX_CSTRING) && !defined(_CSTRING_)
#   include <cstring>
#endif

namespace alib {  namespace boxing  { namespace detail {
/**
* Inner struct providing a pointer and a length.<br>
* A \c constexpr constructor is given (not shown in documentation).
*/
struct StructArray
{
    const void* Pointer; ///< The pointer to the array.
    integer     Length;  ///< The length of the array.

  // constexpr constructors
  #if !defined(ALIB_DOX)
    constexpr StructArray( const void* p, integer l) : Pointer( p ), Length ( l )      {}
  #endif
};

/**
 * Inner union with various \c constexpr constructors (not shown in documentation) to support
 * the initialization of the outer union as \c constexpr.
 *
 * Collects scalar integrals and arrays of those.
 */
union  UnionIntegrals
{
     int8_t          Int8   ;   ///<  8-bit signed integral.
    uint8_t         UInt8   ;   ///<  8-bit unsigned integral.
     int16_t         Int16  ;   ///< 16-bit signed integral.
    uint16_t        UInt16  ;   ///< 16-bit unsigned integral.

  #if defined(ALIB_DOX)
     int32_t         Int32  ;   ///< 32-bit signed integral.   Available only if platform is not of 32-bit.
    uint32_t        UInt32  ;   ///< 32-bit unsigned integral. Available only if platform is not of 32-bit.
     int64_t         Int64  ;   ///< 64-bit signed integral.   Available only if platform is not of 64-bit.
    uint64_t        UInt64  ;   ///< 64-bit unsigned integral. Available only if platform is not of 64-bit.
  #elif  ALIB_SIZEOF_INTEGER != 4
     int32_t         Int32  ;
    uint32_t        UInt32  ;
  #elif  ALIB_SIZEOF_INTEGER != 8
     int64_t         Int64  ;
    uint64_t        UInt64  ;
  #endif

     integer         Int    ;   ///< Signed integral of platform-dependent size.
    uinteger        UInt    ;   ///< Unsigned integral of platform-dependent size.

     int8_t          Array8  [2 * sizeof(uinteger) / sizeof( int8_t  )]; ///< Array of  8-bit signed integrals of length 16 on 64-bit platform, 8 on a 32-bit platform.
     int16_t         Array16 [2 * sizeof(uinteger) / sizeof( int16_t )]; ///< Array of 16-bit signed integrals of length  8 on 64-bit platform, 4 on a 32-bit platform.

  #if defined(ALIB_DOX)
     int32_t         Array32 [2 * sizeof(uinteger) / sizeof( int32_t )]; ///< Array of 32-bit signed integrals of length 4 on a 64-bit platform. Not available on 32-bit platforms.
     int64_t         Array64 [2 * sizeof(uinteger) / sizeof( int64_t )]; ///< Array of 64-bit signed integrals of length 1 on a 32-bit platform. Not available on 64-bit platforms.

  #elif  ALIB_SIZEOF_INTEGER != 4
     int32_t         Array32 [2 * sizeof(uinteger) / sizeof( int32_t )];
  #elif  ALIB_SIZEOF_INTEGER != 8
     int64_t         Array64 [2 * sizeof(uinteger) / sizeof( int64_t )];
  #endif

     integer          Array  [ 2                                      ]; ///< Array of 64-bit signed integrals of length  2 on 64-bit platform, 1 on a 32-bit platform.
    uinteger         UArray  [ 2                                      ]; ///< Array of 64-bit unsigned integrals of length  2 on 64-bit platform, 1 on a 32-bit platform.


  // constexpr constructors
  #if !defined(ALIB_DOX)
    constexpr UnionIntegrals     (  integer v1, integer v2 )  :  Array    { v1, v2 }  {}
    constexpr UnionIntegrals     (  int8_t     value )        :  Int8     { value }  {}
    constexpr UnionIntegrals     ( uint8_t     value )        : UInt8     { value }  {}
    constexpr UnionIntegrals     (  int16_t    value )        :  Int16    { value }  {}
    constexpr UnionIntegrals     ( uint16_t    value )        : UInt16    { value }  {}
  #if    ALIB_SIZEOF_INTEGER != 4
    constexpr UnionIntegrals     (  int32_t    value )        :  Int32    { value }  {}
    constexpr UnionIntegrals     ( uint32_t    value )        : UInt32    { value }  {}
  #elif  ALIB_SIZEOF_INTEGER != 8
    constexpr UnionIntegrals     (  int64_t    value )        :  Int64    { value }  {}
    constexpr UnionIntegrals     ( uint64_t    value )        : UInt64    { value }  {}
  #endif
    constexpr UnionIntegrals     (  integer    value )        :  Int      { value }  {}
    constexpr UnionIntegrals     ( uinteger    value )        : UInt      { value }  {}



    #if   ALIB_SIZEOF_INTGAP == 2
    constexpr UnionIntegrals     (  intGap_t   value )  :  Int16    { value }  {}
    constexpr UnionIntegrals     ( uintGap_t   value )  : UInt16    { value }  {}
    #elif ALIB_SIZEOF_INTGAP == 4
        #if  ALIB_SIZEOF_INTEGER != 4
        constexpr UnionIntegrals     (  intGap_t   value )  :  Int32    { value }  {}
        constexpr UnionIntegrals     ( uintGap_t   value )  : UInt32    { value }  {}
        #else
        constexpr UnionIntegrals     (  intGap_t   value )  :  Int      { value }  {}
        constexpr UnionIntegrals     ( uintGap_t   value )  : UInt      { value }  {}
        #endif
    #elif ALIB_SIZEOF_INTGAP == 8
        #if  ALIB_SIZEOF_INTEGER != 8
        constexpr UnionIntegrals     (  intGap_t   value )  :  Int64    { value }  {}
        constexpr UnionIntegrals     ( uintGap_t   value )  : UInt64    { value }  {}
        #else
        constexpr UnionIntegrals     (  intGap_t   value )  :  Int      { value }  {}
        constexpr UnionIntegrals     ( uintGap_t   value )  : UInt      { value }  {}
        #endif
    #else
        #error "ALIB_SIZEOF_INTGAP not matched. Supported sizes are 2, 4 and 8."
    #endif
  #endif
};

/**
 * Inner union with various \c constexpr constructors (not shown in documentation) to support
 * the initialization of the outer union as \c constexpr.
 *
 * Collects scalar floating points and arrays of those.
 */
union  UnionFloatingPoints
{
    float           Float  ;   ///< A \c float value.
    double          Double ;   ///< A \c double value.
#if (ALIB_SIZEOF_LONGDOUBLE_REPORTED <= 2 * ALIB_SIZEOF_INTEGER ) || defined(ALIB_DOX)
    long double     LDouble;   ///< A <c>long double</c> value. Available only if sizeof(long double) is smaller or equal than 2 x sizeof(integer), which is for example not true with GCC on Linux 32 -bit.
#endif

    float           FloatArray  [2 * sizeof(uinteger) / sizeof(float      )]; ///< Array of \c float.  The Length is usually  4 on 64-bit platform, 2 on a 32-bit platform.
    double          DoubleArray [2 * sizeof(uinteger) / sizeof(double     )]; ///< Array of \c double. The Length is usually 2 on 64-bit platform, 1 on a 32-bit platform.

  #if !defined(ALIB_DOX)
    constexpr UnionFloatingPoints( float       value )     : Float     { value }  {}
    constexpr UnionFloatingPoints( double      value )     : Double    { value }  {}
#if (ALIB_SIZEOF_LONGDOUBLE_REPORTED <= 2 * ALIB_SIZEOF_INTEGER ) || defined(ALIB_DOX)
    constexpr UnionFloatingPoints( long double value )     : LDouble   { value }  {}
#endif
  #endif
};


/**
 * Inner union with various \c constexpr constructors (not shown in documentation) to support
 * the initialization of the outer union as \c constexpr.
 *
 * Collects a \c void and a \c char pointer and arrays of those.<br>
 * In addition, non-pointer field #Memory (type \c char) is given, which can be be used to
 * receive a pointer to the start of the placeholder by applying <c>operator&</c>.
 * This \c char* then can be reinterpreted without braking the
 * \https{strict-aliasing rule,stackoverflow.com/questions/98650/what-is-the-strict-aliasing-rule}
 * when compiling the code with higher optimization levels.
 */
union  UnionPointers
{
           void*    Void;         ///< A \c void pointer.
    const  void*   CVoid;         ///< A constant \c void pointer.
           char*    Char;         ///< A \c void pointer.
    const  char*   CChar;         ///< A constant \c void pointer.
          wchar*   WChar;         ///< A \c wchar pointer.
    const wchar*  CWChar;         ///< A constant \c wchar pointer.

           void*    VoidArray[2]; ///< Array of <c>void*</c> of length \c 2.
    const  void*   CVoidArray[2]; ///< Array of <c>const void*</c> of length \c 2.
           char*    CharArray[2]; ///< Array of <c>char*</c> of length \c 2.
    const  char*   CCharArray[2]; ///< Array of <c>const char*</c> of length \c 2.
          wchar*   WCharArray[2]; ///< Array of <c>wchar*</c> of length \c 2.
    const wchar*  CWCharArray[2]; ///< Array of <c>const wchar*</c> of length \c 2.

    char  Memory;                 ///< Not a pointer but becomes one with applying <c>operator&</c>.

  #if !defined(ALIB_DOX)
    constexpr UnionPointers(        void*  value )   :   Void   { value }  {}
    constexpr UnionPointers( const  void*  value )   :  CVoid   { value }  {}
    constexpr UnionPointers(        char*  value )   :   Char   { value }  {}
    constexpr UnionPointers( const  char*  value )   :  CChar   { value }  {}
    constexpr UnionPointers(       wchar*  value )   :  WChar   { value }  {}
    constexpr UnionPointers( const wchar*  value )   : CWChar   { value }  {}

    constexpr UnionPointers( const  void*  v1, const  void* v2 ) :   CVoidArray { v1, v2 }  {}
    constexpr UnionPointers( const  char*  v1, const  char* v2 ) :   CCharArray { v1, v2 }  {}
    constexpr UnionPointers( const wchar*  v1, const wchar* v2 ) :  CWCharArray { v1, v2 }  {}
  #endif
};

} // namespace alib::boxing[::detail]

/** ************************************************************************************************
 * A \alib{boxing,Box::data,protected member of this union} is contained in class
 * \alib{boxing,Box} to store a boxed object.
 * This member is passed as an argument to static methods \b Write and \b Read of type traits
 * struct \alib{boxing,T_Boxer}, which implement boxing and unboxing.
 *
 * This union declares different inner structs and unions and contains one corresponding member of
 * each. This sorts the union fields into different groups, which his also helpful when
 * debugging instances of type box.
 *
 * The overall size of this union is two times the size of \c std::size_t, hence 16 bytes on a
 * 64-bit and 8 bytes on a 32-bit system.
 *
 * Virtually any sort of data might be written into the union. With non-injective boxing, what means
 * that two or more types are boxed to the same target type, the format that type uses has to be
 * implemented by all \b Write and \b Read methods of any specialization of \alib{boxing,T_Boxer}.
 * Otherwise, undefined behavior occurs.
 *
 * This type offers two sets of templated overloaded methods, named \b %Write and \b Read.
 * In addition to be overloaded, the methods use TMP to be selected by the compiler only for
 * certain template types.<br>
 * The methods cover boxing and unboxing of the most frequent types like
 * - fundamental types,
 * - "fitting" value types that are copy assignable,
 * - "fitting" value types that are not copy assignable,
 * - pointers and
 * - arrays (only boxing).
 *
 * For these type, the default implementation of \b T_Boxer::Write and \b Read, as given
 * for example with macro \ref ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING.
 *
 * ####Custom Boxing####
 * Custom implementations of boxing and unboxing may read from and write to the union data
 * directly.<br>
 * In the latter case, a "continuous" use of the available data is suggested. Aat least, gaps
 * in writing should be initialized with a value (e.g. \c 0). The rational for this is
 * that the default implementations of box-functions \alib{boxing,FHashcode} and
 * \alib{boxing,FEquals} use only the first \e N relevant bytes. If now, gaps are not written,
 * they contain "random" data, what would cause a failure of these default functions.<br>
 *
 * On the same token, if a customization of a non-array type writes a different length then
 * C++ operator \c sizeof reports for the mapped type, then also struct TMP
 * \alib{boxing,T_SizeInPlaceholder} has to be specialized for that type, so that method
 * \alib{boxing,Box::GetPlaceholderUsageLength} reports the right value.
 * Not that also method #Clear, used when boxing \e nulled pointers, only clears as much
 * data in this struct as reported by \alib{boxing,T_SizeInPlaceholder}.
 *
 * ####Constexpr Boxing####
 * As explained in chapter \ref alib_boxing_more_opt_constexpr, method
 * \alib{boxing,T_Boxer::Write} may be defined with a different signature that returns a
 * value of this union, instead of receiving one to be filled. The aim is to allowing
 * \c constexpr instances of class \b Box to be created from likewise \c constexpr values.
 * The C++ language rules for \c constexpr constructors impose some restrictions. No reinterpret
 * casts have to be performed. Furthermore, an union constructor must not initialize a field of an
 * inner union by accessing it using the dot operator. Instead, the inner unions need to provide
 * \c constexpr constructors that are called for the field of the outer union.
 *
 * While this library defines <c>constexpr</c>-boxing for all fundamental types and for most
 * library types of other modules (if it provides customized boxing for), still such customization
 * is considered "expert use" as the gain to do it for custom types is marginal.
 * For this reason, the constructors of inner structs and unions, and all reflection of those
 * with a corresponding constructor of this outer union are \b not documented.
 * This is to keep the documentation lean and non-repetitive. Therefore, for reference
 * of the \c constexpr constructors available for this union, please consult the
 * source code, found in file \b alib/boxing/placeholder.inl.
 *
 * \note
 *   Unions
 *   \alib{boxing::detail,StructArray},
 *   \alib{boxing::detail,UnionIntegrals},
 *   \alib{boxing::detail,UnionFloatingPoints} and
 *   \alib{boxing::detail,UnionPointers} are defined in the \b detail namespace instead of being
 *   inner types. This is due to a restriction of the \b MSC compiler.
 *
 * \see
 *   Chapter \ref alib_boxing_customizing "7. Customizing Boxing" of the Programmer's Manual of
 *   \alib_boxing_nl.<br>
 *   Furthermore structs \alib{boxing,T_Boxer} and \alib{boxing,T_SizeInPlaceholder}.
 * ************************************************************************************************/
union Placeholder
{
    detail::UnionPointers       Pointers;         ///< Collection of \c void, \c char and \b character pointers.
    detail::StructArray         Array;            ///< Used when storing C++ arrays.
    detail::UnionIntegrals      Integrals;        ///< Collection of integrals of different sizes.
    detail::UnionFloatingPoints FloatingPoints;   ///< Collection of floating points of different sizes.

    #if ALIB_DEBUG
        character*              Debugger_String;   ///< This union field was inserted only for debug display.
        integer                 Debugger_Integral; ///< This union field was inserted only for debug display.
    #endif


    // constexpr constructors
    #if !defined(ALIB_DOX)

    // This unnecessary default initialization with default constructor will be optimized out.
    constexpr Placeholder()                                       : Integrals(0)           {}

    // Pointer construction
    constexpr Placeholder(        void* p                   )     : Pointers( p )          {}
    constexpr Placeholder( const  void* p                   )     : Pointers( p )          {}
    constexpr Placeholder(        char* p                   )     : Pointers( p )          {}
    constexpr Placeholder( const  char* p                   )     : Pointers( p )          {}
    constexpr Placeholder(       wchar* p                   )     : Pointers( p )          {}
    constexpr Placeholder( const wchar* p                   )     : Pointers( p )          {}
    constexpr Placeholder( const  void* p1, const  void* p2 )     : Pointers( p1, p2 )     {}
    constexpr Placeholder(        char* p1,        char* p2 )     : Pointers( p1, p2 )     {}
    constexpr Placeholder( const  char* p1, const  char* p2 )     : Pointers( p1, p2 )     {}
    constexpr Placeholder(       wchar* p1,       wchar* p2 )     : Pointers( p1, p2 )     {}
    constexpr Placeholder( const wchar* p1, const wchar* p2 )     : Pointers( p1, p2 )     {}

    template<typename TMapped>
    constexpr Placeholder( const TMapped* p )  : Pointers( p )    {}

    // Integral construction
    constexpr Placeholder(  int8_t     value )  : Integrals      ( value )  {}
    constexpr Placeholder(  int16_t    value )  : Integrals      ( value )  {}
    constexpr Placeholder(  int32_t    value )  : Integrals      ( value )  {}
    constexpr Placeholder(  int64_t    value )  : Integrals      ( value )  {}
    constexpr Placeholder(  intGap_t   value )  : Integrals      ( value )  {}
    constexpr Placeholder( uint8_t     value )  : Integrals      ( value )  {}
    constexpr Placeholder( uint16_t    value )  : Integrals      ( value )  {}
    constexpr Placeholder( uint32_t    value )  : Integrals      ( value )  {}
    constexpr Placeholder( uint64_t    value )  : Integrals      ( value )  {}
    constexpr Placeholder( uintGap_t   value )  : Integrals      ( value )  {}
    constexpr Placeholder( integer word1, integer word2 ) : Integrals( word1, word2 )      {}

    // Float construction
    constexpr Placeholder( float       value )  : FloatingPoints ( value )  {}
    constexpr Placeholder( double      value )  : FloatingPoints ( value )  {}
#if ALIB_SIZEOF_LONGDOUBLE_REPORTED <= 2 * ALIB_SIZEOF_INTEGER
    constexpr Placeholder( long double value )  : FloatingPoints ( value )  {}
#endif

    // Array construction
    template<typename TArray>
    constexpr Placeholder( const TArray* tpointer, integer length ) : Array( tpointer, length ) {}

    #endif

    /**
     * Returns a pointer of type \p{TReturn}.
     * @tparam TReturn The requested pointer type
     * @return The pointer stored.
     */
    template<typename TReturn>
    constexpr TReturn*   Pointer    ()                    const { return reinterpret_cast<TReturn*>( Pointers.Char); }

    /**
     * Returns a pointer of type \c void*.
     * @return The pointer stored.
     */
    constexpr void*      VoidPointer()                    const { return Pointers.Void;         }

    /**
     * Sets a pointer of type \c char*.
     * @param value The value to set.
     */
    constexpr
    void                        Pointer    ( const char* value )       { Pointers.CChar= value;        }

    /**
     * Sets a pointer of type \c void*.
     * @param value The value to set.
     */
    constexpr
    void                        VoidPointer( const void* value )       { Pointers.CVoid= value;        }

    /**
     * Returns the length of a stored array (the second word stored).
     * @return The length stored.
     */
    constexpr integer     Length ()                       const { return GetInteger(1);  }

    /**
     * Returns the length of a stored array (the second word stored).
     * @return The length stored.
     */
    constexpr uinteger   ULength ()                       const { return GetUInteger(1); }

    ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
    /**
     * Returns the signed integral at index \p{idx}.
     * @param idx The index requested. Only \c 0 and \c 1 is allowed.
     * @return The integral value one or two.
     */
    constexpr  integer   GetInteger( int idx )            const { return Integrals.Array [idx];  }

    /**
     * Sets \p{value} at index \p{idx}.
     * @param idx   The index to store \p{value} in. Only \c 0 and \c 1 is allowed.
     * @param value The value to set.
     */
    constexpr
    void                        SetInteger( int idx, integer value )
    {
        Integrals.Array [idx]= value;
    }

    /**
     * Returns the unsigned integral at index \p{idx}.
     * @param idx The index requested. Only \c 0 and \c 1 is allowed.
     * @return The integral value one or two.
     */
    constexpr uinteger   GetUInteger( int idx )          const { return Integrals.UArray[idx];  }

    /**
     * Sets \p{value} at index \p{idx}.
     * @param idx   The index to store \p{value} in. Only \c 0 and \c 1 is allowed.
     * @param value The value to set.
     */
    constexpr
    void                        SetUInteger( int idx, uinteger value )
    {
        Integrals.UArray[idx]= value;
    }
    ALIB_WARNINGS_RESTORE


    /** ********************************************************************************************
     * Clears this box data.
     *
     * It has to be assured that all memory used by a mapped type are cleared.
     * For example, the default implementations of box-functions \alib{boxing,FHashcode} and
     * \alib{boxing,FEquals} are using the relevant bytes of this placeholder and those must
     * not be of random value.
     *
     * For efficiency reasons, the rest should not be cleared.
     *
     * @tparam UsageLength The number of bytes to clear.
     * ********************************************************************************************/
    template<unsigned int UsageLength>
    constexpr void    Clear()
    {
        static_assert( UsageLength > 0 && ( UsageLength <= 2 * sizeof(uinteger) ),
                       "Invalid usage length given" );

        Integrals.Array[0]= 0;
        ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
        if constexpr( UsageLength > sizeof(integer) )
            Integrals.Array[1]= 0;
        ALIB_WARNINGS_RESTORE
    }

    // #############################################################################################
    // #########################             Boxing            #####################################
    // #############################################################################################

    #if defined(ALIB_DOX)

    /** ********************************************************************************************
     * This version of the overloaded methods is selected using TMP with most types, namly
     * fundamental, pointers and those value types that are copy assignable.
     *
     * The type is stored at the start of the data, by reinterpreting the address of field
     * \alib{boxing::detail,UnionPointers::Memory} to a pointer to \p{TMapped} and using the
     * C++ assign operator on that dereferenced pointer.
     *
     * @tparam  TMapped  The mapped type to store.
     * @param   value    The value of type \p{TMapped} to store.
     * ********************************************************************************************/
    template<typename TMapped>
    inline void Write( const TMapped& value);
    #else

    template<typename TMapped>
    ALIB_FORCE_INLINE
    ATMP_VOID_IF(     std::is_copy_assignable<TMapped>::value
                         && (sizeof(TMapped) <= 2 * sizeof(uinteger))    )
    Write( const TMapped& value)
    {
        *reinterpret_cast<TMapped*>( &Pointers ) = value;
    }

    #endif

    #if defined(ALIB_DOX)
    /** ********************************************************************************************
     * This version of the overloaded methods is selected using TMP with types boxed as values
     * that are not copy-assignbable.
     *
     * The copying is performed using \c memcpy.
     * This is necessary to avoid de-referencing type-punned pointers which would
     * break the strict-aliasing rule when compiling the code with higher optimization levels.
     * Note that modern compilers like GCC usually optimize the invocation of \c memcpy out.<br>
     *
     * @tparam  TMapped  The mapped type to store.
     * @param   value    The value of type \p{TMapped} to store.
     * ********************************************************************************************/
    template<typename TMapped>
    inline  void Write( const TMapped& value);
    #else

    template<typename TMapped>
    ALIB_FORCE_INLINE
    ATMP_VOID_IF(    !std::is_copy_assignable<TMapped>::value
                  && (sizeof(TMapped) <= 2 * sizeof(uinteger))    )
    Write( const TMapped& value)
    {
        std::memcpy( &Pointers.Memory, &value, sizeof(TMapped) );
    }

    #endif


    #if defined(ALIB_DOX)
    /** ********************************************************************************************
     * This version of the overloaded methods is used for boxing C++ array types.
     * The type and length of the array is stored in field #Array of the data union.
     *
     * Note that for unboxing custom types from C++ array types, a custom implementation of
     * \alib{boxing,T_Boxer::Read} is needed. Such implementation reads the pointer and length
     * directly from this struct.
     * (I.e. there is no overloaded method \b Read available for arrays.)
     *
     * @tparam  TArray  The templated pointer type.
     * @param   pointer The pointer to store.
     * @param   length  The array's length to store.
     * ********************************************************************************************/
    template<typename TArray>
    inline constexpr
    void Write( const TArray* pointer, integer length );
    #else

    template<typename TArray>
    ALIB_FORCE_INLINE constexpr
    void Write( const TArray* pointer, integer length )
    {
        Array.Pointer =  pointer;
        Array.Length  =  length;
    }
    #endif


    // #############################################################################################
    // #########################            Unboxing           #####################################
    // #############################################################################################

    /** ********************************************************************************************
     * Templated read method for value types.
     * The value is dereferenced from the start of the placeholder memory.
     *
     * @tparam  TMapped  The value type to unbox.
     * @return  The value stored.
     * ********************************************************************************************/
    template<typename TMapped>
    ALIB_FORCE_INLINE constexpr
    TMapped Read()                                                                             const
    {
        return *reinterpret_cast<const TMapped*>( &Pointers.Memory );
    }


}; // union Placeholder

static_assert( sizeof(Placeholder) == 2 * sizeof(std::size_t),
               "Size of boxing::Placeholder is not two times the size of 'size_t'. "
               "Compilation platform not supported." );

}} // namespace [alib::boxing]

#endif // HPP_ALIB_BOXING_PLACEHOLDER
