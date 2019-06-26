#pragma once
#include "reflection.h"

#include <cstdint>
#include <string>

namespace Orz {

struct Hoo {
  int a;
  int b;
  int *c;
};

namespace Test {
struct Meta(Range(10, 10), EditorVisible) Human {
  Human();
  Human(int);
  MetaExclude() Human(const Human &);
  ~Human();
  Human &operator=(const Human &);
  int age;
  MetaExclude() uint32_t age2;
  int32_t age3;
  std::string &alive;
  const volatile uint8_t height;
  MetaExclude() const char *name;
  static Hoo ho;
  const volatile Hoo *ho_ref;
  const Hoo *const c_ho_ref;
  static int haa;
  void foo();
  Meta(Range(1, &oorb)) int ooro;
  void foo1(int);
  int oorb;
  static void bar();
  MetaInject()
private:
  const volatile Hoo *cv_ho_ref;
  std::string desc;
};
} // namespace Test
} // namespace Orz
