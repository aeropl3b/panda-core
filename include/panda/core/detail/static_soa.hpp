#include <xlib/core/mpl/conditional.h>

#include <type_traits>
#include <algorithm>

namespace xlib
{
namespace detail
{
// SOA Element Value Type
template < class T, class = std::void_t<> >
struct soa_element_value_type;

template < class T >
struct soa_element_value_type<T, std::void_t<typename T::value_type>>
{
   using vaule_type = typename T::value_type;
};

template < class T >
struct soa_element_value_type<T*,void>
{
   using value_type = T;
};

// SOA Element Referece
template < class T, class = std::void_t<> >
struct soa_element_reference;

template < class T >
struct soa_element_reference<T, std::void_t<typename T::reference>>
{
   using vaule_type = typename T::reference;
};

template < class T >
struct soa_element_reference<T*,void>
{
   using value_type = T;
};

// SOA size_of handler
template < class T, class  = void >
struct soa_size_of;

template < class T >
struct soa_size_of<T*,void>
{
   size_t operator()(const T* data) noexcept
   {
      if(!data) return 0;

      size_t* x = (size_t*)((void*)data);
      return x[-1];
   }
};

template < class T >
struct soa_size_of<T,
   std::void_t<decltype(std::declval<T>().size())>
>
{
   size_t operator()(const T& data) noexcept
   {
      return data.size();
   }
};

// SOA resize handler
template < class T, class = void >
struct soa_resize;

template < class T >
struct soa_resize<T*,void>
{
   void operator()(T*& data, size_t n)
   {
      bool needs_realloc = false;
      auto old_size = soa_size_of<T*>()(data);
      if(data)
      {
         // Check the pointer capacity, if it is greater than the size
         size_t* x = (size_t*)((void*)data);
         if(x[-2] >= n)
         {
            // Shrinking, effective old size is now new size
            old_size = n;
            // Reduce memory footprint
            if(n < (x[-2] / 4))
            {
               needs_realloc = true;
            }
            x[-1] = n;
         }
         // Growing requires realloc
         else
         {
            needs_realloc = true;
         }
      }
      else
      {
         needs_realloc = true;
      }

      if(needs_realloc)
      {
         T* old_data = data;
         // Allocate the memory for the new size data
         auto head = new char[sizeof(T) * n + sizeof(size_t)*2];
         // Set the size in front of the allocation
         reinterpret_cast<size_t*>(head)[0] = n;
         reinterpret_cast<size_t*>(head)[1] = n;
         // Initialize the data pointer and use the offset head
         data = new((T*)(head + sizeof(size_t) * 2)) T[n]();
         // Copy the old data into the new data
         std::copy(old_data, old_data + old_size, data);
         // Clean up the old memory if previously allocated
         if(old_data)
         {
            size_t* old_head = (size_t*)((void*)old_data);
            old_head = &(old_head[-2]);
            delete[] (T*)old_head;
         }
      }
      // TODO
      // std::fill(data + old_size, data + n, default_value);
   }
};

template < class T >
struct soa_resize<T, std::void_t<decltype(std::declval<T>().resize(std::declval<size_t>()))> >
{
   size_t operator()(T& data, size_t n)
   {
      data.resize(n);
      // TODO
      // std::fill(data + old_size, data + n, default_value);
   }
};

template < class T >
struct soa_dtor
{
   void operator()(T& data)
   {
   }
};

template < class T >
struct soa_dtor<T*>
{
   void operator()(T*& data)
   {
      if(!data) return;
      size_t* x = reinterpret_cast<size_t*>(data);
      x = x - 2;
      delete[] reinterpret_cast<char*>(x);
      data = nullptr;
   }
};

template < class T >
struct soa_reorder
{
   template < class Int, class = std::enable_if_t<std::is_integral<Int>::value> >
   void operator()(T& data, const std::vector<Int>& new_index_map)
   {
      Int N = soa_size_of<T>()(data);
      T copy_buffer = T();
      soa_resize<T>()(copy_buffer, N);
      for(Int i = 0; i < N; i++)
      {
         copy_buffer[i] = data[i];
      }
      std::swap(data, copy_buffer);
      soa_dtor<T>()(copy_buffer);
   }
};

template < template<class> class CallBack, class Tuple, class Args, size_t... Indices >
void for_each_impl(Tuple&& data, Args&& args, std::index_sequence<Indices...>)
{
   using eval = int[];
   (void)eval{1,
      (std::apply(CallBack<std::tuple_element_t<Indices,std::remove_reference_t<Tuple>>>(), std::tuple_cat(std::forward_as_tuple(std::get<Indices>(std::forward<Tuple>(data))), args)), void(), int{})...};
}

template < template<class> class CallBack, class Tuple, class... Args >
void for_each(Tuple&& data, Args&&... args)
{
   for_each_impl<CallBack>(std::forward<Tuple>(data), std::forward_as_tuple(args...), std::make_index_sequence<std::tuple_size<std::remove_reference_t<Tuple>>::value>());
}

template < class CallBack, class SOATuple, class ArgsTuple, size_t... Indices >
decltype(auto)
apply_to_element_impl(SOATuple&& soa_args, size_t i, CallBack&& f, ArgsTuple&& args, std::index_sequence<Indices...>)
{
   return std::apply(std::forward<CallBack>(f), std::tuple_cat(std::forward_as_tuple((std::get<Indices>(std::forward<SOATuple>(soa_args))[i])...), std::forward<ArgsTuple>(args))); ;
}

template < class SOATuple, size_t... Indices >
decltype(auto)
get_element_impl(const SOATuple& soa_args, size_t i, std::index_sequence<Indices...>)
{
   return std::make_tuple(std::get<Indices>(soa_args)[i]...);
}

template < class T, class... Types >
struct type_in_list;

template < class T, class T1, class... Types >
struct type_in_list<T, T1, Types...>: 
   xlib::mpl::if_t<std::is_convertible<T*,T1*>,
      std::true_type, type_in_list<T, Types...>
   >
{};

/** Empty list case
 */
template < class T >
struct type_in_list<T>: std::false_type
{};

template < class T, class... Types >
inline constexpr bool type_in_list_v = type_in_list<T,Types...>::value;

template < class Tuple, size_t... Indices >
decltype(auto) forward_tuple_indices(Tuple&& data, std::index_sequence<Indices...>)
{
   return std::forward_as_tuple(std::get<Indices>(data)...);
}

} // namespace detail

template < class... Types >
template < size_t I >
static_soa<Types...>::meta_handle_t<I> static_soa<Types...>::get_handle()
{
   return meta_handle_t<I>(this);
}

template < class... Types >
template < size_t I >
static_soa<Types...>::reference<I> static_soa<Types...>::get_data() noexcept
{
   return std::get<I>(_data);
}

template < class... Types >
template < class T, size_t I >
static_soa<Types...>::reference<I> static_soa<Types...>::get_data(const meta_handle<T,I>& mh) noexcept
{
   assert(mh.parent() == this);
   static_assert(std::is_same_v<meta_handle<T,I>::value_type, static_soa<Types...>::value_type<I>>, "Mismatch between meta handle and static_soa types");

   return mh.data();
}

template < class... Types >
template < class T >
T& static_soa<Types...>::get_data(const handle& h)
{
   static_assert(detail::type_in_list_v<T, Types...>, "Attempting to extract ");
   return h.template data<T>();
}

template < class... Types >
template < size_t I >
static_soa<Types...>::const_reference<I> static_soa<Types...>::get_data() const noexcept
{
   return std::get<I>(_data);
}

template < class... Types >
template < template<class> class CallBack, class Indices, class... Args >
void static_soa<Types...>::apply_to_indices(Indices&& indices, Args&&... args)
{
   detail::for_each<CallBack>(detail::forward_tuple_indices(_data, std::forward<Indices>(indices)), std::forward<Args>(args)...);
}

template < class... Types >
template < template<class> class CallBack, class... Args >
void static_soa<Types...>::apply(Args&&... args)
{
   this->apply_to_indices<CallBack>(std::index_sequence_for<Types...>(), std::forward<Args>(args)...);
}

template < class... Types >
template < class CallBack, class... Args >
decltype(auto)
static_soa<Types...>::apply_to_element(size_t i, CallBack&& f, Args&&... args)
{
   return detail::apply_to_element_impl(_data, i, std::forward<CallBack>(f), std::forward_as_tuple<Args...>(args...), std::make_index_sequence<sizeof...(Types)>());
}

template < class... Types >
template < class CallBack, class... Args >
void static_soa<Types...>::apply_per_element(CallBack&& f, Args&&... args)
{
   size_t n = this->size();
   for(size_t i = 0; i < n; ++i)
   {
      apply_to_element(i, std::forward<CallBack>(f), i, std::forward<Args>(args)...);
   }
}

template < class... Types >
decltype(auto) static_soa<Types...>::get_element(size_t i)
{
   return detail::get_element_impl(_data, i, std::make_index_sequence<sizeof...(Types)>());
}

template < class... Types >
void static_soa<Types...>::resize(size_t n)
{
   this->apply<detail::soa_resize>(n);
}

template < class... Types >
size_t static_soa<Types...>::size() const noexcept
{
   return detail::soa_size_of<static_soa<Types...>::value_type<0>>()(std::get<0>(_data));
}

template < class... Types >
template < class T, class >
void static_soa<Types...>::reorder(const std::vector<T>& new_index_map)
{
   assert(std::numeric_limits<T>::max() > this->size());

   this->apply<detail::soa_reorder>(new_index_map);
}

} // namespace xlib

