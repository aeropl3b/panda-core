#include <gtest/gtest.h>

#include <vector>

#include "panda/test/generator.h"

#include "panda/core/unordered_map.h"

template <class T> struct HashableType {
  T _;

  operator T &() { return _; }
  operator const T &() const { return _; }

  friend bool operator<(const HashableType &a, const HashableType &b) {
    return a._ < b._;
  }
  friend bool operator>(const HashableType &a, const HashableType &b) {
    return a._ > b._;
  }
  friend bool operator==(const HashableType &a, const HashableType &b) {
    return a._ == b._;
  }
  friend bool operator!=(const HashableType &a, const HashableType &b) {
    return a._ != b._;
  }
};

namespace std {
template <class T> struct hash<HashableType<T>> : public ::std::hash<T> {};
} // namespace std

namespace panda {
namespace test {

template <class T> struct generator<HashableType<T>> {
  HashableType<T> operator()() {
    HashableType<T> ht;
    ht._ = generator<T>()();
    return ht;
  }
};

} // namespace test
} // namespace panda

template <class T> class unordered_map_test : public ::testing::Test {
public:
  using KeyType = typename std::tuple_element<0, T>::type;
  using MappedType = typename std::tuple_element<1, T>::type;
  using ValueType = std::pair<KeyType, MappedType>;
};
#define TUP(K, V) std::tuple<K, V>
using TestTypes =
    ::testing::Types<TUP(int, int), TUP(char, char), TUP(char, std::string),
                     TUP(std::string, char),
                     TUP(HashableType<std::string>, char),
                     TUP(std::size_t, std::vector<double>)>;
TYPED_TEST_SUITE(unordered_map_test, TestTypes);

#define _PANDA_UNORDERED_MAP_TEST_INIT_LIST                                    \
  ::panda::test::generator<KeyType> key_gen;                                   \
  ::panda::test::generator<MappedType> value_gen;                              \
  std::initializer_list<std::pair<const KeyType, MappedType>> init_list = {    \
      {key_gen(), value_gen()},                                                \
      {key_gen(), value_gen()},                                                \
      {key_gen(), value_gen()},                                                \
      {key_gen(), value_gen()}}
#define _PANDA_UNORDERED_MAP_TEST_CONSTRUCTOR_VALIDATE_MAP                     \
  std::set<KeyType> dups;                                                      \
  for (const auto &el : init_list) {                                           \
    auto it = map.find(el.first);                                              \
    ASSERT_NE(it, map.end());                                                  \
    bool is_unique;                                                            \
    std::tie(std::ignore, is_unique) = dups.insert(it->first);                 \
    if (!is_unique)                                                            \
      continue;                                                                \
    ASSERT_EQ(it->first, el.first);                                            \
    ASSERT_EQ(it->second, el.second);                                          \
  }                                                                            \
  (void)0

/**
 * CONSTRUCTOR TESTS
 */
TYPED_TEST(unordered_map_test, default_contructor) {
  using KeyType = typename TestFixture::KeyType;
  using MappedType = typename TestFixture::MappedType;

  panda::unordered_map<KeyType, MappedType> map;
}

TYPED_TEST(unordered_map_test, constructor_1) {
  using KeyType = typename TestFixture::KeyType;
  using MappedType = typename TestFixture::MappedType;

  panda::unordered_map<KeyType, MappedType> map(1);
}

TYPED_TEST(unordered_map_test, contructor_2) {
  using KeyType = typename TestFixture::KeyType;
  using MappedType = typename TestFixture::MappedType;

  panda::unordered_map<KeyType, MappedType> map(1, std::hash<KeyType>());
}

TYPED_TEST(unordered_map_test, contructor_3) {
  using KeyType = typename TestFixture::KeyType;
  using MappedType = typename TestFixture::MappedType;

  panda::unordered_map<KeyType, MappedType> map(1, std::hash<KeyType>(),
                                                std::equal_to<KeyType>());
}

TYPED_TEST(unordered_map_test, iterator_contructor) {
  using KeyType = typename TestFixture::KeyType;
  using MappedType = typename TestFixture::MappedType;

  _PANDA_UNORDERED_MAP_TEST_INIT_LIST;
  panda::unordered_map<KeyType, MappedType> map(init_list.begin(),
                                                init_list.end());
  _PANDA_UNORDERED_MAP_TEST_CONSTRUCTOR_VALIDATE_MAP;
}

TYPED_TEST(unordered_map_test, iterator_contructor_1) {
  using KeyType = typename TestFixture::KeyType;
  using MappedType = typename TestFixture::MappedType;

  _PANDA_UNORDERED_MAP_TEST_INIT_LIST;
  panda::unordered_map<KeyType, MappedType> map(init_list.begin(),
                                                init_list.end(), 10);
  _PANDA_UNORDERED_MAP_TEST_CONSTRUCTOR_VALIDATE_MAP;
}

TYPED_TEST(unordered_map_test, iterator_contructor_2) {
  using KeyType = typename TestFixture::KeyType;
  using MappedType = typename TestFixture::MappedType;

  _PANDA_UNORDERED_MAP_TEST_INIT_LIST;
  panda::unordered_map<KeyType, MappedType> map(
      init_list.begin(), init_list.end(), 10, std::hash<KeyType>());
  _PANDA_UNORDERED_MAP_TEST_CONSTRUCTOR_VALIDATE_MAP;
}

TYPED_TEST(unordered_map_test, iterator_contructor_3) {
  using KeyType = typename TestFixture::KeyType;
  using MappedType = typename TestFixture::MappedType;

  _PANDA_UNORDERED_MAP_TEST_INIT_LIST;
  panda::unordered_map<KeyType, MappedType> map(
      init_list.begin(), init_list.end(), 10, std::hash<KeyType>(),
      std::equal_to<KeyType>());
  _PANDA_UNORDERED_MAP_TEST_CONSTRUCTOR_VALIDATE_MAP;
}

TYPED_TEST(unordered_map_test, list_contructor_assign) {
  using KeyType = typename TestFixture::KeyType;
  using MappedType = typename TestFixture::MappedType;

  _PANDA_UNORDERED_MAP_TEST_INIT_LIST;
  panda::unordered_map<KeyType, MappedType> map = init_list;
  _PANDA_UNORDERED_MAP_TEST_CONSTRUCTOR_VALIDATE_MAP;
}

TYPED_TEST(unordered_map_test, list_contructor) {
  using KeyType = typename TestFixture::KeyType;
  using MappedType = typename TestFixture::MappedType;

  _PANDA_UNORDERED_MAP_TEST_INIT_LIST;
  panda::unordered_map<KeyType, MappedType> map(init_list);
  _PANDA_UNORDERED_MAP_TEST_CONSTRUCTOR_VALIDATE_MAP;
}

TYPED_TEST(unordered_map_test, list_contructor_1) {
  using KeyType = typename TestFixture::KeyType;
  using MappedType = typename TestFixture::MappedType;

  _PANDA_UNORDERED_MAP_TEST_INIT_LIST;
  panda::unordered_map<KeyType, MappedType> map(init_list, 1);
  _PANDA_UNORDERED_MAP_TEST_CONSTRUCTOR_VALIDATE_MAP;
}

TYPED_TEST(unordered_map_test, list_contructor_2) {
  using KeyType = typename TestFixture::KeyType;
  using MappedType = typename TestFixture::MappedType;

  _PANDA_UNORDERED_MAP_TEST_INIT_LIST;
  panda::unordered_map<KeyType, MappedType> map(init_list, 1,
                                                std::hash<KeyType>());
  _PANDA_UNORDERED_MAP_TEST_CONSTRUCTOR_VALIDATE_MAP;
}

TYPED_TEST(unordered_map_test, list_contructor_3) {
  using KeyType = typename TestFixture::KeyType;
  using MappedType = typename TestFixture::MappedType;

  _PANDA_UNORDERED_MAP_TEST_INIT_LIST;
  panda::unordered_map<KeyType, MappedType> map(
      init_list, 1, std::hash<KeyType>(), std::equal_to<KeyType>());
  _PANDA_UNORDERED_MAP_TEST_CONSTRUCTOR_VALIDATE_MAP;
}

// TODO
// copy construct
// move constructor
// swap
// operator= move
// operator= copy
// operator= initializer_list

/**
 * ITERATOR TESTS
 */

#define _PANDA_UNORDERED_MAP_TEST_ITERATOR_VALIDATE_MAP(_BEGIN, _END)          \
  {                                                                            \
    auto begin = _BEGIN;                                                       \
    auto end = _END;                                                           \
    std::vector<typename TestFixture::ValueType> vec(begin, end);              \
    ASSERT_EQ(map.size(), vec.size());                                         \
    for (auto &el : vec) {                                                     \
      auto it = map.find(el.first);                                            \
      ASSERT_NE(it, map.end());                                                \
      ASSERT_EQ(el.first, it->first);                                          \
      ASSERT_EQ(el.second, it->second);                                        \
    }                                                                          \
  }                                                                            \
  (void)0

TYPED_TEST(unordered_map_test, iterator) {
  using KeyType = typename TestFixture::KeyType;
  using MappedType = typename TestFixture::MappedType;

  _PANDA_UNORDERED_MAP_TEST_INIT_LIST;
  panda::unordered_map<KeyType, MappedType> map(init_list);
  _PANDA_UNORDERED_MAP_TEST_ITERATOR_VALIDATE_MAP(map.begin(), map.end());
}

TYPED_TEST(unordered_map_test, const_iterator) {
  using KeyType = typename TestFixture::KeyType;
  using MappedType = typename TestFixture::MappedType;

  _PANDA_UNORDERED_MAP_TEST_INIT_LIST;
  const panda::unordered_map<KeyType, MappedType> map(init_list);
  _PANDA_UNORDERED_MAP_TEST_ITERATOR_VALIDATE_MAP(map.begin(), map.end());
}

TYPED_TEST(unordered_map_test, citerator) {
  using KeyType = typename TestFixture::KeyType;
  using MappedType = typename TestFixture::MappedType;

  _PANDA_UNORDERED_MAP_TEST_INIT_LIST;
  panda::unordered_map<KeyType, MappedType> map(init_list);
  _PANDA_UNORDERED_MAP_TEST_ITERATOR_VALIDATE_MAP(map.cbegin(), map.cend());
}

TYPED_TEST(unordered_map_test, reverse_iterator) {
  using KeyType = typename TestFixture::KeyType;
  using MappedType = typename TestFixture::MappedType;

  _PANDA_UNORDERED_MAP_TEST_INIT_LIST;
  panda::unordered_map<KeyType, MappedType> map(init_list);
  _PANDA_UNORDERED_MAP_TEST_ITERATOR_VALIDATE_MAP(map.rbegin(), map.rend());
}

TYPED_TEST(unordered_map_test, reverse_const_iterator) {
  using KeyType = typename TestFixture::KeyType;
  using MappedType = typename TestFixture::MappedType;

  _PANDA_UNORDERED_MAP_TEST_INIT_LIST;
  const panda::unordered_map<KeyType, MappedType> map(init_list);
  _PANDA_UNORDERED_MAP_TEST_ITERATOR_VALIDATE_MAP(map.rbegin(), map.rend());
}

TYPED_TEST(unordered_map_test, reverse_citerator) {
  using KeyType = typename TestFixture::KeyType;
  using MappedType = typename TestFixture::MappedType;

  _PANDA_UNORDERED_MAP_TEST_INIT_LIST;
  panda::unordered_map<KeyType, MappedType> map(init_list);
  _PANDA_UNORDERED_MAP_TEST_ITERATOR_VALIDATE_MAP(map.crbegin(), map.crend());
}

/**
 * INSERTION/EMPLACE TESTS
 */

TYPED_TEST(unordered_map_test, insert_duplicate) {
  using KeyType = typename TestFixture::KeyType;
  using MappedType = typename TestFixture::MappedType;
  _PANDA_UNORDERED_MAP_TEST_INIT_LIST;
  panda::unordered_map<KeyType, MappedType> map(init_list);

  std::map<KeyType,MappedType> ulist(init_list.begin(), init_list.end());
  for(auto &el: ulist) {
    // Change the value of the mapped type
    el.second = value_gen();

    bool test;
    auto it_find = map.find(el.first);
    typename panda::unordered_map<KeyType, MappedType>::iterator it_insert;
    std::tie(it_insert, test) = map.insert(el);
    ASSERT_FALSE(test);
    ASSERT_EQ(it_insert, it_find);
    // Key should be the same
    ASSERT_EQ(it_insert->first, el.first);
    // Mapped should be different
    ASSERT_NE(it_insert->second, el.second);
  }
}

TYPED_TEST(unordered_map_test, insert_unique) {
  using KeyType = typename TestFixture::KeyType;
  using MappedType = typename TestFixture::MappedType;
  _PANDA_UNORDERED_MAP_TEST_INIT_LIST;
  std::set<typename TestFixture::ValueType> ulist(init_list.begin(), init_list.end());
  panda::unordered_map<KeyType, MappedType> map;
  for (auto &el : ulist) {
    bool test;
    auto it_find = map.find(el.first);
    ASSERT_EQ(it_find, map.end());
    typename panda::unordered_map<KeyType, MappedType>::iterator it_insert;
    std::tie(it_insert, test) = map.insert(el);
    ASSERT_TRUE(test);
    ASSERT_NE(it_insert, map.end());
    ASSERT_EQ(it_insert->first, el.first);
    ASSERT_EQ(it_insert->second, el.second);
  }
  ASSERT_EQ(map.size(), ulist.size());
}

// TODO
// insert_or_assign
// emplace pair_args 
// emplace key_mapped_args
// try_emplace key_mapped_args
// erase by iterator
// erase by const_iterator
// erase by const_iterator range
// erase by key

/**
 * QUERY TESTS
 */

// TODO
// find
// find<K>
// at
// operator[]
// contains(Key)
// count(Key)
// size
// empty

/**
 * MISC. TESTS
 */
// load_factor_<>
// max_load_factor
// ...
