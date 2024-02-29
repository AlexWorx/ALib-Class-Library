/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_monomem of the \aliblong.
 *
 * \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_MONOMEM_SELF_CONTAINED
#define HPP_ALIB_MONOMEM_SELF_CONTAINED 1

#if !defined(HPP_ALIB) && !defined(ALIB_DOX)
#   include "alib/alib.hpp"
#endif

#if !defined (HPP_ALIB_MONOMEM_MONOALLOCATOR)
#   include "alib/monomem/monoallocator.hpp"
#endif

namespace alib {  namespace monomem {

/** ************************************************************************************************
 * This templated class supports the implementation of types whose fields are allocated within
 * a \alib{monomem,MonoAllocator}. In addition the monotonic allocator itself is also
 * self-contained. This results in the following:
 * - The size of an instance of this class is equal to the size of a single C++ pointer.
 *   The only member is a pointer to an object of internal type \alib{monomem::SelfContained,Fields}.
 * - A single dynamic memory allocation is performed to create a derived instance of the class.
 * - With method #Allocator, the self-contained \alib{monomem,MonoAllocator} can be used to
 *   allocate further objects.
 *
 * \alib uses this type in various areas. A prominent sample is class \alib{lang,Exception}
 * which is the unique <c>C++ throwable</c> used by the library.<br>
 * The general use-case for this type is similar to the use-case of class %MonoAllocator, with
 * the addition that the life-cycle of the allocated objects are bound to the life-cycle of
 * an instance of the derived type. A well understandable sample for this is found with type
 * \alib{config,Variable} implemented in module \alib_config.
 *
 *
 * The proposed schematic for using this class is as follows:
 * 1. A struct or class named for example \b MyTypesFields, that incorporates all fields of the
 *    self-contained class \b MyType is to be declared.<br>
 *    Such declaration might be placed in a sub-namespace called \b detail.
 * 2. Type \b MyType inherits this class \b SelfContained using \b MyTypesFields as template
 *    parameter.
 * 3. Constructors of \b MyType invoke the constructor of \b SelfContained providing the chunk
 *    size of the allocator along with the arguments for the construction of \b MyTypesFields.
 * 5. The copy-constructor of \b MyType should either be deleted (the usual case as self-contained
 *    objects most probably are not copyable) or defined to create another self-contained object
 *    of the derived type.
 * 6. A move-constructor of \b MyType should be defined that just invokes this struct's move
 *    constructor, forwarding the argument using \c std::move.
 * 7. Instead of keyword \c this, methods of \b MyType use method #Self to access field members
 *    (which are defined in helper struct \b MyTypesFields).
 * 8. Methods of the derived type might allocate further objects using the internal monotonic allocator,
 *    which is accessed with method #Allocator.
 *
 * A quick sample code of this schematic is given with chapter
 * \ref alib_monomem_selfcontained_allocator of the Programmer's Manual of this \alibmod.
 *
 * It is superfluous to say that instances of derived types should not be created using C++ keyword
 * <c>new</c> but instead should be created on stack memory or embedded as value members in other
 * types.
 *
 *
 * Finally this type supports memory resets. For this, a \alib{monomem,MonoAllocator::Snapshot}
 * is created right after construction. Method #Reset resets the internal monotonic allocator using
 * this snapshot. Before that, the \p{TContained} object is destructed and afterwards is
 * "re-constructed" using <em>C++ placement new</em>. For the latter, variadic construction
 * parameters are accepted by method #Reset.<br>
 * The memory reserved by #Reset can be increased with method #TakeSnapshot. Furthermore,
 * the monotonic allocator's pair of methods \b Reset and \b TakeSnapshot are also allowed to be used
 * to implement more complex (nested) snapshots.
 *
 * @tparam TContained  Default-constructible type of the stored object.
 *
 * \see
 *   Chapter
 *   \ref alib_monomem_selfcontained_allocator of the Programmer's Manual of this \alibmod.<br>
 *   For more complex sample (code) see classes \alib{lang,Exception} and/or
 *   \alib{config,Variable}, which uses the mechanisms provided by this type.
 **************************************************************************************************/
template<typename TContained>
class SelfContained
{
    public:
        /**
         * The data members that are allocated in the first chunk of the allocator.
         */
        struct Fields
        {
            /** The allocator that this class is contained in.*/
            MonoAllocator              allocator;

            /** Snapshot pointing behind this object in the first allocated chunk. */
            MonoAllocator::Snapshot    snapshot;

            /** The custom fields */
            TContained                  custom;

            /**
             * Constructor.
             * @tparam TArgs                The argument types used for constructing \p{TContained}.
             * @param  firstChunk           The first argument to field #allocator.
             * @param  initialChunkSize     The second argument to field #allocator.
             * @param  chunkGrowthInPercent Growth faktor in percent (*100), applied to each
             *                              allocation of a next chunk size.
             *                              Values provided should be greater than 100.<p>
             * @param  args                 The arguments to construct the instance of
             *                              \p{TContained}.
             */
            template<typename... TArgs>
            Fields( MonoAllocator::Chunk* firstChunk,
                    size_t initialChunkSize, unsigned int chunkGrowthInPercent,
                    TArgs&&... args )
            : allocator( firstChunk, initialChunkSize, chunkGrowthInPercent )
            , custom(  std::forward<TArgs>(args)... )
            {}
        };

    protected:
        /**
         * The only member of this class. It points to the start of the 'effective' members,
         * residing in the first chunk of the mono allocator.
         */
        Fields*   fields;

    public:

        /** ********************************************************************************************
         * Creates object #fields allocated in the first chunk of the monotonic allocator which is found
         * in that struct.
         *
         * The instance of custom type \p{TContained} which is found in #fields will be constructed
         * using the given variadic arguments \p{args}.
         *
         * @tparam TArgs                The argument types used for constructing \p{TContained}.
         * @param  initialChunkSize     The initial size of memory chunks used with the monotonic
         *                              allocator.
         * @param  chunkGrowthInPercent Optional growth factor in percent (*100), applied to each
         *                              allocation of a next chunk size.
         *                              Values provided should be greater than 100.
         * @param   args                The arguments to construct the instance of \p{TContained}.
         **********************************************************************************************/
        template<typename... TArgs>
        SelfContained( size_t initialChunkSize, unsigned int chunkGrowthInPercent, TArgs&&... args )
        {
            ALIB_ASSERT_ERROR( sizeof(Fields) < initialChunkSize - MonoAllocator::MaxUsableSpaceLoss(),
                               "MONOMEM", "Chunk size to small to create self-contained object."  )
            auto firstChunk= MonoAllocator::Chunk::create( initialChunkSize - MonoAllocator::MaxUsableSpaceLoss() );

            fields=  reinterpret_cast<Fields*>( firstChunk->alloc( sizeof( Fields), alignof(Fields) ) );
            new (fields) Fields( firstChunk, initialChunkSize, chunkGrowthInPercent,
                                 std::forward<TArgs>(args)... );

            fields->snapshot= fields->allocator.TakeSnapshot();
        }

        /** ****************************************************************************************
         * Resets the monotonic allocator that this object is contained in to the snapshot created
         * right after construction.
         * The allocated memory chunks will remain allocated and reused. Prior to resetting,
         * the destructor of the custom object of type \p{TContained} is invoked and after
         * the reset, in-place construction is performed.
         *
         * To preserve further custom objects that might have been initially allocated, method
         * #TakeSnapshot might be used after the initial allocations (aka after construction of
         * the derived type). In this case, a derived type should expose an own version of this
         * method which prior and after the invocation this base implementation, either destroys
         * and reconstructs own objects or just leaves them intact.
         *
         * It is up to the implementation of the derived class if this method should be exposed
         * or not.
         *
         * It is furthermore allowed to use methods \alib{monomem,MonoAllocator::TakeSnapshot}
         * and \alib{monomem,MonoAllocator::Reset(const Snapshot&)} directly, to allow more flexible,
         * nested snapshots.
         *
         * @tparam  TArgs    The argument types used for re-constructing \p{TContained}.
         * @param   args     The arguments to re-construct the instance of \p{TContained}.
         ******************************************************************************************/
        template<typename... TArgs>
        void    Reset( TArgs&&... args )
        {
            monomem::Destruct(&fields->custom);

            fields->allocator.Reset( fields->snapshot );

            new (&fields->custom) TContained( std::forward<TArgs>(args)... );
        }


    /** ********************************************************************************************
     * Deleted default constructor.
     **********************************************************************************************/
    SelfContained()                                                                        = delete;

    /** ********************************************************************************************
     * Deleted copy constructor.
     **********************************************************************************************/
    SelfContained(SelfContained& )                                                         = delete;

    /** ********************************************************************************************
     * Move constructor. Sets field #fields of \p{src} to \c nullptr.
     *
     * @param move The object to move.
     **********************************************************************************************/
    SelfContained(SelfContained&& move)                                                     noexcept
    {
        this->fields=  move.fields;
        move .fields=  nullptr;
    }

    /** ********************************************************************************************
     * Deleted copy constructor.
     * @return Nothing (deleted).
     **********************************************************************************************/
    SelfContained& operator=(SelfContained& )                                              = delete;

    /** ********************************************************************************************
     * Move assignment operator. Sets field #fields of \p{src} to \c nullptr.
     *
     * @param src The object to move.
     * @return A reference to this object.
     **********************************************************************************************/
    SelfContained& operator=(SelfContained&& src)
    {
        this->allocator = src.allocator;
        src.allocator   = nullptr;
        return *this;
    }

    /** ********************************************************************************************
     * Destructor.
     * If #fields is not \c nullptr, the destructors of \p{TContained} and the monotonic allocator are
     * explicitly invoked.
     **********************************************************************************************/
    ~SelfContained()
    {
        if( !fields )  // might happen if moved.
            return;

        // Destruct the contained objects
        monomem::Destruct( &fields->custom    );
        monomem::Destruct( &fields->allocator );
    }

    /** ********************************************************************************************
     * Returns a non-constant reference to the stored object of type \p{TContained}.
     * @return A reference to \p{TContained}.
     **********************************************************************************************/
    TContained&             Self()
    {
        return fields->custom;
    }

    /** ********************************************************************************************
     * Returns a constant reference to the stored object of type \p{TContained}.
     * @return A constant reference to \p{TContained}.
     **********************************************************************************************/
    const TContained&       Self()                                                             const
    {
        return fields->custom;
    }

    /** ********************************************************************************************
     * Returns a non-constant reference to the self-contained allocator object.
     * @return A reference to the self-contained allocator object.
     **********************************************************************************************/
    MonoAllocator&         Allocator()
    {
        return fields->allocator;
    }

    /** ********************************************************************************************
     * Returns a constant reference to the self-contained allocator object.
     * @return A constant reference to the self-contained allocator object.
     **********************************************************************************************/
    const MonoAllocator&   Allocator()                                                        const
    {
        return fields->allocator;
    }

    /** ********************************************************************************************
     * With the construction of a self-contained object, a \alib{monomem,MonoAllocator::Snapshot}
     * is taken, used with method #Reset to clear the allocator without disposing the
     * memory of the self-contained object itself.
     *
     * With this method, this snapshot can be modified to use the current fill of the allocator.
     * Note that it is still allowed to reset the monotonic allocator "manually",
     * using custom snapshot objects received with \alib{monomem,MonoAllocator::TakeSnapshot}.
     **********************************************************************************************/
    void                    TakeSnapshot()
    {
        *fields->snapshot= *fields->allocator.TakeSnapshot();
    }
};

}} // namespace [alib::monomem]

#endif // HPP_ALIB_MONOMEM_SELF_CONTAINED
