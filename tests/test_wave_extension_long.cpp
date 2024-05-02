#include <sal.h>
#include <cassert>
#include <cstring>

void test_wave_extension_long() {
    assert(strcmp(".wave", WAVE_EXTENSION_LONG) == 0);
}

int main(void) {
    test_wave_extension_long();
    return 0;
}