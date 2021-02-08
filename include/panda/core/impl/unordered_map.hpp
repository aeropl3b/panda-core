
namespace panda {

template <class Key, class T, class Hash, class KeyEqual>
class unordered_map<Key, T, Hash, KeyEqual>::node {
  enum status : char { kOPEN, kDELETE, kUSE } _status = kOPEN;
  std::pair<const Key, T> _data;

  node() : _status(kOPEN) {}
  node(const Key &key, const T &value) : _status(kUSE), _data(key, value) {}
  node(const std::pair<Key, T> &value) : _status(kUSE), _data(value) {}

  friend class unordered_map::iterator_common_impl;
  friend class unordered_map;

  // Mark as deleted and force clear the memory for the node::_data to the
  // default state
  void del() {
    _status = kDELETE;
    std::swap(_data, decltype(_data)());
  }

public:
  Key key() { return _data.first; }
  std::pair<const Key, T> &value() { return _data.second; }
  T &mapped() { return _data.second; }

  void swap(node &other) {
    std::swap(this->_status, other._status);
    std::swap(this->_data, other._data);
  }
};

template <class Key, class T, class Hash, class KeyEqual>
unordered_map<Key, T, Hash, KeyEqual>::unordered_map()
    : unordered_map(0, Hash(), KeyEqual()) {}

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
    : _max_load_factor(__default_max_load_factor),
      _data(static_cast<size_type>(count / _max_load_factor)), _hash(hash),
      _key_equal(equal), {}

template <class Key, class T, class Hash, class KeyEqual>
template <class InputIt>
unordered_map<Key, T, Hash, KeyEqual>::unordered_map(InputIt first,
                                                     InputIt last)
    : unordered_map(first, last, 0, Hash(), KeyEqual()) {}

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
    : _max_load_factor(__default_max_load_factor),
      _data(static_cast<size_type>(std::max(count, std::distance(first, last)) /
                                   _max_load_factor)),
      _hash(hash), _key_equal(equal) {
  auto it = first;
  for (; it != last; it++) {
    this->insert(*it);
  }
}

template <class Key, class T, class Hash, class KeyEqual>
template <class Iterator>
class unordered_map<Key, T, Hash, KeyEqual>::iterator_common_impl {
protected:
  using node_vec_iterator = std::vector<unorded_map::node>::iterator;

  node_vec_iterator _it, _begin, _end;

  iterator_common_impl(node_vec_iterator it, node_vec_iterator begin,
                       node_vec_iterator end)
      : _it(it), _begin(begin), _end(end) {}

public:
  Iterator operator++() {
    ++_it;
    while (_it != _end && it->status() != kUSE)
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
    while (_it != _begin && it->status() != kUSE)
      --_it;
    return *this;
  }
  Iterator operator--(int) {
    auto save = *this;
    this->operator++();
    return save;
  }
};

template <class Key, class T, class Hash, class KeyEqual>
class unordered_map<Key, T, Hash, KeyEqual>::const_iterator_impl
    : unordered_map<Key, T, Hash, KeyEqual>::iterator_common_impl<
          unordered_map<Key, T, Hash, KeyEqual>::const_iterator_impl> {
  const_iterator_impl(node_vec_iterator it, node_vec_iterator begin,
                      node_vec_iterator end)
      : iterator_common_impl(it, begin, end) {}
  friend unordered_map;

public:
  const std::pair<const Key, T> &operator*() { return _it->value(); }
  const std::pair<const Key, T> *operator->() { return &(_it->value()); }
};

template <class Key, class T, class Hash, class KeyEqual>
class unordered_map<Key, T, Hash, KeyEqual>::iterator_impl
    : unordered_map<Key, T, Hash, KeyEqual>::iterator_common_impl<
          unordered_map<Key, T, Hash, KeyEqual>::iterator_impl> {
  iterator_impl(node_vec_iterator it, node_vec_iterator begin,
                node_vec_iterator end)
      : iterator_common_impl(it, begin, end) {}
  friend unordered_map;

public:
  std::pair<const Key, T> &operator*() { return _it->value(); }
  std::pair<const Key, T> *operator->() { return &(_it->value()); }

  const_iterator_impl() const { return const_iterator_impl(_it, _begin, _end); }
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
typename unordered_map<Key, T, Hash, KeyEqual>::cosnt_iterator
unordered_map<Key, T, Hash, KeyEqual>::end() const {
  return const_iterator(_data.end(), _data.begin(), _data.end());
}

template <class Key, class T, class Hash, class KeyEqual>
typename unordered_map<Key, T, Hash, KeyEqual>::const_iterator
unordered_map<Key, T, Hash, KeyEqual>::cbegin() const {
  return const_iterator(_data.begin(), _data.begin(), _data.end());
}

template <class Key, class T, class Hash, class KeyEqual>
typename unordered_map<Key, T, Hash, KeyEqual>::cosnt_iterator
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
    m_stored--;
  }
  // If it is underloaded by a factor of 4, then rehash with a the smaller size
  if (this->load_factor() < (this->max_load_factor() / 4)) {
    this->rehash((m_stored * 2) / _max_load_factor);
  }
  return it;
}

template <class Key, class T, class Hash, class KeyEqual>
typename unordered_map<Key, T, Hash, KeyEqual>::iterator
unordered_map<Key, T, Hash, KeyEqual>::erase(Key key) {
  return this->erase(this->find(key));
}

template <class Key, class T, class Hash, class KeyEqual>
std::pair<unordered_map<Key, T, Hash, KeyEqual>::size_type,bool> unordered_map<Key, T, Hash, KeyEqual>::hash_to_index(size_type hashed_key)
{
  auto cap = _data.size();
  auto start = _hash(value.first) % _data.size();
  auto index = start;
  bool created = true;

  while (_data[index] == kUSE && created) {
    if (_hash(_data[index].key()), hashed_key) {
      created = false;
    } else {
      index++;
      if (index == cap) {
        // Loop back around if it hits the end
        index = 0;
      }
#if defined(PANDA_UNORDERED_MAP_ALLOW_MAX_LOAD_ONE)
      if (index == start) {
        // This should never happen so long has the _max_load_factor < 1
        this->rehash(_data.size() * 2);
        return this->hash_to_index(hashed_key);
      }
#endif
    }
  }
  return index;
}

template <class Key, class T, class Hash, class KeyEqual>
std::pair<typename unordered_map<Key, T, Hash, KeyEqual>::iterator, bool>
unordered_map<Key, T, Hash, KeyEqual>::insert(const std::pair<Key, T> &value) {
  size_type index;
  bool created;
  std::tie(index,created) = hash_to_index(_hash(value.first));

  if (created) {
    new (&_data[index]) node(value);
    m_stored++;
    if (this->load_factor() >= this->max_load_factor()) {
      this->rehash((m_stored * 2) / this->max_load_factor());
      return this->find(value.first);
    }
  }

  return std::make_pair(
      iterator(_data.begin() + index, _data.begin(), _data.end()), created);
}

template <class Key, class T, class Hash, class KeyEqual>
typename unordered_map<Key, T, Hash, KeyEqual>::iterator
unordered_map<Key, T, Hash, KeyEqual>::find(const Key &key) {
  bool exists;
  size_type index;
  std::tie(index,exists) = hash_to_index(_hash(key));
  if(!exists)
  {
    return this->end();
  }
  else
  {
    return iterator(_data.begin() + index, _data.begin(), _data.end());
  }
}

} // namespace panda
