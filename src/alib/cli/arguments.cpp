// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2023 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)
#   if !defined (HPP_ALIB_CLI_CLI)
#      include "alib/cli/arguments.hpp"
#   endif
#   if !defined (HPP_ALIB_CLI_COMMANDLINE)
#      include "alib/cli/commandline.hpp"
#   endif
#   if !defined (HPP_ALIB_STRINGS_UTIL_TOKENIZER)
#      include "alib/strings/util/tokenizer.hpp"
#   endif
#   if !defined (HPP_ALIB_ENUMS_RECORDPARSER)
#      include "alib/enums/recordparser.hpp"
#   endif
#   if !defined(HPP_ALIB_RESULTS_REPORT)
#      include "alib/results/report.hpp"
#   endif
#endif // !defined(ALIB_DOX)


// ##########################################################################################
// ### Tuple loaders
// ##########################################################################################

namespace aworx { namespace lib { namespace cli {

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
            if( paramDecl->Name().StartsWith<true, Case::Ignore>( tknzr.Actual ) )
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
        if( param->Name().Equals( name ) )
            return param;

    return nullptr;
}


bool Option::Read( OptionDecl& decl, String& argProbablyReplaced, const integer argNo )
{
    String     identifier     = decl.Identifier();
    character  identifierC    = decl.IdentifierChar();
    auto       qtyArgsExpected= decl.QtyExpectedArgsFollowing();
    auto       qtyArgsLeft    = CmdLine->ArgsLeft.size() -1;

    // read single/double hyphen options once
    Substring arg= argProbablyReplaced;
    Substring inArgArgument;
    auto      valueSeparator= arg.IndexOf<false, Case::Sensitive>( decl.ValueSeparator() );
    if( valueSeparator > 0 )
        arg.Split<false>( valueSeparator, inArgArgument, decl.ValueSeparator().Length() );

    bool potentialIllegalContinuation= false;
    if ( !(   (     identifier.IsNotEmpty()
               &&  arg.ConsumeString(A_CHAR("--"))
               &&  arg.Length() >=  decl.MinimumRecognitionLength()
               && (    identifier.StartsWith<true, Case::Ignore>( arg )
                    || true == (potentialIllegalContinuation= arg.StartsWith<true,Case::Ignore>( identifier )) )

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
        auto nextChar= arg.CharAt<false>( identifier.Length() );
        if( !isalnum( nextChar ) )
            throw Exception( ALIB_CALLER_NULLED, cli::Exceptions::IllegalOptionNameContinuation,
                             identifier, argNo, CmdLine->GetArg(argNo) );
        return false;
    }

    Declaration= &decl;
    Position=    argNo;

    QtyArgsConsumed= 1;


    // store in-arg argument
    if( valueSeparator > 0)
    {
        Args.PushBack( inArgArgument );
        if(qtyArgsExpected > 0 )
            --qtyArgsExpected;
    }


    // error: not enough params
    if (  qtyArgsExpected > integer(qtyArgsLeft) )
        throw Exception( ALIB_CALLER_NULLED, cli::Exceptions::MissingOptionValue,
                         decl.Identifier(), argNo, CmdLine->GetArg(argNo),
                         qtyArgsExpected, qtyArgsLeft                              );

    // store arg strings
    for( integer i= 0; i < qtyArgsExpected; ++i )
        Args.PushBack( CmdLine->GetArg(argNo + 1 + i) );
    QtyArgsConsumed+= qtyArgsExpected;

    return true;
}

bool Command::Read( CommandDecl& decl )
{
    auto&  identifier=  decl.Identifier();
    String arg       =  CmdLine->PeekArg();

    if (     arg.Length() <  decl.MinimumRecognitionLength()
         ||  !identifier.StartsWith<true,Case::Ignore>( arg )  )
        return false;

    Declaration= &decl;
    Position=    CmdLine->ArgsLeft[0];
    CmdLine->PopArg();
    QtyArgsConsumed= 1;

    if( decl.Parameters.Size() == 0 )
        return true;

    auto paramDeclIt= decl.Parameters.begin();
    while( paramDeclIt != decl.Parameters.end() )
    {
        Parameter param( CmdLine );
        bool continueReading= param.Read( **paramDeclIt );
        if( param.QtyArgsConsumed )
        {
            QtyArgsConsumed+= param.QtyArgsConsumed;
            Parameter* paramFound= CmdLine->allocator.Emplace<Parameter>( param );
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
            if( paramDecl->Name().Equals(name) )
            {
                found = true;
                break;
            }
        ALIB_ASSERT_ERROR( found, "CLI", "Requested parameter {!Q} not defined.", name )
    #endif

    for( auto* param : ParametersMandatory )
        if( param->Declaration->Name().Equals( name ) )
            return param;

    for( auto* param : ParametersOptional )
        if( param->Declaration->Name().Equals( name ) )
            return param;

    return nullptr;
}

String Command::GetParsedParameterArg( const String& name )
{
    Parameter* param= GetParsedParameter( name );
    return param && param->Args.IsNotEmpty() ? param->Args.Front()
                                             : NullString();
}


bool Parameter::Read( ParameterDecl& decl )
{
    Substring arg=     CmdLine->PeekArg();
    if( arg.IsNull() )
        return false;

    auto& identifier=  decl.Identifier();
    if ( identifier.IsEmpty() && decl.IsOptional() )
        return false;

    auto      valueSeparator= arg.IndexOf<false, Case::Sensitive>( decl.ValueSeparator() );
    Substring inArgArgument;
    if( valueSeparator > 0 )
        arg.Split<false>( valueSeparator, inArgArgument, decl.ValueSeparator().Length() );

    if (         identifier.IsEmpty()
         || (    arg.Length() >=  decl.MinimumRecognitionLength()
              && identifier.StartsWith<true,Case::Ignore>( arg ) )  )
    {
        QtyArgsConsumed= 1;
        Declaration=     &decl;
        Position=        CmdLine->ArgsLeft[0];
        CmdLine->PopArg();
        int qtyArgsExpected= decl.QtyExpectedArgsFollowing();

        if( decl.Identifier().IsEmpty() )
        {
            Args.PushBack( arg );
        }
        else
        {
            if( inArgArgument.IsNotEmpty() )
            {
                Args.PushBack( inArgArgument );
                --qtyArgsExpected;
            }
        }

        // stop command?
        if( qtyArgsExpected < 0 )
            return false;

        // error: not enough params
        if ( qtyArgsExpected > static_cast<integer>(CmdLine->ArgsLeft.size()) )
            throw Exception( ALIB_CALLER_NULLED, cli::Exceptions::MissingParameterValue,
                             decl.Name(), Position, CmdLine->GetArg(Position),
                             qtyArgsExpected, CmdLine->ArgsLeft.size() );

        // store arg strings
        for( size_t i= 0; i < static_cast<size_t>( qtyArgsExpected ); ++i )
        {
            Args.PushBack( CmdLine->GetArg(*CmdLine->ArgsLeft.begin()) );
            CmdLine->ArgsLeft.erase( CmdLine->ArgsLeft.begin() );
        }
        QtyArgsConsumed+= static_cast<size_t>( qtyArgsExpected );
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
    enums::EnumRecordParser::Get( qtyExpectedArgsFollowing                 );
    enums::EnumRecordParser::Get( shortcutReplacementString                , true );
}

void ERParameterDecl::Parse()
{
    enums::EnumRecordParser::Get( ERSerializable::EnumElementName          );
    enums::EnumRecordParser::Get( ERSerializable::MinimumRecognitionLength );
    enums::EnumRecordParser::Get( identifier                               );
    enums::EnumRecordParser::Get( valueSeparator                           );
    enums::EnumRecordParser::Get( valueListSeparator                       );
    enums::EnumRecordParser::Get( qtyExpectedArgsFollowing                 );
    enums::EnumRecordParser::Get( isOptional                               , true );
}

void ERExitCodeDecl ::Parse()
{
    enums::EnumRecordParser::Get( ERSerializable::EnumElementName          );
                                  ERSerializable::MinimumRecognitionLength = 0;
    enums::EnumRecordParser::Get( associatedCLIException                   , true );
}

}}}// namespace aworx::lib::cli
