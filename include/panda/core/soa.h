#pragma once

#include "xlib/core/class_traits.h"

#include <type_traits>
#include <set>
#include <mutex>

namespace xlib
{

class soa_bucket: non_copyable
{
   class entry
   {
      size_t size() const;
      void resize(size_t n);

      soa_bucket* parent() const;
   };
   template < class T >
   class meta_entry;

public:
   ~soa_bucket();
   /** Basic handle
    */
   class handle
   {
   public:
      soa_bucket::entry* entry();

      template < class T >
      T& data();

   private:
      handle(soa_bucket::entry* e): _e(e) {}

      soa_bucket::entry* _e;
      friend soa_bucket;
   };

   /** Handle that carries type meta
    */
   template < class T >
   class meta_handle: public handle
   {
   public:
      T& data()
      {
         return static_cast<handle*>(this)->data<T>();
      }
   }

   /** Register data to the soa bucket, use default constructor for T
    * @return Handle to registered data
    */
   template < class T >
   soa_bucket::meta_handle<T> register_data();

   /** Register data to the soa bucket based on a copy of a T container
    * @param copied data storage to copied into the soa bucket
    * @return Handle to registered data
    */
   template < class T, class = std::enable_if_t<std::is_copy_constructible<T>::value> >
   soa_bucket::meta_handle<T> register_data(const T& copied);

   /** Register data to the soa bucket based on a copy of a T container
    * @param moved data storaged moved into the soa bucket
    * @return Handle to registered data
    */
   template < class T, class = std::enable_if_t<std::is_move_constructible<T>::value> >
   soa_bucket::meta_handle<T> register_data(T&& move);

   /** Get data referenced by the handle
    * @param h handle with meta template info
    * @return data container
    */
   template < class T >
   T& get_data(const soa_bucket::meta_handle<T>& h);

   /** Get data referenced by the handle
    * @tparam T Container type expected from the 
    * @param h handle
    * @return data container
    */
   template < class T >
   T& get_data(const soa_bucket::handle& h);

   /** Get data referenced by the handle
    * @tparam T Container type expected from the 
    * @param h handle with meta template info
    * @return entry data stored in the 
    */
   template < class T >
   soa_bucket::entry* get_entry(const soa_bucket::handle& h);

   /** Link two dataset together, data_handle depends on the data of depency_handle
    * @param h handle to some data in soa bucket
    * @param dependency_handle handle that handle h is dependent on
    */
   void add_data_dependency(const handle& h, const handle& dependency_handle);

private:
   struct entry_info;

   std::mutex _map_lock;
   std::set<entry_info*> _data;
};

} // namespace xlib

#include "detail/soa.hpp"