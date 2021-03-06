#include <functional>
#include <type_traits>

#include <iostream>
#include <vector>
#include <cassert>

#define HAS_MEMBER_DEF(name)                                                   \
  namespace detail {                                                           \
  template <class T, class F = void> class HasMember_##name {                  \
    template <class _Ux>                                                       \
    static std::true_type                                                      \
    Check(int,                                                                 \
          typename std::enable_if<std::is_void<F>::value,                      \
                                  decltype(&_Ux::name) *>::type = nullptr);    \
    template <class _Ux>                                                       \
    static std::true_type                                                      \
    Check(int, decltype(std::mem_fn<F>(&_Ux::name)) * = nullptr);              \
    template <class _Ux>                                                       \
    static std::true_type Check(int, typename _Ux::name * = nullptr);          \
    template <class _Ux> static std::false_type Check(...);                    \
                                                                               \
  public:                                                                      \
    static constexpr bool value = decltype(Check<T>(0))::value;                \
  };                                                                           \
  }
#define HAS_MEMBER(type, name, ...)                                            \
  detail::HasMember_##name<type, ##__VA_ARGS__>::value

#define HAS_DEF(name)                                                          \
  namespace detail {                                                           \
  template <class F = void> class Has_##name {                                 \
    template <class _Ux>                                                       \
    static std::true_type                                                      \
    Check(int, typename std::enable_if<std::is_void<F>::value,                 \
                                       decltype(&name) *>::type = nullptr);    \
    template <class _Ux>                                                       \
    static std::true_type                                                      \
    Check(int, decltype(std::bind<void, typename std::decay<F>::type>(         \
                   &name)) * = nullptr);                                       \
    template <class _Ux> static std::false_type Check(...);                    \
                                                                               \
  public:                                                                      \
    static constexpr bool value = decltype(Check<void>(0))::value;             \
  };                                                                           \
  }
#define HAS(name, ...) detail::Has_##name<##__VA_ARGS__>::value

HAS_MEMBER_DEF(push_back);
HAS_MEMBER_DEF(size);
HAS_MEMBER_DEF(value_type);
HAS_MEMBER_DEF(first);
HAS_MEMBER_DEF(x);

#if defined(_MSC_VER) && (_MSC_VER >= 1900)
void Print() {}
void Print(int) {}
void X() {}
int aaa;

HAS_DEF(Print)
HAS_DEF(Print1)
HAS_DEF(X)
HAS_DEF(aaa)
HAS_DEF(bbb)
#endif

template <bool C, class Ft, class Ff, class... Args>
typename std::enable_if<C, size_t>::type Exec(Ft &&ft, Ff &&ff, Args &&... args) {
  return std::forward<Ft>(ft)(std::forward<Args>(args)...);
}
template <bool C, class Ft, class Ff, class... Args>
typename std::enable_if<!C, size_t>::type Exec(Ft &&ft, Ff &&ff, Args &&... args) {
  return std::forward<Ff>(ff)(std::forward<Args>(args)...);
}
template <class T> auto Size(const T &obj) {
  return Exec<HAS_MEMBER(T, size)>(
    [](const auto &obj) { return obj.size(); },
    [](const auto &obj) { return -1; }, obj);
}

int main() {
  static_assert(HAS_MEMBER(std::vector<int>, push_back) == false, "????????????");
  static_assert(HAS_MEMBER(std::vector<int>, push_back, void(const int &)), "??????????????????????????????");
  static_assert(HAS_MEMBER(std::vector<int>, size), "???????????????");
  static_assert(HAS_MEMBER(std::vector<int>, size, size_t()) == false, "????????????????????????????????????");
  static_assert(HAS_MEMBER(std::vector<int>, size, size_t() const), "????????????????????????????????????");
  static_assert(HAS_MEMBER(std::vector<int>, value_type), "????????????");

  using IntIntPairType = std::pair<int, int>; // ??????????????????,?????????
  static_assert(HAS_MEMBER(IntIntPairType, first), "????????????");
  static_assert(HAS_MEMBER(IntIntPairType, x) == false, "?????????");

  std::vector<int> a;
  IntIntPairType b;

  assert(Size(a) == 0);
  assert(Size(b) == -1);

#if defined(_MSC_VER) && (_MSC_VER >= 1900)
  static_assert(HAS(Print) == false, "????????????");
  static_assert(HAS(Print, void(int)), "??????????????????????????????");
  static_assert(HAS(X), "???????????????");
  static_assert(HAS(X, void(int)) == false, "????????????????????????????????????");
  static_assert(HAS(X, void()), "????????????????????????????????????");
  static_assert(HAS(aaa), "??????");
  static_assert(HAS(bbb) == false, "?????????");
#endif
}