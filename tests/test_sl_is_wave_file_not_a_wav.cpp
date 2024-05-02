#include <sal.h>
#include <cassert>
#include <cstring>
#include "wave_file_defines.h"

void test_is_wave_file_not_a_wav() {
    assert(sl_is_wave_file(WAVE_FILE_EMPTY_TXT) == SL_NOT_A_WAVE_FILE);
}

int main(void) {
    test_is_wave_file_not_a_wav();
    return 0;
}