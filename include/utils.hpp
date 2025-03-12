
#define DELEGATE(OBJ, METHOD, METHOD_AS)                                       \
  template <typename... Args> decltype(auto) METHOD_AS(Args &&...args) {       \
    return OBJ->METHOD(std::forward<Args>(args)...);                           \
  }
