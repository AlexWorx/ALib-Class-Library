//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
ALIB_EXPORT namespace alib { namespace system {

/// File-to-memory loader with optional mmap backend.
/// This class loads a file into memory and provides a stable pointer + length, which is only
/// allowed to be read sequentially.
///
/// ### Backend options: ###
/// - <b>mmap</b>-mapping where supported.
/// - Portable "read-all" into an internal buffer (always available, i.e on WindowsOS).
class MappedFile {
  public:
    /// A minimal sequential-only cursor over the file data.
    /// Enforces the "forward-only" usage pattern in calling code.
    /// @tparam T The data type to sequentially load from this cursor.
    template<typename T>
    class Data {
      protected:
        const T* start; ///< Current pointer position.
        const T* end;   ///< Pointer to the end of the data.

      public:
        /// Defaulted default-constructor.
        Data()                                                     : start(nullptr), end(nullptr) {}

        /// Constructor.
        /// @param pStart Pointer to the start of the data.
        /// @param length The length of the data.
        Data(const void* pStart, size_t length)
        : start(static_cast<const T*>(pStart))
        , end  (static_cast<const T*>(pStart) + length/sizeof(T)) {
            ALIB_ASSERT_ERROR((reinterpret_cast<size_t>(start) & (alignof(T) - 1)) == 0,
                   "SYSTEM",  "MappedFile::Data: start pointer is not aligned to T.")
            ALIB_ASSERT_ERROR((length % sizeof(T)) == 0,
                   "SYSTEM",  "MappedFile::Data: byte length is not a multiple of sizeof(T).")
            ALIB_ASSERT_ERROR((reinterpret_cast<size_t>(end) & (alignof(T) - 1)) == 0,
                   "SYSTEM",  "MappedFile::Data: end pointer is not aligned to T.")
        }

        /// Remaining value.
        /// @return The number of bytes from current position to end.
        integer     Remaining()                                                       const noexcept
        { return integer( ( end - start ) / std::ptrdiff_t(sizeof(T))); }

        /// True if at end.
        /// @return \c true if the current position reached #".end", \c false otherwise.
        bool        IsEOF()                                  const noexcept { return start >= end; }

        /// Get the next value and advance.
        /// @tparam TCheck  Defaults to #"alib::CHK;2", which is the normal invocation mode.
        ///                 If #"alib::NC;2" is given, no range check is performed.
        ///                 In debug builds, an assertion is raised if the cursor is at end.
        /// @return The value at current position.
        template <typename TCheck = alib::CHK>
        T           Next() {
            if constexpr ( TCheck::value ) {
                if (start>= end)
                    return T{0};
            } else
                ALIB_ASSERT_ERROR( start < end, "SYSTEM", "MappedFile::Data::Next: overflow." )
            return *start++;
        }

        /// Shortcut to method #".Next".
        /// @tparam TCheck  Defaults to #"alib::CHK;2", which is the normal invocation mode.
        ///                 If #"alib::NC;2" is given, no range check is performed.
        ///                 In debug builds, an assertion is raised if the cursor is at end.
        /// @return The value at current position.
        template <typename TCheck = alib::CHK>
        T operator  ()()                                                          { return Next(); }

        /// Get a pointer to the current position (read-only).
        /// @return Pointer to current position.
        const T     Current()                                      const noexcept { return *start; }

        /// Same as #".Current".
        /// @return Pointer to current position.
        const T     operator*()                                    const noexcept { return *start; }

        /// Advance by n values (must stay within range).
        /// @param  n Number of values to advance.
        /// @tparam TIntegral The type of the parameter \p{n}.
        template <typename TIntegral>
        requires std::integral<TIntegral>
        void        Skip(TIntegral n)                                                  { start+=n; }

        /// Clones this object to represent values of template type \p{U}.
        /// @tparam U The type of the cloned data instance
        /// @return A clone of this instance.
        template <typename U>
        Data<U>     Clone()                                                           const noexcept
        { return  Data<U>(start, size_t(end - start) * sizeof(T)); }
    };

  protected:
    std::size_t                     size        = 0;       ///< Size of the loaded data.
    std::vector<std::max_align_t>   noMMapBuf;             ///< Internal buffer used for fallback read mode.
    #if defined(ALIB_POSIX_MAPPED_FILES) && ALIB_POSIX_MAPPED_FILES > 0
    void*                           mapAddr     = nullptr; ///< Address of the memory-mapped region.
    #endif

  public:
    /// Default constructor. Creates an empty view.
    MappedFile()                                                                           =default;

    MappedFile(const MappedFile&)           = delete; ///< Deleted copy constructor.
    MappedFile(MappedFile&&)                = delete; ///< Deleted move constructor.
    MappedFile& operator=(const MappedFile&)= delete; ///< Deleted copy assignment operator.
    MappedFile& operator=(MappedFile&& )    = delete; ///< Deleted assignment operator.
                                                      ///< @return Void (deleted).
    ~MappedFile()                       { Close(); }  ///< Destructor. Calls #".Close".

    /// Load a file (replaces previous contents).
    /// @param path        The file's path.
    /// @param knownSize   File size in bytes if known. Defaults to \c max() , which
    ///                    triggers size detection.
    /// @param disableMMap If \c true, skips \e mmap mode and uses standard read methods.
    ///                    Defaults to \c false.
    /// @param willNeed    If \c true (the default), \b MADV_WILLNEED is passed to the function
    ///                    <c>::madvise</c>. Otherwise \b MADV_SEQUENTIAL is passed.
    /// @throws std::runtime_error on failure to open/read/map the file.
    /// @return The error code of the operation.
    std::errc Open( const CPathString&  path,
                    std::size_t         knownSize  = (std::numeric_limits<std::size_t>::max)(),
                    bool                disableMMap= false,
                    bool                willNeed   = true  );

    /// Release resources (unmap / free buffer).
    void            Close()                                                                noexcept;

    /// @return \c true if the file was read with \e mmap mode, \c false otherwise.
    bool            IsMMap()                                                        const noexcept {
        #if defined(ALIB_POSIX_MAPPED_FILES) && ALIB_POSIX_MAPPED_FILES > 0
            return mapAddr != nullptr;
        #else
            return false;
        #endif
    }

    /// Returns a data-cursor for sequential read operations of the buffer.
    /// @tparam T The data type to sequentially load from this cursor.
    /// @return The data-cursor.
    template<typename T>
    Data<T>        GetData()                                                        const noexcept {
        static_assert( alignof(T) <= alignof(std::max_align_t),
                       "MappedFile fallback buffer cannot guarantee this over-alignment." );
        #if defined(ALIB_POSIX_MAPPED_FILES) && ALIB_POSIX_MAPPED_FILES > 0
            return Data<T>(mapAddr ? mapAddr : noMMapBuf.data(), size);
        #else
            return Data<T>(noMMapBuf.data(), size);
        #endif
    }

    /// @return File size in bytes.
    std::size_t     Size()                                           const noexcept { return size; }

    /// @return \c true if the view is empty.
    bool            IsEmpty()                                   const noexcept { return size == 0; }
};



} // namespace alib[::system]

/// Type alias in namespace #"%alib".
using     MappedFile =   system::MappedFile;

} // namespace [alib]
