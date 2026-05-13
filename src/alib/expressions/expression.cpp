namespace alib {  namespace expressions {


ExpressionVal::ExpressionVal( MonoAllocator&  ma,
                              const String&   sourceString,
                              Scope*          pCTScope           )
: allocator     ( ma )
, ctScope       ( pCTScope )
, name          ( nullptr )
, program       ( nullptr )
, originalString( ma, sourceString )          { normalizedString.SetBuffer(sourceString.Length()); }

ExpressionVal::~ExpressionVal() {
    allocator.DbgLock(false);
    if(program)
       delete static_cast<detail::Program*>(program);
    lang::Destruct(*ctScope);
}

String   ExpressionVal::Name() {
    if( name.IsNull() )
        name= EXPRESSIONS.GetResource("ANON_EXPR_NAME");
    return name;
}

alib::Box  ExpressionVal::ResultType()
{
    ALIB_ASSERT_ERROR( program, "EXPR", "Internal error: Expression without program" )
        return static_cast<detail::Program*>(program)->ResultType();
}

alib::Box  ExpressionVal::Evaluate( Scope& scope ) {
    ALIB_ASSERT_ERROR( program, "EXPR","Internal error: Expression without program" )
    ALIB_DBG( Ticks startTime; )

        Box result= static_cast<detail::Program*>(program)->Run( scope );


    ALIB_DBG( DbgLastEvaluationTime= startTime.Age(); )

    return result;
}


String     ExpressionVal::GetOptimizedString() {
    if( optimizedString.IsNull() )
        static_cast<detail::Program*>(program)->compiler.getOptimizedExpressionString(*this);
    return optimizedString;
}

integer   ExpressionVal::GetProgramLength()
{ return static_cast<detail::Program*>(program)->Length(); }

bool      ExpressionVal::IsConstant() {
    detail::Program* prg= static_cast<detail::Program*>(program);
    return prg->Length() == 1 && prg->At(0).IsConstant();
}

int       ExpressionVal::CtdOptimizations()
{ return static_cast<detail::Program*>(program)->CtdOptimizations(); }

#if ALIB_DEBUG
AString  DbgList(Expression expression)
{ return detail::VirtualMachine::DbgList(*static_cast<detail::Program*>(expression->GetProgram()));}
#endif

}} // namespace [alib::expressions]


#undef EOS
