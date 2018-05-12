// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
/** @file */ // Hello Doxygen



//  set include guard
#ifndef HPP_ALIB_BOXING_INTERFACE_BUILTIN
#define HPP_ALIB_BOXING_INTERFACE_BUILTIN 1


namespace aworx { namespace lib { namespace boxing {

/** ************************************************************************************************
 * This is one of the built-in \alib{boxing,Interface} classes of <b>%ALib %Boxing</b>.
 * This interface is invokable with all boxable types, as a default implementation is registered
 * (see \alib{boxing,Boxer::DefineDefaultInterface} and is internally invoked by
 * \alib{boxing,Box::operator==}.
 *
 * The default implementation is provided with this base interface class itself.
 *
 * For non-array types, this default implementation checks if both boxes share the same type and
 * data, without interpreting the data in any way.<br>
 * For array types, the contents of the array is checked using \c memcmp() if types and lengths
 * are the same.
 *
 * For custom types boxed as pointers (the common, default), a templated default implementation
 * is available that relies on the availability of <c>operator==</c> for the type. Hence, the
 * preferred way of providing this interface for custom types is:
 *
 * 1. Implement C++ <c>operator==</c> for the custom type.
 * 2. Place maco ALIB_BOXING_DEFINE_IEQUALS_FOR_COMPARABLE_TYPE in the bootstrap section of the
 *    software.
 *
 **************************************************************************************************/
class IEquals : public Interface
{
    public:

    /**
     * Declares a static singleton used for the default interface registration and returns that.
     * \note We do not inherit template class Singleton here, because each of our descendants
     *       needs to do this as well, which would lead to some confusion.
     * @return A singleton instance of myself.
     */
    static IEquals* GetDefaultSingleton()
    {
        static IEquals me;
        return &me;
    }

    /**
     * Constructor providing our runtime type to the parent
     */
    IEquals() : boxing::Interface( typeid(IEquals) ) {}

    /**
     * Overrideable method that evaluates if two boxed objects equal to each other.
     * The default implementation checks if both boxes share the same type and data.
     * (it does not interpret the data).
     *
     * @param lhs  The box that the interface was invoked on.
     * @param rhs  The boxed value to compare us to.
     * @return \c true if \p{lhs} equals \p{rhs}, \c false otherwise.
     */
    ALIB_API
    virtual bool Invoke( const Box& lhs, const Box& rhs );
};

/**
 * Templated implementation of #IEquals for boxable types which have <c>operator==</c> implemented.
 *
 * To define this interface for a custom type, macro
 * \ref ALIB_BOXING_DEFINE_IEQUALS_FOR_COMPARABLE_TYPE has to be placed in bootstrap section of
 * a software that uses this type.
 */
template<typename TComparable>
class IEquals_TComparable : public IEquals, public Singleton<IEquals_TComparable<TComparable>>
{
    public:
    /**
     * The interface method, writing the applicable type.
     * @param lhs  The box that the interface was invoked on.
     * @param rhs  The boxed value to compare us to.
     * @return \c true if \p{lhs} equals \p{rhs}, \c false otherwise.
     */
    virtual inline
    bool Invoke( const Box& lhs, const Box& rhs )                                           override
    {
        return *lhs.Unbox<TComparable>() ==  *rhs.Unbox<TComparable>();
    }
};

#define ALIB_BOXING_DEFINE_IEQUALS_FOR_COMPARABLE_TYPE(TComparable)                                \
aworx::lib::boxing::DefineInterface<TComparable ,                                                  \
                                    false,                                                         \
                                    aworx::lib::boxing::IEquals_TComparable<TComparable>>();




/** ************************************************************************************************
 * This is one of the built-in \alib{boxing,Interface} classes of <b>%ALib %Boxing</b>.
 * This interface is invokable with all boxable types, as a default implementation is registered
 * (see \alib{boxing,Boxer::DefineDefaultInterface}.
 * The default implementation is provided with this base interface class itself.
 *
 * The default implementation compares the type and if it is the same, just compares the stored
 * raw values against each other. Specifics for array types are \b not implemented.
 *
 * This leads to a nested sort order, with the type information being the outer order and
 * the boxed data being the inner.
 *
 * Type-specific implementations should use the following implementation scheme:
 *
 *      if( rhs.IsType<AComparableType1>() )
 *          return lhs.Unbox<MyTypbe>() < comp.Unbox<AComparableType>();
 *
 *      if( rhs.IsType<AComparableType2>() )
 *          return lhs.Unbox<MyType>() < comp.Unbox<AComparableType2>();
 *
 *      if( ...
 *          return ...
 *
 *      return std::type_index( lhs.GetTypeInfo() ) < std::type_index( rhs.GetTypeInfo() );
 *
 * The important thing is the last line: if types are not comparable the result of the comparison
 * of the \c std::type_index of the two boxed types is returned.
 * Only with this, for example \c std::sort will work properly on containers of boxes of
 * mixed types.
 *
 * \note
 *   It is a matter of the compiler (and can not be determined by the user code) how the
 *   types are sorted (outer sorting). Furthermore, the data value might not be reasonably comparable
 *   between boxes of the same type. "Reasonably" here means in respect to retrieving a
 *   reasonable sort order.
 *
 *   Consequently, the use of this comparison operator with custom types is subject to
 *   "knowing what I am doing" and knowing how the comparison is implemented for
 *   those types.
 *
 *   With boxing \alib{boxing,ftypes,C++ fundamental types}, interfaces are provided that
 *   allow to compare singed against unsigned integers and both against double values.
 *
 * This interface is invoked by \alib{boxing,Box::operator<}. The following sample demonstrates
 * this:
 *   \snippet "DOX_ALIB_BOXING.cpp"     DOX_ALIB_BOXING_COMPARE
 *
 * It generates the following output:
 *
 *   \snippet "DOX_ALIB_BOXING_COMPARE.txt"     OUTPUT
 *
 **************************************************************************************************/
class IIsLess : public Interface
{
    public:

    /**
     * Declares a static singleton used for the default interface registration and returns that.
     * \note We do not inherit template class Singleton here, because each of our descendants
     *       needs to do this as well, which would lead to some confusion.
     * @return A singleton instance of myself.
     */
    static IIsLess* GetDefaultSingleton()
    {
        static IIsLess me;
        return &me;
    }

    /**
     * Constructor providing our runtime type to the parent
     */
    IIsLess() : boxing::Interface( typeid(IIsLess) ) {}

    /**
     * Overrideable method that evaluates if two boxed object is smaller than another.
     * The default implementation just compares the raw data.
     *
     * @param lhs  The box that the interface was invoked on.
     * @param rhs  The boxed value to compare us to.
     * @return \c true if \p{lhs} is smaller than \p{rhs}, \c false otherwise.
     */
    virtual bool Invoke( const Box& lhs, const Box& rhs );
};



/** ************************************************************************************************
 * This is one of the built-in \ref aworx::lib::boxing::Interface "Interface" classes of
 * <b>%ALib %Boxing</b>. This interface is invokable with all boxable types, as a
 * default implementation is registered (see
 * \ref aworx::lib::boxing::Boxer::DefineDefaultInterface "Boxer::DefineDefaultInterface").
 *
 * The availability of this interface is the basis to implement the concept of
 * <em>"nullable types"</em>, e.g. pointer types or custom types that (already) support the concept
 * of being \e nullable.
 *
 * The default implementation returns \c true if the raw value of box data equals \c 0, otherwise
 * \c false. This gives a good default behaviour for all boxable types
 * stored as pointers to the original object.
 *
 * Derived class \ref aworx::lib::boxing::IIsNull_false "IIsNull_false" is provided which returns
 * constant value \c false. This may be registered with custom types that are supposed to never be
 * \e nulled.
 * The default implementation of
 * \ref alib_namespace_boxing_types_ftypes "boxing of fundamental types" will add this
 * specialization to integer and floating point types. Hence boxed values of these types will
 * never be considered \e nulled, even if they represent value \c 0.
 **************************************************************************************************/
class IIsNull : public Interface
{
    public:

    /**
     * Declares a static singleton used for the default interface registration and returns that.
     * \note We do not inherit template class Singleton here, because each of our descendants
     *       needs to do this as well, which would lead to some confusion.
     * @return A singleton instance of myself.
     */
    static IIsNull* GetDefaultSingleton()
    {
        static IIsNull me;
        return &me;
    }

    /**
     * Constructor providing our runtime type to the parent
     */
    IIsNull() : Interface( typeid(IIsNull) ) {}

    /**
     * See class description.
     *
     * @param box   The box that the interface was invoked on.
     * @return \c true if \p{box} represents a nulled value, \c false otherwise.
     */
    virtual bool Invoke( const Box& box )
    {
        return box.PeekRawValue() == 0;
    }
};

/** ************************************************************************************************
 * This specialization of interface #IIsNull returns constant \c false.
 * It may be registered with custom types that do not provide the concept of being \e nulled.
 * The default implementation of
 * \ref alib_namespace_boxing_types_ftypes "boxing of fundamental types" will add this
 * specialization to integer and floating point types.
 **************************************************************************************************/
class IIsNull_false : public IIsNull , public Singleton<IIsNull_false>
{
    public:

    /**
     * Returns constant \c false. See class description for more information.
     *
     * @param box   The box that the interface was invoked on. This value is ignored.
     * @return Constant \c false.
     */
    virtual bool Invoke( const Box& box )
    {
        (void) box;
        return false;
    }
};


/** ************************************************************************************************
 * This is one of the built-in \ref aworx::lib::boxing::Interface "Interface" classes of
 * <b>%ALib %Boxing</b>. This interface is invokable with all boxable types, as a
 * default implementation is registered (see
 * \ref aworx::lib::boxing::Boxer::DefineDefaultInterface "Boxer::DefineDefaultInterface").
 *
 * The availability of this interface is the basis to provide the concept of
 * <em>"emptiness"</em> of boxed objects.
 * E.g. array and string types are candidates where such definition makes sense.
 *
 * For array types (see \ref aworx::lib::boxing::ArrayBoxerT "ArrayBoxerT"), the default
 * implementation returns \c true if method \alib{boxing,Box::Length} returns \c 0 for \p{box}.
 *
 * For non-array types this default implementation returns the result of the invocation of
 * interface #IIsNull on \p{box}.
 * This way, by default the concept of <em>"emptiness of a value"</em> is equal
 * to the concept of <em>"nullable values"</em> for non-array types.
 * In combination with the fact that the default implementation of
 * \ref alib_namespace_boxing_types_ftypes "boxing of fundamental types" will add a specialization
 * of \b %IIsNull that returns constant false to integer and floating point types, such types
 * will never be considered "empty", even if they represent value \c 0.
 **************************************************************************************************/
class IIsEmpty : public Interface
{
    public:

    /**
     * Declares a static singleton used for the default interface registration and returns that.
     * \note We do not inherit template class Singleton here, because each of our descendants
     *       needs to do this as well, which would lead to some confusion.
     * @return A singleton instance of myself.
     */
    static IIsEmpty* GetDefaultSingleton()
    {
        static IIsEmpty me;
        return &me;
    }

    /**
     * Constructor providing our runtime type to the parent
     */
    IIsEmpty() : Interface( typeid(IIsEmpty) ) {}

    /**
     * Overridable method that evaluates if the provided box object is \e "empty".
     *
     * @param box   The box that the interface was invoked on.
     * @return \c true if \p{box} represents an empty value, \c false otherwise.
     */
    ALIB_API
    virtual bool Invoke( const Box& box )
    {
        return box.IsArray() ? box.Length() == 0
                             : box.Invoke<IIsNull, bool>();
    }
};

/** ************************************************************************************************
 * This is one of the built-in \ref aworx::lib::boxing::Interface "Interface" classes of
 * <b>%ALib %Boxing</b>. This interface is invokable with all boxable types, as a
 * default implementation is registered (see
 * \ref aworx::lib::boxing::Boxer::DefineDefaultInterface "Boxer::DefineDefaultInterface").
 *
 * This interface is used to give an answer to the question if a boxed value represents boolean
 * value \c true or \c false. This is useful if "yes/no" decisions should be taken based on
 * arbitrary boxed values.
 *
 * For non-array types, the raw boxed value is taken: If it is not \c 0, \c true is returned,
 * otherwise \c false.
 *
 * For array types (see \ref aworx::lib::boxing::ArrayBoxerT "ArrayBoxerT"), the default
 * implementation returns \c true if method \alib{boxing,Box::Length} returns a value different
 * to \c 0, otherwise \c false is returned.
 **************************************************************************************************/
class IIsTrue : public Interface
{
    public:

    /**
     * Declares a static singleton used for the default interface registration and returns that.
     * \note We do not inherit template class Singleton here, because each of our descendants
     *       needs to do this as well, which would lead to some confusion.
     * @return A singleton instance of myself.
     */
    static IIsTrue* GetDefaultSingleton()
    {
        static IIsTrue me;
        return &me;
    }

    /**
     * Constructor providing our runtime type to the parent
     */
    IIsTrue() : Interface( typeid(IIsTrue) ) {}

    /**
     * Overridable method that evaluates if the provided box object is \e "true".
     *
     * @param box   The box that the interface was invoked on.
     * @return \c true if \p{box} represents a "true-value", \c false otherwise.
     */
    ALIB_API
    virtual bool Invoke( const Box& box )
    {
        return box.IsArray() ? box.Length      () != 0
                             : box.PeekRawValue() != 0;
    }
};


}} // namespace aworx[::lib::boxing]

/// Type alias in namespace #aworx.
using     IEquals    =   aworx::lib::boxing::IEquals;

/// Type alias in namespace #aworx.
using     IIsNull    =   aworx::lib::boxing::IIsNull;

/// Type alias in namespace #aworx.
using     IIsEmpty   =   aworx::lib::boxing::IIsEmpty;

/// Type alias in namespace #aworx.
using     IIsLess    =   aworx::lib::boxing::IIsLess;

/// Type alias in namespace #aworx.
using     IIsTrue    =   aworx::lib::boxing::IIsTrue;
} // namespace aworx

#endif // HPP_ALIB_BOXING_INTERFACE_BUILTIN
