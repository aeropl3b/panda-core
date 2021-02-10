#include <type_traits>
#include <algorithm>

#include <boost/mpl/if.hpp>

namespace panda
{
namespace core
{
namespace detail
{

#if 0
template < template<class> class CallBack, class Tuple, class Args, size_t... Is >
void for_each_impl(Tuple&& data, Args&& args, std::index_sequence<Is...>)
{
   using eval = int[];
   (void)eval{1,
      (std::apply(CallBack<std::tuple_element_t<Is,std::remove_reference_t<Tuple>>>(), std::tuple_cat(std::forward_as_tuple(std::get<Is>(std::forward<Tuple>(data))), args)), void(), int{})...};
}

template < template<class> class CallBack, class Tuple, class... Args >
void for_each(Tuple&& data, Args&&... args)
{
   for_each_impl<CallBack>(std::forward<Tuple>(data), std::forward_as_tuple(args...), std::make_index_sequence<std::tuple_size<std::remove_reference_t<Tuple>>::value>());
}

template < class CallBack, class SOATuple, class ArgsTuple, size_t... Is >
decltype(auto)
apply_to_element_impl(SOATuple&& soa_args, size_t i, CallBack&& f, ArgsTuple&& args, std::index_sequence<Is...>)
{
   return std::apply(std::forward<CallBack>(f), std::tuple_cat(std::forward_as_tuple((std::get<Is>(std::forward<SOATuple>(soa_args))[i])...), std::forward<ArgsTuple>(args))); ;
}

template < class SOATuple, size_t... Is >
decltype(auto)
get_element_impl(const SOATuple& soa_args, size_t i, std::index_sequence<Is...>)
{
   return std::make_tuple(std::get<Is>(soa_args)[i]...);
}
#endif

template < class T, class... Ts >
struct type_in_list;

template < class T, class T1, class... Ts >
struct type_in_list<T, T1, Ts...>:
   boost::mpl::if_<std::is_convertible<T*,T1*>,
      std::true_type, type_in_list<T, Ts...>
   >::type
{};

/** Empty list case
 */
template < class T >
struct type_in_list<T>: std::false_type
{};

template < class T, class... Ts >
inline constexpr bool type_in_list_v = type_in_list<T,Ts...>::value;

template < class Tuple, size_t... Is >
decltype(auto) forward_tuple_indices(Tuple&& data, std::index_sequence<Is...>)
{
   return std::forward_as_tuple(std::get<Is>(data)...);
}

} // namespace detail

template < class... Ts >
template < size_t I >
typename static_soa<Ts...>::template meta_handle_t<I> static_soa<Ts...>::get_handle()
{
   return meta_handle_t<I>(this);
}

template < class... Ts >
template < size_t I >
typename static_soa<Ts...>::template reference<I> static_soa<Ts...>::get() noexcept
{
   return std::get<I>(_data);
}

template < class... Ts >
template < class T, size_t I >
typename static_soa<Ts...>::template reference<I> static_soa<Ts...>::get(const meta_handle<T,I>& mh) noexcept
{
   assert(mh.parent() == this);
   static_assert(std::is_same_v<meta_handle<T,I>::value_type, static_soa<Ts...>::value_type<I>>, "Mismatch between meta handle and static_soa types");

   return mh.data();
}

template < class... Ts >
template < class T >
T& static_soa<Ts...>::get(const handle& h)
{
   static_assert(detail::type_in_list_v<T, Ts...>, "Attempting to extract ");
   return h.template data<T>();
}

template < class... Ts >
template < size_t I >
typename static_soa<Ts...>::template const_reference<I> static_soa<Ts...>::get() const noexcept
{
   return std::get<I>(_data);
}

#if 0
template < class... Ts >
template < template<class> class CallBack, class Is, class... Args >
void static_soa<Ts...>::apply_to_indices(Is&& indices, Args&&... args)
{
   detail::for_each<CallBack>(detail::forward_tuple_indices(_data, std::forward<Is>(indices)), std::forward<Args>(args)...);
}

template < class... Ts >
template < template<class> class CallBack, class... Args >
void static_soa<Ts...>::apply(Args&&... args)
{
   this->apply_to_indices<CallBack>(std::index_sequence_for<Ts...>(), std::forward<Args>(args)...);
}

template < class... Ts >
template < class CallBack, class... Args >
decltype(auto)
static_soa<Ts...>::apply_to_element(size_t i, CallBack&& f, Args&&... args)
{
   return detail::apply_to_element_impl(_data, i, std::forward<CallBack>(f), std::forward_as_tuple<Args...>(args...), std::make_index_sequence<sizeof...(Ts)>());
}

template < class... Ts >
template < class CallBack, class... Args >
void static_soa<Ts...>::apply_per_element(CallBack&& f, Args&&... args)
{
   size_t n = this->size();
   for(size_t i = 0; i < n; ++i)
   {
      apply_to_element(i, std::forward<CallBack>(f), i, std::forward<Args>(args)...);
   }
}

template < class... Ts >
decltype(auto) static_soa<Ts...>::get_element(size_t i)
{
   return detail::get_element_impl(_data, i, std::make_index_sequence<sizeof...(Ts)>());
}

#endif

template < class Visitor, class Tuple, std::size_t... Is, class... Args >
void visit_impl(Tuple& tuple, std::index_sequence<Is...>, Args&&...args)
{
   using eval = int[];
   (void)eval{int{}, (Visitor{}.operator()(std::get<Is>(tuple), std::forward<Args>(args)...),int{})...};
}

template < class VisitorTuple, class Tuple, std::size_t... Is, class... Args >
void visit_impl(const VisitorTuple&& visitors, Tuple& tuple, std::index_sequence<Is...>, Args&&...args)
{
   using eval = int[];
   (void)eval{int{}, (std::get<Is>(visitors)(std::get<Is>(tuple),std::forward<Args>(args)...),int{})...};
}

template < class... Ts >
template < class Visitor, class... Args >
void static_soa<Ts...>::visit(Args&&...args)
{
   visit_impl<Visitor>(_data, std::index_sequence_for<Ts...>{}, std::forward<Args>(args)...);
}

template < class... Ts >
void static_soa<Ts...>::resize(size_t n)
{
   std::size_t orig_size = this->size();
   this->visit<resizer>(n);
   if(n > orig_size)
   {
      for(auto& init: _initializer)
      {
         init.second(orig_size);
      }
   }
}

template < class... Ts >
size_t static_soa<Ts...>::size() const noexcept
{
   return std::get<0>(_data).size();
}

template < class... Ts >
template < class T, class >
void static_soa<Ts...>::remap(const std::vector<T>& new_index_map)
{
   assert(std::numeric_limits<T>::max() > this->size());

   this->visit<remapper>(new_index_map);
}

} // namespace core
} // namespace panda

