// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#if !defined(HPP_ALIB_BOXING_PROPPERINCLUDE)
#   error "ALib sources with ending '.inl' must not be included from outside."
#endif

namespace aworx { namespace lib { namespace boxing  {

/** ************************************************************************************************
 * This built-in \ref alib_boxing_functions "box function" constitutes the concept of
 * "nullable types". Nullable types are pointer types, array types and custom types that are
 * nullable. Arithmetical types are never \e nulled, even if they contain value \c 0 or \c 0.0.
 *
 * A default implementation is given, which returns \c false (not <em>nulled</em>!) if the first word in
 * the placeholder is not \c 0. Hence this works for array and pointer types.
 *
 * A constant function return \c true is given with static method #ConstantTrue. This function
 * is set as the specific implementation for all arithmetical types, as well as for type \c bool.
 *
 * \note
 *   The reason why this function is not declared as \b FIsNull, but instead is negated, is that
 *   if any function is called on a \ref alib_boxing_more_void_void "void box", a
 *   default value is returned. The default value of \c bool is \c false, which probably better
 *   fits - even if conceptually a void box is undefined and therefore neither \e nulled or not.
 **************************************************************************************************/
struct FIsNotNull
{
    /**
     * Signature of the invokable function.
     *
     * @param  self The box that the function was invoked on.
     * @return Return \c false if a boxed value is \e nulled, \c true otherwise.
     */
    using Signature = bool (*) ( const Box& self );

    /**
     * This implementation of function #FIsNotNull returns constant \c true.
     * It may be registered with custom types that do not provide the concept of being \e nulled.
     * \ref alib_manual_bootstrapping "Bootstrap" function
     * \alib{boxing::Init} registers this implementation with type \c bool and all integral,
     * floating point and character types.
     *
     * @return Constant \c true.
     */
    ALIB_API
    static bool ConstantTrue( const Box& );
};


/** ************************************************************************************************
 * This function returns a hashcode on contents of the box. This is useful if boxes
 * are to be used as key-values of containers like \c std::unordered_map.
 *
 * Its default implementation creates a hash code using the raw placeholder values and
 * in case of array types over the array memory used.
 *
 * A templated version that compares the first N-bytes is given with #UsePlaceholderBytes.
 * Because the number of bytes to use are given with the template parameter, the method compiles
 * to shortest code. It is registered with all fundamental types.
 *
 * \note
 *   Compatibility header \ref alib/compatibility/std_boxing_functional.hpp specializes
 *   functors \c std::hash, \c std::equal_to and \c std::less for use with containers of the
 *   C++ standard library.
 *
 * \see
 *   Method \alib{boxing,Box::Hashcode}, which calls this function.
 **************************************************************************************************/
struct FHashcode
{
    /**
     * Signature of the invokable function.
     *
     * @param  self The box that the hash code is to be calculated for.
     * @return The hash code.
     */
    using Signature = size_t (*) ( const Box& self );

    /**
     * Templated hash function usable with types boxed as values.
     * For pointer types, a custom variant that collects type-specific hashable data is recommended.
     *
     * @param  self The box to calculate a hash code for.
     * @tparam N    The number of bytes to check.
     * @return The hash code.
     */
    template<size_t N>
    static size_t UsePlaceholderBytes( const Box& self );

};


/** ************************************************************************************************
 * This function compares two boxes.
 *
 * A default implementation is \alib{boxing,RegisterDefault,registered} that compares the types
 * (\alib{boxing,Box::IsSameType}) and if equal, with arrays compares the array's length,
 * \e nulled state and finally the contents using \c memcmp.<br>
 * For non-array types, it compares the relevant bytes in the placeholder.
 * The number of leading relevant bytes is available with method
 * \alib{boxing,Box::GetPlaceholderUsageLength}.
 *
 * Type-specific implementations are given and registered for fundamental types. Integrals
 * of different sizes and floating point values will be compared by using
 * \alib{boxing,Box::UnboxSignedIntegral}, \alib{boxing,Box::UnboxUnsignedIntegral} and
 * \alib{boxing,Box::UnboxFloatingPoint} and appropriate casting.
 * In the case of floating point comparison, an epsilon distance is duly taken into account.
 *
 * For custom types, with #ComparableTypes, a templated implementation is suggested: Rather than
 * implementing a specific box-function, the custom type should implement \c operator== and register
 * an instantiation of the templated function.
 *
 * \see
 *   Method \alib{boxing,Box::operator==}, which calls this function.
 **************************************************************************************************/
struct FEquals
{
    /**
     * Signature of the invokable function.
     *
     * @param self The box that the function was invoked on.
     * @param rhs  The box to compare.
     * @return Return value and type is implementation specific.
     */
    using Signature = bool (*) ( const Box& self, const Box& rhs);


    #if ALIB_DOCUMENTATION_PARSER
    /**
     * Templated implementation for function #FEquals, usable with boxable types which have
     * <c>operator==</c> implemented.
     *
     * \note
     *   This method interally is provide twice, once for types boxed as pointers and one for
     *   types boxed as values, and selected using TMP.<br>
     *   If a type is boxed as pointer, then \p{TComparable} has to be given as such pointer type.
     *   For comparison, the unboxed pointers will be dereferenced. If one is \e nulled, \c false
     *   is returned, if both are \e nulled, true.
     *
     * \see
     *   Macro \ref ALIB_BOXING_DEFINE_FEQUALS_FOR_COMPARABLE_TYPE.
     *
     * @param self The box that the function was invoked on.
     * @param rhs  The boxed value to compare.
     * @tparam TComparable  The mapped type that can be compared using <c>operator==</c>.
     *
     * @return \c true if \p{self} equals \p{rhs}, \c false otherwise.
     */
    template<typename TComparable>
    static
    bool  ComparableTypes( const Box& self, const Box& rhs );

    #else

    template<typename TComparable>
    static
    ATMP_T_IF(bool, !ATMP_IS_PTR(TComparable) )
    ComparableTypes( const Box& self, const Box& rhs );

    template<typename TComparable>
    static
    ATMP_T_IF(bool,  ATMP_IS_PTR(TComparable) )
    ComparableTypes( const Box& self, const Box& rhs );
    #endif
};

#define ALIB_BOXING_DEFINE_FEQUALS_FOR_COMPARABLE_TYPE( TComparable )                              \
aworx::lib::boxing::Register< FEquals,                                                             \
   aworx::lib::boxing::TMappedTo<TComparable >>(FEquals::ComparableTypes<TComparable>);            \


/** ************************************************************************************************
 * This function provides a relational comparison of two boxes.
 *
 * A default implementation is \alib{boxing,RegisterDefault,registered} that compares the types.
 * If they are equal, the first \c uinteger values in the placeholders are compared.
 * Specifics for array types are \b not implemented with that default version.
 *
 * If the types are not the same, the result of the comparison of the run-time type information
 * object is returned. For this, method \alib{boxing,Box::TypeID} is invoked on both boxes
 * and to allow <c>operator\<</c> on those, \c std::type_index is applied.
 * This leads to a "nested" sort order, with the type information being the outer order and
 * the boxed data being the inner.<br>
 * To keep this overall order intact, type-specific implementations should use the following
 * implementation scheme:
 *
 *      if( rhs.IsType<AComparableType1>() )
 *          return MyCompare( self.Unbox<MyType>, rhs.Unbox<AComparableType1>();
 *
 *      if( rhs.IsType<AComparableType2>() )
 *          return MyCompare( self.Unbox<MyType>, rhs.Unbox<AComparableType2>();
 *
 *      if( ...
 *          return ...
 *
 *      return std::type_index( self.TypeID() ) < std::type_index( rhs.TypeID() );
 *
 * With this scheme in place, for example \c std::sort will work properly on containers of boxes of
 * mixed types. The following sample demonstrates this. It uses a specialization of
 * \c std::less<T> for type \b Box. This is found in a compatibility header:
 *
 *   \snippet "DOX_ALIB_BOXING.cpp"     DOX_ALIB_BOXING_FISLESS_INCLUDES
 *
 * With that, the following code compiles fine:
 *   \snippet "DOX_ALIB_BOXING.cpp"     DOX_ALIB_BOXING_FISLESS
 *
 * It generates the following output:
 *
 *   \snippet "DOX_ALIB_BOXING_FISLESS.txt"     OUTPUT
 *
 * As can be seen from the result, a proper "outer" sorting is in place. Nevertheless,
 * floating-point and integral values follow the inner sorting.
 * This is because specific implementations of the functions are registered for all arithmetic
 * types, which allow comparisons of mixed types.
 * The same is true for the different C++ character types.
 *
 * \note
 *   It is a matter of the compiler (and can not be determined by the user code) how the
 *   types are sorted (outer sorting).<br>
 *   Furthermore, the default implementation that simply compares the first \b uinteger of the
 *   placeholder is unlikely to produce "reasonable" results.
 *
 * Type-specific implementations are given and registered for fundamental types. Integrals
 * of different sizes and floating point values will be compared by using
 * \alib{boxing,Box::UnboxSignedIntegral}, \alib{boxing,Box::UnboxUnsignedIntegral} and
 * \alib{boxing,Box::UnboxFloatingPoint} and appropriate casting.
 *
 * If module \alibmod_strings is included in the distribution, an implementation for
 * arrays of \alib{characters,nchar}, \alib{characters,wchar} and \alib{characters,xchar} is given.
 *
 * For custom types, with #ComparableTypes, a templated implementation is suggested: Rather than
 * implementing a specific box-function, the custom type should implement \c operator< and register
 * an instantiation of the templated function.
 *
 * \see
 *   Method \alib{boxing,Box::operator<}, which calls this function.
 **************************************************************************************************/
struct FIsLess
{
    /**
     * Signature of the invokable function.
     *
     * @param self The box that the function was invoked on.
     * @param rhs  The box to compare.
     * @return Return value and type is implementation specific.
     */
    using Signature = bool (*) ( const Box& self, const Box& rhs);


    #if ALIB_DOCUMENTATION_PARSER
    /**
     * Templated implementation for function #FIsLess, usable with boxable types which have
     * <c>operator\<</c> implemented.
     *
     * \note
     *   This method interally is provide twice, once for types boxed as pointers and one for
     *   types boxed as values, and selected using TMP.<br>
     *   If a type is boxed as pointer, then \p{TComparable} has to be given as such pointer type.
     *   For comparison, the unboxed pointers will be dereferenced. If both are \e nulled, \c false
     *   is returned, if only \p{self} is \e nulled, \c true and if only \p{rhs} is \e nulled,
     *   then \c false.
     *
     * \see
     *   Macro \ref ALIB_BOXING_DEFINE_FISLESS_FOR_COMPARABLE_TYPE.
     *
     * @param self The box that the function was invoked on.
     * @param rhs  The boxed value to compare.
     * @tparam TComparable  The mapped type that can be compared using <c>operator\<</c>.
     *
     * @return \c true if \p{self} equals \p{rhs}, \c false otherwise.
     */
    template<typename TComparable>
    static
    bool  ComparableTypes( const Box& self, const Box& rhs );

    #else
    template<typename TComparable> static  ATMP_T_IF(bool, !ATMP_IS_PTR(TComparable) )
    ComparableTypes( const Box& self, const Box& rhs );

    template<typename TComparable> static  ATMP_T_IF(bool,  ATMP_IS_PTR(TComparable) )
    ComparableTypes( const Box& self, const Box& rhs );
    #endif
};

#define ALIB_BOXING_DEFINE_FISLESS_FOR_COMPARABLE_TYPE( TComparable )                              \
aworx::lib::boxing::Register< FIsLess,                                                             \
   aworx::lib::boxing::TMappedTo<TComparable >>(FIsLess::ComparableTypes<TComparable>);            \


#if ALIB_MODULE_MEMORY
// forwards
} namespace memory  { class MemoryBlocks; }  namespace boxing  {


/** ************************************************************************************************
 * This type declares a built-in \ref alib_boxing_functions "box-function".<br>
 * Besides mandatory parameter \p{self}, implementations expect a reference to a
 * block allocator of type \alib{memory,MemoryBlocks}.
 * With that, a deep copy of the boxed object can be allocated.
 *
 * The function is provided for use-cases where boxes have to "survive" the end of
 * the lifecycle of the original object.
 *
 * A default implementation of this function is provided. While this just does nothing
 * for non-array types, with array types, the complete contents of the array is cloned.
 * With this in place, all boxed character arrays (strings) are cloned.
 * This is done regardless if it is necessary or not. For example, if the boxed string
 * was created from a C++ string literal, a deep copy is unnecessary.
 * While this imposes a little overhead, there is no way to avoid this because the origin of a
 * boxed array can not be determined.
 *
 * \attention
 *   Only objects that do not need to be destructed are allowed to be cloned using the
 *   block-allocator given. This is because \b no destructor will be invoked for boxed objects
 *   (which is true in general for \alibmod_boxing).<br>
 *   Of-course a custom implementation could create and allocate the object in a custom place,
 *   that allows later destruction. Alternatively, a custom method could just assure that
 *   an object will not be deleted, e.g. by increasing a usage counter and leave the given box
 *   untouched.
 *
 * \note
 *   Today, this is the only box-function type found within \alib, that requires a
 *   mutable "this"  pointer (aka a non constant parameter \p{self}).<br>
 *   Consequently, if the function is called, the compiler selects the non-constant version of
 *   method \alib{boxing,Box::Call}.<br>
 *   Furthermore it has to be noted, that an invocation of this function might change the type
 *   together with the contents of \p{self}. Therefore, results of any already performed
 *   type-guessing are not valid after an invocation.
 *
 * \attention
 *   If a mapped type has no specialization for this function, there are three possibilities:
 *   1. The original value is not deleted during the lifecycle of the box.
 *   2. The type was boxed as a value type (or, very unlikely, otherwise is safe to be unboxed,
 *      even after the deletion of the original value).
 *   3. Undefined behaviour (crash) due to unboxing the value after deletion of the original
 *      object.
 *
 * #### Availability ####
 * This box-function is available only if module \alibmod_memory is included in the
 * \alibdist.
 **************************************************************************************************/
struct FClone
{
    /**
     * Signature of the box-function.
     *
     * @param self    The mutable box that the function was invoked on.
     * @param memory  A block-allocator that may be used for cloning.
     */
    using Signature = void (*) ( boxing::Box& self, memory::MemoryBlocks& memory );
};
#endif

/** ************************************************************************************************
 * This is one of the built-in \ref alib_boxing_functions "box-functions" of \alibmod_nolink_boxing.
 * This function is used to give an answer to the question if a boxed value represents boolean
 * value \c true or \c false. This is useful if "yes/no" decisions should be taken based on
 * arbitrary boxed values.
 *
 * A default implementation is registered which for non-array types just interprets the first
 * integral value in the \alib{boxing,Placeholder}: If it is not \c 0, \c true is returned, \c false
 * otherwise.<br>
 * For array types, the default implementation returns \c true if method
 * \alib{boxing,Box::UnboxLength} returns a value different to \c 0, otherwise \c false is returned.
 **************************************************************************************************/
struct FIsTrue
{
    /**
     * Signature of the invokable function.
     *
     * @param  self The box that the function was invoked on.
     * @return Return value and type is implementation specific.
     */
    using Signature = bool (*) ( const Box& self);
};



#if ALIB_MODULE_STRINGS
/** ************************************************************************************************
 * Implementations of this \ref alib_boxing_functions "box-function" write the content of the
 * data stored in the box to the given \b %AString object.
 *
 * A default implementation is registered. This writes out the raw value of the first
 * \c uinteger field of the boxe's \alib{boxing,Placeholder} in hexadecimal format.
 * For pointer types, such raw value reflect the memory address of the boxable.
 * In debug-compilations, in addition the type name of the boxed value is written.
 *
 * Templated static method #Appendable can be used to avoid implementations for those mapped types
 * that specialized type traits functor \alib{strings,T_Append} that makes values of the type
 * usable with \alib{strings,TAString::Append,AString::Append} already.
 *
 * \note
 *   This is a templated (!) function declaration that defines three different box-functions at
 *   once, namely \b FAppend<character>, \b FAppend<complementChar> and \b FAppend<strangeChar>.
 *
 * This box-function is usually invoked only indirectly, by "appending" a box to an \b AString,
 * as shown here:
 *
 *      Box     box= 42;
 *      AString text;
 *
 *      text << "The answer is: "; // appends a string literal
 *      text << box;               // translates to: box.Call<FAppend<character>>( text )
 *
 * #### Availability ####
 * This box-function is available only if module \alibmod_strings is included in the \alibdist.
 *
 * \see
 *    - Manual chapter \ref alib_boxing_strings_fappend
 *    - Chapter \ref alib_stringformat_custom_types "3. Formatting Custom Types" of
 *      the Programmer's Manual of \alibmod_stringformat, which gives
 *      a sample implementation of this function, as well as introduces a more powerful
 *      box-function \alib{stringformat,FFormat}.
 *
 * @tparam TChar  The character type of the destination \alib{strings,TAString,AString} given with
 *                parameter \p{target}.
 **************************************************************************************************/
template<typename TChar>
struct FAppend
{
    /**
     * Signature of the invokable function.
     *
     * @param  self   The box that the function was invoked on.
     * @param  target The target \b %AString of character type \p{TChar}.
     */
    using Signature = void (*) ( const Box& self, strings::TAString<TChar>& target );


    #if ALIB_DOCUMENTATION_PARSER
    /**
     * Static templated implementation of \ref FAppend for boxed types which are appendable.
     *
     * Once a type is made \e appendable by specializing type traits functor
     * \alib{strings,T_Append}, then this static templated function can be used "as is"
     * and registered with the corresponding mapped type.
     *
     * \note
     *   This method interally is provide twice, once for types boxed as pointers and one for
     *   types boxed as values, and selected using TMP.<br>
     *   If a type is boxed as pointer, then \p{TComparable} has to be given as such pointer type.
     *   For comparison, the unboxed pointers will be dereferenced. This means, that
     *   it is believed that \alib{strings,T_Append} is specialized for the non-pointer type.
     *   If this is not the case, then two options exists:
     *   1. Specialize functor \alib{strings,T_Append} for the non-pointer type in parallel.
     *   2. Do not use this inlined implementation, but rather provide a custom one that
     *      does not dereference unboxed pointers.
     *
     * \see
     *  Macros
     *  - ALIB_BOXING_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE
     *  - ALIB_BOXING_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE_N
     *  - ALIB_BOXING_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE_W
     *  - ALIB_BOXING_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE_X
     *
     * @tparam TAppendable   The "appendable" mapped box type that the function is to be implemented
     *                       for.
     * @param  self   The box that the function was invoked on.
     * @param  target The target \b %AString of character type \p{TChar}.
     */
    template<typename TAppendable>
    inline static
    void Appendable( const Box& self, strings::TAString<TChar>& target );
    #else

    template<typename TAppendable>  inline static ATMP_VOID_IF( !ATMP_IS_PTR(TAppendable) )
    Appendable( const Box& self, strings::TAString<TChar>& target );

    template<typename TAppendable>  inline static ATMP_VOID_IF(  ATMP_IS_PTR(TAppendable) )
    Appendable( const Box& self, strings::TAString<TChar>& target );

    #endif


    /**
     * Implementation template for box-function #FAppend for appendable types wrapped in
     * \c std::reference_wrapper to \ref alib_boxing_customizing_identity "bypass custom boxing".
     *
     * This static function template can be used for any type \p{TAppendable} that has a
     * specialization of functor \alib{strings,T_Append} defined and that becomes boxed in the
     * wrapper type.
     *
     * @tparam TAppendable   The "appendable" wrapped mapped box type that the function is to be
     *                       implemented for.
     * @param self   The box that the function was invoked on.
     * @param target The target \b %BoxedAs\<AString\> object.
     */
    template<typename TAppendable>
    inline static
    void WrappedAppendable( const Box& self, strings::TAString<TChar>& target);


};


#define ALIB_BOXING_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE(TAppendable)                              \
aworx::lib::boxing::Register<FAppend<character>, aworx::lib::boxing::TMappedTo<TAppendable>>       \
                      (aworx::lib::boxing::FAppend<character>::Appendable<TAppendable>);           \

#define ALIB_BOXING_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE_N(TAppendable)                            \
aworx::lib::boxing::Register<FAppend<nchar  >, aworx::lib::boxing::TMappedTo<TAppendable>>         \
                      (aworx::lib::boxing::FAppend<nchar    >::Appendable<TAppendable>);           \

#define ALIB_BOXING_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE_W(TAppendable)                            \
aworx::lib::boxing::Register<FAppend<wchar  >, aworx::lib::boxing::TMappedTo<TAppendable>>         \
                      (aworx::lib::boxing::FAppend<wchar    >::Appendable<TAppendable>);           \

#define ALIB_BOXING_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE_X(TAppendable)                            \
aworx::lib::boxing::Register<FAppend<xchar  >, aworx::lib::boxing::TMappedTo<TAppendable>>         \
                      (aworx::lib::boxing::FAppend<xchar    >::Appendable<TAppendable>);           \

#endif //ALIB_MODULE_STRINGS

}}} // namespace [aworx::lib::boxing]
