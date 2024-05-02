#include <sal.h>
#include <cassert>
#include <cstring>
#include "wave_file_defines.h"

void test_is_wave_file_short2() {
    assert(sl_is_wave_file(WAVE_FILE_EMPTY_FILE4) == SL_SUCCESS);
}

int main(void) {
    test_is_wave_file_short2();
    return 0;
}