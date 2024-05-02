#include <sal.h>
#include <cassert>
#include "wave_file_defines.h"

void test_sl_is_file_success() {
    SL_RETURN_CODE ret = sl_is_file(WAVE_FILE_EMPTY_FILE);
    assert(ret == SL_SUCCESS);
}

int main(void) {
    test_sl_is_file_success();
    return 0;
}