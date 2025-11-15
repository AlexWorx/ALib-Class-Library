//==================================================================================================
/// \file
/// This header-file is part of module \alib_alox of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib {  namespace lox { namespace detail {

//==================================================================================================
/// This class is an internal helper to export current scope values.
/// It was extracted from class \b %Lox to keep that class clean from this somehow 'cluttered' code.
///
/// The current purpose of this class is to write information into an AString buffer. This is used
/// by method \ref alib::lox::Lox::State "Lox::State". In the future, other export formats,
/// like JSON might be implemented here.
///
/// \note As this is a pure internal helper-class. Documentation may be limited.
//==================================================================================================
class ScopeDump
{
  protected:
    /// The target to write to.
    NAString&                       targetBuffer;

    /// String to identify global keys.
    const NString                   noKey;

    /// The maximum length of a key. Adjusts (increases) over life-cycle.
    integer                         maximumKeyLength;

  #if !ALIB_SINGLE_THREADED
    /// User-defined threads names.
    ScopeInfo::ThreadDictionary&    threadDict;
  #endif

  public:

  #if DOXYGEN
    /// Constructor
    /// @param threadDictionary User-defined threads names.
    /// @param target           Target string to write to.
    /// @param noKeyHashKey     String to identify global keys.
    /// @param maxKeyLength     The start value for the maximum length of a key.
    ScopeDump( ScopeInfo::ThreadDictionary& threadDictionary, const NString noKeyHashKey,
               NAString& target, int maxKeyLength= 10 )
    : targetBuffer(target)
    , noKey(noKeyHashKey)
    , maximumKeyLength(maxKeyLength)
    , threadDict(threadDictionary)
    {}
  #else
    ScopeDump( IF_ALIB_THREADS( ScopeInfo::ThreadDictionary& threadDictionary,)
               const NString noKeyHashKey, NAString& target, int maxKeyLength= 10 )
    : targetBuffer(target)
    , noKey(noKeyHashKey)
    , maximumKeyLength(maxKeyLength)
    IF_ALIB_THREADS(, threadDict(threadDictionary) )                                              {}
  #endif

    /// Writes hash tables stored in a ScopeStore. Keys are Strings.
    /// Value types currently supported are LogData* and int.
    /// @param store The store to use.
    /// @return The total number of hash table entries written.
    template<typename T>
    ALIB_DLL
    int writeStoreMap( ScopeStore<T, false>* store );


    /// Writes the contents of the ScopeStore used for <em>Scope Domains</em>.
    /// @param store         The store to use
    /// @param indentSpaces  The number of spaces to add at the beginning of each line.
    /// @return The total number of <em>Scope Domains</em> written.
    template<typename T>
    ALIB_DLL
    int writeStore( ScopeStore<T,  true>* store, int indentSpaces );

  //################################################################################################
  // Internals
  //################################################################################################
  protected:
    /// Helper method to #writeStoreMap.
    /// @param map       The map to use
    /// @param prefix    A prefix string for each line
    /// @return The number of objects written
    template<typename T>
    ALIB_DLL
    integer writeStoreMapHelper(SSMap<T>& map, const NString& prefix );

  #if !ALIB_SINGLE_THREADED
    /// Helper method to write thread information.
    /// @param threadID  The thread id.
    /// @return The target to allow concatenated calls.
    ALIB_DLL
    NAString& storeThreadToScope( ThreadID threadID );
  #endif

    /// Helper method to write a StringTree's key as scope information.
    /// @param key  The key used for the \b %StringTree.
    /// @return The target to allow concatenated calls.
    ALIB_DLL
    NAString& storeKeyToScope( String key );

}; // ScopeDump


//! @cond NO_DOX
// needed to suppress pedantic warnings with clang
extern template   int ScopeDump::writeStore   ( ScopeStore<NString                 , true >* store, int indentSpaces );
extern template   int ScopeDump::writeStore   ( ScopeStore<PrefixLogable*          , true >* store, int indentSpaces );
extern template   int ScopeDump::writeStoreMap( ScopeStore<SSMap<int>* , false>* store );
extern template   int ScopeDump::writeStoreMap( ScopeStore<SSMap<Box>* , false>* store );
//! @endcond


}}} // namespace [alib::lox::detail]
