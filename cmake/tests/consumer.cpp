#include <mathicgb.h>
#include <mathic.h>
#include <memtailor.h>

#if defined(EXPECT_DEBUG) != defined(MATHICGB_DEBUG)
#error Debug ABI must agree with the installed library
#endif
static_assert(__cplusplus >= 201703L, "C++17 must be inherited");
#if defined(EXPECT_NO_TBB) != defined(MATHICGB_NO_TBB)
#error TBB setting must agree with the installed library
#endif

int main() {
  libmathicIsPresent();
  memt::Arena arena;
  void* p = arena.alloc(32);
  arena.freeTop(p);
  mgb::GroebnerConfiguration config(101, 2, 1);
  return config.modulus() == 101 && config.varCount() == 2 ? 0 : 1;
}
