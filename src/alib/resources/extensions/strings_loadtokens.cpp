// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib_precompile.hpp"
#if !defined(ALIB_C20_MODULES) || ((ALIB_C20_MODULES != 0) && (ALIB_C20_MODULES != 1))
#   error "Symbol ALIB_C20_MODULES has to be given to the compiler as either 0 or 1"
#endif
#if ALIB_C20_MODULES
    module;
#endif
// ======================================   Global Fragment   ======================================
#include "alib/resources/resources.prepro.hpp"
// ===========================================   Module   ==========================================
#if ALIB_C20_MODULES
    module ALib.Resources;
#   if ALIB_ENUMRECORDS
      import ALib.EnumRecords;
#   endif
#else
#   include "ALib.EnumRecords.Bootstrap.H"
#   include "ALib.Resources.H"
#endif
// ======================================   Implementation   =======================================
#if ALIB_ENUMRECORDS
// Windows.h might bring in max/min macros
#if defined( max )
    #undef max
    #undef min
#endif


namespace alib::strings::util {


#   include "ALib.Lang.CIFunctions.H"
#if !DOXYGEN
void LoadResourcedTokens(  ResourcePool&         resourcePool,
                           const NString&        resourceCategory,
                           const NString&        resourceName,
                           strings::util::Token* token,
               ALIB_DBG(   int                   dbgSizeVerifier, )
                           character             outerSeparator,
                           character             innerSeparator        )
{
    ALIB_DBG( int tableSize= 0; )
    int resourceNo= -1; // disable number parsing

    Substring parser= resourcePool.Get( resourceCategory, resourceName ALIB_DBG(, false ) );
    if( parser.IsNull() )
        resourceNo= 0; // enable number parsing

    for( ;; )
    {
        if (resourceNo >= 0)
            parser= resourcePool.Get( resourceCategory, NString256() << resourceName << resourceNo++
                                      ALIB_DBG(, false ) );

        ALIB_ASSERT_ERROR( resourceNo != 1 || parser.IsNotNull(),  "STRINGS/TOK",
          "Resource string(s) \"{}/{}(nn)\" not found when parsing token.",
          resourceCategory, resourceName )

        if( parser.IsEmpty() )
            break;

        while( parser.IsNotEmpty() )
        {
            String actValue= parser.ConsumeToken( outerSeparator );
            token->Define( actValue, innerSeparator );

            #if ALIB_DEBUG
                NCString errorMessage(nullptr);
                switch( token->DbgGetError() )
                {
                    case Token::DbgDefinitionError::OK:
                        break;
                    case Token::DbgDefinitionError::EmptyName:
                        errorMessage= "No token name found.";
                        break;
                    case Token::DbgDefinitionError::ErrorReadingSensitivity:
                        errorMessage= "Sensitivity value not found.";
                        break;
                    case Token::DbgDefinitionError::ErrorReadingMinLengths:
                        errorMessage= "Error parsing the list of minimum lengths.";
                        break;
                    case Token::DbgDefinitionError::TooManyMinLengthsGiven:
                        errorMessage= " A maximum of 7 minimum length values was exceeded.";
                        break;
                    case Token::DbgDefinitionError::InconsistentMinLengths:
                        errorMessage= "The number of given minimum length values is greater than 1 "
                                      "but does not match the number of segments in the identifier.";
                        break;
                    case Token::DbgDefinitionError::NoCaseSchemeFound:
                        errorMessage= "More than one minimum length value was given but no "
                                      "segmentation scheme could be detected." ;
                        break;
                    case Token::DbgDefinitionError::MinLenExceedsSegmentLength:
                        errorMessage= "A minimum length is specified to be higher than the token "
                                      "name, respectively the according segment name.";
                        break;
                    case Token::DbgDefinitionError::DefinitionStringNotConsumed:
                        errorMessage= "The definition string was not completely consumed.";
                        break;
                    case Token::DbgDefinitionError::ZeroMinLengthAndNotLastCamelHump:
                        errorMessage= "Zero minimum length provided for segment which is not the last\n"
                                      "of a camel case token.";
                        break;

                    default: ALIB_ERROR( "RESOURCES", "Illegal switch state." ) break;
                }

                if( errorMessage.IsNotEmpty() )
                {
                    ALIB_ERROR( "STRINGS", errorMessage,
                        "\n(While reading token table.)\n"
                        "    Resource category (module name):  \"{}\"\n"
                        "    Resource name:                    \"{}\"\n"
                        "    Token value parsed:               \"{}\"",
                        resourceCategory, resourceName, actValue          )
                }

            #endif


            ++token;
            ALIB_DBG( tableSize++; )
        }
    }

    // check if there are more coming (a gap in numbered definition)
    #if ALIB_DEBUG
    if( resourceNo > 1 )
        for( int i= 0 ; i < 35 ; ++i )
        {
            if( resourcePool.Get( resourceCategory, NString256() << resourceName << (resourceNo + i)
                                  ALIB_DBG(, false ) ).IsNotNull() )
            {
                ALIB_ERROR( "STRINGS",
                   "Detected a \"gap\" in numbering of resource strings while parsing "
                   "resource token table: "
                   "From index {} to {}.\n"
                   "  Resource category/name: {} / {}",
                   resourceNo - 1, resourceNo + i - 1, resourceCategory, resourceName )
            }
        }
    #endif


    ALIB_ASSERT_ERROR( dbgSizeVerifier == tableSize, "STRINGS/TOK",
        "Size mismatch in resourced token table:\n"
        "    Resource category (module name):  \"{}\"\n"
        "    Resource name:                    \"{}\"\n"
        "    Resourced table size:             [{}]\n"
        "    Expected table size:              [{}]",
        resourceCategory, resourceName, tableSize, dbgSizeVerifier )
}
#endif // !DOXYGEN

#   include "ALib.Lang.CIMethods.H"

} // namespace [alib::strings::util]

#endif // ALIB_ENUMRECORDS
