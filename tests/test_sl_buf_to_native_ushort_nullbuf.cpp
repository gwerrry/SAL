#include <sal.h>
#include <cassert>

void test_sl_buf_to_native_ushort_nullbuf() {
  SLuchar* buf = NULL;

  SLushort result = sl_buf_to_native_ushort(buf, 2);
  assert(result == 0);
}

int main(void) {
  test_sl_buf_to_native_ushort_nullbuf();
  return 0;
}