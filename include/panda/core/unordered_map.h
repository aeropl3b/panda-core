#pragma once

#include "panda/core/mpl.h" // panda::mpl::is_empty_object 

#include <cmath>   // std::ceil std::max
#include <cstddef> // std::size_t std::ptrdiff_t

#include <functional>       // std::hash std::equal_to
#include <initializer_list> // std::initalizer_list
#include <utility>          // std::pair
#include <vector>           // std::vector
#include <iterator>         // std::bidirectional_iterator_tag std::reverse_iterator std::make_reverse_iterator

#include <boost/mpl/if.hpp> // boost::mpl::if_

namespace panda {

/** Drop in replacement for std::unordered_map that uses open addressing which
 * is in general faster than chaining forced by STL spec
 */
// TODO (rk) make allocator aware
template <class Key, class T, class Hash = std::hash<Key>,
          class KeyEqual = std::equal_to<Key>
          /*, class Allocator = std::allocator<std::pair<cosnt Key,T>>*/>
class unordered_map {
  static constexpr float __DEFAULT_MAX_LOAD_FACTOR = 0.7f;
  static constexpr std::size_t __INVALID_INDEX = ~static_cast<std::size_t>(0);
  static constexpr std::size_t __ONE = static_cast<std::size_t>(1);

  class node;
  template <class Iterator> class iterator_common_impl;
  class const_iterator_impl;
  class iterator_impl;

  using container =
      std::vector<node /*, typename Allocator::rebind<node>::other */>;

public:
  using key_type = Key;
  using mapped_type = T;
  using value_type = std::pair<const Key, T>;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using hasher = typename boost::mpl::if_<::panda::mpl::is_empty_object<Hash>,
                                          Hash, Hash &>::type;
  using key_equal =
      typename boost::mpl::if_<::panda::mpl::is_empty_object<KeyEqual>,
                               KeyEqual, KeyEqual &>::type;
  using reference = value_type &;
  using const_reference = const value_type &;
  using pointer =
      value_type *; /* typename std::allocator_traits<Allocator>::pointer */
  using const_pointer = value_type const
      *; /* typename std::allocator_traits<Allocator>::cosnt_pointer */
  using iterator = iterator_impl;
  using const_iterator = const_iterator_impl;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;
  using node_type = node;
  // using allocator_type = Allocator;

  unordered_map();
  ~unordered_map();

  // unordered_map(const Allocator &alloc));

  unordered_map(size_type count /*, const Allocator& alloc = Allocator()*/);

  unordered_map(size_type count,
                const Hash &hash /*, const Allocator& alloc = Allocator()*/);

  unordered_map(
      size_type count, const Hash &hash,
      const KeyEqual &equal /*, const Allocator& alloc = Allocator()*/);

  template <class InputIt>
  unordered_map(InputIt first,
                InputIt last /*, const Allocator& alloc = Allocator()*/);

  template <class InputIt>
  unordered_map(InputIt first, InputIt last,
                size_type count /*, const Allocator& alloc = Allocator()*/);

  template <class InputIt>
  unordered_map(InputIt first, InputIt last, size_type count,
                const Hash &hash /*, const Allocator& alloc = Allocator()*/);

  template <class InputIt>
  unordered_map(
      InputIt first, InputIt last, size_type count, const Hash &hash,
      const KeyEqual &equal /*, const Allocator& alloc = Allocator()*/);

  unordered_map(std::initializer_list<
                value_type> /*, const Allocator& alloc = Allocator()*/);
  unordered_map(std::initializer_list<value_type>,
                size_type /*, const Allocator& alloc = Allocator()*/);
  unordered_map(std::initializer_list<value_type>, size_type,
                const Hash & /*, const Allocator& alloc = Allocator()*/);
  unordered_map(std::initializer_list<value_type>, size_type, const Hash &,
                const KeyEqual & /*, const Allocator& alloc = Allocator()*/);

  unordered_map(const unordered_map &);
  unordered_map(unordered_map &&);

  unordered_map &operator=(unordered_map);

  // Iterators
  iterator begin();
  iterator end();
  const_iterator cbegin() const;
  const_iterator cend() const;
  const_iterator begin() const;
  const_iterator end() const;

  reverse_iterator rbegin() {
    return std::make_reverse_iterator(this->end());
  }
  reverse_iterator rend() { return std::make_reverse_iterator(this->begin()); }
  const_reverse_iterator crbegin() const {
    return std::make_reverse_iterator(this->cend());
  }
  const_reverse_iterator crend() const {
    return std::make_reverse_iterator(this->cbegin());
  }
  const_reverse_iterator rbegin() const {
    return std::make_reverse_iterator(this->end());
  }
  const_reverse_iterator rend() const {
    return std::make_reverse_iterator(this->begin());
  }

  // Capacity
  bool empty() const { return _impl->_stored == 0; }
  size_type size() const { return _impl->_stored - _impl->_deleted; }
  size_type max_size() const {
    return static_cast<size_type>(std::numeric_limits<size_type>::max() *
                                  _impl->_max_load_factor);
  }

  // Modifiers
  void clear() { _data.clear(); }
  iterator erase(iterator pos);
  iterator erase(const_iterator pos);
  iterator erase(const_iterator first, const_iterator last);
  iterator erase(Key key);

  std::pair<iterator, bool> insert(const std::pair<Key, T> &);
  template <class M>
  std::pair<iterator, bool> insert_or_assign(const Key key, M &&value);
  template <class... Args>
  std::pair<iterator, bool> emplace(Key key, Args... value);
  template <class... Args> std::pair<iterator, bool> emplace(Args... args);
  template <class... Args>
  std::pair<iterator, bool> try_emplace(Key key, Args... value);

  friend void swap(unordered_map &a, unordered_map &b) {
    std::swap(a._impl, b._impl);
    std::swap(a._data, b._data);
  }

  void swap(unordered_map &other) { swap(*this, other); }

  // TODO: (rk) c++17 methods
  // node_type extract(iterator);
  // template < class Map >
  // void merge(Map& other);

  // Lookup
  iterator find(const Key &key);
  template <class K> iterator find(const K &x);

  const_iterator find(const Key &key) const;
  template <class K> const_iterator find(const K &x) const;

  T &at(const Key &key) {
    auto it = this->find(key);
    if (it == this->end())
      throw std::out_of_range();
    return it->second;
  }
  const T &at(const Key &key) const {
    auto it = this->find(key);
    if (it == this->end())
      throw std::out_of_range();
    return it->second;
  }

  T &operator[](const Key &key) { return this->find(key)->second; }
  const T &operator[](const Key &key) const { return this->find(key)->second; }

  bool contains(const Key &key) { return this->find(key) != this->end(); }
  template <class K> bool contains(const K &x) {
    return this->find(x) != this->end();
  }

  size_type count(const Key &key) const { return this->contains(key) ? 1 : 0; }
  template <class K> size_type count(const K &x) const {
    return this->contains(x) ? 1 : 0;
  }

  // Hash Policy
  float load_factor() const {
    return static_cast<float>(_impl->_stored) /
           static_cast<float>(_data.size());
  }
  float load_factor_deleted() const {
    return static_cast<float>(_impl->_deleted) /
           static_cast<float>(_impl->_stored);
  }
  float load_factor_chain() const {
    return static_cast<float>(this->max_chain()) /
           static_cast<float>(_impl->_stored);
  }
  float max_load_factor() const { return _impl->_max_load_factor; }
  void max_load_factor(float factor) { _impl->_max_load_factor = factor; }

  size_type max_chain() const;
  size_type mean_chain() const;

  void rehash(size_type count);
  void compress() {
    this->rehash(std::ceil((_impl->_stored - _impl->_deleted) /
                           this->max_load_factor()));
  }
  void reserve(size_type count) {
    this->rehash(std::ceil(count / this->max_load_factor()));
  }

  // Observers
  hasher hash_function() { return _hash; }
  key_equal key_eq() { return _key_equal; }

private:
  /**
   */
  using key_to_index_t = std::tuple<size_type, bool>;
  template <class K, class Equal>
  key_to_index_t key_to_index(const K &key, Equal &&is_equal) const;
  key_to_index_t key_to_index(const Key &key) const {
    return this->key_to_index(key, _key_equal);
  }
  template <class K, class Equal>
  key_to_index_t key_to_index(const K &key, Equal &&is_equal);
  key_to_index_t key_to_index(const Key &key) {
    return this->key_to_index(key, _key_equal);
  }

  size_type _2x_stored() {
    return static_cast<size_type>(std::ceil((_impl->_stored - _impl->_deleted) /
                                            this->max_load_factor()));
  }

  size_type __first_used_index() const;
  size_type __last_used_index() const;

  template <class Iterator>
  Iterator __iterator_from_index(size_type index) const {
    if (index == __INVALID_INDEX)
      index = _data.size();
    return Iterator(_data.data() + index, _data.data(),
                    _data.data() + __last_used_index() + 1);
  }

  template <class Iterator> Iterator __iterator_from_index(size_type index) {
    if (index == __INVALID_INDEX)
      index = _data.size();
    return Iterator(_data.data() + index, _data.data(),
                    _data.data() + __last_used_index() + 1);
  }

  struct impl {
    float _max_load_factor; // limit to (_stored) / _data.size()
    size_type _stored;
    size_type _deleted;
  };
  using impl_pointer = impl *;
  static impl_pointer __alloc_impl() {
    auto tmp = new impl;
    tmp->_max_load_factor = __DEFAULT_MAX_LOAD_FACTOR;
    tmp->_stored = 0;
    tmp->_deleted = 0;
    return tmp;
  }
  static void __dealloc_impl(impl_pointer pimpl) { delete pimpl; }

  impl_pointer _impl; // This is a pointer because allocators...

  container _data;
  Hash _hash;
  KeyEqual _key_equal;
};

} // namespace panda

#include "panda/core/impl/unordered_map.hpp"
