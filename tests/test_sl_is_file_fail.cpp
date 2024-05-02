#include <sal.h>
#include <cassert>
#include "wave_file_defines.h"

void test_sl_is_file_fail() {
    SL_RETURN_CODE ret = sl_is_file(WAVE_FILE_TRICKY_FOLDER);
    assert(ret == SL_NOT_A_FILE);
}

int main(void) {
    test_sl_is_file_fail();
    return 0;
}