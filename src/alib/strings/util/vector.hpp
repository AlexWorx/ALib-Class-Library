//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
ALIB_EXPORT namespace alib { namespace strings::util {

//==================================================================================================
/// This is a simple type that publicly inherits container type <c>std::vector</c> to store a
/// list of \alib strings, using an \alib {lang;Allocator}.
///
/// The (current) design of this class could be named "open" or "weak". This is due to the fact
/// that
/// a) The interface of the \c std::vector is public by inheritance, and
/// b) Only a few additional interface methods have been added.
///
/// As a consequence, it is up to the user of the type to care about proper allocation and
/// deallocation of string data: If a string is added using method #".Add", its content is copied
/// to memory allocated with the allocator provided with construction. However, any other string may
/// be added by using the interface of the \c std::vector.
///
/// The typical use case is with class #"TMonoAllocator" or
/// #"TLocalAllocator" provided with module \alib_monomem:
/// - Create a Mono- or LocalAllocator
/// - Create a #"StringVectorMA", passing the allocator.
/// - Gather some string data (copied or otherwise referenced).
/// - Pass it over to a function or otherwise use the vector.
/// - Destruct the objects.
///
/// \attention
///   With other use cases, especially when using type #"StringVectorPA" in combination with
///   class #"TPoolAllocator;PoolAllocator", it has to be well thought how this type
///   is used in respect to the need of freeing memory, especially when strings got allocated with
///   method #".Add".
///
/// @tparam TAllocator The allocator type, as prototyped with class #"lang::Allocator".
//==================================================================================================
template<typename TChar, typename TAllocator>
class TStringVector  : public std::vector< TString<TChar>,
                                           lang::StdAllocator<TString<TChar>, TAllocator> >
{
  public:
    /// The allocator type that \p{TAllocator} specifies.
    using AllocatorType = TAllocator;

  protected:
    /// The vector type that \p{TAllocator} specifies.
    using vectorBase    = std::vector<TString<TChar>,
                                      lang::StdAllocator<TString<TChar>, TAllocator>>;

  public:
    /// Constructor.
    TStringVector()
    : vectorBase()                                                                                {}

    /// Constructor taking an allocator.
    /// @param pAllocator The allocator to use.
    TStringVector(AllocatorType& pAllocator)
    : vectorBase(pAllocator)                                                                      {}

    /// Destructor.
    ~TStringVector()                                                                       =default;

    /// Returns the allocator provided with construction. If this was \e nulled, then this method
    /// can be used to set an external (or new) allocator using the assignment operator.
    /// Has to be set before the first insertion of data.
    ///
    /// @return A reference to the internal allocator.
    AllocatorType&    GetAllocator()                                                        noexcept
    { return vectorBase::get_allocator().GetAllocator(); }

    /// Returns the size of this vector as #"lang::integer".
    /// @return The number of strings stored.
    integer Size()                            const noexcept { return integer(vectorBase::size()); }

    /// Adds a string to the end of the list of strings.
    /// @param src Source string to be copied.
    /// @return The index of the created string in this vector.
    integer Add(const strings::TString<TChar>& src )
    {
        vectorBase::emplace_back( TString<TChar>( GetAllocator(), src ) );
        return integer(vectorBase::size()) - 1;
    }

    /// Receives the string at a valid \p{idx}. If the index is out of bounds, a nulled string
    /// is returned.
    /// @param idx The index to try.
    /// @return The string stored at \p{idx}, if available.
    String TryGet(integer idx ) {
        if (idx >= 0 && idx < Size())
            return vectorBase::at(size_t(idx));

        return NULL_STRING;
    }

}; // class TStringVector

} // namespace alib[::strings::util]

using  StringVector= strings::util::TStringVector<character, lang::HeapAllocator>; ///< Type alias in namespace #"%alib".
using NStringVector= strings::util::TStringVector<nchar    , lang::HeapAllocator>; ///< Type alias in namespace #"%alib".
using WStringVector= strings::util::TStringVector<wchar    , lang::HeapAllocator>; ///< Type alias in namespace #"%alib".

#if ALIB_MONOMEM
  using   StringVectorMA= strings::util::TStringVector<character,  MonoAllocator>; ///< Type alias in namespace #"%alib".
  using  NStringVectorMA= strings::util::TStringVector<nchar    ,  MonoAllocator>; ///< Type alias in namespace #"%alib".
  using  WStringVectorMA= strings::util::TStringVector<wchar    ,  MonoAllocator>; ///< Type alias in namespace #"%alib".
  using   StringVectorPA= strings::util::TStringVector<character,  PoolAllocator>; ///< Type alias in namespace #"%alib".
  using  NStringVectorPA= strings::util::TStringVector<nchar    ,  PoolAllocator>; ///< Type alias in namespace #"%alib".
  using  WStringVectorPA= strings::util::TStringVector<wchar    ,  PoolAllocator>; ///< Type alias in namespace #"%alib".
#endif

} // namespace [alib]
