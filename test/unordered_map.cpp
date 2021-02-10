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
  using ValueType = std::pair<KeyType,MappedType>;
};
#define TUP(K, V) std::tuple<K, V>
using TestTypes =
    ::testing::Types<TUP(int, int), TUP(char, char), TUP(char, std::string),
                     TUP(std::string, char),
                     TUP(HashableType<std::string>, char),
                     TUP(std::size_t, std::vector<double>)>;
TYPED_TEST_SUITE(unordered_map_test, TestTypes);

#define _PANDA_UNORDERED_MAP_TEST_INIT_LIST                        \
  ::panda::test::generator<KeyType> key_gen;                                   \
  ::panda::test::generator<MappedType> value_gen;                               \
  std::initializer_list<std::pair<const KeyType, MappedType>> init_list = {     \
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

TYPED_TEST(unordered_map_test, iterator) {
  using KeyType = typename TestFixture::KeyType;
  using MappedType = typename TestFixture::MappedType;

  _PANDA_UNORDERED_MAP_TEST_INIT_LIST;
  panda::unordered_map<KeyType, MappedType> map(init_list);

  auto begin = map.begin();
  auto end = map.end();
  std::vector<typename TestFixture::ValueType> vec(begin,end);
  for(auto& el: vec)
  {
    ASSERT_NE(map.find(el.first),map.end());
    ASSERT_EQ(map.size(),vec.size());
  }
}

TYPED_TEST(unordered_map_test, const_iterator) {
  using KeyType = typename TestFixture::KeyType;
  using MappedType = typename TestFixture::MappedType;

  _PANDA_UNORDERED_MAP_TEST_INIT_LIST;
  const panda::unordered_map<KeyType, MappedType> map(init_list);

  auto begin = map.begin();
  auto end = map.end();
  std::vector<typename TestFixture::ValueType> vec(begin,end);
  for(auto& el: vec)
  {
    ASSERT_NE(map.find(el.first),map.end());
    ASSERT_EQ(map.size(),vec.size());
  }
}

TYPED_TEST(unordered_map_test, citerator) {
  using KeyType = typename TestFixture::KeyType;
  using MappedType = typename TestFixture::MappedType;

  _PANDA_UNORDERED_MAP_TEST_INIT_LIST;
  panda::unordered_map<KeyType, MappedType> map(init_list);

  auto begin = map.cbegin();
  auto end = map.cend();
  std::vector<typename TestFixture::ValueType> vec(begin,end);
  for(auto& el: vec)
  {
    ASSERT_NE(map.find(el.first),map.end());
    ASSERT_EQ(map.size(),vec.size());
  }
}

