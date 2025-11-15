//==================================================================================================
/// \file
/// This header-file is part of module \alib_lang of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib::lang {

//==================================================================================================
/// Templated helper that reserves memory of size and alignment corresponding to type \p{T}.
/// Along with that, construction, destruction and reinterpretation casts are given in
/// various ways.
///
/// The type is used for class members or (less frequently for local variables), whose construction
/// has to be deferred.
/// @tparam T    The type to replace.
//==================================================================================================
template<typename T>
struct  alignas(alignof(T))  Placeholder
{
    /// the placeholder space.
    char bytes[sizeof(T)];

    /// Constructor. Initializes the occupied memory with 0-bytes.
    Placeholder()                                                                                 {}

    /// Constructs the custom type \p{T} represented by this placeholder.
    /// @tparam TArgs  The argument types used for constructing \p{T}.
    /// @param  args   The arguments to construct the instance of \p{T}.
    template<typename... TArgs>
    void     Construct( TArgs&&... args )          { new (this)  T(std::forward<TArgs>(args)... ); }

    /// Destructs the custom type \p{T} represented by this placeholder.
    void     Destruct()                                                { lang::Destruct( *Get() ); }

    /// Returns a non-constant pointer to the represented instance of type \p{T}.
    /// @return A pointer to \p{T}.
    T*       Get()                                            { return reinterpret_cast<T*>(this); }

    /// Returns a constant pointer to the represented instance of type \p{T}.
    /// @return A pointer to \p{T}.
    const T* Get()                                      const { return reinterpret_cast<T*>(this); }

    /// Overloaded operator to access the represented instance of type \p{T}.
    /// @return A pointer to \p{T}.
    T*       operator->()                                                          { return Get(); }

    /// Overloaded operator to access the represented instance of type \p{T}.
    /// @return A constant pointer to \p{T}.
    const T* operator->()                                                    const { return Get(); }

    /// Overloaded operator to access the represented instance of type \p{T}.
    /// @return A pointer to \p{T}.
    T&       operator*()                                                          { return *Get(); }

    /// Overloaded operator to access the represented instance of type \p{T}.
    /// @return A constant pointer to \p{T}.
    const T& operator*()                                                    const { return *Get(); }
}; // struct Placeholder

} // namespace alib[::lang]
