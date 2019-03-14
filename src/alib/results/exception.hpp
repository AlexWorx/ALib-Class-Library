// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#ifndef HPP_ALIB_RESULTS_EXCEPTION
#define HPP_ALIB_RESULTS_EXCEPTION 1

#if !defined (HPP_ALIB_RESULTS_MESSAGE)
#   include "alib/results/message.hpp"
#endif

ALIB_ASSERT_MODULE(RESULTS)

#if !defined (HPP_ALIB_RESOURCES_ENUM_META_DATA_SPECIFICATION)
#   include "alib/resources/enummetadataspec.hpp"
#endif

#if !defined (HPP_ALIB_RESOURCES_ENUM_META_DATA)
#   include "alib/resources/enummetadata.hpp"
#endif

#if !defined (_GLIBCXX_LIST) && !defined(_LIST_)
#   include <list>
#endif

DOX_MARKER( [DOX_ALIB_ENUM_META_DATA_SPECIFCATION_TheType] )
namespace aworx { namespace lib {  namespace results {
   class Exception;
}}}
DOX_MARKER( [DOX_ALIB_ENUM_META_DATA_SPECIFCATION_TheType] )

DOX_MARKER( [DOX_ALIB_ENUM_META_DATA_SPECIFCATION_decl] )
ALIB_ENUM_SPECIFICATION_DECL( aworx::lib::results::Exception, String, String )
DOX_MARKER( [DOX_ALIB_ENUM_META_DATA_SPECIFCATION_decl] )



namespace aworx { namespace lib { namespace results {


/** ************************************************************************************************
 * The (one and only) <em>"throwable"</em> used with \aliblong.
 *
 * Please consult the Programmer's Manual of module \alibmod_results for
 * \ref alib_results_exceptions "detailed information" about this class and its use.
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
 * This type uses a smart memory model leveraging class \alib{memory,MemoryBlocks} in a
 * "self-contained" fashion. With that, exceptions usually perform only one single dynamic
 * allocation, even if various messages.
 *
 * Although the size of the class is only the size of a pointer (into the self-contained first
 * memory chunk of the allocator), objects of this type have to (can only) be caught by reference.
 * Copy-construction is deleted, as the self-contained block allocator must be referenced only
 * once.
 **************************************************************************************************/
class Exception
{
    protected:
        /** An element of a the (single) linked list of message entries.   */
        struct MessageEntry
        {
            Message         message;  ///< The message,
            MessageEntry*   next;     ///< A pointer to the next message.
        };

        /**
         * The members of this class. Will be created in block allocator, which itself is
         * a part of the members. This way the outer class is only of the size of one pointer
         * (e.g. 8 bytes on usual 64-bit systems).
         */
        struct This
        {
            MemoryBlocks*       memory;     ///< The block allocator that this object and contained objects reside in.
            MessageEntry*       firstEntry; ///< A pointer to the first message.
        };

        /**
         * Pointer to the members of this class, stored in the block allocator (which is the first
         * element within this instance).
         */
        This*                   instance;

    public:
        /**
         * Shortcut to the tuple type of custom enum meta data associated with enumerations
         * stored in \alib{results,Message::Type}. Only enumeration elements that
         * are associated with meta data of this exact type are allowed to be used as code values
         * of exception entries.
         *
         * The tuple type is defined as:
         *
         *      std::tuple<int,aworx::String,aworx::String>.
         *
         * The tuple elements have the following meaning:
         * - As usual with enum meta data, the integral value denotes the underlying enum element
         *   value.
         * - The first string denotes the name of the exception.
         * - The second string provides the resource name of a human readable description text.
         *   The resource string can be retrieved with method
         *   \alib{results,Exception::Format}
         *
         */
        using TEnumMetaData= resources::T_EnumMetaDataSpecification<Exception>::TTuple;


    // #############################################################################################
    // Constructor/Destructor
    // #############################################################################################
    public:
        /** ****************************************************************************************
         * Deleted constructor. Exceptions must be caught only as references.
         * @param src The object to move.
         ******************************************************************************************/
        Exception( Exception& src )  = delete;

        /** ****************************************************************************************
         * Move constructor moves the entries and deletes them in source object.
         * @param src The object to move.
         ******************************************************************************************/
        inline
        Exception(Exception&& src) noexcept
        {
            this->instance    = src.instance;
            src.instance      = nullptr;
        }

        /** ****************************************************************************************
         * Constructs an exception and invokes #Add to create the initial message entry.
         *
         * In case that the enumeration type of given parameter \p{type} is equipped with
         * enum meta information according to tuple #TEnumMetaData, an additional argument
         * is added to the beginning of the list of arguments of the message entry. For more
         * information consult the \ref alib_results_exceptions_res "corresponding section" of the
         * Programmer's Manual.
         *
         * @param file  File name of the place of entry creation.
         * @param line  Line number of the place of entry creation.
         * @param func  Function/method name of the place of entry creation.
         * @param type  An enum element denoting the message type.
         * @param args  The message arguments.
         *
         * @tparam TEnum  Template type of the enumeration element.
         * @tparam TArgs  The variadic template argument types.
         */
        template <typename    TEnum, typename... TArgs  >
        Exception( const NCString& file, int line, const NCString& func, TEnum type, TArgs&&... args )
        {
            construct();
            Add( file,line,func, type, args... );
        }

        /** ****************************************************************************************
         * Destructor
         ******************************************************************************************/
        ALIB_API
        ~Exception();

    // #############################################################################################
    // Interface
    // #############################################################################################
        /** ****************************************************************************************
         * Returns the last message in the list of stored messages.
         *
         * @return The most recently added message.
         ******************************************************************************************/
        ALIB_API
        Message&   Back()                                                                     const;

        /** ****************************************************************************************
         * Returns the number of message entries.
         *
         * @return The number of messages added to this exception.
         ******************************************************************************************/
        ALIB_API
        int        Size()                                                                     const;

        /** ****************************************************************************************
         * Returns field \alib{results,Message::Type} of the \b last message in the list of
         * messages that has a positive underlying enum element value.
         *
         * The rationale here is explained in the
         * \ref alib_results_exception_types "Programmer's Manual".
         * In short, positive and negative enum element values are used to separated
         * "informational entries" (with a negative value) from message entries that change the
         * type of the exception (positive value). Usually, only the latter ones are processed
         * by exception handlers.
         *
         * @return The most high level exception code.
         ******************************************************************************************/
        ALIB_API
        const Enum&   Type()                                                                  const;

#if ALIB_DOCUMENTATION_PARSER
        /** ****************************************************************************************
         * Adds a new message to this exception. The parameters of this method are
         * exactly those that are expected by the \alib{results,Message::Message,constructor}
         * of class \alib{results,Message}.
         *
         * The message object itself is created in the block allocoator of this exception found
         * with \alib{results,Exception::This::memory} of field #instance. After the insertion
         * method \alib{results,Message,CloneArguments} is invoked, which creates "safe" copies
         * the arguments that are guaranteed to survive this exception's lifespan.
         *
         * Internally, this method is implemented twice. A slightly extended variant exists, which
         * is selected by the compiler if the templated type \p{TEnum} of the given enumeration
         * element is equipped with \alib{resources,T_EnumMetaDataDecl,enum meta data} that follows
         * the scheme defined with #TEnumMetaData.<br>
         * If this is the case, this variant of this method inserts anadditional argument to the
         * front of message created. This argument is of string type and is loaded from
         * the resourced enum meta data. Usually, this string argument is a formatter-string
         * that is used to format the arguments of an exception into a human readable message.
         *
         * For more information on using enum meta data with this class, see Programmer's Manual
         * chapter \ref alib_results_exceptions_res "3. Resourced Exceptions".
         *
         * @param file  File name of the place of entry creation.
         * @param line  Line number of the place of entry creation.
         * @param func  Function/method name of the place of entry creation.
         * @param type  An enum element denoting the message type.
         * @param args  The message arguments.
         *
         * @tparam TEnum  The enumeration type used to define the message type.
         * @tparam TArgs  The variadic template argument types.
         * @return Return <c>*this</c> to allow concatenated operations or use with throw statement.
         ******************************************************************************************/
        template <typename TEnum, typename... TArgs> inline
        Exception&  Add( const NCString& file, int line, const NCString& func, TEnum type, TArgs&&... args );
#else

        template <typename TEnum, typename... TArgs> inline
        ATMP_T_IF( Exception&,    std::is_enum<TEnum>::value
                              &&  !ATMP_EQ( typename resources::T_EnumMetaDataDecl<TEnum>::TTuple,
                                            TEnumMetaData ) )
        Add( const NCString& file, int line, const NCString& func, TEnum type, TArgs&&... args )
        {

            Message* newMessage= allocMessage();
            new (newMessage)  Message( file,line,func, type, Boxes( std::forward<TArgs>( args )... ) );
            newMessage->CloneArguments( *instance->memory );
            return *this;
        }



DOX_MARKER( [DOX_ALIB_ENUM_META_DATA_SPECIFCATION_usinginlib] )
template <typename TEnum, typename... TArgs  > inline

ATMP_T_IF( Exception&,     std::is_enum<TEnum>::value
                       &&  ATMP_EQ( typename resources::T_EnumMetaDataDecl<TEnum>::TTuple,
                                    TEnumMetaData ) )
Add( const NCString& file, int line, const NCString& func, TEnum type, TArgs&&... args )
{
    //...
    //...
DOX_MARKER( [DOX_ALIB_ENUM_META_DATA_SPECIFCATION_usinginlib] )

            Message* newMessage= allocMessage();
            new (newMessage)  Message( file,line,func, type );
            newMessage->Args.Add( std::forward<TArgs>( args )... );
            newMessage->CloneArguments( *instance->memory );

            // prepend the resourced string (not needed to be cloned)
            auto& metaInfo= EnumMetaData<TEnum>::GetSingleton();
            metaInfo.CheckLoad();
            TEnumMetaData& tuple= *metaInfo.Get(type);

            ALIB_STRINGS_TO_NARROW(std::get<2>(tuple), resName, 64)
            newMessage->Args.insert(newMessage->Args.begin(),
                          resources::T_Resourced<TEnum>::Resource()->Get(
                              resources::T_Resourced<TEnum>::Category(), resName ALIB_DBG(, true)) );

            return *this;
        }
#endif

        /** ****************************************************************************************
         * Uses class \alib{stringformat,Text} to write all entries of this
         * exception into the given narrow \p{target} string.
         * Entries are expected to have a format string set as their description meta information
         * that corresponds (in respect to the placeholders within the string) to the arguments
         * found in the entry.
         *
         * @param target The target string to format the entry description to.
         ******************************************************************************************/
        ALIB_API
        void   Format( AString& target )  const;

        /** ****************************************************************************************
         * Same as \alib{results::Exception,Format(AString&)const,Format(AString&)}, but writing
         * to a string of complement character width.
         *
         * @param target The target string to format the entry description to.
         ******************************************************************************************/
        ALIB_API
        void   Format( strings::TAString<complementChar>& target )  const
        {
            target << Format();
        }

        /** ****************************************************************************************
         * Inline shortcut to \alib{results::Exception,Format(AString&)const,Format(AString&)}
         * that creates, uses and returns an AString value for the exception's description.
         *
         * @return The formatted description of the Exception.
         ******************************************************************************************/
        inline
        AString Format()  const
        {
            AString result;
            Format( result );
            return result;
        }


    // #############################################################################################
    // std::ForwardIterator
    // #############################################################################################
        /** ****************************************************************************************
         * Implementation of \c std::ForwardIterator that iterates all \alib{results,Message}
         * entries of an  \b %Exception.
         ******************************************************************************************/
        class ForwardIterator
            : public std::iterator< std::forward_iterator_tag,        // iterator_category
                                    Message,                          // value_type
                                    integer,                          // distance type
                                    Message*,                         // pointer
                                    Message&                          // reference
                                  >
        {
            protected:
                /** The pointer to the actual node. */
                MessageEntry* p;

            public:
                /** Constructor.
                 *  @param _p Our initial value       */
                explicit ForwardIterator( MessageEntry* _p = nullptr )
                : p(_p)
                {}

            //######################   To satisfy concept of  InputIterator   ######################

                /** Prefix increment operator.
                 *  @return A reference to ourselves. */
                ForwardIterator& operator++()
                {
                    p= p->next;
                    return *this;
                }

                /** Postfix increment operator.
                 *  @return A a new iterator object that is not increased, yet. */
                ForwardIterator operator++(int)
                {
                    auto result= ForwardIterator(p);
                    p= p->next;
                    return result;
                }

                /** Comparison operator.
                 *  @param other  The iterator to compare ourselves to.
                 *  @return \c true if this and given iterator are equal, \c false otherwise. */
                bool operator==(ForwardIterator other)                                         const
                {
                    return p == other.p;
                }

                /** Comparison operator.
                 *  @param other  The iterator to compare ourselves to.
                 *  @return \c true if this and given iterator are not equal, \c false otherwise. */
                bool operator!=(ForwardIterator other)                                         const
                {
                    return p != other.p;
                }

                /** Retrieves the message that this iterator references.
                 * @return The message reference.                               */
                Message& operator*()                                                           const
                {
                    return p->message;
                }

                /** Retrieves the pointer to the message that this iterator references.
                 * @return The message pointer.                                  */
                Message* operator->()                                                          const
                {
                    return &p->message;
                }
        };

        /**
         * Returns an iterator pointing to the first message entry.
         * @return A forward iterator to the first message entry.
         */
        ForwardIterator begin()                                                                const
        {
            return ForwardIterator( instance->firstEntry );
        }

        /**
         * Returns an iterator representing the end of the message entries.
         * @return The end of this exception's message entries.
         */
        ForwardIterator end()                                                                  const
        {
            return ForwardIterator( nullptr );
        }


    // #############################################################################################
    // protected methods
    // #############################################################################################
    protected:
        /** ****************************************************************************************
         * Internal constructor method.
         ******************************************************************************************/
        ALIB_API
        void construct();

        /** ****************************************************************************************
         * Searches the last message and attaches a new, block-allocated list object.
         * @returns A pointer to the allocated object.
         ******************************************************************************************/
        ALIB_API
        Message* allocMessage();
};

}} // namespace aworx[::lib::exception]

/// Type alias in namespace #aworx.
using     Exception=           aworx::lib::results::Exception;


}  // namespace [aworx]

// #################################################################################################
// Macros
// #################################################################################################
DOX_MARKER( [DOX_ALIB_ENUM_META_DATA_SPECIFCATION_macro] )
#define  ALIB_EXCEPTIONS( TEnum, TModule, ResourceName )                                           \
ALIB_RESOURCED_IN_MODULE( TEnum, TModule, ResourceName)                                            \
ALIB_ENUM_SPECIFICATION(  aworx::lib::results::Exception, TEnum)                                   \
DOX_MARKER( [DOX_ALIB_ENUM_META_DATA_SPECIFCATION_macro] )



// #################################################################################################
// Append
// #################################################################################################
namespace aworx { namespace lib { namespace strings {
#if ALIB_DOCUMENTATION_PARSER
namespace APPENDABLES {
#endif
    /** ********************************************************************************************
     * Specialization of functor \alib{strings,T_Append} for type
     * \alib{results,Exception}.
     * @tparam TChar The character type.
     **********************************************************************************************/
    template<typename TChar> struct T_Append<results::Exception,TChar>
    {
        /**
         * Invokes \alib{results,Exception::Format} passing the \p{target}.
         *
         * @param target The \b AString that method \b Append was invoked on.
         * @param src    The exception to append.
         */
        inline void operator()( TAString<TChar>& target, const results::Exception&  src  )
        {
           src.Format(target);
        }
    };
#if ALIB_DOCUMENTATION_PARSER
}
#endif
}}}


#endif // HPP_ALIB_RESULTS_EXCEPTION
