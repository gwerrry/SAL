#include <sal.h>
#include <cassert>

void test_sl_buf_to_native_uint_shortlen() {
  SLuchar buf[] = {0x01};

  SLuint result = sl_buf_to_native_uint(buf, 1);
  assert(result == 0);
}

int main(void) {
  test_sl_buf_to_native_uint_shortlen();
  return 0;
}
