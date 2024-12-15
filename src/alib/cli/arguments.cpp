// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !DOXYGEN
#   include "alib/cli/arguments.hpp"
#   include "alib/cli/commandline.hpp"
#   include "alib/strings/util/tokenizer.hpp"
#   include "alib/enums/recordparser.hpp"
#   include "alib/lang/message/report.hpp"
#endif // !DOXYGEN


// ##########################################################################################
// ### Tuple loaders
// ##########################################################################################

namespace alib {  namespace cli {

// ##########################################################################################
// ### Methods of Command, Option, etc.
// ##########################################################################################

void CommandDecl::addParamDecls()
{
    Tokenizer tknzr( record.parameters , '/' );
    while( tknzr.Next().IsNotEmpty() )
    {
ALIB_DBG( bool found= false; )

        for( auto* paramDecl : CmdLine.ParameterDecls )
            if( paramDecl->Name().StartsWith<CHK, lang::Case::Ignore>( tknzr.Actual ) )
            {
                Parameters.PushBack( paramDecl );
       ALIB_DBG(found= true; )
                break;
            }

        ALIB_ASSERT_ERROR( found, "CLI",
                           "Parameter named {!Q} not found while loading resources of command {!Q}.",
                           tknzr.Actual, Identifier()  )
    }
}

ParameterDecl* CommandDecl::GetParameterDecl(const String& name )
{
    for( auto* param : Parameters )
        if( param->Name().Equals<NC>( name ) )
            return param;

    return nullptr;
}


bool Option::Read( OptionDecl& decl, String& argProbablyReplaced, const integer argNo )
{
    String     identifier  = decl.Identifier();
    character  identifierC = decl.IdentifierChar();
    auto       argsExpected= decl.QtyExpectedArgsFollowing();
    auto       argsLeft    = CmdLine->ArgsLeft.size() -1;

    // read single/double hyphen options once
    Substring arg= argProbablyReplaced;
    Substring inArgArgument;
    auto      valueSeparator= arg.IndexOf<CHK, lang::Case::Sensitive>( decl.ValueSeparator() );
    if( valueSeparator > 0 )
        arg.Split<NC>( valueSeparator, inArgArgument, decl.ValueSeparator().Length() );

    bool potentialIllegalContinuation= false;
    if ( !(   (     identifier.IsNotEmpty()
               &&  arg.ConsumeString(A_CHAR("--"))
               &&  arg.Length() >=  decl.MinimumRecognitionLength()
               && (    identifier.StartsWith<CHK, lang::Case::Ignore>( arg )
                    || true == (potentialIllegalContinuation= arg.StartsWith<CHK,lang::Case::Ignore>( identifier )) )

             )

          || (     arg.ConsumeChar('-')
               &&  arg.ConsumeChar(identifierC)                             )
             )
        )
        return false;

    // Throw if the identifier was fully matched, but the argument continues with a
    // non-alphanumerical value.
    // (This is only allowed if separator was set in resource. But in this case, the flag will
    // never match, because of the code above...)
    if( potentialIllegalContinuation )
    {
        auto nextChar= arg.CharAt<NC>( identifier.Length() );
        if( !isalnum( nextChar ) )
            throw Exception( ALIB_CALLER_NULLED, cli::Exceptions::IllegalOptionNameContinuation,
                             identifier, argNo, CmdLine->GetArg(argNo) );
        return false;
    }

    Declaration= &decl;
    Position=    argNo;

    ConsumedArguments= 1;


    // store in-arg argument
    if( valueSeparator > 0)
    {
        Args.PushBack( inArgArgument );
        if(argsExpected > 0 )
            --argsExpected;
    }


    // error: not enough params
    if (  argsExpected > integer(argsLeft) )
        throw Exception( ALIB_CALLER_NULLED, cli::Exceptions::MissingOptionValue,
                         decl.Identifier(), argNo, CmdLine->GetArg(argNo),
                         argsExpected, argsLeft                              );

    // store arg strings
    for( integer i= 0; i < argsExpected; ++i )
        Args.PushBack( CmdLine->GetArg(argNo + 1 + i) );
    ConsumedArguments+= argsExpected;

    return true;
}

bool Command::Read( CommandDecl& decl )
{
    auto&  identifier=  decl.Identifier();
    String arg       =  CmdLine->PeekArg();

    if (     arg.Length() <  decl.MinimumRecognitionLength()
         ||  !identifier.StartsWith<CHK,lang::Case::Ignore>( arg )  )
        return false;

    Declaration= &decl;
    Position=    CmdLine->ArgsLeft[0];
    CmdLine->PopArg();
    ConsumedArguments= 1;

    if( decl.Parameters.Count() == 0 )
        return true;

    auto paramDeclIt= decl.Parameters.begin();
    while( paramDeclIt != decl.Parameters.end() )
    {
        Parameter param( CmdLine );
        bool continueReading= param.Read( **paramDeclIt );
        if( param.ConsumedArguments )
        {
            ConsumedArguments+= param.ConsumedArguments;
            Parameter* paramFound= CmdLine->allocator().New<Parameter>(param);
            if ( (*paramDeclIt)->IsOptional() )
                ParametersOptional.PushBack( paramFound );
            else
                ParametersMandatory.PushBack( paramFound );

            // start from the beginning
            paramDeclIt= decl.Parameters.begin();
        }
        else
            ++paramDeclIt;

        // stop here if parameter read signaled this
        if( !continueReading )
            return false;
    }

    return true;
}

Parameter* Command::GetParsedParameter(const String& name )
{
    #if ALIB_DEBUG
        bool found= false;
        for( auto* paramDecl : Declaration->Parameters )
            if( paramDecl->Name().Equals<NC>(name) )
            {
                found = true;
                break;
            }
        ALIB_ASSERT_ERROR( found, "CLI", "Requested parameter {!Q} not defined.", name )
    #endif

    for( auto* param : ParametersMandatory )
        if( param->Declaration->Name().Equals<NC>( name ) )
            return param;

    for( auto* param : ParametersOptional )
        if( param->Declaration->Name().Equals<NC>( name ) )
            return param;

    return nullptr;
}

String Command::GetParsedParameterArg( const String& name )
{
    Parameter* param= GetParsedParameter( name );
    return param && param->Args.IsNotEmpty() ? param->Args.Front()
                                             : NULL_STRING;
}


bool Parameter::Read( ParameterDecl& decl )
{
    Substring arg=     CmdLine->PeekArg();
    if( arg.IsNull() )
        return false;

    auto& identifier=  decl.Identifier();
    if ( identifier.IsEmpty() && decl.IsOptional() )
        return false;

    auto      valueSeparator= arg.IndexOf<CHK, lang::Case::Sensitive>( decl.ValueSeparator() );
    Substring inArgArgument;
    if( valueSeparator > 0 )
        arg.Split<NC>( valueSeparator, inArgArgument, decl.ValueSeparator().Length() );

    if (         identifier.IsEmpty()
         || (    arg.Length() >=  decl.MinimumRecognitionLength()
              && identifier.StartsWith<CHK,lang::Case::Ignore>( arg ) )  )
    {
        ConsumedArguments= 1;
        Declaration=     &decl;
        Position=        CmdLine->ArgsLeft[0];
        CmdLine->PopArg();
        int argsExpected= decl.QtyExpectedArgsFollowing();

        if( decl.Identifier().IsEmpty() )
        {
            Args.PushBack( arg );
        }
        else
        {
            if( inArgArgument.IsNotEmpty() )
            {
                Args.PushBack( inArgArgument );
                --argsExpected;
            }
        }

        // stop command?
        if( argsExpected < 0 )
            return false;

        // error: not enough params
        if ( argsExpected > static_cast<integer>(CmdLine->ArgsLeft.size()) )
            throw Exception( ALIB_CALLER_NULLED, cli::Exceptions::MissingParameterValue,
                             decl.Name(), Position, CmdLine->GetArg(Position),
                             argsExpected, CmdLine->ArgsLeft.size() );

        // store arg strings
        for( size_t i= 0; i < size_t( argsExpected ); ++i )
        {
            Args.PushBack( CmdLine->GetArg(*CmdLine->ArgsLeft.begin()) );
            CmdLine->ArgsLeft.erase( CmdLine->ArgsLeft.begin() );
        }
        ConsumedArguments+= size_t( argsExpected );
    }


    return true;
}

void ERCommandDecl  ::Parse()
{
    enums::EnumRecordParser::Get( ERSerializable::EnumElementName          );
    enums::EnumRecordParser::Get( ERSerializable::MinimumRecognitionLength );
    enums::EnumRecordParser::Get( parameters                               , true );
}

void EROptionDecl   ::Parse()
{
    enums::EnumRecordParser::Get( ERSerializable::EnumElementName          );
    enums::EnumRecordParser::Get( ERSerializable::MinimumRecognitionLength );
    enums::EnumRecordParser::Get( identifierChar                           );
    enums::EnumRecordParser::Get( valueSeparator                           );
    enums::EnumRecordParser::Get( RequiredArguments                 );
    enums::EnumRecordParser::Get( shortcutReplacementString                , true );
}

void ERParameterDecl::Parse()
{
    enums::EnumRecordParser::Get( ERSerializable::EnumElementName          );
    enums::EnumRecordParser::Get( ERSerializable::MinimumRecognitionLength );
    enums::EnumRecordParser::Get( identifier                               );
    enums::EnumRecordParser::Get( valueSeparator                           );
    enums::EnumRecordParser::Get( valueListSeparator                       );
    enums::EnumRecordParser::Get( RequiredArguments                 );
    enums::EnumRecordParser::Get( isOptional                               , true );
}

void ERExitCodeDecl ::Parse()
{
    enums::EnumRecordParser::Get( ERSerializable::EnumElementName          );
                                  ERSerializable::MinimumRecognitionLength = 0;
    enums::EnumRecordParser::Get( associatedCLIException                   , true );
}

}} // namespace alib::cli
