#include <sal.h>
#include <cassert>

void test_sl_buf_to_native_ushort_shortlen() {
    SLuchar buf[] = {0x01};

    SLushort result = sl_buf_to_native_ushort(buf, 1);
    assert(result == 0);
}

int main(void) {
    test_sl_buf_to_native_ushort_shortlen();
    return 0;
}