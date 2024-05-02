#include <sal.h>
#include <cassert>
#include <cstring>
#include "wave_file_defines.h"

void test_is_wave_file_short() {
    assert(sl_is_wave_file(WAVE_FILE_EMPTY_FILE) == SL_SUCCESS);
}

int main(void) {
    test_is_wave_file_short();
    return 0;
}