//==================================================================================================
/// \file
/// This header-file is part of module \alib_boxing of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#if ALIB_DEBUG_BOXING
#include "ALib.Lang.CIFunctions.H"
ALIB_EXPORT namespace alib::boxing::debug {

//==================================================================================================
/// Template class implementing a monotonically allocated vector of variadic tuples whose first
/// element is of type \alib{strings;TString<TChar>;String}.
///
/// When new tuples are added with the method #Add, then memory for copying the provided string is
/// allocated using an internal field of type \alib{MonoAllocator}.
/// This allows adding single strings, which are allocated in bigger memory buffers.
///
/// Standard vector operations, including insertions and deletions are still allowed.
/// In other words, it is allowed to store string objects which are pointing to string data that
/// is \b not allocated in the internal buffers.
///
/// The associated values are especially useful when the vector is sorted (e.g., using
/// \c std::sort), because such associations will be kept intact and allow referencing back
/// to whatever the strings represents.
///
/// \par Availability
///   This function is an extension, which is injected by the higher-level module \alib_format and
///   is accessed through the header file \implude{Format}.
///   Furthermore, the compiler-symbol \ref ALIB_DEBUG_BOXING has to be set.
//==================================================================================================
template< typename... TAssociatedTypes >
struct DbgStringTable  : public std::vector<std::tuple<String, TAssociatedTypes...>>
{
   /// Shortcut to the <c>std::tuple</c>-type that instantiations of this template class store.
   using ElementType= std::tuple<String, TAssociatedTypes...>;

   /// Shortcut to the base type of this type.
   using Base       = std::vector<ElementType>;

   /// Allocator for making copies of inserted strings
   MonoAllocator&   allocator;

   /// Constructor.
   /// @param monoAllocator   A mono allocator used to create copies of the inserted strings
   DbgStringTable( MonoAllocator&  monoAllocator )
   : allocator(monoAllocator)
   {}

   /// Adds a tuple. The contents of the string of the tuple (its first element) is copied
   /// from the given string \p{src}. The memory for this string is allocated using
   /// the allocator provided in the constructor.<br>
   /// The other members of the tuple added are forwarded from variadic parameter block \p{args}.
   ///
   /// @param src   The string to copy into the first member of the tuple.
   /// @param args  Variadic arguments to fill the rest of the inserted tuple.
   ///
   /// @return A reference to the string that was added.
   template <typename... TArgs>
   String& Add( const String& src, TArgs&&... args )
   {
       // on errors, see note above!
       Base::emplace_back( String(allocator, src), std::forward<TArgs>(args)... );
       return std::get<0>( std::vector<std::tuple<String, TAssociatedTypes...>>::back() );
   }
};

/// See function #TypeInfo, which invokes this function using template parameter \p{TBoxable}
///
/// \par Availability
///   This function is an extension, which is injected by the higher-level module \alib_format and
///   is accessed through the header file \implude{Format}.
///   Furthermore, the compiler-symbol \ref ALIB_DEBUG_BOXING has to be set.
///
/// @param[out] target                   The target string.
/// @param      vtable                   The vtable of the mapped type.
/// @param      indent                   Empty or a string of spaces that are alwys preprended
///                                      before each entry.
/// @param      srcIsPointer             Denotes if \p{TBoxable} is a pointer type.
/// @param      srcIsStringType          Denotes if \p{TBoxable} is a string-type in the sense
///                                      that a specialization of the
///                                      struct \alib{characters;ArrayTraits} allows implicit access
///                                      to character array data.
/// @param      srcIsUnboxableStringType Denotes if \p{TBoxable} is a string-type that can,
///                                      due to a specialization of the
///                                      struct \alib{characters;ArrayTraits} be implicitly
///                                      constructed from character array types.
/// @param      isValueTypeCustomized    Denotes if the value type of \p{TBoxable} is customized.
/// @param      isPointerTypeCustomized  Denotes if the pointer type of \p{TBoxable} is
///                                      customized.
/// @param      fitsToPlaceholder        Denotes if the value type of \p{TBoxable} would fit to
///                                      placeholder.
/// @param      copyConstructible        Denotes if the value type of \p{TBoxable} is
///                                      copy constructible.
/// @param      triviallyDestructible    Denotes if the value type of \p{TBoxable} is
///                                      trivially destructible.
/// @param      isUnboxable              Denotes if \p{TBoxable} is unboxable.
ALIB_DLL
void            typeInfo( AString&                 target,
                          const detail::VTable*    vtable,
                          const String&            indent,
                          bool                     srcIsPointer,
                          bool                     srcIsStringType,
                          bool                     srcIsUnboxableStringType,
                          bool                     isValueTypeCustomized,
                          bool                     isPointerTypeCustomized,
                          bool                     fitsToPlaceholder,
                          bool                     copyConstructible,
                          bool                     triviallyDestructible,
                          bool                     isUnboxable                         );


/// Collects necessary information from template type \p{TBoxable} and invokes
/// #typeInfo.<br>
/// See function #TypeInfo.
///
/// \par Availability
///   This function is an extension, which is injected by the higher-level module \alib_format and
///   is accessed through the header file \implude{Format}.
///   Furthermore, the compiler-symbol \ref ALIB_DEBUG_BOXING has to be set.
///
/// @tparam TBoxable The boxable type to investigate in.
/// @param  box      A box containing a "sample" of \p{TBoxable.}. If this box contains a different
///                  type than \p{TBoxable}, this function has undefined behavior.
/// @param  headline The headline to write.
/// @param  indent   Spaces to write before each entry.
/// @return A string containing the requested information.
template<typename TBoxable>
AString  TypeInfo( const Box& box,
                   const String& headline= A_CHAR("Boxing Information For Boxable Type: "),
                   const String& indent  = A_CHAR("  "                                   ) )
{
    AString target;
    using TSrc= std::remove_cv_t<TBoxable>;
    using TVal= ALIB_TVALUE(TBoxable);

    // repeat twice to get auto-tabs adjusted
    ALIB_LOCK_RECURSIVE_WITH(Formatter::DefaultLock)
    Formatter& formatter= *Formatter::Default;
    formatter.Reset();
    for( int theSakeOfAutoTabs= 0 ; theSakeOfAutoTabs < 2 ; ++theSakeOfAutoTabs )
    {
        NString512 demangled(typeid(TSrc) );
        target.Reset() << headline << demangled  << NEW_LINE;
        removeNamespaces( target, headline.Length() );

        if( std::same_as<TSrc, std::nullptr_t> )
        {
            target  << indent << "'std::nullptr_t' sets a box to void state" << NEW_LINE;
            return target;
        }

        typeInfo( target,
                  box.DbgGetVTable(),
                  indent,
                  std::is_pointer_v<TSrc>,
                  boxing::IsStringType<TVal>,
                  boxing::IsUnboxableStringType<TVal>,
                  boxing::IsCustomized<TVal >,
                  boxing::IsCustomized<TVal*>,
                  sizeof(boxing::Placeholder) <= sizeof(TVal),
                  std::is_copy_constructible    <TVal>::value,
                  std::is_trivially_destructible<TVal>::value,
                  boxing::IsUnboxable <TSrc   >                            );

    }
    return target;
}

/// Returns human-readable information about the type \p{TBoxable} in respect to how boxing
/// objects of this type is performed.
///
/// The implementation of this function creates a default value of \p{TBoxable}, boxes this
/// and returns the result of #TypeInfo(const Box&,const String&,const String&).
/// Hence, for types that are not default constructible, inner function \b %TypeInfo
/// has to be used with an accordingly boxed "sample" of type \p{TBoxable}
///
/// \note
///   Does not work for type \c void*, which is not customizable anyhow
///   (always boxed as \c void*).
///
///
/// \par Availability
///   This function is an extension, which is injected by the higher-level module \alib_format and
///   is accessed through the header file \implude{Format}.
///   Furthermore, the compiler-symbol \ref ALIB_DEBUG_BOXING has to be set.
///
/// @tparam TBoxable The boxable type to investigate in.
/// @param  headline The headline to write.
/// @param  indent   Spaces to write before each entry.
/// @return A string containing the requested information.
template<typename TBoxable>
AString  TypeInfo( const String& headline = A_CHAR( "Boxing Information For Boxable Type: ") ,
                   const String& indent   = A_CHAR( "  "                                   )   )

{ return TypeInfo<TBoxable>( TBoxable(), headline, indent ); }


// #############################################################################################
// Dump
// #############################################################################################
/// Takes a vector of pairs of \c std::type_info pointers and a usage counter and returns an
/// \b AString with a sorted list of type names, including their counter information.
///
/// \par Availability
///   This function is an extension, which is injected by the higher-level module \alib_format and
///   is accessed through the header file \implude{Format}.
///   Furthermore, the compiler-symbol \ref ALIB_DEBUG_BOXING has to be set.
///
/// @param input             A list of types.
/// @param headline          The headline to write.
/// @param indent            Spaces to write before each entry.
/// @return A list of demangled type names.
ALIB_DLL
alib::AString  DumpFunctions(
       const std::vector<std::pair<const std::type_info*,uinteger>>& input,
       const String&                                                 headline = EMPTY_STRING,
       const String&                                                 indent   = EMPTY_STRING );

/// Helper for (bigger part of) #DumpFunctions.
///
/// \par Availability
///   This function is an extension, which is injected by the higher-level module \alib_format and
///   is accessed through the header file \implude{Format}.
///   Furthermore, the compiler-symbol \ref ALIB_DEBUG_BOXING has to be set.
/// @param      input            A list of types and usage numbers.
/// @param[out] output           A string to write to.
/// @param      headline         The headline to write.
/// @param      indent           Spaces to write before each entry.
/// @param      tmpStrings       A buffer needed for internal use.
ALIB_DLL
void dumpFunctions ( const std::vector<std::pair<const std::type_info*,uinteger>>& input,
                     AString&                                                      output,
                     const String&                                                 headline,
                     const String&                                                 indent,
                     DbgStringTable<uinteger>&                                     tmpStrings );


/// Lists all mapped types with either static or dynamic \e vtables.
/// Along with each type, its default function implementations are given.
///
/// \par Availability
///   This function is an extension, which is injected by the higher-level module \alib_format and
///   is accessed through the header file \implude{Format}.
///   Furthermore, the compiler-symbol \ref ALIB_DEBUG_BOXING has to be set.
/// @param staticVtables    If \c true, only types with static vtables are listed.
///                         Otherwise only those with dynamic vtables.
/// @param includeFunctions If \c true for each type, the list of specialized functions are
///                         listed.
/// @return A string containing the dump.
ALIB_DLL
AString         DumpVTables( bool staticVtables,
                             bool includeFunctions= false  );

/// Internally used by functions #DumpVTables and #DumpAll.
///
/// \par Availability
///   This function is an extension, which is injected by the higher-level module \alib_format and
///   is accessed through the header file \implude{Format}.
///   Furthermore, the compiler-symbol \ref ALIB_DEBUG_BOXING has to be set.
/// @param[out] target           The target string to write to.
/// @param[out] vtableNames      Output parameter that receives the strings.
/// @param      staticVtables    If \c true, only types with static vtables are listed.
///                              Otherwise only those with dynamic vtables.
/// @param      includeFunctions If \c true, For each \e vtable the list of specialized
///                              function is written.
ALIB_DLL
void     dumpVTables( AString&                                 target,
                      DbgStringTable<const detail::VTable*>&   vtableNames,
                      bool                                     staticVtables,
                      bool                                     includeFunctions );

#if ALIB_DEBUG_CONTAINERS
    /// Invokes \alib{containers;DbgDumpDistribution} on the internal hash table used to register
    /// and fetch implementations of custom box-functions.
    ///
    /// \par Availability
    ///   This function is an extension, which is injected by the higher-level module \alib_format
    ///   and is accessed through the header file \implude{Format}.
    ///   Furthermore, the compiler-symbols \ref ALIB_DEBUG_BOXING and \ref ALIB_DEBUG_CONTAINERS
    ///   are set.
    /// @param[out] target              The target string to write to.
    /// @param      detailedBucketList  If \c true is given, for each bucket a line with its size
    ///                                 value and a "size bar" is written.
    ALIB_DLL
    void            DumpCustomFunctionHashMapMetrics( AString& target, bool detailedBucketList );

#endif

/// First, lists all mapped types with static, then those with dynamic \e vtables.
/// Along with each type, its default function implementations are given.
///
/// \par Availability
///   This function is an extension, which is injected by the higher-level module \alib_format and
///   is accessed through the header file \implude{Format}.
///   Furthermore, the compiler-symbol \ref ALIB_DEBUG_BOXING has to be set.
/// @return A string containing the dump.
ALIB_DLL
AString         DumpAll   ();

}; // namespace [alib::boxing::debug]

#include "ALib.Lang.CIMethods.H"
#endif // ALIB_DEBUG_BOXING

