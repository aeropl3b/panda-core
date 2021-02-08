#pragma once

namespace panda
{
namespace mpl
{

namespace impl
{

template < class X >
struct ebo_test : X
{
  int _;
};

}

template < class X >
using is_empty_object = std::is_same<std::integral_const<std::size_t,sizeof(int)>,std::integral_const<std::size_t,sizeof(impl::ebo_test<X>)>>;

}
}