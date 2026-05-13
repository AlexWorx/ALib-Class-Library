namespace alib {  namespace expressions { namespace detail {

// static creation method
Parser* Parser::Create( Compiler& compiler )
{ return compiler.allocator().New<detail::ParserImpl>(compiler, compiler.allocator ); }


}}} // namespace [alib::expressions::detail]
