// #################################################################################################
//  aworx::lib::lox::core - ALox Logging Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
/** @file */ // Hello Doxygen

// include ALox main header first...
#if !defined (HPP_ALIB_ALOX)
    #include "alib/alox/alox.hpp"
#endif

// then, set include guard
#ifndef HPP_ALOX_CORE_LOGGER
#define HPP_ALOX_CORE_LOGGER 1


// #################################################################################################
// includes
// #################################################################################################

#if !defined (_GLIBCXX_VECTOR) && !defined(_VECTOR_)
    #include <vector>
#endif


namespace aworx { namespace lib { namespace lox {
/** ************************************************************************************************
 * This is the C++ namespace for internal main classes and other things belonging to
 * the <em>%ALox Logging Library</em>.<p>
 * Types and interfaces found in this namespace are not designed for access by users of the
 * library. However, if extending \alox, e.g. by implementing new custom <em>loggers</em>, things
 * found here provide an important foundation.
 *
 * Developed by A-Worx GmbH and published under Boost Software License.
 **************************************************************************************************/
namespace core
{

// forward declarations
class Domain;


/** ************************************************************************************************
 * This is central class of the \alox logging implementation. It is **not** recommended to use this
 * class directly for logging. Instead, use the simple and convenient static interface class
 * Log or, for release logging and other more complex operations use a Lox instance.
 * The class is abstract. To implement an own log stream, derive a new Logger class and implement
 * the abstract method #Log.
 **************************************************************************************************/
class Logger : public lib::threads::SmartLock
{
    // #############################################################################################
    // Internal fields
    // #############################################################################################

    /**
     *  An utility value. Has to be first in the class. Will be initialized prior to any other
     *  field. Usually, library initialization is performed with \alib{lang,Library::Init} on
     *  singleton \ref aworx::lib::ALIB. However, is this is not invoked explicitly on bootstrap,
     *  this fields initialization code will do the job.
     */
    private:    bool _apiInitializer;

    protected:
        /**
         * The name of the \e Logger. Used as a reference to a logger. All loggers attached to a
         * \b %Lox have to differ in their names.
         * If no name is specified with the constructor, the name will by the same as #TypeName.
         */
        NString32                   Name;

        /**
         *  The type name of the \e Logger. This is set by the derived class similar to the class
         *  name.
         */
        NString32                   TypeName;

    // #############################################################################################
    // public fields
    // #############################################################################################
    public:
        /**
         * The number of logs actually performed so far. In a text logger this is similar to the line
         * number, despite the fact that a single log call can produce more than one line.
         */
        int                         CntLogs                                                      =0;

        /// The creation time of the \e Logger
        lib::time::Ticks            TimeOfCreation;

        /// Timestamp of  the last log operation
        lib::time::Ticks            TimeOfLastLog;

    // #############################################################################################
    // Abstract methods
    // #############################################################################################
    public:
        /** ****************************************************************************************
         * This is the central method that derived logger classes have to implement to log a
         * message. When it is invoked the <em>%Log %Domain's %Verbosity</em> was already checked
         * against parameter \p{verbosity}. The only action to take is to perform the log
         * itself.<br>
         * Parameter \p{logables} contains at least one object, the one provided with the log
         * statement. Other objects that might be included in the list, are <em>Prefix Objects</em>
         * corresponding to the \p{scope}.
         *
         * @param dom       The <em>Log Domain</em>.
         * @param verbosity The verbosity of the message.
         * @param logables  The list of objects to log.
         * @param scope     Information about the scope of the <em>Log Statement</em>..
         ******************************************************************************************/
        virtual void   Log( Domain& dom, Verbosity verbosity, Boxes& logables, ScopeInfo& scope) =0;

    // #############################################################################################
    // Constructor/Destructor
    // #############################################################################################
    protected:
        /** ****************************************************************************************
         * Constructs a logger. This constructor is protected because this class is abstract.
         *
         * Note: This constructor is implemented in the header and annotated as inline.
         * This way, the Log::InitALox call receives the size of classes from the compilation unit
         * that invokes the constructor. If different compile options are set, we have a chance to
         * detect them here.
         * @param name     The name of the \e Logger. If empty, it defaults to the type name.
         *                 Will be converted to upper case.
         * @param typeName The type of the \e Logger.
         *                 Will be converted to upper case.
         ******************************************************************************************/
        inline
        Logger( const NString& name, const NString& typeName )
        : _apiInitializer( ALOX.Init() )
        , Name(name)
        , TypeName(typeName)
        , TimeOfCreation ()
        , TimeOfLastLog  ()
        {
            if ( this->Name.IsEmpty() )
                this->Name << typeName;
            this->Name    .ToUpper();
            this->TypeName.ToUpper();
        }

        /** ****************************************************************************************
         *  Destructs a logger
         ******************************************************************************************/
    public:
        virtual  ~Logger()
        {}

    // #############################################################################################
    // Interface
    // #############################################################################################
    public:
        /** ****************************************************************************************
         * Returns the name of this logger. The name has to be unique for all \e %Loggers attached
         * to a \b %Lox.
         * @return The loggers name.
         ******************************************************************************************/
        inline
        const NTString&     GetName()      const  { return Name;      }

        /** ****************************************************************************************
         * Returns the constant type name of this logger. The type name is defined by the class
         * and hence provides a sort of runtime type information.
         * @return The loggers type name.
         ******************************************************************************************/
        inline
        const NTString&     GetTypeName()  const  { return TypeName;  }


}; // class Logger

}}} // namespace aworx[::lib::lox::core]


/// Type alias in namespace #aworx.
using     Logger=       aworx::lib::lox::core::Logger;

}  // namespace aworx



namespace aworx { namespace lib { namespace strings {

// We are faking all template specializations of namespace strings for doxygen into namespace
// strings::applyto to keep the documentation of namespace string clean!
#if defined(DOX_PARSER)
namespace thirdparty {
/**
 * Template type and function specializations found here allow to apply types of logging library
 * \alox to be "applicable" to <b>%ALib Strings</b>.
 *
 * \attention
 *   This is a non-existing namespace, exclusively defined for the
 *   [documentation parser](http://www.stack.nl/~dimitri/doxygen).
 *   All types and functions described in this namespace are residing in namespace
 *   #aworx::lib::strings (just as they have to in respect to C++ rules).
 *   This documentation is moved here to keep the original namespace documentation clean.
 */
namespace alox {

#endif

    /** ********************************************************************************************
     * Specialization of TMP struct
     * \alib{strings,T_Apply} for type \b %Logger.
     **********************************************************************************************/
    template<> struct T_Apply<lox::core::Logger,character>  : public std::true_type
    {

        /** ****************************************************************************************
         * Writes the name of the logger. In case the type name is different, it will be append
         * in braces.
         *
         * See template struct\alib{strings,T_Apply} for more information.
         *
         * @param  target The AString to append \p{src} to.
         * @param  logger The logger to apply.
         * @return The length of the given string \p{src} which was appended to \p{target}.
         ******************************************************************************************/
        static inline integer  Apply( AStringBase<character>& target, const lox::core::Logger& logger )
        {
            integer origTargetLength= target.Length();
            target << logger.GetName();
            if ( !logger.GetName().Equals( logger.GetTypeName() ) )
                target << " (" << logger.GetTypeName() << ")";

            return target.Length() - origTargetLength;
        }
    };

#if defined(DOX_PARSER)
}}
#endif

}}}// namespace [aworx::lib::lox::strings]

#endif // HPP_ALOX_CORE_LOGGER
