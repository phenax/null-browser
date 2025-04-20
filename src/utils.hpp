#pragma once

#include <memory>

#define DELEGATE(OBJ, METHOD, METHOD_AS)                                                           \
  template <typename... Args> inline decltype(auto) METHOD_AS(Args &&...args) {                    \
    return (OBJ)->METHOD(std::forward<Args>(args)...);                                             \
  }

#define DEFINE_SETTER(METHOD, PROPERTY)                                                            \
  template <typename Arg> inline void METHOD(Arg val) { PROPERTY = val; }

#define DEFINE_GETTER(METHOD, EXPR)                                                                \
  decltype(auto) inline METHOD() const { return EXPR; }

#define DEFER(DEFERRED_BLOCK) std::shared_ptr<void> __deferred(nullptr, [](...) DEFERRED_BLOCK);
