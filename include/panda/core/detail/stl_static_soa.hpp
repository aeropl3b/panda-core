
// STL integrations
// std::get<I>(panda::core::static_soa)
// std::tuple_element<I>(panda::core::static_soa)
// std::tuple_size(panda::core::static_soa)
namespace std
{
namespace detail
{
template < std::size_t I, class T >
struct panda_indexed
{
   using type = T;
};

template < class IndexSeq, class... Ts >
struct panda_indexer;

template < std::size_t... Is, class... Ts >
struct panda_indexer<std::index_sequence<Is...>, Ts...> : panda_indexed<Is,Ts>...
{};

template < std::size_t I, class T >
panda_indexed<I,T> panda_select(panda_indexed<I,T>){}
}

template < std::size_t I, class... Ts >
class tuple_element< I, ::panda::core::static_soa<Ts...> >
{
public:
   using type = typename decltype(detail::panda_select<I>(detail::panda_indexer<std::index_sequence_for<Ts...>, Ts...>{}))::type;
};

template < class... Ts >
class tuple_size< ::panda::core::static_soa<Ts...> >:
   std::integral_constant<std::size_t,sizeof...(Ts)> {};

template < std::size_t I, class... Ts >
typename tuple_element<I,::panda::core::static_soa<Ts...>>::type&
get(::panda::core::static_soa<Ts...>& soa)
{
   return soa.template get<I>();
}

template < std::size_t I, class... Ts >
typename tuple_element<I,::panda::core::static_soa<Ts...>>::type const&
get(const ::panda::core::static_soa<Ts...>& soa)
{
   return soa.template get<I>();
}

template < std::size_t I, class... Ts >
typename tuple_element<I,::panda::core::static_soa<Ts...>>::type const&&
get(const ::panda::core::static_soa<Ts...>&& soa)
{
   return soa.template get<I>();
}

template < std::size_t I, class... Ts >
typename tuple_element<I,::panda::core::static_soa<Ts...>>::type &&
get(::panda::core::static_soa<Ts...>&& soa)
{
   return soa.template get<I>();
}

} // namespace std
