//==================================================================================================
/// \file
/// This header-file is part of module \alib_monomem of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
ALIB_EXPORT namespace alib {  namespace monomem {

//==================================================================================================
/// A mono allocator, that incorporates a member of templated size \p{TCapacityInKB}, which is used
/// as the initial buffer for allocation. The class is intended to be instantiated by local
/// variables, hence using stack memory.<br>
/// When the initial buffer is exhausted, further buffers will be heap allocated.
///
/// Note that the #"alib_manual_appendix_typealiases;usual type alias" given in namespace #"alib",
/// in this case misses the leading <em>"T"</em>, and is given with #"LocalAllocator".
/// Besides that, further aliases that denote specific stack sizes are provided with
/// #"LocalAllocator1K",
/// #"LocalAllocator2K",
/// #"LocalAllocator4K",
/// #"LocalAllocator8K",
/// #"LocalAllocator16K",
/// #"LocalAllocator32K", and
/// #"LocalAllocator64K".
///
/// @see Chapter #"alib_contmono_class_monoalloc_local" of the Programmer's Manual of this
///      \alibcamp_nl.
/// @tparam TCapacityInKB The size of the internal buffer in kilobytes.
/// @tparam TAllocator    The allocator type that parent #"TMonoAllocator" should
///                       be #"alib_contmono_chaining;chained" to.<br>
///                       Defaults to #"HeapAllocator".
//==================================================================================================
template<size_t TCapacityInKB, typename TAllocator= lang::HeapAllocator>
class TLocalAllocator : public TMonoAllocator<TAllocator> {
  protected:
    /// Shortcut to our base type.
    using base=  TMonoAllocator<TAllocator>;

    /// Internal memory passed as a first buffer to base class #"%MonoAllocator"
    void* localMemory[TCapacityInKB*1024/sizeof(void*)];

  public:
    /// Default constructor.
    /// @param pBufferGrowthInPercent Optional growth factor in percent, applied to each allocation
    ///                               of a next buffer size in respect to its previous size.
    ///                               Defaults to \c 200, which doubles buffer size with each
    ///                               next internal buffer allocation.
    TLocalAllocator(unsigned pBufferGrowthInPercent= 200)
    : base( ALIB_DBG("LocalAllocator",)
            new (localMemory) detail::Buffer( TCapacityInKB*1024),
            TCapacityInKB,
            pBufferGrowthInPercent )                                                              {}

    /// Constructor that accepts a different underlying allocator
    /// @param pAllocator             The allocator to use for allocation of additional buffers.
    /// @param pBufferGrowthInPercent Optional growth factor in percent, applied to each allocation
    ///                               of a next buffer size in respect to its previous size.
    ///                               Defaults to \c 200, which doubles buffer size with each
    ///                               next internal buffer allocation.
    TLocalAllocator(TAllocator& pAllocator, unsigned pBufferGrowthInPercent= 200)
    : base( ALIB_DBG("LocalAllocator",)
            pAllocator,
            new (localMemory) detail::Buffer( TCapacityInKB*1024),
            TCapacityInKB,
            pBufferGrowthInPercent)                                                               {}

    /// Destructor. Calls #"destructWithExternalBuffer".
    ~TLocalAllocator()                                       { base::destructWithExternalBuffer(); }

    /// Convenience method that returns <c>*this</c> statically cast to base type
    /// #"%MonoAllocator". This is primarily needed in situations where overload resolution
    /// of methods fails, if not exactly this base type is given.
    /// A prominent sample for this is the constructor of type #"AStringMA".
    /// @return A reference to \p{*this}, cast to the base type.
    MonoAllocator& AsMonoAllocator()                  { return static_cast<MonoAllocator&>(*this); }

}; // class TLocalAllocator

}  // namespace alib[::monomem]

/// Type alias in namespace #"%alib".
template<size_t TCapacityInKB>
using     LocalAllocator    =   monomem::TLocalAllocator<TCapacityInKB>;

using     LocalAllocator1K  =   monomem::TLocalAllocator< 1>;  ///< Type alias in namespace #"%alib". Allocates 1kB of stack memory.
using     LocalAllocator2K  =   monomem::TLocalAllocator< 2>;  ///< Type alias in namespace #"%alib". Allocates 2kB of stack memory.
using     LocalAllocator4K  =   monomem::TLocalAllocator< 4>;  ///< Type alias in namespace #"%alib". Allocates 4kB of stack memory.
using     LocalAllocator8K  =   monomem::TLocalAllocator< 8>;  ///< Type alias in namespace #"%alib". Allocates 8kB of stack memory.
using     LocalAllocator16K =   monomem::TLocalAllocator<16>;  ///< Type alias in namespace #"%alib". Allocates 16kB of stack memory.
using     LocalAllocator32K =   monomem::TLocalAllocator<32>;  ///< Type alias in namespace #"%alib". Allocates 32kB of stack memory.
using     LocalAllocator64K =   monomem::TLocalAllocator<64>;  ///< Type alias in namespace #"%alib". Allocates 64kB of stack memory.

} // namespace [alib]
