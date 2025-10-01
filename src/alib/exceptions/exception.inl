//==================================================================================================
/// \file
/// This header-file is part of module \alib_exceptions of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib { namespace exceptions {

/// Internal details of namespace #alib::exceptions.
namespace detail
{
    /// An element of the (single) linked list of message entries of class exception.
    /// A pointer to this type is used as the template parameter \p{T} of
    /// struct \alib{monomem;TSharedMonoVal} that class \alib{exceptions;Exception} is derived from
    /// and this way allocated (self-contained) in a \alib{MonoAllocator}.
    struct ExceptionEntry
    {
        Message           message;          ///< The message,
        ExceptionEntry*   next = nullptr;   ///< A pointer to the next message.
    };
}

//==================================================================================================
/// An \ref alib_enums_records "ALib Enum Record" type used to equip custom enumeration types
/// with records that define an entries of class \alib{exceptions;Exception}.
///
/// Besides the exception entry name, the only field this record adds provides a textual description
/// of an exception entry.
/// If the type trait \alib{resources;ResourcedTraits} is likewise specialized
/// for a custom enumeration type, then this field is interpreted as a resource name to load the
/// description from.
///
/// When parsing the enum records from string data, inherited field
/// \alib{enumrecords;ERSerializable::MinimumRecognitionLength} is not parsed from the string, but set to
/// fixed value \c 0. Therefore, only three fields have to be given per record:
///
/// 1. The custom integral enum value (this is mandatory with every resourced enum record).
/// 2. A string denoting inherited field \alib{enumrecords;ERSerializable::EnumElementName}.
/// 3. A string containing the description text, respectively the resource name of that.
//==================================================================================================
struct ERException : public enumrecords::ERSerializable
{
    /// The description of the exception.
    /// \note
    ///   If the type trait \alib{resources;ResourcedTraits} is specialized for an
    ///   enumeration, this field is interpreted as a resource name to load the description from.
    String      DescriptionOrItsResourceName;

    /// Default constructor leaving the record undefined.
    ERException()                                                               noexcept  = default;


    /// Constructor usually used with static variable declarations (declarations that are not
    /// using enumeration types associated with \ref alib_enums_records "ALib Enum Records" of this
    /// type).
    ///
    /// If used however to define an enum record during bootstrap of software (by user code
    /// that omits the preferred option of parsing resourced strings to create such records), then
    /// each parameter of type \b String passed, has to be of "static nature".
    /// This means, that string buffers and their contents are deemed to survive the life-cycle of
    /// an application. Usually, C++ string literals are passed in such situation.
    /// @param name         The name of the exception. (Usually the enum element's C++ name.)
    /// @param description  The exception's descrption. (Usually a format string.)
    ERException(const String&  name, const String&  description)                            noexcept
    : ERSerializable(name)
    , DescriptionOrItsResourceName(description)
    {}

    /// Implementation of \alib{enumrecords;EnumRecordPrototype::Parse}.
    /// \note Field \alib{enumrecords;ERSerializable::MinimumRecognitionLength} is not read from the
    ///       string, but set to fixed value \c 0.
    ALIB_DLL
    void Parse();
};

//==================================================================================================
/// The (one and only) <em>"throwable"</em> used with \aliblong.
///
/// Please consult the Programmer's Manual of module \alib_exceptions for
/// \ref alib_exceptions_exceptions "detailed information" about this class and its use.
///
/// In short, this class implements the following "exception paradigm":
/// - There is only one exception type.
/// - That type stores a forward list of \alib{exceptions;Message;messages}.
/// - With creation, a first message is added to the list of messages.
/// - While unwinding the stack, new messages may be added to the list.
/// - A new message may either add information to the previous entry or may change the meaning
///   of the exception.
/// - Messages contain IDs of \alib{boxing;Enum;arbitrary scoped enumeration types}.
///   This allows structured processing of Exceptions.
///
/// This type uses a smart memory model leveraging class \alib{monomem;TSharedMonoVal}
/// which places all internal data in a first buffer of a \alib{monomem;TMonoAllocator},
/// even the \b %Exception object itself!
/// With that, exceptions usually perform only one single dynamic allocation, even if various
/// messages with various data objects (\alib{boxing;Box;boxes}) are attached.
/// Only when many messages are added, a next allocation might occur.
/// The allocation size of the monotonic buffers is set to be one kilobyte.
///
/// Although the footprint (<c>sizeof</c>) of the class is just the size of a pointer
/// (One into the first memory buffer of the monotonic allocator), objects of this type should be
/// caught as references. Once caught, copies may be stored for later logging or similar.
///
/// @see Chapter \ref alib_exceptions_exceptions of the Programmer's Manual of
///      camp \alib_exceptions_nl.
//==================================================================================================
class Exception : protected monomem::TSharedMonoVal<detail::ExceptionEntry*, HeapAllocator, void>
{
  protected:
    /// Shortcut to the parent class.
    using base= TSharedMonoVal<detail::ExceptionEntry*, HeapAllocator, void>;

  public:
    /// Deleted copy constructor. Exceptions must be caught only as references.
    Exception( Exception&  )                                                     noexcept = default;

    /// Defaulted move constructor.
    /// @param src The object to move.
    Exception(Exception&& src)                                                   noexcept = default;

    /// Defaulted copy assignment operator.
    /// @return Nothing (deleted).
    Exception& operator=( Exception& )                                           noexcept = default;

    /// Defaulted move assignment operator.
    /// @return Nothing (deleted).
    Exception& operator=( Exception&& )                                          noexcept = default;

    /// Defaulted destructor.
    ~Exception()                                                                 noexcept = default;

    /// Defaulted default constructor.
    Exception()                                                                  noexcept = default;

    /// Constructs an empty instance from \c std::nullptr.
    /// This constructor is necessary to allow assignment of \c nullptr to values of this type,
    /// which clears the automatic pointer.
    Exception(std::nullptr_t)                                                            noexcept {}

    //==============================================================================================
    /// Constructor that allows providing the size of the allocated memory buffer in bytes.
    /// With other constructors, this size is fixed to \c 1kB (1024 bytes).
    /// A higher size may avoid a second allocation (which is not problematic in usual cases).
    ///
    /// \note The use of this constructor is advisable only in seldom cases. The same
    ///       notes as given with the documentation of the default constructor apply.
    ///
    /// @param initialBufferSizeInKB The initial allocation size of the internal
    ///                              \alib{MonoAllocator} in kilobytes (1024 bytes).
    /// @param bufferGrowthInPercent Optional growth factor in percent, applied to the buffer size
    ///                              with each next buffer allocation.
    ///                              With this type, the parameter defaults to \c 100, which does
    ///                              not increase subsequent buffer allocations.
    //==============================================================================================
    template<typename  TIntegral>
    Exception( TIntegral initialBufferSizeInKB, int bufferGrowthInPercent= 100  )
    : TSharedMonoVal( size_t(initialBufferSizeInKB), bufferGrowthInPercent )
    {
        static_assert( !std::is_integral<TIntegral>::value,
            "Erroneous use of Exception constructor overload which expects an integral "
            "value as first parameter to determine the size of the first memory buffer." );
    }

    //==============================================================================================
    /// Constructs an exception and invokes #Add to create the initial message entry.
    ///
    /// In case that the enumeration type of given parameter \p{type} is equipped with
    /// \ref alib_enums_records "ALib Enum Records" according to record type
    /// \alib{exceptions;ERException}, the first argument added to the message entry
    /// is collected from the corresponding enum record. For more
    /// information consult the \ref alib_exceptions_exceptions_res "corresponding section" of the
    /// Programmer's Manual.
    ///
    /// @tparam TEnum  Template type of the enumeration element.
    /// @tparam TArgs  The variadic template argument types.
    ///
    /// @param  ci    Source location of entry creation.
    /// @param  type  An enum element denoting the message type.
    /// @param  args  The message arguments.
    //==============================================================================================
    template<typename  TEnum, typename... TArgs  >
    Exception( const lang::CallerInfo& ci, TEnum type, TArgs&&... args )
    : TSharedMonoVal( 1, 100 )
    {
        ConstructT();
        Add( ci, type, std::forward<TArgs>(args)... );
    }

// #################################################################################################
// Interface
// #################################################################################################
    //==============================================================================================
    /// Returns the last message in the list of stored messages.
    ///
    /// @return The most recently added message.
    //==============================================================================================
    ALIB_DLL
    Message&        Back()                                                                const;

    //==============================================================================================
    /// Returns the number of message entries.
    ///
    /// @return The number of messages added to this exception.
    //==============================================================================================
    ALIB_DLL
    int             Size()                                                                const;

    //==============================================================================================
    /// Returns field \alib{exceptions;Message::Type} of the \b last message in the list of
    /// messages that has a positive underlying enum element value.
    ///
    /// The rationale here is explained in the
    /// \ref alib_exceptions_exception_types "Programmer's Manual".
    /// In short, positive and negative enum element values are used to separated
    /// "informational entries" (with a negative value) from message entries that change the
    /// type of the exception (positive value). Usually, only the latter ones are processed
    /// by exception handlers.
    ///
    /// @return The most high level exception code.
    //==============================================================================================
    ALIB_DLL
    const Enum&     Type()                                                                const;

    //==============================================================================================
    /// Adds a new message to this exception. The parameters of this method are
    /// exactly those that are expected by the \alib{exceptions;Message::Message;constructor}
    /// of class \alib{exceptions;Message}.
    ///
    /// The message object itself is created in the inherited monotonic allocator.
    /// After the insertion, method \alib{boxing;TBoxes;CloneAll} is invoked, which
    /// creates "safe" copies of the arguments to guarantee their survival during this
    /// exception's lifespan.
    ///
    /// If the enumeration type \p{TEnum} (which is deduced from parameter \p{type}) is equipped
    /// with \ref alib_enums_records "ALib Enum Records" of type \alib{exceptions;ERException},
    /// an additional message argument is \b prepended to the message
    /// This argument is of a string-type and is taken from field
    /// \alib{exceptions::ERException;DescriptionOrItsResourceName} of the associated enum record.
    /// As described in chapter alib_exceptions_exceptions_args of the Programmer's Manual,
    /// it is proposed that this argument of string-type, is a formatter-string that is used to
    /// format the arguments of an exception into a human-readable message.
    ///
    /// If furthermore, the type trait \alib{resources;ResourcedTraits} is specialized for
    /// enumeration type \p{TEnum}, then the value of \b DescriptionOrItsResourceName is not
    /// directly prepended but interpreted as a resource name. In this case the resourced
    /// description is prepended instead.
    ///
    /// For more information, consult chapter \ref alib_exceptions_exceptions_res of the
    /// Programmer's Manual.
    ///
    /// @tparam TEnum  The enumeration type used to define the message type.
    /// @tparam TArgs  The variadic template argument types.
    /// @param  ci     Source location of entry creation.
    /// @param  type   An enum element denoting the message type.
    /// @param  args   The message arguments.
    /// @return Return <c>*this</c> to allow concatenated operations or use with throw statement.
    //==============================================================================================
    template <typename TEnum, typename... TArgs> inline
    Exception&  Add( const lang::CallerInfo& ci, TEnum type, TArgs&&... args )
    {
        Message* newMessage= allocMessageLink();
        new (newMessage)  Message( ci, GetAllocator(), type );
        newMessage->Add( std::forward<TArgs>( args )... );
        finalizeMessage(  newMessage,
                          std::is_base_of_v<ERException,typename enumrecords::RecordsTraits<TEnum>::Type>,
                          resources::ResourcedTraits<TEnum>::Pool(),
                          resources::ResourcedTraits<TEnum>::Category()   );
        return *this;
    }


    #if ALIB_FORMAT
        /// Uses class \alib{format;Paragraphs} to write all entries of this
        /// exception into the given narrow \p{target} string.
        /// Entries are expected to have a format string set as their description meta-information
        /// that corresponds (in respect to the placeholders within the string) to the arguments
        /// found in the entry.
        /// \note In multithreaded applications, the\alib{format;Formatter::DefaultLock}
        ///       has to be acquired before invoking this method.
        /// \par Availability
        ///   This method is available only if the module \alib_format is included in the \alibbuild.
        /// @param target The target string to format the entry description to.
        /// @return Returns given \b AString \p target for convenience.
        ALIB_DLL
        AString&  Format( AString& target )                                                   const;

        /// Same as \alib{exceptions::Exception;Format(AString&)const;Format(AString&)}, but writing
        /// to a string of complement character width.
        /// \note In multithreaded applications, the\alib{format;Formatter::DefaultLock}
        ///       has to be acquired before invoking this method.
        /// \par Availability
        ///   This method is available only if the module \alib_format is included in the \alibbuild.
        /// @param target The target string to format the entry description to.
        /// @return Returns given \b AString \p target for convenience.
        ALIB_DLL
        strings::TAString<complementChar, lang::HeapAllocator>&
        Format( strings::TAString<complementChar, lang::HeapAllocator>& target )               const
        {
            target << Format();
            return target;
        }

        /// Inline shortcut to \alib{exceptions::Exception;Format(AString&)const;Format(AString&)}
        /// that creates, uses and returns an AString value for the exception's description.
        /// \note In multithreaded applications, the\alib{format;Formatter::DefaultLock} has to
        ///       be acquired before invoking this method.
        ///
        /// \par Availability
        ///   This method is available only if the module \alib_format is included in the \alibbuild.
        /// @return The formatted description of the Exception.
        AString Format()                                                                       const
        {
            AString result;
            Format( result );
            return result;
        }
    #endif


// #################################################################################################
// std::ForwardIterator
// #################################################################################################
protected:
    //==============================================================================================
    /// Implementation of \c std::ForwardIterator that iterates all \alib{exceptions;Message}
    /// entries of an \b %Exception.
    //==============================================================================================
    template<typename TConstOrMutableMessage>
    class IteratorType
    {
        protected:
            /// The pointer to the actual node.
            detail::ExceptionEntry* p;

        public:
            using iterator_category = std::forward_iterator_tag;  ///< Implementation of <c>std::iterator_traits</c>.
            using value_type        = Message                  ;  ///< Implementation of <c>std::iterator_traits</c>.
            using difference_type   = integer                  ;  ///< Implementation of <c>std::iterator_traits</c>.
            using pointer           = TConstOrMutableMessage*  ;  ///< Implementation of <c>std::iterator_traits</c>.
            using reference         = TConstOrMutableMessage&  ;  ///< Implementation of <c>std::iterator_traits</c>.

        public:
            /// Constructor.
            /// @param _p Our initial value
            explicit IteratorType( detail::ExceptionEntry* _p = nullptr )
            : p(_p)
            {}

            /// Constructor taking a constant entry pointer.
            /// Available for the constant version of this iterator only.
            /// @param entry The initial message entry.
            template<typename TConstEntry>
            requires std::is_const_v<TConstEntry>
            IteratorType( TConstEntry* entry ) : p(const_cast<detail::ExceptionEntry*>(entry))    {}


            /// Constructor taking a mutable iterator.
            /// Available for the constant version of this iterator only.
            /// @param it The mutable iterator used to construct this constant one.
            template<typename TMutableIterator>
            requires std::same_as<TMutableIterator, IteratorType<const Message>>
            IteratorType( TMutableIterator it )                                        : p(it)    {}

        //######################   To satisfy concept of  InputIterator   ######################

            /// Prefix increment operator.
            /// @return A reference to ourselves.
            IteratorType& operator++()
            {
                p= p->next;
                return *this;
            }

            /// Postfix increment operator.
            /// @return A new iterator object that is not increased, yet.
            IteratorType operator++(int)
            {
                auto result= IteratorType(p);
                p= p->next;
                return result;
            }

            /// Comparison operator.
            /// @param other  The iterator to compare ourselves to.
            /// @return \c true if this and given iterator are equal, \c false otherwise.
            bool operator==(IteratorType other)                                                const
            {
                return p == other.p;
            }

            /// Comparison operator.
            /// @param other  The iterator to compare ourselves to.
            /// @return \c true if this and given iterator are not equal, \c false otherwise.
            bool operator!=(IteratorType other)                                                const
            {
                return p != other.p;
            }

            /// Retrieves the message that this iterator references.
            /// @return The message reference.
            TConstOrMutableMessage& operator*()                                                const
            {
                return p->message;
            }

            /// Retrieves the pointer to the message that this iterator references.
            /// @return The message pointer.
            TConstOrMutableMessage* operator->()                                               const
            {
                return &p->message;
            }
    };

public:
    /// The constant iterator exposed by this container.
    using ConstForwardIterator  = IteratorType        <const Message>;

    /// The mutable iterator exposed by this container.
    using ForwardIterator       = IteratorType        <      Message>;


    /// Returns an iterator pointing to the first message entry.
    /// @return A forward iterator to the first message entry.
    ForwardIterator begin()
    {
        return ForwardIterator( **this );
    }

    /// Returns an iterator representing the end of the message entries.
    /// @return The end of this exception's message entries.
    ForwardIterator end()
    {
        return ForwardIterator( nullptr );
    }

    /// Returns an iterator pointing to the first message entry.
    /// @return A forward iterator to the first message entry.
    ConstForwardIterator begin()                                                               const
    {
        return ConstForwardIterator( **this );
    }

    /// Returns an iterator representing the end of the message entries.
    /// @return The end of this exception's message entries.
    ConstForwardIterator end()                                                                 const
    {
        return ConstForwardIterator( nullptr );
    }


// #################################################################################################
// protected methods
// #################################################################################################
protected:
    //==============================================================================================
    /// Searches the last linked message and attaches a new, monotonically allocated list node.
    /// @returns A pointer to the message in the allocated link node.
    //==============================================================================================
    ALIB_DLL
    Message* allocMessageLink();

    //==============================================================================================
    /// Non-inlined portion of the method #Add.
    /// Clones arguments and prepends description argument, in case the enum element of the message
    /// has an enum record attached.
    /// If furthermore, the enum element's record was resourced, then the record's description
    /// value is interpreted as a resource string's name, which is prepended instead.
    /// @param message   The message to finalize.
    /// @param hasRecord Indicates if a record is assigned.
    /// @param pool      If records are resourced, this is the resource pool to use.
    /// @param category  If records are resourced, this is the category to use.
    //==============================================================================================
    ALIB_DLL
    void finalizeMessage( Message* message, bool hasRecord, ResourcePool* pool, const NString& category );


}; // class Exception


} // namespace alib[::exceptions]


/// Type alias in namespace \b alib.
using     Exception=           exceptions::Exception;

}  // namespace [alib]

ALIB_BOXING_VTABLE_DECLARE( alib::exceptions::Exception*, vt_alib_exception )


#if ALIB_CAMP
ALIB_EXPORT namespace alib::system {
/// A Namespace function that creates an according \alib{exceptions;Exception} to a corresponding
/// system error number.
///
/// The small challenge here is that arbitrary error numbers (of an unknown) type might occur, that
/// do not have a corresponding enum record.
/// In this case, \alib{system;SystemErrors;SystemErrors::UNKNOWN} is thrown and only the
/// number is displayed in the description text.
///
/// \par Availability
///   This method is available only if \alib_camp is included in the \alibbuild.
///   The reason for this is that all format strings for the many system errors are only resourced
///   with class \alib{camp;Basecamp}.
/// @param ci    The source location of the exception creation.
/// @param errNo The system's error number.
/// @return An exception object.
ALIB_DLL Exception CreateSystemException( const CallerInfo& ci, int errNo );
}
#endif

// #################################################################################################
// Append
// #################################################################################################
#if ALIB_FORMAT
ALIB_EXPORT namespace alib::strings {
#if DOXYGEN
namespace APPENDABLES {
#endif
    //==============================================================================================
    /// Specialization of functor \alib{strings;AppendableTraits} for type
    /// \alib{exceptions;Exception}.
    ///
    /// \par Availability
    ///   This method is available only if the module \alib_format is included in the \alibbuild.
    /// @tparam TChar      The character type.
    /// @tparam TAllocator The allocator type, as prototyped with \alib{lang;Allocator}.
    //==============================================================================================
    template<typename TChar, typename TAllocator> struct AppendableTraits<exceptions::Exception,TChar,TAllocator>
    {
        /// Invokes \alib{exceptions;Exception::Format} passing the \p{target}.
        ///
        /// @param target The \b AString that method \b Append was invoked on.
        /// @param src    The exception to append.
        void operator()( TAString<TChar,TAllocator>& target, const exceptions::Exception&  src  )  {
           src.Format(target);
        }
    };
#if DOXYGEN
}
#endif
}
#endif // ALIB_FORMAT

