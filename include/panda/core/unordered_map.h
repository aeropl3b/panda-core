#pragma once

#include "panda/core/mpl.h"

#include <cstddef> // std::size_t std::ptrdiff_t

#include <functional> // std::hash std::equal_to
#include <utility>    // std::pair
#include <vector>     // std::vector

#include <boost/mpl/if.hpp>

/**
 * PANDA_UNORDERED_MAP_ALLOW_MAX_LOAD_ONE - allows the max load factor to be set to one (not recommended)
 */

namespace panda {

/** Drop in replacement for std::unordered_map that uses open addressing which
 * is in general faster than chaining forced by STL spec
 */
// TODO (rk) make allocator aware
template <class Key, class T, class Hash = std::hash<Key>,
          class KeyEqual = std::equal_to<Key>>
class unordered_map {
  static constexpr float __default_max_load_factor = 0.7;

  class node;
  class iterator_common_impl;
  class const_iterator_impl;
  class iterator_impl;

public:
  using key_type = Key;
  using mapped_type = T;
  using value_type = std::pair<const Key, T>;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using hasher = typename boost::mpl::_if<::panda::mpl::is_empty_object<Hash>,Hash,Hash&>::type;
  using key_equal = typename boost::mpl::_if<::panda::mpl::is_empty_object<KeyEqual>,KeyEqual,KeyEqual&>::type;
  using reference = value_type &;
  using const_reference = const value_type &;
  using pointer = value_type *;
  using const_pointer = value_type const *;
  using iterator = iterator_impl;
  using const_iterator = const_iterator_impl;
  using local_iterator = iterator_impl;
  using const_local_iterator = const_iterator_impl;
  using node_type = node;

  unordered_map();
 
  unordered_map(size_type count);
 
  unordered_map(size_type count, const Hash &hash);
 
  unordered_map(size_type count, const Hash &hash, const KeyEqual &equal);

  template <class InputIt> unordered_map(InputIt first, InputIt last);

  template <class InputIt>
  unordered_map(InputIt first, InputIt last, size_type count);

  template <class InputIt>
  unordered_map(InputIt first, InputIt last, size_type count, const Hash &hash);

  template <class InputIt>
  unordered_map(InputIt first, InputIt last, size_type count, const Hash &hash,
                const KeyEqual &equal);

  unordered_map(const unorded_map &) = default;
  // unordered_map(unorded_map&&) = default;

  unordered_map &operator=(const unordered_map &) = default;
  // unordered_map& operator=(unordered_map&&) = default;

  // Iterators
  iterator begin();
  iterator end();
  const_iterator cbegin() const;
  const_iterator cend() const;
  const_iterator begin() const;
  const_iterator end() const;

  // Capacity
  bool empty() const { return m_stored == 0; }
  size_type size() const { return m_stored; }
  size_type max_size() const {
    return static_cast<size_type>(std::numberic_limits<size_type>::max() *
                                  _max_load_factor);
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

  void swap(unordered_map &other);

  // TODO: (rk) c++17 methods
  // node_type extract(iterator);
  // template < class Map >
  // void merge(Map& other);

  // Lookup
  iterator find(const Key &key);
  template <class K> iterator find(const K &x);

  cosnt_iterator find(const Key &key) const;
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
    return this->find(key) != this->end();
  }

  size_type count(const Key &key) const { return this->contains(key) ? 1 : 0; }
  template <class K> size_type count(const K &x) const {
    return this->contains(key) ? 1 : 0;
  }

  // Hash Policy
  float load_factor() const {
    return static_cast<float>(_stored) / static_cast<float>(_data.size());
  }
  float max_load_factor() const { return _max_load_factor; }
  void max_load_factor(float factor) { _max_load_factor = factor; }

  void rehash(size_type count);
  void reserve(size_type count);

  // Observers
  hasher hash_function() { return _hash; }
  key_equal key_eq() { return _key_equal; }

private:

  std::pair<size_type,bool> hash_to_index(size_type hashed_key);

  float _max_load_factor; // limit to (_stored + _deleted) / _data.size()
  std::vector<node<T>> _data;
  size_type _stored;
  size_type _deleted;
  Hash _hash;
  KeyEqual _key_equal;
};

} // namespace panda

#include "panda/core/impl/unordered_map.hpp"
