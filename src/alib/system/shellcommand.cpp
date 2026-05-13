namespace alib {  namespace system {


#if !DOXYGEN
// template instantiations
template int alib::system::TShellCommand<lang::HeapAllocator>::Run(
    const NCString&,
    strings::TAString<nchar, lang::HeapAllocator>&,
    strings::util::TStringVector<nchar, lang::HeapAllocator>*     );

#if ALIB_MONOMEM
template int alib::system::TShellCommand<MonoAllocator>::Run(
    const NCString&,
    strings::TAString<nchar, MonoAllocator>&,
    strings::util::TStringVector<nchar, MonoAllocator>*           );
#endif
#endif // !DOXYGEN

}} // namespace [alib::system]
