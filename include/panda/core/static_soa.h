#include <tuple>
#include <utility>
#include <functional>
#include <vector>

namespace xlib
{
namespace detail
{
} // namespace detail

template < class... Types >
class static_soa
{
   using Tuple = std::tuple<std::remove_cv_t<std::remove_reference_t<Types>>...>;
public:
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
      meta_handle(static_soa<Types...>* parent): _parent(parent) {}

      reference data() const noexcept { return _parent->get_data<I>(); }

      static_soa<Types...>* parent() noexcept { return _parent; }
   private:
      static_soa<Types...>* _parent;
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
   reference<I> get_data() noexcept;

   /** Get the data stored at index I in the static_soa container
    * @return data stored at index I in the static_soa container
    */
   template < class T, size_t I >
   reference<I> get_data(const meta_handle<T,I>& mh) noexcept;

   /** Get the data stored at index I in the static_soa container
    * @return data stored at index I in the static_soa container
    */
   template < class T >
   T& get_data(const handle& mh);

   /** Get the data stored at index I in the static_soa container
    * @return data stored at index I in the static_soa container
    */
   template < size_t I >
   const_reference<I> get_data() const noexcept;

   /** Apply a functor of type CallBack to the data in the static soa container
    * @tparam CallBack functor type
    * @tparam CallBack Extra arguments to use when applying CallBack to data
    * @param args List of arguments to forward to the invokation of CallBack
    */
   template < template<class> class CallBack, class... Args >
   void apply(Args&&... args);

   /** Apply a functor of type CallBack to the data in the static soa container
    * @tparam CallBack functor type
    * @tparam Indices index_sequence of elements to pass to the callback
    * @tparam Args... Extra arguments to use when applying CallBack to data
    * @param args List of arguments to forward to the invokation of CallBack
    */
   template < template<class> class CallBack, class Indices, class... Args >
   void apply_to_indices(Indices&& indices, Args&&... args);

   /** Apply a function that takes the Types::reference..., Args... as inputs
    * @tparam CallBack Type of the callback function
    * @tparam Args List of extra argument types
    * @param f Callback function
    * @param args list of extra arguments to pass to f
    * @return Result of f
    */
   template < class CallBack, class... Args >
   decltype(auto)
   apply_to_element(size_t i, CallBack&& f, Args&&... args);

   /** Apply a function that takes the Types::reference..., Args... as inputs
    * @tparam CallBack Type of the callback function
    * @tparam Args List of extra argument types
    * @param f Callback function
    * @param args list of extra arguments to pass to f
    * @return std::vector or the results of f called per element
    */
   template < class CallBack, class... Args >
   void apply_per_element(CallBack&& f, Args&&... args);

   /** Get the element i from all of the arrays in the static soa container
    * @param i index in the arrays to get elements from
    * @return std::tuple with the reference values of all of the array elements
    */
   decltype(auto) get_element(size_t i);

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
   void reorder(const std::vector<T>& new_index_map);

private:
   Tuple _data;
};
} // namespace xlib

#include "detail/static_soa.hpp"

/*
struct ParcelDataStorage
{
   xlib::static_soa<std::vector<xlib::vec<double,3>>>
};

#define CREATE_PARCEL_STORAGE(TYPE, PDATA, MDATA)\
struct TYPE##Parcel\
{\
   using type = std::tuble<UNROLL_TYPES(__VA_ARGS__)>\
   enum TYPE##Ids\
   {\
      cell_index_id,
      position_id,
      velocity_id,
      UNROLL_NAMES_ENUM(__VA_ARGS__),\
      NUM_##TYPE##_VARS\
   };\
   std::vector<int64_t>& cell_index() { return _data->get_data<cell_index_id>(); }
   std::vector<int64_t>& cell_index() { return _data->get_data<position_id>(); }
   std::vector<int64_t>& cell_index() { return _data->get_data<velocity_id>(); }
   UNROLL_NAME_GETTERS(__VA_ARGS__)\
private:\
   xlib::static_soa<
      std::vector<int64_t>,
      std::vector<xlib::vec<double,3>
      std::vector<xlib::vec<double,3>
      UNROLL_TYPES(__VA_ARGS__)>* _data;\
};

// Create a parcel data type
CREATE_PARCEL_STORAGE(Liquid,
   PARCEL_DATA(
      (std::vector<xlib::vec<double,3>>, position),
      (std::vector<xlib::vec<double,3>>, velocity)
      (std::vector<double>, radius),
      (xlib::nd_array<double,2>, species_mf)
   ),
   MEMBER_DATA(
      (double, asdf, 1.0 /* Default value//)
   )
);


int main()
{
   Mesh* mesh = new Mesh;

   // Register parcel data type
   mesh->addParcelData<LiquidParcelData>("liquid_spray_1");
   auto& pdata = mesh->getParcelData<LiquidParcelData>("liquid_spray_1");
   pdata.position(i); // zero overhead, compile time getter
   pdata.position(i)[j];
   pdata.species_mf(i)[j];
   pdata.member<int>("num_species") = num_parcel_species;
   pdata.member<struct species_defintion**>("species") = new struct species_defintion*[num_parcel_species];

}
*/

