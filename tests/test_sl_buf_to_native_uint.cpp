#include <sal.h>
#include <bit>
#include <cassert>

void test_sl_buf_to_native_uint_LE() {
  SLuchar bufLittleEndian[] = {0x04, 0x03, 0x02, 0x01};  // Little endian: 16909060 (0x1020304)

  SLuint result = sl_buf_to_native_uint(bufLittleEndian, 4);
  assert(result == 16909060);
}

void test_sl_buf_to_native_uint_BE() {
  SLuchar bufBigEndian[] = {0x01, 0x02, 0x03, 0x04};  // Big endian: 16909060 (0x1020304)

  SLuint result = sl_buf_to_native_uint(bufBigEndian, 4);
  assert(result == 16909060);
}

int main(void) {
  if constexpr(std::endian::native == std::endian::little)
    test_sl_buf_to_native_uint_LE();
  else
    test_sl_buf_to_native_uint_BE();

  return 0;
}
