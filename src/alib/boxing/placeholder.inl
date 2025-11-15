//==================================================================================================
/// \file
/// This header-file is part of module \alib_boxing of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib {  namespace boxing  { namespace detail {


/// Inner struct providing a pointer and a length.<br>
/// A \c constexpr constructor is given (not shown in documentation).
struct PointerPair
{
    const void* P1; ///< The first pointer.
    const void* P2; ///< The second pointer.

  // constexpr constructors
  #if !DOXYGEN
    constexpr PointerPair( const void* p1)                 : P1(p1), P2(nullptr)                  {}
    constexpr PointerPair( const void* p1, const void* p2) : P1(p1), P2(p2)                       {}
  #endif
};

/// Inner struct providing a pointer and a length.<br>
/// A \c constexpr constructor is given (not shown in documentation).
struct PointerPairMutable
{
    void* P1; ///< The first pointer.
    void* P2; ///< The second pointer.

    /// Constructor allowing \c constexpr initialization.
    /// @param p1 The pointer to store.
    constexpr PointerPairMutable( void* p1)                               : P1(p1), P2(nullptr)   {}

    /// Constructor allowing \c constexpr initialization.
    /// @param p1 The first value to store.
    /// @param p2 The second pointer to store.
    constexpr PointerPairMutable( void* p1, void* p2)                     : P1(p1), P2(p2)        {}
};

/// Inner struct providing a pointer and a length.<br>
/// A \c constexpr constructor is given (not shown in documentation).
struct StructArray
{
    const void* Pointer; ///< The pointer to the array.
    integer     Length;  ///< The length of the array.

    /// Constructor allowing \c constexpr initialization.
    /// @param p  The pointer to the array.
    /// @param l  The length of the array.
    constexpr StructArray( const void* p, integer l)               : Pointer( p ), Length ( l )   {}
};

/// Inner union with various \c constexpr constructors (not shown in documentation) to support
/// the initialization of the outer union as \c constexpr.
///
/// Collects scalar integrals and arrays of those.
union  UnionIntegrals
{
      int8_t         Int8   ;   ///< 8-bit signed integral.
     uint8_t        UInt8   ;   ///< 8-bit unsigned integral.
     int16_t         Int16  ;   ///< 16-bit signed integral.
    uint16_t        UInt16  ;   ///< 16-bit unsigned integral.

  #if DOXYGEN
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

     int8_t          Array8  [2 * sizeof(void*) / sizeof( int8_t  )]; ///< Array of 8-bit signed integrals of length 16 on 64-bit platform, 8 on a 32-bit platform.
     int16_t         Array16 [2 * sizeof(void*) / sizeof( int16_t )]; ///< Array of 16-bit signed integrals of length 8 on 64-bit platform, 4 on a 32-bit platform.

  #if DOXYGEN
     int32_t         Array32 [2 * sizeof(void*) / sizeof( int32_t )]; ///< Array of 32-bit signed integrals of length 4 on a 64-bit platform. Not available on 32-bit platforms.
     int64_t         Array64 [2 * sizeof(void*) / sizeof( int64_t )]; ///< Array of 64-bit signed integrals of length 1 on a 32-bit platform. Not available on 64-bit platforms.

  #elif  ALIB_SIZEOF_INTEGER != 4
     int32_t         Array32 [2 * sizeof(void*) / sizeof( int32_t )];
  #elif  ALIB_SIZEOF_INTEGER != 8
     int64_t         Array64 [2 * sizeof(void*) / sizeof( int64_t )];
  #endif

     integer          Array  [ 2                                   ]; ///< Array of 64-bit signed integrals of length two on 64-bit platform, one on a 32-bit platform.
    uinteger         UArray  [ 2                                   ]; ///< Array of 64-bit unsigned integrals of length two on 64-bit platform, one on a 32-bit platform.


    /// Constructor allowing \c constexpr initialization.
    /// @param v1 The first value to store.
    /// @param v2 The second value to store.
    constexpr UnionIntegrals     (  integer v1, integer v2              )  :  Array    { v1, v2 } {}

    /// Constructor allowing \c constexpr initialization.
    /// @param value The value to store.
    constexpr UnionIntegrals     (  int8_t     value )                     :  Int8     { value }  {}

    /// Constructor allowing \c constexpr initialization.
    /// @param value The value to store.
    constexpr UnionIntegrals     ( uint8_t     value )                     : UInt8     { value }  {}

    /// Constructor allowing \c constexpr initialization.
    /// @param value The value to store.
    constexpr UnionIntegrals     (  int16_t    value )                     :  Int16    { value }  {}

    /// Constructor allowing \c constexpr initialization.
    /// @param value The value to store.
    constexpr UnionIntegrals     ( uint16_t    value )                     : UInt16    { value }  {}
  #if    ALIB_SIZEOF_INTEGER != 4

    /// Constructor allowing \c constexpr initialization.
    /// @param value The value to store.
    constexpr UnionIntegrals     (  int32_t    value )                     :  Int32    { value }  {}

    /// Constructor allowing \c constexpr initialization.
    /// @param value The value to store.
    constexpr UnionIntegrals     ( uint32_t    value )                     : UInt32    { value }  {}
  #elif  ALIB_SIZEOF_INTEGER != 8
    constexpr UnionIntegrals     (  int64_t    value )                     :  Int64    { value }  {}
    constexpr UnionIntegrals     ( uint64_t    value )                     : UInt64    { value }  {}
  #endif
    /// Constructor allowing \c constexpr initialization.
    /// @param value The value to store.
    constexpr UnionIntegrals     (  integer    value )                     :  Int      { value }  {}

    /// Constructor allowing \c constexpr initialization.
    /// @param value The value to store.
    constexpr UnionIntegrals     ( uinteger    value )                     : UInt      { value }  {}


  #if   ALIB_SIZEOF_INTGAP == 2
    /// Constructor allowing \c constexpr initialization.
    /// @param value The value to store.
    constexpr UnionIntegrals     (  intGap_t   value )                   :  Int16    { value }  {}
    /// Constructor allowing \c constexpr initialization.
    /// @param value The value to store.
    constexpr UnionIntegrals     ( uintGap_t   value )                   : UInt16    { value }  {}
  #elif ALIB_SIZEOF_INTGAP == 4
    #if  ALIB_SIZEOF_INTEGER != 4
    /// Constructor allowing \c constexpr initialization.
    /// @param value The value to store.
    constexpr UnionIntegrals     (  intGap_t   value )                   :  Int32    { value }  {}
    /// Constructor allowing \c constexpr initialization.
    /// @param value The value to store.
    constexpr UnionIntegrals     ( uintGap_t   value )                   : UInt32    { value }  {}
    #else
    /// Constructor allowing \c constexpr initialization.
    /// @param value The value to store.
    constexpr UnionIntegrals     (  intGap_t   value )                   :  Int      { value }  {}
    /// Constructor allowing \c constexpr initialization.
    /// @param value The value to store.
    constexpr UnionIntegrals     ( uintGap_t   value )                   : UInt      { value }  {}
    #endif
  #elif ALIB_SIZEOF_INTGAP == 8
    #if  ALIB_SIZEOF_INTEGER != 8
    constexpr UnionIntegrals     (  intGap_t   value )                   :  Int64    { value }  {}
    constexpr UnionIntegrals     ( uintGap_t   value )                   : UInt64    { value }  {}
    #else
    /// Constructor allowing \c constexpr initialization.
    /// @param value The value to store.
    constexpr UnionIntegrals     (  intGap_t   value )                   :  Int      { value }    {}
    /// Constructor allowing \c constexpr initialization.
    /// @param value The value to store.
    constexpr UnionIntegrals     ( uintGap_t   value )                   : UInt      { value }    {}
    #endif
  #else
    #error "ALIB_SIZEOF_INTGAP not matched. Supported sizes are 2, 4 and 8."
  #endif
};

/// Inner union with various \c constexpr constructors (not shown in documentation) to support
/// the initialization of the outer union as \c constexpr.
///
/// Collects scalar floating points and arrays of those.
union  UnionFloatingPoints
{
    float           Float  ;   ///< A \c float value.
    double          Double ;   ///< A \c double value.

    /// Array of \c float. The Length is usually four on 64-bit platform, two on a 32-bit platform.
    float           FloatArray  [2 * sizeof(void*) / sizeof(float      )];

    /// Array of \c double. The Length is usually two on 64-bit platform, one on a 32-bit platform.
    double          DoubleArray [2 * sizeof(void*) / sizeof(double     )];

    /// Constructor allowing \c constexpr initialization.
    /// @param value The value to store.
    constexpr UnionFloatingPoints( float       value )     : Float     { value }                  {}
    /// Constructor allowing \c constexpr initialization.
    /// @param value The value to store.
    constexpr UnionFloatingPoints( double      value )     : Double    { value }                  {}
};



/// Collects byte arrays of each possible size. This is used with overloaded method
/// \alib{boxing;Placeholder::Write} that leverages C++20 function \c std::bit_cast to
/// perform \c constexpr boxing.<br>
/// Consequently, this union does not provide \c constexpr constructors.
union  UnionBytes
{
    std::array<char,  1>   C1;  ///< 1 bytes.
    std::array<char,  2>   C2;  ///< 2 bytes.
    std::array<char,  3>   C3;  ///< 3 bytes.
    std::array<char,  4>   C4;  ///< 4 bytes.
    std::array<char,  5>   C5;  ///< 5 bytes.
    std::array<char,  6>   C6;  ///< 6 bytes.
    std::array<char,  7>   C7;  ///< 7 bytes.
    std::array<char,  8>   C8;  ///< 8 bytes.
    #if  ALIB_SIZEOF_INTEGER == 8
    std::array<char,  9>   C9;  ///< 9 bytes.
    std::array<char, 10>  C10;  ///< 10 bytes.
    std::array<char, 11>  C11;  ///< 11 bytes.
    std::array<char, 12>  C12;  ///< 12 bytes.
    std::array<char, 13>  C13;  ///< 13 bytes.
    std::array<char, 14>  C14;  ///< 14 bytes.
    std::array<char, 15>  C15;  ///< 15 bytes.
    std::array<char, 16>  C16;  ///< 16 bytes.
    #endif
};


} // namespace alib::boxing[::detail]

//==================================================================================================
/// A \alib{boxing;Box::data;protected member} of this union is contained in class
/// \alib{boxing;Box} to store information on a boxed object.
/// This member is passed as an argument to static methods \b Write and \b Read of type-traits
/// struct \alib{boxing;BoxTraits}, which implement boxing and unboxing.
///
/// This union declares different inner structs and unions and contains one corresponding member of
/// each. This sorts the union fields into different groups, which his also helpful when
/// debugging instances of type box.
///
/// The overall size of this union is two times the size of \c std::size_t, hence 16 bytes on a
/// 64-bit and 8 bytes on a 32-bit system.
///
/// Virtually any sort of data might be written into the union. With non-injective boxing, what
/// means that two or more types are boxed to the same target type, the format that type uses
/// has to be implemented by all \b Write and \b Read methods of any specialization of
/// \alib{boxing;BoxTraits}. Otherwise, undefined behavior occurs.
///
/// This type offers two sets of templated overloaded methods, named \b %Write and \b Read.
/// In addition to be overloaded, the methods use C++20 concepts to be selected by the
/// compiler only for certain template types.<br>
/// The methods cover boxing and unboxing of the most frequent types like
/// - fundamental types,
/// - "fitting" value types that are trivially copyable,
/// - "fitting" value types that are not trivially copyable (not \c constexpr)
/// - pointers and
/// - arrays.
///
/// For these types, the default implementation of \b BoxTraits::Write and \b Read, as given
/// for example, with macro \ref ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING often is all that is needed.
///
/// ####Custom Boxing####
/// Custom implementations of boxing and unboxing may read from and write to the union data
/// directly.<br>
/// In this case, a "continuous" use of the available data is suggested. At least, gaps
/// in writing should be initialized with a value (e.g., \c 0). The rationale for this is
/// that the default implementations of box-functions \alib{boxing;FHashcode} and
/// \alib{boxing;FEquals} use only the first \e N relevant bytes. If now, gaps are not written,
/// they contain "random" data, what would cause a failure of these default functions.<br>
///
/// By the same token, if the customization of a non-array type writes a different length than
/// C++ operator \c sizeof reports for the mapped type, then also \alib{boxing;SizeTraits} has
/// to be specialized for that type, so that the method
/// \alib{boxing;Box::GetPlaceholderUsageLength} reports the right value.
/// Note that furthermore method #Clear, which is used when boxing \e nulled pointers, only
/// clears as much data in this struct as reported by \alib{boxing;SizeTraits}.
///
/// ####Constexpr Boxing####
/// While this library defines <c>constexpr</c>-boxing for all fundamental types and for most
/// library types of other \alibmods, still such customization is considered "expert use" as the
/// gain to do it for custom types is marginal. The biggest benefit of \c constexpr boxing,
/// lies in the fact that - if also a customized VTable is provided - such types can be located
/// in global and static instances of class \b Box.
///
/// \see
///   - Chapter \ref alib_boxing_customizing "7. Customizing Boxing" of the Programmer's Manual of
///   \alib_boxing_nl.<br>
///   - Struct \alib{boxing;BoxTraits} and expression \alib{boxing;SizeTraits}.
//==================================================================================================
union Placeholder
{
    detail::PointerPair         PointerPair;        ///< Collection of two \c const \c void pointers.
    detail::PointerPairMutable  PointerPairMutable; ///< Collection of two \c void pointers.
    detail::StructArray         Array;              ///< Used when storing C++ arrays.
    detail::UnionIntegrals      Integrals;          ///< Collection of integrals of different sizes, placed next to each other.
    detail::UnionFloatingPoints FloatingPoints;     ///< Collection of floating points of different sizes.
    detail::UnionBytes          Bytes;              ///< Byte arrays of different length.
    void*                       VoidP;              ///< Just a void pointer.
    #if ALIB_DEBUG
    character*              Debugger_String;   ///< This union field was inserted only for debug display.
    integer                 Debugger_Integral; ///< This union field was inserted only for debug display.
    #endif

    /// Default constructor. Leaves everything uninitialized.
    constexpr Placeholder()                                                                       {}

    // constexpr constructors
    #if !DOXYGEN

    // Pointer construction
    template<typename TPointer>
    constexpr Placeholder( const TPointer* p )  : PointerPair( p )                                {}

    template<typename TP1, typename TP2>
    constexpr Placeholder( const TP1* p1, const TP2* p2 )  : PointerPair( p1, p2 )                {}

    // Integral construction
    template<typename TI>
    requires std::is_integral_v<TI>
    constexpr Placeholder(  TI     value )  : Integrals      ( value )                            {}

    template<typename TI1, typename TI2>
    requires (std::is_integral_v<TI1> && std::is_integral_v<TI2> )
    constexpr Placeholder( TI1 word1, TI2 word2 ) : Integrals( word1, word2 )                     {}

    // Float construction
    constexpr Placeholder( float       value )  : FloatingPoints ( value )                        {}
    constexpr Placeholder( double      value )  : FloatingPoints ( value )                        {}

    // Array construction
    template<typename TArray, typename TIntegral>
    requires std::is_integral_v<TIntegral>
    constexpr Placeholder( const TArray* tpointer, TIntegral length )
              : Array( tpointer, integer(length))                                                 {}

    #endif

    /// Returns a pointer of type \c void*.
    /// @return The pointer stored.
    constexpr const void*     GetVoidPointer()                               const { return VoidP; }

    /// Returns a pointer of type \p{TReturn}.
    /// @tparam TReturn The requested pointer type
    /// @return The pointer stored.
    template<typename TReturn>
    TReturn*  GetPointer    ()   const { return reinterpret_cast<TReturn*>(PointerPairMutable.P1); }

    /// Returns a pointer of type \p{TReturn}.
    /// @tparam TReturn The requested pointer type
    /// @return The pointer stored.
    template<typename TReturn>
    TReturn*  GetPointer2   ()   const { return reinterpret_cast<TReturn*>(PointerPairMutable.P2); }

    /// Sets a pointer of type \c char*.
    /// @param value The value to set.
    constexpr void            SetPointer    ( void* value )               { PointerPair.P1= value; }

    /// Returns the length of a stored array (the second word stored).
    /// @return The length stored.
    constexpr integer         GetLength ()                            const { return Array.Length; }

    /// Clears this box data.
    ///
    /// It has to be ensured that all the memory used by a mapped type is cleared.
    /// For example, the default implementations of box-functions \alib{boxing;FHashcode} and
    /// \alib{boxing;FEquals} are using the relevant bytes of this placeholder, and those must
    /// not be of random value.
    ///
    /// For efficiency reasons, the rest should not be cleared.
    ///
    /// @tparam UsageLength The number of bytes to clear.
    template<unsigned UsageLength>
    constexpr void    Clear() {
        static_assert( UsageLength > 0 && ( UsageLength <= 2 * sizeof(void*) ),
                       "Invalid usage length given" );

        PointerPair.P1= nullptr;
        if constexpr( UsageLength > sizeof(void*) )
            PointerPair.P2= nullptr;
    }

  //################################################################################################
  //############################################# Boxing ###########################################
  //################################################################################################

    /// This version of the overloaded method writes integral values.
    /// @tparam  TIntegral The integral type to store.
    /// @param   value     The value to store.
    template<typename TIntegral>
    requires(  std::is_integral_v<TIntegral> )
    constexpr void Write( const TIntegral& value ) {

        if constexpr( std::is_signed_v<TIntegral>) {

                 if constexpr( sizeof(TIntegral) ==  1 )    Integrals.Int8  =  int8_t (value);
            else if constexpr( sizeof(TIntegral) ==  2 )    Integrals.Int16 =  int16_t(value);
          #if  ALIB_SIZEOF_INTEGER == 4
            else if constexpr( sizeof(TIntegral) ==  4 )    Integrals.Int   =  integer(value);
            else if constexpr( sizeof(TIntegral) ==  8 )    Integrals.Int64 =  int64_t(value);
          #else
            else if constexpr( sizeof(TIntegral) ==  4 )    Integrals.Int32 =  int32_t(value);
            else if constexpr( sizeof(TIntegral) ==  8 )    Integrals.Int   =  integer(value);
          #endif
        } else {
                 if constexpr( sizeof(TIntegral) ==  1 )    Integrals.UInt8 = uint8_t (value);
            else if constexpr( sizeof(TIntegral) ==  2 )    Integrals.UInt16= uint16_t(value);
          #if  ALIB_SIZEOF_INTEGER == 4
            else if constexpr( sizeof(TIntegral) ==  4 )    Integrals.UInt  = uinteger(value);
            else if constexpr( sizeof(TIntegral) ==  8 )    Integrals.UInt64= uint64_t(value);
          #else
            else if constexpr( sizeof(TIntegral) ==  4 )    Integrals.UInt32= uint32_t(value);
            else if constexpr( sizeof(TIntegral) ==  8 )    Integrals.UInt  = uinteger(value);
          #endif
    }   }

    /// This version of the overloaded method writes floating-point values.
    /// @tparam  TFloat The integral type to store.
    /// @param   value     The value to store.
    template<typename TFloat>
    requires(  std::is_floating_point_v<TFloat> )
    constexpr void Write( const TFloat& value ) {
             if constexpr( std::same_as<TFloat, float > )    FloatingPoints.Float = value;
        else if constexpr( std::same_as<TFloat, double> )    FloatingPoints.Double= value;
    }

    /// This version of the overloaded method requires type \p{TTrivial} to:
    /// - be trivially copyable,
    /// - be a non-pointer type,
    /// - that it fits into the placeholder which is of <c>2 * sizeof(void*)</c>, and
    /// - is not integral (handled with other overload).
    ///
    /// This version copies the value using \c std::bit_cast.
    /// This way, this method may be used with \c constexpr use cases that enable
    /// \ref alib_boxing_more_opt_constexpr "constant boxes", which in seldom cases may be required.
    /// @tparam  TTrivial  The mapped type to store.
    /// @param   value    The value of type \p{TTrivial} to store.
    template<typename TTrivial>
    requires(    std::is_trivially_copyable_v<TTrivial>
              && (sizeof(TTrivial) <= 2 * sizeof(void*))
              && !std::is_pointer_v<TTrivial>
              && !std::is_integral_v<TTrivial>
              && !std::is_floating_point_v<TTrivial>   )
    constexpr void Write( const TTrivial& value ) {

             if constexpr( sizeof(TTrivial) ==  1 )    Bytes.C1 = std::bit_cast<std::array<char,  1>>( value );
        else if constexpr( sizeof(TTrivial) ==  2 )    Bytes.C2 = std::bit_cast<std::array<char,  2>>( value );
        else if constexpr( sizeof(TTrivial) ==  3 )    Bytes.C3 = std::bit_cast<std::array<char,  3>>( value );
        else if constexpr( sizeof(TTrivial) ==  4 )    Bytes.C4 = std::bit_cast<std::array<char,  4>>( value );
        else if constexpr( sizeof(TTrivial) ==  5 )    Bytes.C5 = std::bit_cast<std::array<char,  5>>( value );
        else if constexpr( sizeof(TTrivial) ==  6 )    Bytes.C6 = std::bit_cast<std::array<char,  6>>( value );
        else if constexpr( sizeof(TTrivial) ==  7 )    Bytes.C7 = std::bit_cast<std::array<char,  7>>( value );
        else if constexpr( sizeof(TTrivial) ==  8 )    Bytes.C8 = std::bit_cast<std::array<char,  8>>( value );
    #if  ALIB_SIZEOF_INTEGER == 8
        else if constexpr( sizeof(TTrivial) ==  9 )    Bytes.C9 = std::bit_cast<std::array<char,  9>>( value );
        else if constexpr( sizeof(TTrivial) == 10 )    Bytes.C10= std::bit_cast<std::array<char, 10>>( value );
        else if constexpr( sizeof(TTrivial) == 11 )    Bytes.C11= std::bit_cast<std::array<char, 11>>( value );
        else if constexpr( sizeof(TTrivial) == 12 )    Bytes.C12= std::bit_cast<std::array<char, 12>>( value );
        else if constexpr( sizeof(TTrivial) == 13 )    Bytes.C13= std::bit_cast<std::array<char, 13>>( value );
        else if constexpr( sizeof(TTrivial) == 14 )    Bytes.C14= std::bit_cast<std::array<char, 14>>( value );
        else if constexpr( sizeof(TTrivial) == 15 )    Bytes.C15= std::bit_cast<std::array<char, 15>>( value );
        else if constexpr( sizeof(TTrivial) == 16 )    Bytes.C16= std::bit_cast<std::array<char, 16>>( value );
    #endif
    }

    /// Writes two values of arbitrary type into the placeholder.
    /// The method requires that:
    /// - Types \p{T1} and \p{T2} are trivially copyable
    /// - The types are no pointers
    /// - The sum of the types' sizes fit into the placeholder which is of <c>2 * sizeof(void*)</c>.
    ///
    /// This version packs the two given values with no gap into a tuple and then writes that
    /// tuple using \c std::bit_cast. This way, this method may be used with \c constexpr
    /// use cases that enable \ref alib_boxing_more_opt_constexpr "constant boxes", which in
    /// seldom cases may be required. Furthermore, this way, values boxed like this fulfil one
    /// requirement to produce reliable \ref alib_boxing_customizing_placeholder "hash values".
    ///
    /// @tparam  T1  The type of the first value to store.
    /// @tparam  T2  The type of the second value to store.
    /// @param   v1  The first value to store.
    /// @param   v2  The second value to store.
    template<typename T1, typename T2>
    requires(    std::is_trivially_copyable_v<T1> && !std::is_pointer_v<T1>
              && std::is_trivially_copyable_v<T2> && !std::is_pointer_v<T2>
              && ( sizeof(T1) + sizeof(T2) <= 2 * sizeof(void*) )
            )
    constexpr void Write( const T1& v1, const T2& v2 ) {

        // Force the struct to be packed (i.e. no padding in between)
        #pragma pack(push, 1)
        struct PackedTuple {
            T1 first;
            T2 second;
        };
        #pragma pack(pop)

        static_assert(sizeof(PackedTuple) == sizeof(T1) + sizeof(T2),
                      "PackedTuple has unexpected padding!");

        // Pack the two values into our packed tuple.
        PackedTuple value { v1, v2 };

             if constexpr( sizeof(PackedTuple) ==  1 )    Bytes.C1 = std::bit_cast<std::array<char,  1>>( value );
        else if constexpr( sizeof(PackedTuple) ==  2 )    Bytes.C2 = std::bit_cast<std::array<char,  2>>( value );
        else if constexpr( sizeof(PackedTuple) ==  3 )    Bytes.C3 = std::bit_cast<std::array<char,  3>>( value );
        else if constexpr( sizeof(PackedTuple) ==  4 )    Bytes.C4 = std::bit_cast<std::array<char,  4>>( value );
        else if constexpr( sizeof(PackedTuple) ==  5 )    Bytes.C5 = std::bit_cast<std::array<char,  5>>( value );
        else if constexpr( sizeof(PackedTuple) ==  6 )    Bytes.C6 = std::bit_cast<std::array<char,  6>>( value );
        else if constexpr( sizeof(PackedTuple) ==  7 )    Bytes.C7 = std::bit_cast<std::array<char,  7>>( value );
        else if constexpr( sizeof(PackedTuple) ==  8 )    Bytes.C8 = std::bit_cast<std::array<char,  8>>( value );
    #if  ALIB_SIZEOF_INTEGER == 8
        else if constexpr( sizeof(PackedTuple) ==  9 )    Bytes.C9 = std::bit_cast<std::array<char,  9>>( value );
        else if constexpr( sizeof(PackedTuple) == 10 )    Bytes.C10= std::bit_cast<std::array<char, 10>>( value );
        else if constexpr( sizeof(PackedTuple) == 11 )    Bytes.C11= std::bit_cast<std::array<char, 11>>( value );
        else if constexpr( sizeof(PackedTuple) == 12 )    Bytes.C12= std::bit_cast<std::array<char, 12>>( value );
        else if constexpr( sizeof(PackedTuple) == 13 )    Bytes.C13= std::bit_cast<std::array<char, 13>>( value );
        else if constexpr( sizeof(PackedTuple) == 14 )    Bytes.C14= std::bit_cast<std::array<char, 14>>( value );
        else if constexpr( sizeof(PackedTuple) == 15 )    Bytes.C15= std::bit_cast<std::array<char, 15>>( value );
        else if constexpr( sizeof(PackedTuple) == 16 )    Bytes.C16= std::bit_cast<std::array<char, 16>>( value );
    #endif
    }

    /// This version of the overloaded method is used for boxing C++ pointer types.
    ///
    /// Note that for unboxing custom types from C++ array types, a custom implementation of
    /// \alib{boxing;BoxTraits::Read} is needed. Such an implementation reads the pointer and length
    /// directly from this struct. In other words, there is no overloaded method \b Read
    /// available for array types.
    ///
    /// @tparam  TPointer  The pointer type.
    /// @param   pointer   The pointer to store.
    template<typename TPointer>
    constexpr void Write( const TPointer* pointer)                  { *this= Placeholder(pointer); }

    /// This version of the overloaded method is used for boxing C++ array types.
    /// The type and length of the array is stored in the field #Array of the data union.
    ///
    /// Note that for unboxing custom types from C++ array types, a custom implementation of
    /// \alib{boxing;BoxTraits::Read} is needed. Such an implementation reads the pointer and length
    /// directly from this struct.
    /// (I.e. there is no overloaded method \b Read available for arrays.)
    ///
    /// @tparam  TArray  The pointer type.
    /// @param   pointer The pointer to store.
    /// @param   length  The array's length to store.
    template<typename TArray>
    constexpr void Write( const TArray* pointer, integer length )
    { *this= Placeholder(pointer, length); }

    /// This version of the overloaded method is used for boxing two pointer types.
    /// @tparam  TP1     The type of the first pointer.
    /// @tparam  TP2     The type of the second pointer.
    /// @param   p1      The first pointer to store.
    /// @param   p2      The second pointer to store.
    template<typename TP1, typename TP2>
    constexpr void Write( const TP1* p1, const TP2* p2 )             { *this= Placeholder(p1, p2); }

    /// This version of the overloaded method is selected when the type \p{TPointer} is not
    /// trivially copyable, still fits into the placeholder of size <c>2 * sizeof(void*)</c>, and
    /// furthermore does not directly match other overloaded versions.
    ///
    /// The copying is performed using \c memcpy.
    /// This is necessary to avoid de-referencing type-punned pointers which would
    /// break the strict-aliasing rule when compiling the code with higher optimization levels.
    /// Note that modern compilers like GCC usually optimize the invocation of \c memcpy out.<br>
    ///
    /// This version is \b not \c constexpr and thus may not be used to create
    /// \ref alib_boxing_more_opt_constexpr "constant boxes", which in
    /// seldom cases may be required.
    ///
    /// @tparam  TPointer  The type of the value to store.
    /// @param   value    The value to store.
    template<typename TPointer>
    requires(    !std::is_trivially_copyable_v<TPointer>
              && (sizeof(TPointer) <= 2 * sizeof(void*))    )
    void Write( const TPointer& value )
    { std::memcpy( &PointerPair.P1, &value, sizeof(TPointer) ); }

  //################################################################################################
  //############################################ Unboxing ##########################################
  //################################################################################################
    /// Templated read method for value types that fit into this placeholder, which are not
    /// pointers, and furthermore which are not trivially copyable.
    ///
    /// The value is dereferenced from the start of the placeholder memory using
    /// \c reinterpret_cast. With that, this overload is never \c constexpr and thus not marked
    /// so.
    ///
    /// @tparam TMapped  The value type to unbox.
    /// @return The value stored.
    template<typename TMapped>
    requires(  !(    std::is_trivially_copyable_v<TMapped>
                 && (sizeof(TMapped) <= 2 * sizeof(void*)) )
                 && !std::is_pointer_v<TMapped>
            )
    TMapped Read()                       const { return *reinterpret_cast<const TMapped*>( this ); }

    /// Templated method to read pointer types from this placeholder.
    /// The pointer is changed to the desired type using \c reinterpret_cast.
    /// Only if <c>void*</c> is requested, this method can be \c constexpr.
    /// @tparam TPointer The pointer-type to unbox.
    /// @return The value stored.
    template<typename TPointer>
    requires  std::is_pointer_v<TPointer>
    constexpr TPointer Read()                                                                const {
        if constexpr (std::same_as<TPointer, void*>)
            return VoidP;
        return reinterpret_cast<TPointer>( VoidP );
    }

    /// Templated method to read integral types from this placeholder.
    /// @tparam TIntegral The integral type to unbox.
    /// @return The value stored.
    template<typename TIntegral>
    requires(  std::is_integral_v<TIntegral> )
    constexpr TIntegral Read()                                                               const {

        if constexpr( std::is_signed_v<TIntegral>) {

                 if constexpr( sizeof(TIntegral) ==  1 )    return TIntegral(Integrals.Int8 );
            else if constexpr( sizeof(TIntegral) ==  2 )    return TIntegral(Integrals.Int16);
          #if  ALIB_SIZEOF_INTEGER == 4
            else if constexpr( sizeof(TIntegral) ==  4 )    return TIntegral(Integrals.Int  );
            else if constexpr( sizeof(TIntegral) ==  8 )    return TIntegral(Integrals.Int64);
          #else
            else if constexpr( sizeof(TIntegral) ==  4 )    return TIntegral(Integrals.Int32);
            else if constexpr( sizeof(TIntegral) ==  8 )    return TIntegral(Integrals.Int  );
          #endif
        } else {
                 if constexpr( sizeof(TIntegral) ==  1 )    return TIntegral(Integrals.UInt8 );
            else if constexpr( sizeof(TIntegral) ==  2 )    return TIntegral(Integrals.UInt16);
          #if  ALIB_SIZEOF_INTEGER == 4
            else if constexpr( sizeof(TIntegral) ==  4 )    return TIntegral(Integrals.UInt  );
            else if constexpr( sizeof(TIntegral) ==  8 )    return TIntegral(Integrals.UInt64);
          #else
            else if constexpr( sizeof(TIntegral) ==  4 )    return TIntegral(Integrals.UInt32);
            else if constexpr( sizeof(TIntegral) ==  8 )    return TIntegral(Integrals.UInt  );
          #endif
    }   }

    /// Templated method to read integral types from this placeholder.
    /// @tparam TFloat The integral type to unbox.
    /// @return The value stored.
    template<typename TFloat>
    requires(  std::is_floating_point_v<TFloat> )
    constexpr TFloat Read()                                                                  const {
             if constexpr( std::same_as<TFloat, float > )    return TFloat(FloatingPoints.Float );
        else if constexpr( std::same_as<TFloat, double> )    return TFloat(FloatingPoints.Double);
    }

    /// Templated method to read trivially copyable types that fits into the placeholder.
    /// @tparam TTrivial The type to read from this placeholder.
    /// @return The stored value.
    template<typename TTrivial>
    requires(    std::is_trivially_copyable_v<TTrivial>
              && (sizeof(TTrivial) <= 2 * sizeof(void*))
              && !std::is_pointer_v<TTrivial>
              && !std::is_integral_v<TTrivial>
              && !std::is_floating_point_v<TTrivial>
             )
    constexpr TTrivial Read()                                                                const {

             if constexpr( sizeof(TTrivial) ==  1 )    return std::bit_cast<TTrivial>(Bytes.C1);
        else if constexpr( sizeof(TTrivial) ==  2 )    return std::bit_cast<TTrivial>(Bytes.C2);
        else if constexpr( sizeof(TTrivial) ==  3 )    return std::bit_cast<TTrivial>(Bytes.C3);
        else if constexpr( sizeof(TTrivial) ==  4 )    return std::bit_cast<TTrivial>(Bytes.C4);
        else if constexpr( sizeof(TTrivial) ==  5 )    return std::bit_cast<TTrivial>(Bytes.C5);
        else if constexpr( sizeof(TTrivial) ==  6 )    return std::bit_cast<TTrivial>(Bytes.C6);
        else if constexpr( sizeof(TTrivial) ==  7 )    return std::bit_cast<TTrivial>(Bytes.C7);
        else if constexpr( sizeof(TTrivial) ==  8 )    return std::bit_cast<TTrivial>(Bytes.C8);
    #if  ALIB_SIZEOF_INTEGER == 8
        else if constexpr( sizeof(TTrivial) ==  9 )    return std::bit_cast<TTrivial>(Bytes.C9 );
        else if constexpr( sizeof(TTrivial) == 10 )    return std::bit_cast<TTrivial>(Bytes.C10);
        else if constexpr( sizeof(TTrivial) == 11 )    return std::bit_cast<TTrivial>(Bytes.C11);
        else if constexpr( sizeof(TTrivial) == 12 )    return std::bit_cast<TTrivial>(Bytes.C12);
        else if constexpr( sizeof(TTrivial) == 13 )    return std::bit_cast<TTrivial>(Bytes.C13);
        else if constexpr( sizeof(TTrivial) == 14 )    return std::bit_cast<TTrivial>(Bytes.C14);
        else if constexpr( sizeof(TTrivial) == 15 )    return std::bit_cast<TTrivial>(Bytes.C15);
        else if constexpr( sizeof(TTrivial) == 16 )    return std::bit_cast<TTrivial>(Bytes.C16);
    #endif
    }


    /// Templated method that reads two trivially copyable types that jointly fit into the
    /// placeholder.
    /// @tparam  TTrivial1 The type of the first value to read from this placeholder.
    /// @tparam  TTrivial2 The type of the second value to read from this placeholder.
    /// @param   v1        Output parameter. Receives the first value.
    /// @param   v2        Output parameter. Receives the second value.
    template<typename TTrivial1, typename TTrivial2>
    requires(    std::is_trivially_copyable_v<TTrivial1> && !std::is_pointer_v<TTrivial1>
              && std::is_trivially_copyable_v<TTrivial2> && !std::is_pointer_v<TTrivial2>
              && ( sizeof(TTrivial1) + sizeof(TTrivial2) <= 2 * sizeof(void*) )
            )
    constexpr void Read(TTrivial1& v1, TTrivial2& v2)                                        const {

        // Ensure the packed structure has no gaps.
        #pragma pack(push, 1)
        struct PT {
            TTrivial1 v1;
            TTrivial2 v2;
        };
        #pragma pack(pop)

        static_assert(sizeof(PT) == sizeof(TTrivial1) + sizeof(TTrivial2),
                      "PackedTuple has unexpected padding!");

        // Depending on the size of PackedTuple, bit_cast from the corresponding member
        // of Bytes to our packed structure, then assign the values.
             if constexpr( sizeof(PT) ==  1 ) { const auto pt = std::bit_cast<PT>(Bytes.C1);  v1 = pt.v1; v2 = pt.v2; }
        else if constexpr( sizeof(PT) ==  2 ) { const auto pt = std::bit_cast<PT>(Bytes.C2);  v1 = pt.v1; v2 = pt.v2; }
        else if constexpr( sizeof(PT) ==  3 ) { const auto pt = std::bit_cast<PT>(Bytes.C3);  v1 = pt.v1; v2 = pt.v2; }
        else if constexpr( sizeof(PT) ==  4 ) { const auto pt = std::bit_cast<PT>(Bytes.C4);  v1 = pt.v1; v2 = pt.v2; }
        else if constexpr( sizeof(PT) ==  5 ) { const auto pt = std::bit_cast<PT>(Bytes.C5);  v1 = pt.v1; v2 = pt.v2; }
        else if constexpr( sizeof(PT) ==  6 ) { const auto pt = std::bit_cast<PT>(Bytes.C6);  v1 = pt.v1; v2 = pt.v2; }
        else if constexpr( sizeof(PT) ==  7 ) { const auto pt = std::bit_cast<PT>(Bytes.C7);  v1 = pt.v1; v2 = pt.v2; }
        else if constexpr( sizeof(PT) ==  8 ) { const auto pt = std::bit_cast<PT>(Bytes.C8);  v1 = pt.v1; v2 = pt.v2; }
    #if ALIB_SIZEOF_INTEGER > 4
        else if constexpr( sizeof(PT) ==  9 ) { const auto pt = std::bit_cast<PT>(Bytes.C9);  v1 = pt.v1; v2 = pt.v2; }
        else if constexpr( sizeof(PT) == 10 ) { const auto pt = std::bit_cast<PT>(Bytes.C10); v1 = pt.v1; v2 = pt.v2; }
        else if constexpr( sizeof(PT) == 11 ) { const auto pt = std::bit_cast<PT>(Bytes.C11); v1 = pt.v1; v2 = pt.v2; }
        else if constexpr( sizeof(PT) == 12 ) { const auto pt = std::bit_cast<PT>(Bytes.C12); v1 = pt.v1; v2 = pt.v2; }
        else if constexpr( sizeof(PT) == 13 ) { const auto pt = std::bit_cast<PT>(Bytes.C13); v1 = pt.v1; v2 = pt.v2; }
        else if constexpr( sizeof(PT) == 14 ) { const auto pt = std::bit_cast<PT>(Bytes.C14); v1 = pt.v1; v2 = pt.v2; }
        else if constexpr( sizeof(PT) == 15 ) { const auto pt = std::bit_cast<PT>(Bytes.C15); v1 = pt.v1; v2 = pt.v2; }
        else if constexpr( sizeof(PT) == 16 ) { const auto pt = std::bit_cast<PT>(Bytes.C16); v1 = pt.v1; v2 = pt.v2; }
    #endif
    }
}; // union Placeholder

static_assert( sizeof(Placeholder) == 2 * sizeof(std::size_t),
               "Size of boxing::Placeholder is not two times the size of 'size_t'. "
               "Compilation platform not supported." );


//==================================================================================================
/// This is a simple helper type that forms a pair of two values. It is useful when the boxing of
/// such a pair is wanted. The benefit over using <c>std::pair</c> is, that this struct does not
/// provide any extras but the plain public values, and thus it is trivially copyable in the case
/// that both types are.
/// @tparam T1 The type of the first value.
/// @tparam T2 The type of the second first value.
//==================================================================================================
template <typename T1, typename T2>
requires (      std::is_trivially_copyable_v<T1>
            &&  std::is_trivially_copyable_v<T2>
            && ( sizeof(T1) + sizeof(T2) <= 2 * sizeof(void*) )
         )
struct Pair {
    T1 First;    ///< The first value.
    T2 Second;   ///< The second value.
};

//==================================================================================================
/// This is a simple helper function that constructs a \alib{boxing;Pair}.
/// @tparam T1 The type of the first value. Deduced by the compiler.
/// @tparam T2 The type of the second first value. Deduced by the compiler.
/// @param  t1 The first value.
/// @param  t2 The second value.
/// @return The pair of deduced types.
//==================================================================================================
template <typename T1, typename T2>
requires (      std::is_trivially_copyable_v<T1>
            &&  std::is_trivially_copyable_v<T2>
            && ( sizeof(T1) + sizeof(T2) <= 2 * sizeof(void*) )
         )
constexpr Pair<T1, T2> MakePair( const T1& t1, const T2& t2)        { return Pair<T1, T2>{t1, t2}; }

}

/// Type alias in namespace \b alib.
template <typename T, typename U>
using Pair= boxing::Pair<T, U>;

} // namespace [alib::boxing]
