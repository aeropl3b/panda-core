#pragma once

namespace xlib
{
namespace mpl
{

// mpl IF
template < class Condition, class T, class F >
struct if_;

template < class T, class F >
struct if_<std::true_type, T, F>
{
   using type = T;
};

template < class T, class F >
struct if_<std::false_type, T, F>
{
   using type = F;
};

template < class Condition, class T, class F >
using if_t = typename if_<Condition, T, F>::type;

// mpl AND
namespace detail
{
template < size_t N, class... T >
struct and_helper;

template < size_t N, class T1, class... Types >
struct and_helper<N,T1,Types...>: if_t<T1, and_helper<N-1,Types...>, std::false_type>
{};

template <size_t N>
struct and_helper<N>: std::true_type
{};

template <>
struct and_helper<0>: std::false_type
{};
}

template < class... Types >
struct and_: detail::and_helper<sizeof...(Types),Types...>
{};

template < class... Types >
using and_t = typename and_<Types...>::type;

// mpl OR
namespace detail
{
template < class... T >
struct or_helper;

template < class T1, class... Types>
struct or_helper<T1, Types...>: if_t<T1, std::true_type, or_helper<Types...>>
{};

template <>
struct or_helper<>: std::false_type
{};
}

template < class... Types >
struct or_: detail::or_helper<Types...>
{};

template < class... Types >
using or_t = typename or_<Types...>::type;

} // namespace mpl
} // namespace xlib
