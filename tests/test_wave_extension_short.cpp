#include <sal.h>
#include <cassert>
#include <cstring>

void test_wave_extension_short() {
    assert(strcmp(".wav", WAVE_EXTENSION_SHORT) == 0);
}

int main(void) {
    test_wave_extension_short();
    return 0;
}