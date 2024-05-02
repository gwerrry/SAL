#include <sal.h>
#include <cassert>
#include <cstring>
#include "wave_file_defines.h"

void test_is_wave_file_not_a_file() {
    assert(sl_is_wave_file(WAVE_FILE_TRICKY_FOLDER) == SL_NOT_A_FILE);
}

int main(void) {
    test_is_wave_file_not_a_file();
    return 0;
}