// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
/** @file */ // Hello Doxygen

// check for alib.hpp already there but not us
#if !defined (HPP_ALIB)
    #error "include \"alib/alib.hpp\" before including this header"
#endif
#if defined(HPP_COM_ALIB_TEST_INCLUDES) && defined(HPP_ALIB_LANG_EXCEPTION)
    #error "Header already included"
#endif

// then, set include guard
#ifndef HPP_ALIB_LANG_EXCEPTION
#if !defined(DOX_PARSER)
#define HPP_ALIB_LANG_EXCEPTION 1
#endif

#if !defined (HPP_ALIB_LANG_ENUM_ENUM)
    #include "alib/lang/enum.hpp"
#endif

#if !defined (HPP_ALIB_LANG_ENUM_META_DATA_SPECIFICATION)
    #include "alib/lang/enummetadataspec.hpp"
#endif


//! @cond NO_DOX

//! [DOX_ALIB_ENUM_META_DATA_SPECIFCATION_TheType]
namespace aworx { namespace lib { namespace lang {
   class Exception;
}}} // namespace [aworx::lib::lang]
//! [DOX_ALIB_ENUM_META_DATA_SPECIFCATION_TheType]

//! [DOX_ALIB_ENUM_META_DATA_SPECIFCATION_decl]
ALIB_ENUM_SPECIFICATION_DECL( aworx::lib::lang::Exception, String, String )
//! [DOX_ALIB_ENUM_META_DATA_SPECIFCATION_decl]

//! @endcond

namespace aworx { namespace lib { namespace lang {


/** ************************************************************************************************
 * ## Introduction ##
 * This is a class used as a throwable object with C++ keyword \c throw.
 *
 * Often in C++ software, while the stack is unwinded due to a \c throw statement, the original
 * exception gets replaced (by user code) with a more meaningful one.
 * For example, if a resource string can not be loaded, a type of  "resource exception" may be
 * thrown. Then, the caller of this method has different information,  i.e. what kind of resource
 * string could not be loaded.
 *
 * Instead of such replacement, this class allows to add new information to a list of entries.
 * Precisely, the information passed with the constructor call of the class, simply becomes the
 * first entry in the list.  This way, all information along the "call stack" is preserved
 * which allows
 * - Exception handling code to inspect the entries and perform different actions depending
 *   on the original cause.
 * - User output that gives more meaningful information about what happened. (Of-course, this is
 *   especially non-productive debug and test runs.)
 *
 \~Comment ####################################################################################### \~
 * ## Custom %Enum Types As %Exception Codes ##
 * This class is not designed to be a base class for derived exception types. This is because
 * as explained above, objects of this class should not be replaced, but instead filled with entries
 * while the stack is unwinded.
 *
 * With other implementations of this approach to exceptions, usually an important problem is
 * raised: What can be used as an error code and how can error codes be kept "non-overlapping"
 * between different code units and even third party libraries? This class overcomes this
 * problem by leveraging several \alib concepts:
 * - Class \alib{lang,Enum} which allows type-safe storage of enumeration values of arbitrary
 *   types (including C++ 11 scoped enums). To reach this, that class leverages
 * - \ref aworx::lib::boxing "ALib Boxing". This way, runtime-type information about
 *   stored error codes of arbitrary enum elements can be received.
 * - \alib{lang,T_EnumMetaDataDecl,ALib enum meta data } to equip elements of arbitrary enum
 *   types with static meta information. This, besides other things is used to define
 *   the information text of enum entries.
 * - \alib{lang,Resources,ALib Resource Data}, which allows to change the information text of
 *   exceptions (e.g. translate to different languages or to tailor descriptions of exceptions
 *   defined in different code libraries to the appropriate wording of the software using such
 *   library).
 *
 * Finally, to convert an \b %Exceptions, including its list of entries into user readable
 * form:
 * - Exception entries allow to store arbitrary arguments using
 *   \alib{boxing,Boxes, lists of ALib Boxes}, which then can be nicely formatted using
 * - \alib{strings::format,FormatterBase,ALib Formatters}.
 *
 \~Comment ####################################################################################### \~
 * ## Preparing A Custom %Enum Type ##
 * To allow a custom (scoped) enumeration type to be used as an exception error code,
 * macro\ref ALIB_LANG_EXCEPTIONS is to be used with the type. The macro
 * does all that is needed:
 * - \p{TEnum} is equipped with \alib{lang,T_EnumMetaDataDecl,ALib enum meta data }. The meta
 *   data type is fixed to \alib{lang,Exception::TTuple}.
 * - The type is \alib{lang,T_Resourced,declared to be loaded} from library resources.
 * - A custom specialization of \alib{lang,EnumMetaData::CheckLoad} is provided that
 *   reads the meta data tuple table from the resources object of the according \alib{lang,Library}.
 *
 * \note
 *    As a consequence, no other meta information must (can) be declared with type \p{TEnum}
 *    when this macro is used. In cases when more/other meta data is needed for an enum type,
 *    the only way is to create a "mirror" enum class with the same underlying integer types and
 *    probably provide a custom assign operator or a conversion function.
 *    It is up to the user of \alib to solve such requirement externally.
 *
 *
 \~Comment ####################################################################################### \~
 * ## Throwing Class %Exception ##
 * With one or more enum types prepared, class Exception can now be created an thrown.
 * The constructor (as well as method \alib{lang,Exception::Add}) expects caller source
 * information. Usually those are provided using macro
 * \ref ALIB_CALLER_PRUNED. If the source information should still be
 * available in release compilations, then alternatively macro \ref ALIB_CALLER
 * may be used, which does not \e "prune" them.
 *
 * Then the enumeration element of the prepared enum class type is provided.
 * Finally, a list of optional parameters can be given. The optional parameters can be used
 * in two ways:
 * - Catch handlers might investigate into them to receive detailed information to decide
 *   how to act on an exception.
 * - Catch handlers can use method #Describe (alternatively leveraging class
 *   \alib{strings::format,SimpleTextBase,SimpleText} or using
 *   \ref aworx::lib::lox::LogTools::Exception "ALox LogTools" to nicely format the output).
 *
 * Here, a conflict may arise: Method #Describe passes all optional parameters
 * to a \alib{strings::format,FormatterBase,Formatter} along with a format string loaded from
 * resources. If now a catch handler needs some information that should not be passed to a
 * formatter, the way to cope with that is:
 * - Add the non-formattable arguments as the first one in the list.
 * - Add a corresponding amount of suppression placeholders <c>{!X}</c> to the start of the format
 *   string.
 *
 * This way, the argument will not be displayed and is easily accessible for the catch handler.
 *
 * ## Catching Exceptions, Adding Information, Rethrowing ##
 *
 * Due to the fact that \alib exceptions are rather heavy objects, they should be caught by
 * reference (with ampersand <c>'\&'</c>), like this:
 *
 *          catch( aworx::Exception& e )
 *          {
 *             ...
 *          }
 *
 *
 * In the case an exception should be rethrown, method #Add may be used to add one or more new
 * entries to the exception instance. Method #Add has the very same signature as the constructor.
 *
 * For the same reasons that exceptions should be caught by reference, rethrowing should be
 * done without the exception argument. While the following code compiles fine:
 *
 *        catch( aworx::Exception& e )
 *        {
 *           throw e.Add( ... );
 *        }
 *
 * it copies the exception. Therefore the following code scheme is preferred:
 *
 *        catch( aworx::Exception& e )
 *        {
 *           e.Add( ... );
 *           throw;
 *        }
 *
 \~Comment ####################################################################################### \~
 * ## Lifecycle Of Arguments ##
 *
 * The arbitrary arguments added to exception entries are internally stored in \alib{boxing,Boxes}.
 * As documented, fundamental C++ types are boxed as values, while custom types usually get
 * boxed as pointers (or references). Now, when the stack is unwinded, local objects will
 * become unavailable and accessing them is not allowed. This has to be kept in mind when adding
 * arguments: Some hae to be copied before passed and delete later.
 *
 * For all string like types (note: all supported, even 3rd party string classes get boxed as
 * arrays of characters!), for example the name of a file that could not be opened, this class
 * internally creates a copy already. Therefore, local strings can just be passed.
 * The destructor of this class deletes all string copies.
 *
 * For custom types, custom code is needed to delete the objects in the catch handler.
 * In the case that the exception is not caught, the occurring memory leak obviously is the smaller
 * problem.
 *
 * To avoid double deletion of objects, the copy and move constructor of the class have been
 * implemented. They both clear the entry vector of the source object. Hence, Exceptions can not
 * by "duplicated" by assigning one to another.
 *
 \~Comment ####################################################################################### \~
 * ## Using Negative %Exception Codes ##
 *
 * A final feature of class exception is implemented with method \alib{lang,Exception::Code}.
 * This method usually returns the \alib{lang,Enum} object added to the exception most recently.
 * But this is only half the truth: In fact, the method returns the last object in the list of
 * entries that has a positive enumeration value. This way, entries that are purely considered
 * "additional information" - in contrast to entries that somehow change the meaning of an
 * exception - may be declared using negative enumeration code. This way, the final
 * catch handler does not need to switch over just all exception codes that can be thrown, but
 * just over those that are considered to bring a different meaning to an exception.
 *
 \~Comment ####################################################################################### \~
 * ## Sample Code ##
 * A well documented sample of the three simple steps, namely:
 * 1. Define an enum class that lists the exceptions
 * 2. Set defaults for the externalized resource strings for each exception enum element
 * 3. Start throwing!
 *
 * that are needed to use \alib exceptions is given
 * \ref cpp_alib_lang_T_EnumMetaDataSpecification_exception_sample "here".
 *
 * ## See Also ##
 *   - To generate formatted descriptions of exception entries, method
 *     \alib{strings::format,SimpleTextBase::AddException,SimpleText::AddException} may be used.
 *   - With the use of \alox, static tool method \alox{LogTools::Exception} allows to log
 *     exceptions with one line of code.
 *
 * # Reference Documentation #
 **************************************************************************************************/
class Exception
{
    // #############################################################################################
    // Internals
    // #############################################################################################
    protected:


    // #############################################################################################
    // Nested types
    // #############################################################################################
    public:
        /**
         * Shortcut to the tuple type of custom enum meta data.
         * The tuple type is
         *      std::tuple<int,aworx::String,aworx::String>.
         *
         * The first string object denotes the name of the exception. The second provides a format
         * description.
         */
        using TTuple= T_EnumMetaDataSpecification<Exception>::TTuple;

        /**
         * Exception entries as stored in field #Entries.
         * The first is created with the creation of the exception itself.
         * Further entries may be added while the exception is forwarded along the call stack
         * using overloaded methods #Add.
         *
         * Hence the last entry, is probably the one with the most specific error enum element
         * and message, while earlier entries result from "lower level" code and thus
         * usually are more general errors.
         */
        struct Entry
        {
            const NTString File;          ///< Source information of the place of entry creation.
            int            Line;          ///< Source information of the place of entry creation.
            const NTString Func;          ///< Source information of the place of entry creation.
            const Enum     Code;          ///< An enum element of an "exception enabled" enum type.
            Boxes          Args;          ///< The arguments provided by the creator of the entry.
            Library&       EnumLibrary;   ///< The library object that stores meta information of
                                          ///< the enum type used. Will be used with method
                                          ///< #Describe to retrieve the format string.
            TTuple&        ErrorMetaData; ///< The meta data struct of the enum boxed in field
                                          ///< #Code.

            /**
             * Constructor storing parameters. Otherwise empty.
             * @param file          Value of #File.
             * @param line          Value of #Line.
             * @param func          Value of #Func.
             * @param code          Value of #Code.
             * @param args          Value of #Args.
             * @param enumLibrary   Value of #EnumLibrary.
             * @param errorMetaData Value of #ErrorMetaData.
             */
            Entry( const NTString& file, int line, const NTString& func,
                   const Enum&      code       , Boxes&  args,
                   Library&         enumLibrary, TTuple& errorMetaData )
            : File          (file)
            , Line          (line)
            , Func          (func)
            , Code          (code)
            , Args          (args)
            , EnumLibrary   (enumLibrary)
            , ErrorMetaData (errorMetaData)
            {}
        };


    // #############################################################################################
    // Fields
    // #############################################################################################

        /**
         *  The entries added while unwinding the stack. The entries are sorted in order of their
         *  creation. The first element holds is the entry created with the constructor.
         */
        std::vector<Entry>          Entries;

    // #############################################################################################
    // Constructor/Destructor
    // #############################################################################################
    public:
        /** ****************************************************************************************
         * Copy constructor moves the entries and deletes them in source object.
         * @param src The object to move.
         ******************************************************************************************/
        Exception( Exception& src )
        {
            this->Entries= std::move(src.Entries);
            src.Entries.clear();
        }

        /** ****************************************************************************************
         * Move constructor moves the entries and deletes them in source object.
         * @param src The object to move.
         ******************************************************************************************/
        inline
        Exception(Exception&& src) noexcept
        {
            this->Entries= std::move(src.Entries);
            src.Entries.clear();
        }

        /**
         * Constructor creating the initial entry.
         * Besides the given parameters, the library associated with \p{TEnum} is stored as well
         * as its meta data tuple representing element \p{errEnum}.
         *
         * @param file      File name of the place of entry creation.
         * @param line      Line number of the place of entry creation.
         * @param func      Function/method name of the place of entry creation.
         * @param errEnum   An enum element of an "exception enabled" enum type.
         * @param args      The arguments provided by the creator of the entry.
         *                  The library object that stores meta information of the enum type used.
         *                  Will be used with method #Describe to retrieve the format string.
         *
         *
         *
         * @tparam TArgs    The variadic template argument types.
         */
        template <typename TEnum,
                  typename... TArgs  >
        Exception( const NTString& file, int line, const NTString& func,
                   TEnum errEnum, TArgs&&... args )
        {
            AddFromBoxes( file,line,func, errEnum, Boxes( std::forward<TArgs>( args )... ) );
        }


        /** ****************************************************************************************
         * Destructor
         ******************************************************************************************/
        ALIB_API
        ~Exception();



    // #############################################################################################
    // Interface
    // #############################################################################################
         /**
          * Returns field \alib{lang,Exception::Entry::Code,Entry::Code} of the \b last entry in
          * the list that has a positive underlying enum element value.
          *
          * The rational here is explained in the class' documentation. In short, this allows
          * to add informational entries to the exception that are not changing the main exception
          * code and hence reduce the number of 'real' exceptions to be caught by
          * catch-handlers.
          *
          * Note that other codes might be accessed through public field #Entries directly.
          *
          * @return The most high level exception code.
          */
        ALIB_API
        const Enum&   Code();

        /**
         * Adds an entry.
         * Besides the given parameters, the library associated with \p{TEnum} is stored as well
         * as its meta data tuple representing element \p{errEnum}.
         *
         * @param file      File name of the place of entry creation.
         * @param line      Line number of the place of entry creation.
         * @param func      Function/method name of the place of entry creation.
         * @param errEnum   An enum element of an "exception enabled" enum type.
         * @param args      The arguments provided by the creator of the entry.
         *                  The library object that stores meta information of the enum type used.
         *                  Will be used with method #Describe to retrieve the format string.
         *
         * @tparam TArgs    The variadic template argument types.
         * @return Return <c>*this</c> to allow concatenated operations or use with throw statement.
         */
        template <typename TEnum,
                  typename... TArgs  >
        inline
        Exception&   Add( const NTString& file, int line, const NTString& func,
                          TEnum errEnum, TArgs&&... args )
        {
            return AddFromBoxes( file,line, func, errEnum, Boxes( std::forward<TArgs>( args )... ) );
        }

        /**
         * Adds an entry. Non-variadic version using class \b %Boxes for the parameters.
         *
         * Besides the given parameters, the library associated with \p{TEnum} is stored as well
         * as its meta data tuple representing element \p{errEnum}.
         *
         * @param file      File name of the place of entry creation.
         * @param line      Line number of the place of entry creation.
         * @param func      Function/method name of the place of entry creation.
         * @param errEnum   An enum element of an "exception enabled" enum type.
         * @param args      The arguments provided by the creator of the entry.
         *                  The library object that stores meta information of the enum type used.
         *                  Will be used with method #Describe to retrieve the format string.
         * @return Return <c>*this</c> to allow concatenated operations or use with throw statement.
         */
//! [DOX_ALIB_ENUM_META_DATA_SPECIFCATION_usinginlib]
template <typename TEnum> inline
typename std::enable_if< std::is_same< typename T_EnumMetaDataDecl<TEnum>::TTuple,
                                       typename T_EnumMetaDataSpecification<Exception>::TTuple
                                     >::value,
                         Exception&
                       >::type
AddFromBoxes( const NTString& file, int line, const NTString& func, TEnum errEnum, Boxes&& args )
//! [DOX_ALIB_ENUM_META_DATA_SPECIFCATION_usinginlib]
        {
            auto& metaInfo= *EnumMetaData<TEnum>::GetSingleton();
            metaInfo.CheckLoad();
            Entries.emplace_back( file,line,func,
                                  errEnum,
                                  args,
                                  T_Resourced<TEnum>::Lib(),
                                  *metaInfo.Get(errEnum) );
            copyStrings();
            return *this;
        }


    // #############################################################################################
    // Static interface
    // #############################################################################################
    public:
        /**
         * Formats the exception entry into \p{target}.
         * The format string is read from the resources using the \alib{lang,Library} stored
         * with the entry. The resource is located in the meta data of the enum, which is
         * also stored in the entry.
         * @param target The target string to format the entry description to.
         * @param entry  The exception entry.
         */
        static void   Describe( AString& target, Entry& entry );

    // #############################################################################################
    // protected methods
    // #############################################################################################
    protected:
        /**
         * Replaces all string type objects in the most recent entry with copies, created as
         * follows:
         *
         *         BoxedAs<AString>(* new AString( src ))
         *
         * These objects will be searched and deleted in the constructor.
         */
        ALIB_API
        void copyStrings();
};

}} // namespace aworx[::lib::lang]

/// Type alias in namespace #aworx.
using     Exception=           aworx::lib::lang::Exception;


}  // namespace aworx

// #################################################################################################
// Macros (for technical reasons, doxed in file alib.cpp)
// #################################################################################################
//! [DOX_ALIB_ENUM_META_DATA_SPECIFCATION_macro]
#define  ALIB_LANG_EXCEPTIONS( TEnum, ResourceLibrary, ResourceName )                              \
ALIB_ENUM_SPECIFICATION(  aworx::lib::lang::Exception,                                             \
                               TEnum, ResourceLibrary, ResourceName )                              \
//! [DOX_ALIB_ENUM_META_DATA_SPECIFCATION_macro]

// Just use the macro now for the exceptions of namespace aworx::lib::lang
ALIB_LANG_EXCEPTIONS(  aworx::lib::lang::Exceptions, aworx::lib::LANG, ASTR("Exceptions") )

#endif // HPP_ALIB_LANG_EXCEPTION
