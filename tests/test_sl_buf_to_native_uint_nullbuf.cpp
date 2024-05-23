#include <sal.h>
#include <cassert>

void test_sl_buf_to_native_uint_nullbuf() {
  SLuchar* buf = NULL;

  SLushort result = sl_buf_to_native_uint(buf, 4);
  assert(result == 0);
}

int main(void) {
  test_sl_buf_to_native_uint_nullbuf();
  return 0;
}
