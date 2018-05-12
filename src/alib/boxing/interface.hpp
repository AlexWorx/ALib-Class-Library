// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
/** @file */ // Hello Doxygen


//  set include guard
#ifndef HPP_ALIB_BOXING_INTERFACE
#define HPP_ALIB_BOXING_INTERFACE 1

namespace aworx { namespace lib { namespace boxing
{
/** ************************************************************************************************
 * This is the virtual base class to implement virtual methods invokable on supported
 * boxed objects.
 *
 * The following gives a step by step guidance on how to create, attach and invoke virtual interface
 * methods.
 *
 * <b>Defining a Custom %Interface:</b><br>
 * To add a custom interface method to <b>%ALib %Boxing</b>, the following approach has to be taken:
 * 1. A class needs to be derived from this class with a single custom virtual interface
 *    method named \c Invoke.<br>
 *    The first parameter of this virtual method needs to be of type <c>const aworx::Box&</c>. It
 *    will receive the boxed object that the interface method is invoked on. Other parameters
 *    and the return type may be chosen as needed.
 *    The method may be pure virtual.
 * 2. From this class in turn, type-specific versions need to be derived, which implement
 *    method \b Invoke in a type specific way.
 * 3. The type-specific versions should in addition be derived from template class
 *    \ref aworx::lib::lang::Singleton "Singleton". Alternatively, a singleton of themselves
 *    needs to be made accessible by other means.
 * 4. The singletons of the type-specific classes need to be attached to the corresponding
 *    type-specific boxer using method
 *    \ref aworx::lib::boxing::Boxer::DefineInterface   "Boxer::DefineInterface".<br>
 *    If, as proposed above, the type-specific classes use class \b %Singleton for implementing
 *    the singleton pattern, then template namespace function
 *    \ref aworx::lib::boxing::DefineInterface "DefineInterface" can be used for this purpose.
 * 5. Optionally, one default version of the custom interface class may be derived. This
 *    would implement a default behavior for boxed types that have no type-specific version in
 *    place. Default interface singletons are announced using method
 *    \ref aworx::lib::boxing::Boxer::DefineDefaultInterface "Boxer::DefineDefaultInterface",
 *    or preferably with templated namespace function
 *    \ref aworx::lib::boxing::DefineDefaultInterface "DefineDefaultInterface":
 *
 * Steps 4 and 5 must be done only once, e.g. in the bootstrap section of the process.
 *
 * <b>Using a Custom Interface:</b><br>
 * Now, the interface may be used as follows:
 * 1. Template method
 *    \ref aworx::lib::boxing::Box::HasInterface "Box::HasInterface" is used to detect if an
 *    object of type \b %Box, supports a certain interface.
 *    As a template parameter to this method, the main interface class type has to be provided.
 *    (The one that is directly derived from this class \b %Interface and is the parent class
 *    to all type-specific specializations.)<br>
 *    Note that method \ref aworx::lib::boxing::Box::HasInterface "Box::HasInterface" will return
 *    \c false, if the boxed object does not support the interface, even if a default interface
 *    is available.
 * 2. In the case that either \ref aworx::lib::boxing::Box::HasInterface "Box::HasInterface"
 *    returned \c true or a default interface implementation is available (which is expected to be
 *    globally known piece of information), template method
 *    \ref aworx::lib::boxing::Box::Invoke "Box::Invoke" can be used to invoke method \b %Invoke
 *    of the interface attached to the box. Likewise method \b Box::HasInterface, as a template
 *    parameter to this method, the main interface class type has to be provided.<br>
 *    The parameters passed to \b %Box::Invoke are the parameters defined of the interface
 *    method \b %Invoke (!), excluding the
 *    first one, which as described above needs to be <c>const aworx::Box&</c>.<br>
 *    If the return type of custom method \b Invoke is different to \c void, then this return type
 *    needs to be specified as the second template argument to method \b Box::Invoke.
 *
 * It is important to understand:
 * - If a default interface of a certain type is set, then no check using method
 *   \b %Box::HasInterface is needed prior to invoking \b %Box::Invoke, because the latter
 *   will always find some invokable singleton. Furthermore, in this case, method
 *   \b %Box::HasInterface may be used to detect if a type-specific interface is in place.
 *   A typical invocation would look like this:
 *
 *          aBox.Invoke<MyInterface>(5);
 *
 *
 * - If \b no default interface of a certain type is set, then it is advised to use method
 *   \b %Box::HasInterface to check if an interface exists, prior to using
 *   \b %Box::Invoke. In debug compilations an assertion is
 *
 *   The invocation would look like this:
 *
 *          if( aBox.HasInterface<MyInterface>() )
 *              aBox.Invoke<MyInterface>(5);
 *
 * <b>Multiple Invocations:</b><br>
 * In the case that an interface method is to be invoked several times in a row on the same
 * instance \b %Box, the interface singleton might be received with method
 * \ref aworx::lib::boxing::Box::GetInterface "Box::GetInterface" once and method \b %Invoke
 * can then be directly invoked. In this case, parameter \p{box} has to be provided to the call, as
 * demonstrated in this sample:
 *
 *      MyInterface* mIF= aBox.GetInterface<MyInterface>();
 *      if( mIF != nullptr )
 *          for( int i= 0; i< 100; ++i )
 *              mIF->Invoke( aBox, i );
 *
 * Here, object \b aBox is first used to retrieve the interface and then it is passed to each
 * invocation as a first parameter.
 *
 * \see
 *   For a step by step sample see chapter
 *   \ref alib_namespace_boxing_interfaces_sample "A Simple %Interface Sample" of the
 *   documentation of <b>%ALib %Boxing</b>.
 **************************************************************************************************/
class Interface
{
    /// Class \b %Boxer controls this class and is therefore a friend.
    friend class aworx::lib::boxing::Boxer;

    protected:
        /**
         *  Type information of the directly derived class, which is the abstract definition of a
         *  custom interface. Its descendants then all share the same type info.
         */
        const std::type_info&                   typeInfo;

    public:

        /**
         * Constructs an <b>%ALib %Boxing interface</b>.
         * Does nothing but saving the provided type information.
         * This type information has to reflect the directly derived class and to be the same
         * for all grandchild siblings. In detail, type information has to be passed as follows:
         * - Each direct descendant of this class represents an invokable box interface. Those
         *   direct descendants pass their types in the constructor to this class.
         * - Descendants of such interfaces which are specific implementations for
         *   dedicated custom types, do not pass their own type information. In contrast, through
         *   the use of their parent's constructor, they have to pass the same type information
         *   as their parent.
         *
         * @param type The type of the derived interface class.
         */
                 Interface( const std::type_info& type) : typeInfo(type) {}

    /**
     * Virtual destructor.
     */
    virtual ~Interface() {}
};



}}  // namespace aworx[::lib::boxing]

/// Type alias in namespace #aworx.
using     Interface  =   aworx::lib::boxing::Interface;

} // namespace [aworx]


#endif // HPP_ALIB_BOXING_INTERFACE
