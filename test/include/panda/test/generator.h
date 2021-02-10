#pragma once

#include <string>
#include <vector>

namespace panda {
namespace test {

template <class T> struct generator {
  T operator()() { return (T)(rand() % 100); }
};

template <> struct generator<char> {
  char operator()() { return (char)(rand() % ('~' - '!') + '!'); }
};

template <> struct generator<std::string> {
  std::string operator()() {
    auto len = (size_t)(rand() % 20) + 2;
    std::string str(len, ' ');
    for (auto& c: str) {
      c = generator<char>()();
    }
    return str;
  }
};

template <class T> struct generator<std::vector<T>> {
  std::vector<T> operator()() {
    auto len = (size_t)(rand() % 20) + 1;
    std::vector<T> vec(len);
    for (auto &el : vec) {
      el = generator<T>()();
    }
    return vec;
  }
};
} // namespace test
} // namespace panda
