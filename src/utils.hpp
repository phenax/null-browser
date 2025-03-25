#pragma once

#define DELEGATE(OBJ, METHOD, METHOD_AS)                                       \
  template <typename... Args> decltype(auto) METHOD_AS(Args &&...args) {       \
    return OBJ->METHOD(std::forward<Args>(args)...);                           \
  }

#define DEFINE_SETTER(METHOD, PROPERTY)                                        \
  template <typename Arg> void METHOD(Arg val) { PROPERTY = val; }

#define DEFINE_GETTER(METHOD, EXPR)                                            \
  decltype(auto) METHOD() { return EXPR; }

#define DEFINE_FETCHER(TYPE, NAME)                                             \
  void set_##NAME##_fetcher(const std::function<TYPE> &fetcher) {              \
    fetch_##NAME = fetcher;                                                    \
  }                                                                            \
  std::function<TYPE> fetch_##NAME;
