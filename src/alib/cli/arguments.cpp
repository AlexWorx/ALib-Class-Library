// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined (HPP_ALIB_CLI_CLI)
#   include "alib/cli/arguments.hpp"
#endif

#if !defined (HPP_ALIB_CLI_CLIAPP)
#   include "alib/cli/cliapp.hpp"
#endif

#if !defined (HPP_ALIB_RESOURCE_TUPLE_LOADER)
#   include "alib/resources/resourcedtupleloader.hpp"
#endif


// ##########################################################################################
// ### Tuple loaders
// ##########################################################################################
ALIB_ENUM_SPECIFICATION_IMPL( aworx::lib::cli::ParameterDecl )
ALIB_ENUM_SPECIFICATION_IMPL( aworx::lib::cli::CommandDecl   )
ALIB_ENUM_SPECIFICATION_IMPL( aworx::lib::cli::OptionDecl    )
ALIB_ENUM_SPECIFICATION_IMPL( aworx::lib::cli::ExitCodeDecl  )

namespace aworx { namespace lib { namespace cli {

// ##########################################################################################
// ### Methods of Command, Option, etc.
// ##########################################################################################

void CommandDecl::addParamDecls()
{
    Tokenizer tknzr( std::get<3>(Tuple) , '/' );
    while( tknzr.Next().IsNotEmpty() )
    {
ALIB_DBG(int paramNo;
         auto oldSize= Parameters.size();    )
        for( auto& param : Parent.ParameterDecls )
            if( param.Name().StartsWith<true, Case::Ignore>( tknzr.Actual ) )
            {
                Parameters.emplace_back( &param );
                break;
            }

        ALIB_ASSERT_ERROR( oldSize + 1 == Parameters.size(),
                           "Parameter named {!Q} not found while loading resources of command {!Q}.",
                           paramNo, Identifier()  )
    }
}

ParameterDecl* CommandDecl::GetParameterDecl(const String& name )
{
    for( auto* param : Parameters )
        if( param->Name().Equals( name ) )
            return param;

    return nullptr;
}


bool Option::Read( OptionDecl& decl, String& argProbablyReplaced, const size_t argNo )
{
    String identifier =     decl.Identifier();
    character  identifierC=     decl.IdentifierChar();

    size_t qtyArgsExpected= decl.QtyExpectedArgsFollowing();
    size_t qtyArgsLeft=     Parent->ArgsLeft.size();

    // read single/double hyphen options once
    Substring arg= argProbablyReplaced;
    Substring inArgArgument;
    auto      inArgSeparatorPos= arg.IndexOf<false, Case::Sensitive>( decl.InArgSeparator() );
    if( inArgSeparatorPos > 0 )
        arg.Split<false>( inArgSeparatorPos, inArgArgument, decl.InArgSeparator().Length() );

    bool potentialIllegalContinuation= false;
    if ( !(   (     identifier.IsNotEmpty()
               &&  arg.ConsumeString(A_CHAR("--"))
               &&  arg.Length() >=  decl.MinimumParseLen()
               && (    identifier.StartsWith<true, Case::Ignore>( arg )
                    || true == (potentialIllegalContinuation= arg.StartsWith<true,Case::Ignore>( identifier )) )

             )

          || (     arg.ConsumeChar('-')
               &&  arg.ConsumeChar(identifierC)                             )
             )
        )
        return 0;

    // Throw if the identifier was fully matched, but the argument continues with a
    // non-alphanumerical value.
    // (This is only allowed if separator was set in resource. But in this case, the flag will
    // never match, because of the code above...)
    if( potentialIllegalContinuation )
    {
        auto nextChar= arg.CharAt<false>( identifier.Length() );
        if( !isalnum( nextChar ) )
            throw Exception( ALIB_CALLER_NULLED, cli::Exceptions::IllegalOptionNameContinuation,
                             identifier, argNo, Parent->ArgStrings[static_cast<size_t>( argNo )]  );
        return 0;
    }

    Declaration= &decl;
    Position=    argNo;

    QtyArgsConsumed= 1;


    // store in-arg argument
    if( inArgSeparatorPos > 0)
    {
        Args.emplace_back( inArgArgument );
        if(qtyArgsExpected > 0 )
            qtyArgsExpected--;
    }


    // error: not enough params
    if (  qtyArgsExpected > qtyArgsLeft - 1 )
        throw Exception( ALIB_CALLER_NULLED, cli::Exceptions::MissingOptionValue,
                         decl.Identifier(), argNo, Parent->ArgStrings[static_cast<size_t>( argNo )],
                         qtyArgsExpected, qtyArgsLeft                              );

    // store arg strings
    for( size_t i= 0; i < qtyArgsExpected; i++ )
        Args.emplace_back( Parent->ArgStrings[ argNo + 1 + i ]);
    QtyArgsConsumed+= qtyArgsExpected;

    return true;
}

bool Command::Read( CommandDecl& decl )
{
    String identifier =     decl.Identifier();
    String arg        =     Parent->PeekArg();

    if (     arg.Length() >=  decl.MinimumParseLen()
         &&  identifier.StartsWith<true,Case::Ignore>( arg ) )
    {
        Declaration= &decl;
        Position=    Parent->ArgsLeft[0];
        Parent->PopArg();
        QtyArgsConsumed= 1;

        for( auto& paramDecl : decl.Parameters )
        {
            Parameter param( Parent );
            bool continueReading= param.Read( *paramDecl );
            if( param.QtyArgsConsumed )
            {
                QtyArgsConsumed+= param.QtyArgsConsumed;
                if ( paramDecl->IsOptional() )
                    ParametersOptional.emplace_back( param );
                else
                    ParametersMandatory.emplace_back( param );
            }

            // stop here if parameter read signaled this
            if( !continueReading )
                return false;
        }

        return true;
    }

    return false;
}

Parameter* Command::GetParsedParameter(const String& name )
{
    for( auto& param : ParametersMandatory )
        if( param.Declaration->Name().Equals( name ) )
            return &param;

    for( auto& param : ParametersOptional )
        if( param.Declaration->Name().Equals( name ) )
            return &param;

    return nullptr;
}

String Command::GetParsedParameterArg( const String& name )
{
    Parameter* param= GetParsedParameter( name );
    return param && param->Args.size() ? param->Args[0]
                                       : NullString();
}


bool Parameter::Read( ParameterDecl& decl )
{
    Substring arg        =     Parent->PeekArg();
    if( arg.IsNull() )
        return false;

    String    identifier =     decl.Identifier();
    if ( identifier.IsEmpty() && decl.IsOptional() )
        return false;

    int       qtyArgsExpected= decl.QtyExpectedArgsFollowing();
    auto      inArgSeparatorPos= arg.IndexOf<false, Case::Sensitive>( decl.InArgSeparator() );
    Substring inArgArgument;
    if( inArgSeparatorPos > 0 )
        arg.Split<false>( inArgSeparatorPos, inArgArgument, decl.InArgSeparator().Length() );

    if (         identifier.IsEmpty()
         || (    arg.Length() >=  decl.MinimumParseLen()
              && identifier.StartsWith<true,Case::Ignore>( arg ) )  )
    {
        QtyArgsConsumed= 1;
        Declaration=     &decl;
        Position=        Parent->ArgsLeft[0];
        Parent->PopArg();

        if( decl.Identifier().IsEmpty() )
        {
            Args.emplace_back( arg );
        }
        else
        {
            if( inArgArgument.IsNotEmpty() )
            {
                Args.emplace_back( inArgArgument );
                qtyArgsExpected--;
            }
        }

        // stop command?
        if( qtyArgsExpected < 0 )
            return false;

        // error: not enough params
        if ( static_cast<size_t>( qtyArgsExpected ) > Parent->ArgsLeft.size() )
            throw Exception( ALIB_CALLER_NULLED, cli::Exceptions::MissingParameterValue,
                             decl.Name(), Position, Parent->ArgStrings[static_cast<size_t>( Position )],
                             qtyArgsExpected, Parent->ArgsLeft.size() );

        // store arg strings
        for( size_t i= 0; i < static_cast<size_t>( qtyArgsExpected ); i++ )
        {
            Args.emplace_back( Parent->ArgStrings[ *Parent->ArgsLeft.begin() ] );
            Parent->ArgsLeft.erase( Parent->ArgsLeft.begin() );
        }
        QtyArgsConsumed+= static_cast<size_t>( qtyArgsExpected );
    }


    return true;
}


}}}// namespace aworx::lib::cli

