/** Static SOA structure
 * @file
 */
#pragma once

#include <tuple>
#include <utility>
#include <functional>
#include <vector>

namespace panda
{
namespace core
{
namespace detail
{
} // namespace detail

template < class... Ts >
class static_soa
{
   using Tuple = std::tuple<std::remove_cv_t<std::remove_reference_t<Ts>>...>;
public:

   template < class T >
   struct default_initializer
   {
      void operator()(typename T::iterator, typename T::iterator)
      {}
   };

   struct handle;
   template < class T, size_t I >
   struct meta_handle;

   template < class T, size_t I >
   struct meta_handle
   {
      using value_type = T;
      using reference = T&;
      using const_reference = const T&;

      meta_handle() = default;
      meta_handle(static_soa<Ts...>* parent): _parent(parent) {}

      reference data() const noexcept { return _parent->template get<I>(); }

      static_soa<Ts...>* parent() noexcept { return _parent; }
   private:
      static_soa<Ts...>* _parent;
   };

   struct handle
   {
      template < class T, size_t I >
      handle(const meta_handle<T,I>& mh)
      {
         this->_get_data_f = [mh](void) -> void*
         {
            return &mh.data();
         };
      }

      template < class T >
      T& data()
      {
         return *reinterpret_cast<T*>(_get_data_f());
      }
      template < class T >
      T& data() const
      {
         return *reinterpret_cast<T*>(_get_data_f());
      }

   private:
      std::function<void*(void)> _get_data_f;
   };


   template < size_t I >
   using value_type = std::tuple_element_t<I,Tuple>;
   template < size_t I >
   using reference = std::tuple_element_t<I,Tuple>&;
   template < size_t I >
   using const_reference = const std::tuple_element_t<I,Tuple>&;
   template < size_t I >
   using meta_handle_t = meta_handle<value_type<I>, I>;

   /** Get a handle to data stored at index I in the static_soa container
    * @return handle to data stored at index I in the static_soa container
    */
   template < size_t I >
   meta_handle_t<I> get_handle();

   /** Get the data stored at index I in the static_soa container
    * @return data stored at index I in the static_soa container
    */
   template < size_t I >
   reference<I> get() noexcept;

   /** Get the data stored at index I in the static_soa container
    * @return data stored at index I in the static_soa container
    */
   template < class T, size_t I >
   reference<I> get(const meta_handle<T,I>& mh) noexcept;

   /** Get the data stored at index I in the static_soa container
    * @return data stored at index I in the static_soa container
    */
   template < class T >
   T& get(const handle& mh);

   /** Get the data stored at index I in the static_soa container
    * @return data stored at index I in the static_soa container
    */
   template < size_t I >
   const_reference<I> get() const noexcept;

   /** Call a visitor functor of type Visitor to each type stored in static_soa container
    * @tparam Visitor functor type with operator()(Type, Args...) where Type is a type in the static_soa type list Ts...
    * @tparam Args... Extra arguments to use when applying Function to data
    * @param args List of arguments to forward to the invokation of Function
    */
   template < class Visitor, class... Args >
   void visit(Args&&...args);

   template < std::size_t I, class Function, class... Args >
   void register_initializer(std::size_t priority, Function&& initializer)
   {
      this->_initializer.insert({priority, [&](std::size_t offset)
      {
         initializer(std::get<I>(this->_data).begin() + offset, std::get<I>(this->_data).end());
      }});
   }

   /** Apply a functor of type Function to the data in the static soa container
    * @tparam Function functor type
    * @tparam Indices index_sequence of elements to pass to the callback
    * @tparam Args... Extra arguments to use when applying Function to data
    * @param args List of arguments to forward to the invokation of Function
    */
   template < template<class> class Function, class Indices, class... Args >
   void apply_to_indices(Indices&& indices, Args&&... args);

   /** Apply a function that takes the Ts::reference..., Args... as inputs
    * @tparam Function Type of the callback function
    * @tparam Args List of extra argument types
    * @param f Callback function
    * @param args list of extra arguments to pass to f
    * @return Result of f
    */
   template < class Function, class... Args >
   decltype(auto)
   apply_to_element(size_t i, Function&& f, Args&&... args);

   /** Apply a function that takes the Ts::reference..., Args... as inputs
    * @tparam Function Type of the callback function
    * @tparam Args List of extra argument types
    * @param f Callback function
    * @param args list of extra arguments to pass to f
    * @return std::vector or the results of f called per element
    */
   template < class Function, class... Args >
   void apply_per_element(Function&& f, Args&&... args);

   /** Get the element i from all of the arrays in the static soa container
    * @param i index in the arrays to get elements from
    * @return std::tuple with the reference values of all of the array elements
    */
   std::tuple<typename Ts::value_type...> get_element(size_t i);

#if 0
   /** Set the element i from all of the arrays in the static soa container
    * @param i index in the arrays to get elements from
    * @param values std::tuple containing an element value for each array
    */
   void set_element(size_t i, std::tuple<Type::value_type...>);
#endif

   /** Resize all of the data in the static_soa container
    * @param n new size
    */
   void resize(size_t n);

   /** Resize all of the data in the static_soa container
    * @return size of data structures in static_soa container
    */
   size_t size() const noexcept;

   /** Reorder the elements of all of the arrays
    * @param new_index_map new indices of each element
    */
   template < class T, class = std::enable_if_t<std::is_integral<T>::value> >
   void remap(const std::vector<T>& new_index_map);

private:
   struct resizer
   {
      template < class T >
      void operator()(T& data, std::size_t n)
      {
         data.resize(n);
      }
   };

   struct remapper
   {
      template < class T >
      void operator()(T& data, const std::vector<T>& new_index_map)
      {
      }
   };

   Tuple _data;
   std::map<std::size_t,std::function<void(std::size_t n)>> _initializer;
};
} // namespace core
} // namespace panda

#include "detail/static_soa.hpp"
#include "detail/stl_static_soa.hpp"
#include "detail/static_soa_macros.hpp"

