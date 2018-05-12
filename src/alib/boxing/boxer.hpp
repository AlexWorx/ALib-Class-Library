// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
/** @file */ // Hello Doxygen


#if !defined (HPP_ALIB_BOXING_LIB)
    #include "alib/boxing/boxing.hpp"
#endif

//  set include guard
#ifndef HPP_ALIB_BOXING_BOXER
//! @cond NO_DOX
#define HPP_ALIB_BOXING_BOXER 1
//! @endcond

#if ALIB_DEBUG && !defined (_GLIBCXX_VECTOR) && !defined(_VECTOR_)
    #include <vector>
#endif


namespace aworx { namespace lib { namespace boxing
{
// #################################################################################################
// forward declarations
// #################################################################################################
class Box;

// #################################################################################################
// class Boxer
// #################################################################################################
/**
 * This class is used internally with <b>%ALib %Boxing</b> to provide runtime type information and
 * virtual method invocations to boxed values.
 * A singleton instance of a derived type of this class is attached to each
 * \ref aworx::lib::boxing::Box "Box". Such derived type is generated using templated
 * derived class
 * \ref aworx::lib::boxing::BoxerT      "BoxerT<TBoxed>", respectively, in case of array types, class
 * \ref aworx::lib::boxing::ArrayBoxerT "ArrayBoxerT<TBoxed>".
 */
class Boxer
{
    friend class Boxing; ///< A friend class.
    friend class Box;    ///< A friend class.

    // #############################################################################################
    // Protected fields
    // #############################################################################################
    protected:
        /** Information about the encapsulated type. In case of arrays, this represents the
         *  boxer type, while the element type is provided in #elemType. */
        const std::type_info&   type;

        /// In case of array types, information about the element type.
        const std::type_info&   elemType;

        /// In case of array types, information about the size of the element type.
        const size_t            sizeofElemType;

        /// Box interfaces attached using #DefineInterface.
        TypeMap<Interface*>     interfaces;

        /** This static singleton field holds default interfaces that apply to each and every
         * boxed type. See #DefineDefaultInterface  for more information. */
        static ALIB_API
        TypeMap<Interface*>     defaultInterfaces;



    // #############################################################################################
    // Protected Constructor/Destructor
    // #############################################################################################
    protected:
        /**
         * Constructs a boxer. Takes a constant reference to a \b std::typeinfo object
         * which has to represent the type of a derived class.
         *
         * Array types need to use the alternative constructor.
         *
         * @param ptype  Type information of the derived Boxer type.
         */
        Boxer( const std::type_info& ptype )
        : type( ptype ), elemType( ptype ), sizeofElemType(0), interfaces()
        {
            ALIB_DBG( BOXING.dbgCheckNewBoxer( this ); )
        }

        /**
         * Constructs a boxer for array types. Takes a second \b std::typeinfo object
         * which denotes element type of the boxed array.
         *
         * @param pType        Type information of the derived \b %Boxer type.
         * @param pElemType    Type information of the elements of the boxed array.
         * @param pSizeofElem  The size of elements of the boxed array.
         */
        Boxer( const std::type_info& pType,
               const std::type_info& pElemType,
               size_t                pSizeofElem )
         : type( pType ), elemType( pElemType ), sizeofElemType(pSizeofElem), interfaces()
         {
             ALIB_DBG( BOXING.dbgCheckNewBoxer( this ); )
         }


        /**
         * Virtual destructor.<br>
         * Note: This is not really need. However, as derived classes \b %BoxerT and
         * \b %ArrayBoxerT derive from singleton, which is virtual, having this constructor allows
         * to clean memory on termination more 'precisely' and reduce warnings of analytics tools
         * like \b valgrind.
         */
        virtual ~Boxer()  {}


    // #############################################################################################
    // Interface
    // #############################################################################################
    public:
        /**
         * Adds the given interface object to the list of default interfaces. Default interfaces
         * are kept in a static collection and are used with boxable types that have no
         * type-specific  interface set.
         *
         * Adding a default interface is optional. As a sample, \alib provides interface
         * \ref aworx::lib::strings::boxing::IApply "IApply" to write a string
         * representation of an object. Although, being the base to type specific interfaces,
         * this class itself provides a default implementation for \e 'unknown' types,
         * writing the runtime type name and memory address of the object.
         * That 'base' version of the interface is registered with this method.
         *
         * Default interfaces are not detectable with method
         * \ref aworx::lib::boxing::Box::HasInterface "Box::HasInterface", but of-course they are
         * used with
         * \ref aworx::lib::boxing::Box::Invoke       "Box::Invoke" and
         * \ref aworx::lib::boxing::Box::GetInterface "Box::GetInterface" in the moment no
         * type-specific interface is available.
         *
         * \note
         *   This method should not be called directly. It is advised to use namespace function
         *   \ref aworx::lib::boxing::DefineDefaultInterface "DefineDefaultInterface"
         *   instead.
         *
         * @param interface The singleton instance of the default interface class
         */
        static inline void  DefineDefaultInterface( Interface* interface )
        {
            Boxer::defaultInterfaces[interface->typeInfo]=  interface;
            ALIB_DBG( BOXING.dbgKnownInterfaces[interface->typeInfo]= interface; )
        }

        /**
         * Adds the given interface object to the list of interfaces of this boxer. This way,
         * method
         * \ref aworx::lib::boxing::Box::HasInterface "Box::HasInterface" will return true for
         * the provided interface base type and methods
         * \ref aworx::lib::boxing::Box::Invoke       "Box::Invoke" and
         * \ref aworx::lib::boxing::Box::GetInterface "Box::GetInterface" will use, respectively
         * return the given singleton instance.
         *
         * \note
         *   This method should not be called directly. It is advised to use namespace function
         *   \ref aworx::lib::boxing::DefineInterface          "DefineInterface", respectively,
         *   to define interfaces.
         *
         * @param interface The singleton instance of the default interface class
         */
        inline void         DefineInterface( Interface* interface)
        {
            #if ALIB_DEBUG
                if( interfaces.find( interface->typeInfo )  != interfaces.end() )
                {
                    #if ALIB_MODULE_STRINGS
                        ALIB_MESSAGE( ASTR("ALib Boxing: Replacing interface "),
                                      AString(debug::TypeDemangler(interface->typeInfo).Get()).ToCString(),
                                      ASTR(" for type "),
                                      AString(debug::TypeDemangler(type).Get()).ToCString()                )
                    #else
                        ALIB_MESSAGE( ASTR("ALib Boxing: Replacing interface "),
                                              debug::TypeDemangler(interface->typeInfo).Get()             ,
                                      ASTR(" for type "),
                                              debug::TypeDemangler(type).Get()                             )
                    #endif
                }
            #endif

            interfaces.insert(std::make_pair(std::reference_wrapper<const std::type_info>(interface->typeInfo), interface) );

            ALIB_DBG(
                BOXING.dbgKnownInterfaces   [interface->typeInfo]= interface;
                BOXING.dbgKnownInterfaceImpl[typeid(*interface) ]= interface;
            )
        }

        /** ****************************************************************************************
         * Returns \c true if this boxer represents boxed array types. In this case, method
         * \ref aworx::lib::boxing::Box::Length "Box::Length"
         * will return the length of the array and
         * \ref aworx::lib::boxing::Box::UnboxElement "Box::UnboxElement" may be used to
         * access elements of the array.
         *
         * @return \c true if this boxer is representing an array of custom types.
         ******************************************************************************************/
        inline  bool        IsArray()     const { return type != elemType;   }
};


// #################################################################################################
// BoxerT/ArrayBoxerT
// #################################################################################################

/**
 * This is a templated internal helper class for \alib boxing, with the only means to provide a
 * singleton of a derived boxer type used for boxing arrays types.
 */
template<typename TBoxable>
class BoxerT  : public Boxer   , public Singleton<BoxerT<TBoxable>>
{
    /// Allow construction by class \b %Singleton (implementation of strict-singleton concept).
    friend class lang::Singleton<BoxerT<TBoxable>>;

    private:
        /**
         * Constructor, passing the type information of the templated type to parent class \b %Boxer.<br>
         * This constructor is private and class \b %Singleton is a friend, which makes instances
         * of this class being a 'strict' singleton (only that singleton instance exists).
         */
        inline BoxerT() : Boxer(typeid(TBoxable))
        {}
};

/**
 * This is a templated internal helper class for \alib boxing, with the only means to provide a
 * singleton of a derived boxer type used for arrays of a certain element type.
 */
template<typename TElementType>
class ArrayBoxerT : public Boxer, public Singleton<ArrayBoxerT<TElementType>>
{
    /// Allow construction by class \b %Singleton (implementation of strict-singleton concept).
    friend class lang::Singleton<ArrayBoxerT<TElementType>>;

    private:
        /**
         * Constructor. Passes the type of this class and the element type to parent class Boxer.
         *
         * This constructor is private and class \b %Singleton is a friend, which makes each
         * template instance of this class being a 'strict' singleton (only that singleton instance
         * exists).
         */
        inline  ArrayBoxerT() : Boxer( typeid(ArrayBoxerT<TElementType>), typeid(TElementType), sizeof(TElementType) )
        {}
};





}}} // namespace [aworx::lib::boxing]


#endif // HPP_ALIB_BOXING_BOXER
