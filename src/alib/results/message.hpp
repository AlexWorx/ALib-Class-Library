/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_results of the \aliblong.
 *
 * \emoji :copyright: 2013-2019 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_RESULTS_MESSAGE
#define HPP_ALIB_RESULTS_MESSAGE 1

#if !defined (HPP_ALIB_STRINGS_CSTRING)
#   include "alib/strings/cstring.hpp"
#endif

ALIB_ASSERT_MODULE(RESULTS)
ALIB_ASSERT_MODULE(BOXING)

#if !defined(HPP_ALIB_BOXING_ENUM)
#   include "alib/boxing/enum.hpp"
#endif

#if !defined (HPP_ALIB_MONOMEM_SELF_CONTAINED)
#   include "alib/monomem/selfcontained.hpp"
#endif


namespace aworx { namespace lib { namespace results {

/** ************************************************************************************************
 * This struct stores a list of information objects of arbitrary type by publically inheriting type
 * \alib{boxing,Boxes}. In addition, source code location information is attached, which usually
 * refers to the place of construction of an instance of this type.<br>
 * Finally, a type identifier is available with field #Type.
 *
 * Note that while message data might be passed with construction, informational data may be added,
 * changed or removed during the life-cycle of an instance using the inherited interface of
 * class \alib{boxing,Boxes}.
 *
 *
 * Inside \alib, the struct is used with types \alib{results,Exception} and \alib{results,Report}.
 *
 **************************************************************************************************/
struct Message : public Boxes
{
    public:
        /** The file name that this message relates to. */
        NCString        File;

        /** The line number within #File, that this message relates to. */
        int             Line;

        /** The function/method name that this message relates to. */
        NCString        Function;

        /**
         * A type identifier, defined with construction by providing an element of arbitrary
         * enumeration type.
         */
        Enum            Type;

    protected:
        /**
         * Denotes whether this instance is responsible for deleting the (inherited) monotonic
         * allocator or not.
         */
        Responsibility  monoAllocatorResponsibility;

    public:
        /** ****************************************************************************************
         * Constructs a message that does not use a \alib{monomem,MonoAllocator} and therefore
         * uses dynamic memory allocation for storing the boxable arguments.
         * @param file      Information about the scope of invocation.
         * @param line      Information about the scope of invocation.
         * @param function  Information about the scope of invocation.
         * @param type      The message type.
         * @param args      Variadic, templated list of arguments.
         ******************************************************************************************/
        template <typename... TBoxables>
        Message( const NCString& file, int line, const NCString& function,
                 const Enum& type, TBoxables&&... args )
        : Boxes( nullptr)
        , File(file),  Line(line), Function(function), Type(type)
        , monoAllocatorResponsibility(Responsibility::KeepWithSender)
        {
            Add( std::forward<TBoxables>(args)... );
        }

        /** ****************************************************************************************
         * Constructs a message that uses a \alib{monomem,MonoAllocator} to store the boxable
         * arguments. If parameter \p{monoAllocatorResp} equals \b Responsibility::Transfer, then
         * this destructor of this class will delete the object given with \p{monoAllocator}.
         *
         * @param file               Information about the scope of invocation.
         * @param line               Information about the scope of invocation.
         * @param function           Information about the scope of invocation.
         * @param monoAllocator     The allocator to store the arguments in.
         * @param monoAllocatorResp The responsibility for the allocator.
         * @param type               The message type.
         * @param args               Variadic, templated list of arguments.
         ******************************************************************************************/
        template <typename... TBoxables>
        Message( const NCString& file, int line, const NCString& function,
                 MonoAllocator* monoAllocator, Responsibility monoAllocatorResp,
                 const Enum& type, TBoxables&&... args )
        : Boxes( monoAllocator )
        , File(file), Line(line), Function( function), Type( type)
        , monoAllocatorResponsibility(monoAllocatorResp)
        {
            Add( std::forward<TBoxables>(args)... );
        }

        /** ****************************************************************************************
         * Destructor. If a monotonic allocator was provided in the constructor along with
         * flag \b Responsibility::Transfer, this allocator is deleted.
         ******************************************************************************************/
        ~Message()
        {}

        /** ****************************************************************************************
         * Loops over all contained boxes and invokes box-function \alib{boxing,FClone}.
         * The monotonic allocator passed in the constructor of this message is passed to \b FClone.
         * If no allocator was passed in the constructor, this method must not be invoked.
         * This condition is asserted in debug-compilations.
         *
         * It is the responsibility of the user or creator of a message object to invoke this method
         * and to ensure that objects that are referenced in the stored boxes either survive the
         * lifetime of the methods or are duly cloned.
         *
         * \see
         *   Manual chapter \ref alib_results_message_lifecycle for details.
         ******************************************************************************************/
        void CloneArguments()
        {
            CloneAll( *get_allocator().allocator );
        }


};

}}} // namespace [aworx::lib::results]


// Customize boxing of type Message to its parent type Boxes. This allows to "flatten" all
// arguments stored in a message when added to an instance of Boxes.
ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING( aworx::lib::results::Message*, aworx::lib::boxing::Boxes* )


#endif // HPP_ALIB_RESULTS_MESSAGE

