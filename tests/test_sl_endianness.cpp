#include <sal.h>
#include <bit>
#include <cassert>

void test_sl_get_native_endianness() {
  // Determine the expected endianness
  SL_ENDIANNESS expected;
  if constexpr(std::endian::native == std::endian::little)
    expected = SL_LITTLE_ENDIAN;
  else
    expected = SL_BIG_ENDIAN;

  SL_ENDIANNESS actual = sl_get_native_endianness();

  assert(actual == expected);
}

int main(void) {
  test_sl_get_native_endianness();
  return 0;
}
