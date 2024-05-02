#include <sal.h>
#include <bit>
#include <cassert>

void test_sl_buf_to_native_ushort_LE() {
    SLuchar bufLittleEndian[] = {0x02, 0x01};  // Little endian: 258 (0x102)

    SLushort result = sl_buf_to_native_ushort(bufLittleEndian, 2);
    assert(result == 258);
}

void test_sl_buf_to_native_ushort_BE() {
    SLuchar bufBigEndian[] = {0x01, 0x02};     // Big endian: 258 (0x102)

    SLushort result = sl_buf_to_native_ushort(bufBigEndian, 2);
    assert(result == 258);
}

int main(void) {
    if constexpr (std::endian::native == std::endian::little)
        test_sl_buf_to_native_ushort_LE();
    else
        test_sl_buf_to_native_ushort_BE();

    return 0;
}