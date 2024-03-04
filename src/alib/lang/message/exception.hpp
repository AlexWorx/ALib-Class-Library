/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_basecamp of the \aliblong.
 *
 * \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_CAMP_MESSAGE_EXCEPTION
#define HPP_ALIB_CAMP_MESSAGE_EXCEPTION 1

#if !defined (HPP_ALIB_CAMP_MESSAGE_MESSAGE)
#   include "alib/lang/message/message.hpp"
#endif

ALIB_ASSERT_MODULE(CAMP)

#if !defined (HPP_ALIB_ENUMS_RECORDS)
#   include "alib/enums/records.hpp"
#endif

#if !defined (HPP_ALIB_LANG_RESOURCES_RESOURCES)
#   include "alib/lang/resources/resources.hpp"
#endif



namespace alib { namespace lang {

/** Internal details of namespace #alib::lang. */
namespace detail
{
    /**
     * An element of the (single) linked list of message entries of class exception.
     * A pointer to this type is used as the template parameter \p{TContained} of
     * struct \alib{monomem,SelfContained} that class \alib{lang,Exception} is derived from
     * and this way allocated (self-contained) in a \alib{monomem,MonoAllocator}.
     */
    struct ExceptionEntry
    {
        Message           message;          ///< The message,
        ExceptionEntry*   next = nullptr;   ///< A pointer to the next message.
    };
}

/** ************************************************************************************************
 * An \ref alib_enums_records "ALib Enum Record" type used to equip custom enumeration types
 * with records that define an entries of class \alib{lang,Exception}.
 *
 * Besides the exception entry name, the only field this record adds provides a textual description
 * of an exception entry.
 * If TMP struct \alib{lang::resources,T_Resourced} is likewise specialized for a custom enumeration type,
 * then this field is interpreted as a resource name to load the description from.
 *
 * When parsing the enum records from string data, inherited field
 * \alib{enums,ERSerializable::MinimumRecognitionLength} is not parsed from the string, but set to
 * fixed value \c 0. Therefore, only three fields have to be given per record:
 *
 * 1. The custom integral enum value (this is mandatory with every resourced enum record).
 * 2. A string denoting inherited field \alib{enums,ERSerializable::EnumElementName}.
 * 3. A string containing the description text, respectively the resource name of that.
 **************************************************************************************************/
struct ERException : public enums::ERSerializable
{
    /**
     * The description of the exception.
     * \note
     *   If TMP struct \alib{lang::resources,T_Resourced} is specialized for an enumeration,
     *   this field is interpreted as a resource name to load the description from.
     */
    String      DescriptionOrItsResourceName;

    /** Default constructor leaving the record undefined.    */
    ERException()                                                               noexcept  = default;


    /**
     * Constructor usually used with static variable declarations (declarations that are not
     * using enumeration types associated with \ref alib_enums_records "ALib Enum Records" of this
     * type).
     *
     * If used however to define an enum record during bootstrap of a software (by user code
     * that omits the preferred option of parsing resourced strings to create such records), then
     * each parameter of type \b String passed, has to be of "static nature".
     * This means, that string buffers and their contents are deemed to survive the life-cycle of
     * an application. Usually, C++ string literals are passed in such situation.
     * @param name         The name of the exception. (Usually the enum element's C++ name.)
     * @param description  The exception's descrption. (Usually a format string.)
     */
    ERException(const String&  name, const String&  description)                noexcept
    : ERSerializable(name)
    , DescriptionOrItsResourceName(description)
    {}

    /**
     * Implementation of \alib{enums,EnumRecordPrototype::Parse}.
     * \note Field \alib{enums,ERSerializable::MinimumRecognitionLength} is not read from the string, but set
     *       to fixed value \c 0.
     */
    ALIB_API
    void Parse();
};

/** ************************************************************************************************
 * The (one and only) <em>"throwable"</em> used with \aliblong.
 *
 * Please consult the Programmer's Manual of module \alib_basecamp for
 * \ref alib_basecamp_message_exceptions "detailed information" about this class and its use.
 *
 * In short, this class implements the following "exception paradigm":
 * - There is only one exception type.
 * - With creation, a first "message" is added to the list of messages.
 * - While unwinding the stack new messages may be added to the list.
 * - A new message may either add information to the previous entry or may change the meaning
 *   of the exception.
 * - Messages contain IDs of \alib{boxing,Enum,arbitrary scoped enumeration types}.
 *   This allows structured processing of Exceptions.
 *
 * This type uses a smart memory model leveraging class \alib{monomem,MonoAllocator}
 * which places all internal data in \alib{monomem,MonoAllocator}, even this object itself!
 * With that, exceptions usually perform only one single dynamic
 * allocation, even if various messages with various data objects (\alib{boxing,Box,boxes}) are
 * attached.
 *
 * Although the footprint (<c>sizeof</c>) of the class is just the size of a pointer
 * (One into the first chunk of memory of the monotonic allocator), objects of this type have to
 * (can only)
 * be caught by reference. Copy-construction and assignment is deleted.
 **************************************************************************************************/
class Exception : protected monomem::SelfContained<detail::ExceptionEntry*>
{
    public:
        /** ****************************************************************************************
         * Deleted copy constructor. Exceptions must be caught only as references.
         ******************************************************************************************/
        Exception( Exception&  )                                                           = delete;

        /** ****************************************************************************************
         * Move constructor moves the entries and deletes them in source object.
         *
         * @param src The object to move.
         ******************************************************************************************/
        Exception(Exception&& src) noexcept
        : SelfContained( std::move(src) )
        {}

        /** ****************************************************************************************
         * Deleted copy assignment operator.
         * @return Nothing (deleted).
         ******************************************************************************************/
        Exception  operator=( Exception& )                                                 = delete;

        /** ****************************************************************************************
         * Deleted move assignment operator.
         * @return Nothing (deleted).
         ******************************************************************************************/
        Exception& operator=( Exception&& )                                                = delete;

        /** ****************************************************************************************
         * Default constructor.
         * \note If this constructor is used, one or more invocations of #Add have be performed
         *       prior to throwing the exception. Otherwise, the exception does not contain any
         *       information. From this perspective, it could be said that this constructor is
         *       provided for the cases where either more than one entry is added in a loop or
         *       different types of entries are added in a later <c>if statement</c> after
         *       construction. Otherwise, this constructor should be avoided.
         ******************************************************************************************/
        Exception()
        : SelfContained( 1024, 100 )
        {}

        /** ****************************************************************************************
         * Constructor that allows to provide the size of the allocated chunk of memory in bytes.
         * With other constructors, this size is fixed to \c 1024.
         * A higher size may avoid a second allocation (which is not problematic in usual cases).
         *
         * \note The use of this constructor is advisable only in seldom cases. The same
         *       notes as given with the documentation of the default constructor apply.
         *
         * @param initialChunkSize      The initial allocation size of the internal
         *                              \alib{monomem,MonoAllocator}.
         * @param chunkGrowthInPercent  Optional growth faktor in percent (*100), applied to each
         *                              allocation of a next chunk size.
         *                              Values provided should be greater than 100.<p>
         *                              Defaults to 100, which does not increase subsequent
         *                              chunk allocations.
         ******************************************************************************************/
        template<typename  TIntegral>
        Exception( TIntegral initialChunkSize, int chunkGrowthInPercent= 100  )
        : SelfContained( static_cast<size_t>(initialChunkSize), chunkGrowthInPercent )
        {
            static_assert( !std::is_enum<TIntegral>::value,
                           "Errorneous use of Exception constructor. Enumeration element given "
                           "without caller information. Add macro ALIB_CALLER or "
                           "ALIB_CALLER_NULLED as a first argument." );
        }

        /** ****************************************************************************************
         * Constructs an exception and invokes #Add to create the initial message entry.
         *
         * In case that the enumeration type of given parameter \p{type} is equipped with
         * \ref alib_enums_records "ALib Enum Records" according to record type
         * \alib{lang,ERException}, the first argument added to the message entry
         * is collected from the corresponding enum record. For more
         * information consult the \ref alib_basecamp_message_exceptions_res "corresponding section" of the
         * Programmer's Manual.
         *
         * @tparam TEnum  Template type of the enumeration element.
         * @tparam TArgs  The variadic template argument types.
         *
         * @param  file  File name of the place of entry creation.
         * @param  line  Line number of the place of entry creation.
         * @param  func  Function/method name of the place of entry creation.
         * @param  type  An enum element denoting the message type.
         * @param  args  The message arguments.
         ******************************************************************************************/
        template<typename  TEnum, typename... TArgs  >
        Exception( const NCString& file, int line, const NCString& func,
                   TEnum type, TArgs&&... args )
        : SelfContained( 1024, 100 )
        {
            Add( file,line,func, type, std::forward<TArgs>(args)... );
        }

        /** ****************************************************************************************
         * Destructor
         ******************************************************************************************/
        ~Exception()                                                                      = default;

    // #############################################################################################
    // Interface
    // #############################################################################################
        /** ****************************************************************************************
         * Returns the last message in the list of stored messages.
         *
         * @return The most recently added message.
         ******************************************************************************************/
        ALIB_API
        Message&        Back()                                                                const;

        /** ****************************************************************************************
         * Returns the number of message entries.
         *
         * @return The number of messages added to this exception.
         ******************************************************************************************/
        ALIB_API
        int             Size()                                                                const;

        /** ****************************************************************************************
         * Returns field \alib{lang,Message::Type} of the \b last message in the list of
         * messages that has a positive underlying enum element value.
         *
         * The rationale here is explained in the
         * \ref alib_basecamp_message_exception_types "Programmer's Manual".
         * In short, positive and negative enum element values are used to separated
         * "informational entries" (with a negative value) from message entries that change the
         * type of the exception (positive value). Usually, only the latter ones are processed
         * by exception handlers.
         *
         * @return The most high level exception code.
         ******************************************************************************************/
        ALIB_API
        const Enum&     Type()                                                                const;

        /** ****************************************************************************************
         * Adds a new message to this exception. The parameters of this method are
         * exactly those that are expected by the \alib{lang,Message::Message,constructor}
         * of class \alib{lang,Message}.
         *
         * The message object itself is created in the monotonic allocator of this exception
         * received by inherited method \alib{monomem,SelfContained::Allocator}.
         *
         * After the insertion, method \alib{lang,Message,CloneArguments} is invoked, which
         * creates "safe" copies of the arguments to guarantee their survival during this
         * exception's lifespan.
         *
         * If the enumeration type \p{TEnum} (which is deduced from parameter \p{type}) is equipped
         * with \ref alib_enums_records "ALib Enum Records" of type \alib{lang,ERException},
         * an additional message argument is \b prepended to the message
         * This argument is of string type and is taken from field
         * \alib{lang::ERException,DescriptionOrItsResourceName} of the associated enum record.
         * As described in chapter alib_basecamp_message_exceptions_args of the Programmer's Manual, it is
         * proposed that this argument of string type, is a formatter-string that is used to format
         * the arguments of an exception into a human readable message.
         *
         * If furthermore, TMP struct \alib{lang::resources,T_Resourced} is specialized for enumeration
         * type \p{TEnum}, then the value of \b DescriptionOrItsResourceName is not directly
         * prepended, but interpreted as a resource name. In this case the resourced description
         * is prepended instead.
         *
         * For more information consult chapter \ref alib_basecamp_message_exceptions_res of the
         * Programmer's Manual.
         *
         * @tparam TEnum  The enumeration type used to define the message type.
         * @tparam TArgs  The variadic template argument types.
         *
         * @param  file  File name of the place of entry creation.
         * @param  line  Line number of the place of entry creation.
         * @param  func  Function/method name of the place of entry creation.
         * @param  type  An enum element denoting the message type.
         * @param  args  The message arguments.
         * @return Return <c>*this</c> to allow concatenated operations or use with throw statement.
         ******************************************************************************************/
        template <typename TEnum, typename... TArgs> inline
        Exception&  Add( const NCString& file, int line, const NCString& func, TEnum type, TArgs&&... args )
        {
            Message* newMessage= allocMessageLink();
            new (newMessage)  Message( file,line,func, &Allocator(), lang::Responsibility::KeepWithSender, type );
            newMessage->Add( std::forward<TArgs>( args )... );
            finalizeMessage(  newMessage,
                              ATMP_ISOF( typename enums::T_EnumRecords<TEnum>::Type, ERException ),
                              T_Resourced<TEnum>::Pool(),
                              T_Resourced<TEnum>::Category()   );

            return *this;
        }

        /** ****************************************************************************************
         * Uses class \alib{lang::format,Paragraphs} to write all entries of this
         * exception into the given narrow \p{target} string.
         * Entries are expected to have a format string set as their description meta information
         * that corresponds (in respect to the placeholders within the string) to the arguments
         * found in the entry.
         *
         * @param target The target string to format the entry description to.
         * @return Returns given \b AString \p target for convenience.
         ******************************************************************************************/
        ALIB_API
        AString&  Format( AString& target )                                                    const;

        /** ****************************************************************************************
         * Same as \alib{lang::Exception,Format(AString&)const,Format(AString&)}, but writing
         * to a string of complement character width.
         *
         * @param target The target string to format the entry description to.
         * @return Returns given \b AString \p target for convenience.
         ******************************************************************************************/
        ALIB_API
        strings::TAString<complementChar>&
        Format( strings::TAString<complementChar>& target )                                    const
        {
            target << Format();
            return target;
        }

        /** ****************************************************************************************
         * Inline shortcut to \alib{lang::Exception,Format(AString&)const,Format(AString&)}
         * that creates, uses and returns an AString value for the exception's description.
         *
         * @return The formatted description of the Exception.
         ******************************************************************************************/
        AString Format()                                                                       const
        {
            AString result;
            Format( result );
            return result;
        }


    // #############################################################################################
    // std::ForwardIterator
    // #############################################################################################
    protected:
        /** ****************************************************************************************
         * Implementation of \c std::ForwardIterator that iterates all \alib{lang,Message}
         * entries of an \b %Exception.
         ******************************************************************************************/
        template<typename TConstOrMutableMessage>
        class IteratorType
        {
            protected:
                /** The pointer to the actual node. */
                detail::ExceptionEntry* p;

            public:
                using iterator_category = std::forward_iterator_tag;  ///< Implementation of <c>std::iterator_traits</c>.
                using value_type        = Message                  ;  ///< Implementation of <c>std::iterator_traits</c>.
                using difference_type   = integer                  ;  ///< Implementation of <c>std::iterator_traits</c>.
                using pointer           = TConstOrMutableMessage*  ;  ///< Implementation of <c>std::iterator_traits</c>.
                using reference         = TConstOrMutableMessage&  ;  ///< Implementation of <c>std::iterator_traits</c>.

            public:
                /** Constructor.
                 *  @param _p Our initial value       */
                explicit IteratorType( detail::ExceptionEntry* _p = nullptr )
                : p(_p)
                {}

                /** Constructor taking a constant entry pointer.
                 *  Available for the constant version of this iterator only.
                 *  @param entry The initial message entry.   */
                ATMP_SELECT_IF_1TP( typename TConstEntry, std::is_const<TConstEntry>::value )
                IteratorType( TConstEntry* entry )
                : p(const_cast<detail::ExceptionEntry*>(entry))
                {}


                /** Constructor taking a mutable iterator.
                 *  Available for the constant version of this iterator only.
                 *  @param it The mutable iterator to used to construct this constant one.  */
                ATMP_SELECT_IF_1TP( typename TMutableIterator, ATMP_EQ(TMutableIterator, IteratorType<const Message> ) )
                IteratorType( TMutableIterator it )
                : p(it)
                {}

            //######################   To satisfy concept of  InputIterator   ######################

                /** Prefix increment operator.
                 *  @return A reference to ourselves. */
                IteratorType& operator++()
                {
                    p= p->next;
                    return *this;
                }

                /** Postfix increment operator.
                 *  @return A new iterator object that is not increased, yet. */
                IteratorType operator++(int)
                {
                    auto result= IteratorType(p);
                    p= p->next;
                    return result;
                }

                /** Comparison operator.
                 *  @param other  The iterator to compare ourselves to.
                 *  @return \c true if this and given iterator are equal, \c false otherwise. */
                bool operator==(IteratorType other)                                         const
                {
                    return p == other.p;
                }

                /** Comparison operator.
                 *  @param other  The iterator to compare ourselves to.
                 *  @return \c true if this and given iterator are not equal, \c false otherwise. */
                bool operator!=(IteratorType other)                                         const
                {
                    return p != other.p;
                }

                /** Retrieves the message that this iterator references.
                 * @return The message reference.                               */
                TConstOrMutableMessage& operator*()                                            const
                {
                    return p->message;
                }

                /** Retrieves the pointer to the message that this iterator references.
                 * @return The message pointer.                                  */
                TConstOrMutableMessage* operator->()                                           const
                {
                    return &p->message;
                }
        };

    public:
        /** The constant iterator exposed by this container. */
        using ConstForwardIterator  = IteratorType        <const Message>;

        /** The mutable iterator exposed by this container. */
        using ForwardIterator       = IteratorType        <      Message>;


        /**
         * Returns an iterator pointing to the first message entry.
         * @return A forward iterator to the first message entry.
         */
        ForwardIterator begin()
        {
            return ForwardIterator( Self() );
        }

        /**
         * Returns an iterator representing the end of the message entries.
         * @return The end of this exception's message entries.
         */
        ForwardIterator end()
        {
            return ForwardIterator( nullptr );
        }

        /**
         * Returns an iterator pointing to the first message entry.
         * @return A forward iterator to the first message entry.
         */
        ConstForwardIterator begin()                                                           const
        {
            return ConstForwardIterator( Self() );
        }

        /**
         * Returns an iterator representing the end of the message entries.
         * @return The end of this exception's message entries.
         */
        ConstForwardIterator end()                                                             const
        {
            return ConstForwardIterator( nullptr );
        }


    // #############################################################################################
    // protected methods
    // #############################################################################################
    protected:
        /** ****************************************************************************************
         * Searches the last linked message and attaches a new, monotonically allocated list node.
         * @returns A pointer to the message in the allocated link node.
         ******************************************************************************************/
        ALIB_API
        Message* allocMessageLink();

        /** ****************************************************************************************
         * Non-inlined portion of method #Add. Clones arguments and prepends description argument,
         * in case  the enum element of the message has an enum record attached.
         * If furthermore, the enum element's record was resourced, then the record's description
         * value is interpreted as a resource string's name, which is prepended instead.
         * @param message   The message to finalize.
         * @param hasRecord Indicates if a record is assigned.
         * @param pool      If records are resourced, this is the resource pool to use.
         * @param category  If records are resourced, this is the category to use.
         ******************************************************************************************/
        ALIB_API
        void finalizeMessage( Message* message, bool hasRecord, ResourcePool* pool, const NString& category );


}; // class Exception


} // namespace alib[::lang]


/// Type alias in namespace \b alib.
using     Exception=           lang::Exception;

}  // namespace [alib]

ALIB_BOXING_VTABLE_DECLARE( alib::lang::Exception*, vt_alib_exception )


// #################################################################################################
// Append
// #################################################################################################
namespace alib {  namespace strings {
#if defined(ALIB_DOX)
namespace APPENDABLES {
#endif
    /** ********************************************************************************************
     * Specialization of functor \alib{strings,T_Append} for type
     * \alib{lang,Exception}.
     * @tparam TChar The character type.
     **********************************************************************************************/
    template<typename TChar> struct T_Append<lang::Exception,TChar>
    {
        /**
         * Invokes \alib{lang,Exception::Format} passing the \p{target}.
         *
         * @param target The \b AString that method \b Append was invoked on.
         * @param src    The exception to append.
         */
        void operator()( TAString<TChar>& target, const lang::Exception&  src  )
        {
           src.Format(target);
        }
    };
#if defined(ALIB_DOX)
}
#endif
}}


#endif // HPP_ALIB_CAMP_MESSAGE_EXCEPTION
