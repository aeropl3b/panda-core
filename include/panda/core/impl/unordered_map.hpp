#include <iterator>

namespace panda {

template <class Key, class T, class Hash, class KeyEqual>
class unordered_map<Key, T, Hash, KeyEqual>::node {
  enum status : char { kOPEN, kDELETED, kUSED } _status = kOPEN;
  std::pair<const Key, T> _data;

  node(const Key &key, const T &value) : _status(kUSED), _data(key, value) {}
  node(const std::pair<Key, T> &value) : _status(kUSED), _data(value) {}

  friend class unordered_map;

  // Mark as deleted and force clear the memory for the node::_data to the
  // default state
  void del() {
    _status = kDELETED;
    std::swap(_data, decltype(_data)());
  }

public:
  node() : _status(kOPEN) {}

  bool is_open() const { return _status == kOPEN; }
  bool is_deleted() const { return _status == kDELETED; }
  bool is_used() const { return _status == kUSED; }

  Key key() { return _data.first; }
  std::pair<const Key, T> &value() { return _data; }
  const std::pair<const Key, T> &value() const { return _data; }
  T &mapped() { return _data.second; }
  const T &mapped() const { return _data.second; }

  void swap(node &other) {
    std::swap(this->_status, other._status);
    // How does one swap pairs with const?
    auto tmp(other._data);
    new (&other._data) std::pair<const Key, T>(this->_data);
    new (&this->_data) std::pair<const Key, T>(tmp);
  }
};

template <class Key, class T, class Hash, class KeyEqual>
unordered_map<Key, T, Hash, KeyEqual>::~unordered_map() {
  __dealloc_impl(_impl);
}

template <class Key, class T, class Hash, class KeyEqual>
unordered_map<Key, T, Hash, KeyEqual>::unordered_map(const unordered_map &other)
    : _impl(__alloc_impl()), _data(other._data), _hash(other._hash),
      _key_equal(other._key_equal) {
  _impl->_max_load_factor = other._impl->_max_load_factor;
  _impl->_stored = other._impl->_stored;
  _impl->_deleted = other._impl->_deleted;
}

template <class Key, class T, class Hash, class KeyEqual>
unordered_map<Key, T, Hash, KeyEqual>::unordered_map(unordered_map &&other)
    : _impl(other._impl), _data(std::move(other._data)),
      _hash(std::move(other._hash)), _key_equal(std::move(other._key_equal)) {
  other._impl = __alloc_impl();
}

template <class Key, class T, class Hash, class KeyEqual>
unordered_map<Key, T, Hash, KeyEqual> &
unordered_map<Key, T, Hash, KeyEqual>::operator=(unordered_map other) {
  swap(*this, other);
  return *this;
}

template <class Key, class T, class Hash, class KeyEqual>
unordered_map<Key, T, Hash, KeyEqual>::unordered_map()
    : unordered_map(1, Hash(), KeyEqual()) {}

template <class Key, class T, class Hash, class KeyEqual>
unordered_map<Key, T, Hash, KeyEqual>::unordered_map(size_type count)
    : unordered_map(count, Hash(), KeyEqual()) {}

template <class Key, class T, class Hash, class KeyEqual>
unordered_map<Key, T, Hash, KeyEqual>::unordered_map(size_type count,
                                                     const Hash &hash)
    : unordered_map(count, hash, KeyEqual()) {}

template <class Key, class T, class Hash, class KeyEqual>
unordered_map<Key, T, Hash, KeyEqual>::unordered_map(size_type count,
                                                     const Hash &hash,
                                                     const KeyEqual &equal)
    : _impl(__alloc_impl()),
      _data(static_cast<size_type>(std::max(__one, count) /
                                   _impl->_max_load_factor)),
      _hash(hash), _key_equal(equal) {}

template <class Key, class T, class Hash, class KeyEqual>
template <class InputIt>
unordered_map<Key, T, Hash, KeyEqual>::unordered_map(InputIt first,
                                                     InputIt last)
    : unordered_map(first, last, 1, Hash(), KeyEqual()) {}

template <class Key, class T, class Hash, class KeyEqual>
template <class InputIt>
unordered_map<Key, T, Hash, KeyEqual>::unordered_map(InputIt first,
                                                     InputIt last,
                                                     size_type count)
    : unordered_map(first, last, count, Hash(), KeyEqual()) {}

template <class Key, class T, class Hash, class KeyEqual>
template <class InputIt>
unordered_map<Key, T, Hash, KeyEqual>::unordered_map(InputIt first,
                                                     InputIt last,
                                                     size_type count,
                                                     const Hash &hash)
    : unordered_map(first, last, count, hash, KeyEqual()) {}

template <class Key, class T, class Hash, class KeyEqual>
template <class InputIt>
unordered_map<Key, T, Hash, KeyEqual>::unordered_map(InputIt first,
                                                     InputIt last,
                                                     size_type count,
                                                     const Hash &hash,
                                                     const KeyEqual &equal)
    : _impl(__alloc_impl()), _data(std::max(__one, count)), _hash(hash),
      _key_equal(equal) {
  auto it = first;
  for (; it != last; it++) {
    this->insert(*it);
  }
}

template <class Key, class T, class Hash, class KeyEqual>
unordered_map<Key, T, Hash, KeyEqual>::unordered_map(
    std::initializer_list<value_type>
        init /*, const Allocator& alloc = Allocator()*/)
    : unordered_map(init, __one, Hash(), KeyEqual()) {}
template <class Key, class T, class Hash, class KeyEqual>
unordered_map<Key, T, Hash, KeyEqual>::unordered_map(
    std::initializer_list<value_type> init,
    size_type count /*, const Allocator& alloc = Allocator()*/)
    : unordered_map(init, count, Hash(), KeyEqual()) {}
template <class Key, class T, class Hash, class KeyEqual>
unordered_map<Key, T, Hash, KeyEqual>::unordered_map(
    std::initializer_list<value_type> init, size_type count,
    const Hash &hash /*, const Allocator& alloc = Allocator()*/)
    : unordered_map(init, count, hash, KeyEqual()) {}
template <class Key, class T, class Hash, class KeyEqual>
unordered_map<Key, T, Hash, KeyEqual>::unordered_map(
    std::initializer_list<value_type> init, size_type count, const Hash &hash,
    const KeyEqual &equal /*, const Allocator& alloc = Allocator()*/)
    : _impl(__alloc_impl()),
      _data(static_cast<size_type>(std::max({__one, count, init.size()}) /
                                   _impl->_max_load_factor)),
      _hash(hash), _key_equal(equal) {
  for (auto &el : init) {
    this->insert(el);
  }
}

template <class Key, class T, class Hash, class KeyEqual>
template <class Iterator, class ContainerIterator>
class unordered_map<Key, T, Hash, KeyEqual>::iterator_common_impl {
protected:
  ContainerIterator _it, _begin, _end;

  iterator_common_impl(ContainerIterator it, ContainerIterator begin,
                       ContainerIterator end)
      : _it(it), _begin(begin), _end(end) {}

public:
  iterator_common_impl(const Iterator &other)
      : _it(other._it), _begin(other._begin), _end(other._end) {}

  Iterator &operator=(const Iterator &other) {
    if (this != &other) {
      _it = other._it;
      _begin = other._begin;
      _end = other._end;
    }
    return *this;
  }

  Iterator operator++() {
    ++_it;
    while (_it != _end && !_it->is_used())
      ++_it;
    return *this;
  }
  Iterator operator++(int) {
    auto save = *this;
    this->operator++();
    return save;
  }
  Iterator operator--() {
    --_it;
    while (_it != _begin && !_it->is_used())
      --_it;
    return *this;
  }
  Iterator operator--(int) {
    auto save = *this;
    this->operator++();
    return save;
  }

  friend bool operator==(const iterator_common_impl &a,
                         const iterator_common_impl &b) {
    return (a._it == b._it && a._begin == b._begin);
  }
  friend bool operator!=(const iterator_common_impl &a,
                         const iterator_common_impl &b) {
    return !(a == b);
  }
};

template <class Key, class T, class Hash, class KeyEqual>
class unordered_map<Key, T, Hash, KeyEqual>::const_iterator_impl
    : public unordered_map<Key, T, Hash, KeyEqual>::iterator_common_impl<
          unordered_map<Key, T, Hash, KeyEqual>::const_iterator_impl,
          typename unordered_map<Key, T, Hash, KeyEqual>::container::const_iterator> {
  using node_vec_iterator = typename unordered_map::container::const_iterator;
  using iterator_common =
      typename unordered_map<Key, T, Hash, KeyEqual>::iterator_common_impl<
          unordered_map<Key, T, Hash, KeyEqual>::const_iterator_impl,typename unordered_map<Key, T, Hash, KeyEqual>::container::const_iterator>;

  const_iterator_impl(node_vec_iterator it, node_vec_iterator begin,
                      node_vec_iterator end)
      : iterator_common(it, begin, end) {}
  friend unordered_map;

public:
  using difference_type =
      typename unordered_map<Key, T, Hash, KeyEqual>::difference_type;
  using value_type = typename unordered_map<Key, T, Hash, KeyEqual>::value_type;
  using pointer = typename unordered_map<Key, T, Hash, KeyEqual>::const_pointer;
  using reference =
      typename unordered_map<Key, T, Hash, KeyEqual>::const_reference;
  using iterator_category = std::bidirectional_iterator_tag;

  reference operator*() { return this->_it->value(); }
  pointer operator->() { return &(this->_it->value()); }
};

template <class Key, class T, class Hash, class KeyEqual>
class unordered_map<Key, T, Hash, KeyEqual>::iterator_impl
    : public unordered_map<Key, T, Hash, KeyEqual>::iterator_common_impl<
          unordered_map<Key, T, Hash, KeyEqual>::iterator_impl,
          typename unordered_map<Key, T, Hash, KeyEqual>::container::iterator> {
  using node_vec_iterator = typename unordered_map::container::iterator;
  using iterator_common =
      typename unordered_map<Key, T, Hash, KeyEqual>::iterator_common_impl<
          unordered_map<Key, T, Hash, KeyEqual>::iterator_impl,typename unordered_map<Key, T, Hash, KeyEqual>::container::const_iterator>;
  iterator_impl(node_vec_iterator it, node_vec_iterator begin,
                node_vec_iterator end)
      : iterator_common(it, begin, end) {}
  friend unordered_map;

public:
  using difference_type =
      typename unordered_map<Key, T, Hash, KeyEqual>::difference_type;
  using value_type = typename unordered_map<Key, T, Hash, KeyEqual>::value_type;
  using pointer = typename unordered_map<Key, T, Hash, KeyEqual>::pointer;
  using reference = typename unordered_map<Key, T, Hash, KeyEqual>::reference;
  using iterator_category = std::bidirectional_iterator_tag;

  reference operator*() { return this->_it->value(); }
  pointer operator->() { return &(this->_it->value()); }

  operator const_iterator_impl() const {
    return const_iterator_impl(this->_it, this->_begin,
                               this->_end);
  }
};

template <class Key, class T, class Hash, class KeyEqual>
typename unordered_map<Key, T, Hash, KeyEqual>::iterator
unordered_map<Key, T, Hash, KeyEqual>::begin() {
  return iterator(_data.begin(), _data.begin(), _data.end());
}

template <class Key, class T, class Hash, class KeyEqual>
typename unordered_map<Key, T, Hash, KeyEqual>::iterator
unordered_map<Key, T, Hash, KeyEqual>::end() {
  return iterator(_data.end(), _data.begin(), _data.end());
}

template <class Key, class T, class Hash, class KeyEqual>
typename unordered_map<Key, T, Hash, KeyEqual>::const_iterator
unordered_map<Key, T, Hash, KeyEqual>::begin() const {
  return const_iterator(_data.begin(), _data.begin(), _data.end());
}

template <class Key, class T, class Hash, class KeyEqual>
typename unordered_map<Key, T, Hash, KeyEqual>::const_iterator
unordered_map<Key, T, Hash, KeyEqual>::end() const {
  return const_iterator(_data.end(), _data.begin(), _data.end());
}

template <class Key, class T, class Hash, class KeyEqual>
typename unordered_map<Key, T, Hash, KeyEqual>::const_iterator
unordered_map<Key, T, Hash, KeyEqual>::cbegin() const {
  return const_iterator(_data.begin(), _data.begin(), _data.end());
}

template <class Key, class T, class Hash, class KeyEqual>
typename unordered_map<Key, T, Hash, KeyEqual>::const_iterator
unordered_map<Key, T, Hash, KeyEqual>::cend() const {
  return const_iterator(_data.end(), _data.begin(), _data.end());
}

template <class Key, class T, class Hash, class KeyEqual>
typename unordered_map<Key, T, Hash, KeyEqual>::iterator
unordered_map<Key, T, Hash, KeyEqual>::erase(iterator pos) {
  return this->erase(pos, ++pos);
}

template <class Key, class T, class Hash, class KeyEqual>
typename unordered_map<Key, T, Hash, KeyEqual>::iterator
unordered_map<Key, T, Hash, KeyEqual>::erase(const_iterator pos) {
  return this->erase(pos, ++pos);
}

template <class Key, class T, class Hash, class KeyEqual>
typename unordered_map<Key, T, Hash, KeyEqual>::iterator
unordered_map<Key, T, Hash, KeyEqual>::erase(const_iterator first,
                                             const_iterator last) {
  auto it = first;
  for (; it != last; ++it) {
    it->del();
    _impl->_deleted++;
  }
  // If it is underloaded by a factor of 4, then rehash with a the smaller size
  if (this->load_factor_deleted() > this->max_load_factor()) {
    this->rehash(this->_2x_stored());
  }
  return it;
}

template <class Key, class T, class Hash, class KeyEqual>
typename unordered_map<Key, T, Hash, KeyEqual>::iterator
unordered_map<Key, T, Hash, KeyEqual>::erase(Key key) {
  return this->erase(this->find(key));
}

template <class Key, class T, class Hash, class KeyEqual>
template <class K, class Equal>
typename unordered_map<Key, T, Hash, KeyEqual>::key_to_index_t
unordered_map<Key, T, Hash, KeyEqual>::key_to_index(const K &key,
                                                    Equal &&is_equal) const {
  auto cap = _data.size();
  size_type count = 0;
  auto index = _hash(key) % _data.size();
  auto index_del = __invalid_index;
  bool exists = false;

  while (!_data[index].is_open() // stop at the first open slot
         && !exists              // or if the key was found
         && count != cap         // or if we looked at every slot in the map
  ) {
    if (is_equal(_data[index].key(), key)) {
      exists = true;
    } else {
      if (_data[index].is_deleted() && index_del == __invalid_index) {
        index_del = index;
      }
      index = ++index % cap;
      ++count;
    }
  }

  if (count == cap) {
    index = __invalid_index;
  }

  return std::make_tuple(index, exists);
}

template <class Key, class T, class Hash, class KeyEqual>
template <class K, class Equal>
typename unordered_map<Key, T, Hash, KeyEqual>::key_to_index_t
unordered_map<Key, T, Hash, KeyEqual>::key_to_index(const K &key,
                                                    Equal &&is_equal) {
  auto cap = _data.size();
  size_type count = 0;
  auto index = _hash(key) % _data.size();
  auto index_del = __invalid_index;
  bool exists = false;

  while (!_data[index].is_open() // stop at the first open slot
         && !exists              // or if the key was found
         && count != cap         // or if we looked at every slot in the map
  ) {
    if (is_equal(_data[index].key(), key)) {
      exists = true;
    } else {
      if (_data[index].is_deleted() && index_del == __invalid_index) {
        index_del = index;
      }
      index = ++index % cap;
      ++count;
    }
  }

  if (index_del != __invalid_index) {
    if (exists) {
      // If it exists, and there was a deleted slot open before, move the data
      // closer to the hash index. This is not done in the const version
      _data[index_del].swap(_data[index]);
    }
    index = index_del;
  } else if (count == cap) {
    index = __invalid_index;
  }

  return std::make_tuple(index, exists);
}

template <class Key, class T, class Hash, class KeyEqual>
std::pair<typename unordered_map<Key, T, Hash, KeyEqual>::iterator, bool>
unordered_map<Key, T, Hash, KeyEqual>::insert(const std::pair<Key, T> &value) {
  size_type index;
  bool exists;
  std::tie(index, exists) = this->key_to_index(value.first);

  if (index == __invalid_index) {
    this->rehash(this->_2x_stored());
    std::tie(index, exists) = this->key_to_index(value.first);
  }

  if (!exists) {
    new (&_data[index]) node(value);
    _impl->_stored++;
    if (this->load_factor() >= this->max_load_factor()) {
      this->rehash(this->_2x_stored());
      return std::make_pair(this->find(value.first), true);
    }
  }

  return std::make_pair(
      iterator(_data.begin() += index, _data.begin(), _data.end()), !exists);
}

template <class Key, class T, class Hash, class KeyEqual>
typename unordered_map<Key, T, Hash, KeyEqual>::iterator
unordered_map<Key, T, Hash, KeyEqual>::find(const Key &key) {
  bool exists;
  size_type index;
  std::tie(index, exists) = this->key_to_index(key);
  if (!exists) {
    return this->end();
  } else {
    return iterator(_data.begin() += index, _data.begin(), _data.end());
  }
}

template <class Key, class T, class Hash, class KeyEqual>
template <class K>
typename unordered_map<Key, T, Hash, KeyEqual>::iterator
unordered_map<Key, T, Hash, KeyEqual>::find(const K &x) {
  bool exists;
  size_type index;
  std::tie(index, exists) =
      this->key_to_index(x, [](const Key &k, const K &x) { return k == x; });
  if (!exists) {
    return this->end();
  } else {
    return iterator(_data.begin() += index, _data.begin(), _data.end());
  }
}

template <class Key, class T, class Hash, class KeyEqual>
typename unordered_map<Key, T, Hash, KeyEqual>::const_iterator
unordered_map<Key, T, Hash, KeyEqual>::find(const Key &key) const {
  bool exists;
  size_type index;
  std::tie(index, exists) = this->key_to_index(key);
  if (!exists) {
    return this->end();
  } else {
    return iterator(_data.begin() += index, _data.begin(), _data.end());
  }
}

template <class Key, class T, class Hash, class KeyEqual>
template <class K>
typename unordered_map<Key, T, Hash, KeyEqual>::const_iterator
unordered_map<Key, T, Hash, KeyEqual>::find(const K &x) const {
  bool exists;
  size_type index;
  std::tie(index, exists) =
      this->key_to_index(x, [](const Key &k, const K &x) { return k == x; });
  if (!exists) {
    return this->end();
  } else {
    return iterator(_data.begin() += index, _data.begin(), _data.end());
  }
}

template <class Key, class T, class Hash, class KeyEqual>
void unordered_map<Key, T, Hash, KeyEqual>::rehash(size_type count) {
  std::vector<node> save = std::move(_data);
  // Either resize to count or the minimum allowed size for the max_load_factor
  count = std::max({count, static_cast<size_type>(
                               std::ceil((_impl->_stored - _impl->_deleted) /
                                         this->max_load_factor()))});
  _data.resize(count);
  // Reset counters
  _impl->_deleted = 0;
  _impl->_stored = 0;
  for (auto &slot : save) {
    if (slot.is_used()) {
      this->insert(slot.value());
    }
  }
}

template <class Key, class T, class Hash, class KeyEqual>
typename unordered_map<Key, T, Hash, KeyEqual>::size_type
unordered_map<Key, T, Hash, KeyEqual>::max_chain() const {
  size_type chain = 0;
  size_type max_chain = 0;
  if (!this->empty()) {
    for (const auto &slot : _data) {
      if (slot.is_used()) {
        chain++;
      } else {
        max_chain = std::max(chain, max_chain);
        chain = 0;
      }
    }
  }
  return max_chain;
}

template <class Key, class T, class Hash, class KeyEqual>
typename unordered_map<Key, T, Hash, KeyEqual>::size_type
unordered_map<Key, T, Hash, KeyEqual>::mean_chain() const {
  size_type num_chain = 0;
  size_type chain = 0;
  size_type sum_chain = 0;
  if (!this->empty()) {
    for (const auto &slot : _data) {
      if (slot.is_used()) {
        chain++;
      } else {
        sum_chain += chain;
        num_chain++;
        chain = 0;
      }
    }
  }
  return static_cast<size_type>(
      std::ceil(static_cast<float>(sum_chain) / static_cast<float>(num_chain)));
}

} // namespace panda
